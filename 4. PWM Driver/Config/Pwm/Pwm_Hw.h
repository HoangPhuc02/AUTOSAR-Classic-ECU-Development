/****************************************************************************************
*                                PWM_HW.H                                              *
****************************************************************************************
* File Name   : Pwm_Hw.h
* Module      : Pulse Width Modulation (PWM)
* Description : AUTOSAR PWM driver hardware abstraction layer header file
* Version     : 1.0.0 - AUTOSAR compliant implementation with STM32F10x SPL
* Date        : 27/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef PWM_HW_H
#define PWM_HW_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Std_Types.h"
#include "Pwm_Types.h"
#include "Pwm_Cfg.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"


/****************************************************************************************
*                              HARDWARE MAPPING MACROS                                *
****************************************************************************************/
/* Timer instance mapping */
#define PWM_HW_GET_TIMER(HwUnit) \
    ((HwUnit) == PWM_HW_UNIT_TIM1 ? TIM1 : \
     (HwUnit) == PWM_HW_UNIT_TIM2 ? TIM2 : \
     (HwUnit) == PWM_HW_UNIT_TIM3 ? TIM3 : \
     (HwUnit) == PWM_HW_UNIT_TIM4 ? TIM4 : NULL)



/* Timer clock enable mapping */
#define PWM_HW_ENABLE_TIMER_CLOCK(HwUnit) \
    do { \
        if ((HwUnit) == PWM_HW_UNIT_TIM1)      RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); \
        else if ((HwUnit) == PWM_HW_UNIT_TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); \
        else if ((HwUnit) == PWM_HW_UNIT_TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); \
        else if ((HwUnit) == PWM_HW_UNIT_TIM4) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); \
    } while(0)

/* Timer clock disable mapping */
#define PWM_HW_DISABLE_TIMER_CLOCK(HwUnit) \
    do { \
        if ((HwUnit) == PWM_HW_UNIT_TIM1)      RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, DISABLE); \
        else if ((HwUnit) == PWM_HW_UNIT_TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE); \
        else if ((HwUnit) == PWM_HW_UNIT_TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); \
        else if ((HwUnit) == PWM_HW_UNIT_TIM4) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE); \
    } while(0)

/* Channel to TIM_Channel mapping */
// TODO should be update not all mc have same number of channel per timer
// Idea 1 not get constant modulo instead divide by 4 make it divide by MAX_CHANNEL_PER_HW_UNIT
#define PWM_HW_GET_TIM_CHANNEL(ChannelId) \
    (((ChannelId) % 4) == 0 ? TIM_Channel_1 : \
     ((ChannelId) % 4) == 1 ? TIM_Channel_2 : \
     ((ChannelId) % 4) == 2 ? TIM_Channel_3 : TIM_Channel_4)

/****************************************************************************************
*                              HARDWARE VALIDATION MACROS                             *
****************************************************************************************/
#define PWM_HW_IS_VALID_TIMER(HwUnit)       ((HwUnit) < PWM_MAX_HW_UNITS)
#define PWM_HW_IS_VALID_CHANNEL(Channel)    ((Channel) < PWM_CHANNELS_PER_HW_UNIT)
#define PWM_HW_IS_TIMER_ENABLED(HwUnit) \
    (((HwUnit) == PWM_HW_UNIT_TIM1 && PWM_TIM1_ENABLED == STD_ON) || \
     ((HwUnit) == PWM_HW_UNIT_TIM2 && PWM_TIM2_ENABLED == STD_ON) || \
     ((HwUnit) == PWM_HW_UNIT_TIM3 && PWM_TIM3_ENABLED == STD_ON) || \
     ((HwUnit) == PWM_HW_UNIT_TIM4 && PWM_TIM4_ENABLED == STD_ON))

/****************************************************************************************
*                              FUNCTION DECLARATIONS                                   *
****************************************************************************************/

