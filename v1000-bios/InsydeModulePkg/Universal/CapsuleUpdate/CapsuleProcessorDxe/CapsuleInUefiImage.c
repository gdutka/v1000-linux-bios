/** @file
  UEFI image capsule processor on firmware management protocol implementation

;******************************************************************************
;* Copyright (c) 2012 - 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "CapsuleProcessorDxe.h"

#define ESRT_LAST_ATTEMPT_VERSION       L"EsrtLastAttemptVersion"
#define ESRT_LAST_ATTEMPT_STATUS        L"EsrtLastAttemptStatus"
#define CAPSULE_RESULT_VARIABLE         L"Capsule0000"
#define CAPSULE_RESULT_MAX_VARIABLE     L"CapsuleMax"
#define CAPSULE_RESULT_LAST_VARIABLE    L"CapsuleLast"
#define EFI_CAPSULE_REPORT_GUID         { 0x39b68c46, 0xf7fb, 0x441b, { 0xb6, 0xec, 0x16, 0xb0, 0xf6, 0x98, 0x21, 0xf3 }}
#define EFI_SIGNATURE_OWNER_GUID        { 0x79736E49, 0x6564, 0xBBAA, { 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x23, 0x45, 0x67 }}
#define VERSION_STRING_NOT_AVAILABLE    L"VERSION STRING NOT AVAILABLE"

STATIC  CAPSULE_FMP_ENTRY               mCapsuleFmpEntry;
EFI_RESET_SYSTEM                        mOriginalResetSystemPtr;

typedef struct {
  EFI_SIGNATURE_LIST    SignatureListHeader;
  EFI_SIGNATURE_DATA    SignatureData;
} CERTIFICATE_DATA;

typedef struct {
  UINT32                VariableTotalSize;
  UINT32                Reserved;       //for alignment
  EFI_GUID              CapsuleGuid;
  EFI_TIME              CapsuleProcessed;
  EFI_STATUS            CapsuleStatus;
} EFI_CAPSULE_RESULT_VARIABLE_HEADER;

VOID
EFIAPI
DummyResetSystem (
  IN EFI_RESET_TYPE             ResetType,
  IN EFI_STATUS                 ResetStatus,
  IN UINTN                      DataSize,
  IN VOID                       *ResetData OPTIONAL
  )
{
  return;
}

/**
  Hook gRT->ResetSystem to avoid system reset cause firmware update fail

  @param  TRUE      Hook gRT->ResetSystem
          FALSE     Restore gRT->ResetSystem

  @retval None

**/
VOID
HookSystemResetRoutine (
  IN BOOLEAN                    ToHook
  )
{

  if (ToHook) {
    mOriginalResetSystemPtr = gRT->ResetSystem;
    gRT->ResetSystem = DummyResetSystem;
  } else {
    gRT->ResetSystem = mOriginalResetSystemPtr;
  }
}

/**
  Convert ESRT status to FMP CheckImage result

  @param[in] Status                     The value of ESRT status

  @retval IMAGE_UPDATABLE_VALID         Indicates SetImage() will accept the new image and update
                                        the device with the new image.
  @retval IMAGE_UPDATABLE_INVALID       Indicates SetImage() will reject the new image. No additional
                                        information is provided for the rejection
  @retval IMAGE_UPDATABLE_INVALID_TYPE  Indicates SetImage() will reject the new image. The rejection
                                        is due to the new image is not a firmware image recognized for this device
  @retval IMAGE_UPDATABLE_INVALID_OLD   indicates SetImage() will reject the new image. The rejection is due to
                                        the new image version is older than the current firmware image version in
                                        the device
**/
STATIC
UINT32
ConvertEsrtStatusToCheckImageResult (
  IN     ESRT_STATUS    EsrtStatus
  )
{
  UINT32 CheckImageResult;

  switch (EsrtStatus) {
    case ESRT_SUCCESS:
      CheckImageResult = IMAGE_UPDATABLE_VALID;
      break;
    case ESRT_ERROR_INVALID_IMAGE_FORMAT:
      CheckImageResult = IMAGE_UPDATABLE_INVALID_TYPE;
      break;
    case ESRT_ERROR_INCORRECT_VERSION:
      CheckImageResult = IMAGE_UPDATABLE_INVALID_OLD;
      break;
    default:
      CheckImageResult = IMAGE_UPDATABLE_INVALID;
      break;
  }
  return CheckImageResult;
}

/**
  Convert EFI Status to ESRT status

  @param[in] Status     The value of EFI status

  @retval ESRT_SUCCESS
  @retval ESRT_ERROR_UNSUCCESSFUL
  @retval ESRT_ERROR_INSUFFICIENT_RESOURCES
  @retval ESRT_ERROR_INCORRECT_VERSION
  @retval ESRT_ERROR_INVALID_IMAGE_FORMAT
  @retval ESRT_ERROR_AUTHENTICATION
  @retval ESRT_ERROR_AC_NOT_CONNECTED
  @retval ESRT_ERROR_INSUFFICIENT_BATTERY
**/
STATIC
ESRT_STATUS
ConvertEfiStatusToEsrtStatus (
  IN     EFI_STATUS     Status
  )
{
  ESRT_STATUS EsrtStatus;

  switch (Status) {
    case EFI_SUCCESS:
      EsrtStatus = ESRT_SUCCESS;
      break;
    case EFI_INCOMPATIBLE_VERSION:
      EsrtStatus = ESRT_ERROR_INCORRECT_VERSION;
      break;
    case EFI_OUT_OF_RESOURCES:
    case EFI_VOLUME_FULL:
      EsrtStatus = ESRT_ERROR_INSUFFICIENT_RESOURCES;
      break;
    case EFI_UNSUPPORTED:
    case EFI_LOAD_ERROR:
      EsrtStatus = ESRT_ERROR_INVALID_IMAGE_FORMAT;
      break;
    case EFI_SECURITY_VIOLATION:
      EsrtStatus = ESRT_ERROR_AUTHENTICATION;
      break;
    default:
      EsrtStatus = ESRT_ERROR_UNSUCCESSFUL;
      break;
  }
  return EsrtStatus;
}

