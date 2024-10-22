/** @file
  Definitions for restore factory default related code

;******************************************************************************
;* Copyright (c) 2015 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _VAR_RESTORE_FACTORY_DEFAULT_H_
#define _VAR_RESTORE_FACTORY_DEFAULT_H_

#include "Variable.h"

/**
  Initialize restore variable default service. This service uses to restore factory default
  if need.
--*/
VOID
InitializeRestoreVariableDefaultService (
  VOID
  );


#endif
