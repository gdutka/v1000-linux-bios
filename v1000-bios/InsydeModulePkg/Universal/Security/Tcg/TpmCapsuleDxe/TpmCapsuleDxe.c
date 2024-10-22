/** @file
  Firmware Management Protocol implementation for TPM Firmware Update.

;******************************************************************************
;* Copyright (c) 2017 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/VariableLib.h>
#include <Library/PrintLib.h>
#include <Library/DevicePathLib.h>
#include <Library/Tpm12DeviceLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/H2OCpLib.h>

#include <Protocol/TcgService.h>
#include <Protocol/Tcg2Protocol.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/FirmwareManagement.h>

#include <Guid/EfiSystemResourceTable.h>
#include <Guid/FmpCapsule.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/H2OBdsCheckPoint.h>
#include <Guid/TcgEventHob.h>
#include <Guid/TpmInstance.h>

#include <SecureFlash.h>
#include <TpmPolicy.h>

#define ESRT_LAST_ATTEMPT_VERSION       L"EsrtLastAttemptVersion"
#define ESRT_LAST_ATTEMPT_STATUS        L"EsrtLastAttemptStatus"
#define CAPSULE_RESULT_VARIABLE         L"Capsule0000"
#define CAPSULE_RESULT_MAX_VARIABLE     L"CapsuleMax"
#define CAPSULE_RESULT_LAST_VARIABLE    L"CapsuleLast"
#define EFI_CAPSULE_REPORT_GUID         { 0x39b68c46, 0xf7fb, 0x441b, { 0xb6, 0xec, 0x16, 0xb0, 0xf6, 0x98, 0x21, 0xf3 }}
#define EFI_SIGNATURE_OWNER_GUID        { 0x79736E49, 0x6564, 0xBBAA, { 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x23, 0x45, 0x67 }}

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

typedef struct {
  CHAR16                                 *VariableName;
  EFI_GUID                               *VendorGuid;
} VARIABLE_TYPE;

#pragma pack(1)

typedef struct {
  TPM_RQU_COMMAND_HDR  Hdr;
  UINT32               Capability;
  UINT32               CapabilityFlagSize;
  UINT32               CapabilityFlag;
} TPM_CMD_GET_CAPABILITY;

typedef struct {
  TPM_RSP_COMMAND_HDR  Hdr;
  UINT32               ResponseSize;
  TPM_CAP_VERSION_INFO VersionInfo;
} TPM_RSP_GET_CAPABILITY_VERSION_INFO;

#pragma pack()

STATIC EFI_GUID                   mSignatureOwnerGuid          = EFI_SIGNATURE_OWNER_GUID;
STATIC EFI_GUID                   mCapsuleResultVariableGuid   = EFI_CAPSULE_REPORT_GUID;
STATIC EFI_GUID                   mTcg2UpdatedPlatformAuthGuid = {
  0xebab103e, 0x8906, 0x4dc8, 0xbd, 0xd1, 0xa0, 0xbc, 0x26, 0xb1, 0xba, 0x86
};

EFI_FIRMWARE_MANAGEMENT_PROTOCOL  mTpmFirmwareManagement;

/**
  Provides lastest platformAuth value.

  @retval PlatformAuth

**/
STATIC
TPM2B_AUTH *
GetAuthKey (
  VOID
  )
{
  EFI_STATUS           Status;
  TPM2B_AUTH           *Auth;

  Status = gBS->LocateProtocol (&mTcg2UpdatedPlatformAuthGuid, NULL, (VOID **)&Auth);
  if (EFI_ERROR (Status)) {
    //
    // Auth value is default before protocol installation
    //
    return NULL;
  }
  return Auth;
}

