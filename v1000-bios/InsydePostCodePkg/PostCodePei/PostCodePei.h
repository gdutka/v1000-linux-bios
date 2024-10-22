/** @file
  PostCodePei module header file

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

#ifndef _POST_CODE_PEI_H_
#define _POST_CODE_PEI_H_

#include <Library/PeimEntryPoint.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/PostCodeConfigLib.h>
#include <Library/SerialPortPostCodeLib.h>
#include <Library/GraphicVgaLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PostCodeMiscLib.h>
#include <Ppi/EndOfPeiPhase.h>

EFI_STATUS
EFIAPI 
DisableFeatureCallback (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi
  );

VOID
EFIAPI
PostCodeFeatureHandlerEntry (
  VOID
  );

#endif 
