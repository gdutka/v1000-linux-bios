/** @file
  ACPI Support Protocol implementation
;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2016, Linaro Ltd. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c) 1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  AcpiSupportAcpiSupportProtocol.c

Abstract:

  ACPI Support Protocol implementation

--*/

//
// Includes
//
#include "AcpiSupport.h"
//
// The maximum number of tables that pre-allocated.
//
UINTN         mEfiAcpiMaxNumTables = EFI_ACPI_MAX_NUM_TABLES;

//
// Allocation strategy to use for AllocatePages ().
// Runtime value depends on PcdExposedAcpiTableVersions.
//
STATIC EFI_ALLOCATE_TYPE      mAcpiTableAllocType;

EFI_STATUS
EFIAPI
GetAcpiTable (
  IN EFI_ACPI_SUPPORT_PROTOCOL            *This,
  IN INTN                                 Index,
  OUT VOID                                **Table,
  OUT EFI_ACPI_TABLE_VERSION              *Version,
  OUT UINTN                               *Handle
  );

EFI_STATUS
EFIAPI
SetAcpiTable (
  IN EFI_ACPI_SUPPORT_PROTOCOL            *This,
  IN VOID                                 *Table OPTIONAL,
  IN BOOLEAN                              Checksum,
  IN EFI_ACPI_TABLE_VERSION               Version,
  IN OUT UINTN                            *Handle
  );

EFI_STATUS
EFIAPI
PublishTables (
  IN EFI_ACPI_SUPPORT_PROTOCOL            *This,
  IN EFI_ACPI_TABLE_VERSION               Version
  );

EFI_STATUS
AddTableToList (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN VOID                                 *Table,
  IN BOOLEAN                              Checksum,
  IN EFI_ACPI_TABLE_VERSION               Version,
  OUT UINTN                               *Handle
  );

EFI_STATUS
RemoveTableFromList (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN EFI_ACPI_TABLE_VERSION               Version,
  IN UINTN                                Handle
  );

EFI_STATUS
RemoveTableFromRsdt (
  IN OUT EFI_ACPI_TABLE_LIST              *Table,
  IN OUT UINTN                            *NumberOfTableEntries,
  IN OUT EFI_ACPI_DESCRIPTION_HEADER      *Rsdt,
  IN OUT EFI_ACPI_DESCRIPTION_HEADER      *Xsdt OPTIONAL
  );

EFI_STATUS
ChecksumCommonTables (
  IN OUT EFI_ACPI_SUPPORT_INSTANCE        *AcpiSupportInstance
  );

UINT8
EFIAPI
CalculateSum8 (
  IN      CONST UINT8              *Buffer,
  IN      UINTN                   Length
  );

UINT8
EFIAPI
CalculateCheckSum8 (
  IN      CONST UINT8              *Buffer,
  IN      UINTN                   Length
  );

EFI_STATUS
UpdateTableList (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN VOID                                 *Table,
  IN BOOLEAN                              Checksum,
  IN EFI_ACPI_TABLE_VERSION               Version,
  IN UINTN                                Handle
  );

VOID
SyncFadtToRsdtXsdt (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN EFI_ACPI_TABLE_VERSION               Version
  );

//
// Protocol function implementations.
//

/**
 This function returns a table specified by an index if it exists.  The
 function returns a buffer containing the table that the caller must free.
 The function also returns a handle used to identify the table for update or
 deletion using the SetAcpiTable function.

 @param [in]   This             Instance of the protocol.
 @param [in]   Index            Zerobased index of the table to retrieve.
 @param [out]  Table            Returned pointer to the table.
 @param [out]  Version          Versions that the table is currently used in.
 @param [out]  Handle           Handle of the table, used in updating tables.

 @retval EFI_SUCCESS            The function completed successfully.
 @retval EFI_NOT_FOUND          The requested table does not exist.

**/
EFI_STATUS
EFIAPI
GetAcpiTable (
  IN EFI_ACPI_SUPPORT_PROTOCOL            *This,
  IN INTN                                 Index,
  OUT VOID                                **Table,
  OUT EFI_ACPI_TABLE_VERSION              *Version,
  OUT UINTN                               *Handle
  )
{
  EFI_ACPI_SUPPORT_INSTANCE *AcpiSupportInstance;
  INTN                      i;
  LIST_ENTRY                *CurrentLink;
  LIST_ENTRY                *StartLink;
  EFI_ACPI_TABLE_LIST       *CurrentTable;

  //
  // Check for invalid input parameters
  //
  ASSERT (This);
  ASSERT (Table);
  ASSERT (Handle);

  //
  // Get the instance of the protocol
  //
  AcpiSupportInstance = EFI_ACPI_SUPPORT_INSTANCE_FROM_ACPI_SUPPORT_THIS (This);

  //
  // Find the table
  //
  CurrentLink = AcpiSupportInstance->TableList.ForwardLink;
  StartLink   = &AcpiSupportInstance->TableList;
  for (i = 0; (i < Index) && (CurrentLink != StartLink) && CurrentLink; i++) {
    CurrentLink = CurrentLink->ForwardLink;
  }

  if (i != Index || CurrentLink == StartLink) {
    return EFI_NOT_FOUND;
  }
  //
  // Get handle and version
  //
  CurrentTable  = EFI_ACPI_TABLE_LIST_FROM_LINK (CurrentLink);
  *Handle       = CurrentTable->Handle;
  *Version      = CurrentTable->Version;

  //
  // Copy the table
  //
  *Table = AllocateCopyPool (CurrentTable->Table->Length, CurrentTable->Table);
  ASSERT (*Table);

  //
  // Done
  //
  return EFI_SUCCESS;
}


/**
 This function adds, removes, or updates ACPI tables.  If the address is not
 null and the handle value is null, the table is added.  If both the address and
 handle are not null, the table at handle is updated with the table at address.
 If the address is null and the handle is not, the table at handle is deleted.

 @param [in]   This             Instance of the protocol.
 @param [in]   Table            Pointer to a table.
 @param [in]   Checksum         Boolean indicating if the checksum should be calculated.
 @param [in]   Version          Version(s) to set.
 @param [in, out] Handle        Handle of the table.

 @retval EFI_SUCCESS            The function completed successfully.
 @retval EFI_INVALID_PARAMETER  Both the Table and *Handle were NULL.
 @retval EFI_ABORTED            Could not complete the desired request.

**/
EFI_STATUS
EFIAPI
SetAcpiTable (
  IN EFI_ACPI_SUPPORT_PROTOCOL            *This,
  IN VOID                                 *Table OPTIONAL,
  IN BOOLEAN                              Checksum,
  IN EFI_ACPI_TABLE_VERSION               Version,
  IN OUT UINTN                            *Handle
  )
{
  EFI_ACPI_SUPPORT_INSTANCE *AcpiSupportInstance;
  UINT32                    TableSignature;
  UINTN                     SavedHandle;
  EFI_STATUS                Status;

  //
  // Check for invalid input parameters
  //
  ASSERT (This);
  ASSERT (Handle);

  //
  // Get the instance of the protocol
  //
  AcpiSupportInstance = EFI_ACPI_SUPPORT_INSTANCE_FROM_ACPI_SUPPORT_THIS (This);

  //
  // Initialize locals
  //
  //
  // Determine desired action
  //
  if (!*Handle) {
    if (!Table) {
      //
      // Invalid parameter combination
      //
      return EFI_INVALID_PARAMETER;
    } else {
      //
      // Add table
      //
      Status = AddTableToList (AcpiSupportInstance, Table, Checksum, Version, Handle);
      if (!EFI_ERROR(Status)) {
        TableSignature = ((EFI_ACPI_COMMON_HEADER *) Table)->Signature;
        if (TableSignature == EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
          //
          // Synchronize FADT table into the first entry of RSDT/XSDT
          //
          SyncFadtToRsdtXsdt (AcpiSupportInstance, Version);
        }
      }
    }
  } else {
    if (Table) {
      //
      // Update table
      //
      //
      // Update related table data from the current table address when
      // if it can meet the specified condition
      //
      Status = UpdateTableList (AcpiSupportInstance, Table, Checksum, Version, *Handle);
      if (!EFI_ERROR (Status)) {
        return EFI_SUCCESS;
      }
      //
      // Delete the table list entry
      //
      Status = RemoveTableFromList (AcpiSupportInstance, Version, *Handle);
      if (EFI_ERROR (Status)) {
        //
        // Should not get an error here ever, but abort if we do.
        //
        return EFI_ABORTED;
      }
      //
      // Set the handle to replace the table at the same handle
      //
      SavedHandle                         = AcpiSupportInstance->CurrentHandle;
      AcpiSupportInstance->CurrentHandle  = *Handle;

      //
      // Add the table
      //
      Status = AddTableToList (AcpiSupportInstance, Table, Checksum, Version, Handle);
      if (!EFI_ERROR(Status)) {
        TableSignature = ((EFI_ACPI_COMMON_HEADER *) Table)->Signature;
        if (TableSignature == EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
          //
          // Synchronize FADT table into the first entry of RSDT/XSDT
          //
          SyncFadtToRsdtXsdt (AcpiSupportInstance, Version);
        }
      }
      //
      // Restore the saved current handle
      //
      AcpiSupportInstance->CurrentHandle = SavedHandle;
    } else {
      //
      // Delete table
      //
      Status = RemoveTableFromList (AcpiSupportInstance, Version, *Handle);
    }
  }

  if (EFI_ERROR (Status)) {
    //
    // Should not get an error here ever, but abort if we do.
    //
    return EFI_ABORTED;
  }
  //
  // Done
  //
  return EFI_SUCCESS;
}

