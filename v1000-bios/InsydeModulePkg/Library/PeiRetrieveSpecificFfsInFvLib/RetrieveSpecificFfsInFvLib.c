/** @file
  Reterive specific FFS routines
;******************************************************************************
;* Copyright (c) 2012 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>


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
  OUT UINT8                     **Data,
  OUT UINT32                    *DataSize
  )
{
  EFI_FIRMWARE_VOLUME_HEADER  *Fv;
  EFI_FFS_FILE_HEADER         *FileHeader;
  EFI_STATUS                  Status;
  UINT32                      Instance;

  Instance = 0;
  while (TRUE) {
    Status = PeiServicesFfsFindNextVolume (Instance, (VOID **)&Fv);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    FileHeader = NULL;
    //
    // Start new search in volume
    //
    while (TRUE) {
      Status = PeiServicesFfsFindNextFile (EFI_FV_FILETYPE_RAW, Fv, (VOID **)&FileHeader);
      if (Status == EFI_NOT_FOUND) {
        break;
      }

      if (CompareGuid (&(FileHeader->Name), FileGuid)) {
        *Data     = (UINT8 *) FileHeader + sizeof (EFI_FFS_FILE_HEADER);
        *DataSize = *(UINT32 *)(FileHeader->Size) & 0x00FFFFFF;
        *DataSize -= sizeof (EFI_FFS_FILE_HEADER);
        return EFI_SUCCESS;
      }
    }

    Instance += 1;
  }
  return EFI_NOT_FOUND;
}

