/** @file
  Header file to define all of the global variable names which defined in EFI
  specification.

  TODO: This file need to remove if MdePkg defines these globl variable names
        in MdePkg\Include\Guid\GlobalVariable.h

;******************************************************************************
;* Copyright (c) 2013 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef __VARIABLE_GLOBAL_VARIABLE_GUID_H__
#define __VARIABLE_GLOBAL_VARIABLE_GUID_H__
//
// Only define UEFI 2.5 spec new added global variable here and they will be removed if
// these variables are defined MdePkg\Include\Guid\GlobalVariable.h file
//


///
/// Whether the system is operating in Audit Mode (1) or not (0). All other values are reserved.
/// Should be treated as read-only except when DeployedMode is 0. Always becomes read-only after
/// ExitBootServices() is called.
/// Its attribute is BS+RT.
///
#define EFI_AUDIT_MODE_VARIABLE_NAME                L"AuditMode"
///
/// Whether the system is operating in Deployed Mode (1) or not (0). All other values are reserved.
/// Should be treated as read-only when its value is 1. Always becomes read-only after
/// ExitBootServices() is called.
/// Its attribute is BS+RT.
///
#define EFI_DEPLOYED_MODE_VARIABLE_NAME             L"DeployedMode"
///
/// OS-specified recovery options.
/// Its attribute is NV+BS+RT.
///
#define EFI_OS_RECOVERY_ORDER_VARIABLE_NAME         L"OsRecoveryOrder"
///
/// The ordered System Prep Application load option list.
/// Its attribute is BS+RT.
///
#define EFI_SYS_PREP_ORDER_VARIABLE_NAME             L"SysPrepOrder"

///
/// The ordered System Prep Application load option list.
/// Its attribute is NV+BS+RT.
///
#define EFI_SYS_PREP_ORDER_VARIABLE_NAME             L"SysPrepOrder"
#endif
