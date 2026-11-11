# AUTOSAR OS 模块详解（Classic Platform）

> 适用环境：Vector MICROSAR + DaVinci Configurator + NXP S32K144
> 对应 AUTOSAR Classic Platform 4.2，模块名 **Os**
> 核心定位：**静态配置、零动态内存、确定性的实时调度器**——和 FreeRTOS 的本质区别

---

## 0. 一句话直觉：OS 就是"公司排班系统"

| OS 概念 | 类比公司 | 核心作用 |
|---|---|---|
| OS 调度核心 | HR | 决定谁先干活、干多久、谁让谁 |
| Task | 打工人 | 有岗位、有优先级、能不能被打断 |
| ISR 中断 | 突发紧急事 | 领导喊"立刻停手处理" |
| Resource 资源 | 共享打印机 | 一次只能一个人用，抢就锁 |
| Alarm 闹钟 | 晨会闹钟 | 到点必须开，到点必须散 |
| Schedule Table 调度表 | 整套会议时刻表 | 一次排好 N 个闹钟，精确同步 |
| Counter 时钟 | 钟摆 | 闹钟的时间基准（**软件 tick，不是硬件定时器**） |
| Event 事件 | "老板来了"的信号灯 | 等灯亮的人立刻去开门——**不是函数，是 bit 标志** |
| Message 消息 | 发邮件 | Task 间带数据通信 |
| Hook 钩子 | 电梯预留接线端子 | OS 关键时刻预留的插入点 |

---

## 1. AUTOSAR OS vs FreeRTOS 本质区别

| 维度 | AUTOSAR OS | FreeRTOS / RT-Thread |
|---|---|---|
| **配置方式** | **静态**——所有 Task/ISR/Alarm 编译时一次性配好 | 动态——运行时可以 `xTaskCreate` 随时创建/销毁 |
| **内存管理** | **零动态分配**——所有栈/队列空间编译时静态分配 | 支持动态分配（`pvPortMalloc`） |
| **确定性** | **100% 可预测**——下一个 1ms 周期一定会触发 | 有概率不确定——动态创建可能失败、内存碎片可能挂 |
| **调度策略** | 优先级调度 + 抢占/协作（**硬编码，配好不能改**） | 同 AUTOSAR，但运行时可改优先级 |
| **优先级反转** | **强制解决**——Resource 的优先级天花板协议硬编码 | 可选（需程序员手动用互斥锁） |
| **用途** | 汽车电子安全相关（ISO 26262）——**必须确定性** | 通用嵌入式——灵活但不保证确定性 |

**一句话**：AUTOSAR OS 是**汽车安全的操作系统**，FreeRTOS 是**通用嵌入式的操作系统**。你不能让一个"下一秒必须触发的 Task"因为内存碎片没跑上——这就是 AUTOSAR OS 存在的理由。

---

## 2. 核心组件详解

### 2.1 Task——打工人

AUTOSAR OS 的 Task **只有两种**，这是规范硬规定：

| 类型 | 类比 | 核心能力 | 约束 |
|---|---|---|---|
| **Basic Task** | 只能干活/停工 | 三种状态：RUNNING / READY / SUSPENDED | **不能挂起等待事件**——要么跑要么停 |
| **Extended Task** | 会等消息的 | 五种状态：RUNNING / READY / **WAITING** / SUSPENDED / READY | **可以 `WaitEvent` 主动挂起** |

#### "扩展"到底扩展了什么（必须记）

**Extended Task 比 Basic Task 多三个核心能力**——这就是"扩展"的含义，也是周期任务、共享数据保护的核心载体：

| 能力 | Basic Task | Extended Task | 关联知识点 |
|---|---|---|---|
| **阻塞等待**（`WaitEvent`） | ❌ 不能，必须跑完就退 | ✅ 能，卡住等事件，被唤醒后继续跑 | Alarm 触发 Event 实现周期调度 |
| **使用事件（Event）** | ❌ 不能等/置事件 | ✅ 能等（`WaitEvent`）、能清（`ClearEvent`） | Event 只能配给 Extended Task |
| **使用资源（Resource）** | ❌ 不能用 `GetResource/ReleaseResource` | ✅ 能，配合优先级天花板保护共享数据 | 触发"Resource 锁调度"机制（见 2.3） |

#### 调度策略（OsTaskSchedule / OsTaskPrio）

| 策略 | 含义 | 推荐场景 |
|---|---|---|
| **Preemptive（抢占）** | 高优先级 Task 随时打断低优先级 | 实时性要求高的 Task（看门狗、通信） |
| **Cooperative（协作）** | 只有当前 Task `TerminateTask` / `Schedule` 后高优先级才接手 | 不推荐，容易堵 CPU |

> **默认建议**：大部分 Task 用 Preemptive，配合 Resource 解决优先级反转。

#### Task 优先级

- **数字越大优先级越高**（和 FreeRTOS 相反！）
- 配置时必须给每个 Task **唯一优先级**（规范要求，不允许同优先级同级调度）
- 优先级**硬编码**——运行时不能改

#### Task 状态转移（以 Extended Task 为例）

```
                    SetEvent / Alarm 到期
                        ↓
SUSPENDED ──────────────────────────────── READY
   ↑                                              ↓
   │ TerminateTask                   调度器选上
   │                                              ↓
   │ WAITING ←──── WaitEvent ─────── RUNNING
   │           (Extended 独有)
   └──────────────────────────────────────────┘
```

**Basic Task** 没有 WAITING 状态——从 SUSPENDED 进 READY → RUNNING → TerminateTask 回 SUSPENDED，三条路径。

#### Basic vs Extended 完整代码对比

**Basic Task（一次性，靠 Alarm 重复激活）：**
```c
/* 每次 Alarm 激活进来，跑一次就退 */
TASK(Task_BasicLed)
{
    Dio_FlipPin(PORT_DIO_CHANNEL_LED);
    TerminateTask(); /* 必须退，下一次靠 Alarm 再 ActivateTask 进来 */
}
```
Alarm 配置：Action = `ActivateTask(Task_BasicLed)`，Offset = 100 ticks。

**Extended Task（循环跑，靠 Alarm 置事件唤醒）：**
```c
/* 只激活一次，后续靠 Alarm 置事件反复唤醒 */
TASK(Task_ExtendedLed)
{
    while(1) {
        WaitEvent(Event_100ms);        /* 卡在这里等，Alarm 到点置事件后自动唤醒 */
        ClearEvent(Event_100ms);

        /* 能用 Resource 保护共享数据（Basic 做不到） */
        GetResource(Res_LedData);
        g_LedState = !g_LedState;
        ReleaseResource(Res_LedData);

        Dio_WritePin(PORT_DIO_CHANNEL_LED, g_LedState);
    }
    /* 不能 return 结束——必须显式 TerminateTask */
    TerminateTask();
}
```
Alarm 配置：Action = `SetEvent(Task_ExtendedLed, Event_100ms)`，Offset = 100 ticks。

