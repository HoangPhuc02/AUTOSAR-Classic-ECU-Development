/****************************************************************************************
*                                PWM_CFG.C                                             *
****************************************************************************************
* File Name   : Pwm_Cfg.c
* Module      : Pulse Width Modulation (PWM)
* Description : AUTOSAR PWM driver configuration source file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Pwm_Cfg.h"
#include "Pwm_Types.h"
#include "Pwm_Hw.h"

/****************************************************************************************
*                              CONFIGURATION STRUCTURES                               *
****************************************************************************************/

/**
 * @brief PWM Channel Configuration Structure
 * @details Contains configuration for individual PWM channels
 */
Pwm_ChannelConfigType Pwm_ChannelConfig[PWM_MAX_CHANNELS] = 
{
    /* Channel 0 - TIM1 Channel 1 */
    {
        .ChannelId = PWM_CHANNEL_0,
        .HwUnit = PWM_HW_UNIT_TIM1,
        .ChannelClass = PWM_VARIABLE_PERIOD,
        .Period = PWM_DEFAULT_PERIOD,
        .DutyCycle = PWM_DEFAULT_DUTY_CYCLE,
        .Polarity = PWM_HIGH,
        .IdleState = PWM_LOW,
        .NotificationPtr = NULL_PTR,
        .NotificationEdge = PWM_RISING_EDGE,
    },
    
    /* Channel 1 - TIM1 Channel 2 */
    {
        .ChannelId = PWM_CHANNEL_1,
        .HwUnit = PWM_HW_UNIT_TIM1,
        .ChannelClass = PWM_VARIABLE_PERIOD,
        .Period = PWM_DEFAULT_PERIOD,
        .DutyCycle = PWM_DEFAULT_DUTY_CYCLE,
        .Polarity = PWM_HIGH,
        .IdleState = PWM_LOW,
        .NotificationPtr = NULL_PTR,
        .NotificationEdge = PWM_RISING_EDGE
    },
    
    /* Channel 2 - TIM1 Channel 3 */
    {
        .ChannelId = PWM_CHANNEL_2,
        .HwUnit = PWM_HW_UNIT_TIM1,
        .ChannelClass = PWM_VARIABLE_PERIOD,
        .Period = PWM_DEFAULT_PERIOD,
        .DutyCycle = PWM_DEFAULT_DUTY_CYCLE,
        .Polarity = PWM_HIGH,
        .IdleState = PWM_LOW,
        .NotificationPtr = NULL_PTR,
        .NotificationEdge = PWM_RISING_EDGE,
    },
    
    /* Channel 3 - TIM1 Channel 4 */
    {
        .ChannelId = PWM_CHANNEL_3,
        .HwUnit = PWM_HW_UNIT_TIM1,
        .ChannelClass = PWM_VARIABLE_PERIOD,
        .Period = PWM_DEFAULT_PERIOD,
        .DutyCycle = PWM_DEFAULT_DUTY_CYCLE,
        .Polarity = PWM_HIGH,
        .IdleState = PWM_LOW,
        .NotificationPtr = NULL_PTR,
        .NotificationEdge = PWM_RISING_EDGE,
    },
    
    /* Channel 4 - TIM2 Channel 1 */
    {
        .ChannelId = PWM_CHANNEL_4,
        .HwUnit = PWM_HW_UNIT_TIM2,
        .ChannelClass = PWM_FIXED_PERIOD,
        .Period = PWM_DEFAULT_PERIOD,
        .DutyCycle = PWM_DEFAULT_DUTY_CYCLE,
        .Polarity = PWM_HIGH,
        .IdleState = PWM_LOW,
        .NotificationPtr = Pwm_Tim2_Channel1_Notification,
        .NotificationEdge = PWM_FALLING_EDGE,
    },
    
    /* Channel 5 - TIM2 Channel 2 */
    {
        .ChannelId = PWM_CHANNEL_5,
        .HwUnit = PWM_HW_UNIT_TIM2,
        .ChannelClass = PWM_FIXED_PERIOD,
        .Period = PWM_DEFAULT_PERIOD,
        .DutyCycle = PWM_DEFAULT_DUTY_CYCLE,
        .Polarity = PWM_HIGH,
        .IdleState = PWM_LOW,
        .NotificationPtr = Pwm_Tim2_Channel2_Notification,
        .NotificationEdge = PWM_FALLING_EDGE,
    },
    
    /* Channel 6 - TIM2 Channel 3 */
    {
        .ChannelId = PWM_CHANNEL_6,
        .HwUnit = PWM_HW_UNIT_TIM2,
        .ChannelClass = PWM_FIXED_PERIOD,
        .Period = PWM_DEFAULT_PERIOD,
        .DutyCycle = PWM_DEFAULT_DUTY_CYCLE,
        .Polarity = PWM_HIGH,
        .IdleState = PWM_LOW,
        .NotificationPtr = Pwm_Tim2_Channel3_Notification,
        .NotificationEdge = PWM_FALLING_EDGE,
    },
    
    /* Channel 7 - TIM2 Channel 4 */
    {
        .ChannelId = PWM_CHANNEL_7,
        .HwUnit = PWM_HW_UNIT_TIM2,
        .ChannelClass = PWM_FIXED_PERIOD,
        .Period = PWM_DEFAULT_PERIOD,
        .DutyCycle = PWM_DEFAULT_DUTY_CYCLE,
        .Polarity = PWM_HIGH,
        .IdleState = PWM_HIGH,
        .NotificationPtr = NULL_PTR,
        .NotificationEdge = PWM_FALLING_EDGE,
    }
};

