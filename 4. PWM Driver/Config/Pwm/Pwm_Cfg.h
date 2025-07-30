/****************************************************************************************
*                                PWM_CFG.H                                             *
****************************************************************************************
* File Name   : Pwm_Cfg.h
* Module      : Pulse Width Modulation (PWM)
* Description : AUTOSAR PWM driver configuration header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef PWM_CFG_H
#define PWM_CFG_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Std_Types.h"
#include "Pwm_Types.h"



/****************************************************************************************
*                              DEVELOPMENT CONFIGURATION                               *
****************************************************************************************/
#define PWM_DEV_ERROR_DETECT        STD_ON  /*!< Enable/disable development error detection */
#define PWM_VERSION_INFO_API        STD_ON  /*!< Enable/disable version info API */
#define PWM_NOTIFICATION_SUPPORTED  STD_ON  /*!< Enable/disable notification support */
#define PWM_POWER_STATE_SUPPORTED   STD_OFF /*!< Enable/disable power state support */

/****************************************************************************************
*                              FEATURE CONFIGURATION                                   *
****************************************************************************************/
#define PWM_SET_DUTY_CYCLE_API      STD_ON /*!< Enable/disable Pwm_SetDutyCycle API */
#define PWM_SET_PERIOD_AND_DUTY_API STD_ON  /*!< Enable/disable Pwm_SetPeriodAndDuty API */
#define PWM_SET_OUTPUT_TO_IDLE_API  STD_ON  /*!< Enable/disable Pwm_SetOutputToIdle API */
#define PWM_GET_OUTPUT_STATE_API    STD_ON  /*!< Enable/disable Pwm_GetOutputState API */
#define PWM_ENABLE_PHASE_SHIFT      STD_OFF /*!< Enable/disable phase shift support */
#define PWM_ENABLE_VARIABLE_PERIOD  STD_ON  /*!< Enable/disable variable period support */

/****************************************************************************************
*                              SYSTEM CONFIGURATION                                    *
****************************************************************************************/
#define PWM_MAX_CHANNELS            8       /*!< Maximum number of PWM channels */
#define PWM_MAX_HW_UNITS            4       /*!< Maximum number of hardware units */
#define PWM_SYSTEM_FREQUENCY        72000000UL /*!< System frequency in Hz */

/****************************************************************************************
*                              TIMING CONFIGURATION                                    *
****************************************************************************************/
#define PWM_DEFAULT_FREQUENCY       1000    /*!< Default PWM frequency in Hz */
#define PWM_DEFAULT_PERIOD          10000   /*!< Default PWM period in timer ticks (10KHz/10000 = 1Hz)*/
#define PWM_DEFAULT_DUTY_CYCLE      0x4000  /*!< Default duty cycle (50%) */
#define PWM_MIN_PERIOD              1       /*!< Minimum period value */
#define PWM_MAX_PERIOD              65535   /*!< Maximum period value */

/****************************************************************************************
*                              HARDWARE CONFIGURATION                                  *
****************************************************************************************/
/* Timer 1 Configuration */
#define PWM_TIM1_ENABLED            STD_ON
#define PWM_TIM1_PRESCALER          7200      /*!< Prescaler for 10KHz timer clock */
#define PWM_TIM1_MAX_PERIOD         PWM_DEFAULT_PERIOD   /*!< Maximum period for Timer 1 */
#define PWM_TIM1_CHANNELS           4       /*!< Number of channels in Timer 1 */

/* Timer 2 Configuration */
#define PWM_TIM2_ENABLED            STD_ON
#define PWM_TIM2_PRESCALER          7200      /*!< Prescaler for 10KHz timer clock */
#define PWM_TIM2_MAX_PERIOD         PWM_DEFAULT_PERIOD   /*!< Maximum period for Timer 2 */
#define PWM_TIM2_CHANNELS           4       /*!< Number of channels in Timer 2 */

/* Timer 3 Configuration - DISABLED */
#define PWM_TIM3_ENABLED            STD_OFF
#define PWM_TIM3_PRESCALER          7200      /*!< Prescaler for 10KHz timer clock */
#define PWM_TIM3_MAX_PERIOD         65535   /*!< Maximum period for Timer 3 */
#define PWM_TIM3_CHANNELS           4       /*!< Number of channels in Timer 3 */

