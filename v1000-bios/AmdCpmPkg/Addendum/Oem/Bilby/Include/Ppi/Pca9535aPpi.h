/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _PCA9535A_PPI_H_
#define _PCA9535A_PPI_H_

typedef
EFI_STATUS
(EFIAPI *PCA9535A_SET_PPI) (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  IN       UINT16            Data
  );

typedef
EFI_STATUS
(EFIAPI *PCA9535A_GET_PPI) (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  OUT      UINT16            *Data
  );

///
/// This PPI provide interface to set PCA9454A I2C switch chip.
///
typedef struct _EFI_PEI_PCA9535A_PPI {
  UINTN    Revision;                  ///< Revision Number
  PCA9535A_SET_PPI Set;               ///< Get control byte
  PCA9535A_GET_PPI Get;               ///< Set control byte
} EFI_PEI_PCA9535A_PPI;

// Current PPI revision
#define PCA9535A_PPI_REVISION   (0x00)

extern EFI_GUID gPca9535aPpiGuid;

#endif // _PCA9535A_PPI_H_
