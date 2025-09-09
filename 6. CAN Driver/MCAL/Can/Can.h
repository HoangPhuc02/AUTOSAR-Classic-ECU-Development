/****************************************************************************************
*                               CAN.H                                                   *
****************************************************************************************
* File Name   : Can.h
* Module      : Controller Area Network (CAN)
* Description : AUTOSAR Can driver header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/
#ifndef CAN_H
#define CAN_H

#include "Can_Types.h"
#include "Can_Cfg.h"

#if (CAN_DEV_ERROR_DETECT == STD_ON)
    #include "Det.h"
#endif

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CAN_VENDOR_ID                     (0x0001U)
#define CAN_MODULE_ID                     (0x007BU)
#define CAN_INSTANCE_ID                   (0x00U)
#define CAN_AR_RELEASE_MAJOR_VERSION      (4U)
#define CAN_AR_RELEASE_MINOR_VERSION      (4U)
#define CAN_AR_RELEASE_REVISION_VERSION   (0U)
#define CAN_SW_MAJOR_VERSION              (1U)
#define CAN_SW_MINOR_VERSION              (0U)
#define CAN_SW_PATCH_VERSION              (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if Can.h file and Std_Types.h file are of the same vendor */
#if (CAN_VENDOR_ID != STD_TYPES_VENDOR_ID)
    #error "Can.h and Std_Types.h have different vendor ids"
#endif

/* Check if Can.h file and Can_Types.h file are of the same vendor */
#if (CAN_VENDOR_ID != CAN_TYPES_VENDOR_ID)
    #error "Can.h and Can_Types.h have different vendor ids"
#endif

/* Check if Can.h file and Can_Cfg.h file are of the same vendor */
#if (CAN_VENDOR_ID != CAN_CFG_VENDOR_ID)
    #error "Can.h and Can_Cfg.h have different vendor ids"
#endif

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/**
 * @brief   API Service IDs
 * @details Service IDs for CAN driver API functions
 */
#define CAN_INIT_SID                            (0x00U)
#define CAN_GET_VERSION_INFO_SID                (0x07U)
#define CAN_DEINIT_SID                          (0x10U)

#define CAN_SET_BAUD_RATE_SID                   (0x0FU)
#define CAN_SET_CONTROLLER_MODE_SID             (0x03U)
#define CAN_DISABLE_CONTROLLER_INTERRUPTS_SID   (0x04U)
#define CAN_ENABLE_CONTROLLER_INTERRUPTS_SID    (0x05U)
#define CAN_CHECK_WAKEUP_SID                    (0x0BU)
#define CAN_GET_CONTROLLER_ERROR_STATE_SID      (0x11U)
#define CAN_GET_CONTROLLER_MODE_SID             (0x12U)
#define CAN_GET_CONTROLLER_RX_ERROR_COUNTER_SID (0x30U)
#define CAN_GET_CONTROLLER_TX_ERROR_COUNTER_SID (0x31U)
#define CAN_GET_CURRENT_TIME_SID                (0x32U)
#define CAN_ENABLE_EGRESS_TIMESTAMP_SID         (0x33U)
#define CAN_GET_EGRESS_TIMESTAMP_SID            (0x34U)
#define CAN_GET_INGRESS_TIMESTAMP_SID           (0x35U)

#define CAN_WRITE_SID                           (0x06U)

#define CAN_CALLOUT_NAME_SID                    (0x20U)

#define CAN_MAIN_FUNCTION_WRITE_SID             (0x01U)
#define CAN_MAIN_FUNCTION_READ_SID              (0x08U)
#define CAN_MAIN_FUNCTION_BUSOFF_SID            (0x09U)
#define CAN_MAIN_FUNCTION_WAKEUP_SID            (0x0AU)
#define CAN_MAIN_FUNCTION_MODE_SID              (0x0CU)

/**
 * @brief   Development Error Codes
 * @details Error codes for CAN driver development error detection
 * [SWS_Can_91019] 
 */
#define CAN_E_PARAM_POINTER                     (0x01U)     /**< API Service called with wrong parameter */
#define CAN_E_PARAM_HANDLE                      (0x02U)     /**< API Service called with wrong parameter */
#define CAN_E_PARAM_DATA_LENGTH                 (0x03U)     /**< API Service called with wrong parameter */
#define CAN_E_PARAM_CONTROLLER                  (0x04U)     /**< API Service used without initialization */
#define CAN_E_UNINIT                            (0x05U)     /**< Invalid transition for the current mode */
#define CAN_E_TRANSITION                        (0x06U)     /**< Parameter Baudrate has an invalid value */
#define CAN_E_PARAM_BAUDRATE                    (0x07U)     /**< Invalid configuration set selection */
#define CAN_E_INIT_FAILED                       (0x09U)     /**< API service called with invalid PDU ID */
#define CAN_E_PARAM_LPDU                        (0x0AU)     /**< API service called with invalid PDU ID */

