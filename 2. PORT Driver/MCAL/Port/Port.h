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
#include "Port_Types.h"
#include "Port_Cfg.h"
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
