/****************************************************************************************
*                               CAN_CFG.C                                              *
****************************************************************************************
* File Name   : Can_Cfg.c
* Module      : Controller Area Network (CAN)
* Description : AUTOSAR CAN driver configuration source file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 24/08/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "Can_Cfg.h"
#include "Can_Types.h"
#include "Can.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CAN_CFG_VENDOR_ID_C                     (0x0001U)
#define CAN_CFG_MODULE_ID_C                     (0x007BU)
#define CAN_CFG_AR_RELEASE_MAJOR_VERSION_C      (4U)
#define CAN_CFG_AR_RELEASE_MINOR_VERSION_C      (4U)
#define CAN_CFG_AR_RELEASE_REVISION_VERSION_C   (0U)
#define CAN_CFG_SW_MAJOR_VERSION_C              (1U)
#define CAN_CFG_SW_MINOR_VERSION_C              (0U)
#define CAN_CFG_SW_PATCH_VERSION_C              (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if Can_Cfg.c file and Can_Cfg.h file are of the same vendor */
#if (CAN_CFG_VENDOR_ID_C != CAN_CFG_VENDOR_ID)
    #error "Can_Cfg.c and Can_Cfg.h have different vendor ids"
#endif

/* Check AUTOSAR version compatibility */
#if ((CAN_CFG_AR_RELEASE_MAJOR_VERSION_C != CAN_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (CAN_CFG_AR_RELEASE_MINOR_VERSION_C != CAN_CFG_AR_RELEASE_MINOR_VERSION) || \
     (CAN_CFG_AR_RELEASE_REVISION_VERSION_C != CAN_CFG_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR Version Numbers of Can_Cfg.c and Can_Cfg.h are different"
#endif

/* Check software version compatibility */
#if ((CAN_CFG_SW_MAJOR_VERSION_C != CAN_CFG_SW_MAJOR_VERSION) || \
     (CAN_CFG_SW_MINOR_VERSION_C != CAN_CFG_SW_MINOR_VERSION) || \
     (CAN_CFG_SW_PATCH_VERSION_C != CAN_CFG_SW_PATCH_VERSION))
    #error "Software Version Numbers of Can_Cfg.c and Can_Cfg.h are different"
#endif

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/**
 * @brief   CAN Baudrate Configuration Structure
 * @details Contains timing parameters for different CAN baudrates
 */
typedef struct
{
    uint16 BaudRateConfigID;        /**< Baudrate configuration identifier */
    uint32 BaudRate;                /**< Baudrate in bps */
    uint32 Prescaler;               /**< Baud rate prescaler */
    uint32 PropSeg;                 /**< Propagation time segment */
    uint32 PhaseSeg1;               /**< Phase buffer segment 1 */
    uint32 PhaseSeg2;               /**< Phase buffer segment 2 */
    uint32 SJW;                     /**< Synchronization Jump Width */
    uint8  SamplePoint;             /**< Sample point in % */
} Can_BaudrateConfigType;

/**
 * @brief   CAN Controller Configuration Structure
 * @details Contains configuration parameters for a CAN controller
 */
typedef struct
{
    uint8 ControllerId;                         /**< Controller identifier */
    boolean ControllerActivation;               /**< Controller activation flag */
    uint32 ControllerBaseAddress;               /**< Controller base address */
    uint8 ControllerIrqNumber;                  /**< Controller IRQ number */
    uint8 ControllerIrqPriority;                /**< Controller IRQ priority */
    uint16 DefaultBaudrateId;                   /**< Default baudrate configuration ID */
    const Can_BaudrateConfigType* BaudrateConfigs;  /**< Pointer to baudrate configurations */
    uint8 BaudrateConfigsCount;                 /**< Number of baudrate configurations */
    boolean WakeupSupport;                      /**< Wakeup support flag */
    boolean BusoffRecovery;                     /**< Automatic busoff recovery flag */
} Can_ControllerConfigType;

/**
 * @brief   CAN Hardware Object Configuration Structure
 * @details Contains configuration for a hardware transmit/receive object
 */