/**
 This function publishes the specified versions of the ACPI tables by
 installing EFI configuration table entries for them.  Any combination of
 table versions can be published.

 @param [in]   This             Instance of the protocol.
 @param [in]   Version          Version(s) to publish.

 @retval EFI_SUCCESS            The function completed successfully.
 @retval EFI_ABORTED            The function could not complete successfully.

**/
EFI_STATUS
EFIAPI
PublishTables (
  IN EFI_ACPI_SUPPORT_PROTOCOL            *This,
  IN EFI_ACPI_TABLE_VERSION               Version
  )
{
  EFI_ACPI_SUPPORT_INSTANCE *AcpiSupportInstance;
  EFI_STATUS                Status;
  UINT32                    *CurrentRsdtEntry;
  VOID                      *CurrentXsdtEntry;
  UINT64                    Buffer64;

  //
  // Get the instance of the protocol
  //
  AcpiSupportInstance = EFI_ACPI_SUPPORT_INSTANCE_FROM_ACPI_SUPPORT_THIS (This);

  //
  // Reorder tables as some operating systems don't seem to find the
  // FADT correctly if it is not in the first few entries
  //

  //
  // Add FADT as the first entry
  //
  if ((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    CurrentRsdtEntry  = (UINT32 *) ((UINT8 *) AcpiSupportInstance->Rsdt1 + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
    *CurrentRsdtEntry = (UINT32) (UINTN) AcpiSupportInstance->Fadt1;

    CurrentRsdtEntry  = (UINT32 *) ((UINT8 *) AcpiSupportInstance->Rsdt3 + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
    *CurrentRsdtEntry = (UINT32) (UINTN) AcpiSupportInstance->Fadt3;
  }
  if ((Version & ACPI_TABLE_VERSION_GTE_2_0) != 0) {
    CurrentXsdtEntry  = (VOID *) ((UINT8 *) AcpiSupportInstance->Xsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
    //
    // Add entry to XSDT, XSDT expects 64 bit pointers, but
    // the table pointers in XSDT are not aligned on 8 byte boundary.
    //
    Buffer64 = (UINT64) (UINTN) AcpiSupportInstance->Fadt3;
    CopyMem (
      CurrentXsdtEntry,
      &Buffer64,
      sizeof (UINT64)
      );
  }

  //
  // Do checksum again because Dsdt/Xsdt is updated.
  //
  ChecksumCommonTables (AcpiSupportInstance);

  //
  // Add the RSD_PTR to the system table and store that we have installed the
  // tables.
  //
  if ((Version & EFI_ACPI_TABLE_VERSION_1_0B) !=0) {
    Status = gBS->InstallConfigurationTable (&gEfiAcpi10TableGuid, AcpiSupportInstance->Rsdp1);
    if (EFI_ERROR (Status)) {
      return EFI_ABORTED;
    }
  }

  if ((Version & ACPI_TABLE_VERSION_GTE_2_0) != 0) {
    Status = gBS->InstallConfigurationTable (&gEfiAcpiTableGuid, AcpiSupportInstance->Rsdp3);
    if (EFI_ERROR (Status)) {
      return EFI_ABORTED;
    }
  }

  return EFI_SUCCESS;
}

/**
 Installs an ACPI table into the RSDT/XSDT.

 @param [in]   This             Protocol instance pointer.
 @param [in]   AcpiTableBuffer  A pointer to a buffer containing the ACPI table to be installed.
 @param [in]   AcpiTableBufferSize  Specifies the size, in bytes, of the AcpiTableBuffer buffer.
 @param [out]  TableKey         Reurns a key to refer to the ACPI table.

 @retval EFI_SUCCESS            The table was successfully inserted.
 @retval EFI_INVALID_PARAMETER  Either AcpiTableBuffer is NULL, TableKey is NULL, or AcpiTableBufferSize
                                and the size field embedded in the ACPI table pointed to by AcpiTableBuffer
                                are not in sync.
 @retval EFI_OUT_OF_RESOURCES   Insufficient resources exist to complete the request.
 @retval EFI_ACCESS_DENIED      The table signature matches a table already
                                present in the system and platform policy
                                does not allow duplicate tables of this type.

**/
EFI_STATUS
EFIAPI
InstallAcpiTable (
  IN   EFI_ACPI_TABLE_PROTOCOL                    *This,
  IN   VOID                                       *AcpiTableBuffer,
  IN   UINTN                                      AcpiTableBufferSize,
  OUT  UINTN                                      *TableKey
  )
{
  EFI_ACPI_SUPPORT_INSTANCE *AcpiSupportInstance;
  EFI_ACPI_SUPPORT_PROTOCOL *AcpiSupport;
  EFI_STATUS                Status;
  VOID                      *AcpiTableBufferConst;
  EFI_ACPI_TABLE_VERSION    Version;

  //
  // Check for invalid input parameters
  //
  if ((AcpiTableBuffer == NULL) || (TableKey == NULL)
     || (((EFI_ACPI_DESCRIPTION_HEADER *) AcpiTableBuffer)->Length != AcpiTableBufferSize)) {
    return EFI_INVALID_PARAMETER;
  }

  Version = PcdGet32 (PcdAcpiExposedTableVersions);

  //
  // Get the instance of the ACPI table protocol
  //
  AcpiSupportInstance = EFI_ACPI_TABLE_INSTANCE_FROM_ACPI_SUPPORT_THIS (This);
  AcpiSupport = &AcpiSupportInstance->AcpiSupport;

  //
  // Install the ACPI table
  //
  AcpiTableBufferConst = AllocateCopyPool (AcpiTableBufferSize,AcpiTableBuffer);
  *TableKey = 0;
  Status = AddTableToList (
             AcpiSupportInstance,
             AcpiTableBufferConst,
             TRUE,
             Version,
             TableKey
             );
  if (!EFI_ERROR (Status)) {
    Status = PublishTables (
               AcpiSupport,
               Version
               );
  }
  FreePool (AcpiTableBufferConst);

  //
  // Add a new table successfully, notify registed callback
  //
  if (FeaturePcdGet (PcdInstallAcpiSdtProtocol)) {
    if (!EFI_ERROR (Status)) {
      SdtNotifyAcpiList (
        AcpiSupportInstance,
        Version,
        *TableKey
        );
    }
  }

  return Status;
}

/**
 Removes an ACPI table from the RSDT/XSDT.

 @param [in]   This             Protocol instance pointer.
 @param [in]   TableKey         Specifies the table to uninstall.  The key was returned from InstallAcpiTable().

 @retval EFI_SUCCESS            The table was successfully uninstalled.
 @retval EFI_NOT_FOUND          TableKey does not refer to a valid key for a table entry.

**/
EFI_STATUS
EFIAPI
UninstallAcpiTable (
  IN EFI_ACPI_TABLE_PROTOCOL                    *This,
  IN UINTN                                      TableKey
  )
{
  EFI_ACPI_SUPPORT_INSTANCE *AcpiSupportInstance;
  EFI_ACPI_SUPPORT_PROTOCOL *AcpiSupport;
  EFI_STATUS                Status;
  EFI_ACPI_TABLE_VERSION    Version;

  //
  // Get the instance of the ACPI support protocol
  //
  AcpiSupportInstance = EFI_ACPI_TABLE_INSTANCE_FROM_ACPI_SUPPORT_THIS (This);
  AcpiSupport = &AcpiSupportInstance->AcpiSupport;

  Version = PcdGet32 (PcdAcpiExposedTableVersions);

  //
  // Uninstall the ACPI table
  //
  Status = RemoveTableFromList (
             AcpiSupportInstance,
             Version,
             TableKey
             );
  if (!EFI_ERROR (Status)) {
    Status = PublishTables (
               AcpiSupport,
               Version
               );
  }

  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_SUCCESS;
  }
}


/**
 If the number of APCI tables exceeds the preallocated max table number,
 enlarge the table buffer.

 @param [in]   AcpiSupportInstance  ACPI support protocol instance data structure

 @retval EFI_SUCCESS            Reallocate the table beffer successfully.
 @retval EFI_OUT_OF_RESOURCES   Unable to allocate required resources.

**/
EFI_STATUS
ReallocateAcpiTableBuffer (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance
  )
{
  UINTN                      NewMaxTableNumber;
  UINTN                      TotalSize;
  UINT8                      *Pointer;
  EFI_PHYSICAL_ADDRESS       PageAddress;
  EFI_ACPI_SUPPORT_INSTANCE  TempPrivateData;
  EFI_STATUS                 Status;
  UINT64                     CurrentData;

  CopyMem (&TempPrivateData, AcpiSupportInstance, sizeof (EFI_ACPI_SUPPORT_INSTANCE));
  //
  // Enlarge the max table number from mEfiAcpiMaxNumTables to mEfiAcpiMaxNumTables + EFI_ACPI_MAX_NUM_TABLES
  //
  NewMaxTableNumber = mEfiAcpiMaxNumTables + EFI_ACPI_MAX_NUM_TABLES;
  //
  // Create RSDT, XSDT structures and allocate buffers.
  //
  TotalSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER) +         // for ACPI 2.0/3.0 XSDT
              NewMaxTableNumber * sizeof (UINT64);

  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    TotalSize += sizeof (EFI_ACPI_DESCRIPTION_HEADER) +      // for ACPI 1.0 RSDT
                 NewMaxTableNumber * sizeof (UINT32) +
                 sizeof (EFI_ACPI_DESCRIPTION_HEADER) +      // for ACPI 2.0/3.0 RSDT
                 NewMaxTableNumber * sizeof (UINT32);
  }

  //
  // Allocate memory in the lower 32 bit of address range for
  // compatibility with ACPI 1.0 OS.
  //
  // This is done because ACPI 1.0 pointers are 32 bit values.
  // ACPI 2.0 OS and all 64 bit OS must use the 64 bit ACPI table addresses.
  // There is no architectural reason these should be below 4GB, it is purely
  // for convenience of implementation that we force memory below 4GB.
  //
  PageAddress = 0xFFFFFFFF;
  Status = gBS->AllocatePages (
                  mAcpiTableAllocType,
                  EfiACPIReclaimMemory,
                  EFI_SIZE_TO_PAGES (TotalSize),
                  &PageAddress
                  );

  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  Pointer = (UINT8 *) (UINTN) PageAddress;
  ZeroMem (Pointer, TotalSize);

  AcpiSupportInstance->Rsdt1 = (EFI_ACPI_DESCRIPTION_HEADER *) Pointer;
  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    Pointer += (sizeof (EFI_ACPI_DESCRIPTION_HEADER) + NewMaxTableNumber * sizeof (UINT32));
    AcpiSupportInstance->Rsdt3 = (EFI_ACPI_DESCRIPTION_HEADER *) Pointer;
    Pointer += (sizeof (EFI_ACPI_DESCRIPTION_HEADER) + NewMaxTableNumber * sizeof (UINT32));
  }
  AcpiSupportInstance->Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *) Pointer;

  //
  // Update RSDP to point to the new Rsdt and Xsdt address.
  //
  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    AcpiSupportInstance->Rsdp1->RsdtAddress = (UINT32) (UINTN) AcpiSupportInstance->Rsdt1;
    AcpiSupportInstance->Rsdp3->RsdtAddress = (UINT32) (UINTN) AcpiSupportInstance->Rsdt3;
  }
  CurrentData = (UINT64) (UINTN) AcpiSupportInstance->Xsdt;
  CopyMem (&AcpiSupportInstance->Rsdp3->XsdtAddress, &CurrentData, sizeof (UINT64));

  //
  // copy the original Rsdt1, Rsdt3 and Xsdt structure to new buffer
  //
  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    CopyMem (AcpiSupportInstance->Rsdt1, TempPrivateData.Rsdt1, (sizeof (EFI_ACPI_DESCRIPTION_HEADER) + mEfiAcpiMaxNumTables * sizeof (UINT32)));
    CopyMem (AcpiSupportInstance->Rsdt3, TempPrivateData.Rsdt3, (sizeof (EFI_ACPI_DESCRIPTION_HEADER) + mEfiAcpiMaxNumTables * sizeof (UINT32)));
  }
  CopyMem (AcpiSupportInstance->Xsdt, TempPrivateData.Xsdt, (sizeof (EFI_ACPI_DESCRIPTION_HEADER) + mEfiAcpiMaxNumTables * sizeof (UINT64)));

  //
  // Calculate orignal ACPI table buffer size
  //
  TotalSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER) +         // for ACPI 2.0/3.0 XSDT
              mEfiAcpiMaxNumTables * sizeof (UINT64);

  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    TotalSize += sizeof (EFI_ACPI_DESCRIPTION_HEADER) +         // for ACPI 1.0 RSDT
                 mEfiAcpiMaxNumTables * sizeof (UINT32) +
                 sizeof (EFI_ACPI_DESCRIPTION_HEADER) +         // for ACPI 2.0/3.0 RSDT
                 mEfiAcpiMaxNumTables * sizeof (UINT32);
  }

  gBS->FreePages ((EFI_PHYSICAL_ADDRESS)(UINTN)TempPrivateData.Rsdt1, EFI_SIZE_TO_PAGES (TotalSize));

  //
  // Update the Max ACPI table number
  //
  mEfiAcpiMaxNumTables = NewMaxTableNumber;
  return EFI_SUCCESS;
}