/**
  Set platformAuth to NULL.

  @retval EFI_SUCCESS          Operation completed successfully.
  @retval other                Operation was unsuccessful.

**/
STATIC
EFI_STATUS
Tpm20SetPlatformAuthEmpty (
  VOID
  )
{
  EFI_STATUS                Status;
  TPM2B_AUTH                *PlatformAuth;
  TPMS_AUTH_COMMAND         LocalAuthSession;
  TPM2B_AUTH                NewPlatformAuth;

  PlatformAuth = GetAuthKey ();

  if (PlatformAuth == NULL) {
    //
    // No platform auth change needed
    //
    return EFI_SUCCESS;
  }

  //
  // Initialize TPM
  //
  Status = Tpm2RequestUseTpm ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Prepare auth session
  //
  ZeroMem (&LocalAuthSession, sizeof (LocalAuthSession));
  LocalAuthSession.sessionHandle = TPM_RS_PW;
  LocalAuthSession.hmac.size = PlatformAuth->size;
  CopyMem (LocalAuthSession.hmac.buffer, PlatformAuth->buffer, PlatformAuth->size);

  //
  // Set platformAuth to NULL
  //
  ZeroMem (&NewPlatformAuth, sizeof (NewPlatformAuth));
  Status = Tpm2HierarchyChangeAuth (TPM_RH_PLATFORM, &LocalAuthSession, &NewPlatformAuth);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Issue DeferredPhysicalPresence command.

  @retval EFI_SUCCESS      Operation completed successfully.
  @retval Other            Operation was unsuccessful.

**/
STATIC
EFI_STATUS
Tpm12DeferredPhysicalPresence (
  VOID
  )
{
  EFI_STATUS                        Status;
  TPM_RQU_COMMAND_HDR               *Command;
  TPM_RSP_COMMAND_HDR               Response;
  UINT32                            CommandSize;
  UINT32                            ResponseSize;
  UINT32                            AdditionalParameters[5];
  UINTN                             AdditionalParameterSize;
  EFI_TCG_PROTOCOL                  *TcgProtocol;

  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, (VOID **)&TcgProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Tpm12RequestUseTpm ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  AdditionalParameterSize = sizeof (AdditionalParameters);
  Command = (TPM_RQU_COMMAND_HDR *) AllocatePool (sizeof (TPM_RQU_COMMAND_HDR) + AdditionalParameterSize);
  if (Command == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Command->tag            = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  CommandSize             = (UINT32)(sizeof (*Command) + AdditionalParameterSize);
  Command->paramSize      = SwapBytes32 (CommandSize);
  Command->ordinal        = SwapBytes32 (TPM_ORD_SetCapability);
  AdditionalParameters[0] = SwapBytes32 (TPM_SET_STCLEAR_DATA);            // CapabilityArea
  AdditionalParameters[1] = SwapBytes32 (sizeof(UINT32));                  // SubCapSize
  AdditionalParameters[2] = SwapBytes32 (TPM_SD_DEFERREDPHYSICALPRESENCE); // SubCap
  AdditionalParameters[3] = SwapBytes32 (sizeof(UINT32));                  // SetValueSize
  AdditionalParameters[4] = SwapBytes32 (1);                               // UnownedFieldUpgrade; bit0
  CopyMem (Command + 1, AdditionalParameters, AdditionalParameterSize);

  ResponseSize = sizeof (Response);
  Status = TcgProtocol->PassThroughToTpm (
                          TcgProtocol,
                          CommandSize,
                          (UINT8 *)Command,
                          (UINT32)ResponseSize,
                          (UINT8 *)&Response
                          );
  FreePool (Command);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "Tpm12DeferredPhysicalPresence - ReturnCode = %x\n", SwapBytes32 (Response.returnCode)));
  if (SwapBytes32 (Response.returnCode) != TPM_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}

/**
  Send ForceClear command to TPM1.2.

  @retval EFI_SUCCESS      Operation completed successfully.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
STATIC
EFI_STATUS
Tpm12Clear (
  VOID
  )
{
  EFI_STATUS           Status;
  TPM_RQU_COMMAND_HDR  Command;
  TPM_RSP_COMMAND_HDR  Response;
  UINT32               Length;
  EFI_TCG_PROTOCOL     *TcgProtocol;

  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, (VOID **)&TcgProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // send Tpm command TPM_ORD_ForceClear
  //
  Command.tag        = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  Command.paramSize  = SwapBytes32 (sizeof (Command));
  Command.ordinal    = SwapBytes32 (TPM_ORD_ForceClear);

  Length = sizeof (Response);
  Status = TcgProtocol->PassThroughToTpm (
                          TcgProtocol,
                          sizeof (Command),
                          (UINT8 *)&Command,
                          (UINT32)Length,
                          (UINT8 *)&Response
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return (SwapBytes32 (Response.returnCode) == TPM_SUCCESS) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}
/**
  Get system firmware revision for ESRT from capsule image

  @param  CapsuleHeader         Points to a capsule header.

  @return                       The system firmware revision from the capsule image
                                If the signature cannot be found, 0x00000000 will
                                be returned

**/
STATIC
UINT32
GetFirmwareVersionInEsrt (
  IN EFI_CAPSULE_HEADER  *Capsule
  )
{
  EFI_STATUS                                    Status;
  EFI_SYSTEM_RESOURCE_TABLE                     *Esrt;
  UINTN                                         Index;

  Status = EfiGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, (VOID **)&Esrt);
  if (Status != EFI_SUCCESS || Esrt == NULL) {
    //
    // return EFI_UNSUPPORTED if ESRT table is not found
    //
    return 0;
  }
  for (Index = 0; Index < Esrt->FirmwareResourceCount; Index++) {
    if (CompareGuid (&Esrt->FirmwareResources[Index].FirmwareClass, &((EFI_CAPSULE_HEADER *)Capsule)->CapsuleGuid)) {
      return Esrt->FirmwareResources[Index].FirmwareVersion;
    }
  }

  return 0;
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
  Status =  EfiGetSystemConfigurationTable (&gEfiImageSecurityDatabaseGuid, (VOID **)&ImageExeInfoTable);
  if (Status != EFI_SUCCESS || ImageExeInfoTable == NULL) {
    return EFI_NOT_FOUND;
  }
  NumberOfImages = ImageExeInfoTable->NumberOfImages;
  ImageExeInfo = (EFI_IMAGE_EXECUTION_INFO *)(ImageExeInfoTable + 1);
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

  HandleCount  = 0;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || (HandleCount == 0)) {
    Status = EFI_NOT_FOUND;
    goto Exit;
  }
  //
  // Find desired image in all Fvs
  //
  Fv = NULL;
  AuthenticationStatus = 0;
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **)&Fv
                    );
    if (EFI_ERROR (Status)) {
      Status = EFI_LOAD_ERROR;
      goto Exit;
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
    Status = EFI_NOT_FOUND;
    goto Exit;
  }

Exit:
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }
  return Status;
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
    CopyGuid (&CertData->SignatureData.SignatureOwner, &mSignatureOwnerGuid);
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
  @return Others               Failed to disable Capsule security check

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
    Status = LoadCertToVariable ();
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

  //
  // Update ESRT version and status
  //
  CommonSetVariable (
    ESRT_LAST_ATTEMPT_VERSION,
    CapsuleGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    sizeof (UINT32),
    &AttemptVersion
    );
  CommonSetVariable (
    ESRT_LAST_ATTEMPT_STATUS,
    CapsuleGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    sizeof (ESRT_STATUS),
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
             &mCapsuleResultVariableGuid,
             &Size,
             &CapsuleResultVariableName
             );
  if (Status == EFI_NOT_FOUND) {
    UnicodeSPrint (CapsuleResultVariableName, sizeof (CapsuleResultVariableName), L"Capsule%04x", CapsuleResultMaxCounter);
    CommonSetVariable (
      CAPSULE_RESULT_MAX_VARIABLE,
      &mCapsuleResultVariableGuid,
      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
      Size,
      &CapsuleResultVariableName
      );
  }
  StrCpyS (CapsuleResultVariableName, sizeof (CapsuleResultVariableName) / sizeof (CHAR16), CAPSULE_RESULT_VARIABLE);
  Size = sizeof (CHAR16) * 11;
  Status = CommonGetVariable (
             CAPSULE_RESULT_LAST_VARIABLE,
             &mCapsuleResultVariableGuid,
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
    UnicodeSPrint (CapsuleResultVariableName, sizeof (CapsuleResultVariableName), L"Capsule%04x", CapsuleResultCounter);
  }
  //
  // Update UEFI capsule result status
  //
  CapsuleResult.VariableTotalSize = sizeof (EFI_CAPSULE_RESULT_VARIABLE_HEADER);
  CapsuleResult.Reserved          = 0;
  CopyGuid (&CapsuleResult.CapsuleGuid, CapsuleGuid);
  gRT->GetTime (&CapsuleResult.CapsuleProcessed, NULL);
  CapsuleResult.CapsuleStatus     = CapsuleStatus;
  CommonSetVariable (
    CapsuleResultVariableName,
    &mCapsuleResultVariableGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    CapsuleResult.VariableTotalSize,
    &CapsuleResult
    );
  CommonSetVariable (
    CAPSULE_RESULT_LAST_VARIABLE,
    &mCapsuleResultVariableGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
    sizeof (CHAR16) * 11,
    &CapsuleResultVariableName
    );
}

