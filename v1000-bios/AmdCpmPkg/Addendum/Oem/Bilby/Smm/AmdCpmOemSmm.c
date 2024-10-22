/*****************************************************************************
 *
 * Copyright (C) 2017-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmSmm.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Protocol/SmmVariable.h>
#include <Dxe/AmdPbsSetupDxe/AmdPbsConfig.h>
#include <Library/AmdPbsConfigLib/AmdPbsConfigLib.h>
#include <Protocol/SmmPowerButtonDispatch2.h>

#define MXM_RESET                   0x6        // FCH GPIO140
//#define SSD_RESET                 0x6          // FCH GPIO40

CPM_DEFINE_gBS;
CPM_DEFINE_gSmst;

STATIC AMD_CPM_TABLE_PROTOCOL *gCpmTableSmmProtocol;

EFI_STATUS
EFIAPI
AssertResetForPcieDevice (
  VOID
  );

EFI_STATUS
EFIAPI
AmdCpmOemS3Callback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *DispatchContext OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

EFI_STATUS
EFIAPI
AmdCpmOemS4Callback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *DispatchContext OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

EFI_STATUS
EFIAPI
AmdCpmOemS5Callback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *DispatchContext OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

EFI_STATUS
EFIAPI
PowerButtonCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *DispatchContext OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM Adaptive S4 SMM driver
 *
 * This function registers the callback function before go to S4.
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmOemSmmEntryPoint (
  IN      EFI_HANDLE                  ImageHandle,
  IN      EFI_SYSTEM_TABLE            *SystemTable
  )
{
  EFI_STATUS                              Status;
  CPM_SMM_SX_DISPATCH_PROTOCOL            *SxDispatch;
  CPM_SMM_SX_DISPATCH_CONTEXT             SxContext;
  EFI_HANDLE                              SxHandle;
  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL *PowerButtonDispatch2;
  EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT   PowerButtonContext;
  EFI_HANDLE                              PowerButtonHandle;

  If_CpmInSmm (ImageHandle, SystemTable, gBS, gSmst, Status) {

    //
    // Initialize global variables
    //
    Status = gSmst->SmmLocateProtocol (
                                   &gAmdCpmTableSmmProtocolGuid,
                                   NULL,
                                   (VOID**)&gCpmTableSmmProtocol );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_INFO, "OEM-SMM-AmdCpmOemSmmEntryPoint-End-1-Status = %r\n", Status));
      return Status;
    }

    //
    //  Locate SMM Sx dispatch protocol
    //
    Status = CpmSmmLocateProtocol ( &gCpmSmmSxDispatchProtocolGuid,
                                    NULL,
                                    (VOID**)&SxDispatch );
    ASSERT_EFI_ERROR (Status);

    //
    //  Locate SMM Power Button  dispatch protocol
    //
    Status = CpmSmmLocateProtocol ( &gEfiSmmPowerButtonDispatch2ProtocolGuid,
                                    NULL,
                                    (VOID**)&PowerButtonDispatch2 );
    ASSERT_EFI_ERROR (Status);

    SxContext.Type  = SxS3;
    SxContext.Phase = SxEntry;
    Status = SxDispatch->Register ( SxDispatch,
                                    AmdCpmOemS3Callback,
                                    &SxContext,
                                    &SxHandle );
    DEBUG ((DEBUG_INFO, "OEM-SMM-AmdCpmOemSmmEntryPoint-Register-AmdCpmOemS3Callback %r\n", Status));

    SxContext.Type  = SxS4;
    SxContext.Phase = SxEntry;
    Status = SxDispatch->Register ( SxDispatch,
                                    AmdCpmOemS4Callback,
                                    &SxContext,
                                    &SxHandle );
    DEBUG ((DEBUG_INFO, "OEM-SMM-AmdCpmOemSmmEntryPoint-Register-AmdCpmOemS4Callback %r\n", Status));

    SxContext.Type  = SxS5;
    SxContext.Phase = SxEntry;
    Status = SxDispatch->Register ( SxDispatch,
                                    AmdCpmOemS5Callback,
                                    &SxContext,
                                    &SxHandle );
    DEBUG ((DEBUG_INFO, "OEM-SMM-AmdCpmOemSmmEntryPoint-Register-AmdCpmOemS5Callback %r\n", Status));

    //
    // Register the power button SMM event
    //
    PowerButtonContext.Phase = EfiPowerButtonExit;
    Status = PowerButtonDispatch2->Register ( PowerButtonDispatch2,
                                              PowerButtonCallback,
                                              &PowerButtonContext,
                                              &PowerButtonHandle );
    DEBUG ((DEBUG_INFO, "OEM-SMM-AmdCpmOemSmmEntryPoint-Register-PowerButtonCallback %r\n", Status));
  }

  return Status;
}

EFI_STATUS
EFIAPI
AmdCpmOemS3Callback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *DispatchContext OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS                      Status;
  AMD_PBS_SETUP_OPTION            AmdPbsConfiguration;
  UINT32                          Value32;
  UINTN                           VarSize;
  EFI_SMM_VARIABLE_PROTOCOL       *SmmVariable;

  Status = EFI_SUCCESS;
  Status = AssertResetForPcieDevice ();

  Status = CpmSmmLocateProtocol ( &gEfiSmmVariableProtocolGuid,
                                  NULL,
                                  (VOID **)&SmmVariable );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-SMM-AmdCpmOemS3Callback-End-1-Status = %r\n", Status));
    return Status;
  }

  VarSize = sizeof (AMD_PBS_SETUP_OPTION);
  Status = SmmVariable->SmmGetVariable ( AMD_PBS_SETUP_VARIABLE_NAME,
                                         &gAmdPbsSystemConfigurationGuid,
                                         NULL,
                                         &VarSize,
                                         &AmdPbsConfiguration );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-SMM-AmdCpmOemS3Callback-End-2-Status = %r\n", Status));
    return Status;
  }

  if (AmdPbsConfiguration.AmdBlinkLed == 1) {
    Value32 = gCpmTableSmmProtocol->CommonFunction.MmioRead32 (0xFED80300 + 0xE4);
    Value32 &= ~(UINT32)(BIT1 + BIT0);
    Value32 |= BIT0;
    gCpmTableSmmProtocol->CommonFunction.MmioWrite32 (0xFED80300 + 0xE4, Value32);
  }

  return Status;
}

EFI_STATUS
EFIAPI
AmdCpmOemS4Callback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *DispatchContext OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS                      Status;
  AMD_PBS_SETUP_OPTION            AmdPbsConfiguration;
  UINT32                          Value32;
  UINTN                           VarSize;
  EFI_SMM_VARIABLE_PROTOCOL       *SmmVariable;

  Status = EFI_SUCCESS;
  Status = AssertResetForPcieDevice ();

  Status = CpmSmmLocateProtocol ( &gEfiSmmVariableProtocolGuid,
                                  NULL,
                                  (VOID **)&SmmVariable );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-SMM-AmdCpmOemS4Callback-End-1-Status = %r\n", Status));
    return Status;
  }

  VarSize = sizeof (AMD_PBS_SETUP_OPTION);
  Status = SmmVariable->SmmGetVariable ( AMD_PBS_SETUP_VARIABLE_NAME,
                                         &gAmdPbsSystemConfigurationGuid,
                                         NULL,
                                         &VarSize,
                                         &AmdPbsConfiguration );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-SMM-AmdCpmOemS4Callback-End-2-Status = %r\n", Status));
    return Status;
  }

  if (AmdPbsConfiguration.AmdBlinkLed == 1) {
    Value32 = gCpmTableSmmProtocol->CommonFunction.MmioRead32 (0xFED80300 + 0xE4);
    Value32 &= ~(UINT32)(BIT1 + BIT0);
    gCpmTableSmmProtocol->CommonFunction.MmioWrite32 (0xFED80300 + 0xE4, Value32);
  }

  return Status;
}


EFI_STATUS
EFIAPI
AmdCpmOemS5Callback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *DispatchContext OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS                      Status;
  AMD_PBS_SETUP_OPTION            AmdPbsConfiguration;
  UINT32                          Value32;
  UINTN                           VarSize;
  EFI_SMM_VARIABLE_PROTOCOL       *SmmVariable;

  Status = EFI_SUCCESS;
  Status = AssertResetForPcieDevice ();

  Status = CpmSmmLocateProtocol ( &gEfiSmmVariableProtocolGuid,
                                  NULL,
                                  (VOID **)&SmmVariable );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-SMM-AmdCpmOemS5Callback-End-1-Status = %r\n", Status));
    return Status;
  }

  VarSize = sizeof (AMD_PBS_SETUP_OPTION);
  Status = SmmVariable->SmmGetVariable ( AMD_PBS_SETUP_VARIABLE_NAME,
                                         &gAmdPbsSystemConfigurationGuid,
                                         NULL,
                                         &VarSize,
                                         &AmdPbsConfiguration );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-SMM-AmdCpmOemS5Callback-End-2-Status = %r\n", Status));
    return Status;
  }

  if (AmdPbsConfiguration.AmdBlinkLed == 1) {
    Value32 = gCpmTableSmmProtocol->CommonFunction.MmioRead32 (0xFED80300 + 0xE4);
    Value32 &= ~(UINT32)(BIT1 + BIT0);
    gCpmTableSmmProtocol->CommonFunction.MmioWrite32 (0xFED80300 + 0xE4, Value32);
  }

  return Status;
}


EFI_STATUS
EFIAPI
PowerButtonCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *DispatchContext OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{

  EFI_STATUS                      Status;
  AMD_PBS_SETUP_OPTION            AmdPbsConfiguration;
  UINT32                          Value32;
  UINTN                           VarSize;
  EFI_SMM_VARIABLE_PROTOCOL       *SmmVariable;

  Status = EFI_SUCCESS;
  Status = AssertResetForPcieDevice ();

  Status = CpmSmmLocateProtocol ( &gEfiSmmVariableProtocolGuid,
                                  NULL,
                                  (VOID **)&SmmVariable );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-SMM-PowerButtonCallback-End-1-Status = %r\n", Status));
    return Status;
  }

  VarSize = sizeof (AMD_PBS_SETUP_OPTION);
  Status = SmmVariable->SmmGetVariable ( AMD_PBS_SETUP_VARIABLE_NAME,
                                         &gAmdPbsSystemConfigurationGuid,
                                         NULL,
                                         &VarSize,
                                         &AmdPbsConfiguration );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-SMM-PowerButtonCallback-End-2-Status = %r\n", Status));
    return Status;
  }

  if (AmdPbsConfiguration.AmdBlinkLed == 1) {
    Value32 = gCpmTableSmmProtocol->CommonFunction.MmioRead32 (0xFED80300 + 0xE4);
    Value32 &= ~(UINT32)(BIT1 + BIT0);
    gCpmTableSmmProtocol->CommonFunction.MmioWrite32 (0xFED80300 + 0xE4, Value32);
  }

  return Status;
}


EFI_STATUS
EFIAPI
AssertResetForPcieDevice (
  VOID
  )
{
  UINT32       Value32;

  gCpmTableSmmProtocol->CommonFunction.SetGpio (gCpmTableSmmProtocol, MXM_RESET, 0);
  //gCpmTableSmmProtocol->CommonFunction.SetGpio (gCpmTableSmmProtocol, SSD_RESET, 0);

  Value32 = gCpmTableSmmProtocol->CommonFunction.MmioRead32 (0xFED81500 + 0x18);
  Value32 &= ~(BIT22);
  gCpmTableSmmProtocol->CommonFunction.MmioWrite32 (0xFED81500 + 0x18, Value32);

  return EFI_SUCCESS;
}

