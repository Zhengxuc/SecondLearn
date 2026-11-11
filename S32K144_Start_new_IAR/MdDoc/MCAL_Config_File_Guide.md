# AUTOSAR MCAL 配置文件速查手册（S32K144 + Vector MICROSAR）

> **目标读者**：BSW 底软开发者（1-3 年经验），理解 "DaVinci 配置 → GenData 生成文件 → 驱动写寄存器" 这条链路
> **工程**：S32K144_Start_new_IAR（Harman 项目，Vector MICROSAR + DaVinci Configurator 5.18）
> **核心方法**：每调一个 DaVinci 配置项 → 去 GenData 里找对应的宏/配置表 → 确认驱动确实用它

---

## 0. 工程已配置的模块清单

| 层级 | 模块 | GenData 里有生成文件？ | 配置模式 |
|---|---|---|---|
| **MCAL** | Mcu | ✅ Mcu_Cfg.h + Mcu_PBcfg.c | POST_BUILD |
| **MCAL** | Gpt | ✅ Gpt_Cfg.h + Gpt_PBcfg.c | POST_BUILD |
| **MCAL** | Adc | ✅ Adc_Cfg.h + Adc_PBcfg.c + Adc_CfgDefines.h | POST_BUILD |
| **MCAL** | Pwm | ✅ Pwm_Cfg.h + Pwm_PBcfg.c | POST_BUILD |
| **MCAL** | Port | ✅ Port_Cfg.h + Port_PBcfg.c | POST_BUILD |
| **MCAL** | Dio | ✅ Dio_Cfg.h（无 PBcfg——DIO 不需要初始化配置表） | PRECOMPILE |
| **MCAL** | Spi | ✅ Spi_Cfg.h + Spi_PBcfg.c | POST_BUILD |
| **MCAL** | Can | ✅ Can_Cfg.h + **Can_Lcfg.c**（Can_PBcfg.c 是空的！） | PRECOMPILE + LINK |
| **MCAL** | Wdg（看门狗） | ❌ **DaVinci 里没配**（MCAL 驱动源码自带模板，见下文第 9 节） | — |
| **BSW** | CanIf | ✅ CanIf_Cfg.h + CanIf_PBcfg.c | POST_BUILD |
| **BSW** | CanTp | ✅ CanTp_Cfg.h + CanTp_PBcfg.c | POST_BUILD |
| **BSW** | CanSM | ✅ CanSM_Cfg.h | PRECOMPILE |
| **BSW** | Com | ✅ Com_Cfg.h + Com_PBcfg.c | POST_BUILD |
| **BSW** | ComM | ✅ ComM_Cfg.h + ComM_PBcfg.c | POST_BUILD |
| **BSW** | PduR | ✅ PduR_Cfg.h + PduR_PBcfg.c | POST_BUILD |
| **BSW** | EcuM | ✅ EcuM_Cfg.h + EcuM_PBcfg.c（初始化顺序不在 PBcfg，在 EcuM_Lcfg.c / EcuM_Cfg.c） | PRECOMPILE |
| **BSW** | Os | ✅ Os_Cfg.h + 大量 Os_*.h | PRECOMPILE |
| **BSW** | Dem | ✅ Dem_Cfg.h + Dem_PBcfg.c | POST_BUILD |
| **BSW** | Dcm | ✅ Dcm_Cfg.h + Dcm_PBcfg.c | POST_BUILD |
| **BSW** | Nm | ✅ Nm_Cfg.h | PRECOMPILE |
| **BSW** | BswM | ✅ BswM_Cfg.h + BswM_PBcfg.c | POST_BUILD |

---

## 1. Mcu（微控制器单元——时钟/复位/模式）

### 生成文件

| 文件 | 路径 | 核心内容 |
|---|---|---|
| Mcu_Cfg.h | `Appl\GenData\include\Mcu_Cfg.h` | 时钟/模式命名宏 + API 开关 + 配置结构体声明 |
| Mcu_PBcfg.c | `Appl\GenData\src\Mcu_PBcfg.c` | **最核心**——所有寄存器预打包值 + 顶层配置入口 |

### Mcu_Cfg.h 重点（DaVinci 命名宏）

```c
// L540-L542: DaVinci 配的 Clock Setting 宏（替换硬编码）
#define McuClockSettingConfig_0   ((Mcu_ClockType)0U)
#define McuConf_McuClockSettingConfig_McuClockSettingConfig_0   ((Mcu_ClockType)0U)

// L551-L553: DaVinci 配的 Mode Setting 宏
#define McuModeSettingConf_0   ((Mcu_ModeType)0U)
#define McuConf_McuModeSettingConf_McuModeSettingConf_0   ((Mcu_ModeType)0U)

// L197: 时钟初始化开关（关掉 = Mcu_InitClock() 是空函数）
#define MCU_INIT_CLOCK   (STD_ON)

// L426-L432: 复位原因位定义（Mcu_GetResetRawValue() 返回值的 bit0 是看门狗复位！）
#define MCU_RAW_RESET_WDOG         ((uint32)0x00000020U)   // ← 看门狗复位！
#define MCU_RAW_RESET_POR          ((uint32)0x00000080U)   // ← 上电复位
#define MCU_RAW_RESET_SW           ((uint32)0x00000400U)   // ← 软件复位
```

