/** @file

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
--*/
/** 
  Null instance of Sec Platform Hook Lib.

  Copyright (c)  2007 - 2010, Intel Corporation. All rights reserved. <BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


**/

//
// The package level header files this module uses
//
#include "PlatformSecLib.h"
#include <Library/SecOemSvcChipsetLib.h>

#include <Ppi/PspSmmResume.h>
#include <PspResumeMemInfo.h>

extern  EFI_GUID  gCapsuleUpdateDetectedPpiGuid;

EFI_PEI_PPI_DESCRIPTOR mPspSmmResumePpi =
{
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPspSmmResumePpiGuid,
    NULL
};

EFI_PEI_PPI_DESCRIPTOR mCapsuleUpdateDetectedPpi =
{
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gCapsuleUpdateDetectedPpiGuid,
    NULL
};

VOID
InitializeFloatingPointUnits (
  VOID
  );

EFI_PEI_PPI_DESCRIPTOR *
EFIAPI
InitializePpis (
  IN OUT  EFI_SEC_PEI_HAND_OFF    *SecCoreData
  )
{
  PSP_SMM_RSM_MEM_INFO   *PspSmmRsmMemInfo;
  PSP_SMM_RESUME_PPI     *PspSmmResumePpi;
  EFI_PEI_PPI_DESCRIPTOR *PspSmmResumePpiDesc;
  UINT32                 *StackPointer;

  //
  // Look for PSP_SMM_RSM_MEM_INFO in Stack
  //
  StackPointer = (UINT32*)((UINT8*)SecCoreData + SecCoreData->DataSize);
  for (;;) {
    if (StackPointer >= (UINT32*)((UINTN)SecCoreData->StackBase + SecCoreData->StackSize)) {
      return NULL;
    }
    if (*StackPointer != (UINT32)SecCoreData->BootFirmwareVolumeBase) {
      StackPointer++;
      continue;
    }
    if (*(StackPointer + 2) != 0) {
      PspSmmRsmMemInfo = (PSP_SMM_RSM_MEM_INFO *)*(StackPointer + 2);
      break;
    }
    StackPointer++;
  }
  //
  //Init PspSmmResumePpiDesc
  //
  PspSmmResumePpi = (PSP_SMM_RESUME_PPI*)(UINTN)PspSmmRsmMemInfo->TempRegionPtr;
  PspSmmResumePpi->HeapBase = PspSmmRsmMemInfo->RsmHeapPtr;
  PspSmmResumePpiDesc = (EFI_PEI_PPI_DESCRIPTOR*)(UINTN)(PspSmmResumePpi + 1);
  CopyMem (PspSmmResumePpiDesc, &mPspSmmResumePpi, sizeof (mPspSmmResumePpi));
  PspSmmResumePpiDesc->Ppi = PspSmmResumePpi;
  //
  // Install Capsule update detected PPI
  //
  if (*(StackPointer + 1) == 0xCA9501EFul) {
    PspSmmResumePpiDesc->Flags &= ~EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    CopyMem (PspSmmResumePpiDesc + 1, &mCapsuleUpdateDetectedPpi, sizeof (mCapsuleUpdateDetectedPpi));
  }

  return PspSmmResumePpiDesc;

}


/**
  A developer supplied function to perform platform specific operations.

  It's a developer supplied function to perform any operations appropriate to a
  given platform. It's invoked just before passing control to PEI core by SEC
  core. Platform developer may modify the SecCoreData passed to PEI Core.
  It returns a platform specific PPI list that platform wishes to pass to PEI core.
  The Generic SEC core module will merge this list to join the final list passed to
  PEI core.

  @param  SecCoreData           The same parameter as passing to PEI core. It
                                could be overridden by this function.

  @return The platform specific PPI list to be passed to PEI core or
          NULL if there is no need of such platform specific PPI list.

**/
EFI_PEI_PPI_DESCRIPTOR *
EFIAPI
SecPlatformMain (
  IN OUT   EFI_SEC_PEI_HAND_OFF        *SecCoreData
  )
{
  EFI_PEI_PPI_DESCRIPTOR      *PpiList;

  PpiList = NULL;

  InitializeFloatingPointUnits ();
  if (((UINTN)(SecCoreData->TemporaryRamBase) + SecCoreData->TemporaryRamSize - 4) != (UINT32)TopOfCar) {
    //
    // Install PPIs
    //
    PpiList = InitializePpis (SecCoreData);
  }

  if (PcdGetBool (PcdH2ODdtSupported)) {
    OemSvcInitializeInsydeDebugger ();
  }

  OemSvcOemSecPlatformMain (SecCoreData, PpiList);

  return PpiList;

}


/**
  This interface conveys state information out of the Security (SEC) phase into PEI.

  @param  PeiServices               Pointer to the PEI Services Table.
  @param  StructureSize             Pointer to the variable describing size of the input buffer.
  @param  PlatformInformationRecord Pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD.

  @retval EFI_SUCCESS           The data was successfully returned.
  @retval EFI_BUFFER_TOO_SMALL  The buffer was too small.

**/
EFI_STATUS
EFIAPI
SecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN OUT   UINT64                                *StructureSize,
  OUT      EFI_SEC_PLATFORM_INFORMATION_RECORD   *PlatformInformationRecord
  )
{
  UINT32  *BIST;
  UINT32  Size;
  UINT32  Count;

  //
  // The entries of BIST information, together with the number of them,
  // reside in the bottom of stack, left untouched by normal stack operation.
  // This routine copies the BIST information to the buffer pointed by
  // PlatformInformationRecord for output.
  //
  Count = *(TopOfCar - 10); // CPU counts
  Size  = Count * sizeof (IA32_HANDOFF_STATUS);

  if ((*StructureSize) < (UINT64) Size) {
    *StructureSize = Size;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StructureSize  = Size;
  BIST            = (UINT32 *) ((UINT32) TopOfCar - 44);

  CopyMem (PlatformInformationRecord, BIST, sizeof (IA32_HANDOFF_STATUS)); // BSP only

  return EFI_SUCCESS;
}

