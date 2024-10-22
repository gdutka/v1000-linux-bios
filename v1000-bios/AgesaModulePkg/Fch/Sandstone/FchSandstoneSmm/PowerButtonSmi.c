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
#define FILECODE UEFI_SMM_FCHSMM_POWERBUTTONSMI_FILECODE

EFI_STATUS
FchSmmRegisterPowerButtonSmi (
  VOID
  )
{
  EFI_STATUS                              Status;
  FCH_SMM_PWRBTN_DISPATCH2_PROTOCOL       *AmdPwrBtnDispatch;
  FCH_SMM_PWRBTN_REGISTER_CONTEXT         AmdPowerButtonContext;
  EFI_HANDLE                              PowerButtonHandle;

//[-start-240426-IB18160212-modify-]//
  //
  // Register power button SMM event
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmPwrBtnDispatch2ProtocolGuid,
                  NULL,
                  (VOID**)&AmdPwrBtnDispatch
                  );
  ASSERT_EFI_ERROR (Status);
//[end-240426-IB18160212-modify]//

  AmdPowerButtonContext.Phase = EfiPowerButtonExit;
  AmdPowerButtonContext.Order = 0xFF;
  Status = AmdPwrBtnDispatch->Register (
                                AmdPwrBtnDispatch,
                                AmdSmiPowerButtonCallback,
                                &AmdPowerButtonContext,
                                &PowerButtonHandle
                                );

  AmdPowerButtonContext.Phase = EfiPowerButtonEntry;
  AmdPowerButtonContext.Order = 0x89;
  Status = AmdPwrBtnDispatch->Register (
                                AmdPwrBtnDispatch,
                                AmdSmiPowerButtonCallbackTest,
                                &AmdPowerButtonContext,
                                &PowerButtonHandle
                                );
  return Status;
}


//
// Enable sleep state transition
//
VOID
FchEnableSleepTransition ( VOID )
{
  // Set SLP_TYPEn to enable SLP_TYP to do S1/S3/S5
  RwPmio (FCH_PMIOA_REGBE, AccessWidth8, 0xFF, BIT5, NULL);

  // Enable S state transition
  RwPmio (FCH_PMIOA_REG08 + 3, AccessWidth8, (UINT32)~(BIT1 + BIT0), (UINT32) BIT0, NULL);
}


/*++

Routine Description:
  SMI handler to restore ACPI mode

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
AmdSmiPowerButtonCallbackTest (
  IN       EFI_HANDLE                            DispatchHandle,
  IN       CONST FCH_SMM_PWRBTN_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                                  *CommBuffer OPTIONAL,
  IN OUT   UINTN                                 *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS     Status;
  Status = EFI_SUCCESS;
  return Status;
}


EFI_STATUS
EFIAPI
AmdSmiPowerButtonCallback (
  IN       EFI_HANDLE                            DispatchHandle,
  IN       CONST FCH_SMM_PWRBTN_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                                  *CommBuffer OPTIONAL,
  IN OUT   UINTN                                 *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS     Status;
  UINT32         AcpiMmioBase;
  UINT16         AcpiPmbase;
  UINT16         Value16;

  Status = EFI_SUCCESS;

  FchEnableSleepTransition ();
//  GetFchAcpiMmioBase (&AcpiMmioBase, NULL);
  AcpiMmioBase = 0xFED80000;
  GetFchAcpiPmBase (&AcpiPmbase, NULL);
  BackUpCG2 ();
  RestoreAmbaEmmc2D0 ();

  // Turn off SLP_TYP SMI
  RwMem (AcpiMmioBase + SMI_BASE + FCH_SMI_REGB0, AccessWidth32, (UINT32)~(BIT3 + BIT2), 0);

  // Clear power button status
  Value16 = BIT8;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, AcpiPmbase + R_FCH_ACPI_PM1_STATUS, &Value16);

  // power the system off now
  LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT16, AcpiPmbase + R_FCH_ACPI_PM_CONTROL, &Value16);
  Value16 &= (UINT32)~(BIT12 + BIT11 + BIT10);
  Value16 |= 5 << 10;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, AcpiPmbase + R_FCH_ACPI_PM_CONTROL, &Value16);
  Value16 |= BIT13;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, AcpiPmbase + R_FCH_ACPI_PM_CONTROL, &Value16);

  return Status;
}


