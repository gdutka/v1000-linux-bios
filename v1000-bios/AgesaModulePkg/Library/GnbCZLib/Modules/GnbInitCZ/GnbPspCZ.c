/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB PSP access functions
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 326884 $   @e \$Date: 2015-09-11 11:08:59 +0800 (Fri, 11 Sep 2015) $
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
#include  "OptionGnb.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbTable.h"
#include  "GnbSmuInitLibV8.h"
#include  "GnbRegisterAccCZ.h"
#include  "GnbRegistersCZ.h"
#include  "GnbPspCZ.h"
#include  <Library/AmdPspBaseLibV1.h>

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBINITCZ_GNBPSPCZ_FILECODE
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
 * Get SCS data
 *
 *
 * @param[in, out]  EntryAddress  Pointer to SCS data
 * @param[in, out]  EntrySize  The size of SCS data
 * @retval     AGESA_STATUS
 */
AGESA_STATUS
GnbGetScsDataCZ (
  IN OUT   UINT64        *EntryAddress,
  IN OUT   UINT32        *EntrySize
  )
{
  BOOLEAN bValue;

  bValue = PSPEntryInfoV1 (AMD_SCS_BINARY, EntryAddress, EntrySize);
  if (!bValue) {
    IDS_HDT_CONSOLE (GNB_TRACE, " Can not get SCS data Baseaddress.\n");
    return AGESA_ERROR;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, " SCS data Baseaddress @%08x Size=%d\n", *EntryAddress, *EntrySize);
  return AGESA_SUCCESS;
}



