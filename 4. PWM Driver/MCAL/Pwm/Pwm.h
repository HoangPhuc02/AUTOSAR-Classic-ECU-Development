/****************************************************************************************
*                                 PWM.H                                                *
****************************************************************************************
* File Name   : Pwm.h
* Module      : Pulse Width Modulation (PWM)
* Description : AUTOSAR PWM driver main header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef PWM_H
#define PWM_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Std_Types.h"
#include "Pwm_Types.h"
#include "Pwm_Cfg.h"

/****************************************************************************************
*                              VERSION INFORMATION                                     *
****************************************************************************************/
#define PWM_VENDOR_ID               43      /*!< PWM Driver Vendor ID */
#define PWM_MODULE_ID               121     /*!< PWM Driver Module ID */
#define PWM_INSTANCE_ID             0       /*!< PWM Driver Instance ID */

#define PWM_SW_MAJOR_VERSION        2       /*!< PWM Driver Major Version */
#define PWM_SW_MINOR_VERSION        0       /*!< PWM Driver Minor Version */
#define PWM_SW_PATCH_VERSION        0       /*!< PWM Driver Patch Version */

/* AUTOSAR Release Version */
#define PWM_AR_RELEASE_MAJOR_VERSION    4
#define PWM_AR_RELEASE_MINOR_VERSION    4
#define PWM_AR_RELEASE_REVISION_VERSION 0

/****************************************************************************************
*                              SERVICE IDS                                             *
****************************************************************************************/
#define PWM_INIT_ID                    0x00    /*!< Service ID for Pwm_Init */
#define PWM_DEINIT_ID                  0x01    /*!< Service ID for Pwm_DeInit */
#define PWM_SET_DUTY_CYCLE_ID          0x02    /*!< Service ID for Pwm_SetDutyCycle */
#define PWM_SET_PERIOD_AND_DUTY_ID     0x03    /*!< Service ID for Pwm_SetPeriodAndDuty */
#define PWM_SET_OUTPUT_TO_IDLE_ID      0x04    /*!< Service ID for Pwm_SetOutputToIdle */
#define PWM_GET_OUTPUT_STATE_ID        0x05    /*!< Service ID for Pwm_GetOutputState */
#define PWM_DISABLE_NOTIFICATION_ID    0x06    /*!< Service ID for Pwm_DisableNotification */
#define PWM_ENABLE_NOTIFICATION_ID     0x07    /*!< Service ID for Pwm_EnableNotification */
#define PWM_GET_VERSION_INFO_ID        0x08    /*!< Service ID for Pwm_GetVersionInfo */

/* NOT USED */
#define PWM_SET_POWER_STATE_ID          0x09    /*!< Service ID for Pwm_SetPowerState */
#define PWM_GET_CURRENT_POWER_STATE_ID  0x0A    /*!< Service ID for Pwm_GetCurrentPowerState */
#define PWM_GET_TARGET_POWER_STATE_ID   0x0B    /*!< Service ID for Pwm_GetTargetPowerState */
#define PWM_PREPARE_POWER_STATE_ID      0x0C    /*!< Service ID for Pwm_PreparePowerState */
/****************************************************************************************
*                              DEVELOPMENT ERROR CODES                                 *
****************************************************************************************/
#define PWM_E_INT_FAILED                0x10    /*!<  API Pwm_Init service called with wrong parameter*/
#define PWM_E_UNINIT                    0x11    /*!< API called without module initialization */
#define PWM_E_PARAM_CHANNEL             0x12    /*!< API called with invalid channel identifier */
#define PWM_E_PERIOD_UNCHANGEABLE       0x13    /*!< Usage of unauthorized PWM service on PWM channel configured as fixed period */
#define PWM_E_ALREADY_INITIALIZED       0x14    /*!< PWM already initialized */
#define PWM_E_PARAM_POINTER             0x15    /*!< API called with invalid pointer */
// these error codes don't exist
// TODO remove check and remove
#define PWM_E_PARAM_VALUE               0x16    /*!< API called with invalid parameter value */
#define PWM_E_TIMEOUT                   0x17    /*!< Operation timeout */
#define PWM_E_HW_FAILURE                0x18    /*!< Hardware failure */

/*!< NOT USED */
#define PWM_E_POWER_STATE_NOT_SUPPORTED 0x17  /*!<  The requested power state is not supported by the PWM module. */
#define PWM_E_TRANSITION_NOT_POSSIBLE    0x18 /*!<  The requested power state is not reachable from the current one*/
#define PWM_E_PERIPHERAL_NOT_PREPARED     0x19  /*!<  API Pwm_SetPowerState has been called without having called the API Pwm_PreparePowerState before.*/

/****************************************************************************************
*                              GLOBAL VARIABLES                                        *
****************************************************************************************/

/* Driver state */
extern Pwm_DriverStateType Pwm_DriverState;

/* Configuration pointer */
extern const Pwm_ConfigType* Pwm_ConfigPtr;

/****************************************************************************************
*                                 CORE API FUNCTIONS                                  *
****************************************************************************************/
/**
 * @brief PWM notification handler
 * @details Called from interrupt service routine when PWM notification occurs
 * @param[in] ChannelNumber Channel identifier that generated the notification
 * @return void
 */