/**
  Check the integrity of the capsule.

  @param[in]  Capsule            Points to the new image.
  @param[in]  CapsuleSize        Size of the new image in bytes.

  @retval EFI_SUCCESS            The image was successfully checked.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATION The operation could not be performed due to an authentication failure.

**/
STATIC
UINT32
CheckCapsuleIntegrity (
  IN     CONST VOID                             *Capsule,
  IN     UINTN                                  CapsuleSize
  )
{
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER        *FmpCapsuleHeader;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER  *ImageHeader;
  UINT8                                         *EndOfPayload;
  UINT64                                        *ItemOffsetList;
  UINT32                                        ItemNum;
  UINT32                                        CheckImageResult;
  UINTN                                         Index;
  UINT8                                         *Image;

  CheckImageResult = IMAGE_UPDATABLE_VALID;
  FmpCapsuleHeader = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER *)((UINT8 *)Capsule + ((EFI_CAPSULE_HEADER *)Capsule)->HeaderSize);
  if (FmpCapsuleHeader->Version > EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER_INIT_VERSION) {
    return IMAGE_UPDATABLE_INVALID_OLD;
  }
  ItemOffsetList = (UINT64 *)(FmpCapsuleHeader + 1);
  ItemNum = FmpCapsuleHeader->EmbeddedDriverCount + FmpCapsuleHeader->PayloadItemCount;
  if (ItemNum == FmpCapsuleHeader->EmbeddedDriverCount) {
    //
    // No payload element
    //
    return IMAGE_UPDATABLE_INVALID_TYPE;
  }
  if (FmpCapsuleHeader->PayloadItemCount != 0) {
    //
    // Check if the last payload is within capsule image range
    //
    ImageHeader  = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *)((UINT8 *)FmpCapsuleHeader + ItemOffsetList[ItemNum - 1]);
    if (ImageHeader->Version >= EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER_INIT_VERSION) {
      Image = (UINT8 *)(ImageHeader + 1);
    } else {
      Image = (UINT8 *)ImageHeader + OFFSET_OF(EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER, UpdateHardwareInstance);
    }
    EndOfPayload = (UINT8 *)(Image + ImageHeader->UpdateImageSize + ImageHeader->UpdateVendorCodeSize);
  } else {
    //
    // No driver & payload element in FMP
    //
    EndOfPayload = (UINT8 *)(FmpCapsuleHeader + 1);
  }
  if (EndOfPayload != (UINT8 *)Capsule + CapsuleSize) {
    return IMAGE_UPDATABLE_INVALID_TYPE;
  }
  //
  // All the address in ItemOffsetList must be stored in ascending order
  //
  if (ItemNum >= 2) {
    for (Index = 0; Index < ItemNum - 1; Index++) {
      if (ItemOffsetList[Index] >= ItemOffsetList[Index + 1]) {
        return  IMAGE_UPDATABLE_INVALID_TYPE;
      }
    }
  }

  return CheckImageResult;
}

/**
  Get TPM capability TPM version info.

  @param[out] TpmVersion        TPM version.
  @param[out] ManufacturerId    TPM manufacturer ID.

  @retval EFI_SUCCESS      Operation completed successfully.
  @retval Other            Operation was unsuccessful.

**/
STATIC
EFI_STATUS
Tpm12GetCapabilityTpmVersionInfo (
  OUT TPM_VERSION                         *TpmVersion,
  OUT UINT32                              *ManufacturerId
  )
{
  EFI_STATUS                              Status;
  EFI_TCG_PROTOCOL                        *TcgProtocol;
  TPM_CMD_GET_CAPABILITY                  Command;
  TPM_RSP_GET_CAPABILITY_VERSION_INFO     *Response;
  UINT8                                   Buffer[64];

  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, (VOID **)&TcgProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Send Tpm command TPM_ORD_GetCapability
  //
  Command.Hdr.tag            = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  Command.Hdr.paramSize      = SwapBytes32 (sizeof (Command));
  Command.Hdr.ordinal        = SwapBytes32 (TPM_ORD_GetCapability);
  Command.Capability         = SwapBytes32 (TPM_CAP_VERSION_VAL);
  Command.CapabilityFlagSize = SwapBytes32 (sizeof (TPM_CAP_VERSION_VAL));
  Command.CapabilityFlag     = SwapBytes32 (TPM_CAP_VERSION_VAL);

  Status = TcgProtocol->PassThroughToTpm (
                          TcgProtocol,
                          sizeof (Command),
                          (UINT8 *)&Command,
                          (UINT32)sizeof (Buffer),
                          (UINT8 *)&Buffer
                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Tpm12GetCapabilityTpmVersionInfo: SubmitCommand error! -%r\n", Status));
    return Status;
  }

  Response = (TPM_RSP_GET_CAPABILITY_VERSION_INFO *)Buffer;
  if (SwapBytes32 (Response->Hdr.returnCode) != TPM_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "Tpm12GetCapabilityTpmVersionInfo: Response Code error! 0x%08x\r\n", SwapBytes32 (Response->Hdr.returnCode)));
    return  EFI_DEVICE_ERROR;
  }

  CopyMem (TpmVersion, &Response->VersionInfo.version, sizeof (*TpmVersion));
  CopyMem (ManufacturerId, &Response->VersionInfo.tpmVendorID, sizeof (*ManufacturerId));

  return Status;
}

