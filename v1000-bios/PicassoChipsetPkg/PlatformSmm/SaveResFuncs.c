/** @file
  Supports functions that saves and restores to the global register table
  information of PIC, KBC, PCI, CpuState, Edge Level, GPIO, and MTRR.

;*******************************************************************************
;* Copyright (c) 2013 - 2018, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include <SmmPlatform.h>

//
// Global variables.
//
BOOLEAN   mHaveAllocateMsrSaveTableBuffer = FALSE;
UINTN     mCoreIndex = 0;
UINTN     mMsrTableSaveStart = 0;

//
// DeviceRegTable
//
UINT32
MSR_Table [] = {
  0xC0010113,
  0xC0010015,
  0xC001102E
  };
UINT64 *MSR_Table_SAVE;
UINTN MSR_Table_SIZE = (sizeof (MSR_Table) / sizeof (UINT32));


/**
  CpuSaveMsrByCore.

  @param None

  @retval void.

**/
VOID
CpuSaveMsrByCore (
  VOID
  )
{
  UINTN   Index;
  UINT64  MsrValue;

  MsrValue = AsmReadMsr64 (0xC0010010); //SYSCFGR
  MsrValue |= 0x00080000; //MtrrFixDramModEn
  AsmWriteMsr64 (0xC0010010, MsrValue);

  for (Index = 0; Index < MSR_Table_SIZE; Index++) {
    MSR_Table_SAVE[(mCoreIndex * MSR_Table_SIZE) + Index] = AsmReadMsr64 (MSR_Table[Index]);
  }

  MsrValue = AsmReadMsr64 (0xC0010010); //SYSCFGR
  MsrValue &= ~(0x00080000); //MtrrFixDramModEn
  AsmWriteMsr64 (0xC0010010, MsrValue);

  return;
}

/**
  CpuSaveRestoreMsr.

  @param None

  @retval void.

**/
VOID
CpuRestoreMsrByCore (
  VOID
  )
{
  UINTN   Index;
  UINT64  MsrValue;

  MsrValue = AsmReadMsr64 (0xC0010010); //SYSCFGR
  MsrValue |= 0x00080000; //MtrrFixDramModEn
  AsmWriteMsr64 (0xC0010010, MsrValue);

  for (Index = 0; Index < MSR_Table_SIZE; Index++) {
    AsmWriteMsr64 (MSR_Table[Index], MSR_Table_SAVE[(mCoreIndex * MSR_Table_SIZE) + Index]);
  }

  MsrValue = AsmReadMsr64 (0xC0010010); //SYSCFGR
  MsrValue &= ~(0x00080000); //MtrrFixDramModEn
  AsmWriteMsr64 (0xC0010010, MsrValue);

  MsrValue = AsmReadMsr64 (0xC001001F);
  MsrValue &= ~(0x0000400000000000);
  AsmWriteMsr64 (0xC001001F, MsrValue);

  return;
}

/**
  CpuSaveRestoreMsr.

  @param [in]   SaveRestoreFlag  True: write data to Msr registers.
                                 False: read data from Msr registers to global registers.

  @retval EFI_SUCCESS            This routine was handled successfully.

**/
EFI_STATUS
CpuSaveRestoreMsr (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{
  if (!SaveRestoreFlag) {
    if (!mHaveAllocateMsrSaveTableBuffer) {
      UINTN   BufferSize;

      //[-start-240426-IB18160212-modify-]//
      BufferSize = gSmst->NumberOfCpus * MSR_Table_SIZE * sizeof (UINT64);
      gSmst->SmmAllocatePool (EfiRuntimeServicesData, BufferSize, (VOID**)&MSR_Table_SAVE);
      mMsrTableSaveStart = (UINTN)MSR_Table_SAVE;
      mHaveAllocateMsrSaveTableBuffer = TRUE;
	  //[-end-240426-IB18160212-modify-]//
    }
    for (mCoreIndex = 0; mCoreIndex < gSmst->NumberOfCpus; mCoreIndex++) {
      if (mCoreIndex == gSmst->CurrentlyExecutingCpu) {
        CpuSaveMsrByCore ();
      } else {
        gSmst->SmmStartupThisAp ((EFI_AP_PROCEDURE)CpuSaveMsrByCore, mCoreIndex, NULL);
      }
    }
  } else {
    for (mCoreIndex = 0; mCoreIndex < gSmst->NumberOfCpus; mCoreIndex++) {
      if (mCoreIndex == gSmst->CurrentlyExecutingCpu) {
        CpuRestoreMsrByCore ();
      } else {
        gSmst->SmmStartupThisAp ((EFI_AP_PROCEDURE)CpuRestoreMsrByCore, mCoreIndex, NULL);
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  SaveRestoreCpu.

  @param [in]   SaveRestoreFlag  True: write data to Cpu registers.
                                 False: read data from Cpu registers to global registers.

  @retval EFI_SUCCESS            This routine was handled successfully.

**/
EFI_STATUS
SaveRestoreCpu (
  IN  BOOLEAN                       SaveRestoreFlag
  )
{

  CpuSaveRestoreMsr (SaveRestoreFlag);

  return  EFI_SUCCESS;
}
