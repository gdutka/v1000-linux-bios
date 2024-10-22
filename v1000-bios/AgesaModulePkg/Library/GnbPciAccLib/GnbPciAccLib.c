/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access PCI config space registers
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
#include  "Filecode.h"
#include  <Library/AmdBaseLib.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbPciAccLib.h>
#include  <Library/AmdS3SaveLib.h>
#include  <Library/PciLib.h>

#define FILECODE LIBRARY_GNBPCIACCLIB_GNBPCIACCLIB_FILECODE
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
 * Write PCI registers
 *
 *
 *
 * @param[in] Address         PCI address (as presented in PCI_ADDR.AddressValue)
 * @param[in] Width           Access width
 * @param[in] Value           Pointer to value
 * @param[in] StdHeader       Pointer to standard header
 */
VOID
GnbLibPciWrite (
  IN      UINT32              Address,
  IN      ACCESS_WIDTH        Width,
  IN      VOID                *Value,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  if (Width >= AccessS3SaveWidth8) {
    AmdS3SaveScriptPciWrite (Width - AccessS3SaveWidth8 + 1, Address, Value);
    IDS_HDT_CONSOLE (NB_MISC, "  R S3 SAVE Script: Address 0x%08x, Width 0x%08x \n", Address, Width);
  }

  switch (Width) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    PciWrite8 (Address, *((UINT8 *) Value));
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    PciWrite16 (Address, *((UINT16 *) Value));
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    PciWrite32 (Address, *((UINT32 *) Value));
    break;
  default:
    IDS_ERROR_TRAP;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read PCI registers
 *
 *
 *
 * @param[in]   Address         PCI address (as presented in PCI_ADDR.AddressValue)
 * @param[in]   Width           Access width
 * @param[out]  Value           Pointer to value
 * @param[in]   StdHeader       Pointer to standard header
 */

VOID
GnbLibPciRead (
  IN       UINT32             Address,
  IN       ACCESS_WIDTH       Width,
     OUT   VOID               *Value,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  switch (Width) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    *((UINT8 *) Value) = PciRead8 (Address);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    *((UINT16 *) Value) = PciRead16 (Address);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    *((UINT32 *) Value) = PciRead32 (Address);
    break;
  default:
    IDS_ERROR_TRAP;
  }
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * TODO
 *
 */
VOID
GnbLibPciRmw (
  IN       UINT32              Address,
  IN       ACCESS_WIDTH        Width,
  IN       UINT32              Mask,
  IN       UINT32              OrValue,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32  Data;
  GnbLibPciRead (Address, Width, &Data, StdHeader);
  Data = (Data & Mask) | OrValue;
  GnbLibPciWrite (Address, Width, &Data, StdHeader);
}

