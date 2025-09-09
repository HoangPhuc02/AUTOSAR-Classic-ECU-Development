/****************************************************************************************
*                               CAN.C                                                  *
****************************************************************************************
* File Name   : Can.c
* Module      : Controller Area Network (CAN)
* Description : AUTOSAR Can driver implementation file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 27/07/2025
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


/* Check if Lin.c file and Lin.h file are of the same AUTOSAR version */
#if ((LIN_AR_RELEASE_MAJOR_VERSION_C != LIN_AR_RELEASE_MAJOR_VERSION) || \
     (LIN_AR_RELEASE_MINOR_VERSION_C != LIN_AR_RELEASE_MINOR_VERSION) || \
     (LIN_AR_RELEASE_REVISION_VERSION_C != LIN_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR Version Numbers of Lin.c and Lin.h are different"
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
 * @brief   LIN Channel Runtime Information
 * @details Structure to hold runtime information for each LIN channel
 */
typedef struct
{

} Can_ChannelRuntimeType;

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

typedef struct {
    uint32 Prescaler;
    uint32 SJW;
    uint32 BS1;
    uint32 BS2;
} Can_BaudrateConfigType;

typedef enum {
    CAN_BAUD_125KBPS,
    CAN_BAUD_250KBPS,
    CAN_BAUD_500KBPS,
    CAN_BAUD_1MBPS
} Can_BaudrateType;

typedef enum {
    CAN_MODE_NORMAL          = CAN_Mode_Normal,             /*!< normal mode */
    CAN_MODE_LOOPBACK        = CAN_Mode_LoopBack,          /*!< loopback mode */
    CAN_MODE_SILENT          = CAN_Mode_Silent,             /*!< silent mode */
    CAN_MODE_SILENT_LOOPBACK = CAN_Mode_Silent_LoopBack    /*!< loopback combined with silent mode */
} Can_ModeType;
/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/**
 * @brief   CAN Controller Hardware Mapping
 * @details Maps logical CAN controller IDs to physical CAN peripherals using configuration
 */
#define CAN_GET_HW_CONTROLLER(controllerId) \
    ((controllerId) < CAN_MAX_CONTROLLERS ? \
     (CAN_TypeDef*)Can_ControllerHwConfig[controllerId].BaseAddress : NULL)

/**
 * @brief   CAN Controller Clock Enable Mapping
 * @details Maps logical CAN controller IDs to their respective clock enable functions using config
 */
#define CAN_ENABLE_CONTROLLER_CLOCK(controllerId) \
    do { \
        if ((controllerId) < CAN_MAX_CONTROLLERS && Can_ControllerHwConfig[controllerId].Enabled) { \
            RCC_APB1PeriphClockCmd(Can_ControllerHwConfig[controllerId].CanClockCmd, ENABLE); \
        } \
    } while(0)

/**
 * @brief   CAN Controller GPIO Clock Enable Mapping
 * @details Maps logical CAN controller IDs to their GPIO clock enable functions using config
 */
#define CAN_ENABLE_GPIO_CLOCK(controllerId) \
    do { \
        if ((controllerId) < CAN_MAX_CONTROLLERS && Can_ControllerHwConfig[controllerId].Enabled) { \
            RCC_APB2PeriphClockCmd(Can_ControllerHwConfig[controllerId].GpioClockCmd, ENABLE); \
        } \
    } while(0)

/**
 * @brief   CAN Controller GPIO Mapping (DEPRECATED)
 * @details This structure is now replaced by Can_ControllerHwConfig in Can_Cfg.c
 *          Kept for backward compatibility, but should use config-based approach
 */
// static const Can_GpioPinMapping Can_GpioMap[] = {
//     /* CAN1: PA12(TX), PA11(RX) */
//     {GPIOA, GPIO_Pin_12, GPIO_Pin_11, RCC_APB2Periph_GPIOA},
//     /* CAN2: PB13(TX), PB12(RX) - Alternative pins for CAN2 */
//     {GPIOB, GPIO_Pin_13, GPIO_Pin_12, RCC_APB2Periph_GPIOB}
// };

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/
/**
 * @brief   CAN Baudrate Configuration Table
 * @details Pre-calculated baudrate settings for different speeds
 */
// TODO make a function to auto calculate baud rate
static const Can_BaudrateConfigType Can_BaudrateTable[] = {
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
 * @brief LIN Driver Configuration Pointer
 * @note This pointer is set during initialization and used throughout the driver
 *       to access the configuration data.
 * @details It points to the configuration structure defined in Can_Types.h.
 */
const Can_ConfigType* Can_ConfigPtr = NULL_PTR;

/**
 * @brief CAN Channel Runtime Information
 * @details Array to store runtime information for each CAN channel
 */
static Can_ChannelRuntimeType Can_ChannelRuntime[CAN_MAX_CONFIGURED_CHANNELS];

/**
 * @brief CAN Driver State
 */
static boolean Can_DriverInitialized = CAN_UNINIT;

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static void Can_InitHw(uint8 Controller, const Can_ChannelConfigType* ChannelConfig);
static inline void Can_DeInitChannel(uint8 Controller);

static inline void Can_HwConfig(uint8 Controller, uint32 BaudRate);
static inline void Can_FilterConfig(uint8 Controller);
static inline void Can_GpioConfig(uint8 Controller);

// Two way to config baud rate
// Using id default from table or set up by hand
static Std_ReturnType Can_BaudrateConfigID(uint8 Controller, uint16 Baudrate);
static Std_ReturnType Can_BaudrateConfig(uint8 Controller, Can_BaudrateConfigType Baudrate);
// TODO: need to set flag
static inline void Can_EnableInterrupts(uint8 Controller);
static inline void Can_DisableInterrupts(uint8 Controller);


#if (CAN_DEV_ERROR_DETECT == STD_ON)
static inline boolean Can_ValidateDriverInitialized(void);
/**
 * @brief   Validate group parameter
 * @details This function validates if the group parameter is within valid range
 * 
 * @param[in]   Group           Group ID to validate
 * 
 * @return      boolean         TRUE if group is valid, FALSE otherwise
 */

static inline boolean Can_ValidateChannel(Can_ChannelType Channel);


/**
 * @brief   Validate pointer parameter
 * @details This function validates if the pointer parameter is not NULL
 * 
 * @param[in]   Pointer         Pointer to validate
 * 
 * @return      boolean         TRUE if pointer is valid, FALSE otherwise
 */
static inline boolean Can_ValidatePointer(Can_ChannelConfigType* Pointer);
#endif

/*==================================================================================================
*                                   LOCAL FUNCTION 
==================================================================================================*/

static inline void Can_HwConfig(uint8 Controller, uint32 BaudRate)
{
    CAN_TypeDef* CanPtr = CAN_GET_HW_CONTROLLER(Controller);
    CAN_InitTypeDef CAN_InitStructure;
    
    /* Validate controller and get hardware pointer */
    if (CanPtr == NULL || !Can_IsControllerEnabled(Controller)) {
        return; /* Invalid controller or not enabled */
    }
    
    /* Enable CAN controller clock */
    CAN_ENABLE_CONTROLLER_CLOCK(Controller);
    
    /* Reset CAN controller */
    CAN_DeInit(CanPtr);
    CAN_StructInit(&CAN_InitStructure);

    /* CAN Cell Configuration */
    CAN_InitStructure.CAN_TTCM = DISABLE;                  // Time Triggered Communication Mode
    CAN_InitStructure.CAN_ABOM = DISABLE;                  // Automatic Bus-Off Management
    CAN_InitStructure.CAN_AWUM = DISABLE;                  // Automatic Wake-Up Mode
    CAN_InitStructure.CAN_NART = DISABLE;                  // Non-Automatic Retransmission
    CAN_InitStructure.CAN_RFLM = DISABLE;                  // Receive FIFO Locked Mode
    CAN_InitStructure.CAN_TXFP = ENABLE;                   // Transmit FIFO Priority
    CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;        // <<<< LOOPBACK MODE FOR TESTING
    
    CAN_Init(CanPtr, &CAN_InitStructure);
}

static inline void Can_ModeConfig(uint8 Controller, Can_ModeType Can_Mode)
{
    CAN_TypeDef* CanPtr = CAN_GET_HW_CONTROLLER(Controller);
    CAN_InitTypeDef CAN_InitStructure;
    
    /* Validate controller */
    if (CanPtr == NULL || !Can_IsControllerEnabled(Controller)) {
        return; /* Invalid controller or not enabled */
    }
    
    CAN_InitStructure.CAN_Mode = Can_Mode;        // Set desired CAN mode
    CAN_Init(CanPtr, &CAN_InitStructure);
}

static inline void Can_FilterConfig(uint8 Controller)
{
    CAN_TypeDef* CanPtr = CAN_GET_HW_CONTROLLER(Controller);
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    
    /* Validate controller */
    if (CanPtr == NULL || !Can_IsControllerEnabled(Controller)) {
        return; /* Invalid controller or not enabled */
    }
    
    /* Configure filter based on controller ID to avoid conflicts */
    CAN_FilterInitStructure.CAN_FilterNumber = Controller;  /* Use controller ID as filter number */
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;  // Accept all
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;   // Accept all
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
}
static inline void Can_GpioConfig(uint8 Controller)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    const Can_ControllerHwConfigType* HwConfig;
    
    /* Validate controller */
    if (Controller >= CAN_MAX_CONTROLLERS || !Can_IsControllerEnabled(Controller)) {
        return; /* Invalid controller or not enabled */
    }
    
    HwConfig = &Can_ControllerHwConfig[Controller];
    
    /* Enable GPIO clock for CAN pins */
    RCC_APB2PeriphClockCmd(HwConfig->GpioClockCmd, ENABLE);
    
    /* Configure CAN RX pin */
    GPIO_InitStructure.GPIO_Pin = HwConfig->RxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HwConfig->GpioPort, &GPIO_InitStructure);
    
    /* Configure CAN TX pin */
    GPIO_InitStructure.GPIO_Pin = HwConfig->TxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HwConfig->GpioPort, &GPIO_InitStructure);
}