/**
  Convert EFI Status to ESRT status

  @param[in] Status     The value of EFI status

  @retval ESRT_SUCCESS
  @retval ESRT_ERROR_UNSUCCESSFUL
  @retval ESRT_ERROR_INSUFFICIENT_RESOURCES
  @retval ESRT_ERROR_INCORRECT_VERSION
  @retval ESRT_ERROR_INVALID_IMAGE_FORMAT
  @retval ESRT_ERROR_AUTHENTICATION
  @retval ESRT_ERROR_AC_NOT_CONNECTED
  @retval ESRT_ERROR_INSUFFICIENT_BATTERY
**/
STATIC
EFI_STATUS
ConvertEsrtStatusToEfiStatus (
  IN     ESRT_STATUS    EsrtStatus
  )
{
  EFI_STATUS Status;

  switch (EsrtStatus) {
    case ESRT_SUCCESS:
      Status = EFI_SUCCESS;
      break;
    case ESRT_ERROR_INSUFFICIENT_RESOURCES:
      Status = EFI_OUT_OF_RESOURCES;
      break;
    case ESRT_ERROR_INCORRECT_VERSION:
    case ESRT_ERROR_INVALID_IMAGE_FORMAT:
      Status = EFI_INVALID_PARAMETER;
      break;
    case ESRT_ERROR_AUTHENTICATION:
      Status = EFI_ACCESS_DENIED;
      break;
    default:
      Status = EFI_ABORTED;
      break;
  }
  return Status;
}

/**
  Update capsule status onto corresponding variables, including ESRT status
  and Capsule#### and CapsuleLast variables according to UEFI 2.4B 7.5.6

  @param[in] CapsuleGuid        The pointer of capsule GUID
  @param[in] AttemptVersion     The value of last attempt version
  @param[in] AttemptStatus      The value of last attempt status

**/
STATIC
VOID
UpdateCapsuleStatus (
  IN     EFI_GUID       *CapsuleGuid,
  IN     UINT32         AttemptVersion,
  IN     ESRT_STATUS    AttemptStatus,
  IN     EFI_STATUS     CapsuleStatus
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Size;
  EFI_CAPSULE_RESULT_VARIABLE_HEADER    CapsuleResult;
  UINTN                                 CapsuleResultCounter;
  UINTN                                 CapsuleResultMaxCounter;
  CHAR16                                CapsuleResultVariableName[16];
  EFI_GUID                              CapsuleResultVariableGuid = EFI_CAPSULE_REPORT_GUID;
  //
  // Update ESRT version and status
  //
  CommonSetVariable (
    ESRT_LAST_ATTEMPT_VERSION,
    CapsuleGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    sizeof(UINT32),
    &AttemptVersion
    );
  CommonSetVariable (
    ESRT_LAST_ATTEMPT_STATUS,
    CapsuleGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    sizeof(ESRT_STATUS),
    &AttemptStatus
    );
  //
  // Calculate the variable name for UEFI 2.4B 7.5.6
  //
  if ((PcdGet64 (PcdOsIndicationsSupported) & EFI_OS_INDICATIONS_CAPSULE_RESULT_VAR_SUPPORTED) == 0) return;
  CapsuleResultMaxCounter = (UINTN)PcdGet16 (PcdCapsuleMaxResult);
  Size = sizeof (CHAR16) * 11;
  Status = CommonGetVariable (
             CAPSULE_RESULT_MAX_VARIABLE,
             &CapsuleResultVariableGuid,
             &Size,
             &CapsuleResultVariableName
             );
  if (Status == EFI_NOT_FOUND) {
    UnicodeSPrint (CapsuleResultVariableName, sizeof(CapsuleResultVariableName), L"Capsule%04x", CapsuleResultMaxCounter);
    CommonSetVariable (
      CAPSULE_RESULT_MAX_VARIABLE,
      &CapsuleResultVariableGuid,
      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
      Size,
      &CapsuleResultVariableName
      );
  }
  StrCpyS (CapsuleResultVariableName, sizeof(CapsuleResultVariableName) / sizeof(CHAR16), CAPSULE_RESULT_VARIABLE);
  Size = sizeof (CHAR16) * 11;
  Status = CommonGetVariable (
             CAPSULE_RESULT_LAST_VARIABLE,
             &CapsuleResultVariableGuid,
             &Size,
             &CapsuleResultVariableName
             );
  if (!EFI_ERROR (Status)) {
    CapsuleResultCounter = StrHexToUintn (CapsuleResultVariableName + 7);
    if (CapsuleResultCounter == CapsuleResultMaxCounter) {
      CapsuleResultCounter = 0;
    } else {
      CapsuleResultCounter ++;
    }
    UnicodeSPrint (CapsuleResultVariableName, sizeof(CapsuleResultVariableName), L"Capsule%04x", CapsuleResultCounter);
  }
  //
  // Update UEFI capsule result status
  //
  CapsuleResult.VariableTotalSize = sizeof(EFI_CAPSULE_RESULT_VARIABLE_HEADER);
  CapsuleResult.Reserved          = 0;
  CopyGuid (&CapsuleResult.CapsuleGuid, CapsuleGuid);
  gRT->GetTime (&CapsuleResult.CapsuleProcessed, NULL);
  CapsuleResult.CapsuleStatus     = CapsuleStatus;
  CommonSetVariable (
    CapsuleResultVariableName,
    &CapsuleResultVariableGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    CapsuleResult.VariableTotalSize,
    &CapsuleResult
    );
  CommonSetVariable (
    CAPSULE_RESULT_LAST_VARIABLE,
    &CapsuleResultVariableGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    sizeof (CHAR16) * 11,
    &CapsuleResultVariableName
    );
}

