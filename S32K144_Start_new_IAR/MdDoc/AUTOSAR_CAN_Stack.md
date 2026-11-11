# AUTOSAR CAN 通信栈（CAN Stack）学习笔记

> 对应 AUTOSAR Classic Platform 4.x（Vector MICROSAR + DaVinci Configurator）  
> 本文以 **NXP S32K144** 工程为实例；即将切到 **Infineon TC275** 时，请先看下方「通用 vs 平台」图例。  
> **生成代码后的联调 / 发不出帧排查**：见同目录 `AUTOSAR_CAN_Stack_调试与排查.md`。

### 文档标记约定

| 标记 | 含义 | 换到 TC275 |
|---|---|---|
| **`[通用]`** | AUTOSAR / CAN 协议层概念与模块职责 | **直接带走**，换平台仍成立 |
| **`[本项目]`** | 本 S32K144 Start 工程的具体 PDU/ID/配置实例 | 仅作对照示例，TC275 工程会不同 |
| **`[S32K144]`** | NXP FlexCAN、基址、本板收发器/引脚等 | **不能照搬**；TC275 换成 MultiCAN/MCMCAN + 对应 MCAL |

### 换平台带走什么（S32K144 → TC275 速览）

```
带走 [通用]：
  Com / PduR / CanIf / CanSM / CanNm / CanTp 分层与数据流
  Signal / I-PDU / L-PDU、HTH/HRH、TxPdu→Buffer→HTH 配置思维
  MainFunction 挂接、启动序列、Bus-Off/CanSM 思路

必须重配 / 重学 [S32K144→TC275]：
  Can 驱动实现与 Hardware Object（FlexCAN MB → Aurix MultiCAN 对象）
  时钟 / Port / 中断号与优先级（MCU、Os 都换）
  控制器名、基址、波特率位时序寄存器算法
  板级收发器与 CanTrcv 是否启用

通常要重新生成但概念不变：
  CanIf 绑新的 Controller/HO；Com/PduR 若网络矩阵相同可高度复用思路
```

---

## 1. CAN 栈在 AUTOSAR 里的位置 `[通用]`

CAN 栈是 AUTOSAR 中**分层最多**的通信栈。与 Spi/Adc 那种"MCAL 直供上层"不同，CAN 从信号到总线要经过 5 层：

```
┌──────────────────────────────────────────────────────────┐
│ SWC（Rte_Write / Rte_Read 读写信号）              [通用]  │
├──────────────────────────────────────────────────────────┤
│ Com  ── 信号打包/解包（Signal ↔ I-PDU）           [通用]  │
├──────────────────────────────────────────────────────────┤
│ PduR ── PDU 路由到 CanIf / CanTp / LinIf…         [通用]  │
├──────────────────────────────────────────────────────────┤
│ CanIf── PDU→L-PDU、HTH/HRH、收发回调              [通用]  │
├──────────────────────────────────────────────────────────┤
│ Can  ── CAN 驱动（MCAL）：HO/Mailbox、状态机      [通用API]│
├──────────────────────────────────────────────────────────┤
│ Hardware ── 控制器 + 收发器 + 总线     [平台相关，见 §5.1] │
└──────────────────────────────────────────────────────────┘
支线模块 [通用]：CanSM / CanNm / CanTp / CanTrcv（收发器按项目选用）
```

- **`[S32K144]` 本工程硬件实例**：FlexCAN0 + 板载收发器  
- **`[TC275]` 预期**：Aurix MultiCAN（或 MCMCAN）+ 对应收发器；**上四层概念不变，只换 Can 驱动与硬件**

**为什么要分这么多层**：每层只干一件事——Com 不懂总线类型（同一份 Com 配置可跑 CAN/LIN/Ethernet），CanIf 不懂信号语义，Can 驱动不懂上层协议。**换 MCU 主要换 MCAL/硬件；换总线类型主要换 If + 驱动。**

---

## 2. 核心概念（面试必考）`[通用]`

### 2.1 三个 PDU 层次

| 名称 | 所在层 | 内容 |
|---|---|---|
| **Signal** | Com | 应用语义的最小单位（如"车速"，12 bit） |
| **I-PDU** | Com / PduR | Interaction PDU：多个信号打包成的一帧（对应一个 CAN 报文的数据场） |
| **L-PDU** | CanIf / Can | Link PDU：加上 CAN ID 后的完整数据链路帧 |

