/****************************************************************************************
*                                PORT_TYPES.H                                          *
****************************************************************************************
* File Name   : Port_Types.h
* Module      : Port Driver 
* Description : AUTOSAR Port driver type definitions header file
* Details     : Contains all type definitions, enumerations, and structures
*               used by the Port driver according to AUTOSAR standards.
*               Designed for STM32F103 using Standard Peripheral Library (SPL).
*
* Version     : 2.0.0
* Date        : 20/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

#ifndef PORT_TYPES_H
#define PORT_TYPES_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Std_Types.h"

/****************************************************************************************
*                                TYPE DEFINITIONS                                      *
****************************************************************************************/


/* Data Type Typedef */
/*
    * @typedef Port_PinType
    * @brief Type for PORT pin ID.
    * @note This type is used to identify a specific pin in the PORT driver.
    *       (0..47: A0..A15, B0..B15, C0..C15)
*/
typedef uint16 Port_PinType;
/*
    * @typedef Port_PortType
    * @brief Type for PORT ID.
    * @note This type is used to identify a specific port in the PORT driver.
    *       (0=A, 1=B, 2=C, 3=D)
*/
typedef uint8 Port_PortType;  


/**
 * @brief Port pin direction type
 * @details Defines the direction of a port pin
 */
typedef enum
{
    PORT_PIN_IN = 0,    /**< Port pin direction input */
    PORT_PIN_OUT        /**< Port pin direction output */
} Port_PinDirectionType;

/**
 * @typedef Port_PinModeType
 * @brief   Type for Pin mode(DIO, ADC, PWM...)
 * @PORT_PIN_MODE_DIO: Digital Input/Output
 * @PORT_PIN_MODE_ADC: Analog to Digital Converter
 * @PORT_PIN_MODE_PWM: Pulse Width Modulation
 * @PORT_PIN_MODE_SPI: Serial Peripheral Interface
 * @PORT_PIN_MODE_CAN: Controller Area Network
 * @PORT_PIN_MODE_LIN: Local Interconnect Network   
 */
typedef enum
{
    /* Input modes */
    PORT_PIN_MODE_DIO,     

    /* Special modes */
    PORT_PIN_MODE_PWM,            /**< PWM output mode */
    PORT_PIN_MODE_ADC,            /**< ADC input mode */
    PORT_PIN_MODE_SPI,            /**< SPI communication mode */
    PORT_PIN_MODE_I2C,            /**< I2C communication mode */
    PORT_PIN_MODE_UART,           /**< UART communication mode */
    PORT_PIN_MODE_CAN,             /**< CAN communication mode */
    PORT_PIN_MODE_LIN              /**< LIN communication mode */
} Port_PinModeType;

/**
 * @brief Port pin output speed type
 * @details Defines the output speed for port pins in output mode
 */
typedef enum {
    PORT_PIN_SPEED_10MHZ = 1,
    PORT_PIN_SPEED_2MHZ, 
    PORT_PIN_SPEED_50MHZ
} Port_PinSpeedType;

/**
 * @brief Port pin initial value type
 * @details Defines the initial value for output pins
 */
typedef enum
{
    PORT_PIN_LEVEL_LOW = 0,     /**< Initial value low */
    PORT_PIN_LEVEL_HIGH         /**< Initial value high */
} Port_PinLevelType;

/**
 * @brief Port pin direction changeable type
 * @details Defines whether pin direction can be changed during runtime
 */
typedef boolean Port_PinDirectionChangeable;

/**
 * @brief Port pin mode changeable type
 * @details Defines whether pin mode can be changed during runtime
 */
typedef boolean Port_PinModeChangeable;


#define PORT_PIN_PULL_NONE      0
#define PORT_PIN_PULL_UP        1
#define PORT_PIN_PULL_DOWN      2

/**
 * @struct Port_PinConfigType
 * @brief  Detailed configuration for each pin
 *         - PortNum: PORT_ID_A, PORT_ID_B, ...
 *         - PinNum: 0..15 (pin index in the port)
 *         - Mode: PORT_PIN_MODE_DIO, PORT_PIN_MODE_ADC, ...
 *         - Direction: PORT_PIN_IN / PORT_PIN_OUT
 *         - DirectionChangeable: 1 = Allow runtime direction change
 *         - Level: PORT_PIN_LEVEL_HIGH / PORT_PIN_LEVEL_LOW (only if output)
 *         - Pull: PORT_PIN_PULL_NONE / UP / DOWN (only if input)
 *         - Speed: PORT_PIN_SPEED_10MHZ, PORT_PIN_SPEED_2MHZ, PORT_PIN_SPEED_50MHZ
 *         - ModeChangeable: 1 = Allow runtime mode change
 */