/**
 * @brief PWM Hardware Unit Configuration Structure (Only Active Units)
 * @details Contains configuration for PWM hardware units (timers)
 */
Pwm_HwUnitConfigType Pwm_HwUnitConfig[PWM_MAX_HW_UNITS] = 
{
    /* Timer 1 Configuration */
    {
        .HwUnit = PWM_HW_UNIT_TIM1,
        .Prescaler = PWM_TIM1_PRESCALER,
        // .CounterMode = TIM_CounterMode_Up,
        // .ClockDivision = TIM_CKD_DIV1,
        .RepetitionCounter = 0,
        .MaxPeriod = PWM_TIM1_MAX_PERIOD,
        .EnabledChannels = PWM_TIM1_CHANNELS,
        .ClockSource = PWM_CLOCK_SOURCE_INTERNAL,
        .SyncMode = PWM_SYNC_MODE_DISABLED,
        .MasterSlaveMode = PWM_MASTER_SLAVE_DISABLED
    },
    
    /* Timer 2 Configuration */
    {
        .HwUnit = PWM_HW_UNIT_TIM2,
        .Prescaler = PWM_TIM2_PRESCALER,
        // .CounterMode = TIM_CounterMode_Up,
        // .ClockDivision = TIM_CKD_DIV1,
        .RepetitionCounter = 0,
        .MaxPeriod = PWM_TIM2_MAX_PERIOD,
        .EnabledChannels = PWM_TIM2_CHANNELS,
        .ClockSource = PWM_CLOCK_SOURCE_INTERNAL,
        .SyncMode = PWM_SYNC_MODE_DISABLED,
        .MasterSlaveMode = PWM_MASTER_SLAVE_DISABLED
    },
    
    /* Timer 3 Configuration - DISABLED */
    {
        .HwUnit = PWM_HW_UNIT_TIM3,
        .Prescaler = 72, // PWM_TIM3_PRESCALER,
        // .CounterMode = TIM_CounterMode_Up,
        // .ClockDivision = TIM_CKD_DIV1,
        .RepetitionCounter = 0,
        .MaxPeriod = 65535, // PWM_TIM3_MAX_PERIOD,
        .EnabledChannels = 4, // PWM_TIM3_CHANNELS,
        .ClockSource = PWM_CLOCK_SOURCE_INTERNAL,
        .SyncMode = PWM_SYNC_MODE_DISABLED,
        .MasterSlaveMode = PWM_MASTER_SLAVE_DISABLED
    },
    
    /* Timer 4 Configuration - DISABLED */
    {
        .HwUnit = PWM_HW_UNIT_TIM4,
        .Prescaler = 72, // PWM_TIM4_PRESCALER,
        // .CounterMode = TIM_CounterMode_Up,
        // .ClockDivision = TIM_CKD_DIV1,
        .RepetitionCounter = 0,
        .MaxPeriod = 65535, // PWM_TIM4_MAX_PERIOD,
        .EnabledChannels = 4, // PWM_TIM4_CHANNELS,
        .ClockSource = PWM_CLOCK_SOURCE_INTERNAL,
        .SyncMode = PWM_SYNC_MODE_DISABLED,
        .MasterSlaveMode = PWM_MASTER_SLAVE_DISABLED
    }
};

/**
 * @brief PWM Driver Main Configuration Structure
 * @details Main configuration structure for PWM driver initialization
 */
const Pwm_ConfigType Pwm_Config = 
{
    .PwmChannelConfig = Pwm_ChannelConfig,
    .PwmHwUnitConfig = Pwm_HwUnitConfig,
    .PwmMaxChannels = PWM_MAX_CHANNELS,
    .PwmMaxHwUnits = PWM_MAX_HW_UNITS,
    .PwmSystemFrequency = PWM_SYSTEM_FREQUENCY,
    .PwmDefaultFrequency = PWM_DEFAULT_FREQUENCY,
    .PwmDevErrorDetect = PWM_DEV_ERROR_DETECT,
    .PwmVersionInfoApi = PWM_VERSION_INFO_API,
    .PwmNotificationSupported = PWM_NOTIFICATION_SUPPORTED,
    .PwmSetPeriodAndDutyApi = PWM_SET_PERIOD_AND_DUTY_API,
    .PwmSetOutputToIdleApi = PWM_SET_OUTPUT_TO_IDLE_API,
    .PwmGetOutputStateApi = PWM_GET_OUTPUT_STATE_API,
    .PwmEnablePhaseShift = PWM_ENABLE_PHASE_SHIFT,
    .PwmEnableVariablePeriod = PWM_ENABLE_VARIABLE_PERIOD,
    .PwmEnableSafetyChecks = PWM_ENABLE_SAFETY_CHECKS,
    .PwmTimeoutDuration = PWM_TIMEOUT_DURATION,
    .PwmDeadTimeEnabled = PWM_DEADTIME_ENABLED,
    .PwmDeadTimeValue = PWM_DEADTIME_VALUE
};

