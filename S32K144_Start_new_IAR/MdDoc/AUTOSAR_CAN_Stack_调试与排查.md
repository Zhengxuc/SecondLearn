# AUTOSAR CAN 栈：生成后核对与「发不出帧」排查

> 配套文档：`AUTOSAR_CAN_Stack.md`（分层与配置概念）  
> 适用：DaVinci 已配好并生成代码之后的联调  
> 标记：`[通用]` 换 TC275 仍适用；`[本项目]` 仅 S32K144 Start 工程实例

---

## 0. 先建立正确预期

| 该做 | 不该做 |
|---|---|
| 核对 **GenData 配置表**是否对齐 | 一上来通读整份 `BSW/CanIf/CanIf.c` |
| 确认 **Init / ONLINE / MainFunction** 运行时挂接 | 把固定 BSW 算法当日常必读 |
| 按层断点回答「卡在第几层」 | 只在应用里瞎改、不接分析仪 |

**精力建议**：配置表对齐约 60% → 运行时挂接约 30% → 必要时跟 BSW/硬件约 10%。

固定 BSW（`CanIf.c`、`Com.c`、`Can_*.c`）只有断点证明卡在某一 API 时再跟进去看返回值/状态判断。

---

## 1. 生成代码后先核对什么

### 1.1 只看「表」，对齐整条链 `[通用]`

| 层 | 典型文件 | 核对什么 |
|---|---|---|
| **Com** | `Appl/GenData/Com_Lcfg.c`、`Com_Cfg.h` | 信号 ↔ I-PDU、长度/周期、TxMode（Periodic/Direct/Mixed） |
| **PduR** | `PduR_*cfg*` | Com/CanTp ↔ CanIf 的 Source→Dest 是否通 |
| **CanIf** | `CanIf_Lcfg.c`、`CanIf_Cfg.h` | CanId/DLC；`TxPdu→Buffer→HTH`；上层确认/指示是 PduR 还是 CanTp |
| **Can** | `Can_Lcfg.c`、`Can_Cfg.h` | Controller、Hardware Object、波特率、Filter/Mask |
| **Os / SchM** | `Rte*.c`、`Rte_SystemApplication_OsCore0.c` | `Com_MainFunctionTx/Rx`、`CanSM_MainFunction` 等是否进周期任务 |

对齐关系（改错层会白忙）：

| 现象/要改的内容 | 应查哪一层 |
|---|---|
| 信号值/字节序/周期不对 | **Com**（对 DBC） |
| 上层有数据但进不了 CanIf | **PduR** 路由 |
| ID/DLC 不对、绑错通道/Mailbox | **CanIf** |
| 波特率、硬件收不到/发不出 | **Can** + 板级硬件 |

### 1.2 `[本项目]` 快速对照

- 波特率：**500 kbit/s**；控制器：`CT_CAN00`（FlexCAN0）
- Tx 示例 ID：`0x200` / `0x310` / `0x511`（应用）；`0x612`（诊断响应）
- Rx 示例 ID：`0x210` / `0x220`（应用）；`0x610` / `0x614`（诊断）
- 发送链：多个 TxPdu → 同一 Buffer → 同一 HTH → 同一 Tx HO
- 周期任务里已挂：`Com_MainFunctionTx`、`Com_MainFunctionRx`、`CanSM_MainFunction`（见 `Rte_SystemApplication_OsCore0.c`）

### 1.3 应用与启动侧必须确认 `[通用]`

**应用**

- SWC 通过 `Rte_Write_…` 写信号（不要业务里直接 `Can_Write`）
- `[本项目]` 例：`CtLedTask.c` 中 `Rte_Write_CtLedTask_Test_Signal_u16_Siganl(...)`

**启动顺序（概念）**

```
Can_Init → CanIf_Init → PduR_Init → Com_Init
  → CanIf_SetControllerMode(STARTED)   // 常由 CanSM/ComM 间接完成
  → CanIf_SetPduMode(ONLINE)
  → Com_IpduGroupStart
  → 周期任务里 MainFunction 持续跑
```

**板级**

- CAN TX/RX 引脚 Port 复用
- FlexCAN（或 TC275 上 MultiCAN）时钟已开
- 收发器使能（本项目常硬件常开、CanTrcv 未配）

---

### 1.4 各层配置文件怎么看（逐层核对）`[通用]` + `[本项目]` 实例

> 目标：DaVinci 点完生成后，不靠"猜"，而是**按层跳表**确认这条链是否对齐。

