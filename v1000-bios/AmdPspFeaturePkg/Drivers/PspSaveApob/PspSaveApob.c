/* $NoKeywords:$ */
/**
 * @file
 *
 * A Driver used to save APOB to BIOS DIR APOB NV entry
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspSmm Driver
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2015 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
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
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCommunication.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/SmmFwBlockService.h>
#include <Library/AmdPspBaseLibV2.h>
#include "AGESA.h"
#include <Library/AmdPspApobLib.h>

extern EFI_GUID gPspApobSmmCommHandleGuid;

EFI_STATUS
PspSaveApobSmmCommunicateHandler (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context,
  IN OUT VOID    *CommBuffer,
  IN OUT UINTN   *CommBufferSize
  )
{
  UINT8 *ApobS3Buffer;
  UINT32 ApobS3BufferSize;
  UINTN _ApobS3BufferSize;
  EFI_STATUS                              Status;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL     *mSmmFwbServiceProtocol;
  TYPE_ATTRIB TypeAttrib;
  UINT64 ApobNVEntryAddress;
  UINTN  ApobNVEntrySize;
  UINT64 Ignored;

  ApobS3Buffer = NULL;
  ApobS3BufferSize = 0;
  ApobNVEntryAddress = 0;
  ApobNVEntrySize = 0;
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    &mSmmFwbServiceProtocol
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  PspPrepareApobS3Buffer (&ApobS3Buffer, &ApobS3BufferSize);
  ASSERT (ApobS3Buffer != NULL);
  if (ApobS3Buffer == NULL) {
    return EFI_UNSUPPORTED;
  }
  BIOSEntryInfo (APOB_NV_COPY, &TypeAttrib, &ApobNVEntryAddress, (UINT32 *)&ApobNVEntrySize, &Ignored);

  ASSERT (ApobS3BufferSize <=  ApobNVEntrySize);

  if (ApobS3BufferSize > ApobNVEntrySize) {
    return EFI_UNSUPPORTED;
  }

  _ApobS3BufferSize = ApobS3BufferSize;
  mSmmFwbServiceProtocol->Write (mSmmFwbServiceProtocol, (UINTN) ApobNVEntryAddress, &_ApobS3BufferSize, ApobS3Buffer);

  //Check if the data have been written to SPI completely
  ASSERT (_ApobS3BufferSize == ApobS3BufferSize);
  if (_ApobS3BufferSize != ApobS3BufferSize) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

#define PSP_SMM_COMMUNICATION_BUFFER_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data))
VOID
PspSaveApobReadyToBoot (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                           Status;
  UINTN       SmmCommBufferSize;
  EFI_SMM_COMMUNICATION_PROTOCOL  *SmmCommunication;
  UINT8       PspSmmCommBuf [PSP_SMM_COMMUNICATION_BUFFER_SIZE];
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;

  //Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  //Communicate APOB SMM Handle
  SmmCommunication = NULL;
  Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &SmmCommunication);
  ASSERT_EFI_ERROR (Status);

  if (SmmCommunication != NULL) {
    //Init SMM communication buffer header
    ZeroMem(PspSmmCommBuf, PSP_SMM_COMMUNICATION_BUFFER_SIZE);
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) &PspSmmCommBuf;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspApobSmmCommHandleGuid);
    SmmCommBuff->MessageLength = 0;
    //Communicate PSP SMM handler to save PSP BAR3 related register to boot script
    SmmCommBufferSize = PSP_SMM_COMMUNICATION_BUFFER_SIZE;
    SmmCommunication->Communicate (SmmCommunication, &PspSmmCommBuf, &SmmCommBufferSize);
  }
}

EFI_STATUS
PspSaveApobEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_SMM_BASE2_PROTOCOL               *SmmBase;
  BOOLEAN                              InSmm;
  EFI_EVENT                   ExitBootServicesEvent;
  EFI_HANDLE                      Handle;


  //If PSP feature turn off, exit the driver
  if ((CheckPspDevicePresent () == FALSE) ||
      (PcdGetBool (PcdAmdPspEnable) == FALSE)) {
    return EFI_UNSUPPORTED;
  }
  //
  // SMM check
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  } else {
    InSmm = FALSE;
  }

  if (!InSmm) {
    //Register Ready to boot event for APOB SMM handler
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    PspSaveApobReadyToBoot,
                    NULL,
                    &gEfiEventReadyToBootGuid,
                    &ExitBootServicesEvent
                    );
  } else {
    //Register SMM communication handler for work funciton of save APOB
    Handle = NULL;
    Status = gSmst->SmiHandlerRegister (PspSaveApobSmmCommunicateHandler, &gPspApobSmmCommHandleGuid, &Handle);
    ASSERT_EFI_ERROR (Status);
  }
  return Status;
}

