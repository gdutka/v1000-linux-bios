/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH SMM Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "FchSmm.h"
#include "GnbDxio.h"
#include <Library/NbioSmuV10Lib.h>
#define FILECODE UEFI_SMM_FCHSMM_SXSMI_FILECODE

extern  UINT8                        *mFchPciIrqRoutingTable;
extern FCH_DATA_BLOCK                      gFchDataInSmm;

EFI_STATUS
FchSmmRegisterSxSmi (
  VOID
  )
{
  EFI_STATUS                               Status;
  FCH_SMM_SX_DISPATCH2_PROTOCOL            *AmdSxDispatch;
  FCH_SMM_SX_REGISTER_CONTEXT              SxRegisterContext;
  EFI_HANDLE                               SxHandle;

//[-start-240426-IB18160212-modify-]//
  //
  // Register AMD SX SMM
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSxDispatch2ProtocolGuid,
                  NULL,
                  (VOID**)&AmdSxDispatch
                  );
  ASSERT_EFI_ERROR (Status);
//[end-240426-IB18160212-modify]//

  SxRegisterContext.Type  = SxS3;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 0xC0;

  Status = AmdSxDispatch->Register (
                             AmdSxDispatch,
                             AmdSmiS3SleepEntryCallback,
                             &SxRegisterContext,
                             &SxHandle
                             );

  SxRegisterContext.Type  = SxS4;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 0xC0;

  Status = AmdSxDispatch->Register (
                             AmdSxDispatch,
                             AmdSmiS4SleepEntryCallback,
                             &SxRegisterContext,
                             &SxHandle
                             );

  SxRegisterContext.Type  = SxS5;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 0xC0;

  Status = AmdSxDispatch->Register (
                             AmdSxDispatch,
                             AmdSmiS5SleepEntryCallback,
                             &SxRegisterContext,
                             &SxHandle
                             );

  return Status;
}

/**
 * FixPsp4Ehang
 *
 *
 * @retval  VOID
 *
 */
VOID
FixPsp4Ehang (
  OUT VOID
  )
{
  UINT8          Value8;
  UINT32          IoApicNumber;
  AMD_CONFIG_PARAMS      *StdHeader;
  FCH_DATA_BLOCK      *pFchPolicy;

  pFchPolicy = &gFchDataInSmm;
  StdHeader = pFchPolicy->StdHeader;

  ACPIMMIO32 (FCH_AOACx94S0I3_CONTROL) |= FCH_AOACx94S0I3_CONTROL_ARBITER_DIS + FCH_AOACx94S0I3_CONTROL_INTERRUPT_DIS;
  ACPIMMIO32 (0xFEC00000) = 0x3E;
  ACPIMMIO32 (0xFEC00010) = 0xFF;
  LibAmdIoRead (AccessWidth8, FCH_IOMAP_REGED, &Value8, StdHeader);
  ACPIMMIO32 (0xFEC00020) = 0x17;
  LibAmdIoRead (AccessWidth8, FCH_IOMAP_REGED, &Value8, StdHeader);
  IoApicNumber = ACPIMMIO32 (0xFEC00020);

  for (IoApicNumber = 0; IoApicNumber < 24; IoApicNumber++) {
    ACPIMMIO32 (0xFEC00000) = 0x10 + (IoApicNumber << 1);
    ACPIMMIO32 (0xFEC00010) = BIT16;
  }
  // put eMMC into D0 if it is D0
    ACPIMMIO32 (0xFED81E78) |= BIT3;
    ACPIMMIO32 (0xFED81E78) &= ~ 0x3;
}

/*++

Routine Description:


Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
AmdSmiS3SleepEntryCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                              *CommBuffer OPTIONAL,
  IN OUT   UINTN                             *CommBufferSize  OPTIONAL
  )
{
  UINT8          Index;
  UINT8          *pData;
  FCH_DATA_BLOCK      *pFchPolicy;
  pFchPolicy = &gFchDataInSmm;
  // Save entire FCH PCI IRQ routing space (C00/C01)
  pData = mFchPciIrqRoutingTable;
  Index = 0xFF;
  do {
    Index++;
    LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGC00, &Index);
    LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGC01, pData++);
  } while (Index != 0xFF);

  BackUpCG2 ();
  FixPsp4Ehang ();
  FchRVUsb3InitS3EntryProgram (pFchPolicy);
  ProgramPMEDis (0, FALSE);
  RestoreAmbaEmmc2D0 ();
  if (pFchPolicy->FchRunTime.FchDeviceEnableMap & BIT28) {
    ProgramLpcEmmcPins (FALSE);
  }
  if (pFchPolicy->HwAcpi.FchSxEntryXhciPmeEn) {
    ProgramXhciPmeEn(pFchPolicy);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmdSmiS4SleepEntryCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                              *CommBuffer OPTIONAL,
  IN OUT   UINTN                             *CommBufferSize  OPTIONAL
  )
{
  FCH_DATA_BLOCK      *pFchPolicy;
  pFchPolicy = &gFchDataInSmm;

  BackUpCG2 ();
  FixPsp4Ehang ();
  FchRVUsb3InitS3EntryProgram (pFchPolicy);
  ProgramPMEDis (0, FALSE);
  RestoreAmbaEmmc2D0 ();
  if (pFchPolicy->FchRunTime.FchDeviceEnableMap & BIT28) {
    ProgramLpcEmmcPins (FALSE);
  }
  if (pFchPolicy->HwAcpi.FchSxEntryXhciPmeEn) {
    ProgramXhciPmeEn(pFchPolicy);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmdSmiS5SleepEntryCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                              *CommBuffer OPTIONAL,
  IN OUT   UINTN                             *CommBufferSize  OPTIONAL
  )
{
  FCH_DATA_BLOCK      *pFchPolicy;
  pFchPolicy = &gFchDataInSmm;
  BackUpCG2 ();
  //RwMem (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG04 + 2, AccessWidth8, ~BIT3, BIT3);
  FixPsp4Ehang ();
  FchRVUsb3InitS3EntryProgram (pFchPolicy);
  RestoreAmbaEmmc2D0 ();
  if (pFchPolicy->FchRunTime.FchDeviceEnableMap & BIT28) {
    ProgramLpcEmmcPins (FALSE);
  }
  return EFI_SUCCESS;
}

