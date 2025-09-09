/****************************************************************************************
*                               CANIF_CFG.C                                            *
****************************************************************************************
* File Name   : CanIf_Cfg.c
* Module      : CAN Interface (CanIf)
* Description : AUTOSAR CanIf runtime configuration implementation
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 24/08/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "CanIf.h"
#include "CanIf_Cfg.h"
#include "Can.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CANIF_CFG_VENDOR_ID_C                      (0x0001U)
#define CANIF_CFG_MODULE_ID_C                      (0x007CU)
#define CANIF_CFG_AR_RELEASE_MAJOR_VERSION_C       (4U)
#define CANIF_CFG_AR_RELEASE_MINOR_VERSION_C       (4U)
#define CANIF_CFG_AR_RELEASE_REVISION_VERSION_C    (0U)
#define CANIF_CFG_SW_MAJOR_VERSION_C               (1U)
#define CANIF_CFG_SW_MINOR_VERSION_C               (0U)
#define CANIF_CFG_SW_PATCH_VERSION_C               (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if CanIf_Cfg.c file and CanIf.h file are of the same vendor */
#if (CANIF_CFG_VENDOR_ID_C != CANIF_VENDOR_ID)
    #error "CanIf_Cfg.c and CanIf.h have different vendor ids"
#endif

/* Check AUTOSAR version compatibility */
#if ((CANIF_CFG_AR_RELEASE_MAJOR_VERSION_C != CANIF_AR_RELEASE_MAJOR_VERSION) || \
     (CANIF_CFG_AR_RELEASE_MINOR_VERSION_C != CANIF_AR_RELEASE_MINOR_VERSION) || \
     (CANIF_CFG_AR_RELEASE_REVISION_VERSION_C != CANIF_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR Version Numbers of CanIf_Cfg.c and CanIf.h are different"
#endif

/* Check software version compatibility */
#if ((CANIF_CFG_SW_MAJOR_VERSION_C != CANIF_SW_MAJOR_VERSION) || \
     (CANIF_CFG_SW_MINOR_VERSION_C != CANIF_SW_MINOR_VERSION) || \
     (CANIF_CFG_SW_PATCH_VERSION_C != CANIF_SW_PATCH_VERSION))
    #error "Software Version Numbers of CanIf_Cfg.c and CanIf.h are different"
#endif

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*=================================== CAN HRH Configuration ================================*/

/* Rx HRH Configuration for CAN1 Controller */
static const CanIf_HrhConfigType CanIf_HrhConfigData[CANIF_CFG_MAX_RX_HRHS] = 
{
    /* HRH 0: Standard messages reception for CAN1 */
    {
        /* Controller reference */
        .ControllerId = 0U,
        /* HRH ID */
        .HrhId = 0U,
        /* HOH ID from CAN driver */
        .HohId = 0U,
        /* CAN ID Range - Standard frames (0x000 - 0x7FF) */
        .HrhRangeConfig = 
        {
            .HrhRangeBaseId = 0x000U,
            .HrhRangeMask = 0x7FFU,
            .HrhRangeType = CANIF_STANDARD
        },
        /* Software filtering enabled */
        .HrhSoftwareFilter = STD_ON
    },
    
    /* HRH 1: Extended messages reception for CAN1 */
    {
        /* Controller reference */
        .ControllerId = 0U,
        /* HRH ID */
        .HrhId = 1U,
        /* HOH ID from CAN driver */
        .HohId = 1U,
        /* CAN ID Range - Extended frames (0x00000000 - 0x1FFFFFFF) */
        .HrhRangeConfig = 
        {
            .HrhRangeBaseId = 0x00000000U,
            .HrhRangeMask = 0x1FFFFFFFU,
            .HrhRangeType = CANIF_EXTENDED
        },
        /* Software filtering enabled */
        .HrhSoftwareFilter = STD_ON
    }
};

/*=================================== CAN HTH Configuration ================================*/

/* Tx HTH Configuration for CAN1 Controller */
static const CanIf_HthConfigType CanIf_HthConfigData[CANIF_CFG_MAX_TX_HTHS] = 
{
    /* HTH 0: Standard messages transmission for CAN1 */
    {
        /* Controller reference */
        .ControllerId = 0U,
        /* HTH ID */
        .HthId = 0U,
        /* HOH ID from CAN driver */
        .HohId = 2U,
        /* CAN ID type */
        .HthCanIdType = CANIF_STANDARD
    },
    
    /* HTH 1: Extended messages transmission for CAN1 */
    {
        /* Controller reference */
        .ControllerId = 0U,
        /* HTH ID */
        .HthId = 1U,
        /* HOH ID from CAN driver */
        .HohId = 3U,
        /* CAN ID type */
        .HthCanIdType = CANIF_EXTENDED
    }
};

/*============================ CAN Controller Configuration ===========================*/

