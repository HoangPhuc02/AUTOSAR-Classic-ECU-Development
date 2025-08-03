/**
 * @file    Adc_Cfg.h
 * @brief   AUTOSAR ADC Driver Configuration
 * @details This file contains the configuration parameters for the ADC driver.
 *          It includes pre-compile time configuration options, symbolic names
 *          for configured channels and groups, and hardware-specific settings.
 * 
 * @author  AUTOSAR MCAL Team
 * @date    2025-01-08
 * @version 1.0.0
 * 
 * @note    Compliant with AUTOSAR 4.4.0 Specification
 *          Target: STM32F103C8T6
 * 
 * Requirements Traceability:
 * - SWS_Adc_00008: Configuration parameters shall be defined
 * - SWS_Adc_00009: Symbolic names shall be provided
 * - SWS_Adc_00010: Configuration validation shall be supported
 */

#ifndef ADC_CFG_H
#define ADC_CFG_H

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "Std_Types.h"
#include "Adc_Types.h"
#include "MCAL_Cfg.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define LIN_CFG_VENDOR_ID                     (0x0001U)
#define LIN_CFG_MODULE_ID                     (0x007BU)
#define LIN_CFG_AR_RELEASE_MAJOR_VERSION      (4U)
#define LIN_CFG_AR_RELEASE_MINOR_VERSION      (4U)
#define LIN_CFG_AR_RELEASE_REVISION_VERSION   (0U)
#define LIN_CFG_SW_MAJOR_VERSION              (1U)
#define LIN_CFG_SW_MINOR_VERSION              (0U)
#define LIN_CFG_SW_PATCH_VERSION              (0U)

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/**
 * @brief   ADC Driver Configuration Parameters
 * @details Pre-compile time configuration parameters
 */

/* Development Error Detection */
#define ADC_DEV_ERROR_DETECT                  (MCAL_DEV_ERROR_DETECT)

/* Version Info API */
#define LIN_VERSION_INFO_API                  (MCAL_VERSION_INFO_API)

/* Enable/Disable Check Wakeup API */
#define LIN_ENABLE_CHECK_WAKEUP_API           (STD_OFF)

/* Read Group API */
#define ADC_READ_GROUP_API                    (STD_ON)

/* Hardware Trigger API */
#define ADC_HW_TRIGGER_API                    (STD_ON)

/* Group Notification Capability */
#define ADC_GRP_NOTIF_CAPABILITY              (STD_ON)

/* Priority Implementation */
#define ADC_PRIORITY_IMPLEMENTATION           (ADC_PRIORITY_NONE)

/* Result Alignment */
#define ADC_RESULT_ALIGNMENT                  (STD_ON)    /* Right alignment */

/* Power State Change API */
#define ADC_POWER_STATE_CHANGE_API            (STD_OFF)

/**
 * @brief   ADC Driver Instance Configuration
 */
#define ADC_INSTANCE_ID                       (0U)

/**
 * @brief   Number of Configured Elements
 * @details Total number of channels and groups configured
 */
#define ADC_CONFIGURED_CHANNELS               (6U)
#define ADC_CONFIGURED_GROUPS                 (4U)

/**
 * @brief   Symbolic Names for Configured Channels
 * @details These symbolic names can be used by the application to reference
 *          specific channels without knowing their physical channel numbers
 */

/* Sensor Channels */
#define AdcConf_AdcChannel_POTENTIOMETER      (ADC_CHANNEL_0)     /* PA0 */
#define AdcConf_AdcChannel_LIGHT_SENSOR       (ADC_CHANNEL_1)     /* PA1 */
#define AdcConf_AdcChannel_TEMPERATURE_EXT    (ADC_CHANNEL_2)     /* PA2 */
#define AdcConf_AdcChannel_VOLTAGE_MONITOR    (ADC_CHANNEL_3)     /* PA3 */

