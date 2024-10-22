/* $NoKeywords:$ */
/**
 * @file
 *
 * SB services
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
#include  "GnbCommonLib.h"
#include  "GnbSbLib.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBSBLIB_GNBSBLIB_FILECODE
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
 *Get SB IOAPIC Base Address
 *
 *
 * @param[in]  StdHeader       Standard configuration header
 * @retval     ApicBaseAddress
 */
UINT32
SbGetSbIoApicBaseAddress (
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT32 ApicBaseAddress;
  GnbLibIndirectIoBlockRead (0xCD6, 0xCD7, AccessWidth8, 0x34, 4, &ApicBaseAddress, StdHeader);
  return ApicBaseAddress & 0xfffffff8;
}

/*----------------------------------------------------------------------------------------*/
/**
 *Get SB MMIO Base Address
 *
 *
 * @param[in]  StdHeader       Standard configuration header
 * @retval     MMIO base address
 */
UINT32
SbGetSbMmioBaseAddress (
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT32 MmioBaseAddress;
  GnbLibIndirectIoBlockRead (0xCD6, 0xCD7, AccessWidth8, 0x24, 4, &MmioBaseAddress, StdHeader);
  return MmioBaseAddress & 0xfffffffc;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get Alink config address
 *
 * @param[in]  StdHeader       Standard configuration header
 * @retval     AlinkPortAddress
 */
/*----------------------------------------------------------------------------------------*/

UINT16
SbGetAlinkIoAddress (
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{

  UINT16 AlinkPortAddress;
  GnbLibIndirectIoBlockRead (0xCD6, 0xCD7, AccessWidth8, 0xE0, 2, &AlinkPortAddress, StdHeader);
  return  AlinkPortAddress;
}

