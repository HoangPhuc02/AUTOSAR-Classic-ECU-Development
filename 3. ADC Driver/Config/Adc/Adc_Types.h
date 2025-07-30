/****************************************************************************************
*                                ADC_TYPES.H                                          *
****************************************************************************************
* File Name   : Adc_Types.h
* Module      : Analog to Digital Converter (ADC)
* Description : AUTOSAR ADC driver type definitions header file 
* Version     : 3.1.0 - Redesigned for clarity and maintainability
* Date        : 24/06/2025
* Update      : 23/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

#ifndef ADC_TYPES_H
#define ADC_TYPES_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
/* Standard AUTOSAR includes */
#include "Std_Types.h"
#include "stm32f10x_adc.h"
#include "stm32f10x.h"
/****************************************************************************************
*                                 FEATURE MACROS                                       *
****************************************************************************************/


#define ADC_PRIORITY_IMPLEMENTATION 0

/****************************************************************************************
*                              BASIC NUMERIC TYPES                                     *
****************************************************************************************/
/**
 * @brief   Adc_ChannelType
 * @typedef uint8
 * @details Numeric ID of an ADC Channel
 * @reqs    SWS_Adc_00506
 */
typedef uint8 Adc_ChannelType;

/**
 * @brief   Adc_GroupType
 * @typedef uint8
 * @details Numeric ID of an ADC Channel Group
 * @reqs    SWS_Adc_00507
 */
typedef uint8 Adc_GroupType;    

/**
 * @brief   Adc_ValueGroupType
 * @typedef uint16
 * @details Type for reading the converted values of a channel group (raw, without 
 *          further scaling, alignment according to ADC_RESULT_ALIGNMENT).
 * @note    STM32F10x ADC supports up to 12-bit resolution so 16 bits is sufficient.
 * @reqs    SWS_Adc_00508
 */
typedef uint16 Adc_ValueGroupType;

/**
 * @brief  Adc_GroupPriorityType
 * @typedef uint8
 * @details Priority level of the channel. Lowest priority is 0.
 * @reqs   SWS_Adc_00516
 */
typedef uint8 Adc_GroupPriorityType;

/**
 * @brief   Adc_StreamNumSampleType
 * @typedef uint8
 * @details Type for the number of group conversions in streaming access mode
 * @note    In single access mode, parameter is 1
 * @reqs    SWS_Adc_00518
 */
typedef uint8 Adc_StreamNumSampleType;

/**
 * @brief   Adc_HwUnitType
 * @typedef uint8
 * @details Type for the ID of the ADC HW Unit
 */
typedef uint8 Adc_HwUnitType;

/****************************************************************************************
*                              TIMING AND CLOCK TYPES                                  *
****************************************************************************************/
/**
 * @brief   Adc_PrescaleType
 * @typedef uint8
 * @details ADC prescaler value for clock division
 * @reqs    SWS_Adc_00509
 */
typedef uint8 Adc_PrescaleType;

/**
 * @brief   Adc_ResolutionType
 * @typedef uint8
 * @details Channel resolution in number of bits
 * @reqs    SWS_Adc_00512
 */
typedef uint8 Adc_ResolutionType;

/**
 * @brief   Adc_ConversionTimeType
 * @typedef uint32
 * @details ADC conversion time in microseconds or clock cycles
 * @reqs    SWS_Adc_00510
 */
typedef uint32 Adc_ConversionTimeType;

/** 
 * @brief   Adc_SamplingTimeType
 * @typedef uint32
 * @details ADC sampling time in clock cycles
 * @reqs    SWS_Adc_00511
 */
typedef uint32 Adc_SamplingTimeType;

/** 
 * @brief   Adc_HwTriggerTimerType
 * @typedef uint32
 * @details Reload value for ADC embedded timer
 */
typedef uint32 Adc_HwTriggerTimerType;

