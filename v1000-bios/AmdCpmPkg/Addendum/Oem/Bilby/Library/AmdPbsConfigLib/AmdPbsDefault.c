/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <PiPei.h>
#include <Uefi.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Dxe/AmdPbsSetupDxe/AmdPbsConfig.h>
#include <Library/PcdLib.h>

EFI_STATUS
EFIAPI
LoadAmdPbsDefault (
  OUT AMD_PBS_SETUP_OPTION        *AmdPbsConfiguration
  )
{

    AmdPbsConfiguration->PlatformId                                    = ((PcdGet16 (OriginalPlatformId) & BIT2)>>2);
    AmdPbsConfiguration->CurrentId                                     = ((PcdGet16 (CurrentPlatformId) < 0x0008) ? 0 : 1);
    AmdPbsConfiguration->AmdCpmUsedPbsSize                             = AMD_CPM_USED_PBS_SIZE;
    AmdPbsConfiguration->StallForUnlock                                = 0;
    AmdPbsConfiguration->HdtBreakPointBoot                             = 0;
    AmdPbsConfiguration->HdtBreakPointS3                               = 0;
    AmdPbsConfiguration->SpecialVgaFeature                             = 0;
    AmdPbsConfiguration->DgpuAudio                                     = 0;
    AmdPbsConfiguration->PrimaryVideoAdaptor                           = 1;
    AmdPbsConfiguration->SataPort0HddPowerEn                           = 1;
    AmdPbsConfiguration->SataPort1OddPowerEn                           = 1;
    AmdPbsConfiguration->SataPortSsdPowerEn                            = 1;
    AmdPbsConfiguration->EvalAutoDetection                             = 0;
    AmdPbsConfiguration->SdPowerControl                                = 1;
    AmdPbsConfiguration->FchSdConfig                                   = 0;
    AmdPbsConfiguration->MITTWITTControl                               = 2;
    AmdPbsConfiguration->UnusedGppClkOff                               = 0;
    AmdPbsConfiguration->ClockRequest2                                 = 1;
    AmdPbsConfiguration->ClockRequest3                                 = 1;
    AmdPbsConfiguration->ClockRequest4                                 = 0;
    AmdPbsConfiguration->ClockRequest5                                 = 0;
    AmdPbsConfiguration->ClockRequest6                                 = 0;
    AmdPbsConfiguration->KbcSupport                                    = 1;
    AmdPbsConfiguration->XgbePort0PhySwitch                            = 2;
    AmdPbsConfiguration->XgbePort0RJ45Mode                             = 1;
    AmdPbsConfiguration->XgbePort0SpeedSwitch                          = 1;
    AmdPbsConfiguration->XgbePort1PhySwitch                            = 2;
    AmdPbsConfiguration->XgbePort1RJ45Mode                             = 1;
    AmdPbsConfiguration->XgbePort1SpeedSwitch                          = 1;
    AmdPbsConfiguration->VDDP                                          = 0;
    AmdPbsConfiguration->AmdPspKvmMouseProtocol                        = 3;
    AmdPbsConfiguration->AmdBlinkLed                                   = 1;
    AmdPbsConfiguration->DisplaySelect1                                = 1;
    AmdPbsConfiguration->DisplaySelect2                                = 1;
    AmdPbsConfiguration->PxeBoot                                       = 0;
    AmdPbsConfiguration->XgbeVsPcieXn                                  = 0;
    return EFI_SUCCESS;
}