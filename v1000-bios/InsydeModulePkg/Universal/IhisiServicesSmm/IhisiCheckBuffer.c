/** @file

  IHISI check buffer, Copy to SMRAM and restore from SMRAM related functions

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
#include "IhisiServicesSmm.h"
#include "IhisiCommonRegionCommunication.h"
#include "IhisiCheckBuffer.h"

BOOLEAN             mSharedBufferInSmRam;
H2O_IHISI_PARAMS    mOrgSharedBuffer;
H2O_IHISI_PARAMS    mOrgSharedBufferSize;


typedef
EFI_STATUS
(* IHISI_BUFFER_CHECK) (
  VOID
  );

typedef
EFI_STATUS
(* IHISI_STORE_TO_SMRAM) (
  VOID
  );

typedef struct _CMD_BUFFER_FUNCTIONS_MAP {
   IHISI_SMI               Cmd;
   IHISI_BUFFER_CHECK      BufferCheck;
   IHISI_STORE_TO_SMRAM    StoreToSmram;
} CMD_BUFFER_FUNCTIONS_MAP;




EFI_SMM_SAVE_STATE_REGISTER mRegisterParameters [] = {
                               EFI_SMM_SAVE_STATE_REGISTER_RAX,
                               EFI_SMM_SAVE_STATE_REGISTER_RBX,
                               EFI_SMM_SAVE_STATE_REGISTER_RCX,
                               EFI_SMM_SAVE_STATE_REGISTER_RDX,
                               EFI_SMM_SAVE_STATE_REGISTER_RSI,
                               EFI_SMM_SAVE_STATE_REGISTER_RDI,
                               EFI_SMM_SAVE_STATE_REGISTER_FS,
                               EFI_SMM_SAVE_STATE_REGISTER_GS
                              };


//
// Internal help funcitons to support XXXXBufferCheck functions
//

/**
  Internal function to check Whole variable name is located in command buffer

  @retval TRUE        Whole variable name is located in coomand buffer.
  @return FALSE       Any byte of the variable name in not located in command buffer.
**/
STATIC
BOOLEAN
VarNameInCmdBuffer (
  IN  CHAR16     *VarName
  )
{
  UINTN           Index;

  for (Index = 0; IhisiProtBufferInCmdBuffer ((VOID *) (VarName + Index), sizeof (CHAR16)); Index++) {
    if (VarName[Index] == 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Determine if two buffers overlap in memory.

  @param[in] Buff1  Pointer to first buffer
  @param[in] Size1  Size of Buff1
  @param[in] Buff2  Pointer to second buffer
  @param[in] Size2  Size of Buff2

  @retval TRUE      Buffers overlap in memory.
  @retval FALSE     Buffer doesn't overlap.

**/
STATIC
BOOLEAN
IsBufferOverlapped (
  IN UINT8      *Buff1,
  IN UINTN      Size1,
  IN UINT8      *Buff2,
  IN UINTN      Size2
  )
{
  //
  // If buff1's end is less than the start of buff2, then it's ok.
  // Also, if buff1's start is beyond buff2's end, then it's ok.
  //
  if (((Buff1 + Size1) <= Buff2) || (Buff1 >= (Buff2 + Size2))) {
    return FALSE;
  }

  return TRUE;
}

/**
  Vats security check.

  @return UINT32       IHISI status
**/
STATIC
EFI_STATUS
VatsSecurityCheckError (
  VOID
  )
{
  IHISI_VATS_OUTPUT_BUFFER              *OutBuffer;

  OutBuffer = (IHISI_VATS_OUTPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  if (!IhisiProtBufferInCmdBuffer ((VOID *) OutBuffer, sizeof (IHISI_VATS_OUTPUT_BUFFER))) {
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if ((OutBuffer->TableRev) != IHISI_TABLE_REVISION) {
    return IHISI_VATS_OB_TABLE_REV_UNSUPPORTED;
  }

  if (((OutBuffer->TableId1) != IHISI_H2O_SIGNATURE) ||
      ((OutBuffer->TableId2) != IHISI_VAR_SIGNATURE) ||
      ((OutBuffer->TableId3) != IHISI_TBL_SIGNATURE)) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}


//
// Start of XXXXBufferCheck functions
//

/**
  VATSRead check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
VATSReadBufferCheck (
  VOID
  )
{
  IHISI_VATS_INPUT_BUFFER                *InBuffer;
  IHISI_VATS_OUTPUT_BUFFER               *OutBuffer;
  UINT8                                  *VarData;
  CHAR16                                 *VarName;
  EFI_STATUS                             Status;
  UINTN                                  VarLength;
  UINTN                                  InBufferLen;
  UINTN                                  OutBufferLen;

  //
  // Check the valid signature "H2O$Var$Tbl" in obTableId filed.
  //
  Status = VatsSecurityCheckError ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // DS:ESI  Pointer to the beginning of the VATS input buffer.
  // DS:EDI  Pointer to the beginning of the VATS output buffer.
  //
  InBuffer  = (IHISI_VATS_INPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  OutBuffer = (IHISI_VATS_OUTPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  if (!IhisiProtBufferInCmdBuffer ((VOID *) InBuffer, sizeof (IHISI_VATS_INPUT_BUFFER)) ||
      !IhisiProtBufferInCmdBuffer ((VOID *) OutBuffer, sizeof (IHISI_VATS_OUTPUT_BUFFER))) {
    return  EFI_INVALID_PARAMETER;
  }

  VarName   = (CHAR16 *) ((UINTN) InBuffer + (sizeof (IHISI_VATS_INPUT_BUFFER) / sizeof (UINT8)));
  VarData   = (UINT8 *) ((UINTN) OutBuffer + (sizeof (IHISI_VATS_OUTPUT_BUFFER) / sizeof (UINT8)));
  VarLength = (UINTN) OutBuffer->VarLength;
  if (!VarNameInCmdBuffer (VarName) ||
      (VarLength != 0 && !IhisiProtBufferInCmdBuffer ((VOID *) VarData, VarLength))) {
    return  EFI_INVALID_PARAMETER;
  }

  InBufferLen = sizeof (IHISI_VATS_INPUT_BUFFER) + StrSize (VarName);
  OutBufferLen = sizeof (IHISI_VATS_OUTPUT_BUFFER) + VarLength;
  if (IsBufferOverlapped ((UINT8 *) InBuffer, InBufferLen, (UINT8 *) OutBuffer, OutBufferLen)) {
    return  EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  VATSWrite check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
VATSWriteBufferCheck (
  VOID
  )
{
  IHISI_VATS_INPUT_BUFFER                *InBuffer;
  IHISI_VATS_OUTPUT_BUFFER               *OutBuffer;
  UINT8                                  *VarData;
  CHAR16                                 *VarName;
  EFI_STATUS                             Status;
  UINTN                                  InBufferLen;
  UINTN                                  OutBufferLen;
  //
  // Check the valid signature "H2O$Var$Tbl" in obTableId filed.
  //
  Status = VatsSecurityCheckError ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // DS:ESI  Pointer to the beginning of the VATS input buffer.
  // DS:EDI  Pointer to the beginning of the VATS output buffer.
  //
  InBuffer = (IHISI_VATS_INPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  OutBuffer = (IHISI_VATS_OUTPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  if (!IhisiProtBufferInCmdBuffer ((VOID *) InBuffer, sizeof (IHISI_VATS_INPUT_BUFFER)) ||
      !IhisiProtBufferInCmdBuffer ((VOID *) OutBuffer, sizeof (IHISI_VATS_OUTPUT_BUFFER))) {
    return  EFI_INVALID_PARAMETER;
  }

  VarName = (CHAR16 *) ((UINTN) InBuffer + (sizeof (IHISI_VATS_INPUT_BUFFER) / sizeof (UINT8)));
  VarData = (UINT8 *) ((UINTN) OutBuffer + (sizeof (IHISI_VATS_OUTPUT_BUFFER) / sizeof (UINT8)));
  if (!VarNameInCmdBuffer (VarName) ||
      (OutBuffer->VarLength != 0 && !IhisiProtBufferInCmdBuffer ((VOID *) VarData, OutBuffer->VarLength))) {
    return  EFI_INVALID_PARAMETER;
  }

  InBufferLen = sizeof (IHISI_VATS_INPUT_BUFFER) + StrSize (VarName);
  OutBufferLen = sizeof (IHISI_VATS_OUTPUT_BUFFER) + OutBuffer->VarLength;
  if (IsBufferOverlapped ((UINT8 *) InBuffer, InBufferLen, (UINT8 *) OutBuffer, OutBufferLen)) {
    return  EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  VATSNext check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
VATSNextBufferCheck (
  VOID
  )
{
  IHISI_VATS_INPUT_BUFFER                *InBuffer;
  CHAR16                                 *VarName;
  UINTN                                  NameSize;

  //
  // DS:ESI  Pointer to the beginning of the VATS input buffer.
  // DS:ECX  Next Variable Name buffer size
  //
  InBuffer  = (IHISI_VATS_INPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  if (!IhisiProtBufferInCmdBuffer ((VOID *) InBuffer, sizeof (IHISI_VATS_INPUT_BUFFER))) {
    return  EFI_INVALID_PARAMETER;
  }
  NameSize  = (UINT32) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  VarName = (CHAR16 *) ((UINTN) InBuffer + (sizeof (IHISI_VATS_INPUT_BUFFER) / sizeof (UINT8)));

  if (!IhisiProtBufferInCmdBuffer ((VOID *) VarName, NameSize)) {
    return  EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  FBTSGetSupportVersion check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FBTSGetSupportVersionBufferCheck (
  VOID
  )
{
  FBTS_TOOLS_VERSION_BUFFER                 *VersionPtr;

  VersionPtr = (FBTS_TOOLS_VERSION_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  if (!IhisiProtBufferInCmdBuffer ((VOID *) VersionPtr, sizeof (FBTS_PLATFORM_STATUS_BUFFER))) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}


/**
  FBTSGetPlatformInfo check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FBTSGetPlatformInfoBufferCheck (
  VOID
  )
{
  FBTS_PLATFORM_INFO_BUFFER                *PlatformInfoPtr;
  FBTS_EXTEND_PLATFORM_INFO_TABLE_OUTPUT    *ExtendPlatformInfoOutput;
  FBTS_EXTEND_PLATFORM_INFO_TABLE_INPUT     *ExternPlatformInfoInput;

  PlatformInfoPtr = (FBTS_PLATFORM_INFO_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  //
  // Check input address and whole input buffer is located in command buffer.
  //
  if (!IhisiProtBufferInCmdBuffer ((VOID *) PlatformInfoPtr, sizeof (FBTS_PLATFORM_INFO_BUFFER))) {
    return EFI_INVALID_PARAMETER;
  }


  ExtendPlatformInfoOutput = (FBTS_EXTEND_PLATFORM_INFO_TABLE_OUTPUT *)(UINTN)IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  if (!IhisiProtBufferInCmdBuffer ((VOID *) ExtendPlatformInfoOutput, sizeof (FBTS_EXTEND_PLATFORM_INFO_TABLE_OUTPUT))){
    return EFI_INVALID_PARAMETER;
  }
  ExternPlatformInfoInput = (FBTS_EXTEND_PLATFORM_INFO_TABLE_INPUT *)(UINTN)IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  if (ExternPlatformInfoInput->Sigature != EXTEND_PLATFORM_INPUT_BUFFER_SIGNATURE) {
    //
    // Tool do not support Extend Platform Info, no change , return success.
    //
    return EFI_SUCCESS;
  }

  if (ExternPlatformInfoInput->StructureSize < sizeof (FBTS_EXTEND_PLATFORM_INFO_TABLE_INPUT)) {
    //
    // The structure size should bigger than Signature size
    //
    return EFI_INVALID_PARAMETER;
  }

  if (!IhisiProtBufferInCmdBuffer ((VOID *) ExternPlatformInfoInput, ExternPlatformInfoInput->StructureSize)){
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  FBTSGetPlatformRomMap check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FBTSGetPlatformRomMapBufferCheck (
  VOID
  )
{
  FBTS_PLATFORM_ROM_MAP_BUFFER             *InputRomMapBuffer;
  FBTS_PLATFORM_PRIVATE_ROM_BUFFER         *InputPrivateRomMapBuffer;
  //
  // Get ROM map protection structure.
  //
  InputRomMapBuffer = (FBTS_PLATFORM_ROM_MAP_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  //
  // Check input address and whole input buffer isn't located in SM RAM.
  //
  if (!IhisiProtBufferInCmdBuffer ((VOID *) InputRomMapBuffer, sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER))) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get ROM map protection structure.
  //
  InputPrivateRomMapBuffer = (FBTS_PLATFORM_PRIVATE_ROM_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  //
  // Check input address and whole input buffer isn't located in SM RAM.
  //
  if (!IhisiProtBufferInCmdBuffer ((VOID *) InputPrivateRomMapBuffer, sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER))) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  FBTSGetFlashPartInfo check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FBTSGetFlashPartInfoBufferCheck (
  VOID
  )
{
  UINT8                                     *FlashInfo;
  UINTN                                     FlashInfoSize;
  UINT8                                     *FlashBlockMap;

  //
  // DS:EDI - Pointer to flash part information structure.
  //
  FlashInfo     = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  FlashInfoSize = sizeof(FBTS_FLASH_DEVICE);
  //
  // Check output address and whole output bufferis located in command buffer.
  //
  if (!IhisiProtBufferInCmdBuffer ((VOID *) FlashInfo , FlashInfoSize)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // DS:ESI - Pointer to flash part block map structure.
  //
  FlashBlockMap     = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  //
  // Check output address and whole output buffer is located in command buffer.
  //
  if (!IhisiProtBufferInCmdBuffer ((VOID *) FlashBlockMap, sizeof (FD_BLOCK_MAP))) {
    return EFI_INVALID_PARAMETER;
  }


  return EFI_SUCCESS;
}

/**
  FBTSRead check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FBTSReadBufferCheck (
  VOID
  )
{
  UINTN                           Size;
  UINTN                           Offset;
  UINTN                           Address;
  UINT8                          *DataBuffer;

  //
  // ECX    - Size to read.
  // DS:ESI - Pointer to returned data buffer. Size in ECX.
  // EDI    - Target linear address to read.
  //
  DataBuffer = (UINT8 *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  Size       = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  Address    = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  Offset     = 0;
  if (!IhisiProtBufferInCmdBuffer ((VOID *) DataBuffer, Size) || IhisiProtBufferOverlapSmram ((VOID *) Address, Size)) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}


/**
  FBTSWrite check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FBTSWriteBufferCheck (
  VOID
  )
{
  UINTN                           WriteSize;
  UINTN                           RomBaseAddress;
  UINT8                          *WriteDataBuffer;

  //
  // ECX    - Size to write.
  // DS:ESI - Pointer to returned data buffer. Size in ECX.
  // EDI    - Target linear address to write.
  //
  WriteDataBuffer = (UINT8 *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  WriteSize       = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  RomBaseAddress  = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  if (!IhisiProtBufferInCmdBuffer ((VOID *) WriteDataBuffer, WriteSize) || IhisiProtBufferOverlapSmram ((VOID *) RomBaseAddress, WriteSize)) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  FBTSComplete check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FBTSCompleteBufferCheck (
  VOID
  )
{
  FBTS_FLASH_COMPLETE_STATUS   *FlashCompleteStatus;

  FlashCompleteStatus = (FBTS_FLASH_COMPLETE_STATUS *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  if (!IhisiProtBufferInCmdBuffer ((VOID *) FlashCompleteStatus, sizeof (FBTS_FLASH_COMPLETE_STATUS))) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  FBTSGetRomFileAndPlatformTable check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FBTSGetRomFileAndPlatformTableBufferCheck (
  VOID
  )
{
  UINT8           *OutputDataBuffer;
  UINT8           *InputDataBuffer;
  UINTN           DataSize;


  DataSize = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  OutputDataBuffer = (UINT8 *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  InputDataBuffer  = (UINT8 *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  if (!IhisiProtBufferInCmdBuffer ((VOID *) InputDataBuffer, DataSize) ||
      !IhisiProtBufferInCmdBuffer ((VOID *) OutputDataBuffer, DataSize)) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}


/**
  FBTSGetWholeBiosRomMap check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FBTSGetWholeBiosRomMapBufferCheck (
  VOID
  )
{
  UINTN                                 RomMapSize;
  UINTN                                 NumberOfRegions;
  UINT8                                *RomMapPtr;
  FBTS_INTERNAL_BIOS_ROM_MAP           *BiosRomMap;
  EFI_STATUS                           Status;

  Status = GetDefaultBiosRomMap ((FBTS_INTERNAL_BIOS_ROM_MAP **)&BiosRomMap, &NumberOfRegions);
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check input address and whole input buffer isn't located in SM RAM.
  //
  RomMapPtr  = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  RomMapSize = NumberOfRegions * sizeof (FBTS_INTERNAL_BIOS_ROM_MAP);
  if (!IhisiProtBufferInCmdBuffer ((VOID *) RomMapPtr, RomMapSize)) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  FETSWrite check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FETSWriteBufferCheck (
  VOID
  )
{
  UINTN               SizeToFlash;
  UINT8              *FlashingDataBuffer;

  //
  // Get flashing data from address stored in ESI
  //
  FlashingDataBuffer = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  //
  // Get the size to flash from EDI
  //
  SizeToFlash = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  if (!IhisiProtBufferInCmdBuffer ((VOID *) FlashingDataBuffer, SizeToFlash)) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}


/**
  OEMSFOEMExCommunication check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
OEMSFOEMExCommunicationBufferCheck (
  VOID
  )
{
  AP_COMMUNICATION_DATA_TABLE      *ApCommDataBuffer;
  UINTN                            BufferSize;

  ApCommDataBuffer = (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (!IhisiProtBufferInCmdBuffer ((VOID *) ApCommDataBuffer, sizeof(AP_COMMUNICATION_DATA_TABLE))) {
    return EFI_INVALID_PARAMETER;
  }

  BufferSize = ApCommDataBuffer->StructureSize;
  if (BufferSize < sizeof(BIOS_COMMUNICATION_DATA_TABLE) && BufferSize < sizeof(AP_COMMUNICATION_DATA_TABLE)) {
    return EFI_INVALID_PARAMETER;
  }
  if (ApCommDataBuffer->Signature == AP_COMMUNICATION_SIGNATURE) {
    return EFI_SUCCESS;
  } else if (ApCommDataBuffer->Signature == BIOS_COMMUNICATION_SIGNATURE) {
    if (BufferSize < sizeof(BIOS_COMMUNICATION_DATA_TABLE)) {
      BufferSize = sizeof(BIOS_COMMUNICATION_DATA_TABLE);
    }
    if (!IhisiProtBufferInCmdBuffer ((VOID *) ApCommDataBuffer, BufferSize)) {
      return EFI_INVALID_PARAMETER;
    }
  }
  return EFI_INVALID_PARAMETER;
}

/**
  OEMSFOEMExDataWrite check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
OEMSFOEMExDataWriteBufferCheck (
  VOID
  )
{
  UINTN               WriteSize;
  UINT8               *WriteDataBuffer;

  WriteDataBuffer = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  WriteSize       = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  if (!IhisiProtBufferInCmdBuffer ((VOID *) WriteDataBuffer, WriteSize)){
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  OEMSFOEMExDataRead check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
OEMSFOEMExDataReadBufferCheck (
  VOID
  )
{
  UINTN                            ReadSize;
  UINT8                            *ReadDataBuffer;

  ReadDataBuffer = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  if (!IhisiProtBufferInCmdBuffer ((VOID *)(UINTN)IhisiProtReadCpuReg32(EFI_SMM_SAVE_STATE_REGISTER_RDI), sizeof (UINT32))){
    return EFI_INVALID_PARAMETER;
  }
  ReadSize = *(UINT32 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  if (!IhisiProtBufferInCmdBuffer ((VOID *) ReadDataBuffer, ReadSize)){
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  FBTSCommonCommunication check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FBTSCommonCommunicationBufferCheck (
  VOID
  )
{
  INPUT_DATA_STRUCTURE          *InputData;
  OUTPUT_DATA_STRUCTURE         *OutputData;

  InputData  = (INPUT_DATA_STRUCTURE *) (UINTN) IhisiProtReadCpuReg32(EFI_SMM_SAVE_STATE_REGISTER_RCX);
  OutputData = (OUTPUT_DATA_STRUCTURE *) (UINTN) IhisiProtReadCpuReg32(EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (!IhisiProtBufferInCmdBuffer  ((VOID *) InputData, sizeof (INPUT_DATA_STRUCTURE)) ||
      !IhisiProtBufferInCmdBuffer  ((VOID *) OutputData, sizeof (OUTPUT_DATA_STRUCTURE))) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  FBTSCommonRead check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
FBTSCommonReadBufferCheck (
  VOID
  )
{
  UINT32                                  OutputBufferSize;
  UINT8                                  *OutputBuffer;

  if (!mCommonRegionReadControl.WantToReadData) {
    return EFI_SUCCESS;
  }
  OutputBuffer = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  if (!IhisiProtBufferInCmdBuffer ((VOID *)(UINTN)IhisiProtReadCpuReg32(EFI_SMM_SAVE_STATE_REGISTER_RDI), sizeof (UINT32))) {
    return EFI_INVALID_PARAMETER;
  }
  CopyMem (&OutputBufferSize, (VOID *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI), sizeof (UINT32));
  if (!IhisiProtBufferInCmdBuffer  ((VOID *) OutputBuffer, OutputBufferSize)) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
  IhisiAuth check buffer function.

  @return EFI_SUCCESS             All of buffer is secure.
  @return EFI_INVALID_PARAMETER   At least one data in buffer is insecure.
**/
STATIC
EFI_STATUS
IhisiAuthBufferCheck (
  VOID
  )
{
  IHISI_AUTH_LOCK                   *CredentialData ;

  CredentialData = (IHISI_AUTH_LOCK *)(UINTN)IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  if (CredentialData != NULL &&
      (!IhisiProtBufferInCmdBuffer ((VOID *) CredentialData, sizeof (IHISI_AUTH_LOCK)) ||
      !IhisiProtBufferInCmdBuffer ((VOID *) CredentialData, CredentialData->Size + sizeof (IHISI_AUTH_LOCK)))) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

//
// Internal help funcitons to support XXXXStoreBufferToSmram functions
//

/**
  Helper function to set value to IHISI parameter.

  @param[in] Register             Register value.
  @param[in] Value                Value to be set to IHISI parameter.
  @param[in] IhisiParas           Pointer to EFI_SMM_SAVE_STATE_REGISTER instance..

  @retval EFI_SUCCESS             Store value to IHISI parameter successfully.
  @retval EFI_INVALID_PARAMETER   IhisiParas is NULL.
  @retval EFI_NOT_FOUND           Input Register value is unsupported.
**/
STATIC
EFI_STATUS
SetValueToIhisiParameter (
  IN    EFI_SMM_SAVE_STATE_REGISTER       Register,
  IN    UINT64                            Value,
  OUT   H2O_IHISI_PARAMS                  *IhisiParas
  )
{
  UINTN       Index;
  UINT64      *Parameter;

  if (IhisiParas == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Parameter = &IhisiParas->Param1;
  for (Index = 0; Index < TOTAL_PARAMETERS; Index++) {
    if (mRegisterParameters[Index] == Register) {
      *(Parameter + Index) = Value;
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

/**
  Helper function to store data buffer to SMRAM and return SMRAM address.
  Caller is responsible freeing the buffer.

  @param[in] Register             Register value.
  @param[in] DataBuffer           DataBuffer to be stored to SMRAM.
  @param[in] DataSize             Size of data buffer in bytes.

  @return the start address of SMRAM or NULL if unable to allocate SMRAM.
**/
STATIC
VOID *
StoreBufferToSmram (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  VOID                              *DataBuffer,
  IN  UINTN                             DataSize
  )
{
  VOID    *SmmBuffer;

  if (DataBuffer == NULL) {
    return NULL;
  }

  SmmBuffer = AllocatePool (DataSize);
  if (SmmBuffer == NULL) {
    return NULL;
  }
  CopyMem (SmmBuffer, DataBuffer, DataSize);
  SetValueToIhisiParameter (RegisterNum, (UINT64)(UINTN)DataBuffer, &mOrgSharedBuffer);
  SetValueToIhisiParameter (RegisterNum, (UINT64)DataSize, &mOrgSharedBufferSize);
  IhisiProtWriteCpuReg32 (RegisterNum, (UINT32)(UINTN)SmmBuffer);
  return SmmBuffer;
}

//
// Start of XXXXStoreBufferToSmram functions
//

/**
  VATSRead Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
VATSReadStoreBufferToSmram (
  VOID
  )
{
  IHISI_VATS_INPUT_BUFFER                *InBuffer;
  IHISI_VATS_OUTPUT_BUFFER               *OutBuffer;
  CHAR16                                 *VarName;
  UINTN                                  InBufferLen;
  UINTN                                  OutBufferLen;
  IHISI_VATS_INPUT_BUFFER                *InBufferInSmm;
  IHISI_VATS_OUTPUT_BUFFER               *OutBufferInSmm;

  InBuffer     = (IHISI_VATS_INPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  OutBuffer    = (IHISI_VATS_OUTPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  VarName      = (CHAR16 *) ((UINTN) InBuffer + (sizeof (IHISI_VATS_INPUT_BUFFER) / sizeof (UINT8)));
  InBufferLen  = sizeof (IHISI_VATS_INPUT_BUFFER) + StrSize (VarName);
  OutBufferLen = sizeof (IHISI_VATS_OUTPUT_BUFFER) + (UINTN) OutBuffer->VarLength;

  InBufferInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, InBuffer, InBufferLen);
  if (InBufferInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  OutBufferInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RDI, OutBuffer, OutBufferLen);
  if (OutBufferInSmm == NULL) {
    FreePool (InBufferInSmm);
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}

/**
  VATSWrite Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
VATSWriteStoreBufferToSmram (
  VOID
  )
{
  IHISI_VATS_INPUT_BUFFER                *InBuffer;
  IHISI_VATS_OUTPUT_BUFFER               *OutBuffer;
  CHAR16                                 *VarName;
  UINTN                                  InBufferLen;
  UINTN                                  OutBufferLen;
  IHISI_VATS_INPUT_BUFFER                *InBufferInSmm;
  IHISI_VATS_OUTPUT_BUFFER               *OutBufferInSmm;


  //
  // DS:ESI  Pointer to the beginning of the VATS input buffer.
  // DS:EDI  Pointer to the beginning of the VATS output buffer.
  //
  InBuffer     = (IHISI_VATS_INPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  OutBuffer    = (IHISI_VATS_OUTPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  VarName      = (CHAR16 *) ((UINTN) InBuffer + (sizeof (IHISI_VATS_INPUT_BUFFER) / sizeof (UINT8)));
  InBufferLen  = sizeof (IHISI_VATS_INPUT_BUFFER) + StrSize (VarName);
  OutBufferLen = sizeof (IHISI_VATS_OUTPUT_BUFFER) + (UINTN) OutBuffer->VarLength;


  InBufferInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, InBuffer, InBufferLen);
  if (InBufferInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  OutBufferInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RDI, OutBuffer, OutBufferLen);
  if (OutBufferInSmm == NULL) {
    FreePool (InBufferInSmm);
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}


/**
  VATSNext Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
VATSNextStoreBufferToSmram (
  VOID
  )
{
  IHISI_VATS_INPUT_BUFFER                *InBuffer;
  UINTN                                  InBufferLen;
  IHISI_VATS_INPUT_BUFFER                *InBufferInSmram;
  CHAR16                                 *VarName;
  UINTN                                  NameSize;

  //
  // DS:ESI  Pointer to the beginning of the VATS input buffer.
  // DS:ECX  Next Variable Name buffer size
  //
  InBuffer  = (IHISI_VATS_INPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  NameSize  = (UINT32) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  InBufferLen = sizeof (IHISI_VATS_INPUT_BUFFER) + NameSize;
  VarName = (CHAR16 *) ((UINTN) InBuffer + (sizeof (IHISI_VATS_INPUT_BUFFER) / sizeof (UINT8)));

  InBufferInSmram = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, InBuffer, InBufferLen);
  if (InBufferInSmram == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}

/**
  FBTSGetSupportVersion Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FBTSGetSupportVersionStoreBufferToSmram (
  VOID
  )
{
  FBTS_PLATFORM_STATUS_BUFFER                  *PlatformStatus;
  FBTS_PLATFORM_STATUS_BUFFER                  *PlatformStatusInSmm;
  UINTN                                        PlatformStatusSize;

  PlatformStatus         = (FBTS_PLATFORM_STATUS_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  PlatformStatusSize      = sizeof (FBTS_PLATFORM_STATUS_BUFFER);

  PlatformStatusInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RDI, PlatformStatus, PlatformStatusSize);
  if (PlatformStatusInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}

/**
  FBTSGetPlatformInfo Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FBTSGetPlatformInfoStoreBufferToSmram (
  VOID
  )
{
  FBTS_PLATFORM_INFO_BUFFER                      *PlatformInfo;
  FBTS_EXTEND_PLATFORM_INFO_TABLE_OUTPUT         *ExtendPlatformInfo;
  FBTS_PLATFORM_INFO_BUFFER                      *PlatformInfoInSmm;
  FBTS_EXTEND_PLATFORM_INFO_TABLE_OUTPUT         *ExtendPlatformInfoInSmm;
  UINTN                                          PlatformInfoSize;
  UINTN                                          ExtendPlatformInfoSize;
  FBTS_EXTEND_PLATFORM_INFO_TABLE_INPUT         *ExternPlatformInfoInput;

  ExtendPlatformInfo      = (FBTS_EXTEND_PLATFORM_INFO_TABLE_OUTPUT *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  ExtendPlatformInfoSize  = sizeof (FBTS_EXTEND_PLATFORM_INFO_TABLE_OUTPUT);
  ExternPlatformInfoInput = (FBTS_EXTEND_PLATFORM_INFO_TABLE_INPUT *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  if (ExternPlatformInfoInput->Sigature == EXTEND_PLATFORM_INPUT_BUFFER_SIGNATURE) {
    if (ExtendPlatformInfoSize < ExternPlatformInfoInput->StructureSize) {
      ExtendPlatformInfoSize = ExternPlatformInfoInput->StructureSize;
    }
  }
  PlatformInfo     = (FBTS_PLATFORM_INFO_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  PlatformInfoSize = sizeof (FBTS_PLATFORM_INFO_BUFFER);


  ExtendPlatformInfoInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, ExtendPlatformInfo, ExtendPlatformInfoSize);
  if (ExtendPlatformInfoInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  PlatformInfoInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RDI, PlatformInfo, PlatformInfoSize);
  if (PlatformInfoInSmm == NULL) {
     FreePool (ExtendPlatformInfoInSmm);
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}

/**
  FBTSGetPlatformRomMap Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FBTSGetPlatformRomMapStoreBufferToSmram (
  VOID
  )
{
  FBTS_PLATFORM_ROM_MAP_BUFFER             *InputRomMapBuffer;
  FBTS_PLATFORM_PRIVATE_ROM_BUFFER         *InputPrivateRomMapBuffer;
  FBTS_PLATFORM_ROM_MAP_BUFFER             *InputRomMapBufferInSmm;
  FBTS_PLATFORM_PRIVATE_ROM_BUFFER         *InputPrivateRomMapBufferInSmm;
  UINTN                                    InputRomMapBufferSize;
  UINTN                                    InputPrivateRomMapBufferSize;

  InputPrivateRomMapBuffer     = (FBTS_PLATFORM_PRIVATE_ROM_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  InputPrivateRomMapBufferSize = sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER);
  InputRomMapBuffer            = (FBTS_PLATFORM_ROM_MAP_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  InputRomMapBufferSize        = sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER);

  InputPrivateRomMapBufferInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, InputPrivateRomMapBuffer, InputPrivateRomMapBufferSize);
  if (InputPrivateRomMapBufferInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  InputRomMapBufferInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RDI, InputRomMapBuffer, InputRomMapBufferSize);
  if (InputRomMapBufferInSmm == NULL) {
     FreePool (InputPrivateRomMapBufferInSmm);
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}

/**
  FBTSGetFlashPartInfo Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FBTSGetFlashPartInfoStoreBufferToSmram (
  VOID
  )
{
  UINT8                                     *FlashInfo;
  UINT8                                     *FlashBlockMap;
  UINT8                                     *FlashInfoInSmm;
  UINT8                                     *FlashBlockMapInSmm;
  UINTN                                     FlashInfoSize;
  UINTN                                     FlashBlockMapSize;
  //
  // DS:EDI - Pointer to flash part information structure.
  //
  FlashBlockMap     = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  FlashBlockMapSize = sizeof (FD_BLOCK_MAP);
  FlashInfo         = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  FlashInfoSize     = sizeof(FBTS_FLASH_DEVICE);


  FlashBlockMapInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, FlashBlockMap, FlashBlockMapSize);
  if (FlashBlockMapInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  FlashInfoInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RDI, FlashInfo, FlashInfoSize);
  if (FlashInfoInSmm == NULL) {
     FreePool (FlashBlockMapInSmm);
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}


/**
  FBTSRead Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FBTSReadStoreBufferToSmram (
  VOID
  )
{
  UINTN                           Size;
  UINT8                          *DataBuffer;
  UINT8                          *DataBufferInSmm;

  DataBuffer = (UINT8 *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  Size       = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  DataBufferInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, DataBuffer, Size);
  if (DataBufferInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}

/**
  FBTSWrite Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FBTSWriteStoreBufferToSmram (
  VOID
  )
{
  UINTN                           Size;
  UINT8                          *DataBuffer;
  UINT8                          *DataBufferInSmm;

  DataBuffer = (UINT8 *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  Size       = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);


  DataBufferInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, DataBuffer, Size);
  if (DataBufferInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}


/**
  FBTSComplete Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FBTSCompleteStoreBufferToSmram (
  VOID
  )
{
  FBTS_FLASH_COMPLETE_STATUS   *CompleteStatus;
  FBTS_FLASH_COMPLETE_STATUS   *CompleteStatusInSmm;
  UINTN                        CompleteStatusSize;

  CompleteStatus     = (FBTS_FLASH_COMPLETE_STATUS *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  CompleteStatusSize = sizeof (FBTS_FLASH_COMPLETE_STATUS);

  CompleteStatusInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, CompleteStatus, CompleteStatusSize);
  if (CompleteStatusInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}


/**
  FBTSGetRomFileAndPlatformTable Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FBTSGetRomFileAndPlatformTableBufferToSmram (
  VOID
  )
{
  //
  // The code is handled by chipset code and the output buffer size doesn't define very clear.
  // Therefore, FBTSGetRomFileAndPlatformTable doesn't store memory to SMRAM.
  //
  return EFI_UNSUPPORTED;
}

/**
  FBTSGetWholeBiosRomMap Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FBTSGetWholeBiosRomMapStoreBufferToSmram (
  VOID
  )
{
  //
  // FBTSGetWholeBiosRomMap funciton doesn't provide output buffer size
  // Therefore, FBTSGetRomFileAndPlatformTable doesn't store memory to SMRAM.
  //
  return EFI_UNSUPPORTED;
}


/**
  FETSWrite Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FETSWriteStoreBufferToSmram (
  VOID
  )
{
  UINTN               FlashingDataSize;
  UINT8              *FlashingData;
  UINT8              *FlashingDataInSmm;

  //
  // Get flashing data from address stored in ESI
  //
  FlashingData = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  FlashingDataSize = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  FlashingDataInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, FlashingData, FlashingDataSize);
  if (FlashingDataInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}


/**
  OEMSFOEMExCommunication Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
OEMSFOEMExCommunicationStoreBufferToSmram (
  VOID
  )
{
   //
   // Only has output buffer and need NOT copy output buffer to SMRAM
   // because the data will be copied from SMRAM.
   //
  return EFI_UNSUPPORTED;
}


/**
  OEMSFOEMExDataWrite Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
OEMSFOEMExDataWriteStoreBufferToSmram (
  VOID
  )
{
  UINTN               WriteDataSize;
  UINT8               *WriteData;
  UINT8               *WriteDataInSmm;

  WriteData       = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  WriteDataSize   = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  WriteDataInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, WriteData, WriteDataSize);
  if (WriteDataInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}


/**
  OEMSFOEMExDataRead Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
OEMSFOEMExDataReadStoreBufferToSmram (
  VOID
  )
{
  UINTN                            ReadDataSize;
  UINT8                            *ReadData;
  UINT8                            *ReadDataInSmm;

  ReadData = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  ReadDataSize = *(UINT32 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  ReadDataInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, ReadData, ReadDataSize);
  if (ReadDataInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}

/**
  FBTSCommonCommunication Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FBTSCommonCommunicationStoreBufferToSmram (
  VOID
  )
{
  UINT8         *Data;
  UINT8         *DataInSmm;
  UINTN         DataSize;

  Data  = (UINT8 *)(UINTN)IhisiProtReadCpuReg32(EFI_SMM_SAVE_STATE_REGISTER_RCX);
  DataSize = sizeof (INPUT_DATA_STRUCTURE);
  if (DataSize < sizeof (OUTPUT_DATA_STRUCTURE)) {
    DataSize = sizeof (OUTPUT_DATA_STRUCTURE);
  }
  DataInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RCX, Data, DataSize);
  if (DataInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;

}


/**
  FBTSCommonRead Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
FBTSCommonReadStoreBufferToSmram (
  VOID
  )
{
  UINT32                                  OutputBufferSize;
  UINT8                                  *OutputBuffer;
  UINT8                                  *OutputBufferInSmm;

  if (!mCommonRegionReadControl.WantToReadData) {
    return EFI_UNSUPPORTED;
  }
  OutputBuffer = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  CopyMem (&OutputBufferSize, (VOID *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI), sizeof (UINT32));

  OutputBufferInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, OutputBuffer, OutputBufferSize);
  if (OutputBufferInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}


/**
  IhisiAuth Store buffer to SMRAM function.

  @return EFI_SUCCESS             Store all shared buffer to SMRAM successfully.
  @return EFI_OUT_OF_RESOURCES    Unable allocate SMRAM to store shared buffer from SMRAM.
**/
STATIC
EFI_STATUS
IhisiAuthStoreBufferToSmram (
  VOID
  )
{
  IHISI_AUTH_LOCK                   *CredentialData;
  IHISI_AUTH_LOCK                   *CredentialDataInSmm;
  UINTN                             CredentialDataSize;

  CredentialData = (IHISI_AUTH_LOCK *)(UINTN)IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  if (CredentialData == NULL) {
    return EFI_UNSUPPORTED;
  }
  CredentialDataSize = CredentialData->Size + sizeof (IHISI_AUTH_LOCK);

  CredentialDataInSmm = StoreBufferToSmram (EFI_SMM_SAVE_STATE_REGISTER_RSI, CredentialData, CredentialDataSize);
  if (CredentialDataInSmm == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}


CMD_BUFFER_FUNCTIONS_MAP  mCmdbufferFunctions[] = {
    {VATSRead                       , VATSReadBufferCheck                      , VATSReadStoreBufferToSmram                   }, // 0x00
    {VATSWrite                      , VATSWriteBufferCheck                     , VATSWriteStoreBufferToSmram                  }, // 0x01
    {VATSNext                       , VATSNextBufferCheck                      , VATSNextStoreBufferToSmram                   }, // 0x05
    {FBTSGetSupportVersion          , FBTSGetSupportVersionBufferCheck         , FBTSGetSupportVersionStoreBufferToSmram      }, // 0x10
    {FBTSGetPlatformInfo            , FBTSGetPlatformInfoBufferCheck           , FBTSGetPlatformInfoStoreBufferToSmram        }, // 0x11
    {FBTSGetPlatformRomMap          , FBTSGetPlatformRomMapBufferCheck         , FBTSGetPlatformRomMapStoreBufferToSmram      }, // 0x12
    {FBTSGetFlashPartInfo           , FBTSGetFlashPartInfoBufferCheck          , FBTSGetFlashPartInfoStoreBufferToSmram       }, // 0x13
    {FBTSRead                       , FBTSReadBufferCheck                      , FBTSReadStoreBufferToSmram                   }, // 0x14
    {FBTSWrite                      , FBTSWriteBufferCheck                     , FBTSWriteStoreBufferToSmram                  }, // 0x15
    {FBTSComplete                   , FBTSCompleteBufferCheck                  , FBTSCompleteStoreBufferToSmram               }, // 0x16
    {FBTSGetRomFileAndPlatformTable , FBTSGetRomFileAndPlatformTableBufferCheck, FBTSGetRomFileAndPlatformTableBufferToSmram  }, // 0x17
    {FBTSGetWholeBiosRomMap         , FBTSGetWholeBiosRomMapBufferCheck        , FBTSGetWholeBiosRomMapStoreBufferToSmram     }, // 0x1E
    {FETSWrite                      , FETSWriteBufferCheck                     , FETSWriteStoreBufferToSmram                  }, // 0x20
    {OEMSFOEMExCommunication        , OEMSFOEMExCommunicationBufferCheck       , OEMSFOEMExCommunicationStoreBufferToSmram    }, // 0x41
    {OEMSFOEMExDataWrite            , OEMSFOEMExDataWriteBufferCheck           , OEMSFOEMExDataWriteStoreBufferToSmram        }, // 0x42
    {OEMSFOEMExDataRead             , OEMSFOEMExDataReadBufferCheck            , OEMSFOEMExDataReadStoreBufferToSmram         }, // 0x47
    {FBTSCommonCommunication        , FBTSCommonCommunicationBufferCheck       , FBTSCommonCommunicationStoreBufferToSmram    }, // 0x49
    {FBTSCommonRead                 , FBTSCommonReadBufferCheck                , FBTSCommonReadStoreBufferToSmram             }, // 0x4B
    {IhisiAuth                      , IhisiAuthBufferCheck                     , IhisiAuthStoreBufferToSmram                  }  // 0x84
  };

/**
  According to input IHISI command to check shared buffer is whether valid.

  @param[in] Cmd                Input IHISI SMI function command.

  @retval EFI_SUCCESS            All of shared data buffer is valid.
  @retval EFI_INVALID_PARAMETER  Any of shared buffer is invalid.
**/
EFI_STATUS
CheckSharedBuffer (
  IHISI_SMI      Cmd
  )
{
  UINTN         Index;
  UINTN         TotalCnt;
  EFI_STATUS    Status;

  Status = EFI_SUCCESS;
  TotalCnt = sizeof (mCmdbufferFunctions) / sizeof (CMD_BUFFER_FUNCTIONS_MAP);
  for (Index = 0; Index < TotalCnt; Index++) {
    if (mCmdbufferFunctions[Index].Cmd == Cmd) {
      Status = mCmdbufferFunctions[Index].BufferCheck ();
      break;
    }
  }
  return Status;
}

/**
  According to input IHISI command to store shared buffer to SMRAM.

  @param[in] Cmd                 Input IHISI SMI function command.

  @retval EFI_SUCCESS            Copy shared buffer to SMRAM successfully.
  @retval EFI_OUT_OF_RESOURCES   Unable allocate SMRAM to store share buffer.
**/
EFI_STATUS
StoreSharedBufferToSmram (
  IHISI_SMI      Cmd
  )
{
  UINTN         Index;
  UINTN         TotalCnt;
  EFI_STATUS    Status;

  if (mSharedBufferInSmRam) {
    return EFI_SUCCESS;
  }
  ZeroMem (&mOrgSharedBuffer,     sizeof (H2O_IHISI_PARAMS));
  ZeroMem (&mOrgSharedBufferSize, sizeof (H2O_IHISI_PARAMS));

  Status = EFI_UNSUPPORTED;
  TotalCnt = sizeof (mCmdbufferFunctions) / sizeof (CMD_BUFFER_FUNCTIONS_MAP);
  for (Index = 0; Index < TotalCnt; Index++) {
    if (mCmdbufferFunctions[Index].Cmd == Cmd) {
      Status = mCmdbufferFunctions[Index].StoreToSmram ();
      break;
    }
  }
  if (!EFI_ERROR (Status)) {
    mSharedBufferInSmRam = TRUE;
  }
  if (Status != EFI_OUT_OF_RESOURCES) {
    Status = EFI_SUCCESS;
  }
  return Status;
}

/**
  Restored data buffer from SMRAM to original buffer and free SMRAM.

  @retval EFI_SUCCESS            Restore all of the data SMRAM successfully.
  @retval EFI_ALREADY_STARTED    No any data in SMRAM.
**/
EFI_STATUS
RestoreSharedBufferFromSmram (
  VOID
  )
{
  UINTN      Index;
  UINT64     *Parameter;
  UINT64     *ParameterSize;
  UINT64     CurrentAddress;

  if (!mSharedBufferInSmRam) {
    return EFI_ALREADY_STARTED;
  }
  Parameter      = &mOrgSharedBuffer.Param1;
  ParameterSize  = &mOrgSharedBufferSize.Param1;
  for (Index = 0; Index < TOTAL_PARAMETERS; Index++) {
    if (*(Parameter + Index) != 0) {
      CurrentAddress = (UINT64)IhisiProtReadCpuReg32 (mRegisterParameters[Index]);
      CopyMem (
        (VOID *)(UINTN)(*(Parameter + Index)),
        (VOID *)(UINTN)CurrentAddress,
        (UINTN)(*(ParameterSize + Index))
        );
      FreePool ((VOID *)(UINTN)CurrentAddress);
      IhisiProtWriteCpuReg32 (mRegisterParameters[Index], (UINT32)(UINTN)(*(Parameter + Index)));
    }
  }

  mSharedBufferInSmRam = FALSE;
  return EFI_SUCCESS;
}