/* Internal Channels */
#define AdcConf_AdcChannel_TEMP_SENSOR        (ADC_CHANNEL_TEMP_SENSOR)
#define AdcConf_AdcChannel_VREFINT            (ADC_CHANNEL_VREFINT)

/**
 * @brief   Symbolic Names for Configured Groups
 * @details These symbolic names can be used by the application to reference
 *          specific groups without knowing their physical group numbers
 */
#define AdcConf_AdcGroup_SENSOR_BANK          (ADC_GROUP_SENSOR_BANK)
#define AdcConf_AdcGroup_VOLTAGE_MONITOR      (ADC_GROUP_VOLTAGE_MONITOR)
#define AdcConf_AdcGroup_TEMPERATURE_MONITOR  (ADC_GROUP_TEMPERATURE_MONITOR)
#define AdcConf_AdcGroup_DIAGNOSTIC           (ADC_GROUP_DIAGNOSTIC)

/**
 * @brief   ADC Configuration Set Symbolic Names
 * @details Symbolic names for different configuration sets
 */
#define AdcConfigSet_Default                  (&Adc_Config)

/**
 * @brief   Configuration Validation Macros
 * @details Macros for validating configuration parameters
 */
#define ADC_VALIDATE_CHANNEL(channel) \
    ((channel) < ADC_MAX_CHANNEL_NUMBER)

#define ADC_VALIDATE_GROUP(group) \
    ((group) < ADC_MAX_GROUP_NUMBER)

#define ADC_VALIDATE_POINTER(ptr) \
    ((ptr) != NULL_PTR)

/**
 * @brief   Hardware-Specific Configuration
 * @details Configuration parameters specific to STM32F103C8T6
 */

/* ADC Clock Configuration */
#define ADC_CLOCK_PRESCALER                   (4U)        /* ADC clock = PCLK2/4 = 18MHz */
#define ADC_CONVERSION_TIME_US                (1U)        /* Approximate conversion time */

/* ADC Resolution */
#define ADC_RESOLUTION                        (ADC_RESOLUTION_12BIT)

/* Default Sampling Times */
#define ADC_DEFAULT_SAMPLING_TIME             (ADC_SMPR_SMP_55_5_CYCLES)
#define ADC_FAST_SAMPLING_TIME                (ADC_SMPR_SMP_7_5_CYCLES)
#define ADC_SLOW_SAMPLING_TIME                (ADC_SMPR_SMP_239_5_CYCLES)

/**
 * @brief   Group Configuration Parameters
 * @details Default configuration parameters for different group types
 */

/* Sensor Bank Group Configuration */
#define ADC_SENSOR_BANK_CHANNELS              (4U)
#define ADC_SENSOR_BANK_CONVERSION_MODE       (ADC_CONV_MODE_ONESHOT)
#define ADC_SENSOR_BANK_TRIGGER_SOURCE        (ADC_TRIGG_SRC_SW)
#define ADC_SENSOR_BANK_ACCESS_MODE           (ADC_ACCESS_MODE_SINGLE)
#define ADC_SENSOR_BANK_NOTIFICATION          (STD_ON)

/* Voltage Monitor Group Configuration */
#define ADC_VOLTAGE_MONITOR_CHANNELS          (1U)
#define ADC_VOLTAGE_MONITOR_CONVERSION_MODE   (ADC_CONV_MODE_CONTINUOUS)
#define ADC_VOLTAGE_MONITOR_TRIGGER_SOURCE    (ADC_TRIGG_SRC_SW)
#define ADC_VOLTAGE_MONITOR_ACCESS_MODE       (ADC_ACCESS_MODE_STREAMING)
#define ADC_VOLTAGE_MONITOR_NOTIFICATION      (STD_ON)

