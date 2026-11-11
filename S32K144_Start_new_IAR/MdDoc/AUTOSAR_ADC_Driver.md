# AUTOSAR ADC Driver（MCAL Adc）学习笔记

> 适用环境：Vector MICROSAR + DaVinci Configurator + NXP S32K144（ADC12 外设）
> 对应 AUTOSAR Classic Platform 4.2，模块名 **Adc**

---

## 1. Adc 模块在 AUTOSAR 里的位置

Adc 属于 **MCAL 层**，负责把模拟信号转换为数字量。没有复杂的上层栈，直接被 SWC/CDD/IoHwAb 调用：

```
┌────────────────────────────────────────────────────┐
│  SWC / CDD / IoHwAb                                 │
│  （本项目：CtLedTask、Adc_ModePractice_Framework）   │
├────────────────────────────────────────────────────┤
│  Adc（MCAL）── Group 调度 + 转换触发 + 结果缓冲      │
├────────────────────────────────────────────────────┤
│  Hardware ── S32K144 ADC0/ADC1（12 位逐次逼近）      │
└────────────────────────────────────────────────────┘
```

**本项目实际配置**（S32K144_Start 工程）：

| 项 | 值 |
|---|---|
| 分辨率 | 12 bit（0 ~ 4095） |
| 触发源 | 硬件触发（HW Trigger） |
| 转换模式 | OneShot（单次转换） |
| 硬件平均 | 关闭 |
| DMA | 未使用 |
| 参考电压 | VREFH / VREFL |
| Group | AdcGroup0，带 Notification 回调 `AdcGroup0Notification()` |

---

## 2. 核心概念模型：Channel → Group → HwUnit

Adc 的配置也是一棵三层树（与 Spi 的 Channel/Job/Sequence 对仗，面试必考）：

```
HwUnit（物理 ADC 外设：ADC0 / ADC1）
   ↑ 被引用
Group = Channel 0 + Channel 1 + ...        ← Group = 转换与调度的最小单位
   ↑ 包含
Channel（一个模拟输入通道的引用 + 采样参数）
```

| 概念 | 定义 | 类比 |
|---|---|---|
| **Channel** | 一个模拟输入引脚的逻辑引用（如 ADC0_SE4） | 一路电压信号 |
| **Group** | 一组 Channel 的有序集合，**触发、转换、读取都以 Group 为单位** | "一次采集这几个通道" |
| **HwUnit** | 物理 ADC 外设实例（S32K144 有 ADC0/ADC1） | 哪个 ADC 模块干活 |

要点：
- **一次触发，整个 Group 的所有 Channel 顺序转换**，结果按序放入结果缓冲
- 同一 HwUnit 上多个 Group 可以配**优先级**（AdcGroupPriority），驱动按队列仲裁
- 不同 Group 可以映射到不同 HwUnit，实现并行采样

---

## 3. 四个配置维度（决定 Group 行为的关键）

每个 AdcGroup 由四个正交属性定义，组合出不同用法：

### 3.1 触发源 AdcGroupTriggSrc

| 值 | 含义 | 启动 API |
|---|---|---|
| `ADC_TRIGG_SRC_SW` | 软件触发 | `Adc_StartGroupConversion()` 启动一次 / 启动连续 |
| `ADC_TRIGG_SRC_HW` | 硬件触发（PDB/定时器/外部引脚） | `Adc_EnableHardwareTrigger()` 使能后由硬件自动触发 |

### 3.2 转换模式 AdcGroupConversionMode

| 值 | 含义 |
|---|---|
| `ADC_CONV_MODE_ONESHOT` | 触发一次，Group 转换一轮就停 |
| `ADC_CONV_MODE_CONTINUOUS` | 触发后不停转换，直到 `Adc_StopGroupConversion()` |

### 3.3 访问模式 AdcGroupAccessMode

| 值 | 含义 |
|---|---|
| `ADC_ACCESS_MODE_SINGLE` | 每次转换只保留最新一轮结果（每 Channel 1 个值） |
| `ADC_ACCESS_MODE_STREAMING` | 连续存 N 轮采样到缓冲（流式批量采集） |

### 3.4 流缓冲模式 AdcStreamingBufferMode（仅 Streaming 有效）