/**
  Get TPM firmware version

  @retval EFI_SUCCESS      Operation completed successfully.
  @retval Other            Operation was unsuccessful.
**/
STATIC
EFI_STATUS
GetTpmFirmwareVersion (
  OUT UINT32                    *FirmwareVersion,
  OUT UINT32                    *LowestSupportedFirmwareVersion
  )
{
  EFI_STATUS                    Status;
  UINT32                        ManufacturerId;
  UINT32                        FirmwareVersion1;
  UINT32                        FirmwareVersion2;
  UINT32                        EsrtTpmFwVersion;
  UINT32                        EsrtTpmLowestSupportedFwVersion;

  FirmwareVersion1 = 0;
  FirmwareVersion2 = 0;
  ManufacturerId   = 0;
  if (CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20DtpmGuid)) {
    Status = Tpm2GetCapabilityFirmwareVersion (&FirmwareVersion1, &FirmwareVersion2);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "GetTpmFirmwareVersion: Tpm2GetCapabilityFirmwareVersion - %r\n", Status));
    }

    Status = Tpm2GetCapabilityManufactureID (&ManufacturerId);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "GetTpmFirmwareVersion: Tpm2GetCapabilityManufactureID - %r\n", Status));
    }

    if (AsciiStriCmp ((CHAR8 *)&ManufacturerId, "IFX") == 0) {
      //
      // e.g., 0x28083200 for version 7.40.2098.0
      // FirmwareVersion1 = 0x00070028
      // FirmwareVersion2 = 0x00083200
      //
      EsrtTpmFwVersion = (UINT32)(((FirmwareVersion1 & 0xFF) << 24) | (FirmwareVersion2 & 0xFFFFFF));
    } else if (AsciiStriCmp ((CHAR8 *)&ManufacturerId, "NTC") == 0) {
      //
      // e.g., 0x054F3738 for version 5.79.55.56
      // FirmwareVersion1 = 0x0005004F
      // FirmwareVersion2 = 0x00370038
      //
      EsrtTpmFwVersion = (UINT32)(((FirmwareVersion1 & 0xFF0000) << 8) | ((FirmwareVersion1 & 0xFF) << 16) | ((FirmwareVersion2 & 0xFF0000) >> 8) | (FirmwareVersion2 & 0xFF));
    } else if (AsciiStriCmp ((CHAR8 *)&ManufacturerId, "STM") == 0) {
      //
      // e.g., 0x00490008 for version 4908
      // FirmwareVersion1 = 0x00490008
      // FirmwareVersion2 = don't care
      //
      EsrtTpmFwVersion = FirmwareVersion1;
    } else {
      EsrtTpmFwVersion = FirmwareVersion1;
    }
  } else if (CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid)) {
    Status = Tpm12GetCapabilityTpmVersionInfo ((TPM_VERSION *)&FirmwareVersion2, &ManufacturerId);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "GetTpmFirmwareVersion: Tpm12GetCapabilityTpmVersionInfo - %r\n", Status));
    }
    EsrtTpmFwVersion = SwapBytes32 (FirmwareVersion2);
  } else {
    DEBUG ((DEBUG_ERROR, "TPM Capsule Update is not supported - %r\n", EFI_UNSUPPORTED));
    return EFI_UNSUPPORTED;
  }

  EsrtTpmLowestSupportedFwVersion = 0;
  if (FeaturePcdGet (PcdH2ODxeCpUpdateAcpiEsrtTpmFwVersionSupported)) {
    H2O_DXE_CP_UPDATE_ACPI_ESRT_TPM_FW_VERSION_DATA        CpUpdateAcpiEsrtTpmFwVersionData;

    CpUpdateAcpiEsrtTpmFwVersionData.Size                           = sizeof (H2O_DXE_CP_UPDATE_ACPI_ESRT_TPM_FW_VERSION_DATA);
    CpUpdateAcpiEsrtTpmFwVersionData.Status                         = H2O_CP_TASK_NORMAL;
    CpUpdateAcpiEsrtTpmFwVersionData.TpmVersion                     = CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20DtpmGuid) ? H2O_DXE_CP_TPM_2_0 : H2O_DXE_CP_TPM_1_2;
    CpUpdateAcpiEsrtTpmFwVersionData.ManufacturerId                 = ManufacturerId;
    CpUpdateAcpiEsrtTpmFwVersionData.FwVersion                      = ((UINT64)FirmwareVersion1 << 32) | FirmwareVersion2;
    CpUpdateAcpiEsrtTpmFwVersionData.FwVersionInEsrt                = EsrtTpmFwVersion;
    CpUpdateAcpiEsrtTpmFwVersionData.LowestSupportedFwVersionInEsrt = EsrtTpmLowestSupportedFwVersion;

    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2ODxeCpUpdateAcpiEsrtTpmFwVersionGuid));
    H2OCpTrigger (&gH2ODxeCpUpdateAcpiEsrtTpmFwVersionGuid, &CpUpdateAcpiEsrtTpmFwVersionData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", CpUpdateAcpiEsrtTpmFwVersionData.Status));
    if (CpUpdateAcpiEsrtTpmFwVersionData.Status == H2O_CP_TASK_UPDATE) {
      EsrtTpmFwVersion                = CpUpdateAcpiEsrtTpmFwVersionData.FwVersionInEsrt;
      EsrtTpmLowestSupportedFwVersion = CpUpdateAcpiEsrtTpmFwVersionData.LowestSupportedFwVersionInEsrt;
    }
  }

  *FirmwareVersion                = EsrtTpmFwVersion;
  *LowestSupportedFirmwareVersion = EsrtTpmLowestSupportedFwVersion;

  return EFI_SUCCESS;
}

