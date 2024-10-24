/** @file
  Include file for InstallVerbTable PEIM.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _INSTALL_VERB_TABLE_H_
#define _INSTALL_VERB_TABLE_H_

#include <Base.h>
#include <Ppi/Stall.h>
#include <Ppi/VerbTable.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiChipsetSvcLib.h>
#include <Guid/AzaliaPolicy.h>

#define STALL_TIME                      100
#define DETECT_CODEC_STALL_TIME         1
#define HDA_INIT_FAIL_RETRY_STALL_TIME  300
#define TIME_OUT_LOOP_INIT              0x00
#define TIME_OUT_MAX_LOOP               1000
#define DETECT_CODEC_TIME_OUT_MAX_LOOP  10000
#define HDA_MAX_SDIN_NUM                0x0F
#define HDA_MAX_SDIN_FLG                0x7FFF
#define HDA_SDIN_INIT                   0x01
#define HDA_SDIN_NUM                    0x04
#define HDA_SDIN_FLG                    0x0F
#define HDA_NO_CODEC                    0x00

//
// Codec Parameters and Controls Command
//
#define CONTROL_GET_PARAMETER       0xF00
#define PARAMETER_VENDOR_DEVICE_ID  0x00
#define PARAMETER_REVISION_ID       0x02

//
// Register Definition
//
#define HDA_REG_GCAP                0x00
#define HDA_REG_GCTL                0x08
#define HDA_REG_GCTL_BIT_CRST       BIT0
#define HDA_REG_WAKEEN              0x0C
#define HDA_REG_STATESTS            0x0E
#define HDA_REG_STATESTS_BIT_SDIN0  0x01
#define HDA_REG_ICW                 0x60
#define HDA_REG_IRR                 0x64
#define HDA_REG_ICS                 0x68
#define HDA_REG_ICS_BIT_ICB         BIT0
#define HDA_REG_ICS_BIT_IRV         BIT1

//
// Common Register Definition
//
#define FCH_GPIO_REG10           0x0A
#define FCH_GPIO_REG167          0xA7
#define FCH_GPIO_REG168          0xA8
#define FCH_GPIO_REG169          0xA9
#define FCH_GPIO_REG170          0xAA
//
// PM IO Register Definition
//
#define FCH_PMIOA_REGEB          0xEB         // AzEn
//
// PCI Device Register Definition
//
#define FCH_AZ_REG04             0x04        // PCI Command
#define FCH_AZ_REG10             0x10        // Lower Base Address Register
#define FCH_AZ_REG42             0x42        // Misc Control 2 Register

#define FCH_IOMAP_REGC00         0x0C00  // Pci_Intr_Index
#define FCH_IOMAP_REGC01         0x0C01  // Pci_Intr_Data

#define AZALIA_ENABLE               1
#define AZALIA_DISABLE              0
//
// Function Definition
//
EFI_STATUS
PeimInstallVerbTableInitEntry (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
InstallVerbTablePpi (
  IN CONST EFI_PEI_SERVICES                **PeiServices
  );

EFI_STATUS
InstallVerbTable (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN COMMON_CHIPSET_AZALIA_VERB_TABLE      *OemVerbTableAddress
  );

EFI_STATUS
HdaInitialize (
  IN CONST EFI_PEI_SERVICES                **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi
  );

EFI_STATUS
HdaReset (
  IN CONST EFI_PEI_SERVICES                **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi
  );

EFI_STATUS
FindOutCodec (
  IN     CONST EFI_PEI_SERVICES            **PeiServices,
  IN     UINT32                            HdaBar,
  IN     EFI_PEI_STALL_PPI                 *StallPpi,
  IN OUT UINT16                            *SdinWake
  );

EFI_STATUS
DeliverCommandToCodec (
  IN CONST EFI_PEI_SERVICES                **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi,
  IN UINT32                                CommandData
  );

EFI_STATUS
ReceiveCodecData (
  IN     CONST EFI_PEI_SERVICES            **PeiServices,
  IN     UINT32                            HdaBar,
  IN     EFI_PEI_STALL_PPI                 *StallPpi,
  IN OUT UINT32                            *ResponseData
  );

EFI_STATUS
ChipsetHdaInitHook (
  IN UINT32                                HdaBar
  );
  
EFI_STATUS
ConfigureAzalia (
  IN CONST EFI_PEI_SERVICES                **PeiServices
  );

EFI_STATUS
DetectAndInitializeAzalia (
  IN CONST EFI_PEI_SERVICES                **PeiServices,
  IN AZALIA_POLICY                         *AzaliaPolicy
  );
#endif
