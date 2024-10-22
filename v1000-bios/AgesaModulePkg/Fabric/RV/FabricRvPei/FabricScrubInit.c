/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric DRAM Scrub initialization.
 *
 * This funtion initializes the scrub features of PIE.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "AMD.h"
#include <Library/IdsLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersRV.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/ApobCommonServiceLib.h>
#include <Library/BaseMemoryLib.h>
#include "Filecode.h"
#include "FabricScrubInit.h"

#define FILECODE FABRIC_RV_FABRICRVPEI_FABRICSCRUBINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define NUM_SCRUB_RATE_SETTINGS             16
#define mGetIntLvNumSockets(DramLimitAddr)  (DramLimitAddr.Field.IntLvNumSockets + 1)
#define mGetIntLvNumDies(DramLimitAddr)     (1 << DramLimitAddr.Field.IntLvNumDies)

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT64 GetDramScrubSize (UINTN Socket, UINTN Die);
UINTN  MatchScrubRate (UINT64 ScrubRateInNSec);

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
// In nano seconds
UINT64 DramScrubRateEncodings[NUM_SCRUB_RATE_SETTINGS] = {
  640,      // 0, 640ns
  1280,     // 1, 1.28us
  2560,     // 2, 2.56us
  5120,     // 3, 5.12us
  10200,    // 4, 10.2us
  20500,    // 5, 20.5us
  41000,    // 6, 41.0us
  81900,    // 7, 81.9us
  163800,   // 8, 163.8us
  327700,   // 9, 327.7us
  655400,   // A, 655.4us
  1310000,  // B, 1.31ms
  2620000,  // C, 2.62ms
  5240000,  // D, 5.24ms
  10490000, // E, 10.49ms
  20970000, // F, 20.97ms
 };

/* -----------------------------------------------------------------------------*/
/**
 *
 *  FabricScrubInit
 *
 *  Description:
 *    This funtion initializes the DRAM scrub features of PIE.
 *
 */
VOID
FabricScrubInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       AMD_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  )
{
  UINTN                                            Socket;
  UINTN                                            Die;
  UINTN                                            NumOfProcessorPresent;
  UINTN                                            NumOfDiesOnSocket;
  UINTN                                            ScrubRateIndex;
  UINT64                                           DramScrubSizeOnDie;
  UINTN                                            ScrubTimeInHour;
  UINTN                                            i;
  UINT64                                           ScrubTimeInNSec;
  UINT64                                           ScrubRateInNSec;
  UINT32                                           ApobInstanceId;
  BOOLEAN                                          ScrubRedirection;
  BOOLEAN                                          IsEccEnabled;
  REDIRECT_SCRUB_CTRL_REGISTER                     RedirectScrubCtrl;
  DRAM_SCRUB_BASE_ADDR_REGISTER                    DramScrubBaseAddr;
  DRAM_SCRUB_LIMIT_ADDR_REGISTER                   DramScrubLimitAddr;
  EFI_STATUS                                       Status;
  UINT8                                            MaxChannelsPerDie;

  // Time = (MemSizeInBlk) * (DramScrubRate).
  //
  // Calculation methods:
  // 1. TimeHr * 1.1
  // 2. round to closest and faster time with a scrub rate

  IDS_HDT_CONSOLE (CPU_TRACE, "FabricScrubInit Entry\n");

  ScrubRedirection = PcdGetBool (PcdAmdFabricEccScrubRedirection);
  IDS_HDT_CONSOLE (CPU_TRACE, "IDS_HOOK IDS_HOOK_PIE_REDIRECT_SCRUB_CTRL_INIT\n");
  IDS_HOOK (IDS_HOOK_PIE_REDIRECT_SCRUB_CTRL_INIT, NULL, (VOID *) &ScrubRedirection);

  ScrubTimeInHour = PcdGet8 (PcdAmdFabricDramScrubTime);
  if (ScrubTimeInHour == 0xFF) {
    ScrubTimeInHour = 24; // The default of Auto maps to 24 hours
  }

  if (ScrubTimeInHour == 0) {
    // Disable scrubber
    return;
  }

  NumOfProcessorPresent = FabricTopologyGetNumberOfProcessorsPresent ();
  ApobGetMaxChannelsPerDie (0, &MaxChannelsPerDie);

  for (Socket = 0; Socket < NumOfProcessorPresent; Socket++) {
    NumOfDiesOnSocket = FabricTopologyGetNumberOfDiesOnSocket (Socket);
    for (Die = 0; Die < NumOfDiesOnSocket; Die++) {

      DramScrubSizeOnDie = GetDramScrubSize (Socket, Die);
      if (DramScrubSizeOnDie == 0) {
        continue;
      }

      ApobInstanceId = ((UINT32) Socket << 8) | (UINT32) Die;
      IsEccEnabled = FALSE;
      for (i = 0; i < MaxChannelsPerDie; i++) {
        Status = ApobGetEccEnable (ApobInstanceId, i, &IsEccEnabled);
        if (IsEccEnabled) {
          break;
        }
      }

      if (!IsEccEnabled) {
        continue;
      }

      ScrubTimeInNSec = MultU64x32 (3960000000000ull, (UINT32) ScrubTimeInHour); // 60 * 60 * 1000 * 1000 * 1000 * 11 Add 10% (* 11 / 10)
      ScrubRateInNSec = DivU64x64Remainder (ScrubTimeInNSec, RShiftU64 (DramScrubSizeOnDie, 6), NULL); // 64 bytes per block
      IDS_HDT_CONSOLE (CPU_TRACE, "ScrubRateInNSec = 0x%0X\n", ScrubRateInNSec);
      ScrubRateIndex = MatchScrubRate (ScrubRateInNSec);
      IDS_HDT_CONSOLE (CPU_TRACE, "ScrubRateIndex = 0x%0X\n", ScrubRateIndex);

      DramScrubLimitAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMSCRUBLIMITADDR_FUNC, DRAMSCRUBLIMITADDR_REG, FABRIC_REG_ACC_BC);
      DramScrubLimitAddr.Fields.DramScrubRate = ScrubRateIndex;
      FabricRegisterAccWrite (Socket, Die, DRAMSCRUBLIMITADDR_FUNC, DRAMSCRUBLIMITADDR_REG, FABRIC_REG_ACC_BC, DramScrubLimitAddr.Value, FALSE);

      for (i = 0; i < MaxChannelsPerDie; i++) {
        ApobGetEccEnable (ApobInstanceId, i, &IsEccEnabled);
        RedirectScrubCtrl.Value = FabricRegisterAccRead (Socket, Die, REDIRECTSCRUBCTRL_FUNC, REDIRECTSCRUBCTRL_REG, (RV_CS0_INSTANCE_ID + i));
        RedirectScrubCtrl.Fields.EnRedirScrub = (ScrubRedirection && IsEccEnabled) ? 1 : 0;
        FabricRegisterAccWrite (Socket, Die, REDIRECTSCRUBCTRL_FUNC, REDIRECTSCRUBCTRL_REG, (RV_CS0_INSTANCE_ID + i), RedirectScrubCtrl.Value, FALSE);
      }

      // Enable scrubber
      DramScrubBaseAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMSCRUBBASEADDR_FUNC, DRAMSCRUBBASEADDR_REG, FABRIC_REG_ACC_BC);
      DramScrubBaseAddr.Fields.DramScrubEn = 1;
      FabricRegisterAccWrite (Socket, Die, DRAMSCRUBBASEADDR_FUNC, DRAMSCRUBBASEADDR_REG, FABRIC_REG_ACC_BC, DramScrubBaseAddr.Value, FALSE);
    }
  }
  IDS_HDT_CONSOLE (CPU_TRACE, "FabricScrubInit Exit\n");
}


