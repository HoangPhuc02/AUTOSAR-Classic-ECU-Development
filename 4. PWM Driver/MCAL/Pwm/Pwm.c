/****************************************************************************************
*                                 PWM.C                                                *
****************************************************************************************
* File Name   : Pwm.c
* Module      : Pulse Width Modulation (PWM)
* Description : AUTOSAR PWM driver main implementation
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Pwm.h"
#include "Pwm_Types.h"
#include "Pwm_Hw.h"
#include "Det.h"


/****************************************************************************************
*                              GLOBAL VARIABLES                                        *
****************************************************************************************/

/**
 * @brief PWM driver state
 * @details Indicates whether the PWM driver is initialized or not
 */
Pwm_DriverStateType Pwm_DriverState = PWM_STATE_UNINIT;

/**
 * @brief PWM configuration pointer
 * @details Pointer to the current PWM configuration
 */
const Pwm_ConfigType* Pwm_ConfigPtr = NULL_PTR;

/****************************************************************************************
*                              LOCAL FUNCTION PROTOTYPES                              *
****************************************************************************************/
/* if detective error is turn on*/
#if (PWM_DEV_ERROR_DETECT == STD_ON)
static inline Std_ReturnType Pwm_ValidateCfgPtr(const Pwm_ConfigType* ConfigPtr, uint8 ServiceId);
static inline Std_ReturnType Pwm_ValidateInit(uint8 ServiceId);
static inline Std_ReturnType Pwm_ValidateChannel(Pwm_ChannelType ChannelNumber, uint8 ServiceId);
static inline Std_ReturnType Pwm_ValidateChannelClass(Pwm_ChannelType ChannelNumber, uint8 ServiceId);
static inline Std_ReturnType Pwm_ValidatePointer(const void* Pointer, uint8 ServiceId);
static inline Std_ReturnType Pwm_ValidateDutyCycle(uint16 DutyCycle, uint8 ServiceId);
static inline Std_ReturnType Pwm_ValidatePeriod(Pwm_PeriodType Period, uint8 ServiceId);
#endif

/****************************************************************************************
*                              INITIALIZATION FUNCTIONS                               *
****************************************************************************************/

/**
 * @brief Service for PWM initialization
 * @details [SWS_Pwm_00095] Definition of API function Pwm_Init
 * @param[in] ConfigPtr Pointer to configuration set
 * @return void
 * @ServiceID 0x00
 * @Sync Synchronous
 * @Reentrancy Non Reentrant
 *  [SWS_Pwm_00118]
 */
// TODO Check hw
/* If the duty cycle parameter equals:
 [SWS_Pwm_20009]
 Upstream requirements: SRS_SPAL_12057
 0%or 100%: Thenthe PWMoutput signal shall be in the state according to the
 configured polarity parameter
 [SWS_Pwm_30009]
 Upstream requirements: SRS_SPAL_12057
 >0% and <100%: Then the PWM output signal shall be modulated according to
 parameters period, duty cycle and configured polarity*/
void Pwm_Init(const Pwm_ConfigType* ConfigPtr)
{
    uint8 ChannelIndex;
    uint8 HwUnitIndex;
    
#if (PWM_DEV_ERROR_DETECT == STD_ON)
    /* Check if driver is already initialized  [SWS_Pwm_00118]*/
    if (Pwm_DriverState == PWM_STATE_INIT)
    {
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_INIT_ID, PWM_E_ALREADY_INITIALIZED);
        return;
    }
    
    /* Validate configuration pointer */
    if (Pwm_ValidateCfgPtr(ConfigPtr, PWM_INIT_ID) != E_OK)
    {
        return;
    }
