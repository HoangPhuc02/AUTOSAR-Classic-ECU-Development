/****************************************************************************************
*                               LIN.H                                            *
****************************************************************************************
* File Name   : Lin.h
* Module      : Local Interface Network (LIN)
* Description : AUTOSAR Lin driver header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef LIN_H
#define LIN_H

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Std_Types.h"
#include "Lin_Types.h"
#include "Lin_Cfg.h"

#if (LIN_DEV_ERROR_DETECT == STD_ON)
    #include "Det.h"
#endif

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define LIN_VENDOR_ID                     (0x0001U)
#define LIN_MODULE_ID                     (0x007BU)
#define LIN_INSTANCE_ID                   (0x00U)
#define LIN_AR_RELEASE_MAJOR_VERSION      (4U)
#define LIN_AR_RELEASE_MINOR_VERSION      (4U)
#define LIN_AR_RELEASE_REVISION_VERSION   (0U)
#define LIN_SW_MAJOR_VERSION              (1U)
#define LIN_SW_MINOR_VERSION              (0U)
#define LIN_SW_PATCH_VERSION              (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if Lin.h file and Std_Types.h file are of the same vendor */
#if (LIN_VENDOR_ID != STD_TYPES_VENDOR_ID)
    #error "Lin.h and Std_Types.h have different vendor ids"
#endif

/* Check if Lin.h file and Lin_Types.h file are of the same vendor */
#if (LIN_VENDOR_ID != LIN_TYPES_VENDOR_ID)
    #error "Lin.h and Lin_Types.h have different vendor ids"
#endif

/* Check if Lin.h file and Lin_Cfg.h file are of the same vendor */
#if (LIN_VENDOR_ID != LIN_CFG_VENDOR_ID)
    #error "Lin.h and Lin_Cfg.h have different vendor ids"
#endif

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/**
 * @brief   API Service IDs
 * @details Service IDs for LIN driver API functions
 */
#define LIN_INIT_SID                            (0x00U)
#define LIN_GET_VERSION_INFO_SID                (0x01U)
#define LIN_SEND_FRAME_SID                      (0x04U)
#define LIN_GOTO_SLEEP_SID                      (0x06U)
#define LIN_WAKEUP_SID                          (0x07U)
#define LIN_GET_STATUS_SID                      (0x08U)
#define LIN_GOTO_SLEEP_INTERNAL_SID             (0x09U)
#define LIN_CHECK_WAKEUP_SID                    (0x0AU)
#define LIN_WAKEUP_INTERNAL_SID                 (0x0BU)


/**
 * @brief   Development Error Codes
 * @details Error codes for LIN driver development error detection
 */
#define LIN_E_UNINIT                (0x00U) /**< API service called without module initialization */
#define LIN_E_INVALID_CHANNEL       (0x02U) /**< API service used with an invalid or inactive channel parameter */
#define LIN_E_INVALID_POINTER       (0x03U) /**< API service called with invalid configuration pointer */
#define LIN_E_STATE_TRANSITION      (0x04U) /**< Invalid state transition for the current state */
#define LIN_E_PARAM_POINTER         (0x05U) /**< API service called with a NULL pointer */


/**
 * @brief   Module State Definitions
 * @details Definitions for module initialization state
 */
#define LIN_UNINIT                             (FALSE)
#define LIN_INIT                               (TRUE)


#define LIN_SYNC_BYTE                           0x55U /**< LIN Sync Byte */
/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
extern const Lin_ConfigType* Lin_ConfigPtr;
/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief   Initializes the LIN module
 * @details This function initializes the Lin driver with the configuration
 *          pointed to by the parameter Config.
 * 
 * @param[in]   Config   Pointer to LIN driver configuration set
 * 
 * @return      None
 * 
 * @pre         LIN driver must be uninitialized
 * @post        LIN driver is initialized and ready for group conversions
 * 
 * @note        This function must be called before any other LIN driver function
 * 
 * @implements  Lin_Init_Activity
 * [SWS_Lin_00006]
 */
void Lin_Init (const Lin_ConfigType* Config);


#if (LIN_ENABLE_CHECK_WAKEUP_API == STD_ON)
/**
 * @brief   Checks if a wakeup has occurred on the addressed LIN channel
 * @details This function checks if a wakeup event has occurred on the specified LIN channel.
 * 
 * @param[in]   Channel     LIN channel to be addressed
 * 
 * @service_id 0x0a
 * @reentrancy Non Reentrant
 * @synchronization Synchronous
 * 
 * @return      Std_ReturnType
 * @retval      E_OK        API call has been accepted (wakeup detected)
 * @retval      E_NOT_OK    API call has not been accepted (no wakeup detected or error)
 * 
 * @implements  Lin_CheckWakeup_Activity
 * [SWS_Lin_00160]
 */
Std_ReturnType Lin_CheckWakeup(uint8 Channel);
#endif /* LIN_ENABLE_CHECK_WAKEUP_API */


#if (LIN_VERSION_INFO_API == STD_ON)
/**
 * @brief Returns the version information of the Lin module.
 *
 * This function fills the provided Std_VersionInfoType structure with
 * the version information of the Lin module.
 *
 * @param[out] versioninfo Pointer to a Std_VersionInfoType structure where
 *                         the version information will be stored.
 *
 * @service_id 0x01
 * @reentrancy Reentrant
 * @synchronization Synchronous
 *
 * @note This function does not return a value; the version information is
 *       provided via the output parameter.
 * [SWS_Lin_00161]
 */
void Lin_GetVersionInfo(Std_VersionInfoType* versioninfo);

#endif /* LIN_VERSION_INFO_API */


