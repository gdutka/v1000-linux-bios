/** @file
  Functions for Dmitool

;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "PnpSmm.h"
#include <Protocol/LegacyBios.h>
#include <Protocol/SmmCpu.h>
#include <Library/FlashRegionLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SafeIntLib.h>

#define  SMBIOS_TABLE_EXTRA_MEMORY  SIZE_4KB 

SMBIOS_TABLE_ENTRY_POINT      *mSmbiosTableEntryPoint = NULL;
SMBIOS_TABLE_3_0_ENTRY_POINT  *mSmbiosTableEntryPoint64Bit = NULL;
SMBIOS_STRUCTURE              *mSmbiosTable = NULL;
SMBIOS_STRUCTURE              *mSmbiosTable64Bit = NULL;
INT32                         TotalIncreasedStringLength = 0;
EFI_PHYSICAL_ADDRESS          mSMBIOSTableEntryAddress = 0;
EFI_PHYSICAL_ADDRESS          mSMBIOSTableEntryAddress64Bit = 0;
EFI_SMM_CPU_PROTOCOL          *mSmmCpu = NULL;
UINTN                         mCpuIndex =0;
CHAR8                         mSmbio30Signature[] = SMBIOS_30_SIGNATURE;
BOOLEAN                       mSmmSmbiosPnpInitialized = FALSE;

PNP_REDIRECTED_ENTRY_POINT mPnpDmiFunctions[] = {
  Pnp0x50,
  Pnp0x51,
  Pnp0x52,
  UnsupportedPnpFunction, //Pnp0x53,
  Pnp0x54,
  Pnp0x55,
  Pnp0x56,
  Pnp0x57
};

FRAME_SIZE_INFO mFrameSizeInfos[] = {
  {0x50, sizeof(PNP_FUNCTION_0x50_FRAME)},
  {0x51, sizeof(PNP_FUNCTION_0x51_FRAME)},
  {0x52, sizeof(PNP_FUNCTION_0x52_FRAME)},
  //Pnp0x53 is unsupported
  {0x54, sizeof(PNP_FUNCTION_0x54_FRAME)},
  {0x55, sizeof(PNP_FUNCTION_0x55_FRAME)},
  {0x56, sizeof(PNP_FUNCTION_0x56_FRAME)},
  {0x57, sizeof(PNP_FUNCTION_0x57_FRAME)},
};

STATIC
EFI_STATUS
ClearBiosEventLog (
  IN UINTN                              GPNVBase,
  IN UINTN                              GPNVLength
  );

/**
  This fucntion uses to read saved CPU double word register by CPU index

  @param[In]   RegisterNum   Register number which want to get
  @param[In]   CpuIndex      CPU index number to get register.
  @param[Out]  RegisterData  pointer to output register data

  @retval EFI_SUCCESS   Read double word register successfully
  @return Other         Any error occured while disabling all secure boot SMI functions successful.

**/
EFI_STATUS
GetDwordRegisterByCpuIndex (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINTN                             CpuIndex,
  OUT UINT32                            *RegisterData
  )
{
  UINTN                       Width;

  Width = sizeof (UINT32);
  *RegisterData = 0;

  if (RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_CS ||
      RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_DS ||
      RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_ES ||
      RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_SS ||
      RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_FS ||
      RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_GS) {
    Width = sizeof (UINT16);
  }
  return mSmmCpu->ReadSaveState (
                    mSmmCpu,
                    Width,
                    RegisterNum,
                    CpuIndex,
                    RegisterData
                    );
}

/**
  This fucntion uses to set saved CPU double word register by CPU index

  @param[In]  RegisterNum   Register number which want to get
  @param[In]  CpuIndex      CPU index number to get register.
  @param[In]  RegisterData  pointer to output register data

  @retval EFI_SUCCESS   Set double word register successfully
  @return Other         Any error occured while disabling all secure boot SMI functions successful.

**/
EFI_STATUS
SetDwordRegisterByCpuIndex (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINTN                             CpuIndex,
  IN  UINT32                            *RegisterData
  )
{
  UINTN                       Width;

  Width = sizeof (UINT32);

  if (RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_CS ||
      RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_DS ||
      RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_ES ||
      RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_SS ||
      RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_FS ||
      RegisterNum == EFI_SMM_SAVE_STATE_REGISTER_GS) {
    Width = sizeof (UINT16);
  }

  return mSmmCpu->WriteSaveState (
                    mSmmCpu,
                    Width,
                    RegisterNum,
                    CpuIndex,
                    RegisterData
                    );
}

/**
  Ensures that mSmbiosTableEntryPoint and mSmbiosTable are filled in.

  @param  none

**/
VOID
LocateSmbiosTable (
  )
{
  UINT32             *Address;
  BOOLEAN            IsSmbios64BitTable;
  BOOLEAN            IsSmbios32BitTable;
  BOOLEAN            LegacySmbios64BitTableFound;
  BOOLEAN            LegacySmbios32BitTableFound;

  IsSmbios32BitTable = FALSE;
  IsSmbios64BitTable = FALSE;
  LegacySmbios32BitTableFound = FALSE;
  LegacySmbios64BitTableFound = FALSE;

  if (((PcdGet16 (PcdSmbiosVersion) >> 8) < 0x3) ||
      (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT0) == BIT0))) {
    IsSmbios32BitTable = TRUE;
  }

  if (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT1) == BIT1)) {
    IsSmbios64BitTable = TRUE;
  }

  //
  // Locate SMBIOS structure table entry point when Legacy boot.
  //
  if (IsSmbios32BitTable && ((mSmbiosTableEntryPoint == NULL) || ((UINT32)(UINTN)mSmbiosTableEntryPoint->AnchorString != (UINT32)SMBIOS_SIGNATURE))) {
    // CODE ANALYSIS (VS19.0.2) - Reports C28182. Added typecast to Address = Address + 4.
    for (Address = (UINT32 *)(UINTN) 0xF0000; (UINTN)Address < 0x100000; Address = (UINT32*) ((UINTN) Address + 4)) {
      mSmbiosTableEntryPoint = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN) Address;
      if (*(UINT32 *)mSmbiosTableEntryPoint == (UINT32)SMBIOS_SIGNATURE) {
        mSmbiosTable = (SMBIOS_STRUCTURE *)(UINTN)mSmbiosTableEntryPoint->TableAddress;
        LegacySmbios32BitTableFound = TRUE;
        break;
      }
    }
  }

  if (IsSmbios64BitTable && ((mSmbiosTableEntryPoint64Bit == NULL) || (AsciiStrnCmp ((CHAR8*)mSmbiosTableEntryPoint64Bit->AnchorString, mSmbio30Signature, sizeof(mSmbio30Signature)) != 0 ))) {
    // CODE ANALYSIS (VS19.0.2) - Reports C28182. Added typecast to Address = Address + 4.
    for (Address = (UINT32 *)(UINTN) 0xF0000; (UINTN)Address < 0x100000; Address = (UINT32*) ((UINTN) Address + 4)) {
      mSmbiosTableEntryPoint64Bit = (SMBIOS_TABLE_3_0_ENTRY_POINT *)(UINTN) Address;
      if (AsciiStrnCmp ((CHAR8*)mSmbiosTableEntryPoint64Bit->AnchorString, mSmbio30Signature, AsciiStrLen (mSmbio30Signature)) == 0 ) {
        mSmbiosTable64Bit = (SMBIOS_STRUCTURE *)(UINTN)mSmbiosTableEntryPoint64Bit->TableAddress;
        LegacySmbios64BitTableFound = TRUE;
        break;
      }
    }
  }

  if (IsSmbios32BitTable && IsSmbios64BitTable){
    if (LegacySmbios32BitTableFound == TRUE && LegacySmbios64BitTableFound == TRUE){
      return;
    }
  } else if (IsSmbios32BitTable){
    if (LegacySmbios32BitTableFound == TRUE){
      return;
    }
  } else if (IsSmbios64BitTable){
    if (LegacySmbios64BitTableFound == TRUE){
      return;
    }
  }


  //
  // If the entry point is not found in CSM,
  // locate SMBIOS structure table entry point at Runtime.
  //
  if (IsSmbios32BitTable) {
    mSmbiosTableEntryPoint = (SMBIOS_TABLE_ENTRY_POINT*)(UINTN)mSMBIOSTableEntryAddress;
    mSmbiosTable = (SMBIOS_STRUCTURE*)(UINTN)mSmbiosTableEntryPoint->TableAddress;
  }

  if (IsSmbios64BitTable) {
    mSmbiosTableEntryPoint64Bit = (SMBIOS_TABLE_3_0_ENTRY_POINT*)(UINTN)mSMBIOSTableEntryAddress64Bit;
    mSmbiosTable64Bit = (SMBIOS_STRUCTURE*)(UINTN)((SMBIOS_TABLE_3_0_ENTRY_POINT*)mSmbiosTableEntryPoint64Bit)->TableAddress;
  }
}

/**
  Locates the smbios structure with a handle of *Structure.

  @param[In]   Structure      Handle of structure to attempt to locate.
                              - If 0 on entry, then the first avaiable structure is found.
                              - On exit, *Structure is updated with the next structure handle.
                              A return handle of 0xffff indicates there are no more structures.
                              *Structure is always updated with useful information.
  @param[Out]  Location       Return the structure location if it was found.
  @param[Out]  Size           The size of structure location if it was found.

  @retval EFI_SUCCESS    The structure was found. **Location and *Size are valid.
  @retval EFI_NOT_FOUND  The structure was not found.

**/
EFI_STATUS
LocateSmbiosStructure (
  IN BOOLEAN                            IsSmbios32BitTable,
  IN OUT UINT16                         *Structure,
  OUT SMBIOS_STRUCTURE                  **Location,
  OUT UINTN                             *Size
  )
{
  SMBIOS_STRUCTURE                      *Iter;
  SMBIOS_STRUCTURE                      *Next;
  SMBIOS_STRUCTURE                      *End;

  if (IsSmbios32BitTable &&(*Structure == 0) && ((*Location) != NULL) && (*(UINT32 *)(*Location) == (UINT32)SMBIOS_SIGNATURE)) {
    *Structure = mSmbiosTable->Handle;
  }

  if (!IsSmbios32BitTable &&(*Structure == 0) && ((*Location) != NULL) && ( AsciiStrnCmp ((CHAR8*)*Location, mSmbio30Signature, AsciiStrLen(mSmbio30Signature)) == 0)) {
    *Structure = mSmbiosTable64Bit->Handle;
  }

  if (IsSmbios32BitTable){
    Iter = mSmbiosTable;
  } else {
    Iter = mSmbiosTable64Bit;
  }

  if (Iter == NULL){
    return EFI_NOT_FOUND;
  }
  //
  // Adjust the end pointer if SMBIOS data is modified, such as strings.
  //
  if (IsSmbios32BitTable) {
    End = (SMBIOS_STRUCTURE *)(UINTN)(((SMBIOS_TABLE_ENTRY_POINT*)mSmbiosTableEntryPoint)->TableAddress + ((SMBIOS_TABLE_ENTRY_POINT*)mSmbiosTableEntryPoint)->TableLength);
  } else {
    End = (SMBIOS_STRUCTURE *)(UINTN)(((SMBIOS_TABLE_3_0_ENTRY_POINT*)mSmbiosTableEntryPoint64Bit)->TableAddress + ((SMBIOS_TABLE_3_0_ENTRY_POINT*)mSmbiosTableEntryPoint64Bit)->TableMaximumSize);
  }

  if (TotalIncreasedStringLength >= 0x00) {
    End = (SMBIOS_STRUCTURE *)((UINTN)End + (UINTN)TotalIncreasedStringLength);
  } else {
    End = (SMBIOS_STRUCTURE *)((UINTN)End - (UINTN)(~(TotalIncreasedStringLength - 1)));
  }

  while (TRUE) {
    for (Next = (SMBIOS_STRUCTURE *)((UINT8 *)Iter + Iter->Length); *(UINT16 *)Next != 0; Next = (SMBIOS_STRUCTURE *)((UINTN)Next + 1));
    Next = (SMBIOS_STRUCTURE *)((UINTN)Next + 2);
    if (Iter >= End) {
      //
      // End-of-list indicator
      //
      *Structure = 0xffff;
      return EFI_NOT_FOUND;
    } else if (Iter->Handle == *Structure) {
      *Structure = (UINT16)((Next < End) ? Next->Handle : 0xffff);
      *Location = Iter;
      *Size = (UINTN)Next - (UINTN)Iter;
      return EFI_SUCCESS;
    }
    Iter = Next;
  }
}