数据流：`Signal →（Com 打包）→ I-PDU →（PduR 路由）→（CanIf 加 ID 变 L-PDU）→（Can 写 HO/Mailbox）→ 总线`

### 2.2 HTH / HRH / Mailbox / Controller（易混，先分清）

| 名字 | 所在层 | 是什么 | 标记 |
|---|---|---|---|
| **Controller** | CanIf / Can | 一整路 CAN 外设（逻辑通道） | `[通用]` 概念 |
| **Mailbox / Hardware Object（HO）** | **Can（MCAL）** | 硬件发送/接收对象 | `[通用]` 概念；**实现因芯片而异** |
| **HTH** | **CanIf** | 对某个 **Tx HO** 的逻辑句柄 | `[通用]` |
| **HRH** | **CanIf** | 对某个 **Rx HO** 的逻辑句柄 | `[通用]` |

- **`[本项目/S32K144]`**：Controller=`CT_CAN00` → 物理 **FlexCAN0**；HO = FlexCAN Message Buffer  
- **`[TC275]`**：Controller 仍是 Can/CanIf 里的逻辑名；物理对象变为 **MultiCAN Message Object** 等，由 Infineon/供应商 Can 驱动暴露

关系（通用）：

```
Controller ──拥有──► 若干 HO（Mailbox / Message Object）
                          │
               CanIf 用 HTH/HRH 引用它们
                          │
        Tx: TxPdu → Buffer → HTH → Tx HO → 总线
        Rx: 总线 → Rx HO → HRH → 按 CanId 匹配 RxPdu → 上层
```

- 上层**只认 CanIf PDU ID**——换芯片只改 Can 驱动 + CanIf 对 HO 的引用，Com/PduR 思路可复用
- **BasicCAN**：一个 HTH/HRH 可被多个 PDU 共用（`[本项目]` 即如此）
- **FullCAN**：一个 HO 通常只对应一个固定 CAN ID

### 2.3 PDU ID 路由链 `[通用]`

```
Com_TxIpduId → PduR 路由表 → CanIf_TxPduId → Buffer → HTH → 物理 HO
```

改路由改 PduR；改 CAN ID / DLC / 绑哪个 HO 改 CanIf；改波特率/物理对象改 Can（MCAL）。

---

## 3. 各模块职责 `[通用]`（API 名跨平台相同）

| 模块 | 层 | 职责 | 关键 API | 换 TC275 |
|---|---|---|---|---|
| **Com** | 服务层 | 信号打包/解包、周期/事件、超时、字节序 | `Com_SendSignal` / `ReceiveSignal` | 概念带走 |
| **PduR** | 服务层 | PDU 路由：Com↔CanIf、CanTp↔CanIf、网关 | 纯配置 | 概念带走 |
| **CanTp** | 服务层 | ISO 15765-2 分段（诊断） | `CanTp_Transmit` | 概念带走 |
| **CanSM** | 服务层 | FULL/SILENT/NO_COM、Bus-Off 恢复 | 被 ComM 调 | 概念带走 |
| **CanNm** | 服务层 | 网络管理睡眠/唤醒 | `CanNm_NetworkRequest` | 概念带走 |
| **CanIf** | ECU 抽象 | PDU↔HTH/HRH、收发回调、控制器模式 | `CanIf_Transmit` 等 | 概念带走，**重绑 HO** |
| **Can** | MCAL | 控制器驱动：HO、波特率、中断、Bus-Off | `Can_Write` / `SetControllerMode` | **换实现**（FlexCAN→MultiCAN） |
| **CanTrcv** | 驱动 | 收发器 Normal/Standby/Sleep | `CanTrcv_SetOpMode` | 按板级选用 |

---

## 4. 两条数据流（理解整个栈的关键）`[通用]`

### 4.1 发送流程（自上而下）

```
① SWC: Rte_Write(Signal) 
② Com: 信号打包进 I-PDU 缓冲（周期 PDU 等 Com_MainFunctionTx 统一发）
③ Com → PduR_ComTransmit → CanIf_Transmit(PduId, data)
④ CanIf: 查 TxPdu 得 CanId/DLC；若 HTH 忙则入 Tx Buffer（PRIO_BY_CANID/FIFO），空闲则 Can_Write(Hth, PduInfo)
⑤ Can: 写入空闲 Mailbox，置发送标志 → 硬件仲裁后发出
⑥ 发送完成中断 → CanIf_TxConfirmation →（若 Buffer 有排队则继续发）→ PduR → Com_TxConfirmation
```

