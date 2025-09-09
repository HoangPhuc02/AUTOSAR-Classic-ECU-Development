/****************************************************************************************
*                               CAN_TYPES.H                                            *
****************************************************************************************
* File Name   : Can_Types.h
* Module      : Controller Area Network (CAN)
* Description : AUTOSAR Can driver type definitions header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef CAN_TYPES_H
#define CAN_TYPES_H

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "Std_Types.h"
#include "stm32f10x.h"
#include "misc.h"

/**
 * @brief   File Overview
 * @details This header file contains AUTOSAR-compliant type definitions for CAN and LIN drivers:
 *
 * CAN Driver Types:
 *   - Can_ControllerStateType: CAN controller states (UNINIT, STARTED, STOPPED, SLEEP)
 *   - Can_ErrorStateType: CAN error states (ACTIVE, PASSIVE, BUSOFF)
 *   - Can_ErrorType: CAN error types (bit monitoring, ACK, arbitration, CRC, etc.)
 *   - Can_IdType: CAN identifier (Standard/Extended, 11/29-bit)
 *   - Can_HwHandleType: Hardware object handle (8/16-bit)
 *   - Can_PduType: CAN PDU structure (ID, length, data pointer)
 *   - Can_HwType: Hardware info structure (ID, handle, controller)
 *   - Can_TimeStampType: Time stamp structure (seconds, nanoseconds)
 *   - Can_ConfigType: Configuration structure
 *
 * Constants and Macros:
 *   - CAN_BUSY: Return value for busy state
 *   - CAN ID masks for Standard/Extended frames
 */

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CAN_TYPES_VENDOR_ID                     (0x0001U)
#define CAN_TYPES_MODULE_ID                     (0x007BU)
#define CAN_TYPES_AR_RELEASE_MAJOR_VERSION      (4U)
#define CAN_TYPES_AR_RELEASE_MINOR_VERSION      (4U)
#define CAN_TYPES_AR_RELEASE_REVISION_VERSION   (0U)
#define CAN_TYPES_SW_MAJOR_VERSION              (1U)
#define CAN_TYPES_SW_MINOR_VERSION              (0U)
#define CAN_TYPES_SW_PATCH_VERSION              (0U)


/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/**
 * @brief   CAN Controller State Type
 * @details States that are used by the several ControllerMode functions.
 *          Range:
 *              CAN_CS_UNINIT  -- 0x00 CAN controller state UNINIT
 *              CAN_CS_STARTED -- 0x01 CAN controller state STARTED
 *              CAN_CS_STOPPED -- 0x02 CAN controller state STOPPED
 *              CAN_CS_SLEEP   -- 0x03 CAN controller state SLEEP
 * @note    Available via Can_GeneralTypes.h
 * SWS_Can_91013
 */
typedef enum _Can_ControllerStateType
{
    CAN_CS_UNINIT  = 0x00,    /**< CAN controller state UNINIT */
    CAN_CS_STARTED = 0x01,    /**< CAN controller state STARTED */
    CAN_CS_STOPPED = 0x02,    /**< CAN controller state STOPPED */
    CAN_CS_SLEEP   = 0x03     /**< CAN controller state SLEEP */
} Can_ControllerStateType;

/**
 * @brief   CAN Error State Type
 * @details Error states of a CAN controller.
 *          Range:
 *              CAN_ERRORSTATE_ACTIVE  -- The CAN controller takes fully part in communication
 *              CAN_ERRORSTATE_PASSIVE -- The CAN controller takes part in communication, but does not send active error frames
 *              CAN_ERRORSTATE_BUSOFF  -- The CAN controller does not take part in communication
 * @note    Available via Can_GeneralTypes.h
 * SWS_Can_91003
 */
typedef enum _Can_ErrorStateType
{
    CAN_ERRORSTATE_ACTIVE,     /**< The CAN controller takes fully part in communication */
    CAN_ERRORSTATE_PASSIVE,    /**< The CAN controller takes part in communication, but does not send active error frames */
    CAN_ERRORSTATE_BUSOFF      /**< The CAN controller does not take part in communication */
} Can_ErrorStateType;