/**
  Find the location and size of a string within a smbios structure.

  @param[In]   StringRef      The string number to search for within the structure.
  @param[In]   Structure      The Pointer to the structure to search.
  @param[Out]  Location       Return the string location if it was found.
  @param[Out]  Size           The size of string location if it was found.

  @retval EFI_SUCCESS    the string was found. **Location and *Size are valid.
  @retval EFI_NOT_FOUND  The structure was not found.

**/
EFI_STATUS
LocateStringOfStructure (
  IN UINT8                              StringRef,
  IN SMBIOS_STRUCTURE                   *Structure,
  OUT CHAR8                             **Location,
  OUT UINTN                             *Size
  )
{
  CHAR8                                 *Next;

  if (StringRef == 0) {
    return EFI_NOT_FOUND;
  }

  Next = (CHAR8 *)(((UINTN)Structure) + Structure->Length);
  do {
    *Location = Next;
    *Size = AsciiStrLen (*Location);
    Next = *Location + *Size + 1;
  } while ((--StringRef) != 0 && (*Next != 0));

  return (StringRef == 0) ? EFI_SUCCESS : EFI_NOT_FOUND;
}

/**
  Get the total number of existing strings within a smbios structure

  @param   Structure             The Pointer to the smbios structure.

  @return  The total number of existing strings within a smbios structure
**/
UINT8
GetNumberStringOfStructure (
  IN SMBIOS_STRUCTURE                   *Structure
  )
{
  CHAR8                                 *Next;
  UINT8                                 NumberOfStrings;

  Next = (CHAR8 *)(((UINTN)Structure) + Structure->Length);
  if (*Next == 0 && *(Next + 1) == 0) {
    return 0;
  }

  NumberOfStrings = 0;
  while (TRUE) {
    if (*Next == 0) {
      NumberOfStrings += 1;
      if (*(Next + 1) == 0) {
        break;
      }
    }
    Next++;
  }

  return NumberOfStrings;
}