/**
  Get the certificate from firmware volume

  @param NameGuid               Pointer to the file GUID of the certificate
  @param Buffer                 Returned the address of the certificate
  @param Size                   Pointer to the size of the certificate

  @retval EFI_SUCCESS           The certificate was successfully retrieved
  @retval EFI_NOT_FOUND         Failed to find the certificate
  @retval EFI_LOAD_ERROR        Firmware Volume Protocol error
**/
STATIC
EFI_STATUS
GetCertificateData (
  IN     EFI_GUID               *NameGuid,
  IN OUT VOID                   **Buffer,
  IN OUT UINTN                  *Size
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINT32                        AuthenticationStatus;

  Fv = NULL;
  AuthenticationStatus = 0;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || (HandleCount == 0)) {
    return EFI_NOT_FOUND;
  }
  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **)&Fv
                    );
    if (EFI_ERROR (Status)) {
      return EFI_LOAD_ERROR;
    }
    *Buffer = NULL;
    *Size = 0;
    Status = Fv->ReadSection (
                   Fv,
                   NameGuid,
                   EFI_SECTION_RAW,
                   0,
                   Buffer,
                   Size,
                   &AuthenticationStatus
                   );
    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  if (Index >= HandleCount) {
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}

/**
  Load the certificate data to "SecureFlashCertData" variable
  The certificate is used when the Capsule image is loaded via gBS->LoadImage()

  @param  None

  @retval EFI_SUCCESS          Certificate variable was successfully set
  @retval EFI_NOT_FOUND        Certificate data was not found
  @retval EFI_OUT_OF_RESOURCES Out of memory
**/
STATIC
EFI_STATUS
LoadCertToVariable (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINT8                       *FileBuffer;
  UINTN                       FileSize;
  CERTIFICATE_DATA            *CertData;
  UINTN                       CertSize;
  EFI_GUID                    SignatureOwnerGuid = EFI_SIGNATURE_OWNER_GUID;


  if (PcdGetBool (PcdH2OCapsuleSignatureDatabaseSupported)) {
    CertData = NULL;
    Status = GetCertificateData (PcdGetPtr (PcdSecureFlashCertificateFile), (VOID **)&CertData, &CertSize);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
  } else {
    FileBuffer = NULL;
    Status = GetCertificateData (PcdGetPtr (PcdSecureFlashCertificateFile), (VOID **)&FileBuffer, &FileSize);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
    CertSize = sizeof (CERTIFICATE_DATA) + FileSize;
    CertData = AllocatePool (CertSize);
    ASSERT (CertData != NULL);
    if (CertData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CertData->SignatureListHeader.SignatureType = gEfiCertX509Guid;
    CertData->SignatureListHeader.SignatureSize = (UINT32)FileSize + sizeof (EFI_GUID);
    CertData->SignatureListHeader.SignatureListSize = CertData->SignatureListHeader.SignatureSize +
                                                      sizeof (EFI_SIGNATURE_LIST);
    CopyGuid (&CertData->SignatureData.SignatureOwner, &SignatureOwnerGuid);
    CopyMem (CertData->SignatureData.SignatureData, FileBuffer, FileSize);
    FreePool (FileBuffer);
  }

  Status = CommonSetVariable (
             L"SecureFlashCertData",
             &gSecureFlashInfoGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             CertSize,
             CertData
             );
  FreePool (CertData);
  return Status;
}

/**

  Enable/disable security check of Capsule images

  @param[in] Enabled           The switch of security check of Capsule images

  @retval EFI_SUCCESS          Security check of Capsule images is disabled
  @return others               Failed to disable Capsule security check

**/
STATIC
EFI_STATUS
CapsuleSecurityCheck (
  IN     BOOLEAN        Enabled
  )
{
  EFI_STATUS  Status;
  UINT8       SetupMode;

  if (Enabled) {
    Status = LoadCertToVariable();
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Set SecureFlashSetupMode variable to trigger image verification process.
    //
    SetupMode = USER_MODE;
    Status = CommonSetVariable (
               SECURE_FLASH_SETUP_MODE_NAME,
               &gSecureFlashInfoGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
               sizeof(SetupMode),
               &SetupMode
               );
  } else {
    //
    // Clear ceritificate data variable
    //
    Status = CommonSetVariable (
               L"SecureFlashCertData",
               &gSecureFlashInfoGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
               0,
               NULL
               );
    //
    // Clear SecureFlashSetupMode variable
    //
    Status = CommonSetVariable (
               SECURE_FLASH_SETUP_MODE_NAME,
               &gSecureFlashInfoGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
               0,
               NULL
               );
  }
  return Status;
}

/**
  Update the status when is it EFI_SECURITY_VIOLATION

  @param[in]  Status            Status for input

  @return                       The updated status if input status is EFI_SECURITY_VIOLATION

**/
STATIC
EFI_STATUS
SecurityViolationStatusUpdate (
  IN     EFI_STATUS     Status
  )
{
  EFI_IMAGE_EXECUTION_INFO_TABLE        *ImageExeInfoTable;
  EFI_IMAGE_EXECUTION_INFO              *ImageExeInfo;
  UINTN                                 Index;
  UINTN                                 NumberOfImages;

  if (Status != EFI_SECURITY_VIOLATION) {
    return Status;
  }
  ImageExeInfoTable = NULL;
  EfiGetSystemConfigurationTable (&gEfiImageSecurityDatabaseGuid, (VOID**)&ImageExeInfoTable);
  if (ImageExeInfoTable == NULL) {
    return Status;
  }
  NumberOfImages = ImageExeInfoTable->NumberOfImages;
  ImageExeInfo = (EFI_IMAGE_EXECUTION_INFO*)(ImageExeInfoTable + 1);
  for (Index = 0; Index <  NumberOfImages - 1; Index++) {
    ImageExeInfo = (EFI_IMAGE_EXECUTION_INFO *) (((UINT8 *) ImageExeInfo) + ImageExeInfo->InfoSize);
  }
  if (ImageExeInfo->Action == EFI_IMAGE_EXECUTION_AUTH_UNTESTED) {
    //
    // Change the status for the image without signature
    //
    Status = EFI_INVALID_PARAMETER;
  }
  return Status;
}

/**
 Internal function to initialize H2O_BDS_CP_CAPSULE_CHECK_UPDATE_CAPABILITY_DATA data and trigger
 gH2OBdsCpCapsuleCheckUpdateCapabilityGuid checkpoint.

 @param[in]      Capsule         Points to the capsule.
 @param[in]      CapsuleSize     Size of the capsule in bytes.
 @param[in, out] AttemptStatus   Points to the capsule update check status.

 @retval EFI_SUCCESS             Trigger gH2OBdsCpCapsuleCheckUpdateCapabilityGuid checkpoint successfully.
 @return Other                   Other error occurred while triggering gH2OBdsCpCapsuleCheckUpdateCapabilityGuid checkpoint.
**/
STATIC
EFI_STATUS
TriggerCpCapsuleCheckUpdateCapability (
  IN     VOID                                         *Capsule,
  IN     UINTN                                        CapsuleSize,
  IN OUT UINT32                                       *AttemptStatus
  )
{
  EFI_STATUS                                          Status;
  H2O_BDS_CP_CAPSULE_CHECK_UPDATE_CAPABILITY_DATA     CpData;
  UINT32                                              AttemptErrorBits;

  AttemptErrorBits = (*AttemptStatus == ESRT_SUCCESS) ? 0 : CheckCapsuleUpdateCapability ((EFI_CAPSULE_HEADER *) Capsule);

  CpData.Size        = sizeof (H2O_BDS_CP_CAPSULE_CHECK_UPDATE_CAPABILITY_DATA);
  CpData.Status      = H2O_CP_TASK_NORMAL;
  CpData.Capsule     = Capsule;
  CpData.CapsuleSize = CapsuleSize;
  CpData.ErrorBits   = AttemptErrorBits;

  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpCapsuleCheckUpdateCapabilityGuid));
  Status = H2OCpTrigger (&gH2OBdsCpCapsuleCheckUpdateCapabilityGuid, &CpData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", CpData.Status));

  if (CpData.Status == H2O_CP_TASK_UPDATE) {
    *AttemptStatus = ConvertCapsuleErrorBitsToStatus (CpData.ErrorBits);
  }

  return Status;
}

