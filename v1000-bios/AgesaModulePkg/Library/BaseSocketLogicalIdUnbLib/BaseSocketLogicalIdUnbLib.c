/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include <Library/BaseLib.h>
#include <Library/BaseSocLogicalIdXlatLib.h>
#include <Library/PciLib.h>
#include "Filecode.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_BASESOCKETLOGICALIDUNBLIB_BASESOCKETLOGICALIDUNBLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

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
  IN       UINTN   Socket,
     OUT   UINT32  *RawId
  )
{
  if (Socket != 0) {
    return AGESA_BOUNDS_CHK;
  }

  if (RawId == NULL) {
    return AGESA_BOUNDS_CHK;
  }

  *RawId = PciRead32 (MAKE_SBDFO (0, 0, 0x18, 3, 0xFC));
  return AGESA_SUCCESS;
}

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
  )
{
  UINT32        RawId;
  AGESA_STATUS  Status;

  Status = BaseGetRawIdOnGivenSocket (Socket, &RawId);

  if (Status != AGESA_SUCCESS) {
    return Status;
  }

  if (BaseSocConvertRawToLogical (RawId, LogicalId)) {
    return AGESA_SUCCESS;
  } else {
    return AGESA_ERROR;
  }
}


