/** @file
  This driver provides IHISI interface in SMM mode

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
#include "IhisiOemExtraDataCommunication.h"
#include <IndustryStandard/Oa3_0.h>
#include <Library/SmmChipsetSvcLib.h>
#include <Library/BaseOemSvcKernelLib.h>
#include <Library/SmmOemSvcKernelLib.h>
#include <Library/FlashRegionLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/FmpAuthenticationLib.h>
#include <Library/FmpDependencyLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Guid/ImageAuthentication.h>


STATIC UINT8     mOemExtraDataType;
STATIC UINTN     mRomBaseAddress;
STATIC UINTN     mRomSize;
STATIC BOOLEAN   mEcIdle = FALSE;
AP_COMMUNICATION_DATA_TABLE      mApCommDataBuffer;
STATIC UINT8     *mCertList = NULL;
STATIC UINTN     mCertSize;

typedef struct _IHISI_OEM_COMMUNICATION_INFO {
  OEM_EXTRA_DATA_INPUT_DATATYPE_CODE  DataType;
  UINTN                               OemDataSize;
  UINTN                               OemReceivedSize;
  UINT8                               *OemDataBuffer;
} IHISI_OEM_COMMUNICATION_INFO;


STATIC IHISI_OEM_COMMUNICATION_INFO  mOemCommunicationInfo;

STATIC
IHISI_REGISTER_TABLE
OEM_EXT_COMMON_REGISTER_TABLE[] = {
  //
  // AH=41h
  //
  { OEMSFOEMExCommunication, "S41Kn_CommuSaveRegs", KernelCommunicationSaveRegs             }, \
  { OEMSFOEMExCommunication, "S41Cs_ExtDataCommun", ChipsetOemExtraDataCommunication        }, \
  { OEMSFOEMExCommunication, "S41OemT01Vbios00000", OemIhisiS41T1Vbios                      }, \
  { OEMSFOEMExCommunication, "S41OemT54LogoUpdate", OemIhisiS41T54LogoUpdate                }, \
  { OEMSFOEMExCommunication, "S41OemT55CheckSignB", OemIhisiS41T55CheckBiosSignBySystemBios }, \
  { OEMSFOEMExCommunication, "S41OemReservedFun00", OemIhisiS41ReservedFunction             }, \
  { OEMSFOEMExCommunication, "S41Kn_T51EcIdelTrue", KernelT51EcIdelTrue                     }, \
  { OEMSFOEMExCommunication, "S41Kn_ExtDataCommun", KernelOemExtraDataCommunication         }, \
  { OEMSFOEMExCommunication, "S41Kn_T51EcIdelFals", KernelT51EcIdelFalse                    }, \
  { OEMSFOEMExCommunication, "S41OemT50Oa30RWFun0", OemIhisiS41T50a30ReadWrite              }, \

  //
  // AH=42h
  //
  { OEMSFOEMExDataWrite, "S42Cs_ExtDataWrite0", ChipsetOemExtraDataWrite,         }, \
  { OEMSFOEMExDataWrite, "S42Kn_T50EcIdelTrue", KernelT50EcIdelTrue,              }, \
  { OEMSFOEMExDataWrite, "S42Kn_ExtDataWrite0", KernelOemExtraDataWrite,          }, \
  { OEMSFOEMExDataWrite, "S42Kn_T50EcIdelFals", KernelT50EcIdelFalse,             }, \
  { OEMSFOEMExDataWrite, "S42Cs_DShutdownMode", ChipsetOemExtraDataDosShutdownMode}, \

  //
  // AH=47h
  //
  { OEMSFOEMExDataRead, "S47Cs_ExtDataRead00", ChipsetOemExtraDataRead}, \
  { OEMSFOEMExDataRead, "S47Kn_ExtDataRead00", KernelOemExtraDataRead }
};

typedef struct {
  UINT32  Signature;
  UINT32  HeaderSize;
  UINT32  FwVersion;
  UINT32  LowestSupportedVersion;
} FMP_PAYLOAD_HEADER;

///
/// Identifier is used to make sure the data in the header is for this structure
/// and version.  If the structure changes update the last digit.
///
#define FMP_PAYLOAD_HEADER_SIGNATURE SIGNATURE_32 ('M', 'S', 'S', '1')

/**
  Helper function to safely calculate the size of all headers
  within an EFI_FIRMWARE_IMAGE_AUTHENTICATION structure.

  @param[in]  Image                 Pointer to the image.
  @param[in]  ImageSize             Size of the.

  @retval  UINT32>0  Valid size of all the headers.
  @retval  0         Structure is bad and size cannot be found.

**/
STATIC
UINT32
GetAllHeaderSize (
  IN CONST EFI_FIRMWARE_IMAGE_AUTHENTICATION  *Image,
  IN UINTN                                    ImageSize
  )
{
  UINT32                        CalculatedSize;
  UINT32                        DependenciesSize;
  FMP_PAYLOAD_HEADER            *FmpPayloadHeader;

  if (Image == NULL) {
    return 0;
  }

  GetImageDependency ((EFI_FIRMWARE_IMAGE_AUTHENTICATION *)Image, ImageSize, &DependenciesSize, NULL);
  //
  // Attempt to get the FMP_PAYLOAD_HEADER
  //
  FmpPayloadHeader = (FMP_PAYLOAD_HEADER *)((UINT8 *)Image + sizeof (Image->MonotonicCount) + Image->AuthInfo.Hdr.dwLength  + DependenciesSize);
  if (FmpPayloadHeader == NULL) {
    return 0;
  }

  if (FmpPayloadHeader->Signature != FMP_PAYLOAD_HEADER_SIGNATURE) {
    return 0;
  }

  CalculatedSize = sizeof (Image->MonotonicCount) +
                   DependenciesSize +
                   (UINT32)(FmpPayloadHeader->HeaderSize) +
                   Image->AuthInfo.Hdr.dwLength;

  return CalculatedSize;
}

/**
  Get the certificate from firmware volume
**/
STATIC
VOID
GetCertificateData (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINT32                        AuthenticationStatus;
  UINT8                         *Buffer;
  UINTN                         Size;

  if (mCertList != NULL) {
    return;
  }
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || (HandleCount == 0)) {
    return;
  }
  //
  // Find desired image in all Fvs
  //
  Buffer = NULL;
  Size   = 0;
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **)&Fv
                    );
    if (EFI_ERROR (Status)) {
      break;
    }
    Status = Fv->ReadSection (
                   Fv,
                   PcdGetPtr (PcdSecureFlashCertificateFile),
                   EFI_SECTION_RAW,
                   0,
                   &Buffer,
                   &Size,
                   &AuthenticationStatus
                   );
    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  gBS->FreePool (HandleBuffer);
  if (Index >= HandleCount || Size == 0 || Buffer == NULL) {
    return;
  }

  mCertList = AllocatePool (Size);
  if (mCertList!= NULL) {
    mCertSize = Size;
    CopyMem ((VOID*)mCertList, (VOID*)Buffer, mCertSize);;
  }
  gBS->FreePool (Buffer);

}

