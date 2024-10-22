/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access MMIO registers.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "Gnb.h"
#include  "GnbLibMemAcc.h"
#include  <Library/AmdS3SaveLib.h>

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBCOMMONLIB_GNBLIBMEMACC_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Write Memory/MMIO registers
 *
 *
 *
 * @param[in] Address         Physical Address
 * @param[in] Width           Access width
 * @param[in] Value           Pointer to value
 * @param[in] StdHeader       Standard configuration header
 */

VOID
GnbLibMemWrite (
  IN      UINT64        Address,
  IN      ACCESS_WIDTH  Width,
  IN      VOID          *Value,
  IN      VOID          *StdHeader
  )
{
  if (Width >= AccessS3SaveWidth8) {
    if (Width == AccessS3SaveWidth64) {
      AmdS3SaveScriptMemWrite (AccessS3SaveWidth32 - 0x80, Address, Value);
      AmdS3SaveScriptMemWrite (AccessS3SaveWidth32 - 0x80, Address + 4, (VOID *) ((UINT64)Value + 4));
    } else {
      AmdS3SaveScriptMemWrite (Width - 0x80, Address, Value);
    }
  }
  if ((Width == AccessWidth64) || (Width == AccessS3SaveWidth64)) {
    LibAmdMemWrite (AccessWidth32, Address, Value, StdHeader);
    LibAmdMemWrite (AccessWidth32, Address + 4, (VOID *) ((UINT64)Value + 4), StdHeader);
  } else {
    LibAmdMemWrite (Width, Address, Value, StdHeader);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read Memory/MMIO registers
 *
 *
 *
 * @param[in]   Address         Physical Address
 * @param[in]   Width           Access width
 * @param[out]  Value           Pointer to value
 * @param[in]   StdHeader       Standard configuration header
 */

VOID
GnbLibMemRead (
  IN       UINT64       Address,
  IN       ACCESS_WIDTH Width,
     OUT   VOID         *Value,
  IN       VOID         *StdHeader
  )
{
  if (Width == AccessWidth64) {
    LibAmdMemRead (AccessWidth32, Address, Value, StdHeader);
    LibAmdMemRead (AccessWidth32, Address + 4, (VOID *) ((UINT64)Value + 4), StdHeader);
  } else {
    LibAmdMemRead (Width, Address, Value, StdHeader);
  }
}





