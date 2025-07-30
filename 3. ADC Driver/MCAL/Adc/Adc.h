/****************************************************************************************
*                                ADC.H                                                  *
****************************************************************************************
* File Name   : Adc.h
* Module      : Analog to Digital Converter (ADC)
* Description : AUTOSAR ADC driver header file 
* Version     : 3.1.0
* Date        : 24/06/2025
* Update      : 23/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

#ifndef ADC_H
#define ADC_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
/* Standard AUTOSAR includes */
#include "Std_Types.h"
#include "Adc_Cfg.h"
#include "Adc_Types.h"
/* Hardware specific includes */
#include "stm32f10x_adc.h"

/****************************************************************************************
*                              VERSION INFORMATION                                     *
****************************************************************************************/
/* AUTOSAR specification version information */
#define ADC_VENDOR_ID           0x0001  /* Vendor ID for ADC driver */ 
#define ADC_MODULE_ID           0x0037  /* Module ID for ADC driver (37 per AUTOSAR) */
#define ADC_SW_MAJOR_VERSION    3
#define ADC_SW_MINOR_VERSION    1
#define ADC_SW_PATCH_VERSION    0 
#define ADC_AR_RELEASE_MAJOR_VERSION    4
#define ADC_AR_RELEASE_MINOR_VERSION    4
#define ADC_AR_RELEASE_PATCH_VERSION    0

/****************************************************************************************
*                                   MACRO DEFINITIONS                                   *
****************************************************************************************/
/* ADC Status Macros */
/* Define macros for ADC initialization state */
/* Define macros for ADC conversion modes */
/* Define macros for ADC power states */
/* Define macros for other ADC related configurations */


/****************************************************************************************
*                                 ERROR CODES                                          *
****************************************************************************************/
/* Define error codes for ADC module */
/* Development Errors */
#define ADC_E_UNINIT                    0x0AU   /* ADC driver not initialized */
#define ADC_E_ALREADY_INITIALIZED       0x0dU   /* ADC driver already initialized */
#define ADC_E_PARAM_POINTER             0x14U   /* Incorrect buffer pointer */
#define ADC_E_PARAM_CONFIG              0x1FU   /* Invalid configuration parameter */
#define ADC_E_PARAM_INVALID_GROUP       0x15U   /* Non existing group */
#define ADC_E_WRONG_CONV_MODE           0x16U   /* API called for a group configured for continuous conversion mode.  */
#define ADC_E_WRONG_TRIGG_SRC           0x17U   /* API call not allowed according group configuration. */
#define ADC_E_NOTIF_CAPABILITY          0x18U   /* API called and notification function pointer is NULL. */
#define ADC_E_BUFFER_UNINIT             0x19U   /* API called while result buffer pointer is not initialized. */
#define ADC_E_POWER_STATE_NOT_SUPPORTED 0x1BU   /* API call with unsupported power state request. */
#define ADC_E_PERIPHERAL_NOT_PREPARED   0x1DU   /* AD not prepared for requested target power state. */

/*Runtime Errors*/

#define ADC_E_BUSY                     0x0BU   /*  API is called while another conversion is already running, 
                                                 a HW trigger is already enabled, 
                                                 a request is already stored in the queue. */
#define ADC_E_IDLE                     0x0CU   /* API is called while group is in state ADC_IDLE or
                                                 non enabled group. */
#define ADC_E_NOT_DISENGAGED           0x1AU   /* API called while one or more ADC groups are not
                                                 in IDLE state. */
#define ADC_E_TRANSITION_NOT_POSSIBLE  0x1CU   /* Requested power state can not be reached. */

