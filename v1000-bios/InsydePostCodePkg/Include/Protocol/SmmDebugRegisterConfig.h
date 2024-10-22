/** @file

  Smm Debug Register Configure protocol definition

;******************************************************************************
;* Copyright (c) 2016 - 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SMM_DEBUG_REGISTER_CONFIG_H_
#define _SMM_DEBUG_REGISTER_CONFIG_H_

typedef
VOID
(EFIAPI *CONFIG_DEBUG_REGISTER) (
  VOID
  );

typedef struct _SMM_DEBUG_REGISTER_CONFIG_PROTOCOL {
  CONFIG_DEBUG_REGISTER ConfigDebugRegister;
} SMM_DEBUG_REGISTER_CONFIG_PROTOCOL;

extern EFI_GUID gH2OPostCodeSmmDebugRegProtocolGuid;

#endif
