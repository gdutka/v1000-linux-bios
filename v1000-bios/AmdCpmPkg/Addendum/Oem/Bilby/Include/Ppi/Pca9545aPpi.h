/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _PCA9545A_PPI_H_
#define _PCA9545A_PPI_H_

typedef
EFI_STATUS
(EFIAPI *PCA9545A_SET_PPI) (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             ControlByte
  );

typedef
EFI_STATUS
(EFIAPI *PCA9545A_GET_PPI) (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  OUT      UINT8             *ControlByte
  );

///
/// This PPI provide interface to set PCA9454A I2C switch chip.
///
typedef struct _EFI_PEI_PCA9545A_PPI {
  UINTN    Revision;                  ///< Revision Number
  PCA9545A_SET_PPI Set;               ///< Get control byte
  PCA9545A_GET_PPI Get;               ///< Set control byte
} EFI_PEI_PCA9545A_PPI;

// Current PPI revision
#define PCA9545A_PPI_REVISION   (0x00)

extern EFI_GUID gPca9545aPpiGuid;

#endif // _PCA9545A_PPI_H_
