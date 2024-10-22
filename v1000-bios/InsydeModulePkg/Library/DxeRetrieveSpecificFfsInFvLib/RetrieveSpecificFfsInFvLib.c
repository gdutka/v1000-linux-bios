/** @file
  Reterive specific FFS routines
;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/HobLib.h>

/**
  Retrive firmware binary from a PE formated image (standalone flash utility)

 @param [in]   FileGuid         A pointer to the file GUID
 @param [out]  Data             A pointer to the data being loaded
 @param [out]  DataSize         A pointer to the size of the data loaded

 @retval EFI_SUCCESS            The file data was successfully loaded
 @retval EFI_NOT_FOUND          Unable to find the file

**/
EFI_STATUS
RetrieveSpecificFfsInFv (
  IN  EFI_GUID                  *FileGuid,
  OUT VOID                      **Data,
  OUT UINTN                     *DataSize
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINT32                        AuthenticationStatus;
  EFI_PEI_HOB_POINTERS          GuidHob;

  Fv = NULL;
  AuthenticationStatus = 0;

  if (CompareGuid (FileGuid, (EFI_GUID *)PcdGetPtr (PcdSecureFlashPublicKeyFile))) {
    GuidHob.Raw = GetHobList ();
    GuidHob.Raw = GetNextGuidHob ((EFI_GUID *)PcdGetPtr (PcdSecureFlashPublicKeyFile), GuidHob.Raw);
    if (GuidHob.Raw == NULL) {
      return EFI_NOT_FOUND;
    }
    *Data       = GET_GUID_HOB_DATA (GuidHob.Guid);
    *DataSize   = GET_GUID_HOB_DATA_SIZE (GuidHob.Guid);

    return EFI_SUCCESS;
  } 

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || (HandleCount == 0)) {
    return EFI_NOT_FOUND;
  }

  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **)&Fv
                    );

    if (EFI_ERROR (Status)) {
      return EFI_LOAD_ERROR;
    }

    *Data = NULL;
    *DataSize = 0;
    Status = Fv->ReadSection (
                   Fv,
                   FileGuid,
                   EFI_SECTION_RAW,
                   0,
                   Data,
                   DataSize,
                   &AuthenticationStatus
                   );

    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  if (Index >= HandleCount) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}
