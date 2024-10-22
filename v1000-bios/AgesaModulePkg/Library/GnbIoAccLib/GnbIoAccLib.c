/* $NoKeywords:$ */
/**
 * @file
 *
* Service procedure to access I/O registers.
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
#include <Porting.h>
#include <AMD.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/GnbIoAccLib.h>
#include <Library/AmdS3SaveLib.h>

#define FILECODE LIBRARY_GNBIOACCLIB_GNBIOACCLIB_FILECODE
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

/*---------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/**
 * Write I/O Port
 *
 *
 *
 * @param[in] Address         Physical Address
 * @param[in] Width           Access width
 * @param[in] Value           Pointer to value
 * @param[in] StdHeader       Standard configuration header
 */

VOID
GnbLibIoWrite (
  IN       UINT16      Address,
  IN       ACCESS_WIDTH Width,
  IN       VOID         *Value,
  IN       VOID         *StdHeader
  )
{
  if (Width >= AccessS3SaveWidth8) {
    AmdS3SaveScriptIoWrite (Width - 0x80, (UINT64)Address, Value);
  }
  LibAmdIoWrite (Width, Address, Value, StdHeader);
}
/**
 * Read IO port
 *
 *
 *
 * @param[in]   Address         Physical Address
 * @param[in]   Width           Access width
 * @param[out]  Value           Pointer to value
 * @param[in]   StdHeader       Standard configuration header
 */

VOID
GnbLibIoRead (
  IN       UINT16       Address,
  IN       ACCESS_WIDTH Width,
     OUT   VOID         *Value,
  IN       VOID         *StdHeader
  )
{
  LibAmdIoRead (Width, Address, Value, StdHeader);
}


