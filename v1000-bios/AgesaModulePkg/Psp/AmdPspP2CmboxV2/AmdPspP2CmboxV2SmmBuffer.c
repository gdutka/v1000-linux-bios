/*****************************************************************************
 *
 * Copyright 2019 - 2019 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software and documentation (if
 * any) (collectively, the "Materials") pursuant to the terms and conditions of
 * the Software License Agreement included with the Materials.  If you do not
 * have a copy of the Software License Agreement, contact your AMD
 * representative for a copy.
 *
 * You agree that you will not reverse engineer or decompile the Materials, in
 * whole or in part, except as allowed by applicable law.
 *
 * WARRANTY DISCLAIMER:  THE MATERIALS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE
 * MATERIALS WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM
 * CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE
 * OF THE MATERIAL IS ASSUMED BY YOU.  Some jurisdictions do not allow the
 * exclusion of implied warranties, so the above exclusion may not apply to
 * You.
 *
 * LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL
 * NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
 * INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
 * THE MATERIALS OR THIS AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  In no event shall AMD's total
 * liability to You for all damages, losses, and causes of action (whether in
 * contract, tort (including negligence) or otherwise) exceed the amount of
 * $100 USD. You agree to defend, indemnify and hold harmless AMD and its
 * licensors, and any of their directors, officers, employees, affiliates or
 * agents from and against any and all loss, damage, liability and other
 * expenses (including reasonable attorneys' fees), resulting from Your use of
 * the Materials or violation of the terms and conditions of this Agreement.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS:  The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgment of AMD's proprietary rights in them.
 *
 * EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as
 * stated in the Software License Agreement.
 *******************************************************************************
 */
#include <Base.h>
#include "AGESA.h"
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Protocol/PspMboxSmmBufferAddressProtocol.h>
#include <Filecode.h>

#define FILECODE PSP_AMDPSPP2CMBOXV2_AMDPSPP2CMBOXV2SMMBUFFER_FILECODE


#define PSP_SMM_BUFFER_ALIGNMENT    (0x20u) // Must be 32-byte aligned
#define PSP_MBOX_SMM_BUFFER_OFFSET  0u
#define PSP_MBOX_SMM_FLAG_OFFSET    0x00000C00 //Start from 3K


PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL mPspMboxSmmBufferAddressProtocol;

EFI_STATUS
EFIAPI
AmdPspP2CmboxV2SmmBufferEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINT8                           *LocalPspMboxSmmBuffer;
  UINT8                           *PspMboxSmmBuffer;
  BOOLEAN                         *PspMboxSmmFlagAddr;
  EFI_HANDLE                      Handle;
  UINT32                          PspSmmBufferPages;

  LocalPspMboxSmmBuffer = NULL;
  PspMboxSmmBuffer = NULL;
  PspMboxSmmFlagAddr = NULL;
  PspSmmBufferPages = 0;

  AGESA_TESTPOINT (TpPspP2CmboxV2Entry, NULL);
  //We are now in SMM
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.AmdPspP2CmboxSmmBuffer Enter\n");
  PspSmmBufferPages = PcdGet32 (PcdAmdPspSmmBufferPages);

  // Allocate SMM buffer to hold C2P message after bootdone
  LocalPspMboxSmmBuffer = AllocateAlignedPages(PspSmmBufferPages, PSP_SMM_BUFFER_ALIGNMENT);
  ZeroMem (LocalPspMboxSmmBuffer, PspSmmBufferPages * EFI_PAGE_SIZE);
  ASSERT (LocalPspMboxSmmBuffer != NULL);
  if (LocalPspMboxSmmBuffer != NULL) {
    //Assign the address for C2P RT buffer, C2P SMM flag
    PspMboxSmmBuffer = LocalPspMboxSmmBuffer + PSP_MBOX_SMM_BUFFER_OFFSET;
    PspMboxSmmFlagAddr = LocalPspMboxSmmBuffer + PSP_MBOX_SMM_FLAG_OFFSET;
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPspMboxSmmBuffer 0x%x\n", PspMboxSmmBuffer);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPspMboxSmmFlagAddr 0x%x\n", PspMboxSmmFlagAddr);
    //Saving the address through SMM protocol
    Handle = NULL;
    mPspMboxSmmBufferAddressProtocol.PspMboxSmmBuffer = PspMboxSmmBuffer;
    mPspMboxSmmBufferAddressProtocol.PspMboxSmmFlagAddr = PspMboxSmmFlagAddr;
    gSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gPspMboxSmmBufferAddressProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mPspMboxSmmBufferAddressProtocol
                      );
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspP2CmboxSmmBuffer Exit\n");
  AGESA_TESTPOINT (TpPspP2CmboxV2Exit, NULL);

  return EFI_SUCCESS;
}



