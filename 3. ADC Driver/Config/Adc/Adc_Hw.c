/****************************************************************************************
*                                ADC_HW.C                                              *
****************************************************************************************
* File Name   : Adc_Hw.c
* Module      : Analog to Digital Converter (ADC) - Hardware Layer
* Description : AUTOSAR ADC driver hardware abstraction layer source file 
* Version     : 3.1.0 - Major redesign with enhanced functionality
* Date        : 24/06/2025
* Update      : 23/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Adc_Hw.h"
#include "Adc_Cfg.h"
#include "Adc_Types.h"
#include "Std_Types.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

/****************************************************************************************
*                                 GLOBAL VARIABLES                                     *
****************************************************************************************/

/****************************************************************************************
*                                 QUEUE CONFIGURATIONS                                 *
****************************************************************************************/
/* Queue for ADC Hardware Unit 1 */
static Adc_GroupType AdcHw_GroupQueueHw1[ADC1_QUEUE_SIZE] = {ADC_INVALID_GROUP_ID};

/* Queue for ADC Hardware Unit 2 */
static Adc_GroupType AdcHw_GroupQueueHw2[ADC2_QUEUE_SIZE] = {ADC_INVALID_GROUP_ID};

/* Runtime data arrays */
static volatile Adc_RuntimeGroupType Adc_RuntimeGroups[ADC_MAX_GROUPS] = {0};
static volatile Adc_RuntimeHwUnitType Adc_RuntimeHwUnits[ADC_MAX_HW_UNITS] = 
{
    /* ADC Hardware Unit 1 */
    {
        .CurrentGroupId = ADC_INVALID_GROUP_ID,
        .HwUnitState    = HW_STATE_IDLE,
        .QueueGroup     = AdcHw_GroupQueueHw1,        
        .QueueMaxSize   = ADC_DEFAULT_QUEUE_SIZE,         
        .QueueHead      = 0,              
        .QueueTail      = 0,
        .QueueCount     = 0,             
    },
    /* ADC Hardware Unit 2 */
    {
        .CurrentGroupId = ADC_INVALID_GROUP_ID,
        .HwUnitState    = HW_STATE_IDLE,
        .QueueGroup     = AdcHw_GroupQueueHw2,            
        .QueueMaxSize   = ADC_DEFAULT_QUEUE_SIZE,         
        .QueueHead      = 0,          
        .QueueTail      = 0,              
        .QueueCount     = 0,
    }
};

/* Deferred processing variables */
static volatile uint8 AdcHw_DeferredProcessingFlag[ADC_MAX_HW_UNITS] = {0};
static volatile Adc_HwUnitType AdcHw_PendingUnits[ADC_MAX_HW_UNITS];
static volatile uint8 AdcHw_PendingCount = 0;

/* ISR performance tracking */
#if (ADC_ENABLE_ISR_MONITORING == STD_ON)
static uint32 AdcHw_IsrStartTime[ADC_MAX_HW_UNITS];
static uint32 AdcHw_MaxIsrTime[ADC_MAX_HW_UNITS];
#endif

/****************************************************************************************
*                                 STATIC FUNCTION PROTOTYPES                          *
****************************************************************************************/
static inline Std_ReturnType AdcHw_ConfigureHwModule(Adc_HwUnitType HwUnitId);
static inline Std_ReturnType AdcHw_ConfigureClocks(Adc_HwUnitType HwUnitId);
static inline Std_ReturnType AdcHw_ConfigureNvic(Adc_HwUnitType HwUnitId);

static void AdcHw_HandleChannelSequencing(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);
static void AdcHw_StartNextConversion(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);
static void AdcHw_CallNotification(Adc_GroupType GroupId);
static uint32 AdcHw_GetCurrentTime(void);

static inline Std_ReturnType AdcHw_ConfigureHwModuleGroup(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);
static inline Std_ReturnType AdcHw_ConfigureHwModuleGroupDMA(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);
static inline Std_ReturnType AdcHw_ConfigureHwModuleGroupIT(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);
/***************************************************************************************
*                                 INITIALIZATION FUNCTIONS                            *
****************************************************************************************/

/**
 * @brief Initialize ADC hardware module
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_Init(Adc_HwUnitType HwUnitId)
{
    /* Validate hardware unit */
    if (ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE)
    {
        return E_NOT_OK;
    }
    
    /* Get ADC hardware module */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    if (ADCx == NULL)
    {
        return E_NOT_OK;
    }
    
    /* Configure clocks */
    if (AdcHw_ConfigureClocks(HwUnitId) != E_OK)
    {
        return E_NOT_OK;
    }
    
    /* Configure hardware module */
    if (AdcHw_ConfigureHwModule(HwUnitId) != E_OK)
    {
        return E_NOT_OK;
    }
    
    /* Configure NVIC */
    if (AdcHw_ConfigureNvic(HwUnitId) != E_OK)
    {
        return E_NOT_OK;
    }
    
    /* Initialize DMA if enabled */
    #if (ADC_ENABLE_DMA == STD_ON)
    if (Adc_HwUnitConfig[HwUnitId].AdcHw_DMAAvailable)
    {
        if (AdcHw_InitDma(HwUnitId, 0) != E_OK)
        {
            return E_NOT_OK;
        }
    }
    #endif
    
    /* Perform calibration - Commented out for now */
    // ADC_StartCalibration(ADCx);
    // while (ADC_GetCalibrationStatus(ADCx) != RESET)
    // {
    //     /* Wait for calibration to complete */
    // }
    
    /* Initialize runtime data */
    AdcHw_ResetHwRuntime(HwUnitId);
    
    return E_OK;
}

/**
 * @brief Deinitialize ADC hardware module
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_DeInit(Adc_HwUnitType HwUnitId)
{
    /* Validate hardware unit */
    if (ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE)
    {
        return E_NOT_OK;
    }
    
    /* Get ADC hardware module */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    if (ADCx == NULL)
    {
        return E_NOT_OK;
    }
    
    /* Stop any ongoing conversions */
    ADC_SoftwareStartConvCmd(ADCx, DISABLE);
    ADC_Cmd(ADCx, DISABLE);
    
    /* Disable interrupts */
    AdcHw_DisableInterrupt(HwUnitId, ADC_INTERRUPT_EOC);
    
    /* Deinitialize DMA if enabled */
    #if (ADC_ENABLE_DMA == STD_ON)
    if (Adc_HwUnitConfig[HwUnitId].AdcHw_DMAAvailable)
    {
        AdcHw_DeInitDma(HwUnitId);
    }
    #endif
    
    /* Reset ADC module */
    ADC_DeInit(ADCx);
    
    /* Reset runtime data */
    AdcHw_ResetHwRuntime(HwUnitId);
    
    return E_OK;
}


/****************************************************************************************
*                               SW CONVERSION CONTROL FUNCTIONS                        *
****************************************************************************************/
/**
 * @brief Start software-triggered conversion
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note: 
 * - Called from start software
 * - Go to next sw conversion when stop last session if using QUEUE
 * - Recall function after hw finish if using QUEUE
 */
