/****************************************************************************************
*                                ADC_HW.H                                              *
****************************************************************************************
* File Name   : Adc_Hw.h
* Module      : Analog to Digital Converter (ADC) - Hardware Layer
* Description : AUTOSAR ADC driver hardware abstraction layer header file 
* Version     : 3.1.0 - Redesigned for clarity and maintainability
* Date        : 23/07/2025
* Update      : 23/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

#ifndef ADC_HW_H
#define ADC_HW_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
/* Standard AUTOSAR includes */
#include "Std_Types.h"
#include "Adc_Types.h"
#include "Adc_Cfg.h"




/****************************************************************************************
*                              HARDWARE VALIDATION MACROS                             *
****************************************************************************************/

/**
 * @brief Validate ADC hardware unit ID
 * @param id Hardware unit identifier (0-based indexing)
 * @return TRUE if valid, FALSE otherwise
 */
#define ADC_HW_IS_VALID_UNIT(id)    (((id) >= 0) && ((id) < ADC_MAX_HW_UNITS))

/**
 * @brief Validate ADC group ID
 * @param id Group identifier (0-based indexing)
 * @return TRUE if valid, FALSE otherwise
 */
#define ADC_HW_IS_VALID_GROUP(id)   (((id) >= 0) && ((id) < ADC_MAX_GROUPS))

/**
 * @brief Validate ADC channel ID
 * @param id Channel identifier (0-based indexing)
 * @return TRUE if valid, FALSE otherwise
 */
#define ADC_HW_IS_VALID_CHANNEL(id) (((id) >= 0) && ((id) < ADC_MAX_CHANNELS))

/****************************************************************************************
*                              HARDWARE ABSTRACTION MACROS                            *
****************************************************************************************/

/**
 * @brief Get DMA interrupt flag for specified ADC unit
 * @param id ADC hardware unit ID (1 for ADC1, 2 for ADC2)
 * @return DMA interrupt flag or 0 if invalid unit
 */
#define ADC_HW_GET_DMA_IT_FLAG(id)  ((id == 1) ? DMA1_IT_TC1 : 0)

/**
 * @brief Get ADC interrupt flag for specified unit
 * @param id ADC hardware unit ID (1 for ADC1, 2 for ADC2)
 * @return ADC interrupt flag or 0 if invalid unit
 */
#define ADC_HW_GET_ADC_IT_FLAG(id)  ((id == 1) ? ADC_IT_EOC : 0)

/****************************************************************************************
*                              HARDWARE STATUS MACROS                                 *
****************************************************************************************/

/**
 * @brief Check if ADC module is enabled
 * @param ADCx Pointer to ADC peripheral (ADC1, ADC2, etc.)
 * @return Non-zero if enabled, 0 if disabled
 */
#define ADC_HW_IS_ENABLED(ADCx)     ((ADCx->CR2 & ADC_CR2_ADON) != 0)

/**
 * @brief Check if ADC conversion is currently in progress
 * @param ADCx Pointer to ADC peripheral (ADC1, ADC2, etc.)
 * @return Non-zero if converting, 0 if idle
 */
#define ADC_HW_IS_CONVERTING(ADCx)  ((ADCx->SR & ADC_SR_STRT) != 0)

/**
 * @brief Check if DMA is enabled for the ADC
 * @param ADCx Pointer to ADC peripheral (ADC1, ADC2, etc.)
 * @return Non-zero if DMA enabled, 0 if disabled
 */
#define ADC_HW_IS_DMA_ENABLED(ADCx) ((ADCx->CR2 & ADC_CR2_DMA) != 0)

/**
 * @brief Check if end-of-conversion flag is set
 * @param ADCx Pointer to ADC peripheral (ADC1, ADC2, etc.)
 * @return Non-zero if EOC flag set, 0 if not set
 */
#define ADC_HW_IS_EOC_SET(ADCx)     ((ADCx->SR & ADC_SR_EOC) != 0)

/****************************************************************************************
*                              EXTERNAL DECLARATIONS                                   *
****************************************************************************************/
/* Configuration arrays */
extern Adc_GroupDefType     Adc_GroupConfig[ADC_MAX_GROUPS];
extern Adc_HwUnitDefType    Adc_HwUnitConfig[ADC_MAX_HW_UNITS];


/****************************************************************************************
*                              INITIALIZATION FUNCTIONS                               *
****************************************************************************************/
/**
 * @brief Initialize ADC hardware module
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_Init(Adc_HwUnitType HwUnitId);

/**
 * @brief Deinitialize ADC hardware module
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_DeInit(Adc_HwUnitType HwUnitId);

/****************************************************************************************
*                              CONVERSION CONTROL FUNCTIONS                           *
****************************************************************************************/

