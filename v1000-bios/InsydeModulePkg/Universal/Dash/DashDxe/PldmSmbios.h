/** @file
  Platform Level Data Model for SMBIOS Data Transfer

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

#ifndef _DASH_PLDM_SMBIOS_H_
#define _DASH_PLDM_SMBIOS_H_

#include <Protocol/Smbios.h>
#include <Library/PcdLib.h>
#include <Guid/SmBios.h>
#include "Pldm.h"

#pragma pack (1)


typedef struct {
  UINT32             NextDataTransferHandle;
  PLDM_TRANSFER_FLAG TransferFlag;
  UINT8              SmBiosTableData[1];
} PLDM_SMBIOS_STRUCTURE_TABLE;

//
// SMBIOS record Header
//
// An SMBIOS internal Record is an EFI_SMBIOS_RECORD_HEADER followed by (RecordSize - HeaderSize) bytes of
//  data. The format of the data is defined by the SMBIOS spec.
//
//
#define EFI_SMBIOS_RECORD_HEADER_VERSION  0x0100
typedef struct {
  UINT16      Version;
  UINT16      HeaderSize;
  UINTN       RecordSize;
  EFI_HANDLE  ProducerHandle;
  UINTN       NumberOfStrings;
} EFI_SMBIOS_RECORD_HEADER;


//
// Private data structure to contain the SMBIOS record. One record per
//  structure. SmbiosRecord is a copy of the data passed in and follows RecordHeader .
//
#define EFI_SMBIOS_ENTRY_SIGNATURE  SIGNATURE_32 ('S', 'r', 'e', 'c')
typedef struct {
  UINT32                    Signature;
  LIST_ENTRY                Link;
  EFI_SMBIOS_RECORD_HEADER  *RecordHeader;
  UINTN                     RecordSize;
  //
  // Indicate which table this record is added to.
  //
  BOOLEAN                   Smbios32BitTable;
  BOOLEAN                   Smbios64BitTable;
} EFI_SMBIOS_ENTRY;

#pragma pack ()

#endif