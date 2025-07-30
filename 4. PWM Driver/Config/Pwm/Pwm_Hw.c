/****************************************************************************************
*                                 PWM_HW.C                                             *
****************************************************************************************
* File Name   : Pwm_Hw.c
* Module      : Pulse Width Modulation (PWM)
* Description : AUTOSAR PWM driver hardware abstraction layer implementation
* Version     : 1.0.0 - AUTOSAR compliant implementation with STM32F10x SPL
* Date        : 27/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Pwm_Hw.h"
#include "Pwm_Cfg.h"
#include "Det.h"

/****************************************************************************************
*                              LOCAL VARIABLES                                         *
****************************************************************************************/
static uint8 Pwm_UpdateInterruptUsers[PWM_MAX_HW_UNITS] = {0};

/****************************************************************************************
*                              INITIALIZATION FUNCTIONS                               *
****************************************************************************************/

/**
 * @brief Initializes PWM hardware unit
 * @details Configures timer hardware for PWM operation
 * @param[in] HwUnit Hardware unit identifier
 * @param[in] ConfigPtr Pointer to hardware unit configuration
 * @return E_OK if initialization successful, E_NOT_OK otherwise
 */
Std_ReturnType PwmHw_InitHwUnit(Pwm_HwUnitType HwUnit, const Pwm_HwUnitConfigType* ConfigPtr)
{
    Std_ReturnType RetVal = E_OK;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    /* Validate parameters */
    if ((HwUnit >= PWM_MAX_HW_UNITS) || (ConfigPtr == NULL_PTR))
    {
        RetVal = E_NOT_OK;
    }
    else
    {
        /* Enable timer clock */
        PWM_HW_ENABLE_TIMER_CLOCK(HwUnit);
        TIM_TypeDef* TIM_Instance = PWM_HW_GET_TIMER(HwUnit);
        /* Initialize timer base configuration */
        TIM_TimeBaseStructure.TIM_Period = ConfigPtr->MaxPeriod - 1;
        TIM_TimeBaseStructure.TIM_Prescaler = ConfigPtr->Prescaler - 1;
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // TODO default need to fix
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseStructure.TIM_RepetitionCounter = ConfigPtr->RepetitionCounter;
        
        /* Configure timer */
        
        TIM_TimeBaseInit(TIM_Instance, &TIM_TimeBaseStructure);
        
        /* Enable ARR preload */
        TIM_ARRPreloadConfig(TIM_Instance, ENABLE);
        
        /* Enable timer */
        TIM_Cmd(TIM_Instance, ENABLE);

    }
    
    return RetVal;
}

/**
 * @brief Deinitializes PWM hardware unit
 * @details Resets timer hardware and disables clocks
 * @param[in] HwUnit Hardware unit identifier
 * @return E_OK if deinitialization successful, E_NOT_OK otherwise
 */
Std_ReturnType PwmHw_DeInitHwUnit(Pwm_HwUnitType HwUnit)
{
    Std_ReturnType RetVal = E_OK;
    
    /* Validate parameters */
    if (HwUnit >= PWM_MAX_HW_UNITS)
    {
        RetVal = E_NOT_OK;
    }
    else
    {
        TIM_TypeDef* TIM_Instance = PWM_HW_GET_TIMER(HwUnit);
        /* Disable timer */
        if (TIM_Instance != NULL_PTR)
        {
            TIM_Cmd(TIM_Instance, DISABLE);
            TIM_DeInit(TIM_Instance);
        }
        
        /* Disable timer clock */
        PWM_HW_DISABLE_TIMER_CLOCK(HwUnit);
    }
    
    return RetVal;
}

/**
 * @brief Initializes PWM channel hardware
 * @details Configures GPIO pins and timer channels for PWM output
 * @param[in] ChannelId Channel identifier
 * @param[in] ChannelConfig Pointer to channel configuration
 * @return E_OK if initialization successful, E_NOT_OK otherwise
 */
