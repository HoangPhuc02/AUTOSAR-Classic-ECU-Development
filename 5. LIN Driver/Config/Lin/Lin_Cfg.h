/****************************************************************************************
*                               LIN_CFG.H                                            *
****************************************************************************************
* File Name   : Lin_Cfg.h
* Module      : Local Interface Network (LIN)
* Description : AUTOSAR Lin configuration driver header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef LIN_CFG_H
#define LIN_CFG_H

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "Std_Types.h"
#include "Lin_Types.h"
#include "stm32f10x.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define LIN_CFG_VENDOR_ID                     (0x0001U)
#define LIN_CFG_MODULE_ID                     (0x007BU)
#define LIN_CFG_AR_RELEASE_MAJOR_VERSION      (4U)
#define LIN_CFG_AR_RELEASE_MINOR_VERSION      (4U)
#define LIN_CFG_AR_RELEASE_REVISION_VERSION   (0U)
#define LIN_CFG_SW_MAJOR_VERSION              (1U)
#define LIN_CFG_SW_MINOR_VERSION              (0U)
#define LIN_CFG_SW_PATCH_VERSION              (0U)

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/**
 * @brief   LIN Driver General Configuration
 * @details General configuration parameters for LIN driver
 */

/* Development Error Detection */
#define LIN_DEV_ERROR_DETECT                  (STD_OFF)

/* Version Info API */
#define LIN_VERSION_INFO_API                  (STD_ON)

/* De-initialization API */
#define LIN_DEINIT_API                        (STD_ON)

/* Enable/Disable Check Wakeup API */
#define LIN_ENABLE_CHECK_WAKEUP_API           (STD_OFF)

/* Wakeup Support Configuration */
#define LIN_WAKEUP_SUPPORT_API                (STD_OFF)

/* Wake up validation API */
#define LIN_WAKEUP_VALIDATION_API             (STD_OFF)


/**
 * @brief   Number of Configured Elements
 * @details Total number of channels configured
 */
#define LIN_MAX_CONFIGURED_CHANNELS           (2U)  /* USART1 as master and USART2 as slave */
#define LIN_MAX_DATA_LENGTH                   (8U)  /* Maximum LIN frame data length */

/**
 * @brief   LIN Channel Definitions
 * @details Symbolic names for configured LIN channels
 */
#define LIN_CHANNEL_0                         (0U)  /* USART1 - Master */
#define LIN_CHANNEL_1                         (1U)  /* USART2 - Slave */


/* LIN Break Field Configuration */
#define LIN_BREAK_LENGTH_10BIT                (10U)
#define LIN_BREAK_LENGTH_11BIT                (11U)
#define LIN_DEFAULT_BREAK_LENGTH              LIN_BREAK_LENGTH_11BIT

/* LIN Timing Configuration */
#define LIN_HEADER_TIMEOUT_MS                 (10U)
#define LIN_RESPONSE_TIMEOUT_MS               (20U)
#define LIN_WAKEUP_TIMEOUT_MS                 (150U)


/**
 * @brief   LIN Channel Configuration IDs
 * @details Symbolic names for channel configuration
 */
#define LinConf_LinChannel_USART1_MASTER      (LIN_CHANNEL_0)
#define LinConf_LinChannel_USART2_SLAVE       (LIN_CHANNEL_1)


/**
 * @brief   Configuration Validation Macros
 * @details Macros for validating configuration parameters
 */
#define LIN_VALIDATE_CHANNEL(channel) \
    ((channel) < LIN_MAX_CONFIGURED_CHANNELS)

#define LIN_VALIDATE_POINTER(ptr) \
    ((ptr) != NULL_PTR)

#define LIN_VALIDATE_PID(pid) \
    (((pid) & 0x3FU) <= 0x3FU)

#define LIN_VALIDATE_DATA_LENGTH(length) \
    (((length) > 0U) && ((length) <= LIN_MAX_DATA_LENGTH))

/**
 * @brief   Hardware-Specific Configuration
 * @details Configuration parameters specific to STM32F103C8T6
 */

/* USART Configuration */
#define LIN_USART1_BAUDRATE                   (19200U)    /* LIN standard baudrate */
#define LIN_USART2_BAUDRATE                   (19200U)    /* LIN standard baudrate */
#define LIN_USART3_BAUDRATE                   (19200U)    /* LIN standard baudrate */

/* Default Configuration Values */
#define LIN_DEFAULT_WORD_LENGTH               (8U)        /* 8-bit word length */
#define LIN_DEFAULT_STOP_BITS                 (1U)        /* 1 stop bit */
#define LIN_DEFAULT_PARITY                    (0U)        /* No parity */

/* NVIC Priority Configuration */
#define LIN_USART1_IRQ_PRIORITY               (1U)
#define LIN_USART2_IRQ_PRIORITY               (1U)
#define LIN_USART3_IRQ_PRIORITY               (1U)

/**
 * @brief   Channel-Specific Wakeup Support Configuration
 * @details Individual wakeup support configuration for each channel
 */
#define LIN_CHANNEL_0_WAKEUP_SUPPORT          (STD_OFF)    /* USART1 no wakeup support */
#define LIN_CHANNEL_1_WAKEUP_SUPPORT          (STD_OFF)    /* USART2 no wakeup support */



/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/**
 * @brief   LIN Configuration Structure
 * @details External declaration of the LIN configuration structure
 */
extern const Lin_ConfigType* Lin_ConfigPtr;
/**
 * @brief   LIN Channel Configuration Structures
 * @details External declarations of individual channel configurations
 */
extern const Lin_ChannelConfigType LinChannelConfig_USART1_Master;
extern const Lin_ChannelConfigType LinChannelConfig_USART2_Slave;



/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief   Configuration Validation Functions
 * @details Functions to validate LIN configuration at runtime
 */
#if (LIN_DEV_ERROR_DETECT == STD_ON)
Std_ReturnType Lin_ValidateGlobalConfigPtr(const Lin_ConfigType* ConfigPtr);
Std_ReturnType Lin_ValidateChannelConfig(const Lin_ChannelConfigType* ChannelConfigPtr);
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIN_CFG_H */

/**
 * @}
 */
