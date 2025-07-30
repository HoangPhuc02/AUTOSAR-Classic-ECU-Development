/****************************************************************************************
*                                ADC.C                                                  *
****************************************************************************************
* File Name   : Adc.c
* Module      : Analog to Digital Converter (ADC)
* Description : AUTOSAR ADC driver source file 
* Version     : 3.1.0 - Redesigned for clarity and maintainability
* Date        : 24/06/2025
* Update      : 23/07/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Adc.h"
#include "Det.h"
#include "Adc_Cfg.h"
#include "Adc_Hw.h"
#include "Adc_Types.h"
#include "stm32f10x_adc.h"
#include "misc.h"

/* Include error detection if enabled */
#if (ADC_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif

/****************************************************************************************
*                                 GLOBAL VARIABLES                                     *
****************************************************************************************/
typedef enum
{
    ADC_DRIVER_STATE_UNINIT = 0,   /*!< Driver uninitialized */
    ADC_DRIVER_STATE_INITIALIZED,   /*!< Driver initialized */
} Adc_DriverStateType;
/* Driver state */
static Adc_DriverStateType Adc_DriverState = ADC_DRIVER_STATE_UNINIT;

/* Configuration pointer */
static const Adc_ConfigType* Adc_ConfigPtr = NULL_PTR;

/* Runtime data arrays */
/* Update to private only can be controlled by ADC_HW*/
// Adc_RuntimeGroupType Adc_RuntimeGroups[ADC_MAX_GROUPS];
// Adc_RuntimeHwUnitType Adc_RuntimeHwUnits[ADC_MAX_HW_UNITS];

/* Result buffers */
Adc_ValueGroupType Adc_ResultBuffer[ADC_MAX_GROUPS][ADC_MAX_BUFFER_SIZE];

/* Performance counters */
#if (ADC_ENABLE_DEBUG_SUPPORT == STD_ON)
Adc_PerformanceCountersType Adc_PerformanceCounters;
#endif

/****************************************************************************************
*                                 STATIC FUNCTION PROTOTYPES                          *
****************************************************************************************/
#if(ADC_DEV_ERROR_DETECT == STD_ON)
static Std_ReturnType Adc_ValidateInit(uint8 ApiId);
static Std_ReturnType Adc_ValidateGroup(Adc_GroupType Group, uint8 ApiId);
static Std_ReturnType Adc_ValidatePointer(const void* Ptr, uint8 ApiId);
static Std_ReturnType Adc_ValidateGroupForStart(Adc_GroupType Group);
static Std_ReturnType Adc_ValidateGroupForStop(Adc_GroupType Group);
#endif
static void Adc_InitializeRuntimeData(void);
static void Adc_ResetRuntimeData(void);


static void Adc_HandleGroupCompletion(Adc_GroupType Group);
static void Adc_UpdateGroupStatus(Adc_GroupType Group, Adc_StatusType NewStatus);

/****************************************************************************************
*                                 INITIALIZATION FUNCTIONS                            *
****************************************************************************************/
/**
 * @brief   Initializes the ADC hardware units and driver
 * @param[in] ConfigPtr Pointer to configuration set
 * @return  void
 * @reqs    SWS_Adc_00365
 */
void Adc_Init(const Adc_ConfigType* ConfigPtr)
{
#if(ADC_DEV_ERROR_DETECT == STD_ON)
    /* Development error detection */
    if (Adc_DriverState != ADC_DRIVER_STATE_UNINIT)
    {
        Det_ReportError(ADC_MODULE_ID, 0, ADC_INIT_ID, ADC_E_ALREADY_INITIALIZED);
        return;
    }
    
    if (ConfigPtr == NULL_PTR)
    {
        Det_ReportError(ADC_MODULE_ID, 0, ADC_INIT_ID, ADC_E_PARAM_CONFIG);
        return;
    }
#endif
    /* Store configuration pointer */
    Adc_ConfigPtr = ConfigPtr;
    
    /* Initialize runtime data */
    Adc_InitializeRuntimeData();
    
    /* Initialize hardware units */
    for (Adc_HwUnitType i = 0; i < ConfigPtr->NumHwUnits; i++)
    {
        /* Initialize hardware unit */
        if (AdcHw_Init(i) != E_OK)
        {
            /* Reset driver state on failure */
            Adc_DriverState = ADC_DRIVER_STATE_UNINIT;
            return;
        }
    }
    for (Adc_GroupType i = 0; i < ConfigPtr->NumGroups; i++)
    {
        Adc_DisableGroupNotification(i);
    }

    /* Initialize performance counters */
    #if (ADC_ENABLE_DEBUG_SUPPORT == STD_ON)
    memset(&Adc_PerformanceCounters, 0, sizeof(Adc_PerformanceCountersType));
    #endif
    
    /* Set driver state to initialized */
    Adc_DriverState = ADC_DRIVER_STATE_INITIALIZED;
}