/**
 * @brief Start software-triggered ADC conversion
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] GroupId ADC group ID to start conversion
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note This function initiates software-triggered conversion for the specified group
 *       If queuing is enabled, the group will be added to the conversion queue
 */
Std_ReturnType AdcHw_StartSwConversion(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);

/**
 * @brief Stop software-triggered ADC conversion
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] GroupId ADC group ID to stop conversion
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note This function stops ongoing conversion and may start next group from queue if enabled
 */
Std_ReturnType AdcHw_StopSwConversion(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);

/**
 * @brief Start hardware-triggered ADC conversion
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] GroupId ADC group ID to start conversion
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note This function configures external trigger for hardware-triggered conversion
 */
Std_ReturnType AdcHw_StartHwConversion(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);

/**
 * @brief Stop hardware-triggered ADC conversion
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] GroupId ADC group ID to stop conversion
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note This function disables external trigger and may recall software conversions
 */
Std_ReturnType AdcHw_StopHwConversion(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);

/**
 * @brief Recall next software conversion from queue
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note Called after hardware conversion completion to process pending software requests
 *       Only used when ADC_ENABLE_QUEUING is enabled
 */
Std_ReturnType AdcHw_RecallSwConversion(Adc_HwUnitType HwUnitId);
/****************************************************************************************
*                              HW CONFIGURATION FUNCTIONS                               *
****************************************************************************************/
/**
 * @brief Configure ADC hardware module for a specific group
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] GroupId ADC group ID to configure
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_ConfigureGroup(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);

/**
 * @brief Configure ADC channels for a specific group
 * @param[in] ADCx Pointer to ADC peripheral (ADC1, ADC2, etc.)
 * @param[in] HwUnitConfig Pointer to hardware unit configuration
 * @param[in] GroupConfig Pointer to group configuration containing channel settings
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note This function configures channel rank, sampling time, and sequence for the group
 */
Std_ReturnType AdcHw_ConfigureChannels(ADC_TypeDef* ADCx, 
                                       Adc_HwUnitDefType* HwUnitConfig, 
                                       Adc_GroupDefType* GroupConfig);


/****************************************************************************************
*                              DMA CONFIGURATION FUNCTIONS                             *
****************************************************************************************/

/**
 * @brief Initialize DMA for ADC conversion
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] GroupId ADC group ID for DMA configuration
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note Configures DMA channel, buffer, and interrupt settings for the specified group
 *       Only available when ADC_ENABLE_DMA is STD_ON
 */
Std_ReturnType AdcHw_InitDma(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);

/**
 * @brief Deinitialize DMA for ADC conversion
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note Disables DMA channel and clears configuration for the specified ADC unit
 */
Std_ReturnType AdcHw_DeInitDma(Adc_HwUnitType HwUnitId);

/****************************************************************************************
*                              INTERRUPT FUNCTIONS                                    *
****************************************************************************************/

/**
 * @brief Enable ADC interrupts
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] InterruptType Type of interrupt to enable (see ADC_Hw_Interrupt_types)
 *                          - ADC_INTERRUPT_EOC: End of conversion interrupt
 *                          - ADC_INTERRUPT_DMA_TC: DMA transfer complete interrupt
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note Multiple interrupt types can be enabled using bitwise OR operation
 */
Std_ReturnType AdcHw_EnableInterrupt(Adc_HwUnitType HwUnitId, uint8 InterruptType);

/**
 * @brief Disable ADC interrupts
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] InterruptType Type of interrupt to disable (see ADC_Hw_Interrupt_types)
 *                          - ADC_INTERRUPT_EOC: End of conversion interrupt
 *                          - ADC_INTERRUPT_DMA_TC: DMA transfer complete interrupt
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note Multiple interrupt types can be disabled using bitwise OR operation
 */
Std_ReturnType AdcHw_DisableInterrupt(Adc_HwUnitType HwUnitId, uint8 InterruptType);

/****************************************************************************************
*                              RESULT HANDLING FUNCTIONS                              *
****************************************************************************************/

/**
 * @brief Read ADC conversion result for a group
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] GroupId ADC group ID whose results to read
 * @param[out] ResultPtr Pointer to buffer where results will be stored
 * @return E_OK if successful, E_NOT_OK otherwise
 * @note Results are only available when group status is ADC_COMPLETED or ADC_STREAM_COMPLETED
 */
