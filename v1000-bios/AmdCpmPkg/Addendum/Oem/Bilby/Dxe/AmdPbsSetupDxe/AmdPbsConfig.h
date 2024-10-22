/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_PBS_CONFIG_H_
#define _AMD_PBS_CONFIG_H_


// {A339D746-F678-49B3-9FC7-54CE0F9DF226}
#define AMD_PBS_SETUP_GUID \
  { 0xa339d746, 0xf678, 0x49b3, {0x9f, 0xc7, 0x54, 0xce, 0xf, 0x9d, 0xf2, 0x26} }

extern EFI_GUID gAmdPbsSystemConfigurationGuid;

#define AMD_PBS_SETUP_VARIABLE_NAME  L"AMD_PBS_SETUP"

// {B863B959-0EC6-4033-99C1-8FD89F040222}
#define AMD_PBS_SETUP_FORMSET_GUID \
  { 0xb863b959, 0xec6, 0x4033, {0x99, 0xc1, 0x8f, 0xd8, 0x9f, 0x4, 0x2, 0x22} }

//[-start-190704-IB06070039-add]//
#define AMD_CBS_FORMSET_CLASS_GUID \
  { \
    0x5ead73ce, 0xcc14, 0x4ff7, {0xaa, 0x31, 0xa6, 0x64, 0x93, 0xc6, 0x15, 0x3d} \
  }
//[-end-190704-IB06070039-add]//

#define AMD_PBS_SETUP_FORM_ID             11
#define AMD_PBS_FWVER_FORM_ID             12
#define AMD_PBS_SETUP_CLASS                1
#define AMD_PBS_SETUP_SUB_CLASS            0

#define AMD_CPM_USED_PBS_SIZE ((UINT8) (OFFSET_OF (AMD_PBS_SETUP_OPTION,ReservedBuffer)))
#define KEY_XGBE_PCIE         0x2019 // EMBSWDEV-6181: Question Id for XgbeVsPcieXn
#define KEY_XGBE_PORT0_PHY_SWITCH  0x2010 // Question ID for Xgbe PHY switch
#define KEY_XGBE_PORT0_RJ45_MODE   0x2011 // Question ID for Xgbe RJ45 port mode
#define KEY_XGBE_PORT1_PHY_SWITCH  0x2012 // Question ID for Xgbe PHY switch
#define KEY_XGBE_PORT1_RJ45_MODE   0x2013 // Question ID for Xgbe RJ45 port mode