> `[通用]` 周期报文不是 `Rte_Write` 时立即发，而是 Com 按周期在 `Com_MainFunctionTx` 里统一触发。  
> `[本项目]` 4 个 TxPdu 共用 1 个 BasicCAN HTH，因此依赖 CanIf Tx Buffer 排队（TC275 工程是否如此看该项目 HO 分配）。

### 4.2 接收流程（自下而上）

```
① 总线报文 → HO/Mailbox 匹配（Filter/Mask）→ 接收中断
② Can: 读出数据 → CanIf_RxIndication(Hrh, data)
③ CanIf: 查表得 PduId → PduR_RxIndication → Com_RxIndication
④ Com: 解包 I-PDU → 更新各 Signal 缓冲
⑤ SWC: 下个周期 Rte_Read(Signal) 读到最新值
```

> `[通用]` 接收是中断驱动直达 Com；应用读取是**拉模式**（Rte_Read 读缓冲）。

---

## 5. 配置要点（DaVinci）

### 5.1 Can 硬件与 Can 驱动（MCAL）

> 分层记忆：`[通用]` **Can 驱动**把寄存器/中断包成 AUTOSAR API；**CanIf** 决定 Controller / HTH/HRH / CanId。  
> `[S32K144]` 硬件=FlexCAN；生成代码：`Appl/GenData/Can_Cfg.h`、`Can_Lcfg.c`；底层 `MCAL/Can_TS_…`。  
> `[TC275]` 硬件=MultiCAN/MCMCAN；换 Infineon（或 SIP 自带）Can MCAL，**API 名仍是 `Can_Write` 等**。

#### 5.1.1 硬件整条链路

**`[通用]` 物理链路（任何 MCU 一样）**

```
MCU 内 CAN 控制器
        │ TX/RX 脚（Port 配置）
        ▼
外部 CAN 收发器  ←→  CAN_H / CAN_L
        │
     两端 120Ω 终端
```

| 部件 | 作用 | 标记 |
|---|---|---|
| **片内 CAN 控制器** | 协议、波特率、收发对象、Filter、错误/Bus-Off | 概念`[通用]`，IP 核`[平台]` |
| **收发器** | MCU 电平 ↔ 差分总线 | `[通用]`；是否走 CanTrcv 看项目 |
| **总线** | 双绞线差分 | `[通用]` |

**`[S32K144]` 本工程实例**

```
FlexCAN0 @ 0x40024000  →  Port TX/RX  →  板载收发器  →  总线
逻辑名 CT_CAN00；本工程只用这一路
```

**`[TC275]` 学习时注意**：控制器名/基址/节点(Node)/Message Object 编号都不同；DaVinci 里仍配 `CanController` + `CanHardwareObject`，只是背后寄存器手册换成 Aurix。

#### 5.1.2 控制器内部能力

**`[通用]` 能力清单（面试用）**

| 能力 | 作用 |
|---|---|
| **Baudrate 位时序** | Prescaler/Seg 等凑比特率；采样点通常 75%~87.5% |
| **收发对象（HO）** | 可配 Tx/Rx，带 ID/DLC/数据（NXP 常叫 Mailbox，Infineon 常叫 Message Object） |
| **Filter / Mask** | 硬件只收匹配 ID |
| **控制器状态** | STOPPED / STARTED / SLEEP；错误计数满 → Bus-Off |
| **中断** | Tx/Rx 完成、Bus-Off、错误 |

**BasicCAN vs FullCAN** `[通用]`（AUTOSAR/CanIf 配置用语）

| 类型 | 含义 |
|---|---|
| **FullCAN** | 一个 HO 盯一个固定 ID |
| **BasicCAN** | 一个 HO（可带 Mask）覆盖多种 ID，多 PDU 共用 |

**`[本项目/S32K144]` HO 实例**

| HO | 类型 | 被谁引用 |
|---|---|---|
| `CN_CAN00_…_Tx` | Tx BasicCAN | CanIf HTH |
| `CN_CAN00_…_Rx` | Rx BasicCAN | CanIf HRH |

#### 5.1.3 Can 驱动职责 `[通用 API]` / `[平台实现]`

**Can 驱动**：对上 AUTOSAR 标准接口，对下写本芯片寄存器。CanIf **不直接碰寄存器**。

