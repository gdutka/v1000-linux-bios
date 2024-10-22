/** @file
  Library  Constructor of PeiFlashRegionLib
  
;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/FlashRegionLib.h>
#include <Library/BaseLib.h>
#include <Library/H2OCpLib.h>
#include <Library/PostCodeLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Ppi/Pcd.h>


STATIC EFI_GUID       mFdmContentHobGuid    = { 0x77f7a464, 0x7409, 0x4a19, { 0x8a, 0xae, 0x3f, 0x36, 0xa2, 0x02, 0x8b, 0x93 } };



/**
  Build FDM content HOB.

  @param PeiServices            General purpose services available to every PEIM.
  @param NotifyDescriptor       Pointer to EFI_PEI_NOTIFY_DESCRIPTOR
  @param Ppi                    Pointer to the PPI data associated with this function.

  @retval EFI_SUCCESS           if the interface could be successfully installed.
  @return Others                Some error occurs during the execution of this function.

**/
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyFlashRegionLibCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
  {

  H2O_FLASH_DEVICE_MAP_HEADER   *FdmHeader;
  EFI_HOB_GUID_TYPE             *GuidHob;
  UINT64                        *HobData;

  GuidHob  = GetFirstGuidHob (&mFdmContentHobGuid);
  if (GuidHob == NULL) {
    FdmHeader =(H2O_FLASH_DEVICE_MAP_HEADER*)(UINTN) PcdGet64(PcdH2OFlashDeviceMapStart);
    HobData = (UINT64*) BuildGuidHob (&mFdmContentHobGuid, (UINTN) FdmHeader->Size);
    if (HobData == NULL){
       return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (HobData, FdmHeader, FdmHeader->Size);
  }
  return EFI_SUCCESS;
}




static EFI_PEI_NOTIFY_DESCRIPTOR      mNotifyPpiList[] = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    MemoryDiscoveredPpiNotifyFlashRegionLibCallback
  };


/**
  The constructor function for Flash Region Libray.

  @param[in]  FileHandle  Handle of the file being invoked.
  @param[in]  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
**/
EFI_STATUS
EFIAPI
PeiFlashRegionLibInit (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{

  EFI_HOB_GUID_TYPE                 *GuidHob;
  EFI_STATUS                        Status;


  GuidHob  = GetFirstGuidHob (&mFdmContentHobGuid);
  if (GuidHob == NULL) {
    //
    // Notify NotifyPpiList
    //
    Status = PeiServicesNotifyPpi (&mNotifyPpiList[0]);
    ASSERT_EFI_ERROR (Status);
  }
  
  return EFI_SUCCESS;
}
