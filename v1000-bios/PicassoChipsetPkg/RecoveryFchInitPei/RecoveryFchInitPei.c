/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "RecoveryFchInitPei.h"

STATIC UINTN mMmioBaseRegPciAdr  = 0;
STATIC UINTN mMmioLimitRegPciAdr = 0;

EFI_STATUS
RemoveMmioResource (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mRemoveMmioResourceNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  RemoveMmioResource
};

EFI_STATUS
RemoveMmioResource (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{

  DEBUG ((EFI_D_INFO, "[%a] Entry\n", __FUNCTION__));


  DEBUG ((EFI_D_INFO, "[%a] Exit\n", __FUNCTION__));

  return EFI_SUCCESS;

}

/**
  The entry of the recovery Fch initialize.

  @param   FfsHeader                 The pointer to the FFS header of the file to load.
  @param   PeiServices               General purpose services available to every PEIM.

  @retval  EFI_SUCCESS               Add description for return value

**/
EFI_STATUS
RecoveryFchInitEntryPoint (
  IN EFI_FFS_FILE_HEADER         *FfsHeader,
  IN CONST EFI_PEI_SERVICES      **PeiServices
  )
{
  EFI_STATUS            Status;
  UINT32                MmioResBase = 0;
  UINT32                OrgMmioResBase;
  UINT32                MmioResSize;
  FABRIC_RESOURCE_FOR_EACH_RB   MmioForEachDie;  

  Status         = EFI_SUCCESS;
  MmioResBase    = 0;
  OrgMmioResBase = 0;
  MmioResSize    = 0;

  DEBUG ((EFI_D_INFO, "RecoveryFchInitEntryPoint Entry.\n"));

  MmioResBase = (UINT32)PcdGet64 (PcdPciExpressBaseAddress) + (UINT32)PcdGet32 (PcdPciExpressSize);

  //
  // Allocate MMIO resources.
  //
  Status = FabricGetAvailableResource (&MmioForEachDie);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, " RecoveryFchInitEntryPoint:Get Fabric Resource failed.\n")); 
    return EFI_OUT_OF_RESOURCES; 
  } 

  if (MmioForEachDie.NonPrefetchableMmioSizeBelow4G[0][0].Size >= (SIZE_256MB + SIZE_2MB + SIZE_8MB)) {
    MmioResBase = (UINT32)MmioForEachDie.NonPrefetchableMmioSizeBelow4G[0][0].Base + SIZE_256MB + SIZE_2MB;
    MmioResSize = (UINT32)MmioForEachDie.NonPrefetchableMmioSizeBelow4G[0][0].Size - (SIZE_256MB + SIZE_2MB);                                                  
  }
  
  if ((MmioForEachDie.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size >= (SIZE_256MB + SIZE_2MB + SIZE_8MB)) && MmioResBase == 0) {
    MmioResBase = (UINT32)MmioForEachDie.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Base + (SIZE_256MB + SIZE_2MB);
    MmioResSize = (UINT32)MmioForEachDie.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size - (SIZE_256MB + SIZE_2MB);
  }   

  OrgMmioResBase = MmioResBase;

  DEBUG ((EFI_D_INFO, "  MMIO = 0x%x, Length = 0x%x\n", MmioResBase, MmioResSize));

  Status = RecoveryInitGppPortDevice (PeiServices, &MmioResBase);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "  RecoveryInitGppPortDevice FAIL!! (%r)\n", Status));
  }

  DEBUG ((EFI_D_ERROR, "  ** Assigned Size = %x **\n", MmioResSize));
  DEBUG ((EFI_D_ERROR, "  **     Used Size = %x **\n", (UINT32)(MmioResBase - OrgMmioResBase)));
  if ((MmioResBase - OrgMmioResBase) > MmioResSize) {
    DEBUG ((EFI_D_ERROR, "  !!! Out Of Resource !!!\n"));
  }

  //
  // Install notification in order to remove the Mmio resource.
  //
  if ((PcdGetBool (PcdUseFastCrisisRecovery)) &&
      (mMmioBaseRegPciAdr  != 0) &&
      (mMmioLimitRegPciAdr != 0)
     ) {
    (**PeiServices).NotifyPpi (PeiServices, &mRemoveMmioResourceNotify);
  }

  DEBUG ((EFI_D_INFO, "RecoveryFchInitEntryPoint Exit.\n"));

  return EFI_SUCCESS;

}
