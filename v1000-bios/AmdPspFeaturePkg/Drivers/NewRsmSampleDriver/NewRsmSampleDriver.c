/* $NoKeywords:$ */
/**
 * @file
 *
 * PspResumeService usage reference Codes
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 281178 $   @e \$Date: 2013-12-18 02:14:15 -0600 (Wed, 18 Dec 2013) $
 *
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2014 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
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
#include "PspDebug.h"
#include <AmdPspBaseLibV2.h>
#include <Protocol/PspResumeServiceProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>

UINT32 PspResumeCallback2Context = 0xDB000003;

EFI_STATUS
PspResumeCallback1 (
  IN  RESUME_TYPE   ResumeType,
  IN  VOID          *Context
  )
{
  UINT32 Value;
  if (ResumeType == ResumeFromConnectedStandby) {
    Value = 0xDB000001;
    LibAmdIoWrite (AccessWidth32, 0xE0, &Value, NULL);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
PspResumeCallback2 (
  IN  RESUME_TYPE   ResumeType,
  IN  VOID          *Context
  )
{
  UINT32 Value;
  if (ResumeType == ResumeFromConnectedStandby) {
    Value = 0xDB000002;
    LibAmdIoWrite (AccessWidth32, 0xE0, &Value, NULL);
    LibAmdIoWrite (AccessWidth32, 0xE0, Context, NULL);
  } else if (ResumeType == ResumeFromS3) {
    Value = 0xDB000004;
    LibAmdIoWrite (AccessWidth32, 0xE0, &Value, NULL);
  }
  return EFI_SUCCESS;

}

EFI_STATUS
PspResumeCallback3 (
  IN  RESUME_TYPE   ResumeType,
  IN  VOID          *Context
  )
{
  UINT32 Value;
  if (ResumeType == ResumeFromConnectedStandby) {
    Value = 0xDB000005;
    LibAmdIoWrite (AccessWidth32, 0xE0, &Value, NULL);
  }
  return EFI_SUCCESS;
}


EFI_STATUS
NewRsmDrvEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  PSP_RESUME_SERVICE_PROTOCOL *PspRsmServiceProtocol;

  //If PSP feature turn off, exit the driver
  Status = CheckPspDevicePresent ();
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  PSP_DEBUG ("NewRsmDrv Running in SMM\n");
  //In security consideration, all resume callback routine should put in SMM ram
  Status = gSmst->SmmLocateProtocol (
                  &gPspResumeServiceProtocolGuid,
                  NULL,
                  &PspRsmServiceProtocol
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //Big number denotes the high priority.
  //In above example, the calling sequence will be
  //Callback2 -> Callback3 -> Callback1
  Status = PspRsmServiceProtocol->Register (
                                PspRsmServiceProtocol,
                                &PspResumeCallback1,
                                NULL,
                                1,
                                NULL
                                );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //Sample with Context
  Status = PspRsmServiceProtocol->Register (
                                PspRsmServiceProtocol,
                                &PspResumeCallback2,
                                &PspResumeCallback2Context,
                                3,
                                NULL
                                );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PspRsmServiceProtocol->Register (
                                PspRsmServiceProtocol,
                                &PspResumeCallback3,
                                NULL,
                                2,
                                NULL
                                );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}