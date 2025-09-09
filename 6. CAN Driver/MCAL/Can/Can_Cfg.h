/****************************************************************************************
*                               CAN_CFG.H                                              *
****************************************************************************************
* File Name   : Can_Cfg.h
* Module      : Controller Area Network (CAN)
* Description : AUTOSAR CAN driver configuration header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 24/08/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef CAN_CFG_H
#define CAN_CFG_H

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "Std_Types.h"
#include "Can_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CAN_CFG_VENDOR_ID                     (0x0001U)
#define CAN_CFG_MODULE_ID                     (0x007BU)
#define CAN_CFG_AR_RELEASE_MAJOR_VERSION      (4U)
#define CAN_CFG_AR_RELEASE_MINOR_VERSION      (4U)
#define CAN_CFG_AR_RELEASE_REVISION_VERSION   (0U)
#define CAN_CFG_SW_MAJOR_VERSION              (1U)
#define CAN_CFG_SW_MINOR_VERSION              (0U)
#define CAN_CFG_SW_PATCH_VERSION              (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if Can_Cfg.h file and Std_Types.h file are of the same vendor */
#if (CAN_CFG_VENDOR_ID != STD_TYPES_VENDOR_ID)
    #error "Can_Cfg.h and Std_Types.h have different vendor ids"
#endif

/* Check if Can_Cfg.h file and Can_Types.h file are of the same vendor */
#if (CAN_CFG_VENDOR_ID != CAN_TYPES_VENDOR_ID)
    #error "Can_Cfg.h and Can_Types.h have different vendor ids"
#endif

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/*================================ General Configuration Parameters ===============================*/

/**
 * @brief   Development Error Detection
 * @details Switches the Development Error Detection and Notification on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00036
 */
#define CAN_DEV_ERROR_DETECT                STD_ON

/**
 * @brief   Version Info API
 * @details Switches the Can_GetVersionInfo function on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00037
 */
#define CAN_VERSION_INFO_API                STD_ON

/**
 * @brief   De-init API
 * @details Switches the Can_DeInit function on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00038
 */
#define CAN_DEINIT_API                      STD_ON

/**
 * @brief   Set Baudrate API
 * @details Switches the Can_SetBaudrate function on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00484
 */
#define CAN_SET_BAUDRATE_API                STD_ON

/**
 * @brief   Wakeup Support
 * @details Enables/disables the wakeup functionality of the CAN driver.
 *          true: Enabled, false: Disabled
 * SWS_Can_00358
 */
#define CAN_WAKEUP_SUPPORT                  STD_ON

/**
 * @brief   Get Controller Error State API
 * @details Switches the Can_GetControllerErrorState function on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00488
 */
#define CAN_GET_CONTROLLER_ERROR_STATE_API  STD_ON

/**
 * @brief   Get Controller Mode API
 * @details Switches the Can_GetControllerMode function on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00491
 */
#define CAN_GET_CONTROLLER_MODE_API         STD_ON

/**
 * @brief   Get Controller RX Error Counter API
 * @details Switches the Can_GetControllerRxErrorCounter function on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00492
 */
#define CAN_GET_CONTROLLER_RX_ERROR_COUNTER_API  STD_ON

/**
 * @brief   Get Controller TX Error Counter API
 * @details Switches the Can_GetControllerTxErrorCounter function on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00493
 */
#define CAN_GET_CONTROLLER_TX_ERROR_COUNTER_API  STD_ON

/**
 * @brief   Get Current Time API
 * @details Switches the Can_GetCurrentTime function on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00494
 */
#define CAN_GET_CURRENT_TIME_API            STD_OFF

/**
 * @brief   Enable Egress Time Stamp API
 * @details Switches the Can_EnableEgressTimeStamp function on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00496
 */
#define CAN_ENABLE_EGRESS_TIME_STAMP_API    STD_OFF

/**
 * @brief   Get Egress Time Stamp API
 * @details Switches the Can_GetEgressTimeStamp function on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00497
 */
#define CAN_GET_EGRESS_TIME_STAMP_API       STD_OFF

/**
 * @brief   Get Ingress Time Stamp API
 * @details Switches the Can_GetIngressTimeStamp function on or off.
 *          true: Enabled, false: Disabled
 * SWS_Can_00498
 */
#define CAN_GET_INGRESS_TIME_STAMP_API      STD_OFF

/**
 * @brief   L-PDU Callout Support
 * @details Enables/disables the L-PDU callout functionality.
 *          true: Enabled, false: Disabled
 * SWS_Can_00500
 */
#define CAN_LPDU_CALLOUT_SUPPORT            STD_OFF

/*============================ Processing Configuration Parameters =============================*/

/**
 * @brief   TX Processing Configuration
 * @details Defines whether TX confirmation is handled via interrupt or polling.
 *          INTERRUPT: Interrupt-driven processing
 *          POLLING: Polling-based processing
 * SWS_Can_00103
 */
