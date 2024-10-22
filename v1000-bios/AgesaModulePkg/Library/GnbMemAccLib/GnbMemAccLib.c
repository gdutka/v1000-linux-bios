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
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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
#include  "Porting.h"
#include  "AMD.h"
#include  "Library/AmdBaseLib.h"
#include  "Library/GnbMemAccLib.h"
#include  <Library/AmdS3SaveLib.h>
#include  "Filecode.h"
#define FILECODE LIBRARY_GNBMEMACCLIB_GNBMEMACCLIB_FILECODE
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
      AmdS3SaveScriptMemWrite (AccessS3SaveWidth32 - 0x80, Address + 4, (VOID *) ((UINTN)Value + 4));
    } else {
      AmdS3SaveScriptMemWrite (Width - 0x80, Address, Value);
    }
  }
  if ((Width == AccessWidth64) || (Width == AccessS3SaveWidth64)) {
    LibAmdMemWrite (AccessWidth32, Address, Value, StdHeader);
    LibAmdMemWrite (AccessWidth32, Address + 4, (VOID *) ((UINTN)Value + 4), StdHeader);
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
    LibAmdMemRead (AccessWidth32, Address + 4, (VOID *) ((UINTN)Value + 4), StdHeader);
  } else {
    LibAmdMemRead (Width, Address, Value, StdHeader);
  }
}