| 值 | 含义 |
|---|---|
| `ADC_STREAM_BUFFER_LINEAR` | 缓冲写满 N 个采样后自动停止 |
| `ADC_STREAM_BUFFER_CIRCULAR` | 环形缓冲，写满回卷覆盖最旧数据，永不停止 |

### 组合合法性（重点记忆）

| 组合 | 是否合法 | 说明 |
|---|---|---|
| OneShot + Single | ✔ | 最常用：触发一次读一轮 |
| OneShot + Streaming(Linear) | ✔ | 触发一次采 N 轮后自动停 |
| Continuous + Streaming(Circular) | ✔ | 持续后台采样，随时取最新 |
| **Continuous + Linear** | ✘ **矛盾组合** | Linear 写满即停，与 Continuous 的"不停"语义冲突 |
| **Continuous + Single** | ✘ 不建议 | 无缓冲积累，意义不大且中断风暴 |

---

## 4. 结果缓冲（Buffer）规则

**长度公式（必记）**：

```
BUF_LEN = ChannelCount × StreamingNumSamples
```

| 配置 | 缓冲布局 |
|---|---|
| 1 ch + Single | `[ch0]` 长度 1 |
| 3 ch + Single | `[ch0, ch1, ch2]` 长度 3 |
| 1 ch + Streaming(8) | `[s0..s7]` 长度 8 |
| 3 ch + Streaming(4) | `[c0c1c2, c0c1c2, ...]` 4 轮 × 3 通道 = 12 |

缓冲由用户分配、通过 `Adc_SetupResultBuffer(Group, buf)` 绑定，**必须保持有效直到 Stop**。

---

## 5. Group 状态机

```
IDLE ──Start/EnableHwTrigger──> BUSY ──一轮转换完成──> IDLE(OneShot)
                                  │                        或 STREAM_COMPLETED(Linear 写满)
                                  ├──Streaming/Circular──> 持续 BUSY
                                  └──StopGroupConversion──> IDLE
```

查询 API：`Adc_GetGroupStatus()` → `ADC_IDLE / ADC_BUSY / ADC_COMPLETED / ADC_STREAM_COMPLETED`。

**关键语义**：`ADC_COMPLETED` 只表示"缓冲里有新数据可读"；**`Adc_ReadGroup()` 读走后状态回到 BUSY/IDLE**，不读就一直保持 COMPLETED。

---

## 6. 配置要点（DaVinci / EB tresos）

### 6.1 AdcGeneral 全局开关

| 参数 | 作用 |
|---|---|
| `AdcEnableStartStopGroupApi` | 生成 Start/Stop API（软件触发必须 ON） |
| `AdcHwTriggerApi` | 生成 `Adc_EnableHardwareTrigger()`（HW 触发必须 ON） |
| `AdcGrpNotifCapability` | 使能 Group 完成回调 |
| `AdcReadGroupApi` | 生成 `Adc_ReadGroup()` |
| `AdcDevErrorDetect` | 开发期 ON |

### 6.2 AdcHwUnit（S32K144 ADC12 特性）

| 参数 | 要点 |
|---|---|
| 分辨率 | 8/10/12 bit（本项目 12） |
| 参考电压 | VREFH/VREFL，注意硬件上 VREFH 是否接稳（悬空读数全飘） |
| 时钟源/分频 | ADC 时钟有上限（S32K144 典型 ≤ 40 MHz 量级），分频别超 |
| 采样时间 | 高阻抗信号源要拉长采样时间，否则充不满采样电容、读数偏低 |
| 硬件平均 | AVGE，4/8/16/32 次平均，抑噪但增加转换时间 |
| **校准** | S32K144 ADC 上电必须完成自校准再使用（MCAL 通常在 Init 里做） |

### 6.3 AdcGroup（核心）

| 参数 | 要点 |
|---|---|
| TriggSrc / ConversionMode / AccessMode / BufferMode | 见第 3 节组合表 |
| StreamingNumSamples | Streaming 的缓冲深度 N |
| Notification | 完成回调函数名（如 `AdcGroup0Notification`） |
| AdcGroupPriority | 同 HwUnit 多 Group 的仲裁优先级 |
| **触发源硬件关联** | HW 触发选 PDB 时，还需在 PDB 模块配置触发周期与通道 |

### 6.4 配套模块

