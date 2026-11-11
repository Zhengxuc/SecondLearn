/**********************************************************************************************************************
 *  Adc_ModePractice_Framework.c
 *  -------------------------------------------------------------------------------------------------------------------
 *  用途：ADC 触发/转换/访问/流式缓冲 组合练习框架（先搭逻辑，DaVinci 配好后再填宏与尺寸）
 *
 *  使用方法：
 *    1. 在 DaVinci 里按下方「配置对照表」改 AdcGroup
 *    2. 重新生成代码后，填写本文件 TODO 区的宏
 *    3. 改 ADC_PRACTICE_CASE 选择练习场景
 *    4. 把 Init / Trigger / Notification / Poll 挂到 CtLedTask（或其它 SWC）里
 *
 *  注意：本文件默认不参与编译；确认无误后再加入工程，或把代码拷进 CtLedTask.c
 *********************************************************************************************************************/

#include "Adc.h"
#include "Std_Types.h"

/*====================================================================================================================
 * 一、DaVinci 配置对照（改 Group 属性，不是改应用代码）
 *
 *  AdcGroupConversionMode     : ADC_CONV_MODE_ONESHOT / ADC_CONV_MODE_CONTINUOUS
 *  AdcGroupTriggSrc           : ADC_TRIGG_SRC_SW / ADC_TRIGG_SRC_HW
 *  AdcGroupAccessMode         : ADC_ACCESS_MODE_SINGLE / ADC_ACCESS_MODE_STREAMING
 *  AdcStreamingNumSamples     : Single 通常=1；Streaming = N（缓冲深度）
 *  AdcStreamingBufferMode     : ADC_STREAM_BUFFER_LINEAR / ADC_STREAM_BUFFER_CIRCULAR
 *                               （仅 AccessMode=STREAMING 时有意义）
 *  AdcNotification            : 填回调函数名（如 AdcGroup0Notification）；轮询练习可先关掉
 *
 *  全局开关（AdcGeneral）：
 *    AdcHwTriggerApi          : 硬件触发练习必须 ON  -> 生成 ADC_HW_TRIGGER_API
 *    AdcGrpNotifCapability     : 回调练习必须 ON
 *    AdcEnableStartStopGroupApi: SW 启停必须 ON（你当前已是 ON）
 *
 *  结果缓冲长度公式（必记）：
 *      BUF_LEN = ChannelCount * StreamingNumSamples
 *      例：1 ch + Single(samples=1)           -> [1]
 *          1 ch + Streaming samples=8         -> [8]
 *          3 ch + Streaming samples=4         -> [12]
 *====================================================================================================================*/

/*--------------------------------------------------------------------------------------------------------------------
 * 二、配置填空区：DaVinci 生成后从 Adc_CfgDefines.h 抄过来
 *--------------------------------------------------------------------------------------------------------------------*/
#if 0 /* 生成并确认宏名后改为 1，或拷到 CtLedTask.c 后打开 */

#ifndef ADC_PRACTICE_CASE
#define ADC_PRACTICE_CASE   1   /* 1~6 见下方场景 */
#endif

/* TODO: 生成后的 Group 符号名，当前工程示例是 AdcConf_AdcGroup_AdcGroup */
#define ADC_PRACTICE_GROUP          AdcConf_AdcGroup_AdcGroup

/* TODO: 与 DaVinci 中该 Group 的 Channel 个数、StreamingNumSamples 保持一致 */
#define ADC_PRACTICE_CH_COUNT       (1u)
#define ADC_PRACTICE_STREAM_SAMPLES (1u)   /* Single 填 1；Streaming 填 N */

#define ADC_PRACTICE_BUF_LEN        (ADC_PRACTICE_CH_COUNT * ADC_PRACTICE_STREAM_SAMPLES)

static Adc_ValueGroupType AdcPractice_ResultBuffer[ADC_PRACTICE_BUF_LEN];
static Adc_ValueGroupType AdcPractice_ReadBuffer[ADC_PRACTICE_BUF_LEN];

/*--------------------------------------------------------------------------------------------------------------------
 * 三、公共 Init：所有场景都要 SetupResultBuffer；要回调再 EnableNotification
 *--------------------------------------------------------------------------------------------------------------------*/