#endif
    
    /* Store configuration pointer */
    Pwm_ConfigPtr = ConfigPtr;
    
    
    /* Initialize hardware units */
    for (HwUnitIndex = 0; HwUnitIndex < ConfigPtr->PwmMaxHwUnits; HwUnitIndex++)
    {
        const Pwm_HwUnitConfigType* HwUnitConfig = &ConfigPtr->PwmHwUnitConfig[HwUnitIndex];
        
        /* Check if hardware unit is enabled */
        if (PWM_HW_IS_TIMER_ENABLED(HwUnitConfig->HwUnit))
        {
            PwmHw_InitHwUnit(HwUnitConfig->HwUnit, HwUnitConfig);
        }
    }
    
    /* Initialize PWM channels */
    for (ChannelIndex = 0; ChannelIndex < ConfigPtr->PwmMaxChannels; ChannelIndex++)
    {
        Pwm_ChannelConfigType* ChannelConfig = &ConfigPtr->PwmChannelConfig[ChannelIndex];
        
        /* Check if channel's hardware unit is enabled */
        if (PWM_HW_IS_TIMER_ENABLED(ChannelConfig->HwUnit))
        {
            (void)PwmHw_InitChannel(ChannelConfig->ChannelId);
            ChannelConfig->NotificationEnabled = FALSE;
        }
    }
    NVIC_EnableIRQ(TIM1_UP_IRQn);
    NVIC_EnableIRQ(TIM1_CC_IRQn);
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_EnableIRQ(TIM4_IRQn);
    /* Set driver state to initialized */
    Pwm_DriverState = PWM_STATE_INIT;
}

/**
 * @brief Service for PWM De-Initialization
 * @details [SWS_Pwm_00096] Definition of API function Pwm_DeInit
 * @return void
 * @ServiceID 0x01
 * @Sync Synchronous
 * @Reentrancy Non Reentrant
 */
void Pwm_DeInit(void)
{
    uint8 HwUnitIndex;
    uint8 ChannelIndex;
    
#if (PWM_DEV_ERROR_DETECT == STD_ON)
    /* Check if driver is initialized */
    if (Pwm_ValidateInit(PWM_DEINIT_ID) != E_OK)
    {
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_DEINIT_ID, PWM_E_UNINIT);
        return;
    }
#endif
    
    /* Set all channels to idle state */
    for (ChannelIndex = 0; ChannelIndex < PWM_MAX_CHANNELS; ChannelIndex++)
    {
        (void)PwmHw_SetOutputToIdle(ChannelIndex);
        (void)PwmHw_DisableNotification(ChannelIndex);
    }
    
    /* De-initialize hardware units */
    for (HwUnitIndex = 0; HwUnitIndex < PWM_MAX_HW_UNITS; HwUnitIndex++)
    {
        (void)PwmHw_DeInitHwUnit(HwUnitIndex);
    }
    
    /* Clear configuration pointer */
    Pwm_ConfigPtr = NULL_PTR;
    
    /* Set driver state to uninitialized */
    Pwm_DriverState = PWM_STATE_UNINIT;
}

/****************************************************************************************
*                              CHANNEL CONTROL FUNCTIONS                              *
****************************************************************************************/
#if(PWM_SET_DUTY_CYCLE_API == STD_ON)
/**
 * @brief Service to set the duty cycle of the PWM channel
 * @details [SWS_Pwm_00097] Definition of API function Pwm_SetDutyCycle
 * @param[in] ChannelNumber Numeric identifier of the PWM channel
 * @param[in] DutyCycle Min=0x0000 Max=0x8000
 * @return void
 * @ServiceID 0x02
 * @Sync Synchronous
 * @Reentrancy Reentrant for different channel numbers
 */
void Pwm_SetDutyCycle(Pwm_ChannelType ChannelNumber, uint16 DutyCycle)
{
#if (PWM_DEV_ERROR_DETECT == STD_ON)
    /* Check if driver is initialized */
    if (Pwm_ValidateInit(PWM_SET_DUTY_CYCLE_ID) != E_OK)
    {
        return;
    }
    
    /* Validate channel ID */
    if (Pwm_ValidateChannel(ChannelNumber, PWM_SET_DUTY_CYCLE_ID) != E_OK)
    {
        return;
    }
    
    /* Validate duty cycle */
    if (Pwm_ValidateDutyCycle(DutyCycle, PWM_SET_DUTY_CYCLE_ID) != E_OK)
    {
        return;
    }
#endif
    
    /* Set duty cycle */
    (void)PwmHw_SetDutyCycle(ChannelNumber, DutyCycle);
}
#endif

#if (PWM_SET_PERIOD_AND_DUTY_API == STD_ON)
/**
 * @brief Service to set the period and the duty cycle of a PWM channel
 * @details [SWS_Pwm_00098] Definition of API function Pwm_SetPeriodAndDuty
 * @param[in] ChannelNumber Numeric identifier of the PWM channel
 * @param[in] Period Period of the PWM signal
 * @param[in] DutyCycle Min=0x0000 Max=0x8000
 * @return void
 * @ServiceID 0x03
 * @Sync Synchronous
 * @Reentrancy Reentrant for different channel numbers
 */