/**
  AH=42h, Logo Data Write.
  54h = LogoUpdate

  @param[in]  WriteDataBuffer   Pointer to the data buffer prepare to write.
  @param[in]  WriteSize         Size of the data.
  @param[in]  RomBaseAddress    Base address of the SPI ROM(MMIO).

  @retval EFI_SUCCESS           Successfully returns.
**/
EFI_STATUS
EFIAPI
LogoDataWrite (
  IN UINT8                   *WriteDataBuffer,
  IN UINTN                   WriteSize,
  IN UINTN                   RomBaseAddress
  )
{
  EFI_STATUS              Status;
  UINT8                   LoopCount;
  UINT8                   BackupFlashMode;

  LoopCount = 0;
  Status = EFI_UNSUPPORTED;
  BackupFlashMode = *(mSmmFwBlockService->FlashMode);
  *(mSmmFwBlockService->FlashMode) = SMM_FW_FLASH_MODE;

  while ((EFI_ERROR (Status)) && (LoopCount < 100)) {
    Status = mSmmFwBlockService->EraseBlocks (
                                   mSmmFwBlockService,
                                   RomBaseAddress,
                                   &mRomSize
                                   );
    Status = mSmmFwBlockService->Write (
                                   mSmmFwBlockService,
                                   RomBaseAddress,
                                   &WriteSize,
                                   WriteDataBuffer
                                   );
    LoopCount++;
  }
  *(mSmmFwBlockService->FlashMode) = BackupFlashMode;
  return Status;
}

/**
  Inernal function to check if the application data type is valid

  @param[in]  Datatype   Application data type

  @retval TRUE   Application data type is valid.
  @retval FALSE  Application data type is invalid.
**/
STATIC
BOOLEAN
IsValidApDataType (
  IN  UINT8     Datatype
  )
{
  if (Datatype == Vbios                     || Datatype == Oa30ReadWrite          ||
      Datatype == Oa30Erase                 || Datatype == Oa30PopulateHeader     ||
      Datatype == Oa30DePopulateHeader      || Datatype == LogoUpdate             ||
      Datatype == CheckBiosSignBySystemBios || Datatype == ClaenSecureKey         ||
      Datatype == FactoryKeyRestore         || Datatype == PfatUpdate             ||
      Datatype == BiosPasswordErase         || Datatype == PfatReturn             ||
      Datatype == StartTimeMeasure          || Datatype == QueryTimeInterval      ||
      Datatype == GetSpiLockTable           || Datatype == AccessViaSpi           ||
      Datatype == PassErrorLog              || Datatype == GetOemCommonTable      ||
      Datatype == WriteOa2Marker            || Datatype == WriteOa2PublicKey      ||
      Datatype == EraseOa2Marker            || Datatype == EraseOa2PublicKey         ||
      Datatype == RefereExtendDataType      || Datatype == SignedLogoUpdate) {
    return TRUE;
  }
  return FALSE;
}

STATIC
BOOLEAN
MsdmExist (
  )
{
  EFI_STATUS                       Status;
  UINTN                            DataSize;
  BOOLEAN                          MsdmExist;
  UINTN                            RomBaseAddress;
  EFI_ACPI_MSDM_DATA_STRUCTURE     *MsdmData;

  MsdmData = NULL;
  MsdmExist = FALSE;

  DataSize = sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE);
  RomBaseAddress = (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionMsdmGuid, 1);

  MsdmData = AllocatePool (DataSize);
  if (MsdmData == NULL) {
    goto Done;
  }
  Status = mSmmFwBlockService->Read (
                                 mSmmFwBlockService,
                                 RomBaseAddress,
                                 0,
                                 &DataSize,
                                 (UINT8*) MsdmData
                                 );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  if ((MsdmData->MsdmVersion == 0xFFFFFFFF) &&
      (MsdmData->MdsmDataType == 0xFFFFFFFF) &&
      (MsdmData->MsdmDataLength == 0xFFFFFFFF)) {
    goto Done;
  }

  MsdmExist = TRUE;

Done:
  if (MsdmData != NULL) {
    FreePool (MsdmData);
  }
  return MsdmExist;
}

EFI_STATUS
EFIAPI
SetApandBiosCommDataBuffer (
  AP_COMMUNICATION_DATA_TABLE       *ApCommDataBuffer,
  BIOS_COMMUNICATION_DATA_TABLE     *BiosCommDataBuffer
  )
{

  if ((ApCommDataBuffer->Signature != AP_COMMUNICATION_SIGNATURE) && (ApCommDataBuffer->Signature != BIOS_COMMUNICATION_SIGNATURE)) {
    return EFI_UNSUPPORTED;
  }


  if (ApCommDataBuffer->Signature == BIOS_COMMUNICATION_SIGNATURE) {
    CopyMem(BiosCommDataBuffer, (VOID *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX), sizeof (BIOS_COMMUNICATION_DATA_TABLE));
    CopyMem(ApCommDataBuffer, &mApCommDataBuffer, sizeof (AP_COMMUNICATION_DATA_TABLE));
  } else {
    ZeroMem (BiosCommDataBuffer, sizeof (BIOS_COMMUNICATION_DATA_TABLE));
    BiosCommDataBuffer->Signature        = BIOS_COMMUNICATION_SIGNATURE;
    BiosCommDataBuffer->StructureSize    = sizeof (BIOS_COMMUNICATION_DATA_TABLE);
    BiosCommDataBuffer->BlockSize        = OemExtraBlockSize4k;
    BiosCommDataBuffer->DataSize         = OemExtraDataSize64k;
    BiosCommDataBuffer->PhysicalDataSize = 0;
    BiosCommDataBuffer->ErrorReturn      = 0;
  }
  return EFI_SUCCESS;
}