/**
 * @brief   Returns all ADC HW Units to power-on reset state
 * @return  void
 * @reqs    SWS_Adc_00366
 */
void Adc_DeInit(void)
{
    /* Validate driver state */
    if (Adc_ValidateInit(ADC_DEINIT_ID) != E_OK)
    {
        return;
    }
    
    /* Check if all groups are idle */
    for (Adc_GroupType i = 0; i < ADC_MAX_GROUPS; i++)
    {
        if ((AdcHw_GetGroupRuntimeStatus(i) != ADC_IDLE) && \
            (AdcHw_GetGroupRuntimeStatus(i) != ADC_STREAM_COMPLETED))
        {
            #if (ADC_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError(ADC_MODULE_ID, 0, ADC_DEINIT_ID, ADC_E_BUSY);
            #endif
            return;
        }
    }
    
    /* Deinitialize hardware units */
    for (uint8 i = 0; i < Adc_ConfigPtr->NumHwUnits; i++)
    {
        AdcHw_DeInit(i);
    }
    
    /* Reset runtime data */
    Adc_ResetRuntimeData();
    
    /* Reset driver state */
    Adc_DriverState = ADC_DRIVER_STATE_UNINIT;
    Adc_ConfigPtr = NULL_PTR;
}

/**
 * @brief   Sets up the result buffer for ADC group conversions
 * @param[in] Group Numeric ID of requested ADC channel group
 * @param[in] DataBufferPtr Pointer to result buffer
 * @return  E_OK if successful, E_NOT_OK otherwise
 * @reqs    SWS_Adc_91000
 */
Std_ReturnType Adc_SetupResultBuffer(Adc_GroupType Group,
                                     Adc_ValueGroupType* DataBufferPtr)
{
    /* Validate parameters */
    if (Adc_ValidateInit(ADC_SETUP_RESULT_BUFFER_ID) != E_OK)
    {
        return E_NOT_OK;
    }
    
    if (Adc_ValidateGroup(Group, ADC_SETUP_RESULT_BUFFER_ID) != E_OK)
    {
        return E_NOT_OK;
    }
    
    if (Adc_ValidatePointer(DataBufferPtr, ADC_SETUP_RESULT_BUFFER_ID) != E_OK)
    {
        return E_NOT_OK;
    }
    
    /* Check if group is idle */
    if (AdcHw_GetGroupRuntimeStatus(Group) != ADC_IDLE)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_SETUP_RESULT_BUFFER_ID, ADC_E_BUSY);
        #endif
        return E_NOT_OK;
    }

    /* Set up result buffer */
    // reset buffer
    for(uint16 i; i < Adc_GroupConfig[Group].Adc_ValueResultSize; i++)
    {
        Adc_GroupConfig[Group].Adc_ValueResultPtr[i] = 0;
    }
    // Allow group conversion
    Adc_GroupConfig[Group].Adc_SetupBufferFlag = 1;
    return E_OK;
}

/****************************************************************************************
*                                 CONVERSION CONTROL FUNCTIONS                        *
****************************************************************************************/
/**
 * @brief   Starts conversion of all channels in the requested ADC group
 * @param[in] Group Numeric ID of the ADC channel group
 * @return  void
 * @reqs    SWS_Adc_00508
 */
