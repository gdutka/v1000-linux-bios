/** @file
  This driver provides IHISI interface in SMM mode

;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "IhisiFets.h"
#include <Library/SmmChipsetSvcLib.h>
#include <Library/SmmOemSvcKernelLib.h>

#define     EC_PART_SIZE     EC256K
#define     MAX_IHISI_EC_PART_SIZE EC2048K

STATIC
IHISI_REGISTER_TABLE
FETS_REGISTER_TABLE[] = {
  //
  // AH=20h
  //
  { FETSWrite, "S20OemDoBeforeWrite", OemFetsDoBeforeWrite        }, \
  { FETSWrite, "S20OemEcIdleTrue000", OemFetsEcIdleTrue           }, \
  { FETSWrite, "S20OemFetsWrite0000", OemFetsWrite                }, \
  { FETSWrite, "S20OemEcIdleFalse00", OemFetsEcIdleFalse          }, \
  { FETSWrite, "S20OemDoAfterWrite0", OemFetsDoAfterWrite         }, \
  { FETSWrite, "S20Cs_ShutdownMode0", ChipsetFetsWriteShutdownMode}, \

  //
  // AH=21h
  //
  { FETSGetEcPartInfo, "S21OemGetEcPartInfo", OemGetEcPartInfo}
};
/**
  AH=20h,  FetsWrite before process, hook SmmCsSvcIhisiFetsDoBeforeFlashing.

  @retval EFI_SUCCESS    Flash EC successful.
  @return Other          Flash EC failed.
**/
EFI_STATUS
EFIAPI
OemFetsDoBeforeWrite (
  VOID
  )
{
  EFI_STATUS          Status;
  UINTN               SizeToFlash;
  UINT8               DestBlockNo;
  UINT8               ActionAfterFlashing;
  UINT8              *FlashingDataBuffer;
  UINT32              Ecx;

  Status              = EFI_SUCCESS;
  FlashingDataBuffer  = NULL;
  SizeToFlash         = 0;
  DestBlockNo         = 0;
  ActionAfterFlashing = 0;

  //
  // Get flashing data from address stored in ESI
  //
  FlashingDataBuffer = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  //
  // Get the size to flash from EDI
  //
  SizeToFlash = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);


  //
  // Get destined block number from CH
  //
  DestBlockNo = (UINT8) (IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX) >> 8);

  if ((SizeToFlash == 0) || (FlashingDataBuffer == NULL)) {
    return IHISI_FBTS_READ_FAILED;
  }

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS20HookFetsDoBeforeFlashing \n"));
  Status = OemSvcIhisiS20HookFetsDoBeforeFlashing (&FlashingDataBuffer, &SizeToFlash, &DestBlockNo);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS20HookFetsDoBeforeFlashing Status: %r\n", Status));

  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI, (UINT32)SizeToFlash);
  Ecx = IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX) & 0xFFFF00FF;
  Ecx |= (UINT32)DestBlockNo << 8;
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX, (UINT32)Ecx);

  return IHISI_SUCCESS;
}

/**
  AH=20h,  FetsWrite Ec idle true, hook SmmCsSvcIhisiFetsEcIdle.

  @retval EFI_SUCCESS    Flash EC successful.
  @return Other          Flash EC failed.
**/
EFI_STATUS
EFIAPI
OemFetsEcIdleTrue (
  VOID
  )
{
  EFI_STATUS    Status;

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS20HookFetsEcIdle \n"));
  Status = OemSvcIhisiS20HookFetsEcIdle (TRUE);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS20HookFetsEcIdle Status: %r\n", Status));
  return EFI_SUCCESS;
}

/**
  AH=20h, FetsWrite, Flash EC.

  @retval EFI_SUCCESS    Flash EC successful.
  @return Other          Flash EC failed.
**/
EFI_STATUS
EFIAPI
OemFetsWrite (
  VOID
  )
{
  EFI_STATUS          Status;
  UINTN               SizeToFlash;
  UINT8               DestBlockNo;
  UINT8               ActionAfterFlashing;
  UINT8              *FlashingDataBuffer;

  Status              = EFI_SUCCESS;
  FlashingDataBuffer  = NULL;
  SizeToFlash         = 0;
  DestBlockNo         = 0;
  ActionAfterFlashing = 0;

  //
  // Get flashing data from address stored in ESI
  //
  FlashingDataBuffer = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  //
  // Get the size to flash from EDI
  //
  SizeToFlash = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  //
  // Get destined block number from CH
  //
  DestBlockNo = (UINT8) (IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX) >> 8);

  //
  // Get action after flashing from CL
  //
  ActionAfterFlashing = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  if ((SizeToFlash == 0) || (FlashingDataBuffer == NULL)) {
    return IHISI_FBTS_READ_FAILED;
  }

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS20HookFetsEcFlash \n"));
  Status = OemSvcIhisiS20HookFetsEcFlash  (FlashingDataBuffer, SizeToFlash, DestBlockNo);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS20HookFetsEcFlash Status: %r\n", Status));

  return IHISI_SUCCESS;
}