/**
  AH=41h, OEM Extra Data Communication type.

  50h = Oa30ReadWrite

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS  Read or write OA3.0 successful.
**/
EFI_STATUS
EFIAPI
Oa30ReadWriteFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{
  UINT32            MsdmDataSize;

  MsdmDataSize = sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE);
  switch (ApCommDataBuffer->DataSize) {

  case OemExtraReportReadSize:
    BiosCommDataBuffer->Signature        = BIOS_COMMUNICATION_SIGNATURE;
    BiosCommDataBuffer->StructureSize    = sizeof (BIOS_COMMUNICATION_DATA_TABLE);
    BiosCommDataBuffer->BlockSize        = OemExtraBlockSize4k;
    BiosCommDataBuffer->DataSize         = OemExtraReportReadSize;
    BiosCommDataBuffer->PhysicalDataSize = MsdmDataSize;
    break;

  case OemExtraSkipSizeCheck:
    BiosCommDataBuffer->Signature        = BIOS_COMMUNICATION_SIGNATURE;
    BiosCommDataBuffer->StructureSize    = sizeof (BIOS_COMMUNICATION_DATA_TABLE);
    BiosCommDataBuffer->BlockSize        = OemExtraBlockSize4k;
    BiosCommDataBuffer->DataSize         = OemExtraSkipSizeCheck;   //Don't care
    BiosCommDataBuffer->PhysicalDataSize = 0x00;                   //Don't care
    break;

  case OemExtraReportWriteSize:
    BiosCommDataBuffer->Signature        = BIOS_COMMUNICATION_SIGNATURE;
    BiosCommDataBuffer->StructureSize    = sizeof (BIOS_COMMUNICATION_DATA_TABLE);
    BiosCommDataBuffer->BlockSize        = OemExtraBlockSize4k;
    BiosCommDataBuffer->DataSize         = OemExtraReportWriteSize;
    BiosCommDataBuffer->PhysicalDataSize = MsdmDataSize;   //bin size
    break;

  default:
    break;
  }

  if (!MsdmExist ()) {
    BiosCommDataBuffer->ErrorReturn    = (BiosCommDataBuffer->ErrorReturn | ERROR_RETURE_OA30_NOT_EXIST);
  }

  return EFI_SUCCESS;
}

/**
  AH=41h, OEM Extra Data Communication type.
  51h = Oa30Erase Ec Idle true
**/
EFI_STATUS
EFIAPI
KernelT51EcIdelTrue (
  VOID
  )
{
  EFI_STATUS        OemSvcStatus;

  if (!IsValidApDataType (mOemExtraDataType)) {
    return IHISI_INVALID_PARAMETER;
  }
  mEcIdle = FALSE;
  if (MsdmExist () && mOemExtraDataType == Oa30Erase) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEcIdle \n"));
    OemSvcStatus = OemSvcEcIdle (TRUE);
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEcIdle Status: %r\n", OemSvcStatus));
    mEcIdle = TRUE;
  }

  return EFI_SUCCESS;
}

/**
  AH=41h, OEM Extra Data Communication type.
  51h = Oa30Erase Ec Idle False
**/
EFI_STATUS
EFIAPI
KernelT51EcIdelFalse (
  VOID
  )
{
  EFI_STATUS        OemSvcStatus;

  if (mEcIdle) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEcIdle \n"));
    OemSvcStatus = OemSvcEcIdle (FALSE);
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEcIdle Status: %r\n", OemSvcStatus));
    mEcIdle = FALSE;
  }

  return EFI_SUCCESS;
}

/**
  AH=41h, OEM Extra Data Communication type.

  51h = Oa30Erase

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS  Erase OA3.0 successful.
**/
EFI_STATUS
EFIAPI
Oa30EraseFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{
  EFI_STATUS                   Status;
  UINT8                        LoopCount;
  UINTN                        RomBaseAddress;
  UINTN                        RomSize;

  RomBaseAddress = (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionMsdmGuid, 1);
  RomSize = (UINTN) FdmGetNAtSize (&gH2OFlashMapRegionMsdmGuid, 1);
  LoopCount = 0;
  Status = EFI_SUCCESS;

  if (!MsdmExist ()) {
    BiosCommDataBuffer->ErrorReturn = (BiosCommDataBuffer->ErrorReturn | ERROR_RETURE_OA30_NOT_EXIST);
    goto Done;
  }
  Status = EFI_UNSUPPORTED;
  while ((EFI_ERROR (Status)) && (LoopCount < 100)) {
    Status = mSmmFwBlockService->EraseBlocks (
                                   mSmmFwBlockService,
                                   RomBaseAddress,
                                   &RomSize
                                   );
    LoopCount++;
  }

Done:
  BiosCommDataBuffer->DataSize = ApCommDataBuffer->DataSize;
  BiosCommDataBuffer->PhysicalDataSize = ApCommDataBuffer->PhysicalDataSize;

  return Status;
}


/**
  AH=41h, OEM Extra Data Communication type.

  52h = Oa30PopulateHeader

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   populate header successful.
**/
EFI_STATUS
EFIAPI
Oa30PopulateHeaderFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  AH=41h, OEM Extra Data Communication type 53h to de-populate header.

  53h = Oa30DePopulateHeader

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   populate header successful.
**/
EFI_STATUS
EFIAPI
Oa30DePopulateHeaderFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  AH=42h, OEM Extra Data Write.
  50h = Oa30ReadWrite Ec Idle true
**/
EFI_STATUS
EFIAPI
KernelT50EcIdelTrue (
  VOID
  )
{
  UINTN           WriteSize;
  UINT32          MsdmDataSize;
  EFI_STATUS      OemSvcStatus;

  WriteSize    = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  MsdmDataSize = sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE);
  mEcIdle      = FALSE;

  if ((WriteSize == MsdmDataSize) && (mOemExtraDataType == Oa30ReadWrite)) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEcIdle \n"));
    OemSvcStatus = OemSvcEcIdle (TRUE);
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEcIdle Status: %r\n", OemSvcStatus));
    mEcIdle = TRUE;
  }

  return EFI_SUCCESS;
}

/**
  AH=42h, OEM Extra Data Write.
  50h = Oa30ReadWrite Ec Idle false
**/
EFI_STATUS
EFIAPI
KernelT50EcIdelFalse (
  VOID
  )
{
  UINTN           WriteSize;
  UINT32          MsdmDataSize;
  EFI_STATUS      OemSvcStatus;

  WriteSize       = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  MsdmDataSize = sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE);

  if (mEcIdle) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEcIdle \n"));
    OemSvcStatus = OemSvcEcIdle (FALSE);
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEcIdle Status: %r\n", OemSvcStatus));
    mEcIdle = FALSE;
  }
  return EFI_SUCCESS;
}

