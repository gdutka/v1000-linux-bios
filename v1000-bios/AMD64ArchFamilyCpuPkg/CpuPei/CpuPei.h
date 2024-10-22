/** @file

  Implementation of CPU driver for PEI phase.
  This PEIM is to expose the Cache PPI.

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

#ifndef _PEI_CPUPEIM_H_
#define _PEI_CPUPEIM_H_


#include <PiPei.h>

#include <Ppi/Cache.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/SecPlatformInformation.h>
#include <Guid/HtBistHob.h>

#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/MtrrLib.h>

#endif
