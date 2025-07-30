/***************************************************************************************
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
#include "Common/Inc/Std_Types.h"
#include "stm32f10x_gpio.h"
#include "Config/Inc/Dio_Cfg.h"
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
*                                   MACRO                                               *
****************************************************************************************/

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

/* 
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


/***************************************************************************************
 *                              TYPE DEFINITIONS                                        *
****************************************************************************************/
/**
 * @typedef Dio_ChannelType
 * @brief Type for DIO channel ID.
 * @note This type is used to identify a specific channel (pin) on a port.
 *       The channel ID is typically defined in the DIO configuration file.
 */
typedef uint8 Dio_ChannelType;    // ID of DIO channel

/** 
 * @typedef Dio_PortType
 * @brief Type for DIO port ID.
 * @note This type is used to identify a specific port in the DIO driver.
 *       The port ID is typically defined in the DIO configuration file.
 */
typedef uint8 Dio_PortType;       // Type for DIO port

/** 
 * @typedef Dio_LevelType
 * @brief Type for DIO level (high or low).
 * @note This type is used to represent the physical level of a pin (channel).
 *       It can be either high (STD_HIGH) or low (STD_LOW).
 */
typedef uint8 Dio_LevelType;     // Type for DIO level (high or low) 

/** 
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
/**
 * @brief Return the value of the specified channel.
 * @param[in] Channel ID: ID of the channel to be initialized.
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