| API / 行为 | 作用 | 标记 |
|---|---|---|
| `Can_Init()` | 写波特率、HO、Filter、中断 | API`[通用]`，内容`[平台]` |
| `Can_SetControllerMode` | 启停控制器 | `[通用]` |
| `Can_Write(Hth, PduInfo)` | 写入对应 Tx HO 并发送 | `[通用]` |
| Tx/Rx 中断 → `CanIf_*` 回调 | 上报完成/接收 | `[通用]` |
| Bus-Off 检测 → CanIf/CanSM | 错误恢复链 | `[通用]` |

驱动**管**：初始化、HO 管理、收发、控制器状态。  
驱动**不管**：信号（Com）、路由（PduR）、PDU↔CanId（CanIf）。

和 CanIf 的接缝 `[通用]`：

```
CanIf_Transmit → CanId+HTH → Can_Write → 硬件发出 → TxConf → …
总线 → Rx HO → CanIf_RxIndication(Hrh,…) → 匹配 RxPdu → PduR/CanTp
```

#### 5.1.4 DaVinci Can 模块配什么

| 配置项 | 要点 | 标记 |
|---|---|---|
| **CanController** | 哪路控制器、默认波特率、中断/Polling | 容器`[通用]`；选哪路 IP`[平台]` |
| **BaudrateConfig** | 位时序；经典 CAN / CAN FD | 参数语义`[通用]`；寄存器算法`[平台]` |
| **CanHardwareObject** | Tx/Rx、Basic/Full、Filter/Mask | `[通用]` 结构 |
| **处理模式** | 中断或 Polling；**CAN 中断优先于高频采样中断** | 原则`[通用]`；IRQ 号`[平台]` |
| **Bus-Off** | 上报；恢复多在 CanSM | `[通用]` |

#### 5.1.5 `[本项目/S32K144]` 对照代码

```
CT_CAN00  = FlexCAN0 @ 0x40024000，500 kbit/s
  ├─ CN_CAN00_…_Tx  ← CanIf HTH（4 个 TxPdu 共用）
  └─ CN_CAN00_…_Rx  ← CanIf HRH（4 个 RxPdu 共用）
```

分层一句话：

> **`[通用]`** 硬件跑协议与 HO；Can 提供 `Can_Write`/Mode；CanIf 定通道与 CanId；PduR 定进 CanIf 还是 LinIf。  
> **`[平台]`** 只有 Can 驱动实现 + 控制器 IP（FlexCAN vs MultiCAN）必须按 MCU 重配。

### 5.2 CanIf（DaVinci 配置详解 · 重点）`[通用]` + `[本项目]` 实例

> `[通用]` Com 管信号，PduR 管“谁连谁”，**CanIf 管“PDU ↔ CAN ID/DLC ↔ HTH/HRH”**。  
> `[本项目]` 生成代码：`Appl/GenData/CanIf_Cfg.h`、`CanIf_Lcfg.c`（TC275 工程路径/表内容会变，树结构通常同类）。

#### 5.2.1 DaVinci 树一眼看懂 `[通用]`

```
CanIf
├── CanIfCtrlDrvCfgs          → 绑哪个 Can 驱动 / 控制器
├── CanIfTrcvDrvCfgs          → 收发器 CanTrcv（可空）
├── CanIfDispatchCfg          → BusOff 等事件通知谁（通常 CanSM）
├── CanIfInitCfg              ★ 真正干活的配置
│   ├── CanIfBufferCfgs       → Tx 软件缓冲（排队）
│   ├── CanIfInitHohCfgs      → HTH / HRH（引用 Can 的 HO）
│   │   ├── CanIfHrhCfgs
│   │   └── CanIfHthCfgs
│   ├── CanIfRxPduCfgs        → 接收：CanId/Mask/DLC → 上层 Indication
│   └── CanIfTxPduCfgs        → 发送：CanId/DLC → Buffer → 上层 Confirmation
├── CanIfPrivateCfg           → 内部行为（滤波算法、DLC Check 等）
└── CanIfPublicCfg            → 对外能力开关（TxBuffer、TrcvHandling、DevError…）
```

**平时真正要改/核对的**：`InitCfg` 里的 TxPdu / RxPdu（ID、DLC、上层模块）和 Hoh 引用是否正确。  
换 TC275：树还是这棵，只是 Controller/HO 引用改成新 Can 模块对象。