/**
  Populates the EFI_FIRMWARE_IMAGE_DESCRIPTOR structure in the private
  context structure.

  @param[in,out] Private  Pointer to the private context structure for the
                          Firmware Management Protocol instance.

**/
STATIC
VOID
PopulateDescriptor (
  IN OUT CAPSULE_FMP_ENTRY     *CapsuleFmpEntry
  )
{
  EFI_STATUS                    Status;
  EFI_GUID                      FirmwareClass;
  UINT32                        FirmwareVersion;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR *Descriptor;
  UINT32                        LastAttemptVersion;
  UINT32                        LastAttemptStatus;
  UINTN                         Size;

  if (CapsuleFmpEntry->DescriptorPopulated) {
    return;
  }

  Descriptor = &CapsuleFmpEntry->Descriptor;

  CopyGuid (&Descriptor->ImageTypeId, &mCapsuleFmpEntry.ImageTypeGuid);
  Descriptor->ImageIndex                  = CapsuleFmpEntry->ImageIndex;
  Descriptor->ImageId                     = Descriptor->ImageIndex;
  Descriptor->ImageIdName                 = NULL;
  Descriptor->HardwareInstance            = 0;
  Descriptor->LowestSupportedImageVersion = PcdGet32(PcdH2OEsrtSystemFirmwareLowestSupportedVersion);
  Descriptor->AttributesSupported         = (IMAGE_ATTRIBUTE_IMAGE_UPDATABLE | IMAGE_ATTRIBUTE_RESET_REQUIRED | IMAGE_ATTRIBUTE_IN_USE);
  Descriptor->AttributesSetting           = (IMAGE_ATTRIBUTE_IMAGE_UPDATABLE | IMAGE_ATTRIBUTE_RESET_REQUIRED | IMAGE_ATTRIBUTE_IN_USE);
  Descriptor->Compatibilities             = 0;
  Descriptor->Size                        = 0;

  //
  // Get the version.
  //
  Status = GetEsrtFirmwareInfo (&FirmwareClass, &FirmwareVersion);
  if (!EFI_ERROR (Status)) {
    Descriptor->Version = FirmwareVersion;
  }

  //
  // Free the current version name.  Shouldn't really happen but this populate
  // function could be called multiple times (to refresh).
  //
  if (Descriptor->VersionName != NULL) {
    FreePool (Descriptor->VersionName);
    Descriptor->VersionName = NULL;
  }

  Size = BVDT_MAX_STR_SIZE;
  Descriptor->VersionName = AllocatePool (Size);
  if (Descriptor->VersionName != NULL) {
    //
    // Attempt to get the version string from BvdtLib.
    //
    Status = GetBvdtInfo ((BVDT_TYPE) BvdtCcbVer, &Size, Descriptor->VersionName);
    if (EFI_ERROR (Status)) {
      CopyMem (Descriptor->VersionName, VERSION_STRING_NOT_AVAILABLE, sizeof (VERSION_STRING_NOT_AVAILABLE));
    }
  }

  //
  // Get last attemp version & status.
  //
  Size = sizeof(UINT32);
  Status = CommonGetVariable (
             ESRT_LAST_ATTEMPT_VERSION,
             &Descriptor->ImageTypeId,
             &Size,
             &LastAttemptVersion
             );
  if (Status == EFI_SUCCESS) {
    Descriptor->LastAttemptVersion = LastAttemptVersion;
  }
  Size = sizeof(UINT32);
  Status = CommonGetVariable (
             ESRT_LAST_ATTEMPT_STATUS,
             &Descriptor->ImageTypeId,
             &Size,
             &LastAttemptStatus
             );
  if (Status == EFI_SUCCESS) {
    Descriptor->LastAttemptStatus = LastAttemptStatus;
  }

  CapsuleFmpEntry->DescriptorPopulated = TRUE;
}

