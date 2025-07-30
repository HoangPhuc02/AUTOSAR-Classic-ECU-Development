/****************************************************************************************
*                               PWM_TYPES.H                                            *
****************************************************************************************
* File Name   : Pwm_Types.h
* Module      : Pulse Width Modulation (PWM)
* Description : AUTOSAR PWM driver type definitions header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/


#ifndef PWM_TYPES_H
#define PWM_TYPES_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Std_Types.h"
#include "stm32f10x.h"


/****************************************************************************************
*                              TYPE DEFINITIONS                                        *
****************************************************************************************/

/**
 * @brief Numeric identifier of a PWM channel
 * @details [SWS_Pwm_00106] Definition of datatype Pwm_ChannelType
 * Range: 8..32 bit implementation specific
 */
typedef uint8 Pwm_ChannelType;

/**
 * @brief Definition of the period of a PWM channel
 * @details [SWS_Pwm_00107] Definition of datatype Pwm_PeriodType
 * Range: 8..32 bit implementation specific
 */
typedef uint16 Pwm_PeriodType;

/**
 * @brief Output state of a PWM channel
 * @details [SWS_Pwm_00108] Definition of datatype Pwm_OutputStateType
 */
typedef enum
{
    PWM_HIGH = 0x00,    /*!< The PWM channel is in high state */
    PWM_LOW  = 0x01     /*!< The PWM channel is in low state */
} Pwm_OutputStateType;

/**
 * @brief Definition of the type of edge notification of a PWM channel
 * @details [SWS_Pwm_00109] Definition of datatype Pwm_EdgeNotificationType
 */
typedef enum
{
    PWM_RISING_EDGE  = 0x00,    /*!< Notification on rising edge */
    PWM_FALLING_EDGE = 0x01,    /*!< Notification on falling edge */
    PWM_BOTH_EDGES   = 0x02     /*!< Notification on both edges */
} Pwm_EdgeNotificationType;

/**
 * @brief Defines the class of a PWM channel
 * @details [SWS_Pwm_00110] Definition of datatype Pwm_ChannelClassType
 */
typedef enum
{
    PWM_VARIABLE_PERIOD      = 0x00,    /*!< Variable period, duty cycle and period can be changed */
    PWM_FIXED_PERIOD         = 0x01,    /*!< Fixed period, only duty cycle can be changed */
    PWM_FIXED_PERIOD_SHIFTED = 0x02     /*!< Fixed shifted period, cannot be changed */
} Pwm_ChannelClassType;

/**
 * @brief Result of the requests related to power state transitions
 * @details [SWS_Pwm_00165] Definition of datatype Pwm_PowerStateRequestResultType
 */
typedef enum
{
    PWM_SERVICE_ACCEPTED    = 0x00,    /*!< Power state change executed */
    PWM_NOT_INIT           = 0x01,    /*!< PWM Module not initialized */
    PWM_SEQUENCE_ERROR     = 0x02,    /*!< Wrong API call sequence */
    PWM_HW_FAILURE         = 0x03,    /*!< Hardware failure preventing power state change */
    PWM_POWER_STATE_NOT_SUPP = 0x04,  /*!< Requested power state not supported */
    PWM_TRANS_NOT_POSSIBLE = 0x05     /*!< Direct transition not possible or HW busy */
} Pwm_PowerStateRequestResultType;


/**
 * @brief Power state currently active or set as target power state
 * @details [SWS_Pwm_00197] Definition of datatype Pwm_PowerStateType
 */
typedef enum
{
    PWM_FULL_POWER = 0x00    /*!< Full Power mode */
    /* Additional power modes 1..255 with decreasing power consumption */
} Pwm_PowerStateType;

/**
 * @brief PWM channel polarity
 */
typedef enum
{
    PWM_POLARITY_HIGH = 0x00,    /*!< PWM signal starts high */
    PWM_POLARITY_LOW  = 0x01     /*!< PWM signal starts low */
} Pwm_PolarityType;

/**
 * @brief PWM channel idle state
 */
