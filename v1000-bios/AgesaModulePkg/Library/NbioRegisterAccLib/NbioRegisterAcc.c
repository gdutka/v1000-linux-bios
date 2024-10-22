/* $NoKeywords:$ */
/**
 * @file
 *
 * NBIO Register Access Methods.
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
#include  "Gnb.h"
#include  "NbioRegisterTypes.h"
#include  <Library/AmdBaseLib.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/NbioRegisterAccLib.h>
#include  <Library/NbioHandleLib.h>
#include  <Filecode.h>
#define FILECODE LIBRARY_NBIOREGISTERACCLIB_NBIOREGISTERACC_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#undef GNB_TRACE_ENABLE
#define GNB_IGNORED_PARAM             0xFF
#define ORB_WRITE_ENABLE              0//0x100
#define IOMMU_L1_WRITE_ENABLE         0x80000000ul
#define IOMMU_L2_WRITE_ENABLE         0x100


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
NbioRegisterWriteDump (
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       UINT32              Value
  );


/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to read all register spaces.
 *
 *
 *
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[out] Value             Return value
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @retval    AGESA_STATUS
 */
AGESA_STATUS
NbioRegisterRead (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
     OUT   VOID                *Value,
  IN       UINT32              Flags
  )
{
  ACCESS_WIDTH  Width;
  PCI_ADDR      GnbPciAddress;
//  UINT64        GmmBase;

  GnbPciAddress = NbioGetHostPciAddress (GnbHandle);
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;

  GNB_DEBUG_CODE (
  IDS_HDT_CONSOLE (NB_MISC, "  R READ Space 0x%x  Address 0x%04x\n",
      RegisterSpaceType,
      Address
    );
  );

  switch (RegisterSpaceType) {
  case TYPE_D0F0:
    GnbLibPciRead (
      GnbPciAddress.AddressValue | Address,
      Width,
      Value,
      NULL
    );
    break;

  case TYPE_SMN:
    {
      UINT64  TempData;
      GnbLibPciIndirectRead (
        GnbPciAddress.AddressValue | D0F0xB8_ADDRESS,
        (Address & (~0x3ull)),
        Width,
        &TempData,
        NULL
      );
      if ((Address & 0x3) != 0) {
        //Non aligned access allowed to fuse block
        GnbLibPciIndirectRead (
          GnbPciAddress.AddressValue | D0F0xB8_ADDRESS,
          (Address & (~0x3ull)) + 4,
          Width,
          ((UINT32 *) &TempData) + 1,
          NULL
        );
      }
      *((UINT32*) Value) = (UINT32) RShiftU64 (TempData, ((Address & 0x3) * 8));
      break;
    }

  default:
    ASSERT (FALSE);
    break;
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to write all register spaces.
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[out] Value             The value to write
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @retval    AGESA_STATUS
 */
AGESA_STATUS
NbioRegisterWrite (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  )
{

  ACCESS_WIDTH    Width;
  PCI_ADDR        GnbPciAddress;
//  UINT64          GmmBase;

  GnbPciAddress = NbioGetHostPciAddress (GnbHandle);
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;

  GNB_DEBUG_CODE (
    NbioRegisterWriteDump (RegisterSpaceType, Address, *((UINT32*)Value));
  );

  switch (RegisterSpaceType) {
  case TYPE_D0F0:
    GnbLibPciWrite (
      GnbPciAddress.AddressValue | Address,
      Width,
      Value,
      NULL
    );
    break;

  case TYPE_SMN:
    //SMU, access D0F0xBC_x[FFFFFFFF:00000000]
    GnbLibPciIndirectWrite (
      GnbPciAddress.AddressValue | D0F0xB8_ADDRESS,
      Address,
      Width,
      Value,
      NULL
    );
    break;

  default:
    ASSERT (FALSE);
    break;
  }

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[in] Mask               And Mask
 * @param[in] Value              Or value
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @retval    VOID
 */
VOID
NbioRegisterRMW (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       UINT32              Mask,
  IN       UINT32              Value,
  IN       UINT32              Flags
  )
{
  UINT32  Data;
  NbioRegisterRead (GnbHandle, RegisterSpaceType, Address, &Data, Flags);
  Data = (Data & Mask) | Value;
  NbioRegisterWrite (GnbHandle, RegisterSpaceType, Address, &Data, Flags);
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to dump all write register spaces.
 *
 *
 *
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset
 * @param[in] Value              The value to write
 */
VOID
NbioRegisterWriteDump (
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       UINT32              Value
  )
{
  IDS_HDT_CONSOLE (NB_MISC, "  R WRITE Space %a  Address 0x%04x, Value 0x%04x\n",
    (RegisterSpaceType == TYPE_D0F0) ? "TYPE_D0F0" : (
    (RegisterSpaceType == TYPE_SMN) ? "TYPE_SMN" : (
    (RegisterSpaceType == TYPE_D0F0xBC) ? "TYPE_D0F0xBC" : "Invalid")),
    Address,
    Value
  );
}