/**
 * @brief   Configure CAN controller baudrate using predefined ID
 */
static Std_ReturnType Can_BaudrateConfigID(uint8 Controller, uint16 BaudrateID)
{
    Std_ReturnType ReturnValue = E_NOT_OK;
    CAN_TypeDef* CanPtr = CAN_GET_HW_CONTROLLER(Controller);
    CAN_InitTypeDef CAN_InitStructure;
    
    /* Validate controller */
    if (CanPtr == NULL || !Can_IsControllerEnabled(Controller)) {
        return E_NOT_OK;
    }
    
    /* Validate baudrate configuration ID */
    if (BaudrateID >= (sizeof(Can_BaudrateTable) / sizeof(Can_BaudrateTable[0])))
    {
        return E_NOT_OK;
    }

    /* Initialize CAN structure */
    CAN_StructInit(&CAN_InitStructure);
    
    /* Configure timing parameters */
    CAN_InitStructure.CAN_Prescaler = Can_BaudrateTable[BaudrateID].Prescaler;
    CAN_InitStructure.CAN_SJW       = Can_BaudrateTable[BaudrateID].SJW;
    CAN_InitStructure.CAN_BS1       = Can_BaudrateTable[BaudrateID].BS1;
    CAN_InitStructure.CAN_BS2       = Can_BaudrateTable[BaudrateID].BS2;
    
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
    
    return ReturnValue;
}