/* CanIf Controller Configuration */
static const CanIf_CtrlConfigType CanIf_CtrlConfigData[CANIF_CFG_MAX_CONTROLLERS] = 
{
    /* Controller 0: CAN1 */
    {
        /* Controller ID */
        .ControllerId = 0U,
        /* CAN driver controller ID */
        .CanControllerId = 0U,
        /* Wakeup support */
        .WakeupSupport = STD_OFF,
        /* Wakeup source */
        .WakeupSource = 0U,
        /* Number of Tx buffers */
        .TxBufferCount = CANIF_CFG_TX_BUFFER_SIZE,
        /* Reference to first HTH */
        .FirstHthIndex = 0U,
        /* Number of HTHs */
        .HthCount = 2U,
        /* Reference to first HRH */
        .FirstHrhIndex = 0U,
        /* Number of HRHs */
        .HrhCount = 2U
    }
};

/*============================ Rx PDU Configuration ===========================*/

/* Rx PDU Configuration */
static const CanIf_RxPduConfigType CanIf_RxPduConfigData[CANIF_CFG_MAX_RX_PDUS] = 
{
    /* Rx PDU 0: Engine Speed */
    {
        /* Rx PDU ID */
        .RxPduId = 0U,
        /* CAN ID */
        .RxPduCanId = 0x100U,
        /* CAN ID type */
        .RxPduCanIdType = CANIF_STANDARD,
        /* DLC */
        .RxPduDlc = 8U,
        /* HRH reference */
        .RxPduHrhId = 0U,
        /* User RxIndication function */
        .RxPduUserRxIndicationUL = NULL_PTR,
        /* User RxIndication name for linking */
        .RxPduUserRxIndicationName = "PduR_CanIfRxIndication",
        /* Read notification status API */
        .RxPduReadNotifyStatus = STD_ON,
        /* Read data API */
        .RxPduReadData = STD_ON
    },
    
    /* Rx PDU 1: Vehicle Speed */
    {
        /* Rx PDU ID */
        .RxPduId = 1U,
        /* CAN ID */
        .RxPduCanId = 0x200U,
        /* CAN ID type */
        .RxPduCanIdType = CANIF_STANDARD,
        /* DLC */
        .RxPduDlc = 8U,
        /* HRH reference */
        .RxPduHrhId = 0U,
        /* User RxIndication function */
        .RxPduUserRxIndicationUL = NULL_PTR,
        /* User RxIndication name for linking */
        .RxPduUserRxIndicationName = "PduR_CanIfRxIndication",
        /* Read notification status API */
        .RxPduReadNotifyStatus = STD_ON,
        /* Read data API */
        .RxPduReadData = STD_ON
    },
    
    /* Rx PDU 2: Diagnostic Request */
    {
        /* Rx PDU ID */
        .RxPduId = 2U,
        /* CAN ID */
        .RxPduCanId = 0x7DFU,
        /* CAN ID type */
        .RxPduCanIdType = CANIF_STANDARD,
        /* DLC */
        .RxPduDlc = 8U,
        /* HRH reference */
        .RxPduHrhId = 0U,
        /* User RxIndication function */
        .RxPduUserRxIndicationUL = NULL_PTR,
        /* User RxIndication name for linking */
        .RxPduUserRxIndicationName = "Dcm_CanIfRxIndication",
        /* Read notification status API */
        .RxPduReadNotifyStatus = STD_OFF,
        /* Read data API */
        .RxPduReadData = STD_OFF
    }
};

/*============================ Tx PDU Configuration ===========================*/