/**
  Returns information about the current firmware image(s) of the device.

  This function allows a copy of the current firmware image to be created and saved.
  The saved copy could later been used, for example, in firmware image recovery or rollback.

  @param[in]      This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in, out] ImageInfoSize      A pointer to the size, in bytes, of the ImageInfo buffer.
                                     On input, this is the size of the buffer allocated by the caller.
                                     On output, it is the size of the buffer returned by the firmware
                                     if the buffer was large enough, or the size of the buffer needed
                                     to contain the image(s) information if the buffer was too small.
  @param[in, out] ImageInfo          A pointer to the buffer in which firmware places the current image(s)
                                     information. The information is an array of EFI_FIRMWARE_IMAGE_DESCRIPTORs.
  @param[out]     DescriptorVersion  A pointer to the location in which firmware returns the version number
                                     associated with the EFI_FIRMWARE_IMAGE_DESCRIPTOR.
  @param[out]     DescriptorCount    A pointer to the location in which firmware returns the number of
                                     descriptors or firmware images within this device.
  @param[out]     DescriptorSize     A pointer to the location in which firmware returns the size, in bytes,
                                     of an individual EFI_FIRMWARE_IMAGE_DESCRIPTOR.
  @param[out]     PackageVersion     A version number that represents all the firmware images in the device.
                                     The format is vendor specific and new version must have a greater value
                                     than the old version. If PackageVersion is not supported, the value is
                                     0xFFFFFFFF. A value of 0xFFFFFFFE indicates that package version comparison
                                     is to be performed using PackageVersionName. A value of 0xFFFFFFFD indicates
                                     that package version update is in progress.
  @param[out]     PackageVersionName A pointer to a pointer to a null-terminated string representing the
                                     package version name. The buffer is allocated by this function with
                                     AllocatePool(), and it is the caller's responsibility to free it with a call
                                     to FreePool().

  @retval EFI_SUCCESS                The device was successfully updated with the new image.
  @retval EFI_BUFFER_TOO_SMALL       The ImageInfo buffer was too small. The current buffer size
                                     needed to hold the image(s) information is returned in ImageInfoSize.
  @retval EFI_INVALID_PARAMETER      ImageInfoSize is NULL.
  @retval EFI_DEVICE_ERROR           Valid information could not be returned. Possible corrupted image.

**/
EFI_STATUS
EFIAPI
UefiImageGetImageInfo (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *This,
  IN OUT UINTN                             *ImageInfoSize,
  IN OUT EFI_FIRMWARE_IMAGE_DESCRIPTOR     *ImageInfo,
  OUT    UINT32                            *DescriptorVersion,
  OUT    UINT8                             *DescriptorCount,
  OUT    UINTN                             *DescriptorSize,
  OUT    UINT32                            *PackageVersion,
  OUT    CHAR16                            **PackageVersionName
  )
{
  //
  // Check for valid pointer
  //
  if (ImageInfoSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check the buffer size
  // NOTE: Check this first so caller can get the necessary memory size it must allocate.
  //
  if (*ImageInfoSize < (sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR))) {
    *ImageInfoSize = sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR);
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // Confirm that buffer isn't null
  //
  if ( (ImageInfo == NULL) || (DescriptorVersion == NULL) || (DescriptorCount == NULL) || (DescriptorSize == NULL)
       || (PackageVersion == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Set the size to whatever we need
  //
  *ImageInfoSize = sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR);

  //
  // Make sure the descriptor has already been loaded or refreshed
  //
  PopulateDescriptor (&mCapsuleFmpEntry);

  //
  // Copy the image descriptor
  //
  CopyMem (ImageInfo, &mCapsuleFmpEntry.Descriptor, sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR));

  *DescriptorVersion = EFI_FIRMWARE_IMAGE_DESCRIPTOR_VERSION;
  *DescriptorCount   = 1;
  *DescriptorSize    = sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR);
  //
  // means unsupported
  //
  *PackageVersion = 0xFFFFFFFF;

  //
  // Do not update PackageVersionName since it is not supported in this instance.
  //

  return EFI_SUCCESS;
}