Std_ReturnType PwmHw_InitChannel(Pwm_ChannelType ChannelId)
{
    Std_ReturnType RetVal = E_OK;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    Pwm_HwUnitType HwUnitId;
    Pwm_ChannelConfigType* ChannelConfigPtr = &Pwm_ChannelConfig[ChannelId];
    /* Validate parameters */
    if (ChannelId >= PWM_MAX_CHANNELS)
    {
        RetVal = E_NOT_OK;
    }
    else
    {

        /* Get timer channel */
        HwUnitId = ChannelConfigPtr->HwUnit;

        uint16 TIM_Channel = PWM_HW_GET_TIM_CHANNEL(ChannelId);
        TIM_TypeDef* TIM_Instance = PWM_HW_GET_TIMER(HwUnitId);
        uint16 CompareValue = (uint16)(((uint32)(ChannelConfigPtr->DutyCycle) * (uint32)(ChannelConfigPtr->Period)) >> 15);
        /* Configure timer output compare */
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   // default
        TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
        TIM_OCInitStructure.TIM_Pulse = CompareValue;
        TIM_OCInitStructure.TIM_OCPolarity = (ChannelConfigPtr->Polarity == PWM_HIGH) ? TIM_OCPolarity_High : TIM_OCPolarity_Low;
        TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
        TIM_OCInitStructure.TIM_OCIdleState = (ChannelConfigPtr->IdleState == PWM_HIGH) ? TIM_OCIdleState_Set : TIM_OCIdleState_Reset;
        TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
        
        /* Initialize output compare based on channel */
        switch (TIM_Channel)
        {
            case TIM_Channel_1:
                TIM_OC1Init(TIM_Instance, &TIM_OCInitStructure);
                TIM_OC1PreloadConfig(TIM_Instance, TIM_OCPreload_Enable);
                break;
            case TIM_Channel_2:
                TIM_OC2Init(TIM_Instance, &TIM_OCInitStructure);
                TIM_OC2PreloadConfig(TIM_Instance, TIM_OCPreload_Enable);
                break;
            case TIM_Channel_3:
                TIM_OC3Init(TIM_Instance, &TIM_OCInitStructure);
                TIM_OC3PreloadConfig(TIM_Instance, TIM_OCPreload_Enable);
                break;
            case TIM_Channel_4:
                TIM_OC4Init(TIM_Instance, &TIM_OCInitStructure);
                TIM_OC4PreloadConfig(TIM_Instance, TIM_OCPreload_Enable);
                break;
            default:
                RetVal = E_NOT_OK;
                break;
        }
        
        if (RetVal == E_OK)
        {
            /* Initialize channel runtime data */

            ChannelConfigPtr->IdleStateSet = FALSE;

            /* Enable main output for advanced timers */
            if (ChannelConfigPtr->HwUnit == PWM_HW_UNIT_TIM1)
            {
                TIM_CtrlPWMOutputs(TIM1, ENABLE);
            }
        }
    }
    
    return RetVal;
}

/****************************************************************************************
*                              DUTY CYCLE CONTROL FUNCTIONS                          *
****************************************************************************************/
#if (PWM_SET_DUTY_CYCLE_API == STD_ON)
/**
 * @brief Sets PWM duty cycle for a channel
 * @details Updates the compare value for the specified channel
 * @param[in] ChannelId Channel identifier
 * @param[in] DutyCycle New duty cycle value (0x0000 to 0x8000)
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note validate in api  
 */
Std_ReturnType PwmHw_SetDutyCycle(Pwm_ChannelType ChannelId, uint16 DutyCycle)
{
    Std_ReturnType RetVal = E_OK;
    uint16 CompareValue;
    uint16 TIM_Channel = PWM_HW_GET_TIM_CHANNEL(ChannelId);
    TIM_TypeDef* TIM_Instance = PWM_HW_GET_TIMER(Pwm_ChannelConfig[ChannelId].HwUnit);
    /* Validate parameters */
    if ((ChannelId >= PWM_MAX_CHANNELS) || (DutyCycle > 0x8000))
    {
        RetVal = E_NOT_OK;
    }
    else if(Pwm_ChannelConfig[ChannelId].IdleStateSet == TRUE)
    {
        PwmHw_InitChannel(ChannelId);

        // Pwm_ChannelConfig[ChannelId].IdleStateSet = FALSE;
        
        // Now continue with normal duty cycle setting...
    }
    else
    {
        /* Calculate compare value */
        //  AbsoluteDutyCycle = ((uint32)AbsolutePeriodTime * RelativeDutyCycle) >> 15;
        // shift bits to left means divide it by 2^15
        // when u use duty cycle as max 0x8000 when u shift it will like 2^15 / 2^15 = 1 or 100%
        // when u want 50% which 0x4000 / 2^15 = 1/2 
        CompareValue = (uint16)(((uint32)(DutyCycle) * (uint32)(Pwm_ChannelConfig[ChannelId].Period)) >> 15);

        /* Update compare value based on channel */
        switch (TIM_Channel)
        {
            case TIM_Channel_1:
                TIM_SetCompare1(TIM_Instance, CompareValue);
                break;
            case TIM_Channel_2:
                TIM_SetCompare2(TIM_Instance, CompareValue);
                break;
            case TIM_Channel_3:
                TIM_SetCompare3(TIM_Instance, CompareValue);
                break;
            case TIM_Channel_4:
                TIM_SetCompare4(TIM_Instance, CompareValue);
                break;
            default:
                RetVal = E_NOT_OK;
                break;
        }
        
        if (RetVal == E_OK)
        {
            /* Update runtime data */
            Pwm_ChannelConfig[ChannelId].DutyCycle = DutyCycle;
        }
    }
    
    return RetVal;
}
#endif