### Mcu_PBcfg.c 重点（6 张配置表）

**配置表 1：SOSC/SIRC/FIRC/SPLL 时钟寄存器（L360-L494）**
```c
// L467: SPLL 配置——DaVinci 配的 PLL 倍频 = 24
SCG_SPLLCFG_PREDIV_U32((uint32)0U) | SCG_SPLLCFG_MULT_U32((uint32)24U)
// L481: PLL 分频 = /3
SCG_SPLLDIV_SPLLDIV2_U32((uint32)3U)
```

**配置表 2：SCG_ClockConfigPB0_0（L508-L549）——Run 模式时钟源选 PLL**
```c
// Run 模式核心时钟 = PLL / 1
SCG_SCS_SPLL_U32 | SCG_DIVCORE_U32((uint32)1U) | SCG_DIVBUS_U32((uint32)1U) | SCG_DIVSLOW_U32((uint32)3U)
// VLPR 模式核心时钟 = SIRC / 1
SCG_SCS_SIRC_U32 | SCG_DIVCORE_U32((uint32)1U) | SCG_DIVBUS_U32((uint32)0U)
```

**配置表 3：PCC_aRegistersConfigPB0_0（L589-L915）——外设时钟开关 + 时钟源**
```c
// L684: ADC0 时钟开 + 时钟源选 SOSC
PCC_CLOCK_ENABLED_U32 | PCC_PCS_SOSC_U32
// L644: LPIT（GPT 用）时钟开 + 时钟源选 FIRC
PCC_CLOCK_ENABLED_U32 | PCC_PCS_FIRC_U32
// L717: FLEXCAN0 时钟开
PCC_CLOCK_ENABLED_U32 | PCC_PCS_UNAVAILABLE_U32
// L887: LPSPI0 时钟开 + 时钟源选 SOSC
PCC_CLOCK_ENABLED_U32 | PCC_PCS_SOSC_U32
```

**配置表 4：RCM/PMC/SIM/SMC IP 配置（L983-L1178）——复位/电源管理**
```c
// L1000: RCM 复位中断——看门狗复位中断开/关
RCM_SRIE_WDOG_INT_DIS_U32   // ← 看门狗复位中断关掉了
```

**配置表 5：Mcu_ClockConfigPB0（L941-L972）——顶层时钟配置入口**

**配置表 6：McuModuleConfiguration（L1189-L1222）——** `Mcu_Init(&McuModuleConfiguration)` 读的就是这个

### 高频坑点

| 坑 | PBcfg 位置 | 表现 |
|---|---|---|
| PLL 倍频/分频配错 | L473-L483 | 所有外设时钟跑飞、Can 波特率不对、ADC 采样不准 |
| 外设时钟没开 | PCC 表里某个模块没 `PCC_CLOCK_ENABLED_U32` | 读该外设寄存器全 0，外设写不进去——**Mcu_Init() 是所有外设初始化的前置条件** |
| RCM_SRIE_WDOG_INT 关了 | L1000 | 看门狗复位了但进不了 ISR——调试 HardFault 时看门狗是关键排查项 |

---

## 2. Gpt（通用定时器——之前你详细学过的）

### 生成文件

| 文件 | 路径 | 核心内容 |
|---|---|---|
| Gpt_Cfg.h | `Appl\GenData\include\Gpt_Cfg.h` | Channel 命名宏（`GptConf_GptChannelConfiguration_...`）+ API 开关 |
| Gpt_PBcfg.c | `Appl\GenData\src\Gpt_PBcfg.c` | **Gpt_InitChannelPB[1]** 单张表——Channel 预打包寄存器值 + 回调函数指针 |

### 关键配置项（回顾）

- Channel 0 硬件定时器：LPIT0_CH0（Gpt_Cfg.h L222）
- Tick 值计算：DaVinci 配的 Period → Gpt_PBcfg.c 里的 `GptInitChannelValue` 宏值
- Notification 回调：`&Gpt_10ms`（PBcfg 里直接写函数指针）
- **配置生效链**：`Mcu_Init()`（开 LPIT 时钟）→ `Gpt_Init()`（写 LPIT_MCR/TIMER0_TVAL）→ `Gpt_StartTimer()`（使能）

---

## 3. Adc（模数转换器——之前你详细学过的）

### 生成文件

| 文件 | 路径 | 核心内容 |
|---|---|---|
| Adc_CfgDefines.h | `Appl\GenData\include\Adc_CfgDefines.h` | **Channel/Group 命名宏 + API 开关**——Adc 独有的文件 |
| Adc_Cfg.h | `Appl\GenData\include\Adc_Cfg.h` | Adc_ConfigType 结构体声明 + Group/Channel 数量 |
| Adc_PBcfg.c | `Appl\GenData\src\Adc_PBcfg.c` | **3 张配置表**（HwUnit/Channels/Groups）+ 配置入口 |

### Adc_CfgDefines.h 重点

