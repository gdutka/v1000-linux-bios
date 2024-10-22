/* $NoKeywords:$ */
/**
 * @file
 *
 * Contains the definition of the SMU10
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: Nbio/SMU
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
#ifndef SMU10_BIOS_IF_H
#define SMU10_BIOS_IF_H

//
// Assume standard IA-32 alignment.
// BugBug: Need to check portability of long long
//
typedef unsigned long long  uint64_t;
typedef long long           int64_t;
typedef unsigned int        uint32_t;
typedef int                 int32_t;
typedef unsigned short      uint16_t;
typedef short               int16_t;
typedef unsigned char       uint8_t;
typedef signed char         int8_t;

// *** IMPORTANT ***
// SMU TEAM: Always increment the interface version if
// any structure is changed in this file
#define SMU10_BIOS_IF_VERSION 19

typedef struct {
  uint32_t Version;

  //PCD infrastructure limits
  uint32_t SUSTAINED_POWER_LIMIT;           //Power [mWatts] (STAPM)
  uint32_t FAST_PPT_LIMIT;                  //Power [mWatts] (fPPT)
  uint32_t SLOW_PPT_LIMIT;                  //Power [mWatts] (sPPT)
  uint32_t SLOW_PPT_TIME_CONSTANT;          //Time constant (seconds)
  uint8_t  SYSTEM_TEMPERATURE_TRACKING;     //Tskin feedback from EC; 1 = enabled; 0 = disabled
  uint8_t  STAPM_BOOST_OVERRIDE;            //1 = use value in STAPM_BOOST; 0 = use SMU FW default
  uint8_t  STAPM_BOOST;                     //1 = enabled; 0 = disabled
  uint8_t  AA_MODE_EN;                      //A+A power/thermal sharing algorithm; 1 = enable; 0 = disable
  uint32_t STAPM_TIME_CONSTANT;             //Time constant (seconds)
  uint32_t THERMCTL_LIMIT;                  //Tctrl (THM)
  uint32_t VRM_CURRENT_LIMIT;               //Current [mA] (VDDCR_VDD TDC)
  uint32_t VRM_MAXIMUM_CURRENT_LIMIT;       //Current [mA] (VDDCR_VDD EDC)
  uint32_t VRM_SOC_CURRENT_LIMIT;           //Current [mA] (VDDCR_SOC TDC)
  uint32_t VRM_SOC_MAXIMUM_CURRENT_LIMIT;   //Current [mA] (VDDCR_SOC EDC)
  uint32_t PSI0_CURRENT_LIMIT;              //Current [mA] (VDDCR_VDD PSI0)
  uint32_t PSI0_SOC_CURRENT_LIMIT;          //Current [mA] (VDDCR_SOC PSI0)
  uint32_t PROCHOT_L_DEASSERTION_RAMP_TIME; //Time [ms] (PROCHOT)
  uint8_t  SYSTEM_CONFIG;                   //0 = use fused default; >0 = user specified value
  uint8_t  itemp;
  uint8_t  Voltage_SlewRate;                //Only used in conjunction with WaitVidCompDis = 1
  uint8_t  spare;

  //PCD/CBS fan table
  uint8_t FanTable_Override;     //[1 means use the full set of data specified below; 0 means use default fan table]
  uint8_t FanTable_Hysteresis;
  uint8_t FanTable_TempLow;
  uint8_t FanTable_TempMed;
  uint8_t FanTable_TempHigh;
  uint8_t FanTable_TempCritical;
  uint8_t FanTable_PwmLow;
  uint8_t FanTable_PwmMed;
  uint8_t FanTable_PwmHigh;
  uint8_t FanTable_PwmFreq;      //[0 = 25kHz; 1 = 100Hz]
  uint8_t FanTable_Polarity;     //[0 = negative; 1 = positive]
  uint8_t FanTable_spare;

  //CBS debug options
  uint8_t  CoreDldoPsmMargin;       //[PSM count 1 ~ 1.8mV]
  uint8_t  GfxDldoPsmMargin;        //[PSM count 1 ~ 1.8mV]
  uint8_t  ForceFanPwmEn;           //[1 means use the ForceFanPwm value below]
  uint8_t  ForceFanPwm;             //[% 0-100]
  uint8_t  CoreStretchThreshEn;     //[1 means use the CoreStretchThresh value below]
  uint8_t  CoreStretchThresh;       //[1 = 2.5%]
  uint8_t  L3StretchThreshEn;       //[1 means use the L3StretchThresh value below]
  uint8_t  L3StretchThresh;         //[1 = 2.5%]
  uint8_t  GfxStretchThreshEn;      //[1 means use the GfxStretchThresh value below]
  uint8_t  GfxStretchThresh;        //[1 = 2.5%]
  uint8_t  GfxStretchAmountEn;
  uint8_t  GfxStretchAmount;
  uint8_t  CoreDldoBypass;          //[1 means bypass]
  uint8_t  GfxDldoBypass;           //[1 means bypass]
  uint8_t  XiSeparationEn;          //[1 means use the XiSeparation value below]
  uint8_t  XiSeparationHigh;        //[0 = async mode; 3 = 2 cycle; 4 = 2.5 cycle; 5 = 3 cycle; 6 = 3.5 cycle; 7 = 4 cycle]
  uint8_t  XiSeparationLow;         //[0 = async mode; 3 = 2 cycle; 4 = 2.5 cycle; 5 = 3 cycle; 6 = 3.5 cycle; 7 = 4 cycle]
  uint8_t  AvfsCoeffTable_Override; //[1 = override; 0 = use defaults]
  //<Embedded_Override_Start>  
  union {
    uint8_t  EmbeddedPlatformEn;    //member for RV1
    uint8_t  spare1;                //member for RV2
    uint8_t  MP2CLK_PwrDn;          //member for PCO
  }Field;
  //<Embedded_Override_End>
  uint8_t  SbTsiAlertComparatorModeEn;
  int32_t  VddcrVddVoltageMargin;   //[mV]
  int32_t  VddcrSocVoltageMargin;   //[mV]
  uint16_t FcwSlewFrac_L3FidTotalSteps;
  uint8_t  FcwSlewFrac_L3FidTotalStepsEn;
  uint8_t  WaitVidCompDis;          //[0 = use VOTF feedback from VR; 1 = use internal timer]
  uint8_t  UseCcxFreqCurveFuses;
  uint8_t  UseGfxFreqCurveFuses;
  uint8_t  UseCacSiddFuses;
  uint8_t  ApplyRichtekVrmPsi0Workaround;

  //CBS debug options [AVFS coeffients - signed 2's complement with 24 fractional bits]
  int32_t CorePsfFreq2;
  int32_t CorePsfFreq;
  int32_t CorePsfGfxActive;
  int32_t CorePsfTemp;
  int32_t CorePsfSidd;
  int32_t CorePsfCac;
  int32_t CorePsfNumActiveCores;
  int32_t CorePsfSigma;
  int32_t CorePsfAcBtc;
  int32_t L3PsfFreq2;
  int32_t L3PsfFreq;
  int32_t L3PsfGfxActive;
  int32_t L3PsfTemp;
  int32_t L3PsfSidd;
  int32_t L3PsfCac;
  int32_t L3PsfNumActiveCores;
  int32_t L3PsfSigma;
  int32_t L3PsfAcBtc;
  int32_t GfxPsfFreq2;
  int32_t GfxPsfFreq;
  int32_t GfxPsfGfxActive;
  int32_t GfxPsfTemp;
  int32_t GfxPsfSidd;
  int32_t GfxPsfCac;
  int32_t GfxPsfNumActiveCores;
  int32_t GfxPsfSigma;
  int32_t GfxPsfAcBtc;

  //PCD/CBS Telemetry setup
  uint32_t Telemetry_VddcrVddSlope;
  int32_t Telemetry_VddcrVddOffset;
  uint32_t Telemetry_VddcrSocSlope;
  int32_t Telemetry_VddcrSocOffset;

  // LIVmin Entry voltage
  uint8_t  LivMinEntryVID;
  uint8_t  spare3[3]; //placeholder for RV2 LIVmin Entry voltage
  uint32_t Telemetry_VddcrVddSlope2;
  uint32_t Telemetry_VddcrVddSlope3;
  uint32_t Telemetry_VddcrVddSlope4;
  uint32_t Telemetry_VddcrVddSlope5;
  uint32_t MinS0i3SleepTimeInMs;
  //PCD/CBS STT (System Temperature Tracking) Configuration
  uint32_t STT_MinLimit; // Power [mWatts]
  uint8_t  CalculateIOPhyDataDis;
  uint8_t  FmaxTempThreshold;
  uint16_t FmaxTempFrequency;

  uint8_t  UsbPortsToClearWceWde[2]; //Set bits to indicate that WceWde needs to be cleared
  uint8_t  minSocVidOffset;          // ULV SOC vid adjustment
  uint8_t  AclkDpm0Freq400MHz;

  uint16_t ForceVddcrVddVoltage; //[mV; 0 means no force]
  uint16_t ForceVddcrSocVoltage; //[mV; 0 means no force]

  uint32_t WLAN; //[31:24]Offset,[23:16]PcieBus,[15:8]PcieDevice,[7:0]PcieFunction
  uint8_t spare7;          //Reserved for Set_LPCRST
  uint8_t SparseControlDisable; //Default=0
  uint8_t spare8[2];       //placeholder
  uint32_t spare9[10];
} PPTable_t;


#endif