typedef enum
{
    PWM_IDLE_LOW  = 0x00,    /*!< Idle state is low */
    PWM_IDLE_HIGH = 0x01     /*!< Idle state is high */
} Pwm_IdleStateType;

/**
 * @brief PWM driver state
 */
typedef enum
{
    PWM_STATE_UNINIT = 0x00,    /*!< PWM driver uninitialized */
    PWM_STATE_INIT   = 0x01     /*!< PWM driver initialized */
} Pwm_DriverStateType;

/**
 * @brief PWM channel state
 */
typedef enum
{
    PWM_CHANNEL_IDLE      = 0x00,    /*!< Channel is in idle state */
    PWM_CHANNEL_RUNNING   = 0x01,    /*!< Channel is running */
    PWM_CHANNEL_STOPPED   = 0x02     /*!< Channel is stopped */
} Pwm_ChannelStateType;

/**
 * @brief PWM hardware unit type
 */
typedef uint8 Pwm_HwUnitType;

/**
 * @brief PWM duty cycle type (0x0000 = 0%, 0x8000 = 100%)
 */
typedef uint16 Pwm_DutyCycleType;

/**
 * @brief PWM phase shift type
 */
typedef uint16 Pwm_PhaseShiftType;

/**
 * @brief PWM frequency type
 */
typedef uint32 Pwm_FrequencyType;



/**
 * @brief PWM notification function pointer type
 * @details Function pointer type for PWM notification callbacks
 */
typedef void (*Pwm_NotificationFunctionType)(void);

/**
 * @brief PWM channel configuration structure
 * @details Contains all configuration parameters for a PWM channel
 */
typedef struct
{
    const Pwm_ChannelType           ChannelId;              /*!< Channel identifier */
    const Pwm_HwUnitType            HwUnit;                 /*!< Hardware unit assignment */
    const Pwm_ChannelClassType      ChannelClass;           /*!< Channel class (fixed/variable period) */
    Pwm_PeriodType                  Period;          /*!< Default period value */
    uint16                          DutyCycle;       /*!< Default duty cycle value */
    Pwm_OutputStateType             Polarity;               /*!< Output polarity */
    Pwm_OutputStateType             IdleState;              /*!< Idle state */
    Pwm_NotificationFunctionType    NotificationPtr;        /*!< Notification function pointer */
    Pwm_EdgeNotificationType        NotificationEdge;       /*!< Notification edge type */
    boolean                         NotificationEnabled;    /*!< Notification enabled flag */
    boolean                         IdleStateSet;           /*!< Idle state set flag */
} Pwm_ChannelConfigType;


/**
 * @brief PWM hardware unit configuration structure
 * @details Contains all configuration parameters for a PWM hardware unit
 */
typedef struct
{
    const Pwm_HwUnitType        HwUnit;                 /*!< Hardware unit identifier */
    // Turn this into macro and get timer instace base on hw id
    // TIM_TypeDef*                TimerInstance;          /*!< Timer instance pointer */

    // Init hw timer base parameter
    Pwm_PeriodType              MaxPeriod;              /*!< Maximum period value not minus 1 */
    uint16                      Prescaler;              /*!< Timer prescaler value not minus 1*/
    uint16                      CounterMode;            /*!< Counter mode */
    uint16                      ClockDivision;          /*!< Clock division */
    uint8                       RepetitionCounter;      /*!< Repetition counter */

    uint8                       NbrOfEnabledChannels;
    // TODO check where this is called
    uint8                       EnabledChannels;        /*!< Number of enabled channels */
    uint8                       ClockSource;            /*!< Clock source selection */
    uint8                       SyncMode;               /*!< Synchronization mode */
    uint8                       MasterSlaveMode;        /*!< Master/slave mode */
} Pwm_HwUnitConfigType;



/**
 * @brief PWM driver configuration structure
 * @details Main configuration structure for PWM driver
 */
