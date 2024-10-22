/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _M24LC128_PPI_H_
#define _M24LC128_PPI_H_

typedef
EFI_STATUS
(EFIAPI *M24LC128_WRITE_PPI) (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINTN                   BusSelect,
  IN UINTN                   SlaveAddress,
  IN UINT16                  Offset,
  IN UINT32                  Length,
  IN UINT8                   *Data
  );

typedef
EFI_STATUS
(EFIAPI *M24LC128_READ_PPI) (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINTN                   BusSelect,
  IN UINTN                   SlaveAddress,
  IN UINT16                  Offset,
  IN UINT32                  Length,
  OUT UINT8                  *Data
  );

///
/// This PPI provide interface to access Micron 24LC2128 EEPROM chip.
///
typedef struct _EFI_PEI_M24LC128_PPI {
  UINTN    Revision;                     ///< Revision Number
  M24LC128_WRITE_PPI Write;              ///< Write routing
  M24LC128_READ_PPI  Read;               ///< Read routing
} EFI_PEI_M24LC128_PPI;

// Current PPI revision
#define M24LC128_PPI_REVISION   0x00

extern EFI_GUID gM24Lc128PpiGuid;

#endif // _M24LC128_PPI_H_

