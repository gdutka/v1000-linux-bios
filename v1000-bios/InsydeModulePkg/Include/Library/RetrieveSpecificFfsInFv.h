//;******************************************************************************
//;* Copyright (c) 1983-2016, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   RetrieveSpecificFfsInFv.h
//;
//; Abstract:
//;
//;   Get specific FFS from FV
//;
#ifndef _RETRIEVE_SPECIFIC_FFS_H_
#define _RETRIEVE_SPECIFIC_FFS_H_

#include <Uefi.h>

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
  );
#endif

