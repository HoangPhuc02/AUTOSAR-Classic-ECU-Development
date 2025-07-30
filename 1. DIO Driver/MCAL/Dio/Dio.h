/****************************************************************************************
*                                   DIO.H                                              *
****************************************************************************************
* File Name   : Dio.h
* Module      : Digital Input/Output (DIO)
* Description : AUTOSAR DIO driver header file
* Version     : 1.0.0
* Date        : 20/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

#ifndef DIO_H
#define DIO_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Std_Types.h"
#include "stm32f10x_gpio.h"
/****************************************************************************************
*                              VERSION INFORMATION                                     *
****************************************************************************************/
/*
 Port is the same as port in the hardware,
 Channel is the same as pin in the hardware,
 ChannelGroup is a group of channels (pins) on the same port.
*/   
#define DIO_VENDOR_ID           0x1234  /* Vendor ID for DIO driver */ 
#define DIO_MODULE_ID           0x5678  /* Module ID for DIO driver */
#define DIO_SW_MAJOR_VERSION    1
#define DIO_SW_MINOR_VERSION    0
#define DIO_SW_PATCH_VERSION    0


/****************************************************************************************
* MACRO TO IDENTIFY GPIO PORT AND GPIO PIN FROM CHANNEL ID                             *
****************************************************************************************/
#define DIO_PORT_A 0    /* Mapping for GPIO Port A */
#define DIO_PORT_B 1    /* Mapping for GPIO Port B */
#define DIO_PORT_C 2    /* Mapping for GPIO Port C */
#define DIO_PORT_D 3    /* Mapping for GPIO Port D */
/* Macro to identify GPIO Port */
#define DIO_GET_PORT_CHANNEL_ID(ChannelId) \
    ((GPIO_TypeDef *)((uint32_t)(GPIOA_BASE + ((ChannelId) / 16) * 0x400)))

/*
#define DIO_GET_PORT_CHANNEL_ID(ChannelId)   ((ChannelId < 16) ? GPIOA : \
                                   (ChannelId < 32) ? GPIOB : \
                                   (ChannelId < 48) ? GPIOC : \
                                   (ChannelId < 64) ? GPIOD : NULL_PTR)
*/
/* Macro to identify GPIO pin */
#define DIO_GET_PIN_CHANNEL_ID(ChannelId)    (1U << ((ChannelId) % 16))

/**  
* Macro to identify Channel ID
* @param[in] GPIOx: GPIO port (A, B, C, D, etc.)
* @param[in] Pin: GPIO pin number (0-15)
*/
#define DIO_GET_CHANNEL_ID(GPIOx, Pin) \
    (Dio_ChannelType)(((GPIOx) << 4) + (Pin))

#define DIO_GET_PORT_PORT_ID(PortId) \
    ((GPIO_TypeDef *)((uint32_t)(GPIOA_BASE + ((PortId) * 0x400))))

/*
#define DIO_GET_PORT_PORT_ID(PortId)   ((PortId == DIO_PORT_A) ? GPIOA : \
                                        (PortId == DIO_PORT_B) ? GPIOB : \
                                        (PortId == DIO_PORT_C) ? GPIOC : \
                                        (PortId == DIO_PORT_D) ? GPIOD : NULL_PTR)
*/   
/**********************************************************************************
 * MACRO TO CONVERT GPIO PIN TO CHANNEL ID
 ****************************************************************************************/
#define DIO_CHANNEL_A0      DIO_GET_CHANNEL_ID(DIO_PORT_A, 0)  // GPIOA Pin 0
#define DIO_CHANNEL_A1      DIO_GET_CHANNEL_ID(DIO_PORT_A, 1)  // GPIOA Pin 1
#define DIO_CHANNEL_A2      DIO_GET_CHANNEL_ID(DIO_PORT_A, 2)  // GPIOA Pin 2
#define DIO_CHANNEL_A3      DIO_GET_CHANNEL_ID(DIO_PORT_A, 3)  // GPIOA Pin 3
#define DIO_CHANNEL_A4      DIO_GET_CHANNEL_ID(DIO_PORT_A, 4)  // GPIOA Pin 4
#define DIO_CHANNEL_A5      DIO_GET_CHANNEL_ID(DIO_PORT_A, 5)  // GPIOA Pin 5
#define DIO_CHANNEL_A6      DIO_GET_CHANNEL_ID(DIO_PORT_A, 6)  // GPIOA Pin 6
#define DIO_CHANNEL_A7      DIO_GET_CHANNEL_ID(DIO_PORT_A, 7)  // GPIOA Pin 7
#define DIO_CHANNEL_A8      DIO_GET_CHANNEL_ID(DIO_PORT_A, 8)  // GPIOA Pin 8
#define DIO_CHANNEL_A9      DIO_GET_CHANNEL_ID(DIO_PORT_A, 9)  // GPIOA Pin 9
#define DIO_CHANNEL_A10     DIO_GET_CHANNEL_ID(DIO_PORT_A, 10) // GPIOA Pin 10
#define DIO_CHANNEL_A11     DIO_GET_CHANNEL_ID(DIO_PORT_A, 11) // GPIOA Pin 11
#define DIO_CHANNEL_A12     DIO_GET_CHANNEL_ID(DIO_PORT_A, 12) // GPIOA Pin 12
#define DIO_CHANNEL_A13     DIO_GET_CHANNEL_ID(DIO_PORT_A, 13) // GPIOA Pin 13
#define DIO_CHANNEL_A14     DIO_GET_CHANNEL_ID(DIO_PORT_A, 14) // GPIOA Pin 14
#define DIO_CHANNEL_A15     DIO_GET_CHANNEL_ID(DIO_PORT_A, 15) // GPIOA Pin 15

