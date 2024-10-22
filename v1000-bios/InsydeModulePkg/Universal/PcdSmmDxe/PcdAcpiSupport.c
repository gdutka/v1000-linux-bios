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
#include "PcdAcpiSupport.h"

H2O_ACPI_GET_PCD_VALUE               *mGlobalPcdArea = NULL;
extern EFI_GET_PCD_INFO_PROTOCOL     mEfiGetPcdInfoInstance;
extern EFI_PCD_PROTOCOL              mEfiPcdInstance;

#define LibPcdGetExPtr(X, Y) mEfiPcdInstance.GetPtr (X, Y)

/**
  Software SMI callback for Read PCD which is called from ACPI method.

  Caution: This function may receive untrusted input.
  ACPINvs are external input, so this function will validate
  its data structure to be valid value.

  @retval H2O_PCD_ACPI_STATUS_SUCCESS  Success
  @retval H2O_PCD_ACPI_STATUS_INVALID_PCD  Invalid PCD
  @retval H2O_PCD_ACPI_STATUS_INVALID_ACTION  Invalid PCD Action

**/
H2O_PCD_STATUS
ParamterCheck (
  UINTN     *Size
  )
{
  EFI_STATUS      Status;
  EFI_PCD_INFO    EfiPcdInfo;

  Status = EFI_SUCCESS;

  if (mGlobalPcdArea->PcdAction != READ_PCD_COMMAND) {
    return H2O_PCD_ACPI_STATUS_INVALID_ACTION;
  }
  if (CompareGuid (&mGlobalPcdArea->PcdGuid, &gZeroGuid)) {
    return H2O_PCD_ACPI_STATUS_INVALID_PCD;
  }

  Status = mEfiGetPcdInfoInstance.GetInfo (&mGlobalPcdArea->PcdGuid, mGlobalPcdArea->PcdTokenNumber, &EfiPcdInfo);
  if (EFI_ERROR(Status)) {
    return H2O_PCD_ACPI_STATUS_INVALID_PCD;
  }

  *Size = EfiPcdInfo.PcdSize;
  mGlobalPcdArea->PcdDataSize = (UINT32) EfiPcdInfo.PcdSize;
  mGlobalPcdArea->PcdType = EfiPcdInfo.PcdType;
  return H2O_PCD_ACPI_STATUS_SUCCESS;

}

/**
  Software SMI callback for Read PCD which is called from ACPI method.

  Caution: This function may receive untrusted input.
  ACPINvs are external input, so this function will validate
  its data structure to be valid value.

  @retval EFI_SUCCESS             The interrupt was handled successfully.

**/
EFI_STATUS
SmmGetPcdCommon (
  VOID
  )
{
  EFI_STATUS        Status;
  H2O_PCD_STATUS    PcdStatus;
  UINTN             Size;
  EFI_PCD_PROTOCOL  *Pcd;
  VOID              *Buffer;

  Size = 0;
  PcdStatus = H2O_PCD_ACPI_STATUS_SUCCESS;
  Status = EFI_UNSUPPORTED;
  Pcd    = &mEfiPcdInstance;

  PcdStatus = ParamterCheck(&Size);
  if (PcdStatus != H2O_PCD_ACPI_STATUS_SUCCESS) {
    return Status;
  }

  switch(mGlobalPcdArea->PcdType) {

    case EFI_PCD_TYPE_8:
      *(UINT8 *) mGlobalPcdArea->PcdData = Pcd->Get8(&mGlobalPcdArea->PcdGuid,mGlobalPcdArea->PcdTokenNumber);
      break;

    case EFI_PCD_TYPE_16:
      *(UINT16 *) mGlobalPcdArea->PcdData = Pcd->Get16(&mGlobalPcdArea->PcdGuid,mGlobalPcdArea->PcdTokenNumber);
      break;

    case EFI_PCD_TYPE_32:
      *(UINT32 *) mGlobalPcdArea->PcdData = Pcd->Get32(&mGlobalPcdArea->PcdGuid,mGlobalPcdArea->PcdTokenNumber);
    break;

    case EFI_PCD_TYPE_64:
      *(UINT64 *) mGlobalPcdArea->PcdData = Pcd->Get64(&mGlobalPcdArea->PcdGuid,mGlobalPcdArea->PcdTokenNumber);
      break;

    case EFI_PCD_TYPE_BOOL:
      *(BOOLEAN *) mGlobalPcdArea->PcdData = Pcd->GetBool(&mGlobalPcdArea->PcdGuid,mGlobalPcdArea->PcdTokenNumber);
    break;

    case EFI_PCD_TYPE_PTR:
        if (Size > MAX_PCD_DATA_SIZE) {
          PcdStatus = H2O_PCD_ACPI_STATUS_DATA_TOO_SMALL;    // PcdDataSize was too small to hold the PCD's data
          break;
        }
        Buffer = Pcd->GetPtr(&mGlobalPcdArea->PcdGuid,mGlobalPcdArea->PcdTokenNumber);
        CopyMem(mGlobalPcdArea->PcdData, Buffer, Size);
        break;
    default:
      PcdStatus = H2O_PCD_ACPI_STATUS_INVALID_PCD;             // wrong PCD Type specified
      Status = EFI_INVALID_PARAMETER;
      break;
  }

  mGlobalPcdArea->PcdStatus = PcdStatus;
  if (PcdStatus == H2O_PCD_ACPI_STATUS_SUCCESS) {
    Status = EFI_SUCCESS;
  }
  return Status;

}


