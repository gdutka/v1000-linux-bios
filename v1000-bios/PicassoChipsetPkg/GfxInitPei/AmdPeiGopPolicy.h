/*****************************************************************************
 *
 * Copyright 2015 - 2017 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
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
#ifndef _AMD_PEI_GOP_POLICY_H_
#define _AMD_PEI_GOP_POLICY_H_

#define AMD_PEI_GOP_FORMAT_REVISION          1
#define AMD_PEI_GOP_CONTENT_REVISION         1

#pragma pack(1)

typedef struct _AMD_PEI_GOP_POLICY AMD_PEI_GOP_POLICY_V1_1;

typedef struct _AMD_PEI_GOP_POLICY_HEADER
{
  UINT16 TableSize;
  UINT8  FormatRevision;   //mainly used for a hw function, when the parser is not backward compatible 
  UINT8  ContentRevision;  //change it when a data table has a structure change, or a hw function has a input/output parameter change                                
} AMD_PEI_GOP_POLICY_HEADER;

struct _AMD_PEI_GOP_POLICY {
  AMD_PEI_GOP_POLICY_HEADER   Header;
  UINT32                      GfxBar0;                // BAR0: PciReg 0x10
  UINT32                      GfxBar1;                // BAR1: PciReg 0x14
  UINT32                      GfxBar2;                // BAR2: PciReg 0x18
  UINT32                      GfxBar3;                // BAR3: PciReg 0x1C
  UINT32                      GfxBar4;                // BAR4: PciReg 0x20
  UINT32                      GfxBar5;                // BAR5: PciReg 0x24         
  VOID                        *VbiosImage;            // VBIOS image ptr
  BOOLEAN                     LidStatus;              // 1:Lid Close 0:Lid Open
  UINT32                      HorizontalResolution;   // Requested horizontal resolution 
  UINT32                      VerticalResolution;     // Requested vertical resolution
};
#pragma pack()

#endif