/**
 * @brief Sends a LIN header and, if necessary, a LIN response.
 *
 * This service is used to transmit a LIN frame on the specified channel. The direction
 * of the frame response (master response, slave response, slave-to-slave communication)
 * is provided by the PduInfoPtr parameter. Only LIN master nodes can use this service.
 *
 * @service_id 0x04
 * @reentrancy Non Reentrant
 * @sync_async Asynchronous
 *
 * @param[in] Channel LIN channel to be addressed.
 * @param[in] PduInfoPtr Pointer to PDU containing the PID, checksum model, response type,
 *                       Dl and SDU data pointer.
 *
 * @return Std_ReturnType
 *         - E_OK: Send command has been accepted.
 *         - E_NOT_OK: Send command has not been accepted, development or production error occurred.
 *
 * @note  This service is only applicable for LIN master node (available only if the ECU
 *         has any LIN master channel).
 */
Std_ReturnType Lin_SendFrame ( uint8 Channel, const Lin_PduType* PduInfoPtr ); 

/**
 * @brief   Sends a go-to-sleep command on the specified LIN channel.
 * @details This service instructs the driver to transmit a go-to-sleep-command
 *          on the addressed LIN channel. Only used for LIN master nodes.
 * 
 * @param[in]   Channel     LIN channel to be addressed
 * 
 * @return      Std_ReturnType
 * @retval      E_OK        Sleep command has been accepted
 * @retval      E_NOT_OK    Sleep command has not been accepted, development or production error occurred
 * 
 * @service_id  0x06
 * @reentrancy  Non Reentrant
 * @sync_async  Asynchronous
 * 
 * @implements  [SWS_Lin_00166]
 */
Std_ReturnType Lin_GoToSleep(uint8 Channel);

/**
 * @brief   Sets the specified LIN channel to sleep mode (internal operation).
 * @details This function sets the channel state to LIN_CH_SLEEP, enables wake-up detection,
 *          and optionally sets the LIN hardware unit to reduced power operation mode (if supported by hardware).
 *          This service is synchronous and non-reentrant.
 *
 * @param[in]   Channel     LIN channel to be addressed
 *
 * @return      Std_ReturnType
 * @retval      E_OK        Command has been accepted
 * @retval      E_NOT_OK    Command has not been accepted, development or production error occurred
 *
 * @service_id  0x09
 * @reentrancy  Non Reentrant
 * @synchronization Synchronous
 *
 * @implements  [SWS_Lin_00167]
 */
Std_ReturnType Lin_GoToSleepInternal(uint8 Channel);


/**
 * @brief Generates a wake-up pulse on the specified LIN channel and sets its state to LIN_CH_OPERATIONAL.
 *
 * @service_id 0x07
 * @reentrancy Non Reentrant
 * @sync_async Asynchronous
 *
 * @param[in] Channel LIN channel to be addressed.
 *
 * @return Std_ReturnType
 *         - E_OK: Wake-up request has been accepted.
 *         - E_NOT_OK: Wake-up request has not been accepted, development or production error occurred.
 *
 * [SWS_Lin_00169]
 */
Std_ReturnType Lin_Wakeup(uint8 Channel);
 
/**
 * @brief   Sets the specified LIN channel to operational state without generating a wake-up pulse.
 * @details This function sets the channel state to LIN_CH_OPERATIONAL for the addressed LIN channel.
 *          No wake-up pulse is generated. This service is asynchronous and non-reentrant.
 *
 * @param[in]   Channel     LIN channel to be addressed
 *
 * @return      Std_ReturnType
 * @retval      E_OK        Wake-up request has been accepted
 * @retval      E_NOT_OK    Wake-up request has not been accepted, development or production error occurred
 *
 * @service_id  0x0B
 * @reentrancy  Non Reentrant
 * @sync_async  Asynchronous
 *
 * [SWS_Lin_00256]
 */
Std_ReturnType Lin_WakeupInternal(uint8 Channel);


/**
 * @brief   Gets the status of the LIN driver for the specified channel.
 * @details This function returns the current status of the LIN channel and provides a pointer
 *          to the buffer where the current SDU (Service Data Unit) is stored.
 *          Only used for LIN master nodes.
 *
 * @param[in]   Channel     LIN channel to be checked.
 * @param[out]  Lin_SduPtr  Pointer to pointer to a shadow buffer or memory mapped LIN hardware
 *                          receive buffer where the current SDU is stored.
 *
 * @return      Lin_StatusType
 *              - LIN_NOT_OK: Development or production error occurred
 *              - LIN_TX_OK: Successful transmission
 *              - LIN_TX_BUSY: Ongoing transmission (Header or Response)
 *              - LIN_TX_HEADER_ERROR: Erroneous header transmission
 *              - LIN_TX_ERROR: Erroneous response transmission
 *              - LIN_RX_OK: Reception of correct response
 *              - LIN_RX_BUSY: Ongoing reception, but checksum byte not received
 *              - LIN_RX_ERROR: Erroneous response reception
 *              - LIN_RX_NO_RESPONSE: No response byte has been received so far
 *              - LIN_OPERATIONAL: Channel is woken up from sleep, no data sent
 *              - LIN_CH_SLEEP: Channel is in sleep state, wake-up detection enabled
 *
 * @service_id  0x08
 * @reentrancy  Non Reentrant
 * @synchronization Synchronous
 *
 * [SWS_Lin_00168]
 */
Lin_StatusType Lin_GetStatus(uint8 Channel, const uint8** Lin_SduPtr);


#ifdef __cplusplus
}
#endif

#endif /* ADC_H */

/**
 * @}
 */
