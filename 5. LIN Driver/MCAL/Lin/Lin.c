/****************************************************************************************
*                               LIN_TYPES.H                                            *
****************************************************************************************
* File Name   : Lin.c
* Module      : Local Interface Network (LIN)
* Description : AUTOSAR Lin driver implementation file
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
#include "Lin.h"
#include "Lin_Types.h"
#include "Lin_Cfg.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

#if (LIN_DEV_ERROR_DETECT == STD_ON)
    #include "Det.h"
#endif

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define LIN_VENDOR_ID_C                     (0x0001U)
#define LIN_MODULE_ID_C                     (0x007BU)
#define LIN_AR_RELEASE_MAJOR_VERSION_C      (4U)
#define LIN_AR_RELEASE_MINOR_VERSION_C      (4U)
#define LIN_AR_RELEASE_REVISION_VERSION_C   (0U)
#define LIN_SW_MAJOR_VERSION_C              (1U)
#define LIN_SW_MINOR_VERSION_C              (0U)
#define LIN_SW_PATCH_VERSION_C              (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if Lin.c file and Lin.h file are of the same vendor */
#if (LIN_VENDOR_ID_C != LIN_VENDOR_ID)
    #error "Lin.c and Lin.h have different vendor ids"
#endif

/* Check if Lin.c file and Lin.h file are of the same AUTOSAR version */
#if ((LIN_AR_RELEASE_MAJOR_VERSION_C != LIN_AR_RELEASE_MAJOR_VERSION) || \
     (LIN_AR_RELEASE_MINOR_VERSION_C != LIN_AR_RELEASE_MINOR_VERSION) || \
     (LIN_AR_RELEASE_REVISION_VERSION_C != LIN_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR Version Numbers of Lin.c and Lin.h are different"
#endif

/* Check if Lin.c file and Lin.h file are of the same software version */
#if ((LIN_SW_MAJOR_VERSION_C != LIN_SW_MAJOR_VERSION) || \
     (LIN_SW_MINOR_VERSION_C != LIN_SW_MINOR_VERSION) || \
     (LIN_SW_PATCH_VERSION_C != LIN_SW_PATCH_VERSION))
    #error "Software Version Numbers of Lin.c and Lin.h are different"
#endif

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/**
 * @brief LIN Frame Transmission State
 * @details Internal states for frame transmission state machine
 */
typedef enum _Lin_FrameTransmissionStateType
{
    LIN_FRAME_IDLE,                   /**< Idle state, no transmission */
    LIN_FRAME_TX_HEADER_BREAK,        /**< Transmitting break field */
    LIN_FRAME_TX_HEADER_SYNC,         /**< Transmitting sync field */
    LIN_FRAME_TX_HEADER_PID,          /**< Transmitting PID field */
    LIN_FRAME_TX_RESPONSE_DATA,       /**< Transmitting data bytes */
    LIN_FRAME_TX_RESPONSE_CHECKSUM,   /**< Transmitting checksum */
    LIN_FRAME_TX_COMPLETE,            /**< Transmission completed */
    LIN_FRAME_RX_DATA,                /**< Receiving data bytes */
    LIN_FRAME_RX_CHECKSUM             /**< Receiving checksum */
} Lin_FrameTransmissionStateType;

/**
 * @brief   LIN Channel Runtime Information
 * @details Structure to hold runtime information for each LIN channel
 */
typedef struct
{

    Lin_StatusType LinChannelState;             /**< Channel State */
    Lin_StatusType LinFrameStatus;              /**< Frame Status */
    Lin_FrameTransmissionStateType LinFrameTransmissionState; /**< Frame Transmission State */

    Lin_FramePidType LinCurrentPid;                         /**< Current PID */
    Lin_PduType LinCurrentPdu;                              /**< Current PDU */
    uint8 LinFrameBuffer[LIN_MAX_DATA_LENGTH];              /**< Frame Buffer */
    uint8 LinTxBuffer[LIN_MAX_DATA_LENGTH];                 /**< TX Buffer */
    uint8 LinRxBuffer[LIN_MAX_DATA_LENGTH];                 /**< RX Buffer */
    uint8 LinDataIndex;                                     /**< Current data index */
    uint8 LinFrameLength;                                   /**< Frame Length */
    uint8 LinCalculatedChecksum;                            /**< Calculated checksum */

    boolean LinWakeupFlag;                      /**< Wakeup Flag */
    uint32 LinTimeoutCounter;                   /**< Timeout Counter */
} Lin_ChannelRuntimeType;

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/

/**
 * @brief LIN Driver Configuration Pointer
 * @note This pointer is set during initialization and used throughout the driver
 *       to access the configuration data.
 * @details It points to the configuration structure defined in Lin_Types.h.
 */
const Lin_ConfigType* Lin_ConfigPtr = NULL_PTR;

/**
 * @brief LIN Channel Runtime Information
 * @details Array to store runtime information for each LIN channel
 */
static Lin_ChannelRuntimeType Lin_ChannelRuntime[LIN_MAX_CONFIGURED_CHANNELS];

/**
 * @brief LIN Driver State
 */