/**
  If OEM_DMI_STORE address full to arrange.

  @param[Out]  DmiEnd    Pointer to function 0x52 data buffer input.

  @retval EFI_SUCCESS    The function return valid value.
  @retval Others         Error occurs.

**/
EFI_STATUS
DMISpaceNotEnough (
  OUT UINT16                  *DmiEnd
  )
{
  EFI_STATUS                  Status;
  UINTN                       DmiSize;
  UINTN                       NvStorageDmiBase;
  UINTN                       NvSpareAddr;
  UINTN                       NvSpareSize;
  UINTN                       UsingSpareSize;
  UINT16                      NotEnoughPtr;
  UINTN                       Size;
  DMI_RECLAIM_IN_FTW_SPARE    DmiReclaimInFtwSpare;
  UINT8                       *NotEnoughBuffer;
  UINT8                       *Buffer;
  DMI_STRING_STRUCTURE        *CurrentPtr;
  UINT8                       *Ptr;

  NotEnoughBuffer  = NULL;

  CurrentPtr       = NULL;
  NotEnoughPtr     = 4;

  NvStorageDmiBase = (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionSmbiosUpdateGuid, 1);
  if (NvStorageDmiBase == 0) {
    return EFI_NOT_FOUND;
  }

  DmiSize = (UINTN) FdmGetNAtSize (&gH2OFlashMapRegionSmbiosUpdateGuid, 1);

  NotEnoughBuffer = AllocateRuntimePool (DmiSize);
  if (NotEnoughBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SetMem (NotEnoughBuffer, DmiSize, 0xFF);
  *(UINT32 *)NotEnoughBuffer = DMI_UPDATE_STRING_SIGNATURE;

  //
  // Search smbios Valid, If it is found, set OEM_DMI_STORE address
  //
  Buffer = (UINT8 *)(NvStorageDmiBase + sizeof (DMI_UPDATE_STRING_SIGNATURE));
  while ((UINTN)Buffer < (NvStorageDmiBase + DmiSize)) {
    CurrentPtr = (DMI_STRING_STRUCTURE *)Buffer;
    if (CurrentPtr->Type == 0xFF) {
      break;
    }
    if (CurrentPtr->Valid == 0xFF) {
      if ((UINTN) (NotEnoughPtr + CurrentPtr->Length) > DmiSize) {
        FreePool(NotEnoughBuffer);
        return EFI_BAD_BUFFER_SIZE;
      }
      CopyMem ((NotEnoughBuffer+NotEnoughPtr), Buffer, CurrentPtr->Length);
      NotEnoughPtr = NotEnoughPtr + CurrentPtr->Length;
    }
    Buffer = Buffer + CurrentPtr->Length;
  }

  //
  // Flash whole buffer to NV_FTW_SPARE
  //
  NvSpareAddr = (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionFtwBackupGuid, 1);
  if (NvSpareAddr == 0) {
    FreePool(NotEnoughBuffer);
    return EFI_NOT_FOUND;
  }
  NvSpareSize = (UINTN) FdmGetNAtSize(&gH2OFlashMapRegionFtwBackupGuid, 1);
  UsingSpareSize = DmiSize * DMI_USING_FTW_BACKUP_MULTIPLE_OF_DMI_REGION;
  if (UsingSpareSize > NvSpareSize) {
    FreePool(NotEnoughBuffer);
    return EFI_NOT_FOUND;
  }
  Status = mSmmFwBlockService->EraseBlocks (
                                 mSmmFwBlockService,
                                 NvSpareAddr,
                                 &UsingSpareSize
                                 );
  if (!EFI_ERROR(Status)) {
    Status = mSmmFwBlockService->Write (
                                   mSmmFwBlockService,
                                   NvSpareAddr + sizeof (DMI_RECLAIM_IN_FTW_SPARE),
                                   &DmiSize,
                                   NotEnoughBuffer
                                   );
  }
  if (EFI_ERROR(Status)) {
    FreePool(NotEnoughBuffer);
    return Status;
  }

  //
  // Write DmiReclaimGuid and DataLength wtihout Valid Byte in NV_FTW_SPARE
  //
  CopyGuid(&DmiReclaimInFtwSpare.DmiReclaimGuid, &gH2OFlashMapRegionSmbiosUpdateGuid);
  DmiReclaimInFtwSpare.DataLength = (UINT32) (DmiSize + sizeof (DMI_RECLAIM_IN_FTW_SPARE));
  Size = (sizeof (DMI_RECLAIM_IN_FTW_SPARE) - sizeof (DmiReclaimInFtwSpare.Valid));
  Ptr = (UINT8 *) &DmiReclaimInFtwSpare;
  Status = mSmmFwBlockService->Write (
                                 mSmmFwBlockService,
                                 NvSpareAddr,
                                 &Size,
                                 Ptr
                                 );
  if (EFI_ERROR(Status)) {
    FreePool(NotEnoughBuffer);
    return Status;
  }

  //
  // Write Valid Byte in NV_FTW_SPARE
  //
  DmiReclaimInFtwSpare.Valid = DMI_USING_FTW_BACKUP_VALID_FLAG;
  Size = sizeof (DmiReclaimInFtwSpare.Valid);
  Status = mSmmFwBlockService->Write (
                                 mSmmFwBlockService,
                                 NvSpareAddr + sizeof (DMI_RECLAIM_IN_FTW_SPARE) - sizeof (DmiReclaimInFtwSpare.Valid),
                                 &Size,
                                 &DmiReclaimInFtwSpare.Valid
                                 );
  if (EFI_ERROR(Status)) {
    FreePool(NotEnoughBuffer);
    return Status;
  }

  //
  // Flash whole buffer to OEM_DMI_STORE
  //
  Status = mSmmFwBlockService->EraseBlocks (
                                 mSmmFwBlockService,
                                 NvStorageDmiBase,
                                 &DmiSize
                                 );
  if (!EFI_ERROR (Status)) {
    Status = mSmmFwBlockService->Write (
                                   mSmmFwBlockService,
                                   NvStorageDmiBase,
                                   &DmiSize,
                                   NotEnoughBuffer
                                   );
  }
  FreePool(NotEnoughBuffer);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Erase backup in NV_FTW_SPARE
  //
  Status = mSmmFwBlockService->EraseBlocks (
                                 mSmmFwBlockService,
                                 NvSpareAddr,
                                 &UsingSpareSize
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (DmiEnd!= NULL) {
    *DmiEnd = NotEnoughPtr;
  }

  return EFI_SUCCESS;
}

/**
  Get the remaining size and expected reclaim size from OEM_DMI_STORE

  @param[out]   RemainingSize   The pointer to the remaining Size
  @param[out]   ReclaimSize     The pointer to the reclaim size

  @retval       EFI_SUCCESS     The function return valid value.
  @retval       Others          Failed to get the valid value.

**/
EFI_STATUS
GetDmiReclaimInfo (
   OUT UINTN                  *RemainSize,
   OUT UINTN                  *ReclaimSize
   )
{
  UINTN                       Remaining;
  UINTN                       Reclaim;
  UINTN                       DmiSize;
  UINTN                       DmiAddr;
  UINT16                      DmiEnd;
  UINT8                       *Buffer;
  DMI_STRING_STRUCTURE        *CurrentPtr;

  if (RemainSize == NULL || ReclaimSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DmiAddr = (UINTN) FdmGetNAtAddr(&gH2OFlashMapRegionSmbiosUpdateGuid, 1);
  if (DmiAddr == 0) {
    return EFI_NOT_FOUND;
  }

  DmiSize = (UINTN) FdmGetNAtSize(&gH2OFlashMapRegionSmbiosUpdateGuid, 1);
  if (DmiSize == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // First 4 bytes are "$DMI"
  //
  Buffer = (UINT8 *)(UINTN) DmiAddr;
  if (!(*(UINT32 *)Buffer == DMI_UPDATE_STRING_SIGNATURE)){
    return EFI_NOT_FOUND;
  }
  Buffer += sizeof (DMI_UPDATE_STRING_SIGNATURE);
  DmiEnd = sizeof (DMI_UPDATE_STRING_SIGNATURE);

  Reclaim = 0;
  Remaining = 0;
  while (DmiEnd < (DmiSize - DMI_TOTAL_RECLAIM_RESERVERED_SIZE)) {
    CurrentPtr = (DMI_STRING_STRUCTURE *) Buffer;
    if (CurrentPtr->Type == 0xFF) {
      //
      // Remaining size
      //
      Remaining += (UINTN) (DmiSize - DMI_TOTAL_RECLAIM_RESERVERED_SIZE - DmiEnd);
      break;
    }

    if (CurrentPtr->Valid == 0) {
      //
      // Reclaim size
      //
      Reclaim += CurrentPtr->Length;
    }

    //
    // Try to find next.
    //
    DmiEnd = DmiEnd + CurrentPtr->Length;
    Buffer = Buffer + CurrentPtr->Length;
  }

  *RemainSize = Remaining;
  *ReclaimSize = Reclaim;
  return EFI_SUCCESS;
}

/**
  Get the start and free space address of OEM_DMI_STORE region.

  @param[Out]  DmiRegionAddr        Start address of OEM_DMI_STORE region.
  @param[Out]  DmiRegionSize        Size of OEM_DMI_STORE region.
  @param[Out]  FreeSpaceOffset      Offset of free space in OEM_DMI_STORE region.

  @retval EFI_SUCCESS               Addresses and Size are get successfully.
  @retval EFI_INVALID_PARAMETER     Inputs are NULL.
  @retval EFI_NOT_FOUND             Error occurs.

*/
EFI_STATUS
GetDmiFreeSpace (
  OUT  UINT64                           *DmiRegionAddr,
  OUT  UINT64                           *DmiRegionSize,
  OUT  UINT16                           *FreeSpaceOffset
  )
{
  UINT64                      DmiAddr;
  UINT64                      DmiSize;
  UINT16                      DmiEnd;
  UINT8                       *Buffer;
  UINT8                       *Ptr;
  DMI_STRING_STRUCTURE        *CurrentPtr;

  if (DmiRegionAddr == NULL || DmiRegionSize == NULL || FreeSpaceOffset == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DmiAddr = FdmGetNAtAddr(&gH2OFlashMapRegionSmbiosUpdateGuid, 1);
  if (DmiAddr == 0) {
    return EFI_NOT_FOUND;
  }

  DmiSize = FdmGetNAtSize(&gH2OFlashMapRegionSmbiosUpdateGuid, 1);
  if (DmiSize == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // First 4 bytes are "$DMI"
  //
  Ptr = (UINT8 *)(UINTN) DmiAddr;
  Buffer = Ptr + sizeof (DMI_UPDATE_STRING_SIGNATURE);

  //
  // Search OEM_DMI_STORE free space
  //
  DmiEnd = 4;
  while (DmiEnd < (DmiSize - DMI_TOTAL_RECLAIM_RESERVERED_SIZE)) {
    CurrentPtr = (DMI_STRING_STRUCTURE *)Buffer;

    if (CurrentPtr->Type == 0xFF) {
      //
      // The space is free, we could use it, so break.
      //
      break;
    }
    //
    // Try to find next.
    //
    DmiEnd = DmiEnd + CurrentPtr->Length;
    Buffer = Buffer + CurrentPtr->Length;
  }

  *DmiRegionAddr    = DmiAddr;
  *DmiRegionSize    = DmiSize;
  *FreeSpaceOffset  = DmiEnd;

  return EFI_SUCCESS;
}

/**
  If after EndOfDxe, write DMI_RECLAIM_STRING_SIGNATURE into OEM_DMI_STORE to perform reclaim in next post.
  Otherwise, perform reclaim right now.

  @param[In]   DmiAddr            Address of OEM_DMI_STORE region.
  @param[In]   DmiSize            Size of OEM_DMI_STORE region.
  @param[Out]  DmiEnd             Offset of free space in OEM_DMI_STORE region.

  @retval   EFI_SUCCESS      Performed reclaim DMI space successfully.
  @retval   EFI_UNSUPPORTED  No more space to reclaim or it's after EndOfDxe.
  @retval   Others           Some errors may occurs during the reclaim process.

*/
EFI_STATUS
ReclaimDmiSpace (
  IN   UINT64       DmiAddr,
  IN   UINT64       DmiSize,
  OUT  UINT16       *DmiEnd
  )
{
  UINTN                       RemainingSize;
  UINTN                       ReclaimSize;
  UINT32                      ReclaimFlag;
  UINT8                       *Ptr;
  UINTN                       Size;
  EFI_STATUS                  Status;

  RemainingSize = 0;
  ReclaimSize = 0;
  Status = GetDmiReclaimInfo (&RemainingSize, &ReclaimSize);
  if (EFI_ERROR(Status) || ReclaimSize == 0) {
    return EFI_UNSUPPORTED;
  }
  if (mSmmSmbiosEndOfDxe) {
    //
    // Writting flag for next post to reclaim
    //
    Ptr = (UINT8 *)(UINTN) DmiAddr;
    Ptr += (DmiSize - sizeof (DMI_RECLAIM_STRING_SIGNATURE));
    if ((*(UINT32 *) Ptr == 0xFFFFFFFF)) {
      ReclaimFlag = DMI_RECLAIM_STRING_SIGNATURE;
      Ptr = (UINT8 *) &ReclaimFlag;
      Size = sizeof (DMI_RECLAIM_STRING_SIGNATURE);
      mSmmFwBlockService->Write (
                            mSmmFwBlockService,
                            (UINTN)(DmiAddr + DmiSize - sizeof(DMI_RECLAIM_STRING_SIGNATURE)),
                            &Size,
                            Ptr
                            );
    }
    return EFI_UNSUPPORTED;
  }
  Status = DMISpaceNotEnough (DmiEnd);
  if (EFI_ERROR(Status)){
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Set SMBIOS Structure to OEM_DMI_STORE address.

  @param[In]  Data           Pointer to function 0x52 data buffer input.
  @param[In]  Structure      The Pointer to the structure to search.

  @retval EFI_SUCCESS    The function sets valid value for Data.
  @retval EFI_NOT_FOUND  Error occurs.

**/
EFI_STATUS
SetDMI (
  IN FUNC_0x52_DATA_BUFFER              *Data,
  IN SMBIOS_STRUCTURE                   *Structure
  )
{
  EFI_STATUS                  Status;
  UINTN                       Index;
  UINTN                       BlockSize;
  UINTN                       BSize;
  UINT8                       Invalid;
  UINT16                      DmiEnd;
  UINT64                      DmiAddr;
  UINT64                      DmiSize;
  UINT64                      NewDataEnd;
  UINT8                       *Buffer;
  UINT8                       *Ptr;
  UINT8                       *SavePtr;
  DMI_STRING_STRUCTURE        *CurrentPtr;

  BSize      = 0x1;
  Invalid    = 0;
  SavePtr    = NULL;
  CurrentPtr = NULL;

  Status = GetDmiFreeSpace (&DmiAddr, &DmiSize, &DmiEnd);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if ((DmiEnd + Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof(UINT8)) >
      (DmiSize - DMI_TOTAL_RECLAIM_RESERVERED_SIZE))) {
    Status = ReclaimDmiSpace (DmiAddr, DmiSize, &DmiEnd);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }
  //
  // Set update data to OEM_DMI_STORE address.
  //
  SavePtr = AllocateRuntimePool (Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof (UINT8)));
  if (SavePtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CurrentPtr         = (DMI_STRING_STRUCTURE *)SavePtr;
  CurrentPtr->Type   = Structure->Type;
  CurrentPtr->Offset = Data->FieldOffset;
  CurrentPtr->Valid  = 0xFF;

  switch (Data->Command) {

  case DMI_STRING_CHANGE:
    CurrentPtr->Length = Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof (UINT8) - 1);
    for (Index = 0; Index + 1 < Data->DataLength; Index++) {
      CurrentPtr->String[Index] = Data->StructureData[Index];
    }
    break;

  default:
    CurrentPtr->Length = Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof (UINT8));
    for (Index = 0; Index < Data->DataLength; Index++) {
      CurrentPtr->String[Index] = Data->StructureData[Index];
    }
    break;
  }

  BlockSize = CurrentPtr->Length;
  if (DmiEnd + BlockSize > (DmiSize - DMI_TOTAL_RECLAIM_RESERVERED_SIZE)) {
    FreePool (SavePtr);
    return EFI_UNSUPPORTED;
  }
  NewDataEnd = (UINT64) (DmiAddr + DmiEnd);
  Status = mSmmFwBlockService->Write (
                                 mSmmFwBlockService,
                                 (UINTN) NewDataEnd,
                                 &BlockSize,
                                 SavePtr
                                 );
  FreePool (SavePtr);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Set invalid byte in OEM_DMI_STORE space if we find the same data
  //
  DmiEnd = 4;
  Ptr = (UINT8 *)(UINTN) DmiAddr;
  Buffer = Ptr + sizeof (DMI_UPDATE_STRING_SIGNATURE);
  while (DmiEnd < (NewDataEnd - DmiAddr)) {
    CurrentPtr = (DMI_STRING_STRUCTURE *)Buffer;
    if (CurrentPtr->Type == 0xFF) {
      //
      // The space is free, we could use it, so break.
      //
      break;
    }

    if ((CurrentPtr->Type == Structure->Type) &&
        (CurrentPtr->Offset == Data->FieldOffset) &&
        (CurrentPtr->Valid == 0xFF)) {
      //
      // Set invalid byte in it.
      //
      Status = mSmmFwBlockService->Write (
                                     mSmmFwBlockService,
                                     (UINTN)(DmiAddr + DmiEnd + 2),
                                     &BSize,
                                     &Invalid
                                     );
    }
    //
    // Try to find next.
    //
    DmiEnd = DmiEnd + CurrentPtr->Length;
    Buffer = Buffer + CurrentPtr->Length;
  }

  return Status;
}

BOOLEAN
VaildGPNVCheck (
  IN OUT UINT16                     *Index,
  IN UINT16                         Handle
  )
{
  UINT16                            TempIndex;

  if (mIsOemGPNVMap == FALSE ||
      OemGPNVHandleCount >= FixedPcdGet16 (PcdDefaultGpnvMapBufferSize) ||
      *Index >= FixedPcdGet16 (PcdDefaultGpnvMapBufferSize) ||
      *Index >= OemGPNVHandleCount) {
    return FALSE;
  }

  for (TempIndex = *Index; TempIndex < OemGPNVHandleCount; TempIndex++) {
    if (GPNVMapBuffer.PlatFormGPNVMap[TempIndex].Handle == Handle) {
      *Index = TempIndex;
      return TRUE;
    }
  }

  return FALSE;
}

EFI_STATUS
WriteGPNV (
  IN UINT16                             Handle,
  IN UINT8                              *GPNVBuffer
  )
{
  EFI_STATUS                            Status;
  UINTN                                 BlockSize;
  UINT8                                 *WritingBuffer;
  UINTN                                 BlockBaseAddress;
  UINTN                                 WritingBufferOffset;
  UINT8                                 EraseCount;
  UINT8                                 WriteCount;

  BlockSize = 0x10000;
  WritingBuffer = AllocateRuntimePool (BlockSize);
  if (WritingBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  BlockBaseAddress = (GPNVMapBuffer.PlatFormGPNVMap[Handle].BaseAddress) & (~0xFFFF);
  //
  // Copy whole block data to buffer
  //
  CopyMem (WritingBuffer, (VOID *)(UINTN)BlockBaseAddress, BlockSize);

  //
  // Copy modified GPNV data to buffer
  //
  WritingBufferOffset = GPNVMapBuffer.PlatFormGPNVMap[Handle].BaseAddress - BlockBaseAddress;
  if (GPNVMapBuffer.PlatFormGPNVMap[Handle].GPNVSize > (BlockSize - WritingBufferOffset)) {
    FreePool (WritingBuffer);
    return EFI_INVALID_PARAMETER;
  }
  CopyMem ((VOID *)(WritingBuffer + WritingBufferOffset), (VOID *)GPNVBuffer, GPNVMapBuffer.PlatFormGPNVMap[Handle].GPNVSize);

  //
  // Flash whole buffer to rom
  //
  EraseCount = 0;
  WriteCount = 0;
  do {
    Status = mSmmFwBlockService->EraseBlocks (
                                   mSmmFwBlockService,
                                   BlockBaseAddress,
                                   (UINTN *)&BlockSize
                                   );

    if (!EFI_ERROR (Status)) {
      Status = mSmmFwBlockService->Write (
                                     mSmmFwBlockService,
                                     BlockBaseAddress,
                                     (UINTN *)&BlockSize,
                                     WritingBuffer
                                     );
      if (!EFI_ERROR (Status)) {
        FreePool (WritingBuffer);
        return Status;
      } else {
        WriteCount++;
      }
    } else {
      EraseCount++;
    }
  } while ((EraseCount < 100) && (WriteCount < 100));

  FreePool (WritingBuffer);
  return Status;
}

UINT32
LogicalToPhysicalAddress (
  IN UINT32  LinearAddress,
  IN UINT32  CR3Value,
  IN UINT32  CR4Value,
  IN UINTN   SegementSelector
  )
{
  UINT32  *TmpPtr;
  UINT32  Buffer32;
  UINT32  Edi;

  if ((CR3Value & 0x00000001) == 0) {
    return ((UINT32)SegementSelector << 4 ) + LinearAddress;
  }

  //
  // 32bit Mode SMI: transfer to physical address
  // IA-32 Intel Architecture Software Developer's Manual
  // Volume 3 - Chapter 3.7
  // there is 2 necessay condition:
  // 1.Base address in Segement Selector(GDT) must be 0
  //   if not 0, the Linear address need add the base address first
  //   Check Chapter 3.4 "Logical and Linear Addresses"
  if (SegementSelector != 0x10) {
    //
    // Base Address in SegementSelector 0x10 is set to 0 in SEC Phase.
    // Other will need to be read GDT...not impelement yet.Just return.
    //
    GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RDI, mCpuIndex, &Edi);
    LinearAddress += Edi;
  }

  //
  // 2.Only support 4MByte Pages now for XP-SP2
  //   4KByte pages isn't verified.
  //   if need support 4KByte Pages for Win2K...etc,
  //   check Chapter 3.7.1
  //

  // Use CR4 Bit5 to check 2MBytes Page for XD enable
  if (CR4Value & 0x20) {
    Buffer32 = CR3Value & 0xFFFFFFE0;

    //
    // Page Directory Pointer Table Entry
    //
    TmpPtr = (UINT32 *)(UINTN)(Buffer32 + ((LinearAddress & 0xC0000000) >> 27));
    Buffer32 = *TmpPtr;

    //
    // 2MByte Page - Page Directory Entry
    //
    TmpPtr = (UINT32 *)(UINTN)((Buffer32 & 0xFFFFF000) + ((LinearAddress & 0x3FE00000) >> 18));
    Buffer32 = *TmpPtr;

    if ((Buffer32 & 0x80) == 0) {
      //
      // 4KByte Pages - Page Table
      //
      TmpPtr = (UINT32 *)(UINTN)((Buffer32 & 0xFFFFF000) + ((LinearAddress & 0x001FF000) >> 9));
      Buffer32 = *TmpPtr;
      //
      // 4-KByte Page - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFFFF000) + (LinearAddress & 0x00000FFF);
    } else {
      //
      // 2MByte Page - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFE00000) + (LinearAddress & 0x001FFFFF);
    }
  } else {
    //
    // Get Page-Directory from CR3
    //
    Buffer32 = CR3Value & 0xFFFFF000;

    //
    // Page Directory Entry
    //
    TmpPtr = (UINT32 *)(UINTN)(Buffer32 + ((LinearAddress & 0xFFC00000) >> 20));
    Buffer32 = *TmpPtr;

    //
    // Check 4KByte/4MByte Pages
    //
    if ((Buffer32 & 0x80) == 0) {
      //
      // 4KByte Pages - Page Table
      //
      TmpPtr = (UINT32 *)(UINTN)((Buffer32 & 0xFFFFF000) + ((LinearAddress & 0x003FF000) >> 10));
      Buffer32 = *TmpPtr;
      //
      // 4-KByte Page - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFFFF000) + (LinearAddress & 0x00000FFF);
    } else {
      //
      // 4MByte Pages - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFC00000) + (LinearAddress & 0x003FFFFF);
    }
  }

  return Buffer32;
}

/**
  Does a quick conversion from a 16-bit C far pointer to a linear address.
  Unfortunately, this can only support far pointers from 16-bit real mode.

  @param[In]  Ptr     16-bit far pointer.

  @return  VOID*      The linear address of Ptr.

**/
VOID *
PnpFarToLinear (
  IN PNP_FAR_PTR                        Ptr
  )
{
  UINT32                                CR3Value;
  UINT32                                CR4Value;
  UINT32                                Ebx, Ecx;
  UINTN                                 SegementSelector;


  GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RBX, mCpuIndex, &Ebx);
  GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RCX, mCpuIndex, &Ecx);

  //
  // Call by new way that AP trigger software SMI directly when the signature is "$ISB'
  //
  if (Ebx == SIGNATURE_32 ('$', 'I', 'S', 'B')) {
    return (VOID *)(UINTN)(UINT32)(Ptr.Offset | Ptr.Segment << 16);
  }

  CR3Value = Ebx;
  CR4Value = Ecx;
  SegementSelector = (UINTN)Ptr.Segment;

  return (VOID *)(UINTN)LogicalToPhysicalAddress ((UINT32)Ptr.Offset, CR3Value, CR4Value, SegementSelector);
}

/**
  Helper function for Pnp0x52, process command DMI_BYTE_CHANGE, DMI_WORD_CHANGE, and DMI_DWORD_CHANGE.

  @param[In]  Data           Buffer of data to SetStructure.
  @param[In]  Structure      The structure to be set.

  @retval DMI_SUCCESS    The function executed successfully.
  @retval DMI_READ_ONLY  The structure is not Type1, the block to be set is not at offset 8.
  @retval DMI_BAD_PARAMETER Parameter is invalid

**/
INT16
Pnp0x52ChangeFixedLength (
  IN FUNC_0x52_DATA_BUFFER             *Data,
  IN SMBIOS_STRUCTURE                  *Structure
  )
{
  UINT8                       *ByteTmpPtr;
  UINT16                      *WordTmpPtr;
  UINT32                      *DWordTmpPtr;
  EFI_STATUS                  Status;
  UINT8                       ByteTemp;
  UINT16                      WordTemp;
  UINT32                      DWordTemp;
  UINTN                       Index;

  ByteTmpPtr  = NULL;
  WordTmpPtr  = NULL;
  DWordTmpPtr = NULL;
  ByteTemp    = 0;
  WordTemp    = 0;
  DWordTemp   = 0;

  for (Index = 0; Index < UpdateableStringCount; Index++) {
    if ((Structure->Type == mUpdatableStrings[Index].Type) &&
        (Data->FieldOffset == mUpdatableStrings[Index].FixedOffset)) {
      break;
    }
  }

  if (Index == UpdateableStringCount) {
    return DMI_READ_ONLY;
  }

  switch (Data->Command) {

  case DMI_BYTE_CHANGE:
    ByteTmpPtr = (UINT8 *)((UINTN)Structure + Data->FieldOffset);
    if (ByteTmpPtr == NULL) {
      return DMI_BAD_PARAMETER;
    }
    ByteTemp   = *ByteTmpPtr;
    ByteTemp   = ByteTemp & ((UINT8)(Data->ChangeMask));
    ByteTemp   = ByteTemp | ((UINT8)((Data->ChangeValue) & ~(Data->ChangeMask)));

    Data->DataLength = sizeof (UINT8);
    CopyMem ((VOID *)&Data->StructureData, (VOID *)&ByteTemp, Data->DataLength);
    break;

  case DMI_WORD_CHANGE:
    WordTmpPtr = (UINT16 *) ((UINTN)Structure + Data->FieldOffset);
    if (WordTmpPtr == NULL) {
      return DMI_BAD_PARAMETER;
    }
    WordTemp   = *WordTmpPtr;
    WordTemp   = WordTemp & ((UINT16)(Data->ChangeMask));
    WordTemp   = WordTemp | ((UINT16)((Data->ChangeValue) & ~(Data->ChangeMask)));

    Data->DataLength = sizeof (UINT16);
    CopyMem ((VOID *)&Data->StructureData, (VOID *)&WordTemp, Data->DataLength);
    break;

  case DMI_DWORD_CHANGE:
    DWordTmpPtr = (UINT32 *) ((UINTN)Structure + Data->FieldOffset);
    if (DWordTmpPtr == NULL) {
      return DMI_BAD_PARAMETER;
    }
    DWordTemp   = *DWordTmpPtr;
    DWordTemp   = DWordTemp & ((UINT32)(Data->ChangeMask));
    DWordTemp   = DWordTemp | ((UINT32)((Data->ChangeValue) & ~(Data->ChangeMask)));

    Data->DataLength = sizeof (UINT32);
    CopyMem ((VOID *)&Data->StructureData, (VOID *)&DWordTemp, Data->DataLength);

    break;
  }

  Status = SetDMI (Data, Structure);
  if (EFI_ERROR (Status)) {
    return DMI_READ_ONLY;
  }

  switch (Data->Command) {

  case DMI_BYTE_CHANGE:
    if (ByteTmpPtr == NULL) {
      return DMI_BAD_PARAMETER;
    }
    CopyMem (ByteTmpPtr, &ByteTemp, sizeof (UINT8));
    break;

  case DMI_WORD_CHANGE:
    if (WordTmpPtr == NULL) {
      return DMI_BAD_PARAMETER;
    }
    CopyMem (WordTmpPtr, &WordTemp, sizeof (UINT16));
    break;

  case DMI_DWORD_CHANGE:
    if (DWordTmpPtr == NULL) {
      return DMI_BAD_PARAMETER;
    }
    CopyMem (DWordTmpPtr, &DWordTemp, sizeof (UINT32));
    break;

  default:
    break;
  }

  return DMI_SUCCESS;
}

/**
  Get size of allocated SMBIOS table buffer and current SMBIOS table.

  @param[In]  IsSmbios32BitTable     Indicate is using 32 bits table(TRUE)/64 bits table(FALSE).
  @param[Out] TotalSmbiosBufferSize  Size of buffer for SMBIOS table allocated in SmbiosDxe driver.
  @param[Out] CurrentTableSize       Size of current SMBIOS table.

  @retval    EFI_SUCCESS             The function executed successfully.
  @retval    EFI_INVALID_PARAMETER   Parameter is invalid.

*/
EFI_STATUS
GetTableAndBufferSize (
  IN  BOOLEAN                   IsSmbios32BitTable,
  OUT UINTN                     *TotalSmbiosBufferSize,
  OUT UINTN                     *CurrentTableSize
  )
{
  UINT32                                SmbiosTableLength;

  if (TotalSmbiosBufferSize == NULL || CurrentTableSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsSmbios32BitTable) {
    SmbiosTableLength = ((SMBIOS_TABLE_ENTRY_POINT*)mSmbiosTableEntryPoint)->TableLength;
  } else {
    SmbiosTableLength = ((SMBIOS_TABLE_3_0_ENTRY_POINT*)mSmbiosTableEntryPoint64Bit)->TableMaximumSize;
  }

  if (TotalIncreasedStringLength >= 0x00) {
    *CurrentTableSize = (UINTN)SmbiosTableLength + (UINTN)TotalIncreasedStringLength;
  } else {
    *CurrentTableSize = (UINTN)SmbiosTableLength - (UINTN)(~(TotalIncreasedStringLength - 1));
  }
  //
  // This value is based on SmbiosDxe driver. If the buffer allocation method is changed, this size will be different.
  //
  *TotalSmbiosBufferSize = EFI_PAGES_TO_SIZE ((UINTN) EFI_SIZE_TO_PAGES (SmbiosTableLength + SMBIOS_TABLE_EXTRA_MEMORY));

  return EFI_SUCCESS;
}

/**
  Find string within the SMBIOS structure

  @param[In]  CountFieldType   Indicates if current type is string count field type.
  @param[In]  Data             Buffer of data to SetStructure.
  @param[In]  Structure        The structure to be set.
  @param[Out] String           Location of string indicate by Data->FieldOffset.
  @param[Out] StringSize       Size of String.

  @retval     EFI_SUCCESS             The function executed successfully.
  @retval     EFI_INVALID_PARAMETER   Parameter is invalid.

*/
EFI_STATUS
FindStringInSmbiosTable (
  IN   BOOLEAN                 CountFieldType,
  IN   FUNC_0x52_DATA_BUFFER   *Data,
  IN   SMBIOS_STRUCTURE        *Structure,
  OUT  CHAR8                   **String,
  OUT  UINTN                   *StringSize
  )
{
  EFI_STATUS         Status;
  CHAR8              *StructureString;
  UINTN              StructureStringSize;

  if (Data == NULL || Structure == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (String == NULL || StringSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (CountFieldType) {
    Status = LocateStringOfStructure (
               (UINT8)Data->FieldOffset,
               Structure,
               &StructureString,
               &StructureStringSize
               );
  } else {
    Status = LocateStringOfStructure (
               ((UINT8 *)Structure)[Data->FieldOffset],
               Structure,
               &StructureString,
               &StructureStringSize
               );
  }
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  *String     = StructureString;
  *StringSize = StructureStringSize;

  return EFI_SUCCESS;
}

/**
  Helper function for Pnp0x52, process command DMI_STRING_CHANGE.

  @param[In]  Data           Buffer of data to SetStructure.
  @param[In]  Structure      The structure to be set.

  @retval DMI_SUCCESS          The function executed successfully.
  @retval DMI_BAD_PARAMETER    The length of data is invalid or can not locate the string in the structure.
  @retval DMI_READ_ONLY        Cannot set the data in the structure.

**/
INT16
Pnp0x52ChangeString (
  IN BOOLEAN                               IsSmbios32BitTable,
  IN FUNC_0x52_DATA_BUFFER                 *Data,
  IN SMBIOS_STRUCTURE                      *Structure
  )
{
  CHAR8                                 *StructureString;
  CHAR8                                 *TempstructureString;
  UINTN                                 StructureStringSize;
  UINTN                                 TempstructureStringSize;
  UINTN                                 Index;
  EFI_STATUS                            Status;
  UINT8                                 *SmbiosStoreArea;
  UINTN                                 CurrentTableSize;
  UINT8                                 *DmiPtr;
  UINTN                                 TotalSmbiosBufferSize;
  STRING_COUNT_TABLE                    *CountFieldTable;
  UINTN                                 CountFieldTableSize;
  UINT8                                 RecordLength;
  BOOLEAN                               CountFieldType = FALSE;
  EFI_PCD_PROTOCOL                      *Pcd;
  UINT64                                SmbiosTableAddress;


  TempstructureString = NULL;
  TempstructureStringSize = 0;
  SmbiosStoreArea = NULL;
  
  DmiPtr = (UINT8 *)(UINTN) FdmGetNAtAddr(&gH2OFlashMapRegionSmbiosUpdateGuid, 1);

  if (!(*(UINT32 *)DmiPtr == DMI_UPDATE_STRING_SIGNATURE)){
    return DMI_READ_ONLY;
  }

  Status = mSmst->SmmLocateProtocol (
                    &gEfiPcdProtocolGuid,
                    NULL,
                    (VOID **)&Pcd
                    );
  if (EFI_ERROR (Status)) {
    return DMI_FUNCTION_NOT_SUPPORTED;
  }

  CountFieldTable = (STRING_COUNT_TABLE *)Pcd->GetPtr (&gSmbiosTokenSpaceGuid, PcdToken (PcdSmbiosStringCountFieldOffset));
  CountFieldTableSize = Pcd->GetSize (&gSmbiosTokenSpaceGuid, PcdToken (PcdSmbiosStringCountFieldOffset)) / sizeof (STRING_COUNT_TABLE);
  RecordLength = 0;
  for (Index = 0; Index < CountFieldTableSize; Index++) {
    if (Structure->Type == CountFieldTable[Index].Type) {
      RecordLength = (UINT8)Pcd->GetSize (&gSmbiosTokenSpaceGuid, PcdToken (PcdType000Record) + CountFieldTable[Index].Type) - 1;
      CountFieldType = TRUE;
      break;
    }
  }

  Status = FindStringInSmbiosTable (CountFieldType, Data, Structure, &StructureString, &StructureStringSize);
  if (EFI_ERROR(Status)) {
    return DMI_BAD_PARAMETER;
  }

  if (CountFieldType) {
    Data->FieldOffset += RecordLength;
  }

  for (Index = 0; Index < UpdateableStringCount; Index++) {
    if ((Structure->Type == mUpdatableStrings[Index].Type) &&
        (Data->FieldOffset == mUpdatableStrings[Index].FixedOffset)) {
      break;
    }
  }
  if (Index == UpdateableStringCount) {
    return DMI_READ_ONLY;
  }

  Status = SetDMI (Data, Structure);
  if (EFI_ERROR (Status)) {
    return DMI_READ_ONLY;
  }

  Status = GetTableAndBufferSize (IsSmbios32BitTable, &TotalSmbiosBufferSize, &CurrentTableSize);
  if (EFI_ERROR(Status)) {
    return DMI_BAD_PARAMETER;
  }

  if (IsSmbios32BitTable) {
    SmbiosTableAddress = ((SMBIOS_TABLE_ENTRY_POINT*)mSmbiosTableEntryPoint)->TableAddress;
  } else {
    SmbiosTableAddress = ((SMBIOS_TABLE_3_0_ENTRY_POINT*)mSmbiosTableEntryPoint64Bit)->TableAddress;
  }

  TempstructureString = StructureString + StructureStringSize + 1;
  TempstructureStringSize = CurrentTableSize - (UINTN)(TempstructureString - SmbiosTableAddress);

  SmbiosStoreArea = AllocateRuntimePool (CurrentTableSize);
  if (SmbiosStoreArea == NULL) {
    return PNP_BUFFER_TOO_SMALL;
  }

  CopyMem (SmbiosStoreArea, TempstructureString, TempstructureStringSize);
  CopyMem (StructureString, Data->StructureData, Data->DataLength);
  TempstructureString = StructureString + Data->DataLength;
  CopyMem (TempstructureString, SmbiosStoreArea, TempstructureStringSize);
  FreePool (SmbiosStoreArea);

  TotalIncreasedStringLength = TotalIncreasedStringLength + (UINT32)Data->DataLength - (UINT32)(StructureStringSize + 1);
  return DMI_SUCCESS;
}

/**
  Helper function for Pnp0x52, process command DMI_BLOCK_CHANGE.

  @param[In]  Data           Buffer of data to SetStructure.
  @param[In]  Structure      The structure to be set.

  @retval DMI_SUCCESS          The function executed successfully.
  @retval DMI_BAD_PARAMETER    The length of data is invalid or can not locate the string in the structure.
  @retval DMI_READ_ONLY        The structure is not Type1, the block to be set is not at offset 8.

**/
INT16
Pnp0x52ChangeBlock (
  IN FUNC_0x52_DATA_BUFFER                 *Data,
  IN SMBIOS_STRUCTURE                      *Structure
  )
{
  UINT8                       *TmpPtr;
  UINT8                       *DataPtr;
  UINTN                       CopyLength;
  EFI_STATUS                  Status;


  TmpPtr     = (UINT8 *)((UINTN)Structure + Data->FieldOffset);
  DataPtr    = (UINT8 *)Data->StructureData;
  CopyLength = (UINTN)Data->DataLength;

  Status = SetDMI (Data, Structure);
  if (EFI_ERROR (Status)) {
    return DMI_READ_ONLY;
  }
  CopyMem (TmpPtr, Data->StructureData, Data->DataLength);

  return DMI_SUCCESS;
}


/**

  Get the full size of SMBIOS structure including optional strings that follow the formatted structure.

  @param This                   The EFI_SMBIOS_PROTOCOL instance.
  @param Head                   Pointer to the beginning of SMBIOS structure.
  @param Size                   The returned size.
  @param NumberOfStrings        The returned number of optional strings that follow the formatted structure.

  @retval EFI_SUCCESS           Size retured in Size.
  @retval EFI_INVALID_PARAMETER Input SMBIOS structure mal-formed or Size is NULL.

**/
EFI_STATUS
EFIAPI
GetSmbiosStructureSize (
  IN   EFI_SMBIOS_TABLE_HEADER          *Head,
  OUT  UINTN                            *Size,
  OUT  UINTN                            *NumberOfStrings
  )
{
  UINTN  FullSize;
  UINTN  StrLen;
  UINTN  MaxLen;
  INT8*  CharInStr;

  if (Size == NULL || NumberOfStrings == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FullSize = Head->Length;
  CharInStr = (INT8*)Head + Head->Length;
  *Size = FullSize;
  *NumberOfStrings = 0;
  StrLen = 0;

  //
  // SMBIOS 3.0 defines the Structure table maximum size as DWORD field limited to 0xFFFFFFFF bytes.
  // Locate the end of string as long as possible.
  //
  MaxLen = SMBIOS_3_0_TABLE_MAX_LENGTH;

  //
  // look for the two consecutive zeros, check the string limit by the way.
  //
  while (*CharInStr != 0 || *(CharInStr+1) != 0) {
    if (*CharInStr == 0) {
      *Size += 1;
      CharInStr++;
    }

    for (StrLen = 0 ; StrLen < MaxLen; StrLen++) {
      if (*(CharInStr+StrLen) == 0) {
        break;
      }
    }

    if (StrLen == MaxLen) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // forward the pointer
    //
    CharInStr += StrLen;
    *Size += StrLen;
    *NumberOfStrings += 1;
  }

  //
  // count ending two zeros.
  //
  *Size += 2;
  return EFI_SUCCESS;
}

/**
  Check if input values in data buffer are illegal or over the limit according to Data->Command.

  @param[In]  IsSmbios32BitTable  Indicate is using 32 bits table(TRUE)/64 bits table(FALSE).
  @param[In]  Data                Buffer of data to SetStructure.
  @param[In]  Structure           The structure to be set.
  @param[In]  StructureSize       Size of Structure.

  @retval   TRUE   Input values are legal.
  @retval   FALSE  Input values are illegal.

*/
BOOLEAN
CheckInputValuesByCommand (
  IN  BOOLEAN                   IsSmbios32BitTable,
  IN  FUNC_0x52_DATA_BUFFER     *Data,
  IN  EFI_SMBIOS_TABLE_HEADER   *Structure,
  IN  UINTN                     StructureSize
  )
{
  EFI_STATUS                Status;
  UINT16                    OffsetResult;
  EFI_PCD_PROTOCOL          *Pcd;
  BOOLEAN                   CountFieldType;
  UINT8                     RecordLength;
  UINTN                     Index;
  UINT8                     Result8;
  STRING_COUNT_TABLE        *CountFieldTable;
  UINTN                     CountFieldTableSize;
  CHAR8                     *StructureString;
  UINTN                     StructureStringSize;
  UINTN                     TotalSmbiosBufferSize;
  UINTN                     CurrentTableSize;
  UINTN                     NewTableSize;

  if (Data == NULL || Structure == NULL || StructureSize == 0) {
    return FALSE;
  }

  switch (Data->Command) {
  case DMI_BYTE_CHANGE:
  case DMI_WORD_CHANGE:
  case DMI_DWORD_CHANGE:
    Status = SafeUint16Add (Data->FieldOffset, Data->DataLength, &OffsetResult);
    if (EFI_ERROR(Status) || (OffsetResult > StructureSize - 2)) {
      return FALSE;
    }
    break;

  case DMI_STRING_CHANGE:
    Status = mSmst->SmmLocateProtocol (
                      &gEfiPcdProtocolGuid,
                      NULL,
                      (VOID **)&Pcd
                      );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }

    RecordLength = 0;
    CountFieldType = FALSE;
    CountFieldTable = (STRING_COUNT_TABLE *)Pcd->GetPtr (&gSmbiosTokenSpaceGuid, PcdToken (PcdSmbiosStringCountFieldOffset));
    CountFieldTableSize = Pcd->GetSize (&gSmbiosTokenSpaceGuid, PcdToken (PcdSmbiosStringCountFieldOffset)) / sizeof (STRING_COUNT_TABLE);
    for (Index = 0; Index < CountFieldTableSize; Index++) {
      if (Structure->Type == CountFieldTable[Index].Type) {
        RecordLength = (UINT8)Pcd->GetSize (&gSmbiosTokenSpaceGuid, PcdToken (PcdType000Record) + CountFieldTable[Index].Type) - 1;
        CountFieldType = TRUE;
        break;
      }
    }

    if (CountFieldType) {
      if (Data->FieldOffset > GetNumberStringOfStructure (Structure)) {
        return FALSE;
      }
      Status = SafeUint8Add (Data->FieldOffset, RecordLength, &Result8);
      if (EFI_ERROR(Status)) {
        return FALSE;
      }
    } else {
      if (Data->FieldOffset > StructureSize - 2) {
        return FALSE;
      }
    }
    if (Data->DataLength > 0xff || Data->DataLength <= 1) {
      return FALSE;
    }
    Status = FindStringInSmbiosTable (CountFieldType, Data, Structure, &StructureString, &StructureStringSize);
    if (!EFI_ERROR(Status)) {
      Status = GetTableAndBufferSize (IsSmbios32BitTable, &TotalSmbiosBufferSize, &CurrentTableSize);
      if (!EFI_ERROR(Status)) {
        if ((UINTN)Data->DataLength > (StructureStringSize + 1)) {
          NewTableSize = CurrentTableSize + (UINTN)Data->DataLength - (StructureStringSize + 1);
          if (NewTableSize > TotalSmbiosBufferSize) {
            return FALSE;
          }
        }
      } else {
        return FALSE;
      }
    } else {
      return FALSE;
    }

    break;

  case DMI_BLOCK_CHANGE:
    if (Structure->Type != 1 || Data->FieldOffset != 8) {
      return FALSE;
    }
    if (Data->DataLength != sizeof(EFI_GUID)) {
      return FALSE;
    }
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/**

  Check if input values in data buffer are illegal or over the limit.
  If free space in OEM_DMI_STORE region is not enough for new data to write,
  DMI reclaim will be performed. After reclaim, if space still not enough, will
  be considered as illegal situation.

  @param[In]  Data                   Buffer of data to set Structure.
  @param[Out] Smbios32BitCanModify   Indicate if SMBIOS 32bit table can be modify.
  @param[Out] Smbios64BitCanModify   Indicate if SMBIOS 64bit table can be modify.

  @retval EFI_SUCCESS            Input values are legal.
  @retval EFI_INVALID_PARAMETER  Input values are illegal.
  @retval Others                 Error occurs when locate structure or string in SMBIOS table.

*/

EFI_STATUS
CheckInputValuesAndReclaimDmiIfNeeded (
  IN   FUNC_0x52_DATA_BUFFER  *Data,
  OUT  BOOLEAN                *Smbios32BitCanModify,
  OUT  BOOLEAN                *Smbios64BitCanModify
  )
{
  EFI_STATUS                Status;
  EFI_SMBIOS_TABLE_HEADER   *Structure;
  UINTN                     StructureSize;
  EFI_SMBIOS_TABLE_HEADER   *Structure64Bit;
  UINTN                     StructureSize64Bit;
  BOOLEAN                   IsSmbios32BitTable;
  BOOLEAN                   IsSmbios64BitTable;
  UINT16                    StructureHandle;
  UINT64                    DmiAddr;
  UINT64                    DmiSize;
  UINT16                    DmiEnd;
  UINT16                    DmiStructureHeaderSize;
  UINT16                    NewDmiStructureSize;
  UINT16                    NewDmiEnd;
  UINT16                    StructureHandle64Bit;

  *Smbios32BitCanModify = FALSE;
  *Smbios64BitCanModify = FALSE;
  IsSmbios32BitTable = FALSE;
  IsSmbios64BitTable = FALSE;
  StructureHandle = Data->StructureHeader.Handle;
  StructureHandle64Bit = Data->StructureHeader.Handle;
  Structure = NULL;
  Structure64Bit = NULL;
  StructureSize = 0;
  StructureSize64Bit = 0;

  if (((PcdGet16 (PcdSmbiosVersion) >> 8) < 0x3) ||
      (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT0) == BIT0))) {
    IsSmbios32BitTable = TRUE;
  }
  if (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT1) == BIT1)) {
    IsSmbios64BitTable = TRUE;
  }

  if (!IsSmbios32BitTable && !IsSmbios64BitTable) {
    return EFI_NOT_FOUND;
  }

  if (IsSmbios32BitTable) {
    Status = LocateSmbiosStructure (IsSmbios32BitTable, &StructureHandle, &Structure, &StructureSize);
    if(EFI_ERROR(Status)){
      return EFI_NOT_FOUND;
    }
  }

  if (IsSmbios64BitTable) {
    Status = LocateSmbiosStructure (FALSE, &StructureHandle64Bit, &Structure64Bit, &StructureSize64Bit);
    if(EFI_ERROR(Status)){
      return EFI_NOT_FOUND;
    }
  }

  //
  // Check if OEM_DMI_STORE region has enough space.
  //
  Status = GetDmiFreeSpace (&DmiAddr, &DmiSize, &DmiEnd);
  if (EFI_ERROR(Status)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Data->Command == DMI_STRING_CHANGE) {
    DmiStructureHeaderSize = sizeof(DMI_STRING_STRUCTURE) - sizeof(UINT8) - 1;
  } else {
    DmiStructureHeaderSize = sizeof(DMI_STRING_STRUCTURE) - sizeof(UINT8);
  }

  Status = SafeUint16Add (Data->DataLength, DmiStructureHeaderSize, &NewDmiStructureSize);
  if (EFI_ERROR(Status)) {
    return EFI_INVALID_PARAMETER;
  }
  if (IsSmbios32BitTable && IsSmbios64BitTable) {
    //
    // If both 32bit & 64bit table exists, SetDMI() will do twice
    // so check if the calculation will overflow.
    //
    Status = SafeUint16Mult (NewDmiStructureSize, 2, &NewDmiStructureSize);
    if (EFI_ERROR(Status)) {
      return EFI_INVALID_PARAMETER;
    }
  }
  Status = SafeUint16Add (DmiEnd, NewDmiStructureSize, &NewDmiEnd);
  if (EFI_ERROR(Status)) {
    return EFI_INVALID_PARAMETER;
  }
  if (NewDmiEnd > (DmiSize - DMI_TOTAL_RECLAIM_RESERVERED_SIZE)) {
    //
    // Space is not enough, do reclaim.
    //
    Status = ReclaimDmiSpace (DmiAddr, DmiSize, &DmiEnd);
    if (EFI_ERROR(Status) || ((DmiEnd + NewDmiStructureSize) > (DmiSize - DMI_TOTAL_RECLAIM_RESERVERED_SIZE))) {
      //
      // Space still not enough to write in new data, so return.
      //
      return EFI_INVALID_PARAMETER;
    }
  }

  if (IsSmbios32BitTable) {
    *Smbios32BitCanModify = CheckInputValuesByCommand (IsSmbios32BitTable, Data, Structure, StructureSize);
  }
  if (IsSmbios64BitTable) {
    *Smbios64BitCanModify = CheckInputValuesByCommand (FALSE, Data, Structure64Bit, StructureSize64Bit);
  }
  if ((IsSmbios32BitTable && !(*Smbios32BitCanModify)) ||
      (IsSmbios64BitTable && !(*Smbios64BitCanModify))) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  PnP function 0x50, Get SMBIOS Information.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS     The function was executed successfully.

**/
INT16
Pnp0x50 (
  IN VOID            *Frame
  )
{
  UINT8                                 *DmiBIOSRevision;
  UINT16                                *NumStructures;
  UINT16                                *StructureSize;
  UINT32                                *DmiStorageBase;
  UINT16                                *DmiStorageSize;
  BOOLEAN     IsSmbios32BitTable;

  IsSmbios32BitTable = FALSE;

  if (((PcdGet16 (PcdSmbiosVersion) >> 8) < 0x3) ||
      (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT0) == BIT0))) {
    IsSmbios32BitTable = TRUE;
  }

  LocateSmbiosTable ();

  if (IsSmbios32BitTable){

    DmiBIOSRevision = (UINT8 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->DmiBIOSRevision);
    if (!BufferInCmdBufferPnp ((VOID *) DmiBIOSRevision, sizeof(UINT8))) {
      return DMI_BAD_PARAMETER;
    }
    *DmiBIOSRevision = mSmbiosTableEntryPoint->SmbiosBcdRevision;

    NumStructures = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->NumStructures);
    if (!BufferInCmdBufferPnp ((VOID *) NumStructures, sizeof(UINT16))) {
      return DMI_BAD_PARAMETER;
    }
    *NumStructures = mSmbiosTableEntryPoint->NumberOfSmbiosStructures;

    StructureSize = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->StructureSize);
     if (!BufferInCmdBufferPnp ((VOID *) StructureSize, sizeof(UINT16))) {
      return DMI_BAD_PARAMETER;
    }
    *StructureSize = mSmbiosTableEntryPoint->MaxStructureSize;

    DmiStorageBase = (UINT32 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->DmiStorageBase);
    if (!BufferInCmdBufferPnp ((VOID *) DmiStorageBase, sizeof(UINT32))) {
      return DMI_BAD_PARAMETER;
    }
    *DmiStorageBase = (UINT32)(UINTN)mSmbiosTable;

    DmiStorageSize = (UINT16 *)PnpFarToLinear(((PNP_FUNCTION_0x50_FRAME *)Frame)->DmiStorageSize);
    if (!BufferInCmdBufferPnp ((VOID *) DmiStorageSize, sizeof(UINT16))) {
      return DMI_BAD_PARAMETER;
    }
    *DmiStorageSize = mSmbiosTableEntryPoint->TableLength;

  } else {

    UINT64  TableAddress;
    UINT64  TableEndAddress;
    UINTN   NumberOfStrings;
    UINTN   Size;
    UINT16  NumberOfStructure;
    UINTN   MaxStructureSize;
    EFI_STATUS  Status;

    NumberOfStructure = 0;
    MaxStructureSize = 0;

    if (mSmbiosTableEntryPoint64Bit == NULL){
      return DMI_FUNCTION_NOT_SUPPORTED;
    }
    TableAddress = mSmbiosTableEntryPoint64Bit->TableAddress;
    TableEndAddress = mSmbiosTableEntryPoint64Bit->TableAddress + mSmbiosTableEntryPoint64Bit->TableMaximumSize;

    while (TableAddress < TableEndAddress){
      Status = GetSmbiosStructureSize ((EFI_SMBIOS_TABLE_HEADER*)(UINTN)TableAddress, &Size, &NumberOfStrings);
      if (EFI_ERROR(Status)){
        break;
      }

      if (MaxStructureSize < Size){
        MaxStructureSize = Size;
      }
      NumberOfStructure++;
      TableAddress += Size;
    }

    DmiBIOSRevision = (UINT8 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->DmiBIOSRevision);
    if (!BufferInCmdBufferPnp ((VOID *) DmiBIOSRevision, sizeof(UINT8))) {
      return DMI_BAD_PARAMETER;
    }
    *DmiBIOSRevision = (UINT8) ((PcdGet16 (PcdSmbiosVersion) >> 4) & 0xf0) | (UINT8) (PcdGet16 (PcdSmbiosVersion) & 0x0f);

    NumStructures = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->NumStructures);
    if (!BufferInCmdBufferPnp ((VOID *) NumStructures, sizeof(UINT16))) {
      return DMI_BAD_PARAMETER;
    }
    *NumStructures = NumberOfStructure;

    StructureSize = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->StructureSize);
     if (!BufferInCmdBufferPnp ((VOID *) StructureSize, sizeof(UINT16))) {
      return DMI_BAD_PARAMETER;
    }
    *StructureSize = (UINT16) MaxStructureSize;

    DmiStorageBase = (UINT32 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->DmiStorageBase);
    if (!BufferInCmdBufferPnp ((VOID *) DmiStorageBase, sizeof(UINT32))) {
      return DMI_BAD_PARAMETER;
    }
    *DmiStorageBase = (UINT32)(UINTN)mSmbiosTable64Bit;

    DmiStorageSize = (UINT16 *)PnpFarToLinear(((PNP_FUNCTION_0x50_FRAME *)Frame)->DmiStorageSize);
    if (!BufferInCmdBufferPnp ((VOID *) DmiStorageSize, sizeof(UINT16))) {
      return DMI_BAD_PARAMETER;
    }
    *DmiStorageSize = (UINT16)((SMBIOS_TABLE_3_0_ENTRY_POINT*)mSmbiosTableEntryPoint64Bit)->TableMaximumSize;
  }

  return DMI_SUCCESS;
}

