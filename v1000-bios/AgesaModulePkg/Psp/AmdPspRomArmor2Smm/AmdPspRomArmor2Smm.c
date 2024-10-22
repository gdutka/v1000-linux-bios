/*****************************************************************************
 *
 * Copyright (C) 2020-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/
#include "AmdPspRomArmor2Smm.h"
#include <Library/IdsLib.h>
#include <Filecode.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>

#define FILECODE PSP_AMDPSP_ROM_ARMOR2_SMM_FILECODE

BOOLEAN                             *mArmorLibPspMboxSmmFlagAddr = NULL;
MBOX_ROM_ARMOR_FLASH_COMMAND_BUFFER mMboxRomArmorCommandBuffer;
ROMARMOR_BIOS_NVSTORE               mBIOSNvStore;
VOID                                *mAmdPspRomArmorRegistrationEndOfS3Resume = NULL;
BOOLEAN                             mBootFromRam = TRUE;

/**
  Read the 3 byte manufacture and device ID from the SPI flash.

  This routine must be called at or below TPL_NOTIFY.
  This routine reads the 3 byte manufacture and device ID from the flash part
  filling the buffer provided.

  @param[in]  This    Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data structure.
  @param[out] Buffer  Pointer to a 3 byte buffer to receive the manufacture and
                      device ID.



  @retval EFI_SUCCESS            The manufacture and device ID was read
                                 successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL
  @retval EFI_DEVICE_ERROR       Invalid data received from SPI flash part.

**/
EFI_STATUS
EFIAPI
GetFlashId (
  IN  CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  OUT UINT8                             *Buffer
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Read data from the SPI flash at not fast speed

  This routine must be called at or below TPL_NOTIFY.
  This routine reads data from the SPI part in the buffer provided.

  @param[in]  This           Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                             structure.
  @param[in]  FlashAddress   Address in the flash to start reading
  @param[in]  LengthInBytes  Read length in bytes
  @param[out] Buffer         Address of a buffer to receive the data

  @retval EFI_SUCCESS            The data was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or
                                 FlashAddress >= This->FlashSize, or
                                 LengthInBytes > This->FlashSize - FlashAddress

**/
EFI_STATUS
EFIAPI
LfReadData (
  IN  CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN  UINT32                            FlashAddress,
  IN  UINT32                            LengthInBytes,
  OUT UINT8                             *Buffer
  )
{
  return EFI_UNSUPPORTED;
}
/**
  Read the flash status register.

  This routine must be called at or below TPL_NOTIFY.
  This routine reads the flash part status register.

  @param[in]  This           Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                             structure.
  @param[in]  LengthInBytes  Number of status bytes to read.
  @param[out] FlashStatus    Pointer to a buffer to receive the flash status.

  @retval EFI_SUCCESS  The status register was read successfully.

**/
EFI_STATUS
EFIAPI
ReadStatus (
  IN  CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN  UINT32                            LengthInBytes,
  OUT UINT8                             *FlashStatus
  )
{
  return EFI_UNSUPPORTED;
}
/**
  Write the flash status register.

  This routine must be called at or below TPL_N OTIFY.
  This routine writes the flash part status register.

  @param[in] This           Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                            structure.
  @param[in] LengthInBytes  Number of status bytes to write.
  @param[in] FlashStatus    Pointer to a buffer containing the new status.

  @retval EFI_SUCCESS           The status write was successful.
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate the write buffer.

**/
EFI_STATUS
EFIAPI
WriteStatus (
  IN CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN UINT32                            LengthInBytes,
  IN UINT8                             *FlashStatus
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Read data from the SPI flash.

  This routine must be called at or below TPL_NOTIFY.
  This routine reads data from the SPI part in the buffer provided.

  @param[in]  This           Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                             structure.
  @param[in]  FlashAddress   Address in the flash to start reading
  @param[in]  LengthInBytes  Read length in bytes
  @param[out] Buffer         Address of a buffer to receive the data

  @retval EFI_SUCCESS            The data was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or
                                 FlashAddress >= This->FlashSize, or
                                 LengthInBytes > This->FlashSize - FlashAddress

**/
EFI_STATUS
EFIAPI
ReadData (
  IN  CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN  UINT32                            FlashAddress,
  IN  UINT32                            LengthInBytes,
  OUT UINT8                             *Buffer
  )
{
  EFI_STATUS Status;
  UINT32  ByteCounter;
  UINT32 CurrentAddress;
  UINT8 *CurrentBuffer;
  UINT32 Length;

  IDS_HDT_CONSOLE_PSP_TRACE ("SPI ReadData FlashAddress=0x%x, LengthInBytes=0x%x, Buffer = 0x%x\n", FlashAddress, LengthInBytes, Buffer);

  Status = EFI_DEVICE_ERROR;
  if (Buffer == NULL ||
      FlashAddress >= This->FlashSize ||
      LengthInBytes > This->FlashSize - FlashAddress) {
    return EFI_INVALID_PARAMETER;
  }

  mMboxRomArmorCommandBuffer.Transaction = READ_ACCESS;
  for (ByteCounter = 0; ByteCounter < LengthInBytes;) {
    CurrentAddress = FlashAddress + ByteCounter;
    CurrentBuffer = Buffer + ByteCounter;
    Length = LengthInBytes - ByteCounter;
    if (Length > SIZE_4KB) {
      Length = SIZE_4KB;
    }
    IDS_HDT_CONSOLE_PSP_TRACE ("  CurrentAddress=0x%x, Length=0x%x\n", CurrentAddress, Length);
    mMboxRomArmorCommandBuffer.Offset = CurrentAddress;
    mMboxRomArmorCommandBuffer.Size = Length;
    Status = PspMboxBiosCmdArmorSpiTransction (&mMboxRomArmorCommandBuffer, mArmorLibPspMboxSmmFlagAddr);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("SPI Read Error:%r\n", Status);
      ASSERT (FALSE);
      break;
    }
    CopyMem (CurrentBuffer, mMboxRomArmorCommandBuffer.Buffer, Length);
    ByteCounter += Length;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("SPI ReadData Buffer[0] = 0x%x\n", Buffer[0]);

  return Status;
}

/**
  Write data to the SPI flash.

  This routine must be called at or below TPL_NOTIFY.
  This routine breaks up the write operation as necessary to write the data to
  the SPI part.

  @param[in] This           Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                            structure.
  @param[in] FlashAddress   Address in the flash to start writing
  @param[in] LengthInBytes  Write length in bytes
  @param[in] Buffer         Address of a buffer containing the data

  @retval EFI_SUCCESS            The data was written successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or
                                 FlashAddress >= This->FlashSize, or
                                 LengthInBytes > This->FlashSize - FlashAddress
  @retval EFI_OUT_OF_RESOURCES   Insufficient memory to copy buffer.

**/
EFI_STATUS
EFIAPI
WriteData (
  IN CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN UINT32                            FlashAddress,
  IN UINT32                            LengthInBytes,
  IN UINT8                             *Buffer
  )
{
  EFI_STATUS  Status;
  UINT32 ByteCounter;
  UINT32 CurrentAddress;
  UINT32 Length;
  UINT8 *CurrentBuffer;

  Status = EFI_DEVICE_ERROR;
  if (Buffer == NULL ||
      LengthInBytes == 0 ||
      FlashAddress >= This->FlashSize ||
      LengthInBytes > This->FlashSize - FlashAddress) {
    return EFI_INVALID_PARAMETER;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("SPI WriteData FlashAddress=0x%x, LengthInBytes=0x%x, Buffer = 0x%x, Buffer[0] = 0x%x\n", FlashAddress, LengthInBytes, Buffer, Buffer[0]);

  if ((FlashAddress >= mBIOSNvStore.LABase) && ((FlashAddress + LengthInBytes) <= (mBIOSNvStore.LABase + mBIOSNvStore.Size))) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Copy write data to = 0x%x\n", mBIOSNvStore.ShadowBase + (FlashAddress - mBIOSNvStore.LABase));
    CopyMem (mBIOSNvStore.ShadowBase + (FlashAddress - mBIOSNvStore.LABase), Buffer, LengthInBytes);
  }

  mMboxRomArmorCommandBuffer.Transaction = WRITE_ACCESS;
  for (ByteCounter = 0; ByteCounter < LengthInBytes;) {
    CurrentAddress = FlashAddress + ByteCounter;
    CurrentBuffer = Buffer + ByteCounter;
    Length = LengthInBytes - ByteCounter;
    if (Length > SIZE_4KB) {
      Length = SIZE_4KB;
    }
    IDS_HDT_CONSOLE_PSP_TRACE ("  CurrentAddress=0x%x, Length=0x%x\n", CurrentAddress, Length);
    CopyMem (mMboxRomArmorCommandBuffer.Buffer, CurrentBuffer, Length);
    mMboxRomArmorCommandBuffer.Offset = CurrentAddress;
    mMboxRomArmorCommandBuffer.Size = Length;
    Status = PspMboxBiosCmdArmorSpiTransction (&mMboxRomArmorCommandBuffer, mArmorLibPspMboxSmmFlagAddr);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("SPI Write Error:%r\n", Status);
      ASSERT (FALSE);
      break;
    }
    ByteCounter += Length;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("SPI WriteData Exit\n");

  return Status;
}

/**
  Efficiently erases one or more 4KiB regions in the SPI flash.

  This routine must be called at or below TPL_NOTIFY.
  This routine uses a combination of 4 KiB and larger blocks to erase the
  specified area.

  @param[in] This          Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                           structure.
  @param[in] FlashAddress  Address within a 4 KiB block to start erasing
  @param[in] BlockCount    Number of 4 KiB blocks to erase

  @retval EFI_SUCCESS            The erase was completed successfully.
  @retval EFI_INVALID_PARAMETER  FlashAddress >= This->FlashSize, or
                                 BlockCount * 4 KiB
                                   > This->FlashSize - FlashAddress

**/
EFI_STATUS
EFIAPI
Erase (
  IN CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN UINT32                            FlashAddress,
  IN UINT32                            BlockCount
  )
{
  EFI_STATUS  Status;
  UINT32 EraseLength;
  UINT32 CurrentAddress;
  UINT32 BlockCounter;

  Status = EFI_DEVICE_ERROR;
  EraseLength = BlockCount * SIZE_4KB;
  // Align start Address to 4KB
  CurrentAddress = FlashAddress & ~(SIZE_4KB - 1);
  if (BlockCount == 0 ||
      CurrentAddress >= This->FlashSize ||
      EraseLength > This->FlashSize - CurrentAddress) {
    return EFI_INVALID_PARAMETER;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("SPI Erase FlashAddress=%x, BlockCount=%x\n", CurrentAddress, BlockCount);

  if ((CurrentAddress >= mBIOSNvStore.LABase) && ((CurrentAddress + EraseLength) <= (mBIOSNvStore.LABase +  mBIOSNvStore.Size))) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Set FF to address = 0x%x\n", mBIOSNvStore.ShadowBase + (CurrentAddress - mBIOSNvStore.LABase));
    SetMem (mBIOSNvStore.ShadowBase + (CurrentAddress - mBIOSNvStore.LABase), EraseLength, 0xFF);
  }

  mMboxRomArmorCommandBuffer.Size = SIZE_4KB;
  mMboxRomArmorCommandBuffer.Transaction = ERASE;
  for (BlockCounter= 0; BlockCounter < BlockCount; BlockCounter++) {
    mMboxRomArmorCommandBuffer.Offset = CurrentAddress + (BlockCounter * SIZE_4KB);
    Status = PspMboxBiosCmdArmorSpiTransction (&mMboxRomArmorCommandBuffer, mArmorLibPspMboxSmmFlagAddr);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("SPI Erase Error:%r\n", Status);
      ASSERT (FALSE);
      break;
    }
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("SPI Erase Exit\n");

  return Status;
}

EFI_STATUS
EFIAPI
AmdRomArmor2Init (
  IN OUT EFI_SPI_NOR_FLASH_PROTOCOL *Protocol
  )
{
  MBOX_ROM_ARMOR_ENFORCE              RomArmor2EnforceBuffer;
  PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL *PspMboxSmmBufferAddressProtocol;
  SMM_REQ_BUFFER                      SmmInfoReq;
  BIOS_MBOX                           *PspToBiosMbox;
  EFI_STATUS                          Status;
  EFI_PHYSICAL_ADDRESS                ShadowBase;
  EFI_BOOT_MODE                       BootMode;

  IDS_HDT_CONSOLE_PSP_TRACE ("%a - ENTRY\n", __FUNCTION__);

  Status = gSmst->SmmLocateProtocol (&gPspMboxSmmBufferAddressProtocolGuid, NULL, &PspMboxSmmBufferAddressProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mArmorLibPspMboxSmmFlagAddr = PspMboxSmmBufferAddressProtocol->PspMboxSmmFlagAddr;
  IDS_HDT_CONSOLE_PSP_TRACE ("\tmPspMboxSmmFlagAddr 0x%x\n", mArmorLibPspMboxSmmFlagAddr);

  mBIOSNvStore.LABase = PcdGet32 (PcdFlashNvStorageVariableBase) & 0xFFFFFF; //Assuming 16MB or higher flash part.
  mBIOSNvStore.Size = FixedPcdGet32 (PcdFlashNvStorageVariableSize) \
                    + FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize) \
                    + FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize);
  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiRuntimeServicesData,
                  EFI_SIZE_TO_PAGES (mBIOSNvStore.Size),
                  &ShadowBase);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tFailed to allocate memory: %r, BIOSNvStore.LABase:0x%x, Size:0x%x\n", Status, mBIOSNvStore.LABase, mBIOSNvStore.Size);
    ASSERT (Status == EFI_SUCCESS);
  }

  mBIOSNvStore.ShadowBase = (UINT8 *) (UINTN) ShadowBase;

  //
  // Shadow BIOS NV store to DRAM
  //
  CopyMem (mBIOSNvStore.ShadowBase, (UINT8 *)(UINTN)PcdGet32 (PcdFlashNvStorageVariableBase), mBIOSNvStore.Size);
  //
  // Map BIOS NV Store MMIO to DRAM
  //
  PcdSet64S (PcdFlashNvStorageVariableBase64, (UINT64)(UINTN)ShadowBase);
  PcdSet64S (PcdFlashNvStorageFtwWorkingBase64, (UINT64)(UINTN)(ShadowBase + FixedPcdGet32 (PcdFlashNvStorageVariableSize)));
  PcdSet64S (PcdFlashNvStorageFtwSpareBase64, (UINT64)(UINTN)(ShadowBase + FixedPcdGet32 (PcdFlashNvStorageVariableSize)
                                                                        + FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize)));

  IDS_HDT_CONSOLE_PSP_TRACE (" PcdFlashNvStorageVariableBase64 = 0x%lx\n",
        PcdGet64 (PcdFlashNvStorageVariableBase64));
  IDS_HDT_CONSOLE_PSP_TRACE (" PcdFlashNvStorageFtwWorkingBase64 = 0x%lx\n",
        PcdGet64 (PcdFlashNvStorageFtwWorkingBase64));
  IDS_HDT_CONSOLE_PSP_TRACE (" PcdFlashNvStorageFtwSpareBase64 = 0x%lx\n",
        PcdGet64 (PcdFlashNvStorageFtwSpareBase64));

  ZeroMem (&RomArmor2EnforceBuffer, sizeof (MBOX_ROM_ARMOR_ENFORCE));
  BootMode = GetBootModeHob();
  if ((BootMode == BOOT_ON_FLASH_UPDATE) || (BootMode == BOOT_IN_RECOVERY_MODE)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Capsule Update \n");
    RomArmor2EnforceBuffer.CapsuleUpdate = 1;
  }
  Status = PspMboxBiosCmdArmorEnterSmmOnlyMode2 (&RomArmor2EnforceBuffer, mArmorLibPspMboxSmmFlagAddr);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  Protocol->FlashSize = RomArmor2EnforceBuffer.FlashSize;

  //Prepare SMM_REQ_BUFFER
  ZeroMem (&SmmInfoReq, sizeof (SmmInfoReq));
  PspToBiosMbox = (BIOS_MBOX *) AllocatePages (1);
  ZeroMem (PspToBiosMbox, SIZE_4KB);

  PspToBiosMbox->MboxCmd      = 0;
  PspToBiosMbox->MboxSts.CommandReady = TRUE;
  SmmInfoReq.PSPSmmDataRegion =   (UINT64) PspToBiosMbox;
  SmmInfoReq.PspSmmDataLength =   SIZE_4KB;
  //Get TsegBase, TsegSize
  // mTsegMask = (~(mTsegMask & MSR_SMMMASK_TSEGMASK_BITS) + 1) & MSR_SMMMASK_TSEGMASK_BITS;
  SmmInfoReq.SMMBase = AsmReadMsr64 (MSR_SMMADDR);
  SmmInfoReq.SMMBase &= MSR_SMMADDR_TSEGBASE_BITS;
  SmmInfoReq.SMMMask = AsmReadMsr64 (MSR_SMMMASK);
  SmmInfoReq.PspMboxSmmBufferAddress = (UINT64) (UINTN) PspMboxSmmBufferAddressProtocol->PspMboxSmmBuffer;
  SmmInfoReq.PspMboxSmmFlagAddress = (UINT64) (UINTN) PspMboxSmmBufferAddressProtocol->PspMboxSmmFlagAddr;
  //Fill SmmRegisterInfo

  SmmInfoReq.SmmTrigInfo.Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98;
  SmmInfoReq.SmmTrigInfo.AddressType = SMM_TRIGGER_MEM;
  SmmInfoReq.SmmTrigInfo.ValueWidth = SMM_TRIGGER_DWORD;
  SmmInfoReq.SmmTrigInfo.ValueAndMask = (UINT32) ~BIT25;
  SmmInfoReq.SmmTrigInfo.ValueOrMask = BIT25;

  SmmInfoReq.SmmRegInfo.SmiEnb.Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98;
  SmmInfoReq.SmmRegInfo.SmiEnb.AddressType = SMM_TRIGGER_MEM;
  SmmInfoReq.SmmRegInfo.SmiEnb.ValueWidth = SMM_TRIGGER_DWORD;
  SmmInfoReq.SmmRegInfo.SmiEnb.RegBitMask = BIT31;
  SmmInfoReq.SmmRegInfo.SmiEnb.ExpectValue = 0;

  SmmInfoReq.SmmRegInfo.Eos.Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98;
  SmmInfoReq.SmmRegInfo.Eos.AddressType = SMM_TRIGGER_MEM;
  SmmInfoReq.SmmRegInfo.Eos.ValueWidth = SMM_TRIGGER_DWORD;
  SmmInfoReq.SmmRegInfo.Eos.RegBitMask = BIT28;
  SmmInfoReq.SmmRegInfo.Eos.ExpectValue = BIT28;

  SmmInfoReq.SmmRegInfo.FakeSmiEn.Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA8;
  SmmInfoReq.SmmRegInfo.FakeSmiEn.AddressType = SMM_TRIGGER_MEM;
  SmmInfoReq.SmmRegInfo.FakeSmiEn.ValueWidth = SMM_TRIGGER_DWORD;
  SmmInfoReq.SmmRegInfo.FakeSmiEn.RegBitMask = BIT3 + BIT2;
  SmmInfoReq.SmmRegInfo.FakeSmiEn.ExpectValue = BIT2;

  PspMboxBiosCmdSmmInfo (&SmmInfoReq);

  IDS_HDT_CONSOLE_PSP_TRACE ("%a - EXIT\n", __FUNCTION__);
  return Status;
}

