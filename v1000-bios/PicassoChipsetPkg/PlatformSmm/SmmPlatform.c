/** @file
  A SMM driver for SMM platform initialization.

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include <SmmPlatform.h>

//
// Global variables.
//
UINT16                              mAcpiPmBaseAddr;
SYSTEM_CONFIGURATION                *mSetupVariable;
CBS_CONFIG                          *mAmdCbsVariable;

/**
 This function either writes to or read from global register table the data of
 Gpio, SVID, LevelEdge, Pci, CpuState, Mtrr, Sio, Kbc, and Pic registers.

 @param [in]   SaveRestoreFlag  True: write data to SMM IO registers.
                                False: read data from IO to global registers.

 @retval EFI_SUCCESS

**/
EFI_STATUS
SaveRestoreState (
  IN BOOLEAN                        SaveRestoreFlag
  )
{
  EFI_STATUS              Status;

  Status = SaveRestoreCpu (SaveRestoreFlag);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  The notification of AcpiEnableCallbackStart used for enabling ACPI mode.

  Dispatched on reads from APM port with value EFI_ACPI_ENABLE_SW_SMI
  Disables the SW SMI Timer.
  ACPI events are disabled and ACPI event status is cleared.
  SCI mode is then enabled.
  Disable SW SMI Timer
  Clear all ACPI event status and disable all ACPI events
  Disable PM sources except power button
  Clear status bits
  Disable GPE0 sources
  Clear status bits
  Disable GPE1 sources
  Clear status bits
  Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)
  Enable SCI

  @param [in] Protocol              Points to the protocol's unique identifier.
  @param [in] Interface             Points to the interface instance.
  @param [in] Handle                The handle on which the interface was installed.

  @retval EFI_SUCCESS               This routine was handled successfully.

**/
EFI_STATUS
EnableAcpiCallbackStart (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                    Handle
  )
{
  UINT8       Value8;
  Value8 = 0;

  //
  // To disable Power Button SMI.
  //
  Value8 = MmioRead8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC);
  Value8 &= ~(BIT6 + BIT7);
  MmioWrite8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC, Value8);
  Value8 = MmioRead8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB2);
  Value8 &= ~(BIT4 + BIT5);
  MmioWrite8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB2, Value8);

//  if (FeaturePcdGet (PcdLegacyFreeTiSupport)) {
//#ifdef EC_INIT_ACPI_EN
//    EcEnable ();
//    EcAcDcSwitch (SetupVariable->amdAcDcSwitch, SetupVariable->amdDcTimer, SetupVariable->amdAcTimer);
//#endif
//  }
    OemSvcEcAcpiMode (TRUE);

  //
  // Save Status to SMM
  //
  SaveRestoreState (FALSE);

  return EFI_SUCCESS;
}

/**
  The notification of AcpiDisableCallbackStart used for disabling ACPI mode.

  Dispatched on reads from APM port with value 0xA1
  ACPI events are disabled and ACPI event status is cleared.
  SCI mode is then disabled.
  Clear all ACPI event status and disable all ACPI events
  Disable PM sources except power button
  Clear status bits
  Disable GPE0 sources
  Clear status bits
  Disable GPE1 sources
  Clear status bits
  Disable SCI

  @param [in] Protocol              Points to the protocol's unique identifier.
  @param [in] Interface             Points to the interface instance.
  @param [in] Handle                The handle on which the interface was installed.

  @retval EFI_SUCCESS               This routine was handled successfully.

**/
EFI_STATUS
DisableAcpiCallbackStart (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                    Handle
  )
{
  OemSvcEcAcpiMode (FALSE);

  return EFI_SUCCESS;
}

/**
  SetSataDmaCapable.

  @param None.

  @retval void.

**/
VOID
SetSataDmaCapable (
  VOID
  )
{
  UINT8       BusB;
  UINT16      IoBase;

  IoBase = 0;

  //
  // Internal PCIe GPP Bridge 0 to Bus B. (PPR.1 Table 13)
  //
  BusB = PciExpressRead8 (
            PCI_EXPRESS_LIB_ADDRESS (
            0,
            8,
            2,
            0x19
            )
         );

  if ((PciExpressRead32 (
         PCI_EXPRESS_LIB_ADDRESS (
           BusB,
           0,
           0,
           R_VENDOR_ID
           )
         ) != 0xFFFFFFFF)) {

    IoBase = (PciExpressRead16 (
                PCI_EXPRESS_LIB_ADDRESS (
                  BusB,
                  0,
                  0,
                  R_BAR4
                  )
                ) & 0xFFF0);
    IoWrite8 (
      (IoBase + 0x02),
      (IoRead8 (IoBase + 0x02) | (BIT5 + BIT6))
      );
    IoWrite8 (
      (IoBase + 0x0a),
      (IoRead8 (IoBase + 0x0a) | (BIT5 + BIT6))
      );
  }

  return ;
}