#### 1.4.1 先判断"配置到底落在哪个文件"

| `Xxx_PBcfg.c` 里有没有 `CONST(...)` 数组 | 配置实体在哪 | 本工程模块 |
|---|---|---|
| **有** | `Xxx_PBcfg.c`（POST_BUILD） | Mcu / Gpt / Adc / Pwm / Port / Spi |
| **没有（空壳，只有 include）** | `Xxx_Lcfg.c`（LINK / PRECOMPILE） | **Can / CanIf / PduR / Com / CanTp** |

本工程实测：`Can_PBcfg.c`、`Com_PBcfg.c`、`CanTp_PBcfg.c` **全是空壳** → 一律去对应的 `*_Lcfg.c` 找表。

#### 1.4.2 逐层总表（先记住这张）

| 层 | 配置文件（`Appl\GenData\`） | 主表 | 这一层管什么 |
|---|---|---|---|
| **Can**（MCAL） | `Can_Lcfg.c` | `Can_ControllerConfig` / `Can_InitObject` / `Can_InitBasicCan` / `Can_Mailbox` / `Can_CanIfChannelId` | 寄存器级：基址、位时序、验收滤波、硬件对象 |
| **CanIf** | `CanIf_Lcfg.c` | `CanIf_TxPduConfig` / `CanIf_RxPduConfig` / `CanIf_MailBoxConfig` / `CanIf_RxIndicationFctList` / `CanIf_TxConfirmationFctList` | **CAN ID ↔ PDU ID 的翻译表** |
| **PduR** | `PduR_Lcfg.c` | `PduR_RmSrcRom` / `PduR_RmDestRom` / `PduR_RmGDestRom` / `PduR_MmRom` | 纯路由：谁发 → 怎么走 → 交给谁 |
| **Com** | `Com_Lcfg.c` | `Com_TxSigInfo` / `Com_TxPduInfo` / `Com_TxModeInfo`(+True/False) / `Com_RxPduInfo` / `Com_RxSigInfo` | Signal 位布局 + 发送模式/周期 |
| **CanTp** | `CanTp_Lcfg.c` | `CanTp_RxPduMap` / `CanTp_RxSduCfg` / `CanTp_RxSduCfgInd` / `CanTp_RxSduSnv2Hdl`（+Tx 侧同族表） | 诊断分段：单帧/多帧、BS、STmin |
| **Dcm** | `Dcm_Lcfg.c` | `Dcm_CfgDidMgrDidInfo` / `Dcm_CfgDiagServiceInfo` / `Dcm_CfgNetRxPduInfo` | 诊断服务表 + DID 表 |
| **CanSM** | `CanSM_Cfg.h` + `CanSM_Lcfg.c` | 宏定义（无 CONST 表）+ 运行时 `CanSM_ChannelVarRecord` | Bus-Off 恢复参数、通道状态 |
| **CanNm** | 本工程**未配置** | — | 无 `CanNm_*.c`（只有空占位头文件） |

#### 1.4.3 Can（MCAL）：`Appl\GenData\Can_Lcfg.c`

| 表 | 行 | 关键字段 | 本工程值 |
|---|---|---|---|
| `Can_ControllerConfig[1]` | L148 | BaseAddress / InterruptMask1 / 默认波特率 / Mailbox 索引范围 / 滤波器数 / HwStart-Stop | `0x40024000` / `0x00000101` / 500 / 见下 / 8 / RxBasic 1~8、TxBasic 8~9 |
| `Can_InitObject[1]` | L223 | **CBT**（位时序）/ **Control1** | `0x80E018A5` / `0x072D2006` |
| `Can_InitObjectBaudrate[1]` | L243 | 波特率（kbps） | **500** |
| `Can_InitBasicCan[8]` | L172 | Code / Mask（验收滤波） | `[0]` = `0x00000000` / `0x0003FFFF`（**全通**） |
| `Can_InitBasicCanIndex[1]` | L199 | 滤波器起始索引 | — |
| `Can_InitObjectStartIndex[2]` | L263 | InitObject 分组索引 | — |
| `Can_Mailbox[3]` | L292 | HwHandle + 类型 | Tx(HO 8) / Rx(HO 0) / Unused(FIFO 23) |
| `Can_TxOffsetHwToLog[1]` | L314 | HTH 硬件号 → 逻辑号 | — |
| `Can_CanIfChannelId[1]` | L99 | **Controller → CanIf 通道映射** | 与 CanIf 交接的唯一桥 |

运行时（非配置）：`Can_ActiveSendObject[2]` L338、`Can_ControllerData[1]` L367。

**怎么看**：
1. `BaseAddress` 确认是哪个 FlexCAN 实例；
2. `Can_InitObject[0].CBT` 就是**波特率真正的落地值**（改 DaVinci 波特率/采样点，这个数会变）；
3. `Can_Mailbox[]` 决定有几个硬件收发对象；
4. `Can_InitBasicCan[]` 是本工程**全通滤波** → 真正的过滤发生在 CanIf 那层。

#### 1.4.4 CanIf：`Appl\GenData\CanIf_Lcfg.c`（最关键的一层）

**A. `CanIf_TxPduConfig[4]`（L373）——发送翻译表**

| Index | CanId | UpperLayerTxPduId | Dlc | MailBox | 完成回调 | 含义 |
|---|---|---|---|---|---|---|
| 0 | `0x0612` | `CanTpConf_CanTpTxFcNPdu_...` | 8 | 0 | `CanTp_TxConfirmation` | 诊断响应/流控 |
| 1 | `0x0511` | `PduRConf_..._msg_MyECU_Lamp...` | 8 | 0 | `PduR_CanIfTxConfirmation` | 车灯 |
| 2 | `0x0310` | `PduRConf_..._msg_Transmit2...` | 8 | 0 | `PduR_CanIfTxConfirmation` | 普通信号 |
| 3 | `0x0200` | `PduRConf_..._msg_Transmit...` | 1 | 0 | `PduR_CanIfTxConfirmation` | 普通信号 |

**B. `CanIf_RxPduConfig[4]`（L268）——接收翻译表**

| Index | RxPduCanId | RxPduMask | UpperPduId | Dlc | 指示回调 | 含义 |
|---|---|---|---|---|---|---|
| 0 | `0x0614` | `0x47FF` | `CanTpConf_CanTpRxNPdu_...` | 8 | `CanTp_RxIndication` | 诊断**功能寻址**请求 |
| 1 | `0x0610` | `0x07FF` | `CanTpConf_CanTpRxNPdu_...` | 8 | `CanTp_RxIndication` | 诊断**物理寻址**请求 |
| 2 | `0x0220` | `0x07FF` | `PduRConf_PduRSrcPdu_...` | 8 | `PduR_CanIfRxIndication` | 普通信号 |
| 3 | `0x0210` | `0x07FF` | `PduRConf_PduRSrcPdu_...` | 1 | `PduR_CanIfRxIndication` | 普通信号 |

**C. 其余四张辅助表**

| 表 | 行 | 作用 |
|---|---|---|
| `CanIf_MailBoxConfig[2]` | L191 | `[0]`=Tx basic（管 TxPduId 0~3，`PRIOBYCANID`）；`[1]`=Rx basic（管 RxPduId 0~3）|
| `CanIf_RxIndicationFctList[3]` | L239 | `0`=NULL / `1`=`CanTp_RxIndication` / `2`=`PduR_CanIfRxIndication` |
| `CanIf_TxConfirmationFctList[3]` | L343 | `0`=NULL / `1`=`CanTp_TxConfirmation` / `2`=`PduR_CanIfTxConfirmation` |
| `CanIf_TxPduQueueIndex[4]` | L399 | TxPdu handle → Tx buffer handle（多 PDU 共用 HTH 时的排队）|
| `CanIf_CtrlStates`（VAR） | L425 | **运行时**控制器/PDU 模式，不是配置 |

**怎么看 CanIf（三个必查点）**：
1. **Mask 匹配规则**：`(收到ID & Mask) == (配置ID & Mask)`。`0x07FF` = 11 位精确匹配；`0x47FF` = 屏蔽高 3 位（`0x0614`/`0x0615` 都能进）——这就是诊断功能寻址的做法。
2. **回调函数索引**：`RxIndicationFctListIdx=1` → TP 路（不适配 UDS）；`=2` → IF 路（Com）。填错则信号走错分支。
3. **`UpperPduId` 前缀**：`CanTpConf_*` 说明这一帧归 CanTp；`PduRConf_*` 说明归 PduR→Com。

#### 1.4.5 PduR：`Appl\GenData\PduR_Lcfg.c`（4 张表，三层索引）

```
PduR_RmSrcRom[i]  ──1:n──►  PduR_RmDestRom[j]  ──n:1──►  PduR_RmGDestRom[k]
（谁发起 + 目标范围）        （路由类型 + 两侧索引）        （真实 DestHnd）
                                                                  │
                                                                  ▼
                                                            PduR_MmRom
                                                        （Transmit/RxIndication 函数指针）