```c
// L405: Channel 命名宏
#define Adc0Channel2   0

// L412/L419: Group 命名宏（两个等价，后者是 Ecuc 2108 兼容）
#define AdcGroup0   0
#define AdcConf_AdcGroup_AdcGroup0   0

// L157: Notification 开关（关掉 = PBcfg 里的回调没用）
#define ADC_GRP_NOTIF_CAPABILITY   (STD_ON)
```

### Adc_PBcfg.c 三张配置表

**表 1：Adc_HwUnitCfg[]（L240-L300）——硬件寄存器预打包**
```c
ADC12BSARV2_CFG1_CLOCK_DIV1_U32 |
ADC12BSARV2_CFG1_12_BITS_RESOLUTION_U32 |    // ← DaVinci 配的 Resolution
ADC12BSARV2_CFG1_ALTCLK1_U32                  // ← DaVinci 配的 ClockSource
```

**表 2：Adc_ChannelsCfg_0[]（L307-L314）——Channel 绑 GPIO**
```c
(Adc_ChannelType)(ADC_ID_CHANNEL_U8(2))       // ← AN_2 通道
```

**表 3：Adc_GroupsCfg[]（L384-L468）——Group 完整配置**
```c
ADC_CONV_MODE_ONESHOT |                       // ← 转换模式：单次/连续（★ 之前踩的坑）
ADC_TRIGG_SRC_SW |                            // ← 触发源：软件/PDB
&AdcGroup0Notification |                      // ← ★ Notification 回调（之前踩的坑：回调里不能调 Rte_Write）
Adc_aResultsBufferPtr (NULL_PTR!)             // ← ★ Result Buffer 没配（要自己调 Adc_SetupResultBuffer）
```

### 高频坑点（你之前踩过的）

| 坑 | PBcfg 位置 | 表现 |
|---|---|---|
| Continuous + 回调过重 | `ADC_CONV_MODE_CONTINUOUS` + Notification | 回调里调 Rte_Write → 占死 CPU |
| Streaming 模式用 LINEAR 没用 CIRCULAR | `StreamingMode=ADC_STREAM_BUFFER_LINEAR` | 连续采样会越界 |
| Result Buffer = NULL_PTR | L420 | 没调 `Adc_SetupResultBuffer()` → 读值全 0 |
| ClockSource 配错 | `Adc_HwUnitCfg[].CFG1` | 采样不准——ALTCLK1 / SOSC / SIRC 频率差很大 |

---

## 4. Pwm（脉冲宽度调制——底层是 FTM）

### 生成文件

| 文件 | 路径 | 核心内容 |
|---|---|---|
| Pwm_Cfg.h | `Appl\GenData\include\Pwm_Cfg.h` | Channel 命名宏 + API 开关 |
| Pwm_PBcfg.c | `Appl\GenData\src\Pwm_PBcfg.c` | 5 张配置表（Channel/IpChannel/FtmModule/FtmIp + 顶层入口） |

### Pwm_Cfg.h 重点

```c
// L400-L406: Channel 命名宏
#define PwmChannel0   ((Pwm_ChannelType)0U)
#define PwmConf_PwmChannelConfigSet_PwmChannel0   ((Pwm_ChannelType)0U)

// L231: Notification 开关
#define PWM_NOTIFICATION_SUPPORTED   (STD_ON)
```

### Pwm_PBcfg.c 重点

**Channel 配置（L215-L239）**：
```c
PWM_LOW,                    // ← 极性
PWM_HIGH,                   // ← Idle 状态
((uint16)16384),           // ← 默认占空比（50% of 0xFFFF）
FTM_0_CH_0,                // ← 硬件通道
PWM_EDGE_ALIGNED           // ← 对齐模式
```

**Ftm Module 配置（L244-L287）**——**Period 值直接决定 PWM 频率！**
```c
FTM_0_MOD register: ((Pwm_PeriodType) 0)   // ← Period 是 0！= 1 tick？→ 频率极高
// L272: ★ DaVinci 配的 Period 值直接写这里——单位是 FTM 时钟分频后的 tick 数
```

### 高频坑点

| 坑 | PBcfg 位置 | 表现 |
|---|---|---|
| Period 值 0 | L272 | PWM 输出是 1 tick 的脉冲→频率极高，示波器看不到 |
| FTM 时钟没开 | Mcu_PBcfg.c PCC 表里的 FTM0/FTM1 | PWM 输出常亮/常灭 |
| Polarity/Idle 方向搞反 | L220-L221 | LED 常亮或不亮（LED 一般是 LOW 亮） |
| GPIO 没配成 FTM ALT 模式 | Port_PBcfg.c | PWM 输出从 GPIO 出来但不是 PWM——Port_Cfg.h L174 `PORT0_FTM2_CH1` |

---

## 5. Port（端口——所有外设 GPIO 的源头）

### 生成文件

| 文件 | 路径 | 核心内容 |
|---|---|---|
| Port_Cfg.h | `Appl\GenData\include\Port_Cfg.h` | **Pin 命名宏 + 所有 Pin Mode 选项（L172-L586）**——Port 独有的大段宏 |
| Port_PBcfg.c | `Appl\GenData\src\Port_PBcfg.c` | 1 张配置表 `Port_aPinConfigDefault[16]` + 顶层入口 |