/**
  UpdatePmeSetting

  @param None.

  @retval void.

**/
VOID
UpdatePmeSetting (
  VOID
  )
{
  UINT8 Buffer;

  // Program LAN PME_En bit.
  S5WakeOnLanSetting();

  // Update PciExpWakeDis bit
  Buffer = IoRead8 (mAcpiPmBaseAddr + PM1_EN_HIGH_BYTE);
  if (mSetupVariable->WakeOnPME) {
    Buffer &= ~BIT6;
  } else {
    Buffer |= BIT6;
  }
  IoWrite8 (mAcpiPmBaseAddr + PM1_EN_HIGH_BYTE, Buffer);
}

/**
  The notification of AcpiRestoreCallbackStart used for restoring ACPI mode.

  @param [in] Protocol              Points to the protocol's unique identifier.
  @param [in] Interface             Points to the interface instance.
  @param [in] Handle                The handle on which the interface was installed.

  @retval EFI_SUCCESS               This routine was handled successfully.

**/
EFI_STATUS
RestoreAcpiCallbackStart (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                    Handle
  )
{
  SaveRestoreState (TRUE);

  //
  // To avoid S3 resume fail when only have NVMe SSD to boot in OS and disable sata controller. 
  //
  if (mAmdCbsVariable->CbsCmnFchSataEnable != 0x0) {
    SetSataDmaCapable ();
  } 

  return EFI_SUCCESS;
}

