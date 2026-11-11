# AUTOSAR SPI Handler/Driver（MCAL Spi）学习笔记

> 适用环境：Vector MICROSAR + DaVinci Configurator + NXP S32K144（LPSPI 外设）
> 对应 AUTOSAR Classic Platform 4.2，模块名 **Spi**（SPI Handler/Driver）

---

## 1. Spi 模块在 AUTOSAR 里的位置

Spi 属于 **MCAL 层**的通信驱动，全称 SPI Handler/Driver。它不像 CAN/LIN 那样有完整的通信栈（Com/PduR），而是直接被上层使用：

```
┌────────────────────────────────────────────────────┐
│  CDD / IoHwAb / EEP/FLS 等上层模块                  │
│  （本项目：CddSbc —— SBC 系统基础芯片驱动）          │
├────────────────────────────────────────────────────┤
│  Spi（MCAL）── Channel/Job/Sequence 调度 + 传输     │
├────────────────────────────────────────────────────┤
│  Hardware ── S32K144 LPSPI0/1/2                    │
└────────────────────────────────────────────────────┘
```

典型使用场景：读写 SBC（系统基础芯片）、外部 EEPROM/Flash、SPI 接口传感器、外扩 ADC 等。

**本项目实际配置**（S32K144_Start 工程）：

| 项 | 值 |
|---|---|
| 外设 | LPSPI_1（LPSPI_0/2 关闭） |
| 功能级别 | LEVEL 0（简单同步） |
| 缓冲类型 | 仅 EB（外部缓冲） |
| Channel | 1 个：`SpiChannel_SBC_Read` |
| Job / Sequence | 各 1 个：`SpiJob_0` / `SpiSequence_0_SBC` |
| 外部设备 | SBC（片选 PCS3） |
| 时序 | Mode 0（CPOL=0, CPHA=第一个边沿采样），100 kHz，8 bit，MSB 先传 |
| DMA | 未使用 |

---

## 2. 核心概念模型（最重要）：Channel → Job → Sequence

Spi 驱动的配置本质是一棵**三层树**，面试必考：

```
ExternalDevice（外部设备：片选/波特率/时序参数）
      ↑ 被引用
   ┌──Job 0──────────────────┐
   │  Channel 0 → Channel 1 → Channel 2 ...│   ← Job = 一次片选有效的完整传输
   └─────────────────────────┘
      ↑ 被引用
Sequence = Job 0 → Job 1 → Job 2 ...        ← Sequence = 一次完整调度单位
```

| 概念 | 定义 | 类比（以读写 SBC 为例） |
|---|---|---|
| **Channel** | 一段连续数据的缓冲描述（宽度/长度/缓冲区），**传输的最小数据单位** | "读寄存器命令 2 字节" |
| **Job** | 一个或多个 Channel 的有序集合，**绑定一个 ExternalDevice（即一种片选+时序）**，片选拉低到抬高的完整过程 | "对 SBC 发起一次完整读操作" |
| **Sequence** | 一个或多个 Job 的有序集合，**传输调度的最小单位**（SyncTransmit/AsyncTransmit 传的参数就是它） | "先读状态寄存器，再写控制寄存器" |
| **ExternalDevice** | 外部设备参数集：波特率、CPOL/CPHA、数据宽度、片选引脚、CS 建立/保持时间 | "SBC 这颗芯片的电气约定" |
| **HwUnit** | 物理 SPI 外设实例（LPSPI_0/1/2） | 芯片上的哪个 SPI 口 |

要点：
- **一个 Job 只属于一个 ExternalDevice**——片选和波特率是 Job 级的
- 同一总线挂多个从设备 = 多个 ExternalDevice + 各自 Job，可编进同一个 Sequence 依次传输
- Sequence 是**调度与仲裁单位**：多个调用方请求不同 Sequence 时由 Spi 驱动排队仲裁

---

## 3. 三种缓冲区类型（IB / EB / SB）

Channel 的数据缓冲区有三种形态，由 `SpiChannelBuffersAllowed` 限定：

| 类型 | 全称 | 数据流向 | 使用 API | 特点 |
|---|---|---|---|---|
| **IB** | Internal Buffer | 驱动内部缓冲 | `Spi_WriteIB()` 写入、`Spi_ReadIB()` 读出 | 简单但多一次拷贝，长度受限 |
| **EB** | External Buffer | 用户提供缓冲，**直接收发** | `Spi_SetupEB()` 绑定收发指针 | 零拷贝、长度灵活，**本项目使用** |
| **SB** | IB+EB 混合 | — | — | 较少用 |

