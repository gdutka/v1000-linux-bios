/**
 * @file
 *
 * Fan Policy definitions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Nbio
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#ifndef _FAN_POLICY_H_
#define _FAN_POLICY_H_

/// @todo Comments: Need to document
typedef struct {
  UINT8 ForceFanPwmEn;         ///< [1 means use the ForceFanPwm value below]
  UINT8 ForceFanPwm;           ///< [% 0-100]
  UINT8 FanTable_Override;     ///< [1 means use the full set of data specified below; 0 means use default fan table]
  UINT8 FanTable_Hysteresis;   ///<
  UINT8 FanTable_TempLow;      ///<
  UINT8 FanTable_TempMed;      ///<
  UINT8 FanTable_TempHigh;     ///<
  UINT8 FanTable_TempCritical; ///<
  UINT8 FanTable_PwmLow;       ///<
  UINT8 FanTable_PwmMed;       ///<
  UINT8 FanTable_PwmHigh;      ///<
  UINT8 FanTable_PwmFreq;      ///< [0  = 25kHz; 1 = 100Hz]
  UINT8 FanTable_Polarity;     ///< [0 = negative; 1 = positive]
} FAN_POLICY;

/// @todo Comments: Need to document
typedef struct {
  UINT32           TdpTjMax;   ///<
  FAN_POLICY       FanPolicy;  ///<
} FAN_POLICY_TABLE;


extern FAN_POLICY_TABLE  DefaultAM4FanTable[];

#define SMUTHM_SMN_Address 0x59800 
//SMU Registers to access the SB-TSI(SB Temperature Sensor Interface)Registers
#define SMUSBI_SBIREGADDR  0x200  //SMUTHMx00000200
#define SMUSBI_SBIREGDATA  0x204  //SMUTHMx00000204 (SMUSBI_SBIREGDATA)

#endif