void Adc_StartGroupConversion(Adc_GroupType Group)
{
    /* Validate parameters */
    Adc_ValidateInit(ADC_START_GROUP_CONVERSION_ID);
    Adc_ValidateGroup(Group, ADC_START_GROUP_CONVERSION_ID);
    
    /* Validate group for start */
    if (Adc_ValidateGroupForStart(Group) != E_OK)
    {
        return;
    }

    /* Get group configuration */
    const Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[Group];
    Adc_HwUnitType HwUnit = GroupConfig->Adc_HwUnitId;
    if (GroupConfig->Adc_TriggerSource == ADC_TRIGG_SRC_HW)
    {
        return;
    }
    /* Start conversion based on configuration */

    /* Software triggered conversion */
    if (AdcHw_StartSwConversion(HwUnit, Group) == E_OK)
    {
        // do something
    }

}

/**
 * @brief   Stops conversion of the requested ADC Channel group
 * @param[in] Group Numeric ID of the ADC channel group
 * @return  void
 * @reqs    SWS_Adc_00509
 */
void Adc_StopGroupConversion(Adc_GroupType Group)
{
    /* Validate parameters */
    Adc_ValidateInit(ADC_STOP_GROUP_CONVERSION_ID);
    Adc_ValidateGroup(Group, ADC_STOP_GROUP_CONVERSION_ID);
    
    /* Validate group for stop */
    if (Adc_ValidateGroupForStop(Group) != E_OK)
    {
        return;
    }
    
    /* Get group configuration */
    const Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[Group];
    Adc_HwUnitType HwUnit = GroupConfig->Adc_HwUnitId;
    
    /* Stop conversion based on configuration */

    /* Software triggered conversion */
    if (AdcHw_StopSwConversion(HwUnit, Group) == E_OK)
    {
        Adc_UpdateGroupStatus(Group, ADC_IDLE);
    }
    

}

/**
 * @brief   Reads the group conversion result of the last completed conversion
 * @param[in] Group Numeric ID of requested ADC channel group
 * @param[out] DataBufferPtr Pointer to buffer for storing results
 * @return  E_OK if results available, E_NOT_OK otherwise
 * @reqs    SWS_Adc_00369
 */
Std_ReturnType Adc_ReadGroup(Adc_GroupType Group,
                             Adc_ValueGroupType* DataBufferPtr)
{
    /* Validate parameters */
    if (Adc_ValidateInit(ADC_READ_GROUP_ID) != E_OK)
    {
        return E_NOT_OK;
    }
    
    if (Adc_ValidateGroup(Group, ADC_READ_GROUP_ID) != E_OK)
    {
        return E_NOT_OK;
    }
    
    if (Adc_ValidatePointer(DataBufferPtr, ADC_READ_GROUP_ID) != E_OK)
    {
        return E_NOT_OK;
    }
    
    /* Check if results are available */
    if ((Adc_GetGroupStatus(Group) == ADC_IDLE) || 
        (Adc_GetGroupStatus(Group) == ADC_BUSY))
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_READ_GROUP_ID, ADC_E_IDLE);
        #endif
        return E_NOT_OK;
    }
    
    /* Get group configuration */
    const Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[Group];
    Adc_HwUnitType HwUnit = GroupConfig->Adc_HwUnitId;
    
    /* Read results */
    if (AdcHw_ReadResult(HwUnit, Group, DataBufferPtr) == E_NOT_OK)
    {
        return E_NOT_OK;
    }
    return E_OK;
}

/****************************************************************************************
*                                 HARDWARE TRIGGER FUNCTIONS                          *
****************************************************************************************/
/**
 * @brief   Enables the hardware trigger for the requested ADC Channel group
 * @param[in] Group Numeric ID of the ADC channel group
 * @return  void
 * @reqs    SWS_Adc_91001
 */