Std_ReturnType AdcHw_StartSwConversion(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    /* Validate parameters */
    if ((ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE) || (ADC_HW_IS_VALID_GROUP(GroupId) == FALSE))
    {
        return E_NOT_OK;
    }
    
    /* Check if hardware unit is busy */
    /* If Hw trigger is enabled not allow any conversion request*/
    if (AdcHw_GetHwUnitState(HwUnitId) == HW_STATE_HW)
    {
        return E_NOT_OK;
    }

    #if (ADC_ENABLE_QUEUING == STD_ON)
    /* Check is it called from start software not from recall or stop software */
    if (Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId != GroupId)
    {
        /* Add to queue if enable*/
        return AdcHw_AddGroupToQueue(HwUnitId, GroupId);
    }
    #else 
    /* Check if there is any conversion on progess */
    if (Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId != ADC_INVALID_GROUP_ID)
    {
        return E_NOT_OK;
    }
    #endif

    /* Get ADC hardware module */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    const Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[GroupId];
    const Adc_HwUnitDefType* HwUnitConfig = &Adc_HwUnitConfig[HwUnitId];
    
    if (ADCx == NULL_PTR)
    {
        return E_NOT_OK;
    }
    
    /* Setting adc hardware before make conversion*/
    if (AdcHw_ConfigureGroup(HwUnitId, GroupId) != E_OK)
    {
        return E_NOT_OK;
    }
    
    /* Update runtime data */
    /* Register group to runtime hw*/
    Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = GroupId;
    Adc_RuntimeHwUnits[HwUnitId].HwUnitState = HW_STATE_SW;
    
    /** Reset Runtime Group status */
    Adc_RuntimeGroups[GroupId].CurrentChannelId = 0;
    Adc_RuntimeGroups[GroupId].SampleCounter = 0;
    Adc_RuntimeGroups[GroupId].BufferIndex = 0;
    
    /* Enable interrupts */
    if (HwUnitConfig->AdcHw_DMAAvailable && GroupConfig->Adc_InterruptType == ADC_HW_DMA)
    {
        #if (ADC_ENABLE_DMA == STD_ON)
        if (AdcHw_InitDma(HwUnitId, GroupId) != E_OK)
        {
            return E_NOT_OK;
        }
        AdcHw_DisableInterrupt(HwUnitId, ADC_INTERRUPT_EOC);
        AdcHw_EnableInterrupt(HwUnitId, ADC_INTERRUPT_DMA_TC);
        ADC_DMACmd(ADCx, ENABLE);
        #else    
        AdcHw_EnableInterrupt(HwUnitId, ADC_INTERRUPT_EOC);
        #endif
    }
    else 
    {
        AdcHw_EnableInterrupt(HwUnitId, ADC_INTERRUPT_EOC);
    }
    
    AdcHw_SetGroupStatus(GroupId, ADC_BUSY);
    ADC_Cmd(ADCx, ENABLE);
    
    /* Start conversion */
    ADC_SoftwareStartConvCmd(ADCx, ENABLE);
    
    return E_OK;
}

/**
 * @brief Stop software-triggered conversion
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_StopSwConversion(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    /* Validate parameters */
    if ((ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE) || (ADC_HW_IS_VALID_GROUP(GroupId) == FALSE))
    {
        return E_NOT_OK;
    }
    
    /* Check if this group is currently converting */
    if (Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId != GroupId)
    {
        #if (ADC_ENABLE_QUEUING == STD_ON)
        /* Try to remove from queue */
        return AdcHw_RemoveGroupFromQueue(HwUnitId, GroupId);
        #else
        return E_NOT_OK;
        #endif
    }

    /* Get ADC hardware module first - common for both paths */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    if (ADCx == NULL)
    {
        return E_NOT_OK;
    }
    
    #if (ADC_ENABLE_DMA == STD_ON)
    /* Disable DMA if it was enabled for this group */
    if (Adc_HwUnitConfig[HwUnitId].AdcHw_DMAAvailable && 
        Adc_GroupConfig[GroupId].Adc_InterruptType == ADC_HW_DMA)
    {
        AdcHw_DeInitDma(HwUnitId);
    }
    else
    {
        AdcHw_DisableInterrupt(HwUnitId, ADC_INTERRUPT_EOC);
    }
    #endif
    /* Stop conversion - common hardware operation */
    ADC_SoftwareStartConvCmd(ADCx, DISABLE);
    ADC_Cmd(ADCx, DISABLE);
    
    #if (ADC_ENABLE_QUEUING == STD_ON)
        /* Try to process next group from queue if available */
        Adc_GroupType NextGroup = AdcHw_GetNextGroupFromQueue(HwUnitId);
        if (NextGroup != ADC_INVALID_GROUP_ID)
        {
            Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = NextGroup;
            return AdcHw_StartSwConversion(HwUnitId, NextGroup);
        }
    #endif
    
    /* If no next group or queuing disabled, reset everything */
    AdcHw_ResetHwRuntime(HwUnitId);
    AdcHw_SetGroupStatus(GroupId, ADC_IDLE);
    return E_OK;
}

#if (ADC_ENABLE_QUEUING == STD_ON)
/**
 * @brief Recall software-triggered conversion  
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note : call after finishing hw conversion or hw conversion is stopped
 *         Only used when queue enabled
 */
Std_ReturnType AdcHw_RecallSwConversion(Adc_HwUnitType HwUnitId)
{
    // No queue in the list 
    if(Adc_RuntimeHwUnits[HwUnitId].QueueCount == 0)
    {
        return E_NOT_OK;
    }
    
    Adc_RuntimeHwUnits[HwUnitId].HwUnitState = HW_STATE_SW;
    // Adc_RuntimeGroups[GroupId].Status = ADC_IDLE;

    // Get the group id from head of the queue
    Adc_GroupType CurrentGroup = Adc_RuntimeHwUnits[HwUnitId].QueueGroup[Adc_RuntimeHwUnits[HwUnitId].QueueHead];
    if(CurrentGroup != ADC_INVALID_GROUP_ID)
    {
        Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = CurrentGroup;
        AdcHw_StartSwConversion(HwUnitId, CurrentGroup);
    }
    
    return E_OK;
}
 #endif