#### Extended Task 的限制

1. **不能直接 return 结束**：必须用 `TerminateTask()`，否则 OS 检测到任务栈未清理触发 `ErrorHook`（`E_OS_MISSINGEND`）
2. **WaitEvent 只能等已配置的事件掩码**：必须在 DaVinci 里给扩展 Task 配好 EventMask
3. **WaitEvent 会永久阻塞**：如果没有对应的 Event 被置（Alarm 没启动、Event 配错掩码），Task 永远 WAITING
4. **Resource 不能嵌套**：同一个 Extended Task 不能重复 `GetResource` 同一个 Resource

---

### 2.2 ISR 中断——突发紧急事

AUTOSAR OS 把中断**硬分成两类**，这是规范。核心区分标准：**OS 能不能感知它的存在**。

#### Cat1 ISR（一类中断）vs Cat2 ISR（二类中断）

| 维度 | Cat1 ISR | Cat2 ISR |
|---|---|---|
| **OS 知不知道?** | **完全不知道**——中断来了直接跑，OS 连"发生了中断"都不知道 | **知道**——中断来了先过 OS，OS 再跳你的服务函数 |
| **能调 OS 服务吗?** | **一个都不能**——连 `SetEvent` / `ActivateTask` 都不行 | **能**——大部分 `OsXXX` 服务都能调（SetEvent、IncrementCounter、ActivateTask 等） |
| **能被 OS 屏蔽吗?** | **只被 `DisableAllInterrupts` 挡住**；`SuspendOSInterrupts`（只屏蔽 Cat2）和 `SuspendAllInterrupts`（OS 级锁）**都挡不住 Cat1** | **被 `SuspendOSInterrupts` 和 `SuspendAllInterrupts` 挡住** |
| **优先级谁管?** | 纯 **NVIC 硬件**——OS 完全管不着 | 硬件优先级仍由 NVIC 管，但 OS **会把它纳入可调度性分析**（判断"这个 ISR 能抢占哪个 Task"） |
| **响应延迟** | **最低**——绕过 OS，直接硬件派发 | 略高——要先过 OS 派发层 |
| **典型用途** | SysTick 心跳、PWM 周期触发、编码器采样——只做硬件寄存器操作、不需要 OS | CAN 接收（要调 `CanIf_RxIndication` → 激活 Task）、SPI 传输完成（要唤醒 Task） |

#### 代码示例对比

```c
/* Cat1 ISR：只能做纯硬件操作 */
void IsrGptCat1(void)
{
    PIT_TFLG0 |= PIT_TFLG0_TIF_MASK;   /* 清中断标志 */
    /* ❌ SetEvent(Task_Example, Event_10ms);  编译报错或行为未定义 */
    /* ❌ Os_IncrementCounter(...);             编译报错 */
}

/* Cat2 ISR：能调 OS 服务 */
void IsrGptCat2(void)
{
    PIT_TFLG0 |= PIT_TFLG0_TIF_MASK;   /* 清中断标志 */
    Os_IncrementCounter(Counter_Main); /* ✅ 合法——OS 驱动 Counter */
    SetEvent(Task_Example, Event_10ms); /* ✅ 合法——OS 能看见 */
}
```

#### 三种中断屏蔽锁（高频考点）

OS 提供三种屏蔽 API，**屏蔽范围逐层递进**：

| API | 屏蔽范围 | Cat1 ISR | Cat2 ISR | Task | 可嵌套 | 典型场景 |
|---|---|---|---|---|---|---|
| `DisableAllInterrupts` | **所有中断** | ❌ 进不来 | ❌ 进不来 | 不受影响 | ❌ 不可嵌套 | 极短临界区（几个寄存器操作），全系统级屏蔽 |
| `SuspendAllInterrupts` | **所有中断** | ❌ 进不来 | ❌ 进不来 | 不受影响 | ✅ 可嵌套 | 一般临界区——保存旧屏蔽状态，嵌套层数=屏蔽层数 |
| `SuspendOSInterrupts` | **只屏蔽 Cat2 ISR** | ✅ **照样能进** | ❌ 进不来 | 不受影响 | ✅ 可嵌套 | Resource 锁调度（见 2.3）——OS 自己内部常用 |

**三层锁的层次关系**：
```
┌─ Cat1 ISR 层（硬件层，永远在最顶） ─┐
│  不被 SuspendOSInterrupts 挡住       │
├─ SuspendOSInterrupts 屏蔽区 ─────┤
│  Cat2 ISR 被挡                      │
├─ SuspendAllInterrupts / DisableAllInterrupts 屏蔽区 ─┤
│  Cat1 和 Cat2 都被挡                 │
├─ 调度器裁决区 ─────────────────────┤
│  Task 被 SuspendAllInterrupts 内部的锁挡住 │
└───────────────────────────────────────┘
```

#### 硬件优先级 vs 软件优先级：为什么"不在一个层次"

你之前问过这个问题，这里系统化：

> **硬件优先级（中断）由 NVIC 硬件裁决，永远压过一切；软件优先级（Task）由 OS 调度器裁决，只在任务内部排队。两者跨域不可比。**

```
        ┌──────────── 硬件层（永远在上） ────────────┐
        │   Cat1 / Cat2 ISR   ← NVIC 硬件直接裁决      │
        ├──────────── 软件层（永远在下） ────────────┤
        │   Task（高优先级）  ← OS 调度器裁决          │
        │   Task（低优先级）                           │
        └───────────────────────────────────────────┘
```

- 中断**永远压过 Task**——不管 Task 优先级多高，只要中断没被屏蔽，CPU 立刻跳去跑 ISR
- Task **永远压不过中断**——Task 不可能"抢占"一个正在执行或已挂起的中断
- **真正可比的是同域内部**：Task vs Task 比软件优先级（数字越大越高），ISR vs ISR 比硬件优先级（Cortex-M 上**数字越小越高**）
- **方向相反是高频坑**：AUTOSAR Task 优先级数字越大越高，Cortex-M NVIC 中断数字越小越高——配错直接导致行为反了

#### 工程实践：中断优先级配置约束

| 场景 | Cat1/Cat2 选择 | 硬件优先级建议 |
|---|---|---|
| 看门狗 WDOG | **Cat1**（纯硬件复位） | 最高（0 或 1）——必须能打断一切 |
| CAN 接收 FlexCAN | **Cat2**（要走 CanIf_RxIndication → 激发任务） | 高于通信相关 Task，低于 WDOG |
| ADC Continuous 采样 | **Cat1**（只清标志、回调不干活） | 低于 CAN——不能让 ADC 占死 CPU |
| SPI 传输完成 | 看场景：纯硬件完成=Cat1；要激活任务=Cat2 | 低于 CAN |
| SysTick / GPT 心跳 | **Cat1**（纯硬件计数器操作） | 低于 WDOG，高于 ADC |

> **项目教训**：ADC Continuous 中断回调里不能调 Rte_Write / Com_SendSignal——占死 CPU 导致其他周期任务无法调度。且 ADC 中断硬件优先级必须**低于 CAN**，否则会阻塞 CAN 通信。