/**
  PnP function 0x51, Get SMBIOS Structure.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS            The function was executed successfully.
  @retval     DMI_INVALID_HANDLE     Invalid handle.

**/
INT16
Pnp0x51 (
  IN VOID            *Frame
  )
{
  UINT16                                *Structure;
  UINT8                                 *DmiStrucBuffer;
  SMBIOS_STRUCTURE                      *Location;
  UINTN                                 Size;
  BOOLEAN                               IsSmbios32BitTable;

  IsSmbios32BitTable = FALSE;

  if (((PcdGet16 (PcdSmbiosVersion) >> 8) < 0x3) ||
      (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT0) == BIT0))) {
    IsSmbios32BitTable = TRUE;
  } else {
    IsSmbios32BitTable = FALSE;
  }

  LocateSmbiosTable ();

  Structure      = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x51_FRAME *)Frame)->Structure);
  DmiStrucBuffer = (UINT8 *)PnpFarToLinear (((PNP_FUNCTION_0x51_FRAME *)Frame)->DmiStrucBuffer);
  Location = (SMBIOS_STRUCTURE *)DmiStrucBuffer;
  if (!BufferInCmdBufferPnp ((VOID *) Structure, sizeof(UINT16))) {
     return DMI_BAD_PARAMETER;
  }
  if (!EFI_ERROR (LocateSmbiosStructure (IsSmbios32BitTable, Structure, &Location, &Size))) {
    if (!BufferInCmdBufferPnp ((VOID *) DmiStrucBuffer, Size)) {
      return DMI_BAD_PARAMETER;
    }
    CopyMem (DmiStrucBuffer, (VOID *)Location, Size);
    return DMI_SUCCESS;
  } else {
    return DMI_INVALID_HANDLE;
  }
}

