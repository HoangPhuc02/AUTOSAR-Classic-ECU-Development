/****************************************************************************************
*                               CANIF.C                                                *
****************************************************************************************
* File Name   : CanIf.c
* Module      : CAN Interface (CanIf)
* Description : AUTOSAR CanIf driver implementation
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 24/08/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "CanIf.h"
#include "CanIf_Cbk.h"

#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    #include "Det.h"
#endif

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CANIF_VENDOR_ID_C                      (0x0001U)
#define CANIF_MODULE_ID_C                      (0x007CU)
#define CANIF_AR_RELEASE_MAJOR_VERSION_C       (4U)
#define CANIF_AR_RELEASE_MINOR_VERSION_C       (4U)
#define CANIF_AR_RELEASE_REVISION_VERSION_C    (0U)
#define CANIF_SW_MAJOR_VERSION_C               (1U)
#define CANIF_SW_MINOR_VERSION_C               (0U)
#define CANIF_SW_PATCH_VERSION_C               (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if CanIf.c file and CanIf.h file are of the same vendor */
#if (CANIF_VENDOR_ID_C != CANIF_VENDOR_ID)
    #error "CanIf.c and CanIf.h have different vendor ids"
#endif

/* Check AUTOSAR version compatibility */
#if ((CANIF_AR_RELEASE_MAJOR_VERSION_C != CANIF_AR_RELEASE_MAJOR_VERSION) || \
     (CANIF_AR_RELEASE_MINOR_VERSION_C != CANIF_AR_RELEASE_MINOR_VERSION) || \
     (CANIF_AR_RELEASE_REVISION_VERSION_C != CANIF_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR Version Numbers of CanIf.c and CanIf.h are different"
#endif

/* Check software version compatibility */
#if ((CANIF_SW_MAJOR_VERSION_C != CANIF_SW_MAJOR_VERSION) || \
     (CANIF_SW_MINOR_VERSION_C != CANIF_SW_MINOR_VERSION) || \
     (CANIF_SW_PATCH_VERSION_C != CANIF_SW_PATCH_VERSION))
    #error "Software Version Numbers of CanIf.c and CanIf.h are different"
#endif

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/* CanIf internal state machine states */
#define CANIF_UNINIT            (0U)
#define CANIF_INIT              (1U)

/* Invalid PDU ID */
#define CANIF_INVALID_PDU_ID    (0xFFFFU)

/* Maximum DLC */
#define CANIF_MAX_DLC           (8U)

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/

/* CanIf module state */
static uint8 CanIf_ModuleState = CANIF_UNINIT;

/* Pointer to configuration */
static const CanIf_ConfigType* CanIf_ConfigPtr_Local = NULL_PTR;

/* Controller states */
static CanIf_ControllerModeType CanIf_ControllerStates[CANIF_CFG_MAX_CONTROLLERS];

/* PDU channel modes */
static CanIf_PduModeType CanIf_PduChannelModes[CANIF_CFG_MAX_CONTROLLERS];

/* Tx buffer for storing Tx PDU data */
#if (CANIF_CFG_TX_BUFFER_SIZE > 0U)
static uint8 CanIf_TxBuffer[CANIF_CFG_MAX_TX_PDUS][CANIF_MAX_DLC];
#endif

/* Rx buffer for storing Rx PDU data */
#if (CANIF_READ_RX_PDU_DATA_API == STD_ON)
static uint8 CanIf_RxBuffer[CANIF_CFG_MAX_RX_PDUS][CANIF_MAX_DLC];
static PduLengthType CanIf_RxBufferLength[CANIF_CFG_MAX_RX_PDUS];
#endif

/* Tx notification status */
#if (CANIF_READ_TX_PDU_NOTIFY_STATUS_API == STD_ON)
static CanIf_NotifStatusType CanIf_TxNotificationStatus[CANIF_CFG_MAX_TX_PDUS];
#endif

/* Rx notification status */
#if (CANIF_READ_RX_PDU_NOTIFY_STATUS_API == STD_ON)
static CanIf_NotifStatusType CanIf_RxNotificationStatus[CANIF_CFG_MAX_RX_PDUS];
#endif