typedef struct
{
    Can_HwHandleType HwObjectId;                /**< Hardware object identifier */
    uint8 ControllerId;                         /**< Associated controller ID */
    boolean IsTransmit;                         /**< TRUE for HTH, FALSE for HRH */
    Can_IdType CanIdValue;                      /**< CAN ID value */
    Can_IdType CanIdMask;                       /**< CAN ID mask for filtering */
    boolean CanIdExtended;                      /**< Extended ID flag */
    uint8 CanObjectType;                        /**< Object type (BASIC/FULL) */
    uint8 MaxDataLength;                        /**< Maximum data length */
} Can_HwObjectConfigType;

/**
 * @brief   CAN Filter Configuration Structure
 * @details Contains configuration for message acceptance filters
 */
typedef struct
{
    uint8 FilterNumber;                         /**< Filter bank number */
    uint8 FilterMode;                           /**< Filter mode (ID/MASK or ID/LIST) */
    uint8 FilterScale;                          /**< Filter scale (16-bit or 32-bit) */
    uint32 FilterIdHigh;                        /**< Filter ID high */
    uint32 FilterIdLow;                         /**< Filter ID low */
    uint32 FilterMaskIdHigh;                    /**< Filter mask ID high */
    uint32 FilterMaskIdLow;                     /**< Filter mask ID low */
    uint8 FilterFIFOAssignment;                 /**< FIFO assignment (0 or 1) */
    boolean FilterActivation;                   /**< Filter activation flag */
} Can_FilterConfigType;

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/* CAN Timing Parameters for 36MHz CAN Clock */
#define CAN_36MHZ_125KBPS_PRESCALER         (18U)
#define CAN_36MHZ_125KBPS_PROP_SEG          (1U)
#define CAN_36MHZ_125KBPS_PHASE_SEG1        (13U)
#define CAN_36MHZ_125KBPS_PHASE_SEG2        (2U)
#define CAN_36MHZ_125KBPS_SJW               (1U)

#define CAN_36MHZ_250KBPS_PRESCALER         (9U)
#define CAN_36MHZ_250KBPS_PROP_SEG          (1U)
#define CAN_36MHZ_250KBPS_PHASE_SEG1        (13U)
#define CAN_36MHZ_250KBPS_PHASE_SEG2        (2U)
#define CAN_36MHZ_250KBPS_SJW               (1U)

#define CAN_36MHZ_500KBPS_PRESCALER         (4U)
#define CAN_36MHZ_500KBPS_PROP_SEG          (1U)
#define CAN_36MHZ_500KBPS_PHASE_SEG1        (14U)
#define CAN_36MHZ_500KBPS_PHASE_SEG2        (3U)
#define CAN_36MHZ_500KBPS_SJW               (1U)

#define CAN_36MHZ_1MBPS_PRESCALER           (2U)
#define CAN_36MHZ_1MBPS_PROP_SEG            (1U)
#define CAN_36MHZ_1MBPS_PHASE_SEG1          (14U)
#define CAN_36MHZ_1MBPS_PHASE_SEG2          (3U)
#define CAN_36MHZ_1MBPS_SJW                 (1U)

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/**
 * @brief   CAN Baudrate Configurations for Controller 0
 * @details Pre-calculated timing parameters for different baudrates
 */
