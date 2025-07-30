/****************************************************************************************
*                                ISR.C                                                  *
****************************************************************************************
* File Name   : isr.c
* Description : Interrupt Service Routine Source File
* Details     : Interrupt implementation.
* Version     : 1.0.0
* Date        : 01/07/1015
* Author      : hoangphuc540101@gmail.com
* Github      : https://github.com/HoangPhuc01
****************************************************************************************/

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "misc.h" // For NVIC configuration
#include "Pwm.h"


void TIM1_IRQHandler(void)
{
    // Check if the timer update interrupt is pending
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM1, TIM_IT_Update);
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
    if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM1, TIM_IT_CC1);
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
    }
    if (TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM1, TIM_IT_CC2);
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
    }
    if (TIM_GetITStatus(TIM1, TIM_IT_CC3) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM1, TIM_IT_CC3);
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC3);
    }
    if (TIM_GetITStatus(TIM1, TIM_IT_CC4) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM1, TIM_IT_CC4);
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC4);
    }
}   
void TIM2_IRQHandler(void)
{
    // Check if the timer update interrupt is pending
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM2, TIM_IT_Update);
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
    if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM2, TIM_IT_CC1);
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
    }
    if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM2, TIM_IT_CC2);
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
    }
    if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM2, TIM_IT_CC3);
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
    }
    if (TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM2, TIM_IT_CC4);
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
    }
} 
void TIM3_IRQHandler(void)
{
    // Check if the timer update interrupt is pending
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM3, TIM_IT_Update);
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
    if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM3, TIM_IT_CC1);
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
    }
    if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM3, TIM_IT_CC2);
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    }
    if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM3, TIM_IT_CC3);
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
    }
    if (TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM3, TIM_IT_CC4);
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
    }
} 

void TIM4_IRQHandler(void)
{
    // Check if the timer update interrupt is pending
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM4, TIM_IT_Update);
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
    if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM4, TIM_IT_CC1);
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
    }
    if (TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM4, TIM_IT_CC2);
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);
    }
    if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM4, TIM_IT_CC3);
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
    }
    if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)
    {

        // Clear the interrupt flag
        Pwm_NotificationHandler(PWM_HW_UNIT_TIM4, TIM_IT_CC4);
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
    }
} 