/* Dynamic CAN IDs */
#if (CANIF_SET_DYNAMIC_TX_ID_API == STD_ON)
static Can_IdType CanIf_DynamicTxIds[CANIF_CFG_MAX_TX_PDUS];
#endif

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/* Internal helper functions */
static Std_ReturnType CanIf_ValidateControllerId(uint8 ControllerId);
static Std_ReturnType CanIf_ValidateTxPduId(PduIdType TxPduId);
static Std_ReturnType CanIf_ValidateRxPduId(PduIdType RxPduId);
static boolean CanIf_IsInitialized(void);
static PduIdType CanIf_FindRxPduByCanId(Can_IdType CanId, uint8 ControllerId);
static void CanIf_InitializeNotificationStatus(void);
static void CanIf_InitializeBuffers(void);

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/**
 * @brief   Validates the controller ID
 * @param[in] ControllerId - Controller ID to validate
 * @return  Std_ReturnType
 */
static Std_ReturnType CanIf_ValidateControllerId(uint8 ControllerId)
{
    Std_ReturnType RetVal = E_NOT_OK;
    
    if (ControllerId <= CanIf_ConfigPtr_Local->CanIfMaxCtrlId)
    {
        RetVal = E_OK;
    }
    
    return RetVal;
}

/**
 * @brief   Validates the Tx PDU ID
 * @param[in] TxPduId - Tx PDU ID to validate
 * @return  Std_ReturnType
 */
static Std_ReturnType CanIf_ValidateTxPduId(PduIdType TxPduId)
{
    Std_ReturnType RetVal = E_NOT_OK;
    
    if (TxPduId <= CanIf_ConfigPtr_Local->CanIfMaxTxPduId)
    {
        RetVal = E_OK;
    }
    
    return RetVal;
}

/**
 * @brief   Validates the Rx PDU ID
 * @param[in] RxPduId - Rx PDU ID to validate
 * @return  Std_ReturnType
 */
static Std_ReturnType CanIf_ValidateRxPduId(PduIdType RxPduId)
{
    Std_ReturnType RetVal = E_NOT_OK;
    
    if (RxPduId <= CanIf_ConfigPtr_Local->CanIfMaxRxPduId)
    {
        RetVal = E_OK;
    }
    
    return RetVal;
}

/**
 * @brief   Checks if CanIf is initialized
 * @return  boolean
 */
static boolean CanIf_IsInitialized(void)
{
    return (CanIf_ModuleState == CANIF_INIT) ? TRUE : FALSE;
}

/**
 * @brief   Finds Rx PDU by CAN ID
 * @param[in] CanId - CAN ID to search for
 * @param[in] ControllerId - Controller ID
 * @return  PduIdType - PDU ID or CANIF_INVALID_PDU_ID if not found
 */
static PduIdType CanIf_FindRxPduByCanId(Can_IdType CanId, uint8 ControllerId)
{
    PduIdType PduIndex;
    PduIdType FoundPduId = CANIF_INVALID_PDU_ID;
    const CanIf_RxPduConfigType* RxPduConfig;
    
    for (PduIndex = 0U; PduIndex <= CanIf_ConfigPtr_Local->CanIfMaxRxPduId; PduIndex++)
    {
        RxPduConfig = &CanIf_ConfigPtr_Local->CanIfRxPduConfig[PduIndex];
        
        /* Check if CAN ID matches and HRH belongs to the controller */
        if ((RxPduConfig->RxPduCanId == CanId) && 
            (CanIf_ConfigPtr_Local->CanIfHrhConfig[RxPduConfig->RxPduHrhId].ControllerId == ControllerId))
        {
            FoundPduId = PduIndex;
            break;
        }
    }
    
    return FoundPduId;
}

/**
 * @brief   Initializes notification status arrays
 */
static void CanIf_InitializeNotificationStatus(void)
{
    uint8 Index;
    
#if (CANIF_READ_TX_PDU_NOTIFY_STATUS_API == STD_ON)
    for (Index = 0U; Index <= CanIf_ConfigPtr_Local->CanIfMaxTxPduId; Index++)
    {
        CanIf_TxNotificationStatus[Index] = CANIF_NO_NOTIFICATION;
    }
#endif

#if (CANIF_READ_RX_PDU_NOTIFY_STATUS_API == STD_ON)
    for (Index = 0U; Index <= CanIf_ConfigPtr_Local->CanIfMaxRxPduId; Index++)
    {
        CanIf_RxNotificationStatus[Index] = CANIF_NO_NOTIFICATION;
    }
#endif
}

/**
 * @brief   Initializes internal buffers
 */