/*
  Add current table entry to the Rsdt1 by version

  @param [in]   AcpiSupportInstance     Instance of the protocol.
  @param [in]   CurrentTableList        ACPI Table Linked List.
  @param [in]   Version                 Version to add Rsdt1.

  @retval EFI_SUCCESS                   The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES          Could not allocate a required resource.

*/
EFI_STATUS
AddToRsdt1 (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  EFI_ACPI_TABLE_LIST                     *CurrentTableList,
  IN EFI_ACPI_TABLE_VERSION               Version
  )
{
  EFI_STATUS                Status;
  UINT32                    *CurrentRsdtEntry;

  Status = EFI_SUCCESS;
  if ((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    //
    // If the table number exceed the gEfiAcpiMaxNumTables, enlarge the table buffer
    // And make sure the space of one table to reserve
    //
    if (AcpiSupportInstance->NumberOfTableEntries1 + 1 >= mEfiAcpiMaxNumTables) {
      Status = ReallocateAcpiTableBuffer (AcpiSupportInstance);
      ASSERT_EFI_ERROR (Status);
    }
    CurrentRsdtEntry = (UINT32 *)
      (
      (UINT8 *) AcpiSupportInstance->Rsdt1 +
      sizeof (EFI_ACPI_DESCRIPTION_HEADER) +
      AcpiSupportInstance->NumberOfTableEntries1 *
      sizeof (UINT32)
      );

    //
    // Add entry to the RSDT unless its the FACS or DSDT
    //
    *CurrentRsdtEntry = (UINT32) (UINTN) CurrentTableList->Table;

    //
    // Update RSDT length
    //
    AcpiSupportInstance->Rsdt1->Length = AcpiSupportInstance->Rsdt1->Length + sizeof (UINT32);

    AcpiSupportInstance->NumberOfTableEntries1++;
  }
  return Status;
}

/*
  Add current table entry to the Rsdt3/Xsdt by version

  @param [in]   AcpiSupportInstance     Instance of the protocol.
  @param [in]   CurrentTableList        ACPI Table Linked List.
  @param [in]   Version                 Version to add Rsdt3 Xsdt.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES   Could not allocate a required resource.

*/
EFI_STATUS
AddToRsdt3Xsdt (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  EFI_ACPI_TABLE_LIST                     *CurrentTableList,
  IN EFI_ACPI_TABLE_VERSION               Version
  )
{
  VOID                *CurrentXsdtEntry;
  UINT32              *CurrentRsdtEntry;
  UINT64              Buffer64;
  EFI_STATUS          Status;

  Status = EFI_SUCCESS;
  if ((Version & ACPI_TABLE_VERSION_GTE_2_0) != 0) {
    //
    // If the table number exceed the gEfiAcpiMaxNumTables, enlarge the table buffer
    // And make sure the space of one table to reserve
    //
    if (AcpiSupportInstance->NumberOfTableEntries3 + 1 >= mEfiAcpiMaxNumTables) {
      Status = ReallocateAcpiTableBuffer (AcpiSupportInstance);
      ASSERT_EFI_ERROR (Status);
    }

    if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
      //
      // At this time, it is assumed that RSDT and XSDT maintain parallel lists of tables.
      // If it becomes necessary to maintain separate table lists, changes will be required.
      //
      CurrentRsdtEntry = (UINT32 *)
        (
          (UINT8 *) AcpiSupportInstance->Rsdt3 +
           sizeof (EFI_ACPI_DESCRIPTION_HEADER) +
           AcpiSupportInstance->NumberOfTableEntries3 *
           sizeof (UINT32)
        );

      //
      // Add entry to the RSDT
      //
      *CurrentRsdtEntry = (UINT32) (UINTN) CurrentTableList->Table;

      //
      // Update RSDT length
      //
      AcpiSupportInstance->Rsdt3->Length = AcpiSupportInstance->Rsdt3->Length + sizeof (UINT32);
    }

    //
    // This pointer must not be directly dereferenced as the XSDT entries may not
    // be 64 bit aligned resulting in a possible fault.  Use CopyMem to update.
    //
    CurrentXsdtEntry = (VOID *)
      (
        (UINT8 *) AcpiSupportInstance->Xsdt +
        sizeof (EFI_ACPI_DESCRIPTION_HEADER) +
        AcpiSupportInstance->NumberOfTableEntries3 *
        sizeof (UINT64)
      );

    //
    // Add entry to XSDT, XSDT expects 64 bit pointers, but
    // the table pointers in XSDT are not aligned on 8 byte boundary.
    //
    Buffer64 = (UINT64) (UINTN) CurrentTableList->Table;
    CopyMem (
      CurrentXsdtEntry,
      &Buffer64,
      sizeof (UINT64)
      );
    //
    // Update length
    //
    AcpiSupportInstance->Xsdt->Length = AcpiSupportInstance->Xsdt->Length + sizeof (UINT64);

    AcpiSupportInstance->NumberOfTableEntries3++;
  }
  return Status;
}

/**
  This function add Dsdt table to the appropriate table version.

  @param [in]   AcpiSupportInstance     Instance of the protocol.
  @param [in]   CurrentTableList        ACPI Table Linked List
  @param [in]   Version                 Version to add the appropriate table.

**/
VOID
AddDsdtTable (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN EFI_ACPI_TABLE_LIST                  *CurrentTableList,
  IN EFI_ACPI_TABLE_VERSION               Version
  )
{
  UINT64                 Buffer64;

  //
  // Add the table to the appropriate table version
  //
  if ((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    //
    // Save a pointer to the table
    //
    AcpiSupportInstance->Dsdt1 = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTableList->Table;

    //
    // If FADT already exists, update table pointers.
    //
    if (AcpiSupportInstance->Fadt1 != NULL) {
      AcpiSupportInstance->Fadt1->Dsdt = (UINT32) (UINTN) AcpiSupportInstance->Dsdt1;

      //
      // Checksum FADT table
      //
      AcpiPlatformChecksum (
        AcpiSupportInstance->Fadt1,
        AcpiSupportInstance->Fadt1->Header.Length,
        OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
        Checksum)
        );
    }
  }

  if ((Version & ACPI_TABLE_VERSION_GTE_2_0) != 0) {
    //
    // Save a pointer to the table
    //
    AcpiSupportInstance->Dsdt3 = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTableList->Table;

    //
    // If FADT already exists, update table pointers.
    //
    if (AcpiSupportInstance->Fadt3 != NULL) {
      if ((UINT64)(UINTN)AcpiSupportInstance->Dsdt3 < 0x100000000LL) {
        AcpiSupportInstance->Fadt3->Dsdt = (UINT32) (UINTN) AcpiSupportInstance->Dsdt3;
        //
        // Comment block "the caller installs the tables in "FADT, DSDT" order"
        // The below comments are also in "the caller installs the tables in "DSDT, FADT" order" comment block.
        //
        // The ACPI specification, up to and including revision 5.1 Errata A,
        // allows the DSDT and X_DSDT fields to be both set in the FADT.
        // (Obviously, this only makes sense if the DSDT address is representable in 4 bytes.)
        // Starting with 5.1 Errata B, specifically for Mantis 1393 <https://mantis.uefi.org/mantis/view.php?id=1393>,
        // the spec requires at most one of DSDT and X_DSDT fields to be set to a nonzero value,
        // but strangely an exception is 6.0 that has no this requirement.
        //
        // Here we do not make the DSDT and X_DSDT fields mutual exclusion conditionally
        // by checking FADT revision, but always set both DSDT and X_DSDT fields in the FADT
        // to have better compatibility as some OS may have assumption to only consume X_DSDT
        // field even the DSDT address is < 4G.
        //
        Buffer64 = AcpiSupportInstance->Fadt3->Dsdt;
      } else {
        AcpiSupportInstance->Fadt3->Dsdt = 0;
        Buffer64 = (UINT64) (UINTN) AcpiSupportInstance->Dsdt3;
      }
      CopyMem (&AcpiSupportInstance->Fadt3->XDsdt, &Buffer64, sizeof (UINT64));

      //
      // Checksum FADT table
      //
      AcpiPlatformChecksum (
        AcpiSupportInstance->Fadt3,
        AcpiSupportInstance->Fadt3->Header.Length,
        OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
        Checksum)
      );
    }
  }
  return;
}