/****************************************************************************************
*                              CONVERSION MODE ENUMS                                   *
****************************************************************************************/
/**
 * @brief   Adc_TriggerSourceType
 * @typedef enum
 * @details Trigger source configuration for ADC Channel Group
 * @reqs    SWS_Adc_00514
 */
typedef enum
{
    ADC_TRIGG_SRC_SW = 0x00U,        /*!< Software trigger by API Call */
    ADC_TRIGG_SRC_HW = 0x01U,        /*!< Hardware trigger by HW event */
} Adc_TriggerSourceType;

/**
 * @brief   Adc_GroupConvModeType
 * @typedef enum
 * @details Conversion mode for ADC groups
 * @reqs    SWS_Adc_00515
 */
typedef enum
{
    ADC_CONV_MODE_ONESHOT = 0x00U,        /*!< One-shot conversion mode */
    ADC_CONV_MODE_CONTINUOUS = 0x01U,     /*!< Continuous conversion mode */
} Adc_GroupConvModeType;

/**
 * @brief   Adc_GroupAccessModeType
 * @typedef enum
 * @details Access mode to group conversion results
 * @reqs    SWS_Adc_00528
 */
typedef enum
{
    ADC_ACCESS_MODE_SINGLE = 0x00U,     /*!< Single access mode */
    ADC_ACCESS_MODE_STREAMING = 0x01U   /*!< Streaming access mode */
} Adc_GroupAccessModeType;

/**
 * @brief   Adc_HwTriggerSignalType
 * @typedef enum
 * @details Hardware trigger edge detection configuration
 * @reqs    SWS_Adc_00520
 */
typedef enum 
{
    ADC_HW_TRIG_RISING_EDGE = 0x00U,    /*!< Hardware trigger rising edge */
    ADC_HW_TRIG_FALLING_EDGE = 0x01U,   /*!< Hardware trigger falling edge */
    ADC_HW_TRIG_BOTH_EDGES = 0x02U      /*!< Hardware trigger both edges */
} Adc_HwTriggerSignalType;

/****************************************************************************************
*                              BUFFER AND STREAM ENUMS                                 *
****************************************************************************************/
/**
 * @brief   Adc_StreamBufferModeType
 * @typedef enum
 * @details Streaming access mode buffer type
 * @reqs    SWS_Adc_00519
 */
typedef enum 
{
    ADC_STREAM_BUFFER_LINEAR = 0x00U,    /*!< Linear buffer - stops when full */
    ADC_STREAM_BUFFER_CIRCULAR = 0x01U   /*!< Circular buffer - wraps around */
} Adc_StreamBufferModeType;

/**
 * @brief   Adc_GroupReplacementType
 * @typedef enum
 * @details Group replacement mechanism for priority interruption
 * @reqs    SWS_Adc_00523
 */
typedef enum 
{
    ADC_GROUP_REPL_ABORT_RESTART = 0x00U,   /*!< Abort and restart conversion */
    ADC_GROUP_REPL_SUSPEND_RESUME = 0x01U,  /*!< Suspend and resume conversion */
} Adc_GroupReplacementType;

/**
 * @brief   Adc_ResultAlignmentType
 * @typedef enum
 * @details ADC result alignment in result buffer
 * @reqs    SWS_Adc_00525
 */
typedef enum 
{
    ADC_ALIGN_LEFT = 0x00U,     /*!< Left alignment */
    ADC_ALIGN_RIGHT = 0x01U     /*!< Right alignment */
} Adc_ResultAlignmentType;

/****************************************************************************************
*                              STATUS ENUMS                                            *
****************************************************************************************/
/**
 * @brief   Adc_StatusType
 * @typedef enum
 * @details Current status of ADC channel group conversion
 * @reqs    SWS_Adc_00513   
 */