/**
  AH=20h,  FetsWrite Ec idle false, hook SmmCsSvcIhisiFetsEcIdle.

  @retval EFI_SUCCESS    Flash EC successful.
  @return Other          Flash EC failed.
**/
EFI_STATUS
EFIAPI
OemFetsEcIdleFalse (
  VOID
  )
{
  EFI_STATUS    Status;

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS20HookFetsEcIdle \n"));
  Status = OemSvcIhisiS20HookFetsEcIdle (FALSE);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS20HookFetsEcIdle Status: %r\n", Status));

  return IHISI_SUCCESS;
}


/**
  AH=20h,  FetsWrite after process, hook SmmCsSvcIhisiFetsDoAfterFlashing.

  @retval EFI_SUCCESS    Flash EC successful.
  @return Other          Flash EC failed.
**/
EFI_STATUS
EFIAPI
OemFetsDoAfterWrite (
  VOID
  )
{
  EFI_STATUS          Status;
  UINT8               ActionAfterFlashing;

  ActionAfterFlashing = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS20HookFetsDoAfterFlashing \n"));
  Status = OemSvcIhisiS20HookFetsDoAfterFlashing (ActionAfterFlashing);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS20HookFetsDoAfterFlashing Status: %r\n", Status));
  if (Status == EFI_SUCCESS) {
    return IHISI_END_FUNCTION_CHAIN;
  }

  return IHISI_SUCCESS;
}

/**
  AH=20h,  FetsWrite Dos shutown, hook SmmCsSvcIhisiFetsShutdown/SmmCsSvcIhisiFetsReboot.

  @retval EFI_SUCCESS    Flash EC successful.
  @return Other          Flash EC failed.
**/
EFI_STATUS
EFIAPI
ChipsetFetsWriteShutdownMode (
  VOID
  )
{
  UINT8               ActionAfterFlashing;

  ActionAfterFlashing = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  switch (ActionAfterFlashing) {
    case EcFlashDosReboot:
      SmmCsSvcIhisiFetsReboot ();
      break;

    case EcFlashDoshutdown:
      SmmCsSvcIhisiFetsShutdown ();
      break;

    case EcFlashOSShutdown:
    case EcFlashOSReboot:
    case EcFlashDoNothing:
    case EcFlashContinueToFlash:
    default:
      break;
  }

  return IHISI_SUCCESS;
}

/**
  AH=21h, Get EC part information.hook SmmCsSvcIhisiFetsGetEcPartInfo.

  @retval EFI_SUCCESS     Get EC part information successful.
**/
EFI_STATUS
EFIAPI
OemGetEcPartInfo (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT32                                Ecx;
  UINT32                                EcPartSize;
  FETS_EC_PART_SIZE_STRUCTURE_INPUT     *EcPartSizeTable;
  UINT32                                IhisiStatus;

  IhisiStatus = IHISI_SUCCESS;
  EcPartSize  = EC_PART_SIZE;

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS21HookFetsGetPartInfo \n"));
  Status = OemSvcIhisiS21HookFetsGetPartInfo (&IhisiStatus, &EcPartSize);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS21HookFetsGetPartInfo Status: %r\n", Status));

  //
  // When the total count of EC image size is not defined in CH, it need to report the real size in ESI.
  //
  if (Status == EFI_MEDIA_CHANGED &&  EcPartSize > MAX_IHISI_EC_PART_SIZE) {
    EcPartSizeTable = (FETS_EC_PART_SIZE_STRUCTURE_INPUT *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

    if (!IhisiProtBufferInCmdBuffer ((VOID *) EcPartSizeTable, sizeof(FETS_EC_PART_SIZE_STRUCTURE_INPUT))) {
      return IHISI_BUFFER_RANGE_ERROR;
    }
    //
    //BIOS must make sure the input buffer signature is "$BUFFER_", then can use this buffer to return data.
    //If the signature is incorrect, that means the buffer does not be prepared by AP.
    //When BIOS need to use this buffer but the signatureis incorrect, then BIOS can return IHISI_OB_LEN_TOO_SMALL in AL.
    //
    if (EcPartSizeTable->Signature != EC_PARTSIZE_SIGNATURE) {
      return IHISI_OB_LEN_TOO_SMALL;
    }

    EcPartSizeTable->Size = EcPartSize;
    EcPartSize = REPORT_FROM_ESI;
  }

  Ecx = IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX) & 0xFFFFFF00;
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX, Ecx|EcPartSize);

  return (EFI_STATUS)IhisiStatus;
}

EFI_STATUS
InstallFetsServices (
  VOID
  )
{
  EFI_STATUS              Status;
  IHISI_REGISTER_TABLE   *SubFuncTable;
  UINT16                  TableCount;

  SubFuncTable = FETS_REGISTER_TABLE;
  TableCount = sizeof(FETS_REGISTER_TABLE)/sizeof(FETS_REGISTER_TABLE[0]);
  Status = RegisterIhisiSubFunction (SubFuncTable, TableCount);
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
