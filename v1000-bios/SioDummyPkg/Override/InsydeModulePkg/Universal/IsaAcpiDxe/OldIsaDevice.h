/** @file
  Declarations for old ISA device protocols

  All definition in this file must be consistent with the definition in
  SioSchXXXPkg/Include/SioResource.h in any Super I/O 5.0 package.

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

#ifndef _OLD_ISA_DEVICE_H_
#define _OLD_ISA_DEVICE_H_

#include <Uefi.h>

#include <Protocol/H2OIsaDevice.h>

#define EFI_ISA_DEVICE_RESOURCE   H2O_ISA_DEVICE_RESOURCE

typedef EFI_STATUS (EFIAPI *SET_POWER) (BOOLEAN);
typedef EFI_STATUS (EFIAPI *ENABLE_DEVICE) (BOOLEAN);
typedef EFI_STATUS (EFIAPI *GET_POSSIBLE_RESOURCE) (EFI_ISA_DEVICE_RESOURCE **, UINT32 *);
typedef EFI_STATUS (EFIAPI *GET_CURRENT_RESOURCE) (EFI_ISA_DEVICE_RESOURCE **);
typedef EFI_STATUS (EFIAPI *SET_RESOURCE) (EFI_ISA_DEVICE_RESOURCE *);

typedef struct {
  SET_POWER               SetPower;
  GET_POSSIBLE_RESOURCE   GetPossibleResource;
  SET_RESOURCE            SetResource;
  ENABLE_DEVICE           EnableDevice;
  GET_CURRENT_RESOURCE    GetCurrentResource;
} EFI_ISA_ACPI_DEVICE_FUNCTION;

#endif