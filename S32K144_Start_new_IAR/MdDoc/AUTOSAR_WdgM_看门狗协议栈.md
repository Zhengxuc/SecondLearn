# AUTOSAR 看门狗协议栈（Watchdog Stack）学习笔记

> 适用环境：Vector MICROSAR + DaVinci Configurator + NXP S32K144（WDOG）
> 对应 AUTOSAR Classic Platform 4.x

---

## 1. 为什么需要看门狗协议栈

裸机时代应用代码直接喂狗，任何一个任务跑飞，其它任务仍可能"误喂狗"，导致 MCU 卡死却不复位。
AUTOSAR 把**喂狗决策权**从应用手里收走，集中到 WdgM（Watchdog Manager）：

- 应用只负责"打点"（Checkpoint）证明自己活着、按序执行
- WdgM 周期性评估所有监控结果，**全部通过才喂狗**
- 任何一个被监控实体失败且超过容忍度，WdgM 停止喂狗 → 硬件超时复位

一句话：**应用管打点，WdgM 管判断，Wdg 管执行，硬件管复位。**

---

## 2. 分层架构

```
┌─────────────────────────────────────────────────────────────┐
│ Application 层                                               │
│   SWC ── 在 Runnable 里调 WdgM_CheckpointReached(SE, CP) 打点│
├─────────────────────────────────────────────────────────────┤
│ RTE 层                                                       │
│   RTE ── 生成的 C/S 接口，透传打点调用到 WdgM                │
├─────────────────────────────────────────────────────────────┤
│ System Service 层                                            │
│   WdgM ── 监控策略 + 本地/全局状态机 + 喂狗决策（核心）      │
├─────────────────────────────────────────────────────────────┤
│ ECU Abstraction 层                                           │
│   WdgIf ── 设备抽象，按 Device ID 路由，统一 Mode 接口       │
├─────────────────────────────────────────────────────────────┤
│ MCAL 层                                                      │
│   Wdg ── 操作 WDOG 寄存器：超时、模式、喂狗                  │
├─────────────────────────────────────────────────────────────┤
│ Hardware                                                     │
│   WDOG ── S32K144 片上独立看门狗（LPO 128 kHz）              │
└─────────────────────────────────────────────────────────────┘
```

两条关键数据流：

| 路径 | 方向 | 内容 |
|---|---|---|
| ① 打点上报 | SWC → RTE → WdgM | `WdgM_CheckpointReached(SE_ID, CP_ID)` |
| ② 喂狗执行 | WdgM → WdgIf → Wdg → WDOG | 仅当全局状态为 OK 时，`WdgIf_SetTriggerCondition()` → 写寄存器喂狗 |

### 各模块职责

| 模块 | 全称 | 职责 | 关键 API |
|---|---|---|---|
| WdgM | Watchdog Manager | 三种监控判定、Local/Global 状态机、喂狗决策 | `WdgM_CheckpointReached()`、`WdgM_MainFunction()`、`WdgM_GetGlobalStatus()` |
| WdgIf | Watchdog Interface | 多看门狗设备抽象与路由、Mode 透传 | `WdgIf_SetMode()`、`WdgIf_SetTriggerCondition()` |
| Wdg | Watchdog Driver | 直接操作硬件寄存器，设置超时/模式/喂狗 | `Wdg_Init()`、`Wdg_SetMode()`、`Wdg_SetTriggerCondition()` |

---

## 3. WdgM 三种监控方式（重点）

### 3.1 Alive Supervision（存活监控）

- **监控什么**：SE 是否按预期频率"活着"——统计每个监控周期内的打点次数
- **配置参数**：
  - `SupervisionCycle`：监控周期（= `WdgM_MainFunction` 调用周期的整数倍）
  - `ExpectedAliveIndications`：每周期期望打点次数
  - `MinMargin` / `MaxMargin`：允许的向下/向上偏差（容差）
- **判定**：`期望 - MinMargin ≤ 实际打点次数 ≤ 期望 + MaxMargin`，否则失败
- **抓的故障**：任务卡死、死循环不退出、任务不再被调度

### 3.2 Deadline Supervision（时限监控）

