/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe ALIB
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

#include  "AGESA.h"
#include  "Ids.h"
#include  "AlibSsdtCZD.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBINITCZ_ALIBCZD_FILECODE
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

VOID *
PcieAlibGetBaseTableCZD (
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Get base SSDT table
 *
 *
 *
 * @param[in]     StdHeader       Standard Configuration Header
 * @retval        pointer to SSTD table
 */
VOID *
PcieAlibGetBaseTableCZD (
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  return &AlibSsdtCZD[0];
}

