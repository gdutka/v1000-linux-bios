/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _PCA9535A_DXE_H_
#define _PCA9535A_DXE_H_


//
// Functions Prototypes
//

EFI_STATUS
EFIAPI
Pca9535aDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
EFIAPI
Pca9535aSetDxe (
  IN       EFI_DXE_PCA9535A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  IN       UINT16            Data
  );

EFI_STATUS
EFIAPI
Pca9535aGetDxe (
  IN       EFI_DXE_PCA9535A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  OUT      UINT16            *Data
  );

#endif // _PCA9535A_DXE_H_
