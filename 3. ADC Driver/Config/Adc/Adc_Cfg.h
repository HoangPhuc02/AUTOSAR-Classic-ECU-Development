/****************************************************************************************
*                                ADC_CFG.H                                              *
****************************************************************************************
* File Name   : Adc_Cfg.h
* Module      : Analog to Digital Converter (ADC)
* Description : AUTOSAR ADC driver configuration header file 
* Version     : 3.1.0 - Redesigned for clarity and maintainability
* Date        : 24/06/2025
* Update      : 23/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

#ifndef ADC_CFG_H
#define ADC_CFG_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Std_Types.h"
#include "Adc_Types.h"

/****************************************************************************************
*                              VERSION INFORMATION                                     *
****************************************************************************************/


/****************************************************************************************
*                              DEVELOPMENT CONFIGURATION                               *
****************************************************************************************/
#define ADC_DEV_ERROR_DETECT        STD_ON  /*!< Enable/disable development error detection */
#define ADC_VERSION_INFO_API        STD_ON  /*!< Enable/disable version info API */
#define ADC_NOTIFICATION_SUPPORTED  STD_ON  /*!< Enable/disable notification support */
#define ADC_POWER_STATE_SUPPORTED   STD_OFF /*!< Enable/disable power state support */

/****************************************************************************************
*                              FEATURE CONFIGURATION                                   *
****************************************************************************************/

#define ADC_ENABLE_QUEUING          STD_ON  /*!< Enable group queuing support */ 
#define ADC_ENABLE_PRIORITY         STD_OFF /*!< Enable priority-based interruption */ 
#define ADC_ENABLE_DMA              STD_ON  /*!< Enable DMA support */ 


#define ADC_ENABLE_STREAMING        STD_ON  /*!< Enable streaming mode */ 
#define ADC_ENABLE_NOTIFICATIONS    STD_ON  /*!< Enable notification callbacks */ 

/*Not used */
#define ADC_ENABLE_LIMIT_CHECKING   STD_OFF /*!< Enable limit checking */ 
#define ADC_ENABLE_POWER_MANAGEMENT STD_OFF /*!< Enable power management */ 


/****************************************************************************************
*                              SYSTEM CONFIGURATION                                    *
****************************************************************************************/
/* System Limits */
#define ADC_MAX_GROUPS              10      /*!< Maximum number of ADC groups */
#define ADC_MAX_CHANNELS            16      /*!< Maximum number of ADC channels */ 
#define ADC_MAX_HW_UNITS            2       /*!< Maximum number of ADC hardware units */
/****************************************************************************************
*                              CONFIGURATION PARAMETERS                                *
****************************************************************************************/


/* Buffer Configuration */
#define ADC_MAX_BUFFER_SIZE         256                         /*!< Maximum result buffer size per group */
#define ADC_DEFAULT_MAX_GROUP       5                           /*!< Default maximum number of groups */
#define ADC_DEFAULT_QUEUE_SIZE      ADC_DEFAULT_MAX_GROUP       /*!< Default queue size per hardware unit */ 

/* Real-time Safety Configuration */
/* UNUSED - Max ISR processing time not used in source code */
// #define ADC_MAX_ISR_PROCESSING_TIME_US  50  /*!< Maximum ISR processing time in microseconds */ 
#define ADC_ENABLE_ISR_MONITORING   STD_OFF  /*!< Enable ISR execution time monitoring */


/* UNUSED - Runtime checks not used in source code */
// #define ADC_ENABLE_RUNTIME_CHECKS   STD_ON  /*!< Enable runtime parameter checks */ 
#define ADC_ENABLE_DEBUG_SUPPORT    STD_OFF  /*!< Enable debug support */

/****************************************************************************************
*                              HARDWARE CONFIGURATION                                  *
****************************************************************************************/
/* UNUSED - Hardware configuration not used in source code */
/* ADC1 Configuration */
#define ADC1_ENABLED                STD_ON
#define ADC1_DMA_ENABLED            STD_ON
#define ADC1_DMA_CHANNEL            DMA1_Channel1
#define ADC1_MAX_GROUPS             ADC_DEFAULT_MAX_GROUP
#define ADC1_QUEUE_SIZE             ADC_DEFAULT_QUEUE_SIZE
#define ADC1_QUEUE_ENABLE           STD_OFF

