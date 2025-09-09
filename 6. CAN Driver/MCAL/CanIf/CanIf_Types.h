/****************************************************************************************
*                               CANIF_TYPES.H                                          *
****************************************************************************************
* File Name   : CanIf_Types.h
* Module      : CAN Interface (CanIf)
* Description : AUTOSAR CanIf driver type definitions header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 24/08/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef CANIF_TYPES_H
#define CANIF_TYPES_H

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "Std_Types.h"
#include "Can_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   File Overview
 * @details This header file contains AUTOSAR-compliant type definitions for CanIf driver:
 *
 * CanIf Driver Types:
 *   - CanIf_ControllerModeType: Controller operating modes (UNINIT, STOPPED, STARTED, SLEEP)
 *   - CanIf_PduModeType: PDU channel modes (OFFLINE, TX_OFFLINE, ONLINE)
 *   - CanIf_NotifStatusType: Notification status types (NO_NOTIFICATION, TX_RX_NOTIFICATION)
 *   - CanIf_TxBufferModeType: TX buffer handling modes (SINGLE, FIFO)
 *   - CanIf_ConfigType: Configuration structure for CanIf initialization
 *   - CanIf_HrhConfigType: Hardware Receive Handle configuration
 *   - CanIf_HthConfigType: Hardware Transmit Handle configuration
 *   - CanIf_TxPduConfigType: TX PDU configuration structure
 *   - CanIf_RxPduConfigType: RX PDU configuration structure
 *
 * Constants and Macros:
 *   - CanIf API return values and error codes
 *   - Configuration limits and validation macros
 */

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CANIF_TYPES_VENDOR_ID                     (0x0001U)
#define CANIF_TYPES_MODULE_ID                     (0x007CU)
#define CANIF_TYPES_AR_RELEASE_MAJOR_VERSION      (4U)
#define CANIF_TYPES_AR_RELEASE_MINOR_VERSION      (4U)
#define CANIF_TYPES_AR_RELEASE_REVISION_VERSION   (0U)
#define CANIF_TYPES_SW_MAJOR_VERSION              (1U)
#define CANIF_TYPES_SW_MINOR_VERSION              (0U)
#define CANIF_TYPES_SW_PATCH_VERSION              (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if CanIf_Types.h file and Std_Types.h file are of the same vendor */
#if (CANIF_TYPES_VENDOR_ID != STD_TYPES_VENDOR_ID)
    #error "CanIf_Types.h and Std_Types.h have different vendor ids"
#endif

/*==================================================================================================
*                                            CONSTANTS
==================================================================================================*/

/**
 * @brief   CanIf Invalid PDU ID
 * @details Used to indicate an invalid PDU ID
 */
#define CANIF_INVALID_PDU_ID                    ((PduIdType)0xFFFFU)

/**
 * @brief   CanIf Invalid CAN ID
 * @details Used to indicate an invalid CAN ID
 */
#define CANIF_INVALID_CAN_ID                    ((Can_IdType)0xFFFFFFFFUL)

/**
 * @brief   CanIf Invalid Controller ID
 * @details Used to indicate an invalid controller ID
 */
#define CANIF_INVALID_CONTROLLER_ID             ((uint8)0xFFU)

/**
 * @brief   CanIf Invalid Hardware Handle
 * @details Used to indicate an invalid hardware handle
 */
#define CANIF_INVALID_HW_HANDLE                 ((Can_HwHandleType)0xFFFFU)

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/**
 * @brief   CanIf Controller Mode Type
 * @details Operating modes of the CAN controller from CanIf perspective.
 * @note    Available via CanIf_Types.h
 * SWS_CANIF_00745
 */
typedef enum _CanIf_ControllerModeType
{
    CANIF_CS_UNINIT    = 0x00U,    /**< Controller is not initialized */
    CANIF_CS_SLEEP     = 0x01U,    /**< Controller is in sleep mode */  
    CANIF_CS_STARTED   = 0x02U,    /**< Controller is active/started */
    CANIF_CS_STOPPED   = 0x03U     /**< Controller is stopped */
} CanIf_ControllerModeType;

/**
 * @brief   CanIf PDU Mode Type
 * @details PDU channel modes for transmission and reception control.
 * @note    Available via CanIf_Types.h
 * SWS_CANIF_00746
 */
typedef enum _CanIf_PduModeType
{
    CANIF_OFFLINE      = 0x00U,    /**< Transmit and receive path is offline */
    CANIF_TX_OFFLINE   = 0x01U,    /**< Transmit path is offline, receive path is online */
    CANIF_TX_ONLINE    = 0x02U,    /**< Transmit and receive path is online */
    CANIF_ONLINE       = 0x02U     /**< Alias for CANIF_TX_ONLINE (backward compatibility) */
} CanIf_PduModeType;

/**
 * @brief   CanIf Notification Status Type
 * @details Return type of CanIf_ReadTxNotifStatus and CanIf_ReadRxNotifStatus.
 * @note    Available via CanIf_Types.h
 * SWS_CANIF_00747
 */