```c
/* EB 使用模式（本项目 CddSbc 的实际用法） */
Spi_DataBufferType txCmd[4], rxData[4];
txCmd[0] = 0x7E << 1 | 1;   /* SBC 读命令：地址 + 读位 */
txCmd[1] = 0xFF;            /* Dummy 字节，时钟驱动数据移入 */
Spi_SetupEB(SpiConf_SpiChannel_SpiChannel_SBC_Read, txCmd, rxData, 2);
```

## 4. 三种功能级别（LEVEL 0 / 1 / 2）

由 `SpiLevelDelivered` 选择，决定可用的传输 API：

| Level | 名称 | 可用 API | 说明 |
|---|---|---|---|
| **0** | Simple Synchronous | 仅 `Spi_SyncTransmit()` | 阻塞式传输，无后台处理，代码最小。**本项目使用** |
| **1** | Basic Asynchronous | 仅 `Spi_AsyncTransmit()` + `Spi_MainFunction_Handling()` | 非阻塞，传输由 MainFunction/中断推进 |
| **2** | Enhanced | 两者皆可 | 同时支持同步+异步 |

> 注意：LEVEL 0 下没有异步概念，不需要周期调度 `Spi_MainFunction_Handling()`；LEVEL 1/2 必须挂 OS 周期任务调用它，否则传输永远不完成。

---

## 5. SPI 时序基础（配置前必须懂）

### 四种模式 = CPOL（空闲电平）× CPHA（采样边沿）

| Mode | CPOL | CPHA | 空闲电平 | 采样时刻 | 本项目 |
|---|---|---|---|---|---|
| 0 | 0 | 0 | 低 | 第 1 个边沿（上升沿） | ✔ SBC |
| 1 | 0 | 1 | 低 | 第 2 个边沿（下降沿） | |
| 2 | 1 | 0 | 高 | 第 1 个边沿（下降沿） | |
| 3 | 1 | 1 | 高 | 第 2 个边沿（上升沿） | |

**模式必须和从设备 datasheet 一致**，否则数据错位。本项目 SBC 用 Mode 0（`LPSPI_TCR_CPOL_LOW | CPHA_LEADING`）。

### 其它时序参数

- **波特率**：主从双方上限取低者；本项目 100 kHz
- **位序**：MSB/LSB first（本项目 MSB）
- **帧宽**：8/16/32 bit（本项目 8 bit，TCR 配置 `WIDTH_1 + 7`）
- **CS 建立/保持时间**：`TimeClk2Cs`、`Cs2Clk` 等，从设备苛刻时必须配

---

## 6. 状态机

### Sequence 状态（`Spi_GetSequenceResult()`）

```
SEQ_OK ──新请求──> SEQ_PENDING ──传完──> SEQ_OK
                      │
                      └──失败──> SEQ_FAILED
```

### Job 状态（`Spi_GetJobResult()`）

`JOB_OK` / `JOB_PENDING` / `JOB_FAILED` / `JOB_QUEUED`

### 驱动整体状态（`Spi_GetStatus()`）

`SPI_UNINIT` →（`Spi_Init()`）→ `SPI_IDLE` ⇄（传输中）`SPI_BUSY`

---

## 7. 配置要点（DaVinci / EB tresos）

按容器自上而下：

| 容器 | 关键参数 | 要点 |
|---|---|---|
| **SpiGeneral** | LevelDelivered | 只同步选 0，最省资源 |
| | ChannelBuffersAllowed | 用 EB 选 USAGE1 |
| | DevErrorDetect | 开发期 ON，量产可 OFF 省代码 |
| **SpiExternalDevice** | Baudrate / CPOL / CPHA / DataWidth | **与从设备 datasheet 逐项核对** |
| | CsIdentifier / CsPolarity | 片选引脚、有效电平 |
| **SpiChannel** | ChannelType | IB 或 EB |
| | DataWidth / DefaultTxValue | EB 时传 `0xFF` 这类 Dummy 值读数据 |
| **SpiJob** | DeviceAssignment | 引用 ExternalDevice（决定片选和时序） |
| | ChannelList | 按序引用 Channel |
| **SpiSequence** | JobList | 按序引用 Job |
| | InterruptibleSequence | Level 1/2 下是否可被高优先级 Sequence 打断 |
| **SpiHwUnit** | 外设实例映射 | LPSPI_1 + 引脚（PORT 模块配 SCK/MOSI/MISO/PCS3） |

