/****************************************************************************************
*                               CANIF.H                                                *
****************************************************************************************
* File Name   : CanIf.h
* Module      : CAN Interface (CanIf)
* Description : AUTOSAR CanIf driver header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 24/08/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef CANIF_H
#define CANIF_H

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "Std_Types.h"
#include "CanIf_Types.h"
#include "CanIf_Cfg.h"
#include "Can.h"

#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    #include "Det.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CANIF_VENDOR_ID                     (0x0001U)
#define CANIF_MODULE_ID                     (0x007CU)
#define CANIF_INSTANCE_ID                   (0x00U)
#define CANIF_AR_RELEASE_MAJOR_VERSION      (4U)
#define CANIF_AR_RELEASE_MINOR_VERSION      (4U)
#define CANIF_AR_RELEASE_REVISION_VERSION   (0U)
#define CANIF_SW_MAJOR_VERSION              (1U)
#define CANIF_SW_MINOR_VERSION              (0U)
#define CANIF_SW_PATCH_VERSION              (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if CanIf.h file and Std_Types.h file are of the same vendor */
#if (CANIF_VENDOR_ID != STD_TYPES_VENDOR_ID)
    #error "CanIf.h and Std_Types.h have different vendor ids"
#endif

/* Check if CanIf.h file and CanIf_Types.h file are of the same vendor */
#if (CANIF_VENDOR_ID != CANIF_TYPES_VENDOR_ID)
    #error "CanIf.h and CanIf_Types.h have different vendor ids"
#endif

/* Check if CanIf.h file and CanIf_Cfg.h file are of the same vendor */
#if (CANIF_VENDOR_ID != CANIF_CFG_VENDOR_ID)
    #error "CanIf.h and CanIf_Cfg.h have different vendor ids"
#endif

/* Check AUTOSAR version compatibility */
#if ((CANIF_AR_RELEASE_MAJOR_VERSION != CANIF_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (CANIF_AR_RELEASE_MINOR_VERSION != CANIF_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (CANIF_AR_RELEASE_REVISION_VERSION != CANIF_TYPES_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR Version Numbers of CanIf.h and CanIf_Types.h are different"
#endif

/*==================================================================================================
*                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       DEFINES AND MACROS
==================================================================================================*/

/* API Service IDs */
#define CANIF_SID_INIT                          (0x01U)
#define CANIF_SID_DEINIT                        (0x02U)
#define CANIF_SID_SET_CONTROLLER_MODE           (0x03U)
#define CANIF_SID_GET_CONTROLLER_MODE           (0x04U)
#define CANIF_SID_GET_CONTROLLER_ERROR_STATE    (0x4BU)
#define CANIF_SID_TRANSMIT                      (0x05U)
#define CANIF_SID_CANCEL_TX_REQUEST             (0x06U)
#define CANIF_SID_READ_RX_PDU_DATA              (0x07U)
#define CANIF_SID_READ_TX_NOTIF_STATUS          (0x08U)
#define CANIF_SID_READ_RX_NOTIF_STATUS          (0x09U)
#define CANIF_SID_SET_PDU_MODE                  (0x0AU)
#define CANIF_SID_GET_PDU_MODE                  (0x0BU)
#define CANIF_SID_GET_VERSION_INFO              (0x0CU)
#define CANIF_SID_SET_DYNAMIC_TX_ID             (0x0DU)
#define CANIF_SID_SET_TRANSCEIVER_MODE          (0x0EU)
#define CANIF_SID_GET_TRANSCEIVER_MODE          (0x0FU)
#define CANIF_SID_GET_TRANSCEIVER_ERROR_STATE   (0x10U)
#define CANIF_SID_SET_TRANSCEIVER_WAKEUP_MODE   (0x11U)
#define CANIF_SID_SET_BAUDRATE                  (0x27U)
#define CANIF_SID_CONTROLLER_BUSOFF             (0x16U)
#define CANIF_SID_RX_INDICATION                 (0x14U)
#define CANIF_SID_TX_CONFIRMATION               (0x13U)
#define CANIF_SID_CONTROLLER_MODE_INDICATION    (0x17U)