UINT64
GetDramScrubSize (
  IN       UINTN Socket,
  IN       UINTN Die
  )
{

  DRAM_SCRUB_BASE_ADDR_REGISTER   DramScrubBaseAddr;
  DRAM_SCRUB_LIMIT_ADDR_REGISTER  DramScrubLimitAddr;
  DRAM_BASE_ADDRESS_REGISTER      DramBaseAddr;
  DRAM_LIMIT_ADDRESS_REGISTER     DramLimitAddr;
  DRAM_HOLE_CONTROL_REGISTER      DramHoleControl;
  UINT64                          DramScrubSize;
  UINT32                          AddrMapPair;
  UINTN                           IntLvNum;

  DramScrubBaseAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMSCRUBBASEADDR_FUNC, DRAMSCRUBBASEADDR_REG, FABRIC_REG_ACC_BC);
  DramScrubLimitAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMSCRUBLIMITADDR_FUNC, DRAMSCRUBLIMITADDR_REG, FABRIC_REG_ACC_BC);
  DramScrubSize = LShiftU64 ((UINT64) ((DramScrubLimitAddr.Fields.DramScrubLimitAddr + 1) - DramScrubBaseAddr.Fields.DramScrubBaseAddr), 20);

  for (AddrMapPair = 0; AddrMapPair < 4; AddrMapPair++) {
    DramBaseAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMBASEADDR0_FUNC, DRAMBASEADDR0_REG + (AddrMapPair << 3), RV_PIE_INSTANCE_ID);
    DramLimitAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMLIMITADDR0_FUNC, DRAMLIMITADDR0_REG + (AddrMapPair << 3), RV_PIE_INSTANCE_ID);

    // There should be only one of LgcyMmioHoleEn set in the maps.
    if (DramBaseAddr.Field.LgcyMmioHoleEn == 1) {
      DramHoleControl.Value = FabricRegisterAccRead (Socket, Die, DRAMHOLECTRL_FUNC, DRAMHOLECTRL_REG, RV_PIE_INSTANCE_ID);
      if (DramHoleControl.Field.DramHoleValid == 1) {
        DramScrubSize -= (0x100 - DramHoleControl.Field.DramHoleBase) << 24;
      }
    }

    IntLvNum = mGetIntLvNumDies (DramLimitAddr) * mGetIntLvNumSockets (DramLimitAddr);
    if (IntLvNum > 1) {
      DramScrubSize -= LShiftU64 ((UINT64) (((DramLimitAddr.Field.DramLimitAddr + 1) - DramBaseAddr.Field.DramBaseAddr) / IntLvNum), 28);
    }
  }

  return (DramScrubSize);
}


UINTN
MatchScrubRate (
  IN       UINT64 ScrubRateInNSec
  )
{
  UINTN   Index;
  UINTN   ScrubRateIndex;

  ScrubRateIndex = 0;
  for (Index =   0; Index < NUM_SCRUB_RATE_SETTINGS; Index++) {
    if (ScrubRateInNSec >= DramScrubRateEncodings[Index]) {
      ScrubRateIndex = Index;
    } else {
      break;
    }
  }
  return (ScrubRateIndex);
}