#### 5.2.2 发送配置引用链（必须背）`[通用]`

```
CanIfTxPduCfg
  ├─ CanId / Dlc / IdType          → 把 PduR 下来的 I-PDU 变成 L-PDU
  ├─ Tx Pdu Buffer Ref             → 指向 CanIfBufferCfg
  ├─ Tx Pdu User Tx Confirmation   → PDUR 或 CAN_TP
  └─ Tx Pdu Ref                    → EcuC PDU（和 PduR 对齐）
        │
        ▼
CanIfBufferCfg（软件队列）
  ├─ Buffer Size                   → 能排队几条（本项目 = 4）
  ├─ Tx Buffer Handling Type       → PRIO_BY_CANID 或 FIFO
  └─ Buffer Hth Ref                → 指向 CanIfHthCfg
        │
        ▼
CanIfHthCfg
  ├─ Hth Can Ctrl Id Ref           → 属于哪个控制器（逻辑通道）
  └─ Hth Id Sym Ref                → 引用 Can 模块哪个 Tx HO
```

`[本项目]` 实际链路：

```
4 个 TxPdu ──► 同一个 Buffer(CHNL_…) ──► 同一个 HTH(CN_CAN00_…_Tx)
              Size=4, PRIO_BY_CANID         └─► Can HO / CT_CAN00(FlexCAN0)
```

`[通用]` 多个 TxPdu 共用 1 个 BasicCAN HTH 时，硬件忙就先入 Buffer；`PRIO_BY_CANID` = 出队时 **CAN ID 小的优先**（不是 FIFO）。

#### 5.2.3 接收配置引用链 `[通用]`

```
总线帧 → Can Rx HO → HRH
              │
              ▼
CanIf 在该 HRH 下按 RxPdu 的 CanId+Mask 软件匹配
              │
              ▼
CanIfRxPduCfg
  ├─ CanId / Mask / Dlc
  ├─ 挂哪个 HRH
  └─ User Rx Indication UL → PDUR 或 CAN_TP
```

`[通用]` 接收**没有** Tx 那种软件 Buffer；靠硬件 Filter + CanIf 软件 Mask。`DLC Check=ON` 时长度不足则丢帧。

#### 5.2.4 各容器配什么

| 容器 | 配什么 `[通用]` | `[本项目]` 现状 |
|---|---|---|
| **CanIfCtrlDrvCfgs** | 逻辑控制器 ↔ Can Controller | 仅 `CT_CAN00`→FlexCAN0 |
| **CanIfTrcvDrvCfgs** | 绑定 CanTrcv | **空**（TrcvHandling OFF） |
| **CanIfDispatchCfg** | BusOff 等通知上层 | 通常 CanSM |
| **CanIfBufferCfgs** | Tx 软件缓冲 Size / FIFO·PRIO / HTH | 1×Buffer，Size=4，PRIO_BY_CANID |
| **CanIfHthCfgs / HrhCfgs** | Ctrl 引用 + Can HO 符号引用 | Tx/Rx 各 1 个 BasicCAN |
| **CanIfTxPduCfgs / RxPduCfgs** | CanId/DLC/上层确认或指示 | 各 4 条（见下表） |
| **Private / Public** | DLC Check、TxBuffer、唤醒等开关 | DLC ON；无扩展帧；无 Trcv |

**为何 Trcv 可为空** `[通用说明 + 本项目选择]`：无睡眠/唤醒需求时，收发器硬件常开即可，软件只启停控制器。需要 Standby/Sleep/PN 时再配 CanTrcv。TC275 板子同样按硬件方案决定，不是 Aurix 强制要配。

#### 5.2.5 `[本项目]` PDU 实表（对照 `CanIf_Lcfg.c`；换平台勿死记 ID）

**TxPdu → L-PDU**

| CanIf TxPdu | CAN ID | DLC | 上层 Confirmation | 用途 |
|---|---|---|---|---|
| `msg_diag_Response_MyECU_…` | `0x612` | 8 | **CanTp** | 诊断响应 |
| `msg_MyECU_Lamp_…` | `0x511` | 8 | **PduR→Com** | Lamp |
| `msg_Transmit2_…` | `0x310` | 8 | **PduR→Com** | 应用发 |
| `msg_Transmit_…` | `0x200` | 1 | **PduR→Com** | 应用发 |

**RxPdu ← 总线**