static Std_ReturnType Can_BaudrateConfig(uint8 Controller, Can_BaudrateConfigType Baudrate)
{
    Std_ReturnType ReturnValue = E_NOT_OK;
    CAN_TypeDef* CanPtr = CAN_GET_HW_CONTROLLER(Controller);
    CAN_InitTypeDef CAN_InitStructure;
    
    /* Validate controller */
    if (CanPtr == NULL || !Can_IsControllerEnabled(Controller)) {
        return E_NOT_OK;
    }

    /* Initialize CAN structure */
    CAN_StructInit(&CAN_InitStructure);
    
    /* Configure timing parameters */
    CAN_InitStructure.CAN_Prescaler = Baudrate.Prescaler;
    CAN_InitStructure.CAN_SJW       = Baudrate.SJW;
    CAN_InitStructure.CAN_BS1       = Baudrate.BS1;
    CAN_InitStructure.CAN_BS2       = Baudrate.BS2;
    
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
    
    return ReturnValue;
}

// TODO: need to set flag
static inline void Can_EnableInterrupts(uint8 Controller)
{
    CAN_TypeDef* CanPtr = CAN_GET_HW_CONTROLLER(Controller);
    
    /* Validate controller */
    if (CanPtr == NULL || !Can_IsControllerEnabled(Controller)) {
        return; /* Invalid controller or not enabled */
    }
    
    /* Enable CAN interrupts */
    CAN_ITConfig(CanPtr, CAN_IT_TME, ENABLE);    /* Transmit mailbox empty interrupt */
    CAN_ITConfig(CanPtr, CAN_IT_FMP0, ENABLE);   /* FIFO 0 message pending interrupt */
    CAN_ITConfig(CanPtr, CAN_IT_FMP1, ENABLE);   /* FIFO 1 message pending interrupt */
    CAN_ITConfig(CanPtr, CAN_IT_BOF, ENABLE);    /* Bus-off interrupt */
    CAN_ITConfig(CanPtr, CAN_IT_EPV, ENABLE);    /* Error passive interrupt */
    CAN_ITConfig(CanPtr, CAN_IT_EWG, ENABLE);    /* Error warning interrupt */
}

