/** @file
  A Pei Timer Library implementation which uses the Time Stamp Counter in the processor.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include <PiPei.h>
#include <Library/HobLib.h>
#include <Guid/TscFrequency.h>
#include "TscTimerLibInternal.h"

/**  Get TSC frequency from TSC frequency GUID HOB, if the HOB is not found, build it.

  @return The number of TSC counts per second.

**/
UINT64
InternalGetTscFrequency (
  VOID
  )
{
  EFI_HOB_GUID_TYPE       *GuidHob;
  VOID        *DataInHob;
  UINT64      TscFrequency;

  //
  // Get TSC frequency from TSC frequency GUID HOB.
  //
  GuidHob = GetFirstGuidHob (&gEfiTscFrequencyGuid);
  if (GuidHob != NULL) {
    DataInHob = GET_GUID_HOB_DATA (GuidHob);
    TscFrequency = * (UINT64 *) DataInHob;
    return TscFrequency;
  }

  //
  // TSC frequency GUID HOB is not found, build it.
  //

  TscFrequency = InternalCalculateTscFrequency ();
  //
  // TscFrequency is now equal to the number of TSC counts per second, build GUID HOB for it.
  //
  BuildGuidDataHob (
    &gEfiTscFrequencyGuid,
    &TscFrequency,
    sizeof (UINT64)
    );

  return TscFrequency;
}