#define DIO_CHANNEL_B0      DIO_GET_CHANNEL_ID(DIO_PORT_B, 0)  // GPIOB Pin 0
#define DIO_CHANNEL_B1      DIO_GET_CHANNEL_ID(DIO_PORT_B, 1)  // GPIOB Pin 1
#define DIO_CHANNEL_B2      DIO_GET_CHANNEL_ID(DIO_PORT_B, 2)  // GPIOB Pin 2
#define DIO_CHANNEL_B3      DIO_GET_CHANNEL_ID(DIO_PORT_B, 3)  // GPIOB Pin 3
#define DIO_CHANNEL_B4      DIO_GET_CHANNEL_ID(DIO_PORT_B, 4)  // GPIOB Pin 4
#define DIO_CHANNEL_B5      DIO_GET_CHANNEL_ID(DIO_PORT_B, 5)  // GPIOB Pin 5
#define DIO_CHANNEL_B6      DIO_GET_CHANNEL_ID(DIO_PORT_B, 6)  // GPIOB Pin 6
#define DIO_CHANNEL_B7      DIO_GET_CHANNEL_ID(DIO_PORT_B, 7)  // GPIOB Pin 7
#define DIO_CHANNEL_B8      DIO_GET_CHANNEL_ID(DIO_PORT_B, 8)  // GPIOB Pin 8
#define DIO_CHANNEL_B9      DIO_GET_CHANNEL_ID(DIO_PORT_B, 9)  // GPIOB Pin 9
#define DIO_CHANNEL_B10     DIO_GET_CHANNEL_ID(DIO_PORT_B, 10) // GPIOB Pin 10
#define DIO_CHANNEL_B11     DIO_GET_CHANNEL_ID(DIO_PORT_B, 11) // GPIOB Pin 11
#define DIO_CHANNEL_B12     DIO_GET_CHANNEL_ID(DIO_PORT_B, 12) // GPIOB Pin 12
#define DIO_CHANNEL_B13     DIO_GET_CHANNEL_ID(DIO_PORT_B, 13) // GPIOB Pin 13
#define DIO_CHANNEL_B14     DIO_GET_CHANNEL_ID(DIO_PORT_B, 14) // GPIOB Pin 14
#define DIO_CHANNEL_B15     DIO_GET_CHANNEL_ID(DIO_PORT_B, 15) // GPIOB Pin 15

#define DIO_CHANNEL_C0      DIO_GET_CHANNEL_ID(DIO_PORT_C, 0)  // GPIOC Pin 0
#define DIO_CHANNEL_C1      DIO_GET_CHANNEL_ID(DIO_PORT_C, 1)  // GPIOC Pin 1
#define DIO_CHANNEL_C2      DIO_GET_CHANNEL_ID(DIO_PORT_C, 2)  // GPIOC Pin 2
#define DIO_CHANNEL_C3      DIO_GET_CHANNEL_ID(DIO_PORT_C, 3)  // GPIOC Pin 3
#define DIO_CHANNEL_C4      DIO_GET_CHANNEL_ID(DIO_PORT_C, 4)  // GPIOC Pin 4
#define DIO_CHANNEL_C5      DIO_GET_CHANNEL_ID(DIO_PORT_C, 5)  // GPIOC Pin 5
#define DIO_CHANNEL_C6      DIO_GET_CHANNEL_ID(DIO_PORT_C, 6)  // GPIOC Pin 6
#define DIO_CHANNEL_C7      DIO_GET_CHANNEL_ID(DIO_PORT_C, 7)  // GPIOC Pin 7
#define DIO_CHANNEL_C8      DIO_GET_CHANNEL_ID(DIO_PORT_C, 8)  // GPIOC Pin 8
#define DIO_CHANNEL_C9      DIO_GET_CHANNEL_ID(DIO_PORT_C, 9)  // GPIOC Pin 9
#define DIO_CHANNEL_C10     DIO_GET_CHANNEL_ID(DIO_PORT_C, 10) // GPIOC Pin 10
#define DIO_CHANNEL_C11     DIO_GET_CHANNEL_ID(DIO_PORT_C, 11) // GPIOC Pin 11
#define DIO_CHANNEL_C12     DIO_GET_CHANNEL_ID(DIO_PORT_C, 12) // GPIOC Pin 12
#define DIO_CHANNEL_C13     DIO_GET_CHANNEL_ID(DIO_PORT_C, 13) // GPIOC Pin 13
#define DIO_CHANNEL_C14     DIO_GET_CHANNEL_ID(DIO_PORT_C, 14) // GPIOC Pin 14
#define DIO_CHANNEL_C15     DIO_GET_CHANNEL_ID(DIO_PORT_C, 15) // GPIOC Pin 15