void Pwm_SetPeriodAndDuty(Pwm_ChannelType ChannelNumber, Pwm_PeriodType Period, uint16 DutyCycle)
{
    const Pwm_ChannelConfigType* ChannelConfig;
    
#if (PWM_DEV_ERROR_DETECT == STD_ON)
    /* Check if driver is initialized */
    if (Pwm_ValidateInit(PWM_SET_PERIOD_AND_DUTY_ID) != E_OK)
    {
        return;
    }
    
    /* Validate channel ID */
    if (Pwm_ValidateChannel(ChannelNumber, PWM_SET_PERIOD_AND_DUTY_ID) != E_OK)
    {
        return;
    }
    
    /* Validate period */
    if (Pwm_ValidatePeriod(Period, PWM_SET_PERIOD_AND_DUTY_ID) != E_OK)
    {
        return;
    }
    
    /* Validate duty cycle */
    if (Pwm_ValidateDutyCycle(DutyCycle, PWM_SET_PERIOD_AND_DUTY_ID) != E_OK)
    {
        return;
    }
    
    /* Check if channel supports variable period */
    if (Pwm_ValidateChannelClass(ChannelNumber, PWM_SET_PERIOD_AND_DUTY_ID) != E_OK)
    {
        return;
    }
#endif
    
    /* Get channel configuration */
    ChannelConfig = Pwm_GetChannelConfig(ChannelNumber);
    
    /* Check if channel class allows period change */

    // TODO why need that when it already haved
    if (ChannelConfig->ChannelClass != PWM_VARIABLE_PERIOD)
    {
#if (PWM_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_PERIOD_AND_DUTY_ID, PWM_E_PERIOD_UNCHANGEABLE);
#endif
        return;
    }
    
    /* Set period and duty cycle */
    (void)PwmHw_SetPeriodAndDuty(ChannelNumber, Period, DutyCycle);
}
#endif /* PWM_SET_PERIOD_AND_DUTY_API */

#if (PWM_SET_OUTPUT_TO_IDLE_API == STD_ON)
/**
 * @brief Service to set the PWM output to the configured Idle state
 * @details [SWS_Pwm_00099] Definition of API function Pwm_SetOutputToIdle
 * @param[in] ChannelNumber Numeric identifier of the PWM channel
 * @return void
 * @ServiceID 0x04
 * @Sync Synchronous
 * @Reentrancy Reentrant for different channel numbers
 */
void Pwm_SetOutputToIdle(Pwm_ChannelType ChannelNumber)
{
#if (PWM_DEV_ERROR_DETECT == STD_ON)
    /* Check if driver is initialized */
    if (Pwm_ValidateInit(PWM_SET_OUTPUT_TO_IDLE_ID) != E_OK)
    {
        return;
    }
    
    /* Validate channel ID */
    if (Pwm_ValidateChannel(ChannelNumber, PWM_SET_OUTPUT_TO_IDLE_ID) != E_OK)
    {
        return;
    }
#endif
    
    /* Set output to idle */
    (void)PwmHw_SetOutputToIdle(ChannelNumber);
}
#endif /* PWM_SET_OUTPUT_TO_IDLE_API */

#if (PWM_GET_OUTPUT_STATE_API == STD_ON)
/**
 * @brief Service to read the internal state of the PWM output signal
 * @details [SWS_Pwm_00100] Definition of API function Pwm_GetOutputState
 * @param[in] ChannelNumber Numeric identifier of the PWM channel
 * @return PWM_HIGH The PWM output state is high
 * @return PWM_LOW The PWM output state is low
 * @ServiceID 0x05
 * @Sync Synchronous
 * @Reentrancy Reentrant
 */
Pwm_OutputStateType Pwm_GetOutputState(Pwm_ChannelType ChannelNumber)
{
    Pwm_OutputStateType OutputState = PWM_LOW;
    
#if (PWM_DEV_ERROR_DETECT == STD_ON)
    /* Check if driver is initialized */
    if (Pwm_ValidateInit(PWM_GET_OUTPUT_STATE_ID) != E_OK)
    {
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_GET_OUTPUT_STATE_ID, PWM_E_UNINIT);
        return PWM_LOW;
    }
    
    /* Validate channel ID */
    if (Pwm_ValidateChannel(ChannelNumber, PWM_GET_OUTPUT_STATE_ID) != E_OK)
    {
        return PWM_LOW;
    }