- **Port**：模拟输入引脚配成 analog 复用
- **PDB**（HW 触发时）：产生周期性触发信号
- **中断**：Adc 中断优先级规划（见常见坑 #1）

---

## 7. 使用要点（代码）

### 标准流程（软件触发 + OneShot + Single）

```c
/* 初始化阶段（一次） */
Adc_Init(&Adc_Config);
Adc_SetupResultBuffer(AdcConf_AdcGroup_AdcGroup0, ResultBuffer);

/* 周期任务里：触发 → 等完成 → 读取 */
Adc_StartGroupConversion(AdcConf_AdcGroup_AdcGroup0);
/* OneShot 很快完成；可用 Notification 回调或轮询状态 */
while (Adc_GetGroupStatus(AdcConf_AdcGroup_AdcGroup0) != ADC_COMPLETED) { }
Adc_ReadGroup(AdcConf_AdcGroup_AdcGroup0, &adcValue);
```

### 硬件触发模式

```c
Adc_SetupResultBuffer(AdcConf_AdcGroup_AdcGroup0, ResultBuffer);
Adc_EnableHardwareTrigger(AdcConf_AdcGroup_AdcGroup0);  /* 之后 PDB 自动周期触发 */
/* Notification 回调或任务里 Adc_ReadGroup() 取数 */
```

### Streaming + Circular（持续后台采样）

```c
Adc_StartGroupConversion(group);   /* Continuous + Circular */
/* 任意时刻取最新采样位置 */
Adc_StreamNumSampleType valid;
valid = Adc_GetStreamLastPointer(group, &samplePtr);
/* samplePtr 指向缓冲中最新一轮的起始，valid = 当前有效采样轮数 */
```

### ReadGroup vs GetStreamLastPointer（区别常考）

| API | 行为 |
|---|---|
| `Adc_ReadGroup()` | **拷贝**一轮（或整个缓冲）到用户缓冲，并把状态清为可读下一轮的标记 |
| `Adc_GetStreamLastPointer()` | **不拷贝**，只返回驱动缓冲中最新一轮的指针和有效轮数，零拷贝但内部有临界区 |

### Notification 回调纪律（本项目血泪经验）

```c
void AdcGroup0Notification(void)
{
    /* ✔ 只做：置标志位 / 读走数据 */
    /* ✘ 禁止：Rte_Write、Com_SendSignal、浮点运算、阻塞调用 */
}
```

---

## 8. 常见坑（按踩坑频率排序，含本项目实测教训）

| # | 坑 | 现象 | 对策 |
|---|---|---|---|
| 1 | **Continuous 模式中断回调里做重操作**（Rte_Write/Com_SendSignal） | 中断频率 = 转换频率（可达 kHz 级），CPU 被占满，其它任务饿死 | 回调里只置标志，业务放周期任务处理 |
| 2 | **Continuous + Linear 缓冲** | 语义矛盾，行为异常 | Continuous 必须配 Circular |
| 3 | 以为 Streaming 能降低中断频率 | NXP S32K MCAL 的 Streaming 只是批量缓冲，**中断照样每次转换触发** | 降频率要靠降低触发源频率（PDB 周期），不是缓冲 |
| 4 | 中断里频繁调 `GetStreamLastPointer` | 内部临界区开关中断开销大，周期任务调度被挤占 | 改到周期任务里调 |
| 5 | **ADC 中断优先级 ≥ CAN 中断** | ADC 高频中断阻塞 CAN 收发，通信丢帧 | ADC 中断优先级必须低于 CAN |
| 6 | 忘了 `Adc_SetupResultBuffer` | StartGroupConversion 返回错误/数据丢 | Init 后先绑缓冲再触发 |
| 7 | ReadGroup 后才发现没转换完 | 读到旧数据 | 先查 `ADC_COMPLETED` 或在 Notification 里读 |
| 8 | VREFH 硬件没接稳 / 采样时间太短 | 读数漂移、偏低 | 查硬件参考源；高阻信号拉长采样时间 |
| 9 | 缓冲长度算错 | 数据错位/越界 | 死记 `BUF_LEN = 通道数 × StreamingNumSamples` |
| 10 | HW 触发只配了 ADC 没配 PDB | 永远等不到触发 | ADC + PDB 两侧都要配 |

---

## 9. 面试要点（高频 Q&A）

### Q1：Adc 的 Channel、Group、HwUnit 是什么关系？