/* DET Error Codes */
#define CANIF_E_PARAM_CANID                     (0x01U)
#define CANIF_E_PARAM_DLC                       (0x02U)
#define CANIF_E_PARAM_HRH                       (0x03U)
#define CANIF_E_PARAM_LPDU                      (0x04U)
#define CANIF_E_PARAM_CONTROLLER                (0x05U)
#define CANIF_E_PARAM_CONTROLLERID              (0x06U)
#define CANIF_E_PARAM_WAKEUPSOURCE              (0x07U)
#define CANIF_E_PARAM_TRCV                      (0x08U)
#define CANIF_E_PARAM_TRCVMODE                  (0x09U)
#define CANIF_E_PARAM_TRCVWAKEUPMODE            (0x0AU)
#define CANIF_E_PARAM_PDU_MODE                  (0x0BU)
#define CANIF_E_PARAM_POINTER                   (0x0CU)
#define CANIF_E_UNINIT                          (0x30U)
#define CANIF_E_NOK_NOSUPPORT                   (0x31U)
#define CANIF_E_INVALID_TX_PDU_ID               (0x50U)
#define CANIF_E_INVALID_RX_PDU_ID               (0x60U)
#define CANIF_E_INVALID_DLC                     (0x61U)
#define CANIF_E_STOPPED                         (0x70U)
#define CANIF_E_NOT_SLEEP                       (0x71U)
#define CANIF_E_WAKEUP_VALIDATION               (0x72U)

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
*                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
*                                     FUNCTION PROTOTYPES
==================================================================================================*/

/*================================ Services for CanIf Initialization =================================*/

/**
 * @brief   Initializes the CanIf module
 * @details This function initializes the CanIf module according to the configuration set
 *          referenced by the parameter ConfigPtr.
 * @param[in] ConfigPtr - Pointer to driver configuration
 * @note    Synchronous, Non-reentrant
 * SWS_CANIF_00001
 */
void CanIf_Init(const CanIf_ConfigType* ConfigPtr);

/**
 * @brief   De-initializes the CanIf module
 * @details This function de-initializes the CanIf module.
 * @note    Synchronous, Non-reentrant
 * SWS_CANIF_00003
 */
void CanIf_DeInit(void);

/**
 * @brief   Returns version information of this module
 * @details This function returns the version information of this module. The version information
 *          includes: Module Id, Vendor Id, Vendor specific version numbers.
 * @param[out] VersionInfoPtr - Pointer to where to store the version information of this module
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00158
 */
#if (CANIF_VERSION_INFO_API == STD_ON)
void CanIf_GetVersionInfo(Std_VersionInfoType* VersionInfoPtr);
#endif

/*======================== Services for Controller Mode Control ============================*/

/**
 * @brief   Calls the corresponding CAN Driver service for changing of the CAN controller mode
 * @details This service calls Can_SetControllerMode for changing of the CAN controller mode.
 * @param[in] ControllerId - Abstracted CanIf ControllerId which is assigned to a CAN controller
 * @param[in] ControllerMode - Requested controller mode
 * @return  Std_ReturnType
 *          E_OK: Controller mode request has been accepted
 *          E_NOT_OK: Controller mode request has not been accepted
 * @note    Synchronous, Non-reentrant
 * SWS_CANIF_00003
 */
Std_ReturnType CanIf_SetControllerMode(uint8 ControllerId, CanIf_ControllerModeType ControllerMode);

/**
 * @brief   Reports about the current status of the requested CAN controller
 * @details This service reports about the current status of the requested CAN controller.
 * @param[in] ControllerId - Abstracted CanIf ControllerId which is assigned to a CAN controller
 * @param[out] ControllerModePtr - Pointer to a memory location where the current mode of the CAN controller will be stored
 * @return  Std_ReturnType
 *          E_OK: Controller mode request has been accepted
 *          E_NOT_OK: Controller mode request has not been accepted
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00229
 */
Std_ReturnType CanIf_GetControllerMode(uint8 ControllerId, CanIf_ControllerModeType* ControllerModePtr);

/**
 * @brief   Reports about the current error status of the requested CAN controller
 * @details This service obtains the error state of the CAN controller.
 * @param[in] ControllerId - Abstracted CanIf ControllerId which is assigned to a CAN controller
 * @param[out] ErrorStatePtr - Pointer to a memory location where the error state of the CAN controller will be stored
 * @return  Std_ReturnType
 *          E_OK: Error state request has been accepted
 *          E_NOT_OK: Error state request has not been accepted
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00734
 */
#if (CAN_GET_CONTROLLER_ERROR_STATE_API == STD_ON)
Std_ReturnType CanIf_GetControllerErrorState(uint8 ControllerId, Can_ErrorStateType* ErrorStatePtr);
#endif