/* Timer 4 Configuration - DISABLED */
#define PWM_TIM4_ENABLED            STD_OFF
#define PWM_TIM4_PRESCALER          7200      /*!< Prescaler for 10KHz timer clock */
#define PWM_TIM4_MAX_PERIOD         65535   /*!< Maximum period for Timer 4 */
#define PWM_TIM4_CHANNELS           4       /*!< Number of channels in Timer 4 */

/****************************************************************************************
*                              SAFETY CONFIGURATION                                    *
****************************************************************************************/
#define PWM_TIMEOUT_DURATION        1000    /*!< Timeout duration in milliseconds */
#define PWM_ENABLE_SAFETY_CHECKS    STD_ON  /*!< Enable safety checks */
#define PWM_DEADTIME_ENABLED        STD_OFF /*!< Enable deadtime feature */
#define PWM_DEADTIME_VALUE          100     /*!< Deadtime value in nanoseconds */

/****************************************************************************************
*                              ADDITIONAL CONSTANTS                                    *
****************************************************************************************/
/* PWM Driver Instance */
#define PWM_INSTANCE_ID             0       /*!< PWM Driver Instance ID */

/* PWM Channel IDs - Active Channels */
#define PWM_CHANNEL_0               0       /*!< PWM Channel 0 */
#define PWM_CHANNEL_1               1       /*!< PWM Channel 1 */
#define PWM_CHANNEL_2               2       /*!< PWM Channel 2 */
#define PWM_CHANNEL_3               3       /*!< PWM Channel 3 */
#define PWM_CHANNEL_4               4       /*!< PWM Channel 4 */
#define PWM_CHANNEL_5               5       /*!< PWM Channel 5 */
#define PWM_CHANNEL_6               6       /*!< PWM Channel 6 */
#define PWM_CHANNEL_7               7       /*!< PWM Channel 7 */

/* PWM Channel IDs - Unused Channels (Commented Out) */
// #define PWM_CHANNEL_8               8       /*!< PWM Channel 8 */
// #define PWM_CHANNEL_9               9       /*!< PWM Channel 9 */
// #define PWM_CHANNEL_10              10      /*!< PWM Channel 10 */
// #define PWM_CHANNEL_11              11      /*!< PWM Channel 11 */
// #define PWM_CHANNEL_12              12      /*!< PWM Channel 12 */
// #define PWM_CHANNEL_13              13      /*!< PWM Channel 13 */
// #define PWM_CHANNEL_14              14      /*!< PWM Channel 14 */
// #define PWM_CHANNEL_15              15      /*!< PWM Channel 15 */

/* PWM Hardware Unit Constants */
#define PWM_HW_UNIT_INVALID         0xFF    /*!< Invalid hardware unit */
#define PWM_CHANNELS_PER_HW_UNIT    4       /*!< Channels per hardware unit */

/* PWM Clock Source Configuration */
#define PWM_CLOCK_SOURCE_INTERNAL   0       /*!< Internal clock source */
#define PWM_CLOCK_SOURCE_EXTERNAL   1       /*!< External clock source */

/* PWM Synchronization Mode */
#define PWM_SYNC_MODE_DISABLED      0       /*!< Synchronization disabled */
#define PWM_SYNC_MODE_ENABLED       1       /*!< Synchronization enabled */

/* PWM Master/Slave Mode */
#define PWM_MASTER_SLAVE_DISABLED   0       /*!< Master/slave mode disabled */
#define PWM_MASTER_SLAVE_ENABLED    1       /*!< Master/slave mode enabled */

/* PWM Configuration Constants */
#define PWM_CONFIGURED_CHANNELS     8       /*!< Number of configured channels */
#define PWM_MAX_NOTIFICATIONS       8       /*!< Maximum number of notification functions */

/* PWM Duty Cycle Constants */
#define PWM_DUTY_CYCLE_100_PERCENT  0x8000  /*!< 100% duty cycle */

