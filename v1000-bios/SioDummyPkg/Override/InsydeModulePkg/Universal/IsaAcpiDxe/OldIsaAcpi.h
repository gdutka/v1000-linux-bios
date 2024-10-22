/** @file
  Declarations for OldIsaAcpi.c

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _OLD_ISA_ACPI_H_
#define _OLD_ISA_ACPI_H_

#include <Uefi.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Protocol/IsaIo.h>

#include "OldIsaDevice.h"

#define NULL_ID                           0xFF

#define H2O_OLD_ISA_DEVICE_SIGNATURE      SIGNATURE_32 ('O', 'I', 's', 'D')
#define H2O_ISA_DEVICE_CONTEXT_SIGNATURE  SIGNATURE_32 ('I', 's', 'a', 'D')

typedef struct _H2O_OLD_ISA_DEVICE        H2O_OLD_ISA_DEVICE;

typedef struct {
  UINT32              Hid;
  EFI_GUID            *Guid;
  H2O_OLD_ISA_DEVICE  *Devices;
  UINTN               DeviceCount;
} H2O_OLD_ISA_PROTOCOL;

typedef struct {
  UINT32                    Signature;
  EFI_HANDLE                Handle;
  H2O_ISA_DEVICE_PROTOCOL   DeviceProtocol;
  H2O_ISA_DEVICE_INFO       DeviceInfo;
  H2O_ISA_DEVICE_RESOURCE   DeviceResource;
} H2O_ISA_DEVICE_CONTEXT;

#define H2O_ISA_DEVICE_CONTEXT_FROM_PROTOCOL(a) \
  CR (a, \
      H2O_ISA_DEVICE_CONTEXT, \
      DeviceProtocol, \
      H2O_ISA_DEVICE_CONTEXT_SIGNATURE \
      )

struct _H2O_OLD_ISA_DEVICE {
  UINT32                        Signature;
  EFI_ISA_ACPI_DEVICE_FUNCTION  *FunctionPtr;
  H2O_ISA_DEVICE_CONTEXT        DeviceContext;
};

#define H2O_OLD_ISA_DEVICE_FROM_CONTEXT(a) \
  CR ( \
      a, \
      H2O_OLD_ISA_DEVICE, \
      DeviceContext, \
      H2O_OLD_ISA_DEVICE_SIGNATURE \
      )

/**
  Publish old-policy ISA devices.

  Term "old-policy" means the old interface associated with gEfiIsaPnpXXXDeviceProtocolGuid
  which is about to be obsolete. This function would convert its interface to a new one.

  @retval EFI_SUCCESS   Successfully publishes old-policy ISA devices.
  @retval Other         The old-policy ISA devices could not be published.

**/
EFI_STATUS
PublishOldPolicyIsaDevices (
  VOID
  );

/**
  Revoke old-policy ISA devices.

  Term "old-policy" means the old interface associated with gEfiIsaPnpXXXDeviceProtocolGuid
  which is about to be obsolete. This function would uninstall protocol interface for all
  old-policy ISA devices and free new interfaces created by PublishOldPolicyIsaDevices().
  This function should be called only when error happens.

  @retval EFI_SUCCESS   Successfully revokes old-policy ISA devices.
  @retval Other         The old-policy ISA devices could not be revoked.

**/
EFI_STATUS
RevokeOldPolicyIsaDevices (
  VOID
  );

#endif