---

### 2.3 Resource 资源——共享打印机

#### 解决什么：优先级反转

场景：
```
低优先级 Task B 拿着 Resource（比如共享变量）
  ↑
高优先级 Task A 要抢（被 B 挡）
  ↑
中优先级 Task C 插队（打断 B）
→ C 跑 → B 不能释放 → A 饿死！
```

#### 优先级天花板协议（Priority Ceiling Protocol）

针对 **Task vs Task** 共享 Resource：

1. 每个 Resource 配一个**天花板优先级**（取所有要访问它的 Task 里最高的）
2. 任何 Task 拿到 Resource 后，**它的软件优先级立刻升到天花板**（挡住 C 插队）
3. 释放 Resource 后，优先级恢复

#### "任务 vs 中断"跨层共享 Resource：锁调度机制（你之前问的核心）

**Task vs Cat2 ISR 共享 Resource 时，优先级天花板协议没用**——为什么？

因为：
- Task 优先级是**软件优先级**，Cat2 ISR 优先级是**硬件优先级**
- **两者不在一个层次，数字不可比**——没法说"把 Task 优先级抬到比 ISR 高"
- 如果放任 Cat2 ISR 在 Task 持 Resource 时进来，ISR 也会调 `GetResource`，但 Resource 被 Task 占着，而 **ISR 不能阻塞/等待** → 冲突无解

**OS 的解决方案：锁调度（比优先级天花板更"霸道"）**

> 一旦某个 Task 持有的 Resource 还被 Cat2 ISR 使用过，OS 会：
> 1. **锁住 Task 调度**——所有 Task（包括跟这个 Resource 无关的）都不能抢占当前 Task
> 2. **屏蔽所有 Cat2 ISR**——用 `SuspendOSInterrupts` 把 Cat2 全挡住
> 3. **Cat1 ISR 不受影响**——仍然能进来（硬件层最高层）

**关键**：锁调度是**全局性的**——连跟这个 Resource 无关的 Task 和 Cat2 ISR 也一起被挡。这正是你引文里"包括与当前 Resource 无关的任务和二类中断"的含义。

**为什么要锁这么大？** 两个字：**怕拖长**。

逻辑链：
```
锁住调度 → 当前 Task 不受任何打扰
        → 以最快速度跑完临界区
        → 尽快 ReleaseResource
        → 等资源的 Cat2 ISR 尽快被放行，不用等太久
```

如果**不**锁调度，当前 Task 会被别的 Task/ISR 打断、被拖慢，临界区**拖长**，Resource 迟迟不释放，那个等资源的 Cat2 ISR 就被"锁"着饿得更久。

#### 锁调度的代价（必须知道）

- **高优先级但无关的 Task 也被挡住**——实时性下降
- **Cat2 ISR 全被挡住**——系统响应性下降
- **所以这类临界区必须极短**——写个共享变量值就走，别做耗时操作

#### Resource 使用规则速查

| 共享场景 | 机制 | 轻/重 |
|---|---|---|
| Task vs Task | 优先级天花板（只抬优先级） | ✅ 轻——只挡低优先级 Task |
| **Task vs Cat2 ISR** | **锁调度**（挡所有 Task + 所有 Cat2） | ❌ 重——必须极短 |
| Task vs Cat1 ISR | **不能用 Resource**——Cat1 不能调 OS API，只能用 MCAL 层临界区（`SuspendAllInterrupts`） | 最重 |

#### 什么时候用 Resource

- ✅ 多个 Task 共享同一块全局数据
- ✅ 多个 Task 共享同一个 MCAL 接口（多个 Task 都要 `Can_Send`）
- ✅ **Cat2 ISR 和 Task 共享数据**（触发锁调度机制）
- ❌ Task 私有数据（只有自己访问）
- ❌ Cat1 ISR 访问的数据（Cat1 不能调 OS API，用 `SuspendAllInterrupts`）

---

### 2.4 Counter / Alarm / Schedule Table——时钟、闹钟、整套时刻表

#### Counter：软件时间抽象（不是硬件定时器）

**本质先焊死**：
> ❌ Counter 不是硬件定时器
> ✅ Counter 是 **OS 维护的「软件计数器（Ticks）」**，硬件定时器（PIT/STM）只是它的「心跳源」

类比：Counter 是你手机里的"时钟应用（软件）"靠"晶振（硬件）"走秒，Counter 是那个软件计数器，PIT/STM 是晶振。

**为什么要做这个抽象？**
- 省硬件定时器：一个 Counter 可以挂 **N 个 Alarm 和 Schedule Table**
- 可移植：换 MCU 时只要换 Counter 驱动源，上层 Alarm 配置不用改

##### Counter 的核心属性（3 个必记）

| 属性 | 含义 | 类比 | 高频坑 |
|---|---|---|---|
| **Tick** | 抽象计数单位，Counter 每次 +1 的"步长" | 秒表的"滴答" | ❌ **把 tick 当毫秒**！tick 是相对单位，**物理时间 = tick 数 × 每个 tick 的物理时长** |
| **MaxAllowedValue** | 最大计数值，到了就重置为 0 | 秒表的上限（比如9999） | ❌ 配太小导致 Alarm Offset 绕回：Max=10, Offset=100 → 实际是 0 tick 立刻触发（100 mod 10=0） |
| **MinCycle** | 最小定时周期，Alarm/Schedule Table 的 Offset 不能比这个小 | 秒表的"最小可设时间" | 配太大浪费精度，配太小超过 OS 可调度性 |

##### Counter 的两种驱动方式

驱动方式决定了 tick 怎么来。

**① 硬件驱动（99% 场景用）**

核心：硬件定时器周期性中断，中断里调 `IncrementCounter` 让 tick +1。

| 驱动源 | 硬件 | 特点 | 精度 | 适用场景 |
|---|---|---|---|---|
| **PIT 驱动（普通）** | S32K144 PIT（属于 MCAL Gpt 模块） | 固定周期中断（比如配 1ms），每次中断只做一件事：`IncrementCounter` | = PIT 中断周期（比如 1ms） | 大多数周期任务（10ms/100ms 均匀定时） |
| **STM 驱动（高精度）** | S32K144 STM（System Timer Module） | OS **直接读 STM 硬件寄存器的计数值作为 Counter tick**——甚至可以不触发中断 | = STM 精度（ns 级） | 高精度延时、FlexRay 时间触发总线、全局时间同步 |

**`OsDriverHighResolution` 开关**：
| 开关状态 | Counter 类型 | 驱动源 | tick 来源 |
|---|---|---|---|
| **不勾选（默认）** | 普通 Counter | PIT（Gpt 模块） | PIT 中断里手动 `IncrementCounter` |
| **勾选** | 高精度 Counter | STM（或 MCU 特定 HR Timer） | **OS 直接读硬件寄存器**（不用中断） |

**② 软件驱动（极少见）**