static const Can_BaudrateConfigType Can_Controller0_BaudrateConfigs[] =
{
    /* 125 kbps Configuration */
    {
        .BaudRateConfigID = CAN_BAUDRATE_125KBPS_ID,
        .BaudRate = 125000UL,
        .Prescaler = CAN_36MHZ_125KBPS_PRESCALER,
        .PropSeg = CAN_36MHZ_125KBPS_PROP_SEG,
        .PhaseSeg1 = CAN_36MHZ_125KBPS_PHASE_SEG1,
        .PhaseSeg2 = CAN_36MHZ_125KBPS_PHASE_SEG2,
        .SJW = CAN_36MHZ_125KBPS_SJW,
        .SamplePoint = 87U  /* Sample point at 87% */
    },
    /* 250 kbps Configuration */
    {
        .BaudRateConfigID = CAN_BAUDRATE_250KBPS_ID,
        .BaudRate = 250000UL,
        .Prescaler = CAN_36MHZ_250KBPS_PRESCALER,
        .PropSeg = CAN_36MHZ_250KBPS_PROP_SEG,
        .PhaseSeg1 = CAN_36MHZ_250KBPS_PHASE_SEG1,
        .PhaseSeg2 = CAN_36MHZ_250KBPS_PHASE_SEG2,
        .SJW = CAN_36MHZ_250KBPS_SJW,
        .SamplePoint = 87U  /* Sample point at 87% */
    },
    /* 500 kbps Configuration */
    {
        .BaudRateConfigID = CAN_BAUDRATE_500KBPS_ID,
        .BaudRate = 500000UL,
        .Prescaler = CAN_36MHZ_500KBPS_PRESCALER,
        .PropSeg = CAN_36MHZ_500KBPS_PROP_SEG,
        .PhaseSeg1 = CAN_36MHZ_500KBPS_PHASE_SEG1,
        .PhaseSeg2 = CAN_36MHZ_500KBPS_PHASE_SEG2,
        .SJW = CAN_36MHZ_500KBPS_SJW,
        .SamplePoint = 83U  /* Sample point at 83% */
    },
    /* 1 Mbps Configuration */
    {
        .BaudRateConfigID = CAN_BAUDRATE_1MBPS_ID,
        .BaudRate = 1000000UL,
        .Prescaler = CAN_36MHZ_1MBPS_PRESCALER,
        .PropSeg = CAN_36MHZ_1MBPS_PROP_SEG,
        .PhaseSeg1 = CAN_36MHZ_1MBPS_PHASE_SEG1,
        .PhaseSeg2 = CAN_36MHZ_1MBPS_PHASE_SEG2,
        .SJW = CAN_36MHZ_1MBPS_SJW,
        .SamplePoint = 83U  /* Sample point at 83% */
    }
};

/**
 * @brief   CAN Hardware Object Configurations
 * @details Configuration for all hardware transmit and receive objects
 */
static const Can_HwObjectConfigType Can_HwObjectConfigs[] =
{
    /* Transmit Hardware Objects (HTH) */
    {
        .HwObjectId = CAN_HTH_0,
        .ControllerId = CAN_CONTROLLER_0_ID,
        .IsTransmit = TRUE,
        .CanIdValue = 0x000U,                    /* Don't care for TX */
        .CanIdMask = 0x000U,                     /* Don't care for TX */
        .CanIdExtended = FALSE,                  /* Standard ID by default */
        .CanObjectType = 0U,                     /* BASIC CAN object */
        .MaxDataLength = 8U                      /* Maximum 8 bytes */
    },
    {
        .HwObjectId = CAN_HTH_1,
        .ControllerId = CAN_CONTROLLER_0_ID,
        .IsTransmit = TRUE,
        .CanIdValue = 0x000U,                    /* Don't care for TX */
        .CanIdMask = 0x000U,                     /* Don't care for TX */
        .CanIdExtended = FALSE,                  /* Standard ID by default */
        .CanObjectType = 0U,                     /* BASIC CAN object */
        .MaxDataLength = 8U                      /* Maximum 8 bytes */
    },
    {
        .HwObjectId = CAN_HTH_2,
        .ControllerId = CAN_CONTROLLER_0_ID,
        .IsTransmit = TRUE,
        .CanIdValue = 0x000U,                    /* Don't care for TX */
        .CanIdMask = 0x000U,                     /* Don't care for TX */
        .CanIdExtended = FALSE,                  /* Standard ID by default */
        .CanObjectType = 0U,                     /* BASIC CAN object */
        .MaxDataLength = 8U                      /* Maximum 8 bytes */
    },
    /* Receive Hardware Objects (HRH) */
    {
        .HwObjectId = CAN_HRH_0,
        .ControllerId = CAN_CONTROLLER_0_ID,
        .IsTransmit = FALSE,
        .CanIdValue = 0x000U,                    /* Accept all messages */
        .CanIdMask = 0x000U,                     /* Accept all messages */
        .CanIdExtended = FALSE,                  /* Standard ID */
        .CanObjectType = 0U,                     /* BASIC CAN object */
        .MaxDataLength = 8U                      /* Maximum 8 bytes */
    },
    {
        .HwObjectId = CAN_HRH_1,
        .ControllerId = CAN_CONTROLLER_0_ID,
        .IsTransmit = FALSE,
        .CanIdValue = 0x000U,                    /* Accept all messages */
        .CanIdMask = 0x000U,                     /* Accept all messages */
        .CanIdExtended = TRUE,                   /* Extended ID */
        .CanObjectType = 0U,                     /* BASIC CAN object */
        .MaxDataLength = 8U                      /* Maximum 8 bytes */
    }
};

