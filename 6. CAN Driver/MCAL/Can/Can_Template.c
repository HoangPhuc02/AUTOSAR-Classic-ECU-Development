/****************************************************************************************
*                               CAN.C                                                  *
****************************************************************************************
* File Name   : Can.c
* Module      : Controller Area Network (CAN)
* Description : AUTOSAR CAN driver implementation file - Template implementation
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 24/08/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Can.h"
#include "Can_Types.h"
#include "Can_Cfg.h"
#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#if (CAN_DEV_ERROR_DETECT == STD_ON)
    #include "Det.h"
#endif

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CAN_VENDOR_ID_C                     (0x0001U)
#define CAN_MODULE_ID_C                     (0x007BU)
#define CAN_AR_RELEASE_MAJOR_VERSION_C      (4U)
#define CAN_AR_RELEASE_MINOR_VERSION_C      (4U)
#define CAN_AR_RELEASE_REVISION_VERSION_C   (0U)
#define CAN_SW_MAJOR_VERSION_C              (1U)
#define CAN_SW_MINOR_VERSION_C              (0U)
#define CAN_SW_PATCH_VERSION_C              (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if Can.c file and Can.h file are of the same vendor */
#if (CAN_VENDOR_ID_C != CAN_VENDOR_ID)
    #error "Can.c and Can.h have different vendor ids"
#endif

/* Check if Can.c file and Can.h file are of the same AUTOSAR version */
#if ((CAN_AR_RELEASE_MAJOR_VERSION_C != CAN_AR_RELEASE_MAJOR_VERSION) || \
     (CAN_AR_RELEASE_MINOR_VERSION_C != CAN_AR_RELEASE_MINOR_VERSION) || \
     (CAN_AR_RELEASE_REVISION_VERSION_C != CAN_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR Version Numbers of Can.c and Can.h are different"
#endif

/* Check if Can.c file and Can.h file are of the same software version */
#if ((CAN_SW_MAJOR_VERSION_C != CAN_SW_MAJOR_VERSION) || \
     (CAN_SW_MINOR_VERSION_C != CAN_SW_MINOR_VERSION) || \
     (CAN_SW_PATCH_VERSION_C != CAN_SW_PATCH_VERSION))
    #error "Software Version Numbers of Can.c and Can.h are different"
#endif

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/**
 * @brief   CAN Channel Runtime Information
 * @details Structure to hold runtime information for each CAN channel/controller
 */
typedef struct
{
    Can_ControllerStateType ControllerState;       /**< Current Controller State */
    Can_ErrorStateType ErrorState;                 /**< Current Error State */
    boolean InterruptsEnabled;                     /**< Interrupt Enable Status */
    boolean WakeupFlag;                           /**< Wakeup Detection Flag */
    uint8 RxErrorCounter;                         /**< Receive Error Counter */
    uint8 TxErrorCounter;                         /**< Transmit Error Counter */
    uint32 TimeoutCounter;                        /**< Timeout Counter for operations */
} Can_ControllerRuntimeType;

/**
 * @brief   CAN Hardware Object Information
 * @details Structure to hold information for each hardware object (HTH/HRH)
 */
typedef struct
{
    Can_HwHandleType HwHandle;                    /**< Hardware Object Handle */
    Can_IdType CanId;                            /**< Associated CAN ID */
    uint8 ControllerId;                          /**< Controller ID this HW object belongs to */
    boolean InUse;                               /**< Hardware object usage flag */
} Can_HwObjectRuntimeType;

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/* STM32F103 CAN Hardware specific defines */
#define CAN_CONTROLLER_0                    (0U)
#define CAN_CONTROLLER_1                    (1U)

/* CAN Baudrate Configuration IDs */
#define CAN_BAUDRATE_125KBPS               (0U)
#define CAN_BAUDRATE_250KBPS               (1U)
#define CAN_BAUDRATE_500KBPS               (2U)
#define CAN_BAUDRATE_1MBPS                 (3U)