- **机制**：完全没有硬件定时器，靠代码在主循环 / 另一个 Counter 的 Alarm 回调 / Cat2 ISR 里**手动调 `IncrementCounter`**
- **优势**：灵活（可模拟时间加速/减速）、不需要额外硬件
- **适用**：无硬件定时器的 MCU、测试模拟场景
- **坑**：tick 物理时间完全取决于调用频率，容易因代码阻塞不准

##### tick 物理时间换算（永恒公式）

```
物理时间 = tick 数 × 每个 tick 的物理时长
```
- 硬件驱动（PIT）：每个 tick 的物理时长 = **PIT 的中断周期**（比如 PIT 配 2ms 中断 → 每个 tick = 2ms；Alarm Offset=10 → 实际 20ms）
- 硬件驱动（STM）：每个 tick 的物理时长 = **STM 的计数精度**（比如 1us/tick）
- 软件驱动：每个 tick 的物理时长 = **你调 IncrementCounter 的周期**

##### 完整定时链条

```
┌──────────── 硬件心跳源 ────────────┐
│  S32K144 PIT/STM 硬件定时器        │
│  （比如 PIT 配 1ms 周期中断）        │
└───────────────────────────────────┘
                ↓ 硬件中断（或直接读硬件寄存器）
┌──────────── OS Counter ────────────┐
│  软件维护的 tick 计数器            │
│  每次心跳 → tick +1                │
│  到 MaxAllowedValue → 重置为 0     │
└───────────────────────────────────┘
                ↓ tick 持续递增
┌──────────── 定时组件 ────────────┐
│  Alarm：tick == Offset → 触发 Action │
│  Schedule Table：tick == ExpiryPointOffset → 触发 N 个 Action │
└───────────────────────────────────┘
                ↓ Action 执行
┌──────────── 任务/事件 ────────────┐
│  SetEvent(Task_Ext, Event_10ms)    │
│  ActivateTask(Task_Basic)          │
└───────────────────────────────────┘
```

##### Counter 溢出

Counter 是有限位宽（通常 16 位），到 `MaxAllowedValue` 溢出回 0。OS 内部用**差值**计算下一次到期，溢出不会丢事件。但如果 Alarm 周期超过 Counter 一个完整周期，要小心 Offset 绕回。

---

#### Alarm：单个定时闹钟

Alarm 是绑在 Counter 上的**单点定时触发**——Counter tick 到 Offset 时触发一个 Action。

##### Alarm 的四种 Action

| Action | 作用 | 本质 | 典型用途 |
|---|---|---|---|
| `ActivateTask` | 激活一个 Task | 触发信号，让 Task 进 READY | Basic Task 的周期触发 |
| **`SetEvent`** | 置位某 Extended Task 的**事件位** | **状态标志（不是函数！）** | Extended Task 的周期唤醒 |
| `IncrementCounter` | 递增另一个 Counter | 触发信号 | Counter 链式驱动 |
| **`AlarmCallback`** | 调用一个**回调函数** | ✅ 这才是真函数 | 定时执行一段代码（通常只做 SetEvent，别在回调里干重活） |

> **Event vs AlarmCallback 的混淆点**：Alarm 配 Action=SetEvent 时，触发的只是事件位标志——**不是函数调用**；Alarm 配 Action=AlarmCallback 时，才是直接调用你写的函数。

##### Alarm 的局限（为什么需要 Schedule Table）

1. **Alarm 之间相位难保证**——多个 Alarm 各自独立挂同一个 Counter 但各有 offset，要"A 在 T0、B 在 T0+1ms、C 在 T0+2ms"精确间隔很难，相位会漂
2. **一个 Alarm 只能触发一个动作**——`ActivateTask` / `SetEvent` / `Callback` 四选一，想同一时刻触发 3 个 Task 得摆 3 个 Alarm（offset 全一样），丑且难维护

---

#### Schedule Table：整套会议时刻表

Schedule Table 解决 Alarm 的三个痛点——**多动作同时触发、精确相位同步、时间触发系统的核心组件**。

##### 结构

一个 Schedule Table 由以下组成：
- 一个 Counter（且只能一个——OS 规范硬规定）
- 若干 **Expiry Point（终结点/到期点）**，按 offset（Counter tick）排序
- 每个终结点挂 **1~N 个 Action**（这是 Alarm 做不到的！）

```
Counter 时间轴 (单位 = tick)
offset:  0        1        2        3        4        5 = 周期(duration)
         │        │        │        │        │        │
终结点:  ●────────●────────●─────────────────●────────┘
        EP1      EP2      EP3               EP4        ↻ 回到 EP1
         │        │        │                 │
动作:  Act_A    Act_B    SetEvent_C        Act_A
       Set_E_A  Act_C    Act_D             Set_E_A
                         ↑ 一个终结点同时触发 3 个动作！
```

> offset 是**相对调度表起点**的计数器 tick 值，物理时间 = offset × Counter 的 tick 时长（同 Counter 换算公式）。

##### 两种调度方式

| 方式 | 行为 | 典型用途 |
|---|---|---|
| **Single-Shot（单次）** | 启动后**只跑一遍**，到终点（duration 到）就停止；每个终结点只执行一次 | 上电初始化序列、一次性模式切换 |
| **Repeating（循环）** | 启动后**反复跑**，到终点自动回起点循环，每个终结点**周期性**执行 | **周期激活任务（最常用）** |

##### 关键 API

```c
/* 启动调度表 */
StartScheduleTableAbs(TableID, Start);   /* 绝对对齐：起点对齐到计数器值 Start */
StartScheduleTableRel(TableID, Offset);  /* 相对启动：从当前计数器 + Offset 开始 */

StopScheduleTable(TableID);              /* 停止 */
GetScheduleTableStatus(TableID, &Status);/* 查状态：STOPPED/RUNNING/SUSPENDED */

NextScheduleTable(TableID, NextTableID); /* 链式切换：本表跑完自动切到下一张 */
SyncScheduleTable(TableID, CounterValue);/* 显式同步——把表位置对齐到指定计数器值 */
```

**`NextScheduleTable` 的隐藏威力**：多张表串联，实现"模式 A 跑完自动切模式 B"的时序编排。

##### 同步机制（杀手锏，Alarm 没有）

调度表有两种同步，这是它相对 Alarm 最大的优势：

| 同步 | 含义 | 场景 |
|---|---|---|
| **Implicit（隐式）** | 起点相对计数器对齐一次，之后靠计数器自己走保持精确间隔 | 大多数单 ECU 场景 |
| **Explicit（显式）** | 运行时用 `SyncScheduleTable()` 把表位置**重新对齐到某个绝对计数器值** | **多 ECU 全局时间同步**（FlexRay 时间触发总线，整车所有节点对齐同一个全局时钟） |

##### Schedule Table vs Alarm