void AdcPractice_Init(void)
{
  (void)Adc_SetupResultBuffer(ADC_PRACTICE_GROUP, AdcPractice_ResultBuffer);

#if (ADC_GRP_NOTIF_CAPABILITY == STD_ON)
  /* 轮询场景可注释下一行 */
  Adc_EnableGroupNotification(ADC_PRACTICE_GROUP);
#endif
}

/*--------------------------------------------------------------------------------------------------------------------
 * 四、场景框架（只保留一种参与链接；用 ADC_PRACTICE_CASE 切换）
 *
 *  CASE 1: SW + OneShot + Single          —— 你现在最接近的配置
 *  CASE 2: SW + Continuous + Single       —— Start 一次持续转，需 Stop 释放
 *  CASE 3: SW + OneShot + Streaming Linear—— 攒满 N 次采样后完成
 *  CASE 4: SW + Continuous + Streaming Circular —— 环形流，需 Stop 才停
 *  CASE 5: HW + OneShot + Single          —— 需 AdcHwTriggerApi=ON
 *  CASE 6: 无回调轮询读                   —— 配合同一 Group，用状态机读
 *--------------------------------------------------------------------------------------------------------------------*/

#if (ADC_PRACTICE_CASE == 1)
/*
 * DaVinci:
 *   TriggSrc=SW, ConvMode=ONESHOT, AccessMode=SINGLE, StreamingNumSamples=1
 * 应用节奏:
 *   周期任务里 Start；转换完进 Notification（或轮询 COMPLETED）再 ReadGroup；
 *   OneShot 完成后 Group 回到 idle，下次要再测必须再 Start。
 */
void AdcPractice_Trigger(void)
{
  if (Adc_GetGroupStatus(ADC_PRACTICE_GROUP) == ADC_IDLE)
  {
    Adc_StartGroupConversion(ADC_PRACTICE_GROUP);
  }
}

void AdcPractice_Notification(void)
{
  (void)Adc_ReadGroup(ADC_PRACTICE_GROUP, AdcPractice_ReadBuffer);
  /* TODO: 使用 AdcPractice_ReadBuffer[0 .. CH_COUNT-1] */
}

#elif (ADC_PRACTICE_CASE == 2)
/*
 * DaVinci:
 *   TriggSrc=SW, ConvMode=CONTINUOUS, AccessMode=SINGLE, samples=1
 * 应用节奏:
 *   Init 后（或首次）Start 一次即可连续转换；
 *   Notification 会反复进，或周期任务里 ReadGroup；
 *   不用时必须 StopGroupConversion，否则占着硬件/队列。
 */
void AdcPractice_Trigger(void)
{
  static uint8 started = 0u;
  if (started == 0u)
  {
    Adc_StartGroupConversion(ADC_PRACTICE_GROUP);
    started = 1u;
  }
}

void AdcPractice_Stop(void)
{
  Adc_StopGroupConversion(ADC_PRACTICE_GROUP);
}

void AdcPractice_Notification(void)
{
  (void)Adc_ReadGroup(ADC_PRACTICE_GROUP, AdcPractice_ReadBuffer);
}

#elif (ADC_PRACTICE_CASE == 3)
/*
 * DaVinci:
 *   TriggSrc=SW, ConvMode=ONESHOT, AccessMode=STREAMING,
 *   StreamingBufferMode=LINEAR, StreamingNumSamples=N (N>1)
 *   BUF_LEN = CH * N
 * 应用节奏:
 *   Start 一次后，驱动连续采满 N 组样本（Linear 满了结束）；
 *   完成状态多为 ADC_STREAM_COMPLETED；
 *   读法 A: ReadGroup 读「当前最新一组」通道值
 *   读法 B: GetStreamLastPointer 拿最后有效采样位置，再按环形/线性布局解析整缓冲
 */
void AdcPractice_Trigger(void)
{
  if (Adc_GetGroupStatus(ADC_PRACTICE_GROUP) == ADC_IDLE)
  {
    Adc_StartGroupConversion(ADC_PRACTICE_GROUP);
  }
}

void AdcPractice_Notification(void)
{
  Adc_ValueGroupType *samplePtr = NULL_PTR;
  Adc_StreamNumSampleType validSamples;

  validSamples = Adc_GetStreamLastPointer(ADC_PRACTICE_GROUP, &samplePtr);
  (void)validSamples;
  (void)samplePtr;
  /* TODO: 按 [sample][channel] 布局解析 AdcPractice_ResultBuffer */

  /* 若只关心最新一组通道： */
  (void)Adc_ReadGroup(ADC_PRACTICE_GROUP, AdcPractice_ReadBuffer);
}

