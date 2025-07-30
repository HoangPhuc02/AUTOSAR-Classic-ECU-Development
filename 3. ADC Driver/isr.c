/****************************************************************************************
*                                ISR.C                                                  *
****************************************************************************************
* File Name   : isr.c
* Description : Interrupt Service Routine Source File
* Details     : Interrupt implementation.
* Version     : 1.0.0
* Date        : 01/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "misc.h" // For NVIC configuration
// #include "Dio.h"
// #include "Port.h"

#include "Adc_Cfg.h"
volatile uint32 it_count =0;
volatile uint32 dma_count = 0;

void ADC1_2_IRQHandler(void)
{
    it_count++;
    // Check if the ADC conversion is complete
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET)
    {
        Adc_TransferComplete_Callback(ADC1);

        // Clear the interrupt flag
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    }

    else if (ADC_GetITStatus(ADC2, ADC_IT_EOC) != RESET)
    {
        // Handle overrun error
        Adc_TransferComplete_Callback(ADC2);

        // Clear the interrupt flag
        ADC_ClearITPendingBit(ADC2, ADC_IT_EOC);
    }
}
void DMA1_Channel1_IRQHandler(void)
{
    // add for test
    dma_count++;
    if (DMA_GetITStatus(DMA1_IT_TC1))
    {
        Adc_DmaTransferComplete_Callback(DMA1_Channel1);
        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }
}