/**
  Checks if the firmware image is valid for the device.

  This function allows firmware update application to validate the firmware image without
  invoking the SetImage() first.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  ImageIndex         A unique number identifying the firmware image(s) within the device.
                                 The number is between 1 and DescriptorCount.
  @param[in]  Image              Points to the new image.
  @param[in]  ImageSize          Size of the new image in bytes.
  @param[out] ImageUpdatable     Indicates if the new image is valid for update. It also provides,
                                 if available, additional information if the image is invalid.

  @retval EFI_SUCCESS            The image was successfully checked.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATION The operation could not be performed due to an authentication failure.

**/
EFI_STATUS
EFIAPI
UefiImageCheckImage (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL       *This,
  IN     UINT8                                  ImageIndex,
  IN     CONST VOID                             *Capsule,
  IN     UINTN                                  CapsuleSize,
  OUT    UINT32                                 *ImageUpdatable
  )
{
  EFI_STATUS                    Status;
  UINT32                        CheckImageResult;
  EFI_SYSTEM_RESOURCE_TABLE     *Esrt;
  UINTN                         Index;
  EFI_HANDLE                    CapsuleHandle;
  UINT32                        AttemptVersion;
  ESRT_STATUS                   AttemptStatus;
  UINT32                        LastAttemptVersion;
  ESRT_STATUS                   LastAttemptStatus;
  //
  // Check ERST in system configuratin table to ensure the capsule is actually supported by BIOS
  //
  AttemptVersion     = 0;
  LastAttemptVersion = 0;
  AttemptStatus      = ESRT_SUCCESS;
  LastAttemptStatus  = ESRT_SUCCESS;
  Esrt               = NULL;
  Status = EfiGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, (VOID **)&Esrt);
  if (Status != EFI_SUCCESS || Esrt == NULL) {
    //
    // return EFI_UNSUPPORTED if ESRT table is not found
    //
    return EFI_UNSUPPORTED;
  }
  for (Index = 0, LastAttemptStatus = ESRT_SUCCESS; Index < Esrt->FirmwareResourceCount; Index++) {
    if (CompareGuid (&Esrt->FirmwareResources[Index].FirmwareClass, &((EFI_CAPSULE_HEADER*)Capsule)->CapsuleGuid)) {
      LastAttemptVersion = Esrt->FirmwareResources[Index].LastAttemptVersion;
      LastAttemptStatus  = Esrt->FirmwareResources[Index].LastAttemptStatus;
      AttemptVersion     = LastAttemptVersion;
      AttemptStatus      = LastAttemptStatus;
      break;
    }
  }
  if (Index >= (UINTN)Esrt->FirmwareResourceCount) {
    return EFI_UNSUPPORTED;
  }
  AttemptVersion = GetCapsuleSystemFirmwareVersion ((EFI_CAPSULE_HEADER*)Capsule);
  if (CapsuleSize > 0x10000000) {
    //
    // Record the ESRT status for out of resources
    //
    CheckImageResult = IMAGE_UPDATABLE_INVALID;
    //
    // Update capsule status onto corresponding variables
    //
    AttemptStatus = ESRT_ERROR_INSUFFICIENT_RESOURCES;
    UpdateCapsuleStatus (&((EFI_CAPSULE_HEADER*)Capsule)->CapsuleGuid, AttemptVersion, AttemptStatus, EFI_OUT_OF_RESOURCES);
    goto Exit;
  }
  //
  // Try to load image to verify the image format
  //
  CapsuleHandle = NULL;
  CapsuleSecurityCheck (TRUE);
  Status = gBS->LoadImage (
                  FALSE,
                  gImageHandle,
                  NULL,
                  (UINT8*)Capsule + ((EFI_CAPSULE_HEADER*)Capsule)->HeaderSize,
                  CapsuleSize - ((EFI_CAPSULE_HEADER*)Capsule)->HeaderSize,
                  (VOID **)&CapsuleHandle
                  );
  CapsuleSecurityCheck (FALSE);
  if (EFI_ERROR (Status)) {
    //
    // Record the ESRT status for out of resources
    //
    AttemptStatus    = ConvertEfiStatusToEsrtStatus (Status);
    CheckImageResult = IMAGE_UPDATABLE_INVALID;
    //
    // Update capsule status onto corresponding variables
    //
    UpdateCapsuleStatus (&((EFI_CAPSULE_HEADER*)Capsule)->CapsuleGuid, AttemptVersion, AttemptStatus, SecurityViolationStatusUpdate (Status));
    goto Exit;
  }
  if (CapsuleHandle) {
    gBS->UnloadImage (CapsuleHandle);
  }
  AttemptStatus = PreInstallationCheck ((EFI_CAPSULE_HEADER*)Capsule);
  if (FeaturePcdGet (PcdH2OBdsCpCapsuleCheckUpdateCapabilitySupported)) {
    TriggerCpCapsuleCheckUpdateCapability ((VOID *) Capsule, CapsuleSize, &AttemptStatus);
  }

  CheckImageResult = ConvertEsrtStatusToCheckImageResult (AttemptStatus);
  if (AttemptStatus != ESRT_SUCCESS) {
    //
    // Update capsule status onto corresponding variables
    //
    UpdateCapsuleStatus (&((EFI_CAPSULE_HEADER*)Capsule)->CapsuleGuid, AttemptVersion, AttemptStatus, ConvertEsrtStatusToEfiStatus (AttemptStatus));
  }
