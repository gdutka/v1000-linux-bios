/** @file
 GenericUtilityLib
 BDS library definition, include the file and data structure

;******************************************************************************
;* Copyright (c) 2012 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2004 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

#ifndef _INTERNAL_UTILITY_LIB_H_
#define _INTERNAL_UTILITY_LIB_H_

#include <Uefi.h>
#include <IndustryStandard/Pci22.h>

#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/BootOptionPolicy.h>
#include <Protocol/DevicePath.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/H2OBdsServices.h>

#include <Guid/GlobalVariable.h>
#include <Guid/DebugMask.h>
#include <Guid/FileInfo.h>
#include <Guid/InsydeModuleHii.h>

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/DevicePathLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/GenericUtilityLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#define VAR_FLAG  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE

#endif

