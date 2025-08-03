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
 * @brief   LIN Channel Runtime Information
 * @details Structure to hold runtime information for each LIN channel
 */
typedef struct
{
    Lin_StatusType LinChannelState;             /**< Channel State */
    Lin_StatusType LinFrameStatus;              /**< Frame Status */
    uint8 LinCurrentPid;                        /**< Current PID */
    uint8 LinFrameBuffer[LIN_MAX_DATA_LENGTH];  /**< Frame Buffer */
    uint8 LinFrameIndex;                        /**< Frame Index */
    uint8 LinFrameLength;                       /**< Frame Length */
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
static Lin_ChannelRuntimeType Lin_ChannelRuntime[LIN_MAX_CHANNELS];

/**
 * @brief LIN Driver State
 */
static boolean Lin_DriverInitialized = FALSE;

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static void Lin_InitChannel(uint8 Channel, const Lin_ChannelConfigType* ChannelConfigPtr);
static inline void Lin_DeInitChannel(uint8 Channel);

static inline void Lin_ConfigureUSART(uint8 Channel, uint32 BaudRate);
static inline void Lin_EnableInterrupts(uint8 Channel);
static inline void Lin_DisableInterrupts(uint8 Channel);

static uint8 Lin_CalculatePid(uint8 Id);
static uint8 Lin_CalculateChecksum(Lin_FrameCsModelType cs, uint8 pid, const uint8* data, uint8 length)

static void Lin_SendBreakField(uint8 Channel);
static void Lin_SendSyncField(uint8 Channel);
static void Lin_SendPidField(uint8 Channel, uint8 Pid);
static void Lin_SendDataField(uint8 Channel, const uint8* DataPtr, uint8 Length);
static void Lin_SendChecksumField(uint8 Channel, uint8 Checksum);

#if (LIN_DEV_ERROR_DETECT == STD_ON)
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
static void Lin_InitChannel(uint8 Channel, const Lin_ChannelConfigType* ChannelConfigPtr)
{   
    /* Configure USART */
    Lin_ConfigureUSART(Channel, ChannelConfigPtr->LinChannelBaudRate);
    
    /* Initialize channel runtime information */
    Lin_ChannelRuntime[Channel].LinChannelState = LIN_OPERATIONAL;
    Lin_ChannelRuntime[Channel].LinFrameStatus = LIN_NOT_OK;
    Lin_ChannelRuntime[Channel].LinCurrentPid = 0x00U;
    Lin_ChannelRuntime[Channel].LinFrameIndex = 0U;
    Lin_ChannelRuntime[Channel].LinFrameLength = 0U;
    Lin_ChannelRuntime[Channel].LinWakeupFlag = FALSE;
    Lin_ChannelRuntime[Channel].LinTimeoutCounter = 0U;

    /* Clear frame buffer */
    for (uint8 i = 0U; i < LIN_MAX_DATA_LENGTH; i++)
    {
        Lin_ChannelInfo[Channel].LinFrameBuffer[i] = 0x00U;
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
    USART_SendData(ChannelConfig->LinHwChannel, LIN_SYNC_FIELD_VALUE);
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
 * @brief Configure USART for LIN communication
 */
static inline void Lin_ConfigureUSART(uint8 Channel, uint32 BaudRate)
{
    USART_InitTypeDef USART_InitStructure;
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Enable USART clock */
    if (ChannelConfig->USARTx == USART1)
    {
        RCC_APB2PeriphClockCmd(ChannelConfig->USART_RCC, ENABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(ChannelConfig->USART_RCC, ENABLE);
    }
    
    /* Configure USART */
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(ChannelConfig->USARTx, &USART_InitStructure);
    
    /* Enable LIN mode */
    USART_LINCmd(ChannelConfig->USARTx, ENABLE);
    
    /* Set LIN break detection length */
    USART_LINBreakDetectLengthConfig(ChannelConfig->USARTx, USART_LINBreakDetectLength_11b);
    
    /* Enable USART */
    USART_Cmd(ChannelConfig->USARTx, ENABLE);
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
    USART_ITConfig(ChannelConfig->USARTx, USART_IT_RXNE, ENABLE);
    USART_ITConfig(ChannelConfig->USARTx, USART_IT_LBD, ENABLE);
}
/**
 * @brief Disable USART interrupts for LIN channel
 */
static inline void Lin_DisableInterrupts(uint8 Channel)
{
    const Lin_ChannelConfigType* ChannelConfig = &Lin_ConfigPtr->LinChannel[Channel];
    
    /* Disable USART interrupts */
    USART_ITConfig(ChannelConfig->USARTx, USART_IT_RXNE, DISABLE);
    USART_ITConfig(ChannelConfig->USARTx, USART_IT_LBD, DISABLE);
    USART_ITConfig(ChannelConfig->USARTx, USART_IT_TXE, DISABLE);
    USART_ITConfig(ChannelConfig->USARTx, USART_IT_TC, DISABLE);
}
static uint8 Lin_CalculatePid(uint8 Id)
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

#if (ADC_DEV_ERROR_DETECT == STD_ON)

static inline boolean Lin_ValidateChannel(Lin_ChannelType Channel)
{
    /* Check if channel ID is within valid range */
    if (Channel >= Lin_ConfigPtr->LinNumberOfChannels)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_INIT_SID, LIN_E_PARAM_CHANNEL);
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
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/**
 * @brief   Initialize the ADC driver
 */
void Lin_Init (const Lin_ConfigType* Config);
{
    uint8 group_idx;
    uint8 channel_idx;

#if (LIN_DEV_ERROR_DETECT == STD_ON)
    if (NULL_PTR == Config)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_INIT_SID, LIN_E_PARAM_POINTER);
        return;
    }
    if(TRUE == Lin_DriverInitialized)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_INIT_SID, LIN_E_ALREADY_INITIALIZED);
        return;
    }
    else
#endif
    {
        /* Store configuration pointer */
        Lin_ConfigPtr = Config;
        
        /* Initialize all configured channels */
        for (uint8 Channel = 0U; Channel < Config->LinNumberOfChannels; Channel++)
        {
            Lin_InitChannel(Channel, &Config->LinChannel[Channel]);
        }
        
        /* Set driver as initialized */
        Lin_DriverInitialized = TRUE;
    }
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

#if (ADC_ENABLE_START_STOP_GROUP_API == STD_ON)
/**
 * @brief   Start group conversion
 */
FUNC(void, ADC_CODE) Adc_StartGroupConversion(
    VAR(Adc_GroupType, AUTOMATIC) Group
)
{
#if (ADC_DEV_ERROR_DETECT == STD_ON)
    /* Check if driver is initialized */
    if (ADC_UNINIT == Adc_Status)
    {
        Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, ADC_START_GROUP_CONVERSION_SID, ADC_E_UNINIT);
    }
    /* Check if group is valid */
    else if (FALSE == Adc_ValidateGroup(Group))
    {
        Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, ADC_START_GROUP_CONVERSION_SID, ADC_E_PARAM_GROUP);
    }
    /* Check if group is idle */
    else if (Adc_GroupRuntime[Group].Status != ADC_IDLE)
    {
        Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, ADC_START_GROUP_CONVERSION_SID, ADC_E_BUSY);
    }
    /* Check if result buffer is initialized */
    else if (FALSE == Adc_GroupRuntime[Group].ResultBufferInitialized)
    {
        Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, ADC_START_GROUP_CONVERSION_SID, ADC_E_BUFFER_UNINIT);
    }
    else
