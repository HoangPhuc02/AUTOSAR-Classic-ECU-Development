/****************************************************************************************
*                                   PORT.H                                             *
****************************************************************************************
* File Name   : Port.h
* Module      : Port Driver 
* Description : AUTOSAR Port driver header file
* Details     : Provide APIs for Port Driver . This driver is
*               designed to work with STM32F103 using SPL.
*               The driver allows configuration, mode change, direction change,
*               refresh, and version information retrieval for port/pin.
* Version     : 1.0.0
* Date        : 20/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

#ifndef PORT_H
#define PORT_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Std_Types.h"
// #include "Port_cfg.h"  // Include configuration header
#include "stm32f10x_gpio.h"
#include "Config.h"
/****************************************************************************************
*                              VERSION INFORMATION                                     *
****************************************************************************************/
#define PORT_VENDOR_ID          1234U
#define PORT_MODULE_ID          81U
#define PORT_SW_MAJOR_VERSION   1U
#define PORT_SW_MINOR_VERSION   0U
#define PORT_SW_PATCH_VERSION   0U
/*
 Port is the same as port in the hardware,
 Channel is the same as pin in the hardware,
 ChannelGroup is a group of channels (pins) on the same port.
*/

/****************************************************************************************
*                               MACRO                                                   *
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

/* Definition of Pin modes DIO, ADC,...*/
#define PORT_PIN_MODE_DIO       0
#define PORT_PIN_MODE_ADC       1
#define PORT_PIN_MODE_PWM       2
#define PORT_PIN_MODE_SPI       3
#define PORT_PIN_MODE_CAN       4
#define PORT_PIN_MODE_LIN       5


#define PORT_PIN_PULL_NONE      0
#define PORT_PIN_PULL_UP        1
#define PORT_PIN_PULL_DOWN      2

/* Definition of initial level values for pin */
#define PORT_PIN_LEVEL_LOW      0
#define PORT_PIN_LEVEL_HIGH     1

/***************************************************************************************
 *                              TYPE DEFINITIONS                                        *
****************************************************************************************/

/* Data Type Typedef */
/*
    * @typedef Port_PinType
    * @brief Type for PORT pin ID.
    * @note This type is used to identify a specific pin in the PORT driver.
    *       (0..47: A0..A15, B0..B15, C0..C15)
*/
typedef uint8 Port_PinType;
/*
    * @typedef Port_PortType
    * @brief Type for PORT ID.
    * @note This type is used to identify a specific port in the PORT driver.
    *       (0=A, 1=B, 2=C, 3=D)
*/
typedef uint8 Port_PortType;  

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
typedef uint8 Port_PinModeType;


/* Enum and Structure typedef */
/**
 * @typedef Port_PinDirectionType
 * @brief   ENUM for Direction of Port Pin
 */
typedef enum {
    PORT_PIN_IN  = 0x00,   // Input  
    PORT_PIN_OUT = 0x01    // Output 
} Port_PinDirectionType;

typedef enum {
  PORT_PIN_SPEED_10MHZ = 1,
  PORT_PIN_SPEED_2MHZ, 
  PORT_PIN_SPEED_50MHZ
} Port_PinSpeedType;

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
    uint8                   ModeChangeable;      // 1 = Allow runtime mode change  
    Port_PinDirectionType   Direction;          // Initial direction 
    uint8                   DirectionChangeable; // 1 = Allow runtime direction change 
    uint8                   Level;               // PORT_PIN_LEVEL_LOW, PORT_PIN_LEVEL_HIGH
    uint8                   Pull;                // PORT_PIN_PULL_NONE, PORT_PIN_PULL_UP, PORT_PIN_PULL_DOWN
    Port_PinSpeedType       Speed ;              // PORT_PIN_SPEED_10MHZ, PORT_PIN_SPEED_2MHZ, PORT_PIN_SPEED_50MHZ
} Port_PinConfigType;

/**
 * @struct Port_ConfigType
 * @brief  Structure for Port configuration
 *         - PinConfigs: Pointer to an array of Port_PinConfigType
 *         - PinCount: Number of configured pins
 */
typedef struct {
    const Port_PinConfigType* PinConfigs; /**< Array of pin configurations */
    uint16 PinCount;                     /**< Number of configured pins */
} Port_ConfigType;

/****************************************************************************************
 *                              GLOBAL VARIABLES                                      *
 ****************************************************************************************/


/****************************************************************************************
*                              FUNCTION PROTOTYPES                                     *
****************************************************************************************/

/** 
 * @brief   Initialize all Ports/Pins according to the configuration
 * @details This function will configure each pin according to the config table.
 * @param[in] ConfigPtr Pointer to the Port configuration
 */
void Port_Init(const Port_ConfigType* ConfigPtr);

/**
 * @brief Change the direction of a Port pin (if runtime change is allowed)
 * @details
 * This function will change the direction (IN/OUT) of the pin, if allowed in the configuration.
 * @param[in] Pin Pin number (0..n-1)
 * @param[in] Direction Desired direction
*/
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction);

/**
 * @brief Refresh the direction of all pins that are not allowed to change direction at runtime
 * @note Only pins that are configured with DirectionChangeable = 0 will be refreshed.
 */
void Port_RefreshPortDirection(void);

/**
 * @brief   Get version information of the Port Driver
 * @param[out] versioninfo  Pointer to the Std_VersionInfoType structure to receive version
 */
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo);

/**
 * @brief   Set the mode of a pin (if allowed)
 * @param[in] Pin    Pin number (0..n-1)
 * @param[in] Mode   Desired mode (DIO, ADC, PWM, etc.)
*/
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);

#endif /* PORT_H */