static boolean Lin_DriverInitialized = LIN_UNINIT;

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static void Lin_InitChannel(uint8 Channel, const Lin_ChannelConfigType* ChannelConfig);
static inline void Lin_DeInitChannel(uint8 Channel);

static inline void Lin_ConfigureUSART(uint8 Channel, uint32 BaudRate);
static inline void Lin_EnableInterrupts(uint8 Channel);
static inline void Lin_DisableInterrupts(uint8 Channel);

static uint8 Lin_CalculatePid(const uint8 Id);
static uint8 Lin_CalculateChecksum(Lin_FrameCsModelType cs, uint8 pid, const uint8* data, uint8 length);

static void Lin_SendBreakField(uint8 Channel);
static void Lin_SendSyncField(uint8 Channel);
static void Lin_SendPidField(uint8 Channel, uint8 Pid);
static void Lin_SendDataField(uint8 Channel, const uint8* DataPtr, uint8 Length);
static void Lin_SendChecksumField(uint8 Channel, uint8 Checksum);


void Lin_TxInterruptHandler(Lin_ChannelType Channel);
void Lin_RxInterruptHandler(Lin_ChannelType Channel);

#if (LIN_DEV_ERROR_DETECT == STD_ON)
static inline boolean Lin_ValidateDriverInitialized(void);
/**
 * @brief   Validate group parameter
 * @details This function validates if the group parameter is within valid range
 * 
 * @param[in]   Group           Group ID to validate
 * 
 * @return      boolean         TRUE if group is valid, FALSE otherwise
 */

static inline boolean Lin_ValidateChannel(Lin_ChannelType Channel);


/**
 * @brief   Validate pointer parameter
 * @details This function validates if the pointer parameter is not NULL
 * 
 * @param[in]   Pointer         Pointer to validate
 * 
 * @return      boolean         TRUE if pointer is valid, FALSE otherwise
 */
static inline boolean Lin_ValidatePointer(Lin_ChannelConfigType* Pointer);
#endif

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/
/**
 * @brief Initialize a LIN channel
 */
static void Lin_InitChannel(uint8 Channel, const Lin_ChannelConfigType* ChannelConfig)
{   
    /* Configure USART */
    Lin_ConfigureUSART(Channel, ChannelConfig->LinChannelBaudRate);
    
    /* Initialize channel runtime information */
    Lin_ChannelRuntime[Channel].LinChannelState = LIN_OPERATIONAL;
    Lin_ChannelRuntime[Channel].LinFrameStatus = LIN_NOT_OK;
    Lin_ChannelRuntime[Channel].LinFrameTransmissionState = LIN_FRAME_IDLE;
    Lin_ChannelRuntime[Channel].LinCurrentPid = 0x00U;
    Lin_ChannelRuntime[Channel].LinDataIndex = 0U;
    Lin_ChannelRuntime[Channel].LinFrameLength = 0U;
    Lin_ChannelRuntime[Channel].LinWakeupFlag = FALSE;
    Lin_ChannelRuntime[Channel].LinTimeoutCounter = 0U;

    /* Clear frame buffer */
    for (uint8 i = 0U; i < LIN_MAX_DATA_LENGTH; i++)
    {
        Lin_ChannelRuntime[Channel].LinFrameBuffer[i] = 0x00U;
        Lin_ChannelRuntime[Channel].LinTxBuffer[i] = 0x00U;
        Lin_ChannelRuntime[Channel].LinRxBuffer[i] = 0x00U;
    }
    
    /* Enable interrupts */
    Lin_EnableInterrupts(Channel);
}

/**
 * @brief Deinitialize a LIN channel
 */
static void Lin_DeInitChannel(uint8 Channel)
{
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Disable interrupts */
    Lin_DisableInterrupts(Channel);
    
    /* Disable USART */
    USART_Cmd(ChannelConfig->LinHwChannel, DISABLE);
    
    /* Reset channel state */
    Lin_ChannelRuntime[Channel].LinChannelState = LIN_UNINIT;
    Lin_ChannelRuntime[Channel].LinFrameStatus = LIN_NOT_OK;
}

/**
 * @brief Send LIN break field
 */
static void Lin_SendBreakField(uint8 Channel)
{
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Send break */
    USART_SendBreak(ChannelConfig->LinHwChannel);
}

/**
 * @brief Send LIN sync field
 */
static void Lin_SendSyncField(uint8 Channel)
{
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Wait for TXE flag */
    while (USART_GetFlagStatus(ChannelConfig->LinHwChannel, USART_FLAG_TXE) == RESET);
    
    /* Send sync byte */
    USART_SendData(ChannelConfig->LinHwChannel, LIN_SYNC_BYTE);
}

/**
 * @brief Send LIN PID field
 */
static void Lin_SendPidField(uint8 Channel, uint8 Pid)
{
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Wait for TXE flag */
    while (USART_GetFlagStatus(ChannelConfig->LinHwChannel, USART_FLAG_TXE) == RESET);
    
    /* Send PID */
    USART_SendData(ChannelConfig->LinHwChannel, Pid);
}

