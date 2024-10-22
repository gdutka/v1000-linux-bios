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
#include  "GnbIommu.h"
#include  "GnbCommonLib.h"
#include  "GnbSbIommuLib.h"
#include  "GnbIvrsLib.h"
#include  "GnbSbLib.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBIOMMUIVRS_GNBSBIOMMULIB_FILECODE
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
 * Create IVHD entry
 *
 *
 * @param[in]  Ivhd            IVHD header pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
SbCreateIvhdEntries (
     OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  PCI_ADDR  Start;
  PCI_ADDR  End;
  PCI_ADDR  PciAddress;
  UINT32    Value;
  AMD_LATE_PARAMS        *LateParamsPtr;
  IDS_HDT_CONSOLE (GNB_TRACE, "SbCreateIvhdEntries Entry\n");
  LateParamsPtr = (AMD_LATE_PARAMS *) StdHeader;
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x14, 4, 0);
// P2P alias entry
  GnbLibPciRead (PciAddress.AddressValue | 0x18, AccessWidth32, &Value, StdHeader);
  Start.AddressValue = MAKE_SBDFO (0, (Value >> 8) & 0xff, 0, 0, 0);
  End.AddressValue = MAKE_SBDFO (0, (Value >> 16) & 0xff, 0x1f, 0x7, 0);
  GnbIvhdAddDeviceAliasRangeEntry (Start, End, PciAddress, 0, Ivhd, StdHeader);
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x14, 0, 0);
// HPET
  GnbIvhdAddSpecialDeviceEntry (IvhdSpecialDeviceHpet, PciAddress, 0, 0, Ivhd, StdHeader);

// APIC
  if (LateParamsPtr->GnbLateConfiguration.FchIoapicId != 0xff) {
    GnbIvhdAddSpecialDeviceEntry (
      IvhdSpecialDeviceIoapic,
      PciAddress,
      LateParamsPtr->GnbLateConfiguration.FchIoapicId,
      0xD7,
      Ivhd,
      StdHeader
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "SbCreateIvhdEntries Exit\n");
}