static void CanIf_InitializeBuffers(void)
{
    uint8 PduIndex, DataIndex;
    
#if (CANIF_CFG_TX_BUFFER_SIZE > 0U)
    for (PduIndex = 0U; PduIndex <= CanIf_ConfigPtr_Local->CanIfMaxTxPduId; PduIndex++)
    {
        for (DataIndex = 0U; DataIndex < CANIF_MAX_DLC; DataIndex++)
        {
            CanIf_TxBuffer[PduIndex][DataIndex] = 0U;
        }
    }
#endif

#if (CANIF_READ_RX_PDU_DATA_API == STD_ON)
    for (PduIndex = 0U; PduIndex <= CanIf_ConfigPtr_Local->CanIfMaxRxPduId; PduIndex++)
    {
        CanIf_RxBufferLength[PduIndex] = 0U;
        for (DataIndex = 0U; DataIndex < CANIF_MAX_DLC; DataIndex++)
        {
            CanIf_RxBuffer[PduIndex][DataIndex] = 0U;
        }
    }
#endif

#if (CANIF_SET_DYNAMIC_TX_ID_API == STD_ON)
    for (PduIndex = 0U; PduIndex <= CanIf_ConfigPtr_Local->CanIfMaxTxPduId; PduIndex++)
    {
        CanIf_DynamicTxIds[PduIndex] = CanIf_ConfigPtr_Local->CanIfTxPduConfig[PduIndex].TxPduCanId;
    }
#endif
}

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================ Services for CanIf Initialization =================================*/

/**
 * @brief   Initializes the CanIf module
 */
void CanIf_Init(const CanIf_ConfigType* ConfigPtr)
{
    uint8 ControllerIndex;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if ConfigPtr is not NULL */
    if (NULL_PTR == ConfigPtr)
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_INIT, CANIF_E_PARAM_POINTER);
        return;
    }
    
    /* Check if already initialized */
    if (CanIf_ModuleState == CANIF_INIT)
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_INIT, CANIF_E_UNINIT);
        return;
    }
#endif

    /* Store configuration pointer */
    CanIf_ConfigPtr_Local = ConfigPtr;
    
    /* Initialize controller states */
    for (ControllerIndex = 0U; ControllerIndex <= ConfigPtr->CanIfMaxCtrlId; ControllerIndex++)
    {
        CanIf_ControllerStates[ControllerIndex] = CANIF_CS_SLEEP;
        CanIf_PduChannelModes[ControllerIndex] = CANIF_OFFLINE;
    }
    
    /* Initialize notification status and buffers */
    CanIf_InitializeNotificationStatus();
    CanIf_InitializeBuffers();
    
    /* Set module state to initialized */
    CanIf_ModuleState = CANIF_INIT;
}

/**
 * @brief   De-initializes the CanIf module
 */
void CanIf_DeInit(void)
{
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (CanIf_ModuleState == CANIF_UNINIT)
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_DEINIT, CANIF_E_UNINIT);
        return;
    }
#endif

    /* Reset configuration pointer */
    CanIf_ConfigPtr_Local = NULL_PTR;
    
    /* Set module state to uninitialized */
    CanIf_ModuleState = CANIF_UNINIT;
}

/**
 * @brief   Returns version information of this module
 */
#if (CANIF_VERSION_INFO_API == STD_ON)
void CanIf_GetVersionInfo(Std_VersionInfoType* VersionInfoPtr)
{
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if VersionInfoPtr is not NULL */
    if (NULL_PTR == VersionInfoPtr)
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_GET_VERSION_INFO, CANIF_E_PARAM_POINTER);
        return;
    }
#endif

    /* Fill version information */
    VersionInfoPtr->vendorID = CANIF_VENDOR_ID;
    VersionInfoPtr->moduleID = CANIF_MODULE_ID;
    VersionInfoPtr->sw_major_version = CANIF_SW_MAJOR_VERSION;
    VersionInfoPtr->sw_minor_version = CANIF_SW_MINOR_VERSION;
    VersionInfoPtr->sw_patch_version = CANIF_SW_PATCH_VERSION;
}
#endif

/*======================== Services for Controller Mode Control ============================*/

/**
 * @brief   Calls the corresponding CAN Driver service for changing of the CAN controller mode
 */
Std_ReturnType CanIf_SetControllerMode(uint8 ControllerId, CanIf_ControllerModeType ControllerMode)
{
    Std_ReturnType RetVal = E_NOT_OK;
    Can_ControllerStateType CanControllerMode;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_CONTROLLER_MODE, CANIF_E_UNINIT);
        return E_NOT_OK;
    }
    
    /* Validate controller ID */
    if (E_NOT_OK == CanIf_ValidateControllerId(ControllerId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_CONTROLLER_MODE, CANIF_E_PARAM_CONTROLLERID);
        return E_NOT_OK;
    }
