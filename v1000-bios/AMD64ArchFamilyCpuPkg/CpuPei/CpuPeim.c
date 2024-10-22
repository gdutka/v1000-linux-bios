/** @file

 Implementation of CPU driver for PEI phase.
 This PEIM is to expose the Cache PPI.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "CpuPei.h"
#include "Bist.h"

extern PEI_CACHE_PPI                mCachePpi;

//
// Ppis to be installed
//
EFI_PEI_PPI_DESCRIPTOR           mPpiList[] = { 
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiCachePpiGuid,
    &mCachePpi
  }
};

//
// Notification to build BIST information
//
STATIC EFI_PEI_NOTIFY_DESCRIPTOR        mNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMasterBootModePpiGuid,
    BuildBistAndCpuInfoHob
  }
};

/**
  The Entry point of the CPU PEIM

  This function is the Entry point of the CPU PEIM which will install the CachePpi and 
  BuildBISTHob notifier. And also the function will deal with the relocation to memory when 
  permanent memory is ready
 
  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services. 
                          
  @retval EFI_SUCCESS   CachePpi is installed successfully.

**/
EFI_STATUS
EFIAPI
CpuPeimInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
	EFI_STATUS  Status;

  //
  // Install PPIs
  //
  Status = PeiServicesInstallPpi(&mPpiList[0]);
  ASSERT_EFI_ERROR (Status);

  //
  // Register for PPI Notifications
  //
  Status = PeiServicesNotifyPpi (&mNotifyList[0]);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