/**
  This function add Facs table to the appropriate table version.

  @param [in]   AcpiSupportInstance     Instance of the protocol.
  @param [in]   CurrentTableList        ACPI Table Linked List
  @param [in]   Version                 Version to add the appropriate table.

**/
VOID
AddFacsTable (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN EFI_ACPI_TABLE_LIST                  *CurrentTableList,
  IN EFI_ACPI_TABLE_VERSION               Version
  )
{
  UINT64              Buffer64;

  //
  // Add the table to the appropriate table version
  //
  if ((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    //
    // Save a pointer to the table
    //
    AcpiSupportInstance->Facs1 = (EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *) CurrentTableList->Table;

    //
    // If FADT already exists, update table pointers.
    //
    if (AcpiSupportInstance->Fadt1 != NULL) {
      AcpiSupportInstance->Fadt1->FirmwareCtrl = (UINT32) (UINTN) AcpiSupportInstance->Facs1;

      //
      // Checksum FADT table
      //
      AcpiPlatformChecksum (
        AcpiSupportInstance->Fadt1,
        AcpiSupportInstance->Fadt1->Header.Length,
        OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
        Checksum)
        );
    }
  }

  if ((Version & ACPI_TABLE_VERSION_GTE_2_0) != 0) {
    //
    // Save a pointer to the table
    //
    if ((Version & EFI_ACPI_TABLE_VERSION_4_0 || Version & EFI_ACPI_TABLE_VERSION_5_0 || Version & EFI_ACPI_TABLE_VERSION_6_0) != 0) {
      AcpiSupportInstance->Facs4 = (EFI_ACPI_4_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *) CurrentTableList->Table;
    } else {
      AcpiSupportInstance->Facs3 = (EFI_ACPI_3_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *) CurrentTableList->Table;
    }
    //
    // If FADT already exists, update table pointers.
    //
    if (AcpiSupportInstance->Fadt3) {
      if ((Version & EFI_ACPI_TABLE_VERSION_4_0 || Version & EFI_ACPI_TABLE_VERSION_5_0 || Version & EFI_ACPI_TABLE_VERSION_6_0) != 0) {
        if ((UINTN)AcpiSupportInstance->Facs4 < 0x100000000LL) {
          AcpiSupportInstance->Fadt3->FirmwareCtrl = (UINT32) (UINTN) AcpiSupportInstance->Facs4;
          Buffer64 = 0;
        } else {
          AcpiSupportInstance->Fadt3->FirmwareCtrl = 0;
          Buffer64 = (UINT64) (UINTN) AcpiSupportInstance->Facs4;
        }
      } else {
        AcpiSupportInstance->Fadt3->FirmwareCtrl  = (UINT32) (UINTN) AcpiSupportInstance->Facs3;
        Buffer64 = (UINT64) (UINTN) AcpiSupportInstance->Facs3;
      }

      CopyMem (
        &AcpiSupportInstance->Fadt3->XFirmwareCtrl,
        &Buffer64,
        sizeof (UINT64)
        );

      //
      // Checksum FADT table
      //
      AcpiPlatformChecksum (
        AcpiSupportInstance->Fadt3,
        AcpiSupportInstance->Fadt3->Header.Length,
        OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
        Checksum)
        );
    }
  }
  return;
}

/**
  This function add Fadt table to the appropriate table version.

  @param [in]   AcpiSupportInstance     Instance of the protocol.
  @param [in]   CurrentTableList        ACPI Table Linked List
  @param [in]   Version                 Version to add the appropriate table.

**/
VOID
AddFadtable (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN EFI_ACPI_TABLE_LIST                  *CurrentTableList,
  IN EFI_ACPI_TABLE_VERSION               Version
  )
{
  UINT64              Buffer64;

  //
  // Add the table to the appropriate table version
  //
  if ((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    //
    // Save a pointer to the table
    //
    AcpiSupportInstance->Fadt1 = (EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE *) CurrentTableList->Table;

    //
    // Update pointers in FADT.  If tables don't exist this will put NULL pointers there.
    //
    AcpiSupportInstance->Fadt1->FirmwareCtrl  = (UINT32) (UINTN) AcpiSupportInstance->Facs1;
    AcpiSupportInstance->Fadt1->Dsdt          = (UINT32) (UINTN) AcpiSupportInstance->Dsdt1;

    //
    // RSDP OEM information is updated to match the FADT OEM information
    //
    CopyMem (
      &AcpiSupportInstance->Rsdp1->OemId,
      &AcpiSupportInstance->Fadt1->Header.OemId,
      6
      );

    //
    // RSDT OEM information is updated to match the FADT OEM information.
    //
    CopyMem (
      &AcpiSupportInstance->Rsdt1->OemId,
      &AcpiSupportInstance->Fadt1->Header.OemId,
      6
      );

    CopyMem (
      &AcpiSupportInstance->Rsdt1->OemTableId,
      &AcpiSupportInstance->Fadt1->Header.OemTableId,
      sizeof (UINT64)
      );
    AcpiSupportInstance->Rsdt1->OemRevision = AcpiSupportInstance->Fadt1->Header.OemRevision;
  }

  if ((Version & ACPI_TABLE_VERSION_GTE_2_0) != 0) {
    //
    // Save a pointer to the table
    //
    AcpiSupportInstance->Fadt3 = (EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *) CurrentTableList->Table;

    //
    // Update pointers in FADT.  If tables don't exist this will put NULL pointers there.
    // Note: If the FIRMWARE_CTRL is non-zero, then X_FIRMWARE_CTRL must be zero, and
    // vice-versa.
    //
    if ((Version & EFI_ACPI_TABLE_VERSION_4_0 || Version & EFI_ACPI_TABLE_VERSION_5_0 || Version & EFI_ACPI_TABLE_VERSION_6_0) != 0){
      if ((UINT64)(UINTN)AcpiSupportInstance->Facs4 < 0x100000000LL) {
        AcpiSupportInstance->Fadt3->FirmwareCtrl  = (UINT32) (UINTN) AcpiSupportInstance->Facs4;
        Buffer64 = 0;
      } else {
        AcpiSupportInstance->Fadt3->FirmwareCtrl = 0;
        Buffer64 = (UINT64) (UINTN) AcpiSupportInstance->Facs4;
      }
    } else {
      AcpiSupportInstance->Fadt3->FirmwareCtrl = (UINT32) (UINTN) AcpiSupportInstance->Facs3;
      Buffer64 = (UINT64) (UINTN) AcpiSupportInstance->Facs3;
    }
    CopyMem (
      &AcpiSupportInstance->Fadt3->XFirmwareCtrl,
      &Buffer64,
      sizeof (UINT64)
      );

    if ((UINT64)(UINTN)AcpiSupportInstance->Dsdt3 < 0x100000000LL) {
      AcpiSupportInstance->Fadt3->Dsdt = (UINT32) (UINTN) AcpiSupportInstance->Dsdt3;
      //
      // Comment block "the caller installs the tables in "DSDT, FADT" order"
      // The below comments are also in "the caller installs the tables in "FADT, DSDT" order" comment block.
      //
      // The ACPI specification, up to and including revision 5.1 Errata A,
      // allows the DSDT and X_DSDT fields to be both set in the FADT.
      // (Obviously, this only makes sense if the DSDT address is representable in 4 bytes.)
      // Starting with 5.1 Errata B, specifically for Mantis 1393 <https://mantis.uefi.org/mantis/view.php?id=1393>,
      // the spec requires at most one of DSDT and X_DSDT fields to be set to a nonzero value,
      // but strangely an exception is 6.0 that has no this requirement.
      //
      // Here we do not make the DSDT and X_DSDT fields mutual exclusion conditionally
      // by checking FADT revision, but always set both DSDT and X_DSDT fields in the FADT
      // to have better compatibility as some OS may have assumption to only consume X_DSDT
      // field even the DSDT address is < 4G.
      //
      Buffer64 = AcpiSupportInstance->Fadt3->Dsdt;
    } else {
      AcpiSupportInstance->Fadt3->Dsdt = 0;
      Buffer64 = (UINT64) (UINTN) AcpiSupportInstance->Dsdt3;
    }
    CopyMem (&AcpiSupportInstance->Fadt3->XDsdt, &Buffer64, sizeof (UINT64));

    //
    // RSDP OEM information is updated to match the FADT OEM information
    //
    CopyMem (
      &AcpiSupportInstance->Rsdp3->OemId,
      &AcpiSupportInstance->Fadt3->Header.OemId,
      6
      );

    if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
      //
      // RSDT OEM information is updated to match FADT OEM information.
      //
      CopyMem (
        &AcpiSupportInstance->Rsdt3->OemId,
        &AcpiSupportInstance->Fadt3->Header.OemId,
        6
        );
      CopyMem (
        &AcpiSupportInstance->Rsdt3->OemTableId,
        &AcpiSupportInstance->Fadt3->Header.OemTableId,
        sizeof (UINT64)
        );
     AcpiSupportInstance->Rsdt3->OemRevision = AcpiSupportInstance->Fadt3->Header.OemRevision;
    }

    //
    // XSDT OEM information is updated to match FADT OEM information.
    //
    CopyMem (
      &AcpiSupportInstance->Xsdt->OemId,
      &AcpiSupportInstance->Fadt3->Header.OemId,
      6
      );
    CopyMem (
      &AcpiSupportInstance->Xsdt->OemTableId,
      &AcpiSupportInstance->Fadt3->Header.OemTableId,
      sizeof (UINT64)
      );
    AcpiSupportInstance->Xsdt->OemRevision = AcpiSupportInstance->Fadt3->Header.OemRevision;
    if (Version & EFI_ACPI_TABLE_VERSION_5_0) {
      AcpiSupportInstance->Fadt5 = (EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)AcpiSupportInstance->Fadt3;
    }
    if (Version & EFI_ACPI_TABLE_VERSION_6_0) {
      AcpiSupportInstance->Fadt6 = (EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE *)AcpiSupportInstance->Fadt3;
    }
  }

  return;
}