typedef enum
{
    ADC_IDLE = 0x00U,               /*!< Group not started, no result available */
    ADC_BUSY = 0x01U,               /*!< Group conversion ongoing, no result yet */
    ADC_COMPLETED = 0x02U,          /*!< Conversion round completed, result available */
    ADC_STREAM_COMPLETED = 0x03U    /*!< Streaming buffer completely filled */
} Adc_StatusType;

/****************************************************************************************
*                              PRIORITY ENUMS                                          *
****************************************************************************************/
/**
 * @brief   Adc_PriorityImplementationType
 * @typedef enum
 * @details Priority mechanism configuration
 * @reqs    SWS_Adc_00522
 */
typedef enum 
{
    ADC_PRIORITY_NONE = 0x00U,      /*!< No priority mechanism */
    ADC_PRIORITY_HW = 0x01U,        /*!< Hardware priority only */
    ADC_PRIORITY_HW_SW = 0x02U      /*!< Hardware and software priority */
} Adc_PriorityImplementationType;

/****************************************************************************************
*                              LIMIT CHECKING ENUMS                                    *
****************************************************************************************/
/**
 * @brief   Adc_ChannelRangeSelectType
 * @typedef enum
 * @details Valid conversion value range configuration for limit checking
 * @reqs    SWS_Adc_00524
 */
typedef enum 
{
    ADC_RANGE_UNDER_LOW = 0x00U,     /*!< Range below low limit */
    ADC_RANGE_BETWEEN = 0x01U,       /*!< Range between limits */
    ADC_RANGE_OVER_HIGH = 0x02U,     /*!< Range above high limit */
    ADC_RANGE_ALWAYS = 0x03U,        /*!< Complete range - no limits */
    ADC_RANGE_NOT_UNDER_LOW = 0x04U, /*!< Range above low limit */
    ADC_RANGE_NOT_BETWEEN = 0x05U,   /*!< Range outside limits */
    ADC_RANGE_NOT_OVER_HIGH = 0x06U  /*!< Range below high limit */
} Adc_ChannelRangeSelectType;

/****************************************************************************************
*                              POWER MANAGEMENT ENUMS                                  *
****************************************************************************************/
/**
 * @brief   Adc_PowerStateType
 * @typedef enum
 * @details ADC module power state
 * @reqs    SWS_Adc_00526
 */
typedef enum
{
    ADC_FULL_POWER = 0x00U,         /*!< Full power mode */
    ADC_LOW_POWER = 0x01U,          /*!< Low power mode */
} Adc_PowerStateType;

/**
 * @brief   Adc_PowerStateRequestResultType
 * @typedef enum
 * @details Result of power state transition requests
 * @reqs    SWS_Adc_00527
 */
typedef enum
{
    ADC_SERVICE_ACCEPTED = 0x00U,       /*!< Power state change executed */
    ADC_NOT_INIT = 0x01U,               /*!< ADC Module not initialized */
    ADC_SEQUENCE_ERROR = 0x02U,         /*!< Wrong API call sequence */
    ADC_HW_FAILURE = 0x03U,             /*!< Hardware failure */
    ADC_POWER_STATE_NOT_SUPP = 0x04U,   /*!< Power state not supported */
    ADC_TRANS_NOT_POSSIBLE = 0x05U      /*!< Transition not possible */
} Adc_PowerStateRequestResultType;

/****************************************************************************************
*                              HARDWARE SPECIFIC ENUMS                                 *
****************************************************************************************/
/**
 * @brief   Adc_NvicType
 * @typedef enum
 * @details Hardware interrupt configuration type
 */
typedef enum 
{
    ADC_HW_DMA = 0x00U,             /*!< DMA-based interrupt handling */
    ADC_HW_EOC = 0x01U              /*!< End-of-conversion interrupt */
} Adc_NvicType;

/****************************************************************************************
*                              CALLBACK FUNCTION TYPES                                 *
****************************************************************************************/
/**
 * @brief   Adc_NotificationCallBack
 * @typedef Function pointer
 * @details Callback function for group conversion completion notification
 */
typedef void (*Adc_NotificationCallBack)(void);