| 维度 | Alarm | Schedule Table |
|---|---|---|
| 驱动 Counter | 一个 Counter | **恰好一个** Counter（硬规定） |
| 每次触发动作数 | **1 个**（四选一） | **N 个**（每个终结点可挂多个 Action） |
| 相对相位 | 各 Alarm 独立，难保证 | **同轴天然精确**（共享一条 offset 轴） |
| 同步 | 无 | 隐式 / **显式（全局时间对齐）** |
| 执行模式 | 周期重复 | **单次 / 循环** |
| 典型用途 | 单个周期任务/事件 | **多任务协同、时间触发系统** |

**一句话**：Alarm 是"单个闹钟"，Schedule Table 是"排好时刻表的一套闹钟群"。

##### DaVinci 配置要点 + 常见坑

| 项 | 注意 |
|---|---|
| 选 Counter | 调度表只认一个 Counter，确认它挂的 GPT 周期符合 tick 精度 |
| 终结点 offset | **必须严格递增**，且最后一个 offset < duration（周期），否则终点覆盖/报错 |
| 周期 duration | 循环模式下 duration 就是"跑一圈"的时间 |
| 动作 | 一个终结点可挂多个 Action；按配置顺序执行 |
| 启动 | 上电后在 Task / StartupHook 里 `StartScheduleTableRel/Abs`——**别忘启动！** |

**高频坑**：
1. ❌ **tick 当毫秒**：忘了 Counter 物理周期，offset 算错（和 Alarm 同源）
2. ❌ **最后一个终结点 offset ≥ duration**：跑到周期外，行为异常
3. ❌ **忘了启动**：表配好了但没 `StartScheduleTable`，永不触发
4. ❌ **Repeating 模式当成 Single-Shot**：跑完一圈就停
5. ❌ **多 ECU 没做显式同步**：各表各自起跑，整车节拍漂移

---

### 2.5 Event / Message——Task 间通信

#### Event 的本质（必须彻底搞懂）

你之前问过"Alarm 触发的 Event 是什么，可以理解为一个函数吗"——**答案是否定的**：

> **Event 不是函数，它是一组 bit 标志位（bit mask）。** Alarm 的 `SetEvent` Action 只是把这组标志位置 1，不是调用任何函数。

**机制链条**：
```
GPT 硬件定时中断
        ↓ Counter 计数 +1
Counter 到达 Alarm Offset
        ↓ OS 执行 Alarm Action = SetEvent(Task, EventMask)
        ↓ 只是把目标 Task 的事件标志位置 1（纯赋值操作）
Task 若正卡在 WaitEvent(EventMask) → 变 Ready → 参与调度
        ↓ WaitEvent 返回
Task 函数体里 WaitEvent 之后的代码开始跑   ← 这里才是"函数在执行"
```

所以"事件来了之后跑的东西" = **Task 函数体里 WaitEvent 之后那一段代码**，不是 Event 本身。

**Event 的三个关键属性**：
1. **只有 Extended Task 才有 Event**——Basic Task 不能 WaitEvent/SetEvent
2. **Event 是 bit mask**——一个 Task 可以配多个 Event，`SetEvent` 必须指定是哪一个（或哪几个，按位或）
3. **Event 与设置者解耦**——发 Event 的一方（Alarm/ISR/Task）不需要知道接收方在干嘛，接收方只需 WaitEvent 等着

#### Event vs AlarmCallback 对比

两者都是"被触发的东西"，但定位不同：
- **Event（SetEvent Action）**：只是置位标志，**不直接执行代码**——是 OS 的"通知机制"
- **AlarmCallback**：直接调用一个函数——**是 Alarm 的直接执行动作**

**推荐实践**：AlarmCallback 里**只做 SetEvent**，别在回调里干重活（回调跑在 ISR 上下文，干重活会占死 CPU——和 ADC Continuous 回调的教训同源）。

#### Message（带数据的通信）

| 维度 | Event | Message |
|---|---|---|
| 本质 | 异步、单 bit、可叠加 | 同步队列、带数据、可排队 |
| 能等吗? | `WaitEvent` | `ReceiveMessage` |
| 谁能用? | 只有 Extended Task | 只有 Extended Task |
| 适用 | 通知"干这件事" | 传递数据（CAN 报文内容、传感器数据） |

> **Event 可叠加**：多次 SetEvent 同一个 Event 不会重复计数——只记住"置了没"，不记住"置了几次"。Message 队列会排队，每条都能收到。

---

### 2.6 Hook——电梯预留接线端子

#### 什么是 Hook

Hook（钩子）= **OS 框架预留的"插入点 / 回调点"**。OS 已经把函数原型和调用时机都定好了，函数体默认空；**你不调用它，是 OS 在特定时刻自动来调你填进去的代码**。

类比：Hook 像电梯里的**预留按钮接线端子**——电梯厂家把"消防强制归底""检修锁梯"的接线口留好了，你接上自己的设备就行，不用改电梯主板。

#### AUTOSAR OS 6 个标准 Hook

| Hook | OS 什么时候自动调 | 干什么 | 典型用途 |
|---|---|---|---|
| **ErrorHook** | **任何 OS 服务返回非 E_OK 时** | 集中处理错误 | 错误日志、参数校验、故障计数、死循环调试——**最常用** |
| **StartupHook** | `StartOS()` 里、调度器**启动前** | 多任务开始前的最后初始化 | 启动阶段一次性硬件初始化、配 Schedule Table |
| **ShutdownHook** | `ShutdownOS()` 里、调度器**停止后** | 关机收尾 | 保存数据到 NVM、硬件进安全状态 |
| **PreTaskHook** | **每个 Task 开始执行前** | 任务切换前的动作 | 跟踪/调试：记录"当前跑的是谁"、测任务切换时间 |
| **PostTaskHook** | **每个 Task 结束前** | 任务结束时的动作 | 测量任务执行 WCET、跟踪调度序列 |
| **ProtectionHook** | 发生**保护违例**（栈溢出、时间超限、内存访问违例）时 | 决定怎么处理违规 | 返回 `PROTECTION_IGNORE` / `TERMINATE_TASK` / `SHUTDOWN` |

**每个 Hook 都有两个硬约束**：
1. **被动调用——别主动喊它**：Hook 是 OS 回调你，不是你调 OS；OS 到了那个时刻自动调
2. **运行上下文有限制**：
   - ErrorHook 跑在**调用者（Task 或 ISR）的上下文**里
   - Pre/PostTaskHook **只用于计时/跟踪，不要调用影响调度的服务**（否则扰乱任务时序）

#### Hook 和 AlarmCallback 的区别

两者都是"回调"，但**定位完全不同**：
- **AlarmCallback**：你在配 Alarm 时**主动指定**它到点执行，属于"业务定时动作"，时机由你配的 Alarm Offset 决定
- **Hook**：OS 预定义的**固定时机插入点**，属于"系统级扩展点"，时机由 OS 硬定（比如"出错就调 ErrorHook""每个 Task 开始前调 PreTaskHook"）

#### DaVinci 配置与代码