/**
  AH=42h, OEM Extra Data Write.
  50h = Oa30ReadWrite

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
EFIAPI
Oa30DataWrite (
  UINT8                   *WriteDataBuffer,
  UINTN                   WriteSize,
  UINTN                   RomBaseAddress
  )
{
  EFI_STATUS              Status;
  UINT8                   LoopCount;
  UINT32                  MsdmDataSize;

  MsdmDataSize = sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE);

  if (WriteSize == MsdmDataSize) {
    UINT8                          *ReturnDataBuffer = NULL;
    UINTN                          Index2 = 0;
    UINTN                          EraseSize = 0x1000;
    UINT8                          *TEMP;

    ReturnDataBuffer = AllocatePool (0x1000);
    if (ReturnDataBuffer == NULL) {
      return IHISI_OUT_OF_RESOURCES;
    }

    Status = mSmmFwBlockService->Read (
                                   mSmmFwBlockService,
                                   RomBaseAddress,
                                   0,
                                   &EraseSize,
                                   ReturnDataBuffer
                                   );
    //
    // Only modify the first 0x31 bytes
    //
    TEMP = ReturnDataBuffer;
    for (Index2 = 0; Index2 < MsdmDataSize; Index2++) {
      TEMP[Index2] = WriteDataBuffer[Index2];
    }

    LoopCount = 0;
    Status = EFI_UNSUPPORTED;
    *(mSmmFwBlockService->FlashMode) = SMM_FW_FLASH_MODE;

    while ((EFI_ERROR (Status)) && (LoopCount < 100)) {
      Status = mSmmFwBlockService->EraseBlocks (
                                     mSmmFwBlockService,
                                     RomBaseAddress,
                                     &EraseSize
                                     );
      Status = mSmmFwBlockService->Write (
                                     mSmmFwBlockService,
                                     RomBaseAddress,
                                     &EraseSize,
                                     ReturnDataBuffer
                                     );
      LoopCount++;
    }
    FreePool (ReturnDataBuffer);
  } else {
    return EFI_INVALID_PARAMETER;
  }

  return IHISI_SUCCESS;
}

/**
  AH=47h, OEM Extra Data Read.

  50h = Oa30ReadWrite

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
EFIAPI
Oa30DataRead (
  UINT8                   *ReadDataBuffer,
  UINTN                   *ReadSize,
  UINTN                   RomBaseAddress
  )
{
  EFI_STATUS                       Status;
  UINT8                            *ReturnDataBuffer = NULL;
  UINTN                            DataSize = 0x1000;

  ReturnDataBuffer = AllocatePool (DataSize);
  if (ReturnDataBuffer == NULL) {
    return IHISI_OUT_OF_RESOURCES;
  }

  Status = mSmmFwBlockService->Read (
                                 mSmmFwBlockService,
                                 RomBaseAddress,
                                 0,
                                 &DataSize,
                                 ReturnDataBuffer
                                 );

  *ReadSize = sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE);
  CopyMem (ReadDataBuffer, ReturnDataBuffer, *ReadSize);

  return Status;
}

/**
  AH=41h, OEM Extra Data Communication type.

  Hook chipset service SmmCsSvcIhisiOemExtCommunication.

  @retval EFI_SUCCESS            OEM Extra Data Write successful.
  @return Other                  OEM Extra Data Write failed.
**/
EFI_STATUS
EFIAPI
ChipsetOemExtraDataCommunication (
  VOID
  )
{
  EFI_STATUS                        Status;
  EFI_STATUS                        OemSvcStatus;
  BIOS_COMMUNICATION_DATA_TABLE     *BiosCommDataBuffer;
  AP_COMMUNICATION_DATA_TABLE       *ApCommDataBuffer;
  UINTN                             BufferSize;

  ApCommDataBuffer   = (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  BiosCommDataBuffer = AllocatePool (sizeof (BIOS_COMMUNICATION_DATA_TABLE));
  if (BiosCommDataBuffer == NULL) {
    return IHISI_OUT_OF_RESOURCES;
  }
  BufferSize = ApCommDataBuffer->StructureSize;
  if (BufferSize < sizeof(BIOS_COMMUNICATION_DATA_TABLE)) {
    BufferSize = sizeof(BIOS_COMMUNICATION_DATA_TABLE);
  }
  if (!IsValidApDataType (ApCommDataBuffer->DataType)) {
    return IHISI_INVALID_PARAMETER;
  }

  Status = SetApandBiosCommDataBuffer(ApCommDataBuffer, BiosCommDataBuffer);
  if (EFI_ERROR(Status)) {
    FreePool (BiosCommDataBuffer);
    return Status;
  }

  OemSvcStatus = SmmCsSvcIhisiOemExtCommunication(ApCommDataBuffer, BiosCommDataBuffer);
  Status = OemSvcStatus;
  switch (OemSvcStatus) {
    case EFI_SUCCESS:
      Status = IHISI_END_FUNCTION_CHAIN;
      break;

    case EFI_UNSUPPORTED:
    case EFI_MEDIA_CHANGED:
      if ((ApCommDataBuffer->DataType == Oa30ReadWrite) ||
          (ApCommDataBuffer->DataType == Oa30Erase) ||
          (ApCommDataBuffer->DataType == Oa30PopulateHeader) ||
          (ApCommDataBuffer->DataType == Oa30DePopulateHeader) ||
          (ApCommDataBuffer->DataType == LogoUpdate) ||
          (ApCommDataBuffer->DataType == SignedLogoUpdate)) {
        Status = IHISI_SUCCESS;
      }
      break;
  }
  if (OemSvcStatus == EFI_MEDIA_CHANGED) {
    CopyMem (ApCommDataBuffer, BiosCommDataBuffer, sizeof (BIOS_COMMUNICATION_DATA_TABLE));
  }

  FreePool (BiosCommDataBuffer);

  return Status;

}

/**
  AH=42h, OEM Extra Data Write.

  Hook chipset service SmmCsSvcIhisiOemExtDataWrite.

  @retval EFI_SUCCESS            OEM Extra Data Write successful.
  @return Other                  OEM Extra Data Write failed.
**/
EFI_STATUS
EFIAPI
ChipsetOemExtraDataWrite (
  VOID
  )
{
  EFI_STATUS          Status;
  UINTN               WriteSize;
  UINT8               ShutdownMode;
  UINTN               RomBaseAddress;
  UINT8               *WriteDataBuffer;
  UINT32              Ecx;

  WriteDataBuffer = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  WriteSize       = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  RomBaseAddress  = (UINTN) (IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX) >> 8);
  ShutdownMode    = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  Status = SmmCsSvcIhisiOemExtDataWrite(mOemExtraDataType, WriteDataBuffer, &WriteSize, &RomBaseAddress, ShutdownMode);
  switch (Status) {
    case EFI_SUCCESS:
      Status = IHISI_END_FUNCTION_CHAIN;
      break;

    case EFI_UNSUPPORTED:
      if (mOemExtraDataType == Oa30ReadWrite) {
        mRomBaseAddress = (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionMsdmGuid, 1);
        mRomSize = (UINTN) FdmGetNAtSize (&gH2OFlashMapRegionMsdmGuid, 1);
      } else if (mOemExtraDataType == LogoUpdate || mOemExtraDataType == SignedLogoUpdate) {
        //
        // logo region to be written. If the logo region is different with kernel. Please specify logo region here.
        // If the logo region size is 0, the logo update function will return failed.
        //
        mRomBaseAddress = (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionLogoGuid, 1);
        mRomSize = (UINTN) FdmGetNAtSize (&gH2OFlashMapRegionLogoGuid, 1);
      }
    case EFI_MEDIA_CHANGED:
      IhisiProtWriteCpuReg32(EFI_SMM_SAVE_STATE_REGISTER_RDI, (UINT32)WriteSize);
      Ecx = IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
      Ecx &=0xFFFF00FF;
      Ecx |= (RomBaseAddress << 8);
      IhisiProtWriteCpuReg32(EFI_SMM_SAVE_STATE_REGISTER_RCX, Ecx);
      Status = IHISI_SUCCESS;
      break;

    default:
      break;
  }
  return Status;
}