/****************************************************************************************
*                              CONFIGURATION STRUCTURES                                *
****************************************************************************************/
/**
 * @brief   Adc_ChannelDefType
 * @typedef struct
 * @details Channel definition structure
 */
typedef struct
{
    Adc_ChannelType         Adc_ChannelId;          /*!< Channel ID */
    Adc_SamplingTimeType    Adc_ChannelSampTime;    /*!< Sampling time */
} Adc_ChannelDefType;


typedef enum 
{
    ADC_NOTIFICATION_ENABLE           = STD_ON , /*!< ADC notification enabled*/
    ADC_NOTIFICATION_DISABLE          = STD_OFF /*!< ADC notification disabled*/
}Adc_NotificationEnableType;

/**
 * @brief   Adc_GroupDefType
 * @typedef struct
 * @details Group definition structure with complete configuration
 * @reqs    SWS_Adc_00517
 */
typedef struct
{
    /* Basic Group Information */
    const Adc_HwUnitType          Adc_HwUnitId;           /*!< Hardware unit ID */
    const Adc_GroupType           Adc_GroupId;            /*!< Group ID */
    const Adc_GroupPriorityType   Adc_GroupPriority;      /*!< Group priority */
    
    /* Conversion Configuration */
    const Adc_GroupAccessModeType Adc_GroupAccessMode;    /*!< Access mode */
    const Adc_GroupConvModeType   Adc_GroupConvMode;      /*!< Conversion mode */
    const Adc_GroupReplacementType Adc_GroupReplacement;  /*!< Replacement mechanism */
    
    /* Status and Alignment */
    volatile Adc_StatusType          Adc_Status;             /*!< Current status */
    const Adc_ResultAlignmentType Adc_ResultAlignment;    /*!< Result alignment */
    
    /* Channel Configuration */
    const Adc_ChannelDefType*     Adc_ChannelGroup;       /*!< Channel array */
    const Adc_ChannelType         Adc_NbrOfChannel;       /*!< Number of channels */
    
    /* Trigger Configuration */
    const Adc_TriggerSourceType   Adc_TriggerSource;      /*!< Trigger source */
    const Adc_HwTriggerSignalType Adc_HwTriggerSignal;    /*!< HW trigger signal */
    const Adc_HwTriggerTimerType  Adc_HwTriggerTimer;     /*!< HW trigger timer */
    
    /* Streaming Configuration */
    Adc_StreamBufferModeType Adc_StreamBufferMode;  /*!< Buffer mode */
    Adc_StreamNumSampleType Adc_StreamNumSamples;   /*!< Number of samples */
    
    /* Result Buffer Configuration */
    Adc_ValueGroupType*     Adc_ValueResultPtr;     /*!< Result buffer pointer */
    uint16                  Adc_ValueResultSize;    /*!< Result buffer size */
    uint8                   Adc_SetupBufferFlag;   /*!< Check buffer is reset 1: Reset, 0: Not reset yet*/
    
    /* Notification Configuration */
    Adc_NotificationCallBack     Adc_NotificationCb;    /*!< Notification callback */
    Adc_NotificationEnableType   Adc_NotificationEnable; /*!< Notification enable flag */
    Adc_NvicType                 Adc_InterruptType;     /*!< NVIC type */

} Adc_GroupDefType;

/**
 * @brief   ADC DMA availability
 * @typedef enum
 * @details Hardware DMA availability for ADC
 */
typedef enum 
{
    ADC_DMA_AVAILABLE           = STD_ON , /*!< ADC DMA available*/
    ADC_DMA_NOT_AVAILABLE       = STD_OFF /*!< ADC DMA not available*/
}Adc_HwDmaAvailable;



/**
 * @brief   Adc_HwUnitDefType
 * @typedef struct
 * @details Hardware unit definition structure
 */