在 Os → Hooks 下按需勾选启用，启用后 OS 生成对应调用点。**没启用的 Hook OS 根本不调，零开销**；启用了就要实现，否则链接报错（未定义符号）。

```c
/* 典型 ErrorHook 实现——调试救命稻草 */
/* MICROSAR 签名扩展了 Error 码 + 服务名 + 行号 */
void Os_Error(StatusType Error, const char* Func, int Line)
{
    /* 常见错误码对照 Os_Types.h 的 E_OS_xxx：
       E_OS_MISSINGEND       — Task 没 TerminateTask
       E_OS_NO_RES           — Resource 不存在
       E_OS_STATE            — 状态不对（给 SUSPENDED Task SetEvent）
       E_OS_DISALLOWED_CONTEXT — ISR1 里调了 OS API
       E_OS_LIMIT            — 激活队列满了
    */
    LogError(Error, Func, Line);
    while(1) { /* 死循环让调试器断住 */ }
}

/* StartupHook 里启动 Schedule Table */
void Os_StartupHook(void)
{
    StartScheduleTableRel(Table_BCM, 10);   /* 启动调度表，延时 10 tick */
}
```

---

## 3. DaVinci 配置模型速查

| 配置项 | 核心参数 | 作用 | 归属章节 |
|---|---|---|---|
| **OsTask** | Name / Type (Basic/Extended) / Priority / Activation (AutoStart/None) / Scheduling / StackSize / EventMask | 定义每个 Task | 2.1 |
| **OsIsr** | Name / Type (Cat1/Cat2) / InterruptNumber (NVIC 号) | 定义中断 | 2.2 |
| **OsResource** | Name / CeilingPriority / Accessing Tasks/Isrs | 定义共享资源 | 2.3 |
| **OsCounter** | Name / MinCycle / MaxAllowedValue / Reference (PIT/STM) / HighResolution | 定义时钟源 | 2.4 |
| **OsAlarm** | Name / Counter / Action (SetEvent/Callback/ActivateTask) / Offset / Cycle | 定义定时闹钟 | 2.4 |
| **OsScheduleTable** | Name / Counter / Duration / Mode (Repeating/SingleShot) / ExpiryPoints | 定义整套时刻表 | 2.4 |
| **OsEvent** | Name / Mask | 定义事件 | 2.5 |
| **OsMessage** | Name / Length / QueueSize | 定义消息队列 | 2.5 |
| **OsHook** | 勾选 6 个标准 Hook | 启用扩展点 | 2.6 |
| **OsAppMode** | Name / Activated Tasks/Isrs/Alarms/Tables | 应用模式 | — |

生成代码：`Os_PBcfg.c`（配置表）+ `Os_Internal.c`（内部实现）+ `Os_MemMap.h`（内存映射）+ `Os_Hooks.c`（你填的 Hook 实现）。

---

## 4. 代码使用模式

### 4.1 初始化与启动

```c
/* ① EcuM 第一段就调 */
Os_Init(&Os_Config);

/* ② 启动 OS——这之后才有调度，StartOS 正常不返回 */
StartOS(AppMode_Application);
```

### 4.2 Basic Task 入口

```c
TASK(Task_Example)
{
    while(1) {
        /* 周期逻辑 */

        /* Basic Task 必须显式让渡 CPU，否则调度器不知道什么时候切换 */
        TerminateTask();    /* 本次结束，回 SUSPENDED 等下一次触发 */
        // 或者 Schedule(); /* 主动让渡但保持 READY 状态 */
    }
}
```

### 4.3 Extended Task（WaitEvent + Resource 模式，**推荐写法**）

```c
TASK(Task_ExtendedExample)
{
    while(1) {
        WaitEvent(Event_MySignal);      /* 挂起，直到被 SetEvent 唤醒 */
        ClearEvent(Event_MySignal);

        /* 能用 Resource 保护共享数据（Basic Task 做不到） */
        GetResource(Res_SharedData);
        g_SharedVar = g_IncomingData;
        ReleaseResource(Res_SharedData);

        /* 跑业务逻辑 */
        ProcessData(g_SharedVar);
    }

    TerminateTask();    /* 不能直接 return */
}
```

### 4.4 Alarm 回调（注意上下文）

```c
void AlarmCallback_100ms(void)
{
    /* 回调在 ISR 上下文里，不能做耗时操作！ */
    /* 推荐只做 SetEvent 唤醒 Task——重活交给 Task */
    SetEvent(Task_CtLedTask, Event_100ms);
    /* ❌ 别在回调里调 Rte_Write / Com_SendSignal —— 占死 CPU */
}
```

### 4.5 Schedule Table 驱动周期任务

```c
/* 多任务在同一时刻精确触发 */
StartScheduleTableRel(Table_MainLoop, 0);  /* 启动主循环调度表 */

/* 终结点 EP1（offset=0）同时触发：
   - ActivateTask(Task_Watchdog)      // 看门狗
   - SetEvent(Task_Led, Event_100ms)  // LED 闪烁
   - SetEvent(Task_Can, Event_10ms)   // CAN 通信
*/
```

### 4.6 Error Hook（调试救命稻草）

```c
void Os_Error(StatusType Error, const char* Func, int Line)
{
    /* 常见错误码对照 Os_Types.h：
       E_OS_MISSINGEND       — Task 没 TerminateTask
       E_OS_NO_RES           — Resource 不存在
       E_OS_STATE            — 状态不对
       E_OS_DISALLOWED_CONTEXT — ISR1 里调了 OS API
    */
    LogError(Error, Func, Line);
    while(1) { /* 死循环让调试器断住，看 Error 码 */ }
}
```

---

## 5. 工程落地：加一个 10ms Task 的完整步骤

1. **DaVinci → OS → OsTask → Add**：
   - Name: `Task_10ms`
   - Type: **Extended Task**（要 WaitEvent、可能用 Resource）
   - Priority: 比现有 Task 高（比如现有 50ms Task 是 10，新的给 15）
   - Scheduling: **Preemptive**
   - StackSize: **≥ 512 字节**（先配大，后期优化）

2. **DaVinci → OS → OsEvent → Add**：
   - Name: `Event_10ms`
   - Mask: 默认 0x01（bit0）

3. **DaVinci → OS → OsCounter → Add**：
   - Name: `Counter_Tick`
   - Reference: 绑到 PIT 模块的 1ms 中断源
   - MaxAllowedValue: 65535

4. **DaVinci → OS → OsAlarm → Add**：
   - Name: `Alarm_10ms`
   - Counter: `Counter_Tick`
   - Offset: **10**（PIT 1ms/tick → 实际 10ms）
   - Cycle: **10**（每 10 tick 重复一次）
   - Action: **SetEvent → Task_10ms → Event_10ms**

5. **DaVinci → OS → OsAppMode → 勾选新 Task 和新 Alarm**

6. **重新生成整个 MCAL**（OS 配置变了，必须全量重生成）

7. **在生成的 `Os_PBcfg.c` 里找到 `TASK(Task_10ms)` 入口，加你的逻辑**