/**
 * @brief   Runtime Error Codes
 * @details Error codes for CAN driver runtime error detection
 * [SWS_Can_91020] 
 */
#define CAN_E_DATALOST                         (0x01U)     /**< Received CAN message is lost */



extern const Can_ConfigType* Can_ConfigPtr;
/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/


/*======================= Services affecting the complete hardware unit ==========================*/
/**
 * @brief   Initializes the CAN Hardware Units and Controllers
 * @details This function initializes the CAN Hardware Units and Controllers according to the
 *          configuration set referenced by the parameter Config.
 * @param[in] Config - Pointer to driver configuration
 * @note    Synchronous, Non-reentrant
 * SWS_Can_00223
 */
void Can_Init(const Can_ConfigType* Config);

/**
 * @brief   Returns version information of this module
 * @details This function returns the version information of this module. The version information
 *          includes: Module Id, Vendor Id, Vendor specific version numbers (BSW00407).
 * @param[out] versioninfo - Pointer to where to store the version information of this module
 * @note    Synchronous, Reentrant
 * SWS_Can_00224
 */
#if (CAN_VERSION_INFO_API == STD_ON)
void Can_GetVersionInfo(Std_VersionInfoType* versioninfo);
#endif

/**
 * @brief   De-initializes the module
 * @details This function de-initializes the module.
 * @note    Synchronous, Non-reentrant
 * SWS_Can_00228
 */
#if (CAN_DEINIT_API == STD_ON)
void Can_DeInit(void);
#endif

/*======================== Services affecting one single CAN Controller =========================*/

/**
 * @brief   This service shall set the baud rate configuration of the CAN controller
 * @details This service shall set the baud rate configuration of the CAN controller.
 *          Depending on necessary baud rate modifications the controller might have to reset.
 * @param[in] Controller - CAN controller for which the status shall be changed
 * @param[in] BaudRateConfigID - References a baud rate configuration by ID
 * @return  Std_ReturnType
 *          E_OK: Service request accepted
 *          E_NOT_OK: Service request not accepted
 * @note    Synchronous, Non-reentrant
 * SWS_Can_00491
 */
#if (CAN_SET_BAUDRATE_API == STD_ON)
Std_ReturnType Can_SetBaudrate(uint8 Controller, uint16 BaudRateConfigID);
#endif

/**
 * @brief   This function performs the change of the CAN controller state
 * @details This function performs the change of the CAN controller state.
 * @param[in] Controller - CAN controller for which the status shall be changed
 * @param[in] Transition - Transition value to request new CAN controller state
 * @return  Std_ReturnType - CAN_OK: request accepted, CAN_NOT_OK: request not accepted
 * @note    Asynchronous, Non-reentrant
 * SWS_Can_00230
 */
Std_ReturnType Can_SetControllerMode(uint8 Controller, Can_ControllerStateType Transition);


// Interrupt function 
/**
 * @brief   This function disables all interrupts for this CAN controller
 * @details This function disables all interrupts for this CAN controller.
 * @param[in] Controller - CAN controller for which interrupts shall be disabled
 * @note    Synchronous, Reentrant
 * SWS_Can_00231
 */
void Can_DisableControllerInterrupts(uint8 Controller);

/**
 * @brief   This function enables all allowed interrupts for this CAN controller
 * @details This function enables all allowed interrupts for this CAN controller.
 * @param[in] Controller - CAN controller for which interrupts shall be re-enabled
 * @note    Synchronous, Reentrant
 * SWS_Can_00232
 */
void Can_EnableControllerInterrupts(uint8 Controller);

/**
 * @brief   This function checks if a wakeup has occurred for the given controller
 * @details This function checks if a wakeup has occurred for the given controller.
 * @param[in] Controller - Controller to be checked for a wakeup
 * @return  Std_ReturnType
 *          E_OK: Wakeup was detected for the given controller
 *          E_NOT_OK: No wakeup was detected for the given controller
 * @note    Synchronous, Reentrant
 * SWS_Can_00360
 */
#if (CAN_WAKEUP_SUPPORT == STD_ON)
Std_ReturnType Can_CheckWakeup(uint8 Controller);
#endif

/**
 * @brief   This function obtains the error state of the CAN controller
 * @details This service obtains the error state of the CAN controller.
 * @param[in] ControllerId - Abstracted CanIf ControllerId which is assigned to a CAN controller
 * @param[out] ErrorStatePtr - Pointer to a memory location where the error state of the CAN controller will be stored
 * @return  Std_ReturnType
 *          E_OK: Error state request has been accepted
 *          E_NOT_OK: Error state request has not been accepted
 * @note    Synchronous, Reentrant
 * SWS_Can_00489
 */