/****************************************************************************************
*                              CONFIGURATION ACCESS FUNCTIONS                         *
****************************************************************************************/

/**
 * @brief Validates PWM configuration structure
 * @details Checks configuration parameters for consistency and validity
 * @param[in] ConfigPtr Pointer to configuration structure
 * @return E_OK if configuration is valid, E_NOT_OK otherwise
 */
Std_ReturnType Pwm_ValidateConfig(const Pwm_ConfigType* ConfigPtr)
{
    Std_ReturnType RetVal = E_OK;
    uint8 ChannelIndex;
    uint8 HwUnitIndex;
    
    /* Check for NULL pointer */
    if (ConfigPtr == NULL_PTR)
    {
        RetVal = E_NOT_OK;
    }
    else
    {
        /* Validate channel configuration */
        for (ChannelIndex = 0; ChannelIndex < ConfigPtr->PwmMaxChannels; ChannelIndex++)
        {
            const Pwm_ChannelConfigType* ChannelConfig = &ConfigPtr->PwmChannelConfig[ChannelIndex];
            
            /* Check channel ID */
            if (ChannelConfig->ChannelId != ChannelIndex)
            {
                RetVal = E_NOT_OK;
                break;
            }
            
            /* Check hardware unit */
            if (ChannelConfig->HwUnit >= ConfigPtr->PwmMaxHwUnits)
            {
                RetVal = E_NOT_OK;
                break;
            }
            
            /* Check period range */
            if ((ChannelConfig->Period < PWM_MIN_PERIOD) || 
                (ChannelConfig->Period > PWM_MAX_PERIOD))
            {
                RetVal = E_NOT_OK;
                break;
            }
            
            /* Check duty cycle range */
            if (ChannelConfig->DutyCycle > 0x8000)
            {
                RetVal = E_NOT_OK;
                break;
            }
        }
        
        /* Validate hardware unit configuration */
        if (RetVal == E_OK)
        {
            for (HwUnitIndex = 0; HwUnitIndex < ConfigPtr->PwmMaxHwUnits; HwUnitIndex++)
            {
                const Pwm_HwUnitConfigType* HwUnitConfig = &ConfigPtr->PwmHwUnitConfig[HwUnitIndex];
                
                /* Check hardware unit ID */
                if (HwUnitConfig->HwUnit != HwUnitIndex)
                {
                    RetVal = E_NOT_OK;
                    break;
                }
                
                /* Check prescaler value */
                if (HwUnitConfig->Prescaler == 0)
                {
                    RetVal = E_NOT_OK;
                    break;
                }
                
                /* Check max period */
                if (HwUnitConfig->MaxPeriod == 0)
                {
                    RetVal = E_NOT_OK;
                    break;
                }
            }
        }
    }
    
    return RetVal;
}

/**
 * @brief Gets channel configuration by channel ID
 * @param[in] ChannelId Channel identifier
 * @return Pointer to channel configuration or NULL_PTR if not found
 */
const Pwm_ChannelConfigType* Pwm_GetChannelConfig(Pwm_ChannelType ChannelId)
{
    const Pwm_ChannelConfigType* ConfigPtr = NULL_PTR;
    
    if (ChannelId < PWM_MAX_CHANNELS)
    {
        ConfigPtr = &Pwm_ChannelConfig[ChannelId];
    }
    
    return ConfigPtr;
}

/**
 * @brief Gets hardware unit configuration by hardware unit ID
 * @param[in] HwUnit Hardware unit identifier
 * @return Pointer to hardware unit configuration or NULL_PTR if not found
 */
const Pwm_HwUnitConfigType* Pwm_GetHwUnitConfig(Pwm_HwUnitType HwUnit)
{
    const Pwm_HwUnitConfigType* ConfigPtr = NULL_PTR;
    
    if (HwUnit < PWM_MAX_HW_UNITS)
    {
        ConfigPtr = &Pwm_HwUnitConfig[HwUnit];
    }
    
    return ConfigPtr;
}

/****************************************************************************************
*                                   NOTE                                               *
****************************************************************************************
* Main PWM configuration structures have been moved to Pwm_Cfg.h:
* - Pwm_ChannelConfig[PWM_MAX_CHANNELS] - Channel configurations (8 active channels)
* - Pwm_HwUnitConfig[PWM_MAX_HW_UNITS] - Hardware unit configurations (4 units)
* - Pwm_Config - Main configuration structure
* 
* Unused configurations have been commented out:
* - Timer 3 and Timer 4 configurations
* - Channel IDs 8-15 definitions
* - Unused notification callbacks
****************************************************************************************/

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
