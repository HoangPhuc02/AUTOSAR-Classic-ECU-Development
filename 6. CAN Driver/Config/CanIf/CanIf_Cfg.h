/****************************************************************************************
*                               CANIF_CFG.H                                            *
****************************************************************************************
* File Name   : CanIf_Cfg.h
* Module      : CAN Interface (CanIf)
* Description : AUTOSAR CanIf driver configuration header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 24/08/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef CANIF_CFG_H
#define CANIF_CFG_H

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "Std_Types.h"
#include "CanIf_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CANIF_CFG_VENDOR_ID                     (0x0001U)
#define CANIF_CFG_MODULE_ID                     (0x007CU)
#define CANIF_CFG_AR_RELEASE_MAJOR_VERSION      (4U)
#define CANIF_CFG_AR_RELEASE_MINOR_VERSION      (4U)
#define CANIF_CFG_AR_RELEASE_REVISION_VERSION   (0U)
#define CANIF_CFG_SW_MAJOR_VERSION              (1U)
#define CANIF_CFG_SW_MINOR_VERSION              (0U)
#define CANIF_CFG_SW_PATCH_VERSION              (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if CanIf_Cfg.h file and Std_Types.h file are of the same vendor */
#if (CANIF_CFG_VENDOR_ID != STD_TYPES_VENDOR_ID)
    #error "CanIf_Cfg.h and Std_Types.h have different vendor ids"
#endif

/* Check if CanIf_Cfg.h file and CanIf_Types.h file are of the same vendor */
#if (CANIF_CFG_VENDOR_ID != CANIF_TYPES_VENDOR_ID)
    #error "CanIf_Cfg.h and CanIf_Types.h have different vendor ids"
#endif

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/*================================ General Configuration Parameters ===============================*/

/**
 * @brief   Development Error Detection
 * @details Switches the Development Error Detection and Notification on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00001
 */
#define CANIF_DEV_ERROR_DETECT                  STD_ON

/**
 * @brief   Version Info API
 * @details Switches the CanIf_GetVersionInfo function on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00002
 */
#define CANIF_VERSION_INFO_API                  STD_ON

/**
 * @brief   Set Dynamic TX ID API
 * @details Switches the CanIf_SetDynamicTxId API on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00003
 */
#define CANIF_SET_DYNAMIC_TX_ID_API             STD_OFF

/**
 * @brief   Wake Up Support API
 * @details Switches the wake up support on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00004
 */
#define CANIF_WAKEUP_SUPPORT                    STD_ON

/**
 * @brief   Wake Up Validation API
 * @details Switches the wake up validation API on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00005
 */
#define CANIF_WAKEUP_VALIDATION_API             STD_ON

/**
 * @brief   Read RX PDU Data API
 * @details Switches the CanIf_ReadRxPduData API on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00006
 */
#define CANIF_READ_RX_PDU_DATA_API              STD_ON

/**
 * @brief   Read RX PDU Notify Status API
 * @details Switches the CanIf_ReadRxNotifStatus API on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00007
 */
#define CANIF_READ_RX_PDU_NOTIFY_STATUS_API     STD_ON

/**
 * @brief   Read TX PDU Notify Status API
 * @details Switches the CanIf_ReadTxNotifStatus API on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00008
 */
#define CANIF_READ_TX_PDU_NOTIFY_STATUS_API     STD_ON

/**
 * @brief   TX Confirmation Polling Support
 * @details Switches the TX confirmation polling support on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00009
 */
#define CANIF_TX_CONFIRM_POLLING_SUPPORT        STD_OFF

/**
 * @brief   Cancel TX Support API
 * @details Switches the CanIf_CancelTxRequest API on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00010
 */
#define CANIF_CANCEL_TX_SUPPORT_API             STD_OFF

/**
 * @brief   Set Baudrate API
 * @details Switches the CanIf_SetBaudrate API on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00011
 */
#define CANIF_SET_BAUDRATE_API                  STD_ON

/**
 * @brief   Trigger Transmit API
 * @details Switches the trigger transmit API on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00012
 */
#define CANIF_TRIGGER_TRANSMIT_SUPPORT          STD_OFF

/**
 * @brief   TX Buffering Support
 * @details Switches the TX buffering support on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00013
 */
#define CANIF_TX_BUFFERING                      STD_ON

/**
 * @brief   RX Buffer Size
 * @details Number of bytes reserved for RX buffering.
 *          0 = RX buffering disabled
 * SWS_CANIF_00014
 */
