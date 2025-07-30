 /****************************************************************************************
*                                ADC_CFG.C                                              *
****************************************************************************************
* File Name   : Adc_Cfg.c
* Module      : Analog to Digital Converter (ADC)
* Description : AUTOSAR ADC driver configuration source file
* Version     : 3.1.0 - Redesigned for clarity and maintainability
* Date        : 24/06/2025
* Update      : 23/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Adc_Cfg.h"
#include "Adc_Hw.h"
#include "Adc_Types.h"

/****************************************************************************************
*                                 CHANNEL CONFIGURATIONS                               *
****************************************************************************************/
/* Channel configuration for Group 1 */
static const Adc_ChannelDefType Adc_ChannelGroup1[] = 
{
    {
        .Adc_ChannelId = 0,                          /* PA0 - ADC1_IN0 */
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    {
        .Adc_ChannelId = 1,                          /* PA1 - ADC1_IN1 */
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    }
};
#define ADC_CHANNEL_GROUP_1_SIZE sizeof(Adc_ChannelGroup1)/sizeof(Adc_ChannelDefType)


/* Channel configuration for Group 2 */
static const Adc_ChannelDefType Adc_ChannelGroup2[] = 
{
    {
        .Adc_ChannelId = 1,                          /* PA2 - ADC1_IN2 */
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    {
        .Adc_ChannelId = 0,                          /* PA3 - ADC1_IN3 */
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    }
};
#define ADC_CHANNEL_GROUP_2_SIZE sizeof(Adc_ChannelGroup2)/sizeof(Adc_ChannelDefType)

/* Channel configuration for Group 3 (streaming example) */
static const Adc_ChannelDefType Adc_ChannelGroup3[4] = 
{
    {
        .Adc_ChannelId = 5,                          /* PA5 - ADC1_IN5 */
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    {
        .Adc_ChannelId = 6,                          /* PA6 - ADC1_IN6 */
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    {
        .Adc_ChannelId = 7,                          /* PA7 - ADC1_IN7 */
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    {
        .Adc_ChannelId = 8,                          /* PB0 - ADC1_IN8 */
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    }
};
#define ADC_CHANNEL_GROUP_3_SIZE sizeof(Adc_ChannelGroup3)/sizeof(Adc_ChannelDefType)

/****************************************************************************************
*                                 RESULT BUFFER CONFIGURATIONS                         *
****************************************************************************************/
/* Result buffers for each group */
static Adc_ValueGroupType Adc_Group1_ResultBuffer[2 * 4];          /* 2 channels, 1 sample */
static Adc_ValueGroupType Adc_Group2_ResultBuffer[2 * 3];          /* 3 channels, 1 sample */
static Adc_ValueGroupType Adc_Group3_ResultBuffer[4 * 5];         /* 4 channels, 16 samples */

/****************************************************************************************
*                                 NOTIFICATION CALLBACKS                               *
****************************************************************************************/
/**
 * @brief Notification callback for Group 1
 * @return void
 */
__attribute__((weak)) void Adc_Group1_Notification(void)  
{
    /* User-defined notification handling for Group 1 */
    /* This can be used to signal completion to application */
}

// /**
//  * @brief Notification callback for Group 2
//  * @return void
//  */
__attribute__((weak)) void Adc_Group2_Notification(void) 
{
    /* User-defined notification handling for Group 2 */
    /* This can be used to signal completion to application */
}

// /**
//  * @brief Notification callback for Group 3
//  * @return void
//  */
__attribute__((weak))  void Adc_Group3_Notification(void) 
{
    /* User-defined notification handling for Group 3 */
    /* This can be used to signal completion to application */
}
/****************************************************************************************
*                                 STREAMING CALLBACKS                                 *
****************************************************************************************/
/**
 * @brief Streaming threshold callback
 * @param[in] GroupId Group ID
 * @param[in] ThresholdLevel Threshold level (0-100%)
 * @return void
 */
void Adc_StreamingThreshold_Callback(Adc_GroupType GroupId, uint8 ThresholdLevel)
{
    /* Handle streaming threshold reached */
    /* This can be used to process partial results */
}

/**
 * @brief Buffer overflow callback
 * @param[in] GroupId Group ID
 * @return void
 */
void Adc_BufferOverflow_Callback(Adc_GroupType GroupId)
{
    /* Handle buffer overflow */
    /* This can be used for error recovery */
}

/****************************************************************************************
*                                 SAFETY CALLBACKS                                    *
****************************************************************************************/
/**
 * @brief Conversion timeout callback
 * @param[in] GroupId Group ID
 * @return void
 */
void Adc_ConversionTimeout_Callback(Adc_GroupType GroupId)
{
    /* Handle conversion timeout */
    /* This can be used for error recovery */
}

/**
 * @brief Watchdog timeout callback
 * @param[in] GroupId Group ID
 * @return void
 */
void Adc_WatchdogTimeout_Callback(Adc_GroupType GroupId)
{
    /* Handle watchdog timeout */
    /* This can be used for error recovery */
}

/****************************************************************************************
*                                 GROUP CONFIGURATIONS                                 *
****************************************************************************************/
Adc_GroupDefType Adc_GroupConfig[ADC_MAX_GROUPS] = 
{
    /* Group 1: Single access, one-shot, software trigger */
    {
        .Adc_HwUnitId = ADC_INSTANCE_1,                               /* ADC Hardware Unit 0 (ADC1) */
        .Adc_GroupId = 0,                                /* Group 0 (internally Group 1) */
        .Adc_GroupPriority = 1,                          /* Highest priority */
        .Adc_GroupAccessMode = ADC_ACCESS_MODE_SINGLE,
        .Adc_ValueResultSize = 2,
        .Adc_StreamNumSamples = 1,     

        // .Adc_GroupAccessMode = ADC_ACCESS_MODE_STREAMING,
        // .Adc_StreamNumSamples = 4, 
        // .Adc_ValueResultSize = 8,    
        
        .Adc_GroupConvMode = ADC_CONV_MODE_ONESHOT,
        // .Adc_GroupConvMode = ADC_CONV_MODE_CONTINUOUS,
        .Adc_GroupReplacement = ADC_GROUP_REPL_ABORT_RESTART,
        .Adc_Status = ADC_IDLE,
        .Adc_ResultAlignment = ADC_ALIGN_RIGHT,
        .Adc_ChannelGroup = (Adc_ChannelDefType*)Adc_ChannelGroup1,
        .Adc_NbrOfChannel = 2,                           /* 2 channels: PA0, PA1 */
        .Adc_TriggerSource = ADC_TRIGG_SRC_SW,
        .Adc_HwTriggerSignal = ADC_HW_TRIG_RISING_EDGE,
        .Adc_HwTriggerTimer = 0,
        // .Adc_StreamBufferMode = ADC_STREAM_BUFFER_LINEAR,
        .Adc_StreamBufferMode = ADC_STREAM_BUFFER_CIRCULAR,
                  /* 4 samples per channel */
        
        .Adc_ValueResultPtr = Adc_Group1_ResultBuffer,
                     /* 2 channels × 4 samples = 8 */
        .Adc_SetupBufferFlag = 0,
        .Adc_NotificationCb = Adc_Group1_Notification,
        .Adc_NotificationEnable = ADC_NOTIFICATION_DISABLE,
        .Adc_InterruptType = ADC_HW_DMA
    },
    
    /* Group 2: Single access, continuous, software trigger */
    {
        .Adc_HwUnitId = ADC_INSTANCE_1,                               /* ADC Hardware Unit 0 (ADC1) */
        .Adc_GroupId = 1,                                /* Group 1 (internally Group 2) */
        .Adc_GroupPriority = 2,                          /* Medium priority */
        .Adc_GroupAccessMode = ADC_ACCESS_MODE_STREAMING,
        .Adc_GroupConvMode = ADC_CONV_MODE_CONTINUOUS,
        .Adc_GroupReplacement = ADC_GROUP_REPL_ABORT_RESTART,
        .Adc_Status = ADC_IDLE,
        .Adc_ResultAlignment = ADC_ALIGN_RIGHT,
        .Adc_ChannelGroup = (Adc_ChannelDefType*)Adc_ChannelGroup1,
        .Adc_NbrOfChannel = 2,    /* 2 channels */
        .Adc_TriggerSource = ADC_TRIGG_SRC_SW,
        .Adc_HwTriggerSignal = ADC_HW_TRIG_RISING_EDGE,
        .Adc_HwTriggerTimer = 0,
        .Adc_StreamBufferMode = ADC_STREAM_BUFFER_LINEAR,
        .Adc_StreamNumSamples = 4,
        .Adc_ValueResultPtr = Adc_Group1_ResultBuffer,
        .Adc_ValueResultSize = 8,                        /* 3 channels × 2 samples = 6 */
        .Adc_SetupBufferFlag = 0,
        .Adc_NotificationCb = Adc_Group1_Notification,
        .Adc_NotificationEnable = ADC_NOTIFICATION_DISABLE,
        .Adc_InterruptType = ADC_HW_DMA
    },
    
    /* Group 3: Streaming access, one-shot, software trigger */
    {
        .Adc_HwUnitId = ADC_INSTANCE_1,                               /* ADC Hardware Unit 0 (ADC1) */
        .Adc_GroupId = 2,                                /* Group 2 (internally Group 3) */
        .Adc_GroupPriority = 3,                          /* Low priority */
        .Adc_GroupAccessMode = ADC_ACCESS_MODE_STREAMING,
        .Adc_GroupConvMode = ADC_CONV_MODE_ONESHOT,
        .Adc_GroupReplacement = ADC_GROUP_REPL_SUSPEND_RESUME,
        .Adc_Status = ADC_IDLE,
        .Adc_ResultAlignment = ADC_ALIGN_RIGHT,
        .Adc_ChannelGroup = (Adc_ChannelDefType*)Adc_ChannelGroup3,
        .Adc_NbrOfChannel = ADC_CHANNEL_GROUP_3_SIZE,    /* 4 channels */
        .Adc_TriggerSource = ADC_TRIGG_SRC_SW,
        .Adc_HwTriggerSignal = ADC_HW_TRIG_RISING_EDGE,
        .Adc_HwTriggerTimer = 0,
        .Adc_StreamBufferMode = ADC_STREAM_BUFFER_CIRCULAR,
        .Adc_StreamNumSamples = 5,
        .Adc_ValueResultPtr = Adc_Group3_ResultBuffer,
        .Adc_ValueResultSize = 20,                       /* 4 channels × 5 samples = 20 */
        .Adc_SetupBufferFlag = 0,
        .Adc_NotificationCb = Adc_Group3_Notification,
        .Adc_NotificationEnable = ADC_NOTIFICATION_ENABLE,
        .Adc_InterruptType = ADC_HW_DMA
    },
    
    /* Additional groups can be added here */
    /* Group 4: Reserved for future use */
    {
        .Adc_HwUnitId = ADC_INSTANCE_2,                               /* ADC Hardware Unit 1 (ADC2) */
        .Adc_GroupId = 3,                                /* Group 3 (internally Group 4) */
        .Adc_GroupPriority = 4,
        .Adc_GroupAccessMode = ADC_ACCESS_MODE_SINGLE,
        .Adc_GroupConvMode = ADC_CONV_MODE_ONESHOT,
        .Adc_GroupReplacement = ADC_GROUP_REPL_ABORT_RESTART,
        .Adc_Status = ADC_IDLE,
        .Adc_ResultAlignment = ADC_ALIGN_RIGHT,
        .Adc_ChannelGroup = NULL_PTR,
        .Adc_NbrOfChannel = 0,
        .Adc_TriggerSource = ADC_TRIGG_SRC_SW,
        .Adc_HwTriggerSignal = ADC_HW_TRIG_RISING_EDGE,
        .Adc_HwTriggerTimer = 0,
        .Adc_StreamBufferMode = ADC_STREAM_BUFFER_LINEAR,
        .Adc_StreamNumSamples = 1,
        .Adc_ValueResultPtr = NULL_PTR,
        .Adc_ValueResultSize = 0,
        .Adc_SetupBufferFlag = 0,
        .Adc_NotificationCb = NULL_PTR,
        .Adc_NotificationEnable = ADC_NOTIFICATION_DISABLE
    },
    
    /* Group 5: Reserved for future use */
    {
        .Adc_HwUnitId = ADC_INSTANCE_2,                               /* ADC Hardware Unit 1 (ADC2) */
        .Adc_GroupId = 4,                                /* Group 4 (internally Group 5) */
        .Adc_GroupPriority = 5,
        .Adc_GroupAccessMode = ADC_ACCESS_MODE_SINGLE,
        .Adc_GroupConvMode = ADC_CONV_MODE_ONESHOT,
        .Adc_GroupReplacement = ADC_GROUP_REPL_ABORT_RESTART,
        .Adc_Status = ADC_IDLE,
        .Adc_ResultAlignment = ADC_ALIGN_RIGHT,
        .Adc_ChannelGroup = NULL_PTR,
        .Adc_NbrOfChannel = 0,
        .Adc_TriggerSource = ADC_TRIGG_SRC_SW,
        .Adc_HwTriggerSignal = ADC_HW_TRIG_RISING_EDGE,
        .Adc_HwTriggerTimer = 0,
        .Adc_StreamBufferMode = ADC_STREAM_BUFFER_LINEAR,
        .Adc_StreamNumSamples = 1,
        .Adc_ValueResultPtr = NULL_PTR,
        .Adc_ValueResultSize = 0,
        .Adc_SetupBufferFlag = 0,

        .Adc_NotificationCb = NULL_PTR,
        .Adc_NotificationEnable = ADC_NOTIFICATION_DISABLE
    },
    
    /* Remaining groups initialized to safe defaults */
    {0}, {0}, {0}, {0}, {0}  /* Groups 6-10 */
};

/****************************************************************************************
*                                 HARDWARE UNIT CONFIGURATIONS                         *
****************************************************************************************/

Adc_HwUnitDefType Adc_HwUnitConfig[ADC_MAX_HW_UNITS] = 
{
    /* ADC Hardware Unit 0 (ADC1) */
    {
        .AdcHW_UnitId = 0,                               /* Hardware Unit 0 corresponds to ADC1 */
        //.AdcHW_Prescale = ADC_CLOCK_PRESCALER,       /* Clock prescaler */
        .AdcHw_QueueEnable = ADC_ENABLE_QUEUING,         /* Queue enabled */
        .AdcHw_PriorityEnable = ADC_PRIORITY_IMPLEMENTATION,
        // .AdcHw_DMAAvailable = ADC_DMA_AVAILABLE,         /* DMA available */
        // Test
        .AdcHw_DMAAvailable = ADC_DMA_AVAILABLE,     /* DMA not available */
    },
    
    /* ADC Hardware Unit 1 (ADC2) */
    {
        .AdcHW_UnitId = 1,                               /* Hardware Unit 1 corresponds to ADC2 */
        //.AdcHW_Prescale = ADC_CLOCK_PRESCALER,       /* Clock prescaler */
        .AdcHw_QueueEnable = ADC_ENABLE_QUEUING,         /* Queue enabled */
        .AdcHw_PriorityEnable = ADC_PRIORITY_IMPLEMENTATION,
        .AdcHw_DMAAvailable = ADC_DMA_NOT_AVAILABLE,     /* DMA not available */
    }
};

/****************************************************************************************
*                                 CHANNEL CONFIGURATIONS                               *
****************************************************************************************/
Adc_ChannelDefType Adc_ChannelConfig[ADC_MAX_CHANNELS] = 
{
    /* Channel 0 - PA0 */
    {
        .Adc_ChannelId = 0,
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    
    /* Channel 1 - PA1 */
    {
        .Adc_ChannelId = 1,
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    
    /* Channel 2 - PA2 */
    {
        .Adc_ChannelId = 2,
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    
    /* Channel 3 - PA3 */
    {
        .Adc_ChannelId = 3,
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    
    /* Channel 4 - PA4 */
    {
        .Adc_ChannelId = 4,
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    
    /* Channel 5 - PA5 */
    {
        .Adc_ChannelId = 5,
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    
    /* Channel 6 - PA6 */
    {
        .Adc_ChannelId = 6,
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    
    /* Channel 7 - PA7 */
    {
        .Adc_ChannelId = 7,
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    
    /* Channel 8 - PB0 */
    {
        .Adc_ChannelId = 8,
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    
    /* Channel 9 - PB1 */
    {
        .Adc_ChannelId = 9,
        .Adc_ChannelSampTime = ADC_SAMPLING_TIME_DEFAULT
    },
    
    /* Remaining channels initialized to safe defaults */
    {10, ADC_SAMPLING_TIME_DEFAULT},
    {11, ADC_SAMPLING_TIME_DEFAULT},
    {12, ADC_SAMPLING_TIME_DEFAULT},
    {13, ADC_SAMPLING_TIME_DEFAULT},
    {14, ADC_SAMPLING_TIME_DEFAULT},
    {15, ADC_SAMPLING_TIME_DEFAULT}
};

/****************************************************************************************
*                                 MAIN CONFIGURATION                                   *
****************************************************************************************/
const Adc_ConfigType Adc_Config = 
{
    .HwUnits = Adc_HwUnitConfig,
    .NumHwUnits = ADC_MAX_HW_UNITS,
    .Groups = Adc_GroupConfig,
    .NumGroups = ADC_MAX_GROUPS
};

/****************************************************************************************
*                                 INTERRUPT SERVICE ROUTINES                          *
****************************************************************************************/
// /**
//  * @brief ADC1 and ADC2 interrupt service routine
//  * @return void
//  */
// void ADC1_2_IRQHandler(void)
// {
//     /* Check ADC1 */
//     if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET)
//     {
//         AdcHw_InterruptHandler(1);  /* Hardware Unit 0 */
//     }
    
//     /* Check ADC2 */
//     if (ADC_GetITStatus(ADC2, ADC_IT_EOC) != RESET)
//     {
//         AdcHw_InterruptHandler(2);  /* Hardware Unit 1 */
//     }
// }

// /**
//  * @brief DMA1 Channel 1 interrupt service routine
//  * @return void
//  */
// void DMA1_Channel1_IRQHandler(void)
// {
//     /* Check transfer complete */
//     if (DMA_GetITStatus(DMA1_IT_TC1) != RESET)
//     {
//         AdcHw_DmaInterruptHandler(0);  /* Hardware Unit 0 */
//         DMA_ClearITPendingBit(DMA1_IT_TC1);
//     }
    
//     /* Check transfer error */
//     if (DMA_GetITStatus(DMA1_IT_TE1) != RESET)
//     {
//         /* Handle DMA error */
//         DMA_ClearITPendingBit(DMA1_IT_TE1);
//     }
// }
/****************************************************************************************
*                                 HARDWARE EVENT CALLBACKS                            *
****************************************************************************************/
/**
 * @brief Transfer complete callback
 * @param[in] ADCx ADC hardware module
 * @return void
 */
void Adc_TransferComplete_Callback(ADC_TypeDef* ADCx)
{
    /* Determine hardware unit */
    Adc_HwUnitType HwUnit = (ADCx == ADC1) ? 0 : 1;  /* Unit 0 for ADC1, Unit 1 for ADC2 */
    
    /* Call hardware interrupt handler */
    AdcHw_InterruptHandler(ADCx,HwUnit);
}

/**
 * @brief DMA transfer complete callback
 * @param[in] DMAx_Channely DMA channel
 * @return void
 */
void Adc_DmaTransferComplete_Callback(DMA_Channel_TypeDef* DMAx_Channely)
{
    /* Determine hardware unit based on DMA channel */
    // Adc_HwUnitType HwUnit = (DMAx_Channely == DMA1_Channel1) ? 0 : 1;  /* Unit 0 for ADC1, Unit 1 for ADC2 */
    Adc_HwUnitType HwUnit = 0;  /* ADC1 uses Hardware Unit 0 */
    /* Call DMA interrupt handler */
    AdcHw_DmaInterruptHandler(DMAx_Channely, HwUnit);
}

/**
 * @brief Error callback
 * @param[in] ADCx ADC hardware module
 * @param[in] ErrorCode Error code
 * @return void
 */
void Adc_Error_Callback(ADC_TypeDef* ADCx, uint32 ErrorCode)
{
    /* Handle ADC errors */
    /* This can be used for error logging and recovery */
}
/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