typedef struct
{
    const Pwm_ChannelConfigType*    PwmChannelConfig;           /*!< Pointer to channel configurations */
    const Pwm_HwUnitConfigType*     PwmHwUnitConfig;            /*!< Pointer to hardware unit configurations */
    uint8                           PwmMaxChannels;             /*!< Maximum number of channels */
    uint8                           PwmMaxHwUnits;              /*!< Maximum number of hardware units */
    uint32                          PwmSystemFrequency;         /*!< System frequency */
    uint32                          PwmDefaultFrequency;        /*!< Default PWM frequency */
    uint8                           PwmDevErrorDetect;          /*!< Development error detection */
    uint8                           PwmVersionInfoApi;          /*!< Version info API */
    uint8                           PwmNotificationSupported;   /*!< Notification support */
    uint8                           PwmSetPeriodAndDutyApi;     /*!< Set period and duty API */
    uint8                           PwmSetOutputToIdleApi;      /*!< Set output to idle API */
    uint8                           PwmGetOutputStateApi;       /*!< Get output state API */
    uint8                           PwmEnablePhaseShift;        /*!< Phase shift support */
    uint8                           PwmEnableVariablePeriod;    /*!< Variable period support */
    uint8                           PwmEnableSafetyChecks;      /*!< Safety checks */
    uint16                          PwmTimeoutDuration;         /*!< Timeout duration */
    uint8                           PwmDeadTimeEnabled;         /*!< Dead time enabled */
    uint16                          PwmDeadTimeValue;           /*!< Dead time value */
} Pwm_ConfigType;

/****************************************************************************************
*                              CONSTANTS AND MACROS                                   *
****************************************************************************************/

/* PWM Channel Constants */
#define PWM_MAX_CHANNELS            16    /*!< Maximum number of PWM channels */
#define PWM_MAX_HW_UNITS            4     /*!< Maximum number of hardware units */

/* PWM Duty Cycle Constants */
#define PWM_DUTY_CYCLE_0_PERCENT    0x0000    /*!< 0% duty cycle */
#define PWM_DUTY_CYCLE_50_PERCENT   0x4000    /*!< 50% duty cycle */
#define PWM_DUTY_CYCLE_100_PERCENT  0x8000    /*!< 100% duty cycle */

/* PWM Period Constants */
#define PWM_PERIOD_MIN              1         /*!< Minimum period value */
#define PWM_PERIOD_MAX              0xFFFF    /*!< Maximum period value */

/* PWM Hardware Units */
#define PWM_HW_UNIT_TIM1            0    /*!< Timer 1 */
#define PWM_HW_UNIT_TIM2            1    /*!< Timer 2 */
#define PWM_HW_UNIT_TIM3            2    /*!< Timer 3 */
#define PWM_HW_UNIT_TIM4            3    /*!< Timer 4 */

/* PWM Channels per Hardware Unit */
#define PWM_CHANNELS_PER_HW_UNIT    4    /*!< Channels per timer unit */

/* Invalid Values */
#define PWM_INVALID_CHANNEL         0xFF    /*!< Invalid channel identifier */
#define PWM_INVALID_HW_UNIT         0xFF    /*!< Invalid hardware unit identifier */

/****************************************************************************************
*                              VALIDATION MACROS                                      *
****************************************************************************************/

/* Channel validation */
#define PWM_IS_VALID_CHANNEL(ch)        ((ch) < PWM_MAX_CHANNELS)

/* Hardware unit validation */
#define PWM_IS_VALID_HW_UNIT(unit)      ((unit) < PWM_MAX_HW_UNITS)

/* Duty cycle validation */
#define PWM_IS_VALID_DUTY_CYCLE(duty)   ((duty) <= PWM_DUTY_CYCLE_100_PERCENT)

/* Period validation */
#define PWM_IS_VALID_PERIOD(period)     (((period) >= PWM_PERIOD_MIN) && ((period) <= PWM_PERIOD_MAX))

/* Polarity validation */
#define PWM_IS_VALID_POLARITY(pol)      (((pol) == PWM_POLARITY_HIGH) || ((pol) == PWM_POLARITY_LOW))

/* Edge notification validation */
#define PWM_IS_VALID_EDGE_NOTIFICATION(edge) \
    (((edge) == PWM_RISING_EDGE) || ((edge) == PWM_FALLING_EDGE) || ((edge) == PWM_BOTH_EDGES))

#endif /* PWM_TYPES_H */

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