---

## 6. 常见坑（按踩坑频率排序，含近期教训）

| # | 坑 | 现象 | 对策 |
|---|---|---|---|
| 1 | Basic Task 忘 `TerminateTask` / `Schedule` | 一个 Task 堵死，其他永远跑不上 | 每个 Task 末尾必须显式让渡 |
| 2 | Extended Task 没 Event/Alarm 触发 | Task 永远 WAITING 状态 | 启动时必须有 Alarm 或其他 Task/ISR 调 SetEvent |
| 3 | ISR 1 里调 OS API | 编译报 `E_OS_DISALLOWED_CONTEXT` | ISR 1 只能调 MCAL，不能调 OS API |
| 4 | Resource 天花板设太低 | 优先级反转没解决，偶发卡死 | 天花板必须 ≥ 所有访问者的最高优先级 |
| 5 | 忘配 Idle Task | OS 跑飞、无法启动 | 规范要求至少 1 个 Idle Task（最低优先级，啥也不干） |
| 6 | Task 栈太小 | 栈溢出、程序跑飞、偶发 Crash | 先配大一点（1024 字节），后期优化 |
| 7 | Alarm Counter 溢出丢事件 | 周期性 Task 偶尔不跑 | 用 OS 内部 MaxAllowedValue，或定期重置 |
| 8 | ISR 和 Task 共享数据没加 Resource | 数据竞争、读到脏数据 | Cat2 ISR 共享用 Resource（触发锁调度，临界区必须极短）；Cat1 ISR 用 `SuspendAllInterrupts` |
| 9 | **把 Counter tick 当毫秒** | Alarm/Schedule Table 定时不准（Offset=10 以为是 10ms，实际是 20ms） | 记住：**物理时间 = tick 数 × tick 物理时长**，tick 物理时长 = PIT/STM 中断周期或 STM 精度 |
| 10 | **把 Event 当函数** | 以为 SetEvent 会直接跑代码 | Event 是 bit 标志位，只是置位；真正"做事"的是 Task 函数体里 WaitEvent 之后的代码 |
| 11 | **Extended Task 直接 return 结束** | OS 报 `E_OS_MISSINGEND` 或行为异常 | 必须显式 `TerminateTask()` |
| 12 | **Schedule Table 忘了启动** | 表配好了永不触发 | 在 StartupHook 或首个 Task 里调 `StartScheduleTableRel/Abs` |
| 13 | **Schedule Table 最后终结点 offset ≥ duration** | 跑到周期外，行为异常 | offset 必须严格递增，最后一个 < duration |
| 14 | **AlarmCallback 里干重活** | 回调跑在 ISR 上下文，占死 CPU 导致其他任务无法调度 | 回调里只做 SetEvent，重活交给 Task |
| 15 | **Cat1 ISR 和 Cat2 ISR 硬件优先级方向搞反** | 行为反了（本该高优先级的被挡住） | Cortex-M 上中断数字**越小越高**；AUTOSAR Task 数字**越大越高**——方向相反 |
| 16 | **Resource 临界区写太长（锁调度）** | Cat2 ISR 和无关 Task 全被挡住，系统实时性雪崩 | 任务 vs Cat2 ISR 共享 Resource 的临界区必须**极短**——写个共享变量值就走 |
| 17 | **`SuspendOSInterrupts` 当成全屏蔽** | Cat1 ISR 照样进来（以为挡得住） | SuspendOSInterrupts **只挡 Cat2**，要挡 Cat1 必须用 `DisableAllInterrupts` 或 `SuspendAllInterrupts` |

---

## 7. 面试高频 Q&A（必背，含新增）

### Q1：AUTOSAR OS 和 FreeRTOS 有什么本质区别？

**答**：核心三点——**静态配置、零动态内存、硬编码确定性**。AUTOSAR OS 所有 Task/ISR/Alarm 在编译时就确定，运行时不能创建/销毁；内存全部静态分配，没有 `pvPortMalloc`；调度策略硬编码优先级天花板解决优先级反转。这三点保证 100% 可预测——下一个 1ms 周期一定会触发。FreeRTOS 动态、灵活、不保证确定性，适合通用嵌入式，不适合汽车安全相关。

### Q2：Basic Task 和 Extended Task 的区别？什么时候用哪个？

**答**：Extended Task 比 Basic 多三个核心能力——**阻塞等待（WaitEvent）、使用事件（Event）、使用资源（Resource）**。Basic 三种状态（RUNNING/READY/SUSPENDED），Extended 五种（多 WAITING）。用 Basic：不需要等待外部事件的简单任务。用 Extended：**需要周期调度（Alarm→Event）、需要 Resource 保护共享数据**的业务任务（绝大多数周期任务都该用 Extended）。

### Q3：ISR Cat1 和 Cat2 的核心区别？为什么要分两类？

**答**：**OS 能不能感知**——Cat1 完全绕过 OS，直接跑硬件 ISR，**不能调任何 OS 服务**（连 SetEvent 都不行）；Cat2 经过 OS 派发，**能调大部分 OS 服务**（SetEvent、IncrementCounter、ActivateTask 等），OS 能把它纳入可调度性分析。Cat1 不被 `SuspendOSInterrupts` 屏蔽但被 `DisableAllInterrupts` 屏蔽。为什么分？Cat1 给"太紧急、太简单"的中断开绿色通道（SysTick、PWM），Cat2 给"需要和 OS 交互"的中断（CAN、ADC、GPT）。

### Q4：Resource 优先级天花板协议是什么？为什么能解决优先级反转？跨层共享为什么要锁调度？

**答**：优先级反转场景：低优先级 Task B 拿 Resource → 中优先级 C 插队打断 B → 高优先级 A 要抢 Resource 但被 B 挡 → A 饿死。天花板协议（Task vs Task）：每个 Resource 配天花板优先级（取最高访问者），**任何 Task 拿到 Resource 后优先级立刻升到天花板**，挡住 C 插队。

**跨层共享（Task vs Cat2 ISR）为什么锁调度？** 因为 Task 软件优先级和 ISR 硬件优先级**不在一个层次，不可比**——没法用天花板协议抬优先级。OS 只能**锁住调度**：挡所有 Task + 所有 Cat2 ISR，让当前 Task 以最快速度跑完临界区、尽快释放 Resource，避免等资源的 Cat2 ISR 被饿太久。代价是无关 Task/ISR 也被挡，临界区必须极短。

### Q5：硬件优先级和软件优先级为什么"不在一个层次"？跨域能比吗？

**答**：硬件优先级（中断）由 NVIC 硬件裁决，永远压过一切 Task；软件优先级（Task）由 OS 调度器裁决，只在 Task 内部排队。**两者跨域不可比**——你不能拿 Task 优先级 20 和 ISR 优先级 3 比，它们不在同一坐标系里。中断必胜的前提是"没被屏蔽"——被临界区屏蔽时中断会排队等待。同域内部才可比：Task vs Task 比软件优先级（数字越大越高），ISR vs ISR 比硬件优先级（Cortex-M 数字越小越高）。