#define DIO_CHANNEL_D0      DIO_GET_CHANNEL_ID(DIO_PORT_D, 0)  // GPIOD Pin 0
#define DIO_CHANNEL_D1      DIO_GET_CHANNEL_ID(DIO_PORT_D, 1)  // GPIOD Pin 1
#define DIO_CHANNEL_D2      DIO_GET_CHANNEL_ID(DIO_PORT_D, 2)  // GPIOD Pin 2
#define DIO_CHANNEL_D3      DIO_GET_CHANNEL_ID(DIO_PORT_D, 3)  // GPIOD Pin 3
#define DIO_CHANNEL_D4      DIO_GET_CHANNEL_ID(DIO_PORT_D, 4)  // GPIOD Pin 4
#define DIO_CHANNEL_D5      DIO_GET_CHANNEL_ID(DIO_PORT_D, 5)  // GPIOD Pin 5
#define DIO_CHANNEL_D6      DIO_GET_CHANNEL_ID(DIO_PORT_D, 6)  // GPIOD Pin 6
#define DIO_CHANNEL_D7      DIO_GET_CHANNEL_ID(DIO_PORT_D, 7)  // GPIOD Pin 7
#define DIO_CHANNEL_D8      DIO_GET_CHANNEL_ID(DIO_PORT_D, 8)  // GPIOD Pin 8
#define DIO_CHANNEL_D9      DIO_GET_CHANNEL_ID(DIO_PORT_D, 9)  // GPIOD Pin 9
#define DIO_CHANNEL_D10     DIO_GET_CHANNEL_ID(DIO_PORT_D, 10) // GPIOD Pin 10
#define DIO_CHANNEL_D11     DIO_GET_CHANNEL_ID(DIO_PORT_D, 11) // GPIOD Pin 11
#define DIO_CHANNEL_D12     DIO_GET_CHANNEL_ID(DIO_PORT_D, 12) // GPIOD Pin 12
#define DIO_CHANNEL_D13     DIO_GET_CHANNEL_ID(DIO_PORT_D, 13) // GPIOD Pin 13
#define DIO_CHANNEL_D14     DIO_GET_CHANNEL_ID(DIO_PORT_D, 14) // GPIOD Pin 14
#define DIO_CHANNEL_D15     DIO_GET_CHANNEL_ID(DIO_PORT_D, 15) // GPIOD Pin 15
/***************************************************************************************
 *                              TYPE DEFINITIONS                                        *
****************************************************************************************/
/*
    * @typedef Dio_ChannelType
    * @brief Type for DIO channel ID.
    * @note This type is used to identify a specific channel (pin) on a port.
    *       The channel ID is typically defined in the DIO configuration file.
*/
typedef uint8 Dio_ChannelType;    // ID of DIO channel

/*
    * @typedef Dio_PortType
    * @brief Type for DIO port ID.
    * @note This type is used to identify a specific port in the DIO driver.
    *       The port ID is typically defined in the DIO configuration file.
*/
typedef uint8 Dio_PortType;       // Type for DIO port

/*
    * @typedef Dio_LevelType
    * @brief Type for DIO level (high or low).
    * @note This type is used to represent the physical level of a pin (channel).
    *       It can be either high (STD_HIGH) or low (STD_LOW).
*/
typedef uint8 Dio_LevelType;     // Type for DIO level (high or low) 

/*
    * @typedef Dio_ChannelGroupType
    * @brief Type for DIO channel group.
    * @note This type is used to define a group of channels (pins) on the same port.
    *       It includes a mask to specify which channels are included in the group,
    *       an offset to indicate the position of the group, and the port to which it belongs.
*/
typedef struct 
{
    uint16 mask;                  // Mask of the channel group's position
    uint8  offset;                // Position of the Channel Group on the port counted from LSB
    Dio_PortType port;            // Port to which the channel group belongs
}Dio_ChannelGroupType;