EFI_STATUS
EFIAPI
AmdRomArmor2S3Init (
  )
{
  MBOX_ROM_ARMOR_ENFORCE RomArmor2EnforceBuffer;
  ZeroMem (&RomArmor2EnforceBuffer, sizeof (MBOX_ROM_ARMOR_ENFORCE));
  return PspMboxBiosCmdArmorEnterSmmOnlyMode2 (&RomArmor2EnforceBuffer, mArmorLibPspMboxSmmFlagAddr);
}


/**
  Notification for SMM EndOfS3Resume protocol.

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @retval EFI_SUCCESS   Notification runs successfully.
**/
EFI_STATUS
EFIAPI
AmdPspRomArmorEndOfS3ResumeNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS         Status;

  Status = AmdRomArmor2S3Init ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // For Boot from RAM, notify PSP FW Boot done at end of S3 resume directly
  // For Boot from ROM, SBIOS must trigger SMI PcdFchOemAfterSystemWakeSwSmi in ACPI _WAK method,
  // AGESA will handle the SMI to call AmdRomArmor2NotifyPspBootDone
  if (mBootFromRam) {
    PspMboxBiosCmdExitBootServices ();
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmdRomArmor2NotifyPspBootDone (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  // Notify PSP FW Boot done
  return PspMboxBiosCmdExitBootServices ();
}

EFI_STATUS
AmdRomArmor2NotifyPspBootDoneCallback (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS        Status;
  FCH_SMM_SW_DISPATCH2_PROTOCOL      *SwDispatch;
  FCH_SMM_SW_REGISTER_CONTEXT        SwContext;
  EFI_HANDLE                         SwHandle;

  Status = gSmst->SmmLocateProtocol (&gFchSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);

  SwContext.AmdSwValue  = PcdGet8 (PcdFchOemAfterSystemWakeSwSmi);
  SwContext.Order       = 0x80;
  Status = SwDispatch->Register (
                          SwDispatch,
                          AmdRomArmor2NotifyPspBootDone,
                          &SwContext,
                          &SwHandle
                          );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("  AmdRomArmor2NotifyPspBootDone is not registered\n");
    return Status;
  }
  return EFI_SUCCESS;
}