### Q6：三种中断屏蔽锁（DisableAllInterrupts / SuspendAllInterrupts / SuspendOSInterrupts）的区别？

**答**：
| API | Cat1 | Cat2 | 可嵌套 | 典型场景 |
|---|---|---|---|---|
| DisableAllInterrupts | ❌ | ❌ | ❌ | 极短临界区 |
| SuspendAllInterrupts | ❌ | ❌ | ✅ | 一般临界区，保存/恢复旧状态 |
| SuspendOSInterrupts | ✅ | ❌ | ✅ | OS 内部用（Resource 锁调度） |

**方向相反是坑**：AUTOSAR Task 数字越大优先级越高，Cortex-M NVIC 中断数字**越小**越高。

### Q7：Counter 是什么？和硬件定时器什么关系？tick 物理时间怎么算？

**答**：Counter 是 **OS 维护的软件计数器（Ticks）**，硬件定时器（PIT/STM）只是它的心跳源——**Counter 不是硬件定时器**。tick 物理时间 = tick 数 × 每个 tick 的物理时长。每个 tick 的物理时长取决于驱动源：PIT 驱动 = PIT 中断周期（比如 1ms/tick），STM 驱动 = STM 计数精度（ns 级），软件驱动 = 你手动调 IncrementCounter 的周期。这是永恒公式，别把 tick 当毫秒。

### Q8：Counter 的两种驱动方式？OsDriverHighResolution 开关什么意思？

**答**：硬件驱动（99% 场景）：PIT 驱动（普通，中断手动 IncrementCounter，精度=PIT 周期）、STM 驱动（高精度，OS 直接读硬件寄存器，ns 级）。软件驱动：手动调 IncrementCounter，灵活但易受阻塞影响。`OsDriverHighResolution` 勾=用 STM 高精度驱动，不勾=用 PIT 普通驱动。

### Q9：Alarm 和 Counter 的关系？Alarm 四种 Action？

**答**：Counter 是时钟源（由 PIT/STM 驱动 tick），Alarm 是绑在 Counter 上的单点定时——Counter tick 到 Offset 时触发 Action。四种 Action：ActivateTask（激活 Basic Task）、**SetEvent（置 Event 标志位，Extended Task 用——不是函数！）**、IncrementCounter（链式驱动）、AlarmCallback（直接调函数，只做 SetEvent 别干重活）。

### Q10：Event 的本质？为什么说它不是函数？Alarm→Event 完整链路？

**答**：Event 是 **bit mask 标志位**，不是函数。Alarm 配 Action=SetEvent 时，Counter tick 到 Offset → OS 只是把目标 Task 的事件标志位**置 1**，不是调用任何函数。真正"做事"的是 Task 函数体里 `WaitEvent` 之后的那段代码——Task 被置位的 Event 从 WAITING 唤醒变 READY，被调度后才开始跑。机制链：GPT 中断 → Counter tick+1 → Alarm Offset 到期 → OS 置 Event bit → Task 变 Ready → 调度器切换 → WaitEvent 返回 → 跑后续代码。

### Q11：Schedule Table 和 Alarm 的区别？为什么需要它？

**答**：Alarm 三个痛点：相位难保证、一个 Alarm 只能触发一个 Action、无全局时间同步。Schedule Table 是**整套时刻表**：一个 Counter + 若干 Expiry Point（按 offset 排序）+ 每个终结点可挂 N 个 Action。核心优势：同轴天然精确同步、终结点多动作、支持显式全局时间同步（FlexRay 总线）。两种模式：Single-Shot（跑一遍就停）、Repeating（循环跑，用得最多）。一句话：Alarm 是单个闹钟，Schedule Table 是一套同步闹钟群。

### Q12：Hook 是什么？OS 提供哪 6 个标准 Hook？什么时候被调？

**答**：Hook 是 OS 在关键时刻预留的**插入点/回调点**——OS 定好函数原型和调用时机，你只实现函数体，OS 在特定时刻自动调你。6 个标准 Hook：ErrorHook（任何 OS 服务返回非 E_OK 时，最常用）、StartupHook（StartOS 里调度器启动前）、ShutdownHook（ShutdownOS 里调度器停止后）、PreTaskHook（每个 Task 开始前，用于跟踪）、PostTaskHook（每个 Task 结束前，用于测执行时间）、ProtectionHook（保护违例时决定怎么处理）。和 AlarmCallback 的区别：AlarmCallback 是你**主动指定**的业务定时动作，Hook 是 OS **预定义**的系统级插入点。

### Q13：Task 调度的完整流程？

**答**：`StartOS` → OS 初始化所有 Task（AutoStart 的进 READY）→ 调度器选优先级最高的 READY Task 进 RUNNING → 若 Extended Task 等 Alarm，Counter tick 到 Offset 时 ISR 里 SetEvent → Task 从 WAITING 进 READY → 调度器切换 → Task 跑逻辑 → TerminateTask（回 SUSPENDED 等下一次）。

### Q14：为什么 Idle Task 不能删？它在干嘛？

**答**：规范要求每个 OS 配置至少 1 个 Idle Task，优先级必须最低。其他 Task 都不跑时，OS 跑 Idle Task——通常里面调 `SchM_Sleep` 让 MCU 进低功耗模式。没有 Idle Task，调度器找不到可跑的 Task 就飞了。

### Q15：OS 的 AutoStart 和 OsAppMode 是什么？

**答**：OsAppMode 是"应用模式"——不同启动模式可以启用不同的 Task/ISR/Alarm 配置。AutoStart 是 Task 属性——标 AutoStart 的 Task 在 OS 启动时自动进 READY（不需要 Alarm 或 SetEvent 唤醒），通常 Idle Task 必须 AutoStart。AppMode 典型用法：启动模式只启最小集，正常模式启全部。

---

## 8. 一句话总结

> **AUTOSAR OS = 静态配置的确定性调度器**：
> - Task 只有 Basic/Extended 两种（Extended 多三能力：WaitEvent / Event / Resource）
> - ISR 只有 Cat1/2 两类（Cat1 绕过 OS 不调 OS API，Cat2 经过 OS 可调）
> - **硬件优先级（中断）和软件优先级（Task）不在一个层次，跨域不可比**
> - Resource 跨层共享时靠**锁调度**（挡所有 Task + Cat2 ISR），临界区必须极短
> - Event 是 bit 标志位不是函数，Task 函数体 WaitEvent 之后的代码才是真正执行的
> - Schedule Table 是整套同步时刻表，解决 Alarm 相位漂移和单动作限制
> - Hook 是 OS 预留的系统级插入点，ErrorHook 是调试救命稻草
> - Counter 是软件 tick，硬件 PIT/STM 是心跳源，**tick 物理时间 = tick 数 × tick 时长**是永恒公式
>
> 配置核心是"所有东西编译时就确定"——运行时不能改，这是汽车安全的硬约束。