#define INTERRUPT                           (0U)
#define POLLING                             (1U)
#define CAN_TX_PROCESSING                   INTERRUPT

/**
 * @brief   RX Processing Configuration  
 * @details Defines whether RX indication is handled via interrupt or polling.
 *          INTERRUPT: Interrupt-driven processing
 *          POLLING: Polling-based processing
 * SWS_Can_00104
 */
#define CAN_RX_PROCESSING                   INTERRUPT

/**
 * @brief   Bus-off Processing Configuration
 * @details Defines whether bus-off recovery is handled via interrupt or polling.
 *          INTERRUPT: Interrupt-driven processing
 *          POLLING: Polling-based processing
 * SWS_Can_00105
 */
#define CAN_BUSOFF_PROCESSING               INTERRUPT

/**
 * @brief   Wakeup Processing Configuration
 * @details Defines whether wakeup events are handled via interrupt or polling.
 *          INTERRUPT: Interrupt-driven processing
 *          POLLING: Polling-based processing
 * SWS_Can_00106
 */
#define CAN_WAKEUP_PROCESSING               INTERRUPT

/**
 * @brief   Mode Processing Configuration
 * @details Defines whether mode changes are handled via interrupt or polling.
 *          INTERRUPT: Interrupt-driven processing
 *          POLLING: Polling-based processing
 * SWS_Can_00372
 */
#define CAN_MODE_PROCESSING                 POLLING

/*============================== Hardware Configuration Parameters =============================*/

/**
 * @brief   Maximum Number of CAN Controllers supported by driver
 * @details Specifies the maximum number of CAN controllers supported by driver.
 *          This defines the array sizes and should be >= CAN_CONTROLLERS_AVAILABLE
 */
#define CAN_MAX_CONTROLLERS                 (2U)

/**
 * @brief   Number of CAN Controllers Available on Current Platform
 * @details Specifies the actual number of CAN controllers available on current board.
 *          This should be configured based on the target hardware:
 *          - STM32F103C8T6: 1 (only CAN1)
 *          - STM32F103VET6: 2 (CAN1 + CAN2)
 *          - Other MCUs: Configure according to datasheet
 */
#define CAN_CONTROLLERS_AVAILABLE           (1U)

/**
 * @brief   Controller Enable Status Array
 * @details Defines which controllers are enabled in the current configuration
 */
#define CAN_CONTROLLER_ENABLE_MASK          (0x01U)  /* Only CAN1 enabled for STM32F103C8 */

/**
 * @brief   Maximum Number of Hardware Objects
 * @details Total number of hardware objects (HTH + HRH) supported per controller.
 *          STM32F103 CAN: 3 TX mailboxes + 2 RX FIFOs = 5 per controller
 */
#define CAN_MAX_HW_OBJECTS_PER_CONTROLLER   (5U)
#define CAN_MAX_HW_OBJECTS                  (CAN_MAX_HW_OBJECTS_PER_CONTROLLER * CAN_CONTROLLERS_AVAILABLE)

/**
 * @brief   Maximum Number of Hardware Transmit Objects (HTH)
 * @details Number of transmit hardware objects per controller.
 *          STM32F103 CAN: 3 TX mailboxes per controller
 */
#define CAN_MAX_HTH_OBJECTS_PER_CONTROLLER  (3U)
#define CAN_MAX_HTH_OBJECTS                 (CAN_MAX_HTH_OBJECTS_PER_CONTROLLER * CAN_CONTROLLERS_AVAILABLE)

/**
 * @brief   Maximum Number of Hardware Receive Objects (HRH)
 * @details Number of receive hardware objects per controller.
 *          STM32F103 CAN: 2 RX FIFOs per controller
 */
#define CAN_MAX_HRH_OBJECTS_PER_CONTROLLER  (2U)
#define CAN_MAX_HRH_OBJECTS                 (CAN_MAX_HRH_OBJECTS_PER_CONTROLLER * CAN_CONTROLLERS_AVAILABLE)

/**
 * @brief   Maximum TX Message Buffer Size
 * @details Maximum number of messages that can be buffered for transmission per controller
 */
#define CAN_MAX_TX_BUFFER_SIZE              (10U)

/**
 * @brief   Maximum RX Message Buffer Size
 * @details Maximum number of messages that can be buffered for reception per controller
 */
#define CAN_MAX_RX_BUFFER_SIZE              (20U)

/**
 * @brief   CAN Clock Source
 * @details Clock source for CAN peripheral (APB1 clock for STM32F103)
 *          Value in Hz - 36MHz for STM32F103 at 72MHz system clock
 */
#define CAN_CLOCK_FREQUENCY                 (36000000UL)

/*================================ Controller Specific Configuration ===========================*/

/**
 * @brief   CAN Controller Hardware Mapping Structure
 * @details Maps logical controller ID to hardware resources
 */
