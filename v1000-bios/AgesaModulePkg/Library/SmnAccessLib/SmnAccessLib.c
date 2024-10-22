/* $NoKeywords:$ */
/**
 * @file
 *
 * SMN Register Access Methods
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
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
#include  <Library/BaseLib.h>
#include  <AGESA.h>
#include  <Library/SmnAccessLib.h>
#include  <Library/AmdBaseLib.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Filecode.h>
#define FILECODE LIBRARY_SMNACCESSLIB_SMNACCESSLIB_FILECODE
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
/*
 *  Routine to read all register spaces.
 *
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[out] Value             Return value
 * @retval     VOID
 */
VOID
SmnRegisterRead (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
     OUT   VOID                *Value
  )
{
  ACCESS_WIDTH  Width;
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus = BusNumber;
  Width = AccessWidth32;

  GnbLibPciIndirectRead (
    GnbPciAddress.AddressValue | D0F0xB8_ADDRESS,
    Address,
    Width,
    Value,
    NULL
  );
  return;
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to write all register spaces.
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Value             The value to write
 * @param[in]  Flags             Flags - BIT0 indicates S3 save/restore
 * @retval     VOID
 */
VOID
SmnRegisterWrite (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  )
{

  ACCESS_WIDTH  Width;
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus = BusNumber;
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;

  GnbLibPciIndirectWrite (
    GnbPciAddress.AddressValue | D0F0xB8_ADDRESS,
    Address,
    Width,
    Value,
    NULL
  );
  return;
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Mask              And Mask
 * @param[in]  Value             Or value
 * @retval     VOID
 */
VOID
SmnRegisterRMW (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue,
  IN       UINT32              Flags
  )
{
  UINT32  Data;
  SmnRegisterRead (BusNumber, Address, &Data);
  Data = (Data & AndMask) | OrValue;
  SmnRegisterWrite (BusNumber, Address, &Data, Flags);
}


