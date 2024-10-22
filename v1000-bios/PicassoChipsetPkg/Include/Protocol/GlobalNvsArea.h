/** @file
  Definition for Global NVS Area Protocol

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _GLOBAL_NVS_AREA_H_
#define _GLOBAL_NVS_AREA_H_

#include "OemGlobalNvsArea.h"
//
// Global NVS Area Protocol GUID
//
#define EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID \
{ 0x74e1e48, 0x8132, 0x47a1, 0x8c, 0x2c, 0x3f, 0x14, 0xad, 0x9a, 0x66, 0xdc }
//
// Revision id - Added TPM related fields
//
#define GLOBAL_NVS_AREA_RIVISION_1       1

#define DEFAULT_BATTERY_VALUE           25600
#define PRIMARY_DISPLAY                 1
#define DEFAULT_BRIGHTNESS_LEVEL        60

#define PIPE_A_CRT                      0x01
#define PIPE_A_TV1                      0x02
#define PIPE_A_EFP1                     0x04
#define PIPE_A_LFP                      0x08
#define PIPE_A_TV2                      0x10
#define PIPE_A_EFP2                     0x20
#define PIPE_B_CRT                      PIPE_A_CRT  << 8
#define PIPE_B_TV1                      PIPE_A_TV1  << 8
#define PIPE_B_EFP1                     PIPE_A_EFP1 << 8
#define PIPE_B_LFP                      PIPE_A_LFP  << 8
#define PIPE_B_TV2                      PIPE_A_TV2  << 8
#define PIPE_B_EFP2                     PIPE_A_EFP2 << 8

#define DEFAULT_LCD_ADDRESS             0x400

//
// Global NVS Area definition
//
// Please following the format to let GenGloblNvs tool auto generate the GloblNvs.asi
// <UINT8 or UINT16,UINT32,UINT64> <Name>;   // <AsiName>
// Please do not remove the signature.
#pragma pack (1)
typedef struct {
//  GenGloblNvs.exe signature
  UINT8 SmiFuncCall;                // SMIF
  UINT8 SmiFuncCallPram0;           // PRM0
  UINT8 SmiFuncCallPram1;           // PRM1
  UINT8 BrightnessLevel;            // BRTL
  UINT8 DispToggleList;             // TLST
  UINT8 IgdStatus;                  // IGDS
  UINT16 LcdAddr;                   // LCDA
  UINT16 CurrentDispState;          // CSTE
  UINT16 NextDispState;             // NSTE
  UINT16 CurrentAttachedDevList;    // CADL
  UINT16 PreAttachedDevList;        // PADL
  UINT8 LidStatus;                  // LIDS
  UINT8 PowerModeStatus;            // PWRS
  UINT32 BatteryValue;              // BVAL
  UINT16 AvailDispDevList;          // ADDL
  UINT8 SmiCommand;                 // BCMD
  UINT8 SmiCmdSubFunc;              // SBFN
  UINT32 FuncIdForAtif;             // DID
  UINT8 AtifInfo[0x100];            // INFO
  UINT8 TopOfMemLo;                 // TOML
  UINT8 TopOfMemHi;                 // TOMH
  UINT8 EnabledCpuBitMap;           // CEBP
  UINT8 Cpu0LowestState;            // C0LS
  UINT8 Cpu1LowestState;            // C1LS
  UINT8 Cpu0HighestState;           // C0HS
  UINT8 Cpu1HighestState;           // C1HS
  UINT32 RomSize;                   // ROMS
  UINT8 MuxFlag;                    // MUXF
  UINT8 PriDispDevNum;              // PDDN
  UINT8 ConnectStandbySupport;      // CNSB
  UINT8 AcpiReduceHwSupport;        // RDHW
  UINT8 DisableAcpiS3;              // DAS3
  UINT8 amdTNBHystersis;            // TNBH
  UINT8 amdTCPU0;                   // TCP0
  UINT8 amdTCPU1;                   // TCP1
  UINT8 amdTNB;                     // ATNB
  UINT8 amdPwmCPU0;                 // PCP0
  UINT8 amdPwmCPU1;                 // PCP1
  UINT8 amdPwmNb;                   // PWMN
  UINT8 amdLcdPanelType;            // LPTY
  UINT8 M92Disable;                 // M92D
  UINT8 WakeOnPME;                  // WKPM
  UINT8 LightSensor;                // ALST
  UINT8 amdFusionUtilityControl;    // AFUC
  UINT8 amdExternalUSBController;   // EXUS
  UINT8 GlobalNvsData0E;            // GV0E
  UINT8 amdWirelessSwitchSetting;   // WLSH
  UINT8 amdThermalSensorSelect;     // TSSS
  UINT8 amdOddZeroPowerControl;     // AOZP
  UINT8 AcpiThermalZone;            // TZFG
  UINT8 amdSystemBootWithPS0;       // BPS0
  UINT8 NbIoApic;                   // NAPC
  UINT32 PcieBaseAddress;           // PCBA
  UINT32 PcieBaseLimit;             // PCBL
  UINT8 AmdWlanWwan;                // WLAN
  UINT8 AmdBlueTooth;               // BLTH
  UINT8 AmdGpsSensor;               // GPSS
  UINT8 AmdNfcSensor;               // NFCS
  UINT8 amdSecondaryBatterySupport; // SBTY
  UINT16 BoardId;                   // BDID
  UINT8 MITTWITTControl;            // MWTT
  UINT8 AcpMode;                    // ACPM
  UINT8 KbcSupport;                 // KBCS
  UINT8 EcSupport;                  // ACEC
  UINT8 AmdDptcControl;             // DPTC
//>>EnableAbove4GBMmio++
  UINT8 Mmio64AslSupport;           // MM64 : Enable/Disable 64bits MMIO support
  UINT64 MmioMem64Base[2];          // HMBx : 64 bits MMIO base address
  UINT64 MmioMem64Limit[2];         // HMMx : 64 bits MMIO limit
  UINT64 MmioMem64Length[2];        // HMLx : 64 bits MMIO length
//<<EnableAbove4GBMmio++
} EFI_GLOBAL_NVS_AREA;
#pragma pack ()

//
// Global NVS Area Protocol
//
typedef struct _EFI_GLOBAL_NVS_AREA_PROTOCOL {
  EFI_GLOBAL_NVS_AREA        *Area;
  EFI_OEM_GLOBAL_NVS_AREA    *OemArea;
} EFI_GLOBAL_NVS_AREA_PROTOCOL;

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiGlobalNvsAreaProtocolGuid;

#endif