/**
 * @brief Send LIN data field
 */
static void Lin_SendDataField(uint8 Channel, const uint8* DataPtr, uint8 Length)
{
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    for (uint8 i = 0U; i < Length; i++)
    {
        /* Wait for TXE flag */
        while (USART_GetFlagStatus(ChannelConfig->LinHwChannel, USART_FLAG_TXE) == RESET);
        
        /* Send data byte */
        USART_SendData(ChannelConfig->LinHwChannel, DataPtr[i]);
    }
}

/**
 * @brief Send LIN checksum field
 */
static void Lin_SendChecksumField(uint8 Channel, uint8 Checksum)
{
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Wait for TXE flag */
    while (USART_GetFlagStatus(ChannelConfig->LinHwChannel, USART_FLAG_TXE) == RESET);
    
    /* Send checksum */
    USART_SendData(ChannelConfig->LinHwChannel, Checksum);
}

/**
 * @brief Abort ongoing transmission
 * 
 * @param[in] Channel LIN channel identifier
 */
static void Lin_AbortTransmission(uint8 Channel)
{
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Disable all transmission interrupts */
    USART_ITConfig(ChannelConfig->LinHwChannel, USART_IT_TC, DISABLE);
    USART_ITConfig(ChannelConfig->LinHwChannel, USART_IT_TXE, DISABLE);
    
    /* Reset frame state */
    Lin_ChannelRuntime[Channel].LinFrameStatus = LIN_OPERATIONAL;
    Lin_ChannelRuntime[Channel].LinFrameTransmissionState = LIN_FRAME_IDLE;
    Lin_ChannelRuntime[Channel].LinDataIndex = 0U;
    
    /* Clear frame buffer */
    for (uint8 i = 0U; i < LIN_MAX_DATA_LENGTH; i++)
    {
        Lin_ChannelRuntime[Channel].LinFrameBuffer[i] = 0x00U;
    }
    
    /* Re-enable USART interrupts */
    Lin_EnableInterrupts(Channel);
}

/**
 * @brief Internal function to start LIN header transmission
 * 
 * @param[in] Channel LIN channel identifier
 * 
 * @return Std_ReturnType
 */
static Std_ReturnType Lin_StartHeaderTransmission(Lin_ChannelType Channel)
{
    const Lin_ChannelConfigType* ChannelConfig;
    Lin_ChannelRuntimeType* ChannelRuntime;
    USART_TypeDef* usartPtr;
    
    ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    ChannelRuntime = &Lin_ChannelRuntime[Channel];
    usartPtr = ChannelConfig->LinHwChannel;
    
    /* Send LIN Break */
    /* [SWS_Lin_00013] The LIN driver shall send a break field before the sync byte */
    USART_SendBreak(usartPtr);
    
    /* Set state to wait for break completion */
    ChannelRuntime->LinFrameTransmissionState = LIN_FRAME_TX_HEADER_SYNC;
    
    /* Enable TC (Transmission Complete) interrupt to detect break completion */
    USART_ITConfig(usartPtr, USART_IT_TC, ENABLE);
    
    return E_OK;
}

/**
 * @brief Configure USART for LIN communication
 */
static inline void Lin_ConfigureUSART(uint8 Channel, uint32 BaudRate)
{
    USART_InitTypeDef USART_InitStructure;
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Enable USART clock */
    if (ChannelConfig->LinHwChannel == USART1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    }
    else if (ChannelConfig->LinHwChannel == USART2)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    }
    else if (ChannelConfig->LinHwChannel == USART3)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    }
    
    /* Configure USART */
    USART_InitStructure.USART_BaudRate = ChannelConfig->LinChannelBaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(ChannelConfig->LinHwChannel, &USART_InitStructure);
    
    /* Enable LIN mode */
    USART_LINCmd(ChannelConfig->LinHwChannel, ENABLE);
    
    /* Set LIN break detection length */
    USART_LINBreakDetectLengthConfig(ChannelConfig->LinHwChannel, USART_LINBreakDetectLength_11b);
    
    /* Enable USART */
    USART_Cmd(ChannelConfig->LinHwChannel, ENABLE);
}
/**
 * @brief Enable USART interrupts for LIN channel
 */
static inline void Lin_EnableInterrupts(uint8 Channel)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Configure NVIC */
    NVIC_InitStructure.NVIC_IRQChannel = ChannelConfig->IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ChannelConfig->IRQ_Priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable USART interrupts */
    USART_ITConfig(ChannelConfig->LinHwChannel, USART_IT_RXNE, ENABLE);
    USART_ITConfig(ChannelConfig->LinHwChannel, USART_IT_LBD, ENABLE);
}
/**
 * @brief Disable USART interrupts for LIN channel
 */