/****************************************************************************************
*                              FUNCTION ID HEX                                         *
****************************************************************************************/
#define ADC_INIT_ID                         0x00U   /*!< Function ID for Adc_Init */
#define ADC_DEINIT_ID                       0x01U   /*!< Function ID for Adc_DeInit */
#define ADC_SETUP_RESULT_BUFFER_ID          0x0CU   /*!< Function ID for Adc_SetupResultBuffer */
#define ADC_START_GROUP_CONVERSION_ID       0x02U   /*!< Function ID for Adc_StartGroupConversion */
#define ADC_STOP_GROUP_CONVERSION_ID        0x03U   /*!< Function ID for Adc_StopGroupConversion */
#define ADC_READ_GROUP_ID                   0x04U   /*!< Function ID for Adc_ReadGroup */
#define ADC_ENABLE_HARDWARE_TRIGGER_ID      0x05U   /*!< Function ID for Adc_EnableHardwareTrigger */
#define ADC_DISABLE_HARDWARE_TRIGGER_ID     0x06U   /*!< Function ID for Adc_DisableHardwareTrigger */
#define ADC_ENABLE_GROUP_NOTIFICATION_ID    0x07U   /*!< Function ID for Adc_EnableGroupNotification */
#define ADC_DISABLE_GROUP_NOTIFICATION_ID   0x08U   /*!< Function ID for Adc_DisableGroupNotification */
#define ADC_GET_GROUP_STATUS_ID             0x09U   /*!< Function ID for Adc_GetGroupStatus */
#define ADC_GET_STREAM_LAST_POINTER_ID      0X0BU   /*!< Function ID for Adc_GetStreamLastPointer */
#define ADC_GET_VERSION_INFO_ID             0X0AU   /*!< Function ID for Adc_GetVersionInfo */
#define ADC_SET_POWER_STATE_ID              0x10U   /*!< Function ID for Adc_SetPowerState */
#define ADC_GET_CURRENT_POWER_STATE_ID      0x11U   /*!< Function ID for Adc_GetCurrentPowerState */
#define ADC_GET_TARGET_POWER_STATE_ID       0x12U   /*!< Function ID for Adc_GetTargetPowerState */
#define ADC_PREPARE_POWER_STATE_ID          0x0DU   /*!< Function ID for Adc_PreparePowerState */

/****************************************************************************************
*                              GLOBAL VARIABLES                                        *
****************************************************************************************/
extern Adc_HwUnitDefType Adc_HwUnitConfig[ADC_MAX_HW_UNITS];
extern Adc_GroupDefType Adc_GroupConfig[ADC_MAX_GROUPS];
/****************************************************************************************
*                              FUNCTION PROTOTYPES                                     *
****************************************************************************************/
/* Core ADC Functions */
/* Initialization and deinitialization functions */
/* Group conversion functions */
/* Channel reading functions */
/* Notification functions */
/* Status retrieval functions */

/* Power Management Functions */
/* Power state control functions */
/* Power state retrieval functions */

/* Utility Functions */
/* Version information retrieval */
/* Background processing function */
/**
 * @brief   Initializes the ADC hardware units and driver.
 * 
 * @param[in] ConfigPtr:  Pointer to configuration set in VariantPB 
 *                        (VariantP requires a NULL_PTR).
 * @return  void
 * @note 
 * @reqs    SWS_Adc_00365
 */
void Adc_Init(const Adc_ConfigType* ConfigPtr);

/**
 * @brief   Sets up the result buffer for ADC group conversions. 
 *          Must be called after reset and before starting conversions. 
 *          Application must ensure buffer size is sufficient.
 * 
 * @param[in] Group         Numeric ID of requested ADC channel group.
 * @param[in] DataBufferPtr Pointer to result buffer
 * @return  Std_ReturnType E_OK if successful, E_NOT_OK otherwise
 * @reqs    SWS_Adc_91000
 */
Std_ReturnType Adc_SetupResultBuffer (Adc_GroupType Group,
                                      Adc_ValueGroupType* DataBufferPtr);


/**

 * @brief   Returns all ADC HW Units to a state comparable to their power on reset state.
 * @return  void
 * @reqs    SWS_Adc_00366
 */
void Adc_DeInit (void);


