/** @file
  Definition for Super I/O Policy Protocol

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

#ifndef _SIO_POLICY_PROTOCOL_H_
#define _SIO_POLICY_PROTOCOL_H_

#define EFI_SIO_POLICY_PROTOCOL_GUID \
  { \
    0x4940A33C, 0x13C6, 0x4CD8, 0xA7, 0xA7, 0x4A, 0x60, 0x20, 0xF4, 0xCC, 0x73 \
  }

#define EFI_SIO_COM1_ENABLE          1
#define EFI_SIO_COM1_DISABLE         0
#define EFI_SIO_LPT1_ENABLE          1
#define EFI_SIO_LPT1_DISABLE         0
#define EFI_SIO_FDD_ENABLE           1
#define EFI_SIO_FDD_DISABLE          0
#define EFI_SIO_FDD_WRITE_ENABLE     1
#define EFI_SIO_FDD_WRITE_PROTECT    0
#define EFI_SIO_PS2_KEYBOARD_ENABLE  1
#define EFI_SIO_PS2_KEYBOARD_DISABLE 0
#define EFI_SIO_PS2_MOUSE_ENABLE     1
#define EFI_SIO_PS2_MOUSE_DISABLE    0
#define EFI_SIO_RESERVED_DEFAULT     0

typedef struct {
  UINT16  Com1 : 1;               // 0 = Disable, 1 = Enable
  UINT16  Lpt1 : 1;               // 0 = Disable, 1 = Enable
  UINT16  Floppy : 1;             // 0 = Disable, 1 = Enable
  UINT16  FloppyWriteProtect : 1; // 0 = Write Protect, 1 = Write Enable
  UINT16  Ps2Keyboard : 1;        // 0 = Disable, 1 = Enable
  UINT16  Ps2Mouse : 1;           // 0 = Disable, 1 = Enable
  UINT16  Rsvd : 10;              // 0 = Disable, 1 = Enable
} EFI_SIO_DEVICE_ENABLES;

typedef struct _EFI_SIO_POLICY_PROTOCOL {
  EFI_SIO_DEVICE_ENABLES  DeviceEnables;
} EFI_SIO_POLICY_PROTOCOL;

extern EFI_GUID gEfiSioPolicyProtocolGuid;

#endif
