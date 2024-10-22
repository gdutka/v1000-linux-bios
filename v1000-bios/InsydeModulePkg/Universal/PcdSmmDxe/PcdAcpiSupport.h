/** @file
    This driver implements Global Pcd definition block in ACPI table and
    registers SMI callback functions for ReadPcd

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

#ifndef __PCD_ACPI_SUPPORT_H__
#define __PCD_ACPI_SUPPORT_H__

#include <PiDxe.h>
#include <SmiTable.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/PiPcdInfo.h>
#include <Protocol/PiPcd.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Pi/PiMultiPhase.h>
#include <Guid/ZeroGuid.h>


#define READ_PCD_COMMAND    0

typedef enum {
  H2O_PCD_ACPI_STATUS_SUCCESS,           // PCD data returned in PcdData. PcdDataSize updated with actual PCD data size.
  H2O_PCD_ACPI_STATUS_DATA_TOO_SMALL,    // PcdDataSize was too small to hold the PCD's data. PcdDataSize updated with the actual PCD data size
  H2O_PCD_ACPI_STATUS_INVALID_PCD,       // PCD not found or wrong PCD Type specified
  H2O_PCD_ACPI_STATUS_INVALID_ACTION     // PcdAction not set to a valid PCD action value
} H2O_PCD_STATUS;


///
/// Global PCD Area definition
///
#define MAX_PCD_DATA_SIZE 480

#pragma pack(1)
typedef struct _H2O_ACPI_GET_PCD_VALUE {
  UINT32   PcdDataSize;           // Unsigned integer that specifies the size of the PCD data, in bytes.
  UINT32   PcdTokenNumber;        // Unsigned integer that, along with PcdGuid, uniquely identifies the PCD in the PCD database.
  UINT8    PcdType;               // The returned information associated with the requested TokenNumber. If TokenNumber is 0, then PcdType is set to EFI_PCD_TYPE_8.
  UINT8    PcdStatus;             // the result of retrieving the PCD ,See H2O_PCD_STATUS
  UINT8    PcdAction;             // Enumerated value that specifies the PCD action to take,0 - Read a PCD from the PCD database.
  UINT8    Reserved[5];
  EFI_GUID PcdGuid;               // GUID that, along with the PcdTokenNumber, uniquely identifies a PCD in the PCD database.
  UINT8    PcdData[MAX_PCD_DATA_SIZE];  // the buffer that holds the returned PCD data. The size of this buffer is specified by PcdDataSize.It can not exceed MAX_PCD_DATA_SIZE
} H2O_ACPI_GET_PCD_VALUE;

typedef struct {
  UINT8                  OpRegionOp;
  UINT32                 NameString;
  UINT8                  RegionSpace;
  UINT8                  DWordPrefix;
  UINT32                 RegionOffset;
  UINT8                  WordPrefix;
  UINT16                 RegionLen;
} AML_OP_REGION_32_16;
#pragma pack()

#endif  // __PCD_ACPI_SUPPORT_H__