**别忘配套模块**：Port（引脚复用）、Mcu（时钟使能 LPSPI）、中断/DMA（Level 1/2 或 DMA 模式）。

---

## 8. 使用要点（代码）

### 初始化

```c
Spi_Init(&Spi_ConfigType);   /* EcuM 启动阶段调用，之后驱动进入 IDLE */
```

### 同步传输（Level 0，本项目模式）

```c
/* 典型三步：绑缓冲 → 发起 → 查结果 */
Spi_SetupEB(SpiConf_SpiChannel_SpiChannel_SBC_Read, txBuf, rxBuf, 2);

if (Spi_SyncTransmit(SpiConf_SpiSequence_SpiSequence_0_SBC) == E_OK)
{
    /* 传输完成，rxBuf 里已是 SBC 返回的数据 */
}
```

`Spi_SyncTransmit()` 是**阻塞**的：内部循环等 FIFO/标志位（带 `SPI_TIMEOUT_COUNTER` 超时保护），返回时传输已结束。不要在时间敏感的短周期任务里传太长的数据。

### 异步传输（Level 1/2）

```c
Spi_AsyncTransmit(SpiConf_SpiSequence_SpiSequence_0_SBC);  /* 立即返回 */
/* OS 周期任务中： */
Spi_MainFunction_Handling();   /* 推进状态机 */
/* 轮询结果 */
if (Spi_GetSequenceResult(seq) == SPI_SEQ_OK) { /* 完成 */ }
/* 或注册 Sequence 结束通知回调 */
```

### 常用查询 API

| API | 作用 |
|---|---|
| `Spi_GetStatus()` | 驱动整体 UNINIT/IDLE/BUSY |
| `Spi_GetJobResult(job)` | 某 Job 的传输结果 |
| `Spi_GetSequenceResult(seq)` | 某 Sequence 的传输结果 |
| `Spi_GetHWUnitStatus(hwUnit)` | 硬件单元忙闲（需开 `SpiHwStatusApi`） |

---

## 9. 常见坑（按踩坑频率排序）

| # | 坑 | 现象 | 对策 |
|---|---|---|---|
| 1 | CPOL/CPHA 与从设备不符 | 数据整体错位、读出全 0xFF | 照从设备 datasheet 模式图逐项对 |
| 2 | Dummy 字节没发 | 只发命令不收数据 | 全双工：读几个字节就要补几个 Dummy（如 0xFF） |
| 3 | EB 缓冲是局部变量且用异步 | 函数返回后缓冲失效，数据错乱 | EB 缓冲用 static/全局（本项目即如此） |
| 4 | Level 1/2 忘了调 `Spi_MainFunction_Handling` | AsyncTransmit 永远不完成 | 挂 OS 周期任务 |
| 5 | 同步传输塞在 1ms 快任务里传长帧 | 任务超时、影响调度 | 长数据走异步，或放慢周期任务 |
| 6 | 多从设备共用总线但片选配错 | 读到的是另一颗芯片的响应 | 每颗从设备独立 ExternalDevice + 片选引脚 |
| 7 | 波特率超过从设备上限 | 高速时数据偶发错误 | 主从取低，留余量；长线传输再降速 |
| 8 | Port 模块引脚复用没配 | 总线上完全没有波形 | 检查 SCK/MOSI/MISO/PCS 的 PCR 复用值 |

---

## 10. 面试要点（高频 Q&A）

### Q1：Channel、Job、Sequence 是什么关系？

**答**：三层调度树。Channel 是数据缓冲描述（最小数据单位）；Job 是一组有序 Channel，绑定一个 ExternalDevice，对应一次片选有效的完整传输；Sequence 是一组有序 Job，是传输调度的最小单位，`SyncTransmit/AsyncTransmit` 的参数就是 Sequence。一句话：**Channel 组 Job，Job 组 Sequence，Sequence 发起传输**。

### Q2：IB 和 EB 缓冲区有什么区别？