- **监控什么**：两个 Checkpoint（Start → Stop）之间的**实际耗时**
- **配置参数**：Start CP、Stop CP、`DeadlineMin` / `DeadlineMax`（时间窗口）
- **判定**：测得耗时超出窗口 → 失败
- **抓的故障**：关键链路执行超时（如 CAN 发送链路）、被高优先级任务长时间抢占

### 3.3 Logical Supervision（逻辑监控 / 程序流监控）

- **监控什么**：Checkpoint 到达的**顺序**是否符合预期
- **配置方式**：在 DaVinci 里画 Checkpoint 转换 Graph（哪些 CP 之间允许跳转）
  - Internal Graph：同一个 SE 内部的 CP 顺序
  - External Graph：跨 SE 的 CP 顺序（可监控跨 SWC 的调用链）
- **抓的故障**：程序跑飞、状态机错序、函数跳过关键步骤

### 三种监控对比速记

| 维度 | Alive | Deadline | Logical |
|---|---|---|---|
| 关注点 | 活没活 | 快不快 | 顺序对不对 |
| 度量 | 打点次数 | 时间跨度 | CP 序列 |
| 最少 CP 数 | 1 | 2（Start/Stop） | ≥2 |
| 典型场景 | 周期任务监控 | 关键链路超时 | 复杂流程/状态机 |

---

## 4. 状态机

### 4.1 Local Status（每个 SE 一个）

```
OK ──监控失败──> FAILED ──恢复(容忍内)──> OK
                   │
                   └──超过 ExpiredCycleTol──> EXPIRED
```

- `OK`：监控通过
- `FAILED`：本周期监控失败，但在容忍范围内，仍可恢复
- `EXPIRED`：连续失败超过 `WdgMExpiredSupervisionCycleTol`，该 SE 判死

### 4.2 Global Status（整个 WdgM 一个）

```
OK ──任一SE失败──> FAILED ──恢复──> OK
                     │
                     └──持续失败──> EXPIRED ──停止喂狗──> MCU 复位
```

- 全局状态是所有 SE 本地状态的"聚合"（取最差）
- **EXPIRED 后 WdgM 停止调 `WdgIf_SetTriggerCondition`** → 硬件看门狗超时 → 复位 MCU
- 查询 API：`WdgM_GetLocalStatus(SE)` / `WdgM_GetGlobalStatus()`

---

## 5. 配置要点（DaVinci Configurator）

### 5.1 Wdg 模块（MCAL，S32K144 特性）

| 配置项 | 要点 |
|---|---|
| 时钟源 | 选 LPO 128 kHz（独立时钟，主时钟挂掉也能复位） |
| 超时时间 | 如 1 s。**整个协议栈的时间基准**，其它参数围着它配 |
| Window 模式 | S32K144 WDOG 支持窗口看门狗：**窗口打开前喂狗也立即复位**。能抓"喂狗过快/任务加速跑飞"，但窗口太窄易因调度抖动误复位 |
| 调试冻结位 | 必须让 WDOG 在 Debug 时暂停计数（DBG 控制位），否则打断点就复位，无法调试 |
| 运行模式 | Slow / Fast / Off，对应 WdgIf 的 Mode 切换 |

### 5.2 WdgIf 模块

- 建 Device 实例 → 映射到 Wdg 驱动
- Mode 映射：`WDGIF_SLOW_MODE` / `WDGIF_FAST_MODE` / `WDGIF_OFF_MODE`

### 5.3 WdgM 模块（核心）

| 配置项 | 说明 |
|---|---|
| Supervised Entity (SE) | 每个被监控的 SWC/Runnable 建一个，生成 `WdgMConf_SupervisedEntity_xxx` ID 宏 |
| Checkpoint (CP) | 每个 SE 下定义打点，生成 `WdgMConf_Checkpoint_xxx` ID 宏 |
| Alive Supervision | SupervisionCycle + ExpectedAliveIndications + Min/MaxMargin |
| Deadline Supervision | Start/Stop CP 对 + DeadlineMin/Max |
| Logical Supervision | 画 CP 转换 Graph（Internal / External） |
| WdgMWatchdog | 连接到 WdgIfDevice，`TriggerCondition` 时间必须 **小于** WDG 硬件超时 |
| WdgMMode | 不同运行模式可启停不同监控集（启动期/正常/下电期策略不同） |
| ExpiredSupervisionCycleTol | FAILED → EXPIRED 的容忍周期数 |
| Dem 事件 | 监控失败可上报 DEM 存故障码 |

