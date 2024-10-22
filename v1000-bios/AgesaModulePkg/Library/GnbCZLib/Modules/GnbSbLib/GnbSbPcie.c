/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB-SB link procedure
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
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieInitLib.h"
#include  "GnbSbLib.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBSBLIB_GNBSBPCIE_FILECODE
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
 * Enable/Disable ASPM on GNB-SB link
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

AGESA_STATUS
SbPcieLinkAspmControl (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS    Status;
  PCIE_ASPM_TYPE  Aspm;

  Aspm = Engine->Type.Port.PortData.LinkAspm;

  Status = SbPcieInitAspm (Aspm, GnbLibGetHeader (Pcie));
  if (Status != AGESA_SUCCESS) {
    return AGESA_UNSUPPORTED;
  }

  PcieAspmEnableOnFunction (Engine->Type.Port.Address, Aspm, GnbLibGetHeader (Pcie));
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init SB ASPM.
 * Enable ASPM states on SB
 *
 *
 * @param[in] Aspm       ASPM bitmap.
 * @param[in] StdHeader  Standard configuration header
 */
/*----------------------------------------------------------------------------------------*/

AGESA_STATUS
SbPcieInitAspm (
  IN      PCIE_ASPM_TYPE       Aspm,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT16        AlinkPort;

  AlinkPort = SbGetAlinkIoAddress (StdHeader);
  ASSERT (AlinkPort != 0);
  if (AlinkPort == 0) {
    return AGESA_UNSUPPORTED;
  }
  GnbLibIoRMW (AlinkPort, AccessS3SaveWidth32, 0x0, 0x40000038, StdHeader);
  GnbLibIoRMW (AlinkPort + 4, AccessS3SaveWidth32, 0x0, 0xA0, StdHeader);
  GnbLibIoRMW (AlinkPort, AccessS3SaveWidth32, 0x0, 0x4000003c, StdHeader);
  GnbLibIoRMW (AlinkPort + 4, AccessS3SaveWidth32, 0xffff00ff, 0x6900, StdHeader);
  GnbLibIoRMW (AlinkPort, AccessS3SaveWidth32, 0x0, 0x80000068, StdHeader);
  GnbLibIoRMW (AlinkPort + 4, AccessS3SaveWidth32, 0xfffffffc, Aspm, StdHeader);
  return AGESA_SUCCESS;
}