void Adc_EnableHardwareTrigger(Adc_GroupType Group)
{
    /* Validate parameters */
    Adc_ValidateInit(ADC_ENABLE_HARDWARE_TRIGGER_ID);
    Adc_ValidateGroup(Group, ADC_ENABLE_HARDWARE_TRIGGER_ID);
    
    /* Check if group is configured for hardware trigger */
    // const Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[Group];
    const Adc_GroupDefType* GroupConfig = &Adc_ConfigPtr->Groups[Group];
    if (GroupConfig->Adc_TriggerSource == ADC_TRIGG_SRC_SW)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_ENABLE_HARDWARE_TRIGGER_ID, ADC_E_WRONG_TRIGG_SRC);
        #endif
        return;
    }
    
    /* Check if group is idle */
    if (AdcHw_GetGroupRuntimeStatus(Group) != ADC_IDLE)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_ENABLE_HARDWARE_TRIGGER_ID, ADC_E_BUSY);
        #endif
        return;
    }
    
    /* Enable hardware trigger */
    Adc_HwUnitType Unit = Adc_ConfigPtr->Groups[Group].Adc_HwUnitId;
    if (AdcHw_StartHwConversion(Unit, Group) == E_OK)
    {
        Adc_UpdateGroupStatus(Group, ADC_BUSY);
    }
}

/**
 * @brief   Disables the hardware trigger for the requested ADC Channel group
 * @param[in] Group Numeric ID of the ADC channel group
 * @return  void
 * @reqs    SWS_Adc_91002
 */
void Adc_DisableHardwareTrigger(Adc_GroupType Group)
{
    /* Validate parameters */
    Adc_ValidateInit(ADC_DISABLE_HARDWARE_TRIGGER_ID);
    Adc_ValidateGroup(Group, ADC_DISABLE_HARDWARE_TRIGGER_ID);
    
    /* Check if group is configured for hardware trigger */
    // const Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[Group];
    const Adc_GroupDefType* GroupConfig = &Adc_ConfigPtr->Groups[Group];
    if (GroupConfig->Adc_TriggerSource == ADC_TRIGG_SRC_SW)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_DISABLE_HARDWARE_TRIGGER_ID, ADC_E_WRONG_TRIGG_SRC);
        #endif
        return;
    }
    
    /* Disable hardware trigger */
    Adc_HwUnitType Unit = Adc_ConfigPtr->Groups[Group].Adc_HwUnitId;
    if (AdcHw_StopHwConversion(Unit, Group) == E_OK)
    {
        Adc_UpdateGroupStatus(Group, ADC_IDLE);
    }
}

/****************************************************************************************
*                                 NOTIFICATION FUNCTIONS                              *
****************************************************************************************/
/**
 * @brief   Enables the notification mechanism for the requested ADC Channel group
 * @param[in] Group Numeric ID of the ADC channel group
 * @return  void
 * @reqs    SWS_Adc_91003
 */

void Adc_EnableGroupNotification(Adc_GroupType Group)
{
    /* Validate parameters */
    Adc_ValidateInit(ADC_ENABLE_GROUP_NOTIFICATION_ID);
    Adc_ValidateGroup(Group, ADC_ENABLE_GROUP_NOTIFICATION_ID);
    
    /* Check if notification is configured */
    // const Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[Group];
    const Adc_GroupDefType* GroupConfig = &Adc_ConfigPtr->Groups[Group];
    if (GroupConfig->Adc_NotificationCb == NULL_PTR)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_ENABLE_GROUP_NOTIFICATION_ID, ADC_E_NOTIF_CAPABILITY);
        #endif
        return;
    }
    
    /* Enable notification */
    Adc_GroupConfig[Group].Adc_NotificationEnable = ADC_NOTIFICATION_ENABLE;
}

/**
 * @brief   Disables the notification mechanism for the requested ADC Channel group
 * @param[in] Group Numeric ID of the ADC channel group
 * @return  void
 * @reqs    SWS_Adc_91004
 */
void Adc_DisableGroupNotification(Adc_GroupType Group)
{
    /* Validate parameters */
    Adc_ValidateInit(ADC_DISABLE_GROUP_NOTIFICATION_ID);
    Adc_ValidateGroup(Group, ADC_DISABLE_GROUP_NOTIFICATION_ID);
    
    /* Disable notification */
    // TODO think about private group config and set by function
    Adc_GroupConfig[Group].Adc_NotificationEnable = ADC_NOTIFICATION_DISABLE;
}

/****************************************************************************************
*                                 STATUS FUNCTIONS                                    *
****************************************************************************************/
/**
 * @brief   Returns the current status of the requested ADC Channel group
 * @param[in] Group Numeric ID of the ADC channel group
 * @return  Conversion status for the requested group
 * @reqs    SWS_Adc_00374
 */