/*
    * @typedef Dio_PortLevelType
    * @brief Type for DIO port level.
    * @note This type is used to represent the level of all channels in a port.
    *       It can be larger than the number of channels in the port, allowing for future expansion.
    * @example If a port has 16 channels(pins) its value can be represented like : 1100 1111 0000 1111.
*/
typedef uint16 Dio_PortLevelType; // Size of the largest port (4/8/16/... bits)

/****************************************************************************************
*                                 ERROR CODES                                          *
****************************************************************************************/
#define DIO_E_PARAM_INVALID_CHANNEL_ID  0x0AU // Error code for invalid channel ID
#define DIO_E_PARAM_INVALID_PORT_ID     0x14U // Error code for invalid port ID
#define DIO_E_PARAM_INVALID_GROUP       0x1FU // Error code for invalid channel group ID
#define DIO_E_PARAM_POINTER             0x20U // Error code for invalid level (high or low)       

/****************************************************************************************
*                              FUNCTION PROTOTYPES                                     *
****************************************************************************************/
/* Channel read-write function*/
/*
    * @brief Return the value of the specified channel.
    * @param Channel ID: ID of the channel to be initialized.
    * @return Dio_LevelType: The level of the channel (high or low).
    * @note STD_HIGH: The physical level of the pin is high.
    *       STD_LOW: The physical level of the pin is low.
    * @error DIO_E_PARAM_INVALID_CHANNEL_ID: If the channel ID is invalid.
*/
Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId);

/*
    * @brief Set a level of a channel.
    * @param ChannelId: ID of the channel to be written.
    * @param Level: The level to be written to the channel (high or low).
    * @note If the channel is configured as an output, it will set the physical level of the pin.
    *       If the channel is configured as an input, it will not affect the physical level of the pin.
    
*/
void Dio_WriteChannel(Dio_ChannelType ChannelId, 
                      Dio_LevelType Level);


/* Port read-write function*/
/*
    * @brief Return the level of all channels of that port.
    * @param PortId: ID of the port .
    * @note If the channel is configured as an output, it will toggle the physical level of the pin.
    *       If the channel is configured as an input, it will not affect the physical level of the pin.
    * @ error DIO_E_PARAM_INVALID_PORT_ID: If the port ID is invalid.
*/
Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId);

/*
    * @brief Set a level of all channels of that port.
    * @param PortId: ID of the port to be written.
    * @param Level: The level to be written to the port (high or low).
    * @note When writing a port which is smaller than DIO_PortLevelType,
    *       the upper bits will be ignored.
    * @ error DIO_E_PARAM_INVALID_PORT_ID: If the port ID is invalid.
*/
void Dio_WritePort(Dio_PortType PortId, 
                   Dio_PortLevelType Level);

/* Channel group read-write function*/
/*
    * @brief Return the level of all channels of that channel group.
    * @param ChannelGroupIdPtr: The channel group to be read.
    * @return Dio_PortLevelType: The level of the channel group.
    * @note If the channel is configured as an output, it will toggle the physical level of the pin.
    *       If the channel is configured as an input, it will not affect the physical level of the pin.
    * @ error DIO_E_PARAM_INVALID_GROUP: If the channel group is invalid.
*/
Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr);

/*
    * @brief Set a level of all channels of that channel group.
    * @param ChannelGroupIdPtr: The channel group to be written.
    * @param Level: The level to be written to the channel group (high or low).
    * @note When writing a channel group which is smaller than DIO_PortLevelType,
    *       the upper bits will be ignored.
    * @ error DIO_E_PARAM_INVALID_GROUP: If the channel group is invalid.
*/
void Dio_WriteChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr, 
                           Dio_PortLevelType Level);

/*
    * @brief Get the version information of the DIO driver.
    * @param versioninfo: Pointer to a structure that will hold the version information.
    * @note The version information includes vendor ID, module ID, software major version,
    *       software minor version, and software patch version.
*/
void Dio_GetVersionInfo(Std_VersionInfoType* versioninfo);

/*
    * @brief Flip the level of a channel.
    * @param ChannelId: ID of the channel to be flipped.
    * @return Dio_LevelType: The new level of the channel.
    * @note If the channel is configured as an output, it will toggle the physical level of the pin.
    *       If the channel is configured as an input, it will not affect the physical level of the pin.
*/
Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId);

/*
    * @brief Service to set the value of a given port with required mask.
    * @param PortId: ID of the port to be written.
    * @param Level: The level to be written to the port (high or low).
    * @param Mask: The mask to be applied to the port.
    * @note When writing a port which is smaller than DIO_PortLevelType,
    *       the upper bits will be ignored.
    * @ error DIO_E_PARAM_INVALID_PORT_ID: If the port ID is invalid.
*/
void Dio_MaskWritePort(Dio_PortType PortId, 
                        Dio_PortLevelType Level, 
                        Dio_PortLevelType Mask);


#endif /* DIO_H */

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/