/**
 * @brief   Starts the conversion of all channels of the requested ADC Channel group.
 * 
 * @param[in] Group: Numeric ID of the ADC channel group to start conversion.
 * @return  void
 * @note    This function initiates the conversion process for the specified group.
 * @reqs    SWS_Adc_00508
 */
void Adc_StartGroupConversion (Adc_GroupType Group);

/**
 * @brief  Stops the conversion of the requested ADC Channel group.
 * 
 * @param[in] Group: Numeric ID of the ADC channel group to stop conversion.
 * @return void
 * @note This function stops the ongoing conversion process for the specified group.
 * @reqs SWS_Adc_00509
 */
void Adc_StopGroupConversion (Adc_GroupType Group);

/**
 * @brief Reads the group conversion result of the last completed conversion round of the requested
 group and stores the channel values starting at the DataBufferPtr address. The group channel
 values are stored in ascending channel number order ( in contrast to the storage layout of the
 result buffer if streaming access is configured).
 * 
 * @param[in]   Group: Numeric ID of requested ADC channel group
 * @param[out]  DataBufferPtr:  AD results of all channels of 
 *                              the selected group are stored in
 *                              the data buffer addressed with the pointer.
 * @return  Std_ReturnType
 *          E_OK: results are available and written to the data buffer
 *          E_NOT_OK: results are not available or an error occurred
 * @reqs   SWS_Adc_00369
 */
Std_ReturnType Adc_ReadGroup (Adc_GroupType Group,
                               Adc_ValueGroupType* DataBufferPtr);

/**
 * @brief  Enables the hardware trigger for the requested ADC Channel group
 * @param[in] Group: Numeric ID of the ADC channel group
 * @return  void
 * @reqs   SWS_Adc_91001
 */
void Adc_EnableHardwareTrigger (Adc_GroupType Group);

/**
 * @brief  Disables the hardware trigger for the requested ADC Channel group
 * @param[in] Group: Numeric ID of the ADC channel group
 * @return  void
 * @reqs   SWS_Adc_91002
 */
void Adc_DisableHardwareTrigger (Adc_GroupType Group);


/**
 * @brief   Enables the notification mechanism for the requested ADC Channel group.
 * 
 * @param[in] Group: Numeric ID of the ADC channel group
 * @return void
 * @reqs    SWS_Adc_91003
 */
void Adc_EnableGroupNotification (Adc_GroupType Group);

/**
 * @brief   Disables the notification mechanism for the requested ADC Channel group.
 * 
 * @param[in] Group: Numeric ID of the ADC channel group
 * @return void
 * @reqs    SWS_Adc_91004
 */
void Adc_DisableGroupNotification (Adc_GroupType Group);

/**
 * @brief   Returns the current status of the requested ADC Channel group.
 * 
 * @param[in] Group: Numeric ID of the ADC channel group
 * @return Adc_StatusType:  Conversion status for the requested group.
 * @reqs    SWS_Adc_00374
 */
Adc_StatusType Adc_GetGroupStatus (Adc_GroupType Group);

/**
 * @brief   Returns the number of valid samples per channel, stored in the result buffer. 
 *          Reads a pointer, pointing to a position in the group result buffer. 
 *          With the pointer position, the results of all group channels 
 *          of the last completed conversion round can be accessed. 
 *          With the pointer and the return value, 
 *          all valid group conversion results can be accessed 
 *          (the user has to take the layout of the result buffer into account).
 *
 * @param[in] Group : Numeric ID of the ADC channel group
 * @param[out] PtrToSamplePtr : Pointer to result buffer pointer 
 * @return Adc_StreamNumSampleType : Number of valid samples per channel.
 * @reqs    SWS_Adc_00375
 */
Adc_StreamNumSampleType Adc_GetStreamLastPointer (Adc_GroupType Group,
                                                  Adc_ValueGroupType** PtrToSamplePtr);

