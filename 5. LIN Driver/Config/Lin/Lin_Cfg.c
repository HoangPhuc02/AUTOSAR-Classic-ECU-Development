/****************************************************************************************
*                               LIN_CFG.C                                            *
****************************************************************************************
* File Name   : Lin_Cfg.c
* Module      : Local Interface Network (LIN)
* Description : AUTOSAR Lin configuration driver implementation file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "Lin_Cfg.h"
#include "Lin_Types.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define LIN_CFG_VENDOR_ID_C                   (0x0001U)
#define LIN_CFG_MODULE_ID_C                   (0x007BU)
#define LIN_CFG_AR_RELEASE_MAJOR_VERSION_C    (4U)
#define LIN_CFG_AR_RELEASE_MINOR_VERSION_C    (4U)
#define LIN_CFG_AR_RELEASE_REVISION_VERSION_C (0U)
#define LIN_CFG_SW_MAJOR_VERSION_C            (1U)
#define LIN_CFG_SW_MINOR_VERSION_C            (0U)
#define LIN_CFG_SW_PATCH_VERSION_C            (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if Lin_Cfg.c file and Lin_Cfg.h file are of the same vendor */
#if (LIN_CFG_VENDOR_ID_C != LIN_CFG_VENDOR_ID)
    #error "Lin_Cfg.c and Lin_Cfg.h have different vendor ids"
#endif

/* Check if Lin_Cfg.c file and Lin_Cfg.h file are of the same AUTOSAR version */
#if ((LIN_CFG_AR_RELEASE_MAJOR_VERSION_C != LIN_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (LIN_CFG_AR_RELEASE_MINOR_VERSION_C != LIN_CFG_AR_RELEASE_MINOR_VERSION) || \
     (LIN_CFG_AR_RELEASE_REVISION_VERSION_C != LIN_CFG_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR Version Numbers of Lin_Cfg.c and Lin_Cfg.h are different"
#endif

/* Check if Lin_Cfg.c file and Lin_Cfg.h file are of the same software version */
#if ((LIN_CFG_SW_MAJOR_VERSION_C != LIN_CFG_SW_MAJOR_VERSION) || \
     (LIN_CFG_SW_MINOR_VERSION_C != LIN_CFG_SW_MINOR_VERSION) || \
     (LIN_CFG_SW_PATCH_VERSION_C != LIN_CFG_SW_PATCH_VERSION))
    #error "Software Version Numbers of Lin_Cfg.c and Lin_Cfg.h are different"
#endif

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/**
 * @brief   LIN Channel 0 Configuration (USART1 - Master)
 * @details Configuration for USART1 as LIN Master node
 */
const Lin_ChannelConfigType LinChannelConfig_USART1_Master = 
{
    /* LinChannelId */
    .LinChannelId = LIN_CHANNEL_0,
    
    /* LinHwChannel - Hardware USART instance */
    .LinHwChannel = USART1,
    
    /* LinChannelBaudRate */
    .LinChannelBaudRate = LIN_USART1_BAUDRATE,
    
    /* LinNodeType */
    .LinNodeType = LIN_MASTER,
    
    /* LinChannelWakeupSupport */
#if (LIN_CHANNEL_0_WAKEUP_SUPPORT == STD_ON)
    .LinChannelWakeupSupport = TRUE,
#else
    .LinChannelWakeupSupport = FALSE,
#endif
    
    /* STM32F103 Specific Configuration */
    .IRQn = USART1_IRQn,
    .IRQ_Priority = LIN_USART1_IRQ_PRIORITY,
};

/**
 * @brief   LIN Channel 1 Configuration (USART2 - Slave)
 * @details Configuration for USART2 as LIN Slave node
 */