#if (PWM_SET_PERIOD_AND_DUTY_API == STD_ON)
/**
 * @brief Sets PWM period and duty cycle for a channel
 * @details Updates both period and duty cycle for variable period channels
 * @param[in] ChannelId Channel identifier
 * @param[in] Period New period value
 * @param[in] DutyCycle New duty cycle value (0x0000 to 0x8000)
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType PwmHw_SetPeriodAndDuty(Pwm_ChannelType ChannelId, Pwm_PeriodType Period, uint16 DutyCycle)
{
    Std_ReturnType RetVal = E_OK;
    uint16 CompareValue;
    
    /* Validate parameters */
    if ((ChannelId >= PWM_MAX_CHANNELS) || (Period == 0) || (DutyCycle > 0x8000))
    {
        RetVal = E_NOT_OK;
    }
    else
    {
        uint16 TIM_Channel = PWM_HW_GET_TIM_CHANNEL(ChannelId);
        TIM_TypeDef* TIM_Instance = PWM_HW_GET_TIMER(Pwm_ChannelConfig[ChannelId].HwUnit);
        /* Update timer period */
        TIM_SetAutoreload(TIM_Instance, Period - 1);
        
        /* Calculate compare value */
        CompareValue = (uint16)(((uint32)(DutyCycle) * (uint32)(Pwm_ChannelConfig[ChannelId].Period)) >> 15);
        
        /* Update compare value based on channel */
        switch (TIM_Channel)
        {
            case TIM_Channel_1:
                TIM_SetCompare1(TIM_Instance, CompareValue);
                break;
            case TIM_Channel_2:
                TIM_SetCompare2(TIM_Instance, CompareValue);
                break;
            case TIM_Channel_3:
                TIM_SetCompare3(TIM_Instance, CompareValue);
                break;
            case TIM_Channel_4:
                TIM_SetCompare4(TIM_Instance, CompareValue);
                break;
            default:
                RetVal = E_NOT_OK;
                break;
        }
        
        if (RetVal == E_OK)
        {
            /* Update runtime data */
            Pwm_ChannelConfig[ChannelId].Period = Period;
            Pwm_ChannelConfig[ChannelId].DutyCycle = DutyCycle;
            Pwm_HwUnitConfig[Pwm_ChannelConfig[ChannelId].HwUnit].MaxPeriod = Period;
        }
    }
    
    return RetVal;
}
#endif
/****************************************************************************************
*                              OUTPUT CONTROL FUNCTIONS                               *
****************************************************************************************/