typedef struct {
    Port_PortType           PortNum;            // 0=A, 1=B, 2=C, 3=D 
    Port_PinType            PinNum;             // 0..15 
    Port_PinModeType        Mode;               // Pin mode (DIO, ADC, etc.)
    Port_PinModeChangeable  ModeChangeable;      // 1 = Allow runtime mode change  
    Port_PinDirectionType   Direction;          // Initial direction 
    Port_PinDirectionChangeable                   DirectionChangeable; // 1 = Allow runtime direction change 
    Port_PinLevelType       Level;               // PORT_PIN_LEVEL_LOW, PORT_PIN_LEVEL_HIGH
    uint8                   Pull;                // PORT_PIN_PULL_NONE, PORT_PIN_PULL_UP, PORT_PIN_PULL_DOWN
    Port_PinSpeedType       Speed ;              // PORT_PIN_SPEED_10MHZ, PORT_PIN_SPEED_2MHZ, PORT_PIN_SPEED_50MHZ
} Port_PinConfigType;

/**
 * @brief Port driver configuration type
 * @details Main configuration structure containing all pin configurations
 */
typedef struct
{
    uint8                        PinCount;  /**< Total number of configured pins */
    const Port_PinConfigType*    PinConfigs;    /**< Array of pin configurations */
} Port_ConfigType;

/****************************************************************************************
*                                SYMBOLIC NAMES                                        *
****************************************************************************************/

/* MACRO TO IDENTIFY GPIO PORT AND GPIO PIN FROM PORT ID */
#define PORT_ID_A   0   /* GPIOA */
#define PORT_ID_B   1   /* GPIOB */
#define PORT_ID_C   2   /* GPIOC */
#define PORT_ID_D   3   /* GPIOD */


/* MACRO TO IDENTIFY GPIO PORT FROM PORT ID*/
#define PORT_GET_PORT(PortNum) \
    ((PortNum) == PORT_ID_A ? GPIOA : \
    (PortNum) == PORT_ID_B ? GPIOB : \
    (PortNum) == PORT_ID_C ? GPIOC : \
    (PortNum) == PORT_ID_D ? GPIOD : NULL_PTR)

/*MACRO TO GET PIN MASK FROM PIN NUMBER*/
#define PORT_GET_PIN_MASK(PinNum)   (1U << (PinNum))
/* Pin IDs (0-15 for each port) */
#define PORT_PIN_0   0U   /**< Pin 0 */
#define PORT_PIN_1   1U   /**< Pin 1 */
#define PORT_PIN_2   2U   /**< Pin 2 */
#define PORT_PIN_3   3U   /**< Pin 3 */
#define PORT_PIN_4   4U   /**< Pin 4 */
#define PORT_PIN_5   5U   /**< Pin 5 */
#define PORT_PIN_6   6U   /**< Pin 6 */
#define PORT_PIN_7   7U   /**< Pin 7 */
#define PORT_PIN_8   8U   /**< Pin 8 */
#define PORT_PIN_9   9U   /**< Pin 9 */
#define PORT_PIN_10  10U  /**< Pin 10 */
#define PORT_PIN_11  11U  /**< Pin 11 */
#define PORT_PIN_12  12U  /**< Pin 12 */
#define PORT_PIN_13  13U  /**< Pin 13 */
#define PORT_PIN_14  14U  /**< Pin 14 */
#define PORT_PIN_15  15U  /**< Pin 15 */

/* Direction changeability flags */
#define PORT_PIN_DIRECTION_CHANGEABLE       STD_ON   /**< Direction can be changed */
#define PORT_PIN_DIRECTION_NOT_CHANGEABLE   STD_OFF  /**< Direction cannot be changed */

/* Mode changeability flags */
#define PORT_PIN_MODE_CHANGEABLE            STD_ON   /**< Mode can be changed */
#define PORT_PIN_MODE_NOT_CHANGEABLE        STD_OFF  /**< Mode cannot be changed */

/****************************************************************************************
*                                VALIDATION MACROS                                     *
****************************************************************************************/

/**
 * @brief Validate port ID
 * @param PortId Port identifier to validate
 * @return TRUE if valid, FALSE otherwise
 */
#define PORT_IS_VALID_PORT_ID(PortId)    ((PortId) <= PORT_ID_D)

/**
 * @brief Validate pin ID
 * @param PinId Pin identifier to validate
 * @return TRUE if valid, FALSE otherwise
 */
#define PORT_IS_VALID_PIN_ID(PinId)      ((PinId) <= PORT_PIN_15)

/**
 * @brief Validate pin direction
 * @param Direction Pin direction to validate
 * @return TRUE if valid, FALSE otherwise
 */
#define PORT_IS_VALID_DIRECTION(Direction) \
    (((Direction) == PORT_PIN_IN) || ((Direction) == PORT_PIN_OUT))

/**
 * @brief Validate pin level
 * @param Level Pin level to validate
 * @return TRUE if valid, FALSE otherwise
 */
#define PORT_IS_VALID_LEVEL(Level) \
    (((Level) == PORT_PIN_LEVEL_LOW) || ((Level) == PORT_PIN_LEVEL_HIGH))

#endif /* PORT_TYPES_H */

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