/**
 * @brief   CAN Error Type
 * @details The enumeration represents a superset of CAN Error Types which typical CAN HW is able to
 *          report. That means not all CAN HW will be able to support the complete set.
 *          Range:
 *              CAN_ERROR_BIT_MONITORING1    -- 0x01 A 0 was transmitted and a 1 was read back
 *              CAN_ERROR_BIT_MONITORING0    -- 0x02 A 1 was transmitted and a 0 was read back
 *              CAN_ERROR_BIT                -- 0x03 The HW reports a CAN bit error but cannot distinguish between MONITORING1 and MONITORING0
 *              CAN_ERROR_CHECK_ACK_FAILED   -- 0x04 Acknowledgement check failed
 *              CAN_ERROR_ACK_DELIMITER      -- 0x05 Acknowledgement delimiter check failed
 *              CAN_ERROR_ARBITRATION_LOST   -- 0x06 The sender lost in arbitration
 *              CAN_ERROR_OVERLOAD           -- 0x07 CAN overload detected via an overload frame
 *              CAN_ERROR_CHECK_FORM_FAILED  -- 0x08 Violations of the fixed frame format
 *              CAN_ERROR_CHECK_STUFFING_FAILED -- 0x09 Stuffing bits not as expected
 *              CAN_ERROR_CHECK_CRC_FAILED   -- 0x0A CRC failed
 *              CAN_ERROR_BUS_LOCK           -- 0x0B Bus lock (Bus is stuck to dominant level)
 * @note    Available via Can_GeneralTypes.h
 * SWS_Can_91021
 */
typedef enum _Can_ErrorType
{
    CAN_ERROR_BIT_MONITORING1      = 0x01,    /**< A 0 was transmitted and a 1 was read back */
    CAN_ERROR_BIT_MONITORING0      = 0x02,    /**< A 1 was transmitted and a 0 was read back */
    CAN_ERROR_BIT                  = 0x03,    /**< The HW reports a CAN bit error but cannot distinguish between MONITORING1 and MONITORING0 */
    CAN_ERROR_CHECK_ACK_FAILED     = 0x04,    /**< Acknowledgement check failed */
    CAN_ERROR_ACK_DELIMITER        = 0x05,    /**< Acknowledgement delimiter check failed */
    CAN_ERROR_ARBITRATION_LOST     = 0x06,    /**< The sender lost in arbitration */
    CAN_ERROR_OVERLOAD             = 0x07,    /**< CAN overload detected via an overload frame. Indicates that the receive buffers of a receiver are full */
    CAN_ERROR_CHECK_FORM_FAILED    = 0x08,    /**< Violations of the fixed frame format */
    CAN_ERROR_CHECK_STUFFING_FAILED = 0x09,   /**< Stuffing bits not as expected */
    CAN_ERROR_CHECK_CRC_FAILED     = 0x0A,    /**< CRC failed */
    CAN_ERROR_BUS_LOCK             = 0x0B     /**< Bus lock (Bus is stuck to dominant level) */
} Can_ErrorType;


/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/**
 * @brief   CAN ID Type
 * @details Represents the Identifier of an L-PDU. The two most significant bits specify the frame type:
 *          00 - CAN message with Standard CAN ID
 *          01 - CAN FD frame with Standard CAN ID
 *          10 - CAN message with Extended CAN ID
 *          11 - CAN FD frame with Extended CAN ID
 *          Range:
 *              Standard 32 Bit: 0..0x400007FF
 *              Extended 32 Bit: 0..0xDFFFFFFF
 * @note    Available via Can_GeneralTypes.h
 * SWS_Can_00416
 */
typedef uint32 Can_IdType;

/**
 * @brief   CAN Hardware Handle Type
 * @details Represents the hardware object handles of a CAN hardware unit.
 *          For CAN hardware units with more than 255 HW objects use extended range.
 *          Range:
 *              Standard: 0..0x0FF
 *              Extended: 0..0xFFFF
 * @note    Available via Can_GeneralTypes.h
 * SWS_Can_00429
 */
typedef uint16 Can_HwHandleType;

/**
 * @brief   PDU ID Type
 * @details Type used to store the identifier of a PDU.
 * @note    Available via ComStack_Types.h
 */
typedef uint16 PduIdType;

/**
 * @brief   CAN PDU Type
 * @details This type unites PduId (swPduHandle), SduLength (length), SduData (sdu), and CanId (id) for any CAN L-SDU.
 * @note    Available via Can_GeneralTypes.h
 * SWS_Can_00415
 */
typedef struct _Can_PduType
{
    PduIdType   swPduHandle;    /**< Handle ID of AUTOSAR COM module for notification */
    uint8       length;         /**< Length of CAN frame (DLC) */
    Can_IdType  id;            /**< Standard/Extended CAN ID of CAN L-PDU */
    uint8*      sdu;           /**< Pointer to L-SDU (payload) */
} Can_PduType;