**答**：Channel 是一路模拟输入的逻辑引用；Group 是一组有序 Channel，是触发/转换/读取的最小单位；HwUnit 是物理 ADC 外设。一次触发，Group 内所有 Channel 顺序转换，结果按序入缓冲。类比 Spi 的 Channel/Job/Sequence。

### Q2：软件触发和硬件触发的区别？各自适用场景？

**答**：软件触发用 `Adc_StartGroupConversion()`，由任务主动发起，适合"我要数据时才采"（如按键检测）；硬件触发用 `Adc_EnableHardwareTrigger()`，由 PDB/定时器自动周期触发，CPU 零参与，适合固定频率采样（如电流环反馈、相电压采样）。电机控制类项目几乎都用 HW 触发保证采样间隔精确。

### Q3：OneShot 和 Continuous 的区别？

**答**：OneShot 触发一次转一轮就停；Continuous 触发后循环转换直到 Stop。Continuous 要警惕中断频率——每次转换都可能进中断，回调里绝不能做重操作，否则 CPU 被占满。

### Q4：Single 和 Streaming 访问模式区别？Linear 和 Circular 呢？

**答**：Single 每次只保留最新一轮结果；Streaming 把连续 N 轮采样存进缓冲。Linear 写满 N 轮自动停止（适合"采一段就分析"）；Circular 环形回卷永不停止（适合"后台持续监控"）。**Continuous 只能配 Circular**，配 Linear 是矛盾组合。

### Q5：结果缓冲长度怎么算？

**答**：`BUF_LEN = ChannelCount × StreamingNumSamples`。3 通道 Single 是 3；3 通道 Streaming(4) 是 12，布局为一轮 3 个通道值连续排 4 轮。

### Q6：`Adc_ReadGroup` 和 `Adc_GetStreamLastPointer` 有什么区别？

**答**：ReadGroup 把数据**拷贝**到用户缓冲并清除完成标志；GetStreamLastPointer **不拷贝**，返回驱动缓冲里最新一轮的指针和有效轮数，零拷贝但有临界区开销。高频场景用后者省拷贝，但别在中断里频繁调。

### Q7：Notification 回调里能做什么？

**答**：只能做轻量操作：置标志位、读走数据。禁止 Rte_Write、Com_SendSignal、阻塞调用——Continuous 模式下回调频率等于转换频率，重操作会占满 CPU 导致其它任务（含通信任务）饿死。

### Q8：ADC 中断优先级怎么规划？

**答**：必须低于对实时性要求更高的通信中断（CAN）。ADC 高频转换中断若优先级过高，会阻塞 CAN 收发导致丢帧。原则：**采样可以晚一点，通信不能断**。

### Q9：S32K144 的 ADC 使用前要做什么硬件准备？

**答**：① 自校准（上电后必须完成）；② VREFH/VREFL 参考电压接稳；③ 模拟引脚 Port 配成 analog；④ 高阻抗信号源拉长采样时间。少了校准读数不准，少了参考电压读数全飘。

### Q10：为什么需要 PDB 模块？

**答**：PDB（Programmable Delay Block）产生精确的周期性触发信号给 ADC。用定时器硬件触发 ADC，采样间隔由硬件保证，不受软件调度抖动影响——这是电机控制 FOC 电流采样的标准做法。

### Q11：多 Group 抢一个 ADC 外设怎么办？

**答**：给 Group 配 AdcGroupPriority，驱动内部排队仲裁，高优先级 Group 可抢占/插队（取决于 MCAL 实现的队列模式）。同 HwUnit 的 Group 本质共享一个转换器，物理上不可能同时转。

### Q12：ADC 什么时候需要 DMA？

**答**：Streaming 深缓冲、高采样率场景。中断搬运每轮结果 CPU 开销大，DMA 直接写内存，CPU 只在半满/全满时处理。本项目采样需求低，FIFO/中断方式足够，故未开 DMA。

---

## 10. 一句话总结

> **触发管 Group，转换按序采 Channel，结果缓冲长度 = 通道数 × 采样轮数。**
> 配置核心是想清四个维度的组合：**触发源 × 转换模式 × 访问模式 × 缓冲模式**；
> 使用核心是记住 Continuous 模式的纪律：**回调要轻、缓冲要 Circular、中断优先级要让着 CAN**。
