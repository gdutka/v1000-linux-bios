/** @file
  InstallVerbTablePei peim will produces the InstallVerbTable Ppi for Pei driver.

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
/*++

  CPUs and Chipsets Porting File, This file contains coding which is specific to 
  how InsydeH2O supports CPUs and chipsets independently of the core code. All 
  CPUs, chipset and/or hardware platform specific modifications will be included  
  in this file.
  
--*/

#ifndef _RECOVERY_CPU_INIT_PEI_
#define _RECOVERY_CPU_INIT_PEI_

#include <Base.h>
#include <Library/PeiServicesLib.h>
#include <Library/PciExpressLib.h>
#include <Library/BaseLib.h>
#include <Ppi/CpuIo.h>
#include <Include/Ppi/CpuIo.h>
#include <Ppi/RecoveryModule.h>
#include <Ppi/PciCfg2.h>

#ifndef MAX_SUPPORTED_MEM
  #ifndef PCIEX_BASE_ADDRESS
    #ifndef PCI_EXPRESS_BASE
      #define     MAX_SUPPORTED_MEMORY     0xE0000000
    #else
      #define     MAX_SUPPORTED_MEMORY     PCI_EXPRESS_BASE
    #endif
  #else
    #define     MAX_SUPPORTED_MEMORY     PCIEX_BASE_ADDRESS
  #endif
#endif

#define PCIEX_LENGTH_1  0x08000000
#define RES_MEM_BASE_2 (MAX_SUPPORTED_MEMORY + PCIEX_LENGTH_1)
#define RES_MEM_LIMIT_2 (0xFEC00000 - 1)

#define PEI_PCI_CFG_ADDRESS(bus, dev, func, reg)  ( \
      (UINT64) ((((UINTN) bus) << 24) + (((UINTN) dev) << 16) + (((UINTN) func) << 8) + ((UINTN) reg)) \
    ) & 0x00000000ffffffff

EFI_STATUS
ReocoveryCpuInitEntryPoint (
  IN EFI_FFS_FILE_HEADER                                    *FfsHeader,
  IN CONST EFI_PEI_SERVICES                                 **PeiServices
  );
#endif