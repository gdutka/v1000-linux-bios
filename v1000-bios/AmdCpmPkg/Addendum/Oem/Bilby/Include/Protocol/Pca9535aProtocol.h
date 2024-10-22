/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _PCA9535A_PROTOCOL_H_
#define _PCA9535A_PROTOCOL_H_
///
/// Forward declaration for the EFI_DXE_PCA9535A_PROTOCOL
///
typedef struct _EFI_DXE_PCA9535A_PROTOCOL EFI_DXE_PCA9535A_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *PCA9535A_SET) (
  IN       EFI_DXE_PCA9535A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  IN       UINT16            Data
  );

typedef
EFI_STATUS
(EFIAPI *PCA9535A_GET) (
  IN       EFI_DXE_PCA9535A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  OUT      UINT16            *Data
  );

///
/// This PPI provide interface to set PCA9535A I2C switch chip.
///
struct _EFI_DXE_PCA9535A_PROTOCOL {
  UINTN    Revision;              ///< Revision Number
  PCA9535A_SET Set;               ///< Get control byte
  PCA9535A_GET Get;               ///< Set control byte
};

// Current DXE revision
#define PCA9535A_DXE_REVISION   (0x00)

extern EFI_GUID gPca9535aProtocolGuid;

#endif // _PCA9535A_PROTOCOL_H_