/**
  PnP function 0x52, Set SMBIOS Structure.

  @param[In]      Frame                 Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS           The function was executed successfully.
  @retval     DMI_BAD_PARAMETER     Invalid command.

**/
INT16
Pnp0x52 (
  IN VOID            *Frame
  )
{
  FUNC_0x52_DATA_BUFFER                 *Data;
  SMBIOS_STRUCTURE                      *StructureLocation;
  SMBIOS_STRUCTURE                      *StructureLocation64Bit;
  UINT16                                StructureHandle;
  UINT16                                StructureHandle64Bit;
  UINT8                                 OldFieldOffset;
  UINTN                                 StructureSize;
  UINTN                                 StructureSize64Bit;
  INT16                                 ReturnStatus;
  INT16                                 ReturnStatus64Bit;
  EFI_STATUS                            Status;
  BOOLEAN                               IsSmbios32BitTable;
  BOOLEAN                               IsSmbios64BitTable;
  FUNC_0x52_DATA_BUFFER                 *OrgData;
  BOOLEAN                               Smbios32BitCanModify;
  BOOLEAN                               Smbios64BitCanModify;

  IsSmbios32BitTable = FALSE;
  IsSmbios64BitTable = FALSE;
  ReturnStatus = DMI_SUCCESS;
  ReturnStatus64Bit = DMI_SUCCESS;
  StructureLocation  = NULL;
  StructureLocation64Bit  = NULL;

  Data = (FUNC_0x52_DATA_BUFFER *)PnpFarToLinear (((PNP_FUNCTION_0x52_FRAME *)Frame)->DmiDataBuffer);
  if (Data == NULL || !BufferInCmdBufferPnp ((VOID *)Data, sizeof(FUNC_0x52_DATA_BUFFER) - sizeof(UINT8))) {
    return DMI_BAD_PARAMETER;
  }
  //
  // Check passed in DataLength is correct in case DMI_BYTE_CHANGE, case DMI_WORD_CHANGE, case DMI_DWORD_CHANGE
  //
  if ((Data->Command == DMI_BYTE_CHANGE && Data->DataLength != 1) ||
      (Data->Command == DMI_WORD_CHANGE && Data->DataLength != 2) ||
      (Data->Command == DMI_DWORD_CHANGE && Data->DataLength != 4)) {
    return DMI_BAD_PARAMETER;
  }

  if (!BufferInCmdBufferPnp ((VOID *) Data, sizeof(FUNC_0x52_DATA_BUFFER) + Data->DataLength - sizeof(UINT8))) {
    return DMI_BAD_PARAMETER;
  }
  //
  // Copy FUNC_0x52_DATA_BUFFER to SMRAM to prevent from TOCTOU attack.
  //
  OrgData = Data;
  Data = AllocateCopyPool (sizeof(FUNC_0x52_DATA_BUFFER) + Data->DataLength - sizeof(UINT8), OrgData);
  if (Data == NULL) {
    return DMI_SUCCESS;
  }

  if (((PcdGet16 (PcdSmbiosVersion) >> 8) < 0x3) ||
      (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT0) == BIT0))) {
    IsSmbios32BitTable = TRUE;
  }
  if (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT1) == BIT1)) {
    IsSmbios64BitTable = TRUE;
  }
  //
  // Check inputs values are valid or not.
  //
  Status = CheckInputValuesAndReclaimDmiIfNeeded (Data, &Smbios32BitCanModify, &Smbios64BitCanModify);
  if (EFI_ERROR(Status)) {
    if (IsSmbios32BitTable && !Smbios32BitCanModify) {
      ReturnStatus = DMI_BAD_PARAMETER;
    }
    if (IsSmbios64BitTable && !Smbios64BitCanModify) {
      ReturnStatus64Bit = DMI_BAD_PARAMETER;
    }
    if ((!Smbios32BitCanModify && !Smbios64BitCanModify)) {
      goto Done;
    }
  }

  StructureHandle      = Data->StructureHeader.Handle;
  StructureHandle64Bit = StructureHandle;

  if (IsSmbios32BitTable && Smbios32BitCanModify) {
    Status = LocateSmbiosStructure (IsSmbios32BitTable, &StructureHandle, &StructureLocation, &StructureSize);
    if(EFI_ERROR(Status)){
      goto Done;
    }
  }

  if (IsSmbios64BitTable && Smbios64BitCanModify) {
    Status = LocateSmbiosStructure (FALSE, &StructureHandle64Bit, &StructureLocation64Bit, &StructureSize64Bit);
    if(EFI_ERROR(Status)){
      goto Done;
    }
  }

  switch (Data->Command) {

  case DMI_BYTE_CHANGE:
  case DMI_WORD_CHANGE:
  case DMI_DWORD_CHANGE:
    if (IsSmbios32BitTable && Smbios32BitCanModify){
      ReturnStatus = Pnp0x52ChangeFixedLength (Data, StructureLocation);
    }

    if (IsSmbios64BitTable && Smbios64BitCanModify){
      ReturnStatus64Bit = Pnp0x52ChangeFixedLength (Data, StructureLocation64Bit);
    }
    break;

  case DMI_STRING_CHANGE:
    OldFieldOffset = Data->FieldOffset;
    if (IsSmbios32BitTable && Smbios32BitCanModify){
      ReturnStatus = Pnp0x52ChangeString (IsSmbios32BitTable, Data, StructureLocation);
    }

    if (IsSmbios64BitTable && Smbios64BitCanModify){
      if (Data->FieldOffset != OldFieldOffset) {
        Data->FieldOffset = OldFieldOffset;
      }
      ReturnStatus64Bit = Pnp0x52ChangeString (FALSE, Data, StructureLocation64Bit);
    }
    break;

  case DMI_BLOCK_CHANGE:
    if (IsSmbios32BitTable && Smbios32BitCanModify){
      ReturnStatus = Pnp0x52ChangeBlock (Data, StructureLocation);
    }

    if (IsSmbios64BitTable && Smbios64BitCanModify){
      ReturnStatus64Bit = Pnp0x52ChangeBlock (Data, StructureLocation64Bit);
    }
    break;

  default:
    ReturnStatus = DMI_BAD_PARAMETER;
    break;
  }

