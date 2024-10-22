/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "InstallVerbTableDxe.h"
#include "PiDxe.h"

#define pAzaliaSsid PcdGet32(PcdAzaliaSsid)
#define pAzaliaPinCfg PcdGet8(PcdAzaliaPinCfg)
#define pAzaliaSdin0 PcdGet8(PcdAzaliaSdin0)
#define pAzaliaSdin1 PcdGet8(PcdAzaliaSdin1)
#define pAzaliaSnoop PcdGet8(PcdAzaliaSnoop)
#define pAzaliaDummy PcdGet8(PcdAzaliaDummy)
#define pAzaliaFrontPanel PcdGet8(PcdAzaliaFrontPanel)
#define pFrontPanelDetected PcdGet8(PcdFrontPanelDetected)

EFI_EVENT FchCalloutDxeEvent;
VOID      *mFchCalloutDxeEventRegistration;

/**
  Entry point of this driver

  @param ImageHandle     Handle of driver image
  @param SystemTable     Point to EFI_SYSTEM_TABLE

  @retval EFI_OUT_OF_RESOURCES  Can not allocate memory resource
  @retval EFI_DEVICE_ERROR      Can not install the protocol instance
  @retval EFI_SUCCESS           Success to initialize the Pci host bridge.
**/
EFI_STATUS
EFIAPI
InstallVerbtableDxeInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->CreateEvent (
                   EVT_NOTIFY_SIGNAL,
                   TPL_CALLBACK,
                   FchCalloutDxeAzaliaInitCallBack,
                   NULL,
                   &FchCalloutDxeEvent                
                   );
  if (EFI_ERROR(Status)) {
    return EFI_ABORTED;
  }

  Status = gBS->RegisterProtocolNotify (
                  &gFchDxeCalloutProtocolGuid,
                  FchCalloutDxeEvent,
                  &mFchCalloutDxeEventRegistration
                  );

  return EFI_SUCCESS;
}

VOID
EFIAPI
FchCalloutDxeAzaliaInitCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS            Status;
  KERNEL_CONFIGURATION  KernelSetup;

  Status = GetKernelConfiguration (&KernelSetup);
  if (EFI_ERROR (Status)){
    return;
  }

  Status = SetAzaliaPolicy (&KernelSetup);
  gBS->CloseEvent (Event);
}

STATIC
EFI_STATUS
SetAzaliaPolicy (
  IN KERNEL_CONFIGURATION  *SetupVariable
  )
{
  EFI_STATUS                      Status;
  AZALIA_POLICY                   AzaliaPolicy;

  AzaliaPolicy.AzaliaController                           = SetupVariable->Azalia;
  AzaliaPolicy.AzaliaSsid                                 = pAzaliaSsid;
  AzaliaPolicy.AzaliaPinCfg                               = pAzaliaPinCfg;
  AzaliaPolicy.AZALIACONFIGINFO.AzaliaConfig.AzaliaSdin0  = pAzaliaSdin0;
  AzaliaPolicy.AZALIACONFIGINFO.AzaliaConfig.AzaliaSdin1  = pAzaliaSdin1;
  AzaliaPolicy.AzaliaSnoop                                = pAzaliaSnoop;

//
// different part.
//
  AzaliaPolicy.AzaliaDummy                                = pAzaliaDummy;
  AzaliaPolicy.AzaliaFrontPanel                           = pAzaliaFrontPanel;
  AzaliaPolicy.FrontPanelDetected                         = pFrontPanelDetected;

  Status = gRT->SetVariable (
                  AZALIA_POLICY_VARIABLE,
                  &gAzaliaPolicyGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof(AzaliaPolicy),
                  (VOID *)(&AzaliaPolicy)
                  );

  return Status;
}