**答**：IB 是驱动内部缓冲，先 `Spi_WriteIB` 拷入、传完 `Spi_ReadIB` 读出，多一次拷贝但缓冲由驱动管理；EB 是用户提供的缓冲，`Spi_SetupEB` 绑定指针后**直接 DMA/寄存器收发**，零拷贝、长度灵活，但要求缓冲在传输期间保持有效（不能是栈上临时变量）。

### Q3：Level 0/1/2 的区别？

**答**：Level 0 只支持同步传输 `Spi_SyncTransmit`，阻塞、无后台任务；Level 1 只支持异步 `Spi_AsyncTransmit` + 必须周期调用 `Spi_MainFunction_Handling`；Level 2 两者都支持。资源占用递增，选型按"是否需要非阻塞"。

### Q4：SPI 四种模式的含义？怎么选？

**答**：CPOL 决定空闲时 SCK 电平（0=低/1=高），CPHA 决定采样边沿（0=第一个边沿/1=第二个）。组合出 Mode 0~3。选型不看喜好——**从设备 datasheet 规定什么就用什么**，主从必须一致。

### Q5：同步传输和异步传输的实现差异？

**答**：同步是调用方线程内轮询硬件标志位直到传完（带超时计数），占用当前任务时间；异步是把请求入队后立即返回，由中断或 `Spi_MainFunction_Handling` 逐步推进，完成后通过通知回调或 `GetSequenceResult` 告知。同步简单实时性好，异步不阻塞但要调度配合。

### Q6：一条 SPI 总线挂多个从设备怎么配？

**答**：每个从设备配一个 **ExternalDevice**（各自的波特率/模式/片选引脚），每个 Job 引用对应 ExternalDevice，多个 Job 可编进同一 Sequence 顺序执行。片选切换由驱动按 Job 自动完成。注意不同从设备时序参数可以不同，这正是 ExternalDevice 存在的意义。

### Q7：读一个 SPI 从设备寄存器的完整流程？

**答**：① 组帧：命令字节 + Dummy 字节（全双工，发几个时钟才能收几个字节）；② `Spi_SetupEB` 绑定收发缓冲；③ `Spi_SyncTransmit(sequence)`；④ 从接收缓冲解析。SPI 没有"纯读"操作，读 = 发 Dummy。

### Q8：为什么 SBC/EEPROM 这类外设走 CDD 直调 Spi，而不是走 Com 栈？

**答**：Com/PduR 那套是为 CAN/LIN/Ethernet 总线报文设计的；SPI 是板级芯片间总线，没有 PDU/信号概念。AUTOSAR 的做法是 MCAL Spi 直接暴露给 CDD 或 IoHwAb 使用，本项目 CddSbc 就是典型例子。

### Q9：Spi 传输失败怎么定位？

**答**：三步：① `Spi_GetSequenceResult` / `Spi_GetJobResult` 看哪级失败；② 开 `SpiDevErrorDetect` 看 DET 错误码（参数错/未初始化/并发冲突）；③ 硬件层面抓波形——没波形查 Port/Mcu 配置，有波形但数据错查 CPOL/CPHA/位序/波特率。

### Q10：片选 CS 是软件控制还是硬件控制？

**答**：MCAL 配置时可选：硬件片选（LPSPI 的 PCS 引脚，驱动自动控制时序）或 GPIO 软件片选（驱动翻转 Dio）。硬件片选时序精确，软件片选灵活（可跨器件复用）。本项目用硬件片选 PCS3。

### Q11：Spi 驱动需要做 DMA 吗？

**答**：可选。短帧/低频通信（如读 SBC 状态）用 FIFO 轮询足够；长帧高速（如刷显示屏、读 Flash 大数据）配 DMA 能显著降 CPU 占用。配置开关 `SpiDmaUsed`，并联动 MCU 的 DMAMUX/EDMA 配置。

### Q12：Sequence 的可打断属性（Interruptible）有什么用？

**答**：Level 1/2 下，高优先级 Sequence 可打断正在传输的低优先级 Sequence（在 Job 边界处），传完再恢复。用于保证关键通信（如安全相关传感器）不被大流量低优先级传输（如刷屏幕）阻塞。Level 0 同步传输天然不可打断。

---

## 11. 一句话总结

> **Channel 组 Job、Job 组 Sequence，ExternalDevice 定时序，Sequence 发起传输。**
> 配置核心是和从设备 datasheet 对齐三件事：**模式（CPOL/CPHA）、波特率、帧宽**；使用核心是按数据量选 Level（同步阻塞 vs 异步调度）。
