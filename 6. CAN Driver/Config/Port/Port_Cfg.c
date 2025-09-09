/**********************************************************
* File Name   : Port_cfg.c
* Module      : Port Driver 
* Description : Port Driver Configuration Source File
* Details     : File configures the GPIO pins for the Port Driver
*               according to AUTOSAR standards, designed for STM32F103
*               using the Standard Peripheral Library (SPL).
* Version     : 1.0.0
* Date        : 27/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
 **********************************************************/

#include "Port_cfg.h"

/**
 * @brief Configuration for the Port Driver
 *        - PinCount: Total number of pins configured
 *        - PinConfigs: Pointer to an array of pin configurations
 */
const Port_ConfigType PortCfg_Port = {
    .PinCount = PORT_CFG_PIN_COUNTS,
    .PinConfigs = PortCfg_Pins
};

const Port_PinConfigType PortCfg_Pins[PORT_CFG_PIN_COUNTS] = {
    // ADC IN A0
    {
        .PortNum = PORT_ID_A,
        .PinNum = 0,
        .Mode = PORT_PIN_MODE_ADC,
        .Direction = PORT_PIN_IN,
        .DirectionChangeable = 0,
        .Level = PORT_PIN_LEVEL_HIGH,
        .Pull = PORT_PIN_PULL_NONE,
        .ModeChangeable = 1,
        .Speed = PORT_PIN_SPEED_10MHZ,
    },
    // PWM OUT A8
    {
        .PortNum = PORT_ID_A,
        .PinNum = 8,
        .Mode = PORT_PIN_MODE_PWM,
        .Direction = PORT_PIN_OUT,
        .DirectionChangeable = 0,
        .Level = PORT_PIN_LEVEL_HIGH,
        .Pull = PORT_PIN_PULL_NONE,
        .ModeChangeable = 1,
        .Speed = PORT_PIN_SPEED_50MHZ,
    },
    // LED ON BOARD PC13
    {
        .PortNum = PORT_ID_C,
        .PinNum = 13,
        .Mode = PORT_PIN_MODE_DIO,
        .Direction = PORT_PIN_OUT,
        .DirectionChangeable = 0,
        .Level = PORT_PIN_LEVEL_HIGH, // TURN OFF
        .Pull = PORT_PIN_PULL_NONE,
        .ModeChangeable = 1,
        .Speed = PORT_PIN_SPEED_10MHZ,
    },
    // CAN TX A12
    {
        .PortNum = PORT_ID_A,
        .PinNum = 12,
        .Mode = PORT_PIN_MODE_CAN,
        .ModeChangeable = 0,
        .Direction = PORT_PIN_OUT,
        .DirectionChangeable = 0,
        .Level = PORT_PIN_LEVEL_HIGH,
        .Pull  = PORT_PIN_PULL_NONE,
        .Speed = PORT_PIN_SPEED_50MHZ,
    },
    // CAN RX A11
    {
        .PortNum = PORT_ID_A,
        .PinNum = 11,
        .Mode = PORT_PIN_MODE_CAN,
        .ModeChangeable = 0,
        .Direction = PORT_PIN_IN,
        .DirectionChangeable = 0,
        .Level = PORT_PIN_LEVEL_HIGH,
        .Pull  = PORT_PIN_PULL_NONE,
        .Speed = PORT_PIN_SPEED_50MHZ,
    }
};