/****************************************************************************************
*                               HW CONVERSION CONTROL FUNCTIONS                        *
****************************************************************************************/
/**
 * @brief Start hardware-triggered conversion
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
// TODO check config for hw
Std_ReturnType AdcHw_StartHwConversion(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    /* Validate parameters */
    if ((ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE) || (ADC_HW_IS_VALID_GROUP(GroupId) == FALSE))
    {
        return E_NOT_OK;
    }
    
    /* Get ADC hardware module */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    if (ADCx == NULL)
    {
        return E_NOT_OK;
    }
    
    /* Configure group */
    if (AdcHw_ConfigureGroup(HwUnitId, GroupId) != E_OK)
    {
        return E_NOT_OK;
    }
    // Already have a trigger
    if (AdcHw_GetHwUnitState(HwUnitId) == HW_STATE_HW)
    {
        return E_NOT_OK;
    }

    AdcHw_DisableInterrupt(HwUnitId, ADC_INTERRUPT_DMA_TC);
    // should be make into inline function
    ADC_Cmd(ADCx,DISABLE);
    /* Get group configuration */
    Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[GroupId];
    
    /* Configure hardware trigger */
    // TODO get trigger source hw and set
    // uint32 TriggerSource = AdcHw_GetHwTriggerSource(GroupConfig->Adc_TriggerSource);
    ADC_ExternalTrigConvCmd(ADCx, ENABLE);
    
    /* Configure trigger signal */
    if (GroupConfig->Adc_HwTriggerSignal == ADC_HW_TRIG_RISING_EDGE)
    {
        /* Configure for rising edge */
    }
    else if (GroupConfig->Adc_HwTriggerSignal == ADC_HW_TRIG_FALLING_EDGE)
    {
        /* Configure for falling edge */
    }
    else
    {
        /* Configure for both edges */
    }
    
    /* Update runtime data */
    Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = GroupId;
    Adc_RuntimeHwUnits[HwUnitId].HwUnitState = HW_STATE_HW;
    AdcHw_SetGroupStatus(GroupId, ADC_BUSY);
    
    /* Enable interrupts */

    AdcHw_EnableInterrupt(HwUnitId, ADC_INTERRUPT_EOC);
    ADC_Cmd(ADCx,ENABLE);
    return E_OK;
}

/**
 * @brief Stop hardware-triggered conversion
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_StopHwConversion(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    /* Validate parameters */
    if ((ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE) || (ADC_HW_IS_VALID_GROUP(GroupId) == FALSE))
    {
        return E_NOT_OK;
    }
    
    /* Get ADC hardware module */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    if (ADCx == NULL)
    {
        return E_NOT_OK;
    }
    
    /* Disable hardware trigger */
    ADC_ExternalTrigConvCmd(ADCx, DISABLE);
    
    /* Disable interrupts */
    AdcHw_DisableInterrupt(HwUnitId, ADC_INTERRUPT_EOC);
    
    /* Update runtime data */
    Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = ADC_INVALID_GROUP_ID;
    // Adc_RuntimeHwUnits[HwUnitId].HwUnitState = FALSE;
    Adc_RuntimeHwUnits[HwUnitId].HwUnitState = HW_STATE_IDLE;
    AdcHw_SetGroupStatus(GroupId, ADC_IDLE);
    #if (ADC_ENABLE_QUEUING == STD_ON)
        AdcHw_RecallSwConversion(HwUnitId);
    #endif
    return E_OK;
}


/****************************************************************************************
*                                  CONFIGURATION FUNCTIONS                              *
****************************************************************************************/
/**
 * @brief Configure ADC channels for a group
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_ConfigureChannels(ADC_TypeDef* ADCx, Adc_HwUnitDefType* HwUnitConfig, Adc_GroupDefType* GroupConfig)
{
    if(GroupConfig->Adc_InterruptType == ADC_HW_EOC)
    {    
        const Adc_ChannelDefType* ChannelConfig = &GroupConfig->Adc_ChannelGroup[0];
        ADC_RegularChannelConfig(ADCx, ChannelConfig->Adc_ChannelId, 1, ChannelConfig->Adc_ChannelSampTime);
    }
    else
    {
        /* Configure each channel in the group */
        for (uint8 i = 0; i < GroupConfig->Adc_NbrOfChannel; i++)
        {
            const Adc_ChannelDefType* ChannelConfig = &GroupConfig->Adc_ChannelGroup[i];
            ADC_RegularChannelConfig(ADCx, ChannelConfig->Adc_ChannelId, i + 1, ChannelConfig->Adc_ChannelSampTime);
        }
    }
    return E_OK;
}
/**
 * @brief Configure ADC hardware module for a specific group
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_ConfigureGroup(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    /* Configure channels for this group */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    if (ADCx == NULL)
    {
        return E_NOT_OK;
    }

    Adc_HwUnitDefType* HwUnitConfig = &Adc_HwUnitConfig[HwUnitId];
    Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[GroupId];

    AdcHw_ConfigureHwModuleGroup(HwUnitId, GroupId);

    if (AdcHw_ConfigureChannels(ADCx, HwUnitConfig, GroupConfig) != E_OK)
    {
        return E_NOT_OK;
    }
    return E_OK;
}


/****************************************************************************************
*                                 RESULT HANDLING FUNCTIONS                           *
****************************************************************************************/
/**
 * @brief Read conversion result
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @param[out] ResultPtr Pointer to result buffer
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_ReadResult(Adc_HwUnitType HwUnitId, 
                               Adc_GroupType GroupId, 
                               Adc_ValueGroupType* ResultPtr)
{
    /* Validate parameters */
    if ((ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE) || (ADC_HW_IS_VALID_GROUP(GroupId) == FALSE) || (ResultPtr == NULL))
    {
        return E_NOT_OK;
    }
    
    /* Get group configuration */
    Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[GroupId];
    
    /* Check if results are available */
    if ((Adc_RuntimeGroups[GroupId].Status != ADC_COMPLETED) &&
        (Adc_RuntimeGroups[GroupId].Status != ADC_STREAM_COMPLETED))
    {
        return E_NOT_OK;
    }
    
    /* Copy results to output buffer */
    uint16 ResultSize = GroupConfig->Adc_NbrOfChannel;
    Adc_StreamNumSampleType SampleCounter = (Adc_RuntimeGroups[GroupId].SampleCounter - 1) * ResultSize;

    for (uint16 i = 0; i < ResultSize; i++)
    {
        ResultPtr[i] = GroupConfig->Adc_ValueResultPtr[i + SampleCounter] ;
    }
    AdcHw_HandleReadResultState(HwUnitId, GroupId);
    return E_OK;
}


/**
 * @brief Read ADC conversion result for a group
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] GroupId ADC group ID whose results to read
 * @return none
 * @note This function handles the state after reading results, such as updating group status
 *       and notifying callbacks if necessary. It is called internally after reading results.
 */   
void AdcHw_HandleReadResultState(Adc_HwUnitType HwUnitId, 
                               Adc_GroupType GroupId)
{
    if(Adc_RuntimeGroups[GroupId].Status == ADC_STREAM_COMPLETED)
    {
        if(Adc_GroupConfig[GroupId].Adc_GroupConvMode == ADC_CONV_MODE_CONTINUOUS &&
            (Adc_GroupConfig[GroupId].Adc_GroupAccessMode == ADC_ACCESS_MODE_SINGLE ||
            (Adc_GroupConfig[GroupId].Adc_GroupAccessMode == ADC_ACCESS_MODE_STREAMING &&
            Adc_GroupConfig[GroupId].Adc_StreamBufferMode == ADC_STREAM_BUFFER_CIRCULAR)))
        {
            AdcHw_SetGroupStatus(GroupId, ADC_BUSY);
        }
        else
        {
            /* Reset sample counter for next single conversion */
            AdcHw_SetGroupStatus(GroupId, ADC_IDLE);
            /* Handle oneshot mode completion */
            ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
            ADC_Cmd(ADCx, DISABLE);
            
            #if (ADC_ENABLE_QUEUING == STD_ON)
            /* Signal group completion for deferred processing */
            AdcHw_DeferredProcessingFlag[HwUnitId] = 1;
            if (AdcHw_PendingCount < ADC_MAX_HW_UNITS)
            {
                AdcHw_PendingUnits[AdcHw_PendingCount] = HwUnitId;
                AdcHw_PendingCount++;
            } 
            #else 
            Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = ADC_INVALID_GROUP_ID;
            Adc_RuntimeHwUnits[HwUnitId].HwUnitState = HW_STATE_IDLE;  
            #endif     
    
        }
    }
}