/**
  This function adds an ACPI table to the table list.  It will detect FACS and
  allocate the correct type of memory and properly align the table.

 @param [in]   AcpiSupportInstance  Instance of the protocol.
 @param [in]   Table            Table to add.
 @param [in]   Checksum         Does the table require checksumming.
 @param [in]   Version          The version of the list to add the table to.
 @param [out]  Handle           Pointer for returning the handle.

 @retval EFI_SUCCESS            The function completed successfully.
 @retval EFI_OUT_OF_RESOURCES   Could not allocate a required resource.
 @retval EFI_ACCESS_DENIED      The table signature matches a table already
                                present in the system and platform policy
                                does not allow duplicate tables of this type.

**/
EFI_STATUS
AddTableToList (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN VOID                                 *Table,
  IN BOOLEAN                              Checksum,
  IN EFI_ACPI_TABLE_VERSION               Version,
  OUT UINTN                               *Handle
  )
{
  EFI_STATUS          Status;
  EFI_ACPI_TABLE_LIST *CurrentTableList;
  UINT32              CurrentTableSignature;
  UINT32              CurrentTableSize;
  BOOLEAN             AddToRsdt;

  if (AcpiSupportInstance == NULL || Table == NULL || Handle == NULL) {
    //
    // Check for invalid input parameters
    //
    ASSERT (AcpiSupportInstance);
    ASSERT (Table);
    ASSERT (Handle);
    return EFI_INVALID_PARAMETER;
  }

  //
  // Init locals
  //
  AddToRsdt = TRUE;

  //
  // Create a new list entry
  //
  CurrentTableList = AllocatePool (sizeof (EFI_ACPI_TABLE_LIST));
  if (CurrentTableList == NULL) {
    ASSERT (CurrentTableList);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Determine table type and size
  //
  CurrentTableSignature = ((EFI_ACPI_COMMON_HEADER *) Table)->Signature;
  CurrentTableSize      = ((EFI_ACPI_COMMON_HEADER *) Table)->Length;

  //
  // Allocate a buffer for the table.  All tables are allocated in the lower 32 bits of address space
  // for backwards compatibility with ACPI 1.0 OS.
  //
  // This is done because ACPI 1.0 pointers are 32 bit values.
  // ACPI 2.0 OS and all 64 bit OS must use the 64 bit ACPI table addresses.
  // There is no architectural reason these should be below 4GB, it is purely
  // for convenience of implementation that we force memory below 4GB.
  //
  CurrentTableList->PageAddress   = 0xFFFFFFFF;
  CurrentTableList->NumberOfPages = EFI_SIZE_TO_PAGES (CurrentTableSize);

  //
  // Allocation memory type depends on the type of the table
  //
  if ((CurrentTableSignature == EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) ||
      (CurrentTableSignature == EFI_ACPI_4_0_UEFI_ACPI_DATA_TABLE_SIGNATURE)) {
    //
    // Allocate memory for the FACS.  This structure must be aligned
    // on a 64 byte boundary and must be ACPI NVS memory.
    // Using AllocatePages should ensure that it is always aligned.
    // Do not change signature for new ACPI version because they are same.
    //
    // UEFI table also need to be in ACPI NVS memory, because some data field
    // could be updated by OS present agent. For example, BufferPtrAddress in
    // SMM communication ACPI table.
    //
    ASSERT ((EFI_PAGE_SIZE % 64) == 0);
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiACPIMemoryNVS,
                    CurrentTableList->NumberOfPages,
                    &CurrentTableList->PageAddress
                    );
  } else {
    //
    // All other tables are ACPI reclaim memory, no alignment requirements.
    //
    Status = gBS->AllocatePages (
                    mAcpiTableAllocType,
                    EfiACPIReclaimMemory,
                    CurrentTableList->NumberOfPages,
                    &CurrentTableList->PageAddress
                    );
  }
  //
  // Check return value from memory alloc.
  //
  if (EFI_ERROR (Status)) {
    FreePool (CurrentTableList);
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Update the table pointer with the allocated memory start
  //
  CurrentTableList->Table = (EFI_ACPI_COMMON_HEADER *) (UINTN) CurrentTableList->PageAddress;

  //
  // Initialize the table contents
  //
  CurrentTableList->Signature = EFI_ACPI_TABLE_LIST_SIGNATURE;
  CopyMem (CurrentTableList->Table, Table, CurrentTableSize);
  CurrentTableList->Handle  = AcpiSupportInstance->CurrentHandle++;
  *Handle                   = CurrentTableList->Handle;
  CurrentTableList->Version = Version;

  //
  // Update internal pointers if this is a required table.  If it is a required
  // table and a table of that type already exists, return an error.
  //
  // Calculate the checksum if the table is not FACS.
  //
  switch (CurrentTableSignature) {

  case EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
    //
    // We don't add the FADT in the standard way because some
    // OS expect the FADT to be early in the table list.
    // So we always add it as the first element in the list.
    //
    AddToRsdt = FALSE;

    //
    // Check that the table has not been previously added.
    //
    if (((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0 && AcpiSupportInstance->Fadt1 != NULL) ||
        ((Version & EFI_ACPI_TABLE_VERSION_2_0)  != 0 && AcpiSupportInstance->Fadt3 != NULL) ||
        ((Version & EFI_ACPI_TABLE_VERSION_3_0)  != 0 && AcpiSupportInstance->Fadt3 != NULL) ||
        ((Version & EFI_ACPI_TABLE_VERSION_4_0)  != 0 && AcpiSupportInstance->Fadt3 != NULL) ||
        ((Version & EFI_ACPI_TABLE_VERSION_5_0)  != 0 && AcpiSupportInstance->Fadt5 != NULL) ||
        ((Version & EFI_ACPI_TABLE_VERSION_6_0)  != 0 && AcpiSupportInstance->Fadt6 != NULL)
        ) {
      gBS->FreePages (CurrentTableList->PageAddress, CurrentTableList->NumberOfPages);
      FreePool (CurrentTableList);
      return EFI_ACCESS_DENIED;
    }
    //
    // Add the table to the appropriate table version
    //
    AddFadtable (AcpiSupportInstance, CurrentTableList, Version);
    //
    // Checksum the table
    //
    if (Checksum) {
      AcpiPlatformChecksum (
        CurrentTableList->Table,
        CurrentTableList->Table->Length,
        OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
        Checksum)
        );
    }
    break;

  case EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE:
    //
    // Check that the table has not been previously added.
    //
    if (((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0 && AcpiSupportInstance->Facs1 != NULL) ||
       ((Version & EFI_ACPI_TABLE_VERSION_2_0)   != 0 && AcpiSupportInstance->Facs3 != NULL)  ||
       ((Version & EFI_ACPI_TABLE_VERSION_3_0)   != 0 && AcpiSupportInstance->Facs3 != NULL)  ||
       ((Version & EFI_ACPI_TABLE_VERSION_4_0)   != 0 && AcpiSupportInstance->Facs4 != NULL)  ||
       ((Version & EFI_ACPI_TABLE_VERSION_5_0)   != 0 && AcpiSupportInstance->Facs4 != NULL)  ||
       ((Version & EFI_ACPI_TABLE_VERSION_6_0)   != 0 && AcpiSupportInstance->Facs4 != NULL)
       ) {
      gBS->FreePages (CurrentTableList->PageAddress, CurrentTableList->NumberOfPages);
      FreePool (CurrentTableList);
      return EFI_ACCESS_DENIED;
    }
    //
    // FACS is referenced by FADT and is not part of RSDT
    //
    AddToRsdt = FALSE;

    //
    // Add the table to the appropriate table version
    //
    AddFacsTable (AcpiSupportInstance, CurrentTableList, Version);
    break;

  case EFI_ACPI_1_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    //
    // Check that the table has not been previously added.
    //
    if (((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0 && AcpiSupportInstance->Dsdt1 != NULL) ||
        ((Version & ACPI_TABLE_VERSION_GTE_2_0)  != 0 && AcpiSupportInstance->Dsdt3 != NULL)
        ) {
      gBS->FreePages (CurrentTableList->PageAddress, CurrentTableList->NumberOfPages);
      FreePool (CurrentTableList);
      return EFI_ACCESS_DENIED;
    }
    //
    // DSDT is referenced by FADT and is not part of RSDT
    //
    AddToRsdt = FALSE;

    //
    // Add the table to the appropriate table version
    //
    AddDsdtTable (AcpiSupportInstance, CurrentTableList, Version);
    //
    // Checksum the table
    //
    if (Checksum) {
      AcpiPlatformChecksum (
        CurrentTableList->Table,
        CurrentTableList->Table->Length,
        OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
        Checksum)
        );
    }
    break;

  default:
    //
    // Checksum the table
    //
    if (Checksum) {
      AcpiPlatformChecksum (
        CurrentTableList->Table,
        CurrentTableList->Table->Length,
        OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
        Checksum)
        );
    }
    break;
  }
  //
  // Add the table to the current list of tables
  //
  InsertTailList (&AcpiSupportInstance->TableList, &CurrentTableList->Link);

  //
  // Add the table to RSDT and/or XSDT table entry lists.
  //
  if (AddToRsdt) {
    //
    // Add to ACPI 1.0b table tree
    //
    AddToRsdt1 (AcpiSupportInstance, CurrentTableList, Version);
    //
    // Add to ACPI 2.0/3.0 table tree
    //
    AddToRsdt3Xsdt (AcpiSupportInstance, CurrentTableList, Version);
  }

  ChecksumCommonTables (AcpiSupportInstance);
  return EFI_SUCCESS;
}

/**
 This function finds the table specified by the handle and returns a pointer to it.
 If the handle is not found, EFI_NOT_FOUND is returned and the contents of Table are
 undefined.

 @param [in]   Handle           Table to find.
 @param [in]   TableList        Table list to search
 @param [out]  Table            Pointer to table found.

 @retval EFI_SUCCESS            The function completed successfully.
 @retval EFI_NOT_FOUND          No table found matching the handle specified.

**/
EFI_STATUS
FindTableByHandle (
  IN UINTN                                Handle,
  IN LIST_ENTRY                           *TableList,
  OUT EFI_ACPI_TABLE_LIST                 **Table
  )
{
  LIST_ENTRY          *CurrentLink;
  EFI_ACPI_TABLE_LIST *CurrentTable;

  //
  // Check for invalid input parameters
  //
  ASSERT (Table);

  //
  // Find the table
  //
  CurrentLink = TableList->ForwardLink;

  while (CurrentLink != TableList) {
    CurrentTable = EFI_ACPI_TABLE_LIST_FROM_LINK (CurrentLink);
    if (CurrentTable->Handle == Handle) {
      //
      // Found handle, so return this table.
      //
      *Table = CurrentTable;
      return EFI_SUCCESS;
    }

    CurrentLink = CurrentLink->ForwardLink;
  }
  //
  // Table not found
  //
  return EFI_NOT_FOUND;
}

/**
 This function removes a basic table from the RSDT and/or XSDT.
 For Acpi 1.0 tables, pass in the Rsdt.
 For Acpi 2.0 tables, pass in both Rsdt and Xsdt.

 @param [in, out] Table         Pointer to table found.
 @param [in, out] NumberOfTableEntries  Current number of table entries in the RSDT/XSDT
 @param [in, out] Rsdt          Pointer to the RSDT to  remove from
 @param [in, out] Xsdt          Pointer to the Xsdt to remove from

 @retval EFI_SUCCESS            The function completed successfully.
 @retval EFI_INVALID_PARAMETER  The table was not found in both Rsdt and Xsdt.

**/
EFI_STATUS
RemoveTableFromRsdt (
  IN OUT EFI_ACPI_TABLE_LIST              * Table,
  IN OUT UINTN                            *NumberOfTableEntries,
  IN OUT EFI_ACPI_DESCRIPTION_HEADER      * Rsdt OPTIONAL,
  IN OUT EFI_ACPI_DESCRIPTION_HEADER      * Xsdt OPTIONAL
  )
{
  UINT32  *CurrentRsdtEntry;
  VOID    *CurrentXsdtEntry;
  UINT64  CurrentTablePointer64;
  UINTN   i;

  //
  // Check for invalid input parameters
  //
  ASSERT (Table);
  ASSERT (NumberOfTableEntries);
  ASSERT (Rsdt || Xsdt);

  //
  // Find the table entry in the RSDT and XSDT
  //
  for (i = 0; i < *NumberOfTableEntries; i++) {
    //
    // At this time, it is assumed that RSDT and XSDT maintain parallel lists of tables.
    // If it becomes necessary to maintain separate table lists, changes will be required.
    //
    if (Rsdt != NULL) {
      CurrentRsdtEntry = (UINT32 *) ((UINT8 *) Rsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER) + i * sizeof (UINT32));
    } else {
      CurrentRsdtEntry = NULL;
    }
    if (Xsdt != NULL) {
      //
      // This pointer must not be directly dereferenced as the XSDT entries may not
      // be 64 bit aligned resulting in a possible fault.  Use CopyMem to update.
      //
      CurrentXsdtEntry = (VOID *) ((UINT8 *) Xsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER) + i * sizeof (UINT64));

      //
      // Read the entry value out of the XSDT
      //
      CopyMem (&CurrentTablePointer64, CurrentXsdtEntry, sizeof (UINT64));
    } else {
      //
      // Initialize to NULL
      //
      CurrentXsdtEntry      = 0;
      CurrentTablePointer64 = 0;
    }
    //
    // Check if we have found the corresponding entry in both RSDT and XSDT
    //
    if (((Rsdt == NULL) || *CurrentRsdtEntry == (UINT32) (UINTN) Table->Table) &&
        ((Xsdt == NULL) || CurrentTablePointer64 == (UINT64) (UINTN) Table->Table)
        ) {
      //
      // Found entry, so copy all following entries and shrink table
      // We actually copy all + 1 to copy the initialized value of memory over
      // the last entry.
      //
      if (Rsdt != NULL) {
        CopyMem (CurrentRsdtEntry, CurrentRsdtEntry + 1, (*NumberOfTableEntries - i) * sizeof (UINT32));
        Rsdt->Length = Rsdt->Length - sizeof (UINT32);
      }
      if (Xsdt != NULL) {
        CopyMem (CurrentXsdtEntry, ((UINT64 *) CurrentXsdtEntry) + 1, (*NumberOfTableEntries - i) * sizeof (UINT64));
        Xsdt->Length = Xsdt->Length - sizeof (UINT64);
      }
      break;
    } else if (i + 1 == *NumberOfTableEntries) {
      //
      // At the last entry, and table not found
      //
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // Checksum the tables
  //
  if (Rsdt != NULL) {
    AcpiPlatformChecksum (
      Rsdt,
      Rsdt->Length,
      OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
      Checksum)
      );
  }

  if (Xsdt != NULL) {
    AcpiPlatformChecksum (
      Xsdt,
      Xsdt->Length,
      OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
      Checksum)
      );
  }
  //
  // Decrement the number of tables
  //
  (*NumberOfTableEntries)--;

  return EFI_SUCCESS;
}