static inline void Lin_DisableInterrupts(uint8 Channel)
{
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Disable USART interrupts */
    USART_ITConfig(ChannelConfig->LinHwChannel, USART_IT_RXNE, DISABLE);
    USART_ITConfig(ChannelConfig->LinHwChannel, USART_IT_LBD, DISABLE);
    USART_ITConfig(ChannelConfig->LinHwChannel, USART_IT_TXE, DISABLE);
    USART_ITConfig(ChannelConfig->LinHwChannel, USART_IT_TC, DISABLE);
}
static uint8 Lin_CalculatePid(const uint8 Id)
{
    uint8 pid = Id & 0x3FU;  /* Keep only 6 bits for ID */
    uint8 p0, p1;
    
    /* Calculate parity bits */
    p0 = ((pid >> 0) & 1U) ^ ((pid >> 1) & 1U) ^ ((pid >> 2) & 1U) ^ ((pid >> 4) & 1U);
    p1 = ~(((pid >> 1) & 1U) ^ ((pid >> 3) & 1U) ^ ((pid >> 4) & 1U) ^ ((pid >> 5) & 1U)) & 1U;
    
    /* Add parity bits */
    pid |= (p0 << 6U) | (p1 << 7U);
    
    return pid;
}
static uint8 Lin_CalculateChecksum(Lin_FrameCsModelType cs, uint8 pid, const uint8* data, uint8 length)
{
    uint16 checksum = 0x00;  /* Start with PID */
    if(cs == LIN_ENHANCED_CS)
    {
        checksum += pid;  /* Add data length for enhanced checksum */
    }
    /* Calculate checksum */
    for (uint8 i = 0; i < length; i++)
    {
        checksum += data[i];
        if (checksum > 0xFFU)
        {
            checksum -= 0xFFU;  /* Wrap around if overflow */
        }
    }
    
    return ~checksum;  /* Invert the result for LIN checksum */
}

#if (LIN_DEV_ERROR_DETECT == STD_ON)

static inline boolean Lin_ValidateDriverInitialized(void)
{
    /* Check if driver is initialized */
    if (Lin_DriverInitialized == LIN_INIT)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_INIT_SID, LIN_E_STATE_TRANSITION);
        return TRUE;
    }
    return FALSE;
}

static inline boolean Lin_ValidateChannel(Lin_ChannelType Channel)
{
    /* Check if channel ID is within valid range */
    if (Channel >= Lin_ConfigPtr->LinNumberOfChannels)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_INIT_SID, LIN_E_INVALID_CHANNEL);
        return FALSE;
    }
    return TRUE;
}

/**
 * @brief   Validate pointer parameter
 * @details This function validates if the pointer parameter is not NULL
 * 
 * @param[in]   Pointer         Pointer to validate
 * 
 * @return      boolean         TRUE if pointer is valid, FALSE otherwise
 */
static inline boolean Lin_ValidatePointer(Lin_ChannelConfigType* Pointer)
{
    if (Pointer == NULL_PTR)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_INIT_SID, LIN_E_PARAM_POINTER);
        return FALSE;
    }
    return TRUE;
}
#endif
/*==================================================================================================
*                                       INTERRUPT FUNCTIONS
==================================================================================================*/
/**
 * @brief Complete interrupt handler for USART1 (example)
 * This should be called from the actual interrupt vector
 */
void USART1_IRQHandler(void)
{
    /* Find the channel using USART1 */
    for (uint8 channel = 0U; channel < Lin_ConfigPtr->LinNumberOfChannels; channel++)
    {
        if (Lin_ConfigPtr->LinChannel[channel].LinHwChannel == USART1)
        {
            /* Handle both TX and RX interrupts */
            if (USART_GetITStatus(USART1, USART_IT_TC) != RESET ||
                USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
            {
                Lin_TxInterruptHandler(channel);
            }
            
            if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET ||
                USART_GetITStatus(USART1, USART_IT_LBD) != RESET)
            {
                Lin_RxInterruptHandler(channel);
            }
            break;
        }
    }
}

/**
 * @brief Complete interrupt handler for USART2 (example)
 * This should be called from the actual interrupt vector
 */
void USART2_IRQHandler(void)
{
    /* Find the channel using USART2 */
    for (uint8 channel = 0U; channel < Lin_ConfigPtr->LinNumberOfChannels; channel++)
    {
        if (Lin_ConfigPtr->LinChannel[channel].LinHwChannel == USART2)
        {
            /* Handle both TX and RX interrupts */
            if (USART_GetITStatus(USART2, USART_IT_TC) != RESET ||
                USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
            {
                Lin_TxInterruptHandler(channel);
            }
            
            if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET ||
                USART_GetITStatus(USART2, USART_IT_LBD) != RESET)
            {
                Lin_RxInterruptHandler(channel);
            }
            break;
        }
    }
}

/**
 * @brief Complete interrupt handler for USART3 (example)
 * This should be called from the actual interrupt vector
 */
void USART3_IRQHandler(void)
{
    /* Find the channel using USART3 */
    for (uint8 channel = 0U; channel < Lin_ConfigPtr->LinNumberOfChannels; channel++)
    {
        if (Lin_ConfigPtr->LinChannel[channel].LinHwChannel == USART3)
        {
            /* Handle both TX and RX interrupts */
            if (USART_GetITStatus(USART3, USART_IT_TC) != RESET ||
                USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
            {
                Lin_TxInterruptHandler(channel);
            }
            
            if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET ||
                USART_GetITStatus(USART3, USART_IT_LBD) != RESET)
            {
                Lin_RxInterruptHandler(channel);
            }
            break;
        }
    }
}
/**
 * @brief USART interrupt handler for LIN transmission
 * 
 * @param[in] Channel LIN channel identifier
 */