/*======================== Services for PDU Mode Control ============================*/

/**
 * @brief   Sets the requested mode at the L-PDUs of a receive CAN channel
 * @details This service sets the requested mode for the channel specified by the given controller Id.
 * @param[in] ControllerId - All PDUs of the own ECU connected to the corresponding CanIf controller
 * @param[in] PduModeRequest - Requested PDU mode change
 * @return  Std_ReturnType
 *          E_OK: Request for mode transition has been accepted
 *          E_NOT_OK: Request for mode transition has not been accepted
 * @note    Synchronous, Non-reentrant
 * SWS_CANIF_00341
 */
Std_ReturnType CanIf_SetPduMode(uint8 ControllerId, CanIf_PduModeType PduModeRequest);

/**
 * @brief   Reports the current mode of a requested PDU channel
 * @details This service reports the current mode for the channel specified by the given controller Id.
 * @param[in] ControllerId - All PDUs of the own ECU connected to the corresponding CanIf controller
 * @param[out] PduModePtr - Pointer to a memory location where the current mode of the logical PDU channel will be stored
 * @return  Std_ReturnType
 *          E_OK: PDU mode request has been accepted
 *          E_NOT_OK: PDU mode request has not been accepted
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00345
 */
Std_ReturnType CanIf_GetPduMode(uint8 ControllerId, CanIf_PduModeType* PduModePtr);

/*======================== Services for PDU Transmission ============================*/

/**
 * @brief   Requests transmission of a PDU
 * @details This service initiates a request for transmission of the CAN L-PDU specified by the CanTxPduId
 *          and CAN related data in the L-SDU buffer associated with the L-PDU.
 * @param[in] TxPduId - L-PDU handle of CAN L-PDU to be transmitted
 * @param[in] PduInfoPtr - Pointer to a structure with CAN L-PDU related data
 * @return  Std_ReturnType
 *          E_OK: Transmit request has been accepted
 *          E_NOT_OK: Transmit request has not been accepted
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00005
 */
Std_ReturnType CanIf_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr);

/**
 * @brief   Cancels the transmission request
 * @details This service cancels a pending transmission request.
 * @param[in] TxPduId - L-PDU handle of CAN L-PDU to be cancelled
 * @return  Std_ReturnType
 *          E_OK: Cancellation request has been accepted
 *          E_NOT_OK: Cancellation request has not been accepted
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00101
 */
#if (CANIF_CANCEL_TX_SUPPORT_API == STD_ON)
Std_ReturnType CanIf_CancelTxRequest(PduIdType TxPduId);
#endif

/*======================== Services for PDU Reception ============================*/

/**
 * @brief   Provides the Data Length and the received data of the requested CanRxPduId
 * @details This service provides the CAN DLC and the received data of the CAN L-PDU specified by CanRxPduId.
 * @param[in] CanRxPduId - L-PDU handle of CAN L-PDU to be received
 * @param[out] PduInfoPtr - Pointer to a structure where the received CAN L-PDU related data will be stored
 * @return  Std_ReturnType
 *          E_OK: Request for L-SDU data has been accepted
 *          E_NOT_OK: Request for L-SDU data has not been accepted
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00194
 */
#if (CANIF_READ_RX_PDU_DATA_API == STD_ON)
Std_ReturnType CanIf_ReadRxPduData(PduIdType CanRxPduId, PduInfoType* PduInfoPtr);
#endif

/*======================== Services for Notification Status ============================*/

/**
 * @brief   Returns the confirmation status of a specific static or dynamic CAN Tx L-PDU
 * @details This service returns the confirmation status (confirmation occurred or not) of a specific
 *          static or dynamic CAN Tx L-PDU, requested by the CanTxPduId.
 * @param[in] CanTxPduId - L-PDU handle of CAN L-PDU for which the status shall be requested
 * @return  CanIf_NotifStatusType - Current notification status of the corresponding CAN Tx L-PDU
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00202
 */
#if (CANIF_READ_TX_PDU_NOTIFY_STATUS_API == STD_ON)
CanIf_NotifStatusType CanIf_ReadTxNotifStatus(PduIdType CanTxPduId);
#endif

/**
 * @brief   Returns the indication status of a specific CAN Rx L-PDU
 * @details This service returns the indication status (indication occurred or not) of a specific CAN Rx L-PDU,
 *          requested by the CanRxPduId.
 * @param[in] CanRxPduId - L-PDU handle of CAN L-PDU for which the status shall be requested
 * @return  CanIf_NotifStatusType - Current notification status of the corresponding CAN Rx L-PDU
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00230
 */