```

| 表 | 字段 | 说明 |
|---|---|---|
| `PduR_RmSrcRom[8]` | `SrcHnd` / `RmDestRomStartIdx` / `RmDestRomLength` / `MmRomIdx` | 源 PDU（Com/Dcm 的句柄）+ 它的目标从第几条开始、共几条 |
| `PduR_RmDestRom[8]` | `RmGDestRomIdx` / `RmSrcRomIdx` / `RoutingType` | **只有索引，没有句柄**（一条路由一行）|
| `PduR_RmGDestRom[8]` | `DestHnd` / `Direction` / `MmRomIdx` / `RmDestRomIdx` | **全局目标表**，名字里的 **G = Global**，只有它带真实句柄 |
| `PduR_MmRom` | 函数指针 | 最终调用的 `Transmit` / `RxIndication` |

**本工程 8 条路由（按 `RmDestRom` 顺序）**

| # | 类型 | 源 | 目标 |
|---|---|---|---|
| 0 | IF TX | Com `msg_Transmit2` | CanIf `0x0310` |
| 1 | IF TX | Com `msg_Transmit` | CanIf `0x0200` |
| 2 | TP RX | CanTp `msg_diag_Request_MyECU` | Dcm（物理寻址诊断请求）|
| 3 | IF RX | CanIf `0x0210` | Com `msg_Receive` |
| 4 | IF RX | CanIf `0x0220` | Com `msg_Receive2` |
| 5 | IF TX | Com `msg_MyECU_Lamp` | CanIf `0x0511` |
| 6 | TP TX | Dcm `msg_diag_Response` | CanTp（→ CanIf `0x0612`）|
| 7 | TP RX | CanTp `msg_diag_RequestGlobal` | Dcm（功能寻址诊断请求）|

**RoutingType 速查**：`PDUR_IF_*` = 不经分段、直接转发；`PDUR_TP_*` = 走 CanTp 分段；`_TX_` / `_RX_` 决定方向。

#### 1.4.6 Com：`Appl\GenData\Com_Lcfg.c`

| 表 | 行 | 看什么 |
|---|---|---|
| `Com_TxPduInfo[3]` | L350 | `TxBufferLength`（IPdu 字节数）、`TxPduInitValueStart/EndIdx`、信号组范围 |
| `Com_TxSigInfo[6]` | L472 | **`BitLength` / `BitPosition` / `ByteLength` / `StartByteInPduPosition` / `TxPduInfoIdx`** |
| `Com_TxPduInitValue[24]` | L372 | 每个 IPdu 8 字节的上电初值 |
| `Com_TxModeInfo[3]` | L272 | `InitMode` + False/True 模式索引 |
| `Com_TxModeFalse[2]` | L246 | **`TimePeriod`（MainFunction 周期倍数）** |
| `Com_TxModeTrue[1]` | L297 | `Periodic` 等标志 |
| `Com_RxPduInfo[2]` / `Com_RxSigInfo[5]` | L194 / L219 | 接收侧对应表 + 超时监控 |
| `Com_RxAccessInfo[5]` | L138 | Rx 信号的过滤/替换/有效 DLC |

**本工程实例（`msg_MyECU_Lamp` 车灯）**：

| 位置 | 值 | 含义 |
|---|---|---|
| `Com_Cfg.h` | `...sig_LampCnt... = 2u` | Signal ID = 2 |
| `Com_Cbk.h` | `...msg_MyECU_Lamp... = 0u` | PDU ID = 0 |
| `Com_TxSigInfo[2]` | UINT8 / BitLength 8 / BitPosition 0 / StartByte 0 / TxPduInfoIdx 0 | 信号占第 0 字节整字节 |
| `Com_TxPduInfo[0]` | TxBufferLength 8 / InitValue 0~8 | 8 字节 PDU |
| `Com_TxModeInfo[0]` | InitMode TRUE / FalseIdx 1 / TrueIdx 0 | — |
| `Com_TxModeFalse[1]` | **TimePeriod = 10** | 周期 = 10 × `Com_MainFunctionTx` 周期 |

**怎么看 Com**：Signal 名字 → `Com_Cfg.h` 找 ID → `Com_TxSigInfo[ID]` 看位布局 → 用 `TxPduInfoIdx` 跳到 `Com_TxPduInfo` 看长度 → 用 PDU 序号查 `Com_TxModeInfo` 看周期/事件。

#### 1.4.7 CanTp / Dcm / CanSM（诊断支路）

| 层 | 文件 | 看什么 |
|---|---|---|
| **CanTp** | `CanTp_Lcfg.c` | `CanTp_RxPduMap`（N-PDU → N-SDU 映射）、`CanTp_RxSduCfg`（**BS / STmin / N_As/N_Bs/N_Cr 超时**）、`CanTp_RxSduCfgInd`、`CanTp_RxSduSnv2Hdl` |
| **Dcm** | `Dcm_Lcfg.c` | `Dcm_CfgDiagServiceInfo[10]`（支持哪些 UDS 服务：0x10/0x11/0x19/0x27/0x3E/0x85…）、`Dcm_CfgDidMgrDidInfo[1]`（DID 表）、`Dcm_CfgNetRxPduInfo[2]`（物理寻址/功能寻址两路）|
| **CanSM** | `CanSM_Cfg.h`（宏）+ `CanSM_Lcfg.c`（`CanSM_ChannelVarRecord` 运行时） | Bus-Off 恢复：L1/L2 时间、重复次数；本层**没有 CONST 配置表**，参数以宏形式存在头文件 |
| **CanNm** | 本工程未配 | 只有空占位头文件，`Nm` 通用层的函数指针表填不上 → 诊断直接通、无唤醒/休眠 |

**诊断一路的完整跳转**（请求方向）：
`0x0610` → `CanIf_RxPduConfig[1]` → `CanTp_RxIndication` → `CanTp_RxSduCfg`（重组）→ `PduR_RmDestRom[2]` → `Dcm_CfgNetRxPduInfo` → UDS 服务分派。

#### 1.4.8 一条链串起来：`msg_MyECU_Lamp` 从信号到总线

```
① Com_TxSigInfo[2]         sig_LampCnt：UINT8，第 0 字节
        │ TxPduInfoIdx = 0
        ▼