### Port_Cfg.h 重点

```c
// L650-L680: DaVinci 配的 Pin 命名宏（替换硬编码 Port+Pin 号）
#define PortConfigSet_PortContainer_CAN_PortPin_PTE4_CAN0_RX   6
#define PortConfigSet_PortContainer_CAN_PortPin_PTE5_CAN0_TX   5
#define PortConfigSet_PortContainer_SBC_SPI_PortPin_PTB14_SCK   7
#define PortConfigSet_PortContainer_LED_PortPin_PTD15          11

// L172-L586: **Pin Mode 选项对照表**（每个 Pin 的 ALT0~ALT7 可选功能）
#define PORT32_CAN0_RX   (PORT_ALT5_FUNC_MODE)    // ← PTE5 配成 CAN0_RX 选 ALT5
#define PORT33_CAN0_TX   (PORT_ALT5_FUNC_MODE)    // ← PTE4 配成 CAN0_TX 选 ALT5
#define PORT34_LPSPI0_SCK  (PORT_ALT3_FUNC_MODE)  // ← PTB14 配成 LPSPI0_SCK 选 ALT3

// L701: 配置了多少个 Pin
#define PORT_MAX_CONFIGURED_PADS_U16   ((uint16)16)
```

### Port_PBcfg.c 重点

**L253-L272：`Port_aPinConfigDefault[]`——每个 Pin 的 PCR 寄存器打包值**
```c
// 每行 7 个字段：InternalPinId | PCR值 | ... | Direction | GPIO方向 | ... |
{(Port_InternalPinIdType)112, 0x00000100, ..., Direction=2(GPIO), ...},  // PTB0 配成 GPIO
{(Port_InternalPinIdType)46,   0x00000300, ..., Direction=2(GPIO), ...},  // PTB14 配成 LPSPI0_SCK (ALT3)
// ↑ 第二个字段 0x00000300 = PCR_ALT3（对比 0x00000100 = ALT1/GPIO）
```

### 高频坑点

| 坑 | PBcfg 位置 | 表现 |
|---|---|---|
| Pin Mode 配错 | `Port_aPinConfigDefault[].PCR值` | SPI/CAN/UART 不通——示波器看引脚没波形 |
| 方向配反 | `Direction` 字段 | 输入 Pin 写成输出或反过来——读不到 CAN RX 数据 |
| 没用的 Pin 悬空 | `Port_au16NoUnUsedPadsArrayDefault[]`（L154-L229） | EMI 大、误触发中断 |
| GPIO 数字滤波没开 | Port_Cfg.h L592 `PORT_DEV_ERROR_DETECT` + Port_PBcfg.c L300 | 抗干扰差（噪声环境） |

---

## 6. Dio（数字 I/O——Port 的"读/写"接口层）

### 生成文件

| 文件 | 路径 | 核心内容 |
|---|---|---|
| Dio_Cfg.h | `Appl\GenData\include\Dio_Cfg.h` | Channel/Port 命名宏 + API 开关——**无 PBcfg！DIO 不初始化寄存器** |

### Dio_Cfg.h 重点

```c
// L270: Port 命名宏（Dio_ReadPort / Dio_WritePort 用）
#define DioConf_DioPort_DioPort_0   ((uint8)0x03U)   // Port D

// L279: Channel 命名宏（Dio_ReadChannel / Dio_WriteChannel 用）
#define DioConf_DioChannel_DioChannel_PTD16   ((uint16)0x0070U)  // Port D + Pin 4 = 0x070

// L124-L126: API 开关
#define DIO_DEV_ERROR_DETECT    (STD_OFF)
#define DIO_FLIP_CHANNEL_API    (STD_ON)
```

### 关键理解

> **DIO 不写寄存器——Port_Init() 已经把所有 Pin 配好了。DIO 只是 Port 的"逻辑层 API"，Channel/Port 宏值直接对应 Port 号和 Pin 号**

### 高频坑点

| 坑 | 表现 |
|---|---|
| Port 没初始化就调 Dio_WriteChannel() | 写不进去，因为 PCR 寄存器默认是 ALT0（GPIO）但方向是输入 |
| Channel 值是 Port<<8 + Pin，不是纯 Pin 号 | `DioConf_DioChannel_DioChannel_PTD16 = 0x0070` → Port 7, Pin 0 |

---

## 7. Spi（串行外设接口——底层是 LPSPI）

### 生成文件

| 文件 | 路径 | 核心内容 |
|---|---|---|
| Spi_Cfg.h | `Appl\GenData\include\Spi_Cfg.h` | Channel/Job/Sequence 命名宏 + API 开关 |
| Spi_PBcfg.c | `Appl\GenData\src\Spi_PBcfg.c` | Controller/Job/Channel/Sequence 配置表 + 顶层入口 |

### Spi_PBcfg.c 重点（**4 层配置表**——你工程里 2 个 Sequence 各绑 1 个 Job）