Adc_StatusType Adc_GetGroupStatus(Adc_GroupType Group)
{   
    return Adc_GroupConfig[Group].Adc_Status;
}

/**
 * @brief   Returns the number of valid samples per channel in result buffer
 * @param[in] Group Numeric ID of the ADC channel group
 * @param[out] PtrToSamplePtr Pointer to result buffer pointer
 * @return  Number of valid samples per channel
 * @reqs    SWS_Adc_00375
 */
Adc_StreamNumSampleType Adc_GetStreamLastPointer(Adc_GroupType Group,
                                                 Adc_ValueGroupType** PtrToSamplePtr)
{
    /* Validate parameters */
    if (Adc_ValidateInit(ADC_GET_STREAM_LAST_POINTER_ID) != E_OK)
    {
        return 0;
    }
    
    if (Adc_ValidateGroup(Group, ADC_GET_STREAM_LAST_POINTER_ID) != E_OK)
    {
        return 0;
    }
    
    if (Adc_ValidatePointer(PtrToSamplePtr, ADC_GET_STREAM_LAST_POINTER_ID) != E_OK)
    {
        return 0;
    }
    
    /* Get group configuration */
    // const Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[Group];
    const Adc_GroupDefType* GroupConfig = &Adc_ConfigPtr->Groups[Group];
    
    /* Check if streaming mode is enabled */
    if (GroupConfig->Adc_GroupAccessMode != ADC_ACCESS_MODE_STREAMING)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_GET_STREAM_LAST_POINTER_ID, ADC_E_WRONG_CONV_MODE);
        #endif
        return 0;
    }
    
    /* Return streaming information */
    // *PtrToSamplePtr = GroupConfig->Adc_ValueResultPtr[Adc_RuntimeGroups[Group].SampleCounter];
    Adc_StreamNumSampleType NbrOfSample = AdcHw_GetGroupRuntimeSampCounter(Group);
    Adc_ChannelType NbrOfChannel = GroupConfig->Adc_NbrOfChannel;
    *PtrToSamplePtr = &GroupConfig->Adc_ValueResultPtr[(NbrOfSample - 1) * NbrOfChannel];

    AdcHw_HandleReadResultState(GroupConfig->Adc_HwUnitId, Group);
    return NbrOfSample;
}

/****************************************************************************************
*                                 UTILITY FUNCTIONS                                   *
****************************************************************************************/
/**
 * @brief   Returns the version information of the ADC driver
 * @param[out] versioninfo Pointer to version information structure
 * @return  void
 * @reqs    SWS_Adc_00376
 */
void Adc_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
    /* Validate pointer */
    if (versioninfo == NULL_PTR)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_GET_VERSION_INFO_ID, ADC_E_PARAM_POINTER);
        #endif
        return;
    }
    
    /* Fill version information */
    versioninfo->vendorID = ADC_VENDOR_ID;
    versioninfo->moduleID = ADC_MODULE_ID;
    versioninfo->sw_major_version = ADC_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = ADC_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = ADC_SW_PATCH_VERSION;
}

/**
 * @brief   Calibrates the ADC hardware unit
 * @param[in] HwUnit Hardware unit to calibrate
 * @return  E_OK if successful, E_NOT_OK otherwise
 */
// Std_ReturnType Adc_Calibrate(Adc_HwUnitType HwUnit)
// {
//     /* Validate parameters */
//     if (Adc_ValidateInit(ADC_CALIBRATE_ID) != E_OK)
//     {
//         return E_NOT_OK;
//     }

//     if (!AdcHw_ValidateHwUnit(HwUnit))
//     {
//         #if (ADC_DEV_ERROR_DETECT == STD_ON)
//         Det_ReportError(ADC_MODULE_ID, 0, ADC_CALIBRATE_ID, ADC_E_PARAM_CONFIG);
//         #endif
//         return E_NOT_OK;
//     }
    
