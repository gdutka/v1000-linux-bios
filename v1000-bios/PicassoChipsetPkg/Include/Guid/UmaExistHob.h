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


#ifndef _UMA_EXIST_HOB_GUID_H_
#define _UMA_EXIST_HOB_GUID_H_

#define UMA_EXIST_HOB_GUID \
  { \
    0xEB417550, 0xDA25, 0x4F07, 0xA5, 0xAE, 0xE7, 0x11, 0x90, 0x11, 0x0D, 0x02 \
  }

extern EFI_GUID gUmaExistHobGuid;

//
// UmaExistFlag
//
#define  UMA_ENABLE    (1 << 0)  //BIT0: UMA enable
#define  UMA_UNDER_4G  (1 << 1)  //BIT1: UMA is under 4G

typedef struct {
  UINT8                   UmaExistFlag; //BIT0: UMA enable; BIT1: UMA is under 4G
  UINT32                  UmaSize;
} EFI_UMA_EXIST_HOB;

#endif