/* ADC2 Configuration */
#define ADC2_ENABLED                STD_OFF 
#define ADC2_DMA_ENABLED            STD_OFF 
#define ADC2_DMA_CHANNEL            NULL
#define ADC2_MAX_GROUPS             ADC_DEFAULT_MAX_GROUP
#define ADC2_QUEUE_SIZE             ADC_DEFAULT_QUEUE_SIZE
#define ADC2_QUEUE_ENABLE           STD_OFF

/* Clock Configuration */
// #define ADC_CLOCK_PRESCALER         ADC_PCLK2_Div6  /*!< ADC clock prescaler */ 
#define ADC_SAMPLING_TIME_DEFAULT   ADC_SampleTime_28Cycles5  /*!< Default sampling time */

/****************************************************************************************
*                              SAFETY CONFIGURATION                                    *
****************************************************************************************/
/* UNUSED - Safety configuration not used in source code */
/* Timeout Configuration */
// #define ADC_CONVERSION_TIMEOUT_MS   100     /*!< Conversion timeout in milliseconds */ 
// #define ADC_INIT_TIMEOUT_MS         50      /*!< Initialization timeout in milliseconds */ 
// #define ADC_DEINIT_TIMEOUT_MS       50      /*!< De-initialization timeout in milliseconds */ 

/* Error Recovery Configuration */
// #define ADC_ENABLE_ERROR_RECOVERY   STD_ON  /*!< Enable automatic error recovery */ 
// #define ADC_MAX_RETRY_COUNT         3       /*!< Maximum retry count for failed operations */ 

/* Watchdog Configuration */
// #define ADC_ENABLE_WATCHDOG         STD_ON  /*!< Enable conversion watchdog */ 
// #define ADC_WATCHDOG_TIMEOUT_MS     200     /*!< Watchdog timeout in milliseconds */ 

/****************************************************************************************
*                              STREAMING CONFIGURATION                                 *
****************************************************************************************/
/* UNUSED - Streaming configuration not used in source code */
/* Streaming Buffer Configuration */
// #define ADC_STREAMING_BUFFER_SIZE   128     /*!< Default streaming buffer size */ 
// #define ADC_ENABLE_CIRCULAR_BUFFER  STD_ON  /*!< Enable circular buffer support */ 
// #define ADC_BUFFER_OVERFLOW_NOTIFY  STD_ON  /*!< Enable buffer overflow notification */ 

/* Streaming Thresholds */
// #define ADC_STREAM_NOTIFY_THRESHOLD 0.75    /*!< Notification threshold (75% full) */ 
// #define ADC_STREAM_CRITICAL_THRESHOLD 0.9   /*!< Critical threshold (90% full) */ 

/****************************************************************************************
*                              INTERRUPT CONFIGURATION                                 *
****************************************************************************************/
/* UNUSED - Interrupt configuration not used in source code */
/* Interrupt Priorities */
#define ADC_EOC_INTERRUPT_PRIORITY  5       /*!< End-of-conversion interrupt priority */ 
#define ADC_DMA_INTERRUPT_PRIORITY  4       /*!< DMA transfer complete interrupt priority */ 
#define ADC_ERROR_INTERRUPT_PRIORITY 3      /*!< Error interrupt priority */ 

/* Interrupt Enable Masks */
// #define ADC_ENABLE_EOC_INTERRUPT    STD_ON  /*!< Enable end-of-conversion interrupt */ 
// #define ADC_ENABLE_DMA_INTERRUPT    STD_ON  /*!< Enable DMA transfer complete interrupt */ 
// #define ADC_ENABLE_ERROR_INTERRUPT  STD_ON  /*!< Enable error interrupt */ 

/****************************************************************************************
*                              PERFORMANCE CONFIGURATION                               *
****************************************************************************************/
/* UNUSED - Performance configuration not used in source code */
/* Performance Tuning */
// #define ADC_ENABLE_FAST_CONVERSION  STD_ON  /*!< Enable fast conversion mode */ 
// #define ADC_ENABLE_PARALLEL_CONV    STD_OFF /*!< Enable parallel conversion (if supported) */ 
// #define ADC_OPTIMIZE_FOR_SPEED      STD_ON  /*!< Optimize for conversion speed */ 
// #define ADC_OPTIMIZE_FOR_POWER      STD_OFF /*!< Optimize for power consumption */ 

