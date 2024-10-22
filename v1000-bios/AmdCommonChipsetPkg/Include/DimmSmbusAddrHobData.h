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

#ifndef _DIMM_SMBUS_ADDR_HOB_DATA_H_
#define _DIMM_SMBUS_ADDR_HOB_DATA_H_

typedef struct {
  UINT8   SocketId;
  UINT8   ChannelId;
  UINT8   DimmId;
  UINT8   SmbusAddress;
  BOOLEAN DimmPresent;
} DIMM_INFO_SMBUS;

#endif