Done:
  //
  // Restore data from SMRAM to IHISI command buffer.
  //
  CopyMem (OrgData, Data, sizeof(FUNC_0x52_DATA_BUFFER) + Data->DataLength - sizeof(UINT8));
  FreePool (Data);

  ReturnStatus = EFI_ERROR (ReturnStatus64Bit) ? ReturnStatus64Bit : ReturnStatus;

  return ReturnStatus;
}

/**
  default function returning that the call was to an unsupported function.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.(unused)

  @retval     DMI_SUCCESS                 The function was executed successfully.
  @retval     DMI_INVALID_SUBFUNCTION     Invalid command.

**/
INT16
UnsupportedPnpFunction (
  IN VOID               *Frame
  )
{
  return PNP_FUNCTION_NOT_SUPPORTED;
}

/**
  PnP function 0x54.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS     The function was executed successfully.

**/
INT16
Pnp0x54 (
  IN VOID            *Frame
  )
{
  INT16                                SubFunction;
  UINT8                                *Data;
  UINT8                                Control;
  UINTN                                Index;

  SubFunction = (UINT16)((PNP_FUNCTION_0x54_FRAME *)Frame)->SubFunction;
  Data = (UINT8 *)PnpFarToLinear (((PNP_FUNCTION_0x54_FRAME *)Frame)->Data);
  if (!BufferInCmdBufferPnp ((VOID *) Data, sizeof(UINT8))) {
    return DMI_BAD_PARAMETER;
  }
  Control = (UINT8)((PNP_FUNCTION_0x54_FRAME *)Frame)->Control;

  if ((Control & 0x01) == 0) {
    if (SubFunction >= 1) {
      return DMI_INVALID_SUBFUNCTION;
    }
    return DMI_SUCCESS;
  }

  switch (SubFunction) {

  case 0:
    //
    // for event log
    //
    if (!mIsOemGPNVMap) {
      return DMI_FUNCTION_NOT_SUPPORTED;
    }
    for (Index = 0; Index < OemGPNVHandleCount; Index++) {
      ClearBiosEventLog (GPNVMapBuffer.PlatFormGPNVMap[Index].BaseAddress, GPNVMapBuffer.PlatFormGPNVMap[Index].GPNVSize);
    }
    break;

  case 1:
    return DMI_INVALID_SUBFUNCTION;
    break;

  case 2:
    return DMI_INVALID_SUBFUNCTION;
    break;

  default:
    return DMI_INVALID_SUBFUNCTION;
    break;
  }

  return DMI_SUCCESS;
}

