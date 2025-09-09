/***************************************************************************************
*                                   PORT_CFG.H                                        *
****************************************************************************************
* File Name   : Port_cfg.h
* Module      : Port Driver 
* Description : Port Driver Configuration Header File
* Details     : File configures the GPIO pins for the Port Driver
*               according to AUTOSAR standards, designed for STM32F103
*               using the Standard Peripheral Library (SPL).
*
* Version     : 2.0.0
* Date        : 20/06/2025
* Update      : 07/09/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
 **************************************************************************************/

#ifndef PORT_CFG_H
#define PORT_CFG_H

#include "Std_Types.h"
#include "Port_Types.h"


/**********************************************************
 * NUMBER OF PINS CONFIGURED
 **********************************************************/
#define PORT_CFG_PIN_COUNTS    5U

/**********************************************************
 * ARRAY OF PIN CONFIGURATIONS
 **********************************************************/
extern const Port_PinConfigType PortCfg_Pins[PORT_CFG_PIN_COUNTS];
extern const Port_ConfigType PortCfg_Port; 
#endif /* PORT_CFG_H */