void Lin_TxInterruptHandler(Lin_ChannelType Channel)
{
    const Lin_ChannelConfigType* ChannelConfig;
    Lin_ChannelRuntimeType* ChannelRuntime;
    USART_TypeDef* usartPtr;
    
    ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    ChannelRuntime = &Lin_ChannelRuntime[Channel];
    usartPtr = ChannelConfig->LinHwChannel;
    
    /* Handle transmission based on current state */
    switch (ChannelRuntime->LinFrameTransmissionState)
    {
        case LIN_FRAME_TX_HEADER_SYNC:
            /* Break transmission completed, send sync byte */
            if (USART_GetFlagStatus(usartPtr, USART_FLAG_TC) == SET)
            {
                USART_ClearFlag(usartPtr, USART_FLAG_TC);
                USART_SendData(usartPtr, LIN_SYNC_BYTE);
                ChannelRuntime->LinFrameTransmissionState = LIN_FRAME_TX_HEADER_PID;
            }
            break;
            
        case LIN_FRAME_TX_HEADER_PID:
            /* Sync byte transmission completed, send PID */
            if (USART_GetFlagStatus(usartPtr, USART_FLAG_TC) == SET)
            {
                USART_ClearFlag(usartPtr, USART_FLAG_TC);
                USART_SendData(usartPtr, ChannelRuntime->LinCurrentPdu.Pid);
                
                /* Check if response should be transmitted */
                if (ChannelRuntime->LinCurrentPdu.Drc == LIN_FRAMERESPONSE_TX)
                {
                    ChannelRuntime->LinFrameTransmissionState = LIN_FRAME_TX_RESPONSE_DATA;
                }
                else
                {
                    ChannelRuntime->LinFrameTransmissionState = LIN_FRAME_TX_COMPLETE;
                }
            }
            break;
            
        case LIN_FRAME_TX_RESPONSE_DATA:
            /* PID transmission completed, send data bytes */
            if (USART_GetFlagStatus(usartPtr, USART_FLAG_TC) == SET)
            {
                USART_ClearFlag(usartPtr, USART_FLAG_TC);
                
                if (ChannelRuntime->LinDataIndex < ChannelRuntime->LinCurrentPdu.Dl)
                {
                    /* Send next data byte */
                    USART_SendData(usartPtr, ChannelRuntime->LinTxBuffer[ChannelRuntime->LinDataIndex]);
                    ChannelRuntime->LinDataIndex++;
                }
                else
                {
                    /* All data sent, send checksum */
                    USART_SendData(usartPtr, ChannelRuntime->LinCalculatedChecksum);
                    ChannelRuntime->LinFrameTransmissionState = LIN_FRAME_TX_COMPLETE;
                }
            }
            break;
            
        case LIN_FRAME_TX_COMPLETE:
            /* Checksum transmission completed */
            if (USART_GetFlagStatus(usartPtr, USART_FLAG_TC) == SET)
            {
                USART_ClearFlag(usartPtr, USART_FLAG_TC);
                USART_ITConfig(usartPtr, USART_IT_TC, DISABLE);
                
                /* Frame transmission completed successfully */
                ChannelRuntime->LinFrameStatus = LIN_TX_OK;
                ChannelRuntime->LinFrameTransmissionState = LIN_FRAME_IDLE;
                
                /* Notify upper layer */
                // LinIf_TxConfirmation(Channel);
                
                /* Reset to operational state */
                ChannelRuntime->LinChannelState = LIN_OPERATIONAL;
            }
            break;
            
        default:
            /* Invalid state, reset */
            Lin_AbortTransmission(Channel);
            break;
    }
}

/**
 * @brief USART RX interrupt handler for LIN reception
 * 
 * @param[in] Channel LIN channel identifier
 */
