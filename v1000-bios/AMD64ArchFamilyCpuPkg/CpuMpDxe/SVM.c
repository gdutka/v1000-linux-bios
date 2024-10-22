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

#include "SVM.h"

VOID
EnableSVM (
  IN  BOOLEAN   SVMLock
)
{
  UINT64  MsrData;

  if (SVMLock){
    AsmWriteMsr64 (AMD_MSR_SVM_LOCK_KEY, 0);
  }

  MsrData = AsmReadMsr64 (AMD_MSR_VM_CR);
  MsrData &= ~(0x10);
  if (SVMLock){
    MsrData |= 0x8;
  }
  AsmWriteMsr64 (AMD_MSR_VM_CR, MsrData);
}

VOID
DisableSVM (
  IN  BOOLEAN   SVMLock
)
{
  UINT64  MsrData;

  if (SVMLock){
    AsmWriteMsr64 (AMD_MSR_SVM_LOCK_KEY, 0);
  }
  MsrData = AsmReadMsr64 (AMD_MSR_VM_CR);
  MsrData |= 0x10;
  if (SVMLock){
    MsrData |= 0x8;
  }
  AsmWriteMsr64 (AMD_MSR_VM_CR, MsrData);
}

VOID
EnableDisableSVM (
  IN  BOOLEAN   SVMEnable,
  IN  BOOLEAN   SVMLock
)
{  
  if (SVMEnable) {
      EnableSVM (SVMLock);
  } else {
      DisableSVM (SVMLock);
  }
}