Exit:
  if (AttemptVersion != LastAttemptVersion || AttemptStatus != LastAttemptStatus) {
    //
    // Update the last attempt version/status back to ESRT if value get updated and reinstall ESRT table
    //
    Esrt->FirmwareResources[Index].LastAttemptVersion = AttemptVersion;
    Esrt->FirmwareResources[Index].LastAttemptStatus  = AttemptStatus;
    gBS->InstallConfigurationTable (&gEfiSystemResourceTableGuid, Esrt);
  }
  if (ImageUpdatable) {
    *ImageUpdatable = CheckImageResult;
  }
  return EFI_SUCCESS;
}

/**
  Process UEFI image in capsule image

  This function updates the hardware with the new firmware image.
  This function returns EFI_UNSUPPORTED if the firmware image is not updatable.
  If the firmware image is updatable, the function should perform the following minimal validations
  before proceeding to do the firmware image update.
  - Validate the image authentication if image has attribute
    IMAGE_ATTRIBUTE_AUTHENTICATION_REQUIRED.  The function returns
    EFI_SECURITY_VIOLATION if the validation fails.
  - Validate the image is a supported image for this device.  The function returns EFI_ABORTED if
    the image is unsupported.  The function can optionally provide more detailed information on
    why the image is not a supported image.
  - Validate the data from VendorCode if not null.  Image validation must be performed before
    VendorCode data validation.  VendorCode data is ignored or considered invalid if image
    validation failed.  The function returns EFI_ABORTED if the data is invalid.

  VendorCode enables vendor to implement vendor-specific firmware image update policy.  Null if
  the caller did not specify the policy or use the default policy.  As an example, vendor can implement
  a policy to allow an option to force a firmware image update when the abort reason is due to the new
  firmware image version is older than the current firmware image version or bad image checksum.
  Sensitive operations such as those wiping the entire firmware image and render the device to be
  non-functional should be encoded in the image itself rather than passed with the VendorCode.
  AbortReason enables vendor to have the option to provide a more detailed description of the abort
  reason to the caller.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  ImageIndex         A unique number identifying the firmware image(s) within the device.
                                 The number is between 1 and DescriptorCount.
  @param[in]  Image              Points to the new image.
  @param[in]  ImageSize          Size of the new image in bytes.
  @param[in]  VendorCode         This enables vendor to implement vendor-specific firmware image update policy.
                                 Null indicates the caller did not specify the policy or use the default policy.
  @param[in]  Progress           A function used by the driver to report the progress of the firmware update.
  @param[out] AbortReason        A pointer to a pointer to a null-terminated string providing more
                                 details for the aborted operation. The buffer is allocated by this function
                                 with AllocatePool(), and it is the caller's responsibility to free it with a
                                 call to FreePool().

  @retval EFI_SUCCESS            The device was successfully updated with the new image.
  @retval EFI_ABORTED            The operation is aborted.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATION The operation could not be performed due to an authentication failure.

**/
EFI_STATUS
EFIAPI
UefiImageSetImage (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL               *This,
  IN     UINT8                                          ImageIndex,
  IN     CONST VOID                                     *Capsule,
  IN     UINTN                                          CapsuleSize,
  IN     CONST VOID                                     *VendorCode,
  IN     EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS  Progress,
  OUT    CHAR16                                         **AbortReason
  )
{
  EFI_STATUS            Status;
  UINT32                CheckImageResult;
  UINT32                AttemptVersion;
  ESRT_STATUS           AttemptStatus;
  EFI_HANDLE            CapsuleHandle;
  //
  // Check Image before actual action to ensure the capsule is qualified
  //
  Status = UefiImageCheckImage (
             This,
             ImageIndex,
             Capsule,
             CapsuleSize,
             &CheckImageResult
             );
  if (CheckImageResult != IMAGE_UPDATABLE_VALID) {
    return EFI_UNSUPPORTED;
  }
  CapsuleHandle = NULL;
  Status = CapsuleSecurityCheck (TRUE);
  if (!EFI_ERROR (Status)) {
    Status = gBS->LoadImage (
                    FALSE,
                    gImageHandle,
                    NULL,
                    (UINT8*)Capsule + ((EFI_CAPSULE_HEADER*)Capsule)->HeaderSize,
                    CapsuleSize - ((EFI_CAPSULE_HEADER*)Capsule)->HeaderSize,
                    (VOID **)&CapsuleHandle
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Set firmware update flag by flashing the signature onto flash part for seamless recovery
      //
      HookSystemResetRoutine(TRUE);
      SetFirmwareUpdatingFlag (TRUE);
      //
      // Start flash process
      //
      Status = gBS->StartImage (CapsuleHandle, NULL, NULL);
      //
      // Clear firmware update flag by erase the signature in flash part for seamless recovery
      //
      SetFirmwareUpdatingFlag (FALSE);
      HookSystemResetRoutine(FALSE);
    }
  }
  CapsuleSecurityCheck (FALSE);
  AttemptVersion = GetCapsuleSystemFirmwareVersion ((EFI_CAPSULE_HEADER*)Capsule);
  AttemptStatus  = ConvertEfiStatusToEsrtStatus (Status);
  //
  // Update capsule status onto corresponding variables
  //
  UpdateCapsuleStatus (&((EFI_CAPSULE_HEADER*)Capsule)->CapsuleGuid, AttemptVersion, AttemptStatus, SecurityViolationStatusUpdate (Status));
  //
  // Reset system after BIOS flash completed if needed
  //
  if (CapsuleHandle) {
    gBS->UnloadImage (CapsuleHandle);
  }
  return Status;
}

