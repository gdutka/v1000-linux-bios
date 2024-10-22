/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB Single Socket Library
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
#include  <GnbDxio.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/AmdBaseLib.h>
#include  <Library/GnbLib.h>
#include  <Library/GnbPciAccLib.h>
#include  <Library/GnbSSocketLib.h>
#define FILECODE LIBRARY_GNBSSOCKETLIB_GNBSSOCKETLIB_FILECODE
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
 * Get Host bridge PCI Address
 *
 *
 *
 * @param[in]   GnbHandle       GNB handle
 * @param[in]   StdHeader       Standard configuration header
 * @retval                      PCI address of GNB for a given socket/silicon.
 */

PCI_ADDR
GnbFmGetPciAddress (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  PCI_ADDR  Gnb;
  Gnb.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
  return Gnb;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get bus range decoded by GNB
 *
 * Final bus allocation can not be assumed until AmdInitMid
 *
 * @param[in]   GnbHandle       GNB handle
 * @param[out]  StartBusNumber  Beginning of the Bus Range
 * @param[out]  EndBusNumber    End of the Bus Range
 * @param[in]   StdHeader       Standard configuration header
 * @retval                      Status
 */

AGESA_STATUS
GnbFmGetBusDecodeRange (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   UINT8                      *StartBusNumber,
     OUT   UINT8                      *EndBusNumber,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  *StartBusNumber = 0x0;
  *EndBusNumber = 0xff;
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get link to which GNB connected to
 *
 *
 * @param[in]   GnbHandle       GNB handle
 * @param[out]  LinkId          Link to which GNB connected to
 * @param[in]   StdHeader       Standard configuration header
 * @retval                      Status
 */

AGESA_STATUS
GnbFmGetLinkId (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   UINT8                      *LinkId,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{

  *LinkId = 0x00;
  return AGESA_SUCCESS;
}

