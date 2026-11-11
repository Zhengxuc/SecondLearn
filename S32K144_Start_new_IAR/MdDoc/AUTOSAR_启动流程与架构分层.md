# AUTOSAR 启动流程 + Runnable vs Task 架构分层

> 适用工程：S32K144 + Vector MICROSAR + DaVinci Configurator + IAR
> 从"上电复位"到"业务 Runnable 跑起来"的每一步，附工程真实代码路径

---

## 0. 两个必须先搞懂的问题

| 问题 | 回答 |
|---|---|
| **main 函数在哪?** | `Appl\BrsAsrMain.c` L357 |
| **Runnable 里为什么没有 TerminateTask/WaitEvent?** | Runnable 是**应用层业务函数**，Task 框架是**RTE 自动生成**的。分层决定了它们不是一个层级的东西——详见第 4 节 |

---

## 1. 从上电复位到稳定运行：完整 9 步启动链

### Step ① 复位向量 → Reset_Handler（硬件自动）

```
MCU 上电 → CPU 读复位向量表（启动汇编 startup.c）
          → 关全局中断
          → RAM 清零（.bss 段）
          → 复制 .data 段从 Flash 到 RAM
          → 跳 main()
```

**工程文件**: `Device\startup.c`（NXP S32K144 官方启动代码）

### Step ② main() —— OS 初始化 + 跳入 EcuM

**工程文件**: `Appl\BrsAsrMain.c` L357-L372

```c
int main(void)
{
  Os_InitMemory();   // 清 OS 内部 RAM（Task 栈、Event 位、Queue…）
  Os_Init();         // 载入 OS 配置表（生成的 Os_PBcfg.c）

  EcuM_Init();       // ← 进入 EcuM 状态机，从不返回！

  return 0;          // 永远不会执行到这里
}
```

**关键理解**: `EcuM_Init()` 之后 main 就没了——OS 调度器接管，`return 0` 是死代码。

### Step ③ EcuM Startup One —— BSW 模块 RAM 清零

EcuM 状态机自动调 `EcuM_AL_DriverInitZero()`——**所有 BSW 模块的 `_InitMemory()`**:

```c
// EcuM_Callout_Stubs.c L242-L268
BswM_InitMemory();  CanIf_InitMemory();  CanSM_InitMemory();
Can_InitMemory();   Com_InitMemory();    Dcm_InitMemory();
PduR_InitMemory();  Dem_InitMemory();    Rte_InitMemory();
Det_InitMemory();   ComM_InitMemory();   CanTp_InitMemory();
Dem_PreInit(Dem_Config_Ptr);
```

### Step ④ EcuM Startup One —— MCAL 初始化 + PLL 切换（心跳时刻）

**工程文件**: `EcuM_Callout_Stubs.c` L273-L293

```c
void EcuM_AL_DriverInitOne(void)
{
  Mcu_Init( &McuModuleConfiguration );                             // 写模式 + RAM 配置
  Mcu_InitClock(McuConf_McuClockSettingConfig_McuClockSettingConfig_0); // 配 PLL 但不等待
  while (MCU_PLL_LOCKED != Mcu_GetPllStatus());                    // ← 等几百毫秒
  Mcu_DistributePllClock();                                        // ← 心跳切换！从 FIRC→SPLL
  Port_Init( &PortConfigSet );                                     // 引脚方向/复用
  Spi_Init( NULL_PTR );
  Adc_Init( NULL_PTR );
  Pwm_Init( NULL_PTR );
}
```

**三段式原因**: PLL 锁定要时间，InitClock 写完立即返回让 CPU 继续在 FIRC(48MHz)上跑，锁好再 Distribute 切换——**节省启动时间 + 避免瞬间频率突变死机**。

> 漏掉 `Mcu_DistributePllClock` 的后果: 系统永远跑在 FIRC 48MHz，所有波特率/定时器都偏（比如 CAN 实际跑 52kbit/s 而不是 500kbit/s）。

### Step ⑤ 更多 BSW 模块初始化（Com/CanIf/Dem/Dcm/Nvm…）

EcuM 按配置顺序自动调用: `Can_Init → CanIf_Init → PduR_Init → Com_Init → ComM_Init → CanSm_Init → Dcm_Init → Dem_Init → EcuM_EnableWakeupSources`…

### Step ⑥ StartOS —— OS 调度器启动！

**工程文件**: `EcuM_Callout_Stubs.c` L318-L346

```c
void EcuM_StartOS(AppModeType appMode)
{
  StartOS(appMode);   // ← OS 调度器从这里启动，StartOS 正常不返回
}
```

**StartOS 做了什么**:
1. 调用 **StartupHook**（在调度器启动前，你可以在这里做 Schedule Table 启动等）
2. 把 **AutoStart 勾上的 Task** 进 READY 队列
3. 第一个 READY Task 进 RUNNING