#if (CAN_GET_CONTROLLER_ERROR_STATE_API == STD_ON)
Std_ReturnType Can_GetControllerErrorState(uint8 ControllerId, Can_ErrorStateType* ErrorStatePtr);
#endif
/**
 * @brief   This function reports about the current status of the requested CAN controller
 * @details This service reports about the current status of the requested CAN controller.
 * @param[in] Controller - CAN controller for which the status shall be requested
 * @param[out] ControllerModePtr - Pointer to a memory location where the current mode of the CAN controller will be stored
 * @return  Std_ReturnType
 *          E_OK: Controller mode request has been accepted
 *          E_NOT_OK: Controller mode request has not been accepted
 * @note    Synchronous, Non-reentrant
 * SWS_Can_00492
 */
#if (CAN_GET_CONTROLLER_MODE_API == STD_ON)
Std_ReturnType Can_GetControllerMode(uint8 Controller, Can_ControllerStateType* ControllerModePtr);
#endif

/**
 * @brief   Returns the Rx error counter for a CAN controller
 * @details This service obtains the Rx error counter for a CAN controller.
 * @param[in] ControllerId - Abstracted CanIf ControllerId which is assigned to a CAN controller
 * @param[out] RxErrorCounterPtr - Pointer to a memory location where the current Rx error counter of the CAN controller will be stored
 * @return  Std_ReturnType
 *          E_OK: Rx error counter available
 *          E_NOT_OK: Wrong ControllerId, or Rx error counter not available
 * @note    Synchronous, Reentrant
 * SWS_Can_00493
 */
#if (CAN_GET_CONTROLLER_RX_ERROR_COUNTER_API == STD_ON)
Std_ReturnType Can_GetControllerRxErrorCounter(uint8 ControllerId, uint8* RxErrorCounterPtr);
#endif

/**
 * @brief   Returns the Tx error counter for a CAN controller
 * @details This service obtains the Tx error counter for a CAN controller.
 * @param[in] ControllerId - Abstracted CanIf ControllerId which is assigned to a CAN controller
 * @param[out] TxErrorCounterPtr - Pointer to a memory location where the current Tx error counter of the CAN controller will be stored
 * @return  Std_ReturnType
 *          E_OK: Tx error counter available
 *          E_NOT_OK: Wrong ControllerId, or Tx error counter not available
 * @note    Synchronous, Reentrant
 * SWS_Can_00494
 */
#if (CAN_GET_CONTROLLER_TX_ERROR_COUNTER_API == STD_ON)
Std_ReturnType Can_GetControllerTxErrorCounter(uint8 ControllerId, uint8* TxErrorCounterPtr);
#endif

/**
 * @brief   Returns the current time for a CAN controller
 * @details This service obtains the current time for a CAN controller.
 * @param[in] ControllerId - Abstracted CanIf ControllerId which is assigned to a CAN controller
 * @param[out] timeStampPtr - Pointer to a memory location where the current time of the CAN controller will be stored
 * @return  Std_ReturnType
 *          E_OK: Current time available
 *          E_NOT_OK: Wrong ControllerId, or current time not available
 * @note    Synchronous, Reentrant
 * SWS_Can_00495
 */
#if (CAN_GET_CURRENT_TIME_API == STD_ON)
Std_ReturnType Can_GetCurrentTime(uint8 ControllerId, Can_TimeStampType* timeStampPtr);
#endif

/**
 * @brief   Activates egress time stamping for a dedicated L-PDU
 * @details This service activates egress time stamping for a dedicated L-PDU.
 * @param[in] Hth - Information which HW-transmit handle shall be used for transmission
 * @note    Synchronous, Non-reentrant
 * SWS_Can_00497
 */
#if (CAN_ENABLE_EGRESS_TIME_STAMP_API == STD_ON)
void Can_EnableEgressTimeStamp(Can_HwHandleType Hth);
#endif

/**
 * @brief   Returns the egress time stamp for a dedicated L-PDU
 * @details This service obtains the egress time stamp for a dedicated L-PDU.
 * @param[in] TxPduId - L-PDU handle of CAN L-PDU for which the egress time stamp shall be returned
 * @param[in] Hth - Information which HW-transmit handle was used for transmission
 * @param[out] timeStampPtr - Pointer to a memory location where the egress time stamp will be stored
 * @return  Std_ReturnType
 *          E_OK: Egress time stamp available
 *          E_NOT_OK: Wrong TxPduId, or egress time stamp not available
 * @note    Synchronous, Reentrant
 * SWS_Can_00498
 */
#if (CAN_GET_EGRESS_TIME_STAMP_API == STD_ON)
Std_ReturnType Can_GetEgressTimeStamp(PduIdType TxPduId, Can_HwHandleType Hth, Can_TimeStampType* timeStampPtr);
#endif