#endif
    {
        /* Set group status to busy */
        Adc_GroupRuntime[Group].Status = ADC_BUSY;
        Adc_GroupRuntime[Group].CurrentSample = 0U;

        /* Configure group for conversion */
        Adc_ConfigureGroup(&(Adc_ConfigPtr->GroupConfigPtr[Group]), Group);

        /* Start hardware conversion */
        Adc_StartHwConversion(Group);
    }
}

/**
 * @brief   Stop group conversion
 */
FUNC(void, ADC_CODE) Adc_StopGroupConversion(
    VAR(Adc_GroupType, AUTOMATIC) Group
)
{
#if (ADC_DEV_ERROR_DETECT == STD_ON)
    /* Check if driver is initialized */
    if (ADC_UNINIT == Adc_Status)
    {
        Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, ADC_STOP_GROUP_CONVERSION_SID, ADC_E_UNINIT);
    }
    /* Check if group is valid */
    else if (FALSE == Adc_ValidateGroup(Group))
    {
        Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, ADC_STOP_GROUP_CONVERSION_SID, ADC_E_PARAM_GROUP);
    }
    /* Check if group is busy */
    else if (Adc_GroupRuntime[Group].Status == ADC_IDLE)
    {
        Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, ADC_STOP_GROUP_CONVERSION_SID, ADC_E_IDLE);
    }
    else