/**
 * @brief Sets PWM output to idle state
 * @details Forces the PWM output to the configured idle state
 * @param[in] ChannelId Channel identifier
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType PwmHw_SetOutputToIdle(Pwm_ChannelType ChannelId)
{
    Std_ReturnType RetVal = E_OK;
    
    /* Validate parameters */
    if (ChannelId >= PWM_MAX_CHANNELS)
    {
        RetVal = E_NOT_OK;
    }
    else
    {
        uint16 TIM_Channel = PWM_HW_GET_TIM_CHANNEL(ChannelId);
        TIM_TypeDef* TIM_Instance = PWM_HW_GET_TIMER(Pwm_ChannelConfig[ChannelId].HwUnit);
        /* Disable output compare based on channel */
        TIM_CCxCmd(TIM_Instance, TIM_Channel, TIM_CCx_Disable);

        /* Force output to idle state using timer's forced output mode */
        switch (TIM_Channel)
        {
            case TIM_Channel_1: 
                if (Pwm_ChannelConfig[ChannelId].IdleState == PWM_HIGH) {
                    TIM_ForcedOC1Config(TIM_Instance, TIM_ForcedAction_Active);
                } else {    
                    TIM_ForcedOC1Config(TIM_Instance, TIM_ForcedAction_InActive);
                }
                break;
            case TIM_Channel_2:
                if (Pwm_ChannelConfig[ChannelId].IdleState == PWM_HIGH) {
                    TIM_ForcedOC2Config(TIM_Instance, TIM_ForcedAction_Active);
                } else {
                    TIM_ForcedOC2Config(TIM_Instance, TIM_ForcedAction_InActive);
                }
                break;
            case TIM_Channel_3:
                if (Pwm_ChannelConfig[ChannelId].IdleState == PWM_HIGH) {
                    TIM_ForcedOC3Config(TIM_Instance, TIM_ForcedAction_Active);
                } else {
                    TIM_ForcedOC3Config(TIM_Instance, TIM_ForcedAction_InActive);
                }
                break;
            case TIM_Channel_4:
                if (Pwm_ChannelConfig[ChannelId].IdleState == PWM_HIGH) {
                    TIM_ForcedOC4Config(TIM_Instance, TIM_ForcedAction_Active);
                } else {
                    TIM_ForcedOC4Config(TIM_Instance, TIM_ForcedAction_InActive);
                }
                break;
        default: return PWM_LOW;
        }
        /* Re-enable output to apply forced state */
        TIM_CCxCmd(TIM_Instance, TIM_Channel, TIM_CCx_Enable);
        Pwm_ChannelConfig[ChannelId].IdleStateSet = TRUE;
    }
    
    return RetVal;
}

/**
 * @brief Gets PWM output state
 * @details Returns the current logical state of the PWM output
 * @param[in] ChannelId Channel identifier
 * @return PWM_HIGH or PWM_LOW
 */
Pwm_OutputStateType PwmHw_GetOutputState(Pwm_ChannelType ChannelId)
{
    uint16 TIM_Channel = PWM_HW_GET_TIM_CHANNEL(ChannelId);
    TIM_TypeDef* TIM_Instance = PWM_HW_GET_TIMER(Pwm_ChannelConfig[ChannelId].HwUnit);
    uint16_t compare_value;
    Pwm_OutputStateType RetVal;
    switch (TIM_Channel)
    {
        case TIM_Channel_1: compare_value = TIM_Instance->CCR1; break;
        case TIM_Channel_2: compare_value = TIM_Instance->CCR2; break;
        case TIM_Channel_3: compare_value = TIM_Instance->CCR3; break;
        case TIM_Channel_4: compare_value = TIM_Instance->CCR4; break;
        default: return PWM_LOW;
    }
    if(Pwm_HwUnitConfig[Pwm_ChannelConfig[ChannelId].HwUnit].MaxPeriod == 1)
        RetVal = (compare_value > 0) ? PWM_HIGH : PWM_LOW;
    else
        RetVal = (compare_value == 0) ? PWM_LOW : PWM_HIGH;
    return RetVal;
}

