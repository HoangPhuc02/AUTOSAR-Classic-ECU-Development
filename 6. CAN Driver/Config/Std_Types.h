/****************************************************************************************
*                                STD_TYPES.H                                           *
****************************************************************************************
* File Name   : Std_Types.h
* Module      : Standard Types
* Description : AUTOSAR standard type definitions
* Version     : 1.0.0
* Date        : 20/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
* Doc Link    : https://www.autosar.org/fileadmin/standards/R20-11/CP/AUTOSAR_SWS_StandardTypes.pdf
****************************************************************************************/

#ifndef STD_TYPES_H
#define STD_TYPES_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/


/****************************************************************************************
*                              VERSION INFORMATION                                     *
****************************************************************************************/


/****************************************************************************************
*                              BOOLEAN TYPE DEFINITIONS                                *
****************************************************************************************/


/****************************************************************************************
*                              INTEGER TYPE DEFINITIONS                                *
****************************************************************************************/
typedef signed char         sint8;      /* 8-bit signed integer     */
typedef unsigned char       uint8;      /* 8-bit unsigned integer   */
typedef signed short        sint16;     /* 16-bit signed integer    */
typedef unsigned short      uint16;     /* 16-bit unsigned integer  */
typedef signed long         sint32;     /* 32-bit signed integer    */
typedef unsigned long       uint32;     /* 32-bit unsigned integer  */
typedef signed long long    sint64;     /* 64-bit signed integer    */
typedef unsigned long long  uint64;     /* 64-bit unsigned integer  */

typedef enum {FALSE, TRUE}  boolean;
/****************************************************************************************
*                              POINTER  DEFINITIONS                                     *
****************************************************************************************/
#define NULL_PTR ((void*)0) /* Null pointer definition */

/****************************************************************************************
*                              STANDARD RETURN TYPES                                   *
****************************************************************************************/
#define STD_HIGH    0x01u   /* Physical state 5V or 3.3V */ 
#define STD_LOW     0x00u   /* Physical state 0V */ 

#define STD_ON      0x01u   /* Logical state TRUE */
#define STD_OFF     0x00u 

#define STD_ACTIVE  0x01u   /* Logical state ACTIVE */
#define STD_IDLE    0x00u   /* Logical state IDLE */
/*
#ifndef STATUSTYPEDEFINED  
#define STATUSTYPEDEFINED  
#define E_OK 0x00u 
 
typedef unsigned char StatusType; // OSEK compliance  
#endif 
 
#define E_NOT_OK 0x01u
*/ 

typedef uint8 Std_ReturnType; 
#define E_OK        (Std_ReturnType)0x00U  /* No error */
#define E_NOT_OK    (Std_ReturnType)0x01U  /* Error occurred */

/****************************************************************************************
*                              VERSION INFO STRUCTURE                                  *
****************************************************************************************/
/*
This type shall be used to request the version of a BSW module using the 
<Modulename>_GetVersionInfo() function. 
*/
typedef struct Std_VersionInfoType
{
    uint16 vendorID;        /* Vendor ID */
    uint16 moduleID;        /* Module ID */
    uint8 sw_major_version;/* Software major version */
    uint8 sw_minor_version;/* Software minor version */
    uint8 sw_patch_version;/* Software patch version */
} Std_VersionInfoType;

/****************************************************************************************
*                              COMMON MACROS                                           *
****************************************************************************************/


#endif /* STD_TYPES_H */