/**
  Software SMI callback for Read PCD which is called from ACPI method.

  Caution: This function may receive untrusted input.
  ACPINvs are external input, so this function will validate
  its data structure to be valid value.

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[in, out] CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS             The interrupt was handled successfully.

**/
EFI_STATUS
EFIAPI
AcpiReadPcdCallback (
  IN EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                  *Context,
  IN OUT VOID                    *CommBuffer,
  IN OUT UINTN                   *CommBufferSize
  )
{
  EFI_STATUS                       Status;
  H2O_ACPI_GET_PCD_VALUE           *OrgGlobalPcdArea;

  //
  // Copy H2O_ACPI_GET_PCD_VALUE to SMRAM to prevent from TOCTOU attack.
  //
  OrgGlobalPcdArea = mGlobalPcdArea;
  mGlobalPcdArea = AllocateCopyPool (sizeof (H2O_ACPI_GET_PCD_VALUE), OrgGlobalPcdArea);
  if (mGlobalPcdArea == NULL) {
    mGlobalPcdArea = OrgGlobalPcdArea;
    return EFI_SUCCESS;
  }
  mGlobalPcdArea->PcdAction = READ_PCD_COMMAND;
  Status = SmmGetPcdCommon();
  //
  // Restore data from SMRAM to H2O_ACPI_GET_PCD_VALUE buffer.
  //
  CopyMem (OrgGlobalPcdArea, mGlobalPcdArea, sizeof (H2O_ACPI_GET_PCD_VALUE));
  FreePool (mGlobalPcdArea);
  mGlobalPcdArea = OrgGlobalPcdArea;
  return Status;
}

