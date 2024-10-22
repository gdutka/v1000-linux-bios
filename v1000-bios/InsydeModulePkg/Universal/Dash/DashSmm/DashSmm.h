/** @file
  It register callbacks for common Sw or Sx(s3/S4).

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DASH_SMM_
#define _DASH_SMM_


#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/MctpSmbus.h>
#include <Protocol/DashIoCfg.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>


#define ASF_SET_SYSTEM_STATE_CMD      0x01
#define ASF_SET_SYSTEM_STATE_SUB_CMD  0x18
#define ASF_SET_SYSTEM_STATE_VER_NUM  0x10

#define ASF_SYSTEM_STATE_S0           0x00
#define ASF_SYSTEM_STATE_S3           0x03
#define ASF_SYSTEM_STATE_S4           0x04
#define ASF_SYSTEM_STATE_S5           0x05


#pragma pack(1)

typedef struct {
  UINT8  SubCommand;
  UINT8  VersionNumber;
  UINT8  SystemState;
} ASF_SET_SYSTEM_STATE_MSG;

#pragma pack()


#endif