//     /* Check if hardware unit is idle */
//     if (AdcHw_IsUnitBusy(HwUnit))
//     {
//         #if (ADC_DEV_ERROR_DETECT == STD_ON)
//         Det_ReportError(ADC_MODULE_ID, 0, ADC_CALIBRATE_ID, ADC_E_BUSY);
//         #endif
//         return E_NOT_OK;
//     }
    
//     /* Perform calibration */
//     ADC_TypeDef* ADCx = ADC_HW_GET_MODULE_ID(HwUnit);
//     if (ADCx != NULL)
//     {
//         /* Start calibration */
//         ADC_StartCalibration(ADCx);
        
//         /* Wait for calibration to complete */
//         while (ADC_GetCalibrationStatus(ADCx) != RESET)
//         {
//             /* Watchdog protection could be added here */
//         }
        
//         return E_OK;
//     }
    
//     return E_NOT_OK;
// }

/****************************************************************************************
*                                 DEFERRED PROCESSING FUNCTIONS                       *
****************************************************************************************/
/**
 * @brief   Main function for deferred ADC processing
 * @return  void
 */
void Adc_MainFunction(void)
{
    /* Only process if driver is initialized */
    if (Adc_DriverState != ADC_DRIVER_STATE_INITIALIZED)
    {
        return;
    }
    
    AdcHw_MainFunction();
}


/****************************************************************************************
*                                 STATIC HELPER FUNCTIONS                             *
****************************************************************************************/
/**
 * @brief   Validates driver initialization
 * @param[in] ApiId API function ID
 * @return  E_OK if valid, E_NOT_OK otherwise
 */
static inline Std_ReturnType Adc_ValidateInit(uint8 ApiId)
{
    if (Adc_DriverState == ADC_DRIVER_STATE_UNINIT)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ApiId, ADC_E_UNINIT);
        #endif
        return E_NOT_OK;
    }
    return E_OK;
}

/**
 * @brief   Validates group ID
 * @param[in] Group Group ID to validate
 * @param[in] ApiId API function ID
 * @return  E_OK if valid, E_NOT_OK otherwise
 */
static inline Std_ReturnType Adc_ValidateGroup(Adc_GroupType Group, uint8 ApiId)
{
    if (ADC_HW_IS_VALID_GROUP(Group) == FALSE)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ApiId, ADC_E_PARAM_INVALID_GROUP);
        #endif
        return E_NOT_OK;
    }
    return E_OK;
}

/**
 * @brief   Validates pointer
 * @param[in] Ptr Pointer to validate
 * @param[in] ApiId API function ID
 * @return  E_OK if valid, E_NOT_OK otherwise
 */
static inline Std_ReturnType Adc_ValidatePointer(const void* Ptr, uint8 ApiId)
{
    if (Ptr == NULL_PTR)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ApiId, ADC_E_PARAM_POINTER);
        #endif
        return E_NOT_OK;
    }
    return E_OK;
}

/**
 * @brief   Initializes runtime data structures
 * @note    Initializes Adc_RuntimeGroups and Adc_RuntimeHwUnits for controlling state in ADC_HW
 * @return  void
 */
static inline void Adc_InitializeRuntimeData(void)
{
    /* These variable should be static in Adc HW and get set by function*/
    /* Initialize runtime groups */
    for (Adc_GroupType i = 0; i < ADC_MAX_GROUPS; i++)
    {
        AdcHw_ResetGroupRuntime(i);
    }
    
    /* Initialize runtime hardware units */
    for (Adc_HwUnitType i = 0; i < ADC_MAX_HW_UNITS; i++)
    {
        AdcHw_ResetHwRuntime(i);
    }
    
    /* Initialize result buffers */
    // memset(Adc_ResultBuffer, 0, sizeof(Adc_ResultBuffer));
}

/**
 * @brief   Resets runtime data structures
 * @return  void
 */
static void Adc_ResetRuntimeData(void)
{
    Adc_InitializeRuntimeData();
}

/**
 * @brief   Validates group for start operation
 * @param[in] Group Group ID to validate
 * @return  E_OK if valid, E_NOT_OK otherwise
 */