#endif

    /* Map CanIf controller mode to CAN driver mode */
    switch (ControllerMode)
    {
        case CANIF_CS_SLEEP:
            CanControllerMode = CAN_CS_SLEEP;
            break;
        case CANIF_CS_STARTED:
            CanControllerMode = CAN_CS_STARTED;
            break;
        case CANIF_CS_STOPPED:
            CanControllerMode = CAN_CS_STOPPED;
            break;
        default:
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_CONTROLLER_MODE, CANIF_E_PARAM_CONTROLLER);
#endif
            return E_NOT_OK;
    }
    
    /* Call CAN driver */
    RetVal = Can_SetControllerMode(CanIf_ConfigPtr_Local->CanIfCtrlConfig[ControllerId].CanControllerId, CanControllerMode);
    
    return RetVal;
}

/**
 * @brief   Reports about the current status of the requested CAN controller
 */
Std_ReturnType CanIf_GetControllerMode(uint8 ControllerId, CanIf_ControllerModeType* ControllerModePtr)
{
    Std_ReturnType RetVal = E_NOT_OK;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_GET_CONTROLLER_MODE, CANIF_E_UNINIT);
        return E_NOT_OK;
    }
    
    /* Validate controller ID */
    if (E_NOT_OK == CanIf_ValidateControllerId(ControllerId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_GET_CONTROLLER_MODE, CANIF_E_PARAM_CONTROLLERID);
        return E_NOT_OK;
    }
    
    /* Check if ControllerModePtr is not NULL */
    if (NULL_PTR == ControllerModePtr)
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_GET_CONTROLLER_MODE, CANIF_E_PARAM_POINTER);
        return E_NOT_OK;
    }
#endif

    /* Return current controller state */
    *ControllerModePtr = CanIf_ControllerStates[ControllerId];
    RetVal = E_OK;
    
    return RetVal;
}

/**
 * @brief   Reports about the current error status of the requested CAN controller
 */
#if (CAN_GET_CONTROLLER_ERROR_STATE_API == STD_ON)
Std_ReturnType CanIf_GetControllerErrorState(uint8 ControllerId, Can_ErrorStateType* ErrorStatePtr)
{
    Std_ReturnType RetVal = E_NOT_OK;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_GET_CONTROLLER_ERROR_STATE, CANIF_E_UNINIT);
        return E_NOT_OK;
    }
    
    /* Validate controller ID */
    if (E_NOT_OK == CanIf_ValidateControllerId(ControllerId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_GET_CONTROLLER_ERROR_STATE, CANIF_E_PARAM_CONTROLLERID);
        return E_NOT_OK;
    }
    
    /* Check if ErrorStatePtr is not NULL */
    if (NULL_PTR == ErrorStatePtr)
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_GET_CONTROLLER_ERROR_STATE, CANIF_E_PARAM_POINTER);
        return E_NOT_OK;
    }
#endif

    /* Call CAN driver */
    RetVal = Can_GetControllerErrorState(CanIf_ConfigPtr_Local->CanIfCtrlConfig[ControllerId].CanControllerId, ErrorStatePtr);
    
    return RetVal;
}
#endif

/*======================== Services for PDU Mode Control ============================*/

/**
 * @brief   Sets the requested mode at the L-PDUs of a receive CAN channel
 */
Std_ReturnType CanIf_SetPduMode(uint8 ControllerId, CanIf_PduModeType PduModeRequest)
{
    Std_ReturnType RetVal = E_NOT_OK;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_PDU_MODE, CANIF_E_UNINIT);
        return E_NOT_OK;
    }
    
    /* Validate controller ID */
    if (E_NOT_OK == CanIf_ValidateControllerId(ControllerId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_PDU_MODE, CANIF_E_PARAM_CONTROLLERID);
        return E_NOT_OK;
    }
    
    /* Validate PDU mode */
    if ((PduModeRequest != CANIF_OFFLINE) && 
        (PduModeRequest != CANIF_TX_OFFLINE) && 
        (PduModeRequest != CANIF_TX_OFFLINE_ACTIVE) && 
        (PduModeRequest != CANIF_ONLINE))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_PDU_MODE, CANIF_E_PARAM_PDU_MODE);
        return E_NOT_OK;
    }
#endif

    /* Check if controller is started */
    if (CanIf_ControllerStates[ControllerId] == CANIF_CS_STARTED)
    {
        /* Set PDU channel mode */
        CanIf_PduChannelModes[ControllerId] = PduModeRequest;
        RetVal = E_OK;
    }
    else
    {
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_PDU_MODE, CANIF_E_STOPPED);
#endif
    }
    
    return RetVal;
}

/**
 * @brief   Reports the current mode of a requested PDU channel
 */