/**
 This function removes a table and frees any associated memory.

 @param [in]   AcpiSupportInstance  Instance of the protocol.
 @param [in]   Version          Version(s) to delete.
 @param [in, out] Table         Pointer to table found.

 @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
DeleteTable (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN EFI_ACPI_TABLE_VERSION               Version,
  IN OUT EFI_ACPI_TABLE_LIST              *Table
  )
{
  UINT32  CurrentTableSignature;
  BOOLEAN RemoveFromRsdt;

  if (AcpiSupportInstance == NULL || Table == NULL) {
    //
    // Check for invalid input parameters
    //
    ASSERT (AcpiSupportInstance);
    ASSERT (Table);
    return EFI_INVALID_PARAMETER;
  }
  //
  // Init locals
  //
  RemoveFromRsdt        = TRUE;

  if (Table->Table != NULL) {

    CurrentTableSignature = ((EFI_ACPI_COMMON_HEADER *) Table->Table)->Signature;

    //
    // Basic tasks to accomplish delete are:
    //   Determine removal requirements (in RSDT/XSDT or not)
    //   Remove entry from RSDT/XSDT
    //   Remove any table references to the table
    //   If no one is using the table
    //      Free the table (removing pointers from private data and tables)
    //      Remove from list
    //      Free list structure
    //
    //
    // Determine if this table is in the RSDT or XSDT
    //
    if ((CurrentTableSignature == EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) ||
        (CurrentTableSignature == EFI_ACPI_2_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE)
        ) {
      RemoveFromRsdt = FALSE;
    }
    //
    // We don't remove the FADT in the standard way because some
    // OS expect the FADT to be early in the table list.
    // So we always put it as the first element in the list.
    //
    if (CurrentTableSignature == EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
      RemoveFromRsdt = FALSE;
    }

    //
    // Remove the table from RSDT and XSDT
    // This is a basic table, remove it from any lists and the Rsdt and/or Xsdt
    //
    if (Version & EFI_ACPI_TABLE_VERSION_NONE & Table->Version) {
      //
      // Remove this version from the table
      //
      Table->Version = Table->Version &~EFI_ACPI_TABLE_VERSION_NONE;
    }

    if (Version & EFI_ACPI_TABLE_VERSION_1_0B & Table->Version) {
      //
      // Remove this version from the table
      //
      Table->Version = Table->Version &~EFI_ACPI_TABLE_VERSION_1_0B;

      //
      // Remove from Rsdt.  We don't care about the return value because it is
      // acceptable for the table to not exist in Rsdt.
      // We didn't add some tables so we don't remove them.
      //
      if (RemoveFromRsdt) {
        RemoveTableFromRsdt (
          Table,
          &AcpiSupportInstance->NumberOfTableEntries1,
          AcpiSupportInstance->Rsdt1,
          NULL
          );
      }
    }

    if (Version & ACPI_TABLE_VERSION_GTE_2_0 & Table->Version) {
      //
      // Remove this version from the table
      //
      Table->Version = Table->Version &~(Version & ACPI_TABLE_VERSION_GTE_2_0);
      //
      // Remove from Rsdt and Xsdt.  We don't care about the return value
      // because it is acceptable for the table to not exist in Rsdt/Xsdt.
      // We didn't add some tables so we don't remove them.
      //
      if (RemoveFromRsdt) {
        RemoveTableFromRsdt (
          Table,
          &AcpiSupportInstance->NumberOfTableEntries3,
          AcpiSupportInstance->Rsdt3,
          AcpiSupportInstance->Xsdt
          );
      }
    }
    //
    // Free the table, clean up any dependent tables and our private data pointers.
    //
    switch (Table->Table->Signature) {

    case EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
      if ((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
        AcpiSupportInstance->Fadt1 = NULL;
      }

     if ((Version & ACPI_TABLE_VERSION_GTE_2_0) != 0) {
        AcpiSupportInstance->Fadt3 = NULL;
      }

      if ((Version & EFI_ACPI_TABLE_VERSION_5_0) != 0) {
        AcpiSupportInstance->Fadt5 = NULL;
      }

      if ((Version & EFI_ACPI_TABLE_VERSION_6_0) != 0) {
        AcpiSupportInstance->Fadt6 = NULL;
      }

      break;

    case EFI_ACPI_3_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE:
      if ((Version & EFI_ACPI_TABLE_VERSION_1_0B) !=0) {
        AcpiSupportInstance->Facs1 = NULL;

        //
        // Update FADT table pointers
        //
        if (AcpiSupportInstance->Fadt1) {
          AcpiSupportInstance->Fadt1->FirmwareCtrl = 0;

          //
          // Checksum table
          //
          AcpiPlatformChecksum (
            AcpiSupportInstance->Fadt1,
            AcpiSupportInstance->Fadt1->Header.Length,
            OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
            Checksum)
            );
        }
      }

      if ((Version & ACPI_TABLE_VERSION_GTE_2_0) != 0) {
        if ((Version & EFI_ACPI_TABLE_VERSION_4_0 || Version & EFI_ACPI_TABLE_VERSION_5_0 || Version & EFI_ACPI_TABLE_VERSION_6_0) != 0) {
          AcpiSupportInstance->Facs4 = NULL;
        } else {
          AcpiSupportInstance->Facs3 = NULL;
        }
        //
        // Update FADT table pointers
        //
        if (AcpiSupportInstance->Fadt3) {
          AcpiSupportInstance->Fadt3->FirmwareCtrl = 0;
          ZeroMem (&AcpiSupportInstance->Fadt3->XFirmwareCtrl, sizeof (UINT64));

          //
          // Checksum table
          //
          AcpiPlatformChecksum (
            AcpiSupportInstance->Fadt3,
            AcpiSupportInstance->Fadt3->Header.Length,
            OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
            Checksum)
            );
        }
      }
      break;

    case EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
      if ((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
        AcpiSupportInstance->Dsdt1 = NULL;

        //
        // Update FADT table pointers
        //
        if (AcpiSupportInstance->Fadt1 != NULL) {
          AcpiSupportInstance->Fadt1->Dsdt = 0;

          //
          // Checksum table
          //
          AcpiPlatformChecksum (
            AcpiSupportInstance->Fadt1,
            AcpiSupportInstance->Fadt1->Header.Length,
            OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
            Checksum)
            );
        }
      }


      if ((Version & ACPI_TABLE_VERSION_GTE_2_0) != 0) {
        AcpiSupportInstance->Dsdt3 = NULL;

        //
        // Update FADT table pointers
        //
        if (AcpiSupportInstance->Fadt3) {
          AcpiSupportInstance->Fadt3->Dsdt = 0;
          ZeroMem (&AcpiSupportInstance->Fadt3->XDsdt, sizeof (UINT64));

          //
          // Checksum table
          //
          AcpiPlatformChecksum (
            AcpiSupportInstance->Fadt3,
            AcpiSupportInstance->Fadt3->Header.Length,
            OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
            Checksum)
            );
        }
      }
      break;

    default:
      //
      // Do nothing
      //
      break;
    }
  }
  //
  // If no version is using this table anymore, remove and free list entry.
  //
  if (Table->Version == 0) {
    //
    // Free the Table
    //
    gBS->FreePages (Table->PageAddress, Table->NumberOfPages);
    RemoveEntryList (&(Table->Link));
    FreePool (Table);
  }
  //
  // Done
  //
  return EFI_SUCCESS;
}

/**
 This function finds and removes the table specified by the handle.

 @param [in]   AcpiSupportInstance  Instance of the protocol.
 @param [in]   Version          Bitmask of which versions to remove.
 @param [in]   Handle           Table to remove.

 @retval EFI_SUCCESS            The function completed successfully.
 @retval EFI_ABORTED            An error occurred.
 @retval EFI_NOT_FOUND          Handle not found in table list.

**/
EFI_STATUS
RemoveTableFromList (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN EFI_ACPI_TABLE_VERSION               Version,
  IN UINTN                                Handle
  )
{
  EFI_ACPI_TABLE_LIST *Table;
  EFI_STATUS          Status;

  Table = (EFI_ACPI_TABLE_LIST*) NULL;

  //
  // Check for invalid input parameters
  //
  ASSERT (AcpiSupportInstance);

  //
  // Find the table
  //
  Status = FindTableByHandle (
             Handle,
             &AcpiSupportInstance->TableList,
             &Table
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  //
  // Remove the table
  //
  Status = DeleteTable (AcpiSupportInstance, Version, Table);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }
  //
  // Completed successfully
  //
  return EFI_SUCCESS;
}