void Lin_RxInterruptHandler(Lin_ChannelType Channel)
{
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    Lin_ChannelRuntimeType* ChannelRuntime = &Lin_ChannelRuntime[Channel];
    uint8 receivedData;
    
    /* Check for LIN Break Detection */
    if (USART_GetITStatus(ChannelConfig->LinHwChannel, USART_IT_LBD) != RESET)
    {
        USART_ClearITPendingBit(ChannelConfig->LinHwChannel, USART_IT_LBD);
        
        /* Break detected - start of new frame */
        ChannelRuntime->LinFrameTransmissionState = LIN_FRAME_RX_DATA;
        ChannelRuntime->LinDataIndex = 0U;
        ChannelRuntime->LinFrameStatus = LIN_RX_BUSY;
        
        /* Clear RX buffer */
        for (uint8 i = 0U; i < LIN_MAX_DATA_LENGTH; i++)
        {
            ChannelRuntime->LinRxBuffer[i] = 0x00U;
        }
        
        /* Wake up if in sleep mode */
        if (ChannelRuntime->LinChannelState == LIN_CH_SLEEP)
        {
            ChannelRuntime->LinWakeupFlag = TRUE;
            ChannelRuntime->LinChannelState = LIN_OPERATIONAL;
            
            /* Notify upper layer about wakeup */
        }
    }
    
    /* Check for received data */
    if (USART_GetITStatus(ChannelConfig->LinHwChannel, USART_IT_RXNE) != RESET)
    {
        receivedData = USART_ReceiveData(ChannelConfig->LinHwChannel);
        
        switch (ChannelRuntime->LinFrameTransmissionState)
        {
            case LIN_FRAME_RX_DATA:
                /* Receiving sync byte */
                if (receivedData == LIN_SYNC_BYTE)
                {
                    /* Valid sync byte received */
                    ChannelRuntime->LinFrameTransmissionState = LIN_FRAME_RX_DATA;
                }
                else
                {
                    /* Next byte should be PID */
                    ChannelRuntime->LinCurrentPid = receivedData;
                    
                    /* For slave nodes, call header indication */
                    if (ChannelConfig->LinNodeType == LIN_SLAVE)
                    {
                        Lin_PduType pduInfo;
                        pduInfo.Pid = receivedData;
                        pduInfo.SduPtr = ChannelRuntime->LinRxBuffer;
                        
                        /* Call LinIf header indication */
                        // LinIf_HeaderIndication(Channel, &pduInfo);
                    }
                }
                break;
                
            case LIN_FRAME_RX_CHECKSUM:
                /* Receiving data bytes */
                if (ChannelRuntime->LinDataIndex < ChannelRuntime->LinCurrentPdu.Dl)
                {
                    ChannelRuntime->LinRxBuffer[ChannelRuntime->LinDataIndex] = receivedData;
                    ChannelRuntime->LinDataIndex++;
                }
                else
                {
                    /* Checksum received */
                    uint8 calculatedChecksum = Lin_CalculateChecksum(
                        ChannelRuntime->LinCurrentPdu.Cs,
                        ChannelRuntime->LinCurrentPid,
                        ChannelRuntime->LinRxBuffer,
                        ChannelRuntime->LinCurrentPdu.Dl
                    );
                    
                    if (calculatedChecksum == receivedData)
                    {
                        /* Frame received successfully */
                        ChannelRuntime->LinFrameStatus = LIN_RX_OK;
                        
                        /* Notify upper layer */
                        // LinIf_RxIndication(Channel, ChannelRuntime->LinRxBuffer);
                    }
                    else
                    {
                        /* Checksum error */
                        ChannelRuntime->LinFrameStatus = LIN_RX_ERROR;
                        
                        /* Report error to upper layer */
                        // LinIf_LinErrorIndication(Channel, LIN_ERR_RESP_CHKSUM);
                    }
                    
                    /* Reset to idle state */
                    ChannelRuntime->LinFrameTransmissionState = LIN_FRAME_IDLE;
                }
                break;
                
            default:
                /* Invalid state, reset */
                ChannelRuntime->LinFrameTransmissionState = LIN_FRAME_IDLE;
                ChannelRuntime->LinFrameStatus = LIN_RX_ERROR;
                break;
        }
    }
    
    /* Check for errors */
    if (USART_GetFlagStatus(ChannelConfig->LinHwChannel, USART_FLAG_FE) == SET)
    {
        USART_ClearFlag(ChannelConfig->LinHwChannel, USART_FLAG_FE);
        ChannelRuntime->LinFrameStatus = LIN_RX_ERROR;
        // LinIf_LinErrorIndication(Channel, LIN_ERR_RESP_STOPBIT);
    }
    
    if (USART_GetFlagStatus(ChannelConfig->LinHwChannel, USART_FLAG_ORE) == SET)
    {
        USART_ClearFlag(ChannelConfig->LinHwChannel, USART_FLAG_ORE);
        ChannelRuntime->LinFrameStatus = LIN_RX_ERROR;
        // LinIf_LinErrorIndication(Channel, LIN_ERR_RESP_STOPBIT);
    }
}
/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/**
 * @brief   Initialize the LIN driver
 */
void Lin_Init(const Lin_ConfigType* Config)
{
#if (LIN_DEV_ERROR_DETECT == STD_ON)
    if (NULL_PTR == Config)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_INIT_SID, LIN_E_PARAM_POINTER);
        return;
    }
    if(Lin_DriverInitialized == TRUE)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_INIT_SID, LIN_E_STATE_TRANSITION);
        return;
    }
#endif
    /* Store configuration pointer */
    Lin_ConfigPtr = Config;
    
    /* Initialize all configured channels */
    for(uint8 Channel = 0U; Channel < Config->LinNumberOfChannels; Channel++)
    {
        Lin_InitChannel(Channel, &Config->LinChannel[Channel]);
    }
    
    /* Set driver as initialized */
    Lin_DriverInitialized = TRUE;
}

#if (LIN_DEINIT_API == STD_ON)
/**
 * @brief Deinitializes the LIN module
 */