#endif
    {
        /* Stop hardware conversion */
        Adc_StopHwConversion(Group);

        /* Set group status to idle */
        Adc_GroupRuntime[Group].Status = ADC_IDLE;
        Adc_GroupRuntime[Group].CurrentSample = 0U;
    }
}
#endif


#if (lin_VERSION_INFO_API == STD_ON)
/**
 * @brief   Get version information
 */
void Lin_GetVersionInfo(P2VAR(Std_VersionInfoType, AUTOMATIC, LIN_APPL_DATA) VersionInfo)
{
#if (LIN_DEV_ERROR_DETECT == STD_ON)
    /* Check if VersionInfo pointer is NULL */
    if (NULL_PTR == VersionInfo)
    {
        Det_ReportError(LIN_MODULE_ID, LIN_INSTANCE_ID, LIN_GET_VERSION_INFO_SID, LIN_E_PARAM_POINTER);
    }
    else
#endif
    {
        /* Fill version information */
        VersionInfo->vendorID = ADC_VENDOR_ID;
        VersionInfo->moduleID = ADC_MODULE_ID;
        VersionInfo->sw_major_version = ADC_SW_MAJOR_VERSION;
        VersionInfo->sw_minor_version = ADC_SW_MINOR_VERSION;
        VersionInfo->sw_patch_version = ADC_SW_PATCH_VERSION;
    }
}
#endif

/**
 * @brief   Setup result buffer
 */
FUNC(Std_ReturnType, ADC_CODE) Adc_SetupResultBuffer(
    VAR(Adc_GroupType, AUTOMATIC) Group,
    P2VAR(Adc_ValueType, AUTOMATIC, ADC_APPL_DATA) DataBufferPtr
)
{
    Std_ReturnType result = E_NOT_OK;

#if (ADC_DEV_ERROR_DETECT == STD_ON)
    /* Check if driver is initialized */
    if (ADC_UNINIT == Adc_Status)
    {
        Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, ADC_SETUP_RESULT_BUFFER_SID, ADC_E_UNINIT);
    }
    /* Check if group is valid */
    else if (FALSE == Adc_ValidateGroup(Group))
    {
        Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, ADC_SETUP_RESULT_BUFFER_SID, ADC_E_PARAM_GROUP);
    }
    /* Check if DataBufferPtr is valid */
    else if (FALSE == Adc_ValidatePointer(DataBufferPtr))
    {
        Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, ADC_SETUP_RESULT_BUFFER_SID, ADC_E_PARAM_POINTER);
    }
    /* Check if group is idle */
    else if (Adc_GroupRuntime[Group].Status != ADC_IDLE)
    {
        Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, ADC_SETUP_RESULT_BUFFER_SID, ADC_E_BUSY);
    }
    else
#endif
    {
        /* Setup result buffer */
        Adc_GroupRuntime[Group].ResultBufferPtr = DataBufferPtr;
        Adc_GroupRuntime[Group].ResultBufferInitialized = TRUE;
        result = E_OK;
    }

    return result;
}

/*==================================================================================================
*                                       END OF FILE
==================================================================================================*/