/**
  The notification of AcpiRestoreCallbackDone used for restoring ACPI mode.

  @param [in] Protocol              Points to the protocol's unique identifier.
  @param [in] Interface             Points to the interface instance.
  @param [in] Handle                The handle on which the interface was installed.

  @retval EFI_SUCCESS               This routine was handled successfully.

**/
EFI_STATUS
RestoreAcpiCallbackDone (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                    Handle
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
RegisterAcpiCallbacks (
  VOID
  )
{
  EFI_STATUS        Status;
  EFI_EVENT         AcpiEnableCallbackStartEvent;
  EFI_EVENT         AcpiDisableCallbackStartEvent;
  EFI_EVENT         AcpiRestoreCallbackStartEvent;
  EFI_EVENT         AcpiRestoreCallbackDoneEvent;

  //
  // Register ACPI enable handler
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gAcpiEnableCallbackStartProtocolGuid,
                    EnableAcpiCallbackStart,
                    &AcpiEnableCallbackStartEvent
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Register ACPI disable handler
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gAcpiDisableCallbackStartProtocolGuid,
                    DisableAcpiCallbackStart,
                    &AcpiDisableCallbackStartEvent
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Register ACPI restore handler
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gAcpiRestoreCallbackStartProtocolGuid,
                    RestoreAcpiCallbackStart,
                    &AcpiRestoreCallbackStartEvent
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmRegisterProtocolNotify (
                    &gAcpiRestoreCallbackDoneProtocolGuid,
                    RestoreAcpiCallbackDone,
                    &AcpiRestoreCallbackDoneEvent
                    );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  S3 sleep entry callback for save boot current to CMOS using gEfiSmmSxDispatch2ProtocolGuid

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[in, out] CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS             The S3 sleep entry callabck was handled successfully.

**/
EFI_STATUS
EFIAPI
S3SleepEntryCallBack (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS        Status;

  POST_CODE (SMM_S3_SLEEP_CALLBACK);

  AsmWbinvd ();

  UpdatePmeSetting();
  Status = OemSvcS3Callback ();

  WakeToProcessPendingCapsule (mAcpiPmBaseAddr, FixedPcdGet16 (PcdSecureFlashWakeFromS3Time));

  return EFI_SUCCESS;
}

/**
  S4 sleep entry callback for save boot current to CMOS using gEfiSmmSxDispatch2ProtocolGuid

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[in, out] CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS             The S4 sleep entry callabck was handled successfully.

**/
EFI_STATUS
EFIAPI
S4SleepEntryCallBack (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS        Status;

  POST_CODE (SMM_S4_SLEEP_CALLBACK);

  UpdatePmeSetting();
  Status = OemSvcS4Callback ();

  return EFI_SUCCESS;
}

/**
  S5 sleep entry callback for save boot current to CMOS using gEfiSmmSxDispatch2ProtocolGuid

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[in, out] CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS             The S5 sleep entry callabck was handled successfully.

**/
EFI_STATUS
EFIAPI
S5SleepEntryCallBack (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS        Status;
  UINT8             Buffer;

  POST_CODE (SMM_S5_SLEEP_CALLBACK);

  //
  // To see if S5LongRunTest is enabled or not.
  //
  if (mSetupVariable->S5LongRunTest == TRUE) {
    IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_B);
    Buffer = IoRead8 (PCAT_RTC_DATA_REGISTER);
    Buffer |= ALARM_INTERRUPT_ENABLE;
    IoWrite8 (PCAT_RTC_DATA_REGISTER, Buffer);
    Buffer = IoRead8 (mAcpiPmBaseAddr + PM1_EN_HIGH_BYTE);
    Buffer |= RTC_EVENT_ENABLE;
    IoWrite8 (mAcpiPmBaseAddr + PM1_EN_HIGH_BYTE, Buffer);
  }

  UpdatePmeSetting();
  Status = OemSvcS5Callback ();

  return EFI_SUCCESS;
}

/**
  S5 Sleep AcLoss callabck.

  @param [in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param [in]      Context         Points to an optional handler context which was specified when the
                                   handler was registered.
  @param [in, out] CommBuffer      A pointer to a collection of data in memory that will
                                   be conveyed from a non-SMM environment into an SMM environment.
  @param [in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS              The S5 Sleep AcLoss callaback was handled successfully.

**/
EFI_STATUS
EFIAPI
S5SleepAcLossCallBack (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS        Status;

  POST_CODE (SMM_S5_SLEEP_CALLBACK);

  Status = OemSvcS5AcLossCallback ();

  return EFI_SUCCESS;
}

/**
  The setting which is for S5WakeOnLan.

  @param                          None.

  @retval void.

**/
VOID
S5WakeOnLanSetting (
  VOID
  )
{
  UINTN  Bus, Dev, Func, Reg;
  UINTN  BusLimit, FuncLimit;
  UINT8  Value8, PmCap;
  UINT16 PmCsr, Value16;

  Bus = Dev = Func = Reg = 0;
  BusLimit = 0;
  FuncLimit = 0;
  Value8 = PmCap= 0;
  PmCsr = Value16 = 0;

  for ( Bus = 0; Bus <= BusLimit; Bus++) {
    for ( Dev = 0; Dev <= 0x1f; Dev++) {
      for ( Func = 0; Func <= FuncLimit; Func++ ) {
        Value16 = PciExpressRead16 (
                    PCI_EXPRESS_LIB_ADDRESS (
                      Bus,
                      Dev,
                      Func,
                      R_CLASS_CODE
                      )
                    );
        Value8 = PciExpressRead8 (
                    PCI_EXPRESS_LIB_ADDRESS (
                      Bus,
                      Dev,
                      Func,
                      R_MULTIFUNC_CHECK
                      )
                    );
        if (Value8 == 0xFF) {
          continue;
        }
        if (Func == 0 && (Value8 & 0x80)) {  // multi-function
          FuncLimit = 0x07;
        }
        Value8 = PciExpressRead8 (
                   PCI_EXPRESS_LIB_ADDRESS (
                     Bus,
                     Dev,
                     Func,
                     R_BASE_CODE
                     )
                   );
        if (Value8 == 0x02) {  // network controller
          PmCap = PciExpressRead8 (
                    PCI_EXPRESS_LIB_ADDRESS (
                      Bus,
                      Dev,
                      Func,
                      R_CAPABILITY_POINTER
                      )
                    );
          if (mSetupVariable->WakeOnPME) {
            PmCsr = PciExpressOr16 (
                      PCI_EXPRESS_LIB_ADDRESS (
                        Bus,
                        Dev,
                        Func,
                        PmCap + 0x04
                        ),
                      (0x8000 + 0x0100)
                      );
          } else {
            PciExpressAnd16 (
              PCI_EXPRESS_LIB_ADDRESS (
                Bus,
                Dev,
                Func,
                PmCap + 0x04
                ),
              (0xFEFF) // ~BIT8
              );
            PciExpressOr16 (
              PCI_EXPRESS_LIB_ADDRESS (
                Bus,
                Dev,
                Func,
                PmCap + 0x04
                ),
              0x8000 // BIT15
              );
          }
        }
        if (Value16 == 0x0604) {
          if ((PciExpressRead8 (
                 PCI_EXPRESS_LIB_ADDRESS (
                   Bus,
                   Dev,
                   Func,
                   R_SUB_BUS_NUM
                   )
                 ) > BusLimit)) {
            BusLimit = PciExpressRead8 (
                         PCI_EXPRESS_LIB_ADDRESS (
                           Bus,
                           Dev,
                           Func,
                           0x1A
                           )
                         );
          }
        }
      }
      FuncLimit = 0;
    }
  }
}

/**
  S5 Sleep WakeOnTime callBack.

  @param [in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param [in]      Context         Points to an optional handler context which was specified when the
                                   handler was registered.
  @param [in, out] CommBuffer      A pointer to a collection of data in memory that will
                                   be conveyed from a non-SMM environment into an SMM environment.
  @param [in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS              The S5SleepWakeOnTimeCallBack was handled successfully.

**/
EFI_STATUS
EFIAPI
S5SleepWakeOnTimeCallBack (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  S5WakeUpSetting (
    mAcpiPmBaseAddr,
    mSetupVariable
    );

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
RegisterSxHandlers (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_SX_DISPATCH2_PROTOCOL  *SmmSxDispatch2;
  EFI_SMM_SX_REGISTER_CONTEXT    SmmSxDispatchContext;
  EFI_HANDLE                     DispatchHandle;
  EFI_HANDLE                     WakeOnAcLossHandle;
  EFI_HANDLE                     WakeOnTimeHandle;

//[-start-240426-IB18160212-modify-]//
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSxDispatch2ProtocolGuid,
                    NULL,
                    (VOID**)&SmmSxDispatch2
                    );
  ASSERT_EFI_ERROR(Status);
//[-end-240426-IB18160212-modify-]//

  SmmSxDispatchContext.Type  = SxS3;
  SmmSxDispatchContext.Phase = SxEntry;
  Status = SmmSxDispatch2->Register (
                             SmmSxDispatch2,
                             S3SleepEntryCallBack,
                             &SmmSxDispatchContext,
                             &DispatchHandle
                             );
  ASSERT_EFI_ERROR(Status);

  SmmSxDispatchContext.Type  = SxS4;
  SmmSxDispatchContext.Phase = SxEntry;
  Status = SmmSxDispatch2->Register (
                             SmmSxDispatch2,
                             S4SleepEntryCallBack,
                             &SmmSxDispatchContext,
                             &DispatchHandle
                             );
  ASSERT_EFI_ERROR(Status);

  SmmSxDispatchContext.Type  = SxS5;
  SmmSxDispatchContext.Phase = SxEntry;
  Status = SmmSxDispatch2->Register (
                             SmmSxDispatch2,
                             S5SleepEntryCallBack,
                             &SmmSxDispatchContext,
                             &DispatchHandle
                             );
  ASSERT_EFI_ERROR(Status);

  Status = SmmSxDispatch2->Register (
                             SmmSxDispatch2,
                             S5SleepAcLossCallBack,
                             &SmmSxDispatchContext,
                             &WakeOnAcLossHandle
                             );
  ASSERT_EFI_ERROR(Status);

  Status = SmmSxDispatch2->Register (
                             SmmSxDispatch2,
                             S5SleepWakeOnTimeCallBack,
                             &SmmSxDispatchContext,
                             &WakeOnTimeHandle
                             );
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}

/**
  The callback which is for reseting the system.

  @param [in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param [in]      Context         Points to an optional handler context which was specified when the
                                   handler was registered.
  @param [in, out] CommBuffer      A pointer to a collection of data in memory that will
                                   be conveyed from a non-SMM environment into an SMM environment.
  @param [in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS              The OsResetSmi callback was handled successfully.

**/
EFI_STATUS
EFIAPI
OsResetSmi (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS      Status;

  Status = EFI_UNSUPPORTED;

  DEBUG ((EFI_D_INFO, "OsResetSmi Entry\n"));

  Status = OemSvcOsResetCallback ();
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Status = ProcessPendingCapsule ();
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Status = SmmCsSvcResetSystem (EfiResetCold);

  DEBUG ((EFI_D_INFO, "OsResetSmi Exit\n"));

  return EFI_SUCCESS;

}

EFI_STATUS
EFIAPI
RegisterSwHandlers (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SmmSwDispatch2;
  EFI_SMM_SW_REGISTER_CONTEXT    SmmSwDispatchContext;
  EFI_HANDLE                     DispatchHandle;

//[-start-240426-IB18160212-modify-]//
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID**)&SmmSwDispatch2
                    );
  ASSERT_EFI_ERROR (Status);
