/* $NoKeywords:$ */
/**
 * @file
 *
 * PT Secondly Detection Workaround
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2018-04-23 15:14:43 +0800 (Mon, 23 Apr 2018) $
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
#include <Filecode.h>
#include <PiPei.h>
#include <AmdPcieComplex.h>
#include <GnbDxio.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/GnbLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/DxioLibV1.h>
#include <Library/GnbMemAccLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <GnbRegistersRV.h>
#include "PTSecondlyDetectingWorkaround.h"

#define FILECODE LIBRARY_DXIOLIBV1_PTSECONDLYDETECTINGWORKAROUND_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define PCIE0_Link_STATE_ADDRESS                           0x1110006a
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */



/*----------------------------------------------------------------------------------------*/
/**
 * Local delay function
 *
 *
 *
 * @param[in]  PeiServices    Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  xus            Microseconds to delay
 *
 */
VOID
STATIC
NBIO_Delay_Xus (
  IN  CONST     EFI_PEI_SERVICES   **PeiServices,
  IN  UINT32    xus
  )
{
  UINT16                timerAddr;
  UINT32                Delay = 0;
  UINT32                startTime;
  UINT32                elapsedTime;
  EFI_PEI_CPU_IO_PPI    *CpuIo;
  UINTN                 AcpiMemIoBaseAddr = 0;

  CpuIo = (*PeiServices)->CpuIo;

  Delay = xus;
  AcpiMemIoBaseAddr = 0xFED80000;
  GnbLibMemRead (AcpiMemIoBaseAddr + PMIO_BASE + 0x64, AccessWidth16, &(timerAddr),  (VOID*) NULL);
  if (timerAddr == 0 ) {
    Delay = Delay / 2;
    while ( Delay != 0 ) {
      CpuIo->IoRead8 (
         PeiServices,
         CpuIo,
         (UINT64) IO_DELAY_PORT
         );
      Delay --;
    }
  } else {
    startTime = CpuIo->IoRead32 (PeiServices,CpuIo,timerAddr);
    for ( ;; ) {
      elapsedTime = CpuIo->IoRead32 (PeiServices,CpuIo,timerAddr);
      if ( elapsedTime < startTime ) {
        elapsedTime = elapsedTime + MAX_TIMER - startTime;
      } else {
        elapsedTime = elapsedTime - startTime;
      }
      if ((elapsedTime * ELAPSED_TIME_UNIT / ELAPSED_TIME_DIVIDER) > Delay) {
        break;
      }
    }
  }
}


BOOLEAN
CheckPTLaneReversal (
  IN       BOOLEAN                   LaneReversal,
  IN       UINT32                    PciePortLaneStatusAddressData
)
{
  if (LaneReversal) {
    if (!(PciePortLaneStatusAddressData &= BIT0)) {
      return TRUE;
    }
  } else {
    if(PciePortLaneStatusAddressData &= BIT0) {
      return TRUE;
    };
  }
  return FALSE;
}

BOOLEAN
CheckAbnormalPTLaneWidth (
  IN       UINT32                    MaxPTLane,
  IN       UINT32                    PcieLcLinkWidthAddressData
)
{
  UINT32 TempWidthData;

  TempWidthData = 0;
  if (MaxPTLane == 0x4) {     // X4 lane
    TempWidthData  =  (PcieLcLinkWidthAddressData >> 4) & 0x7;
    if (TempWidthData == 0x1 || TempWidthData == 0x2) {
      return TRUE;
    }
  } else if (MaxPTLane == 0x2) { // PT Link X1;
    if (PcieLcLinkWidthAddressData &= BIT4) {
      return TRUE;
    };
  }
  return FALSE;
}

BOOLEAN
CheckSecondDetection (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT32              LcSTATE0Addres,
  IN       UINT32              PcieLinkStateAddress
)
{
  UINT32                                PcieLinkStateData;
  UINT32                                PcieLinkStateData2;
  PCIE_LC_STATE0_STRUCT                 LcState0;
  BOOLEAN                               IsAnotherDetection;

  IsAnotherDetection =  FALSE;

  do {
    NbioRegisterRead (GnbHandle, TYPE_SMN, LcSTATE0Addres, &LcState0.Value, 0);
    NbioRegisterRead (GnbHandle, TYPE_SMN, PcieLinkStateAddress, &PcieLinkStateData, 0);
    PcieLinkStateData = PcieLinkStateData & BIT11;
    NbioRegisterRead (GnbHandle, TYPE_SMN, PcieLinkStateAddress, &PcieLinkStateData2, 0);
    PcieLinkStateData2 = PcieLinkStateData2 & BIT13;
  } while (!((LcState0.Field.LC_CURRENT_STATE == 0x10) && (!PcieLinkStateData) && (PcieLinkStateData2)));

  return IsAnotherDetection;
}

