/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _PCA9545A_PEI_H_
#define _PCA9545A_PEI_H_

EFI_STATUS
EFIAPI
Pca9545aSet (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             ControlByte
  );

EFI_STATUS
EFIAPI
Pca9545aGet (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  OUT      UINT8             *ControlByte
  );

#endif // _PCA9545A_PEI_H_