/**
 * @brief   Returns the ingress time stamp for a dedicated L-PDU
 * @details This service obtains the ingress time stamp for a dedicated L-PDU.
 * @param[in] Hrh - Information which HW-receive handle was used for reception
 * @param[out] timeStampPtr - Pointer to a memory location where the ingress time stamp will be stored
 * @return  Std_ReturnType
 *          E_OK: Ingress time stamp available
 *          E_NOT_OK: Wrong Hrh, or ingress time stamp not available
 * @note    Synchronous, Reentrant
 * SWS_Can_00499
 */
#if (CAN_GET_INGRESS_TIME_STAMP_API == STD_ON)
Std_ReturnType Can_GetIngressTimeStamp(Can_HwHandleType Hrh, Can_TimeStampType* timeStampPtr);
#endif

/*=========================== Services affecting a Hardware Handle ==============================*/

/**
 * @brief   This function is called by CanIf to pass a CAN message to CanDrv for transmission
 * @details This function is called by CanIf to pass a CAN message to CanDrv for transmission.
 * @param[in] Hth - Information which HW-transmit handle shall be used for transmit
 * @param[in] PduInfo - Pointer to SDU user memory, DLC and Identifier
 * @return  Std_ReturnType
 *          E_OK: Write command has been accepted
 *          E_NOT_OK: Development error occurred
 *          CAN_BUSY: No TX hardware buffer available or pre-emptive call of Can_Write that can't be implemented re-entrant
 * @note    Synchronous, Reentrant (for different HTHs)
 * SWS_Can_00233
 */
Std_ReturnType Can_Write(Can_HwHandleType Hth, const Can_PduType* PduInfo);

/* =================================== Call-back notifications ==================================*/

/**
 * @brief   CAN L-PDU callout
 * @details This callout is called by the CAN driver when a CAN L-PDU has been received.
 *          This callout is configurable (ON/OFF) for each HOH.
 * @param[in] Hrh - Hardware Receive Handle
 * @param[in] CanId - Standard/Extended CAN ID of CAN L-PDU
 * @param[in] CanDataLength - Data Length of the received CAN L-PDU
 * @param[in] CanSduPtr - Pointer to L-SDU buffer of the CAN L-PDU
 * @return  boolean
 *          TRUE: L-PDU is accepted by the upper layer
 *          FALSE: L-PDU is not accepted by the upper layer
 * @note    Synchronous, Non-reentrant
 * SWS_Can_00495
 */
#if (CAN_LPDU_CALLOUT_SUPPORT == STD_ON)
boolean Can_LPdu_Callout(uint8 Hrh, Can_IdType CanId, uint8 CanDataLength, const uint8* CanSduPtr);
#endif
/* ===================================== Scheduled functions ====================================*/

/** This function performs the polling of TX confirmation when CAN_TX_PROCESSING is set to
 POLLING. */
/**
 * @brief   This function performs the polling of TX confirmation
 * @details This function performs the polling of TX confirmation when CAN_TX_PROCESSING is set to POLLING.
 * @note    Synchronous, Non-reentrant
 * SWS_Can_00225
 */
#if (CAN_TX_PROCESSING == POLLING)
void Can_MainFunction_Write(void);
#endif

/**
 * @brief   This function performs the polling of RX indications
 * @details This function performs the polling of RX indications when CAN_RX_PROCESSING is set to POLLING.
 * @note    Synchronous, Non-reentrant
 * SWS_Can_00226
 */
#if (CAN_RX_PROCESSING == POLLING)
void Can_MainFunction_Read(void);
#endif

/**
 * @brief   This function performs the polling of bus-off events
 * @details This function performs the polling of bus-off events that are configured statically as 'to be polled'.
 * @note    Synchronous, Non-reentrant
 * SWS_Can_00227
 */
#if (CAN_BUSOFF_PROCESSING == POLLING)
void Can_MainFunction_BusOff(void);
#endif

/**
 * @brief   This function performs the polling of wake-up events
 * @details This function performs the polling of wake-up events that are configured statically as 'to be polled'.
 * @note    Synchronous, Non-reentrant
 * SWS_Can_00369
 */
#if (CAN_WAKEUP_PROCESSING == POLLING)
void Can_MainFunction_Wakeup(void);
#endif

/**
 * @brief   This function performs the polling of CAN controller mode transitions
 * @details This function performs the polling of CAN controller mode transitions that are configured statically as 'to be polled'.
 * @note    Synchronous, Non-reentrant
 * SWS_Can_00373
 */
#if (CAN_MODE_PROCESSING == POLLING)
void Can_MainFunction_Mode(void);
#endif


#endif /* CAN_H */