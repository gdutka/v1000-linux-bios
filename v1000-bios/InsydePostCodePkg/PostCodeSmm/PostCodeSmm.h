/** @file
  PostCodeSmm driver header file

;******************************************************************************
;* Copyright (c) 2016 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _POST_CODE_SMM_H_
#define _POST_CODE_SMM_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/PostCodeConfigLib.h>
#include <Library/SerialPortPostCodeLib.h>
#include <Library/GraphicVgaLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/VgaPostCodeLib.h>
#include <Library/PostCodeMiscLib.h>
#include <Protocol/SmmReadyToLock.h>
#include <Protocol/SmmDebugRegisterConfig.h>
#include <Protocol/VgaSupport.h>

VOID
EFIAPI
PostCodeFeatureHandlerEntry (
  VOID
  );

VOID
EFIAPI 
SmmSetUpDebugRegister (
  VOID
  );

#endif 