Std_ReturnType CanIf_GetPduMode(uint8 ControllerId, CanIf_PduModeType* PduModePtr)
{
    Std_ReturnType RetVal = E_NOT_OK;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_GET_PDU_MODE, CANIF_E_UNINIT);
        return E_NOT_OK;
    }
    
    /* Validate controller ID */
    if (E_NOT_OK == CanIf_ValidateControllerId(ControllerId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_GET_PDU_MODE, CANIF_E_PARAM_CONTROLLERID);
        return E_NOT_OK;
    }
    
    /* Check if PduModePtr is not NULL */
    if (NULL_PTR == PduModePtr)
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_GET_PDU_MODE, CANIF_E_PARAM_POINTER);
        return E_NOT_OK;
    }
#endif

    /* Return current PDU mode */
    *PduModePtr = CanIf_PduChannelModes[ControllerId];
    RetVal = E_OK;
    
    return RetVal;
}

/*======================== Services for PDU Transmission ============================*/

/**
 * @brief   Requests transmission of a PDU
 */
Std_ReturnType CanIf_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr)
{
    Std_ReturnType RetVal = E_NOT_OK;
    const CanIf_TxPduConfigType* TxPduConfig;
    Can_PduType CanPduInfo;
    uint8 ControllerId;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_TRANSMIT, CANIF_E_UNINIT);
        return E_NOT_OK;
    }
    
    /* Validate Tx PDU ID */
    if (E_NOT_OK == CanIf_ValidateTxPduId(TxPduId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_TRANSMIT, CANIF_E_INVALID_TX_PDU_ID);
        return E_NOT_OK;
    }
    
    /* Check if PduInfoPtr is not NULL */
    if (NULL_PTR == PduInfoPtr)
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_TRANSMIT, CANIF_E_PARAM_POINTER);
        return E_NOT_OK;
    }
    
    /* Check DLC */
    if (PduInfoPtr->SduLength > CANIF_MAX_DLC)
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_TRANSMIT, CANIF_E_PARAM_DLC);
        return E_NOT_OK;
    }
#endif

    /* Get Tx PDU configuration */
    TxPduConfig = &CanIf_ConfigPtr_Local->CanIfTxPduConfig[TxPduId];
    
    /* Get controller ID from HTH configuration */
    ControllerId = CanIf_ConfigPtr_Local->CanIfHthConfig[TxPduConfig->TxPduHthId].ControllerId;
    
    /* Check PDU mode */
    if ((CanIf_PduChannelModes[ControllerId] == CANIF_ONLINE) || 
        (CanIf_PduChannelModes[ControllerId] == CANIF_TX_OFFLINE_ACTIVE))
    {
        /* Prepare CAN PDU */
        CanPduInfo.swPduHandle = TxPduId;
        CanPduInfo.length = (uint8)PduInfoPtr->SduLength;
        CanPduInfo.sdu = PduInfoPtr->SduDataPtr;
        
#if (CANIF_SET_DYNAMIC_TX_ID_API == STD_ON)
        /* Check if PDU has dynamic CAN ID */
        if (TxPduConfig->TxPduType == CANIF_DYNAMIC)
        {
            CanPduInfo.id = CanIf_DynamicTxIds[TxPduId];
        }
        else
#endif
        {
            CanPduInfo.id = TxPduConfig->TxPduCanId;
        }
        
        /* Call CAN driver */
        RetVal = Can_Write(CanIf_ConfigPtr_Local->CanIfHthConfig[TxPduConfig->TxPduHthId].HohId, &CanPduInfo);
        
#if (CANIF_CFG_TX_BUFFER_SIZE > 0U)
        /* Store data in Tx buffer if needed */
        if (E_OK == RetVal)
        {
            uint8 DataIndex;
            for (DataIndex = 0U; DataIndex < PduInfoPtr->SduLength; DataIndex++)
            {
                CanIf_TxBuffer[TxPduId][DataIndex] = PduInfoPtr->SduDataPtr[DataIndex];
            }
        }
#endif
    }
    else
    {
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_TRANSMIT, CANIF_E_STOPPED);
#endif
    }
    
    return RetVal;
}

/**
 * @brief   Cancels the transmission request
 */
#if (CANIF_CANCEL_TX_SUPPORT_API == STD_ON)
Std_ReturnType CanIf_CancelTxRequest(PduIdType TxPduId)
{
    Std_ReturnType RetVal = E_NOT_OK;
    const CanIf_TxPduConfigType* TxPduConfig;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_CANCEL_TX_REQUEST, CANIF_E_UNINIT);
        return E_NOT_OK;
    }
    
    /* Validate Tx PDU ID */
    if (E_NOT_OK == CanIf_ValidateTxPduId(TxPduId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_CANCEL_TX_REQUEST, CANIF_E_INVALID_TX_PDU_ID);
        return E_NOT_OK;
    }