/****************************************************************************************
*                                 STATUS FUNCTIONS                                    *
****************************************************************************************/
/**
 * @brief Set private ADC group runtime status
 * @param[in] GroupId ADC group ID
 * @return Sample Counter
 */
Adc_StreamNumSampleType AdcHw_GetGroupRuntimeSampCounter(Adc_GroupType GroupId)
{
    /* Validate parameters */
    if (ADC_HW_IS_VALID_GROUP(GroupId) == FALSE)
    {
        return 255;
    }
    return Adc_RuntimeGroups[GroupId].SampleCounter;
}

/**
 * @brief Get ADC group status
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return Group status
 */
Adc_StatusType AdcHw_GetGroupRuntimeStatus(Adc_GroupType GroupId)
{
    /* Validate parameters */
    if (ADC_HW_IS_VALID_GROUP(GroupId) == FALSE)
    {
        return ADC_IDLE;
    }
    return Adc_RuntimeGroups[GroupId].Status;
}

/**
 * @brief Reset Group Runtime Parameter
 * @param[in] GroupId
 * @return E_NOT_OK if invalid Group, E_OK if reset successfull
 */
Std_ReturnType  AdcHw_ResetGroupRuntime(Adc_GroupType GroupId)
{
    /* Validate parameters */
    if (ADC_HW_IS_VALID_GROUP(GroupId) == FALSE)
    {
        return E_NOT_OK;
    }
    /* Reset runtime data */
    AdcHw_SetGroupStatus(GroupId, ADC_IDLE);
    Adc_RuntimeGroups[GroupId].CurrentChannelId = 0;
    Adc_RuntimeGroups[GroupId].SampleCounter = 0;
    Adc_RuntimeGroups[GroupId].BufferIndex = 0;

    return E_OK;
}

/**
 * @brief Set ADC group status
 * 
 * @param Group 
 * @param Status 
 */
void AdcHw_SetGroupStatus(Adc_GroupType GroupId, Adc_StatusType Status)
{
    Adc_GroupConfig[GroupId].Adc_Status = Status;
    Adc_RuntimeGroups[GroupId].Status = Status;
}


/**
 * @brief Reset private ADC hw runtime  Parameter
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_NOT_OK if invalid Group, E_OK if reset successfull
 */
Std_ReturnType AdcHw_ResetHwRuntime(Adc_HwUnitType HwUnitId)
{
    /* Validate parameters */
    if (ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE)    return E_NOT_OK;


    Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = ADC_INVALID_GROUP_ID;
    Adc_RuntimeHwUnits[HwUnitId].HwUnitState = HW_STATE_IDLE;

    #if( ADC_ENABLE_QUEUING == STD_ON)
    /* Reset runtime data */
    if(Adc_RuntimeHwUnits[HwUnitId].QueueCount != 0)
    {
        for(Adc_GroupType i = 0; i < Adc_RuntimeHwUnits[HwUnitId].QueueMaxSize;i++ )
        {
            Adc_RuntimeHwUnits[HwUnitId].QueueGroup[i] = ADC_INVALID_GROUP_ID ;
        }
    }
    Adc_RuntimeHwUnits[HwUnitId].QueueHead = 0;
    Adc_RuntimeHwUnits[HwUnitId].QueueTail = 0;
    Adc_RuntimeHwUnits[HwUnitId].QueueCount = 0;

    #endif
    return E_OK;
}

/**
 * @brief Get ADC hardware unit current state
 * @param[in] HwUnitId ADC hardware unit ID to query (0 = ADC1, 1 = ADC2)
 * @return Current hardware unit state (HW_STATE_IDLE, HW_STATE_SW, HW_STATE_HW)
 * @note Inline function for fast access to hardware unit state
 */
Adc_HwUnitStateType AdcHw_GetHwUnitState(Adc_HwUnitType HwUnitId)
{
    return Adc_RuntimeHwUnits[HwUnitId].HwUnitState;
}

/**
 * @brief Get current converting channel
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return Current channel ID
 */
Adc_ChannelType AdcHw_GetCurrentChannel(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    /* Validate parameters */
    if ((ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE) || (ADC_HW_IS_VALID_GROUP(GroupId) == FALSE))
    {
        return ADC_INVALID_CHANNEL_ID;
    }
    
    return Adc_RuntimeGroups[GroupId].CurrentChannelId;
}
/****************************************************************************************
*                                 INTERRUPT FUNCTIONS                                 *
****************************************************************************************/
/**
 * @brief Enable ADC interrupts
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] InterruptType Type of interrupt to enable @reg ADC Hw Interrupt types ADC_HW.h
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_EnableInterrupt(Adc_HwUnitType HwUnitId, uint8 InterruptType)
{
    /* Validate parameters */
    if (ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE)
    {
        return E_NOT_OK;
    }
    
    /* Get ADC hardware module */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    if (ADCx == NULL)
    {
        return E_NOT_OK;
    }
    
    /* Enable specific interrupt */
    if (InterruptType & ADC_INTERRUPT_EOC)
    {
        ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE);
    }
    
    if (InterruptType & ADC_INTERRUPT_DMA_TC)
    {
        /* Enable DMA transfer complete interrupt */
        DMA_Channel_TypeDef* DMAx = ADC_HW_GET_DMA_CHANNEL(HwUnitId);
        if (DMAx != NULL)
        {
            DMA_ITConfig(DMAx, DMA_IT_TC, ENABLE);
        }
    }
    
    return E_OK;
}

/**
 * @brief Disable ADC interrupts
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] InterruptType Type of interrupt to disable @reg ADC Hw Interrupt types ADC_HW.h
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_DisableInterrupt(Adc_HwUnitType HwUnitId, uint8 InterruptType)
{
    /* Validate parameters */
    if (ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE)
    {
        return E_NOT_OK;
    }
    
    /* Get ADC hardware module */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    if (ADCx == NULL)
    {
        return E_NOT_OK;
    }
    
    /* Disable specific interrupt */
    if (InterruptType & ADC_INTERRUPT_EOC)
    {
        ADC_ITConfig(ADCx, ADC_IT_EOC, DISABLE);
        NVIC_DisableIRQ(ADC1_2_IRQn);
    }
    
    if (InterruptType & ADC_INTERRUPT_DMA_TC)
    {
        /* Disable DMA transfer complete interrupt */
        DMA_Channel_TypeDef* DMAx = ADC_HW_GET_DMA_CHANNEL(HwUnitId);
        if (DMAx != NULL_PTR)
        {
            DMA_ITConfig(DMAx, DMA_IT_TC, DISABLE);
            NVIC_DisableIRQ(DMA1_Channel1_IRQn);
        }
    }
    
    return E_OK;
}

