/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe port remapping functions.
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
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/TimerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcieConfigLib.h>

#define FILECODE NBIO_PCIE_AMDNBIOPCIERVPEI_PCIEREMAPRV_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

#define DEVFUNC(d, f) ((((UINT8) d) << 3) | ((UINT8) f))

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                     L O C A L   D A T A   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

UINT8 ROMDATA DefaultPortDevMapRV [] = {
  DEVFUNC (1, 1),
  DEVFUNC (1, 2),
  DEVFUNC (1, 3),
  DEVFUNC (1, 4),
  DEVFUNC (1, 5),
  DEVFUNC (1, 6),
  DEVFUNC (1, 7)
};

/*----------------------------------------------------------------------------------------*/
/**
 * Check if engine can be remapped to Device/function number requested by user
 * defined engine descriptor
 *
 *   Function only called if requested device/function does not much native device/function
 *
 * @param[in]  DevFunc             PCI Device(7:3) and Func(2:0)
 * @retval     TRUE                Descriptor can be mapped to engine
 * @retval     FALSE               Descriptor can NOT be mapped to engine
 */

BOOLEAN
PcieCheckPortPciDeviceMapping (
  IN      UINT8         DevFunc
  )
{
  UINT8                 Index;
  for (Index = 0; Index < (sizeof (DefaultPortDevMapRV) / sizeof (DefaultPortDevMapRV[0])); Index++) {
    if (DefaultPortDevMapRV[Index] == DevFunc) {
      return TRUE;
    }
  }
  return FALSE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Map engine to specific PCI device address
 *
 *
 *
 * @param[in]  Engine              Pointer to engine configuration
 * @param[in]  PortDevMap          Pointer to PortDevMap
 * @retval     AGESA_ERROR         Fail to map PCI device address
 * @retval     AGESA_SUCCESS       Successfully allocate PCI address
 */

AGESA_STATUS
PcieMapPortPciAddress (
  IN      PCIe_ENGINE_CONFIG     *Engine,
  IN      UINT8*                 PortDevMap
  )
{
  AGESA_STATUS            Status;
  UINT8                   DevFunc;
  UINT8                   Index;
  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapPortPciAddress Enter\n");
  if (Engine->Type.Port.PortData.DeviceNumber == 0 && Engine->Type.Port.PortData.FunctionNumber == 0) {
    Engine->Type.Port.PortData.DeviceNumber = Engine->Type.Port.NativeDevNumber;
    Engine->Type.Port.PortData.FunctionNumber = Engine->Type.Port.NativeFunNumber;
  }

  DevFunc = (Engine->Type.Port.PortData.DeviceNumber << 3) | Engine->Type.Port.PortData.FunctionNumber;
  if (PcieCheckPortPciDeviceMapping (DevFunc) != TRUE) {
    Status = AGESA_ERROR;
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "    Device %x, Function %x\n", Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber);
    for (Index = 0; Index < sizeof (DefaultPortDevMapRV); ++Index) {
      if (PortDevMap[Index] == DevFunc) {
        Status = AGESA_ERROR;
        break;
      }
    }
  }
  if (Status == AGESA_SUCCESS) {
    PortDevMap[Engine->Type.Port.PortId] = DevFunc;
    IDS_HDT_CONSOLE (GNB_TRACE, "    PortDevMap DevFunc 0x%x, for PortId %d\n", DevFunc, Engine->Type.Port.PortId);
  }
  for (Index = 0; Index < sizeof (DefaultPortDevMapRV); ++Index) {
    if (DevFunc == DefaultPortDevMapRV[Index]) {
      Engine->Type.Port.LogicalBridgeId = Index;
      IDS_HDT_CONSOLE (GNB_TRACE, "    LogicalBridgeId = %x\n", Index);
      break;
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapPortPciAddress Exit [0x%x]\n", Status);
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Map engine to specific PCI device address
 *
 *
 * @param[in]  GnbHandle           Pointer to the Silicon Descriptor for this node
 * @param[in]  PortDevMap          Pointer to PortDevMap
 */

VOID
PcieSetPortPciAddressMap (
  IN      GNB_HANDLE    *GnbHandle,
  IN      UINT8         *PortDevMap
  )
{
  UINT8                       Index;
  UINT8                       DevFuncIndex;
  UINT8                       PortDevMapLocal [sizeof (DefaultPortDevMapRV)];
  SMN_0x13B100C0_STRUCT       DeviceRemap;
  PCIE_TX_REQUESTER_ID_STRUCT TxRequester;
  UINT32                      Value;

  LibAmdMemCopy ((VOID*) PortDevMapLocal, (VOID*) DefaultPortDevMapRV, sizeof (DefaultPortDevMapRV), (AMD_CONFIG_PARAMS *) NULL);
  for (Index = 0; Index < sizeof (DefaultPortDevMapRV); ++Index) {
    if (PortDevMap[Index] != 0) {
      for (DevFuncIndex = 0; DevFuncIndex < sizeof (DefaultPortDevMapRV); ++DevFuncIndex) {
        if (PortDevMapLocal[DevFuncIndex] == PortDevMap[Index]) {
          PortDevMapLocal[DevFuncIndex] = 0;
          break;
        }
      }
    }
  }
  for (Index = 0; Index < sizeof (DefaultPortDevMapRV); ++Index) {
    if (PortDevMap[Index] == 0) {
      for (DevFuncIndex = 0; DevFuncIndex < sizeof (DefaultPortDevMapRV); ++DevFuncIndex) {
        if (PortDevMapLocal[DevFuncIndex] != 0) {
          PortDevMap[Index] = PortDevMapLocal[DevFuncIndex];
          PortDevMapLocal[DevFuncIndex] = 0;
          break;
        }
      }
    }
    NbioRegisterRead (GnbHandle, SMN_0x13B100C0_TYPE, SMN_0x13B100C0_ADDRESS + (Index << 2), &DeviceRemap.Value, 0);
    DeviceRemap.Field.DevFnMap = PortDevMap[Index];
    NbioRegisterWrite (GnbHandle, SMN_0x13B100C0_TYPE, SMN_0x13B100C0_ADDRESS + (Index << 2), &DeviceRemap.Value, 0);

    NbioRegisterRead (GnbHandle, PCIE_TX_REQUESTER_ID_TYPE, PCIE_TX_REQUESTER_ID_ADDRESS + (Index << 12), &TxRequester.Value, 0);
    TxRequester.Field.TxRequesterIdDevice = PortDevMap[Index] >> 3;
    TxRequester.Field.TxRequesterIdBus = 0;
    NbioRegisterWrite (GnbHandle, PCIE_TX_REQUESTER_ID_TYPE, PCIE_TX_REQUESTER_ID_ADDRESS + (Index << 12), &TxRequester.Value, 0);

    NbioRegisterRead (GnbHandle, PCIEP_HW_DEBUG_TYPE, PCIE0_PCIEP_HW_DEBUG_ADDRESS + (Index << 12), &Value, 0);
    Value &= ~(0x7);
    Value |= (PortDevMap[Index] & 0x7);
    DeviceRemap.Field.DevFnMap = PortDevMap[Index];
    NbioRegisterWrite (GnbHandle, PCIEP_HW_DEBUG_TYPE, PCIE0_PCIEP_HW_DEBUG_ADDRESS + (Index << 12), &Value, 0);

  }
}



