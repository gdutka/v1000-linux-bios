/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _PCA9545A_PROTOCOL_H_
#define _PCA9545A_PROTOCOL_H_
///
/// Forward declaration for the EFI_DXE_PCA9545A_PROTOCOL
///
typedef struct _EFI_DXE_PCA9545A_PROTOCOL EFI_DXE_PCA9545A_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *PCA9545A_SET) (
  IN       EFI_DXE_PCA9545A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             ControlByte
  );

typedef
EFI_STATUS
(EFIAPI *PCA9545A_GET) (
  IN       EFI_DXE_PCA9545A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  OUT      UINT8             *ControlByte
  );

///
/// This PPI provide interface to set PCA9545A I2C switch chip.
///
struct _EFI_DXE_PCA9545A_PROTOCOL {
  UINTN    Revision;              ///< Revision Number
  PCA9545A_SET Set;               ///< Get control byte
  PCA9545A_GET Get;               ///< Set control byte
};

// Current DXE revision
#define PCA9545A_DXE_REVISION   (0x00)

extern EFI_GUID gPca9545aProtocolGuid;

#endif // _PCA9545A_PROTOCOL_H_