| CanIf RxPdu | CAN ID | Mask | DLC | 上层 Indication |
|---|---|---|---|---|
| `msg_diag_RequestGlobal_…` | `0x614` | `0x47FF` | 8 | **CanTp** |
| `msg_diag_Request_MyECU_…` | `0x610` | `0x07FF` | 8 | **CanTp** |
| `msg_Receive2_…` | `0x220` | `0x07FF` | 8 | **PduR→Com** |
| `msg_Receive_…` | `0x210` | `0x07FF` | 1 | **PduR→Com** |

`0x07FF` = 标准帧精确匹配；诊断广播 `0x614` 用 `0x47FF` 做带范围/类型位的软件滤波。

整网数据路径：

```
应用:  Com I-PDU ──PduR──► CanIf Tx(0x200/0x310/0x511) ──HTH──► Can_Write
       总线 Rx(0x210/0x220) ──HRH──► CanIf Rx ──PduR──► Com

诊断:  CanTp ──► CanIf Tx(0x612) ──同一 HTH──► Can_Write
       总线 Rx(0x610/0x614) ──同一 HRH──► CanIf Rx ──► CanTp
```

普通通信和诊断 **共用同一 HTH/HRH**，只是 CanIf 表里上层回调不同。

#### 5.2.6 和 Com / PduR 的分工（配置时别改错层）`[通用]`

| 要改什么 | 改哪一层 | 换 TC275 |
|---|---|---|
| 信号布局、周期、字节序 | **Com**（对齐 DBC） | 同网关矩阵可复用思路 |
| 谁连谁（Com↔CanIf、CanTp↔CanIf） | **PduR** | 同 |
| CAN ID、DLC、Buffer/HTH/HRH、上层确认/指示 | **CanIf** | 重绑新 Can 的 HO |
| 波特率、物理 HO、硬件 Filter | **Can（MCAL）** | **整模块按 Aurix 重配** |

#### 5.2.7 配置时盯的 4 件事 `[通用]`

1. **TxPdu / RxPdu**：CanId、DLC、挂哪个 Controller  
2. **HOH 引用**：HTH/HRH 的 `Id Sym Ref` 必须指向 Can 里真实存在的 HO  
3. **Upper layer**：`PduR_*` 还是 `CanTp_*`，与路由一致  
4. **多 PDU 共 HTH**：Buffer Size 与 Handling Type（`PRIO_BY_CANID` / FIFO）符合预期  

### 5.3 PduR `[通用]`

- 纯路由表：`Source(模块,PduId) → Target(模块,PduId)`
- Com→CanIf；CanTp↔CanIf；网关 CanIf→CanIf（跨网段）
- 也可路由到 **LinIf** 等——**总线类型在这一层分叉**

### 5.4 Com `[通用]`

| 参数 | 要点 |
|---|---|
| IPdu | 方向（Tx/Rx）、长度、周期 |
| Signal | bit 偏移、长度、字节序——**必须与 DBC 一致** |
| 发送模式 | Periodic / Direct / Mixed；MinimumDelayTime |
| 超时监控 | Rx 超时，供应用判通信丢失 |
| 初始值/无效值 | 上电默认、Invalid 替换 |

### 5.5 CanSM / CanNm（有网络管理时）`[通用]`

- CanSM：ComM 通道映射、Bus-Off L1/L2 恢复定时器  
- CanNm：NM 周期、Timeout、节点 ID  

---

## 6. 使用要点（代码）`[通用]`（符号名随工程变）

### 6.1 发信号（应用唯一要做的）

```c
/* SWC 周期任务里——RTE 接口，不直接碰 Com */
Rte_Write_PP_Signal_VehicleSpeed(speed);
/* RTE 内部 → Com_SendSignal() → 打包 → 周期到点自动发出 */
```

**架构纪律**：SWC 不直接调 `Com_SendSignal`，走 `Rte_Write`；更不允许直接调 `Can_Write` 绕过抽象层。

### 6.2 收信号

```c
Rte_Read_RP_Signal_EngineSpeed(&speed);   /* 拉模式：读 Com 信号缓冲 */
```

### 6.3 必须挂接的周期函数（SchM/OS 任务）