/****************************************************************************************
*                              NOTIFICATION FUNCTIONS                                 *
****************************************************************************************/
/**
 * @brief Enables PWM notification for a channel
 * @details Enables interrupt generation for the specified channel với quản lý thông minh
 * @param[in] ChannelId Channel identifier
 * @param[in] Notification Notification edge type
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType PwmHw_EnableNotification(Pwm_ChannelType ChannelId, Pwm_EdgeNotificationType Notification)
{
    Std_ReturnType RetVal = E_OK;
    uint16 TIM_Channel = PWM_HW_GET_TIM_CHANNEL(ChannelId);
    TIM_TypeDef* TIM_Instance = PWM_HW_GET_TIMER(Pwm_ChannelConfig[ChannelId].HwUnit);
    Pwm_HwUnitType HwUnit = Pwm_ChannelConfig[ChannelId].HwUnit;
    uint8 channel_bit = (1 << (TIM_Channel >> 2)); // Convert TIM_Channel to bit position
    /* Validate parameters */
    if (ChannelId >= PWM_MAX_CHANNELS)
    {
        RetVal = E_NOT_OK;
    }
    else
    {        
        /* Rising Edge (Update interrupt) */
        if(Notification == PWM_RISING_EDGE || Notification == PWM_BOTH_EDGES)
        {
            /* Mark this channel is being used for Update interrupt */
            Pwm_UpdateInterruptUsers[HwUnit] |= channel_bit;
            
            /* Only enable Update interrupt if it having enable */
            if (!(TIM_Instance->DIER & TIM_IT_Update))
            {
                TIM_ITConfig(TIM_Instance, TIM_IT_Update, ENABLE);
            }
            // if(Pwm_UpdateInterruptUsers[HwUnit] == 0)
            // {
            //     /* Disable Update interrupt if no channel is using it */
            //     TIM_ITConfig(TIM_Instance, TIM_IT_Update, ENABLE);
            // }
        }
        
        /* Falling Edge (CC interrupt) */
        if(Notification == PWM_FALLING_EDGE || Notification == PWM_BOTH_EDGES)
        {
            /* Enable CC interrupt cho channel cụ thể */
            switch (TIM_Channel)
            {
                case TIM_Channel_1:
                    TIM_ITConfig(TIM_Instance, TIM_IT_CC1, ENABLE);
                    break;
                case TIM_Channel_2:
                    TIM_ITConfig(TIM_Instance, TIM_IT_CC2, ENABLE);
                    break;
                case TIM_Channel_3:
                    TIM_ITConfig(TIM_Instance, TIM_IT_CC3, ENABLE);
                    break;
                case TIM_Channel_4:
                    TIM_ITConfig(TIM_Instance, TIM_IT_CC4, ENABLE);
                    break;
                default:
                    RetVal = E_NOT_OK;
                    break;
            }
        }
        
        if (RetVal == E_OK)
        {
            /* Update enabled */
            Pwm_ChannelConfig[ChannelId].NotificationEnabled = TRUE;
            Pwm_ChannelConfig[ChannelId].NotificationEdge = Notification;
        }
    }
    
    return RetVal;
}

/**
 * @brief Disables PWM notification for a channel
 * @details Disables interrupt generation cho channel cụ thể mà không ảnh hưởng channel khác
 * @param[in] ChannelId Channel identifier
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType PwmHw_DisableNotification(Pwm_ChannelType ChannelId)
{
    Std_ReturnType RetVal = E_OK;
    uint16 TIM_Channel = PWM_HW_GET_TIM_CHANNEL(ChannelId);

    Pwm_HwUnitType HwUnit = Pwm_ChannelConfig[ChannelId].HwUnit;
    Pwm_EdgeNotificationType Notification = Pwm_ChannelConfig[ChannelId].NotificationEdge;

    TIM_TypeDef* TIM_Instance = PWM_HW_GET_TIMER(HwUnit);
    uint8 channel_bit = (1 << (TIM_Channel >> 2)); // Convert TIM_Channel to bit position
    
    /* Validate parameters */
    if (ChannelId >= PWM_MAX_CHANNELS)
    {
        RetVal = E_NOT_OK;
    }
    else
    {
        /* Rising Edge (Update interrupt) */
        if(Notification == PWM_RISING_EDGE || Notification == PWM_BOTH_EDGES)
        {
            /* Bỏ đánh dấu channel này khỏi Update interrupt users */
            Pwm_UpdateInterruptUsers[HwUnit] &= ~channel_bit;
            
            /* Chỉ disable Update interrupt nếu không còn channel nào sử dụng */
            if (Pwm_UpdateInterruptUsers[HwUnit] == 0)
            {
                TIM_ITConfig(TIM_Instance, TIM_IT_Update, DISABLE);
            }
        }
        
        /* Xử lý Falling Edge (CC interrupt) */
        if(Notification == PWM_FALLING_EDGE || Notification == PWM_BOTH_EDGES)
        {
            /* Disable CC interrupt cho channel cụ thể */
            switch (TIM_Channel)
            {
                case TIM_Channel_1:
                    TIM_ITConfig(TIM_Instance, TIM_IT_CC1, DISABLE);
                    break;
                case TIM_Channel_2:
                    TIM_ITConfig(TIM_Instance, TIM_IT_CC2, DISABLE);
                    break;
                case TIM_Channel_3:
                    TIM_ITConfig(TIM_Instance, TIM_IT_CC3, DISABLE);
                    break;
                case TIM_Channel_4:
                    TIM_ITConfig(TIM_Instance, TIM_IT_CC4, DISABLE);
                    break;
                default:
                    RetVal = E_NOT_OK;
                    break;
            }
        }
        
        if (RetVal == E_OK)
        {
            /* Update enabled */
            Pwm_ChannelConfig[ChannelId].NotificationEnabled = FALSE;
        }
    }
    
    return RetVal;
}