/**
 * @brief   CAN Filter Configurations
 * @details Configuration for message acceptance filters
 */
static const Can_FilterConfigType Can_FilterConfigs[] =
{
    /* Filter 0 - Accept all Standard ID messages to FIFO 0 */
    {
        .FilterNumber = 0U,
        .FilterMode = 0U,                        /* ID/Mask mode */
        .FilterScale = 1U,                       /* 32-bit scale */
        .FilterIdHigh = 0x0000U,                 /* ID = 0x000 */
        .FilterIdLow = 0x0000U,
        .FilterMaskIdHigh = 0x0000U,             /* Mask = 0x000 (accept all) */
        .FilterMaskIdLow = 0x0000U,
        .FilterFIFOAssignment = 0U,              /* Assign to FIFO 0 */
        .FilterActivation = TRUE
    },
    /* Filter 1 - Accept all Extended ID messages to FIFO 1 */
    {
        .FilterNumber = 1U,
        .FilterMode = 0U,                        /* ID/Mask mode */
        .FilterScale = 1U,                       /* 32-bit scale */
        .FilterIdHigh = 0x0000U,                 /* Extended ID format */
        .FilterIdLow = 0x0004U,                  /* IDE bit set */
        .FilterMaskIdHigh = 0x0000U,             /* Mask = 0x000 (accept all) */
        .FilterMaskIdLow = 0x0004U,              /* Only check IDE bit */
        .FilterFIFOAssignment = 1U,              /* Assign to FIFO 1 */
        .FilterActivation = TRUE
    }
};

/**
 * @brief   CAN Controller 0 Configuration
 * @details Complete configuration for CAN Controller 0
 */
static const Can_ControllerConfigType Can_Controller0_Config =
{
    .ControllerId = CAN_CONTROLLER_0_ID,
    .ControllerActivation = CAN_CONTROLLER_0_ENABLED,
    .ControllerBaseAddress = CAN_CONTROLLER_0_BASE_ADDRESS,
    .ControllerIrqNumber = CAN_CONTROLLER_0_IRQ_NUMBER,
    .ControllerIrqPriority = CAN_CONTROLLER_0_IRQ_PRIORITY,
    .DefaultBaudrateId = CAN_DEFAULT_BAUDRATE_ID,
    .BaudrateConfigs = Can_Controller0_BaudrateConfigs,
    .BaudrateConfigsCount = (sizeof(Can_Controller0_BaudrateConfigs) / sizeof(Can_BaudrateConfigType)),
    .WakeupSupport = (CAN_WAKEUP_SUPPORT == STD_ON) ? TRUE : FALSE,
    .BusoffRecovery = TRUE                       /* Enable automatic busoff recovery */
};

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/**
 * @brief   CAN Controller Configurations Array
 * @details Array containing configuration for all CAN controllers
 */
const Can_ConfigType Can_ControllerConfigs[CAN_MAX_CONTROLLERS] =
{
    /* Controller 0 Configuration */
    {
        .ConfigSet = 0U                          /* Configuration set identifier */
        /* Additional controller-specific configuration members can be added here */
    }
};

/**
 * @brief   Main CAN Configuration Structure
 * @details Main configuration structure containing all CAN driver settings
 */