| MainFunction | 典型周期 | 作用 |
|---|---|---|
| `Com_MainFunctionTx()` | 10 ms | 周期 PDU 发送、MDT 计时 |
| `Com_MainFunctionRx()` | 10 ms | 接收超时监控 |
| `CanIf_MainFunction()` | 10 ms | 控制器状态同步 |
| `CanSM_MainFunction()` | 10 ms | 网络状态机推进 |
| `CanNm_MainFunction()` | 10 ms | NM 报文处理 |
| `Can_MainFunction_Wakeup/BusOff()` | 10 ms | Polling 模式下的事件处理 |

漏挂任何一个，对应功能静默失效——这是新手最常见的"通信不通"原因。

### 6.4 启动序列

```
EcuM → Can_Init → CanIf_Init → PduR_Init → Com_Init
     → CanIf_SetControllerMode(STARTED) → CanIf_SetPduMode(ONLINE)
     → Com_IpduGroupStart → 开始收发
```

---

## 7. 常见坑（按踩坑频率排序）

| # | 坑 | 标记 | 对策要点 |
|---|---|---|---|
| 1 | 字节序/bit 偏移与 DBC 不一致 | `[通用]` | 核对 Intel/Motorola、start bit |
| 2 | 忘挂 `Com_MainFunctionTx` | `[通用]` | 查 Os/SchM 调用链 |
| 3 | IPduGroup 没启动 / PDU 未 ONLINE | `[通用]` | 查 EcuM/ComM 启动序列 |
| 4 | Rx Filter/Mask 配错 | `[通用]`+`[平台]`配法 | 总线工具确认 ID 再核对 Mask |
| 5 | 波特率/采样点不一致 | `[通用]`+`[平台]`位时序 | 分析仪测实际波特率 |
| 6 | 终端电阻缺失 | `[通用]` 硬件 | 总线电阻 ≈ 60Ω |
| 7 | MDT=0 事件风暴 | `[通用]` | 配 MinimumDelayTime |
| 8 | 中断优先级倒挂 | `[通用]` 原则；IRQ`[平台]` | CAN 中断 ≥ 高频采样中断 |
| 9 | Bus-Off 无恢复 | `[通用]` | CanSM + DEM |
| 10 | 诊断与普通 PDU 规划冲突 | `[通用]` | CanTp 与应用 ID 分开规划 |

---

## 8. 面试要点（高频 Q&A）`[通用]`为主

### Q1：画一下 AUTOSAR CAN 栈的分层？

**答**：SWC → RTE → Com → PduR → CanIf → Can（MCAL）→ **片内 CAN 控制器硬件**（S32K 上是 FlexCAN，TC275 上是 MultiCAN 等）。支线：CanSM / CanNm / CanTp / CanTrcv。换 MCU 主要换 Can 驱动与硬件；换总线类型换 If + 驱动。

### Q2：Signal、I-PDU、L-PDU 的区别？

**答**：Signal 是应用语义单位（如车速）；I-PDU 是 Com 层把多个信号打包的一帧数据（对应 CAN 数据场）；L-PDU 是 CanIf 给 I-PDU 加上 CAN ID 后的链路层帧。发送方向 Signal→I-PDU→L-PDU 逐层加头，接收方向逐层剥离。

### Q3：Com 和 PduR 为什么不能合并？

**答**：职责不同。Com 管信号语义（打包、周期、超时），PduR 管与协议无关的 PDU 路由。分开后 Com 可以同时服务 CAN/LIN/Ethernet，PduR 还能做网关（CAN→CAN、CAN→Eth）和诊断路由（CanTp），互不重写。

### Q4：发一帧 CAN 报文的完整路径？

**答**：`Rte_Write` → `Com_SendSignal` 打包进 I-PDU → 周期点 `PduR_ComTransmit` → `CanIf_Transmit` 查表得 HTH → `Can_Write` 写 Mailbox → 硬件仲裁发送 → 发送中断逐级 Confirmation 回 Com。周期报文在 `Com_MainFunctionTx` 统一触发，不是写信号时立即发。

### Q5：收一帧的完整路径？

**答**：报文进 Mailbox 触发中断 → Can 驱动读出 → `CanIf_RxIndication` 查 HRH 得 PDU ID → `PduR_RxIndication` 路由 → `Com_RxIndication` 解包更新信号缓冲 → SWC 下个周期 `Rte_Read` 拉取。接收是中断推送到 Com，应用是拉模式读取。

### Q6：HTH/HRH 是什么？和 Mailbox、Controller 什么关系？