#endif
    
    /* Get output state */
    OutputState = PwmHw_GetOutputState(ChannelNumber);
    
    return OutputState;
}
#endif /* PWM_GET_OUTPUT_STATE_API */

/****************************************************************************************
*                              NOTIFICATION FUNCTIONS                                 *
****************************************************************************************/

#if (PWM_NOTIFICATION_SUPPORTED == STD_ON)
/**
 * @brief Service to disable the PWM signal edge notification
 * @details [SWS_Pwm_00101] Definition of API function Pwm_DisableNotification
 * @param[in] ChannelNumber Numeric identifier of the PWM channel
 * @return void
 * @ServiceID 0x06
 * @Sync Synchronous
 * @Reentrancy Reentrant for different channel numbers
 */
void Pwm_DisableNotification(Pwm_ChannelType ChannelNumber)
{
#if (PWM_DEV_ERROR_DETECT == STD_ON)
    /* Check if driver is initialized */
    if (Pwm_ValidateInit(PWM_DISABLE_NOTIFICATION_ID) != E_OK)
    {
        return;
    }
    
    /* Validate channel ID */
    if (Pwm_ValidateChannel(ChannelNumber, PWM_DISABLE_NOTIFICATION_ID) != E_OK)
    {
        return;
    }
#endif
    if (Pwm_ChannelConfig[ChannelNumber].NotificationEnabled == TRUE)
    {
        /* Disable notification */
        (void)PwmHw_DisableNotification(ChannelNumber);
    }
}

/**
 * @brief Service to enable the PWM signal edge notification according to notification parameter
 * @details [SWS_Pwm_00102] Definition of API function Pwm_EnableNotification
 * @param[in] ChannelNumber Numeric identifier of the PWM channel
 * @param[in] Notification Type of notification
 * @return void
 * @ServiceID 0x07
 * @Sync Synchronous
 * @Reentrancy Reentrant for different channel numbers
 */
void Pwm_EnableNotification(Pwm_ChannelType ChannelNumber, Pwm_EdgeNotificationType Notification)
{
#if (PWM_DEV_ERROR_DETECT == STD_ON)
    /* Check if driver is initialized [SWS_Pwm_00117]*/
    if (Pwm_ValidateInit(PWM_ENABLE_NOTIFICATION_ID) != E_OK)
    {
        return;
    }
    
    /* Validate channel ID [SWS_Pwm_00047]*/
    if (Pwm_ValidateChannel(ChannelNumber, PWM_ENABLE_NOTIFICATION_ID) != E_OK)
    {
        return;
    }
    
    /* Validate notification parameter */
    // TODO need to change the error codes PWM_E_PARAM_VALUE dues to removing
    if ((Notification != PWM_RISING_EDGE) && (Notification != PWM_FALLING_EDGE) && (Notification != PWM_BOTH_EDGES))
    {
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_ENABLE_NOTIFICATION_ID, PWM_E_PARAM_VALUE);
        return;
    }
#endif
    
    /* Enable notification */
    (void)PwmHw_EnableNotification(ChannelNumber, Notification);
}
#endif /* PWM_NOTIFICATION_SUPPORTED */

/****************************************************************************************
*                              VERSION INFORMATION FUNCTIONS                          *
****************************************************************************************/

#if (PWM_VERSION_INFO_API == STD_ON)
/**
 * @brief Service returns the version information of this module
 * @details [SWS_Pwm_00103] Definition of API function Pwm_GetVersionInfo
 * @param[out] VersionInfo Pointer to where to store the version information
 * @return void
 * @ServiceID 0x08
 * @Sync Synchronous
 * @Reentrancy Reentrant
 */
void Pwm_GetVersionInfo(Std_VersionInfoType* VersionInfo)
{
#if (PWM_DEV_ERROR_DETECT == STD_ON)
    /* Validate pointer */
    if (Pwm_ValidatePointer(VersionInfo, PWM_GET_VERSION_INFO_ID) != E_OK)
    {
        return;
    }
#endif
    
    /* Fill version information */
    VersionInfo->vendorID = PWM_VENDOR_ID;
    VersionInfo->moduleID = PWM_MODULE_ID;
    VersionInfo->sw_major_version = PWM_SW_MAJOR_VERSION;
    VersionInfo->sw_minor_version = PWM_SW_MINOR_VERSION;
    VersionInfo->sw_patch_version = PWM_SW_PATCH_VERSION;
}
#endif /* PWM_VERSION_INFO_API */