typedef struct {
    boolean         Enabled;            /**< Controller enable status */
    uint8           ControllerId;       /**< Logical controller ID */
    uint32          BaseAddress;        /**< Hardware base address */
    uint8           TxIrqNumber;        /**< TX interrupt number */
    uint8           RxIrqNumber;        /**< RX interrupt number */
    uint8           ErrorIrqNumber;     /**< Error interrupt number */
    uint8           IrqPriority;        /**< Interrupt priority */
    GPIO_TypeDef*   GpioPort;          /**< GPIO port for CAN pins */
    uint16          TxPin;              /**< TX pin number */
    uint16          RxPin;              /**< RX pin number */
    uint32          GpioClockCmd;       /**< GPIO clock enable command */
    uint32          CanClockCmd;        /**< CAN peripheral clock enable command */
} Can_ControllerHwConfigType;

/**
 * @brief   CAN Controller Configuration Array
 * @details Configuration for all available CAN controllers
 */
extern const Can_ControllerHwConfigType Can_ControllerHwConfig[CAN_MAX_CONTROLLERS];

/**
 * @brief   CAN Controller 0 Configuration (CAN1)
 */
#define CAN_CONTROLLER_0_ENABLED            STD_ON
#define CAN_CONTROLLER_0_ID                 (0U)
#define CAN_CONTROLLER_0_BASE_ADDRESS       (0x40006400UL)  /* CAN1 base address */
#define CAN_CONTROLLER_0_IRQ_NUMBER         (19U)           /* CAN1_TX IRQ */
#define CAN_CONTROLLER_0_IRQ_PRIORITY       (5U)

/**
 * @brief   CAN Controller 1 Configuration (if available)
 */
#define CAN_CONTROLLER_1_ENABLED            STD_OFF
#define CAN_CONTROLLER_1_ID                 (1U)
#define CAN_CONTROLLER_1_BASE_ADDRESS       (0x40006800UL)  /* CAN2 base address */
#define CAN_CONTROLLER_1_IRQ_NUMBER         (63U)           /* CAN2_TX IRQ */
#define CAN_CONTROLLER_1_IRQ_PRIORITY       (5U)

/*=============================== Baudrate Configuration IDs ===============================*/

/**
 * @brief   Baudrate Configuration Identifiers
 * @details Pre-defined baudrate configurations for the CAN controller
 */
#define CAN_BAUDRATE_125KBPS_ID             (0U)
#define CAN_BAUDRATE_250KBPS_ID             (1U)
#define CAN_BAUDRATE_500KBPS_ID             (2U)
#define CAN_BAUDRATE_1MBPS_ID               (3U)

/**
 * @brief   Default Baudrate Configuration
 * @details Default baudrate configuration used during initialization
 */
#define CAN_DEFAULT_BAUDRATE_ID             CAN_BAUDRATE_250KBPS_ID

/*=============================== Hardware Object Configuration =============================*/

/**
 * @brief   Hardware Transmit Handle (HTH) IDs
 * @details Identifiers for hardware transmit objects
 */
#define CAN_HTH_0                           (0U)
#define CAN_HTH_1                           (1U)
#define CAN_HTH_2                           (2U)

/**
 * @brief   Hardware Receive Handle (HRH) IDs
 * @details Identifiers for hardware receive objects
 */
#define CAN_HRH_0                           (3U)  /* FIFO 0 */
#define CAN_HRH_1                           (4U)  /* FIFO 1 */

/*============================ Filter Configuration Parameters ==============================*/

/**
 * @brief   Maximum Number of Message Filters
 * @details Maximum number of acceptance filters that can be configured
 *          STM32F103 CAN1: 14 filters (0-13)
 */
#define CAN_MAX_FILTER_COUNT                (14U)

/**
 * @brief   Default Filter Configuration
 * @details Accept all messages by default (for development/testing)
 */
#define CAN_DEFAULT_FILTER_MODE             STD_ON  /* Accept all messages */

/*============================ Timeout Configuration Parameters =============================*/

/**
 * @brief   Initialization Timeout
 * @details Maximum time to wait for CAN controller initialization (in ms)
 */
#define CAN_INIT_TIMEOUT                    (100U)

/**
 * @brief   Mode Change Timeout
 * @details Maximum time to wait for CAN controller mode change (in ms)
 */
#define CAN_MODE_CHANGE_TIMEOUT             (10U)

/**
 * @brief   TX Confirmation Timeout
 * @details Maximum time to wait for TX confirmation (in ms)
 */
#define CAN_TX_TIMEOUT                      (100U)

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
 * @brief   CAN Configuration Structure Declaration
 * @details External declaration of the CAN configuration structure
 */
extern const Can_ConfigType Can_Config;

/**
 * @brief   CAN Controller Configuration Array Declaration
 * @details External declaration of CAN controller configuration array
 */
extern const Can_ConfigType Can_ControllerConfigs[CAN_MAX_CONTROLLERS];

/*==================================================================================================
*                                     FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* CAN_CFG_H */