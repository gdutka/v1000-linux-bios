/** @file

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
#ifndef _PROCESS_PENDING_CAPSULE_LIB_H_
#define _PROCESS_PENDING_CAPSULE_LIB_H_

//#include <Uefi.h>

EFI_STATUS
SetEspFlashFlag(
  VOID
  );


EFI_STATUS
ProcessPendingCapsule (
  VOID
  );

#endif