#endif

    /* Get Tx PDU configuration */
    TxPduConfig = &CanIf_ConfigPtr_Local->CanIfTxPduConfig[TxPduId];
    
    /* Cancel transmission in CAN driver - not supported in basic CAN driver */
    /* This would require extended CAN driver API */
    RetVal = E_NOT_OK;
    
    return RetVal;
}
#endif

/*======================== Services for PDU Reception ============================*/

/**
 * @brief   Provides the Data Length and the received data of the requested CanRxPduId
 */
#if (CANIF_READ_RX_PDU_DATA_API == STD_ON)
Std_ReturnType CanIf_ReadRxPduData(PduIdType CanRxPduId, PduInfoType* PduInfoPtr)
{
    Std_ReturnType RetVal = E_NOT_OK;
    uint8 DataIndex;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_READ_RX_PDU_DATA, CANIF_E_UNINIT);
        return E_NOT_OK;
    }
    
    /* Validate Rx PDU ID */
    if (E_NOT_OK == CanIf_ValidateRxPduId(CanRxPduId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_READ_RX_PDU_DATA, CANIF_E_INVALID_RX_PDU_ID);
        return E_NOT_OK;
    }
    
    /* Check if PduInfoPtr is not NULL */
    if (NULL_PTR == PduInfoPtr)
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_READ_RX_PDU_DATA, CANIF_E_PARAM_POINTER);
        return E_NOT_OK;
    }
    
    /* Check if SduDataPtr is not NULL */
    if (NULL_PTR == PduInfoPtr->SduDataPtr)
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_READ_RX_PDU_DATA, CANIF_E_PARAM_POINTER);
        return E_NOT_OK;
    }
#endif

    /* Check if read data API is enabled for this PDU */
    if (CanIf_ConfigPtr_Local->CanIfRxPduConfig[CanRxPduId].RxPduReadData == STD_ON)
    {
        /* Copy data from Rx buffer */
        PduInfoPtr->SduLength = CanIf_RxBufferLength[CanRxPduId];
        
        for (DataIndex = 0U; DataIndex < CanIf_RxBufferLength[CanRxPduId]; DataIndex++)
        {
            PduInfoPtr->SduDataPtr[DataIndex] = CanIf_RxBuffer[CanRxPduId][DataIndex];
        }
        
        RetVal = E_OK;
    }
    
    return RetVal;
}
#endif

/*======================== Services for Notification Status ============================*/

/**
 * @brief   Returns the confirmation status of a specific static or dynamic CAN Tx L-PDU
 */
#if (CANIF_READ_TX_PDU_NOTIFY_STATUS_API == STD_ON)
CanIf_NotifStatusType CanIf_ReadTxNotifStatus(PduIdType CanTxPduId)
{
    CanIf_NotifStatusType NotifStatus = CANIF_NO_NOTIFICATION;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_READ_TX_NOTIF_STATUS, CANIF_E_UNINIT);
        return CANIF_NO_NOTIFICATION;
    }
    
    /* Validate Tx PDU ID */
    if (E_NOT_OK == CanIf_ValidateTxPduId(CanTxPduId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_READ_TX_NOTIF_STATUS, CANIF_E_INVALID_TX_PDU_ID);
        return CANIF_NO_NOTIFICATION;
    }
#endif

    /* Check if notification status is enabled for this PDU */
    if (CanIf_ConfigPtr_Local->CanIfTxPduConfig[CanTxPduId].TxPduReadNotifyStatus == STD_ON)
    {
        NotifStatus = CanIf_TxNotificationStatus[CanTxPduId];
        /* Reset notification status after reading */
        CanIf_TxNotificationStatus[CanTxPduId] = CANIF_NO_NOTIFICATION;
    }
    
    return NotifStatus;
}
#endif

/**
 * @brief   Returns the indication status of a specific CAN Rx L-PDU
 */
#if (CANIF_READ_RX_PDU_NOTIFY_STATUS_API == STD_ON)
CanIf_NotifStatusType CanIf_ReadRxNotifStatus(PduIdType CanRxPduId)
{
    CanIf_NotifStatusType NotifStatus = CANIF_NO_NOTIFICATION;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_READ_RX_NOTIF_STATUS, CANIF_E_UNINIT);
        return CANIF_NO_NOTIFICATION;
    }
    
    /* Validate Rx PDU ID */
    if (E_NOT_OK == CanIf_ValidateRxPduId(CanRxPduId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_READ_RX_NOTIF_STATUS, CANIF_E_INVALID_RX_PDU_ID);
        return CANIF_NO_NOTIFICATION;
    }
