/** @file
  Crisis recovery header file

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CRISIS_RECOVERY_PEI_H
#define _CRISIS_RECOVERY_PEI_H

#include <Uefi.h>
#include <PiPei.h>
#include <KernelSetupConfig.h>
#include <Guid/RecoveryDevice.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Guid/FirmwareFileSystem3.h>
#include <Guid/AdminPassword.h>
#include <Guid/SystemPasswordVariable.h>
#include <Guid/H2OCp.h>
#include <Ppi/RecoveryModule.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Ppi/Speaker.h>
#include <Ppi/FirmwareAuthentication.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/FdSupportLib.h>
#include <Library/PostCodeLib.h>
#include <Library/HobLib.h>
#include <Library/VariableLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/PeiChipsetSvcLib.h>
#include <Library/SeamlessRecoveryLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/H2OCpLib.h>

#define FLASH_FAILURE_RETRY_COUNT   5

#define FLVALSIG                    0x0FF0A55A
#define START_FLASH                 3
#define ERASE_DONE                  2
#define PROGRAM_DONE                1


EFI_STATUS
FlashBios (
  IN UINT8                              *RecoveryCapsule,
  IN UINTN                              RecoveryCapsuleSize
  );

EFI_STATUS
EFIAPI
PlatformRecoveryModule (
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_RECOVERY_MODULE_PPI          *This
  );

#endif
