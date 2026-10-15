/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *          File:  CtLedTask.c
 *        Config:  D:/PANDORA/CarCode/AUTOSAR_TOOLs/S32K144_SIP/MICROSAR/CBD1800257_D01_S32K1xx/Applications/S32K144_Start_new_IAR/S32K144_Start.dpa
 *     SW-C Type:  CtLedTask
 *  Generated at:  Tue Oct 13 13:22:46 2026
 *
 *     Generator:  MICROSAR RTE Generator Version 4.19.0
 *                 RTE Core Version 1.19.0
 *       License:  CBD1800257
 *
 *   Description:  C-Code implementation template for SW-C <CtLedTask>
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of version logging area >>                DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/

/* PRQA S 0777, 0779 EOF */ /* MD_MSR_5.1_777, MD_MSR_5.1_779 */

/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of version logging area >>                  DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *
 * AUTOSAR Modelling Object Descriptions
 *
 **********************************************************************************************************************
 *
 * Data Types:
 * ===========
 * ComM_ModeType
 *   uint8 represents integers with a minimum value of 0 and a maximum value of 255.
 *      The order-relation on uint8 is: x < y if y - x is positive.
 *      uint8 has a lexical representation consisting of a finite-length sequence 
 *      of decimal digits (#x30-#x39).
 *      
 *      For example: 1, 0, 126, +10.
 *
 *********************************************************************************************************************/

#include "Rte_CtLedTask.h" /* PRQA S 0857 */ /* MD_MSR_1.1_857 */


/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of include and declaration area >>        DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/

#include "Dio.h"
#include "Adc.h"
#include "Pwm.h"
#include "Spi.h"
//#include "Adc_CfgDefines.h"
/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of include and declaration area >>          DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *
 * Used AUTOSAR Data Types
 *
 **********************************************************************************************************************
 *
 * Primitive Types:
 * ================
 * boolean: Boolean (standard type)
 * uint16: Integer in interval [0...65535] (standard type)
 * uint8: Integer in interval [0...255] (standard type)
 *
 * Enumeration Types:
 * ==================
 * ComM_ModeType: Enumeration of integer in interval [0...3] with enumerators
 *   COMM_NO_COMMUNICATION (0U)
 *   COMM_SILENT_COMMUNICATION (1U)
 *   COMM_FULL_COMMUNICATION (2U)
 *
 *********************************************************************************************************************/


#define CtLedTask_START_SEC_CODE
#include "CtLedTask_MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

/**********************************************************************************************************************
 *
 * Runnable Entity Name: CtLedTask_InitRunnable
 *
 *---------------------------------------------------------------------------------------------------------------------
 *
 * Executed once after the RTE is started
 *
 **********************************************************************************************************************
 *
 * Service Calls:
 * ==============
 *   Service Invocation:
 *   -------------------
 *   Std_ReturnType Rte_Call_UR_CN_CAN00_06ecbb07_GetCurrentComMode(ComM_ModeType *ComMode)
 *     Synchronous Service Invocation. Timeout: None
 *     Returned Application Errors: RTE_E_ComM_UserRequest_E_NOT_OK
 *   Std_ReturnType Rte_Call_UR_CN_CAN00_06ecbb07_GetMaxComMode(ComM_ModeType *ComMode)
 *     Synchronous Service Invocation. Timeout: None
 *     Returned Application Errors: RTE_E_ComM_UserRequest_E_NOT_OK
 *   Std_ReturnType Rte_Call_UR_CN_CAN00_06ecbb07_GetRequestedComMode(ComM_ModeType *ComMode)
 *     Synchronous Service Invocation. Timeout: None
 *     Returned Application Errors: RTE_E_ComM_UserRequest_E_NOT_OK
 *   Std_ReturnType Rte_Call_UR_CN_CAN00_06ecbb07_RequestComMode(ComM_ModeType ComMode)
 *     Synchronous Service Invocation. Timeout: None
 *     Returned Application Errors: RTE_E_ComM_UserRequest_E_MODE_LIMITATION, RTE_E_ComM_UserRequest_E_NOT_OK
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of documentation area >>                    DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/

FUNC(void, CtLedTask_CODE) CtLedTask_InitRunnable(void) /* PRQA S 0850 */ /* MD_MSR_19.8 */
{
/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of runnable implementation >>             DO NOT CHANGE THIS COMMENT!
 * Symbol: CtLedTask_InitRunnable
 *********************************************************************************************************************/
  static uint16 ResultBuffer[1] = {0}; 
  Adc_SetupResultBuffer(AdcConf_AdcGroup_AdcGroup0, ResultBuffer);
  Adc_EnableGroupNotification(AdcConf_AdcGroup_AdcGroup0);
  

  Rte_Call_UR_CN_CAN00_06ecbb07_RequestComMode(COMM_FULL_COMMUNICATION);

/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of runnable implementation >>               DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/
}

/**********************************************************************************************************************
 *
 * Runnable Entity Name: LedRunnable
 *
 *---------------------------------------------------------------------------------------------------------------------
 *
 * Executed if at least one of the following trigger conditions occurred:
 *   - triggered on TimingEvent every 300ms
 *
 **********************************************************************************************************************
 *
 * Input Interfaces:
 * =================
 *   Explicit S/R API:
 *   -----------------
 *   Std_ReturnType Rte_Read_CtLedTask_Recive_Test_Signal_u16_Siganl(uint16 *data)
 *   Std_ReturnType Rte_Read_Rear_LeftWindowPosition_u8_signal(uint8 *data)
 *   Std_ReturnType Rte_Read_Rear_RightWindowPosition_u8_signal(uint8 *data)
 *
 * Output Interfaces:
 * ==================
 *   Explicit S/R API:
 *   -----------------
 *   Std_ReturnType Rte_Write_RearInterLight_Bool_signal(boolean data)
 *   Std_ReturnType Rte_Write_Test_Signal_u16_Siganl(uint16 data)
 *
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of documentation area >>                  DO NOT CHANGE THIS COMMENT!
 * Symbol: LedRunnable_doc
 *********************************************************************************************************************/
//static 

/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of documentation area >>                    DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/

FUNC(void, CtLedTask_CODE) LedRunnable(void) /* PRQA S 0850 */ /* MD_MSR_19.8 */
{
/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of runnable implementation >>             DO NOT CHANGE THIS COMMENT!
 * Symbol: LedRunnable
 *********************************************************************************************************************/

//static unsigned char  LedState=0;
//static int  LedCnt=0;

//static unsigned  char Rear_LeftWindowPosition;
//static unsigned char Rear_RightWindowPosition;
//static unsigned char My_GroupSignal = 70;
//static unsigned char My_GroupSignal1 = 254;

  //spi练习

   static Spi_DataBufferType Send[8]={0u};
   static Spi_DataBufferType Receive[8]={0u};
   static Std_ReturnType result;
   uint32 resultCnt=0;
   Send[0] = 0xFF;
   Send[1] = 0x12;
   Send[2] = 0x34;
   Send[3] = 0x56;
   
   Send[4] = 0x78;
   Send[5] = 0x90;
   Send[6] = 0xAA;
   Send[7] = 0xFF;
   result = Spi_SetupEB(SpiConf_SpiChannel_SpiChannel_Test_Read,Send,Receive,8);
   if(result == E_OK)
  {
      resultCnt++;
  }
  result = Spi_SyncTransmit(SpiConf_SpiSequence_SpiSequence_1_TEST);
      if(result == E_OK)
  {
      resultCnt++;
  }
   
//LedCnt++;
//
//LedState ^= 0x01;


//Rte_Write_CtLedTask_RearInterLight_Bool_signal(1);
//Rte_Read_Rear_LeftWindowPosition_u8_signal(&Rear_LeftWindowPosition);
//Rte_Read_Rear_RightWindowPosition_u8_signal(&Rear_RightWindowPosition);
//Com_SendSignal(ComConf_ComGroupSignal_My_ComGroupSignal,&My_GroupSignal);
//Com_SendSignal(ComConf_ComGroupSignal_My_ComGroupSignal1,&My_GroupSignal1);
//Com_SendSignalGroup(ComConf_ComSignalGroup_My_SignalGroup);
static uint16 swc12ctledtask  = 0;
Rte_Read_CtLedTask_Recive_Test_Signal_u16_Siganl(&swc12ctledtask);//接收来自swc1的 test signal
Rte_Write_CtLedTask_Test_Signal_u16_Siganl(swc12ctledtask);//并通过can发送出去

//Adc_StartGroupConversion(AdcConf_AdcGroup_AdcGroup0);//
// Dio_WriteChannel(112,LedState);




/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of runnable implementation >>               DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/
}


#define CtLedTask_STOP_SEC_CODE
#include "CtLedTask_MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */


/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of function definition area >>            DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/
void AdcGroup0Notification(void)
{
   uint16 adcValue = 0u;
   (void)Adc_ReadGroup(AdcConf_AdcGroup_AdcGroup0, &adcValue);
   
   Pwm_SetDutyCycle(PwmConf_PwmChannelConfigSet_PwmChannel0,adcValue*8);
   
   
   Rte_Write_CtLedTask_Test_Signal_u16_Siganl((uint16)(adcValue / 10u));
 
  
}

/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of function definition area >>              DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of removed code area >>                   DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of removed code area >>                     DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/
