/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *
 *                 This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                 Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                 All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  LICENSE
 *  -------------------------------------------------------------------------------------------------------------------
 *            Module: Com
 *           Program: MSR_Vector_SLP4
 *          Customer: Harman International (China) Holdings Co., Ltd.
 *       Expiry Date: Not restricted
 *  Ordered Derivat.: FS32K148UAT0VLQR
 *    License Scope : The usage is restricted to CBD1800257_D01
 *
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *              File: Com_Lcfg.c
 *   Generation Time: 2026-09-29 10:28:18
 *           Project: S32K144_Start - Version 1.0
 *          Delivery: CBD1800257_D01
 *      Tool Version: DaVinci Configurator  5.18.37 SP1
 *
 *
 *********************************************************************************************************************/


/**********************************************************************************************************************
  MISRA / PClint JUSTIFICATIONS
**********************************************************************************************************************/
/* PRQA  S 1881 EOF */ /* MD_MSR_AutosarBoolean */
/* PRQA  S 1882 EOF */ /* MD_MSR_AutosarBoolean */

/**********************************************************************************************************************
  INCLUDES
**********************************************************************************************************************/
#define V_IL_ASRCOMCFG5_LCFG_SOURCE

#include "Com.h"

#include "Com_Lcfg.h"


#include "SchM_Com.h"

/**********************************************************************************************************************
  LOCAL CONSTANT MACROS
**********************************************************************************************************************/

/**********************************************************************************************************************
  LOCAL FUNCTION MACROS
**********************************************************************************************************************/

/**********************************************************************************************************************
  LOCAL DATA TYPES AND STRUCTURES
**********************************************************************************************************************/

/**********************************************************************************************************************
  LOCAL DATA PROTOTYPES
**********************************************************************************************************************/
/**********************************************************************************************************************
  CONFIGURATION CLASS: PRE_COMPILE
  SECTION: LOCAL DATA TYPES AND STRUCTURES
**********************************************************************************************************************/

/**********************************************************************************************************************
  CONFIGURATION CLASS: LINK
  SECTION: LOCAL DATA TYPES AND STRUCTURES
**********************************************************************************************************************/


/**********************************************************************************************************************
  LOCAL DATA
**********************************************************************************************************************/
/**********************************************************************************************************************
  CONFIGURATION CLASS: PRE_COMPILE
  SECTION: LOCAL DATA
**********************************************************************************************************************/

/**********************************************************************************************************************
  CONFIGURATION CLASS: LINK
  SECTION: LOCAL DATA
**********************************************************************************************************************/