static Std_ReturnType Adc_ValidateGroupForStart(Adc_GroupType Group)
{
    const Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[Group];
    
    /* Check if group is idle */
    if (AdcHw_GetGroupRuntimeStatus(Group) != ADC_IDLE)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_START_GROUP_CONVERSION_ID, ADC_E_BUSY);
        #endif
        return E_NOT_OK;
    }
    
    /* Check if result buffer is configured */
    if (GroupConfig->Adc_ValueResultPtr == NULL_PTR || GroupConfig->Adc_SetupBufferFlag == 0)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_START_GROUP_CONVERSION_ID, ADC_E_BUFFER_UNINIT);
        #endif
        return E_NOT_OK;
    }
    
    /* Check trigger source for SW trigger */
    if (GroupConfig->Adc_TriggerSource != ADC_TRIGG_SRC_SW)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_START_GROUP_CONVERSION_ID, ADC_E_WRONG_TRIGG_SRC);
        #endif
        return E_NOT_OK;
    }
    
    return E_OK;
}

/**
 * @brief   Validates group for stop operation
 * @param[in] Group Group ID to validate
 * @return  E_OK if valid, E_NOT_OK otherwise
 */
static Std_ReturnType Adc_ValidateGroupForStop(Adc_GroupType Group)
{
    const Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[Group];
    
    /* Check if group is busy */
    if ((AdcHw_GetGroupRuntimeStatus(Group) == ADC_IDLE) || 
        (AdcHw_GetGroupRuntimeStatus(Group) == ADC_STREAM_COMPLETED))
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_STOP_GROUP_CONVERSION_ID, ADC_E_IDLE);
        #endif
        return E_NOT_OK;
    }
    
    /* Check trigger source for SW trigger */
    if (GroupConfig->Adc_TriggerSource != ADC_TRIGG_SRC_SW)
    {
        #if (ADC_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(ADC_MODULE_ID, 0, ADC_STOP_GROUP_CONVERSION_ID, ADC_E_WRONG_TRIGG_SRC);
        #endif
        return E_NOT_OK;
    }
    
    return E_OK;
}

/**
 * @brief   Handles group completion logic for read_group() function
 * @param[in] Group Group ID
 * @return  void
 */
// TODO Check this function
static void Adc_HandleGroupCompletion(Adc_GroupType Group)
{
    const Adc_GroupDefType* GroupConfig = &Adc_GroupConfig[Group];
    
    /* Handle status transition based on configuration */
    if (GroupConfig->Adc_GroupAccessMode == ADC_ACCESS_MODE_STREAMING)
    {
        if (GroupConfig->Adc_StreamBufferMode == ADC_STREAM_BUFFER_CIRCULAR)
        {
            /* Circular streaming - transition to BUSY */
            Adc_UpdateGroupStatus(Group, ADC_BUSY);
        }
        else
        {
            /* Linear streaming - transition to IDLE */
            Adc_UpdateGroupStatus(Group, ADC_IDLE);
        }
    }
    else
    {
        /* Single access mode */
        if (GroupConfig->Adc_GroupConvMode == ADC_CONV_MODE_CONTINUOUS)
        {
            /* Continuous mode - transition to BUSY */
            Adc_UpdateGroupStatus(Group, ADC_BUSY);
        }
        else
        {
            /* One-shot mode - transition to IDLE */
            Adc_UpdateGroupStatus(Group, ADC_IDLE);
        }
    }
}

/**
 * @brief   Updates group status
 * @param[in] Group Group ID
 * @param[in] NewStatus New status to set
 * @return  void
 */
static void Adc_UpdateGroupStatus(Adc_GroupType Group, Adc_StatusType NewStatus)
{
    // Adc_RuntimeGroups[Group].Status = NewStatus;
    AdcHw_SetGroupStatus(Group, ADC_IDLE);
    
    /* Update performance counters */
    #if (ADC_ENABLE_DEBUG_SUPPORT == STD_ON)
    switch (NewStatus)
    {
        case ADC_BUSY:
            Adc_PerformanceCounters.ConversionsStarted++;
            break;
        case ADC_COMPLETED:
            Adc_PerformanceCounters.ConversionsCompleted++;
            break;
        case ADC_STREAM_COMPLETED:
            Adc_PerformanceCounters.StreamingCompleted++;
            break;
        default:
            break;
    }
    #endif
}

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