### Step ⑦ 第一个跑的 Task: Default_Init_Task

**工程文件**: `Appl\BrsAsrMain.c` L281-L318

```c
TASK(Default_Init_Task)    // AutoStart 勾上，OS 保证第一个调度到它
{
  EcuM_StartupTwo();        // ← 启动第二阶段

  /* 把所有 OS ISR 中断源打开 */
  for (i = 0; i < OS_ISRID_COUNT; i++) {
    Os_EnableInterruptSource(i, TRUE);
  }
  TerminateTask();
}
```

**为什么不把 StartupTwo 放在 main 里**: 因为 StartOS 之后才有**多任务和中断**——`Rte_Start`、`NvM_ReadAll` 这些操作需要 OS 调度器和中断配合。

### Step ⑧ EcuM_StartupTwo —— RTE + 业务层启动

```
Rte_Start()           启用 RTE 调度框架
NvM_ReadAll()         读所有非易失数据（断电保存的配置/故障码）
BswM 切到正常模式     状态机从 STARTUP→RUN
ComM 启动通信管理     CAN 收发开始
Counter + Alarm 启动  定时任务开始计时
```

### Step ⑨ 稳定运行态 —— OS 在各个 Task/ISR 间轮转

```
┌─ Cat1 ISR ──────────────────────┐  随时能打断一切（硬件层）
│  SysTick / ADC 清标志          │
├─ Cat2 ISR ──────────────────────┤  随时能打断 Task（被 SuspendOSInterrupts 挡住）
│  CAN 接收 → CanIf_RxIndication │
│  GPT → Os_IncrementCounter     │
├─ Task 层 ───────────────────────┤
│  OsTask_APP (Basic, 500ms)     │  Alarm ActivateTask → 跑 LedRunnable → TerminateTask
│  OsTask_BSW_SCHM (Extended)    │  WaitEvent ← Alarm SetEvent → 跑 BSW MainFunctions
│  IdleTask (最低优先级)          │  其他都不跑时 → Sleep 省电
└──────────────────────────────────┘
```

---

## 2. 架构分层图（核心！解释了 Runnable 为什么没有 TerminateTask）

```
┌────────────────────────────────────────────────────┐
│  ASW 应用软件层（你手写的业务代码）                  │
│  ┌──────────────────────────────────────────────┐  │
│  │  SWC 组件                                     │  │
│  │    ├─ LedRunnable()     ← 你写的函数          │  │
│  │    ├─ EMC_Runnable_10ms()                    │  │
│  │    ├─ PWL_Runnable_20ms()                    │  │
│  │    └─ ...                                    │  │
│  │  ❌ 这里绝对不能出现 WaitEvent/TerminateTask  │  │
│  └──────────────────────────────────────────────┘  │
├────────────────────────────────────────────────────┤
│  RTE 层（DaVinci + RTE Generator 自动生成）        │
│  ┌──────────────────────────────────────────────┐  │
│  │  Task 框架（Rte_SystemApplication_OsCore0.c） │  │
│  │  TASK(OsTask_APP) {                            │  │
│  │    LedRunnable();        ← 自动映射调用       │  │
│  │    (void)TerminateTask(); ← 自动生成          │  │
│  │  }                                             │  │
│  │  TASK(OsTask_BSW_SCHM) {                       │  │
│  │    for(;;) {                                   │  │
│  │      WaitEvent(...);  ← 自动生成 Extended 逻辑 │  │
│  │      if(ev & Event_10ms) BswM_MainFunction();  │  │
│  │      ...                                       │  │
│  │    }                                           │  │
│  │  }                                             │  │
│  └──────────────────────────────────────────────┘  │
├────────────────────────────────────────────────────┤
│  BSW 基础软件层                                    │
│  OS / Com / CanIf / NvM / Dcm / EcuM / …         │
├────────────────────────────────────────────────────┤
│  MCAL 驱动层                                       │
│  Mcu / Port / Spi / Adc / Can / Pwm / Gpt        │
└────────────────────────────────────────────────────┘
```

**三个关键边界约束**:
| 边界 | 约束 | 原因 |
|---|---|---|
| ASW 不能直接调 OS API | Runnable 里绝对不能出现 `WaitEvent`/`TerminateTask` | **可移植性**: 同一个 Runnable 理论上能跑在 FreeRTOS/Linux 上，绑死 Vector OS 就废了 |
| RTE 层自动包 Task 框架 | DaVinci 里 Mapping 操作告诉 RTE Generator "这个 Runnable 跑在哪个 Task"，RTE 自动生成完整 Task 函数体 | **职责分离**: 业务逻辑管"做什么"，OS 框架管"什么时候做、怎么调度" |
| BSW/MCAL 不感知业务 | 底层不知道上面有几个 SWC、几个 Runnable | **分层解耦**: 换 MCU 或换 OS，业务层不用改 |