Std_ReturnType AdcHw_ReadResult(Adc_HwUnitType HwUnitId, 
                               Adc_GroupType GroupId, 
                               Adc_ValueGroupType* ResultPtr);

/**
 * @brief Read ADC conversion result for a group
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] GroupId ADC group ID whose results to read
 * @return none
 * @note This function handles the state after reading results, such as updating group status
 *       and notifying callbacks if necessary. It is called internally after reading results.
 */                          
void AdcHw_HandleReadResultState(Adc_HwUnitType HwUnitId, 
                               Adc_GroupType GroupId);




/****************************************************************************************
*                              INTERRUPT HANDLER FUNCTIONS                             *
****************************************************************************************/

/**
 * @brief ADC end-of-conversion interrupt service routine
 * @param[in] ADCx Pointer to ADC peripheral that generated the interrupt
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @return void
 * @note This function handles EOC interrupts, reads conversion data, and manages
 *       channel sequencing for multi-channel groups
 */
void AdcHw_InterruptHandler(ADC_TypeDef* ADCx, Adc_HwUnitType HwUnitId);

/**
 * @brief DMA transfer complete interrupt service routine
 * @param[in] DMAx Pointer to DMA channel that generated the interrupt
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @return void
 * @note This function handles DMA TC interrupts, updates group status, and calls
 *       notification callbacks when conversion is complete
 */
void AdcHw_DmaInterruptHandler(DMA_Channel_TypeDef* DMAx, Adc_HwUnitType HwUnitId);

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
Std_ReturnType AdcHw_AddGroupToQueue(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);

/**
 * @brief Remove group from conversion queue
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_RemoveGroupFromQueue(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);

/**
 * @brief Get next group from queue
 * @param[in] HwUnitId ADC hardware unit ID
 * @return Group ID or ADC_INVALID_GROUP_ID if queue is empty
 */
Adc_GroupType AdcHw_GetNextGroupFromQueue(Adc_HwUnitType HwUnitId);

/**
 * @brief Check if group is in queue
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return index of group in queue , if not in queue return ADC_INVALID_GROUP_ID
 */
Adc_GroupType AdcHw_IsGroupInQueue(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);

/**
 * @brief Clear conversion queue
 * @param[in] HwUnitId ADC hardware unit ID
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType AdcHw_ClearQueue(Adc_HwUnitType HwUnitId);
#endif /* ADC_ENABLE_QUEUING */

/****************************************************************************************
*                              STATUS FUNCTIONS                                       *
****************************************************************************************/

/**
 * @brief Get ADC group runtime sample counter
 * @param[in] GroupId ADC group ID to query
 * @return Current sample counter value (255 if invalid GroupId)
 * @note Returns the number of samples completed for the specified group
 */
Adc_StreamNumSampleType AdcHw_GetGroupRuntimeSampCounter(Adc_GroupType GroupId);

/**
 * @brief Get ADC group runtime status
 * @param[in] GroupId ADC group ID to query
 * @return Current group status (ADC_IDLE, ADC_BUSY, ADC_COMPLETED, ADC_STREAM_COMPLETED)
 * @note Returns ADC_IDLE if invalid GroupId is provided
 */
Adc_StatusType AdcHw_GetGroupRuntimeStatus(Adc_GroupType GroupId);

/**
 * @brief Reset ADC group runtime parameters
 * @param[in] GroupId ADC group ID to reset
 * @return E_OK if successful, E_NOT_OK if invalid GroupId
 * @note Resets sample counter, channel ID, buffer index, and status to idle
 */
Std_ReturnType AdcHw_ResetGroupRuntime(Adc_GroupType GroupId);

/**
 * @brief Set ADC group status
 * @param[in] GroupId ADC group ID to update
 * @param[in] Status New status to set (ADC_IDLE, ADC_BUSY, ADC_COMPLETED, etc.)
 * @return void
 * @note Updates both configuration and runtime status for the group
 */
void AdcHw_SetGroupStatus(Adc_GroupType GroupId, Adc_StatusType Status);

/**
 * @brief Reset ADC hardware unit runtime parameters
 * @param[in] HwUnitId ADC hardware unit ID to reset (0 = ADC1, 1 = ADC2)
 * @return E_OK if successful, E_NOT_OK if invalid HwUnitId
 * @note Resets current group, state, and queue parameters if queuing is enabled
 */
Std_ReturnType AdcHw_ResetHwRuntime(Adc_HwUnitType HwUnitId);