/* Hardware specific limits */
#define CAN_MAX_TX_MAILBOXES               (3U)
#define CAN_MAX_RX_FIFO_SIZE               (3U)

/* Timeout values */
#define CAN_TIMEOUT_DURATION               (1000U)    /**< General timeout in ms */
#define CAN_MODE_CHANGE_TIMEOUT            (100U)     /**< Mode change timeout in ms */

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/**
 * @brief   CAN Baudrate Configuration Table
 * @details Pre-calculated baudrate settings for different speeds
 */
static const struct {
    uint32 Prescaler;
    uint32 SJW;
    uint32 BS1; 
    uint32 BS2;
} Can_BaudrateTable[] = {
    /* 125kbps: (72MHz / (1 + 13 + 2) / 36) = 125kbps */
    {36, CAN_SJW_1tq, CAN_BS1_13tq, CAN_BS2_2tq},    
    /* 250kbps: (72MHz / (1 + 13 + 2) / 18) = 250kbps */
    {18, CAN_SJW_1tq, CAN_BS1_13tq, CAN_BS2_2tq},    
    /* 500kbps: (72MHz / (1 + 13 + 2) / 9) = 500kbps */  
    {9,  CAN_SJW_1tq, CAN_BS1_13tq, CAN_BS2_2tq},    
    /* 1Mbps: (72MHz / (1 + 13 + 2) / 4.5) = 1Mbps */
    {4,  CAN_SJW_1tq, CAN_BS1_6tq,  CAN_BS2_1tq}     
};

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/

/**
 * @brief   CAN Driver State
 * @details Global variable to track if the CAN driver is initialized
 */
static boolean Can_DriverInitialized = FALSE;

/**
 * @brief   CAN Configuration Pointer
 * @details Pointer to the current configuration set
 */
static const Can_ConfigType* Can_ConfigPtr = NULL_PTR;

/**
 * @brief   CAN Controller Runtime Information
 * @details Array to store runtime information for each CAN controller
 */
static Can_ControllerRuntimeType Can_ControllerRuntime[CAN_MAX_HW_UNITS];

/**
 * @brief   CAN Hardware Objects Runtime Information
 * @details Array to store runtime information for hardware objects
 */
static Can_HwObjectRuntimeType Can_HwObjectRuntime[CAN_MAX_TX_MAILBOXES + CAN_MAX_RX_FIFO_SIZE];

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/**
 * @brief   Initialize CAN GPIO pins
 * @details Configure GPIO pins for CAN functionality (PA11/PA12 for CAN1, PB12/PB13 for CAN2)
 * @param[in] Controller - CAN controller ID
 */
static void Can_InitGpio(uint8 Controller);

/**
 * @brief   Configure CAN controller baudrate
 * @details Set the baudrate configuration for the specified controller
 * @param[in] Controller - CAN controller ID
 * @param[in] BaudrateConfigID - Baudrate configuration ID
 * @return  Std_ReturnType - E_OK if successful, E_NOT_OK otherwise
 */
static Std_ReturnType Can_ConfigureBaudrate(uint8 Controller, uint16 BaudrateConfigID);

/**
 * @brief   Configure CAN filters
 * @details Set up CAN message filters for reception
 * @param[in] Controller - CAN controller ID
 */
static void Can_ConfigureFilters(uint8 Controller);

/**
 * @brief   Get STM32 CAN peripheral pointer
 * @details Returns pointer to CAN_TypeDef structure for the specified controller
 * @param[in] Controller - CAN controller ID
 * @return  CAN_TypeDef* - Pointer to CAN peripheral or NULL_PTR if invalid
 */
static CAN_TypeDef* Can_GetControllerPtr(uint8 Controller);

/**
 * @brief   Validate Controller ID
 * @details Check if the controller ID is valid
 * @param[in] Controller - CAN controller ID to validate
 * @return  boolean - TRUE if valid, FALSE otherwise
 */
static boolean Can_ValidateController(uint8 Controller);