void Lin_DeInit(void)
{
#if (LIN_DEV_ERROR_DETECT == STD_ON)
    if (FALSE == Lin_DriverInitialized)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_DEINIT_SID, LIN_E_UNINIT);
        return;
    }
#endif

    /* Deinitialize all channels */
    for (uint8 Channel = 0U; Channel < Lin_ConfigPtr->LinNumberOfChannels; Channel++)
    {
        Lin_DeInitChannel(Channel);
    }
    
    /* Reset configuration pointer */
    Lin_ConfigPtr = NULL_PTR;
    
    /* Set driver as uninitialized */
    Lin_DriverInitialized = FALSE;
}
#endif



Std_ReturnType Lin_SendFrame(uint8 Channel, const Lin_PduType* PduInfoPtr)
{
    const Lin_ChannelConfigType* ChannelConfig;
    Lin_ChannelRuntimeType* ChannelRuntime;
    uint8 i;
    
    ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    ChannelRuntime = &Lin_ChannelRuntime[Channel];

#if (LIN_DEV_ERROR_DETECT == STD_ON)
    if (FALSE == Lin_DriverInitialized)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_SEND_FRAME_SID, LIN_E_UNINIT);
        return E_NOT_OK;
    }
    
    if (Lin_ValidateChannel(Channel) == FALSE)
    {
        return E_NOT_OK;
    }
    
    if (PduInfoPtr == NULL_PTR)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_SEND_FRAME_SID, LIN_E_PARAM_POINTER);
        return E_NOT_OK;
    }

    if ((PduInfoPtr->Drc == LIN_FRAMERESPONSE_TX) && (PduInfoPtr->SduPtr == NULL_PTR))
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_SEND_FRAME_SID, LIN_E_PARAM_POINTER);
        return E_NOT_OK;
    }
#endif
    /* Check if node is configured as master */
    if (ChannelConfig->LinNodeType != LIN_MASTER)
    {
        return E_NOT_OK;
    }
    
    if(ChannelRuntime->LinChannelState != LIN_OPERATIONAL)
    {
        return E_NOT_OK;
    }

    /* [SWS_Lin_00021] Abort current transmission if ongoing */
    if (ChannelRuntime->LinFrameStatus == LIN_TX_BUSY)
    {
        /* Abort current transmission */
        Lin_AbortTransmission(Channel);
    }
    
    ChannelRuntime->LinCurrentPdu = *PduInfoPtr;
    ChannelRuntime->LinFrameStatus = LIN_TX_BUSY;
    ChannelRuntime->LinFrameTransmissionState = LIN_FRAME_TX_HEADER_BREAK;
    ChannelRuntime->LinDataIndex = 0U;

    ChannelRuntime->LinCurrentPid = Lin_CalculatePid(PduInfoPtr->Pid);
    ChannelRuntime->LinCalculatedChecksum = Lin_CalculateChecksum(PduInfoPtr->Cs, PduInfoPtr->Pid, PduInfoPtr->SduPtr, PduInfoPtr->Dl);

    /* Copy data to internal buffer */
    for (i = 0U; i < PduInfoPtr->Dl; i++)
    {
        ChannelRuntime->LinTxBuffer[i] = PduInfoPtr->SduPtr[i];
    }
    
    /* Start transmission by sending break */
    return Lin_StartHeaderTransmission(Channel);
}


/**
 * @brief Get status of the LIN channel
 * 
 * @param[in] Channel LIN channel identifier
 * @param[out] LinSduPtr Pointer to receive the data
 * 
 * @return Lin_StatusType
 */
Lin_StatusType Lin_GetStatus(uint8 Channel,const uint8** LinSduPtr)
{
    Lin_StatusType retVal = LIN_NOT_OK;
    
#if (LIN_DEV_ERROR_DETECT == STD_ON)
    if (FALSE == Lin_DriverInitialized)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_GET_STATUS_SID, LIN_E_UNINIT);
        return LIN_NOT_OK;
    }
    
    if (Lin_ValidateChannel(Channel) == FALSE)
    {
        return LIN_NOT_OK;
    }
    
    if (LinSduPtr == NULL_PTR)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_GET_STATUS_SID, LIN_E_PARAM_POINTER);
        return LIN_NOT_OK;
    }
#endif

    retVal = Lin_ChannelRuntime[Channel].LinChannelState;
    
    /* If data is available, provide pointer to received data */
    if ((retVal == LIN_RX_OK) && (LinSduPtr != NULL_PTR))
    {
        *LinSduPtr = Lin_ChannelRuntime[Channel].LinRxBuffer;
    }
    
    return retVal;
}

/**
 * @brief Go to sleep command
 * 
 * @param[in] Channel LIN channel identifier
 * 
 * @return Std_ReturnType
 */
Std_ReturnType Lin_GoToSleep(uint8 Channel)
{
#if (LIN_DEV_ERROR_DETECT == STD_ON)
    if (FALSE == Lin_DriverInitialized)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_GO_TO_SLEEP_SID, LIN_E_UNINIT);
        return E_NOT_OK;
    }
    
    if (Lin_ValidateChannel(Channel) == FALSE)
    {
        return E_NOT_OK;
    }