/**

  Installation of capsule in WindowsUx processor

**/
EFI_STATUS
InstallCapsuleInUefiImage (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_SYSTEM_RESOURCE_TABLE     *Esrt;
  UINT32                        LastAttemptVersion;
  ESRT_STATUS                   LastAttemptStatus;
  UINTN                         Size;
  //
  // Setup ESRT use for both of UEFI image capsule processor and Windows capsule update
  //
  Esrt = AllocateRuntimeZeroPool (sizeof(EFI_SYSTEM_RESOURCE_TABLE));
  if (Esrt == NULL) {
    ASSERT (Esrt != NULL);
    return EFI_OUT_OF_RESOURCES;
  }
  Esrt->FirmwareResourceCount   = 1;
  Esrt->FirmwareResourceMaximum = MAX_FIRMWARE_RESOURCES;
  Esrt->FirmwareResourceVersion = 1;
  Status = GetEsrtFirmwareInfo (
             &Esrt->FirmwareResources[0].FirmwareClass,
             &Esrt->FirmwareResources[0].FirmwareVersion
             );
  Esrt->FirmwareResources[0].FirmwareType                   = SYSTEM_FIRMWARE_TYPE;
  Esrt->FirmwareResources[0].LowestSupportedFirmwareVersion = PcdGet32(PcdH2OEsrtSystemFirmwareLowestSupportedVersion);
  Size = sizeof(UINT32);
  Status = CommonGetVariable (
             ESRT_LAST_ATTEMPT_VERSION,
             &Esrt->FirmwareResources[0].FirmwareClass,
             &Size,
             &LastAttemptVersion
             );
  if (Status == EFI_SUCCESS) {
    Esrt->FirmwareResources[0].LastAttemptVersion = LastAttemptVersion;
  }
  Size = sizeof(ESRT_STATUS);
  Status = CommonGetVariable (
             ESRT_LAST_ATTEMPT_STATUS,
             &Esrt->FirmwareResources[0].FirmwareClass,
             &Size,
             &LastAttemptStatus
             );
  if (Status == EFI_SUCCESS) {
    Esrt->FirmwareResources[0].LastAttemptStatus = LastAttemptStatus;
  }
  //
  // Install ESRT into configuration table
  //
  Status = gBS->InstallConfigurationTable (&gEfiSystemResourceTableGuid, Esrt);
  //
  // Capsule processor instance installation
  //
  mCapsuleFmp.CapsuleProcessorCount ++;
  mCapsuleFmpEntry.ImageIndex = (UINT8)mCapsuleFmp.CapsuleProcessorCount;
  CopyGuid (&mCapsuleFmpEntry.ImageTypeGuid, &Esrt->FirmwareResources[0].FirmwareClass);
  mCapsuleFmpEntry.SetImage   = UefiImageSetImage;
  mCapsuleFmpEntry.CheckImage = UefiImageCheckImage;
  InsertTailList (&mCapsuleFmp.CapsuleProcessorListHead, (LIST_ENTRY*)&mCapsuleFmpEntry);
  return EFI_SUCCESS;
}