/**
 * @brief   Validate Hardware Handle
 * @details Check if the hardware handle is valid
 * @param[in] Hth - Hardware handle to validate
 * @return  boolean - TRUE if valid, FALSE otherwise
 */
static boolean Can_ValidateHwHandle(Can_HwHandleType Hth);

/**
 * @brief   Report Development Error
 * @details Report development error to DET if enabled
 * @param[in] ApiId - API service ID where error occurred
 * @param[in] ErrorId - Development error ID
 */
static void Can_ReportError(uint8 ApiId, uint8 ErrorId);

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/**
 * @brief   Initialize CAN GPIO pins
 */
static void Can_InitGpio(uint8 Controller)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    if (Controller == CAN_CONTROLLER_0)
    {
        /* Enable GPIOA clock for CAN1 (PA11/PA12) */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        
        /* Configure CAN1 RX pin (PA11) */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        /* Configure CAN1 TX pin (PA12) */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if (Controller == CAN_CONTROLLER_1)
    {
        /* Enable GPIOB clock for CAN2 (PB12/PB13) */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        
        /* Configure CAN2 RX pin (PB12) */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        /* Configure CAN2 TX pin (PB13) */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
}

/**
 * @brief   Configure CAN controller baudrate
 */
static Std_ReturnType Can_ConfigureBaudrate(uint8 Controller, uint16 BaudrateConfigID)
{
    Std_ReturnType ReturnValue = E_NOT_OK;
    CAN_TypeDef* CanPtr;
    CAN_InitTypeDef CAN_InitStructure;
    
    /* Validate baudrate configuration ID */
    if (BaudrateConfigID >= (sizeof(Can_BaudrateTable) / sizeof(Can_BaudrateTable[0])))
    {
        return E_NOT_OK;
    }
    
    /* Get CAN controller pointer */
    CanPtr = Can_GetControllerPtr(Controller);
    if (CanPtr != NULL_PTR)
    {
        /* Initialize CAN structure */
        CAN_StructInit(&CAN_InitStructure);
        
        /* Configure timing parameters */
        CAN_InitStructure.CAN_Prescaler = Can_BaudrateTable[BaudrateConfigID].Prescaler;
        CAN_InitStructure.CAN_SJW = Can_BaudrateTable[BaudrateConfigID].SJW;
        CAN_InitStructure.CAN_BS1 = Can_BaudrateTable[BaudrateConfigID].BS1;
        CAN_InitStructure.CAN_BS2 = Can_BaudrateTable[BaudrateConfigID].BS2;
        
        /* Configure CAN operating mode */
        CAN_InitStructure.CAN_TTCM = DISABLE;  /* Time Triggered Communication Mode */
        CAN_InitStructure.CAN_ABOM = ENABLE;   /* Automatic Bus-Off Management */
        CAN_InitStructure.CAN_AWUM = ENABLE;   /* Automatic Wake-Up Mode */
        CAN_InitStructure.CAN_NART = DISABLE;  /* Non-Automatic Retransmission */
        CAN_InitStructure.CAN_RFLM = DISABLE;  /* Receive FIFO Locked Mode */
        CAN_InitStructure.CAN_TXFP = DISABLE;  /* Transmit FIFO Priority */
        CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  /* Normal operating mode */
        
        /* Initialize CAN controller */
        if (CAN_Init(CanPtr, &CAN_InitStructure) == CAN_InitStatus_Success)
        {
            ReturnValue = E_OK;
        }
    }
    
    return ReturnValue;
}

/**
 * @brief   Configure CAN filters
 */
static void Can_ConfigureFilters(uint8 Controller)
{
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    
    if (Controller == CAN_CONTROLLER_0)
    {
        /* Configure filter 0 for CAN1 - Accept all messages */
        CAN_FilterInitStructure.CAN_FilterNumber = 0;
        CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
        CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
        CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
        CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
        CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
        CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
        CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
        CAN_FilterInit(&CAN_FilterInitStructure);
    }
    /* Note: CAN2 filters are configured differently on STM32F103 */
}

/**
 * @brief   Get STM32 CAN peripheral pointer
 */
static CAN_TypeDef* Can_GetControllerPtr(uint8 Controller)
{
    CAN_TypeDef* CanPtr = NULL_PTR;
    
    switch (Controller)
    {
        case CAN_CONTROLLER_0:
            CanPtr = CAN1;
            break;
        case CAN_CONTROLLER_1:
            #ifdef CAN2  /* CAN2 may not be available on all STM32F103 variants */
            CanPtr = CAN2;
            #endif
            break;
        default:
            CanPtr = NULL_PTR;
            break;
    }
    
    return CanPtr;
}

/**
 * @brief   Validate Controller ID
 */
static boolean Can_ValidateController(uint8 Controller)
{
    return (boolean)(Controller < CAN_MAX_HW_UNITS);
}

/**
 * @brief   Validate Hardware Handle
 */
static boolean Can_ValidateHwHandle(Can_HwHandleType Hth)
{
    return (boolean)(Hth != CAN_HW_HANDLE_INVALID);
}

/**
 * @brief   Report Development Error
 */
static void Can_ReportError(uint8 ApiId, uint8 ErrorId)
{
#if (CAN_DEV_ERROR_DETECT == STD_ON)
    (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, ApiId, ErrorId);
#else
    /* Avoid compiler warnings */
    (void)ApiId;
    (void)ErrorId;
#endif
}

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/**
 * @brief   Initializes the CAN Hardware Units and Controllers
 */
void Can_Init(const Can_ConfigType* Config)
{
    uint8 ControllerIndex;
    
    /* Development error detection */
#if (CAN_DEV_ERROR_DETECT == STD_ON)
    if (Config == NULL_PTR)
    {
        Can_ReportError(CAN_SID_INIT, CAN_E_PARAM_POINTER);
        return;
    }
#endif
    
    /* Store configuration pointer */
    Can_ConfigPtr = Config;
    
    /* Initialize all controller runtime data */
    for (ControllerIndex = 0; ControllerIndex < CAN_MAX_HW_UNITS; ControllerIndex++)
    {
        Can_ControllerRuntime[ControllerIndex].ControllerState = CAN_CS_UNINIT;
        Can_ControllerRuntime[ControllerIndex].ErrorState = CAN_ERRORSTATE_ACTIVE;
        Can_ControllerRuntime[ControllerIndex].InterruptsEnabled = FALSE;
        Can_ControllerRuntime[ControllerIndex].WakeupFlag = FALSE;
        Can_ControllerRuntime[ControllerIndex].RxErrorCounter = 0;
        Can_ControllerRuntime[ControllerIndex].TxErrorCounter = 0;
        Can_ControllerRuntime[ControllerIndex].TimeoutCounter = 0;
        
        /* Initialize GPIO pins for each controller */
        Can_InitGpio(ControllerIndex);
        
        /* Enable CAN peripheral clock */
        if (ControllerIndex == CAN_CONTROLLER_0)
        {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
        }
        else if (ControllerIndex == CAN_CONTROLLER_1)
        {
            #ifdef RCC_APB1Periph_CAN2
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
            #endif
        }
        
        /* Set controller to STOPPED state after hardware initialization */
        Can_ControllerRuntime[ControllerIndex].ControllerState = CAN_CS_STOPPED;
    }
    
    /* Initialize hardware objects runtime data */
    for (ControllerIndex = 0; ControllerIndex < (CAN_MAX_TX_MAILBOXES + CAN_MAX_RX_FIFO_SIZE); ControllerIndex++)
    {
        Can_HwObjectRuntime[ControllerIndex].HwHandle = CAN_HW_HANDLE_INVALID;
        Can_HwObjectRuntime[ControllerIndex].CanId = 0;
        Can_HwObjectRuntime[ControllerIndex].ControllerId = CAN_CONTROLLER_ID_INVALID;
        Can_HwObjectRuntime[ControllerIndex].InUse = FALSE;
    }
    
    /* Set driver as initialized */
    Can_DriverInitialized = TRUE;
}

/**
 * @brief   Returns version information of this module
 */
#if (CAN_VERSION_INFO_API == STD_ON)
void Can_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
#if (CAN_DEV_ERROR_DETECT == STD_ON)
    if (versioninfo == NULL_PTR)
    {
        Can_ReportError(CAN_SID_GET_VERSION_INFO, CAN_E_PARAM_POINTER);
        return;
    }
#endif
    
    versioninfo->vendorID = CAN_VENDOR_ID;
    versioninfo->moduleID = CAN_MODULE_ID;
    versioninfo->sw_major_version = CAN_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = CAN_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = CAN_SW_PATCH_VERSION;
}
#endif