/**
  Entry point of RomArmor2 SMM driver.

  @param ImageHandle  Image handle of this driver.
  @param SystemTable  Pointer to standard EFI system table.

  @retval EFI_SUCCESS       Succeed.
  @retval EFI_DEVICE_ERROR  Fail to install EFI_SPI_SMM_NOR_FLASH_PROTOCOL.
**/
EFI_STATUS
EFIAPI
AmdPspRomArmor2SmmEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                         Status;
  EFI_SPI_NOR_FLASH_PROTOCOL         *Protocol;
  EFI_HANDLE                         Handle;
  FCH_SMM_SW_DISPATCH2_PROTOCOL      *SwDispatch;
  FCH_SMM_SW_REGISTER_CONTEXT        SwContext;
  EFI_HANDLE                         SwHandle;
  VOID                               *Registration;
  TYPE_ATTRIB                        TypeAttrib;
  UINT64                             EntryAddress;
  UINT32                             EntrySize;
  UINT64                             EntryDest;

  if (!PcdGetBool (PcdAmdPspRomArmor2Enabled)) {
    return EFI_UNSUPPORTED;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("%a - ENTRY\n", __FUNCTION__);

  Protocol = AllocateZeroPool (sizeof (EFI_SPI_NOR_FLASH_PROTOCOL));
  ASSERT (Protocol != NULL);
  if (Protocol == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (BIOSEntryInfo (BIOS_FIRMWARE, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest) == FALSE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get the correct Entry of BIOS firmware\n");
  }
  if (TypeAttrib.Copy == 1) {
    mBootFromRam = TRUE;
  } else {
    mBootFromRam = FALSE;
  }
  ZeroMem (&mMboxRomArmorCommandBuffer, sizeof (mMboxRomArmorCommandBuffer));
  mMboxRomArmorCommandBuffer.Header.TotalSize = sizeof (mMboxRomArmorCommandBuffer);
  mMboxRomArmorCommandBuffer.Buffer = AllocateRuntimePages (1);
  ASSERT (mMboxRomArmorCommandBuffer.Buffer != NULL);
  if (mMboxRomArmorCommandBuffer.Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = AmdRomArmor2Init (Protocol);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return Status;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Flash Size = 0x%x\n", Protocol->FlashSize);

  //
  // Register EndOfS3Resume notification
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEdkiiEndOfS3ResumeGuid,
                    AmdPspRomArmorEndOfS3ResumeNotify,
                    &mAmdPspRomArmorRegistrationEndOfS3Resume
                    );
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return Status;
  }

  // For Boot from ROM case, install SMI handler to send boot done command to PSP in S3 resume at system wake(_WAK)
  if (!mBootFromRam) {
    Status = gSmst->SmmLocateProtocol (
                    &gFchSmmSwDispatch2ProtocolGuid,
                    NULL,
                    &SwDispatch
                    );
    if (!EFI_ERROR (Status)) {
      SwContext.AmdSwValue  = PcdGet8 (PcdFchOemAfterSystemWakeSwSmi);
      SwContext.Order       = 0x80;
      Status = SwDispatch->Register (
                              SwDispatch,
                              AmdRomArmor2NotifyPspBootDone,
                              &SwContext,
                              &SwHandle
                              );
      if (EFI_ERROR (Status)) {
        IDS_HDT_CONSOLE_PSP_TRACE ("  AmdRomArmor2NotifyPspBootDone is not registered\n");
        return Status;
      }
    } else {
      Status = gSmst->SmmRegisterProtocolNotify (
                        &gFchSmmSwDispatch2ProtocolGuid,
                        AmdRomArmor2NotifyPspBootDoneCallback,
                        &Registration
                        );
      if (EFI_ERROR (Status)) {
        IDS_HDT_CONSOLE_PSP_TRACE ("\tFailed to register callback function AmdRomArmor2NotifyPspBootDoneCallback: %r\n", Status);
        ASSERT (Status == EFI_SUCCESS);
      }
    }
  }

  Protocol->SpiPeripheral = NULL;
  Protocol->GetFlashid = GetFlashId;
  Protocol->ReadData = ReadData;
  Protocol->LfReadData = LfReadData;
  Protocol->ReadStatus = ReadStatus;
  Protocol->WriteStatus = WriteStatus;
  Protocol->WriteData = WriteData;
  Protocol->Erase = Erase;
  Protocol->EraseBlockBytes = SIZE_4KB;
  Handle = NULL;

  Status = gSmst->SmmInstallProtocolInterface(
                    &Handle,
                    &gEfiSpiSmmNorFlashProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    Protocol
                    );

  IDS_HDT_CONSOLE_PSP_TRACE ("%a: EXIT - Status=%r\n", __FUNCTION__, Status);
  return Status;
}