/**
  Prepare TPM for the update.

  @param[in]  Capsule              Points to the new image.
  @param[in]  CapsuleSize          Size of the new image in bytes.

  @retval EFI_SUCCESS            The image was successfully checked.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATION The operation could not be performed due to an authentication failure.

**/
STATIC
EFI_STATUS
TpmFirmwareUpdatePreConfig (
  VOID
  )
{
  EFI_STATUS        Status;

  Status = EFI_SUCCESS;
  if (CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20DtpmGuid)) {
    //
    // Set platfomAuth empty before the update
    //
    Status = Tpm20SetPlatformAuthEmpty ();
  } else if (CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid)) {
    //
    // Clear the ownership
    //
    Status = Tpm12Clear ();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "TPM Firmware Update Pre-Config: Tpm12Clear - %r\n", Status));
    }
    //
    // Issue deferredPhysicalPresence for TPM_FieldUpgrade
    //
    Status = Tpm12DeferredPhysicalPresence ();
  }
  return Status;
}

/**
  Checks if the firmware image is valid for the device.

  This function allows firmware update application to validate the firmware image without
  invoking the SetImage() first.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  ImageIndex         A unique number identifying the firmware image(s) within the device.
                                 The number is between 1 and DescriptorCount.
  @param[in]  Capsule            Points to the new image.
  @param[in]  CapsuleSize        Size of the new image in bytes.
  @param[out] ImageUpdatable     Indicates if the new image is valid for update. It also provides,
                                 if available, additional information if the image is invalid.

  @retval EFI_SUCCESS            The image was successfully checked.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATION The operation could not be performed due to an authentication failure.

**/
EFI_STATUS
EFIAPI
TpmFmpCheckImage (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL       *This,
  IN     UINT8                                  ImageIndex,
  IN     CONST VOID                             *Capsule,
  IN     UINTN                                  CapsuleSize,
  OUT    UINT32                                 *ImageUpdatable
  )
{
  EFI_STATUS                                    Status;
  UINTN                                         Index;
  UINT32                                        AttemptVersion;
  ESRT_STATUS                                   AttemptStatus;
  UINT32                                        LastAttemptVersion;
  ESRT_STATUS                                   LastAttemptStatus;
  EFI_SYSTEM_RESOURCE_TABLE                     *Esrt;
  UINT32                                        CheckImageResult;

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
    if (CompareGuid (&Esrt->FirmwareResources[Index].FirmwareClass, &((EFI_CAPSULE_HEADER *)Capsule)->CapsuleGuid)) {
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

  CheckImageResult = CheckCapsuleIntegrity (Capsule, CapsuleSize);
  if (CheckImageResult != IMAGE_UPDATABLE_VALID) {
    //
    // Update capsule status onto corresponding variables
    //
    AttemptStatus = ESRT_ERROR_INVALID_IMAGE_FORMAT;
    UpdateCapsuleStatus (&((EFI_CAPSULE_HEADER*)Capsule)->CapsuleGuid, AttemptVersion, AttemptStatus, EFI_UNSUPPORTED);
  }

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
TpmFmpGetImageInfo (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL       *This,
  IN OUT UINTN                                  *ImageInfoSize,
  IN OUT EFI_FIRMWARE_IMAGE_DESCRIPTOR          *ImageInfo,
  OUT    UINT32                                 *DescriptorVersion,
  OUT    UINT8                                  *DescriptorCount,
  OUT    UINTN                                  *DescriptorSize,
  OUT    UINT32                                 *PackageVersion,
  OUT    CHAR16                                 **PackageVersionName
  )
{
  EFI_STATUS            Status;

  if (This == NULL || ImageInfoSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Status = EFI_SUCCESS;
  if (*ImageInfoSize < sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR)) {
    Status = EFI_BUFFER_TOO_SMALL;
  }
  *ImageInfoSize = sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR);
  if (DescriptorVersion != NULL) {
    *DescriptorVersion = EFI_FIRMWARE_IMAGE_DESCRIPTOR_VERSION;
  }
  if (DescriptorCount != NULL) {
    *DescriptorCount = 1;
  }
  if (DescriptorSize != NULL) {
    *DescriptorSize = sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR);
  }
  if (PackageVersion != NULL) {
    //
    // PackageVersion is not supported
    //
    *PackageVersion = 0xFFFFFFFF;
  }
  if (PackageVersionName != NULL) {
    //
    // PackageVersionName is not supported
    //
    *PackageVersionName = NULL;
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (ImageInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ZeroMem (ImageInfo, *ImageInfoSize);

  ImageInfo[0].AttributesSupported = IMAGE_ATTRIBUTE_IMAGE_UPDATABLE;
  ImageInfo[0].AttributesSetting   = IMAGE_ATTRIBUTE_IMAGE_UPDATABLE;
  CopyGuid (&ImageInfo[0].ImageTypeId, PcdGetPtr (PcdH2OEsrtTpmFirmwareGuid));

  return Status;
}

/**
  Retrieves a copy of the current firmware image of the device.

  This function allows a copy of the current firmware image to be created and saved.
  The saved copy could later been used, for example, in firmware image recovery or rollback.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  ImageIndex         A unique number identifying the firmware image(s) within the device.
                                 The number is between 1 and DescriptorCount.
  @param[out] Image              Points to the buffer where the current image is copied to.
  @param[out] ImageSize          On entry, points to the size of the buffer pointed to by Image, in bytes.
                                 On return, points to the length of the image, in bytes.

  @retval EFI_SUCCESS            The device was successfully updated with the new image.
  @retval EFI_BUFFER_TOO_SMALL   The buffer specified by ImageSize is too small to hold the
                                 image. The current buffer size needed to hold the image is returned
                                 in ImageSize.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_NOT_FOUND          The current image is not copied to the buffer.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATIO  The operation could not be performed due to an authentication failure.

**/
EFI_STATUS
EFIAPI
TpmFmpGetImage (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL       *This,
  IN     UINT8                                  ImageIndex,
  IN OUT VOID                                   *Image,
  IN OUT UINTN                                  *ImageSize
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Process UEFI FMP in capsule image

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
TpmFmpSetImage (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL               *This,
  IN     UINT8                                          ImageIndex,
  IN     CONST VOID                                     *Capsule,
  IN     UINTN                                          CapsuleSize,
  IN     CONST VOID                                     *VendorCode,
  IN     EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS  Progress,
  OUT    CHAR16                                         **AbortReason
  )
{
  EFI_STATUS                                    Status;
  UINT32                                        CheckImageResult;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER        *FmpCapsuleHeader;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER  *ImageHeader;
  EFI_HANDLE                                    ImageHandle;
  UINT64                                        *ItemOffsetList;
  UINT32                                        ItemNum;
  UINTN                                         Index;
  UINTN                                         ExitDataSize;
  EFI_HANDLE                                    *HandleBuffer;
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL              *Fmp;
  UINTN                                         NumberOfHandles;
  UINTN                                         DescriptorSize;
  UINT8                                         FmpImageInfoCount;
  UINT32                                        FmpImageInfoDescriptorVer;
  UINTN                                         ImageInfoSize;
  UINT32                                        PackageVersion;
  CHAR16                                        *PackageVersionName;
  UINT8                                         *VendorCode1;
  CHAR16                                        *AbortReason1;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR                 *FmpImageInfoBuf;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR                 *TempFmpImageInfo;
  UINTN                                         DriverLen;
  UINTN                                         Index1;
  UINTN                                         Index2;
  MEMMAP_DEVICE_PATH                            MemMapNode;
  EFI_DEVICE_PATH_PROTOCOL                      *DriverDevicePath;
  UINT32                                        ImageUpdatable;
  UINT32                                        AttemptVersion;
  ESRT_STATUS                                   AttemptStatus;
  UINT8                                         *Image;  

  Status           = EFI_SUCCESS;
  HandleBuffer     = NULL;
  ExitDataSize     = 0;
  DriverDevicePath = NULL;
  //
  // Check Image before actual action to ensure the capsule is qualified
  //
  CheckImageResult = CheckCapsuleIntegrity (Capsule, CapsuleSize);
  if (CheckImageResult != IMAGE_UPDATABLE_VALID) {
    //
    // Update capsule status onto corresponding variables
    //
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }
  FmpCapsuleHeader = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER *)((UINT8 *)Capsule + ((EFI_CAPSULE_HEADER *)Capsule)->HeaderSize);
  ItemOffsetList = (UINT64 *)(FmpCapsuleHeader + 1);
  ItemNum = FmpCapsuleHeader->EmbeddedDriverCount + FmpCapsuleHeader->PayloadItemCount;
  //
  // Capsule in which driver count and payload count are both zero is not processed.
  //
  if (ItemNum == 0) {
    Status = EFI_SUCCESS;
    goto Exit;
  }
  //
  // Try to load & start all the drivers within capsule
  //
  SetDevicePathNodeLength (&MemMapNode.Header, sizeof (MemMapNode));
  MemMapNode.Header.Type     = HARDWARE_DEVICE_PATH;
  MemMapNode.Header.SubType  = HW_MEMMAP_DP;
  MemMapNode.MemoryType      = EfiBootServicesCode;
  MemMapNode.StartingAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)Capsule;
  MemMapNode.EndingAddress   = (EFI_PHYSICAL_ADDRESS)(UINTN)((UINT8 *)Capsule + CapsuleSize - 1);
  DriverDevicePath = AppendDevicePathNode (NULL, &MemMapNode.Header);
  if (DriverDevicePath == NULL) {
    //
    // Update capsule status onto corresponding variables
    //
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  for (Index = 0; Index < FmpCapsuleHeader->EmbeddedDriverCount; Index++) {
    if (FmpCapsuleHeader->PayloadItemCount == 0 && Index == (UINTN)FmpCapsuleHeader->EmbeddedDriverCount - 1) {
      //
      // When driver is last element in the ItemOffsetList array, the driver size is calculated by reference CapsuleImageSize in EFI_CAPSULE_HEADER
      //
      DriverLen = CapsuleSize - ((EFI_CAPSULE_HEADER*)Capsule)->HeaderSize - (UINTN)ItemOffsetList[Index];
    } else {
      DriverLen = (UINTN)ItemOffsetList[Index + 1] - (UINTN)ItemOffsetList[Index];
    }
    CapsuleSecurityCheck (TRUE);
    Status = gBS->LoadImage (
                    FALSE,
                    gImageHandle,
                    DriverDevicePath,
                    (UINT8 *)FmpCapsuleHeader + ItemOffsetList[Index],
                    DriverLen,
                    &ImageHandle
                    );
    CapsuleSecurityCheck (FALSE);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Load Driver Return Status = %r\n", Status));
      continue;
    }
    Status = gBS->StartImage (ImageHandle, &ExitDataSize, NULL);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Start Driver Return Status = %r\n", Status));
      continue;
    }
  }
  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  //
  // Route payload to right FMP instance
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareManagementProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  for(Index1 = 0; Index1 < NumberOfHandles; Index1++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index1],
                    &gEfiFirmwareManagementProtocolGuid,
                    (VOID **)&Fmp
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    ImageInfoSize = 0;
    Status = Fmp->GetImageInfo (
                    Fmp,
                    &ImageInfoSize,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL
                    );
    if (Status != EFI_BUFFER_TOO_SMALL) {
      continue;
    }
    FmpImageInfoBuf = NULL;
    FmpImageInfoBuf = AllocateZeroPool (ImageInfoSize);
    if (FmpImageInfoBuf == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }
    PackageVersionName = NULL;
    Status = Fmp->GetImageInfo (
                    Fmp,
                    &ImageInfoSize,               // ImageInfoSize
                    FmpImageInfoBuf,              // ImageInfo
                    &FmpImageInfoDescriptorVer,   // DescriptorVersion
                    &FmpImageInfoCount,           // DescriptorCount
                    &DescriptorSize,              // DescriptorSize
                    &PackageVersion,              // PackageVersion
                    &PackageVersionName           // PackageVersionName
                    );
    //
    // If FMP GetInformation interface failed, skip this resource
    //
    if (EFI_ERROR (Status)) {
      FreePool (FmpImageInfoBuf);
      continue;
    }
    if (PackageVersionName != NULL) {
      FreePool (PackageVersionName);
    }
    TempFmpImageInfo = FmpImageInfoBuf;
    for (Index2 = 0, Status = EFI_NOT_FOUND; Index2 < FmpImageInfoCount; Index2++) {
      //
      // Check all the payload entry in capsule payload list
      //
      for (Index = FmpCapsuleHeader->EmbeddedDriverCount; Index < ItemNum; Index++) {
        ImageHeader  = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *)((UINT8 *)FmpCapsuleHeader + ItemOffsetList[Index]);
        if (ImageHeader->Version >= EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER_INIT_VERSION) {
          Image = (UINT8 *)(ImageHeader + 1);
        } else {
          Image = (UINT8 *)ImageHeader + OFFSET_OF(EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER, UpdateHardwareInstance);
        }
        if (CompareGuid (&ImageHeader->UpdateImageTypeId, &TempFmpImageInfo->ImageTypeId) &&
            ImageHeader->UpdateImageIndex == TempFmpImageInfo->ImageIndex) {
          ImageUpdatable = 0;
          Status = Fmp->CheckImage (
                          Fmp,
                          TempFmpImageInfo->ImageIndex,
                          (UINT8 *) Image,
                          ImageHeader->UpdateImageSize,
                          &ImageUpdatable
                          );
          if ((EFI_ERROR (Status) || ((ImageUpdatable & IMAGE_UPDATABLE_VALID) == 0)) &&
              (Status != EFI_UNSUPPORTED)) {
            continue;
          }

          Status = TpmFirmwareUpdatePreConfig ();
          if (EFI_ERROR (Status)) {
            DEBUG ((EFI_D_ERROR, "TpmFirmwareUpdatePreConfig failed - %r\n", Status));
          }

          AbortReason1 = NULL;
          VendorCode1 = (ImageHeader->UpdateVendorCodeSize == 0) ? NULL : (UINT8 *)((UINT8 *)(ImageHeader + 1) + ImageHeader->UpdateImageSize);
          Status = Fmp->SetImage (
                          Fmp,
                          TempFmpImageInfo->ImageIndex,       // ImageIndex
                          (UINT8 *) Image,                    // Image
                          ImageHeader->UpdateImageSize,       // ImageSize
                          VendorCode1,                        // VendorCode
                          Progress,                           // Progress
                          &AbortReason1                       // AbortReason
                          );
          if (AbortReason1 != NULL) {
            DEBUG ((EFI_D_ERROR, "%s\n", AbortReason1));
            FreePool (AbortReason1);
          }
          if (!EFI_ERROR (Status)) {
            //
            // Update succeeded, return EFI_SUCCESS
            //
            FreePool (FmpImageInfoBuf);
            goto Exit;
          }
        }
      }
      //
      // Use DescriptorSize to move ImageInfo Pointer to stay compatible with different ImageInfo version
      //
      TempFmpImageInfo = (EFI_FIRMWARE_IMAGE_DESCRIPTOR *)((UINT8 *)TempFmpImageInfo + DescriptorSize);
    }
    FreePool (FmpImageInfoBuf);
  }