const Lin_ChannelConfigType LinChannelConfig_USART2_Slave = 
{
    /* LinChannelId */
    .LinChannelId = LIN_CHANNEL_1,
    
    /* LinHwChannel - Hardware USART instance */
    .LinHwChannel = USART2,
    
    /* LinChannelBaudRate */
    .LinChannelBaudRate = LIN_USART2_BAUDRATE,
    
    /* LinNodeType */
    .LinNodeType = LIN_SLAVE,
    
    /* LinChannelWakeupSupport */
#if (LIN_CHANNEL_1_WAKEUP_SUPPORT == STD_ON)
    .LinChannelWakeupSupport = TRUE,
    
    /* LinChannelEcuMWakeupSource */
    .LinChannelEcuMWakeupSource = ECUM_WKSOURCE_LIN_CHANNEL_1,
#else
    .LinChannelWakeupSupport = FALSE,
#endif
    
    /* STM32F103 Specific Configuration */
    .IRQn = USART2_IRQn,
    .IRQ_Priority = LIN_USART2_IRQ_PRIORITY,
};

/**
 * @brief   LIN Channel Configuration Array
 * @details Array containing all configured LIN channels
 */
const Lin_ChannelConfigType LinChannelConfig[LIN_MAX_CONFIGURED_CHANNELS] = 
{
    /* Channel 0 - USART1 Master */
    LinChannelConfig_USART1_Master,
    
    /* Channel 1 - USART2 Slave */
    LinChannelConfig_USART2_Slave

};

/**
 * @brief   LIN Configuration Set
 * @details Main configuration structure for LIN driver
 */
const Lin_ConfigType LinConfigSet = 
{
    /* LinNumberOfChannels */
    .LinNumberOfChannels = LIN_MAX_CONFIGURED_CHANNELS,
    
    /* LinChannel */
    .LinChannel = LinChannelConfig
};

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

#if (LIN_DEV_ERROR_DETECT == STD_ON)
/**
 * @brief   Validate Global Configuration Pointer
 * @details Validates the global LIN configuration pointer
 * 
 * @param[in] ConfigPtr Pointer to configuration to validate
 * 
 * @return Std_ReturnType
 *   @retval E_OK Configuration is valid
 *   @retval E_NOT_OK Configuration is invalid
 */
Std_ReturnType Lin_ValidateGlobalConfigPtr(const Lin_ConfigType* ConfigPtr)
{
    Std_ReturnType retVal = E_OK;
    
    if (ConfigPtr == NULL_PTR)
    {
        retVal = E_NOT_OK;
    }
    else if (ConfigPtr->LinNumberOfChannels == 0U)
    {
        retVal = E_NOT_OK;
    }
    else if (ConfigPtr->LinNumberOfChannels > LIN_MAX_CONFIGURED_CHANNELS)
    {
        retVal = E_NOT_OK;
    }
    else if (ConfigPtr->LinChannel == NULL_PTR)
    {
        retVal = E_NOT_OK;
    }
    else
    {
        /* Configuration appears valid */
    }
    
    return retVal;
}

/**
 * @brief   Validate Channel Configuration
 * @details Validates individual channel configuration
 * 
 * @param[in] ChannelConfigPtr Pointer to channel configuration to validate
 * 
 * @return Std_ReturnType
 *   @retval E_OK Configuration is valid
 *   @retval E_NOT_OK Configuration is invalid
 */
Std_ReturnType Lin_ValidateChannelConfig(const Lin_ChannelConfigType* ChannelConfigPtr)
{
    Std_ReturnType retVal = E_OK;
    
    if (ChannelConfigPtr == NULL_PTR)
    {
        retVal = E_NOT_OK;
    }
    else if (ChannelConfigPtr->LinHwChannel == NULL_PTR)
    {
        retVal = E_NOT_OK;
    }
    else if ((ChannelConfigPtr->LinNodeType != LIN_MASTER) && 
             (ChannelConfigPtr->LinNodeType != LIN_SLAVE))
    {
        retVal = E_NOT_OK;
    }
    else if (ChannelConfigPtr->LinChannelBaudRate == 0U)
    {
        retVal = E_NOT_OK;
    }
    else
    {
        /* Configuration appears valid */
    }
    
    return retVal;
}
#endif /* LIN_DEV_ERROR_DETECT */

/*==================================================================================================
*                                       END OF FILE
==================================================================================================*/