#define CANIF_RX_BUFFER_SIZE                    (64U)

/**
 * @brief   TX Buffer Size
 * @details Number of bytes reserved for TX buffering.
 *          0 = TX buffering disabled
 * SWS_CANIF_00015
 */
#define CANIF_TX_BUFFER_SIZE                    (128U)

/**
 * @brief   Data Length Check
 * @details Switches the data length check on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00016
 */
#define CANIF_DATA_LENGTH_CHECK                 STD_ON

/**
 * @brief   Multiple Driver Support
 * @details Switches support for multiple CAN drivers on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00017
 */
#define CANIF_MULTIPLE_DRIVER_SUPPORT           STD_OFF

/**
 * @brief   Software Filter Support
 * @details Switches the software filtering support on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00018
 */
#define CANIF_PRIVATE_SOFTWARE_FILTER           STD_ON

/**
 * @brief   Range Reception Support
 * @details Switches the range reception support on or off.
 *          true: Enabled, false: Disabled
 * SWS_CANIF_00019
 */
#define CANIF_RANGE_RECEPTION_SUPPORT           STD_ON

/*=============================== Hardware Configuration Parameters =============================*/

/**
 * @brief   Maximum Number of CAN Controllers
 * @details Specifies the maximum number of CAN controllers supported.
 *          For STM32F103C8T6: CAN1 is available
 */
#define CANIF_MAX_CAN_CONTROLLERS              (1U)

/**
 * @brief   Maximum Number of Hardware Objects
 * @details Total number of hardware objects (HTH + HRH) supported.
 */
#define CANIF_MAX_HW_OBJECTS                   (5U)

/**
 * @brief   Maximum Number of Hardware Transmit Objects (HTH)
 * @details Number of transmit hardware objects.
 */
#define CANIF_MAX_HTH_OBJECTS                  (3U)

/**
 * @brief   Maximum Number of Hardware Receive Objects (HRH)
 * @details Number of receive hardware objects.
 */
#define CANIF_MAX_HRH_OBJECTS                  (2U)

/**
 * @brief   Maximum Number of TX PDUs
 * @details Maximum number of TX PDUs that can be configured.
 */
#define CANIF_MAX_TX_PDUS                      (20U)

/**
 * @brief   Maximum Number of RX PDUs
 * @details Maximum number of RX PDUs that can be configured.
 */
#define CANIF_MAX_RX_PDUS                      (30U)

/**
 * @brief   Maximum PDU Length
 * @details Maximum length of a PDU in bytes.
 */
#define CANIF_MAX_PDU_LENGTH                   (8U)

/*============================ Controller Specific Configuration =============================*/

/**
 * @brief   CanIf Controller 0 Configuration
 */
#define CANIF_CONTROLLER_0_ENABLED             STD_ON
#define CANIF_CONTROLLER_0_ID                  (0U)
#define CANIF_CONTROLLER_0_CAN_CTRL_REF        (0U)    /* Reference to CAN Controller 0 */
#define CANIF_CONTROLLER_0_WAKEUP_SUPPORT      STD_ON
#define CANIF_CONTROLLER_0_WAKEUP_SOURCE       (0x01U)

/**
 * @brief   CanIf Controller 1 Configuration (if available)
 */
#define CANIF_CONTROLLER_1_ENABLED             STD_OFF
#define CANIF_CONTROLLER_1_ID                  (1U)
#define CANIF_CONTROLLER_1_CAN_CTRL_REF        (1U)    /* Reference to CAN Controller 1 */
#define CANIF_CONTROLLER_1_WAKEUP_SUPPORT      STD_OFF
#define CANIF_CONTROLLER_1_WAKEUP_SOURCE       (0x02U)

/*=============================== PDU Configuration IDs ===============================*/

/**
 * @brief   TX PDU Configuration Identifiers
 * @details Pre-defined TX PDU identifiers
 */
#define CANIF_TX_PDU_ENGINE_STATUS             (0U)
#define CANIF_TX_PDU_VEHICLE_SPEED             (1U)
#define CANIF_TX_PDU_BRAKE_STATUS              (2U)
#define CANIF_TX_PDU_DOOR_STATUS               (3U)
#define CANIF_TX_PDU_LIGHT_STATUS              (4U)

/**
 * @brief   RX PDU Configuration Identifiers
 * @details Pre-defined RX PDU identifiers
 */