#elif (ADC_PRACTICE_CASE == 4)
/*
 * DaVinci:
 *   TriggSrc=SW, ConvMode=CONTINUOUS, AccessMode=STREAMING,
 *   StreamingBufferMode=CIRCULAR, StreamingNumSamples=N
 * 应用节奏:
 *   Start 后一直转，缓冲环形覆盖；Notification 可能在半满/满等条件进（看 MCAL/配置）；
 *   用 GetStreamLastPointer 找最新采样；结束时 Stop。
 */
void AdcPractice_Trigger(void)
{
  static uint8 started = 0u;
  if (started == 0u)
  {
    Adc_StartGroupConversion(ADC_PRACTICE_GROUP);
    started = 1u;
  }
}

void AdcPractice_Stop(void)
{
  Adc_StopGroupConversion(ADC_PRACTICE_GROUP);
}

void AdcPractice_Notification(void)
{
  Adc_ValueGroupType *samplePtr = NULL_PTR;
  (void)Adc_GetStreamLastPointer(ADC_PRACTICE_GROUP, &samplePtr);
  (void)samplePtr;
}

#elif (ADC_PRACTICE_CASE == 5)
/*
 * DaVinci:
 *   AdcHwTriggerApi = ON
 *   TriggSrc=HW, ConvMode=ONESHOT, AccessMode=SINGLE
 *   配好 HwTriggerSource / 触发引脚或 PDB/TRGMUX（按板级）
 * 应用节奏:
 *   不调用 StartGroupConversion；
 *   EnableHardwareTrigger 后等外部沿触发；
 *   不用时 DisableHardwareTrigger。
 */
void AdcPractice_Trigger(void)
{
#if (ADC_HW_TRIGGER_API == STD_ON)
  Adc_EnableHardwareTrigger(ADC_PRACTICE_GROUP);
#else
  /* TODO: DaVinci 打开 AdcHwTriggerApi 后重新生成 */
#endif
}

void AdcPractice_Stop(void)
{
#if (ADC_HW_TRIGGER_API == STD_ON)
  Adc_DisableHardwareTrigger(ADC_PRACTICE_GROUP);
#endif
}

void AdcPractice_Notification(void)
{
  (void)Adc_ReadGroup(ADC_PRACTICE_GROUP, AdcPractice_ReadBuffer);
}

#elif (ADC_PRACTICE_CASE == 6)
/*
 * 任意 SW Group，不依赖 Notification（DaVinci 可不配回调，或代码里不 Enable）
 * 周期任务轮询状态再读。
 */
void AdcPractice_Trigger(void)
{
  if (Adc_GetGroupStatus(ADC_PRACTICE_GROUP) == ADC_IDLE)
  {
    Adc_StartGroupConversion(ADC_PRACTICE_GROUP);
  }
}

void AdcPractice_Poll(void)
{
  Adc_StatusType st = Adc_GetGroupStatus(ADC_PRACTICE_GROUP);

  /* OneShot+Single 完成: ADC_STREAM_COMPLETED 或 ADC_COMPLETED（视实现/配置） */
  if ((st == ADC_STREAM_COMPLETED) || (st == ADC_COMPLETED))
  {
    (void)Adc_ReadGroup(ADC_PRACTICE_GROUP, AdcPractice_ReadBuffer);
    /* Streaming 也可用 GetStreamLastPointer */
  }
}

#endif /* ADC_PRACTICE_CASE */

/*--------------------------------------------------------------------------------------------------------------------
 * 五、挂到 CtLedTask 的推荐写法（填空示意，勿直接原样编译）
 *
 *  CtLedTask_InitRunnable:
 *      AdcPractice_Init();
 *
 *  LedRunnable (500ms):
 *      AdcPractice_Trigger();          // CASE1/3/6 每次或按状态 Start
 *      // AdcPractice_Poll();          // 仅 CASE6
 *
 *  回调函数名必须与 DaVinci AdcNotification 字符串一致:
 *      void AdcGroup0Notification(void)
 *      {
 *          AdcPractice_Notification();
 *      }
 *--------------------------------------------------------------------------------------------------------------------*/

#endif /* 填空区总开关 */