/**
 * @brief Get ADC hardware unit current state
 * @param[in] HwUnitId ADC hardware unit ID to query (0 = ADC1, 1 = ADC2)
 * @return Current hardware unit state (HW_STATE_IDLE, HW_STATE_SW, HW_STATE_HW)
 * @note Inline function for fast access to hardware unit state
 */
Adc_HwUnitStateType AdcHw_GetHwUnitState(Adc_HwUnitType HwUnitId);

/**
 * @brief Get current converting channel for a group
 * @param[in] HwUnitId ADC hardware unit ID (0 = ADC1, 1 = ADC2)
 * @param[in] GroupId ADC group ID to query
 * @return Current channel ID being converted (ADC_INVALID_CHANNEL_ID if invalid parameters)
 * @note Returns the channel currently being processed in multi-channel conversions
 */
Adc_ChannelType AdcHw_GetCurrentChannel(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);

/****************************************************************************************
*                              VALIDATION FUNCTIONS                                   *
****************************************************************************************/
/**
 * @brief Validate hardware unit ID
 * @param[in] HwUnitId ADC hardware unit ID
 * @return TRUE if valid, FALSE if invalid
 */
boolean AdcHw_ValidateHwUnit(Adc_HwUnitType HwUnitId);

/**
 * @brief Validate group ID
 * @param[in] GroupId ADC group ID
 * @return TRUE if valid, FALSE if invalid
 */
boolean AdcHw_ValidateGroup(Adc_GroupType GroupId);

/**
 * @brief Validate channel ID
 * @param[in] ChannelId ADC channel ID
 * @return TRUE if valid, FALSE if invalid
 */
boolean AdcHw_ValidateChannel(Adc_ChannelType ChannelId);

/****************************************************************************************
*                              UTILITY FUNCTIONS                                      *
****************************************************************************************/
/**
 * @brief Convert ADC channel ID to hardware channel number
 * @param[in] ChannelId ADC channel ID
 * @return Hardware channel number
 */
uint8 AdcHw_GetHwChannelNumber(Adc_ChannelType ChannelId);

/**
 * @brief Convert sampling time to hardware register value
 * @param[in] SampleTime Sampling time
 * @return Hardware register value
 */
uint32 AdcHw_GetHwSampleTime(Adc_SamplingTimeType SampleTime);

/**
 * @brief Get hardware trigger source
 * @param[in] TriggerSource Trigger source
 * @return Hardware trigger source value
 */
uint32 AdcHw_GetHwTriggerSource(Adc_TriggerSourceType TriggerSource);

/****************************************************************************************
*                              DEFERRED PROCESSING FUNCTIONS                          *
****************************************************************************************/
/**
 * @brief Main function for deferred processing
 * @return void
 * @note This function should be called periodically from the main loop
 */
void AdcHw_MainFunction(void);

/**
 * @brief Process completed conversions
 * @param[in] HwUnitId ADC hardware unit ID
 * @return void
 */
void AdcHw_ProcessCompletedConversions(Adc_HwUnitType HwUnitId);

/**
 * @brief Handle group completion
 * @param[in] HwUnitId ADC hardware unit ID
 * @param[in] GroupId ADC group ID
 * @return void
 */
void AdcHw_HandleGroupCompletion(Adc_HwUnitType HwUnitId, Adc_GroupType GroupId);

/****************************************************************************************
*                              CONSTANTS AND DEFINITIONS                              *
****************************************************************************************/

/* Invalid identifier constants */
#define ADC_INVALID_GROUP_ID        0xFFU    /* Invalid group identifier */
#define ADC_INVALID_CHANNEL_ID      0xFFU    /* Invalid channel identifier */
#define ADC_INVALID_HW_UNIT_ID      0xFFU    /* Invalid hardware unit identifier */

/* ADC Hardware Interrupt Types - Used with Enable/Disable Interrupt functions */
#define ADC_INTERRUPT_EOC           (uint8)0x01U    /* End of conversion interrupt */
#define ADC_INTERRUPT_DMA_TC        (uint8)0x02U    /* DMA transfer complete interrupt */

/* Hardware performance limits and constraints */
#define ADC_HW_MAX_CHANNELS_PER_GROUP   16U     /* Maximum channels per conversion group */
#define ADC_HW_MAX_CONVERSION_TIME_US   20U     /* Maximum conversion time in microseconds */
#define ADC_HW_MAX_SAMPLING_CYCLES      239U    /* Maximum sampling cycles (STM32F103 limit) */

#endif /* ADC_HW_H */

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
