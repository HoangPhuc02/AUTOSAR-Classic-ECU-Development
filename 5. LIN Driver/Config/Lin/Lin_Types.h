/****************************************************************************************
*                               LIN_TYPES.H                                            *
****************************************************************************************
* File Name   : Lin_Types.h
* Module      : Local Interface Network (LIN)
* Description : AUTOSAR Lin driver type definitions header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef LIN_TYPES_H
#define LIN_TYPES_H

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "Std_Types.h"


/**
 * @details
 * This header file declares a total of 7 typedefs for the LIN driver module:
 *   - Lin_FramePidType: Represents a LIN protected identifier (uint8).
 *   - Lin_FrameCsModelType: Enumeration for LIN frame checksum model (Enhanced or Classic).
 *   - Lin_FrameResponseType: Enumeration for LIN frame response type (TX, RX, IGNORE).
 *   - Lin_FrameDlType: Specifies the data length of a LIN frame (uint8).
 *   - Lin_StatusType: Enumeration for LIN frame and channel operation status.
 *   - Lin_SlaveErrorType: Enumeration for LIN slave error types.
 *   - Lin_PduType: Structure containing LIN frame information (PID, checksum model, response type, data length, SDU pointer).
 *   - Lin_ConfigType: Structure for LIN driver configuration (implementation dependent).
 */

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define LIN_TYPES_VENDOR_ID                     (0x0001U)
#define LIN_TYPES_MODULE_ID                     (0x007BU)
#define LIN_TYPES_AR_RELEASE_MAJOR_VERSION      (4U)
#define LIN_TYPES_AR_RELEASE_MINOR_VERSION      (4U)
#define LIN_TYPES_AR_RELEASE_REVISION_VERSION   (0U)
#define LIN_TYPES_SW_MAJOR_VERSION              (1U)
#define LIN_TYPES_SW_MINOR_VERSION              (0U)
#define LIN_TYPES_SW_PATCH_VERSION              (0U)


/*==================================================================================================
*                                             ENUMS
==================================================================================================*/
/**
 * @brief   LIN Frame Checksum Model Type
 * @details This enumeration specifies the checksum model to be used for the LIN Frame.
 *          Range:
 *              LIN_ENHANCED_CS -- Enhanced checksum model
 *              LIN_CLASSIC_CS  -- Classic checksum model
 * @note    Available via Lin_GeneralTypes.h
 * SWS_Lin_00229
 */
typedef enum _Lin_FrameCsModelType
{
    LIN_ENHANCED_CS, 
    LIN_CLASSIC_CS 
} Lin_FrameCsModelType ;


/**
 * @brief   LIN Frame Response Type
 * @details This enumeration specifies whether the frame processor is required to transmit
 *          the response part of the LIN frame.
 *          Range:
 *              LIN_FRAMERESPONSE_TX    -- Response is generated from this node
 *              LIN_FRAMERESPONSE_RX    -- Response is generated from another node and is relevant for this node
 *              LIN_FRAMERESPONSE_IGNORE-- Response is generated from another node and is irrelevant for this node
 * @note    For LIN master nodes, the mapping to the previous definition is as follows:
 *              LIN_MASTER_RESPONSE <-> LIN_FRAMERESPONSE_TX
 *              LIN_SLAVE_RESPONSE  <-> LIN_FRAMERESPONSE_RX
 *              LIN_SLAVE_TO_SLAVE  <-> LIN_FRAMERESPONSE_IGNORE
 * @note    Available via Lin_GeneralTypes.h
 * SWS_Lin_00230
 */ 
typedef enum _Lin_FrameResponseType
{
    LIN_FRAMERESPONSE_TX,      /**< Response is generated from this node (transmit) */
    LIN_FRAMERESPONSE_RX,      /**< Response is generated from another node and is relevant for this node (receive) */
    LIN_FRAMERESPONSE_IGNORE   /**< Response is generated from another node and is irrelevant for this node (ignore) */
} Lin_FrameResponseType;


