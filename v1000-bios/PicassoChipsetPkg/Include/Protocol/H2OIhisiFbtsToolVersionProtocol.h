/** @file
  Definition for H2O IHISI FBTS tool version Protocol

;******************************************************************************
;* Copyright (c) 2013 - 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_IHISI_FBTS_TOOL_VERSION_PROTOCOL_H_
#define _H2O_IHISI_FBTS_TOOL_VERSION_PROTOCOL_H_

typedef struct _H2O_IHISI_FBTS_TOOL_VERSION_PROTOCOL {
  UINT16  IhisiVersion;
} H2O_IHISI_FBTS_TOOL_VERSION_PROTOCOL;


extern EFI_GUID gH2OIhisiFbtsToolVersionProtocolGuid;

#endif //_H2O_IHISI_FBTS_TOOL_VERSION_PROTOCOL_H_