VOID
PTSecondlyDetectingWorkaround (
  IN       GNB_HANDLE                *GnbHandle,
  IN       UINT32                    MaxPTLane,
  IN       BOOLEAN                   LaneReversal,
  IN       UINTN                     PtPortId
)
{
  UINT32                                PcieLcCntLAddress;
  UINT32                                PciePortLaneStatusAddress;
  UINT32                                SmnTempReadData;
  UINT32                                SmnTempReadData2;
  UINT32                                PcieLcLinkWidthAddress;
  UINT32                                PcieLinkStateAddress;
  UINT32                                PcieLinkStateData;
  UINT32                                LcSTATE0Addres;
  EFI_PEI_SERVICES                      **PeiServices;
  BOOLEAN                               IsAbnormalLinkWidth;
  BOOLEAN                               IsAbnormalPTLaneReversal;
  BOOLEAN                               IsAnotherDetection = FALSE;
  UINT32                                PcieLinkStateData2;
  PCIE_LC_STATE0_STRUCT                 LcState0;
  UINT32                                TxCreditAddres;
  UINT32                                TxCreditData;

  SmnTempReadData = 0;
  SmnTempReadData2 = 0;
  PcieLcLinkWidthAddress = 0;
  PcieLinkStateData = 0;
  TxCreditData = 0;
  IsAbnormalLinkWidth = FALSE;
  IsAbnormalPTLaneReversal = FALSE;
  PcieLcCntLAddress = (PCIE0_LC_CNTL_Address | (UINT32)(PtPortId << 12));
  PciePortLaneStatusAddress = (PCIE_PORT_LANE_STATUS_Address | (UINT32)(PtPortId << 12));
  PcieLcLinkWidthAddress = (PCIE_LC_LINK_WIDTH_CNTL_Address | (UINT32)(PtPortId << 12));
  PcieLinkStateAddress = (PCIE0_Link_STATE_ADDRESS | (UINT32)(PtPortId << 12));
  LcSTATE0Addres =(PCIE0_LC_STATE0_ADDRESS | (UINT32)(PtPortId << 12));
  TxCreditAddres =(PCIE_TX_CREDITS_STATUS_Address | (UINT32)(PtPortId << 12));

  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();

  do {
    NbioRegisterRead (GnbHandle, TYPE_SMN, LcSTATE0Addres, &LcState0.Value, 0);
    NbioRegisterRead (GnbHandle, TYPE_SMN, PcieLinkStateAddress, &PcieLinkStateData, 0);
    PcieLinkStateData = PcieLinkStateData & BIT11;
    NbioRegisterRead (GnbHandle, TYPE_SMN, PcieLinkStateAddress, &PcieLinkStateData2, 0);
    PcieLinkStateData2 = PcieLinkStateData2 & BIT13;

    if ((PcieLinkStateData2 == 0) && (PcieLinkStateData ==0) && (LcState0.Field.LC_CURRENT_STATE == 0x10)) {
      NbioRegisterRead (GnbHandle, TYPE_SMN, PciePortLaneStatusAddress, &SmnTempReadData, 0);
      IsAbnormalPTLaneReversal = CheckPTLaneReversal (LaneReversal,SmnTempReadData);
      //check D8 != 0, then do dis/En
      NbioRegisterRead (GnbHandle, TYPE_SMN, TxCreditAddres, &TxCreditData, 0);
      // TxCreditData should be all 0
      while (IsAbnormalPTLaneReversal || TxCreditData) {
        // Write one clear
        NbioRegisterWrite (GnbHandle, TYPE_SMN, TxCreditAddres, &TxCreditData, 0);
        // set LinkDis (bit4) to 1 then delay 50 ms after that set LinkDis(bit4) back to 0.
        NbioRegisterRead (GnbHandle, TYPE_SMN, PcieLcCntLAddress, &SmnTempReadData, 0);
        SmnTempReadData = SmnTempReadData | BIT4;
        NbioRegisterWrite (GnbHandle, TYPE_SMN, PcieLcCntLAddress, &SmnTempReadData, 0);
        NBIO_Delay_Xus ((CONST EFI_PEI_SERVICES**)PeiServices, 50000);   // Delay 50ms

        SmnTempReadData = NbioRegisterRead (GnbHandle, TYPE_SMN, PcieLcCntLAddress, &SmnTempReadData, 0);
        SmnTempReadData &= ~(BIT4);
        NbioRegisterWrite (GnbHandle, TYPE_SMN, PcieLcCntLAddress, &SmnTempReadData, 0);

        IsAnotherDetection = CheckSecondDetection (GnbHandle, LcSTATE0Addres, PcieLinkStateAddress);

        // Check Lane reversal or not?
        NbioRegisterRead (GnbHandle, TYPE_SMN, PciePortLaneStatusAddress, &SmnTempReadData, 0);
        IsAbnormalPTLaneReversal = CheckPTLaneReversal (LaneReversal,SmnTempReadData);
        NbioRegisterRead (GnbHandle, TYPE_SMN, TxCreditAddres, &TxCreditData, 0);
      }
    }
  } while (!((LcState0.Field.LC_CURRENT_STATE == 0x10) &&  (!PcieLinkStateData)&&  (PcieLinkStateData2)  ));
}



