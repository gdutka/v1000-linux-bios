/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe link ASPM L1 SS
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
#include  <OptionGnb.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/AmdBaseLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/PcieMiscCommLib.h>
#include  <GnbRegistersRV.h>
#include  <Library/NbioRegisterAccLib.h>
#include  <Library/AmdS3SaveLib.h>
#include  <Include/AmdPcieComplex.h>

#define FILECODE NBIO_PCIE_AMDNBIOPCIERVDXE_AMDNBIOPCIECLKREQRV_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define   MMIO_GPP_CLK_CNTROL_ADDRESS      0xFED80E00
typedef struct {
  UINT32        GppClk0: 2;
  UINT32        GppClk1: 2;
  UINT32        GppClk4: 2;
  UINT32        GppClk2: 2;
  UINT32        GppClk3: 2;
  UINT32        GppClk5: 2;
  UINT32        GppClk6: 2;
  UINT32        Reservd: 18;
} GPP_CLK_CNTROL_STRUC;

#define ACPIMMIO32(x) (*(volatile GPP_CLK_CNTROL_STRUC*)(UINTN)(x))

typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  PCI_ADDR                DownstreamPort;
} PCIE_CLKREQ_PMM_DATA;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
#define NUMBER_OF_CLKREQB        8
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 Gpio Clk Req Control.

 @param[in]         NbioRootBus           Root Bus number of target NBIO (or chipset)
 @param[in]         ClkReqNumber          ClkReq number request
 @param[in]         ChipsetFlag           Premium Chipset flag (1 == Use Premium Chipset registers)

 @retval            EFI_SUCCESS       Function returns successfully
 @retval            EFI_UNSUPPORTED   Function is not supported
*/
STATIC
VOID
GpioClkReqControl (
  IN      UINT32                     NbioRootBus,
  IN      UINT8                      ClkReqNumber,
  IN      UINT8                      ChipsetFlag
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "%a Bus %d Clk %d Chipset %d\n", __FUNCTION__, NbioRootBus, ClkReqNumber, ChipsetFlag);
  if (ClkReqNumber != 0) {
    switch (ClkReqNumber) {
      case DxioClkReq0:
        ACPIMMIO32 (MMIO_GPP_CLK_CNTROL_ADDRESS).GppClk0 = 1;
        IDS_HDT_CONSOLE (GNB_TRACE, " - Enable ClkReq0\n");
        break;
      case DxioClkReq1:
        ACPIMMIO32 (MMIO_GPP_CLK_CNTROL_ADDRESS).GppClk1 = 1;
        IDS_HDT_CONSOLE (GNB_TRACE, " - Enable ClkReq1\n");
        break;
      case DxioClkReq2:
        ACPIMMIO32 (MMIO_GPP_CLK_CNTROL_ADDRESS).GppClk2 = 1;
        IDS_HDT_CONSOLE (GNB_TRACE, " - Enable ClkReq2\n");
        break;
      case DxioClkReq3:
        ACPIMMIO32 (MMIO_GPP_CLK_CNTROL_ADDRESS).GppClk3 = 1;
        IDS_HDT_CONSOLE (GNB_TRACE, " - Enable ClkReq3\n");
        break;
      case DxioClkReq4:
        ACPIMMIO32 (MMIO_GPP_CLK_CNTROL_ADDRESS).GppClk4 = 1;
        IDS_HDT_CONSOLE (GNB_TRACE, " - Enable ClkReq4\n");
        break;
      case DxioClkReq5:
        ACPIMMIO32 (MMIO_GPP_CLK_CNTROL_ADDRESS).GppClk5 = 1;
        IDS_HDT_CONSOLE (GNB_TRACE, " - Enable ClkReq5\n");
        break;
      case DxioClkReq6:
        ACPIMMIO32 (MMIO_GPP_CLK_CNTROL_ADDRESS).GppClk6 = 1;
        IDS_HDT_CONSOLE (GNB_TRACE, " - Enable ClkReq6\n");
        break;

      default:
        break;
    }
    IDS_HDT_CONSOLE (GNB_TRACE, "%GppClkCntrol 0x%x\n", ACPIMMIO32 (MMIO_GPP_CLK_CNTROL_ADDRESS));
  }
  return; // EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                     L O C A L   D A T A   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

