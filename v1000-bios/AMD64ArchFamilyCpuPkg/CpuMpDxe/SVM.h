//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

  CPUs and Chipsets Porting File, This file contains coding which is specific to
  how InsydeH2O supports CPUs and chipsets independently of the core code. All
  CPUs, chipset and/or hardware platform specific modifications will be included
  in this file.

--*/

#ifndef SVM_H
#define SVM_H

// #include "Tiano.h"
// #include "EfiDriverLib.h"
// #include "EfiCombinationLib.h"
// #include "MpService.h"
// #include "CpuRegs.h"
#include <Library/BaseLib.h>

#define AMD_MSR_VM_CR         0xC0010114
#define AMD_MSR_SVM_LOCK_KEY  0xC0010118

VOID
EnableDisableSVM (
  IN  BOOLEAN SVMEnable,
  IN  BOOLEAN SVMLock
);

#endif
