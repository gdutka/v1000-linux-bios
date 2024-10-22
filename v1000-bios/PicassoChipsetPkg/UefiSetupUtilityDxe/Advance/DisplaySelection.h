/** @file

;*******************************************************************************
;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _DISPLAY_SELECTION_H_
#define _DISPLAY_SELECTION_H_

#include <SetupUtility.h>

#include <Protocol/ComponentName2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/EdidDiscovered.h>
//
// the VOLATILE variable "ActiveDisplayInfo" stores all of the active video controller info to 
// provide the using of SCU. 
//
#define PLUG_IN_VIDEO_DISPLAY_INFO_VAR_NAME  L"PlugInVgaHandles"
#define INVALID_VARSTORE_ID                  0
#define NULL_HANDLE                          ((VOID *) 0)

#pragma pack (1)
//
// For the Plug-in device.
//
typedef struct _VIDEO_OUTPUT_PORT_INFO_ {
  ACPI_ADR_DEVICE_PATH      *PortAcpiADR;
  CHAR16                    *PortName;
} VIDEO_OUTPUT_PORT_INFO;

typedef struct _PLUG_IN_ACTIVE_VIDEO_CONTROLLER_NODE_ {
  EFI_HANDLE                       ControllerHandle;
  CHAR16                           *ControllerName;
  UINTN                            NumOfConnectedPort;
  VIDEO_OUTPUT_PORT_INFO           *ConnectedPortInfo;
  ACPI_ADR_DEVICE_PATH             *DefaultAcpiADR;
  ACPI_ADR_DEVICE_PATH             *SelectedAcpiADR;
} PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO;

#pragma pack ()

#define VIDEO_CONFIG_FORM_ID  0x25

EFI_STATUS
PlugInVgaUpdateInfo (
  VOID
  );

EFI_STATUS
PlugInVgaDisplaySelectionSave (
  VOID
  );

EFI_STATUS
PlugInVideoDisplaySelectionOption (
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  EFI_IFR_TYPE_VALUE                     *Value 
  );

EFI_STATUS
PlugInVideoDisplaySelectionLabel (
  IN  EFI_HII_HANDLE                         HiiHandle
  );

EFI_STATUS
ClearFormDataFromLabel (
  IN     EFI_HII_HANDLE                      HiiHandle,
  IN     EFI_FORM_ID                         FormID,
  IN     UINT16                              Label
  );
#endif
