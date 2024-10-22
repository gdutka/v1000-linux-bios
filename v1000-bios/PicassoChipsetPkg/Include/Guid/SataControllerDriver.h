/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _SATA_CONTROLLER_DRIVER_GUID_H_
#define _SATA_CONTROLLER_DRIVER_GUID_H_

#define SATA_CONTROLLER_DRIVER_GUID \
  { \
    0x8f50dafb, 0xb373, 0x440b, 0x9e, 0xe2, 0x62, 0xb6, 0x65, 0xb6, 0x1d, 0x73 \
  }

#define SATA_RAID_CONTROLLER_GUID \
  { \
    0x353e427b, 0xba1f, 0x4177, 0x8c, 0xa9, 0x44, 0xf0, 0x50, 0xc7, 0xe6, 0xae \
  }

extern EFI_GUID gSataControllerDriverGuid;
extern EFI_GUID gSataRaidControllerGuid;

#endif