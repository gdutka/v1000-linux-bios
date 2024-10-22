/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _M24LC128_PEI_H_
#define _M24LC128_PEI_H_

//Configuration defines.
#define M24LC128_PAGESIZE       (64)

typedef struct _WRITE_PAGE {
  UINT8 prependAddress[2];
  UINT8 TxData[M24LC128_PAGESIZE];
} WRITE_PAGE;


EFI_STATUS
EFIAPI
M24Lc128Write (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINTN                   BusSelect,
  IN UINTN                   SlaveAddress,
  IN UINT16                  Offset,
  IN UINT32                  Length,
  IN UINT8                   *Buffer
  );

EFI_STATUS
EFIAPI
M24Lc128Read (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINTN                   BusSelect,
  IN UINTN                   SlaveAddress,
  IN UINT16                  Offset,
  IN UINT32                  Length,
  OUT UINT8                  *Buffer
  );

EFI_STATUS
PageWrite (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  EFI_PEI_I2C_MASTER_PPI     *I2cMaster,
  IN UINTN                   SlaveAddress,
  IN UINT16                  Offset,
  IN UINT32                  Length,
  IN UINT8                   *Data
  );

#endif // _M24LC128_PEI_H_

