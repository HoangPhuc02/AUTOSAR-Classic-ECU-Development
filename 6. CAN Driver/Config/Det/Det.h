/****************************************************************************************
*                                  DET.H                                               *
****************************************************************************************
* File Name   : Det.h
* Module      : Development Error Tracer
* Description : Development Error Tracer stub for PWM module
* Version     : 1.0.0
* Date        : 27/06/2025
* Author      : hoangphuc540202@gmail.com
* Github      : https://github.com/HoangPhuc02
***************************************************************************************/

#ifndef DET_H
#define DET_H

/****************************************************************************************
*                                 INCLUDE FILES                                        *
****************************************************************************************/
#include "Std_Types.h"

/****************************************************************************************
*                              DEVELOPMENT ERROR TRACER                               *
****************************************************************************************/

/**
 * @brief Reports a development error
 * @details Stub implementation for development error reporting
 * @param[in] ModuleId Module ID
 * @param[in] InstanceId Instance ID
 * @param[in] ApiId API ID
 * @param[in] ErrorId Error ID
 * @return Always returns E_OK in stub implementation
 */
#define Det_ReportError(ModuleId, InstanceId, ApiId, ErrorId) (E_OK)

/**
 * @brief Reports a runtime error
 * @details Stub implementation for runtime error reporting
 * @param[in] ModuleId Module ID
 * @param[in] InstanceId Instance ID
 * @param[in] ApiId API ID
 * @param[in] ErrorId Error ID
 * @return Always returns E_OK in stub implementation
 */
#define Det_ReportRuntimeError(ModuleId, InstanceId, ApiId, ErrorId) (E_OK)

/**
 * @brief Reports a transient fault
 * @details Stub implementation for transient fault reporting
 * @param[in] ModuleId Module ID
 * @param[in] InstanceId Instance ID
 * @param[in] ApiId API ID
 * @param[in] FaultId Fault ID
 * @return Always returns E_OK in stub implementation
 */
#define Det_ReportTransientFault(ModuleId, InstanceId, ApiId, FaultId) (E_OK)

#endif /* DET_H */

/****************************************************************************************
*                                 END OF FILE                                          *
****************************************************************************************/