/**********************************************************************************************************************
  GLOBAL DATA
**********************************************************************************************************************/
/**********************************************************************************************************************
  CONFIGURATION CLASS: PRE_COMPILE
  SECTION: GLOBAL DATA
**********************************************************************************************************************/
/**********************************************************************************************************************
  Com_ConstValueUInt8
**********************************************************************************************************************/
/** 
  \var    Com_ConstValueUInt8
  \brief  Optimized array of commonly used values like initial or invalid values. (BOOLEAN, UINT8)
*/ 
#define COM_START_SEC_CONST_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_ConstValueUInt8Type, COM_CONST) Com_ConstValueUInt8[1] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index     ConstValueUInt8      Referable Keys */
  /*     0 */            0x00u   /* [/ActiveEcuC/Com/ComConfig/sig_RearLeftWindowPosition_omsg_Receive2_oCAN00_89d4d489_RxInitValue, /ActiveEcuC/Com/ComConfig/sig_RearRightWindowPosition_omsg_Receive2_oCAN00_94575df2_RxInitValue, /ActiveEcuC/Com/ComConfig/sig_State_RearInteriorLight_omsg_Receive_oCAN00_63323183_RxInitValue, /ActiveEcuC/Com/ComConfig/sig_State_RearLeftDoor_omsg_Receive_oCAN00_84ad4140_RxInitValue, /ActiveEcuC/Com/ComConfig/sig_State_RearRightDoor_omsg_Receive_oCAN00_a8945098_RxInitValue] */
};
#define COM_STOP_SEC_CONST_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RxAccessInfo
**********************************************************************************************************************/
/** 
  \var    Com_RxAccessInfo
  \brief  Contains all signal layout information necessary for signal access within an I-PDU.
  \details
  Element                   Description
  InitValueUsed             TRUE, if the 0:1 relation has minimum 1 relation pointing to Com_ConstValueUInt8,Com_ConstValueUInt16,Com_ConstValueUInt32,Com_ConstValueUInt64,Com_ConstValueSInt8,Com_ConstValueSInt16,Com_ConstValueSInt32,Com_ConstValueSInt64,Com_ConstValueFloat32,Com_ConstValueFloat64
  BitLength                 Bit length of the signal or group signal.
  BitPosition               Little endian bit position of the signal or group signal within the I-PDU.
  ByteLength                Byte length of the signal or group signal.
  InitValueIdx              the index of the 0:1 relation pointing to Com_ConstValueUInt8,Com_ConstValueUInt16,Com_ConstValueUInt32,Com_ConstValueUInt64,Com_ConstValueSInt8,Com_ConstValueSInt16,Com_ConstValueSInt32,Com_ConstValueSInt64,Com_ConstValueFloat32,Com_ConstValueFloat64
  RxPduInfoIdx              the index of the 1:1 relation pointing to Com_RxPduInfo
  StartByteInPduPosition    Start Byte position of the signal or group signal within the I-PDU.
*/ 
#define COM_START_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_RxAccessInfoType, COM_CONST) Com_RxAccessInfo[5] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
    /* Index    InitValueUsed  BitLength  BitPosition  ByteLength  InitValueIdx  RxPduInfoIdx  StartByteInPduPosition        Referable Keys */
  { /*     0 */          TRUE,        8u,          0u,         1u,           0u,           0u,                     0u },  /* [/ActiveEcuC/Com/ComConfig/sig_RearLeftWindowPosition_omsg_Receive2_oCAN00_89d4d489_Rx, /ActiveEcuC/Com/ComConfig/msg_Receive2_oCAN00_adb9646b_Rx] */
  { /*     1 */          TRUE,        8u,         56u,         1u,           0u,           0u,                     7u },  /* [/ActiveEcuC/Com/ComConfig/sig_RearRightWindowPosition_omsg_Receive2_oCAN00_94575df2_Rx, /ActiveEcuC/Com/ComConfig/msg_Receive2_oCAN00_adb9646b_Rx] */
  { /*     2 */          TRUE,        1u,          2u,         0u,           0u,           1u,                     0u },  /* [/ActiveEcuC/Com/ComConfig/sig_State_RearInteriorLight_omsg_Receive_oCAN00_63323183_Rx, /ActiveEcuC/Com/ComConfig/msg_Receive_oCAN00_2b456e3f_Rx] */
  { /*     3 */          TRUE,        1u,          0u,         0u,           0u,           1u,                     0u },  /* [/ActiveEcuC/Com/ComConfig/sig_State_RearLeftDoor_omsg_Receive_oCAN00_84ad4140_Rx, /ActiveEcuC/Com/ComConfig/msg_Receive_oCAN00_2b456e3f_Rx] */
  { /*     4 */          TRUE,        1u,          1u,         0u,           0u,           1u,                     0u }   /* [/ActiveEcuC/Com/ComConfig/sig_State_RearRightDoor_omsg_Receive_oCAN00_a8945098_Rx, /ActiveEcuC/Com/ComConfig/msg_Receive_oCAN00_2b456e3f_Rx] */
};
#define COM_STOP_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RxPduGrpInfo
**********************************************************************************************************************/
/** 
  \var    Com_RxPduGrpInfo
  \brief  Contains all I-PDU-Group relevant information for Rx I-PDUs.
  \details
  Element                 Description
  PduGrpVectorStartIdx    the start index of the 0:n relation pointing to Com_PduGrpVector
*/ 
#define COM_START_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_RxPduGrpInfoType, COM_CONST) Com_RxPduGrpInfo[2] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
    /* Index    PduGrpVectorStartIdx */
  { /*     0 */                   0u },
  { /*     1 */                   0u }
};
#define COM_STOP_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RxPduInfo
**********************************************************************************************************************/
/** 
  \var    Com_RxPduInfo
  \brief  Contains all relevant common information for Rx I-PDUs.
  \details
  Element                   Description
  RxAccessInfoIndUsed       TRUE, if the 0:n relation has 1 relation pointing to Com_RxAccessInfoInd
  RxDefPduBufferLength      the number of relations pointing to Com_RxDefPduBuffer
  RxDefPduBufferStartIdx    the start index of the 0:n relation pointing to Com_RxDefPduBuffer
  RxSigInfoEndIdx           the end index of the 0:n relation pointing to Com_RxSigInfo
  RxSigInfoStartIdx         the start index of the 0:n relation pointing to Com_RxSigInfo
  Type                      Defines whether rx Pdu is a NORMAL or TP IPdu.
*/ 
#define COM_START_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_RxPduInfoType, COM_CONST) Com_RxPduInfo[2] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
    /* Index    RxAccessInfoIndUsed  RxDefPduBufferLength  RxDefPduBufferStartIdx  RxSigInfoEndIdx  RxSigInfoStartIdx  Type                              Referable Keys */
  { /*     0 */                TRUE,                   8u,                     0u,              2u,                0u, COM_NORMAL_TYPEOFRXPDUINFO },  /* [/ActiveEcuC/Com/ComConfig/msg_Receive2_oCAN00_adb9646b_Rx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Rx_4cbf71f7] */
  { /*     1 */                TRUE,                   1u,                     8u,              5u,                2u, COM_NORMAL_TYPEOFRXPDUINFO }   /* [/ActiveEcuC/Com/ComConfig/msg_Receive_oCAN00_2b456e3f_Rx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Rx_4cbf71f7] */
};
#define COM_STOP_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RxSigInfo
**********************************************************************************************************************/
/** 
  \var    Com_RxSigInfo
  \brief  Contains all relevant information for Rx signals.
  \details
  Element             Description
  SignalProcessing
  ValidDlc            Minimum length of PDU required to completely receive the signal or signal group.
*/ 
#define COM_START_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_RxSigInfoType, COM_CONST) Com_RxSigInfo[5] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
    /* Index    SignalProcessing                          ValidDlc        Referable Keys */
  { /*     0 */ COM_DEFERRED_SIGNALPROCESSINGOFRXSIGINFO,       1u },  /* [/ActiveEcuC/Com/ComConfig/msg_Receive2_oCAN00_adb9646b_Rx] */
  { /*     1 */ COM_DEFERRED_SIGNALPROCESSINGOFRXSIGINFO,       8u },  /* [/ActiveEcuC/Com/ComConfig/msg_Receive2_oCAN00_adb9646b_Rx] */
  { /*     2 */ COM_DEFERRED_SIGNALPROCESSINGOFRXSIGINFO,       1u },  /* [/ActiveEcuC/Com/ComConfig/msg_Receive_oCAN00_2b456e3f_Rx] */
  { /*     3 */ COM_DEFERRED_SIGNALPROCESSINGOFRXSIGINFO,       1u },  /* [/ActiveEcuC/Com/ComConfig/msg_Receive_oCAN00_2b456e3f_Rx] */
  { /*     4 */ COM_DEFERRED_SIGNALPROCESSINGOFRXSIGINFO,       1u }   /* [/ActiveEcuC/Com/ComConfig/msg_Receive_oCAN00_2b456e3f_Rx] */
};
#define COM_STOP_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxModeFalse
**********************************************************************************************************************/
/** 
  \var    Com_TxModeFalse
  \brief  Contains all relevant information for transmission mode false.
  \details
  Element       Description
  TimePeriod    Cycle time factor.
*/ 
#define COM_START_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_TxModeFalseType, COM_CONST) Com_TxModeFalse[2] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
    /* Index    TimePeriod        Referable Keys */
  { /*     0 */         0u },  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  { /*     1 */        10u }   /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
};
#define COM_STOP_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxModeInfo
**********************************************************************************************************************/
/** 
  \var    Com_TxModeInfo
  \brief  Contains all relevant information for transmission mode handling.
  \details
  Element           Description
  InitMode          Initial transmission mode selector of the Tx I-PDU.
  TxModeFalseIdx    the index of the 1:1 relation pointing to Com_TxModeFalse
  TxModeTrueIdx     the index of the 1:1 relation pointing to Com_TxModeTrue
*/ 
#define COM_START_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_TxModeInfoType, COM_CONST) Com_TxModeInfo[3] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
    /* Index    InitMode  TxModeFalseIdx  TxModeTrueIdx        Referable Keys */
  { /*     0 */     TRUE,             1u,            0u },  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  { /*     1 */     TRUE,             0u,            0u },  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  { /*     2 */     TRUE,             1u,            0u }   /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
};
#define COM_STOP_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxModeTrue
**********************************************************************************************************************/
/** 
  \var    Com_TxModeTrue
  \brief  Contains all relevant information for transmission mode true.
  \details
  Element     Description
  Periodic    TRUE if transmission mode contains a cyclic part.
*/ 
#define COM_START_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_TxModeTrueType, COM_CONST) Com_TxModeTrue[1] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
    /* Index    Periodic        Referable Keys */
  { /*     0 */     TRUE }   /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
};
#define COM_STOP_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxPduGrpInfo
**********************************************************************************************************************/
/** 
  \var    Com_TxPduGrpInfo
  \brief  Contains all I-PDU-Group relevant information for Tx I-PDUs.
  \details
  Element                 Description
  PduGrpVectorStartIdx    the start index of the 0:n relation pointing to Com_PduGrpVector
*/ 
#define COM_START_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_TxPduGrpInfoType, COM_CONST) Com_TxPduGrpInfo[3] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
    /* Index    PduGrpVectorStartIdx */
  { /*     0 */                   1u },
  { /*     1 */                   1u },
  { /*     2 */                   1u }
};
#define COM_STOP_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxPduInfo
**********************************************************************************************************************/
/** 
  \var    Com_TxPduInfo
  \brief  Contains all relevant information for Tx I-PDUs.
  \details
  Element                    Description
  TxPduInitValueUsed         TRUE, if the 0:n relation has 1 relation pointing to Com_TxPduInitValue
  TxBufferLength             the number of relations pointing to Com_TxBuffer
  TxPduInitValueEndIdx       the end index of the 0:n relation pointing to Com_TxPduInitValue
  TxPduInitValueStartIdx     the start index of the 0:n relation pointing to Com_TxPduInitValue
  TxSigGrpInfoIndEndIdx      the end index of the 0:n relation pointing to Com_TxSigGrpInfoInd
  TxSigGrpInfoIndStartIdx    the start index of the 0:n relation pointing to Com_TxSigGrpInfoInd
*/ 
#define COM_START_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_TxPduInfoType, COM_CONST) Com_TxPduInfo[3] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
    /* Index    TxPduInitValueUsed  TxBufferLength  TxPduInitValueEndIdx  TxPduInitValueStartIdx  TxSigGrpInfoIndEndIdx                    TxSigGrpInfoIndStartIdx                          Referable Keys */
  { /*     0 */               TRUE,             8u,                   8u,                     0u, COM_NO_TXSIGGRPINFOINDENDIDXOFTXPDUINFO, COM_NO_TXSIGGRPINFOINDSTARTIDXOFTXPDUINFO },  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  { /*     1 */               TRUE,             8u,                  16u,                     8u, COM_NO_TXSIGGRPINFOINDENDIDXOFTXPDUINFO, COM_NO_TXSIGGRPINFOINDSTARTIDXOFTXPDUINFO },  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  { /*     2 */               TRUE,             8u,                  24u,                    16u,                                      1u,                                        0u }   /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
};
#define COM_STOP_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxPduInitValue
**********************************************************************************************************************/
/** 
  \var    Com_TxPduInitValue
  \brief  Initial values used for Tx I-PDU buffer initialization.
*/ 
#define COM_START_SEC_CONST_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_TxPduInitValueType, COM_CONST) Com_TxPduInitValue[24] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index     TxPduInitValue      Referable Keys */
  /*     0 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  /*     1 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  /*     2 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  /*     3 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  /*     4 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  /*     5 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  /*     6 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  /*     7 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  /*     8 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  /*     9 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  /*    10 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  /*    11 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  /*    12 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  /*    13 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  /*    14 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  /*    15 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  /*    16 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  /*    17 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  /*    18 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  /*    19 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  /*    20 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  /*    21 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  /*    22 */           0x00u,  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  /*    23 */           0x00u   /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
};
#define COM_STOP_SEC_CONST_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxSigGrpInfo
**********************************************************************************************************************/
/** 
  \var    Com_TxSigGrpInfo
  \brief  Contains all relevant information for Tx Signal Groups.
  \details
  Element             Description
  TxSigGrpMaskUsed    TRUE, if the 0:n relation has 1 relation pointing to Com_TxSigGrpMask
*/ 
#define COM_START_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_TxSigGrpInfoType, COM_CONST) Com_TxSigGrpInfo[1] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
    /* Index    TxSigGrpMaskUsed        Referable Keys */
  { /*     0 */             TRUE }   /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/My_SignalGroup/My_ComGroupSignal, /ActiveEcuC/Com/ComConfig/My_SignalGroup/My_ComGroupSignal1] */
};
#define COM_STOP_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxSigGrpMask
**********************************************************************************************************************/
/** 
  \var    Com_TxSigGrpMask
  \brief  Signal group mask needed to copy interlaced signal groups to the Tx PDU buffer.
*/ 
#define COM_START_SEC_CONST_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_TxSigGrpMaskType, COM_CONST) Com_TxSigGrpMask[4] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index     TxSigGrpMask      Referable Keys */
  /*     0 */         0xFFu,  /* [/ActiveEcuC/Com/ComConfig/My_SignalGroup] */
  /*     1 */         0x00u,  /* [/ActiveEcuC/Com/ComConfig/My_SignalGroup] */
  /*     2 */         0x00u,  /* [/ActiveEcuC/Com/ComConfig/My_SignalGroup] */
  /*     3 */         0xFFu   /* [/ActiveEcuC/Com/ComConfig/My_SignalGroup] */
};
#define COM_STOP_SEC_CONST_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxSigInfo
**********************************************************************************************************************/
/** 
  \var    Com_TxSigInfo
  \brief  Contains all relevant information for Tx signals and group signals.
  \details
  Element                   Description
  ApplType                  Application data type.
  BitLength                 Bit length of the signal or group signal.
  BitPosition               Little endian bit position of the signal or group signal within the I-PDU.
  ByteLength                Byte length of the signal or group signal.
  StartByteInPduPosition    Start Byte position of the signal or group signal within the I-PDU.
  TxBufferLength            the number of relations pointing to Com_TxBuffer
  TxBufferStartIdx          the start index of the 0:n relation pointing to Com_TxBuffer
  TxPduInfoIdx              the index of the 1:1 relation pointing to Com_TxPduInfo
  TxSigGrpInfoIdx           the index of the 0:1 relation pointing to Com_TxSigGrpInfo
*/ 
#define COM_START_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
CONST(Com_TxSigInfoType, COM_CONST) Com_TxSigInfo[5] = {  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
    /* Index    ApplType                       BitLength  BitPosition  ByteLength  StartByteInPduPosition  TxBufferLength  TxBufferStartIdx  TxPduInfoIdx  TxSigGrpInfoIdx                          Referable Keys */
  { /*     0 */ COM_UINT8_APPLTYPEOFTXSIGINFO,        8u,          8u,         1u,                     1u,             1u,              24u,           2u,                                0u },  /* [/ActiveEcuC/Com/ComConfig/My_SignalGroup/My_ComGroupSignal, /ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  { /*     1 */ COM_SINT8_APPLTYPEOFTXSIGINFO,        8u,         32u,         1u,                     4u,             1u,              27u,           2u,                                0u },  /* [/ActiveEcuC/Com/ComConfig/My_SignalGroup/My_ComGroupSignal1, /ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  { /*     2 */ COM_UINT8_APPLTYPEOFTXSIGINFO,        8u,          0u,         1u,                     0u,             1u,               0u,           0u, COM_NO_TXSIGGRPINFOIDXOFTXSIGINFO },  /* [/ActiveEcuC/Com/ComConfig/sig_LampCnt_omsg_MyECU_Lamp_oCAN00_f37e68ea_Tx, /ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  { /*     3 */ COM_UINT8_APPLTYPEOFTXSIGINFO,        1u,          0u,         0u,                     0u,             1u,               8u,           1u, COM_NO_TXSIGGRPINFOIDXOFTXSIGINFO },  /* [/ActiveEcuC/Com/ComConfig/sig_RearInterLight_omsg_Transmit2_oCAN00_6c233e33_Tx, /ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  { /*     4 */ COM_UINT8_APPLTYPEOFTXSIGINFO,        1u,          0u,         0u,                     0u,             1u,              16u,           2u, COM_NO_TXSIGGRPINFOIDXOFTXSIGINFO }   /* [/ActiveEcuC/Com/ComConfig/sig_RearInteriorLight_omsg_Transmit_oCAN00_49a633c1_Tx, /ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
};
#define COM_STOP_SEC_CONST_UNSPECIFIED
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_CurrentTxMode
**********************************************************************************************************************/
/** 
  \var    Com_CurrentTxMode
  \brief  Current transmission mode state of all Tx I-PDUs.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_CurrentTxModeType, COM_VAR_NOINIT) Com_CurrentTxMode[3];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     2 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_CycleTimeCnt
**********************************************************************************************************************/
/** 
  \var    Com_CycleTimeCnt
  \brief  Current counter value of cyclic transmission.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_CycleTimeCntType, COM_VAR_NOINIT) Com_CycleTimeCnt[3];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     2 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_CyclicSendRequest
**********************************************************************************************************************/
/** 
  \var    Com_CyclicSendRequest
  \brief  Cyclic send request flag used to indicate cyclic transmission mode for all Tx I-PDU.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_CyclicSendRequestType, COM_VAR_NOINIT) Com_CyclicSendRequest[3];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     2 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_DelayTimeCnt
**********************************************************************************************************************/
/** 
  \var    Com_DelayTimeCnt
  \brief  Current counter value of minimum delay counter.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_DelayTimeCntType, COM_VAR_NOINIT) Com_DelayTimeCnt[3];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     2 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_GatewayProcessingISRLockCounter
**********************************************************************************************************************/
#define COM_START_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_GatewayProcessingISRLockCounterType, COM_VAR_NOINIT) Com_GatewayProcessingISRLockCounter;  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
#define COM_STOP_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_HandleRxPduDeferred
**********************************************************************************************************************/
/** 
  \var    Com_HandleRxPduDeferred
  \brief  Array indicating received Rx I-PDUs to be processed deferred within the next call of Com_MainfunctionRx().
*/ 
#define COM_START_SEC_VAR_NOINIT_32BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_HandleRxPduDeferredUType, COM_VAR_NOINIT) Com_HandleRxPduDeferred;  /* PRQA S 0759, 1514, 1533 */  /* MD_CSL_Union, MD_CSL_ObjectOnlyAccessedOnce, MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_Receive2_oCAN00_adb9646b_Rx] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_Receive_oCAN00_2b456e3f_Rx] */