/**
 * @brief ADC interrupt service routine
 * @param[in] HwUnitId ADC hardware unit ID
 * @return void
 */

/****************************************************************************************
*                           INTERRUPT HANDLER FUNCTIONS                                 *
****************************************************************************************/

/**
 * @brief ADC interrupt service routine
 * @param[in] ADCx ADC hardware module pointer
 * @param[in] HwUnitId ADC hardware unit ID
 * @return void
 */
void AdcHw_InterruptHandler(ADC_TypeDef* ADCx, Adc_HwUnitType HwUnitId)
{
    #if (ADC_ENABLE_ISR_MONITORING == STD_ON)
    AdcHw_IsrStartTime[HwUnitId] = AdcHw_GetCurrentTime();
    #endif
    
    /* Validate hardware unit */
    if (ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE)
    {
        return;
    }
    
    /* Get current group */
    Adc_GroupType CurrentGroup = Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId;
    if (CurrentGroup == ADC_INVALID_GROUP_ID)
    {
        return;
    }
        
    /* Get buffer index */
    uint16 BufferIndex = Adc_RuntimeGroups[CurrentGroup].BufferIndex;
    
    /* Write data to buffer index */
    Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[CurrentGroup];
    GroupConfig->Adc_ValueResultPtr[BufferIndex] = ADC_GetConversionValue(ADCx);
    
    /* Handle channel sequencing */
    AdcHw_HandleChannelSequencing(HwUnitId, CurrentGroup);
    
    #if (ADC_ENABLE_ISR_MONITORING == STD_ON)
    uint32 IsrDuration = AdcHw_GetCurrentTime() - AdcHw_IsrStartTime[HwUnitId];
    if (IsrDuration > AdcHw_MaxIsrTime[HwUnitId])
    {
        AdcHw_MaxIsrTime[HwUnitId] = IsrDuration;
    }
    #endif
}

/**
 * @brief DMA interrupt service routine
 * @param[in] DMAx DMA channel pointer
 * @param[in] HwUnitId ADC hardware unit ID
 * @return void
 */
void AdcHw_DmaInterruptHandler(DMA_Channel_TypeDef* DMAx, Adc_HwUnitType HwUnitId)
{
    /* Validate hardware unit */
    if (ADC_HW_IS_VALID_UNIT(HwUnitId) == FALSE)
    {
        return;
    }
        
    /* Get current group */
    Adc_GroupType CurrentGroup = Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId;
    Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[CurrentGroup];
    
    /* Update runtime data with completion status */
    Adc_RuntimeGroups[CurrentGroup].SampleCounter = GroupConfig->Adc_StreamNumSamples;
    Adc_RuntimeGroups[CurrentGroup].CurrentChannelId = GroupConfig->Adc_NbrOfChannel - 1;
    Adc_RuntimeGroups[CurrentGroup].BufferIndex = GroupConfig->Adc_StreamNumSamples * GroupConfig->Adc_NbrOfChannel - 1;
    
    AdcHw_SetGroupStatus(CurrentGroup, ADC_STREAM_COMPLETED);
    
    /* Call notification when done conversion */
    AdcHw_CallNotification(CurrentGroup);
    
    /* Handle continuous vs oneshot mode - this should be fix in ver 3.0 */
    if (GroupConfig->Adc_GroupConvMode == ADC_CONV_MODE_CONTINUOUS)
    {
        /* Reset parameters for continuous mode */
        Adc_RuntimeGroups[CurrentGroup].SampleCounter = 0;
        Adc_RuntimeGroups[CurrentGroup].CurrentChannelId = 0;
        Adc_RuntimeGroups[CurrentGroup].BufferIndex = 0;
        ADC_SoftwareStartConvCmd(ADC_HW_GET_MODULE_ID(HwUnitId), ENABLE); 
    }
    else 
    {
        /* Handle oneshot mode completion */
        ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
        ADC_Cmd(ADCx, DISABLE);
        
        #if (ADC_ENABLE_QUEUING == STD_ON)
        /* Signal group completion for deferred processing */
        AdcHw_DeferredProcessingFlag[HwUnitId] = 1;
        if (AdcHw_PendingCount < ADC_MAX_HW_UNITS)
        {
            AdcHw_PendingUnits[AdcHw_PendingCount] = HwUnitId;
            AdcHw_PendingCount++;
        } 
        #else 
        Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = ADC_INVALID_GROUP_ID;
        Adc_RuntimeHwUnits[HwUnitId].HwUnitState = HW_STATE_IDLE;  
        #endif     
    }
    
    /* TODO: add check if not streaming */
    /* TODO: check if linear or one shot get out call next software */
}
/****************************************************************************************
*                              QUEUE MANAGEMENT FUNCTIONS                             *
****************************************************************************************/
#if (ADC_ENABLE_QUEUING == STD_ON)
/**
 * @brief Add group to conversion queue
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_AddGroupToQueue(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    if(Adc_RuntimeHwUnits[HwUnitId].QueueCount < Adc_RuntimeHwUnits[HwUnitId].QueueMaxSize||
        AdcHw_IsGroupInQueue(HwUnitId, GroupId) == ADC_INVALID_GROUP_ID) 
    {
        Adc_RuntimeHwUnits[HwUnitId].QueueCount++;
        // if you add the first group of this queue
        if(Adc_RuntimeHwUnits[HwUnitId].QueueCount == 1) 
        {
            Adc_RuntimeHwUnits[HwUnitId].QueueGroup[Adc_RuntimeHwUnits[HwUnitId].QueueTail] = GroupId;
            Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId =  GroupId;
        }
        else 
        {
            Adc_RuntimeHwUnits[HwUnitId].QueueTail = (++Adc_RuntimeHwUnits[HwUnitId].QueueTail == Adc_RuntimeHwUnits[HwUnitId].QueueMaxSize) ? 0 : Adc_RuntimeHwUnits[HwUnitId].QueueTail;
            Adc_RuntimeHwUnits[HwUnitId].QueueGroup[Adc_RuntimeHwUnits[HwUnitId].QueueTail] = GroupId;
        }
        return E_OK;
    }
    return E_NOT_OK;
}

/**
 * @brief Remove group from conversion queue
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
// TODO : Need to fix this and document this
Std_ReturnType AdcHw_RemoveGroupFromQueue(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    Adc_GroupType index = AdcHw_IsGroupInQueue(HwUnitId, GroupId);
    if(index != ADC_INVALID_GROUP_ID) 
    {
        if(index == Adc_RuntimeHwUnits[HwUnitId].QueueHead )
        {
            Adc_RuntimeHwUnits[HwUnitId].QueueGroup[index] = ADC_INVALID_GROUP_ID;
            Adc_RuntimeHwUnits[HwUnitId].QueueHead = (++index == Adc_RuntimeHwUnits[HwUnitId].QueueMaxSize) ? 0 : index;
            Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = Adc_RuntimeHwUnits[HwUnitId].QueueGroup[index] ;
        }
        else if(index ==  Adc_RuntimeHwUnits[HwUnitId].QueueTail)
        {
            Adc_RuntimeHwUnits[HwUnitId].QueueGroup[index] = ADC_INVALID_GROUP_ID;
            Adc_RuntimeHwUnits[HwUnitId].QueueTail = (--index == 0) ? Adc_RuntimeHwUnits[HwUnitId].QueueMaxSize - 1 : index;
        }
        // 0 h 0 x t 0 0 0
        // t 0 0 0 h 0 x 0
        // 0 x t 0 0 0 0 h
        // index always < tail
        // if(index > head) if(tail > head) index -> tail
        //                  if(tail < head) index -> max, 0 -> tail
        // if(index < head) -> tail < head index->tail
        else
        {
            // if(index > Adc_RuntimeHwUnits[HwUnitId].QueueHead && Adc_RuntimeHwUnits[HwUnitId].QueueHead > Adc_RuntimeHwUnits[HwUnitId].QueueTail)
            if(index > Adc_RuntimeHwUnits[HwUnitId].QueueTail)
            {
                for(Adc_GroupType i = index; i < Adc_RuntimeHwUnits[HwUnitId].QueueMaxSize - 1; i++)
                {
                    Adc_RuntimeHwUnits[HwUnitId].QueueGroup[i] = Adc_RuntimeHwUnits[HwUnitId].QueueGroup[i + 1];
                }
                Adc_RuntimeHwUnits[HwUnitId].QueueGroup[Adc_RuntimeHwUnits[HwUnitId].QueueMaxSize - 1] = Adc_RuntimeHwUnits[HwUnitId].QueueGroup[0];
                
            }
            for (index = 0; index < Adc_RuntimeHwUnits[HwUnitId].QueueTail - 1; index++)
            {
                Adc_RuntimeHwUnits[HwUnitId].QueueGroup[index] = Adc_RuntimeHwUnits[HwUnitId].QueueGroup[index + 1 ];
            }
            Adc_RuntimeHwUnits[HwUnitId].QueueTail = index;
        }
        // Decrease number of group in queue
        Adc_RuntimeHwUnits[HwUnitId].QueueCount--;
    }
    return E_NOT_OK;
}

/**
 * @brief Get next group from queue
 * @param[in] HwUnitId ADC hardware unit ID
 * @return Group ID or ADC_INVALID_GROUP_ID if queue is empty
 */