### 5.4 调度配置

- `WdgM_MainFunction` 必须挂到 **OS 周期任务**（通过 SchM），典型 10 ms
- **时间链必须满足**（高频考点）：

```
打点周期 < MainFunction周期(SupervisionCycle) < TriggerCondition < WDG硬件超时
```

例如：打点 50 ms、MainFunction 10 ms、TriggerCondition 100 ms、WDG 超时 1 s。

---

## 6. 使用要点（代码）

### 6.1 初始化顺序

由 EcuM 按序自动完成，应用不干预：

```
Wdg_Init()  →  WdgIf_Init()  →  WdgM_Init()
```

初始化完成后 WdgM 进入 Initial Mode，开始监控。

### 6.2 打点（应用唯一要做的事）

```c
/* Alive 监控：周期 Runnable 入口打点 */
void Test_Swc1_MainFunction(void)
{
    WdgM_CheckpointReached(WdgMConf_SupervisedEntity_SE_TestSwc1,
                           WdgMConf_Checkpoint_CP_Main);
    /* ... 业务逻辑 ... */
}

/* Deadline 监控：Start/Stop CP 包住被测代码段 */
WdgM_CheckpointReached(SE_Comm, CP_TxStart);
Com_SendSignal(...);
WdgM_CheckpointReached(SE_Comm, CP_TxDone);

/* Logical 监控：在流程关键节点按序打点 */
WdgM_CheckpointReached(SE_App, CP_Step1_Init);
WdgM_CheckpointReached(SE_App, CP_Step2_Config);
WdgM_CheckpointReached(SE_App, CP_Step3_Run);
```

> MICROSAR 中 SWC 通常通过 RTE 的 C/S 端口调用打点接口；BSW 模块可直接调 WdgM API。

### 6.3 状态查询与故障处理

```c
WdgM_GlobalStatusType g;
WdgM_GetGlobalStatus(&g);          /* WDGM_GLOBAL_STATUS_OK / FAILED / EXPIRED */

WdgM_LocalStatusType l;
WdgM_GetLocalStatus(WdgMConf_SupervisedEntity_SE_TestSwc1, &l);
```

常见做法：一个诊断 SWC 周期读全局状态，通过 Rte_Write 发给 Com 上报，或记录到 DEM。

---

## 7. 常见坑（按踩坑频率排序）

| # | 坑 | 现象 | 对策 |
|---|---|---|---|
| 1 | `WdgM_MainFunction` 周期 > WDG 超时 | 一切正常也复位 | MainFunction 周期 ≤ 超时的 1/10 |
| 2 | 只在初始化打一次点 | 下一周期 Alive 失败 | 打点放周期任务里 |
| 3 | 调试时忘开 WDOG 冻结位 | 一打断点就复位 | 配置 DBG 模式暂停 WDOG |
| 4 | 窗口模式窗口太窄 | 任务抖动导致"早喂"复位 | 先用普通模式，或放宽窗口 |
| 5 | CP 打在中断里，MainFunction 在任务里，频率没算清 | Alive 计数对不上 | 重新核算打点频率与 SupervisionCycle 的比例 |
| 6 | TriggerCondition ≥ 硬件超时 | 偶发复位 | TriggerCondition 必须小于硬件超时并留余量 |
| 7 | 把喂狗当中断回调的一部分，被高负载打断 | 偶发复位 | 喂狗链路放在可靠调度的周期任务上 |

---

## 8. 面试要点（高频 Q&A）

### Q1：AUTOSAR 看门狗栈分几层？各层干什么？

**答**：三层 BSW + 应用。
WdgM（服务层，监控策略与决策）、WdgIf（ECU 抽象层，设备路由）、Wdg（MCAL，寄存器操作）。
应用 SWC 只通过 RTE 打点，不直接喂狗。

### Q2：WdgM 三种监控方式的区别？

**答**：
- Alive：统计打点**次数**，抓任务卡死
- Deadline：测两个 CP 间**耗时**，抓执行超时
- Logical：校验 CP **顺序**，抓程序跑飞/错序