/**
  This function update ACPI table to the current table address
  with the appropriate table version under specified conditions.

  @param [in]   AcpiSupportInstance     Instance of the protocol.
  @param [in]   CurrentTableList        ACPI Table Linked List
  @param [in]   Checksum                Does the table require checksumming.
  @param [in]   Version                 Version to add the appropriate table.
  @param [in]   Handle                  Table to update.

  @retval EFI_SUCCESS                   The function completed successfully.
  @retval EFI_INVALID_PARAMETER         Invalid parameter passed in.
  @retval EFI_ABORTED                   An error occurred.

**/
EFI_STATUS
UpdateTableList (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN VOID                                 *Table,
  IN BOOLEAN                              Checksum,
  IN EFI_ACPI_TABLE_VERSION               Version,
  IN UINTN                                Handle
  )
{
  EFI_STATUS              Status;
  EFI_ACPI_TABLE_LIST     *TableList;
  UINT32                  TableSignature;
  UINT32                  CurrentTableSize;
  BOOLEAN                 AddToRsdt;
  EFI_ACPI_TABLE_VERSION  VersionDel;

  if (AcpiSupportInstance == NULL || Table == NULL) {
    ASSERT (AcpiSupportInstance);
    ASSERT (Table);
    return EFI_INVALID_PARAMETER;
  }

  //
  // Find the table
  //
  TableList = (EFI_ACPI_TABLE_LIST*) NULL;
  Status = FindTableByHandle (
             Handle,
             &AcpiSupportInstance->TableList,
             &TableList
             );
  if (EFI_ERROR (Status) || TableList == NULL) {
    return EFI_ABORTED;
  }

  //
  // Whether new table size is small than the original page size allocated
  //
  CurrentTableSize = ((EFI_ACPI_COMMON_HEADER *) Table)->Length;
  if (EFI_SIZE_TO_PAGES (CurrentTableSize) > TableList->NumberOfPages) {
    return EFI_ABORTED;
  }

  CopyMem (TableList->Table, Table, CurrentTableSize);
  VersionDel = TableList->Version &~Version;
  if (VersionDel != 0) {
    Status = DeleteTable (AcpiSupportInstance, VersionDel, TableList);
    if (!EFI_ERROR (Status) && TableList->Version == 0) {
      //
      // Table list doesn't exist
      //
      return EFI_SUCCESS;
    }
  }

  AddToRsdt = TRUE;
  TableSignature = ((EFI_ACPI_COMMON_HEADER *) Table)->Signature;
  switch (TableSignature) {
  case EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
    //
    // Need additional processing for FADT entry of RSDT/XSDT
    //
    AddToRsdt = FALSE;
    AddFadtable (AcpiSupportInstance, TableList, Version);
    break;

  case EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE:
    AddToRsdt = FALSE;
    AddFacsTable (AcpiSupportInstance, TableList, Version);
    break;

  case EFI_ACPI_1_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    AddToRsdt = FALSE;
    AddDsdtTable (AcpiSupportInstance, TableList, Version);
    break;

  default:
    break;
  }

  if (Checksum) {
    AcpiPlatformChecksum (
      TableList->Table,
      TableList->Table->Length,
      OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
      Checksum)
      );
  }

  if (TableSignature == EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
    //
    // Synchronize FADT table into the first entry of RSDT/XSDT
    //
    SyncFadtToRsdtXsdt (AcpiSupportInstance, Version);
  }

  if (AddToRsdt) {
    if (((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0) && (TableList->Version & EFI_ACPI_TABLE_VERSION_1_0B) == 0) {
      AddToRsdt1 (AcpiSupportInstance, TableList, Version);
    }
    if (((Version & ACPI_TABLE_VERSION_GTE_2_0) != 0) && (TableList->Version & ACPI_TABLE_VERSION_GTE_2_0) == 0) {
      AddToRsdt3Xsdt (AcpiSupportInstance, TableList, Version);
    }
  }

  ChecksumCommonTables (AcpiSupportInstance);

  return EFI_SUCCESS;
}

