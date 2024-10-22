/* $NoKeywords:$ */
/**
 * @file
 *
 * default Fan policy table
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Nbio
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <PiDxe.h>
#include "FanPolicy.h"

// automatic fan policy table
FAN_POLICY_TABLE  DefaultAM4FanTable[] = {
  // 15W TDP/ 95 Tjmax table
  {0x0F0069,
  { 0,   // ForceFanPwmEn [1 means use the ForceFanPwm value below]
    0,   // ForceFanPwm
    1,   // FanTable_Override [1 means use the full set of data specified below; 0 means use default fan table]
    4,   // FanTable_Hysteresis
    50,  // FanTable_TempLow
    90,  // FanTable_TempMed
    90,  // FanTable_TempHigh
    100, // FanTable_TempCritical
    10,   // FanTable_PwmLow
    40,  // FanTable_PwmMed
    100,  // FanTable_PwmHigh
    1,   // FanTable_PwmFreq [0  = 25kHz; 1 = 100Hz]
    1    // FanTable_Polarity [0 = negative; 1 = positive]
  }},
  // 45W TDP/ 95 Tjmax table
  {0x2D0069,
  { 0,   // ForceFanPwmEn [1 means use the ForceFanPwm value below]
    0,   // ForceFanPwm
    1,   // FanTable_Override [1 means use the full set of data specified below; 0 means use default fan table]
    4,   // FanTable_Hysteresis
    50,  // FanTable_TempLow
    90,  // FanTable_TempMed
    90,  // FanTable_TempHigh
    100, // FanTable_TempCritical
    10,   // FanTable_PwmLow
    40,  // FanTable_PwmMed
    100,  // FanTable_PwmHigh
    0,   // FanTable_PwmFreq [0  = 25kHz; 1 = 100Hz]
    1    // FanTable_Polarity [0 = negative; 1 = positive]
  }},
  // null - end of table
  {0x0,
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
};
