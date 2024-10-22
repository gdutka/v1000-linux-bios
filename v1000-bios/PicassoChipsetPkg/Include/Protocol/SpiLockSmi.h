/** @file
  Definition for Spi lock smi Protocol

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPI_LOCK_SMI_PROTOCOL_H_
#define _SPI_LOCK_SMI_PROTOCOL_H_

typedef struct _SPI_LOCK_SMI_PROTOCOL SPI_LOCK_SMI_PROTOCOL;

#define SPI_LOCK_SMI_PROTOCOL_GUID \
 {0x815cd2a5, 0x80aa, 0x462f, 0xa9, 0x94, 0x37, 0xd9, 0xdf, 0xa9, 0xae, 0xa6}

typedef
EFI_STATUS
(EFIAPI *SPI_LOCK_SMI_LOCK_SPI) (
  IN SPI_LOCK_SMI_PROTOCOL  *This
);

typedef
EFI_STATUS
(EFIAPI *SPI_LOCK_SMI_UNLOCK_SPI) (
  IN SPI_LOCK_SMI_PROTOCOL  *This
);

typedef struct _SPI_LOCK_SMI_PROTOCOL {
  SPI_LOCK_SMI_LOCK_SPI    LockSpi;
  SPI_LOCK_SMI_UNLOCK_SPI  UnlockSpi;
  UINTN                    SpiBarAddress;
} SPI_LOCK_SMI_PROTOCOL;

extern EFI_GUID gEfiSpiLockSmiProtocolGuid;

#endif //_SPI_LOCK_SMI_PROTOCOL_H_