/****************************************************************************************
*                              INTERRUPT SERVICE ROUTINES                             *
****************************************************************************************/

/**
 * @brief PWM notification handler
 * @details Called from interrupt service routine when PWM notification occurs
 * @param[in] ChannelNumber Channel identifier that generated the notification
 * @return void
 */
void Pwm_NotificationHandler(Pwm_HwUnitType HwUnit, uint16 TIM_IT)
{
    if(TIM_IT == TIM_IT_Update)
    {
        for(uint8 i = 0; i < PWM_CHANNELS_PER_HW_UNIT; i++)
        {
            Pwm_ChannelType ChannelId = HwUnit * PWM_CHANNELS_PER_HW_UNIT + i;
            /* Check if notification is enabled for the channel */
            if((Pwm_ChannelConfig[ChannelId].NotificationEnabled == TRUE) &&
               (Pwm_ChannelConfig[ChannelId].NotificationEdge == PWM_BOTH_EDGES ||\
                Pwm_ChannelConfig[ChannelId].NotificationEdge == PWM_RISING_EDGE))
            {
                Pwm_ChannelConfig[ChannelId].NotificationPtr();
            }

        }
    }
    else if(TIM_IT == TIM_IT_CC1)
    {
        Pwm_ChannelType ChannelId = HwUnit * PWM_CHANNELS_PER_HW_UNIT + PWM_CHANNEL_0;
        if((Pwm_ChannelConfig[ChannelId].NotificationEnabled == TRUE) &&
            (Pwm_ChannelConfig[ChannelId].NotificationEdge == PWM_BOTH_EDGES ||\
            Pwm_ChannelConfig[ChannelId].NotificationEdge == PWM_FALLING_EDGE))
        {
            Pwm_ChannelConfig[ChannelId].NotificationPtr();
        }
    }
    else if(TIM_IT == TIM_IT_CC2)
    {
        Pwm_ChannelType ChannelId = HwUnit * PWM_CHANNELS_PER_HW_UNIT + PWM_CHANNEL_1;
        if((Pwm_ChannelConfig[ChannelId].NotificationEnabled == TRUE) &&
            (Pwm_ChannelConfig[ChannelId].NotificationEdge == PWM_BOTH_EDGES ||\
            Pwm_ChannelConfig[ChannelId].NotificationEdge == PWM_FALLING_EDGE))
        {
            Pwm_ChannelConfig[ChannelId].NotificationPtr();
        }
    }
    else if(TIM_IT == TIM_IT_CC3)
    {
        Pwm_ChannelType ChannelId = HwUnit * PWM_CHANNELS_PER_HW_UNIT + PWM_CHANNEL_2;
        if((Pwm_ChannelConfig[ChannelId].NotificationEnabled == TRUE) &&
            (Pwm_ChannelConfig[ChannelId].NotificationEdge == PWM_BOTH_EDGES ||\
            Pwm_ChannelConfig[ChannelId].NotificationEdge == PWM_FALLING_EDGE))
        {
            Pwm_ChannelConfig[ChannelId].NotificationPtr();
        }
    }
    else if(TIM_IT == TIM_IT_CC4)
    {
        Pwm_ChannelType ChannelId = HwUnit * PWM_CHANNELS_PER_HW_UNIT + PWM_CHANNEL_3;
        if((Pwm_ChannelConfig[ChannelId].NotificationEnabled == TRUE) &&
            (Pwm_ChannelConfig[ChannelId].NotificationEdge == PWM_BOTH_EDGES ||\
            Pwm_ChannelConfig[ChannelId].NotificationEdge == PWM_FALLING_EDGE))
        {
            Pwm_ChannelConfig[ChannelId].NotificationPtr();
        }
    }
}

/****************************************************************************************
*                              VALIDATION FUNCTIONS                                   *
****************************************************************************************/

#if (PWM_DEV_ERROR_DETECT == STD_ON)
/**
 * @brief Validates PWM config pointer
 * @param[in] ConfigPtr Configuration pointer to validate
 * @param[in] ServiceId Service ID for error reporting
 * @return E_OK if valid, E_NOT_OK otherwise
 */
static inline Std_ReturnType Pwm_ValidateCfgPtr(const Pwm_ConfigType* ConfigPtr, uint8 ServiceId)
{
    Std_ReturnType RetVal = E_OK;
    
    /* Check for NULL pointer */
    /* Check if configuration is valid */
    if ((ConfigPtr == NULL_PTR) || (Pwm_ValidateConfig(ConfigPtr) != E_OK)) 
    {
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, ServiceId, PWM_E_INT_FAILED);
        RetVal = E_NOT_OK;
    }    
    return RetVal;
}

