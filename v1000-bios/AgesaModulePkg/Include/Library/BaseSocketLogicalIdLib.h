/*
 ******************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _BASE_SOCKET_LOGICAL_ID_LIB_H_
#define _BASE_SOCKET_LOGICAL_ID_LIB_H_

#include <AGESA.h>
#include <SocLogicalId.h>

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the raw family / model / stepping for the specified socket
 *
 * @param[in]   Socket             Zero-based socket number to check
 * @param[out]  RawId              Raw CPU ID for the given socket
 *
 * @retval      AGESA_SUCCESS      Given socket's logical ID was successfully found
 * @retval      AGESA_BOUNDS_CHK   Socket is not populated
 * @retval      AGESA_BOUNDS_CHK   RawId is NULL
 */
AGESA_STATUS
BaseGetRawIdOnGivenSocket (
  IN       UINTN  Socket,
     OUT   UINT32  *RawId
  );

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the raw family / model / stepping for the specified socket
 *
 * @param[in]   Socket             Zero-based socket number to check
 * @param[out]  LogicalId          Given socket's logical family and logical revision
 *
 * @retval      AGESA_SUCCESS      Given socket's logical ID was successfully found
 * @retval      AGESA_BOUNDS_CHK   Socket is not populated
 * @retval      AGESA_BOUNDS_CHK   LogicalId is NULL
 * @retval      AGESA_ERROR        The given socket's family is unknown
 */
AGESA_STATUS
BaseGetLogicalIdOnGivenSocket (
  IN       UINTN           Socket,
     OUT   SOC_LOGICAL_ID  *LogicalId
  );

#endif // _BASE_SOCKET_LOGICAL_ID_LIB_H_

