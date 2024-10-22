/** @file

;*******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _PLATFORM_DXE_DRIVER_H
#define _PLATFORM_DXE_DRIVER_H

#include <Uefi.h>
#include <IndustryStandard/Pci.h>

#include <Protocol/PciIo.h>
#include <Protocol/PciEnumerationComplete.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/Ps2Policy.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/HiiString.h>
#include <Protocol/SetupUtility.h>

#include <Pi/PiHob.h>
#include <Library/HobLib.h>

#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciExpressLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/ChipsetConfigLib.h>
#include <Library/DxeOemSvcChipsetLib.h>
#include <Library/EfiRegTableLib.h>

#include <AGESA.h>
#include <YangtzeReg.h>
#include <BiosRamUsage.h>

#include <Guid/PlatformHardwareSwitch.h>
#include <Guid/LoadAmdAhciUefiDriver.h>

#include <Register/LocalApic.h>

//
// Prototypes
//
EFI_STATUS
DxePlatformEntryPoint (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
Ps2PortSwap (
  IN  EFI_HANDLE  Handle
  );

EFI_STATUS
EFIAPI
Ps2SkipHWInit (
  IN  EFI_HANDLE  Handle
  );

VOID
EFIAPI
PlatformSpecialRegInit (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );

extern EFI_REG_TABLE gPlatformRegs[];

#endif