/// Structure for Variables to be used for CPM
typedef struct {
  UINT16        PlatformId;                     ///< ((PcdGet16 (OriginalPlatformId) & BIT2)>>2)
  UINT16        CurrentId;                      ///< ((PcdGet16 (CurrentPlatformId) < 0x0008) ? 0 : 1)
  UINT8         SpecialVgaFeature;              ///< Special Feature
                                                ///<  0: Disabled
                                                ///<  4: HybridGraphics
  UINT8         DgpuAudio;                      ///< Discrete GPU's Audio
                                                ///<  0: Disabled
                                                ///<  1: Keep HW Default Setting
  UINT8         PrimaryVideoAdaptor;            ///< Primary Video Adaptor
                                                ///<  1: Int Graphics (IGD)
                                                ///<  2: Int Graphics (PEG)
  UINT8         SdPowerControl;                 ///< SD Lan power control
                                                ///<  0: Disable
                                                ///<  1: Enable
                                                ///<  4: Under I2C 3 Bus
  UINT8         SataPort0HddPowerEn;            ///< HDD Power Enable
                                                ///<  0: Disabled
                                                ///<  1: Enabled
                                                ///<  2: Manual by jumper
  UINT8         SataPort1OddPowerEn;            ///< ODD Power Enable
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         SataPortSsdPowerEn;             ///< SSD Power Enable
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         EvalAutoDetection;              ///< Eval Auto Detection
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         MITTWITTControl;                ///< MITT/WITT Selection
                                                ///<  0: MITT Only
                                                ///<  1: WITT Only
                                                ///<  2: Both disable
  UINT8         UnusedGppClkOff;                ///< Unused GPP Clocks Off
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         ClockRequest2;                  ///< Clock Request 2
                                                ///<  0: Disable
                                                ///<  1: Enable
  UINT8         ClockRequest3;                  ///< Clock Request 3
                                                ///<  0: Disable
                                                ///<  1: Enable
  UINT8         ClockRequest4;                  ///< Clock Request 4
                                                ///<  0: Disable
                                                ///<  1: Enable
  UINT8         ClockRequest5;                  ///< Clock Request 5
                                                ///<  0: Disable
                                                ///<  1: Enable
  UINT8         ClockRequest6;                  ///< Clock Request 6
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         KbcSupport;                     ///< KBC Support
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         FchSdConfig;

  UINT8         XgbePort0SpeedSwitch;           ///  0 : SPEED_1G , 1 : SPEED_10G
                                                ///  2 : SPEED_100M , 3 : SPEED_ALL
  UINT8         XgbePort0PhySwitch;             ///  0 : RJ45 , 1 : SFP+
  UINT8         XgbePort0RJ45Mode;              ///  1 : 10_1_BACKPLANE
                                                ///  2 : 2_5_BACKPLANE
                                                ///  3 : 1000BASET
                                                ///  4 : 1000BASEX
                                                ///  6 : 10GBASET
                                                ///  7 : 10GBASER
  UINT8         XgbePort1SpeedSwitch;           ///  0 : SPEED_1G , 1 : SPEED_10G
                                                ///  2 : SPEED_100M , 3 : SPEED_ALL
  UINT8         XgbePort1PhySwitch;             ///  0 : RJ45 , 1 : SFP+
  UINT8         XgbePort1RJ45Mode;              ///  1 : 10_1_BACKPLANE
                                                ///  2 : 2_5_BACKPLANE
                                                ///  3 : 1000BASET
                                                ///  4 : 1000BASEX
                                                ///  6 : 10GBASET
                                                ///  7 : 10GBASER
  UINT8         NonEvalDgpuSupport;             ///< Non-Eval Discrete GPU Support
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         DisplaySelect1;                 ///< Display Type Selection
                                                ///< 0: HDMI
                                               ///< 1: DP
  UINT8         DisplaySelect2;                 ///< Display Type Selection
                                               ///< 0: HDMI
                                              ///< 1: DP
  UINT8         XgbeVsPcieXn;                   ///< Xgbe Vs PCIe X2 Support
                                              ///< 0: Xgbe
                                              ///< 1: PCIe X2
                                               ///< 2: PCIe X4
  UINT8         PxeBoot;                        ///< PXE Boot
                                               ///< 0: Disabled
                                               ///< 1: PORT 0
                                               ///< 2: PORT 1
  UINT8         VDDP;                           ///< VDDP voltage
                                                ///<  0: VDDP voltage (0.9V)
                                                ///<  1: VDDP voltage (+10mv)
                                                ///<  2: VDDP voltage (+20mv)
                                                ///<  3: VDDP voltage (+30mv)
                                                ///<  4: VDDP voltage (+40mv)
                                                ///<  5: VDDP voltage (+50mv)
                                                ///<  6: VDDP voltage (+60mv)
                                                ///<  7: VDDP voltage (+70mv)
                                                ///<  8: VDDP voltage (+80mv)
                                                ///<  9: VDDP voltage (+90mv)
                                                ///<  10: VDDP voltage (+100mv)
                                                ///<  11: VDDP voltage (-10mv)
                                                ///<  12: VDDP voltage (-20mv)
                                                ///<  13: VDDP voltage (-30mv)
                                                ///<  14: VDDP voltage (-40mv)
                                                ///<  15: VDDP voltage (-50mv)
                                                ///<  16: VDDP voltage (-60mv)
                                                ///<  17: VDDP voltage (-70mv)
                                                ///<  18: VDDP voltage (-80mv)
                                                ///<  19: VDDP voltage (-90mv)
                                                ///<  20: VDDP voltage (-100mv)
  UINT8         SLTConfigCheck;                 ///< SLT Configuration Check
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         AmdPspKvmMouseProtocol;         ///< AMD KVM Mouse Protocol
                                                ///<  0: Absolute
                                                ///<  1: Simple
                                                ///<  3: Auto
  UINT8         AmdBlinkLed;                    ///< BLINK LED
                                                ///<  0: Disabled
                                                ///<  1: Enabled
                                                ///<  2: GPIO 11 Output Low
                                                ///<  3: GPIO 11 Output High
  UINT8         AmdCpmUsedPbsSize;              ///< AMD_CPM_USED_PBS_SIZE
  UINT8         Reserved0;
  UINT8         StallForUnlock;                 ///< StallForUnlock Support
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         HdtBreakPointBoot;              ///< HdtBreakPointBoot
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         HdtBreakPointS3;                ///< HdtBreakPointS3
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         WakeOnPME;                      ///< WakeOnPME
                                                ///<  0: Disabled
                                                ///<  1: Enabled
  UINT8         Reserved1;

  //
  // This is a fixed-size variable. New fields must be added immediately before
  // ReservedBuffer, and the reserved space must be reduced to keep the size constant.
  //
  UINT8         ReservedBuffer [86];            // Must be the last member in this structure.
} AMD_PBS_SETUP_OPTION;

#endif