/* Temperature Monitor Group Configuration */
#define ADC_TEMPERATURE_MONITOR_CHANNELS      (1U)
#define ADC_TEMPERATURE_MONITOR_CONVERSION_MODE (ADC_CONV_MODE_ONESHOT)
#define ADC_TEMPERATURE_MONITOR_TRIGGER_SOURCE (ADC_TRIGG_SRC_HW)
#define ADC_TEMPERATURE_MONITOR_HW_TRIGGER    (ADC_HW_TRIG_TIMER3_TRGO)
#define ADC_TEMPERATURE_MONITOR_ACCESS_MODE   (ADC_ACCESS_MODE_SINGLE)
#define ADC_TEMPERATURE_MONITOR_NOTIFICATION  (STD_ON)

/* Diagnostic Group Configuration */
#define ADC_DIAGNOSTIC_CHANNELS               (2U)
#define ADC_DIAGNOSTIC_CONVERSION_MODE        (ADC_CONV_MODE_ONESHOT)
#define ADC_DIAGNOSTIC_TRIGGER_SOURCE         (ADC_TRIGG_SRC_SW)
#define ADC_DIAGNOSTIC_ACCESS_MODE            (ADC_ACCESS_MODE_SINGLE)
#define ADC_DIAGNOSTIC_NOTIFICATION          (STD_OFF)

/**
 * @brief   Buffer Sizes
 * @details Buffer sizes for different groups
 */
#define ADC_SENSOR_BANK_BUFFER_SIZE           (ADC_SENSOR_BANK_CHANNELS)
#define ADC_VOLTAGE_MONITOR_BUFFER_SIZE       (16U)       /* Streaming buffer */
#define ADC_TEMPERATURE_MONITOR_BUFFER_SIZE   (ADC_TEMPERATURE_MONITOR_CHANNELS)
#define ADC_DIAGNOSTIC_BUFFER_SIZE            (ADC_DIAGNOSTIC_CHANNELS)

/**
 * @brief   Notification Function Prototypes
 * @details Function prototypes for group notification callbacks
 */
#if (ADC_GRP_NOTIF_CAPABILITY == STD_ON)
extern void Adc_SensorBankNotification(void);
extern void Adc_VoltageMonitorNotification(void);
extern void Adc_TemperatureMonitorNotification(void);
#endif

/**
 * @brief   Performance Configuration
 * @details Performance-related configuration options
 */
#define ADC_OPTIMIZE_FOR_SPEED                (STD_ON)
#define ADC_OPTIMIZE_FOR_SIZE                 (STD_OFF)
#define ADC_DMA_SUPPORTED                     (STD_ON)

/**
 * @brief   Safety Configuration
 * @details Safety-related configuration options
 */
#define ADC_SAFETY_ENABLE                     (STD_OFF)
#define ADC_RUNTIME_ERROR_DETECT              (STD_OFF)
#define ADC_WATCHDOG_ENABLE                   (STD_OFF)

/**
 * @brief   Calibration Configuration
 * @details ADC calibration settings
 */
#define ADC_CALIBRATION_ENABLE                (STD_ON)
#define ADC_CALIBRATION_TIMEOUT_LOOPS         (1000U)

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/**
 * @brief   ADC Configuration Structure
 * @details External declaration of the ADC configuration structure
 */
extern CONST(Adc_ConfigType, ADC_CONST) Adc_Config;

/**
 * @brief   ADC Result Buffers
 * @details External declarations of result buffers for each group
 */
extern VAR(Adc_ValueType, ADC_VAR) Adc_SensorBankBuffer[ADC_SENSOR_BANK_BUFFER_SIZE];
extern VAR(Adc_ValueType, ADC_VAR) Adc_VoltageMonitorBuffer[ADC_VOLTAGE_MONITOR_BUFFER_SIZE];
extern VAR(Adc_ValueType, ADC_VAR) Adc_TemperatureMonitorBuffer[ADC_TEMPERATURE_MONITOR_BUFFER_SIZE];
extern VAR(Adc_ValueType, ADC_VAR) Adc_DiagnosticBuffer[ADC_DIAGNOSTIC_BUFFER_SIZE];

/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

#ifdef __cplusplus
}
#endif

#endif /* ADC_CFG_H */

/**
 * @}
 */
