/* $NoKeywords:$ */
/**
 * @file
 *
 * NBIF Device Control Functions
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 276746 $   @e \$Date: 2014-10-28 12:22:52 -0600 (Mon, 28 Oct 2013) $
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
#include <AMD.h>
#include <Gnb.h>
#include <PiPei.h>
#include <Filecode.h>
#include <GnbRegistersRV.h>
#include <AmdPcieComplex.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>

#define FILECODE NBIO_PCIE_AMDNBIOPCIERVPEI_DEVICECONTROLRV_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * NBIF Device Enable
 *
 *
 *
 * @param[in]  PeiServices   Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  GnbHandle     GNB_HANDLE
 */

VOID
NbioEnableNbifDevice (
  IN        GNB_HANDLE         *GnbHandle,
  IN        UINT8              DeviceType,
  IN        UINT8              StartLane
  )
{
  IDS_HDT_CONSOLE(GNB_TRACE, "NbioEnableNbifDevice (DeviceType=%x)\n", DeviceType);

  switch (DeviceType) {
  case DxioSATAEngine:

    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIF1_SATA_STRAP0_ADDRESS,
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     NBIFMISC0_INTR_LINE_ENABLE_TYPE,
                     NBIFMISC0_INTR_LINE_ENABLE_ADDRESS,
                     (UINT32) ~(1 << 8),
                     (1 << 8),
                     0
                     );
    break;

  case DxioEthernetEngine:
    switch (StartLane) {
    case 0:
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIF1_XGBE0_STRAP0_ADDRESS,
                       (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                       (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       NBIFMISC0_INTR_LINE_ENABLE_TYPE,
                       NBIFMISC0_INTR_LINE_ENABLE_ADDRESS,
                       (UINT32) ~(1 << 9),
                       (1 << 9),
                       0
                       );
      break;
    case 1:
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIF1_XGBE1_STRAP0_ADDRESS,
                       (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                       (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       NBIFMISC0_INTR_LINE_ENABLE_TYPE,
                       NBIFMISC0_INTR_LINE_ENABLE_ADDRESS,
                       (UINT32) ~(1 << 10),
                       (1 << 10),
                       0
                       );
      break;
    default:
      // XGBE is only supported on lanes 1:0, any other is invalid
      ASSERT (FALSE);
    }
    break;

  case DxioHDaudioEngine:
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIF0_HDAUDIO_STRAP0_ADDRESS,
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     NBIFMISC0_INTR_LINE_ENABLE_TYPE,
                     NBIFMISC0_INTR_LINE_ENABLE_ADDRESS,
                     (UINT32) ~(1 << 6),
                     (1 << 6),
                     0
                     );
    break;
  case DxioACPEngine:
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIF0_ACP_STRAP0_ADDRESS,
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    break;
  case DxioMP2Engine:
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIF0_MP2_STRAP0_ADDRESS,
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    break;
  }
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * NBIF Device Disable
 *
 *
 *
 * @param[in]  PeiServices   Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  GnbHandle     GNB_HANDLE
 */

VOID
NbioDisableNbifDevice (
  IN        GNB_HANDLE         *GnbHandle,
  IN        UINT8              DeviceType,
  IN        UINT8              StartLane
  )
{
  IDS_HDT_CONSOLE(GNB_TRACE, "NbioDisableNbifDevice (DeviceType=%x)\n", DeviceType);

  switch (DeviceType) {
  case DxioSATAEngine:

    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIF1_SATA_STRAP0_ADDRESS,
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     NBIFMISC0_INTR_LINE_ENABLE_TYPE,
                     NBIFMISC0_INTR_LINE_ENABLE_ADDRESS,
                     (UINT32) ~(1 << 8),
                     (0 << 8),
                     0
                     );
    break;

  case DxioEthernetEngine:
    switch (StartLane) {
    case 4:
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIF1_XGBE0_STRAP0_ADDRESS,
                       (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                       (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       NBIFMISC0_INTR_LINE_ENABLE_TYPE,
                       NBIFMISC0_INTR_LINE_ENABLE_ADDRESS,
                       (UINT32) ~(1 << 9),
                       (0 << 9),
                       0
                       );
      break;
    case 5:
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIF1_XGBE1_STRAP0_ADDRESS,
                       (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                       (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       NBIFMISC0_INTR_LINE_ENABLE_TYPE,
                       NBIFMISC0_INTR_LINE_ENABLE_ADDRESS,
                       (UINT32) ~(1 << 10),
                       (0 << 10),
                       0
                       );
      break;
    default:
      // XGBE is only supported on lanes 7:4, any other is invalid
      ASSERT (FALSE);
    }
    break;

  case DxioHDaudioEngine:
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIF0_HDAUDIO_STRAP0_ADDRESS,
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     NBIFMISC0_INTR_LINE_ENABLE_TYPE,
                     NBIFMISC0_INTR_LINE_ENABLE_ADDRESS,
                     (UINT32) ~(1 << 6),
                     (0 << 6),
                     0
                     );
    break;
  case DxioACPEngine:
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIF0_ACP_STRAP0_ADDRESS,
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    break;
  case DxioMP2Engine:
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIF0_MP2_STRAP0_ADDRESS,
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    break;
  }
  return;
}


