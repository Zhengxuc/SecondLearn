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
 *          File:  Rte_Test_Swc0.h
 *        Config:  D:/PANDORA/CarCode/AUTOSAR_TOOLs/S32K144_SIP/MICROSAR/CBD1800257_D01_S32K1xx/Applications/S32K144_Start_new_IAR/S32K144_Start.dpa
 *     SW-C Type:  Test_Swc0
 *  Generated at:  Tue Oct 13 13:22:44 2026
 *
 *     Generator:  MICROSAR RTE Generator Version 4.19.0
 *                 RTE Core Version 1.19.0
 *       License:  CBD1800257
 *
 *   Description:  Application header file for SW-C <Test_Swc0> (Contract Phase)
 *********************************************************************************************************************/

/* double include prevention */
#ifndef _RTE_TEST_SWC0_H
# define _RTE_TEST_SWC0_H

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

# include "Rte_Test_Swc0_Type.h"
# include "Rte_DataHandleType.h"


/**********************************************************************************************************************
 * Component Data Structures and Port Data Structures
 *********************************************************************************************************************/

struct Rte_CDS_Test_Swc0
{
  /* dummy entry */
  uint8 _dummy;
};

# define RTE_START_SEC_CONST_UNSPECIFIED
# include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

extern CONSTP2CONST(struct Rte_CDS_Test_Swc0, RTE_CONST, RTE_CONST) Rte_Inst_Test_Swc0; /* PRQA S 0850 */ /* MD_MSR_19.8 */

# define RTE_STOP_SEC_CONST_UNSPECIFIED
# include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

typedef P2CONST(struct Rte_CDS_Test_Swc0, TYPEDEF, RTE_CONST) Rte_Instance;


/**********************************************************************************************************************
 * Init Values for unqueued S/R communication (primitive types only)
 *********************************************************************************************************************/

# define Rte_InitValue_Test_Swc0_Read_u8_Signal (0U)
# define Rte_InitValue_Test_Swc0_Write_u8_Signal (0U)


# define RTE_START_SEC_CODE
# include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

/**********************************************************************************************************************
 * API prototypes
 *********************************************************************************************************************/
FUNC(Std_ReturnType, RTE_CODE) Rte_Read_Test_Swc0_Test_Swc0_Read_u8_Signal(P2VAR(uint8, AUTOMATIC, RTE_TEST_SWC0_APPL_VAR) data); /* PRQA S 0850 */ /* MD_MSR_19.8 */
FUNC(Std_ReturnType, RTE_CODE) Rte_Write_Test_Swc0_Test_Swc0_Write_u8_Signal(uint8 data); /* PRQA S 0850 */ /* MD_MSR_19.8 */

# define RTE_STOP_SEC_CODE
# include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */



/**********************************************************************************************************************
 * Rte_Read_<p>_<d> (explicit S/R communication with isQueued = false)
 *********************************************************************************************************************/
# define Rte_Read_Test_Swc0_Read_u8_Signal Rte_Read_Test_Swc0_Test_Swc0_Read_u8_Signal


/**********************************************************************************************************************
 * Rte_Write_<p>_<d> (explicit S/R communication with isQueued = false)
 *********************************************************************************************************************/
# define Rte_Write_Test_Swc0_Write_u8_Signal Rte_Write_Test_Swc0_Test_Swc0_Write_u8_Signal




# define Test_Swc0_START_SEC_CODE
# include "Test_Swc0_MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */


/**********************************************************************************************************************
 *
 * Runnable Entity Name: Swc_Service_Operation
 *
 *---------------------------------------------------------------------------------------------------------------------
 *
 * Executed if at least one of the following trigger conditions occurred:
 *   - triggered by server invocation for OperationPrototype <Operation> of PortPrototype <Swc_Service>
 *
 **********************************************************************************************************************
 *
 * Runnable prototype:
 * ===================
 *   void Swc_Service_Operation(uint8 argIn, uint8 *argOut)
 *
 *********************************************************************************************************************/

# define RTE_RUNNABLE_Swc_Service_Operation Swc_Service_Operation
FUNC(void, Test_Swc0_CODE) Swc_Service_Operation(uint8 argIn, P2VAR(uint8, AUTOMATIC, RTE_TEST_SWC0_APPL_VAR) argOut); /* PRQA S 0850 */ /* MD_MSR_19.8 */

/**********************************************************************************************************************
 *
 * Runnable Entity Name: Test_Swc0_Init
 *
 *---------------------------------------------------------------------------------------------------------------------
 *
 * Executed once after the RTE is started
 *
 *********************************************************************************************************************/

# define RTE_RUNNABLE_Test_Swc0_Init Test_Swc0_Init
FUNC(void, Test_Swc0_CODE) Test_Swc0_Init(void); /* PRQA S 0850 */ /* MD_MSR_19.8 */

/**********************************************************************************************************************
 *
 * Runnable Entity Name: Test_Swc0_Main
 *
 *---------------------------------------------------------------------------------------------------------------------
 *
 * Executed if at least one of the following trigger conditions occurred:
 *   - triggered on TimingEvent every 50ms
 *
 **********************************************************************************************************************
 *
 * Input Interfaces:
 * =================
 *   Explicit S/R API:
 *   -----------------
 *   Std_ReturnType Rte_Read_Test_Swc0_Read_u8_Signal(uint8 *data)
 *
 * Output Interfaces:
 * ==================
 *   Explicit S/R API:
 *   -----------------
 *   Std_ReturnType Rte_Write_Test_Swc0_Write_u8_Signal(uint8 data)
 *
 *********************************************************************************************************************/

# define RTE_RUNNABLE_Test_Swc0_Main Test_Swc0_Main
FUNC(void, Test_Swc0_CODE) Test_Swc0_Main(void); /* PRQA S 0850 */ /* MD_MSR_19.8 */

# define Test_Swc0_STOP_SEC_CODE
# include "Test_Swc0_MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

# ifdef __cplusplus
} /* extern "C" */
# endif /* __cplusplus */

#endif /* _RTE_TEST_SWC0_H */