**表 1：SpiChannelConfig_PB0[2]（L303-L326）——2 个 EB Channel**
```c
// Channel 0: SBC_Read（External Buffer，4 字节，默认 Buffer=0x55）
EB, (Spi_DataBufferType)0x55u, 4U, &Buffer_PBSpiChannel_SBC_Read, &Spi_aSpiChannelState[0]
// Channel 1: Test_Read（External Buffer，8 字节）
EB, (Spi_DataBufferType)0x55u, 8U, &Buffer_PBSpiChannel_Test_Read, &Spi_aSpiChannelState[1]
```

**表 2：SpiJobConfig_PB0[2]（L338-L433）——2 个 Job，各绑 1 个 Channel + LPSPI TCR 寄存器打包值**
```c
// Job 0: LPSPI0 + Channel SBC_Read + CS=PCS3 + 100kHz + CPOL=LOW + CPHA=LEADING
LPSPI_TCR_CPOL_LOW_U32 | LPSPI_TCR_CPHA_LEADING_U32 | LPSPI_TCR_PRESCALE_1_U32 |
LPSPI_TCR_PCS3_EN_U32 | LPSPI_TCR_CONT_EN_U32      // ← CS3 片选 + 连续模式
CSIB0, LPSPI_0_OFFSET

// Job 1: LPSPI1 + Channel Test_Read + CS=PCS1 + 100kHz + CPOL=LOW + CPHA=LEADING
LPSPI_TCR_CPOL_LOW_U32 | LPSPI_TCR_CPHA_LEADING_U32 | LPSPI_TCR_PRESCALE_1_U32 |
LPSPI_TCR_PCS1_EN_U32 | LPSPI_TCR_CONT_EN_U32      // ← CS1 片选
CSIB1, LPSPI_1_OFFSET

// ↓ TimeClk2Cs/TimeCs2Clk/TimeCs2Cs 这三个 CS 间隔时间（L368-L371）
// 注意：工程里显示 Error=-35.75%——说明 DaVinci 计算的时间和实际寄存器值有偏差
```

**表 3：SpiSequenceConfig_PB0[2]（L444+）——2 个 Sequence，各绑 1 个 Job**
```c
Sequence 0 (SBC) → Job 0
Sequence 1 (TEST) → Job 1
```

### 高频坑点

| 坑 | PBcfg 位置 | 表现 |
|---|---|---|
| CS 信号没开 | Job 里 `LPSPI_TCR_PCSx_EN_U32` 没设置 | 示波器看不到 CS 下降沿，SPI 不触发 |
| CS 间隔时间配太短 | Job 里 `TimeClk2Cs/TimeCs2Clk=255` | CS 和 CLK 间隔太小，从设备读不到 |
| Eb/Ib 配错 | `ChannelType=EB`（External Buffer）但没调 `Spi_SetupBuffer()` | 发送 Buffer 是默认 0x55（垃圾数据） |
| 波特率分频配错 | Job 里 `LPSPI_TCR_PRESCALE_x_U32` | SCK 频率和从设备不匹配 |
| PCC_LPSPI 时钟没开 | Mcu_PBcfg.c L887-L902 | SPI 寄存器写不进去 |
| Port 没配 SPI 引脚 | Port_PBcfg.c L267-L269 | SPI 物理引脚 ALT 模式不对 |

---

## 8. Can（CAN 总线——PRECOMPILE + LINK 模式）

### 生成文件

| 文件 | 路径 | 核心内容 |
|---|---|---|
| Can_Cfg.h | `Appl\GenData\Can_Cfg.h` | 配置模式开关 + 编译时宏 |
| **Can_Lcfg.c** | `Appl\GenData\Can_Lcfg.c` | **6 张配置表——真实的 Controller/Mailbox/Baudrate**（不是 Can_PBcfg.c！） |
| Can_PBcfg.c | `Appl\GenData\Can_PBcfg.c` | **空的！** 因为 `CAN_CONFIGURATION_VARIANT_PRECOMPILE`（见 Can_Cfg.h L105） |

### Can_Lcfg.c 6 张配置表

**表 1：Can_ControllerConfig[1]（L148-L151）——Controller 0 配置**
```c
BaseAddress = 0x40024000uL,          // ← FLEXCAN0 基地址
CanControllerDefaultBaudrate = 500u, // ← 500kbps（DaVinci 配的）
NumberOfMaxMailboxes = 0x20u,        // ← 32 个 Mailbox（硬件上限）
Mailbox 索引表: RxStartIdx=1, TxStartIdx=0, UnusedStartIdx=2
```

**表 2：Can_InitObject[1]（L223-L226）——波特率寄存器打包值**
```c
CBT = 0x80E018A5uL,      // ← DaVinci 波特率配置 → S32K144 FLEXCAN CBT 寄存器
Control1 = 0x072D2006uL  // ← FLEXCAN CTRL1 寄存器
```

**表 3：Can_Mailbox[3]（L292-L297）——3 个 Mailbox**
```c
Mailbox 0: ControllerIdx=0, HwHandle=8,  Type=CAN_TX_BASICCAN_TYPE   // ← TX Mailbox
Mailbox 1: ControllerIdx=0, HwHandle=0,  Type=CAN_RX_BASICCAN_TYPE   // ← RX Mailbox
Mailbox 2: ControllerIdx=0, HwHandle=9,  Type=CAN_UNUSED_CAN_TYPE    // ← Unused
```

