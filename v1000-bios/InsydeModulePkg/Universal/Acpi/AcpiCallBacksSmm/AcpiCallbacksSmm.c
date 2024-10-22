/** @file
  This driver is Common SMM AcpiCallbacks code

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiSmm.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseOemSvcKernelLib.h>

#include <Library/SmmOemSvcKernelLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/AcpiDisableCallbackDone.h>
#include <Protocol/AcpiEnableCallbackDone.h>
#include <Protocol/AcpiRestoreCallbackDone.h>

#include <SmiTable.h>
#include <PostCode.h>


STATIC
EFI_STATUS
EFIAPI
RegEnableAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

STATIC
EFI_STATUS
EFIAPI
RegDisableAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

STATIC
EFI_STATUS
EFIAPI
RegRestoreAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

EFI_HANDLE                          mAcpiCallbacksHandle = NULL;

/**
 Register EnableAcpiCallback, DisableAcpiCallback, RestoreAcpiCallback to Dispatcher

 @param [in]   ImageHandle      Pointer to the loaded image protocol for this driver
 @param [in]   SystemTable      Pointer to the EFI System Table

 @retval Status                 EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SmmAcpiCallbacksEntryPoint (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL         *SwDispatch2;
  EFI_SMM_SW_REGISTER_CONTEXT           SwContext;
  EFI_HANDLE                            SwHandle;

  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID **)&SwDispatch2);
  ASSERT_EFI_ERROR (Status);


  //
  // Register ACPI enable handler
  //
  SwContext.SwSmiInputValue = EFI_ACPI_ENABLE_SW_SMI;
  Status = SwDispatch2->Register (
    SwDispatch2,
    RegEnableAcpiCallback,
    &SwContext,
    &SwHandle
    );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "SmmCommunication SwSmi: %x\n", (UINTN)SwContext.SwSmiInputValue));

  //
  // Register ACPI disable handler
  //
  SwContext.SwSmiInputValue = EFI_ACPI_DISABLE_SW_SMI;
  Status = SwDispatch2->Register (
    SwDispatch2,
    RegDisableAcpiCallback,
    &SwContext,
    &SwHandle
    );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "SmmCommunication SwSmi: %x\n", (UINTN)SwContext.SwSmiInputValue));

  //
  // Register ACPI restore handler
  //
  SwContext.SwSmiInputValue = EFI_ACPI_RESTORE_SW_SMI;
  Status = SwDispatch2->Register (
    SwDispatch2,
    RegRestoreAcpiCallback,
    &SwContext,
    &SwHandle
    );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "SmmCommunication SwSmi: %x\n", (UINTN)SwContext.SwSmiInputValue));

  return EFI_SUCCESS;
}

/**
 SMI handler to enable ACPI mode
 Dispatched on reads from APM port with value EFI_ACPI_ENABLE_SW_SMI
 Install gAcpiEnableCallbackStartProtocolGuid, gAcpiEnableCallbackDoneProtocolGuid protocol
 So platform get notification callbacks
 Call OemServices, so OEM gets chances to customize
 Call OemSvcEcAcpiMode(TRUE) and Show POST code


 @retval None.

**/
STATIC
EFI_STATUS
EFIAPI
RegEnableAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS     OemSvcStatus;

  //
  // PostCode = 0xA6, OS call ACPI enable function
  //
  POST_CODE (SMM_ACPI_ENABLE_START);

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiEnableCallbackStartProtocolGuid,
             NULL
             );
  }

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiEnableCallbackStartProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
   
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEcAcpiMode \n"));
  OemSvcStatus = OemSvcEcAcpiMode (TRUE);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEcAcpiMode Status: %r\n", OemSvcStatus));
  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEnableAcpiCallback \n"));
  OemSvcStatus = OemSvcEnableAcpiCallback ();
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEnableAcpiCallback Status: %r\n", OemSvcStatus));

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiEnableCallbackDoneProtocolGuid,
             NULL
             );
  }

  //
  // PostCode = 0xA7, ACPI enable function complete
  //
  POST_CODE (SMM_ACPI_ENABLE_END);

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiEnableCallbackDoneProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );

  return EFI_SUCCESS;
}

/**
 SMI handler to disable ACPI mode
 Dispatched on reads from APM port with value EFI_ACPI_DISABLE_SW_SMI
 Install gAcpiDisableCallbackStartProtocolGuid, gAcpiDisableCallbackDoneProtocolGuid protocol
 So platform get notification callbacks
 Call OemServices, so OEM gets chances to customize
 Call OemSvcEcAcpiMode(FALSE) and Show POST code


 @retval None.

**/
STATIC
EFI_STATUS
EFIAPI
RegDisableAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS     OemSvcStatus;

  //
  // PostCode = 0xA8, OS call ACPI disable function
  //
  POST_CODE (SMM_ACPI_DISABLE_START);

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiDisableCallbackStartProtocolGuid,
             NULL
             );
  }

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiDisableCallbackStartProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEcAcpiMode \n"));
  OemSvcStatus = OemSvcEcAcpiMode (FALSE);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEcAcpiMode Status: %r\n", OemSvcStatus));

  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDisableAcpiCallback \n"));  
  OemSvcStatus = OemSvcDisableAcpiCallback ();
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDisableAcpiCallback Status: %r\n", OemSvcStatus));

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiDisableCallbackDoneProtocolGuid,
             NULL
             );
  }

  //
  // PostCode = 0xA9, ACPI disable function complete
  //
  POST_CODE (SMM_ACPI_DISABLE_END);

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiDisableCallbackDoneProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );

  return EFI_SUCCESS;
}

/**
 SMI handler to disable ACPI mode
 Dispatched on reads from APM port with value EFI_ACPI_DISABLE_SW_SMI
 Install gAcpiDisableCallbackStartProtocolGuid, gAcpiDisableCallbackDoneProtocolGuid protocol
 So platform get notification callbacks
 Call OemServices, so OEM gets chances to customize
 Call OemSvcEcAcpiMode(TRUE)


 @retval None.

**/
STATIC
EFI_STATUS
EFIAPI
RegRestoreAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS     OemSvcStatus;

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiRestoreCallbackStartProtocolGuid,
             NULL
             );
  }

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiRestoreCallbackStartProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEcAcpiMode \n"));
  OemSvcStatus = OemSvcEcAcpiMode (TRUE);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEcAcpiMode Status: %r\n", OemSvcStatus));
  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcRestoreAcpiCallback \n")); 
  OemSvcStatus = OemSvcRestoreAcpiCallback ();
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcRestoreAcpiCallback Status: %r\n", OemSvcStatus));

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiRestoreCallbackDoneProtocolGuid,
             NULL
             );
  }

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiRestoreCallbackDoneProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );

  return EFI_SUCCESS;
}

