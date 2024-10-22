/** @file
  Definitions for providing an interface for SATA and NVME storage devices 
  to NOT binding with Insyde Storage drivers

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __H2O_STORAGE_IGNORE_DEVICE_GUID_H__
#define __H2O_STORAGE_IGNORE_DEVICE_GUID_H__

#define H2O_STORAGE_IGNORE_DEVICE_GUID \
  { \
    0xae3bfa0f, 0xcb26, 0x43de, 0xa4, 0x34, 0xa1, 0x9c, 0xdc, 0x30, 0xfd, 0xa1 \
  };
#pragma pack(1)

typedef enum {
  Skipped = 0,  //It means this PCD instance will be skipped
  IgnoreAHCI,
  IgnoreNVME
} EFI_IGNORE_DEVICE_DETECT_TYPE;


typedef struct {
  UINT8                             Bus;
  UINT8                             Device;
  UINT8                             Function;
  UINT16                            VendorId;
  UINT16                            DeviceId;
  UINT32                            Flags;
  UINT8                             DeviceType;  
} STORAGE_IGNORE_DEV_TOKEN_SPACE;
#pragma pack()


extern EFI_GUID gH2OIgnoreDeviceListTokenSpaceGuid;

#endif