② Com_TxPduInfo[0]         8 字节 IPdu（ID = 0，见 Com_Cbk.h）
        │ 周期到点（Com_TxModeFalse[1].TimePeriod = 10）
        ▼
③ PduR_RmSrcRom[4]         SrcHnd = Com 的 msg_MyECU_Lamp；目标从 DestRom[5] 起 1 条
        │
        ▼
④ PduR_RmDestRom[5]        RoutingType = IF TX；RmGDestRomIdx = 0
        │
        ▼
⑤ PduR_RmGDestRom[0]       DestHnd = CanIfConf_..._msg_MyECU_Lamp（CanIf 的 TxPdu 句柄）
        │
        ▼
⑥ CanIf_TxPduConfig[1]     CanId = 0x0511，Dlc = 8，MailBox = 0，回调 = PduR_CanIfTxConfirmation
        │
        ▼
⑦ Can_Mailbox[0]           HO 8（CN_CAN00_..._Tx）→ Can_Write() → FLEXCAN0 发出 0x0511
```

**反向（发送完成）**：`CanIf_TxConfirmation` → `CanIf_TxPduConfig[1].TxConfirmationFctListIdx = 2` → `PduR_CanIfTxConfirmation` → Com。

#### 1.4.9 三层 ID 对照（最容易混的地方）

| 层 | ID 类型 | 本工程值 | 定义位置 |
|---|---|---|---|
| Com | **IPdu ID** | `msg_MyECU_Lamp` = 0 | `Com_Cbk.h` |
| Com | **Signal ID** | `sig_LampCnt` = 2 | `Com_Cfg.h` |
| PduR | 源/目标句柄 | `SrcHnd` / `DestHnd` | `PduR_Lcfg.c` |
| CanIf | **Tx Pdu ID / Rx Pdu ID** | `CanIfConf_CanIfTxPduCfg_...` | `CanIf_Lcfg.c`（表内位置即 ID）|
| Can | **HTH / HRH（硬件对象逻辑号）** | Tx = 0，Rx = 1 | `Can_Cfg.h`（`CanConf_CN_CAN00_...`）|
| Can | HwHandle（寄存器对象号） | Tx = 8，Rx = 0 | `Can_Lcfg.c` → `Can_Mailbox[]` |

> 记法：**上层用"句柄宏"，Can 用"HO 逻辑号"，寄存器用"HO 硬件号"**——三者靠 `Can_ControllerConfig` 里的 HwStart/Stop 与 `Can_TxOffsetHwToLog` 对齐。

#### 1.4.10 逐层核对清单（改完配置就按这个走）

| # | 检查项 | 看哪里 | 对齐目标 |
|---|---|---|---|
| 1 | CAN ID 与 DBC 一致 | `CanIf_TxPduConfig` / `CanIf_RxPduConfig` | 总线抓包 ID 一致 |
| 2 | DLC 与实际长度一致 | 同上 `Dlc` | `Com_TxPduInfo.TxBufferLength` |
| 3 | Rx Mask 是否误配 | `CanIf_RxPduConfig.RxPduMask` | 物理/功能寻址是否要屏蔽高位 |
| 4 | 回调索引是否指向正确层 | `CanIf_RxIndicationFctList` / `TxConfirmationFctList` | TP 路 vs IF 路 |
| 5 | PduR 三表索引自洽 | `RmSrcRom → RmDestRom → RmGDestRom` | `RmDestRomStartIdx` + `Length` 覆盖正确 |
| 6 | Com 信号位布局 | `Com_TxSigInfo.BitPosition/BitLength` | DBC 起始位/长度 |
| 7 | 发送周期 | `Com_TxModeFalse.TimePeriod` × MainFunction 周期 | 需求周期 |
| 8 | HTH/HRH 存在且匹配 | `CanIf_MailBoxConfig` ↔ `Can_Mailbox` | 引用真实 HO |
| 9 | 波特率落地值 | `Can_InitObject.CBT` | 500 kbps 位时序 |
| 10 | 诊断两路都配 | `CanIf_RxPduConfig[0][1]` + `Dcm_CfgNetRxPduInfo[2]` | 物理 0x0610 / 功能 0x0614 |

---

## 2. 「一帧发不出去」排查总流程

先接 **CAN 分析仪**，波特率与工程一致（本项目 500k），看目标 CanId 是否出现。

然后 **从上往下** 定位卡点：

```
① 应用有没有写信号？
      Rte_Write / Com_SendSignal 是否调用、缓冲值是否变化
        ↓