/**
  AH=47h, OEM Extra Data Read.

  Hook chipset service SmmCsSvcIhisiOemExtDataRead.

  @retval EFI_SUCCESS            OEM Extra Data Read successful.
  @return Other                  OEM Extra Data Read failed.
**/
EFI_STATUS
EFIAPI
ChipsetOemExtraDataRead (
  VOID
  )
{
  EFI_STATUS                       Status;
  UINTN                            ReadSize;
  UINTN                            RomBaseAddress;
  UINT8                            *ReadDataBuffer;

  ReadSize = 0;
  RomBaseAddress = 0;
  ReadDataBuffer = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  ReadSize = *(UINT32 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);


  Status = SmmCsSvcIhisiOemExtDataRead(mOemExtraDataType, ReadDataBuffer, &ReadSize, &RomBaseAddress);

  //
  // Pass Chipset update parameter to kernel
  //
  switch (Status) {
    case EFI_SUCCESS:
      Status = IHISI_END_FUNCTION_CHAIN;
      break;

    case EFI_MEDIA_CHANGED:
      mRomBaseAddress = RomBaseAddress;
      mRomSize = ReadSize;
      CopyMem ((VOID *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI), &ReadSize, sizeof(UINT32));
      Status = IHISI_SUCCESS;
      break;

    case EFI_UNSUPPORTED:
      if (mOemExtraDataType == Oa30ReadWrite) {
        mRomBaseAddress = (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionMsdmGuid, 1);
        mRomSize = (UINTN) FdmGetNAtSize (&gH2OFlashMapRegionMsdmGuid, 1);
      }
      Status = IHISI_SUCCESS;
      break;

    default:
      Status = IHISI_SUCCESS;
      break;
  }

  return Status;

}

/**
  AH=42h, OEM Extra Data Write.

  Hook chipset service.SmmCsSvcIhisiFbtsShutDown

  @retval EFI_SUCCESS            OEM Extra Data Write successful.
  @return Other                  OEM Extra Data Write failed.
**/
EFI_STATUS
EFIAPI
ChipsetOemExtraDataDosShutdownMode (
  VOID
  )
{
  UINT8           ShutdownMode;
  EFI_STATUS      Status;

  Status = EFI_SUCCESS;
  ShutdownMode    = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  switch (ShutdownMode) {
    case DosReboot:
      Status = SmmCsSvcIhisiFbtsReboot ();
      break;

    case DosShutdown:
      Status = SmmCsSvcIhisiFbtsShutDown ();
      break;

    case DoNothing:
    case WindowsReboot:
    case WindowsShutdown:
    case ContinueToFlash:
      Status = EFI_SUCCESS;
      break;
    default:
      Status = EFI_INVALID_PARAMETER;
      break;
  }

  return Status;
}