/**
  PnP function 0x55, Set SMBIOS Structure.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS            The function was executed successfully.
  @retval     DMI_INVALID_HANDLE     Invalid handle.

**/
INT16
Pnp0x55 (
  IN VOID            *Frame
  )
{
  UINT16                                *Handle;
  UINT16                                HandleTemp;
  UINT16                                *MinGPNVRWSize;
  UINT16                                *GPNVSize;
  UINT32                                *NVStorageBase;
  UINT16                                Index;

  Handle = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x55_FRAME *)Frame)->Handle);
  if (!BufferInCmdBufferPnp ((VOID *) Handle, sizeof(UINT16))) {
    return DMI_BAD_PARAMETER;
  }

  Index = HandleTemp = *Handle;

  if (!VaildGPNVCheck (&Index, HandleTemp)) {
    return  DMI_INVALID_HANDLE;
  }

  MinGPNVRWSize = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x55_FRAME *)Frame)->MinGPNVRWSize);
  if (!BufferInCmdBufferPnp ((VOID *) MinGPNVRWSize, sizeof(UINT16))) {
    return DMI_BAD_PARAMETER;
  }
  *MinGPNVRWSize = (UINT16)GPNVMapBuffer.PlatFormGPNVMap[Index].MinGPNVSize;

  GPNVSize = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x55_FRAME *)Frame)->GPNVSize);
  if (!BufferInCmdBufferPnp ((VOID *) GPNVSize, sizeof(UINT16))) {
    return DMI_BAD_PARAMETER;
  }
  *GPNVSize = (UINT16)GPNVMapBuffer.PlatFormGPNVMap[Index].GPNVSize;

  NVStorageBase = (UINT32 *)PnpFarToLinear (((PNP_FUNCTION_0x55_FRAME *)Frame)->NVStorageBase);
  if (!BufferInCmdBufferPnp ((VOID *) NVStorageBase, sizeof(UINT32))) {
    return DMI_BAD_PARAMETER;
  }
  *NVStorageBase = (UINT32)GPNVMapBuffer.PlatFormGPNVMap[Index].BaseAddress;

  Index++;
  if (Index >= FixedPcdGet16 (PcdDefaultGpnvMapBufferSize)) {
    return DMI_INVALID_HANDLE;
  }

  *Handle = GPNVMapBuffer.PlatFormGPNVMap[Index].Handle;

  return DMI_SUCCESS;
}

/**
  PnP function 0x56, Set SMBIOS Structure.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS            The function was executed successfully.
  @retval     DMI_INVALID_HANDLE     Invalid handle.

**/
INT16
Pnp0x56 (
  IN VOID            *Frame
  )
{
  UINT16                                Handle;
  UINT8                                 *GPNVBuffer;
  UINT16                                Index;

  Handle = (UINT16)((PNP_FUNCTION_0x56_FRAME *)Frame)->Handle;
  GPNVBuffer = (UINT8 *)PnpFarToLinear (((PNP_FUNCTION_0x56_FRAME *)Frame)->GPNVBuffer);

  Index = Handle;
  if (!VaildGPNVCheck (&Index, Handle)) {
    return DMI_INVALID_HANDLE;
  }

  if (!BufferInCmdBufferPnp ((VOID *) GPNVBuffer, GPNVMapBuffer.PlatFormGPNVMap[Index].GPNVSize)) {
    return DMI_BAD_PARAMETER;
  }

  CopyMem (
    GPNVBuffer,
    (VOID *)(UINTN)GPNVMapBuffer.PlatFormGPNVMap[Index].BaseAddress,
    GPNVMapBuffer.PlatFormGPNVMap[Index].GPNVSize
    );

  return DMI_SUCCESS;
}

/**
  PnP function 0x57, Set SMBIOS Structure.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS            The function was executed successfully.
  @retval     DMI_INVALID_HANDLE     Invalid handle.

**/
INT16
Pnp0x57 (
  IN VOID            *Frame
  )
{
  UINT16                                Handle;
  UINT8                                 *GPNVBuffer;
  UINT16                                Index;

  Handle = (UINT16)((PNP_FUNCTION_0x57_FRAME *)Frame)->Handle;
  GPNVBuffer = (UINT8 *)PnpFarToLinear (((PNP_FUNCTION_0x57_FRAME *)Frame)->GPNVBuffer);

  Index = Handle;
  if (! VaildGPNVCheck (&Index, Handle)) {
    return DMI_INVALID_HANDLE;
  }

  if (!BufferInCmdBufferPnp ((VOID *) GPNVBuffer, GPNVMapBuffer.PlatFormGPNVMap[Index].GPNVSize)) {
    return DMI_BAD_PARAMETER;
  }
  WriteGPNV (Index, GPNVBuffer);

  return DMI_SUCCESS;
}

/**
  Get frame size of specific sub-function number.

  @param[In]    Frame      Pointer to input Pnp Function Frame.
  @param[Out]   FrameSize  Size of the input Pnp Function Frame of specific sub-function number.

  @retval   EFI_SUCCESS               Frame size of specific sub-function number get successfully.
  @retval   EFI_INVALID_PARAMETER     Sub-function number is not a legal value.

*/
EFI_STATUS
GetFrameSize (
  IN  PNP_GENERIC_ENTRY_FRAME   *Frame,
  OUT UINTN                     *FrameSize
  )
{
  UINTN    Index;

  if (Frame == NULL || FrameSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!BufferInCmdBufferPnp ((VOID *) Frame, sizeof(PNP_GENERIC_ENTRY_FRAME))) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < ARRAY_SIZE(mFrameSizeInfos); Index++) {
    if (Frame->Function == mFrameSizeInfos[Index].Function) {
      *FrameSize = mFrameSizeInfos[Index].FrameSize;
      return EFI_SUCCESS;
    }
  }
  return EFI_INVALID_PARAMETER;
}

/**
  This fucntion is triggered by SMI function call 0x47.

  @param[In]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[In]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[In]      CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[In]      CommBufferSize  The size of the CommBuffer.

  @retval     EFI_SUCCESS              The callabck was handled successfully.
  @retval     EFI_OUT_OF_RESOURCES     Cannot locate SmmCpuProtocol.
  @retval     EFI_NOT_FOUND            Cannot determine the source of SMI.

**/
EFI_STATUS
EFIAPI
PnPBiosCallback (
  IN  EFI_HANDLE                   DispatchHandle,
  IN  CONST  VOID                  *DispatchContext,
  IN  OUT  VOID                    *CommBuffer,
  IN  OUT  UINTN                   *CommBufferSize
  )
{
  EFI_DWORD_REGS                        *RegBufferPtr;
  UINT32                                CR3Value;
  UINT32                                CR4Value;
  UINTN                                 SegementSelector;
  UINTN                                 Index;
  UINTN                                 CPUFoundIndex;
  PNP_GENERIC_ENTRY_FRAME               *Frame;
  INT16                                 ReturnStatus;
  EFI_STATUS                            Status;
  UINT32                                Eax;
  UINT32                                Edx;
  UINT32                                Ebx;
  UINT32                                Ecx;
  UINT32                                Esi;
  UINT32                                Ss;
  PNP_GENERIC_ENTRY_FRAME               *OrgFrame;
  UINTN                                 FrameSize;

  RegBufferPtr = NULL;

  //
  // Locate Smm Cpu protocol for Cpu save state manipulation
  //
  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid,
                    NULL,
                    (VOID **)&mSmmCpu
                    );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  CPUFoundIndex = mSmst->NumberOfCpus;
  for (Index = 0; Index < mSmst->NumberOfCpus; Index++) {
    Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RAX, Index, &Eax);
    Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RDX, Index, &Edx);
    //
    // Find out which CPU triggered PnP SMI with use Edx, this is first priority.
    //
    if (((Eax & 0xff) == SMM_PnP_BIOS_CALL) && ((Edx & 0xffff) == PcdGet16 (PcdSoftwareSmiPort))) {
      //
      // Cpu found!
      //
      CPUFoundIndex = Index;
      break;
    }
    //
    // Re-check again for SMI trigger without use Edx, this is second priority.
    // if cpu found, continue to look the first priority at next CPU.
    //
    if ((Eax & 0xff) == SMM_PnP_BIOS_CALL) {
      //
      // Cpu found!
      //
      CPUFoundIndex = Index;
    }
  }
  if (CPUFoundIndex == mSmst->NumberOfCpus) {
    //
    // Error out due to CPU not found
    //
    return EFI_NOT_FOUND;
  }
  mCpuIndex = CPUFoundIndex;

  Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RBX, mCpuIndex, &Ecx);
  Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RBX, mCpuIndex, &Ebx);
  Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RSI, mCpuIndex, &Esi);
  if (Ebx == SIGNATURE_32 ('$', 'I', 'S', 'B')) {
    //
    // Call by new way that AP trigger software SMI directly when the signature is "$ISB'
    //
    Frame = (PNP_GENERIC_ENTRY_FRAME *)(UINTN)Esi;
  } else {
    Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_SS, mCpuIndex, &Ss);
    //
    // Call by csm16
    //
    CR3Value = Ebx;
    CR4Value = Ecx;
    SegementSelector = (UINTN)Ss;
    RegBufferPtr = (EFI_DWORD_REGS *)(UINTN)LogicalToPhysicalAddress (Esi, CR3Value, CR4Value, SegementSelector);
    GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RBX, mCpuIndex, &CR3Value);
    GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RCX, mCpuIndex, &CR4Value);
    Frame = (PNP_GENERIC_ENTRY_FRAME *)(UINTN)LogicalToPhysicalAddress (
                                                (UINT32)RegBufferPtr->ESP,
                                                CR3Value,
                                                CR4Value,
                                                (UINTN)RegBufferPtr->SS
                                                );
  }
  //
  // Check if entire PNP_GENERIC_ENTRY_FRAME exists in IHISI command buffer.
  //
  Status = GetFrameSize (Frame, &FrameSize);
  if (EFI_ERROR(Status)) {
    ReturnStatus = (UINT32) DMI_FUNCTION_NOT_SUPPORTED;
    goto Done;
  }
  if (!BufferInCmdBufferPnp ((VOID *) Frame, FrameSize)) {
    ReturnStatus = (UINT32) DMI_BAD_PARAMETER;
    goto Done;
  }
  //
  // Copy PNP_GENERIC_ENTRY_FRAME to SMRAM to prevent from TOCTOU attack.
  //
  OrgFrame = Frame;
  Frame = AllocateCopyPool (FrameSize, OrgFrame);
  if (Frame == NULL) {
    return EFI_SUCCESS;
  }
  ReturnStatus = (UINT32) mPnpDmiFunctions[Frame->Function & 0x7](Frame);
  //
  // Restore data frame from SMRAM to IHISI Command Buffer.
  //
  CopyMem (OrgFrame, Frame, FrameSize);
  FreePool (Frame);