#if (CANIF_READ_RX_PDU_NOTIFY_STATUS_API == STD_ON)
CanIf_NotifStatusType CanIf_ReadRxNotifStatus(PduIdType CanRxPduId);
#endif

/*======================== Services for Dynamic CAN ID Support ============================*/

/**
 * @brief   Reconfigures the corresponding Static or Dynamic Tx L-PDU with a new CAN ID
 * @details This service reconfigures the corresponding Static or Dynamic Tx L-PDU with a new CAN identifier.
 * @param[in] CanTxPduId - L-PDU handle of CAN L-PDU for which the identifier shall be reconfigured
 * @param[in] CanId - Standard/Extended CAN ID of CAN L-PDU
 * @return  Std_ReturnType
 *          E_OK: Request to change the Identifier has been accepted
 *          E_NOT_OK: Request to change the Identifier has not been accepted
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00189
 */
#if (CANIF_SET_DYNAMIC_TX_ID_API == STD_ON)
Std_ReturnType CanIf_SetDynamicTxId(PduIdType CanTxPduId, Can_IdType CanId);
#endif

/*======================== Services for Baudrate Configuration ============================*/

/**
 * @brief   This service shall set the baud rate configuration of the CAN controller
 * @details This service calls Can_SetBaudrate for changing the baud rate of the CAN controller.
 * @param[in] ControllerId - Abstracted CanIf ControllerId which is assigned to a CAN controller  
 * @param[in] BaudRateConfigID - References a baud rate configuration by ID
 * @return  Std_ReturnType
 *          E_OK: Service request accepted, setting of (new) baud rate started
 *          E_NOT_OK: Service request not accepted
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00867
 */
#if (CANIF_SET_BAUDRATE_API == STD_ON)
Std_ReturnType CanIf_SetBaudrate(uint8 ControllerId, uint16 BaudRateConfigID);
#endif

/*=============================== Callback Functions ==================================*/

/**
 * @brief   Indicates a successful reception of a received CAN Rx L-PDU
 * @details This service indicates a successful reception of a received CAN Rx L-PDU to the CanIf after
 *          passing all filters and validation checks.
 * @param[in] Hrh - ID of the corresponding hardware object range
 * @param[in] CanId - Standard/Extended CAN ID of CAN L-PDU that has been received
 * @param[in] CanDlc - Data length code of the received CAN L-PDU
 * @param[in] CanSduPtr - Pointer to L-SDU buffer of the CAN L-PDU that has been received
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00415
 */
void CanIf_RxIndication(const Can_HwType* Mailbox, const PduInfoType* PduInfoPtr);

/**
 * @brief   Confirms the successful transmission of a CAN TxPDU
 * @details This service confirms a successful transmission of a CAN TxPDU in a CAN L-PDU.
 * @param[in] CanTxPduId - L-PDU handle of CAN L-PDU successfully transmitted
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00396
 */
void CanIf_TxConfirmation(PduIdType CanTxPduId);

/**
 * @brief   Indicates a controller state transition referring to the corresponding CAN controller
 * @details This service indicates a controller state transition referring to the corresponding CAN controller
 *          with the abstract CanIf ControllerId.
 * @param[in] ControllerId - Abstract CanIf ControllerId which is assigned to a CAN controller
 * @param[in] ControllerMode - Mode to which the CAN controller transitioned
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00699
 */
void CanIf_ControllerModeIndication(uint8 ControllerId, CanIf_ControllerModeType ControllerMode);

/**
 * @brief   Indicates that the CAN controller is in Bus-Off state
 * @details This service indicates that the CAN controller with the abstract CanIf ControllerId is in Bus-Off state.
 * @param[in] ControllerId - Abstract CanIf ControllerId which is assigned to a CAN controller
 * @note    Synchronous, Reentrant
 * SWS_CANIF_00218
 */
void CanIf_ControllerBusOff(uint8 ControllerId);

/*=============================== Scheduled Functions ====================================*/

/**
 * @brief   This service performs the main function of CanIf
 * @details This function performs the main function of the CanIf for processing mode changes,
 *          wakeup validation, and other periodic tasks.
 * @note    Synchronous, Non-reentrant
 * SWS_CANIF_00001
 */
void CanIf_MainFunction(void);

#ifdef __cplusplus
}
#endif

#endif /* CANIF_H */