typedef struct
{
    const Adc_HwUnitType                  AdcHW_UnitId;           /*!< Hardware unit ID */

    /* Queue Configuration */
    uint8                                 AdcHw_QueueEnable;      /*!< Queue enable flag */
    const Adc_PriorityImplementationType  AdcHw_PriorityEnable; /*!< Priority implementation */

    
    /* Hardware Features */
    const Adc_HwDmaAvailable              AdcHw_DMAAvailable;     /*!< DMA availability ADC_DMA_NOT_AVAILABLE or ADC_DMA_AVAILABLE */
} Adc_HwUnitDefType;

/**
 * @brief   Adc_ConfigType
 * @typedef struct
 * @details Main ADC driver configuration structure
 * @reqs    SWS_Adc_00505
 */
typedef struct
{
    /* Hardware Units Configuration */
    const Adc_HwUnitDefType* HwUnits;       /*!< Hardware units array */
    uint8                    NumHwUnits;     /*!< Number of hardware units */
    
    /* Groups Configuration */
    const Adc_GroupDefType* Groups;         /*!< Groups array */
    uint8                   NumGroups;      /*!< Number of groups */
    
} Adc_ConfigType;

/****************************************************************************************
*                              HARDWARE ABSTRACTION MACROS                            *
****************************************************************************************/
#define ADC_INSTANCE_1 0
#define ADC_INSTANCE_2 1
/**
 * @brief Get ADC hardware module from unit ID
 * @param id Hardware unit ID (1 for ADC1, 2 for ADC2)
 * @return Pointer to ADC_TypeDef structure
 */
#define ADC_HW_GET_MODULE_ID(id) \
    ((id == ADC_INSTANCE_1) ? ADC1 : (id == ADC_INSTANCE_2) ? ADC2 : NULL_PTR)

/**
 * @brief Get DMA channel from ADC unit ID
 * @param id ADC unit ID
 * @return Pointer to DMA_Channel_TypeDef structure
 */
#define ADC_HW_GET_DMA_CHANNEL(id) \
    ((id == ADC_INSTANCE_1) ? DMA1_Channel1 : ((DMA_Channel_TypeDef*)0))

/****************************************************************************************
*                              RUNTIME DATA STRUCTURES                                 *
****************************************************************************************/
/**
 * @brief   Adc_RuntimeGroupType
 * @typedef struct
 * @details Runtime data for ADC groups (mutable during operation)
 */
typedef struct
{
    Adc_StatusType          Status;                 /*!< Current group status */
    Adc_ChannelType         CurrentChannelId;       /*!< Current converting channel */
    Adc_StreamNumSampleType SampleCounter;         /*!< Current sample count */
    uint16                  BufferIndex;            /*!< Current buffer index */
} Adc_RuntimeGroupType;

/**
 * @brief   Adc_RuntimeHwUnitType
 * @typedef struct
 * @details Runtime data for ADC hardware units (mutable during operation)
 */
typedef enum 
{
    HW_STATE_IDLE,      /*!< No conversion in progress (neither SW nor HW triggered) */
    HW_STATE_SW,        /*!< SW conversion in progress */
    HW_STATE_HW         /*!< HW conversion in progress, in this state sw conversion is not allow to takeplace*/
} Adc_HwUnitStateType;

typedef struct
{
    Adc_GroupType           CurrentGroupId;         /*!< Currently active group */
    Adc_HwUnitStateType     HwUnitState;             /*!< Hardware unit state flag */
    
    /* Used for queue and sw conversion*/

    Adc_GroupType*          QueueGroup;             /*!< Group queue*/
    Adc_GroupType           QueueMaxSize;           /*!< Group queue max size*/ 
    Adc_GroupType           QueueHead;              /*!< Queue head index */
    Adc_GroupType           QueueTail;              /*!< Queue tail index */
    Adc_GroupType           QueueCount;             /*!< Number of queued groups */
    
    
} Adc_RuntimeHwUnitType;


#endif /* ADC_TYPES_H */

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