---

## 3. 工程里两个真实 Task 例子（RTE 自动生成的框架）

### 例子 1: OsTask_APP（Basic Task + Alarm ActivateTask）

**RTE 生成代码**: `Appl\GenData\Rte_SystemApplication_OsCore0.c` L429-L436

```c
/* Task: OsTask_APP, Priority: 15, Cycle Time: 0.5s */
TASK(OsTask_APP)
{
  LedRunnable();                  // ← 你的 Runnable 在这里被调用
  (void)TerminateTask();          // ← RTE 自动生成的 Basic Task 结尾
}
```

**工作模式**: Alarm 每 500ms `ActivateTask(OsTask_APP)` → Task 进 READY → 被调度 → 跑 LedRunnable → TerminateTask 回 SUSPENDED → 等下一次 Alarm。

### 例子 2: OsTask_BSW_SCHM（Extended Task + Alarm SetEvent）

**RTE 生成代码**: `Rte_SystemApplication_OsCore0.c` L443-L500+

```c
/* Task: OsTask_BSW_SCHM, Priority: 10 */
TASK(OsTask_BSW_SCHM)
{
  EventMaskType ev;

  for(;;)   /* Extended Task 永远不退出 */
  {
    /* 卡在这里等多个 Event 掩码（10ms/20ms/CanTp） */
    (void)WaitEvent(Rte_Ev_Cyclic2_OsTask_BSW_SCHM_0_10ms
                   | Rte_Ev_Cyclic2_OsTask_BSW_SCHM_0_20ms
                   | Rte_Ev_Run_CanTp_CanTp_MainFunction);
    (void)GetEvent(OsTask_BSW_SCHM, &ev);
    (void)ClearEvent(ev);

    if ((ev & Event_10ms) != 0)
    {
      BswM_MainFunction();            // ← 不同 Event 触发不同 Runnable
      CanSM_MainFunction();
      Com_MainFunctionRx();
      Dcm_MainFunction();
    }
    if ((ev & Event_20ms) != 0)
    {
      ComM_MainFunction_0();
    }
    if ((ev & Event_CanTp) != 0)
    {
      CanTp_MainFunction();
    }
  }
}
```

**工作模式**: 上电 AutoStart 激活一次 → 进 WaitEvent 卡住 → 10ms/20ms Alarm 置对应 Event → Task 被唤醒 → 根据 Event 掩码决定跑哪些 Runnable → 回到 WaitEvent 循环。

**关键**: 你看——**TerminateTask/WaitEvent/ClearEvent 全在 RTE 生成的代码里，你写的 Runnable 里一个都没有**。这就是分层设计。

---

## 4. DaVinci 里的 Mapping 到底做了什么

你在 DaVinci → SWC → Runnable 里做的 Mapping 操作，实际上是告诉 **RTE Generator**:

> "这个 Runnable（LedRunnable）要跑在这个 Task（OsTask_APP）里，这个 Task 的类型是 Basic，由这个 Alarm 周期激活"

RTE Generator 拿到这个信息后,**自动**:
1. 生成 `TASK(OsTask_APP)` 函数体（含 TerminateTask/WaitEvent 等 OS 调用）
2. 生成 Event 掩码定义（`Rte_Ev_Cyclic2_...`）
3. 生成 RTE 的头文件 `Rte_CtLedTask.h`（声明 Runnable 原型）
4. 把 Task 和 Runnable 的绑定关系写进 RTE 配置

**所以你不需要、也绝对不应该在 Runnable 里写 OS 调用**——RTE 替你包好了。

---

## 5. Error[Lc036] no block matches 的真正根因（复习）

你前几天遇到的链接错误:

```
Error[Lc036]: no block or place matches the pattern
"zi section .OS_STACK_OSCORE0_TASK_PRIO20_VAR_NOINIT
 in Os_Stack_Lcfg.o symbols: [OsCfg_Stack_OsCore0_Task_Prio20_Dyn]"
```

**根因**: 你在 DaVinci 里新增了 3 个 Task(Prio20、Prio25、OsTask_EMC_PWL),DaVinci 重新生成了 `Appl\GenData\Os_Link_Core0_Stacks.icf`,但 IAR 工程实际引用的是 `Device\Os_Link_Core0_Stacks.icf`(旧版本,没这三个段)。

**修法**: 每次改 OS 配置后,**手动把 `Appl\GenData\Os_Link_Core0_Stacks.icf` 拷到 `Device\Os_Link_Core0_Stacks.icf` 覆盖**,再编译。

---

## 6. 启动链关键节点记忆卡