#endif

    /* Check if notification status is enabled for this PDU */
    if (CanIf_ConfigPtr_Local->CanIfRxPduConfig[CanRxPduId].RxPduReadNotifyStatus == STD_ON)
    {
        NotifStatus = CanIf_RxNotificationStatus[CanRxPduId];
        /* Reset notification status after reading */
        CanIf_RxNotificationStatus[CanRxPduId] = CANIF_NO_NOTIFICATION;
    }
    
    return NotifStatus;
}
#endif

/*======================== Services for Dynamic CAN ID Support ============================*/

/**
 * @brief   Reconfigures the corresponding Static or Dynamic Tx L-PDU with a new CAN ID
 */
#if (CANIF_SET_DYNAMIC_TX_ID_API == STD_ON)
Std_ReturnType CanIf_SetDynamicTxId(PduIdType CanTxPduId, Can_IdType CanId)
{
    Std_ReturnType RetVal = E_NOT_OK;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_DYNAMIC_TX_ID, CANIF_E_UNINIT);
        return E_NOT_OK;
    }
    
    /* Validate Tx PDU ID */
    if (E_NOT_OK == CanIf_ValidateTxPduId(CanTxPduId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_DYNAMIC_TX_ID, CANIF_E_INVALID_TX_PDU_ID);
        return E_NOT_OK;
    }
#endif

    /* Check if PDU supports dynamic CAN ID */
    if (CanIf_ConfigPtr_Local->CanIfTxPduConfig[CanTxPduId].TxPduType == CANIF_DYNAMIC)
    {
        /* Set new CAN ID */
        CanIf_DynamicTxIds[CanTxPduId] = CanId;
        RetVal = E_OK;
    }
    else
    {
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_DYNAMIC_TX_ID, CANIF_E_PARAM_CANID);
#endif
    }
    
    return RetVal;
}
#endif

/*======================== Services for Baudrate Configuration ============================*/

/**
 * @brief   This service shall set the baud rate configuration of the CAN controller
 */
#if (CANIF_SET_BAUDRATE_API == STD_ON)
Std_ReturnType CanIf_SetBaudrate(uint8 ControllerId, uint16 BaudRateConfigID)
{
    Std_ReturnType RetVal = E_NOT_OK;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_BAUDRATE, CANIF_E_UNINIT);
        return E_NOT_OK;
    }
    
    /* Validate controller ID */
    if (E_NOT_OK == CanIf_ValidateControllerId(ControllerId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_SET_BAUDRATE, CANIF_E_PARAM_CONTROLLERID);
        return E_NOT_OK;
    }
#endif

    /* Call CAN driver */
    RetVal = Can_SetBaudrate(CanIf_ConfigPtr_Local->CanIfCtrlConfig[ControllerId].CanControllerId, BaudRateConfigID);
    
    return RetVal;
}
#endif

/*=============================== Callback Functions ==================================*/

/**
 * @brief   Indicates a successful reception of a received CAN Rx L-PDU
 */
void CanIf_RxIndication(const Can_HwType* Mailbox, const PduInfoType* PduInfoPtr)
{
    PduIdType RxPduId;
    uint8 ControllerId;
    const CanIf_RxPduConfigType* RxPduConfig;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_RX_INDICATION, CANIF_E_UNINIT);
        return;
    }
    
    /* Check parameters */
    if ((NULL_PTR == Mailbox) || (NULL_PTR == PduInfoPtr))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_RX_INDICATION, CANIF_E_PARAM_POINTER);
        return;
    }