Adc_GroupType AdcHw_GetNextGroupFromQueue(Adc_HwUnitType HwUnitId)
{
    Adc_GroupType ret = ADC_INVALID_GROUP_ID;
    if(Adc_RuntimeHwUnits[HwUnitId].QueueCount > 1) 
    {
        // Get the current group index
        Adc_GroupType id = Adc_RuntimeHwUnits[HwUnitId].QueueHead;

        // This is fix to remove using AdcHw_RemoveGroupFromQueue in interrupt handler
        // Clear the current group registered before go to next group
        Adc_RuntimeHwUnits[HwUnitId].QueueGroup[id] = ADC_INVALID_GROUP_ID;

        // Check if head is at the end of the queue
        if(++id == Adc_RuntimeHwUnits[HwUnitId].QueueMaxSize - 1)
            ret =  Adc_RuntimeHwUnits[HwUnitId].QueueGroup[0];
        else 
            ret = Adc_RuntimeHwUnits[HwUnitId].QueueGroup[id];
    }
    return ret;

}

/**
 * @brief Check if group is in queue
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return TRUE if in queue, FALSE otherwise
 */
Adc_GroupType AdcHw_IsGroupInQueue(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    if(Adc_RuntimeHwUnits[HwUnitId].QueueHead > Adc_RuntimeHwUnits[HwUnitId].QueueTail)
    {
        for(Adc_GroupType i = Adc_RuntimeHwUnits[HwUnitId].QueueHead; i < Adc_RuntimeHwUnits[HwUnitId].QueueMaxSize;i++ )
        {
            if(Adc_RuntimeHwUnits[HwUnitId].QueueGroup[i] == GroupId) return i;
        }        
        for(Adc_GroupType i = 0; i <= Adc_RuntimeHwUnits[HwUnitId].QueueTail;i++ )
        {
            if(Adc_RuntimeHwUnits[HwUnitId].QueueGroup[i] == GroupId) return i;
        }
    }
    else
    {
        for(Adc_GroupType i = Adc_RuntimeHwUnits[HwUnitId].QueueHead; i <= Adc_RuntimeHwUnits[HwUnitId].QueueTail;i++ )
        {
            if(Adc_RuntimeHwUnits[HwUnitId].QueueGroup[i] == GroupId) return i;
        }
    }
    return ADC_INVALID_GROUP_ID;
}

/**
 * @brief Clear conversion queue
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_ClearQueue(Adc_HwUnitType HwUnitId)
{
    return AdcHw_ResetHwRuntime(HwUnitId);
}
#endif /* ADC_ENABLE_QUEUING */

/****************************************************************************************
*                                 DEFERRED PROCESSING FUNCTIONS                       *
****************************************************************************************/
// TODO : FOCUS Remove this function
#if (ADC_ENABLE_QUEUING == STD_ON)
/**
 * @brief Main function for deferred processing
 * @return void
 */
void AdcHw_MainFunction(void)
{
    /* Process all pending units */
    for (uint8 i = 0; i < AdcHw_PendingCount; i++)
    {
        Adc_HwUnitType HwUnitId = AdcHw_PendingUnits[i];
        
        if (AdcHw_DeferredProcessingFlag[HwUnitId])
        {
            /* Clear flag atomically */
            __disable_irq();
            AdcHw_DeferredProcessingFlag[HwUnitId] = 0;
            __enable_irq();
            
            /* Process completed conversions */
            AdcHw_ProcessCompletedConversions(HwUnitId);
        }
    }
    
    /* Reset pending count */
    AdcHw_PendingCount = 0;
}

/**
 * @brief Process completed conversions
 * @param[in] HwUnitId ADC hardware unit ID
 * @return void
 */
void AdcHw_ProcessCompletedConversions(Adc_HwUnitType HwUnitId)
{
    /* Get current group */
    Adc_GroupType CurrentGroup = Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId;
    if (CurrentGroup == ADC_INVALID_GROUP_ID)
    {
        return;
    }
    Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = ADC_INVALID_GROUP_ID;
    if(AdcHw_GetHwUnitState(HwUnitId) == HW_STATE_HW)
    {
        AdcHw_RecallSwConversion(HwUnitId);
    }
    else 
    {
        Adc_GroupType NextGroup = AdcHw_GetNextGroupFromQueue(HwUnitId);
        if (NextGroup != ADC_INVALID_GROUP_ID)
        {
            Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = CurrentGroup;
            AdcHw_StartSwConversion(HwUnitId, NextGroup);
        }
        else 
        {
            AdcHw_ResetHwRuntime(HwUnitId);             
        }
    }
    
}
#endif


/****************************************************************************************
*                                 VALIDATION FUNCTIONS                                *
****************************************************************************************/
/**
 * @brief Validate hardware unit ID
 * @param[in] HwUnitId ADC hardware unit ID
 * @return TRUE if valid, FALSE if invalid
 */