UINT8 ROMDATA DefaultClkReqMapCntlRV [NUMBER_OF_CLKREQB][2] = {
  {0xFF, 0},    // CLKREQB_0
  {0xFF, 1},    // CLKREQB_1
  {0xFF, 2},    // CLKREQB_2
  {0xFF, 3},    // CLKREQB_3
  {0xFF, 4},    // CLKREQB_4
  {0xFF, 5},    // CLKREQB_5
  {0xFF, 6},    // CLKREQB_6
  {0xFF, 7},    // CLKREQB_7
};

VOID
UpdateClkReqMapCntl ()
{
  UINT8                 Index;
  UINT8                 Index1;
  UINT8                 data0;

  for (Index = 0; Index < NUMBER_OF_CLKREQB; Index++) {
    for (Index1 = 0; Index1 < NUMBER_OF_CLKREQB; Index1++) {
      if (DefaultClkReqMapCntlRV[Index1][0] == Index) {
        data0 = DefaultClkReqMapCntlRV[Index][1];
        DefaultClkReqMapCntlRV[Index][1] = DefaultClkReqMapCntlRV[Index1][1];
        DefaultClkReqMapCntlRV[Index1][1] = data0;

        data0 = DefaultClkReqMapCntlRV[Index][0];
        DefaultClkReqMapCntlRV[Index][0] = DefaultClkReqMapCntlRV[Index1][0];
        DefaultClkReqMapCntlRV[Index1][0] = data0;

        DefaultClkReqMapCntlRV[Index][0] = 0xFF;
      }
    }
  }
  for (Index = 0; Index < NUMBER_OF_CLKREQB; Index++) {
    IDS_HDT_CONSOLE (GNB_TRACE, "PORT_%d_CLKREQB_MAP: 0x%x - %d\n", Index, DefaultClkReqMapCntlRV[Index][0], DefaultClkReqMapCntlRV[Index][1]);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set CLOCK_POWER_MANAGEMENT_EN on PCIe device function
 *
 *
 *
 * @param[in] AddressValue    PCI address of function.
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieClkReqPmmEnable (
  IN       UINT32            AddressValue
  )
{
  UINT8       PcieCapPtr;
  UINT32      Value;
  PcieCapPtr = GnbLibFindPciCapability (AddressValue, PCIE_CAP_ID, NULL);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (AddressValue | (PcieCapPtr + PCIE_LINK_CAP_REGISTER), AccessWidth32, &Value, NULL);
    Value &= BIT18;
    // Check CLOCK_POWER_MANAGEMENT is Supported
    if (Value != 0) {
      // CLOCK_POWER_MANAGEMENT_EN set to Enable
      GnbLibPciRMW (
      AddressValue | (PcieCapPtr + PCIE_LINK_CTRL_REGISTER) ,
      AccessS3SaveWidth32,
      ~(UINT32)BIT8,
      BIT8,
      NULL
      );
    }
  }
}

SCAN_STATUS
SetPcieClkReqPmmEnableCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_CLKREQ_PMM_DATA    *PcieClkReqPmmData;
  PCIE_DEVICE_TYPE        DeviceType;

  ScanStatus = SCAN_SUCCESS;
  PcieClkReqPmmData = (PCIE_CLKREQ_PMM_DATA *) ScanData;
  IDS_HDT_CONSOLE (GNB_TRACE, "  SetPcieClkReqPmmEnableCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    IDS_HDT_CONSOLE (GNB_TRACE, "  PcieDeviceRootComplex/PcieDeviceDownstreamPort/PcieDeviceUpstreamPort %d\n", DeviceType);
    GnbLibPciScanSecondaryBus (Device, &PcieClkReqPmmData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    IDS_HDT_CONSOLE (GNB_TRACE, "  PcieDeviceEndPoint %d\n", DeviceType);
    PcieClkReqPmmEnable (Device.AddressValue);
    break;
  default:
    IDS_HDT_CONSOLE (GNB_TRACE, "  Unknow DeviceType %d\n", DeviceType);
    break;
  }
  return ScanStatus;
}

VOID
PcieClkReqPmmEnableCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCIE_CLKREQ_PMM_DATA PcieClkReqPmmData;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieClkReqPmmEnableCallback Enter\n");
  PcieClkReqPmmData.ScanData.StdHeader = NULL;
  PcieClkReqPmmData.ScanData.GnbScanCallback = SetPcieClkReqPmmEnableCallback;
  GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieClkReqPmmData.ScanData);

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieClkReqPmmEnableCallback Exit\n");
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to enable PM L1 SS
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieClkReqInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;
  PCIE_LC_PM_CNTL_STRUCT         NBIO_PCIE_LC_PM_CNTL;
  UINT32                         ClkMmioData;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieClkReqInterface Enter\n");
  ClkMmioData = 0;

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if ((PcieLibIsEngineAllocated (PcieEngine)) &&
          (PcieConfigCheckPortStatus (PcieEngine, INIT_STATUS_PCIE_TRAINING_SUCCESS))) {
        if (PcieEngine->Type.Port.PortData.MiscControls.ClkPmSupport == 1) {
          PcieClkReqPmmEnableCallback (PcieEngine, NULL);
        }
        if ((PcieEngine->Type.Port.LinkAspmL1_1 == 1) || (PcieEngine->Type.Port.LinkAspmL1_2 == 1)) {
          if ((PcieEngine->Type.Port.ClkReq != 0) && (PcieEngine->Type.Port.ClkReq <= NUMBER_OF_CLKREQB)) {
            DefaultClkReqMapCntlRV[PcieEngine->Type.Port.ClkReq - 1][0] = PcieEngine->Type.Port.PortId;
            IDS_HDT_CONSOLE (GNB_TRACE, "%d:%d    PortId=%d Clkreq#=%d\n" ,
                             PcieEngine->Type.Port.PortData.DeviceNumber,
                             PcieEngine->Type.Port.PortData.FunctionNumber,
                             PcieEngine->Type.Port.PortId,
                             PcieEngine->Type.Port.ClkReq - 1);
            GpioClkReqControl (GnbHandle->Address.Address.Bus, PcieEngine->Type.Port.ClkReq, 0);
          }
        }
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  // update clk request for S3 case
  LibAmdMemRead (AccessS3SaveWidth32, MMIO_GPP_CLK_CNTROL_ADDRESS, &ClkMmioData, NULL);
  AmdS3MemWrite (AccessWidth32, MMIO_GPP_CLK_CNTROL_ADDRESS, &ClkMmioData, NULL);
  UpdateClkReqMapCntl (DefaultClkReqMapCntlRV[0][0]);
  NbioRegisterRead ( GnbHandle, PCIE_LC_PM_CNTL_TYPE, PCIE_LC_PM_CNTL_ADDRESS, &NBIO_PCIE_LC_PM_CNTL, 0);
  NBIO_PCIE_LC_PM_CNTL.Field.LC_PORT_0_CLKREQB_MAP = DefaultClkReqMapCntlRV[0][1];
  NBIO_PCIE_LC_PM_CNTL.Field.LC_PORT_1_CLKREQB_MAP = DefaultClkReqMapCntlRV[1][1];
  NBIO_PCIE_LC_PM_CNTL.Field.LC_PORT_2_CLKREQB_MAP = DefaultClkReqMapCntlRV[2][1];
  NBIO_PCIE_LC_PM_CNTL.Field.LC_PORT_3_CLKREQB_MAP = DefaultClkReqMapCntlRV[3][1];
  NBIO_PCIE_LC_PM_CNTL.Field.LC_PORT_4_CLKREQB_MAP = DefaultClkReqMapCntlRV[4][1];
  NBIO_PCIE_LC_PM_CNTL.Field.LC_PORT_5_CLKREQB_MAP = DefaultClkReqMapCntlRV[5][1];
  NBIO_PCIE_LC_PM_CNTL.Field.LC_PORT_6_CLKREQB_MAP = DefaultClkReqMapCntlRV[6][1];
  NBIO_PCIE_LC_PM_CNTL.Field.LC_PORT_7_CLKREQB_MAP = DefaultClkReqMapCntlRV[7][1];
  NbioRegisterWrite (GnbHandle, PCIE_LC_PM_CNTL_TYPE, PCIE_LC_PM_CNTL_ADDRESS, &NBIO_PCIE_LC_PM_CNTL, GNB_REG_ACC_FLAG_S3SAVE);

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieClkReqInterface Exit\n");
}