/**
 * @brief   LIN Status Type
 * @details
 * Name: Lin_StatusType
 * Kind: Enumeration
 * Range:
 *   LIN_NOT_OK           -- LIN frame operation return value. Development or production error occurred
 *   LIN_TX_OK            -- LIN frame operation return value. Successful transmission
 *   LIN_TX_BUSY          -- LIN frame operation return value. Ongoing transmission (Header or Response)
 *   LIN_TX_HEADER_ERROR  -- LIN frame operation return value. Erroneous header transmission such as:
 *                             - Mismatch between sent and read back data
 *                             - Identifier parity error
 *                             - Physical bus error
 *   LIN_TX_ERROR         -- LIN frame operation return value. Erroneous response transmission such as:
 *                             - Mismatch between sent and read back data
 *                             - Physical bus error
 *   LIN_RX_OK            -- LIN frame operation return value. Reception of correct response
 *   LIN_RX_BUSY          -- LIN frame operation return value. Ongoing reception: at least one response byte has been received, but the checksum byte has not been received
 *   LIN_RX_ERROR         -- LIN frame operation return value. Erroneous response reception such as:
 *                             - Framing error
 *                             - Overrun error
 *                             - Checksum error
 *                             - Short response
 *   LIN_RX_NO_RESPONSE   -- LIN frame operation return value. No response byte has been received so far
 *   LIN_OPERATIONAL      -- LIN channel state return value. Normal operation; the related LIN channel is ready to transmit next header. No data from previous frame available (e.g. after initialization)
 *   LIN_CH_SLEEP         -- LIN channel state return value. Sleep state operation; in this state wake-up detection from slave nodes is enabled
 * @note: LIN operation states for a LIN channel or frame, as returned by the API service Lin_GetStatus().
 * Available via Lin_GeneralTypes.h
 * SWS_Lin_00233
 */
typedef enum _Lin_StatusType
{
    LIN_NOT_OK,           /**< LIN frame operation return value. Development or production error occurred */
    LIN_TX_OK,            /**< Successful transmission */
    LIN_TX_BUSY,          /**< Ongoing transmission (Header or Response) */
    LIN_TX_HEADER_ERROR,  /**< Erroneous header transmission */
    LIN_TX_ERROR,         /**< Erroneous response transmission */
    LIN_RX_OK,            /**< Reception of correct response */
    LIN_RX_BUSY,          /**< Ongoing reception, checksum byte not received */
    LIN_RX_ERROR,         /**< Erroneous response reception */
    LIN_RX_NO_RESPONSE,   /**< No response byte received so far */
    LIN_OPERATIONAL,      /**< Normal operation, ready to transmit next header */
    LIN_CH_SLEEP          /**< Sleep state operation */
} Lin_StatusType;

/**
 * @brief   LIN Slave Error Type
 * @details
 * Name: Lin_SlaveErrorType
 * Kind: Enumeration
 * Range:
 *   LIN_ERR_HEADER         -- Error in header
 *   LIN_ERR_RESP_STOPBIT   -- Framing error in response
 *   LIN_ERR_RESP_CHKSUM    -- Checksum error
 *   LIN_ERR_RESP_DATABIT   -- Monitoring error of transmitted data bit in response
 *   LIN_ERR_NO_RESP        -- No response
 *   LIN_ERR_INC_RESP       -- Incomplete response
 * Description: This type represents the slave error types that are detected during header reception
 *              and response transmission/reception.
 * Available via Lin_GeneralTypes.h
 * SWS_Lin_91140
 */
typedef enum _Lin_SlaveErrorType
{
    LIN_ERR_HEADER,         /**< Error in header */
    LIN_ERR_RESP_STOPBIT,   /**< Framing error in response */
    LIN_ERR_RESP_CHKSUM,    /**< Checksum error */
    LIN_ERR_RESP_DATABIT,   /**< Monitoring error of transmitted data bit in response */
    LIN_ERR_NO_RESP,        /**< No response */
    LIN_ERR_INC_RESP        /**< Incomplete response */
} Lin_SlaveErrorType;


/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/**
 * @brief Represents a LIN protected identifier.
 *
 * @detail: The value range is 0...0xFE, which includes the LIN identifier (0...0x3F)
 * combined with its two parity bits. This type is used to represent all valid
 * protected identifiers that can be used by the Lin_SendFrame() function.
 *
 * @note Available via Lin_GeneralTypes.h.
 * SWS_Lin_00228
 */
typedef uint8 Lin_FramePidType;

/**
 * @brief   LIN Frame Data Length Type
 * @details Name: Lin_FrameDlType
 *          Kind: Type
 *          Derived from: uint8
 *          Range: 1...8 -- Data length of a LIN Frame
 *          Description: This type is used to specify the number of SDU data bytes to copy.
 *          Available via Lin_GeneralTypes.h
 * SWS_Lin_00231
 */

typedef uint8 Lin_FrameDlType;



