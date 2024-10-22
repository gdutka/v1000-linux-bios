/** @file

***************************************************************************
* Copyright (c) 2013 - 2017, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/
/*****************************************************************************
 *
 * Copyright 2017 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/AmdPspBaseLibV1.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/FdSupportLib.h>
#include <Protocol/H2ODialog.h>
#include <Guid/H2OBdsCheckPoint.h>

#define WARNING_MESSAGE       L"Press Y to reset fTPM, if you have BitLocker or encryption-enabled system, the system will not boot without a recovery key\nPress N to keep previous fTPM record and continue system boot, fTPM will NOT be enabled with new CPU unless fTPM is reset (reinitialized), you could swap back to the old CPU to recover TPM related keys and data"

EFI_RUNTIME_SERVICES          *gRT;

/**
  This function shows a dialog to ask user that user want to rease PSP NVRAM when CPU is swapped

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
PspFactoryResetNotifyFunction (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  )
{
  EFI_STATUS                           Status;
  UINT32                               RegEax;
  UINT64                               EntryAddress;
  UINT32                               EntrySize;
  CHAR16                               *String = WARNING_MESSAGE;
  H2O_DIALOG_PROTOCOL                  *H2ODialog;
  EFI_INPUT_KEY                        Key;
  
  EntryAddress = 0;
  EntrySize = 0;
  RegEax = 0;
  
  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID **)&H2ODialog
                  );  
  if (EFI_ERROR (Status)) {
  	return;
  }

  while (1) {
    Status = H2ODialog->ConfirmDialog (
                        DlgYesNo,
                        FALSE,
                        0,
                        NULL,
                        &Key,
                        String
                        ); 
    if ((Status != EFI_SUCCESS) && (Status != EFI_NOT_READY)) {
  	  return;
    }
    if (Status == EFI_NOT_READY) {
      gBS->Stall (50000);
      continue;
    }
    if (Key.UnicodeChar != CHAR_CARRIAGE_RETURN) {
      return;
    } else {
      break;
    }
  }
  
  AsmCpuid (0x80000001, &RegEax, NULL, NULL, NULL);
  //
  // Check family of CPU
  //  
  if (((RegEax >> 20) & 0xFF) == 0x6) {
    DEBUG ((EFI_D_INFO, "    Family 15\n"));
    PSPEntryInfoV1 (PSP_NV_DATA, &EntryAddress, &EntrySize);
    DEBUG ((EFI_D_INFO, "    PSPEntryInfoV1  Addr = 0x%lx, Size = 0x%x\n", EntryAddress, EntrySize));
  } else if (((RegEax >> 20) & 0xFF) == 0x8) {  
    DEBUG ((EFI_D_INFO, "    Family 17\n"));
    PSPEntryInfoV2 (PSP_NV_DATA, &EntryAddress, &EntrySize);
    DEBUG ((EFI_D_INFO, "    PSPEntryInfoV2  Addr = 0x%lx, Size = 0x%x\n", EntryAddress, EntrySize));
  } else {
    ASSERT (FALSE);
  }
  
  //
  // Flash PSP NVRAM when both setup option is enabled, and gAmdPspFtpmFactoryResetPpiGuid is located
  //
  if ((EntryAddress != 0) && (EntrySize != 0)) {
    DEBUG ((EFI_D_INFO, "    Erasing PSP NVRAM...\n"));
    Status = FlashErase ((UINTN)EntryAddress, (UINTN)EntrySize);
    if (!EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INFO, "    FlashErase Success, Issue system reset\n"));
      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    } else {
      DEBUG ((EFI_D_ERROR, "    FlashErase Failed\n"));
      ASSERT (FALSE);
    }
  } 
}

EFI_STATUS
PspFactoryResetEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_EVENT                            PspFactoryResetEvent;
  VOID                                 *Registration;
  
  DEBUG ((EFI_D_INFO, "PspFactoryResetEntry Entry\n"));

  //
  //If PSP feature turn off, exit the driver
  //
  if ((PcdGetBool (PcdAmdPspEnable) == FALSE) ||
      (PcdGetBool (PcdAmdPspFactoryReset) == FALSE)) {
    DEBUG ((EFI_D_INFO, "PspFactoryResetEntry Exit\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // Register BdsCpDisplayBeforeProtocol notification to show dialog and erase PSP NVRAM
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  PspFactoryResetNotifyFunction,
                  NULL,
                  &PspFactoryResetEvent
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->RegisterProtocolNotify (
                    &gH2OBdsCpDisplayBeforeGuid,
                    PspFactoryResetEvent,
                    &Registration
                    );
  }
  DEBUG ((EFI_D_INFO, "PspFactoryResetEntry Exit\n")); 
  return Status;

}