boolean AdcHw_ValidateHwUnit(Adc_HwUnitType HwUnitId)
{
    return ADC_HW_IS_VALID_UNIT(HwUnitId);
}


/**
 * @brief Validate channel ID
 * @param[in] ChannelId ADC channel ID
 * @return TRUE if valid, FALSE if invalid
 */
boolean AdcHw_ValidateChannel(Adc_ChannelType ChannelId)
{
    return ADC_HW_IS_VALID_CHANNEL(ChannelId);
}

/****************************************************************************************
*                                 CONFIGURE FUNCTIONS                                  *
****************************************************************************************/
/**
 * @brief Configure default hardware module
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
// todo fix included function
static inline Std_ReturnType AdcHw_ConfigureHwModule(Adc_HwUnitType HwUnitId)
{
    /* Get ADC hardware module */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    if(ADCx == NULL_PTR)
    {
        return E_NOT_OK;
    }
    
    /* Initialize ADC structure */
    ADC_InitTypeDef adc;
    adc.ADC_Mode = ADC_Mode_Independent;
    // TODO check if number of channel is one disable scan mode
    adc.ADC_ScanConvMode = DISABLE;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adc.ADC_DataAlign = ADC_DataAlign_Right ;
    adc.ADC_NbrOfChannel = 1;
    
    ADC_Init(ADCx, &adc);
    
    /* Enable ADC */
    // TODO option not enable 
    // ADC_Cmd(ADCx, ENABLE);
    
    return E_OK;
}

/**
 * @brief Configure hardware module followed group requirement
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
static inline Std_ReturnType AdcHw_ConfigureHwModuleGroup(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    // add checks
    Std_ReturnType ret = E_NOT_OK;
    #if(ADC_ENABLE_DMA == STD_ON)  
    if (ADC_DMA_AVAILABLE == Adc_HwUnitConfig[HwUnitId].AdcHw_DMAAvailable)
    {
        /* Get ADC hardware module */
        ret = AdcHw_ConfigureHwModuleGroupDMA(HwUnitId, GroupId);
    }
    else  ret = AdcHw_ConfigureHwModuleGroupIT(HwUnitId, GroupId);
    #else 
    ret = AdcHw_ConfigureHwModuleGroupIT(HwUnitId, GroupId);
    #endif
    return ret;
}
#if(ADC_ENABLE_DMA == STD_ON)  
static inline Std_ReturnType AdcHw_ConfigureHwModuleGroupDMA(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    /* Get ADC hardware module */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[GroupId];
    // const Adc_HwUnitDefType* HwUnitConfig = &Adc_HwUnitConfig[HwUnitId];
    if (ADCx == NULL_PTR)
    {
        return E_NOT_OK;
    }
    ADC_InitTypeDef adc;
    adc.ADC_NbrOfChannel = GroupConfig->Adc_NbrOfChannel;              // Number of channels to be converted
    adc.ADC_ScanConvMode = (GroupConfig->Adc_NbrOfChannel == 1) ? DISABLE : ENABLE;       // Multi channel conversion
    if(GroupConfig->Adc_GroupConvMode == ADC_CONV_MODE_CONTINUOUS || \
        GroupConfig->Adc_GroupAccessMode == ADC_ACCESS_MODE_STREAMING)
    {
        adc.ADC_ContinuousConvMode = ENABLE;
    }
    else 
    {
        adc.ADC_ContinuousConvMode = DISABLE;    //Continuous mode
    }
    if(GroupConfig->Adc_TriggerSource == ADC_TRIGG_SRC_SW)
    {
        adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // No external trigger
    }
    else{
        //do nothing now
        adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; // No external trigger
    }
    adc.ADC_DataAlign = (GroupConfig->Adc_ResultAlignment == ADC_ALIGN_RIGHT) ? ADC_DataAlign_Right : ADC_DataAlign_Left; // Right alignment
    ADC_Init(ADCx, &adc);
    
    /* Enable ADC */
    // TODO option not enable 
    // ADC_Cmd(ADCx, ENABLE);
    
    return E_OK;
}
#endif
static inline Std_ReturnType AdcHw_ConfigureHwModuleGroupIT( Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    /* Get ADC hardware module */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[GroupId];
    // need refix this
    // const Adc_HwUnitDefType* HwUnitConfig = &Adc_HwUnitConfig[HwUnitId];
   
    if (ADCx == NULL)
    {
        return E_NOT_OK;
    }
    ADC_InitTypeDef adc ;
    adc.ADC_ScanConvMode = DISABLE;
    adc.ADC_NbrOfChannel = 1 ;              // Number of channels to be converted
    if(GroupConfig->Adc_NbrOfChannel == 1 && \
       GroupConfig->Adc_GroupConvMode == ADC_CONV_MODE_ONESHOT && \
       GroupConfig->Adc_GroupAccessMode == ADC_ACCESS_MODE_SINGLE)
    {
        adc.ADC_ContinuousConvMode = DISABLE; // Oneshot mode
    }
    else 
    {
        adc.ADC_ContinuousConvMode = ENABLE; // Multi channel conversion
    }
    if(GroupConfig->Adc_TriggerSource == ADC_TRIGG_SRC_SW)
    {
        adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // No external trigger
    }
    // HW trigger
    else{
        //do nothing now
        adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; // No external trigger
    }
    adc.ADC_DataAlign = (GroupConfig->Adc_ResultAlignment == ADC_ALIGN_RIGHT) ? ADC_DataAlign_Right : ADC_DataAlign_Left; // Right alignment
    ADC_Init(ADCx, &adc);
    
    /* Enable ADC */
    // TODO option not enable 
    // ADC_Cmd(ADCx, ENABLE);
    
    return E_OK;
}

/**
 * @brief Configure clocks
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
static inline Std_ReturnType AdcHw_ConfigureClocks(Adc_HwUnitType HwUnitId)
{
    /* Enable ADC clock */
    if(HwUnitId == ADC_INSTANCE_1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        
    }
    else if(HwUnitId == ADC_INSTANCE_2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    }
    else
    {
        return E_NOT_OK;
    }
    
    /* Configure ADC clock prescaler */
    // RCC_ADCCLKConfig(ADC_CLOCK_PRESCALER);
    
    return E_OK;
}