| 步骤 | 关键 API | 本质 | 能卡掉什么 |
|---|---|---|---|
| ② | `Os_InitMemory + Os_Init` | OS 内部初始化 | 没调用 → OS 状态机全错 |
| ④ | `Mcu_DistributePllClock` | **心跳切换点**(FIRC→SPLL) | 没调 → 系统永远 48MHz,所有波特率/定时器都错 |
| ⑥ | `StartOS` | **调度器启动点**,main 从此不返回 | 没调 → 永远单任务,中断不触发 |
| ⑦ | `Default_Init_Task` 里 `EcuM_StartupTwo` | RTE + NvM + BSW 模式切换 | 没调 → RTE 不跑、NvM 数据全丢、Com 不通 |

---

## 7. 面试高频 Q&A

### Q1:main 函数在哪?EcuM_Init 之后发生了什么?

**答**:main 在 `Appl\BrsAsrMain.c`,调了 `Os_InitMemory → Os_Init → EcuM_Init`。EcuM_Init 从不返回——它在 Startup One 阶段调 MCAL 初始化(尤其 Mcu_InitClock + DistributePllClock 切主频),然后 StartOS 启动调度器,main 里 EcuM_Init 后面的 `return 0` 是死代码。StartOS 后第一个跑的是 Default_Init_Task(AutoStart),里面调 EcuM_StartupTwo 启动 RTE + NvM + BSW 模式切换,然后进入稳定运行。

### Q2:Runnable 里为什么没有 TerminateTask/WaitEvent?谁负责调用 Runnable?

**答**:因为 Runnable 是**ASW 应用层业务函数**,绝对不能感知 OS 存在——这是 AUTOSAR 架构分层的硬约束(保证可移植性,同一个 Runnable 能跑在不同 OS/平台上)。RTE 层自动生成的 **Task 框架**(在 `Rte_SystemApplication_OsCore0.c` 里)负责调用 Runnable——Basic Task 框架自动加 TerminateTask,Extended Task 框架自动加 WaitEvent/GetEvent/ClearEvent 和事件分发逻辑。你在 DaVinci 里做的 Mapping 操作告诉 RTE Generator "Runnable 跑在哪个 Task",然后 RTE Generator 自动生成完整 Task 函数体。

### Q3:Mcu 初始化为什么分 InitClock 和 DistributePllClock 两步?

**答**:PLL 锁定需要几百毫秒。InitClock 写完 PLL 配置立即返回(此时 CPU 还跑在默认的 FIRC/SIRC 临时源上),EcuM 趁机继续初始化 Can/Adc/OS 等模块(节省启动时间);等 PLL 锁好后 DistributePllClock 才把系统时钟切到 PLL。拆开做=缩短启动时间 + 避免瞬间频率突变死机。漏掉第三步=系统永远跑在默认 48MHz,所有波特率/定时器都偏。

### Q4:Error[Lc036] no block matches 是什么错?怎么定位?

**答**:IAR 链接器找不到 icf 脚本里定义的段 → 通常是 Os_Link_Core0_Stacks.icf 版本过期。每次在 DaVinci 里改了 Task 配置(新增/删除/改栈大小),OS 会重新生成 icf 到 `Appl\GenData\Os_Link_Core0_Stacks.icf`,但 IAR 工程引用的 `Device\Os_Link_Core0_Stacks.icf` 没自动更新。修法:手动把 GenData 里的拷到 Device 覆盖。这个错本质是"OS 配置变了但链接脚本没跟着变"。

### Q5:Default_Init_Task 为什么要单独存在?为什么不在 main 里做 EcuM_StartupTwo?

**答**:因为 `EcuM_StartupTwo` 里要调 `Rte_Start`、`NvM_ReadAll`、开中断源——这些操作需要 **OS 调度器已经启动**(多任务上下文)和 **中断已经可用**。main 里调 StartOS 之前没有这两样,所以 StartupTwo 必须放在 StartOS 之后。Default_Init_Task 是 AutoStart 的 Task,OS 保证第一个调度到它,这样 StartupTwo 在调度器启动后的"最干净"状态下执行。

---

## 8. 一句话总结

> **启动链**:Reset→main(Os_InitMemory/Os_Init)→EcuM_Init(关中断→MCAL+PLL 切心跳→BSW 初始化→StartOS)→Default_Init_Task(EcuM_StartupTwo:RTE+业务)→稳定运行。
> **分层**:ASW 业务层只写 Runnable(绝对不能有 OS API),RTE 层按 DaVinci Mapping 自动包 Task 框架(TerminateTask/WaitEvent 全在这),BSW/MCAL 不感知上层。
> **核心分界点**: `Mcu_DistributePllClock`(主频切了)和 `StartOS`(多任务接管)——两个记住就不会迷路。