/****************************************************************************************
*                              INITIALIZATION FUNCTIONS                               *
****************************************************************************************/
/**
 * @brief Initialize PWM hardware unit
 * @param[in] HwUnitId Hardware unit identifier
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_InitHwUnit(Pwm_HwUnitType HwUnit, const Pwm_HwUnitConfigType* ConfigPtr);

/**
 * @brief Deinitialize PWM hardware unit
 * @param[in] HwUnitId Hardware unit identifier
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_DeInitHwUnit(Pwm_HwUnitType HwUnitId);

/**
 * @brief Initialize PWM channel
 * @param[in] ChannelId Channel identifier
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_InitChannel(Pwm_ChannelType ChannelId);

/**
 * @brief Deinitialize PWM channel
 * @param[in] ChannelId Channel identifier
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_DeInitChannel(Pwm_ChannelType ChannelId);

/****************************************************************************************
*                              CHANNEL CONTROL FUNCTIONS                              *
****************************************************************************************/
/**
 * @brief Set PWM duty cycle
 * @param[in] ChannelId Channel identifier
 * @param[in] DutyCycle Duty cycle value (0x0000-0x8000)
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_SetDutyCycle(Pwm_ChannelType ChannelId, 
                                  Pwm_DutyCycleType DutyCycle);

/**
 * @brief Set PWM period and duty cycle
 * @param[in] ChannelId Channel identifier
 * @param[in] Period Period value in timer ticks
 * @param[in] DutyCycle Duty cycle value (0x0000-0x8000)
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_SetPeriodAndDuty(Pwm_ChannelType ChannelId, 
                                      Pwm_PeriodType Period,
                                      Pwm_DutyCycleType DutyCycle);

/**
 * @brief Set PWM output to idle state
 * @param[in] ChannelId Channel identifier
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_SetOutputToIdle(Pwm_ChannelType ChannelId);

/**
 * @brief Start PWM channel output
 * @param[in] ChannelId Channel identifier
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_StartChannel(Pwm_ChannelType ChannelId);

/**
 * @brief Stop PWM channel output
 * @param[in] ChannelId Channel identifier
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_StopChannel(Pwm_ChannelType ChannelId);

/****************************************************************************************
*                              STATUS FUNCTIONS                                        *
****************************************************************************************/
/**
 * @brief Get PWM output state
 * @param[in] ChannelId Channel identifier
 * @return PWM_HIGH: High state, PWM_LOW: Low state
 */
Pwm_OutputStateType PwmHw_GetOutputState(Pwm_ChannelType ChannelId);

/**
 * @brief Check if PWM channel is running
 * @param[in] ChannelId Channel identifier
 * @return 1: Running, 0: Stopped
 */
uint8 PwmHw_IsChannelRunning(Pwm_ChannelType ChannelId);

/**
 * @brief Get current period value
 * @param[in] ChannelId Channel identifier
 * @return Current period value
 */
Pwm_PeriodType PwmHw_GetCurrentPeriod(Pwm_ChannelType ChannelId);

/**
 * @brief Get current duty cycle value
 * @param[in] ChannelId Channel identifier
 * @return Current duty cycle value
 */
Pwm_DutyCycleType PwmHw_GetCurrentDutyCycle(Pwm_ChannelType ChannelId);

/****************************************************************************************
*                              NOTIFICATION FUNCTIONS                                  *
****************************************************************************************/
/**
 * @brief Enable PWM edge notification
 * @param[in] ChannelId Channel identifier
 * @param[in] EdgeType Edge type for notification
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_EnableNotification(Pwm_ChannelType ChannelId, 
                                        Pwm_EdgeNotificationType EdgeType);

/**
 * @brief Disable PWM edge notification
 * @param[in] ChannelId Channel identifier
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_DisableNotification(Pwm_ChannelType ChannelId);

/****************************************************************************************
*                              UTILITY FUNCTIONS                                       *
****************************************************************************************/
/**
 * @brief Calculate timer prescaler for desired frequency
 * @param[in] DesiredFreq Desired frequency in Hz
 * @param[in] SystemFreq System frequency in Hz
 * @param[out] Prescaler Calculated prescaler value
 * @param[out] Period Calculated period value
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_CalculateTimerValues(uint32 DesiredFreq,
                                          uint32 SystemFreq,
                                          uint16* Prescaler,
                                          uint16* Period);

/**
 * @brief Convert duty cycle percentage to timer compare value
 * @param[in] DutyCycle Duty cycle (0x0000-0x8000)
 * @param[in] Period Timer period value
 * @return Timer compare value
 */