/**
 * @brief   CAN Hardware Type
 * @details This type defines a data structure which clearly provides a Hardware Object Handle including its
 *          corresponding CAN Controller and therefore Can Driver as well as the specific CanId.
 * @note    Available via Can_GeneralTypes.h
 * SWS_Can_00496
 */
typedef struct _Can_HwType
{
    Can_IdType          CanId;         /**< Standard/Extended CAN ID of CAN L-PDU */
    Can_HwHandleType    Hoh;           /**< ID of the corresponding Hardware Object Range */
    uint8               ControllerId;   /**< Controller Id provided by CanIf clearly identify the corresponding controller */
} Can_HwType;

/**
 * @brief   CAN Time Stamp Type (DRAFT)
 * @details Variables of this type are used to express time stamps based on relative time.
 *          Value range: 
 *              * Seconds: 0 .. 4.294.967.295 s (circa 136 years) 
 *              * Nanoseconds: 0 .. 999.999.999 ns
 * @note    Available via Can_GeneralTypes.h
 * @note    Tags: atp.Status=draft
 * SWS_Can_91029
 */
typedef struct _Can_TimeStampType
{
    uint32 nanoseconds;    /**< Nanoseconds part of the time */
    uint32 seconds;        /**< Seconds part of the time */
} Can_TimeStampType;

/**
 * @brief   CAN Configuration Type
 * @details This is the type of the external data structure containing the overall initialization data for the CAN
 *          driver and SFR settings affecting all controllers. Furthermore it contains pointers to controller
 *          configuration structures. The contents of the initialization data structure are CAN hardware specific.
 * @note    Available via Can.h
 * SWS_Can_00413
 */
typedef struct _Can_ConfigType
{
    /* Implementation specific configuration data */
    uint8 ConfigSet;                    /**< Configuration set identifier */
    /* Note: Additional members will be added based on specific hardware requirements */
} Can_ConfigType;






/*==================================================================================================
*                                     CONSTANTS AND MACROS  
==================================================================================================*/


/*==================================================================================================
*                                       CONSTANTS
==================================================================================================*/

/**
 * @brief   CAN_BUSY Return Value
 * @details Extension to Std_ReturnType for CAN driver API Can_Write()
 *          Indicates that transmit request could not be processed because no transmit object was available
 * @note    Available via Can_GeneralTypes.h
 * SWS_Can_00039
 */
#define CAN_BUSY                    ((Std_ReturnType)0x02U)

/**
 * @brief   CAN Controller ID Invalid
 * @details Invalid CAN Controller ID used for error indication
 */
#define CAN_CONTROLLER_ID_INVALID   ((uint8)0xFFU)

/**
 * @brief   CAN Hardware Handle Invalid
 * @details Invalid CAN Hardware Handle used for error indication
 */
#define CAN_HW_HANDLE_INVALID       ((Can_HwHandleType)0xFFFFU)

/**
 * @brief   CAN Standard ID Mask
 * @details Mask for Standard CAN ID (11-bit)
 */
#define CAN_STD_ID_MASK             ((Can_IdType)0x000007FFU)

/**
 * @brief   CAN Extended ID Mask  
 * @details Mask for Extended CAN ID (29-bit)
 */
#define CAN_EXT_ID_MASK             ((Can_IdType)0x1FFFFFFFU)

/**
 * @brief   CAN Frame Type Mask
 * @details Mask for frame type bits in CAN ID (2 most significant bits)
 */
#define CAN_FRAME_TYPE_MASK         ((Can_IdType)0xC0000000U)

/* CAN Constants */
#define CAN_MAX_HW_UNITS            (2U)        /**< Maximum number of CAN hardware units */
#define CAN_MAX_HW_BAUD_RATES       (1000000U)  /**< Maximum CAN hardware baud rates 1Mbps */
#define CAN_MAX_DATA_LENGTH         (8U)        /**< Maximum data length for CAN frames */

/*==================================================================================================
*                              VALIDATION MACROS                                                  *
==================================================================================================*/

/* Hardware unit validation */
#define CAN_IS_VALID_HW_UNIT(unit)      ((unit) < CAN_MAX_HW_UNITS)

/* Baud rate validation */
#define CAN_IS_VALID_BAUD_RATE(rate)    ((rate) <= CAN_MAX_HW_BAUD_RATES)

/*==================================================================================================
*                                   EXTERNAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   EXTERNAL FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* CAN_TYPES_H */