/**
 * @brief   De-initializes the module
 */
#if (CAN_DEINIT_API == STD_ON)
void Can_DeInit(void)
{
    uint8 ControllerIndex;
    
#if (CAN_DEV_ERROR_DETECT == STD_ON)
    if (Can_DriverInitialized == FALSE)
    {
        Can_ReportError(CAN_SID_DEINIT, CAN_E_UNINIT);
        return;
    }
#endif
    
    /* Reset all controllers to UNINIT state */
    for (ControllerIndex = 0; ControllerIndex < CAN_MAX_HW_UNITS; ControllerIndex++)
    {
        if (Can_ControllerRuntime[ControllerIndex].ControllerState != CAN_CS_UNINIT)
        {
            /* Set controller to STOPPED state first */
            (void)Can_SetControllerMode(ControllerIndex, CAN_CS_STOPPED);
            
            /* Then set to UNINIT */
            Can_ControllerRuntime[ControllerIndex].ControllerState = CAN_CS_UNINIT;
        }
    }
    
    /* Reset driver state */
    Can_DriverInitialized = FALSE;
    Can_ConfigPtr = NULL_PTR;
}
#endif

/**
 * @brief   This service shall set the baud rate configuration of the CAN controller
 */
#if (CAN_SET_BAUDRATE_API == STD_ON)
Std_ReturnType Can_SetBaudrate(uint8 Controller, uint16 BaudRateConfigID)
{
    Std_ReturnType ReturnValue = E_NOT_OK;
    
#if (CAN_DEV_ERROR_DETECT == STD_ON)
    if (Can_DriverInitialized == FALSE)
    {
        Can_ReportError(CAN_SID_SET_BAUDRATE, CAN_E_UNINIT);
        return E_NOT_OK;
    }
    
    if (Can_ValidateController(Controller) == FALSE)
    {
        Can_ReportError(CAN_SID_SET_BAUDRATE, CAN_E_PARAM_CONTROLLER);
        return E_NOT_OK;
    }
    
    if (Can_ControllerRuntime[Controller].ControllerState != CAN_CS_STOPPED)
    {
        Can_ReportError(CAN_SID_SET_BAUDRATE, CAN_E_TRANSITION);
        return E_NOT_OK;
    }
    
    if (BaudRateConfigID >= (sizeof(Can_BaudrateTable) / sizeof(Can_BaudrateTable[0])))
    {
        Can_ReportError(CAN_SID_SET_BAUDRATE, CAN_E_PARAM_BAUDRATE);
        return E_NOT_OK;
    }
#endif
    
    /* Configure the baudrate */
    ReturnValue = Can_ConfigureBaudrate(Controller, BaudRateConfigID);
    
    return ReturnValue;
}
#endif

