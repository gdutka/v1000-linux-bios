/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _PCA9545A_DXE_H_
#define _PCA9545A_DXE_H_


//
// Functions Prototypes
//

EFI_STATUS
EFIAPI
Pca9545aDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
EFIAPI
Pca9545aSetDxe (
  IN       EFI_DXE_PCA9545A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             ControlByte
  );

EFI_STATUS
EFIAPI
Pca9545aGetDxe (
  IN       EFI_DXE_PCA9545A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  OUT      UINT8             *ControlByte
  );

#endif // _PCA9545A_DXE_H_