/****************************************************************************************
*                              EXTERNAL DECLARATIONS                                   *
****************************************************************************************/
extern const Pwm_ConfigType Pwm_Config;
extern Pwm_ChannelConfigType Pwm_ChannelConfig[PWM_MAX_CHANNELS];
extern Pwm_HwUnitConfigType Pwm_HwUnitConfig[PWM_MAX_HW_UNITS];

/****************************************************************************************
*                              CONFIGURATION VALIDATION FUNCTIONS                     *
****************************************************************************************/
Std_ReturnType Pwm_ValidateConfig(const Pwm_ConfigType* ConfigPtr);
const Pwm_ChannelConfigType* Pwm_GetChannelConfig(Pwm_ChannelType ChannelId);
const Pwm_HwUnitConfigType* Pwm_GetHwUnitConfig(Pwm_HwUnitType HwUnit);

/****************************************************************************************
*                              CALLBACK FUNCTION DECLARATIONS                         *
****************************************************************************************/
/* Channel notification callbacks - Active Timers */
extern void Pwm_Tim1_Channel1_Notification(void);
extern void Pwm_Tim1_Channel2_Notification(void);
extern void Pwm_Tim1_Channel3_Notification(void);
extern void Pwm_Tim1_Channel4_Notification(void);
 
extern void Pwm_Tim2_Channel1_Notification(void);
extern void Pwm_Tim2_Channel2_Notification(void);
extern void Pwm_Tim2_Channel3_Notification(void);
extern void Pwm_Tim2_Channel4_Notification(void);

/* Channel notification callbacks - Unused Timers (Commented Out) */
// void Pwm_Tim3_Channel1_Notification(void);
// void Pwm_Tim3_Channel2_Notification(void);
// void Pwm_Tim3_Channel3_Notification(void);
// void Pwm_Tim3_Channel4_Notification(void);

// void Pwm_Tim4_Channel1_Notification(void);
// void Pwm_Tim4_Channel2_Notification(void);
// void Pwm_Tim4_Channel3_Notification(void);
// void Pwm_Tim4_Channel4_Notification(void);

/* Error callback */
void Pwm_ErrorCallback(Pwm_ChannelType ChannelId, uint8 ErrorCode);

/****************************************************************************************
*                              VALIDATION MACROS                                       *
****************************************************************************************/
/* Configuration validation */
#if (PWM_CONFIGURED_CHANNELS > PWM_MAX_CHANNELS)
#error "PWM_CONFIGURED_CHANNELS exceeds PWM_MAX_CHANNELS"
#endif

#if (PWM_MAX_HW_UNITS > 4)
#error "PWM_MAX_HW_UNITS cannot exceed 4 (STM32F10x limitation)"
#endif

#if (PWM_DEFAULT_PERIOD > PWM_MAX_PERIOD)
#error "PWM_DEFAULT_PERIOD exceeds PWM_MAX_PERIOD"
#endif

#if (PWM_DEFAULT_DUTY_CYCLE > PWM_DUTY_CYCLE_100_PERCENT)
#error "PWM_DEFAULT_DUTY_CYCLE exceeds 100%"
#endif

/* Feature validation */
#if (PWM_NOTIFICATION_SUPPORTED == STD_ON) && (PWM_MAX_NOTIFICATIONS == 0)
#error "PWM_MAX_NOTIFICATIONS must be greater than 0 when notifications are supported"
#endif

#if (PWM_ENABLE_PHASE_SHIFT == STD_ON) && (PWM_CONFIGURED_CHANNELS < 2)
#error "Phase shift requires at least 2 channels"
#endif

/* Hardware validation */
#if (PWM_TIM1_ENABLED == STD_OFF) && (PWM_TIM2_ENABLED == STD_OFF) && \
    (PWM_TIM3_ENABLED == STD_OFF) && (PWM_TIM4_ENABLED == STD_OFF)
#error "At least one timer must be enabled"
#endif

#if (PWM_SYSTEM_FREQUENCY == 0)
#error "PWM_SYSTEM_FREQUENCY must be greater than 0"
#endif

#endif /* PWM_CFG_H */

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