**表 4-6：Can_InitBasicCan / Can_InitObjectBaudrate / Can_ControllerData**——验收滤波器 + 波特率数组 + 运行时变量

### 高频坑点

| 坑 | 位置 | 表现 |
|---|---|---|
| 波特率配错 | `Can_InitObject[].CBT` | 发送接收全错帧/无帧 |
| Mailbox 分配不对 | `Can_Mailbox[]` 索引越界 | Can_Send 硬 Fault |
| 验收滤波器配置不对 | `Can_InitBasicCan[]` | RX 全被过滤掉或 RX 中断风暴 |
| PCC_FLEXCAN0 时钟没开 | Mcu_PBcfg.c L717 | CAN 总线收发器不响应 |
| Port 没配 CAN RX/TX | Port_PBcfg.c | Can 总线永远 BusOff |

---

## 9. Wdg（看门狗——**你工程 DaVinci 里没配置**）

> **⚠️ 重要**：以下内容基于 MCAL 驱动自带的**模板文件推断**（`MCAL\Wdg_TS_T40D2M10I1R0\generate_PB\src\Wdg_PBcfg.c`），**不是 DaVinci 实际生成的代码**——宏名和结构体字段以实际配置后的生成文件为准。
> 
> **DaVinci 操作**：在 `Appl\GenData\` 下**没有任何 Wdg 生成文件**——你工程没在 DaVinci 里配 Wdg 模块。如果将来要加：DaVinci 搜索 Wdg → 加模块 → 配 Instance/超时/时钟源 → 重新生成 → GenData 下就有 `Wdg_Cfg.h` + `Wdg_PBcfg.c`。

### 将来会生成的文件（从 MCAL generate 目录推断的模板结构）

| 文件 | 典型路径 | 核心内容 |
|---|---|---|
| Wdg_Cfg.h | `Appl\GenData\include\Wdg_Cfg.h` | Instance 开关（`WDG_INSTANCE0 STD_ON`）+ API 开关 + 配置结构体声明 |
| Wdg_PBcfg.c | `Appl\GenData\src\Wdg_PBcfg.c` | WDOG 寄存器预打包值（TOVAL/TIMEOUT/CS 寄存器）+ 顶层配置入口 |
| WdgIf_Cfg.h | 如果用 WdgIf 模块 | WdgIf → Wdg Instance 映射关系 |

### WDOG 关键寄存器（S32K144）

```c
// WDOG_CS 寄存器（Control & Status）——最关键
WDG_CS_PRES_U32             // ← 运行时禁止再写 CS
WDG_CS_CMD32_ENABLE_U32     // ← 允许 32-bit 刷新命令（A5A5 / 5A5A）
WDG_CS_WDT_ENABLE_U32       // ← ★ 看门狗 Enable/Disable
WDG_CS_CLK_UNDER_WDG_CONTROL_U32  // ← 时钟源选 LPO/FIRC/SOSC/BUSCLK
WDG_CS_IRQMASK_U32          // ← ★ Enable/Disable 看门狗中断
WDG_CS_BYTES_U32            // ← 超时时间（tick 数）

// WDOG_TOVAL 寄存器（Timeout Value）——由 DaVinci 配的超时时间直接决定
WDG_TOVAL_TIMEOUT_VALUE_U32(timeout_ticks)

// WDOG_WIN 寄存器（Window Value）——窗口模式（可选）
// WDOG_FLG 寄存器（Flag）——超时标志

