/**********************************************************
* File name   : Port.c
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
 **********************************************************/

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Port.h"
#include "Port_cfg.h"
#include "stm32f10x_rcc.h"

/****************************************************************************************
*                              STATIC VARIABLES                                        *
****************************************************************************************/
static uint8_t Port_Initialized = 0;  /* Flag check initialization */
extern const Port_ConfigType* Port_Config = NULL_PTR; /* Pointer to the configuration structure */
/****************************************************************************************
 *                              STATIC FUNCTION PROTOTYPES                             *
 ****************************************************************************************/
/**
 * @brief Configure a GPIO pin in DIO mode
 * @param[in] pinCfg Constant Pointer to the pin configuration structure
 * @param[in] pinMask Bitmask for the pin to configure
 */
static void Port_SetModeDIO(const Port_PinConfigType* pinCfg, uint16_t pinMask) ;

 /**
 * @brief Configure a GPIO pin based on AUTOSAR parameters
 * @param[in] pinCfg Constant Pointer to the pin configuration structure
 */
static void Port_ApplyPinConfig(const Port_PinConfigType* pinCfg);

/****************************************************************************************
 *                              STATIC FUNCTION IMPLEMENTATIONS                         *
 ****************************************************************************************/
static void Port_SetModeDIO(const Port_PinConfigType* pinCfg, uint16_t pinMask) {
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_TypeDef* GPIO_Port = PORT_GET_PORT(pinCfg->PortNum);
    GPIO_InitStruct.GPIO_Pin = pinMask;
    GPIO_InitStruct.GPIO_Speed = pinCfg->Speed;

   
    if (pinCfg->Direction == PORT_PIN_OUT) {
        GPIO_InitStruct.GPIO_Mode = (pinCfg->Pull == PORT_PIN_PULL_UP) ? GPIO_Mode_Out_PP : GPIO_Mode_Out_OD;
    } else {
        if (pinCfg->Pull == PORT_PIN_PULL_UP)
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;

        else if (pinCfg->Pull == PORT_PIN_PULL_DOWN)
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;

        else
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }

    GPIO_Init(GPIO_Port, &GPIO_InitStruct);

    /* Nếu là output, set level ban đầu */
    if (pinCfg->Direction == PORT_PIN_OUT) {
        if (pinCfg->Level == PORT_PIN_LEVEL_HIGH)
            GPIO_SetBits((PORT_GET_PORT(pinCfg->PortNum)), pinMask);
        else
            GPIO_ResetBits(PORT_GET_PORT(pinCfg->PortNum), pinMask);
    }
}
static void Port_SetModeLinTx(const Port_PinConfigType* pinCfg, uint16_t pinMask) {
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_TypeDef* GPIO_Port = PORT_GET_PORT(pinCfg->PortNum);
    GPIO_InitStruct.GPIO_Pin = pinMask;
    GPIO_InitStruct.GPIO_Speed = pinCfg->Speed;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_Init(GPIO_Port, &GPIO_InitStruct);

}

static void Port_SetModeLinRx(const Port_PinConfigType* pinCfg, uint16_t pinMask) {
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_TypeDef* GPIO_Port = PORT_GET_PORT(pinCfg->PortNum);
    GPIO_InitStruct.GPIO_Pin = pinMask;
    GPIO_InitStruct.GPIO_Speed = pinCfg->Speed;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(GPIO_Port, &GPIO_InitStruct);

}
static void Port_ApplyPinConfig(const Port_PinConfigType* pinCfg) {
    uint16_t pinMask = PORT_GET_PIN_MASK(pinCfg->PinNum);

    /* Enable source clock for PORT */
    switch (pinCfg->PortNum) {
        case PORT_ID_A: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); break;
        case PORT_ID_B: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); break;
        case PORT_ID_C: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); break;
        case PORT_ID_D: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); break;
        default: return;
    }

    switch(pinCfg->Mode) {
        case PORT_PIN_MODE_DIO:
            /* Set mode DIO */
            Port_SetModeDIO(pinCfg, pinMask);
            break;
        case PORT_PIN_MODE_ADC :
        case PORT_PIN_MODE_PWM :
        case PORT_PIN_MODE_SPI :
        case PORT_PIN_MODE_CAN :
        case PORT_PIN_MODE_LIN_TX:
            Port_SetModeLinTx(pinCfg, pinMask);
            break;
        case PORT_PIN_MODE_LIN_RX:
            Port_SetModeLinRx(pinCfg, pinMask);
            break;
        default:
            return; // Không hỗ trợ mode này
    }
}

/****************************************************************************************
 *                              FUNCTION IMPLEMENTATIONS                                 *
 ****************************************************************************************/

void Port_Init(const Port_ConfigType* ConfigPtr) {
    if (ConfigPtr == NULL_PTR) return;
    // Initialize the Pin 
    for (uint16_t i = 0; i < ConfigPtr->PinCount; i++) {
        Port_ApplyPinConfig(&ConfigPtr->PinConfigs[i]);
    }
    Port_Config = ConfigPtr; 
    Port_Initialized = 1;
}

void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction) {
    if (!Port_Initialized) return;
    if (Pin >= Port_Config->PinCount) return;
    if (!Port_Config->PinConfigs[Pin].DirectionChangeable) return;

    // Update the pin direction
    Port_PinConfigType* pinCfg = (Port_PinConfigType*)&Port_Config->PinConfigs[Pin];
    pinCfg->Direction = Direction;
    Port_ApplyPinConfig(pinCfg);
}


void Port_RefreshPortDirection(void) {
    // return if not initialized
    if(!Port_Initialized) return;
    // Refresh all pins that are not allowed to change direction at runtime
    for(uint16 i = 0;  i < Port_Config->PinCount; i++) {
        if (!Port_Config->PinConfigs[i].DirectionChangeable) {
            Port_ApplyPinConfig(&Port_Config->PinConfigs[i]);
        }
    }
}

void Port_GetVersionInfo(Std_VersionInfoType* versioninfo) {
    if (versioninfo == NULL_PTR) return;
    versioninfo->vendorID         = PORT_VENDOR_ID;
    versioninfo->moduleID         = PORT_MODULE_ID;
    versioninfo->sw_major_version = PORT_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = PORT_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = PORT_SW_PATCH_VERSION;
}


void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode) {
    if (!Port_Initialized) return;
    if (Pin >= Port_Config->PinCount) return;
    if (!Port_Config->PinConfigs[Pin].ModeChangeable) return;

    Port_PinConfigType* pinCfg = (Port_PinConfigType*)&PortCfg_Pins[Pin];
    pinCfg->Mode = Mode;
    Port_ApplyPinConfig(pinCfg);
}