② Com 会不会触发发送？
      Com_MainFunctionTx 是否周期执行
      该 I-PDU 是否已 Com_IpduGroupStart
      TxMode：Periodic 要等周期点；Direct 才接近“写后快发”
        ↓
③ PduR 有没有往下传？
      断点/确认是否进入通往 CanIf 的 Transmit 路径
        ↓
④ CanIf 接没接住？
      CanIf_Transmit 是否进入、返回是否 E_OK
      控制器 STARTED？PDU 模式 ONLINE？（常被 CanSM/ComM 管）
      HTH 是否 BUSY？有无进 Tx Buffer？之后有无 TxConfirmation？
        ↓
⑤ Can 驱动有没有写出？
      Can_Write 是否调用；返回 OK / BUSY / 其它
      Tx 完成中断 / CanIf_TxConfirmation 有没有回来
        ↓
⑥ 硬件与总线
      波特率一致、终端电阻、收发器、TX 脚有无翻转
      有无错误帧 / Bus-Off
```

**经验**：新手「总线上完全没有帧」多数卡在 **②③④**（MainFunction / IPduGroup / ONLINE / STARTED / CanSM 未 FULL_COM），而不是 CanIf 核心算法写错。

---

### 2.1 主动模拟：人为制造「发不出去」

排查能力来自**先会制造故障**。按层注入，一次只改一处，观察现象是否与预期一致。

| 层 | 注入手段 | 预期现象 | 从这个现象能学到什么 |
|---|---|---|---|
| **调度层** | 在 `LedRunnable` 里加 `for` 空转 / 长 `Spi_SyncTransmit` | 帧还在，但**周期抖动**、偶发丢帧 | Task 是非抢占的，占着 CPU 就推迟 `Com_MainFunctionTx` |
| **调度层** | 注释掉 `CtLedTask_InitRunnable()` 里的 `RequestComMode(COMM_FULL)` | **一帧都没有**，ComM 停在 NO_COM | ComM 是 CAN 的"总电源" |
| **应用层** | 注释 `Rte_Write_…`，或函数开头 `return;` | 完全没帧（PERIODIC/事件型都停） | 应用只负责写缓冲 |
| **Com** | 把 TxMode 改成 `NONE`；或临时 `if(0) Com_MainFunctionTx();` | 有信号值、无帧 | PERIODIC 完全依赖 MainFunction |
| **Com** | `Com_IpduGroupStop(0)` | 整组静默 | IPDU Group 是总闸 |
| **PduR** | 把 `RmGDestRom` 的 `DestHnd` 指到别的 CanIf Pdu | 帧还在，但 **CAN ID 变成别的** | PduR 只做搬运，不改内容 |
| **CanIf** | 改 `CanIf_TxPduConfig[i].CanId` / `Dlc` | ID 错 / DLC 错（对端可能丢弃） | ID 与 DLC 在此层落地 |
| **CanIf** | 把 `MailBox` 指向不存在的 HTH | `CanIf_Transmit` 返回 `E_NOT_OK` | HTH 必须与 `Can_Mailbox` 对齐 |
| **Can MCAL** | 波特率 500 → 250 | 对端收不到；错误帧暴涨 → **Bus-Off** | 波特率是所有通信的前提 |
| **Can MCAL** | `Can_SetControllerMode(0, CAN_CS_STOPPED)` | 完全无帧，`Can_Write` 被拒 | 控制器必须先 STARTED |
| **硬件** | 拔 CAN_H 或 CAN_L | 无 ACK → 重发 → Bus-Off | CAN 必须有对端应答 |
| **硬件** | 拆掉一个 120Ω 终端电阻 | 短距离可能正常，长线/高速时错误率上升 | 终端匹配影响信号完整性 |
| **硬件** | 收发器打 Standby（若可控） | MCU TX 脚有波形，总线无波形 | 区分"MCU 没发"和"收发器没送出去" |

### 2.2 单节点无 ACK —— 最容易误判的坑

**CAN 是带应答的协议**：一帧发出去，必须有另一个节点在 ACK 位拉低。

```
只有一块板：
  发帧 → 无人 ACK → 控制器自动重发（直到错误计数超限）
       → Tx Mailbox 一直被占
       → Can_Write 持续返回 CAN_BUSY
       → 看起来"发不出去"，其实是"发出去没人应答"