#endif

    /* Find controller ID from HRH */
    ControllerId = CanIf_ConfigPtr_Local->CanIfHrhConfig[Mailbox->Hoh].ControllerId;
    
    /* Check PDU mode */
    if ((CanIf_PduChannelModes[ControllerId] == CANIF_ONLINE) || 
        (CanIf_PduChannelModes[ControllerId] == CANIF_TX_OFFLINE))
    {
        /* Find matching Rx PDU */
        RxPduId = CanIf_FindRxPduByCanId(Mailbox->CanId, ControllerId);
        
        if (CANIF_INVALID_PDU_ID != RxPduId)
        {
            RxPduConfig = &CanIf_ConfigPtr_Local->CanIfRxPduConfig[RxPduId];
            
#if (CANIF_READ_RX_PDU_DATA_API == STD_ON)
            /* Store data in Rx buffer if enabled */
            if (RxPduConfig->RxPduReadData == STD_ON)
            {
                uint8 DataIndex;
                CanIf_RxBufferLength[RxPduId] = PduInfoPtr->SduLength;
                
                for (DataIndex = 0U; DataIndex < PduInfoPtr->SduLength; DataIndex++)
                {
                    CanIf_RxBuffer[RxPduId][DataIndex] = PduInfoPtr->SduDataPtr[DataIndex];
                }
            }
#endif

#if (CANIF_READ_RX_PDU_NOTIFY_STATUS_API == STD_ON)
            /* Set notification status if enabled */
            if (RxPduConfig->RxPduReadNotifyStatus == STD_ON)
            {
                CanIf_RxNotificationStatus[RxPduId] = CANIF_TX_RX_NOTIFICATION;
            }
#endif

            /* Call upper layer indication function */
            if (NULL_PTR != RxPduConfig->RxPduUserRxIndicationUL)
            {
                RxPduConfig->RxPduUserRxIndicationUL(RxPduId, PduInfoPtr);
            }
        }
    }
}

/**
 * @brief   Confirms the successful transmission of a CAN TxPDU
 */
void CanIf_TxConfirmation(PduIdType CanTxPduId)
{
    const CanIf_TxPduConfigType* TxPduConfig;
    
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_TX_CONFIRMATION, CANIF_E_UNINIT);
        return;
    }
    
    /* Validate Tx PDU ID */
    if (E_NOT_OK == CanIf_ValidateTxPduId(CanTxPduId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_TX_CONFIRMATION, CANIF_E_INVALID_TX_PDU_ID);
        return;
    }
#endif

    TxPduConfig = &CanIf_ConfigPtr_Local->CanIfTxPduConfig[CanTxPduId];
    
    /* Check if Tx confirmation is enabled */
    if (TxPduConfig->TxPduTxConfirmation == STD_ON)
    {
#if (CANIF_READ_TX_PDU_NOTIFY_STATUS_API == STD_ON)
        /* Set notification status if enabled */
        if (TxPduConfig->TxPduReadNotifyStatus == STD_ON)
        {
            CanIf_TxNotificationStatus[CanTxPduId] = CANIF_TX_RX_NOTIFICATION;
        }
#endif

        /* Call upper layer confirmation function */
        if (NULL_PTR != TxPduConfig->TxPduUserTxConfirmationUL)
        {
            TxPduConfig->TxPduUserTxConfirmationUL(CanTxPduId);
        }
    }
}

/**
 * @brief   Indicates a controller state transition referring to the corresponding CAN controller
 */
void CanIf_ControllerModeIndication(uint8 ControllerId, CanIf_ControllerModeType ControllerMode)
{
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_CONTROLLER_MODE_INDICATION, CANIF_E_UNINIT);
        return;
    }
    
    /* Validate controller ID */
    if (E_NOT_OK == CanIf_ValidateControllerId(ControllerId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_CONTROLLER_MODE_INDICATION, CANIF_E_PARAM_CONTROLLERID);
        return;
    }
#endif

    /* Update controller state */
    CanIf_ControllerStates[ControllerId] = ControllerMode;
    
    /* Set PDU mode based on controller mode */
    if (ControllerMode == CANIF_CS_STARTED)
    {
        CanIf_PduChannelModes[ControllerId] = CANIF_ONLINE;
    }
    else
    {
        CanIf_PduChannelModes[ControllerId] = CANIF_OFFLINE;
    }
}

/**
 * @brief   Indicates that the CAN controller is in Bus-Off state
 */
void CanIf_ControllerBusOff(uint8 ControllerId)
{
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    /* Check if module is initialized */
    if (FALSE == CanIf_IsInitialized())
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_CONTROLLER_BUSOFF, CANIF_E_UNINIT);
        return;
    }
    
    /* Validate controller ID */
    if (E_NOT_OK == CanIf_ValidateControllerId(ControllerId))
    {
        Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_CONTROLLER_BUSOFF, CANIF_E_PARAM_CONTROLLERID);
        return;
    }
#endif

    /* Set controller to stopped state */
    CanIf_ControllerStates[ControllerId] = CANIF_CS_STOPPED;
    
    /* Set PDU mode to offline */
    CanIf_PduChannelModes[ControllerId] = CANIF_OFFLINE;
}

/*=============================== Scheduled Functions ====================================*/

/**
 * @brief   This service performs the main function of CanIf
 */
void CanIf_MainFunction(void)
{
    /* This function would handle periodic tasks like:
     * - Wakeup validation
     * - Mode change handling
     * - Error handling
     * - Buffer management
     * 
     * For this basic implementation, no periodic processing is needed
     */
}