/**
  This function add FADT table into the first entry of RSDT/XSDT.

  @param [in]   AcpiSupportInstance  Instance of the protocol.
  @param [in]   Version              Version to sync to RSDT/XSDT.

**/
VOID
SyncFadtToRsdtXsdt (
  IN EFI_ACPI_SUPPORT_INSTANCE            *AcpiSupportInstance,
  IN EFI_ACPI_TABLE_VERSION               Version
  )
{
  UINT32                    *CurrentRsdtEntry;
  VOID                      *CurrentXsdtEntry;
  UINT64                    Buffer64;

  if ((Version & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    //
    // Add FADT as the first entry
    //
    CurrentRsdtEntry  = (UINT32 *) ((UINT8 *) AcpiSupportInstance->Rsdt1 + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
    *CurrentRsdtEntry = (UINT32) (UINTN) AcpiSupportInstance->Fadt1;
  }

  if ((Version & ACPI_TABLE_VERSION_GTE_2_0) != 0) {
    CurrentRsdtEntry  = (UINT32 *) ((UINT8 *) AcpiSupportInstance->Rsdt3 + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
    *CurrentRsdtEntry = (UINT32) (UINTN) AcpiSupportInstance->Fadt3;
    CurrentXsdtEntry  = (VOID *) ((UINT8 *) AcpiSupportInstance->Xsdt + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
    //
    // Add entry to XSDT, XSDT expects 64 bit pointers, but
    // the table pointers in XSDT are not aligned on 8 byte boundary.
    //
    Buffer64 = (UINT64) (UINTN) AcpiSupportInstance->Fadt3;
    CopyMem (
      CurrentXsdtEntry,
      &Buffer64,
      sizeof (UINT64)
      );
  }

  ChecksumCommonTables (AcpiSupportInstance);
}

/**
 This function calculates and updates an UINT8 checksum.

 @param [in]   Buffer           Pointer to buffer to checksum
 @param [in]   Size             Number of bytes to checksum
 @param [in]   ChecksumOffset   Offset to place the checksum result in

 @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
AcpiPlatformChecksum (
  IN VOID       *Buffer,
  IN UINTN      Size,
  IN UINTN      ChecksumOffset
  )
{
  UINT8 Sum;
  UINT8 *Ptr;

  Sum = 0;
  //
  // Initialize pointer
  //
  Ptr = Buffer;

  //
  // set checksum to 0 first
  //
  Ptr[ChecksumOffset] = 0;

  //
  // add all content of buffer
  //
  while ((Size--) != 0) {
    Sum = (UINT8) (Sum + (*Ptr++));
  }
  //
  // set checksum
  //
  Ptr                 = Buffer;
  Ptr[ChecksumOffset] = (UINT8) (0xff - Sum + 1);

  return EFI_SUCCESS;
}

/**
 Checksum all versions of the common tables, RSDP, RSDT, XSDT.

 @param [in, out] AcpiSupportInstance  Protocol instance private data.

 @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
ChecksumCommonTables (
  IN OUT EFI_ACPI_SUPPORT_INSTANCE                 *AcpiSupportInstance
  )
{
  //
  // RSDP ACPI 1.0 checksum for 1.0 table.  This is only the first 20 bytes of the structure
  //
  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    AcpiPlatformChecksum (
      AcpiSupportInstance->Rsdp1,
      sizeof (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER),
      OFFSET_OF (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER,
      Checksum)
      );
   }

  //
  // RSDP ACPI 1.0 checksum for 2.0/3.0 table.  This is only the first 20 bytes of the structure
  //
  AcpiPlatformChecksum (
    AcpiSupportInstance->Rsdp3,
    sizeof (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER),
    OFFSET_OF (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER,
    Checksum)
    );

  //
  // RSDP ACPI 2.0/3.0 checksum, this is the entire table
  //
  AcpiPlatformChecksum (
    AcpiSupportInstance->Rsdp3,
    sizeof (EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER),
    OFFSET_OF (EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER,
    ExtendedChecksum)
    );

  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    //
    // RSDT checksums
    //
    AcpiPlatformChecksum (
      AcpiSupportInstance->Rsdt1,
      AcpiSupportInstance->Rsdt1->Length,
      OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
      Checksum)
      );

    AcpiPlatformChecksum (
      AcpiSupportInstance->Rsdt3,
      AcpiSupportInstance->Rsdt3->Length,
      OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
      Checksum)
      );
    }

  //
  // XSDT checksum
  //
  AcpiPlatformChecksum (
    AcpiSupportInstance->Xsdt,
    AcpiSupportInstance->Xsdt->Length,
    OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER,
    Checksum)
    );

  return EFI_SUCCESS;
}
//
// Protocol constructor
//
/**
 Constructor for the ACPI support protocol.  Initializes instance
 data.

 @param        AcpiSupportInstance  Instance to construct

 @retval EFI_SUCCESS            Instance initialized.
 @retval EFI_OUT_OF_RESOURCES   Unable to allocate required resources.

**/
EFI_STATUS
AcpiSupportAcpiSupportConstructor (
  EFI_ACPI_SUPPORT_INSTANCE                 *AcpiSupportInstance
  )
{
  EFI_STATUS            Status;
  UINT64                CurrentData;
  UINTN                 TotalSize;
  UINTN                 RsdpTableSize;
  UINT8                 *Pointer;
  EFI_PHYSICAL_ADDRESS  PageAddress;

  //
  // Check for invalid input parameters
  //
  ASSERT (AcpiSupportInstance);

  //
  // If ACPI v1.0b is among the ACPI versions we aim to support, we have to
  // ensure that all memory allocations are below 4 GB.
  //
  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    mAcpiTableAllocType = AllocateMaxAddress;
  } else {
    mAcpiTableAllocType = AllocateAnyPages;
  }

  InitializeListHead (&AcpiSupportInstance->TableList);
  AcpiSupportInstance->CurrentHandle              = 1;
  AcpiSupportInstance->AcpiSupport.GetAcpiTable   = GetAcpiTable;
  AcpiSupportInstance->AcpiSupport.SetAcpiTable   = SetAcpiTable;
  AcpiSupportInstance->AcpiSupport.PublishTables  = PublishTables;

  AcpiSupportInstance->AcpiTableProtocol.InstallAcpiTable   = InstallAcpiTable;
  AcpiSupportInstance->AcpiTableProtocol.UninstallAcpiTable = UninstallAcpiTable;
  if (FeaturePcdGet (PcdInstallAcpiSdtProtocol)) {
    SdtAcpiTableAcpiSdtConstructor (AcpiSupportInstance);
  }

  //
  // Create RSDP table
  //
  RsdpTableSize = sizeof (EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER);
  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    RsdpTableSize += sizeof (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER);
  }

  PageAddress = 0xFFFFFFFF;
  Status = gBS->AllocatePages (
                  mAcpiTableAllocType,
                  EfiACPIReclaimMemory,
                  EFI_SIZE_TO_PAGES (RsdpTableSize),
                  &PageAddress
                  );

  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  Pointer = (UINT8 *) (UINTN) PageAddress;
  ZeroMem (Pointer, RsdpTableSize);

  AcpiSupportInstance->Rsdp1 = (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER *) Pointer;
  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    Pointer += sizeof (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER);
  }
  AcpiSupportInstance->Rsdp3 = (EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER *) Pointer;

  //
  // Create RSDT, XSDT structures
  //
  TotalSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER) +         // for ACPI 2.0/3.0 XSDT
              mEfiAcpiMaxNumTables * sizeof (UINT64);

  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    TotalSize += sizeof (EFI_ACPI_DESCRIPTION_HEADER) +      // for ACPI 1.0 RSDT
                 mEfiAcpiMaxNumTables * sizeof (UINT32) +
                 sizeof (EFI_ACPI_DESCRIPTION_HEADER) +      // for ACPI 2.0/3.0 RSDT
                 mEfiAcpiMaxNumTables * sizeof (UINT32);
  }

  //
  // Allocate memory in the lower 32 bit of address range for
  // compatibility with ACPI 1.0 OS.
  //
  // This is done because ACPI 1.0 pointers are 32 bit values.
  // ACPI 2.0 OS and all 64 bit OS must use the 64 bit ACPI table addresses.
  // There is no architectural reason these should be below 4GB, it is purely
  // for convenience of implementation that we force memory below 4GB.
  //
  PageAddress = 0xFFFFFFFF;
  Status = gBS->AllocatePages (
                  mAcpiTableAllocType,
                  EfiACPIReclaimMemory,
                  EFI_SIZE_TO_PAGES (TotalSize),
                  &PageAddress
                  );

  if (EFI_ERROR (Status)) {
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS)(UINTN)AcpiSupportInstance->Rsdp1, EFI_SIZE_TO_PAGES (RsdpTableSize));
    return EFI_OUT_OF_RESOURCES;
  }

  Pointer = (UINT8 *) (UINTN) PageAddress;
  ZeroMem (Pointer, TotalSize);

  AcpiSupportInstance->Rsdt1 = (EFI_ACPI_DESCRIPTION_HEADER *) Pointer;
  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    Pointer += (sizeof (EFI_ACPI_DESCRIPTION_HEADER) + EFI_ACPI_MAX_NUM_TABLES * sizeof (UINT32));
    AcpiSupportInstance->Rsdt3 = (EFI_ACPI_DESCRIPTION_HEADER *) Pointer;
    Pointer += (sizeof (EFI_ACPI_DESCRIPTION_HEADER) + EFI_ACPI_MAX_NUM_TABLES * sizeof (UINT32));
  }
  AcpiSupportInstance->Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *) Pointer;

  //
  // Initialize RSDP
  //
  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    CurrentData = EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE;
    CopyMem (&AcpiSupportInstance->Rsdp1->Signature, &CurrentData, sizeof (UINT64));
    CopyMem (AcpiSupportInstance->Rsdp1->OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof (AcpiSupportInstance->Rsdp1->OemId));
    AcpiSupportInstance->Rsdp1->Reserved    = EFI_ACPI_RESERVED_BYTE;
    AcpiSupportInstance->Rsdp1->RsdtAddress = (UINT32) (UINTN) AcpiSupportInstance->Rsdt1;
  }

  CurrentData = EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE;
  CopyMem (&AcpiSupportInstance->Rsdp3->Signature, &CurrentData, sizeof (UINT64));
  CopyMem (AcpiSupportInstance->Rsdp3->OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof (AcpiSupportInstance->Rsdp3->OemId));
  AcpiSupportInstance->Rsdp3->Revision    = EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER_REVISION;
  AcpiSupportInstance->Rsdp3->Length      = sizeof (EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER);
  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    AcpiSupportInstance->Rsdp3->RsdtAddress = (UINT32) (UINTN) AcpiSupportInstance->Rsdt3;
  }
  CurrentData = (UINT64) (UINTN) AcpiSupportInstance->Xsdt;
  CopyMem (&AcpiSupportInstance->Rsdp3->XsdtAddress, &CurrentData, sizeof (UINT64));
  SetMem (AcpiSupportInstance->Rsdp3->Reserved, 3, EFI_ACPI_RESERVED_BYTE);

  if ((PcdGet32 (PcdAcpiExposedTableVersions) & EFI_ACPI_TABLE_VERSION_1_0B) != 0) {
    //
    // Initialize Rsdt
    //
    // Note that we "reserve" one entry for the FADT so it can always be
    // at the beginning of the list of tables.  Some OS don't seem
    // to find it correctly if it is too far down the list.
    //
    AcpiSupportInstance->Rsdt1->Signature = EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE;
    AcpiSupportInstance->Rsdt1->Length    = sizeof (EFI_ACPI_DESCRIPTION_HEADER);
    AcpiSupportInstance->Rsdt1->Revision  = EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_TABLE_REVISION;
    CopyMem (AcpiSupportInstance->Rsdt1->OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof (AcpiSupportInstance->Rsdt1->OemId));
    CurrentData = PcdGet64 (PcdAcpiDefaultOemTableId);
    CopyMem (&AcpiSupportInstance->Rsdt1->OemTableId, &CurrentData, sizeof (UINT64));
    AcpiSupportInstance->Rsdt1->OemRevision     = PcdGet32 (PcdAcpiDefaultOemRevision);
    AcpiSupportInstance->Rsdt1->CreatorId       = PcdGet32 (PcdAcpiDefaultCreatorId);
    AcpiSupportInstance->Rsdt1->CreatorRevision = PcdGet32 (PcdAcpiDefaultCreatorRevision);
    //
    // We always reserve first one for FADT
    //
    AcpiSupportInstance->NumberOfTableEntries1  = 1;
    AcpiSupportInstance->Rsdt1->Length          = AcpiSupportInstance->Rsdt1->Length + sizeof(UINT32);

    AcpiSupportInstance->Rsdt3->Signature       = EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE;
    AcpiSupportInstance->Rsdt3->Length          = sizeof (EFI_ACPI_DESCRIPTION_HEADER);
    AcpiSupportInstance->Rsdt3->Revision        = EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_TABLE_REVISION;
    CopyMem (AcpiSupportInstance->Rsdt3->OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof (AcpiSupportInstance->Rsdt3->OemId));
    CurrentData = PcdGet64 (PcdAcpiDefaultOemTableId);
    CopyMem (&AcpiSupportInstance->Rsdt3->OemTableId, &CurrentData, sizeof (UINT64));
    AcpiSupportInstance->Rsdt3->OemRevision     = PcdGet32 (PcdAcpiDefaultOemRevision);
    AcpiSupportInstance->Rsdt3->CreatorId       = PcdGet32 (PcdAcpiDefaultCreatorId);
    AcpiSupportInstance->Rsdt3->CreatorRevision = PcdGet32 (PcdAcpiDefaultCreatorRevision);
    //
    // We always reserve first one for FADT
    //
    AcpiSupportInstance->Rsdt3->Length          = AcpiSupportInstance->Rsdt3->Length + sizeof(UINT32);
  }
  AcpiSupportInstance->NumberOfTableEntries3  = 1;

  //
  // Initialize Xsdt
  //
  AcpiSupportInstance->Xsdt->Signature  = EFI_ACPI_3_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE;
  AcpiSupportInstance->Xsdt->Length     = sizeof (EFI_ACPI_DESCRIPTION_HEADER);
  AcpiSupportInstance->Xsdt->Revision   = EFI_ACPI_3_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_REVISION;
  CopyMem (AcpiSupportInstance->Xsdt->OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof (AcpiSupportInstance->Xsdt->OemId));
  CurrentData = PcdGet64 (PcdAcpiDefaultOemTableId);
  CopyMem (&AcpiSupportInstance->Xsdt->OemTableId, &CurrentData, sizeof (UINT64));
  AcpiSupportInstance->Xsdt->OemRevision      = PcdGet32 (PcdAcpiDefaultOemRevision);
  AcpiSupportInstance->Xsdt->CreatorId        = PcdGet32 (PcdAcpiDefaultCreatorId);
  AcpiSupportInstance->Xsdt->CreatorRevision  = PcdGet32 (PcdAcpiDefaultCreatorRevision);
  //
  // We always reserve first one for FADT
  //
  AcpiSupportInstance->Xsdt->Length           = AcpiSupportInstance->Xsdt->Length + sizeof(UINT64);

  ChecksumCommonTables (AcpiSupportInstance);

  //
  // Completed successfully
  //
  return EFI_SUCCESS;
}

/**
 Calculate the sum of all elements in a buffer in unit of UINT8.
 During calculation, the carry bits are dropped.

 @param [in]   Buffer           Pointer to the buffer to carry out the sum operation.
 @param [in]   Length           The size, in bytes, of Buffer .

 @retval Sum                    The sum of Buffer with carry bits dropped during additions.

**/
UINT8
EFIAPI
CalculateSum8 (
  IN      CONST UINT8              *Buffer,
  IN      UINTN                   Length
  )
{
  UINT8     Sum;
  UINTN     Count;

  ASSERT (Buffer != NULL);
  if (Buffer == NULL) {
    return 0;
  }
  ASSERT (Length <= (MAX_ADDRESS - ((UINTN) Buffer) + 1));

  for (Sum = 0, Count = 0; Count < Length; Count++) {
    Sum = (UINT8) (Sum + *(Buffer + Count));
  }

  return Sum;
}

/**
 Returns the two's complement checksum of all elements in a buffer
 of 8-bit values.

 @param [in]   Buffer           Pointer to the buffer to carry out the sum operation.
 @param [in]   Length           The size, in bytes, of Buffer .

 @retval Checksum               The 2's complement checksum of Buffer.

**/
UINT8
EFIAPI
CalculateCheckSum8 (
  IN      CONST UINT8              *Buffer,
  IN      UINTN                   Length
  )
{
  UINT8     CheckSum;

  CheckSum = CalculateSum8 (Buffer, Length);

  //
  // Return the checksum based on 2's complement.
  //
  return (UINT8) (0x100 - CheckSum);
}