#define COM_STOP_SEC_VAR_NOINIT_32BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_Initialized
**********************************************************************************************************************/
/** 
  \var    Com_Initialized
  \brief  Initialization state of Com. TRUE, if Com_Init() has been called, else FALSE.
*/ 
#define COM_START_SEC_VAR_ZERO_INIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_InitializedType, COM_VAR_ZERO_INIT) Com_Initialized = FALSE;  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
#define COM_STOP_SEC_VAR_ZERO_INIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RepCnt
**********************************************************************************************************************/
/** 
  \var    Com_RepCnt
  \brief  Current repetition counter value for replication of transmission requests plus one initial transmit.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_RepCntType, COM_VAR_NOINIT) Com_RepCnt[3];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     2 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RepCycleCnt
**********************************************************************************************************************/
/** 
  \var    Com_RepCycleCnt
  \brief  Current counter value of repetition period for replication of transmission requests.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_RepCycleCntType, COM_VAR_NOINIT) Com_RepCycleCnt[3];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     2 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RxDeadlineMonitoringISRLockCounter
**********************************************************************************************************************/
#define COM_START_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_RxDeadlineMonitoringISRLockCounterType, COM_VAR_NOINIT) Com_RxDeadlineMonitoringISRLockCounter;  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
#define COM_STOP_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RxDefPduBuffer
**********************************************************************************************************************/
/** 
  \var    Com_RxDefPduBuffer
  \brief  Rx I-PDU buffer for deferred signal processing.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_RxDefPduBufferUType, COM_VAR_NOINIT) Com_RxDefPduBuffer;  /* PRQA S 0759, 1514, 1533 */  /* MD_CSL_Union, MD_CSL_ObjectOnlyAccessedOnce, MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_Receive2_oCAN00_adb9646b_Rx] */
  /*   ... */  /* [/ActiveEcuC/Com/ComConfig/msg_Receive2_oCAN00_adb9646b_Rx] */
  /*     7 */  /* [/ActiveEcuC/Com/ComConfig/msg_Receive2_oCAN00_adb9646b_Rx] */
  /*     8 */  /* [/ActiveEcuC/Com/ComConfig/msg_Receive_oCAN00_2b456e3f_Rx] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RxDeferredProcessingISRLockCounter
**********************************************************************************************************************/
#define COM_START_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_RxDeferredProcessingISRLockCounterType, COM_VAR_NOINIT) Com_RxDeferredProcessingISRLockCounter;  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
#define COM_STOP_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RxIPduGroupISRLockCounter
**********************************************************************************************************************/
#define COM_START_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_RxIPduGroupISRLockCounterType, COM_VAR_NOINIT) Com_RxIPduGroupISRLockCounter;  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
#define COM_STOP_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RxPduGrpActive
**********************************************************************************************************************/
/** 
  \var    Com_RxPduGrpActive
  \brief  Rx I-PDU based state (started/stopped) of the corresponding I-PDU-Group.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_RxPduGrpActiveType, COM_VAR_NOINIT) Com_RxPduGrpActive[2];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_Receive2_oCAN00_adb9646b_Rx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Rx_4cbf71f7] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_Receive_oCAN00_2b456e3f_Rx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Rx_4cbf71f7] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_RxSigBufferUInt8
**********************************************************************************************************************/
/** 
  \var    Com_RxSigBufferUInt8
  \brief  Rx Signal and Group Signal Buffer. (BOOLEAN, UINT8)
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_RxSigBufferUInt8Type, COM_VAR_NOINIT) Com_RxSigBufferUInt8[5];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/sig_RearLeftWindowPosition_omsg_Receive2_oCAN00_89d4d489_Rx, /ActiveEcuC/Com/ComConfig/sig_RearLeftWindowPosition_omsg_Receive2_oCAN00_89d4d489_Rx_RxSignalBufferRouting] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/sig_RearRightWindowPosition_omsg_Receive2_oCAN00_94575df2_Rx, /ActiveEcuC/Com/ComConfig/sig_RearRightWindowPosition_omsg_Receive2_oCAN00_94575df2_Rx_RxSignalBufferRouting] */
  /*     2 */  /* [/ActiveEcuC/Com/ComConfig/sig_State_RearInteriorLight_omsg_Receive_oCAN00_63323183_Rx, /ActiveEcuC/Com/ComConfig/sig_State_RearInteriorLight_omsg_Receive_oCAN00_63323183_Rx_RxSignalBufferRouting] */
  /*     3 */  /* [/ActiveEcuC/Com/ComConfig/sig_State_RearLeftDoor_omsg_Receive_oCAN00_84ad4140_Rx, /ActiveEcuC/Com/ComConfig/sig_State_RearLeftDoor_omsg_Receive_oCAN00_84ad4140_Rx_RxSignalBufferRouting] */
  /*     4 */  /* [/ActiveEcuC/Com/ComConfig/sig_State_RearRightDoor_omsg_Receive_oCAN00_a8945098_Rx, /ActiveEcuC/Com/ComConfig/sig_State_RearRightDoor_omsg_Receive_oCAN00_a8945098_Rx_RxSignalBufferRouting] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_SigGrpEventFlag
**********************************************************************************************************************/
/** 
  \var    Com_SigGrpEventFlag
  \brief  Flag is set if a group signal write access caused a triggered event.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_SigGrpEventFlagType, COM_VAR_NOINIT) Com_SigGrpEventFlag[1];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TransmitRequest
**********************************************************************************************************************/
/** 
  \var    Com_TransmitRequest
  \brief  Transmit request flag used for decoupled Tx I-PDU tranmission.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_TransmitRequestType, COM_VAR_NOINIT) Com_TransmitRequest[3];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     2 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxBuffer
**********************************************************************************************************************/
/** 
  \var    Com_TxBuffer
  \brief  Shared uint8 buffer for Tx I-PDUs and ComSignalGroup shadow buffer.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_TxBufferType, COM_VAR_NOINIT) Com_TxBuffer[28];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/sig_LampCnt_omsg_MyECU_Lamp_oCAN00_f37e68ea_Tx] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  /*   ... */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  /*     7 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx] */
  /*     8 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/sig_RearInterLight_omsg_Transmit2_oCAN00_6c233e33_Tx] */
  /*     9 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  /*   ... */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  /*    15 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx] */
  /*    16 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/sig_RearInteriorLight_omsg_Transmit_oCAN00_49a633c1_Tx] */
  /*    17 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/My_SignalGroupSigGrpInTxIPDU] */
  /*   ... */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/My_SignalGroupSigGrpInTxIPDU] */
  /*    20 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/My_SignalGroupSigGrpInTxIPDU] */
  /*    21 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  /*   ... */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  /*    23 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx] */
  /*    24 */  /* [/ActiveEcuC/Com/ComConfig/My_SignalGroup, /ActiveEcuC/Com/ComConfig/My_SignalGroup/My_ComGroupSignal] */
  /*    25 */  /* [/ActiveEcuC/Com/ComConfig/My_SignalGroup] */
  /*    26 */  /* [/ActiveEcuC/Com/ComConfig/My_SignalGroup] */
  /*    27 */  /* [/ActiveEcuC/Com/ComConfig/My_SignalGroup, /ActiveEcuC/Com/ComConfig/My_SignalGroup/My_ComGroupSignal1] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxCyclicProcessingISRLockCounter
**********************************************************************************************************************/
#define COM_START_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_TxCyclicProcessingISRLockCounterType, COM_VAR_NOINIT) Com_TxCyclicProcessingISRLockCounter;  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
#define COM_STOP_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxDeadlineMonitoringISRLockCounter
**********************************************************************************************************************/
#define COM_START_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_TxDeadlineMonitoringISRLockCounterType, COM_VAR_NOINIT) Com_TxDeadlineMonitoringISRLockCounter;  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
#define COM_STOP_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxIPduGroupISRLockCounter
**********************************************************************************************************************/
#define COM_START_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_TxIPduGroupISRLockCounterType, COM_VAR_NOINIT) Com_TxIPduGroupISRLockCounter;  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
#define COM_STOP_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxPduGrpActive
**********************************************************************************************************************/
/** 
  \var    Com_TxPduGrpActive
  \brief  Tx I-PDU based state (started/stopped) of the corresponding I-PDU-Group.
*/ 
#define COM_START_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_TxPduGrpActiveType, COM_VAR_NOINIT) Com_TxPduGrpActive[3];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     2 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */

#define COM_STOP_SEC_VAR_NOINIT_8BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxProcessingISRLockCounter
**********************************************************************************************************************/
#define COM_START_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_TxProcessingISRLockCounterType, COM_VAR_NOINIT) Com_TxProcessingISRLockCounter;  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
#define COM_STOP_SEC_VAR_NOINIT_16BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */

/**********************************************************************************************************************
  Com_TxSduLength
**********************************************************************************************************************/
/** 
  \var    Com_TxSduLength
  \brief  This var Array contains the Com Ipdu Length.
*/ 
#define COM_START_SEC_VAR_NOINIT_32BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */
VAR(Com_TxSduLengthType, COM_VAR_NOINIT) Com_TxSduLength[3];  /* PRQA S 1514, 1533 */  /* MD_CSL_ObjectOnlyAccessedOnce */
  /* Index        Referable Keys */
  /*     0 */  /* [/ActiveEcuC/Com/ComConfig/msg_MyECU_Lamp_oCAN00_818e1651_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     1 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit2_oCAN00_97205322_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */
  /*     2 */  /* [/ActiveEcuC/Com/ComConfig/msg_Transmit_oCAN00_0723e95e_Tx, /ActiveEcuC/Com/ComConfig/MyECU_oCAN00_Tx_1ae5d671] */

#define COM_STOP_SEC_VAR_NOINIT_32BIT
/*lint -save -esym(961, 19.1) */
#include "MemMap.h"  /* PRQA S 5087 */  /* MD_MSR_MemMap */
/*lint -restore */


/**********************************************************************************************************************
  CONFIGURATION CLASS: LINK
  SECTION: GLOBAL DATA
**********************************************************************************************************************/


/**********************************************************************************************************************
  LOCAL FUNCTION PROTOTYPES
**********************************************************************************************************************/



/**********************************************************************************************************************
  LOCAL FUNCTIONS
**********************************************************************************************************************/

/**********************************************************************************************************************
  GLOBAL FUNCTIONS
**********************************************************************************************************************/
/**********************************************************************************************************************
  CONFIGURATION CLASS: PRE_COMPILE
  SECTION: GLOBAL FUNCTIONS
**********************************************************************************************************************/

/**********************************************************************************************************************
  CONFIGURATION CLASS: LINK
  SECTION: GLOBAL FUNCTIONS
**********************************************************************************************************************/






/**********************************************************************************************************************
  END OF FILE: Com_Lcfg.c
**********************************************************************************************************************/