#define CANIF_RX_PDU_ENGINE_COMMAND            (0U)
#define CANIF_RX_PDU_BRAKE_COMMAND             (1U)
#define CANIF_RX_PDU_DOOR_COMMAND              (2U)
#define CANIF_RX_PDU_LIGHT_COMMAND             (3U)
#define CANIF_RX_PDU_DIAGNOSTIC_REQUEST        (4U)

/*=============================== CAN ID Configuration =============================*/

/**
 * @brief   TX CAN IDs
 * @details Pre-defined CAN IDs for transmission
 */
#define CANIF_CAN_ID_ENGINE_STATUS             (0x100U)
#define CANIF_CAN_ID_VEHICLE_SPEED             (0x101U)
#define CANIF_CAN_ID_BRAKE_STATUS              (0x102U)
#define CANIF_CAN_ID_DOOR_STATUS               (0x103U)
#define CANIF_CAN_ID_LIGHT_STATUS              (0x104U)

/**
 * @brief   RX CAN IDs
 * @details Pre-defined CAN IDs for reception
 */
#define CANIF_CAN_ID_ENGINE_COMMAND            (0x200U)
#define CANIF_CAN_ID_BRAKE_COMMAND             (0x201U)
#define CANIF_CAN_ID_DOOR_COMMAND              (0x202U)
#define CANIF_CAN_ID_LIGHT_COMMAND             (0x203U)
#define CANIF_CAN_ID_DIAGNOSTIC_REQUEST        (0x7DFU)

/*============================ Buffer Configuration Parameters =============================*/

/**
 * @brief   TX Buffer Configuration
 * @details Configuration for TX buffering per HTH
 */
#define CANIF_TX_BUFFER_HTH_0_SIZE             (5U)
#define CANIF_TX_BUFFER_HTH_1_SIZE             (5U)
#define CANIF_TX_BUFFER_HTH_2_SIZE             (3U)

/*============================ Timeout Configuration Parameters =============================*/

/**
 * @brief   Main Function Period
 * @details Period of CanIf_MainFunction in milliseconds
 */
#define CANIF_MAIN_FUNCTION_PERIOD             (10U)

/**
 * @brief   TX Confirmation Timeout
 * @details Maximum time to wait for TX confirmation (in main function cycles)
 */
#define CANIF_TX_CONFIRMATION_TIMEOUT          (100U)

/**
 * @brief   Controller Mode Change Timeout
 * @details Maximum time to wait for controller mode change (in main function cycles)
 */
#define CANIF_CONTROLLER_MODE_TIMEOUT          (50U)

/*============================ Notification Function Names =============================*/

/**
 * @brief   Upper Layer Notification Function Names
 * @details Define the names of upper layer callback functions
 */
#define CANIF_UL_RX_INDICATION_NAME            PduR_CanIfRxIndication
#define CANIF_UL_TX_CONFIRMATION_NAME          PduR_CanIfTxConfirmation
#define CANIF_UL_CONTROLLER_BUSOFF_NAME        CanSM_ControllerBusOff
#define CANIF_UL_CONTROLLER_MODE_IND_NAME      CanSM_ControllerModeIndication

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
*                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/**
 * @brief   CanIf Configuration Structure Declaration
 * @details External declaration of the CanIf configuration structure
 */
extern const CanIf_ConfigType CanIf_Config;

/**
 * @brief   CanIf Controller Configuration Array Declaration
 * @details External declaration of CanIf controller configuration array
 */
extern const CanIf_ControllerConfigType CanIf_ControllerConfigs[CANIF_MAX_CAN_CONTROLLERS];

/**
 * @brief   CanIf HTH Configuration Array Declaration
 * @details External declaration of CanIf HTH configuration array
 */
extern const CanIf_HthConfigType CanIf_HthConfigs[CANIF_MAX_HTH_OBJECTS];

/**
 * @brief   CanIf HRH Configuration Array Declaration
 * @details External declaration of CanIf HRH configuration array
 */
extern const CanIf_HrhConfigType CanIf_HrhConfigs[CANIF_MAX_HRH_OBJECTS];

/**
 * @brief   CanIf TX PDU Configuration Array Declaration
 * @details External declaration of CanIf TX PDU configuration array
 */
extern const CanIf_TxPduConfigType CanIf_TxPduConfigs[CANIF_MAX_TX_PDUS];

/**
 * @brief   CanIf RX PDU Configuration Array Declaration
 * @details External declaration of CanIf RX PDU configuration array
 */
extern const CanIf_RxPduConfigType CanIf_RxPduConfigs[CANIF_MAX_RX_PDUS];

/*==================================================================================================
*                                     FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* CANIF_CFG_H */