/**
  Find the operation region in Pcd ACPI table by given Name and Size,
  and initialize it if the region is found.

  @param[in, out] Table          The Pcd item in ACPI table.
  @param[in]      Name           The name string to find in Pcd table.
  @param[in]      Size           The size of the region to find.
  @param[in, out] Address        The allocated address for the found region.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
AssignOpRegion (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN     UINT32                       Name,
  IN     UINT16                       Size,
  IN OUT VOID**                       Address OPTIONAL
  )
{
  EFI_STATUS                          Status;
  AML_OP_REGION_32_16                 *OpRegion;
  EFI_PHYSICAL_ADDRESS                MemoryAddress;
  UINTN                               SizeA;

  SizeA = sizeof (AML_OP_REGION_32_16);

  Status = EFI_NOT_FOUND;
  MemoryAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) *Address;
  //
  // Patch some pointers for the ASL code before loading the SSDT.
  //
  for (OpRegion  = (AML_OP_REGION_32_16 *) (Table + 1);
       OpRegion <= (AML_OP_REGION_32_16 *) ((UINT8 *) Table + Table->Length);
       OpRegion  = (AML_OP_REGION_32_16 *) ((UINT8 *) OpRegion + 1)) {
    if ((OpRegion->OpRegionOp  == AML_EXT_REGION_OP) &&
        (OpRegion->NameString  == Name) &&
        (OpRegion->DWordPrefix == AML_DWORD_PREFIX) &&
        (OpRegion->WordPrefix  == AML_WORD_PREFIX)) {

      if (MemoryAddress == 0) {
        MemoryAddress = SIZE_4GB - 1;
        Status = gBS->AllocatePages (AllocateMaxAddress, EfiACPIMemoryNVS, EFI_SIZE_TO_PAGES (Size), &MemoryAddress);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "AllocatePages Status = %r\n",Status));
          goto Done;
        }
        ZeroMem ((VOID *)(UINTN) MemoryAddress, Size);
      }
      OpRegion->RegionOffset = (UINT32) (UINTN) MemoryAddress;
      OpRegion->RegionLen    = Size;
      Status = EFI_SUCCESS;
      break;
    }
  }

  *Address = (VOID *) (UINTN) MemoryAddress;

Done:
  return Status;
}

/**
  Initialize and publish Pcd items in ACPI table.

  @retval   EFI_SUCCESS     The Pcd ACPI table is published successfully.
  @retval   Others          The Pcd ACPI table is not published.

**/
EFI_STATUS
PublishAcpiTable (
  VOID
  )
{
  EFI_STATUS                     Status;
  UINTN                          TableKey;
  UINTN                          TableSize;
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTable;
  EFI_ACPI_DESCRIPTION_HEADER    *Table;

  //
  // Use instance 1 to get ACPI table, Instance 0 is DXEPcdDataBaseSec.raw
  //
  Status = GetSectionFromFv (
             &gEfiCallerIdGuid,
             EFI_SECTION_RAW,
             1,
             (VOID **) &Table,
             &TableSize
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "GetSectionFromFv Status = %r\n",Status));
    return Status;
  }

  CopyMem (Table->OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof (Table->OemId) );
  Status = AssignOpRegion (Table, SIGNATURE_32 ('G', 'P', 'C', 'D'), (UINT16) sizeof (H2O_ACPI_GET_PCD_VALUE), (VOID **)&mGlobalPcdArea);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AssignOpRegion Status = %r\n",Status));
    return Status;
  }

  //
  // Publish the PCD ACPI table
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Locate gEfiAcpiTableProtocolGuid Protocol Status = %r\n",Status));
    return Status;
  }

  TableKey = 0;
  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        Table,
                        TableSize,
                        &TableKey
                        );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "InstallAcpiTable Status = %r\n",Status));
  }

  return Status;
}


/**

  InitializePcdAcpiSupport allocate space for pcd.asl and publish it's acpi table

  @retval   EFI_SUCCESS     The PCD ACPI table is published successfully.
  @retval   Others          The PCD ACPI table is not published.


**/
EFI_STATUS
InitializePcdAcpiSupport (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_PHYSICAL_ADDRESS           Address;
  UINTN                          Pages;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT    SwContext;
  EFI_HANDLE                     SwHandle;

  //
  // Allocate and initialize the PCD area for SMM and ASL communication.
  //
  Pages = EFI_SIZE_TO_PAGES (sizeof (H2O_ACPI_GET_PCD_VALUE));
  Address = 0xFFFFFFFF;
  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   Pages,
                   &Address
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AllocatePages Status = %r\n",Status));
    return Status;
  }

  mGlobalPcdArea = (H2O_ACPI_GET_PCD_VALUE*) (UINTN)Address;
  SetMem (mGlobalPcdArea, sizeof (H2O_ACPI_GET_PCD_VALUE), 0);
  DEBUG ((EFI_D_INFO, "mGlobalPcdArea is at 0x%X\n", mGlobalPcdArea));

  Status = PublishAcpiTable ();
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "PublishAcpiTable Status = %r\n",Status));
    return Status;
  }

  //
  // Get the Sw dispatch protocol and register SMI callback functions.
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&SwDispatch);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Locate gEfiSmmSwDispatch2ProtocolGuid Protocol Status = %r\n",Status));
    return Status;
  }

  SwContext.SwSmiInputValue = (UINTN) SMM_READ_PCD;
  Status = SwDispatch->Register (SwDispatch, AcpiReadPcdCallback, &SwContext, &SwHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Register ReadPcdCallback Status = %r\n",Status));
    return Status;
  }

  return Status;
}