//[-end-240426-IB18160212-modify-]//

  //
  // Register the OS restart set features disable
  //
  SmmSwDispatchContext.SwSmiInputValue = SMM_OS_RESET_SMI_VALUE;
  Status = SmmSwDispatch2->Register (
                             SmmSwDispatch2,
                             OsResetSmi,
                             &SmmSwDispatchContext,
                             &DispatchHandle
                             );
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}

/**
  When a power button event happens, it shuts off the machine.

  @param [in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param [in]      Context         Points to an optional handler context which was specified when the
                                   handler was registered.
  @param [in, out] CommBuffer      A pointer to a collection of data in memory that will
                                   be conveyed from a non-SMM environment into an SMM environment.
  @param [in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS              The Power Button callaback was handled successfully.

**/
EFI_STATUS
EFIAPI
PowerButtonCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS        Status;
  UINT8             Value8;

  Status = OemSvcPowerButtonCallback ();

  while ((IoRead16 (
            mAcpiPmBaseAddr + R_FCH_ACPI_PM1_STATUS) & B_PWR_BTN_STATUS) != B_PWR_BTN_STATUS) {
  }

  //
  // To clear RTC wakeup status.
  //
  IoWrite8 (
    PCAT_RTC_ADDRESS_REGISTER,
    RTC_ADDRESS_REGISTER_C
    );
  Value8 = IoRead8 (PCAT_RTC_DATA_REGISTER);
  IoWrite16 (
    mAcpiPmBaseAddr + R_FCH_ACPI_PM1_STATUS,
    (UINT16) (IoRead16 (mAcpiPmBaseAddr + R_FCH_ACPI_PM1_STATUS) & B_RTC_STATUS)
    );

  S5WakeUpSetting (
    mAcpiPmBaseAddr,
    mSetupVariable
    );

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
RegisterPowerButtonHandlers (
  VOID
  )
{
  EFI_STATUS                               Status;
  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL  *PowerButtonDispatch2;
  EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT    PowerButtonContext;
  EFI_HANDLE                               PowerButtonHandle;

//[-start-240426-IB18160212-modify-]//
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmPowerButtonDispatch2ProtocolGuid,
                    NULL,
                    (VOID**)&PowerButtonDispatch2
                    );
  ASSERT_EFI_ERROR (Status);
