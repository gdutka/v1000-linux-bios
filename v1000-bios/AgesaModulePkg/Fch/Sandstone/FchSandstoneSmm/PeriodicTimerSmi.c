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
#define FILECODE UEFI_SMM_FCHSMM_PERIODICTIMERSMI_FILECODE

extern FCH_DATA_BLOCK                      gFchDataInSmm;

UINT32 mShortTimerCounter;
UINT32 mLongTimerCounter;

/*++

Routine Description:
  Handler for LongTimer SMI

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  RegisterContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
AmdSmiLongTimerCallback (
  IN       EFI_HANDLE                                DispatchHandle,
  IN       CONST FCH_SMM_PERIODICAL_REGISTER_CONTEXT *RegisterContext,
  IN OUT   EFI_SMM_PERIODIC_TIMER_CONTEXT            *PeriodicTimerContext,
  IN OUT   UINTN                                     *SizeOfContext
  )
{
  //LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, 0x80, &mLongTimerCounter);
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, 0x80, &mLongTimerCounter);
  mLongTimerCounter++;
  return EFI_SUCCESS;
}


/*++

Routine Description:
  Handler for ShortTimer SMI

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  RegisterContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
AmdSmiShortTimerCallback (
  IN       EFI_HANDLE                                DispatchHandle,
  IN       CONST FCH_SMM_PERIODICAL_REGISTER_CONTEXT *RegisterContext,
  IN OUT   EFI_SMM_PERIODIC_TIMER_CONTEXT            *PeriodicTimerContext,
  IN OUT   UINTN                                     *SizeOfContext
  )
{
  //LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, 0x80, &mShortTimerCounter);
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, 0x80, &mShortTimerCounter);
  mShortTimerCounter++;
  return EFI_SUCCESS;
}


EFI_STATUS
FchSmmRegisterPeriodicTimerSmi (
  VOID
  )
{
  EFI_STATUS                              Status;
  FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL   *AmdPeriodicalDispatch;
  FCH_SMM_PERIODICAL_REGISTER_CONTEXT     PeriodicalRegisterContext;
  EFI_HANDLE                              PtHandle;
  FCH_MISC                                *FchMisc;

  FchMisc = &gFchDataInSmm.Misc;

//[-start-240426-IB18160212-modify-]//
  //
  // Periodic Timer SMI Registration
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmPeriodicalDispatch2ProtocolGuid,
                  NULL,
                  (VOID**)&AmdPeriodicalDispatch
                  );
  ASSERT_EFI_ERROR (Status);
//[end-240426-IB18160212-modify]//

  //
  // Due to the post code limitation, we choose to turn on LongTimer and ShortTimer SMIs exclusively.
  // If both are set to enable in setup, we only register the LongTimer SMI.
  //
  if (FchMisc->LongTimer.Enable) {
    PeriodicalRegisterContext.SmiTickInterval  = LONG_TIMER_SMI_INTERVAL;
    PeriodicalRegisterContext.Period           = FchMisc->LongTimer.CycleDuration * LONG_TIMER_SMI_INTERVAL;
    PeriodicalRegisterContext.StartNow         = FchMisc->LongTimer.StartNow;
    mLongTimerCounter = 0;
    Status = AmdPeriodicalDispatch->Register (
                                      AmdPeriodicalDispatch,
                                      AmdSmiLongTimerCallback,
                                      &PeriodicalRegisterContext,
                                      &PtHandle
                                      );
  } else if (FchMisc->ShortTimer.Enable) {
    PeriodicalRegisterContext.SmiTickInterval  = SHORT_TIMER_SMI_INTERVAL;
    PeriodicalRegisterContext.Period           = (FchMisc->ShortTimer.CycleDuration >> 1) * SHORT_TIMER_SMI_INTERVAL;
    PeriodicalRegisterContext.StartNow         = FchMisc->ShortTimer.StartNow;
    mShortTimerCounter = 0;
    Status = AmdPeriodicalDispatch->Register (
                                      AmdPeriodicalDispatch,
                                      AmdSmiShortTimerCallback,
                                      &PeriodicalRegisterContext,
                                      &PtHandle
                                      );
  }
  return Status;
}



