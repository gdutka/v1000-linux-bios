/* $NoKeywords:$ */
/**
 * @file
 *
 * SMU services
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
#ifndef _NBIOSMUV10LIB_H_
#define _NBIOSMUV10LIB_H_

#include <NbioRegisterTypes.h>

#define VDDOFF_VID_FP5 0xF8   //0xB8
#define VDDOFF_VID_AM4 0xFF

// SMU Response Codes:
#define BIOSSMC_Result_OK                    0x1  // The operations was successful.
#define BIOSSMC_Result_Failed                0xFF
#define BIOSSMC_Result_UnknownCmd            0xFE
#define BIOSSMC_Result_CmdRejectedPrereq     0xFD
#define BIOSSMC_Result_CmdRejectedBusy       0xFC

// Message Definitions:
#define SMC_MSG_TestMessage                   0x1
#define SMC_MSG_GetSmuVersion                 0x2
#define SMC_MSG_GetBiosIfVersion              0x3
#define BIOSSMC_MSG_GetNameString             0x4
#define SMC_MSG_EnableSmuFeatures             0x5
#define SMC_MSG_DisableSmuFeatures            0x6
#define SMC_MSG_DramLogSetDramAddrHigh        0x7
#define SMC_MSG_DramLogSetDramAddrLow         0x8
#define SMC_MSG_DramLogSetDramSize            0x9
#define SMC_MSG_DxioTestMessage               0xA
#define SMC_MSG_ReadCoreCacWeightRegister     0xB
#define SMC_MSG_SleepEntry                    0xC
#define SMC_MSG_SetGbeStatus                  0xD
#define SMC_MSG_PowerUpGfx                    0xE
#define SMC_MSG_PowerUpSata                   0xF
#define SMC_MSG_PowerDownSata                 0x10
#define SMC_MSG_DisableSataController         0x11
#define SMC_MSG_SetBiosDramAddrHigh           0x12
#define SMC_MSG_SetBiosDramAddrLow            0x13
#define SMC_MSG_SetToolsDramAddrHigh          0x14
#define SMC_MSG_SetToolsDramAddrLow           0x15
#define SMC_MSG_TransferTableSmu2Dram         0x16 //! RS
#define SMC_MSG_TransferTableDram2Smu         0x17 //! RS
#define SMC_MSG_PowerSourceAC                 0x18
#define SMC_MSG_PowerSourceDC                 0x19
#define SMC_MSG_SetSustainedPowerLimit        0x1A
#define SMC_MSG_SetFastPPTLimit               0x1B
#define SMC_MSG_SetSlowPPTLimit               0x1C
#define SMC_MSG_SetSlowPPTTimeConstant        0x1D
#define SMC_MSG_SetStapmTimeConstant          0x1E
#define SMC_MSG_SetTctlMax                    0x1F
#define SMC_MSG_SetVrmCurrentLimit            0x20
#define SMC_MSG_SetVrmSocCurrentLimit         0x21
#define SMC_MSG_SetVrmMaximumCurrentLimit     0x22
#define SMC_MSG_SetVrmSocMaximumCurrentLimit  0x23
#define SMC_MSG_SetPSI0CurrentLimit           0x24
#define SMC_MSG_SetPSI0SocCurrentLimit        0x25
#define SMC_MSG_SetProchotDeassertionRampTime 0x26
#define SMC_MSG_UpdateSkinTempError           0x27
#define SMC_MSG_SetGpuApertureLow             0x28
#define SMC_MSG_SetGpuApertureHigh            0x29
#define SMC_MSG_StartGpuLink                  0x2A
#define SMC_MSG_StopGpuLink                   0x2B
#define SMC_MSG_UsbD3Entry                    0x2C
#define SMC_MSG_UsbD3Exit                     0x2D
#define SMC_MSG_UsbInit                       0x2E
#define SMC_MSG_AcBtcStartCal                 0x2F
#define SMC_MSG_AcBtcStopCal                  0x30
#define SMC_MSG_AcBtcEndCal                   0x31
#define SMC_MSG_DcBtc                         0x32
#define SMC_MSG_BtcRestoreOnS3Resume          0x33
#define SMC_MSG_SetGpuDeviceId                0x34
#define SMC_MSG_SetUlvVidOffset               0x35
#define SMC_MSG_DisablePSI                    0x36
#define SMC_MSG_EnablePostCode                0x37
#define SMC_MSG_UsbConfigUpdate               0x38
#define SMC_MSG_SetupUSB31ControllerTrap      0x39
#define SMC_MSG_SetVddOffVid                  0x3A
#define SMC_MSG_SetVminFrequency              0x3B
#define SMC_MSG_SetFrequencyMax               0x3C
#define SMC_MSG_SetGfxclkOverdriveByFreqVid   0x3D // AM4 only, check fuse, turn on OD and sets SoftMin GFXCLK, disables infra limits, prochot remains on, not limited by FMax set by fuses. Vid is limited by fused vid
#define SMC_MSG_PowerGateXgbe                 0x3E
#define SMC_MSG_OC_Disable                    0x3F
#define SMC_MSG_OC_VoltageMax                 0x40 //mV
#define SMC_MSG_OC_FrequencyMax               0x41 //MHz
#define SMC_MSG_EnableCC6Filter               0x42
#define SMC_MSG_GetSustainedPowerAndThmLimit  0x43
#define SMC_MSG_SetSoftMaxCCLK                0x44
#define SMC_MSG_SetSoftMinCCLK                0x45
#define SMC_MSG_SetSoftMaxGfxClk              0x46
#define SMC_MSG_SetSoftMinGfxClk              0x47
#define SMC_MSG_SetSoftMaxSocclkByFreq        0x48
#define SMC_MSG_SetSoftMinSocclkByFreq        0x49
#define SMC_MSG_SetSoftMaxFclkByFreq          0x4A
#define SMC_MSG_SetSoftMinFclkByFreq          0x4B
#define SMC_MSG_SetSoftMaxVcn                 0x4C
#define SMC_MSG_SetSoftMinVcn                 0x4D
#define SMC_MSG_SetSoftMaxLclk                0x4E
#define SMC_MSG_SetSoftMinLclk                0x4F
#define SMC_MSG_UsbXhciControllerDisabled     0x50
#define SMC_MSG_GetOpnSpareFuse               0x51
#define SMC_MSG_SetExternalPcieBuses          0x52
#define SMC_MSG_PowerDownGfx                  0x53
#define SMC_MSG_SetSlowPPTLimitApuOnly        0x54
#define SMC_MSG_GetCorePerfOrder              0x55
#define SMC_MSG_SetGenericPeRstGpioAddrOffset 0x56
#define SMC_MSG_SetFitLimitScalar             0x57
#define SMC_MSG_EnableOverclocking            0x58
#define SMC_MSG_SetOverclockCclkFreqAllCores  0x59
#define SMC_MSG_SetOverclockCclkFreqPerCore   0x5A
#define SMC_MSG_SetOverclockVID               0x5B
#define SMC_MSG_GetOverclockCap               0x5C
#define SMC_MSG_SetApmlHighTempThreshold      0x5D
#define SMC_MSG_SetupNvmeTrap                 0x5E
#define SMC_MSG_GetNumNvmeTrap                0x5F
#define SMC_MSG_Spared                        0x60
#define BIOSSMC_MSG_EnableNVMESelfRefresh     0x61
#define BIOSSMC_MSG_GetScratchRegister        0x62
#define BIOSSMC_MSG_SetScratchRegister        0x63
#define SMC_Message_Count                     0x64
#define INVALID_SMU_MSG                       0xFF

#define SMC_MSG_DXIO_SERVICE    SMC_MSG_DxioTestMessage
#define SMC_MSG_S3Entry         SMC_MSG_SleepEntry
typedef struct {
  UINT32 Function : 3;
  UINT32 Device : 5;
  UINT32 Bus : 8;
  UINT32 Spare : 16;
} BIOSSMC_MSG_SetupNvmeTrap_t;
typedef union {
  UINT32 val;
  BIOSSMC_MSG_SetupNvmeTrap_t f;
} BIOSSMC_MSG_SetupNvmeTrap_u;
typedef struct {
  UINT8 NvmeTrapsRemaining;
  UINT8 NvmeTrapsUsed;
  UINT8 NvmeTrapsTotal;
  UINT8 Spare;
} BIOSSMC_MSG_GetNumNvmeTrap_t;
typedef union {
  UINT32 val;
  BIOSSMC_MSG_GetNumNvmeTrap_t f;
} BIOSSMC_MSG_GetNumNvmeTrap_u;


// Address
#define MP1_C2PMSG_MESSAGE_ADDRESS                         0x3B10528UL

// Type
#define MP1_C2PMSG_MESSAGE_TYPE                            TYPE_SMN

#define MP1_C2PMSG_MESSAGE_CONTENT_OFFSET                  0
#define MP1_C2PMSG_MESSAGE_CONTENT_WIDTH                   32
#define MP1_C2PMSG_MESSAGE_CONTENT_MASK                    0xffffffffL

/// MP1_C2PMSG_MESSAGE
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_MESSAGE_STRUCT;

// Address
#define MP1_C2PMSG_RESPONSE_ADDRESS                        0x3B10564UL

// Type
#define MP1_C2PMSG_RESPONSE_TYPE                           TYPE_SMN

#define MP1_C2PMSG_RESPONSE_CONTENT_OFFSET                 0
#define MP1_C2PMSG_RESPONSE_CONTENT_WIDTH                  32
#define MP1_C2PMSG_RESPONSE_CONTENT_MASK                   0xffffffffL

/// MP1_C2PMSG_RESPONSE
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_RESPONSE_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_0_ADDRESS                      0x3B10998UL

// Type
#define MP1_C2PMSG_ARGUMENT_0_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_0_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_0_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_0_CONTENT_MASK                 0xffffffffL

/// MP1_C2PMSG_ARGUMENT_0
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_0_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_1_ADDRESS                      0x3B1099CUL

// Type
#define MP1_C2PMSG_ARGUMENT_1_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_1_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_1_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_1_CONTENT_MASK                 0xffffffffL

/// MP1_C2PMSG_ARGUMENT_1
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_1_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_2_ADDRESS                      0x3B109A0UL

// Type
#define MP1_C2PMSG_ARGUMENT_2_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_2_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_2_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_2_CONTENT_MASK                 0xffffffffL

/// MP1_C2PMSG_ARGUMENT_2
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_2_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_3_ADDRESS                      0x3B109A4UL

// Type
#define MP1_C2PMSG_ARGUMENT_3_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_3_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_3_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_3_CONTENT_MASK                 0xffffffffL

/// MP1_C2PMSG_ARGUMENT_3
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_3_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_4_ADDRESS                      0x3B109a8UL

// Type
#define MP1_C2PMSG_ARGUMENT_4_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_4_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_4_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_4_CONTENT_MASK                 0xffffffffL

/// MP1_C2PMSG_ARGUMENT_4
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_4_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_5_ADDRESS                      0x3B109ACUL

// Type
#define MP1_C2PMSG_ARGUMENT_5_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_5_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_5_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_5_CONTENT_MASK                 0xffffffffL

/// MP1_C2PMSG_ARGUMENT_5
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_5_STRUCT;


/// SMU V10 basic lib

VOID
NbioSmuServiceCommonInitArgumentsV10 (
  IN OUT   UINT32                   *SmuArg
  );

UINT32
NbioSmuServiceRequestV10 (
  IN       PCI_ADDR                 NbioPciAddress,
  IN       UINT32                   RequestId,
  IN OUT   UINT32                   *RequestArgument,
  IN       UINT32                   AccessFlags
  );

AGESA_STATUS
NbioSmuFirmwareTestV10 (
  IN       GNB_HANDLE               *NbioHandle
  );

UINT32
NbioDxioServiceRequestV10 (
  IN       GNB_HANDLE               *NbioHandle,
  IN       UINT32                   DxioMBoxRequest,
  IN  OUT  UINT32                   *DxioMBoxArg,
  IN       UINT32                   AccessFlags
  );

VOID
SmuNotifyS3Entry (
  );

#endif