/**
 * @brief Configure NVIC
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
static inline Std_ReturnType AdcHw_ConfigureNvic(Adc_HwUnitType HwUnitId)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    if (AdcHw_ValidateHwUnit(HwUnitId))
    {
        NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    }
    else
    {
        return E_NOT_OK;
    }
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ADC_EOC_INTERRUPT_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    return E_OK;
}

#if (ADC_ENABLE_DMA == STD_ON)
/**
 * @brief Initialize DMA for ADC
 * @param[in] HwUnitId ADC hardware unit ID  
 * @param[in] GroupId ADC group ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
inline Std_ReturnType AdcHw_InitDma(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // TODO: Add Check
    /* Get group configuration */
    Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[GroupId];

    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    DMA_Channel_TypeDef* DMAx = ADC_HW_GET_DMA_CHANNEL(HwUnitId);

    DMA_InitTypeDef dma;
    // Configure DMA for ADC1 and ADC2
    dma.DMA_PeripheralBaseAddr = (uint32)&ADCx->DR;
    dma.DMA_MemoryBaseAddr = (uint32)GroupConfig->Adc_ValueResultPtr;
    dma.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma.DMA_BufferSize = GroupConfig->Adc_ValueResultSize;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma.DMA_Mode = (GroupConfig->Adc_StreamBufferMode == ADC_STREAM_BUFFER_CIRCULAR) ? DMA_Mode_Circular : DMA_Mode_Normal;
    dma.DMA_Priority = DMA_Priority_High;
    dma.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMAx, &dma);
    DMA_Cmd(DMAx, ENABLE);

    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = ADC_DMA_INTERRUPT_PRIORITY;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    return E_OK;

}
/**
 * @brief
 * 
 * @param[in]
 * @param[in]
 * @return
 */
inline Std_ReturnType AdcHw_DeInitDma(Adc_HwUnitType HwUnitId)
{
    // TODO: Add Check 
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
    DMA_Channel_TypeDef* DMAx = ADC_HW_GET_DMA_CHANNEL(HwUnitId);
    DMA_Cmd(DMAx, DISABLE);
    ADC_DMACmd(ADCx, DISABLE);
    DMA_DeInit(DMAx);
    NVIC_DisableIRQ(DMA1_Channel1_IRQn);
    
    return E_OK;
}
#endif

/****************************************************************************************
*                      HANDLE COMPLETE CONVERSION FUNCTIONS                             *
****************************************************************************************/
/**
 * @brief Handle channel sequencing for interrupt not recommend 
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return void
 * @note : For interrupt handler only
 * @note : this is my try to implement it conversion for multi group instead of using dma
 * work flow
 * -> check if all channel is conversion
 * -> check if is it the last sample (streaming access)
 * -> call for notification
 * -> 
 */
// TODO : it seem to have problem here
static void AdcHw_HandleChannelSequencing(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    /* Get group configuration */
    Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[GroupId];

    
    /* Check if more channels in current sample */
    if (Adc_RuntimeGroups[GroupId].CurrentChannelId < (GroupConfig->Adc_NbrOfChannel - 1))
    {
        /* More channels to convert */
        Adc_RuntimeGroups[GroupId].CurrentChannelId++;
        
        /* Update buffer index */
        Adc_RuntimeGroups[GroupId].BufferIndex++;
        AdcHw_StartNextConversion(HwUnitId, GroupId);
    }
    /* Finish 1 sample*/
    else
    {
        /* All channels done for this sample */
        Adc_RuntimeGroups[GroupId].SampleCounter++;
        /* Check if more samples needed */
        // Finish streaming last sample
        if (Adc_RuntimeGroups[GroupId].SampleCounter >= GroupConfig->Adc_StreamNumSamples)
        {

            AdcHw_SetGroupStatus(GroupId, ADC_STREAM_COMPLETED);
            // this is to make sure it is called right after set and not get stuck while the state it still ADC_BUSY
            AdcHw_CallNotification(GroupId);
            // If stream circular restart the conversion from beginning
            // If continuous mode restart 
            if(GroupConfig->Adc_GroupConvMode == ADC_CONV_MODE_CONTINUOUS )
            {
                /* Handle circular buffer wrapping */
                /* Reset buffer index and sample counter */
                Adc_RuntimeGroups[GroupId].BufferIndex = 0;
                Adc_RuntimeGroups[GroupId].SampleCounter = 0;
                Adc_RuntimeGroups[GroupId].CurrentChannelId = 0;

                /* Continue conversion */
                AdcHw_StartNextConversion(HwUnitId, GroupId);
            }          
            // other go to next state   
            else
            {
                ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);
                AdcHw_DisableInterrupt(HwUnitId, ADC_INTERRUPT_EOC);
                /* Disable ADC */
                ADC_SoftwareStartConvCmd(ADCx, DISABLE);
                ADC_Cmd(ADCx, DISABLE);
               
                #if (ADC_ENABLE_QUEUING == STD_ON)
                    /* All samples completed */
                    /* Tell the adc main function to do next task */
                    AdcHw_DeferredProcessingFlag[HwUnitId] = 1;
                    if (AdcHw_PendingCount < ADC_MAX_HW_UNITS)
                    {
                        AdcHw_PendingUnits[AdcHw_PendingCount] = HwUnitId;
                        AdcHw_PendingCount++;
                    }
                #else 
                    Adc_RuntimeHwUnits[HwUnitId].CurrentGroupId = ADC_INVALID_GROUP_ID;
                    Adc_RuntimeHwUnits[HwUnitId].HwUnitState = HW_STATE_IDLE;
                #endif
            }

        }
        else
        {
            // Still have sample need to do
            /* Start next sample */
            /* Set status to completed after one conversion done*/
            if(Adc_RuntimeGroups[GroupId].Status == ADC_BUSY)
            {
                AdcHw_SetGroupStatus(GroupId, ADC_COMPLETED);
            }
            // this is to make sure that it only call when state is not busy
            AdcHw_CallNotification(GroupId);
            Adc_RuntimeGroups[GroupId].BufferIndex++;
            Adc_RuntimeGroups[GroupId].CurrentChannelId = (++Adc_RuntimeGroups[GroupId].CurrentChannelId == GroupConfig->Adc_NbrOfChannel) ? 0 : Adc_RuntimeGroups[GroupId].CurrentChannelId;
            AdcHw_StartNextConversion(HwUnitId, GroupId);
        }
        
    }
}


/**
 * @brief Start next conversion of the group callfrom intterupt sequence
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return void
 * @note : For interrupt handler only
 * @note my try to simulate dma using interrupt only 
 */
static void AdcHw_StartNextConversion(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId)
{
    /* Get ADC hardware module */
    ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnitId);    
    /* Get group configuration */
    Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[GroupId];

    /* Configure next channel */
    const Adc_ChannelDefType* ChannelConfig = &GroupConfig->Adc_ChannelGroup[Adc_RuntimeGroups[GroupId].CurrentChannelId];
    ADC_RegularChannelConfig(ADCx, ChannelConfig->Adc_ChannelId, 1, ChannelConfig->Adc_ChannelSampTime);
    /* Start conversion */
    ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}


/**
 * @brief Call notification callback
 * @param[in] GroupId ADC group ID
 * @return void
 */
static void AdcHw_CallNotification(Adc_GroupType GroupId)
{
    /* Check if notification is enabled and callback is configured */
    if ((Adc_GroupConfig[GroupId].Adc_NotificationEnable == ADC_NOTIFICATION_ENABLE) &&
        (Adc_GroupConfig[GroupId].Adc_NotificationCb != NULL_PTR))
    {
        /* Call notification callback */
        Adc_GroupConfig[GroupId].Adc_NotificationCb();
    }
}

/**
 * @brief Get current time for performance monitoring
 * @return Current time in microseconds
 */
static uint32 AdcHw_GetCurrentTime(void)
{
    /* Implementation depends on system timer */
    /* This is a placeholder - should be implemented based on system requirements */
    return 0;
}

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