### Q3：为什么不允许应用直接喂狗？

**答**：喂狗决策必须集中。如果应用能直接喂狗，一个任务跑飞后其它任务仍在喂狗，MCU 永远不复位，看门狗失去意义。WdgM 统一评估，任一 SE 失败超容忍就停止喂狗。

### Q4：WdgM 全局状态机有哪些状态？EXPIRED 之后发生什么？

**答**：OK → FAILED → EXPIRED。FAILED 在容忍周期内可恢复 OK；超过 `ExpiredSupervisionCycleTol` 进 EXPIRED。EXPIRED 后 WdgM 停止调 SetTriggerCondition，硬件看门狗超时复位 MCU。EXPIRED 不恢复。

### Q5：MainFunction 周期和硬件超时时间什么关系？

**答**：`MainFunction 周期（SupervisionCycle） < TriggerCondition < 硬件超时`。经验值 MainFunction 周期 ≤ 超时的 1/10。否则正常打点也来不及喂狗，误复位。

### Q6：Alive Supervision 的 ExpectedAliveIndications 和 Min/MaxMargin 是什么？

**答**：每个监控周期期望的打点次数，Min/MaxMargin 是允许的向下/向上偏差。例如期望 5 次、Margin 各 1，则每周期打点 4~6 次都算正常。容差吸收调度抖动。

### Q7：普通看门狗和窗口看门狗的区别？

**答**：普通看门狗只要求"超时期内有喂狗"；窗口看门狗额外要求"不能在窗口打开前喂狗"——喂太早也复位。窗口模式能抓"任务跑飞导致喂狗过快"这类故障，S32K144 WDOG 硬件支持。

### Q8：调试代码时看门狗一直复位怎么办？

**答**：配置 WDOG 的调试冻结位（Debug 模式下暂停计数）；或调试阶段把 WdgM Mode 切到 OFF_MODE。量产代码必须恢复。

### Q9：Internal Graph 和 External Graph 的区别？

**答**：都是 Logical Supervision 的顺序图。Internal 监控**同一个 SE 内部**的 CP 顺序；External 监控**跨 SE** 的 CP 顺序，可用于监控跨 SWC 的完整调用链（如 SWC1→SWC2→SWC3 的端到端流程）。

### Q10：SE 和 CP 是什么？打点 API 参数哪来的？

**答**：SE（Supervised Entity）是被监控的逻辑实体，CP（Checkpoint）是 SE 内的打点位置。两者在 DaVinci 中配置，生成 `WdgMConf_SupervisedEntity_xxx`、`WdgMConf_Checkpoint_xxx` 宏，打点 API `WdgM_CheckpointReached(SE_ID, CP_ID)` 直接用这两个宏。

### Q11：看门狗复位后怎么知道是它干的？

**答**：读 MCU 复位原因寄存器（S32K144 的 RCM_SRS，WDOG 位）；配合 Dem 在 EXPIRED 前记录故障事件，下电不丢失，重启后读出分析。

### Q12：WdgM 的 Mode 有什么用？

**答**：不同运行阶段监控策略不同。典型：启动模式（放宽监控，容忍初始化耗时）、正常模式（全量监控）、下电模式（只监控关键 SE）。通过 `WdgM_SetMode()` 切换，每个 Mode 挂不同的监控参数集。

### Q13：多核场景 WdgM 怎么部署？

**答**：每个核跑一个 WdgM 实例（主核为 Master，其余为 Satellite），SE 按 Runnable 所在核分配；Satellite 核本地判定后把结果同步给 Master，由 Master 统一决定喂狗。

### Q14：一条命令说清整个喂狗链路？

**答**：SWC 打点 → RTE 透传 → WdgM_MainFunction 周期评估三种监控 → 全局状态 OK 时调 WdgIf_SetTriggerCondition → Wdg 写 WDOG 寄存器喂狗；状态 EXPIRED 则停止喂狗，硬件超时复位。

---

## 9. 一句话总结

> **应用只管打点，WdgM 管判断，Wdg 管执行，硬件管复位。**
> 配置的核心是三条时间链匹配：`打点周期 < MainFunction 周期 < TriggerCondition < WDG 超时`。