#endif

    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];

    /* [SWS_Lin_00089] The function Lin_GoToSleep shall send a go-to-sleep-command
       on the addressed LIN channel as defined in LIN Specification 2.1. */
    /* Send sleep command frame */
    Lin_PduType sleepPdu;
    /* Based on LIN Specification 2.1 */
    uint8 sleepData[] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    sleepPdu.Pid = 0x3C;  /* Sleep command PID */
    sleepPdu.Cs = LIN_CLASSIC_CS;
    sleepPdu.Drc = LIN_FRAMERESPONSE_TX;
    sleepPdu.Dl = 8U;
    sleepPdu.SduPtr = sleepData;
    
    /* Send sleep frame */
    Std_ReturnType retVal = Lin_SendFrame(Channel, &sleepPdu);
    
    if (retVal == E_OK)
    {
        /* Set channel to sleep state */
        Lin_ChannelRuntime[Channel].LinChannelState = LIN_CH_SLEEP;
    }
    
    return retVal;
}

/**
 * @brief Go to sleep internal command (for slave nodes)
 * 
 * @param[in] Channel LIN channel identifier
 * 
 * @return Std_ReturnType
 */
Std_ReturnType Lin_GoToSleepInternal(uint8 Channel)
{
#if (LIN_DEV_ERROR_DETECT == STD_ON)
    if (FALSE == Lin_DriverInitialized)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_GO_TO_SLEEP_INTERNAL_SID, LIN_E_UNINIT);
        return E_NOT_OK;
    }
    
    if (Lin_ValidateChannel(Channel) == FALSE)
    {
        return E_NOT_OK;
    }
#endif

    /* Set channel to sleep state without sending frame */
    Lin_ChannelRuntime[Channel].LinChannelState = LIN_CH_SLEEP;
    
    /* Disable transmission interrupts but keep RX for wakeup detection */
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    USART_ITConfig(ChannelConfig->LinHwChannel, USART_IT_TC, DISABLE);
    USART_ITConfig(ChannelConfig->LinHwChannel, USART_IT_TXE, DISABLE);
    
    return E_OK;
}

/**
 * @brief Wake up the LIN channel
 * 
 * @param[in] Channel LIN channel identifier
 * 
 * @return Std_ReturnType
 */
Std_ReturnType Lin_Wakeup(uint8 Channel)
{
#if (LIN_DEV_ERROR_DETECT == STD_ON)
    if (FALSE == Lin_DriverInitialized)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_WAKEUP_SID, LIN_E_UNINIT);
        return E_NOT_OK;
    }
    
    if (Lin_ValidateChannel(Channel) == FALSE)
    {
        return E_NOT_OK;
    }
#endif

    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Send wakeup pulse */
    USART_SendData(ChannelConfig->LinHwChannel, 0x80);  /* Wakeup dominant signal */
    
    /* Wait for transmission complete */
    while (USART_GetFlagStatus(ChannelConfig->LinHwChannel, USART_FLAG_TC) == RESET);
    
    /* Set channel to operational state */
    Lin_ChannelRuntime[Channel].LinChannelState = LIN_OPERATIONAL;
    Lin_ChannelRuntime[Channel].LinFrameStatus = LIN_OPERATIONAL;
    
    /* Re-enable all interrupts */
    Lin_EnableInterrupts(Channel);
    
    return E_OK;
}


/**
 * @brief Check wakeup
 * 
 * @param[in] Channel LIN channel identifier
 * 
 * @return Std_ReturnType
 */
Std_ReturnType Lin_CheckWakeup(uint8 Channel)
{
#if (LIN_DEV_ERROR_DETECT == STD_ON)
    if (FALSE == Lin_DriverInitialized)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_CHECK_WAKEUP_SID, LIN_E_UNINIT);
        return E_NOT_OK;
    }
    
    if (Lin_ValidateChannel(Channel) == FALSE)
    {
        return E_NOT_OK;
    }
#endif

    Std_ReturnType retVal = E_NOT_OK;
    
    if (Lin_ChannelRuntime[Channel].LinWakeupFlag == TRUE)
    {
        Lin_ChannelRuntime[Channel].LinWakeupFlag = FALSE;
        retVal = E_OK;
    }
    
    return retVal;
}



#if (LIN_VERSION_INFO_API == STD_ON)
/**
 * @brief Returns the version information of this module
 * 
 * @param[out] versioninfo Pointer to store version information
 */
void Lin_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
#if (LIN_DEV_ERROR_DETECT == STD_ON)
    if (versioninfo == NULL_PTR)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_GET_VERSION_INFO_SID, LIN_E_PARAM_POINTER);
        return;
    }
#endif

    versioninfo->vendorID = LIN_VENDOR_ID;
    versioninfo->moduleID = LIN_MODULE_ID;
    versioninfo->sw_major_version = LIN_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = LIN_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = LIN_SW_PATCH_VERSION;
}
#endif



/*==================================================================================================
*                                       END OF FILE
==================================================================================================*/