/* Tx PDU Configuration */
static const CanIf_TxPduConfigType CanIf_TxPduConfigData[CANIF_CFG_MAX_TX_PDUS] = 
{
    /* Tx PDU 0: Engine Status */
    {
        /* Tx PDU ID */
        .TxPduId = 0U,
        /* CAN ID */
        .TxPduCanId = 0x300U,
        /* CAN ID type */
        .TxPduCanIdType = CANIF_STANDARD,
        /* DLC */
        .TxPduDlc = 8U,
        /* HTH reference */
        .TxPduHthId = 0U,
        /* User TxConfirmation function */
        .TxPduUserTxConfirmationUL = NULL_PTR,
        /* User TxConfirmation name for linking */
        .TxPduUserTxConfirmationName = "PduR_CanIfTxConfirmation",
        /* Tx confirmation enabled */
        .TxPduTxConfirmation = STD_ON,
        /* Read notification status API */
        .TxPduReadNotifyStatus = STD_ON,
        /* Trigger transmit */
        .TxPduTriggerTransmit = STD_OFF,
        /* Truncation enabled */
        .TxPduTruncation = STD_OFF,
        /* Tx PDU type */
        .TxPduType = CANIF_STATIC
    },
    
    /* Tx PDU 1: Brake System Status */
    {
        /* Tx PDU ID */
        .TxPduId = 1U,
        /* CAN ID */
        .TxPduCanId = 0x400U,
        /* CAN ID type */
        .TxPduCanIdType = CANIF_STANDARD,
        /* DLC */
        .TxPduDlc = 8U,
        /* HTH reference */
        .TxPduHthId = 0U,
        /* User TxConfirmation function */
        .TxPduUserTxConfirmationUL = NULL_PTR,
        /* User TxConfirmation name for linking */
        .TxPduUserTxConfirmationName = "PduR_CanIfTxConfirmation",
        /* Tx confirmation enabled */
        .TxPduTxConfirmation = STD_ON,
        /* Read notification status API */
        .TxPduReadNotifyStatus = STD_ON,
        /* Trigger transmit */
        .TxPduTriggerTransmit = STD_OFF,
        /* Truncation enabled */
        .TxPduTruncation = STD_OFF,
        /* Tx PDU type */
        .TxPduType = CANIF_STATIC
    },
    
    /* Tx PDU 2: Diagnostic Response */
    {
        /* Tx PDU ID */
        .TxPduId = 2U,
        /* CAN ID */
        .TxPduCanId = 0x7E7U,
        /* CAN ID type */
        .TxPduCanIdType = CANIF_STANDARD,
        /* DLC */
        .TxPduDlc = 8U,
        /* HTH reference */
        .TxPduHthId = 0U,
        /* User TxConfirmation function */
        .TxPduUserTxConfirmationUL = NULL_PTR,
        /* User TxConfirmation name for linking */
        .TxPduUserTxConfirmationName = "Dcm_CanIfTxConfirmation",
        /* Tx confirmation enabled */
        .TxPduTxConfirmation = STD_ON,
        /* Read notification status API */
        .TxPduReadNotifyStatus = STD_OFF,
        /* Trigger transmit */
        .TxPduTriggerTransmit = STD_OFF,
        /* Truncation enabled */
        .TxPduTruncation = STD_OFF,
        /* Tx PDU type */
        .TxPduType = CANIF_STATIC
    },
    
    /* Tx PDU 3: Dynamic CAN ID example */
    {
        /* Tx PDU ID */
        .TxPduId = 3U,
        /* CAN ID - will be changed dynamically */
        .TxPduCanId = 0x500U,
        /* CAN ID type */
        .TxPduCanIdType = CANIF_STANDARD,
        /* DLC */
        .TxPduDlc = 8U,
        /* HTH reference */
        .TxPduHthId = 0U,
        /* User TxConfirmation function */
        .TxPduUserTxConfirmationUL = NULL_PTR,
        /* User TxConfirmation name for linking */
        .TxPduUserTxConfirmationName = "PduR_CanIfTxConfirmation",
        /* Tx confirmation enabled */
        .TxPduTxConfirmation = STD_ON,
        /* Read notification status API */
        .TxPduReadNotifyStatus = STD_OFF,
        /* Trigger transmit */
        .TxPduTriggerTransmit = STD_OFF,
        /* Truncation enabled */
        .TxPduTruncation = STD_OFF,
        /* Tx PDU type */
        .TxPduType = CANIF_DYNAMIC
    }
};

/*============================ Transceiver Configuration ===========================*/

#if (CANIF_TRANSCEIVER_SUPPORT == STD_ON)
/* Transceiver Configuration */
static const CanIf_TransceiverConfigType CanIf_TransceiverConfigData[CANIF_CFG_MAX_TRANSCEIVERS] = 
{
    /* Transceiver 0: External CAN transceiver */
    {
        /* Transceiver ID */
        .TransceiverId = 0U,
        /* Controller reference */
        .ControllerId = 0U,
        /* Transceiver driver ID */
        .TransceiverDriverId = 0U,
        /* Wakeup support */
        .WakeupSupport = STD_OFF,
        /* Wakeup source */
        .WakeupSource = 0U
    }
};
#endif /* CANIF_TRANSCEIVER_SUPPORT == STD_ON */

/*============================ Main CanIf Configuration ===========================*/

/* CanIf Configuration Structure */
const CanIf_ConfigType CanIf_Config = 
{
    /* Controllers */
    .CanIfCtrlConfig = CanIf_CtrlConfigData,
    .CanIfMaxCtrlId = (CANIF_CFG_MAX_CONTROLLERS - 1U),
    
    /* HRH (Hardware Receive Handles) */
    .CanIfHrhConfig = CanIf_HrhConfigData,
    .CanIfMaxHrhId = (CANIF_CFG_MAX_RX_HRHS - 1U),
    
    /* HTH (Hardware Transmit Handles) */
    .CanIfHthConfig = CanIf_HthConfigData,
    .CanIfMaxHthId = (CANIF_CFG_MAX_TX_HTHS - 1U),
    
    /* Rx PDUs */
    .CanIfRxPduConfig = CanIf_RxPduConfigData,
    .CanIfMaxRxPduId = (CANIF_CFG_MAX_RX_PDUS - 1U),
    
    /* Tx PDUs */
    .CanIfTxPduConfig = CanIf_TxPduConfigData,
    .CanIfMaxTxPduId = (CANIF_CFG_MAX_TX_PDUS - 1U),
    
#if (CANIF_TRANSCEIVER_SUPPORT == STD_ON)
    /* Transceivers */
    .CanIfTransceiverConfig = CanIf_TransceiverConfigData,
    .CanIfMaxTransceiverId = (CANIF_CFG_MAX_TRANSCEIVERS - 1U),
#endif
    
    /* Configuration set identifier */
    .CanIfConfigSet = 0U
};

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/

/* Pre-compile configuration pointer for CanIf */
const CanIf_ConfigType* CanIf_ConfigPtr = &CanIf_Config;

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
