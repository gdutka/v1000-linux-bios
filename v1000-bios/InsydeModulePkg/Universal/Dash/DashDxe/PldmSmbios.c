/** @file
  Platform Level Data Model for SMBIOS Data Transfer implementation

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "PldmSmbios.h"



    /**

  Get the full size of SMBIOS structure including optional strings that follow the formatted structure.

  @param This                   The EFI_SMBIOS_PROTOCOL instance.
  @param Head                   Pointer to the beginning of SMBIOS structure.
  @param Size                   The returned size.
  @param NumberOfStrings        The returned number of optional strings that follow the formatted structure.

  @retval EFI_SUCCESS           Size retured in Size.
  @retval EFI_INVALID_PARAMETER Input SMBIOS structure mal-formed or Size is NULL.

**/
EFI_STATUS
EFIAPI
GetSmbiosStructureSize (
  IN   EFI_SMBIOS_TABLE_HEADER          *Head,
  OUT  UINTN                            *Size,
  OUT  UINTN                            *NumberOfStrings
  )
{
  UINTN  FullSize;
  UINTN  StrLen;
  UINTN  MaxLen;
  INT8*  CharInStr;

  if (Size == NULL || NumberOfStrings == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FullSize = Head->Length;
  CharInStr = (INT8*)Head + Head->Length;
  *Size = FullSize;
  *NumberOfStrings = 0;
  StrLen = 0;

  //
  // SMBIOS 3.0 defines the Structure table maximum size as DWORD field limited to 0xFFFFFFFF bytes.
  // Locate the end of string as long as possible.
  //
  MaxLen = SMBIOS_3_0_TABLE_MAX_LENGTH;

  //
  // look for the two consecutive zeros, check the string limit by the way.
  //
  while (*CharInStr != 0 || *(CharInStr+1) != 0) {
    if (*CharInStr == 0) {
      *Size += 1;
      CharInStr++;
    }

    for (StrLen = 0 ; StrLen < MaxLen; StrLen++) {
      if (*(CharInStr+StrLen) == 0) {
        break;
      }
    }

    if (StrLen == MaxLen) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // forward the pointer
    //
    CharInStr += StrLen;
    *Size += StrLen;
    *NumberOfStrings += 1;
  }

  //
  // count ending two zeros.
  //
  *Size += 2;
  return EFI_SUCCESS;
}



/**
 Send SBIOS Structure Table Meta Data to the management controller

 @param        This             Pointer to EFI_PLDM_PROTOCOL

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmSetSmbiosStructureTableMetaData (
  EFI_PLDM_PROTOCOL                              *This
  )
{
  PLDM_CONTEXT *Context;
  SMBIOS_TABLE_ENTRY_POINT *SmbiosEntry;
  SMBIOS_TABLE_3_0_ENTRY_POINT  *SmbiosEntry64Bit;
  EFI_STATUS   Status;
  PLDM_SMBIOS_STRUCTURE_TABLE_METADATA PldmRequest;
  UINTN        RecordSize;
  UINT16       MaxStructureSize;
  UINT16       NumberOfSmbiosStructures;
  UINTN                           NumOfStr;
  EFI_SMBIOS_TABLE_HEADER         *SmbiosRecord;
  EFI_SMBIOS_PROTOCOL             *SmbiosProtocol;
  EFI_SMBIOS_HANDLE               SmbiosHandle;


  Context = PLDM_CONTEXT_FROM_THIS(This);

  if (((PcdGet16 (PcdSmbiosVersion) >> 8) < 0x3) ||
      (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT0) == BIT0))) {

    Status = EfiGetSystemConfigurationTable (
             &gEfiSmbiosTableGuid,
             (VOID **)&SmbiosEntry
             );
    EFI_ERROR_RETURN (Status);

    PldmRequest.MajorVersion             = SmbiosEntry->MajorVersion;
    PldmRequest.MinorVersion             = SmbiosEntry->MinorVersion;
    PldmRequest.MaxStructureSize         = SmbiosEntry->MaxStructureSize;
    PldmRequest.TableLength              = Context->SmbiosTableLen; //SmbiosEntry->TableLength;
    PldmRequest.NumberOfSmbiosStructures = SmbiosEntry->NumberOfSmbiosStructures;
    PldmRequest.Crc32                    = Context->SmbiosCrc32;

  } else {

    Status = EfiGetSystemConfigurationTable (
             &gEfiSmbios3TableGuid,
             (VOID **)&SmbiosEntry64Bit
             );
    EFI_ERROR_RETURN (Status);

    RecordSize = 0;
    MaxStructureSize = 0;
    NumOfStr = 0;
    NumberOfSmbiosStructures = 0;

    Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &SmbiosProtocol);
    EFI_ERROR_RETURN (Status);
    //
    // Calculate EPS Table Length
    //
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;

    do {
      Status = SmbiosProtocol->GetNext (SmbiosProtocol, &SmbiosHandle, NULL, &SmbiosRecord, NULL);
      if (Status == EFI_SUCCESS) {
        GetSmbiosStructureSize(SmbiosRecord, &RecordSize, &NumOfStr);
        //
        // Record NumberOfSmbiosStructures, TableLength and MaxStructureSize
        //
        NumberOfSmbiosStructures++;
        if (RecordSize >= MaxStructureSize) {
          MaxStructureSize = (UINT16) RecordSize;
        }
      }
    } while (!EFI_ERROR(Status));

    PldmRequest.MajorVersion             = SmbiosEntry64Bit->MajorVersion;
    PldmRequest.MinorVersion             = SmbiosEntry64Bit->MinorVersion;
    PldmRequest.MaxStructureSize         = MaxStructureSize;
    PldmRequest.TableLength              = (UINT16)SmbiosEntry64Bit->TableMaximumSize; //SmbiosEntry->TableLength;
    PldmRequest.NumberOfSmbiosStructures = NumberOfSmbiosStructures;
    PldmRequest.Crc32                    = Context->Smbios64BitCrc32;

  }
  return ProcessSimplePldmMessage( This,
                                   PLDM_TYPE_SMBIOS,
                                   PLDM_SET_SMBIOS_STRUCTURE_TABLE_METADATA,
                                   &PldmRequest,
                                   sizeof (PldmRequest),
                                   NULL,
                                   0
                                 );
}

/**
 Receive SBIOS Structure Table Meta Data from the management controller

 @param        This             Pointer to EFI_PLDM_PROTOCOL
 @param        PldmResponse     The received SMBIOS Structure Table Meta Data from the management controller

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmGetSmbiosStructureTableMetaData (
  EFI_PLDM_PROTOCOL                              *This,
  PLDM_SMBIOS_STRUCTURE_TABLE_METADATA          *PldmResponse
  )
{
  UINTN      Size;

  Size = sizeof (PLDM_SMBIOS_STRUCTURE_TABLE_METADATA);
  return ProcessSimplePldmMessage( This,
                                     PLDM_TYPE_SMBIOS,
                                     PLDM_GET_SMBIOS_STRUCTURE_TABLE_METADATA,
                                     NULL,
                                     0,
                                     PldmResponse,
                                     &Size
                                   );
}



/**
 Receive SBIOS Structure Table from the management controller

 @param [in]   This             Pointer to EFI_PLDM_PROTOCOL
 @param [out]  SmbiosBuf        Buffer storing the SMBIOS Sturcture Table
 @param [in, out] Size          Number of bytes received for SmbiosBuf

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmGetSmbiosStructureTable (
  IN     EFI_PLDM_PROTOCOL *This,
  OUT    UINT8             *SmbiosBuf,
  IN OUT UINTN             *Size
  )
{
  PLDM_TRANSFER_HEADER TransferHeader;

  return ProcessPldmMessageWithSplitResponses ( This,
                                                PLDM_TYPE_SMBIOS,
                                                PLDM_GET_SMBIOS_STRUCTURE_TABLE,
                                                &TransferHeader,
                                                sizeof (TransferHeader),
                                                SmbiosBuf,
                                                Size
                                              );
}

/**
 Send SBIOS Structure Table to the management controller

 @param        This             Pointer to EFI_PLDM_PROTOCOL

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PldmSetSmbiosStructureTable(
  EFI_PLDM_PROTOCOL                              *This
  )
{
  EFI_STATUS Status;
  SMBIOS_TABLE_ENTRY_POINT *SmbiosEntry;
  UINT8 *SmbiosBuf;
  UINTN Size;
  UINTN Index;
  PLDM_TRANSFER_HEADER TransferHeader;
  EFI_CPU_IO2_PROTOCOL *CpuIo;
  SMBIOS_TABLE_3_0_ENTRY_POINT  *SmbiosEntry64Bit;

  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, (VOID **)&CpuIo);
  EFI_ERROR_RETURN (Status);

  if (((PcdGet16 (PcdSmbiosVersion) >> 8) < 0x3) ||
      (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT0) == BIT0))) {

    Status = EfiGetSystemConfigurationTable (
             &gEfiSmbiosTableGuid,
             (VOID **)&SmbiosEntry
             );
    EFI_ERROR_RETURN (Status);

    if ((SmbiosEntry->TableLength % 4) == 0) {
      Size = SmbiosEntry->TableLength;
    } else {
      Size = SmbiosEntry->TableLength + (4 - (SmbiosEntry->TableLength % 4));
    }

    SmbiosBuf = AllocatePool (Size);
    if (SmbiosBuf == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = CpuIo->Mem.Read (CpuIo, EfiCpuIoWidthUint8, SmbiosEntry->TableAddress, SmbiosEntry->TableLength, SmbiosBuf);
    if(EFI_ERROR(Status)) {
      FreePool(SmbiosBuf);
      return Status;
    }

    for (Index = SmbiosEntry->TableLength; Index < Size; Index++) {
      SmbiosBuf[Index] = 0;
    }


  } else {

    Status = EfiGetSystemConfigurationTable (
             &gEfiSmbios3TableGuid,
             (VOID **)&SmbiosEntry64Bit
             );
    EFI_ERROR_RETURN (Status);

    if ((SmbiosEntry64Bit->TableMaximumSize % 4) == 0) {
      Size = SmbiosEntry64Bit->TableMaximumSize;
    } else {
      Size = SmbiosEntry64Bit->TableMaximumSize + (4 - (SmbiosEntry64Bit->TableMaximumSize % 4));
    }

    SmbiosBuf = AllocatePool (Size);
    if (SmbiosBuf == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = CpuIo->Mem.Read (CpuIo, EfiCpuIoWidthUint8, SmbiosEntry64Bit->TableAddress, SmbiosEntry64Bit->TableMaximumSize, SmbiosBuf);
    if(EFI_ERROR(Status)) {
      FreePool(SmbiosBuf);
      return Status;
    }

    for (Index = SmbiosEntry64Bit->TableMaximumSize; Index < Size; Index++) {
      SmbiosBuf[Index] = 0;
    }

  }

  Status = ProcessPldmMessageWithSplitRequests ( This,
                                                 PLDM_TYPE_SMBIOS,
                                                 PLDM_SET_SMBIOS_STRUCTURE_TABLE,
                                                 &TransferHeader,
                                                 sizeof(TransferHeader),
                                                 SmbiosBuf,
                                                 Size
                                               );

  FreePool(SmbiosBuf);

  return Status;

}