static inline void Can_DisableInterrupts(uint8 Controller)
{
    CAN_TypeDef* CanPtr = CAN_GET_HW_CONTROLLER(Controller);
    
    /* Validate controller */
    if (CanPtr == NULL || !Can_IsControllerEnabled(Controller)) {
        return; /* Invalid controller or not enabled */
    }
    
    /* Disable CAN interrupts */
    CAN_ITConfig(CanPtr, CAN_IT_TME, DISABLE);   /* Transmit mailbox empty interrupt */
    CAN_ITConfig(CanPtr, CAN_IT_FMP0, DISABLE);  /* FIFO 0 message pending interrupt */
    CAN_ITConfig(CanPtr, CAN_IT_FMP1, DISABLE);  /* FIFO 1 message pending interrupt */
    CAN_ITConfig(CanPtr, CAN_IT_BOF, DISABLE);   /* Bus-off interrupt */
    CAN_ITConfig(CanPtr, CAN_IT_EPV, DISABLE);   /* Error passive interrupt */
    CAN_ITConfig(CanPtr, CAN_IT_EWG, DISABLE);   /* Error warning interrupt */
}


#if (CAN_DEV_ERROR_DETECT == STD_ON)
static inline boolean Can_ValidateDriverInitialized(void);
/**
 * @brief   Validate group parameter
 * @details This function validates if the group parameter is within valid range
 * 
 * @param[in]   Group           Group ID to validate
 * 
 * @return      boolean         TRUE if group is valid, FALSE otherwise
 */

static inline boolean Can_ValidateChannel(Can_ChannelType Channel);


/**
 * @brief   Validate pointer parameter
 * @details This function validates if the pointer parameter is not NULL
 * 
 * @param[in]   Pointer         Pointer to validate
 * 
 * @return      boolean         TRUE if pointer is valid, FALSE otherwise
 */
static inline boolean Can_ValidatePointer(Can_ChannelConfigType* Pointer);
#endif





/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/


/*======================= Services affecting the complete hardware unit ==========================*/
void Can_Init(const Can_ConfigType* Config)
{

}

 void Can_GetVersionInfo(Std_VersionInfoType* versioninfo);

 void Can_DeInit(void);

/*======================== Services affecting one single CAN Controller =========================*/

Std_ReturnType Can_SetBaudrate (uint8 Controller, uint16 BaudRateConfigID);

Std_ReturnType Can_SetControllerMode (uint8 Controller, Can_ControllerStateType Transition);

void Can_DisableControllerInterrupts (uint8 Controller);

void Can_EnableControllerInterrupts (uint8 Controller);

Std_ReturnType Can_CheckWakeup (uint8 Controller);

Std_ReturnType Can_GetControllerErrorState (uint8 ControllerId, Can_ErrorStateType* ErrorStatePtr);

Std_ReturnType Can_GetControllerMode (uint8 Controller, Can_ControllerStateType* ControllerModePtr);

Std_ReturnType Can_GetControllerRxErrorCounter (uint8 ControllerId, uint8* RxErrorCounterPtr);

Std_ReturnType Can_GetControllerTxErrorCounter (uint8 ControllerId, uint8* TxErrorCounterPtr);

Std_ReturnType Can_GetCurrentTime (uint8 ControllerId, Can_TimeStampType* timeStampPtr);

void Can_EnableEgressTimeStamp (Can_HwHandleType Hth);

Std_ReturnType Can_GetEgressTimeStamp (PduIdType TxPduId, Can_HwHandleType Hth, Can_TimeStampType* timeStampPtr);

Std_ReturnType Can_GetIngressTimeStamp (Can_HwHandleType Hrh, Can_TimeStampType* timeStampPtr);

/*=========================== Services affecting a Hardware Handle ==============================*/

Std_ReturnType Can_Write (Can_HwHandleType Hth, const Can_PduType* PduInfo);

/* =================================== Call-back notifications ==================================*/
// TODO : redefined this callback
boolean Can_LPDU_CalloutName (uint8 Hrh, Can_IdType CanId, uint8 CanDataLegth, const uint8* CanSduPtr);

/* ===================================== Scheduled functions ====================================*/

/** This function performs the polling of TX confirmation when CAN_TX_PROCESSING is set to
 POLLING. */
void Can_MainFunction_Write (void);

void Can_MainFunction_Read (void);

void Can_MainFunction_BusOff (void);

void Can_MainFunction_Wakeup (void);

void Can_MainFunction_Mode (void);
void Can_MainFunction_Mode (void);