const Can_ConfigType Can_Config =
{
    .ConfigSet = 0U                              /* Default configuration set */
    /* Additional global configuration members can be added here */
};

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/**
 * @brief   Get CAN Controller Configuration
 * @details Returns pointer to configuration structure for specified controller
 * @param[in] ControllerId - Controller identifier
 * @return  Pointer to controller configuration or NULL_PTR if invalid
 */
const Can_ControllerConfigType* Can_GetControllerConfig(uint8 ControllerId)
{
    const Can_ControllerConfigType* ConfigPtr = NULL_PTR;
    
    switch (ControllerId)
    {
        case CAN_CONTROLLER_0_ID:
            ConfigPtr = &Can_Controller0_Config;
            break;
        default:
            /* Invalid controller ID */
            break;
    }
    
    return ConfigPtr;
}

/**
 * @brief   Get CAN Baudrate Configuration
 * @details Returns pointer to baudrate configuration for specified controller and baudrate ID
 * @param[in] ControllerId - Controller identifier
 * @param[in] BaudrateConfigId - Baudrate configuration identifier
 * @return  Pointer to baudrate configuration or NULL_PTR if invalid
 */
const Can_BaudrateConfigType* Can_GetBaudrateConfig(uint8 ControllerId, uint16 BaudrateConfigId)
{
    const Can_BaudrateConfigType* ConfigPtr = NULL_PTR;
    const Can_ControllerConfigType* ControllerConfigPtr;
    uint8 Index;
    
    ControllerConfigPtr = Can_GetControllerConfig(ControllerId);
    
    if (ControllerConfigPtr != NULL_PTR)
    {
        for (Index = 0U; Index < ControllerConfigPtr->BaudrateConfigsCount; Index++)
        {
            if (ControllerConfigPtr->BaudrateConfigs[Index].BaudRateConfigID == BaudrateConfigId)
            {
                ConfigPtr = &ControllerConfigPtr->BaudrateConfigs[Index];
                break;
            }
        }
    }
    
    return ConfigPtr;
}

/**
 * @brief   Get CAN Hardware Object Configuration
 * @details Returns pointer to hardware object configuration for specified handle
 * @param[in] HwHandle - Hardware object handle
 * @return  Pointer to hardware object configuration or NULL_PTR if invalid
 */
const Can_HwObjectConfigType* Can_GetHwObjectConfig(Can_HwHandleType HwHandle)
{
    const Can_HwObjectConfigType* ConfigPtr = NULL_PTR;
    uint8 Index;
    
    for (Index = 0U; Index < (sizeof(Can_HwObjectConfigs) / sizeof(Can_HwObjectConfigType)); Index++)
    {
        if (Can_HwObjectConfigs[Index].HwObjectId == HwHandle)
        {
            ConfigPtr = &Can_HwObjectConfigs[Index];
            break;
        }
    }
    
    return ConfigPtr;
}

/**
 * @brief   Get CAN Filter Configuration
 * @details Returns pointer to filter configuration for specified filter number
 * @param[in] FilterNumber - Filter bank number
 * @return  Pointer to filter configuration or NULL_PTR if invalid
 */
const Can_FilterConfigType* Can_GetFilterConfig(uint8 FilterNumber)
{
    const Can_FilterConfigType* ConfigPtr = NULL_PTR;
    uint8 Index;
    
    for (Index = 0U; Index < (sizeof(Can_FilterConfigs) / sizeof(Can_FilterConfigType)); Index++)
    {
        if (Can_FilterConfigs[Index].FilterNumber == FilterNumber)
        {
            ConfigPtr = &Can_FilterConfigs[Index];
            break;
        }
    }
    
    return ConfigPtr;
}

/**
 * @brief   Get Total Number of Filter Configurations
 * @details Returns the total number of configured message filters
 * @return  Number of filter configurations
 */
uint8 Can_GetFilterConfigCount(void)
{
    return (uint8)(sizeof(Can_FilterConfigs) / sizeof(Can_FilterConfigType));
}