```

**典型误判**：以为 Com/CanIf 配错，实际是总线上没有第二个节点。

**解决**：
1. 接 CANoe / PCAN / USB-CAN 分析仪（既能当对端，又能抓包）；
2. 或让控制器进 **listen-only / loopback（self-test）** 模式先自测内部链路；
3. 观察 `Can_Write` 返回值 + Bus-Off 计数是否持续增长来确认。

### 2.3 三个必做实验（建立直觉，按顺序做）

| # | 实验 | 目的 | 预期结果 |
|---|---|---|---|
| 1 | **只留一块板**，跑周期发送 | 验证"无 ACK"行为 | `Can_Write` 返回 `CAN_BUSY`；Bus-Off 计数上升；分析仪无有效帧 |
| 2 | 注释 `LedRunnable` 里的 `Spi_SyncTransmit`，只留 `Rte_Write` | 排除应用 Task 占 CPU 造成的抖动 | 帧周期变稳定 |
| 3 | `Com_MainFunctionTx` 打断点并计数，同时分析仪抓包 | 验证"Com 发一次 = 总线出一帧" | 计数增量与抓包帧数 1:1（PERIODIC 型） |

### 2.4 返回值速查（断点时看这几个就够）

| API | 返回值 | 含义 | 下一步查 |
|---|---|---|---|
| `Com_SendSignal` | `E_OK` | 写入成功 | 继续看 MainFunction |
| | `E_NOT_OK` | 信号 ID 非法 / 未 Init | 查 `Com_Cfg.h` 的 Signal ID |
| `PduR_ComTransmit` | `E_OK` | 路由命中并下传 | 继续看 CanIf |
| | `E_NOT_OK` | 无路由 / 目标不可用 | 查 `PduR_RmSrcRom` 是否覆盖该 PDU |
| `CanIf_Transmit` | `E_OK` | 已进 Tx Buffer 或已写 HO | 等 TxConfirmation |
| | `E_NOT_OK` | 队列满 / 控制器非 STARTED / PDU 非 ONLINE | 查 `CanIf_CtrlStates` |
| `Can_Write` | `CAN_OK` | 已写入硬件对象 | 等 Tx 完成中断 |
| | `CAN_BUSY` | **上一个还没发出去**（多为无 ACK） | 查对端 / 总线 / Bus-Off |
| | `CAN_NOT_OK` | 控制器未启动 / 参数非法 | 查 `Can_SetControllerMode` |

---

## 3. 各层具体怎么查

### 3.1 ① 应用层

| 查 | 方法 |
|---|---|
| Runnable 是否跑 | 断点 SWC 周期函数 |
| 是否写入 | 断点 `Rte_Write_…`；看 Com 侧信号缓冲 |
| 写了但周期 PDU 不发 | 正常：Periodic 要等 `Com_MainFunctionTx`，不是 Write 瞬间上总线 |

### 3.2 ② Com

| 查 | 方法 |
|---|---|
| MainFunction | 断点 `Com_MainFunctionTx`，确认周期进 |
| IPduGroup | 是否 `Com_IpduGroupStart`；group 停着则整组静默 |
| TxMode / MDT | Periodic vs Direct；MDT 过长会拖发送 |

### 3.3 ③ PduR

| 查 | 方法 |
|---|---|
| 路由存在 | GenData 里该 Com Tx I-PDU 的 Dest 是否指向 CanIf |
| 运行时 | 从 Com 发送路径跟到 PduR 再进 `CanIf_Transmit` |

### 3.4 ④ CanIf

| 查 | 方法 |
|---|---|
| `CanIf_Transmit` | 是否调用；`E_OK` / `E_NOT_OK` |
| 模式 | Controller STARTED、PduMode ONLINE |
| 通道状态 | CanSM 是否 FULL_COM（NO_COM 时往往发不出） |
| 缓冲与 HTH | 多 PDU 共 HTH 时看 Buffer；无 Confirmation 可能卡在忙/未完成 |

对照生成表：`CanIf_TxPduConfig` 的 CanId、DLC、MailBox/HTH 索引、Confirmation 回调（PduR vs CanTp）。

### 3.5 ⑤ Can 驱动

| 查 | 方法 |
|---|---|
| `Can_Write` | 是否进入；BUSY 可能被 CanIf 缓冲重试 |
| 中断 | Tx 完成是否触发；有无进 `CanIf_TxConfirmation` |
| DET | `CANIF_DEV_ERROR_DETECT` 等开启时看是否报未 Init/非法句柄 |

`[平台]`：S32K 跟 FlexCAN 寄存器/IRQ；TC275 跟 MultiCAN——**API 名相同，底层不同**。

### 3.6 ⑥ 硬件

| 查 | 方法 |
|---|---|
| 波特率/采样点 | 分析仪；两端必须一致 |
| 终端 | 总线电阻约 60Ω（两端各 120Ω） |
| 收发器 | EN/STB 是否 Normal |
| 波形 | 示波器看 MCU TX 脚有无边沿（有边沿总线上没有 → 收发器/总线问题） |
| Bus-Off | 错误帧多、节点脱离；查 CanSM 恢复与接线 |

---

## 4. 推荐断点清单（发送）

按优先级下断（一次不要下太多，从上到下开）：

1. 应用：`Rte_Write_…`（你关心的那路信号）
2. `Com_MainFunctionTx`
3. `CanIf_Transmit`
4. `Can_Write`
5. `CanIf_TxConfirmation`（或 `PduR_CanIfTxConfirmation`）

同时看：

- CanIf / CanSM 通道与 PDU 模式相关变量  
- DET 错误钩子（若启用）  
- 分析仪上目标 CanId  

---

## 5. 接收不通时（简表）

反着查：

```
总线有帧？ → 硬件 Filter/Mask 是否放行
  → Can Rx 中断 / CanIf_RxIndication 是否进
  → CanIf 是否匹配到 RxPdu（CanId+Mask）
  → PduR → Com_RxIndication
  → Com 缓冲是否更新
  → Rte_Read 是否读到
```

注意：应用是 **拉模式**——中断只更新 Com，SWC 要自己 `Rte_Read`。

---

## 6. 最小自测步骤（建议每次改配置后）

1. 编译烧录；分析仪波特率与工程一致  
2. 确认周期任务里 `Com_MainFunctionTx` 在跑  
3. SWC 持续 `Rte_Write` 某一 Tx 信号  
4. 分析仪是否出现对应 CanId（本项目如 `0x200`/`0x310`/`0x511`）  
5. 没有 → 按 §2 的 ①→⑥ 逐层断  
6. 有帧但信号值不对 → 回 Com/DBC（字节序、start bit），不是 CanIf 通道问题  

---

## 7. 和 `AUTOSAR_CAN_Stack.md` 的分工

| 文档 | 内容 |
|---|---|
| `AUTOSAR_CAN_Stack.md` | 分层、CanIf 配置树、HTH/Buffer、硬件 vs 通用 |
| **本文** | 逐层核对生成表（§1.4）、发不出/收不到怎么查、主动故障注入（§2.1~2.4）、断点与自测 |

换 **TC275** 时：本文排查顺序全部可复用；只把 §1.2 的 ID/控制器名和 §3.5/§3.6 的硬件对象换成新工程即可。
