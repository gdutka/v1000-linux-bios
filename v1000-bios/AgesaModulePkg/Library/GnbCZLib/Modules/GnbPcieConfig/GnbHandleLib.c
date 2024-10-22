/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB function to create/locate PCIe configuration data area
 *
 * Contain code that create/locate/manages GNB/PCIe configuration
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
#include  "GnbPcieConfig.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBPCIECONFIG_GNBHANDLELIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Get GNB handle
 *
 *
 * @param[in]       StdHeader       Standard configuration header
 */
GNB_HANDLE *
GnbGetHandle (
  IN      AMD_CONFIG_PARAMS                 *StdHeader
  )
{
  PCIe_PLATFORM_CONFIG  *Pcie;
  GNB_HANDLE            *GnbHandle;
  AGESA_STATUS          Status;
  GnbHandle = NULL;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status == AGESA_SUCCESS) {
    GnbHandle = (GNB_HANDLE *) PcieConfigGetChild (DESCRIPTOR_SILICON, &Pcie->Header);
  }
  return GnbHandle;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get GNB socket ID
 *
 *
 * @param[in]       GnbHandle       Gnb handle
 */
UINT8
GnbGetSocketId (
  IN      GNB_HANDLE                        *GnbHandle
  )
{
  return PcieConfigGetParentComplex (GnbHandle)->SocketId;
}

/*----------------------------------------------------------------------------------------*/
/*
 * Get PCI_ADDR of GNB
 *
 *
 * @param[in] Handle          Pointer to GNB_HANDLE
 * @retval    PCI_ADDR        PCI_ADDR of device
 */

PCI_ADDR
GnbGetHostPciAddress (
  IN      GNB_HANDLE          *Handle
  )
{
  ASSERT (Handle != NULL);
  return Handle->Address;
}

/*----------------------------------------------------------------------------------------*/
/*
 * Get GmmBase of GNB
 *
 *
 * @param[in] Handle          Pointer to GNB_HANDLE
 * @retval    UINT64          GmmBase
 */

UINT64
GnbGetGmmBaseAddress (
  IN      GNB_HANDLE          *Handle
  )
{
  ASSERT (Handle != NULL);
  return Handle->GmmBase;
}

