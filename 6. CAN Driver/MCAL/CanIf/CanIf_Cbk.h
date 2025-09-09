/****************************************************************************************
*                               CANIF_CBK.H                                            *
****************************************************************************************
* File Name   : CanIf_Cbk.h
* Module      : CAN Interface (CanIf)
* Description : AUTOSAR CanIf callback functions header file
* Version     : 1.0.0 - AUTOSAR compliant implementation
* Date        : 24/08/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef CANIF_CBK_H
#define CANIF_CBK_H

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "Std_Types.h"
#include "CanIf_Types.h"
#include "Can.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CANIF_CBK_VENDOR_ID                     (0x0001U)
#define CANIF_CBK_MODULE_ID                     (0x007CU)
#define CANIF_CBK_INSTANCE_ID                   (0x00U)
#define CANIF_CBK_AR_RELEASE_MAJOR_VERSION      (4U)
#define CANIF_CBK_AR_RELEASE_MINOR_VERSION      (4U)
#define CANIF_CBK_AR_RELEASE_REVISION_VERSION   (0U)
#define CANIF_CBK_SW_MAJOR_VERSION              (1U)
#define CANIF_CBK_SW_MINOR_VERSION              (0U)
#define CANIF_CBK_SW_PATCH_VERSION              (0U)

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
*                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
*                                     FUNCTION PROTOTYPES
==================================================================================================*/

/**
 * @brief   Indicates a successful reception of a received CAN Rx L-PDU
 * @details This service indicates a successful reception of a received CAN Rx L-PDU to the CanIf after
 *          passing all filters and validation checks. This function is called by the CAN driver.
 * @param[in] Mailbox - Pointer to mailbox structure of received CAN L-PDU
 * @param[in] PduInfoPtr - Pointer to CAN L-PDU buffer including the length (SduLength) and the pointer to the SDU buffer (SduDataPtr)
 * @note    Synchronous, Reentrant
 * @requirements SWS_CANIF_00415, SWS_CANIF_00416, SWS_CANIF_00417
 */
extern void CanIf_RxIndication(const Can_HwType* Mailbox, const PduInfoType* PduInfoPtr);

/**
 * @brief   Confirms the successful transmission of a CAN TxPDU
 * @details This service confirms a successful transmission of a CAN TxPDU in a CAN L-PDU.
 *          This function is called by the CAN driver.
 * @param[in] CanTxPduId - L-PDU handle of CAN L-PDU successfully transmitted
 * @note    Synchronous, Reentrant
 * @requirements SWS_CANIF_00396, SWS_CANIF_00397
 */
extern void CanIf_TxConfirmation(PduIdType CanTxPduId);

/**
 * @brief   Indicates a controller state transition referring to the corresponding CAN controller
 * @details This service indicates a controller state transition referring to the corresponding CAN controller
 *          with the abstract CanIf ControllerId. This function is called by the CAN driver.
 * @param[in] ControllerId - Abstract CanIf ControllerId which is assigned to a CAN controller
 * @param[in] ControllerMode - Mode to which the CAN controller transitioned
 * @note    Synchronous, Reentrant
 * @requirements SWS_CANIF_00699, SWS_CANIF_00700, SWS_CANIF_00701
 */
extern void CanIf_ControllerModeIndication(uint8 ControllerId, CanIf_ControllerModeType ControllerMode);

/**
 * @brief   Indicates that the CAN controller is in Bus-Off state
 * @details This service indicates that the CAN controller with the abstract CanIf ControllerId is in Bus-Off state.
 *          This function is called by the CAN driver.
 * @param[in] ControllerId - Abstract CanIf ControllerId which is assigned to a CAN controller
 * @note    Synchronous, Reentrant
 * @requirements SWS_CANIF_00218, SWS_CANIF_00219
 */
extern void CanIf_ControllerBusOff(uint8 ControllerId);

#ifdef __cplusplus
}
#endif

#endif /* CANIF_CBK_H */
