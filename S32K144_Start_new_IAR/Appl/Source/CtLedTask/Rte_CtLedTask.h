/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *          File:  Rte_CtLedTask.h
 *        Config:  D:/PANDORA/CarCode/AUTOSAR_TOOLs/S32K144_SIP/MICROSAR/CBD1800257_D01_S32K1xx/Applications/S32K144_Start_new_IAR/S32K144_Start.dpa
 *     SW-C Type:  CtLedTask
 *  Generated at:  Tue Oct 13 13:22:44 2026
 *
 *     Generator:  MICROSAR RTE Generator Version 4.19.0
 *                 RTE Core Version 1.19.0
 *       License:  CBD1800257
 *
 *   Description:  Application header file for SW-C <CtLedTask> (Contract Phase)
 *********************************************************************************************************************/

/* double include prevention */
#ifndef _RTE_CTLEDTASK_H
# define _RTE_CTLEDTASK_H

# ifdef RTE_APPLICATION_HEADER_FILE
#  error Multiple application header files included.
# endif
# define RTE_APPLICATION_HEADER_FILE
# ifndef RTE_PTR2ARRAYBASETYPE_PASSING
#  define RTE_PTR2ARRAYBASETYPE_PASSING
# endif

# ifdef __cplusplus
extern "C"
{
# endif /* __cplusplus */

/* include files */

# include "Rte_CtLedTask_Type.h"
# include "Rte_DataHandleType.h"


/**********************************************************************************************************************
 * Component Data Structures and Port Data Structures
 *********************************************************************************************************************/

struct Rte_CDS_CtLedTask
{
  /* dummy entry */
  uint8 _dummy;
};

# define RTE_START_SEC_CONST_UNSPECIFIED
# include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

extern CONSTP2CONST(struct Rte_CDS_CtLedTask, RTE_CONST, RTE_CONST) Rte_Inst_CtLedTask; /* PRQA S 0850 */ /* MD_MSR_19.8 */

# define RTE_STOP_SEC_CONST_UNSPECIFIED
# include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

typedef P2CONST(struct Rte_CDS_CtLedTask, TYPEDEF, RTE_CONST) Rte_Instance;


/**********************************************************************************************************************
 * Init Values for unqueued S/R communication (primitive types only)
 *********************************************************************************************************************/

# define Rte_InitValue_CtLedTask_Recive_Test_Signal_u16_Siganl (0U)
# define Rte_InitValue_RearInterLight_Bool_signal (FALSE)
# define Rte_InitValue_Rear_LeftWindowPosition_u8_signal (0U)
# define Rte_InitValue_Rear_RightWindowPosition_u8_signal (0U)
# define Rte_InitValue_Test_Signal_u16_Siganl (0U)


# define RTE_START_SEC_CODE
# include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

/**********************************************************************************************************************
 * API prototypes
 *********************************************************************************************************************/
FUNC(Std_ReturnType, RTE_CODE) Rte_Read_CtLedTask_CtLedTask_Recive_Test_Signal_u16_Siganl(P2VAR(uint16, AUTOMATIC, RTE_CTLEDTASK_APPL_VAR) data); /* PRQA S 0850 */ /* MD_MSR_19.8 */
FUNC(Std_ReturnType, RTE_CODE) Rte_Read_CtLedTask_Rear_LeftWindowPosition_u8_signal(P2VAR(uint8, AUTOMATIC, RTE_CTLEDTASK_APPL_VAR) data); /* PRQA S 0850 */ /* MD_MSR_19.8 */
FUNC(Std_ReturnType, RTE_CODE) Rte_Read_CtLedTask_Rear_RightWindowPosition_u8_signal(P2VAR(uint8, AUTOMATIC, RTE_CTLEDTASK_APPL_VAR) data); /* PRQA S 0850 */ /* MD_MSR_19.8 */
FUNC(Std_ReturnType, RTE_CODE) Rte_Write_CtLedTask_RearInterLight_Bool_signal(boolean data); /* PRQA S 0850 */ /* MD_MSR_19.8 */
FUNC(Std_ReturnType, RTE_CODE) Rte_Write_CtLedTask_Test_Signal_u16_Siganl(uint16 data); /* PRQA S 0850 */ /* MD_MSR_19.8 */
FUNC(Std_ReturnType, RTE_CODE) Rte_Call_CtLedTask_UR_CN_CAN00_06ecbb07_GetCurrentComMode(P2VAR(ComM_ModeType, AUTOMATIC, RTE_CTLEDTASK_APPL_VAR) ComMode); /* PRQA S 0850 */ /* MD_MSR_19.8 */
FUNC(Std_ReturnType, RTE_CODE) Rte_Call_CtLedTask_UR_CN_CAN00_06ecbb07_GetMaxComMode(P2VAR(ComM_ModeType, AUTOMATIC, RTE_CTLEDTASK_APPL_VAR) ComMode); /* PRQA S 0850 */ /* MD_MSR_19.8 */
FUNC(Std_ReturnType, RTE_CODE) Rte_Call_CtLedTask_UR_CN_CAN00_06ecbb07_GetRequestedComMode(P2VAR(ComM_ModeType, AUTOMATIC, RTE_CTLEDTASK_APPL_VAR) ComMode); /* PRQA S 0850 */ /* MD_MSR_19.8 */
FUNC(Std_ReturnType, RTE_CODE) Rte_Call_CtLedTask_UR_CN_CAN00_06ecbb07_RequestComMode(ComM_ModeType ComMode); /* PRQA S 0850 */ /* MD_MSR_19.8 */

# define RTE_STOP_SEC_CODE
# include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */



/**********************************************************************************************************************
 * Rte_Read_<p>_<d> (explicit S/R communication with isQueued = false)
 *********************************************************************************************************************/
# define Rte_Read_CtLedTask_Recive_Test_Signal_u16_Siganl Rte_Read_CtLedTask_CtLedTask_Recive_Test_Signal_u16_Siganl
# define Rte_Read_Rear_LeftWindowPosition_u8_signal Rte_Read_CtLedTask_Rear_LeftWindowPosition_u8_signal
# define Rte_Read_Rear_RightWindowPosition_u8_signal Rte_Read_CtLedTask_Rear_RightWindowPosition_u8_signal


/**********************************************************************************************************************
 * Rte_Write_<p>_<d> (explicit S/R communication with isQueued = false)
 *********************************************************************************************************************/
# define Rte_Write_RearInterLight_Bool_signal Rte_Write_CtLedTask_RearInterLight_Bool_signal
# define Rte_Write_Test_Signal_u16_Siganl Rte_Write_CtLedTask_Test_Signal_u16_Siganl


/**********************************************************************************************************************
 * Rte_Call_<p>_<o> (C/S invocation)
 *********************************************************************************************************************/
# define Rte_Call_UR_CN_CAN00_06ecbb07_GetCurrentComMode Rte_Call_CtLedTask_UR_CN_CAN00_06ecbb07_GetCurrentComMode
# define Rte_Call_UR_CN_CAN00_06ecbb07_GetMaxComMode Rte_Call_CtLedTask_UR_CN_CAN00_06ecbb07_GetMaxComMode
# define Rte_Call_UR_CN_CAN00_06ecbb07_GetRequestedComMode Rte_Call_CtLedTask_UR_CN_CAN00_06ecbb07_GetRequestedComMode
# define Rte_Call_UR_CN_CAN00_06ecbb07_RequestComMode Rte_Call_CtLedTask_UR_CN_CAN00_06ecbb07_RequestComMode




# define CtLedTask_START_SEC_CODE
# include "CtLedTask_MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */


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

# define RTE_RUNNABLE_CtLedTask_InitRunnable CtLedTask_InitRunnable
FUNC(void, CtLedTask_CODE) CtLedTask_InitRunnable(void); /* PRQA S 0850 */ /* MD_MSR_19.8 */

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

# define RTE_RUNNABLE_LedRunnable LedRunnable
FUNC(void, CtLedTask_CODE) LedRunnable(void); /* PRQA S 0850 */ /* MD_MSR_19.8 */

# define CtLedTask_STOP_SEC_CODE
# include "CtLedTask_MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */


/**********************************************************************************************************************
 * Application errors
 *********************************************************************************************************************/

# define RTE_E_ComM_UserRequest_E_MODE_LIMITATION (2U)

# define RTE_E_ComM_UserRequest_E_NOT_OK (1U)

# ifdef __cplusplus
} /* extern "C" */
# endif /* __cplusplus */

#endif /* _RTE_CTLEDTASK_H */
