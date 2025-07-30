/****************************************************************************************
*                                DIO_CFG.C                                             *
****************************************************************************************
* File Name   : Dio_Cfg.c
* Module      : Digital Input/Output (DIO)
* Description : AUTOSAR DIO driver configuration implementation
* Version     : 1.0.0
* Date        : 20/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
****************************************************************************************/

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Dio.h"

/****************************************************************************************
*                              FUNCTION IMPLEMENTATIONS                                 *
****************************************************************************************/

Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId)
{
    GPIO_TypeDef *GPIO_Port;  // Pointer to the GPIO port
    uint16_t GPIO_Pin;         // Pin number within the port
    Dio_LevelType retLevel;        
    // Step 1: Get the GPIO port and pin from the channel ID
    GPIO_Port = DIO_GET_PORT_CHANNEL_ID(ChannelId);

    if (GPIO_Port == NULL_PTR) {
        // Handle error: Invalid channel ID
        retLevel = STD_LOW; // or some error handling
    }
    else {
        // Step 2: Get the pin number from the channel ID
        GPIO_Pin = DIO_GET_PIN_CHANNEL_ID(ChannelId);

        // Step 3: Read the pin state
        if (GPIO_ReadInputDataBit(GPIO_Port, GPIO_Pin) == Bit_SET) {
            retLevel = STD_HIGH; // Pin is high
        } else {
            retLevel = STD_LOW;  // Pin is low
        }
    }
    // Step 4: Return the level
    return retLevel;
}

void Dio_WriteChannel(Dio_ChannelType ChannelId, 
                      Dio_LevelType Level)
{
    GPIO_TypeDef *GPIO_Port;  // Pointer to the GPIO port
    uint16_t GPIO_Pin;         // Pin number within the port   
    BitAction BitVal = (Level == STD_HIGH) ? Bit_SET : Bit_RESET; // Convert level to BitAction    
    // Step 1: Get the GPIO port and pin from the channel ID
    GPIO_Port = DIO_GET_PORT_CHANNEL_ID(ChannelId);

    if (GPIO_Port == NULL_PTR) {
        // Handle error: Invalid channel ID
        return; // or some error handling
    }
    
    // Step 2: Get the pin number from the channel ID
    GPIO_Pin = DIO_GET_PIN_CHANNEL_ID(ChannelId);

    // Step 3: Write the pin state 
    GPIO_WriteBit(GPIO_Port, GPIO_Pin, BitVal) ;
}

Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId)
{
    GPIO_TypeDef *GPIO_Port;  // Pointer to the GPIO port
    Dio_PortLevelType retPortLevel = 0; // Initialize return value
    // Step 1: Get the GPIO port from the port ID
    GPIO_Port = DIO_GET_PORT_PORT_ID(PortId);
    if (GPIO_Port == NULL_PTR) {
        // Handle error: Invalid port ID
        retPortLevel = 0; // or some error handling
    } else {
        // Step 2: Read the entire state of all channel in port 
        retPortLevel = (Dio_PortLevelType)GPIO_ReadInputData(GPIO_Port);
    }
    return retPortLevel;
}

void Dio_WritePort(Dio_PortType PortId, 
                   Dio_PortLevelType Level)
{
    GPIO_TypeDef *GPIO_Port;  // Pointer to the GPIO port
    // Step 1: Get the GPIO port from the port ID
    GPIO_Port = DIO_GET_PORT_PORT_ID(PortId);
    if (GPIO_Port == NULL_PTR) {
        // Handle error: Invalid port ID
        return;
    } 
    // Step 2: Write the entire state of all channel in port
    GPIO_Write(GPIO_Port, (uint16_t)Level);
}

Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr)
{
    GPIO_TypeDef *GPIO_Port;            // Pointer to the GPIO port
    Dio_PortLevelType retPortLevel = 0; // Initialize return value
    // Step 1: Get the GPIO port from the channel group
    GPIO_Port = DIO_GET_PORT_PORT_ID(ChannelGroupIdPtr->port);

    if (GPIO_Port == NULL_PTR) {
        // Handle error: Invalid channel group
        retPortLevel = 0; // or some error handling
    }
    // Step 2: Read the entire state of all channels in the channel group
    else {
        // Read the input data register of the port
        Dio_PortLevelType portData = (Dio_PortLevelType)GPIO_ReadInputData(GPIO_Port);
        
        // Step 3: Mask the port data with the channel group mask
        retPortLevel =(Dio_PortLevelType) ((portData & ChannelGroupIdPtr->mask) >> ChannelGroupIdPtr->offset);
    }
    return retPortLevel;
}

void Dio_WriteChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr, 
                           Dio_PortLevelType Level)
{
    GPIO_TypeDef *GPIO_Port;            // Pointer to the GPIO port
    // Step 1: Get the GPIO port from the channel group
    GPIO_Port = DIO_GET_PORT_PORT_ID(ChannelGroupIdPtr->port);

    if (GPIO_Port == NULL_PTR) {
        // Handle error: Invalid channel group
        return; // or some error handling
    }
    // Step 2: Read the entire state of all channels in the channel group 
    // Read the input data register of the port
    uint16 portData = GPIO_ReadInputData(GPIO_Port);
    
    // Step 3: Clear the bits corresponding to the channel group mask and set the new level
    // Clear the bits corresponding to the channel group mask
    portData &= ~(ChannelGroupIdPtr->mask); 
    // Set the bits corresponding to the channel group mask with the new level
    portData |= ((Level << ChannelGroupIdPtr->offset) & ChannelGroupIdPtr->mask); 
    GPIO_Write(GPIO_Port, portData);
        
}

void Dio_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
    versioninfo->vendorID = DIO_VENDOR_ID;
    versioninfo->moduleID = DIO_MODULE_ID;
    versioninfo->sw_major_version = DIO_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = DIO_SW_MINOR_VERSION;   
    versioninfo->sw_patch_version = DIO_SW_PATCH_VERSION;
}

Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId)
{
    Dio_LevelType currentLevel = Dio_ReadChannel(ChannelId); // Read the current level of the channel
    Dio_LevelType newLevel = (currentLevel == STD_HIGH) ? STD_LOW : STD_HIGH;
    Dio_WriteChannel(ChannelId, newLevel); // Write the new level to the channel
    return newLevel;
}

void Dio_MaskWritePort(Dio_PortType PortId, 
                        Dio_PortLevelType Level, 
                        Dio_PortLevelType Mask)
{
    GPIO_TypeDef *GPIO_Port;  // Pointer to the GPIO port
    // Step 1: Get the GPIO port from the port ID
    GPIO_Port = DIO_GET_PORT_PORT_ID(PortId);
    if (GPIO_Port == NULL_PTR) {
        // Handle error: Invalid port ID
        return;
    }
    // Step 2: Write the masked level to the port
    uint16 portData = GPIO_ReadInputData(GPIO_Port);
       // Step 3: Clear the bits corresponding to the channel group mask and set the new level
    // Clear the bits corresponding to the channel group mask
    portData &= ~Mask; 
    // Set the bits corresponding to the channel group mask with the new level
    portData |= (Level & Mask); 

    GPIO_Write(GPIO_Port, portData);
}