**答**：Mailbox/HO 在 Can（MCAL）层，是真实硬件对象；HTH/HRH 是 CanIf 对 Tx/Rx HO 的逻辑句柄；Controller（如 CT_CAN00）是一整路 CAN 通道，下面挂多个 HO。上层只认 CanIf PDU ID，经 Buffer→HTH 才落到物理 Mailbox——换芯片、改 Mailbox 分配只改 CanIf↔Can 引用，上层零改动。多 PDU 共用一个 BasicCAN HTH 时，用 CanIf Tx Buffer（`PRIO_BY_CANID`/`FIFO`）排队。

### Q7：周期报文和事件报文在 Com 里怎么实现？

**答**：发送模式（TxMode）：Periodic 由 `Com_MainFunctionTx` 按周期发；Direct 是事件触发立即发（`Com_SendSignal` 直接透传）；Mixed = 周期保底 + 事件加速。事件报文要配 MDT（最小间隔）防止总线洪泛。

### Q8：CAN 中断优先级怎么规划？

**答**：CAN 中断要高于普通周期任务，且**必须高于 ADC 这类高频采样中断**——采样中断阻塞 CAN 会导致丢帧和 Bus-Off。原则：通信中断 > 采样中断 > 后台任务。

### Q9：Bus-Off 是什么？怎么处理？

**答**：发送错误计数到 255，控制器自动脱离总线（自我保护）。处理链：Can 驱动检测 → CanSM 进入恢复流程（双级定时器 L1 快恢复/L2 慢恢复）→ 恢复期间该网络 NO_COM → 上报 DEM 记录故障。应用应能通过 DEM/诊断看到 Bus-Off 历史。

### Q10：超过 8 字节的诊断数据怎么传？

**答**：CanTp 按 ISO 15765-2 分段：首帧 FF、流控帧 FC、连续帧 CF。单帧 SF ≤ 7 字节直接发；更长的由 CanTp 拆包，接收方重组。诊断仪刷写/读长 DID 全靠它。

### Q11：CAN FD 和经典 CAN 在配置上的区别？

**答**：CAN FD：数据段最长 64 字节、数据段可用更高波特率而仲裁段可保持较低速率、控制器和收发器都要支持。配置上 DLC 映射、双波特率、HO 长度都不同。`[本项目]` 经典 CAN（8 字节、500 kbit/s）；TC275 工程按该项目是否启用 FD 另配。

### Q12：CanNm 是干什么的？

**答**：网络管理。没有它，ECU 下电睡觉后总线静默就永远睡着。CanNm 通过周期性 NM 报文实现"有人要通信全网醒、没人要通信协同睡"：任何节点 `CanNm_NetworkRequest()` 就发 NM 帧保持全网唤醒；所有节点释放后进入 Prepare Sleep → Bus Sleep。

### Q13：Rte_Write 之后报文多久上总线？

**答**：取决于 TxMode：Direct 模式立即发（毫秒级）；Periodic 模式等下一个 Com 发送周期点。所以"写信号 ≠ 立即发报文"，实时性要求高的信号要用 Direct 或缩短周期。

---

## 9. 切到 TC275 时的学习清单

| 继续用本文哪几节 | 到新工程要重新动手 |
|---|---|
| §1～§4 分层与数据流 | 新 MCU 的 Can MCAL + 时钟/Port/IRQ |
| §5.2 CanIf 树与 `TxPdu→Buffer→HTH` | CanController / HardwareObject 按 MultiCAN 配 |
| §5.3～§5.5 Com/PduR/CanSM/CanNm | 用新工程 DBC/网络矩阵生成 PDU 表 |
| §6 MainFunction / 启动顺序 | Os 任务与 SchM 在 TC275 工程里重挂 |
| §8 面试问答 | 硬件名词改口：Mailbox↔Message Object 等 |

不要把本文 **`[本项目]`** 的 `0x200/0x210`、`CT_CAN00`、`0x40024000` 当成 TC275 事实；只把 **`[通用]`** 配置思维迁过去。

---

## 10. 一句话总结

> **`[通用]`** Com 管信号、PduR 管路由、CanIf 管抽象、Can 管驱动 API、CanSM 管状态、CanTp 管长帧；发送链 **`TxPdu → Buffer → HTH → Can HO`**。  
> **`[平台]`** 只有片内 CAN IP + Can 驱动实现 + 时钟/引脚/中断随 MCU 变（S32K144=FlexCAN，TC275=MultiCAN 等）。  
> 使用上：**MainFunction 必挂；CAN 中断优先级让不得。**
