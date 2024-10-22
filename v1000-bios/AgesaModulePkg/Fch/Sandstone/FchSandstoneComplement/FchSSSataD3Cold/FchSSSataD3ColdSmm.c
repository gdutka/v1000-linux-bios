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
#include "FchSSSataD3ColdSmm.h"

EFI_STATUS
EFIAPI
AmdSmiSataD3ColdEntryCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  UINT8               Debug;
  UINT32              SmuArg[6];
  UINT32              SmuRequestId;
  UINT32              SmuResponse;
  PCI_ADDR            NbioPciAddress;

  Debug = 0x78;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, 0x80, &Debug);

  // 1, set DEVSLP PAD value
  FchSataD3ColdOverrideDevSlpPad ();
  // 2, put sata into offline mode.
  FchSataD3ColdOfflineMode ();
  // 3, save registers.
  FchSataD3ColdSaveRegisters ();
  // 4, polling each port already enter into offline mode.
  FchSataD3ColdPollingOffline ();
  // 5, Block reset from PCS to PHY.
  FchSataD3ColdBlockReset ();
  // 6, call SMU for D3Cold Entry.
  NbioSmuServiceCommonInitArgumentsV10 (SmuArg);
  NbioPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
  SmuArg[0] = 0x1;
  SmuRequestId = SMC_MSG_PowerDownSata;

  SmuResponse = NbioSmuServiceRequestV10 (NbioPciAddress, SmuRequestId, SmuArg, 0);

  //FchSataD3ColdPowerDown ();

  Debug = 0x79;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, 0x80, &Debug);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmdSmiSataD3ColdExitCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  UINT8               Debug;
  UINT32              SmuArg[6];
  UINT32              SmuRequestId;
  UINT32              SmuResponse;
  PCI_ADDR            NbioPciAddress;

  Debug = 0x89;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, 0x80, &Debug);

  //000 check if we're in D3Cold
  if (FchSataD3ColdClockOn ()) {
    return EFI_SUCCESS;
  }

  //1, Enable FCH_SATA_100M_CLK: SW programs ACPI MISC_REG 0x4C[16]=1
  FchSataD3ColdEnable100MClk ();
  //2, Remove DEVSLP PAD override value: SW program ACPI PMIO_0xDC[1:0] to 2'b00
  FchSataD3ColdRemoveOverrideDevSlpPad ();
  //3, Set D3 resume indication signal and block OOB: SW program LOCAL_RSMU_Reg: 04h[10] = 1
  FchSataD3ColdBlockOOB ();
  //4, call SMU for D3Cold Exit.
  NbioSmuServiceCommonInitArgumentsV10 (SmuArg);
  NbioPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
  SmuArg[0] = 0x1;
  SmuRequestId = SMC_MSG_PowerUpSata;

  SmuResponse = NbioSmuServiceRequestV10 (NbioPciAddress, SmuRequestId, SmuArg, 0);

  //FchSataD3ColdPowerUp ();

  //5, Restore Sata registers
  FchSataD3ColdRestoreRegisters ();
  //6, Unblock reset from PCS to PHY: SW programs LOCAL_RSMU_Reg: 04h[11] = 0
  FchSataD3ColdUnBlockReset ();
  //7, De-assert D3 resume indication signal and un-block OOB: SW program SATA LOCAL_RSMU_Reg: 04h[10] = 0
  FchSataD3ColdUnBlockOOB ();
  Debug = 0x8A;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, 0x80, &Debug);

  return EFI_SUCCESS;
}

/*********************************************************************************
 * Name: FchSataD3ColdSmmEntryPoint
 *
 * Description
 *   Entry point of the SataD3Cold SMM driver
 *   copy FCH_INIT_PROTOCOL to SMM, and register Fch Smm callbacks
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
FchSataD3ColdSmmEntryPoint (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )

{
  EFI_STATUS                               Status;
  FCH_SMM_SW_DISPATCH2_PROTOCOL            *AmdSwDispatch;
  FCH_SMM_SW_REGISTER_CONTEXT              SwRegisterContext;
  EFI_HANDLE                               SwHandle;
  UINT32                                   RTD3ColdEnableMap;

  Status = EFI_SUCCESS;
  RTD3ColdEnableMap = PcdGet32 (FchRTD3ColdEnableMap);

  if ((RTD3ColdEnableMap & BIT15) == 0) {
    return Status;
  }

  //
  //  Locate SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
             &gFchSmmSwDispatch2ProtocolGuid,
             NULL,
             (VOID**)&AmdSwDispatch
             );
  ASSERT_EFI_ERROR (Status);

  SwRegisterContext.AmdSwValue  = PcdGet8 (PcdFchSataD3EntrySwSmi);
  SwRegisterContext.Order       = 0x80;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            AmdSmiSataD3ColdEntryCallback,
                            &SwRegisterContext,
                            &SwHandle
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SwRegisterContext.AmdSwValue  = PcdGet8 (PcdFchSataD3ExitSwSmi);
  SwRegisterContext.Order       = 0x80;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            AmdSmiSataD3ColdExitCallback,
                            &SwRegisterContext,
                            &SwHandle
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return Status;
}