typedef enum _CanIf_NotifStatusType
{
    CANIF_NO_NOTIFICATION      = 0x00U,    /**< No transmit or receive event occurred for the requested L-PDU */
    CANIF_TX_RX_NOTIFICATION   = 0x01U     /**< The requested Rx/Tx CAN L-PDU was successfully transmitted or received */
} CanIf_NotifStatusType;

/**
 * @brief   CanIf TX Buffer Mode Type
 * @details Defines the type of software filtering that is used for a hardware transmit object.
 * @note    Available via CanIf_Types.h
 * SWS_CANIF_00748
 */
typedef enum _CanIf_TxBufferModeType
{
    CANIF_TX_BUFFER_SINGLE     = 0x00U,    /**< Single buffer per HTH */
    CANIF_TX_BUFFER_FIFO       = 0x01U     /**< FIFO buffer per HTH */
} CanIf_TxBufferModeType;

/**
 * @brief   CanIf Private Software Filter Type
 * @details Defines the type of software filtering that is used for a hardware receive object.
 * @note    Available via CanIf_Types.h
 * SWS_CANIF_00749
 */
typedef enum _CanIf_PrivateSoftwareFilterType
{
    CANIF_PRIVATE_SW_FILTER_LINEAR = 0x00U,    /**< Linear software filtering */
    CANIF_PRIVATE_SW_FILTER_BINARY = 0x01U,    /**< Binary software filtering */
    CANIF_PRIVATE_SW_FILTER_INDEX  = 0x02U,    /**< Index software filtering */
    CANIF_PRIVATE_SW_FILTER_TABLE  = 0x03U     /**< Table software filtering */
} CanIf_PrivateSoftwareFilterType;

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/**
 * @brief   CanIf HRH Configuration Type
 * @details Configuration of a hardware receive object (HRH).
 * @note    Available via CanIf_Types.h
 * SWS_CANIF_00750
 */
typedef struct _CanIf_HrhConfigType
{
    Can_HwHandleType                CanIfHrhIdSymRef;           /**< Hardware object handle */
    uint8                          CanIfCanControllerIdRef;    /**< Reference to CAN controller */
    CanIf_PrivateSoftwareFilterType CanIfHrhSoftwareFilter;    /**< Software filter type */
    uint8                          CanIfHrhRangeRxPduLowerRef; /**< Lower bound of RX PDU range */
    uint8                          CanIfHrhRangeRxPduUpperRef; /**< Upper bound of RX PDU range */
} CanIf_HrhConfigType;

/**
 * @brief   CanIf HTH Configuration Type  
 * @details Configuration of a hardware transmit object (HTH).
 * @note    Available via CanIf_Types.h
 * SWS_CANIF_00751
 */
typedef struct _CanIf_HthConfigType
{
    Can_HwHandleType            CanIfHthIdSymRef;           /**< Hardware object handle */
    uint8                      CanIfCanControllerIdRef;    /**< Reference to CAN controller */
    CanIf_TxBufferModeType     CanIfTxBufferMode;          /**< TX buffer mode */
    uint8                      CanIfHthTxPduLowerRef;      /**< Lower bound of TX PDU range */
    uint8                      CanIfHthTxPduUpperRef;      /**< Upper bound of TX PDU range */
} CanIf_HthConfigType;

/**
 * @brief   CanIf TX PDU Configuration Type
 * @details Configuration of a transmit CAN L-PDU.
 * @note    Available via CanIf_Types.h
 * SWS_CANIF_00752
 */
typedef struct _CanIf_TxPduConfigType
{
    PduIdType                  CanIfTxPduId;              /**< PDU identifier */
    Can_IdType                 CanIfTxPduCanId;           /**< CAN identifier */
    uint8                      CanIfTxPduCanIdType;       /**< CAN ID type (STANDARD/EXTENDED) */
    uint8                      CanIfTxPduDlc;             /**< Data Length Code */
    Can_HwHandleType           CanIfTxPduHthRef;          /**< Reference to HTH */
    PduIdType                  CanIfTxPduUserTxConfirmationPduId;  /**< User TX confirmation PDU ID */
    boolean                    CanIfTxPduReadNotifyStatus; /**< Read notify status enabled */
    boolean                    CanIfTxPduTriggerTransmit;  /**< Trigger transmit enabled */
    boolean                    CanIfTxPduTxConfirmation;   /**< TX confirmation enabled */
} CanIf_TxPduConfigType;

/**
 * @brief   CanIf RX PDU Configuration Type
 * @details Configuration of a receive CAN L-PDU.
 * @note    Available via CanIf_Types.h
 * SWS_CANIF_00753
 */