Done:
  if (Ebx == SIGNATURE_32 ('$', 'I', 'S', 'B')) {
    Eax = (UINT32)ReturnStatus;
    Status = SetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RAX, mCpuIndex, &Eax);
  } else {
    *((INT16 *)&(RegBufferPtr->EAX)) = ReturnStatus;
  }

  return EFI_SUCCESS;
}

/**
  This function is SMM Communication call back for SMBIOS PNP functions.

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS            The interrupt was handled and quiesced. No other handlers
                                 should still be called.
**/
EFI_STATUS
EFIAPI
SmmSmbiosPnpHandler (
  IN  EFI_HANDLE                        DispatchHandle,
  IN  CONST VOID                        *RegisterContext,
  IN  OUT VOID                          *CommBuffer,
  IN  OUT UINTN                         *CommBufferSize
  )
{
  SMM_SMBIOS_PNP_COMMUNICATE_HEADER     *SmmSmbiosPnpHeader;
  SMM_SMBIOS_PNP_ADDRESS                *SmmSmbiosPnpEntry;
  UINTN                                 SmmSmbiosPnpCommBufferSize;

  //
  // Address of SMBIOS table entry should only be initialized once.
  // Check if the SMI handler has been already called.
  //
  if (mSmmSmbiosPnpInitialized) {
    return EFI_SUCCESS;
  }

  if (CommBuffer == NULL || CommBufferSize == NULL) {
    return EFI_SUCCESS;
  }
  //
  // Check buffer size, address, must be the memory which is allocated in DXE part entry point to make sure
  // the memory will not overlap SMM RAM.
  //
  if (*CommBufferSize != SMM_SMBIOS_PNP_COMM_BUFF_SIZE - SMM_COMMUNICATE_HEADER_SIZE ||
      (UINTN)CommBuffer != (UINTN)mSmmSMBIOSPnpBuffer + SMM_COMMUNICATE_HEADER_SIZE) {
    return EFI_SUCCESS;
  }
  //
  // Copy SMM_SMBIOS_PNP_COMM_BUFF to SMRAM to prevent from TOCTOU attack.
  //
  SmmSmbiosPnpCommBufferSize = sizeof (SMM_SMBIOS_PNP_COMMUNICATE_HEADER) + sizeof(SMM_SMBIOS_PNP_ADDRESS) - sizeof(UINT8);
  SmmSmbiosPnpHeader = AllocateCopyPool (SmmSmbiosPnpCommBufferSize, CommBuffer);
  if (SmmSmbiosPnpHeader == NULL) {
    return EFI_SUCCESS;
  }
  SmmSmbiosPnpEntry  = (SMM_SMBIOS_PNP_ADDRESS *) SmmSmbiosPnpHeader->Data;

  switch (SmmSmbiosPnpHeader->Function) {
  case SMM_COMM_SMBIOS_PNP_ENTRY_SET:
    if (mSMBIOSTableEntryAddress == 0) {
      mSMBIOSTableEntryAddress = (EFI_PHYSICAL_ADDRESS) SmmSmbiosPnpEntry->Address;
    }

    if (mSMBIOSTableEntryAddress64Bit == 0) {
      mSMBIOSTableEntryAddress64Bit = (EFI_PHYSICAL_ADDRESS) SmmSmbiosPnpEntry->Address64Bit;
    }

    break;

  default:
    break;
  }
  //
  // Restore data from SMRAM to parameter communication buffer.
  //
  CopyMem (CommBuffer, SmmSmbiosPnpHeader, SmmSmbiosPnpCommBufferSize);
  FreePool (SmmSmbiosPnpHeader);

  mSmmSmbiosPnpInitialized = TRUE;

  return EFI_SUCCESS;
}

/**
  Clear BIOS event log data from GPNV area.

  @param[In]  GPNVBase               GPNV Start address
  @param[In]  GPNVLength             GPNV Length

  @retval     DMI_SUCCESS            The function was executed successfully.
  @retval     DMI_INVALID_HANDLE     Invalid handle.

**/
STATIC
EFI_STATUS
ClearBiosEventLog (
  IN UINTN                              GPNVBase,
  IN UINTN                              GPNVLength
  )
{
  EFI_STATUS                            Status;
  EFI_STATUS                            EraseStatus;
  UINT8                                 *Buffer;
  UINT32                                Index;
  UINTN                                 BlockSize;
  UINTN                                 FdSupportEraseSize;
  UINTN                                 EraseCount;
  UINTN                                 EraseStartAddress;

  FdSupportEraseSize = GetFlashBlockSize();
  EraseStartAddress = GPNVBase & (~(FdSupportEraseSize - 1));
  EraseCount = GET_ERASE_SECTOR_NUM (
                                 GPNVBase,
                                 GPNVLength,
                                 EraseStartAddress,
                                 FdSupportEraseSize
                                 );

  BlockSize = FdSupportEraseSize * EraseCount;

  Buffer = AllocateRuntimePool (BlockSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  //Copy whole block data to buffer
  //
  CopyMem(Buffer, (VOID *)(UINTN)EraseStartAddress, BlockSize);

  //
  //Copy modified GPNV data to buffer
  //
  for (Index = 0; Index < GPNVLength; Index++) {
    *(UINT8 *)((Buffer + (GPNVBase) - (GPNVBase & ((UINT32)(~0)) & (~(FdSupportEraseSize - 1))))+Index) = 0xFF;
  }

  //
  //Flash GPNV
  //
  Status = EFI_SUCCESS;
  for (Index = 0; Index < EraseCount; Index++) {

    EraseStatus = FlashErase (
                    EraseStartAddress + FdSupportEraseSize * Index,
                    FdSupportEraseSize
                    );
    if (EFI_ERROR (EraseStatus)) {
      Status = EraseStatus;
    }
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = FlashProgram (
                        (UINT8 *)EraseStartAddress,
                        Buffer,
                        &BlockSize,
                        EraseStartAddress
                        );

  FreePool (Buffer);

  return Status;
}

/**
  Restore data from NV_FTW_SPARE to OEM_DMI_STORE if needed

  @retval EFI_SUCCESS    Success
  @retval Others         Error occurs

**/
EFI_STATUS
FtwSpareToDmiIfNeeded (
  VOID
  )
{
  EFI_STATUS                 Status;
  UINT64                     DmiAddr;
  UINT64                     DmiSize;
  UINT64                     SpareSize;
  UINT64                     NvSpareAddr;
  UINT64                     NvSpareSize;
  UINT8                      *DmiBuffer;
  UINT8                      *Ptr;
  DMI_RECLAIM_IN_FTW_SPARE   *DmiReclaimInFtwSparePtr;

  DmiAddr = FdmGetNAtAddr(&gH2OFlashMapRegionSmbiosUpdateGuid, 1);
  if (DmiAddr == 0) {
    return EFI_NOT_FOUND;
  }

  DmiSize = FdmGetNAtSize(&gH2OFlashMapRegionSmbiosUpdateGuid, 1);
  if (DmiSize == 0) {
    return EFI_NOT_FOUND;
  }

  NvSpareAddr = FdmGetNAtAddr (&gH2OFlashMapRegionFtwBackupGuid, 1);
  if (NvSpareAddr == 0) {
    return EFI_NOT_FOUND;
  }
  NvSpareSize = FdmGetNAtSize(&gH2OFlashMapRegionFtwBackupGuid, 1);
  if (NvSpareSize < DmiSize * DMI_USING_FTW_BACKUP_MULTIPLE_OF_DMI_REGION) {
    return EFI_NOT_FOUND;
  }
  Ptr = (UINT8 *)(UINTN) NvSpareAddr;
  DmiReclaimInFtwSparePtr = (DMI_RECLAIM_IN_FTW_SPARE *) Ptr;
  if (!CompareGuid(&gH2OFlashMapRegionSmbiosUpdateGuid, &DmiReclaimInFtwSparePtr->DmiReclaimGuid) ||
    DmiReclaimInFtwSparePtr->Valid != DMI_USING_FTW_BACKUP_VALID_FLAG) {
    return EFI_NOT_FOUND;
  }

  Status = mSmmFwBlockService->EraseBlocks (
                                 mSmmFwBlockService,
                                 (UINTN) DmiAddr,
                                 (UINTN *) &DmiSize
                                 );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DmiBuffer = AllocateRuntimePool ((UINTN) DmiSize);
  if (DmiBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Ptr = (UINT8 *)(UINTN) NvSpareAddr;
  CopyMem (DmiBuffer,  Ptr + sizeof (DMI_RECLAIM_IN_FTW_SPARE), (UINTN) DmiSize);
  Status = mSmmFwBlockService->Write (
                                 mSmmFwBlockService,
                                 (UINTN) DmiAddr,
                                 (UINTN *)&DmiSize,
                                 DmiBuffer
                                 );
  FreePool(DmiBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SpareSize = DmiSize * DMI_USING_FTW_BACKUP_MULTIPLE_OF_DMI_REGION;
  Status = mSmmFwBlockService->EraseBlocks (
                                 mSmmFwBlockService,
                                 (UINTN) NvSpareAddr,
                                 (UINTN *) &SpareSize
                                 );
  return Status;
}

/**
  Perform DMI reclaim before EndOfDxe if neeeded

  @retval EFI_SUCCESS    Success
  @retval Others         Error occurs

**/
EFI_STATUS
EFIAPI
DmiReclaimBeforeEndOfDxe (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINTN                       DmiSize;
  UINTN                       RemainSize;
  UINTN                       ReclaimSize;
  UINT16                      DmiEnd;
  UINT8                       *Ptr;

  if (mSmmSmbiosEndOfDxe) {
    return EFI_UNSUPPORTED;
  }

  //
  // Restore DMI data from NV_FTW_SPARE to OEM_DMI_STORE if needed
  //
  Status = FtwSpareToDmiIfNeeded();
  if(!EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Perform DMI reclaim from OEM_DMI_STORE if needed
  //
  RemainSize = 0;
  ReclaimSize = 0;
  Status = GetDmiReclaimInfo (&RemainSize, &ReclaimSize);
  if (EFI_ERROR(Status) || ReclaimSize == 0) {
    return EFI_UNSUPPORTED;
  }
  DmiSize = (UINTN) FdmGetNAtSize(&gH2OFlashMapRegionSmbiosUpdateGuid, 1);
  if (DmiSize == 0) {
    return EFI_NOT_FOUND;
  }
  Ptr = (UINT8 *)(UINTN) FdmGetNAtAddr(&gH2OFlashMapRegionSmbiosUpdateGuid, 1);
  if (Ptr == NULL) {
    return EFI_NOT_FOUND;
  }
  Ptr += (DmiSize - sizeof (DMI_RECLAIM_STRING_SIGNATURE));
  if ((*(UINT32 *) Ptr == DMI_RECLAIM_STRING_SIGNATURE) || (RemainSize < DMI_RECLAIM_THRESHOLD)) {
    Status = DMISpaceNotEnough(&DmiEnd);
  }
  return Status;
}