Exit:
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }
  if (DriverDevicePath != NULL) {
    FreePool (DriverDevicePath);
  }
  //
  // Update capsule status onto corresponding variables
  // Intend using the version in ESRT as attempt version, since there's no way to know the version of the updating image.
  //
  AttemptVersion = GetFirmwareVersionInEsrt ((EFI_CAPSULE_HEADER *)Capsule);
  AttemptStatus  = ConvertEfiStatusToEsrtStatus (Status);
  UpdateCapsuleStatus (&((EFI_CAPSULE_HEADER *)Capsule)->CapsuleGuid, AttemptVersion, AttemptStatus, SecurityViolationStatusUpdate (Status));
  return Status;
}

/**
  Returns information about the firmware package.

  This function returns package information.

  @param[in]  This                     A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[out] PackageVersion           A version number that represents all the firmware images in the device.
                                       The format is vendor specific and new version must have a greater value
                                       than the old version. If PackageVersion is not supported, the value is
                                       0xFFFFFFFF. A value of 0xFFFFFFFE indicates that package version
                                       comparison is to be performed using PackageVersionName. A value of
                                       0xFFFFFFFD indicates that package version update is in progress.
  @param[out] PackageVersionName       A pointer to a pointer to a null-terminated string representing
                                       the package version name. The buffer is allocated by this function with
                                       AllocatePool(), and it is the caller's responsibility to free it with a
                                       call to FreePool().
  @param[out] PackageVersionNameMaxLen The maximum length of package version name if device supports update of
                                       package version name. A value of 0 indicates the device does not support
                                       update of package version name. Length is the number of Unicode characters,
                                       including the terminating null character.
  @param[out] AttributesSupported      Package attributes that are supported by this device. See 'Package Attribute
                                       Definitions' for possible returned values of this parameter. A value of 1
                                       indicates the attribute is supported and the current setting value is
                                       indicated in AttributesSetting. A value of 0 indicates the attribute is not
                                       supported and the current setting value in AttributesSetting is meaningless.
  @param[out] AttributesSetting        Package attributes. See 'Package Attribute Definitions' for possible returned
                                       values of this parameter

  @retval EFI_SUCCESS                  The package information was successfully returned.
  @retval EFI_UNSUPPORTED              The operation is not supported.

**/
EFI_STATUS
EFIAPI
TpmFmpGetPackageInfo (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL       *This,
  OUT    UINT32                                 *PackageVersion,
  OUT    CHAR16                                 **PackageVersionName,
  OUT    UINT32                                 *PackageVersionNameMaxLen,
  OUT    UINT64                                 *AttributesSupported,
  OUT    UINT64                                 *AttributesSetting
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Updates information about the firmware package.

  This function updates package information.
  This function returns EFI_UNSUPPORTED if the package information is not updatable.
  VendorCode enables vendor to implement vendor-specific package information update policy.
  Null if the caller did not specify this policy or use the default policy.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  Image              Points to the authentication image.
                                 Null if authentication is not required.
  @param[in]  ImageSize          Size of the authentication image in bytes.
                                 0 if authentication is not required.
  @param[in]  VendorCode         This enables vendor to implement vendor-specific firmware
                                 image update policy.
                                 Null indicates the caller did not specify this policy or use
                                 the default policy.
  @param[in]  PackageVersion     The new package version.
  @param[in]  PackageVersionName A pointer to the new null-terminated Unicode string representing
                                 the package version name.
                                 The string length is equal to or less than the value returned in
                                 PackageVersionNameMaxLen.

  @retval EFI_SUCCESS            The device was successfully updated with the new package
                                 information.
  @retval EFI_INVALID_PARAMETER  The PackageVersionName length is longer than the value
                                 returned in PackageVersionNameMaxLen.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATIO  The operation could not be performed due to an authentication failure.

**/
EFI_STATUS
EFIAPI
TpmFmpSetPackageInfo (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL       *This,
  IN     CONST VOID                             *Image,
  IN     UINTN                                  ImageSize,
  IN     CONST VOID                             *VendorCode,
  IN     UINT32                                 PackageVersion,
  IN     CONST CHAR16                           *PackageVersionName
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Update ESRT.

  @retval EFI_SUCCESS         Operation completed successfully.
  @retval Others              Operation was unsuccessful.
**/
VOID
EFIAPI
UpdateEsrtCallbackEvent (
  IN EFI_EVENT                  Event,
  IN H2O_CP_HANDLE              Handle
  )
{
  EFI_STATUS                    Status;
  EFI_SYSTEM_RESOURCE_TABLE     *Esrt;
  UINTN                         Size;
  UINT32                        AttemptVersion;
  ESRT_STATUS                   AttemptStatus;
  UINT32                        FirmwareVersion;
  UINT32                        LowestSupportedFirmwareVersion;

  H2OCpUnregisterHandler (Handle);

  Status = EfiGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, (VOID **)&Esrt);
  if (Status != EFI_SUCCESS || Esrt == NULL) {
    return;
  }

  if ((Esrt->FirmwareResourceCount + 1) > Esrt->FirmwareResourceMaximum) {
    //
    // Over maximum table count, it should enlarge ESRT table
    //
    return;
  }

  FirmwareVersion                = 0;
  LowestSupportedFirmwareVersion = 0;
  Status = GetTpmFirmwareVersion (&FirmwareVersion, &LowestSupportedFirmwareVersion);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Update ESRT table for TPM capsule
  //
  Esrt->FirmwareResources[Esrt->FirmwareResourceCount].FirmwareClass                   = *(EFI_GUID *)PcdGetPtr (PcdH2OEsrtTpmFirmwareGuid);
  Esrt->FirmwareResources[Esrt->FirmwareResourceCount].FirmwareType                    = DEVICE_FIRMWARE_TYPE;
  Esrt->FirmwareResources[Esrt->FirmwareResourceCount].FirmwareVersion                 = FirmwareVersion;
  Esrt->FirmwareResources[Esrt->FirmwareResourceCount].LowestSupportedFirmwareVersion  = LowestSupportedFirmwareVersion;
  Esrt->FirmwareResources[Esrt->FirmwareResourceCount].CapsuleFlags                    = 0x8010;
  Esrt->FirmwareResources[Esrt->FirmwareResourceCount].LastAttemptVersion              = 0;
  Esrt->FirmwareResources[Esrt->FirmwareResourceCount].LastAttemptStatus               = 0;

  Size = sizeof (UINT32);
  Status = CommonGetVariable (
             ESRT_LAST_ATTEMPT_VERSION,
             &Esrt->FirmwareResources[Esrt->FirmwareResourceCount].FirmwareClass,
             &Size,
             &AttemptVersion
             );
  if (!EFI_ERROR (Status)) {
    Esrt->FirmwareResources[Esrt->FirmwareResourceCount].LastAttemptVersion            = AttemptVersion;
  }

  Size = sizeof(ESRT_STATUS);
  Status = CommonGetVariable (
             ESRT_LAST_ATTEMPT_STATUS,
             &Esrt->FirmwareResources[Esrt->FirmwareResourceCount].FirmwareClass,
             &Size,
             &AttemptStatus
             );
  if (!EFI_ERROR (Status)) {
    Esrt->FirmwareResources[Esrt->FirmwareResourceCount].LastAttemptStatus             = AttemptStatus;
  }

  Esrt->FirmwareResourceCount = Esrt->FirmwareResourceCount + 1;

  //
  // Add an ESRT into configuration table
  //
  Status = gBS->InstallConfigurationTable (&gEfiSystemResourceTableGuid, Esrt);
}

/**
  The driver's entry point.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval Others          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
DriverEntry (
  IN    EFI_HANDLE                  ImageHandle,
  IN    EFI_SYSTEM_TABLE            *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    Handle;
  H2O_CP_HANDLE                 CpHandle;

  if (!(CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20DtpmGuid) ||
        CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid))) {
    DEBUG((EFI_D_INFO, "NO DTPM presented\n"));
    return EFI_UNSUPPORTED;
  }

  if (GetFirstGuidHob (&gTpmErrorHobGuid) != NULL) {
    DEBUG ((EFI_D_ERROR, "TPM error!\n"));
    return EFI_DEVICE_ERROR;
  }

  if (PcdGetBool (PcdTpmHide)) {
    DEBUG((EFI_D_INFO, "TPM is hidden\n"));
    return EFI_UNSUPPORTED;
  }

  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_TPM_FMP_INSTALLATION) != 0) {
    DEBUG ((EFI_D_INFO, "TPM FMP is not supported by design\n"));
    return EFI_UNSUPPORTED;
  }

  if (FeaturePcdGet (PcdH2OBdsCpDisplayBeforeSupported)) {
    //
    // Update device firmware ESRT table
    //
    H2OCpRegisterHandler (
      &gH2OBdsCpDisplayBeforeProtocolGuid,
      UpdateEsrtCallbackEvent,
      H2O_CP_LOW,
      &CpHandle
      );
  }

  //
  // Setup instance
  //
  mTpmFirmwareManagement.GetImageInfo   = TpmFmpGetImageInfo;
  mTpmFirmwareManagement.GetImage       = TpmFmpGetImage;
  mTpmFirmwareManagement.SetImage       = TpmFmpSetImage;
  mTpmFirmwareManagement.CheckImage     = TpmFmpCheckImage;
  mTpmFirmwareManagement.GetPackageInfo = TpmFmpGetPackageInfo;
  mTpmFirmwareManagement.SetPackageInfo = TpmFmpSetPackageInfo;

  //
  // Install FMP into image handle
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiFirmwareManagementProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mTpmFirmwareManagement
                  );

  return Status;
}