//[-end-240426-IB18160212-modify-]//

  //
  // Register the power button SMM event
  //
  PowerButtonContext.Phase = EfiPowerButtonExit;
  Status = PowerButtonDispatch2->Register (
                                   PowerButtonDispatch2,
                                   PowerButtonCallback,
                                   &PowerButtonContext,
                                   &PowerButtonHandle
                                   );
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}

VOID
GetAcpiPmBase (
  )
{
  IoWrite8 (FCH_IOMAP_REGCD6, FCH_PMIOA_REG60 + 1);
  mAcpiPmBaseAddr = (UINT16) IoRead8 (FCH_IOMAP_REGCD7);
  mAcpiPmBaseAddr <<= 8;
  IoWrite8 (FCH_IOMAP_REGCD6, FCH_PMIOA_REG60);
  mAcpiPmBaseAddr |= (UINT16) IoRead8 (FCH_IOMAP_REGCD7);
}

/**
  A SMM driver for SMM platform initialization.

  @param  ImageHandle    Handle for this drivers loaded image protocol.
  @param  SystemTable    EFI system table

  @retval EFI_SUCCESS    The driver installed without error.

**/
EFI_STATUS
EFIAPI
SmmPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                    Status;

  POST_CODE (SMM_SMM_PLATFORM_INIT);

  //
  // Find Setup Variable
  //
  mSetupVariable = (SYSTEM_CONFIGURATION *)CommonGetVariableData (SETUP_VARIABLE_NAME, &gSystemConfigurationGuid);
  if (mSetupVariable == NULL) {
    ASSERT_EFI_ERROR(EFI_NOT_FOUND);
  }

  //
  //Get CBS Variable
  //
  mAmdCbsVariable = (CBS_CONFIG *)CommonGetVariableData (CBS_SYSTEM_CONFIGURATION_NAME, &gCbsSystemConfigurationGuid);
  if (mAmdCbsVariable == NULL) {
    ASSERT_EFI_ERROR(EFI_NOT_FOUND);
  }

  //
  // To get PMx60 AcpiPm1EvtBlk Address.
  //
  GetAcpiPmBase ();

  //
  // Register power button SMI handler
  //
  Status = RegisterPowerButtonHandlers ();
  ASSERT_EFI_ERROR(Status);

  //
  // Register Sx-state SMI handler
  //
  Status = RegisterSxHandlers ();
  ASSERT_EFI_ERROR(Status);

  //
  // Register software SMI handler
  //
  Status = RegisterSwHandlers ();
  ASSERT_EFI_ERROR(Status);

  //
  // Register ACPI Callback handler
  //
  Status = RegisterAcpiCallbacks ();
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}
