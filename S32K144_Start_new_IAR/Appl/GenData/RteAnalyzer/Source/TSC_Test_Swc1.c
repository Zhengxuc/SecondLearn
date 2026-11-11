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
 *          File:  TSC_Test_Swc1.c
 *        Config:  S32K144_Start.dpa
 *   ECU-Project:  MyECU
 *
 *     Generator:  MICROSAR RTE Generator Version 4.19.0
 *                 RTE Core Version 1.19.0
 *       License:  CBD1800257
 *
 *   Description:  Implementation of wrapper software component for Bte-based Rte test cases
 *********************************************************************************************************************/
#include "Rte_Test_Swc1.h"
#include "TSC_Test_Swc1.h"















     /* Client Server Interfaces: */


     /* Service calls */


     /* Mode Interfaces */




     /* Trigger Interfaces */

     /* Inter-Runnable variables */






Std_ReturnType TSC_Test_Swc1_Rte_Read_Test_Swc1_Read_u8_Signal(uint8 *data)
{
  return Rte_Read_Test_Swc1_Read_u8_Signal(data);
}




Std_ReturnType TSC_Test_Swc1_Rte_Write_Test_Swc1_Send_Test_Signal_u16_Siganl(uint16 data)
{
  return Rte_Write_Test_Swc1_Send_Test_Signal_u16_Siganl(data);
}

Std_ReturnType TSC_Test_Swc1_Rte_Write_Test_Swc1_Write_u8_Signal(uint8 data)
{
  return Rte_Write_Test_Swc1_Write_u8_Signal(data);
}





     /* Client Server Interfaces: */
Std_ReturnType TSC_Test_Swc1_Rte_Call_Swc_Service_Operation(uint8 argIn, uint8 *argOut)
{
  return Rte_Call_Swc_Service_Operation(argIn, argOut);
}


     /* Service calls */


     /* Mode Interfaces */




     /* Trigger Interfaces */

     /* Inter-Runnable variables */







     /* Test_Swc1 */
      /* Test_Swc1 */