/**
  AH=41h, Hook OemService OEM Extra Data Communication

  01h = VBIOS
  @retval EFI_SUCCESS    Process OEM extra data communication successful.
  @return Other          Process OEM extra data communication failed.
**/
EFI_STATUS
EFIAPI
OemIhisiS41T1Vbios (
  VOID
  )
{
  EFI_STATUS                        Status;
  BIOS_COMMUNICATION_DATA_TABLE     *BiosCommDataBuffer;
  AP_COMMUNICATION_DATA_TABLE       *ApCommDataBuffer;
  UINTN                             BufferSize;

  ApCommDataBuffer   = (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  if (!IsValidApDataType (ApCommDataBuffer->DataType)) {
    return IHISI_INVALID_PARAMETER;
  }

  BiosCommDataBuffer = AllocatePool (sizeof (BIOS_COMMUNICATION_DATA_TABLE));
  if (BiosCommDataBuffer == NULL) {
    return IHISI_OUT_OF_RESOURCES;
  }
  Status = SetApandBiosCommDataBuffer(ApCommDataBuffer, BiosCommDataBuffer);
  if (EFI_ERROR(Status)) {
    FreePool (BiosCommDataBuffer);
    return Status;
  }

  Status = IHISI_SUCCESS;
  if (ApCommDataBuffer->DataType == Vbios) {
    BufferSize = ApCommDataBuffer->StructureSize;
    if (BufferSize < sizeof(BIOS_COMMUNICATION_DATA_TABLE)) {
      BufferSize = sizeof(BIOS_COMMUNICATION_DATA_TABLE);
    }
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS41T1VbiosFunction \n"));
    Status = OemSvcIhisiS41T1VbiosFunction (ApCommDataBuffer, BiosCommDataBuffer);
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS41T1VbiosFunction Status: %r\n", Status));
    if (Status == EFI_MEDIA_CHANGED) {
      CopyMem (ApCommDataBuffer, BiosCommDataBuffer, sizeof (BIOS_COMMUNICATION_DATA_TABLE));
    }
    Status = IHISI_END_FUNCTION_CHAIN;
  }

  FreePool (BiosCommDataBuffer);

  return Status;
}

/**
  AH=41h, Oem OEM Extra Data Communication

  54h = LogoUpdate
  @retval EFI_SUCCESS    Process OEM extra data communication successful.
  @return Other          Process OEM extra data communication failed.
**/
EFI_STATUS
EFIAPI
OemIhisiS41T54LogoUpdate (
  VOID
  )
{
  EFI_STATUS                        Status;
  BIOS_COMMUNICATION_DATA_TABLE     *BiosCommDataBuffer;
  AP_COMMUNICATION_DATA_TABLE       *ApCommDataBuffer;
  UINTN                             BufferSize;

  ApCommDataBuffer   = (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (!IsValidApDataType (ApCommDataBuffer->DataType)) {
    return IHISI_INVALID_PARAMETER;
  }
  BiosCommDataBuffer = AllocatePool (sizeof (BIOS_COMMUNICATION_DATA_TABLE));
  if (BiosCommDataBuffer == NULL) {
    return IHISI_OUT_OF_RESOURCES;
  }
  Status = SetApandBiosCommDataBuffer(ApCommDataBuffer, BiosCommDataBuffer);
  if (EFI_ERROR(Status)) {
    FreePool (BiosCommDataBuffer);
    return Status;
  }

  Status = IHISI_SUCCESS;
  if (ApCommDataBuffer->DataType == LogoUpdate || ApCommDataBuffer->DataType == SignedLogoUpdate) {
    mOemCommunicationInfo.DataType       = ApCommDataBuffer->DataType;
    mOemCommunicationInfo.OemDataSize    = ApCommDataBuffer->PhysicalDataSize;
    mOemCommunicationInfo.OemReceivedSize = 0;
    if (mOemCommunicationInfo.OemDataBuffer != NULL) {
      FreePool (mOemCommunicationInfo.OemDataBuffer);
      mOemCommunicationInfo.OemDataBuffer = NULL;
    }
    BufferSize = ApCommDataBuffer->StructureSize;
    if (BufferSize < sizeof(BIOS_COMMUNICATION_DATA_TABLE)) {
      BufferSize = sizeof(BIOS_COMMUNICATION_DATA_TABLE);
    }
    if (ApCommDataBuffer->DataType == SignedLogoUpdate) {
      CopyMem (ApCommDataBuffer, BiosCommDataBuffer, sizeof (BIOS_COMMUNICATION_DATA_TABLE));
    }
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS41T54LogoUpdateFunction \n"));
    Status = OemSvcIhisiS41T54LogoUpdateFunction (ApCommDataBuffer, BiosCommDataBuffer);
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS41T54LogoUpdateFunction Status: %r\n", Status));
    if (Status == EFI_MEDIA_CHANGED) {
      CopyMem (ApCommDataBuffer, BiosCommDataBuffer, sizeof (BIOS_COMMUNICATION_DATA_TABLE));
    }
    Status = IHISI_END_FUNCTION_CHAIN;
  }

  FreePool (BiosCommDataBuffer);

  return Status;
}

/**
  AH=41h, Oem OEM Extra Data Communication

  55h = CheckBiosSignBySystemBios
  @retval EFI_SUCCESS    Process OEM extra data communication successful.
  @return Other          Process OEM extra data communication failed.
**/
EFI_STATUS
EFIAPI
OemIhisiS41T55CheckBiosSignBySystemBios (
  VOID
  )
{
  EFI_STATUS                        Status;
  BIOS_COMMUNICATION_DATA_TABLE     *BiosCommDataBuffer;
  AP_COMMUNICATION_DATA_TABLE       *ApCommDataBuffer;
  UINTN                             BufferSize;

  ApCommDataBuffer   = (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (!IsValidApDataType (ApCommDataBuffer->DataType)) {
    return IHISI_INVALID_PARAMETER;
  }
  BiosCommDataBuffer = AllocatePool (sizeof (BIOS_COMMUNICATION_DATA_TABLE));
  if (BiosCommDataBuffer == NULL) {
    return IHISI_OUT_OF_RESOURCES;
  }
  Status = SetApandBiosCommDataBuffer(ApCommDataBuffer, BiosCommDataBuffer);
  if (EFI_ERROR(Status)) {
    FreePool (BiosCommDataBuffer);
    return Status;
  }

  Status = EFI_SUCCESS;
  if (ApCommDataBuffer->DataType == CheckBiosSignBySystemBios) {
    BufferSize = ApCommDataBuffer->StructureSize;
    if (BufferSize < sizeof(BIOS_COMMUNICATION_DATA_TABLE)) {
      BufferSize = sizeof(BIOS_COMMUNICATION_DATA_TABLE);
    }
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS41T55CheckBiosSignBySystemBiosFunction \n"));
    Status = OemSvcIhisiS41T55CheckBiosSignBySystemBiosFunction (ApCommDataBuffer, BiosCommDataBuffer);
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS41T55CheckBiosSignBySystemBiosFunction Status: %r\n", Status));
    if (Status == EFI_MEDIA_CHANGED) {
      CopyMem (ApCommDataBuffer, BiosCommDataBuffer, sizeof (BIOS_COMMUNICATION_DATA_TABLE));
    }
    Status = IHISI_END_FUNCTION_CHAIN;
  }

  FreePool (BiosCommDataBuffer);

  return Status;
}

/**
  AH=41h, Oem OEM Extra Data Communication

  50h = Oa30ReadWrite
  @retval EFI_SUCCESS    Process OEM extra data communication successful.
  @return Other          Process OEM extra data communication failed.
**/
EFI_STATUS
EFIAPI
OemIhisiS41T50a30ReadWrite (
  VOID
  )
{
  EFI_STATUS                        Status;
  BIOS_COMMUNICATION_DATA_TABLE     *BiosCommDataBuffer;
  AP_COMMUNICATION_DATA_TABLE       *ApCommDataBuffer;
  UINTN                             BufferSize;

  ApCommDataBuffer   = (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (!IsValidApDataType (ApCommDataBuffer->DataType)) {
    return IHISI_INVALID_PARAMETER;
  }
  BiosCommDataBuffer = AllocatePool (sizeof (BIOS_COMMUNICATION_DATA_TABLE));
  if (BiosCommDataBuffer == NULL) {
    return IHISI_OUT_OF_RESOURCES;
  }
  BufferSize = ApCommDataBuffer->StructureSize;
  if (BufferSize < sizeof(BIOS_COMMUNICATION_DATA_TABLE)) {
    BufferSize = sizeof(BIOS_COMMUNICATION_DATA_TABLE);
  }
  Status = SetApandBiosCommDataBuffer(ApCommDataBuffer, BiosCommDataBuffer);
  if (EFI_ERROR(Status)) {
    FreePool (BiosCommDataBuffer);
    return Status;
  }

  Status = IHISI_SUCCESS;
  if (ApCommDataBuffer->DataType == Oa30ReadWrite) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS41T50HookOa30ReadWriteFunction \n"));
    Status = OemSvcIhisiS41T50HookOa30ReadWriteFunction (ApCommDataBuffer, BiosCommDataBuffer);
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS41T50HookOa30ReadWriteFunction Status: %r\n", Status));
    if (Status == EFI_MEDIA_CHANGED) {
      CopyMem (ApCommDataBuffer, BiosCommDataBuffer, sizeof (BIOS_COMMUNICATION_DATA_TABLE));
    }
    Status = IHISI_END_FUNCTION_CHAIN;
  }
  FreePool (BiosCommDataBuffer);

  return Status;
}

/**
  AH=41h, Oem OEM Extra Data Communication

  Resserved
  @retval EFI_SUCCESS    Process OEM extra data communication successful.
  @return Other          Process OEM extra data communication failed.
**/
EFI_STATUS
EFIAPI
OemIhisiS41ReservedFunction (
  VOID
  )
{
  EFI_STATUS                        Status;
  BIOS_COMMUNICATION_DATA_TABLE     *BiosCommDataBuffer;
  AP_COMMUNICATION_DATA_TABLE       *ApCommDataBuffer;
  UINTN                             BufferSize;

  ApCommDataBuffer   = (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (!IsValidApDataType (ApCommDataBuffer->DataType)) {
    return IHISI_INVALID_PARAMETER;
  }
  BiosCommDataBuffer = AllocatePool (sizeof (BIOS_COMMUNICATION_DATA_TABLE));
  if (BiosCommDataBuffer == NULL) {
    return IHISI_OUT_OF_RESOURCES;
  }
  BufferSize = ApCommDataBuffer->StructureSize;
  if (BufferSize < sizeof(BIOS_COMMUNICATION_DATA_TABLE)) {
    BufferSize = sizeof(BIOS_COMMUNICATION_DATA_TABLE);
  }
  Status = SetApandBiosCommDataBuffer(ApCommDataBuffer, BiosCommDataBuffer);
  if (EFI_ERROR(Status)) {
    FreePool (BiosCommDataBuffer);
    return Status;
  }

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS41ReservedFunction \n"));
  Status = OemSvcIhisiS41ReservedFunction (ApCommDataBuffer, BiosCommDataBuffer);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS41ReservedFunction Status: %r\n", Status));

  if (Status == EFI_MEDIA_CHANGED) {
    CopyMem (ApCommDataBuffer, BiosCommDataBuffer, sizeof (BIOS_COMMUNICATION_DATA_TABLE));
  }
  FreePool (BiosCommDataBuffer);

  return EFI_SUCCESS;
}

/**
  AH=41h,OEM Extra Data Communication

  Save input registers;

  @retval EFI_SUCCESS    Process OEM extra data communication successful.
  @return Other          Process OEM extra data communication failed.
**/
EFI_STATUS
EFIAPI
KernelCommunicationSaveRegs (
  VOID
  )
{
  AP_COMMUNICATION_DATA_TABLE      *ApCommDataBuffer;
  UINTN                            BufferSize;

  mRomBaseAddress = 0;
  mRomSize        = 0;
  ApCommDataBuffer = (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  if (ApCommDataBuffer->Signature != AP_COMMUNICATION_SIGNATURE) {
    return IHISI_UNSUPPORTED_FUNCTION;
  }

  BufferSize = ApCommDataBuffer->StructureSize;
  if (BufferSize < sizeof(AP_COMMUNICATION_DATA_TABLE)) {
    BufferSize = sizeof(AP_COMMUNICATION_DATA_TABLE);
  }
  if (!IsValidApDataType (ApCommDataBuffer->DataType)) {
    return IHISI_INVALID_PARAMETER;
  }
  mOemExtraDataType = ApCommDataBuffer->DataType;

  CopyMem(&mApCommDataBuffer, ApCommDataBuffer, sizeof(AP_COMMUNICATION_DATA_TABLE));

  IhisiErrorCodeHandler((UINT32)IHISI_SUCCESS);

  return IHISI_SUCCESS;
}

/**
  AH=41h, OEM Extra Data Communication

  @retval EFI_SUCCESS    Process OEM extra data communication successful.
  @return Other          Process OEM extra data communication failed.
**/
EFI_STATUS
EFIAPI
KernelOemExtraDataCommunication (
  VOID
  )
{
  EFI_STATUS                        Status;
  BIOS_COMMUNICATION_DATA_TABLE     *BiosCommDataBuffer;
  AP_COMMUNICATION_DATA_TABLE       *ApCommDataBuffer;
  UINTN                             BufferSize;

  ApCommDataBuffer   = (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (!IsValidApDataType (ApCommDataBuffer->DataType)) {
    return IHISI_INVALID_PARAMETER;
  }
  BiosCommDataBuffer = AllocatePool (sizeof (BIOS_COMMUNICATION_DATA_TABLE));
  if (BiosCommDataBuffer == NULL) {
    return IHISI_OUT_OF_RESOURCES;
  }
  BufferSize = ApCommDataBuffer->StructureSize;
  if (BufferSize < sizeof(BIOS_COMMUNICATION_DATA_TABLE)) {
    BufferSize = sizeof(BIOS_COMMUNICATION_DATA_TABLE);
  }

  Status = SetApandBiosCommDataBuffer(ApCommDataBuffer, BiosCommDataBuffer);
  if (EFI_ERROR(Status)) {
    FreePool (BiosCommDataBuffer);
    return Status;
  }

  switch (ApCommDataBuffer->DataType) {
    case Oa30ReadWrite:
      Status = Oa30ReadWriteFunction (ApCommDataBuffer, BiosCommDataBuffer);
      break;

    case Oa30Erase:
      Status = Oa30EraseFunction (ApCommDataBuffer, BiosCommDataBuffer);
     break;

    case Oa30PopulateHeader:
      Status = Oa30PopulateHeaderFunction (ApCommDataBuffer, BiosCommDataBuffer);
      break;

    case Oa30DePopulateHeader:
      Status = Oa30DePopulateHeaderFunction (ApCommDataBuffer, BiosCommDataBuffer);
      break;

    default:
      Status = EFI_SUCCESS;
      break;
  }

  if (EFI_ERROR (Status)) {
    mOemExtraDataType = 0;
    FreePool (BiosCommDataBuffer);
    return Status;
  }

  CopyMem (ApCommDataBuffer, BiosCommDataBuffer, sizeof (BIOS_COMMUNICATION_DATA_TABLE));

  FreePool (BiosCommDataBuffer);
  return IHISI_SUCCESS;
}

/**
  Free OEM communication  information resource.

**/
STATIC
VOID
FreeOemCommunicationInfoResource (
  VOID
  )
{
  mOemCommunicationInfo.DataType = 0;
  mOemCommunicationInfo.OemDataSize = 0;
  mOemCommunicationInfo.OemReceivedSize = 0;
  if (mOemCommunicationInfo.OemDataBuffer != NULL) {
    FreePool (mOemCommunicationInfo.OemDataBuffer);
    mOemCommunicationInfo.OemDataBuffer = NULL;
  }
}

/**
  Check if it is OEMSFOEMExDataWrite normal sub function.

**/
BOOLEAN
IsOEMSFOEMExDataWriteNormalSubFunction (
  IHISI_SMI                    Cmd
  )
{
  if (Cmd != OEMSFOEMExDataWrite) {
    return FALSE;
  }
  if (mOemExtraDataType != SignedLogoUpdate) {
    return FALSE;
  }
  return TRUE;
}

/**
  AH=42h, OEM Extra Data Write.

  @retval EFI_SUCCESS            OEM Extra Data Write successful.
  @return Other                  OEM Extra Data Write failed.
**/
EFI_STATUS
EFIAPI
KernelOemExtraDataWrite (
  VOID
  )
{
  EFI_STATUS                         Status;
  UINTN                              WriteSize;
  UINTN                              RomBaseAddress;
  UINT8                              *WriteDataBuffer;
  EFI_FIRMWARE_IMAGE_AUTHENTICATION  *Image;
  UINTN                              DataSize;
  UINT8                              *DataBuffer;
  UINT32                             AllHeaderSize;
  UINT8                              *WorkdingBuffer;


  DataBuffer      = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  DataSize        = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  RomBaseAddress  = (UINTN) (IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX) >> 8);

  switch (mOemExtraDataType) {
    case Oa30ReadWrite:
      if (DataSize > mRomSize) {
        Status = IHISI_INVALID_PARAMETER;
      } else {
        RomBaseAddress = mRomBaseAddress;
        Status = Oa30DataWrite(DataBuffer, DataSize, RomBaseAddress);
      }
      break;

    case SignedLogoUpdate:

      RomBaseAddress = mRomBaseAddress;
      if ((DataSize > mRomSize) || (RomBaseAddress < mRomBaseAddress) || (RomBaseAddress >= (mRomBaseAddress + mRomSize))) {
        Status = IHISI_INVALID_PARAMETER;
        break;
      }
      if (mOemCommunicationInfo.OemDataSize == 0 || mOemCommunicationInfo.OemDataSize > mRomSize ||
          mOemCommunicationInfo.DataType != SignedLogoUpdate) {
        Status = IHISI_INVALID_PARAMETER;
        break;
      }

      if (mOemCommunicationInfo.OemDataSize > DataSize) {
        if (mOemCommunicationInfo.OemReceivedSize == 0) {
          if (mOemCommunicationInfo.OemDataBuffer != NULL) {
            FreePool (mOemCommunicationInfo.OemDataBuffer);
          }
          mOemCommunicationInfo.OemDataBuffer = AllocatePool (mOemCommunicationInfo.OemDataSize);
          if (mOemCommunicationInfo.OemDataBuffer == NULL) {
            FreeOemCommunicationInfoResource ();
            Status = IHISI_OUT_OF_RESOURCES;
            break;
          }
        }
        if (mOemCommunicationInfo.OemReceivedSize + DataSize > mOemCommunicationInfo.OemDataSize) {
            FreeOemCommunicationInfoResource ();
            Status = IHISI_INVALID_PARAMETER;
            break;
        }
        WorkdingBuffer = mOemCommunicationInfo.OemDataBuffer + mOemCommunicationInfo.OemReceivedSize;
        CopyMem (WorkdingBuffer, DataBuffer, DataSize);
        mOemCommunicationInfo.OemReceivedSize += DataSize;
        if (mOemCommunicationInfo.OemReceivedSize < mOemCommunicationInfo.OemDataSize) {
          Status = IHISI_SUCCESS;
          break;
        }
        DataBuffer = mOemCommunicationInfo.OemDataBuffer;
        DataSize   = mOemCommunicationInfo.OemDataSize;
      }
      //
      // Payload Image Authentication
      //
      Status = IHISI_ACCESS_PROHIBITED;
      Status = AuthenticateFmpImage (
                 (EFI_FIRMWARE_IMAGE_AUTHENTICATION *)DataBuffer,
                 DataSize,
                 mCertList,
                 mCertSize
                 );
      if (!EFI_ERROR (Status)) {
        Image = (EFI_FIRMWARE_IMAGE_AUTHENTICATION *)DataBuffer;
        AllHeaderSize = GetAllHeaderSize ((EFI_FIRMWARE_IMAGE_AUTHENTICATION *)Image, DataSize);
        if (AllHeaderSize == 0) {
          Status = IHISI_ACCESS_PROHIBITED;
          break;
        }
        WriteSize = DataSize - AllHeaderSize;
        WriteDataBuffer = DataBuffer + AllHeaderSize;
        Status = LogoDataWrite (WriteDataBuffer, WriteSize, RomBaseAddress);
        break;
      }
      FreeOemCommunicationInfoResource ();
      break;

    default:
      Status = IHISI_SUCCESS;
      break;
  }

  return Status;
}

/**
  AH=47h, OEM Extra Data Read.

  @retval EFI_SUCCESS            OEM Extra Data Read successful.
  @return Other                  OEM Extra Data Read failed.
**/
EFI_STATUS
EFIAPI
KernelOemExtraDataRead (
  VOID
  )
{
  EFI_STATUS                       Status;
  UINTN                            ReadSize;
  UINTN                            RomBaseAddress;
  UINT8                            *ReadDataBuffer;

  ReadSize = 0;
  RomBaseAddress = mRomBaseAddress;
  ReadDataBuffer = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  ReadSize = *(UINT32 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);


  switch (mOemExtraDataType) {
    case Oa30ReadWrite:
      Status = Oa30DataRead(ReadDataBuffer, &ReadSize, RomBaseAddress);
      break;

    default:
      Status = EFI_SUCCESS;
      break;
  }

  CopyMem ((VOID *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI), &ReadSize, sizeof(UINT32));

  return Status;
}

EFI_STATUS
InstallOemExtraDataCommunicationServices (
  VOID
  )
{
  EFI_STATUS              Status;
  IHISI_REGISTER_TABLE    *SubFuncTable;
  UINT16                  TableCount;

  SubFuncTable = OEM_EXT_COMMON_REGISTER_TABLE;
  TableCount = sizeof(OEM_EXT_COMMON_REGISTER_TABLE)/sizeof(OEM_EXT_COMMON_REGISTER_TABLE[0]);
  Status = RegisterIhisiSubFunction (SubFuncTable, TableCount);
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }
  //
  // Get secure flash certificate to verify signed logo as kernel default behavior.
  // If project wants to use other key to verify signed logo, need use other to check signed logo.
  //
  GetCertificateData ();
  return EFI_SUCCESS;
}