/**
 * @brief Validates PWM initialization
 * @param[in] ServiceId Service ID for error reporting
 * @return E_OK if valid, E_NOT_OK otherwise
 * [SWS_Pwm_00117]
 */
static inline Std_ReturnType Pwm_ValidateInit(uint8 ServiceId)
{
    if (Pwm_DriverState == PWM_STATE_UNINIT)
    {
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_DEINIT_ID, PWM_E_UNINIT);
        return E_NOT_OK;
    }
    return E_OK;
}
/**
 * @brief Validates PWM channel ID
 * @param[in] ChannelNumber Channel ID to validate
 * @param[in] ServiceId Service ID for error reporting
 * @return E_OK if valid, E_NOT_OK otherwise
 * [SWS_Pwm_00047]
 */
static inline Std_ReturnType Pwm_ValidateChannel(Pwm_ChannelType ChannelNumber, uint8 ServiceId)
{
    Std_ReturnType RetVal = E_OK;
    
    /* Check channel ID range */
    if (ChannelNumber >= PWM_MAX_CHANNELS)
    {
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, ServiceId, PWM_E_PARAM_CHANNEL);
        RetVal = E_NOT_OK;
    }
    
    return RetVal;
}

/**
 * @brief Validates PWM channel class for period change
 * @param[in] ChannelNumber Channel ID to validate
 * @param[in] ServiceId Service ID for error reporting
 * @return E_OK if valid, E_NOT_OK otherwise
 */
static inline Std_ReturnType Pwm_ValidateChannelClass(Pwm_ChannelType ChannelNumber, uint8 ServiceId)
{
    Std_ReturnType RetVal = E_OK;
    const Pwm_ChannelConfigType* ChannelConfig;
    
    /* Get channel configuration */
    ChannelConfig = Pwm_GetChannelConfig(ChannelNumber);
    
    /* Check if channel supports variable period */
    if ((ChannelConfig != NULL_PTR) && (ChannelConfig->ChannelClass != PWM_VARIABLE_PERIOD))
    {
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, ServiceId, PWM_E_PERIOD_UNCHANGEABLE);
        RetVal = E_NOT_OK;
    }
    
    return RetVal;
}

/**
 * @brief Validates pointer parameter
 * @param[in] Pointer Pointer to validate
 * @param[in] ServiceId Service ID for error reporting
 * @return E_OK if valid, E_NOT_OK otherwise
 */
static inline Std_ReturnType Pwm_ValidatePointer(const void* Pointer, uint8 ServiceId)
{
    Std_ReturnType RetVal = E_OK;
    
    /* Check for NULL pointer */
    if (Pointer == NULL_PTR)
    {
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, ServiceId, PWM_E_PARAM_POINTER);
        RetVal = E_NOT_OK;
    }
    
    return RetVal;
}

/**
 * @brief Validates duty cycle value
 * @param[in] DutyCycle Duty cycle to validate
 * @param[in] ServiceId Service ID for error reporting
 * @return E_OK if valid, E_NOT_OK otherwise
 */
static inline Std_ReturnType Pwm_ValidateDutyCycle(uint16 DutyCycle, uint8 ServiceId)
{
    Std_ReturnType RetVal = E_OK;
    
    /* Check duty cycle range */
    if (DutyCycle > 0x8000)
    {
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, ServiceId, PWM_E_DUTY_CYCLE_OUT_OF_RANGE);
        RetVal = E_NOT_OK;
    }
    
    return RetVal;
}

/**
 * @brief Validates period value
 * @param[in] Period Period to validate
 * @param[in] ServiceId Service ID for error reporting
 * @return E_OK if valid, E_NOT_OK otherwise
 */
static inline Std_ReturnType Pwm_ValidatePeriod(Pwm_PeriodType Period, uint8 ServiceId)
{
    Std_ReturnType RetVal = E_OK;
    
    /* Check period range */
    if ((Period < PWM_MIN_PERIOD) || (Period > PWM_MAX_PERIOD))
    {
        (void)Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, ServiceId, PWM_E_PERIOD_OUT_OF_RANGE);
        RetVal = E_NOT_OK;
    }
    
    return RetVal;
}
#endif /* PWM_DEV_ERROR_DETECT */
