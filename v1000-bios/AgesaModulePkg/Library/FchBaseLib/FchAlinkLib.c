/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Base lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright 2008 - 2019 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/FchBaseLib.h>
#include "FchRegistersCommon.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_FCHBASELIB_FCHALINKLIB_FILECODE



/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 * FchReadAlink - Read the Alink Registers
 *
 *
 * @param[in] Index - The index of the Alink register
 * @param[in] StdHeader - Standard configuration header
 *
 *
 */
UINT32
FchReadAlink (
  IN       UINT32              Index,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 Data;
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_INDEX, &Index, StdHeader);
  LibAmdIoRead (AccessWidth32, ALINK_ACCESS_DATA, &Data, StdHeader);
  //Clear Index
  Index = 0;
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_INDEX, &Index, StdHeader);
  return Data;
}

/**
 * FchWriteAlink - Write the Alink Registers
 *
 *
 * @param[in] Index - The index of the Alink register
 * @param[in] Data - The Data of the Alink register
 * @param[in] StdHeader - Standard configuration header
 *
 *
 */
VOID
FchWriteAlink (
  IN       UINT32              Index,
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_INDEX, &Index, StdHeader);
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_DATA, &Data, StdHeader);
  //Clear Index
  Index = 0;
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_INDEX, &Index, StdHeader);
}

/**
 * FchRwAlink - Modify the Alink Registers
 *
 *
 * @param[in] Index - The index of the Alink register
 * @param[in] AndMask - The AND Mask of the Alink register
 * @param[in] OrMask - The OR Mask of the Alink register
 * @param[in] StdHeader - Standard configuration header
 *
 *
 */
VOID
FchRwAlink (
  IN       UINT32              Index,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 AccessType;

  AccessType = Index & 0xE0000000;
  if (AccessType == (AXINDC << 29)) {
    FchWriteAlink ((FCH_AX_INDXC_REG30 | AccessType), Index & 0x1FFFFFFF, StdHeader);
    Index = FCH_AX_DATAC_REG34 | AccessType;
  } else if (AccessType == (AXINDP << 29)) {
    FchWriteAlink ((FCH_AX_INDXP_REG38 | AccessType), Index & 0x1FFFFFFF, StdHeader);
    Index = FCH_AX_DATAP_REG3C | AccessType;
  }
  FchWriteAlink (Index, (FchReadAlink (Index, StdHeader) & AndMask) | OrMask, StdHeader);
}
