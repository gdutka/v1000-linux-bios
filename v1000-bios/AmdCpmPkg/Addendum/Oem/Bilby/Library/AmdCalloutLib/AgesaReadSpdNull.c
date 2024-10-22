/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <Uefi.h>
#include <AGESA.h>
#include <Library/AmdCalloutLib.h>
#include <AgesaReadSpd.h>

/**
 Call the host environment interface to read an SPD's content.

 @param[in]      FcnData
 @param[in,out]  ReadSpd

 @return   void
*/
AGESA_STATUS
AgesaReadSpd (
  IN       UINTN                   FcnData,
  IN OUT   AGESA_READ_SPD_PARAMS   *ReadSpd
  )
{
  return AGESA_UNSUPPORTED;
}
