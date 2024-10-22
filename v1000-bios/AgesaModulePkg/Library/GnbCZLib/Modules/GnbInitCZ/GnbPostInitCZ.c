/**
 * @file
 *
 * GNB post initialization.
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
#include  "GnbNbInitLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbSmuInitLibV8.h"
#include  "GnbRegistersCZ.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBINITCZ_GNBPOSTINITCZ_FILECODE
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

AGESA_STATUS
GnbPostInterfaceCZ (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );


/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Early Post Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
GnbPostInterfaceCZ (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS     Status;
  AGESA_STATUS     AgesaStatus;
  GNB_HANDLE       *GnbHandle;
  PCI_ADDR         GnbPciAddress;
  AGESA_TESTPOINT (TpGnbPostInterfaceCZEntry, NULL);
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbPostInterfaceCZ Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  GnbHandle = GnbGetHandle (StdHeader);
  GnbPciAddress = GnbGetHostPciAddress (GnbHandle);
  Status = GnbSetTom (GnbPciAddress, StdHeader);
  //After memory training, power up GPU
  GnbSmuRequestGpuPowerV8 (GnbHandle, SMC_MSG_PowerUpGpu, StdHeader);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbPostInterfaceCZ Exit [0x%x]\n", AgesaStatus);
  AGESA_TESTPOINT (TpGnbPostInterfaceCZExit, NULL);
  return  AgesaStatus;
}