/**
 * @brief   Lin_PduType Structure Description
 * @details
 * Name: Lin_PduType
 * Kind: Structure
 * Elements:
 *   - Pid
 *     Type: Lin_FramePidType
 *   - Cs
 *     Type: Lin_FrameCsModelType
 *   - Drc
 *     Type: Lin_FrameResponseType
 *   - Dl
 *     Type: Lin_FrameDlType
 *   - SduPtr
 *     Type: uint8*
 * @note: This type is used to provide PID, checksum model, data length and SDU pointer from
 *              the LIN Interface to the LIN driver.
 * Available via Lin_GeneralTypes.h
 * SWS_Lin_00232]
 */
typedef struct
{
    Lin_FramePidType        Pid;      /**< Protected Identifier (PID) for the LIN frame */
    Lin_FrameCsModelType    Cs;       /**< Checksum model type (Enhanced or Classic) */
    Lin_FrameResponseType   Drc;      /**< Frame response type (TX, RX, or IGNORE) */
    Lin_FrameDlType         Dl;       /**< Data length of the LIN frame (1...8 bytes) */
    uint8*                  SduPtr;   /**< Pointer to the frame's SDU (data buffer) */
} Lin_PduType;

/**
 * @typedef Lin_ChannelType
 * @brief Type definition for LIN channel identifier.
 *        Used to uniquely identify a LIN channel within the system.
 */
typedef uint8 Lin_ChannelType;

/**
 * @typedef Lin_BaudRateType
 * @brief Type definition for LIN baud rate.
 *        Represents the communication speed for a LIN channel in bits per second.
 */
typedef uint16 Lin_BaudRateType;

/**
 * @typedef Lin_ClockRefType
 * @brief Type definition for LIN clock reference.
 *        Used to specify the clock source or reference value for LIN timing operations.
 */
typedef uint32 Lin_ClockRefType;

/**
 * @brief LIN Channel Configuration
 */
typedef struct
{
    Lin_ChannelType     LinChannelId;               /**< LIN Channel ID */
    Lin_BaudRateType    LinChannelBaudRate;         /**< LIN Channel Baud Rate */
    USART_TypeDef*      LinHwChannel;               /**< Hardware USART Channel */
    Lin_ClockRefType    LinClockRef;                /**< Clock Reference */
    boolean             LinChannelWakeupSupport;    /**< Wakeup Support */
    uint8_t             IRQn;                   /**< USART IRQ number */
    uint8_t             IRQ_Priority;           /**< IRQ priority */
} Lin_ChannelConfigType;


/**
 * @brief LIN driver configuration structure.
 *
 * This structure contains the overall initialization data for the LIN driver,
 * including hardware-specific settings and SFR (Special Function Register) configurations
 * that affect the LIN channels. A pointer to this structure is provided to the LIN driver
 * initialization routine to configure the driver, LIN hardware unit, and LIN hardware channels.
 *
 * @note The contents of this structure are hardware and implementation dependent.
 *       Refer to the LIN hardware documentation for details on required configuration elements.
 *
 * @available Lin.h
 * [SWS_Lin_00227]
 */

typedef struct
{
    const Lin_ChannelConfigType* LinChannel;  /**< Pointer to channel configurations */
    uint8 LinNumberOfChannels;                /**< Number of LIN channels */
} Lin_ConfigType;

/*==================================================================================================
*                                     CONSTANTS AND MACROS  
==================================================================================================*/

/* Baud rate symbolic names */
#define LIN_BAUD_RATE_9600                           (0U)    /* PA0 - ADC1_IN0 */
#define LIN_BAUD_RATE_19200                           (1U)    /* PA1 - ADC1_IN1 */
#define LIN_BAUD_RATE_20000                           (2U)    /* PA2 - ADC1_IN2 */

/* LIN Hardware Unit  */
#define LIN_HW_UNIT_1                            (0U)    /* LIN hardware unit 1 */
#define LIN_HW_UNIT_2                            (1U)    /* LIN hardware unit 2 */

/* LIN Constants */
#define LIN_MAX_HW_UNITS                      (2U)       /* Maximum number of LIN hardware units */
#define LIN_MAX_HW_BAUD_RATES                 (20000)    /* Maximum number of LIN hardware baud rates 20Kbps*/
/*==================================================================================================
*                              VALIDATION MACROS                                                  *
==================================================================================================*/

/* Hardware unit validation */
#define LIN_IS_VALID_HW_UNIT(unit)      ((unit) < LIN_MAX_HW_UNITS)

/* Baud rate validation */
#define LIN_IS_VALID_BAUD_RATE(rate)    ((rate) <= LIN_MAX_HW_BAUD_RATES)











#ifdef __cplusplus
extern "C"{
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIN_TYPES_H */

