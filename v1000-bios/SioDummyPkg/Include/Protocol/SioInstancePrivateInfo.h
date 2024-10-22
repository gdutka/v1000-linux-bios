/** @file
 This file contains private information protocol

 This protocol is not intended for any code outside of this SIO to use.

;******************************************************************************
;* Copyright 2017 Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/
#ifndef _SIO_INSTANCE_PRIVATE_INFO_H_
#define _SIO_INSTANCE_PRIVATE_INFO_H_

#define SIO_INSTANCE_PRIVATE_INFO_PROTOCOL_GUID \
  { 0x3e06b922, 0x9ad1, 0x4784, 0x81, 0xf5, 0x89, 0xb1, 0x78, 0x47, 0x15, 0xa6 }

typedef enum _SIO_UID_NUMBER {
  SioUid0 = 0,
  SioUid1,
  SioUid2,
  SioUid3,
  SioUid4,
  SioUid5,
  SioUid6,
  SioUid7,
  SioUid8,
  SioUid9,
  SioUidMaximum
} SIO_UID_NUMBER;

typedef struct _SIO_CONFIGURED_RESOURCE {
  UINT8                        ChipVendor;
  UINT8                        ChipInstance;
  UINT16                       ConfigPort;
  UINT8                        DevInstance;
  VOID                         *SetupConfig;
  EFI_HANDLE                   Handle;
} SIO_CONFIGURED_RESOURCE;

typedef EFI_STATUS (EFIAPI *SIO_DEVICE_INITIALIZE) (SIO_CONFIGURED_RESOURCE*);

typedef struct _SIO_LOGICAL_DEVICE_PRIVATE_INFO {
  UINT8                    LogicalDevType;
  UINT8                    DevInstance;
  SIO_DEVICE_INITIALIZE    DevInitialize;
  UINT8                    Ldn;
} SIO_LOGICAL_DEVICE_PRIVATE_INFO;

typedef struct _SIO_INSTANCE_PRIVATE_INFO_PROTOCOL {
  UINT8                              ChipVendor;
  SIO_UID_NUMBER                     Uid;
  UINT8                              ChipIdMSB;
  UINT8                              ChipIdLSB;
  UINT8                              ChipInstance;
  UINT16                             ConfigPort;
  SIO_LOGICAL_DEVICE_PRIVATE_INFO    *LogicalDevInfo;
} SIO_INSTANCE_PRIVATE_INFO_PROTOCOL;

extern EFI_GUID gH2OSioInstancePrivateInfoProtocolGuid;

#endif