// 顶层配置入口
CONST(Wdg_ConfigType, WDG_CONST) Wdg_PBCfgVariantPredefined =
{
    WDG_CONFIGSET_0,   // Instance ID
    &Wdg_Wdog_ConfigPB0 // ← 配置表指针
};
```

### 如果将来要配 Wdg 的完整步骤

1. DaVinci 加 Wdg 模块 → 配 General → Instance 0 → 超时时间（ms）→ 时钟源 → 是否开中断
2. 重新生成代码 → GenData 下有 `Wdg_Cfg.h` + `Wdg_PBcfg.c`
3. 在 `EcuM_Callout_Stubs.c` 的 `EcuM_AL_DriverInitOne` / `Two` 里加：
   ```c
   Wdg_Init(&Wdg_PBCfgVariantPredefined);  // PRECOMPILE 模式无参数
   Wdg_Start(WDGIF_INSTANCE_INFID_1);      // 使能看门狗
   ```
4. 在应用层定期刷新：`Wdg_Trigger()`（超时前必须刷）
5. **调试 HardFault 时看门狗复位排查**：`Mcu_GetResetRawValue()` 读 bit5 = 看门狗复位（见 Mcu_Cfg.h L426）

### Wdg 高频坑点（综合经验）

| 坑 | 位置 | 表现 |
|---|---|---|
| 超时时间配太短 | `WDOG_TOVAL` | 系统一直复位（Boot 循环，每秒复位一次） |
| 超时时间配太长 | `WDOG_TOVAL` | 看门狗没起到作用——跑飞很久才复位 |
| IRQMASK 关了 | `WDOG_CS` | 看门狗复位了没中断——`Mcu_GetResetRawValue()` 读 bit5 确认 |
| EcuM 初始化顺序不对 | EcuM_Callout_Stubs.c | Mcu_Init 先（开时钟）→ Port_Init → Wdg_Init → **Wdg_Start**（在应用层开始跑之前必须 Enable） |
| 没调 Wdg_Trigger() | 应用层 Task | 正常运行中定期刷新——超时复位 |
| 中断开了但 ISR 没注册 | Os_Cfg | Wdg 超时进空指针 ISR → HardFault |

---

## 10. BSW 层模块速查（CanIf/CanTp/Com/EcuM）

### 10.1 CanIf（CAN 接口层——连接 Can 和 Com/PduR）

| 文件 | 路径 | 核心内容 |
|---|---|---|
| CanIf_Cfg.h | `Appl\GenData\CanIf_Cfg.h` | API 开关 + TxBuffer 配置 + Controller 数量（1 = `CANIF_ONE_CONTROLLER_OPTIMIZATION STD_ON`） |
| CanIf_PBcfg.c | `Appl\GenData\CanIf_PBcfg.c` | RxPdu/CanController/CanHwObject 映射表 |

**关键宏**（CanIf_Cfg.h L67-L105）：
```c
#define CANIF_ONE_CONTROLLER_OPTIMIZATION   STD_ON     // ← 只有 1 个 Controller，代码优化
#define CANIF_TRANSMIT_BUFFER               STD_ON     // ← TxBuffer 开了
#define CANIF_DEV_ERROR_DETECT              STD_ON
#define CANIF_DLC_CHECK                     STD_ON
#define CANIF_WAKEUP_SUPPORT                STD_OFF    // ← 唤醒功能没开
```

**配置链路**：
```
Can Controller → CanIf Channel → CanIf Hw Object → CanIf TxBuffer → CanIf Rx Pdu → PduR
```

### 10.2 EcuM（ECU 状态管理——**所有驱动初始化的入口**）

> **Vector MICROSAR 特殊点**：`EcuM_PBcfg.c` 和 `EcuM_Init_PBcfg.c` 在你工程里都是**空的**！——初始化函数列表通过 `EcuM_Cfg.c` 的函数指针数组 `EcuM_DriverInitOne[1] = { EcuM_AL_DriverInitOne }` 间接引用。**真实初始化顺序在用户层文件 `Appl\Source\EcuM_Callout_Stubs.c` 里**。

**你工程真实初始化顺序**（`EcuM_Callout_Stubs.c` L274-L284）：
```c
// EcuM_AL_DriverInitOne 中：
Mcu_Init( &McuModuleConfiguration );                                    // ① Mcu 初始化
Mcu_InitClock(McuConf_McuClockSettingConfig_McuClockSettingConfig_0);  // ② 开 PLL
while (MCU_PLL_LOCKED != Mcu_GetPllStatus());                          // ③ 等 PLL 锁定
Mcu_DistributePllClock();                                               // ④ 分发 PLL 时钟
Port_Init( &PortConfigSet );                                            // ⑤ Port（所有 Pin 配置）
Spi_Init( NULL_PTR );                                                   // ⑥ Spi（PRECOMPILE 无参数）
Adc_Init( NULL_PTR );                                                   // ⑦ Adc
Pwm_Init( NULL_PTR );                                                   // ⑧ Pwm
Gpt_Init( NULL_PTR );                                                   // ⑨ Gpt
// ↓ 注意：Can_Init/Dio_Init/CanIf_Init/Com_Init 没有在 DriverInitOne 里调！
// 只有 Can_InitMemory()/Com_InitMemory() 等清零操作在 EcuM_AL_DriverInitZero 里
// → 说明你工程只初始化了 MCAL 层的 Mcu/Port/Spi/Adc/Pwm/Gpt
```

**配置链路**：
```
DaVinci 配 EcuM → 搜 DriverInitOne → 添加要初始化的模块 → 生成 EcuM_Cfg.c 的函数指针数组
用户层 EcuM_Callout_Stubs.c → 实现 EcuM_AL_DriverInitOne 函数体（手动放初始化顺序）
```

**关键坑**：
- Mcu_Init 必须第一个（开所有外设时钟）
- Mcu_InitClock + while(PLL_LOCKED) + DistributePllClock 三件套不能省
- Port_Init 在 Mcu_InitClock 之后（Port 用 PLL/LPO 时钟）
- 其他 MCAL 模块在 Port 之后（硬件引脚已经就位）
- **将来加 Wdg**：在 Gpt_Init 之后加 `Wdg_Init()` + `Wdg_Start()`

### 10.3 Com / CanTp / CanSM / PduR

| 模块 | Cfg.h 重点 | PBcfg/Lcfg 重点 |
|---|---|---|
| **Com** | Signal/PG 命名宏 + 发送/接收策略 | ComSignal 映射表（PG → Signal） |
| **CanTp** | Baudrate/NAs/Timeout 宏 | Rx Pdu 配置 + FC 通道配置 |
| **CanSM** | CANIF 宏引用 + BusOff 恢复策略 | 无 PBcfg（PRECOMPILE 模式） |
| **PduR** | Pdu 路由路径配置 | Pdu 转发表（Com → CanIf / CanTp → CanIf） |

---

## 11. 通用速查技巧

### 11.1 如何判断一个模块的配置模式

看生成文件：
- **有 PBcfg.c + Cfg.h** → POST_BUILD 模式（Gpt/Adc/Mcu/Pwm/Spi）
- **只有 Cfg.h，PBcfg.c 是空的** → PRECOMPILE 模式 + Cfg.h 里宏定义 `CAN_CONFIGURATION_VARIANT_PRECOMPILE`
- **有 Lcfg.c** → LINK 模式（Can/Lcfg.c）

### 11.2 每个模块的配置生效链

```
DaVinci GUI 配置
    ↓ "Generate Code"
