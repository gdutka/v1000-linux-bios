/** @file
 Help function for MemoryInitPei to report no memory. 
 
***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include "InstallPeiMemoryInternal.h"

/**
 Report no memory. This is platform-specific but to conserve space, it is being built in line.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         ErrorCode           Handle of the file being invoked.

 @retval            None
*/
VOID
ReportMemoryError (
  IN CONST EFI_PEI_SERVICES                   **PeiServices,
  IN       UINTN                              ErrorCode
  )
{
  PEI_SPEAKER_IF_PPI              *SpeakerInterfacePpi;
  EFI_STATUS                      Status;

  if (ErrorCode != 0) {
    POST_CODE (ErrorCode);
  }

  Status = PeiServicesLocatePpi (
                            &gPeiSpeakerInterfacePpiGuid,
                            0,
                            NULL,
                            (VOID**)&SpeakerInterfacePpi
                            );
  if (!EFI_ERROR (Status)) {
    SpeakerInterfacePpi->SetSpeakerToneFrequency (0x500);
    SpeakerInterfacePpi->MemoryErrorBeep ();
  }

  CpuDeadLoop ();
}