void Pwm_NotificationHandler(Pwm_HwUnitType HwUnit, uint16 TIM_IT);

/* === INITIALIZATION === */
/**
 * @brief Service for PWM initialization
 * @details [SWS_Pwm_00095] Definition of API function Pwm_Init
 * @param[in] ConfigPtr Pointer to configuration set
 * @return void
 * @ServiceID 0x00
 * @Sync Synchronous
 * @Reentrancy Non Reentrant
 */
void Pwm_Init(const Pwm_ConfigType* ConfigPtr);

/**
 * @brief Service for PWM De-Initialization
 * @details [SWS_Pwm_00096] Definition of API function Pwm_DeInit
 * @return void
 * @ServiceID 0x01
 * @Sync Synchronous
 * @Reentrancy Non Reentrant
 */
void Pwm_DeInit(void);

/* === BASIC CONTROL === */
/**
 * @brief Service to set the duty cycle of the PWM channel
 * @details [SWS_Pwm_00097] Definition of API function Pwm_SetDutyCycle
 * @param[in] ChannelId Numeric identifier of the PWM channel
 * @param[in] DutyCycle Min=0x0000 Max=0x8000
 * @return void
 * @ServiceID 0x02
 * @Sync Synchronous
 * @Reentrancy Reentrant for different channel numbers
 */
void Pwm_SetDutyCycle(Pwm_ChannelType ChannelNumber, uint16 DutyCycle);

/**
 * @brief Service to set the period and the duty cycle of a PWM channel
 * @details [SWS_Pwm_00098] Definition of API function Pwm_SetPeriodAndDuty
 * @param[in] ChannelId Numeric identifier of the PWM channel
 * @param[in] Period Period of the PWM signal
 * @param[in] DutyCycle Min=0x0000 Max=0x8000
 * @return void
 * @ServiceID 0x03
 * @Sync Synchronous
 * @Reentrancy Reentrant for different channel numbers
 */
void Pwm_SetPeriodAndDuty(Pwm_ChannelType ChannelNumber, 
                          Pwm_PeriodType Period, 
                          uint16 DutyCycle);

/**
 * @brief Service to set the PWM output to the configured Idle state
 * @details [SWS_Pwm_00099] Definition of API function Pwm_SetOutputToIdle
 * @param[in] ChannelId Numeric identifier of the PWM channel
 * @return void
 * @ServiceID 0x04
 * @Sync Synchronous
 * @Reentrancy Reentrant for different channel numbers
 */
void Pwm_SetOutputToIdle(Pwm_ChannelType ChannelNumber);

/**
 * @brief Service to read the internal state of the PWM output signal
 * @details [SWS_Pwm_00100] Definition of API function Pwm_GetOutputState
 * @param[in] ChannelId Numeric identifier of the PWM channel
 * @return PWM_HIGH The PWM output state is high
 * @return PWM_LOW The PWM output state is low
 * @ServiceID 0x05
 * @Sync Synchronous
 * @Reentrancy Reentrant
 */
Pwm_OutputStateType Pwm_GetOutputState(Pwm_ChannelType ChannelNumber);
#if (PWM_NOTIFICATION_SUPPORTED == STD_ON)
/**
 * @brief Service to disable the PWM signal edge notification
 * @details [SWS_Pwm_00101] Definition of API function Pwm_DisableNotification
 * @param[in] ChannelNumber Numeric identifier of the PWM channel
 * @return void
 * @ServiceID 0x06
 * @Sync Synchronous
 * @Reentrancy Reentrant for different channel numbers
 */
void Pwm_DisableNotification(Pwm_ChannelType ChannelNumber);

/**
 * @brief Service to enable the PWM signal edge notification according to notification parameter
 * @details [SWS_Pwm_00102] Definition of API function Pwm_EnableNotification
 * @param[in] ChannelNumber Numeric identifier of the PWM channel
 * @param[in] Notification Type of notification
 * @return void
 * @ServiceID 0x07
 * @Sync Synchronous
 * @Reentrancy Reentrant for different channel numbers
 */
void Pwm_EnableNotification(Pwm_ChannelType ChannelNumber, Pwm_EdgeNotificationType Notification);
#endif

/****************************************************************************************
*                              UTILITY FUNCTIONS                                       *
****************************************************************************************/

/**
 * @brief Convert percentage to duty cycle
 * @param[in] Percentage 0-100%
 * @return Duty cycle value (0x0000-0x8000)
 */
uint16 Pwm_PercentageToDutyCycle(uint8 Percentage);

/**
 * @brief Convert duty cycle to percentage
 * @param[in] DutyCycle Duty cycle value (0x0000-0x8000)
 * @return Percentage 0-100%
 */
uint8 Pwm_DutyCycleToPercentage(uint16 DutyCycle);

/**
 * @brief Get version information
 * @param[out] versioninfo Version information structure
 */
void Pwm_GetVersionInfo(Std_VersionInfoType* versioninfo);



#endif /* PWM_H */

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