uint16 PwmHw_DutyCycleToCompareValue(Pwm_DutyCycleType DutyCycle, 
                                     Pwm_PeriodType Period);

/**
 * @brief Convert timer compare value to duty cycle percentage
 * @param[in] CompareValue Timer compare value
 * @param[in] Period Timer period value
 * @return Duty cycle (0x0000-0x8000)
 */
Pwm_DutyCycleType PwmHw_CompareValueToDutyCycle(uint16 CompareValue, 
                                                Pwm_PeriodType Period);

/**
 * @brief Get hardware unit from channel ID
 * @param[in] ChannelId Channel identifier
 * @return Hardware unit identifier
 */
Pwm_HwUnitType PwmHw_GetHwUnitFromChannel(Pwm_ChannelType ChannelId);

/**
 * @brief Get hardware channel from channel ID
 * @param[in] ChannelId Channel identifier
 * @return Hardware channel number (0-3)
 */
uint8 PwmHw_GetHwChannelFromChannel(Pwm_ChannelType ChannelId);

/****************************************************************************************
*                              TIMER CONFIGURATION FUNCTIONS                          *
****************************************************************************************/
// Todo: Remove function
/**
 * @brief Configure timer base
 * @param[in] HwUnitId Hardware unit identifier
 * @param[in] HwUnitConfig Hardware unit configuration
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_ConfigureTimerBase(Pwm_HwUnitType HwUnitId,
                                        const Pwm_HwUnitConfigType* HwUnitConfig);

// Todo: Remove function
/**
 * @brief Configure timer output compare mode
 * @param[in] HwUnitId Hardware unit identifier
 * @param[in] HwChannel Hardware channel number
 * @param[in] ChannelConfig Channel configuration
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_ConfigureOutputCompare(Pwm_HwUnitType HwUnitId,
                                            uint8 HwChannel,
                                            const Pwm_ChannelConfigType* ChannelConfig);

/**
 * @brief Enable timer counter
 * @param[in] HwUnitId Hardware unit identifier
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_EnableTimer(Pwm_HwUnitType HwUnitId);

/**
 * @brief Disable timer counter
 * @param[in] HwUnitId Hardware unit identifier
 * @return E_OK: Success, E_NOT_OK: Failed
 */
Std_ReturnType PwmHw_DisableTimer(Pwm_HwUnitType HwUnitId);

/****************************************************************************************
*                              INTERRUPT FUNCTIONS                                     *
****************************************************************************************/
/**
 * @brief Timer 1 interrupt handler
 */
void PwmHw_Tim1_IRQHandler(void);

/**
 * @brief Timer 2 interrupt handler
 */
void PwmHw_Tim2_IRQHandler(void);

/**
 * @brief Timer 3 interrupt handler
 */
void PwmHw_Tim3_IRQHandler(void);

/**
 * @brief Timer 4 interrupt handler
 */
void PwmHw_Tim4_IRQHandler(void);

/**
 * @brief Process timer interrupt
 * @param[in] HwUnitId Hardware unit identifier
 */
void PwmHw_ProcessTimerInterrupt(Pwm_HwUnitType HwUnitId);

/****************************************************************************************
*                              VALIDATION FUNCTIONS                                    *
****************************************************************************************/
/**
 * @brief Validate hardware unit
 * @param[in] HwUnitId Hardware unit identifier
 * @return 1: Valid, 0: Invalid
 */
uint8 PwmHw_ValidateHwUnit(Pwm_HwUnitType HwUnitId);

/**
 * @brief Validate channel
 * @param[in] ChannelId Channel identifier
 * @return 1: Valid, 0: Invalid
 */
uint8 PwmHw_ValidateChannel(Pwm_ChannelType ChannelId);

/**
 * @brief Validate duty cycle
 * @param[in] DutyCycle Duty cycle value
 * @return 1: Valid, 0: Invalid
 */
uint8 PwmHw_ValidateDutyCycle(Pwm_DutyCycleType DutyCycle);

/**
 * @brief Validate period
 * @param[in] Period Period value
 * @return 1: Valid, 0: Invalid
 */
uint8 PwmHw_ValidatePeriod(Pwm_PeriodType Period);

#endif /* PWM_HW_H */

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