Appl\GenData\
    include\<Module>_Cfg.h         → 命名宏（替换硬编码）+ API 开关
    include\<Module>_CfgDefines.h  → Adc 独有：额外命名宏 + 开关
    src\<Module>_PBcfg.c           → 配置表（寄存器预打包值）+ 顶层配置入口
    src\<Module>_Lcfg.c            → Can 独有：真实配置表
    ↓ EcuM 初始化时调 <Module>_Init(&ConfigStruct)
MCAL 驱动内部:
    存配置指针 → 循环遍历配置表 → REG_WRITE32(寄存器基地址 + 偏移, 宏值)
        ↓
硬件寄存器被写入 → 外设按 DaVinci 配的参数开始工作
```

### 11.3 判别性实验（学新模块时用）

> **最笨但最有效的方法**：改一个 DaVinci 配置项 → 重新生成 → 搜索生成文件里的变化 → 确认驱动确实用它

**例子**：学 Pwm 时
1. DaVinci 把 FTM Period 从 0xFFFF 改成 0x1000
2. 重新生成
3. 搜索 `Pwm_PBcfg.c` → 找到 `((Pwm_PeriodType) 0x1000)`
4. 去 MCAL 驱动搜 `Pwm_Init → Pwm_Ftm_ModuleConfig[].Period → FTM_MOD寄存器写`
5. 一条链路打通

### 11.4 高频踩坑对照表

| 现象 | 第一个排查点 |
|---|---|
| 外设寄存器读全 0 | Mcu_PBcfg.c PCC 表里该外设时钟没开 |
| GPIO 不输出/不输入 | Port_PBcfg.c 里 PCR 值是否配成了正确的 ALT 模式 |
| SPI/CAN/UART 不通 | 先查 Port Pin Mode → 再查 Mcu PCC 时钟 → 最后查 MCAL 配置表 |
| Gpt/ADC/PWM 中断不进 | Os 中断优先级配置 + MCAL Notification 开关（`*_NOTIFICATION_SUPPORTED`） |
| 系统一直复位 | Mcu_GetResetRawValue() 读 bit5 → 看门狗！→ 查 `Mcu_PBcfg.c L1000 RCM_SRIE_WDOG_INT_DIS` |
| ADC 采样不准 | 查 Adc_HwUnitCfg[].CFG1（Resolution/ClockSource/Divide）+ 查 Mcu PCC ADC 时钟源 |
| DMA 不工作 | Adc_GroupsCfg[] 里没开 DMA + Mcu PCC DMAMUX0 时钟没开 |

---

## 12. 附录：配置模式对照表

| 配置模式 | 宏定义 | 配置文件在哪 | 初始化 API 怎么调 |
|---|---|---|---|
| **PRECOMPILE** | `<MODULE>_CONFIGURATION_VARIANT_PRECOMPILE` + `_PRECOMPILE_SUPPORT STD_ON` | `*_Cfg.h`（宏）+ `*_Lcfg.c`（表） | `<Module>_Init()`（无参数） |
| **LINK** | `<MODULE>_CONFIGURATION_VARIANT_LINKTIME` | `*_Lcfg.c`（表） | `<Module>_Init()`（无参数） |
| **POST_BUILD** | `<MODULE>_CONFIGURATION_VARIANT_POSTBUILD_LOADABLE` + `_PRECOMPILE_SUPPORT STD_OFF` | `*_Cfg.h`（宏）+ `*_PBcfg.c`（表） | `<Module>_Init(&PBCfgVariantPredefined)`（传参数） |
| **混合**（如 Vector Can） | `CAN_CONFIGURATION_VARIANT_PRECOMPILE` | `Can_Cfg.h` + `Can_Lcfg.c`（**不是 Can_PBcfg.c！**） | `Can_Init()`（无参数） |

> **关键区别**：POST_BUILD 模式下 `*_PBcfg.c` 里有 `CONST(Module_ConfigType) Module_PBCfgVariantPredefined = {...}`，`Init()` 时要传这个地址；PRECOMPILE/LINK 模式下 `*_PBcfg.c` 是空的，配置全在 `*_Lcfg.c`，`Init()` 无参数。

---

*文档生成时间：2026-10-23*
*基于工程：CBD1800257_D01_S32K1xx_Applications_S32K144_Start_new_IAR*
*MCAL 版本：S32K14x_MCAL_1_0_1_RTM_ASR_REL_4_2_REV_0002_20180713*
*DaVinci 版本：5.18.37 SP1*