typedef struct _CanIf_RxPduConfigType
{
    PduIdType                  CanIfRxPduId;              /**< PDU identifier */
    Can_IdType                 CanIfRxPduCanId;           /**< CAN identifier */
    uint8                      CanIfRxPduCanIdType;       /**< CAN ID type (STANDARD/EXTENDED) */
    uint8                      CanIfRxPduDlc;             /**< Data Length Code */
    Can_HwHandleType           CanIfRxPduHrhRef;          /**< Reference to HRH */
    PduIdType                  CanIfRxPduUserRxIndicationPduId;    /**< User RX indication PDU ID */
    boolean                    CanIfRxPduReadData;        /**< Read data enabled */
    boolean                    CanIfRxPduReadNotifyStatus; /**< Read notify status enabled */
} CanIf_RxPduConfigType;

/**
 * @brief   CanIf TX Buffer Configuration Type
 * @details Configuration structure for TX buffering.
 * @note    Available via CanIf_Types.h
 * SWS_CANIF_00754
 */
typedef struct _CanIf_TxBufferConfigType
{
    Can_HwHandleType           CanIfBufferHthRef;         /**< Reference to HTH */
    uint8                      CanIfBufferSize;           /**< Buffer size */
} CanIf_TxBufferConfigType;

/**
 * @brief   CanIf Controller Configuration Type
 * @details Configuration of CanIf controller.
 * @note    Available via CanIf_Types.h
 * SWS_CANIF_00755
 */
typedef struct _CanIf_ControllerConfigType
{
    uint8                      CanIfCtrlId;               /**< Controller identifier */
    uint8                      CanIfCtrlCanCtrlRef;       /**< Reference to CAN controller */
    boolean                    CanIfCtrlWakeupSupport;    /**< Wakeup support enabled */
    uint32                     CanIfCtrlWakeupSource;     /**< Wakeup source identifier */
} CanIf_ControllerConfigType;

/**
 * @brief   CanIf Configuration Type
 * @details This type defines a data structure for the post-build parameters of the CanIf.
 * @note    Available via CanIf_Types.h  
 * SWS_CANIF_00756
 */
typedef struct _CanIf_ConfigType
{
    uint8                            ConfigSet;                /**< Configuration set identifier */
    
    /* Controller configuration */
    const CanIf_ControllerConfigType* CanIfControllerConfig;   /**< Pointer to controller configuration array */
    uint8                            CanIfMaxControllers;     /**< Number of controllers */
    
    /* Hardware object configuration */
    const CanIf_HrhConfigType*       CanIfHrhConfig;          /**< Pointer to HRH configuration array */
    uint8                            CanIfMaxHrhs;             /**< Number of HRHs */
    
    const CanIf_HthConfigType*       CanIfHthConfig;          /**< Pointer to HTH configuration array */
    uint8                            CanIfMaxHths;             /**< Number of HTHs */
    
    /* PDU configuration */
    const CanIf_TxPduConfigType*     CanIfTxPduConfig;        /**< Pointer to TX PDU configuration array */
    uint8                            CanIfMaxTxPdus;          /**< Number of TX PDUs */
    
    const CanIf_RxPduConfigType*     CanIfRxPduConfig;        /**< Pointer to RX PDU configuration array */
    uint8                            CanIfMaxRxPdus;          /**< Number of RX PDUs */
    
    /* Buffer configuration */
    const CanIf_TxBufferConfigType*  CanIfTxBufferConfig;     /**< Pointer to TX buffer configuration array */
    uint8                            CanIfMaxTxBuffers;       /**< Number of TX buffers */
    
} CanIf_ConfigType;

/*==================================================================================================
*                                       CONSTANTS
==================================================================================================*/

/* CanIf Constants */
#define CANIF_MAX_CONTROLLERS                   (2U)       /**< Maximum number of CAN controllers */
#define CANIF_MAX_HRHS                          (4U)       /**< Maximum number of HRHs */
#define CANIF_MAX_HTHS                          (3U)       /**< Maximum number of HTHs */
#define CANIF_MAX_TX_PDUS                       (20U)      /**< Maximum number of TX PDUs */
#define CANIF_MAX_RX_PDUS                       (30U)      /**< Maximum number of RX PDUs */
#define CANIF_MAX_TX_BUFFERS                    (10U)      /**< Maximum number of TX buffers */

/*==================================================================================================
*                              VALIDATION MACROS                                                  *
==================================================================================================*/

/* Controller validation */
#define CANIF_IS_VALID_CONTROLLER(ctrl)        ((ctrl) < CANIF_MAX_CONTROLLERS)

/* PDU ID validation */
#define CANIF_IS_VALID_TX_PDU_ID(pdu)          ((pdu) < CANIF_MAX_TX_PDUS)
#define CANIF_IS_VALID_RX_PDU_ID(pdu)          ((pdu) < CANIF_MAX_RX_PDUS)

/* Hardware handle validation */
#define CANIF_IS_VALID_HTH(hth)                ((hth) != CANIF_INVALID_HW_HANDLE)
#define CANIF_IS_VALID_HRH(hrh)                ((hrh) != CANIF_INVALID_HW_HANDLE)

/*==================================================================================================
*                                   EXTERNAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   EXTERNAL FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* CANIF_TYPES_H */