/**
 * @brief   Returns the version information of the ADC driver.
 * 
 * @param[out] versioninfo: Pointer to a structure to hold the version information.
 * @return void
 * @reqs    SWS_Adc_00376
 */
void Adc_GetVersionInfo (Std_VersionInfoType* versioninfo);

/**
 * @brief   Main function for deferred ADC processing
 * @return  void
 */
void Adc_MainFunction(void);

/**
 * @brief   This API configures the Adc module so that 
 *          it enters the already prepared power state, chosen
 *          between a predefined set of configured ones.
 * 
 * @param[out] Result: If the API returns E_OK: ADC_SERVICE_ACCEPTED: Power
 *                     state change executed.
 *                     If the API returns E_NOT_OK: ADC_NOT_INIT: ADC Module not
 *                     initialized. ADC_SEQUENCE_ERROR: wrong API call sequence.
 *                     ADC_HW_FAILURE: the HW module has a failure which
 *                     prevents it to enter the required power state.
 * @return Std_ReturnType: E_OK if Power Mode Changed,
 *                         E_NOT_OK requested rejected
 * @reqs    SWS_Adc_00475
 */
Std_ReturnType Adc_SetPowerState (Adc_PowerStateRequestResultType* Result);

/**
 * @brief   This API returns the current power state of the ADC HW unit
 * 
 * @param[out] CurrentPowerState : The current power mode of the ADC HW Unit
 *                                 is returned in this parameter
 * @param[out] Result : If the API returns E_OK: ADC_SERVICE_ACCEPTED: Current
 *                                         power mode was returned.
 *                      If the API returns E_NOT_OK: ADC_NOT_INIT: ADC Module not
 *                                         initialized.
 * @return Std_ReturnType : E_OK: Mode could be read
 *                          E_NOT_OK: Service is rejected
 * @reqs    SWS_Adc_00476
 */
Std_ReturnType Adc_GetCurrentPowerState (Adc_PowerStateType* CurrentPowerState,
                                        Adc_PowerStateRequestResultType* Result);

/**
 * @brief   This API returns the target power state of the ADC HW unit
 * 
 * @param[out] TargetPowerState : The target power mode of the ADC HW Unit
 * @param[out] Result : If the API returns E_OK: ADC_SERVICE_ACCEPTED: Target
 *                                         power mode was returned.
 *                      If the API returns E_NOT_OK: ADC_NOT_INIT: ADC Module not
 *                                         initialized.
 * @return Std_ReturnType : E_OK: Mode could be read
 *                          E_NOT_OK: Service is rejected
 * @reqs    SWS_Adc_00477
 */
Std_ReturnType Adc_GetTargetPowerState( Adc_PowerStateType* TargetPowerState,
                                        Adc_PowerStateRequestResultType* Result);

/**
 * @brief Construct a new Std_ReturnTypeAdc_PreparePowerState object
 *
 * @param[in] PowerState : The target power state intended to be attained
 * @param[out] Result : - E_OK: ADC_SERVICE_ACCEPTED: ADC
 *                     Module power state preparation was started.
 *                     - E_NOT_OK: 
 *                      - ADC_NOT_INIT: ADC Module not initialized. 
 *                      - ADC_SEQUENCE_ERROR: wrong API call sequence
 *                     (Current Power State = Target Power State). 
 *                      - ADC_POWER_STATE_NOT_SUPP: ADC Module does not support the
 *                     requested power state. 
 *                      - ADC_TRANS_NOT_POSSIBLE: ADC Module cannot transition 
 *                     directly from the current power state to the requested 
 *                     power state or the HW peripheral is still busy.
 * @return  Std_ReturnType : E_OK if Preparation process started,
 *                          E_NOT_OK Service is rejected
 * @reqs    SWS_Adc_00478
 */
Std_ReturnType Adc_PreparePowerState( Adc_PowerStateType PowerState,
                                     Adc_PowerStateRequestResultType* Result);









#endif /* ADC_H */

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/