/* Cache Configuration */
// #define ADC_ENABLE_RESULT_CACHE     STD_ON  /*!< Enable result caching */ 
// #define ADC_CACHE_SIZE              32      /*!< Result cache size */ 

/****************************************************************************************
*                              EXTERNAL DECLARATIONS                                   *
****************************************************************************************/
/* Configuration Arrays */
extern Adc_GroupDefType Adc_GroupConfig[ADC_MAX_GROUPS];
extern Adc_HwUnitDefType Adc_HwUnitConfig[ADC_MAX_HW_UNITS];
/* UNUSED - Channel config array not used */
/* extern const Adc_ChannelDefType Adc_ChannelConfig[ADC_MAX_CHANNELS]; */
extern const Adc_ConfigType Adc_Config;

/* Result Buffers */
extern Adc_ValueGroupType Adc_ResultBuffer[ADC_MAX_GROUPS][ADC_MAX_BUFFER_SIZE];

/****************************************************************************************
*                              CALLBACK FUNCTION DECLARATIONS                         *
****************************************************************************************/
/* Standard Notification Callbacks */
extern void Adc_Group1_Notification(void);
extern void Adc_Group2_Notification(void);
extern void Adc_Group3_Notification(void);
extern void Adc_Group4_Notification(void);

/* Hardware Event Callbacks */
void Adc_TransferComplete_Callback(ADC_TypeDef* ADCx);
void Adc_DmaTransferComplete_Callback(DMA_Channel_TypeDef* DMAx_Channely);
void Adc_Error_Callback(ADC_TypeDef* ADCx, uint32 ErrorCode);

/* Streaming Callbacks */
void Adc_StreamingThreshold_Callback(Adc_GroupType GroupId, uint8_t ThresholdLevel);
void Adc_BufferOverflow_Callback(Adc_GroupType GroupId);

/* Safety Callbacks */
void Adc_ConversionTimeout_Callback(Adc_GroupType GroupId);
void Adc_WatchdogTimeout_Callback(Adc_GroupType GroupId);

/****************************************************************************************
*                              VALIDATION MACROS                                       *
****************************************************************************************/
/* Configuration Validation */
#if (ADC_MAX_CHANNELS > 16)
#error "ADC_MAX_CHANNELS cannot exceed 16"
#endif

/* UNUSED - Channel validation not used since ADC_MAX_CHANNELS is commented out */
/* #if (ADC_MAX_CHANNELS > 18)
#error "ADC_MAX_CHANNELS cannot exceed 18 (STM32F10x limitation)"
#endif */

#if (ADC_MAX_HW_UNITS > 3)
#error "ADC_MAX_HW_UNITS cannot exceed 3 (STM32F10x limitation)"
#endif

#if (ADC_MAX_BUFFER_SIZE > 1024)
#error "ADC_MAX_BUFFER_SIZE exceeds memory constraints"
#endif

/* UNUSED - Feature validation not used since feature flags are commented out */
/* #if (ADC_ENABLE_PRIORITY == STD_ON) && (ADC_ENABLE_QUEUING == STD_OFF)
#error "Priority support requires queuing to be enabled"
#endif */

/* #if (ADC_ENABLE_STREAMING == STD_ON) && (ADC_ENABLE_DMA == STD_OFF)
#error "Streaming mode requires DMA support"
#endif */

/* UNUSED - Safety validation not used since safety defines are commented out */
/* #if (ADC_CONVERSION_TIMEOUT_MS < 10)
#error "ADC_CONVERSION_TIMEOUT_MS must be at least 10ms"
#endif */

/* UNUSED - ISR timing validation not used since timing define is commented out */
/* #if (ADC_MAX_ISR_PROCESSING_TIME_US > 100)
#error "ADC_MAX_ISR_PROCESSING_TIME_US should not exceed 100us for real-time safety"
#endif */


#endif /* ADC_CFG_H */

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