/**
 * @brief   This function performs the change of the CAN controller state
 */
Std_ReturnType Can_SetControllerMode(uint8 Controller, Can_ControllerStateType Transition)
{
    Std_ReturnType ReturnValue = E_NOT_OK;
    CAN_TypeDef* CanPtr;
    uint32 TimeoutCounter = 0;
    
#if (CAN_DEV_ERROR_DETECT == STD_ON)
    if (Can_DriverInitialized == FALSE)
    {
        Can_ReportError(CAN_SID_SET_CONTROLLER_MODE, CAN_E_UNINIT);
        return E_NOT_OK;
    }
    
    if (Can_ValidateController(Controller) == FALSE)
    {
        Can_ReportError(CAN_SID_SET_CONTROLLER_MODE, CAN_E_PARAM_CONTROLLER);
        return E_NOT_OK;
    }
#endif
    
    CanPtr = Can_GetControllerPtr(Controller);
    if (CanPtr == NULL_PTR)
    {
        return E_NOT_OK;
    }
    
    switch (Transition)
    {
        case CAN_CS_STARTED:
            if (Can_ControllerRuntime[Controller].ControllerState == CAN_CS_STOPPED)
            {
                /* Exit initialization mode and start normal operation */
                CanPtr->MCR &= ~CAN_MCR_INRQ;
                
                /* Wait for normal mode */
                while (((CanPtr->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) && (TimeoutCounter < CAN_MODE_CHANGE_TIMEOUT))
                {
                    TimeoutCounter++;
                }
                
                if (TimeoutCounter < CAN_MODE_CHANGE_TIMEOUT)
                {
                    Can_ControllerRuntime[Controller].ControllerState = CAN_CS_STARTED;
                    ReturnValue = E_OK;
                }
            }
            break;
            
        case CAN_CS_STOPPED:
            if ((Can_ControllerRuntime[Controller].ControllerState == CAN_CS_STARTED) ||
                (Can_ControllerRuntime[Controller].ControllerState == CAN_CS_SLEEP))
            {
                /* Enter initialization mode */
                CanPtr->MCR |= CAN_MCR_INRQ;
                
                /* Wait for initialization mode */
                while (((CanPtr->MSR & CAN_MSR_INAK) != CAN_MSR_INAK) && (TimeoutCounter < CAN_MODE_CHANGE_TIMEOUT))
                {
                    TimeoutCounter++;
                }
                
                if (TimeoutCounter < CAN_MODE_CHANGE_TIMEOUT)
                {
                    Can_ControllerRuntime[Controller].ControllerState = CAN_CS_STOPPED;
                    ReturnValue = E_OK;
                }
            }
            break;
            
        case CAN_CS_SLEEP:
            if (Can_ControllerRuntime[Controller].ControllerState == CAN_CS_STOPPED)
            {
                /* Enter sleep mode */
                CanPtr->MCR |= CAN_MCR_SLEEP;
                
                /* Wait for sleep mode */
                while (((CanPtr->MSR & CAN_MSR_SLAK) != CAN_MSR_SLAK) && (TimeoutCounter < CAN_MODE_CHANGE_TIMEOUT))
                {
                    TimeoutCounter++;
                }
                
                if (TimeoutCounter < CAN_MODE_CHANGE_TIMEOUT)
                {
                    Can_ControllerRuntime[Controller].ControllerState = CAN_CS_SLEEP;
                    ReturnValue = E_OK;
                }
            }
            break;
            
        default:
#if (CAN_DEV_ERROR_DETECT == STD_ON)
            Can_ReportError(CAN_SID_SET_CONTROLLER_MODE, CAN_E_TRANSITION);
#endif
            break;
    }
    
    return ReturnValue;
}

/**
 * @brief   This function disables all interrupts for this CAN controller
 */
void Can_DisableControllerInterrupts(uint8 Controller)
{
    CAN_TypeDef* CanPtr;
    
#if (CAN_DEV_ERROR_DETECT == STD_ON)
    if (Can_DriverInitialized == FALSE)
    {
        Can_ReportError(CAN_SID_DISABLE_CONTROLLER_INTERRUPTS, CAN_E_UNINIT);
        return;
    }
    
    if (Can_ValidateController(Controller) == FALSE)
    {
        Can_ReportError(CAN_SID_DISABLE_CONTROLLER_INTERRUPTS, CAN_E_PARAM_CONTROLLER);
        return;
    }
#endif
    
    CanPtr = Can_GetControllerPtr(Controller);
    if (CanPtr != NULL_PTR)
    {
        /* Disable all CAN interrupts */
        CanPtr->IER = 0;
        Can_ControllerRuntime[Controller].InterruptsEnabled = FALSE;
    }
}

/**
 * @brief   This function enables all allowed interrupts for this CAN controller
 */
void Can_EnableControllerInterrupts(uint8 Controller)
{
    CAN_TypeDef* CanPtr;
    
#if (CAN_DEV_ERROR_DETECT == STD_ON)
    if (Can_DriverInitialized == FALSE)
    {
        Can_ReportError(CAN_SID_ENABLE_CONTROLLER_INTERRUPTS, CAN_E_UNINIT);
        return;
    }
    
    if (Can_ValidateController(Controller) == FALSE)
    {
        Can_ReportError(CAN_SID_ENABLE_CONTROLLER_INTERRUPTS, CAN_E_PARAM_CONTROLLER);
        return;
    }
#endif
    
    CanPtr = Can_GetControllerPtr(Controller);
    if (CanPtr != NULL_PTR)
    {
        /* Enable necessary CAN interrupts */
        CanPtr->IER = CAN_IT_TME |    /* Transmit mailbox empty */
                      CAN_IT_FMP0 |   /* FIFO 0 message pending */
                      CAN_IT_FMP1 |   /* FIFO 1 message pending */
                      CAN_IT_BOF |    /* Bus-off */
                      CAN_IT_EPV |    /* Error passive */
                      CAN_IT_EWG;     /* Error warning */
        
        Can_ControllerRuntime[Controller].InterruptsEnabled = TRUE;
    }
}

/**
 * @brief   This function is called by CanIf to pass a CAN message to CanDrv for transmission
 */
Std_ReturnType Can_Write(Can_HwHandleType Hth, const Can_PduType* PduInfo)
{
    Std_ReturnType ReturnValue = E_NOT_OK;
    CAN_TypeDef* CanPtr;
    CanTxMsg TxMessage;
    uint8 TransmitMailbox;
    uint8 Controller;
    uint8 DataIndex;
    
#if (CAN_DEV_ERROR_DETECT == STD_ON)
    if (Can_DriverInitialized == FALSE)
    {
        Can_ReportError(CAN_SID_WRITE, CAN_E_UNINIT);
        return E_NOT_OK;
    }
    
    if (PduInfo == NULL_PTR)
    {
        Can_ReportError(CAN_SID_WRITE, CAN_E_PARAM_POINTER);
        return E_NOT_OK;
    }
    
    if (PduInfo->sdu == NULL_PTR)
    {
        Can_ReportError(CAN_SID_WRITE, CAN_E_PARAM_POINTER);
        return E_NOT_OK;
    }
    
    if (PduInfo->length > 8)
    {
        Can_ReportError(CAN_SID_WRITE, CAN_E_PARAM_DLC);
        return E_NOT_OK;
    }
    
    if (Can_ValidateHwHandle(Hth) == FALSE)
    {
        Can_ReportError(CAN_SID_WRITE, CAN_E_PARAM_HANDLE);
        return E_NOT_OK;
    }
#endif
    
    /* For this template, assume Hth maps directly to controller ID */
    Controller = (uint8)Hth;  /* This should be derived from configuration */
    
#if (CAN_DEV_ERROR_DETECT == STD_ON)
    if (Can_ControllerRuntime[Controller].ControllerState != CAN_CS_STARTED)
    {
        Can_ReportError(CAN_SID_WRITE, CAN_E_TRANSITION);
        return E_NOT_OK;
    }
#endif
    
    CanPtr = Can_GetControllerPtr(Controller);
    if (CanPtr == NULL_PTR)
    {
        return E_NOT_OK;
    }
    
    /* Prepare TX message structure */
    TxMessage.StdId = (uint32)(PduInfo->id & CAN_STD_ID_MASK);
    TxMessage.ExtId = (uint32)(PduInfo->id & CAN_EXT_ID_MASK);
    
    /* Check if Extended ID */
    if ((PduInfo->id & CAN_FRAME_TYPE_MASK) != 0)
    {
        TxMessage.IDE = CAN_ID_EXT;
    }
    else
    {
        TxMessage.IDE = CAN_ID_STD;
    }
    
    TxMessage.RTR = CAN_RTR_DATA;  /* Data frame */
    TxMessage.DLC = PduInfo->length;
    
    /* Copy data */
    for (DataIndex = 0; DataIndex < PduInfo->length; DataIndex++)
    {
        TxMessage.Data[DataIndex] = PduInfo->sdu[DataIndex];
    }
    
    /* Transmit message */
    TransmitMailbox = CAN_Transmit(CanPtr, &TxMessage);
    
    if (TransmitMailbox != CAN_TxStatus_NoMailBox)
    {
        ReturnValue = E_OK;
    }
    else
    {
        ReturnValue = CAN_BUSY;  /* No free mailbox */
    }
    
    return ReturnValue;
}

/*============================= Polling Functions (Template Implementation) ===========================*/

#if (CAN_TX_PROCESSING == POLLING)
void Can_MainFunction_Write(void)
{
    /* Template implementation - Poll TX confirmation */
    uint8 ControllerIndex;
    CAN_TypeDef* CanPtr;
    
    if (Can_DriverInitialized == TRUE)
    {
        for (ControllerIndex = 0; ControllerIndex < CAN_MAX_HW_UNITS; ControllerIndex++)
        {
            CanPtr = Can_GetControllerPtr(ControllerIndex);
            if ((CanPtr != NULL_PTR) && (Can_ControllerRuntime[ControllerIndex].ControllerState == CAN_CS_STARTED))
            {
                /* Check for TX completion - Implementation specific */
                /* This should call CanIf_TxConfirmation() when transmission is complete */
            }
        }
    }
}
#endif

#if (CAN_RX_PROCESSING == POLLING)
void Can_MainFunction_Read(void)
{
    /* Template implementation - Poll RX indication */
    uint8 ControllerIndex;
    CAN_TypeDef* CanPtr;
    CanRxMsg RxMessage;
    
    if (Can_DriverInitialized == TRUE)
    {
        for (ControllerIndex = 0; ControllerIndex < CAN_MAX_HW_UNITS; ControllerIndex++)
        {
            CanPtr = Can_GetControllerPtr(ControllerIndex);
            if ((CanPtr != NULL_PTR) && (Can_ControllerRuntime[ControllerIndex].ControllerState == CAN_CS_STARTED))
            {
                /* Check FIFO 0 for received messages */
                if (CAN_MessagePending(CanPtr, CAN_FIFO0) > 0)
                {
                    CAN_Receive(CanPtr, CAN_FIFO0, &RxMessage);
                    /* This should call CanIf_RxIndication() with received data */
                }
                
                /* Check FIFO 1 for received messages */
                if (CAN_MessagePending(CanPtr, CAN_FIFO1) > 0)
                {
                    CAN_Receive(CanPtr, CAN_FIFO1, &RxMessage);
                    /* This should call CanIf_RxIndication() with received data */
                }
            }
        }
    }
}
#endif

#if (CAN_BUSOFF_PROCESSING == POLLING)
void Can_MainFunction_BusOff(void)
{
    /* Template implementation - Poll bus-off events */
    uint8 ControllerIndex;
    CAN_TypeDef* CanPtr;
    
    if (Can_DriverInitialized == TRUE)
    {
        for (ControllerIndex = 0; ControllerIndex < CAN_MAX_HW_UNITS; ControllerIndex++)
        {
            CanPtr = Can_GetControllerPtr(ControllerIndex);
            if (CanPtr != NULL_PTR)
            {
                /* Check for bus-off condition */
                if ((CanPtr->ESR & CAN_ESR_BOFF) == CAN_ESR_BOFF)
                {
                    Can_ControllerRuntime[ControllerIndex].ErrorState = CAN_ERRORSTATE_BUSOFF;
                    /* This should call CanIf_ControllerBusOff() */
                }
            }
        }
    }
}
#endif

/* Additional API functions would be implemented similarly following AUTOSAR specification */

/*================================================================================================*/
