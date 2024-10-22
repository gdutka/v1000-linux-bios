/** @file
  FMP Authentication PKCS7 handler.
  Provide generic FMP authentication functions for DXE/PEI post memory phase.

  Caution: This module requires additional review when modified.
  This module will have external input - capsule image.
  This external input must be validated carefully to avoid security issue like
  buffer overflow, integer overflow.

  FmpAuthenticatedHandlerPkcs7(), AuthenticateFmpImage() will receive
  untrusted input and do basic validation.

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <PiSmm.h>
#include <Guid/SystemResourceTable.h>
#include <Guid/FirmwareContentsSigned.h>
#include <Guid/WinCertificate.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/FmpAuthenticationLib.h>
#include <Library/PcdLib.h>
#include <Protocol/FirmwareManagement.h>
#include <Protocol/CryptoServices.h>
#include <Guid/SystemResourceTable.h>

#include <Protocol/SmmBase2.h>

STATIC EFI_SMM_SYSTEM_TABLE2                 *mSmst;
/**
  The handler is used to do the authentication for FMP capsule based upon
  EFI_FIRMWARE_IMAGE_AUTHENTICATION.

  Caution: This function may receive untrusted input.

  This function assumes the caller AuthenticateFmpImage()
  already did basic validation for EFI_FIRMWARE_IMAGE_AUTHENTICATION.

  @param[in]  Image                   Points to an FMP authentication image, started from EFI_FIRMWARE_IMAGE_AUTHENTICATION.
  @param[in]  ImageSize               Size of the authentication image in bytes.
  @param[in]  PublicKeyData           The public key data used to validate the signature.
  @param[in]  PublicKeyDataLength     The length of the public key data.

  @retval RETURN_SUCCESS            Authentication pass.
                                    The LastAttemptStatus should be LAST_ATTEMPT_STATUS_SUCCESS.
  @retval RETURN_SECURITY_VIOLATION Authentication fail.
                                    The LastAttemptStatus should be LAST_ATTEMPT_STATUS_ERROR_AUTH_ERROR.
  @retval RETURN_INVALID_PARAMETER  The image is in an invalid format.
                                    The LastAttemptStatus should be LAST_ATTEMPT_STATUS_ERROR_INVALID_FORMAT.
  @retval RETURN_OUT_OF_RESOURCES   No Authentication handler associated with CertType.
                                    The LastAttemptStatus should be LAST_ATTEMPT_STATUS_ERROR_INSUFFICIENT_RESOURCES.
**/
RETURN_STATUS
FmpAuthenticatedHandlerPkcs7 (
  IN EFI_FIRMWARE_IMAGE_AUTHENTICATION  *Image,
  IN UINTN                              ImageSize,
  IN CONST UINT8                        *PublicKeyData,
  IN UINTN                              PublicKeyDataLength
  )
{
  RETURN_STATUS                             Status;
  BOOLEAN                                   CryptoStatus;
  VOID                                      *P7Data;
  UINTN                                     P7Length;
  VOID                                      *TempBuffer;
  CRYPTO_SERVICES2_PROTOCOL                 *CryptoService;

  DEBUG((DEBUG_INFO, "FmpAuthenticatedHandlerPkcs7 - Image: 0x%08x - 0x%08x\n", (UINTN)Image, (UINTN)ImageSize));

  if (mSmst != NULL) {
    Status = mSmst->SmmLocateProtocol (&gCryptoServices2ProtocolGuid, NULL, (VOID**)&CryptoService);
  } else {
    Status = gBS->LocateProtocol (&gCryptoServices2ProtocolGuid, NULL, (VOID**)&CryptoService);
  }
  if (EFI_ERROR(Status)){
    return Status;
  }

  P7Length = Image->AuthInfo.Hdr.dwLength - (OFFSET_OF(WIN_CERTIFICATE_UEFI_GUID, CertData));
  P7Data = Image->AuthInfo.CertData;

  // It is a signature across the variable data and the Monotonic Count value.
  TempBuffer = AllocatePool(ImageSize - Image->AuthInfo.Hdr.dwLength);
  if (TempBuffer == NULL) {
    DEBUG((DEBUG_ERROR, "FmpAuthenticatedHandlerPkcs7: TempBuffer == NULL\n"));
    Status = RETURN_OUT_OF_RESOURCES;
    goto Done;
  }

  CopyMem(
    TempBuffer,
    (UINT8 *)Image + sizeof(Image->MonotonicCount) + Image->AuthInfo.Hdr.dwLength,
    ImageSize - sizeof(Image->MonotonicCount) - Image->AuthInfo.Hdr.dwLength
    );
  CopyMem(
    (UINT8 *)TempBuffer + ImageSize - sizeof(Image->MonotonicCount) - Image->AuthInfo.Hdr.dwLength,
    &Image->MonotonicCount,
    sizeof(Image->MonotonicCount)
    );

  CryptoStatus = CryptoService->Pkcs7Verify(
                   P7Data,
                   P7Length,
                   PublicKeyData,
                   PublicKeyDataLength,
                   (UINT8 *)TempBuffer,
                   ImageSize - Image->AuthInfo.Hdr.dwLength,
                   FALSE
                   );
  FreePool(TempBuffer);
  if (!CryptoStatus) {
    //
    // If PKCS7 signature verification fails, AUTH tested failed bit is set.
    //
    DEBUG((DEBUG_ERROR, "FmpAuthenticatedHandlerPkcs7: Pkcs7Verify() failed\n"));
    Status = RETURN_SECURITY_VIOLATION;
    goto Done;
  }
  DEBUG((DEBUG_INFO, "FmpAuthenticatedHandlerPkcs7: PASS verification\n"));

  Status = RETURN_SUCCESS;

Done:
  return Status;
}

/**
  The function is used to do the authentication for FMP capsule based upon
  EFI_FIRMWARE_IMAGE_AUTHENTICATION.

  The FMP capsule image should start with EFI_FIRMWARE_IMAGE_AUTHENTICATION,
  followed by the payload.

  If the return status is RETURN_SUCCESS, the caller may continue the rest
  FMP update process.
  If the return status is NOT RETURN_SUCCESS, the caller should stop the FMP
  update process and convert the return status to LastAttemptStatus
  to indicate that FMP update fails.
  The LastAttemptStatus can be got from ESRT table or via
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL.GetImageInfo().

  Caution: This function may receive untrusted input.

  @param[in]  Image                   Points to an FMP authentication image, started from EFI_FIRMWARE_IMAGE_AUTHENTICATION.
  @param[in]  ImageSize               Size of the authentication image in bytes.
  @param[in]  PublicKeyData           The public key data used to validate the signature.
  @param[in]  PublicKeyDataLength     The length of the public key data.

  @retval RETURN_SUCCESS            Authentication pass.
                                    The LastAttemptStatus should be LAST_ATTEMPT_STATUS_SUCCESS.
  @retval RETURN_SECURITY_VIOLATION Authentication fail.
                                    The LastAttemptStatus should be LAST_ATTEMPT_STATUS_ERROR_AUTH_ERROR.
  @retval RETURN_INVALID_PARAMETER  The image is in an invalid format.
                                    The LastAttemptStatus should be LAST_ATTEMPT_STATUS_ERROR_INVALID_FORMAT.
  @retval RETURN_UNSUPPORTED        No Authentication handler associated with CertType.
                                    The LastAttemptStatus should be LAST_ATTEMPT_STATUS_ERROR_INVALID_FORMAT.
  @retval RETURN_UNSUPPORTED        Image or ImageSize is invalid.
                                    The LastAttemptStatus should be LAST_ATTEMPT_STATUS_ERROR_INVALID_FORMAT.
  @retval RETURN_OUT_OF_RESOURCES   No Authentication handler associated with CertType.
                                    The LastAttemptStatus should be LAST_ATTEMPT_STATUS_ERROR_INSUFFICIENT_RESOURCES.
**/
RETURN_STATUS
EFIAPI
AuthenticateFmpImage (
  IN EFI_FIRMWARE_IMAGE_AUTHENTICATION  *Image,
  IN UINTN                              ImageSize,
  IN CONST UINT8                        *PublicKeyData,
  IN UINTN                              PublicKeyDataLength
  )
{
  GUID                                      *CertType;
  EFI_STATUS                                Status;

  if ((Image == NULL) || (ImageSize == 0)) {
    return RETURN_UNSUPPORTED;
  }

  if (ImageSize < sizeof(EFI_FIRMWARE_IMAGE_AUTHENTICATION)) {
    DEBUG((DEBUG_ERROR, "AuthenticateFmpImage - ImageSize too small\n"));
    return RETURN_INVALID_PARAMETER;
  }
  if (Image->AuthInfo.Hdr.dwLength <= OFFSET_OF(WIN_CERTIFICATE_UEFI_GUID, CertData)) {
    DEBUG((DEBUG_ERROR, "AuthenticateFmpImage - dwLength too small\n"));
    return RETURN_INVALID_PARAMETER;
  }
  if ((UINTN) Image->AuthInfo.Hdr.dwLength > MAX_UINTN - sizeof(UINT64)) {
    DEBUG((DEBUG_ERROR, "AuthenticateFmpImage - dwLength too big\n"));
    return RETURN_INVALID_PARAMETER;
  }
  if (ImageSize <= sizeof(Image->MonotonicCount) + Image->AuthInfo.Hdr.dwLength) {
    DEBUG((DEBUG_ERROR, "AuthenticateFmpImage - ImageSize too small\n"));
    return RETURN_INVALID_PARAMETER;
  }
  if (Image->AuthInfo.Hdr.wRevision != 0x0200) {
    DEBUG((DEBUG_ERROR, "AuthenticateFmpImage - wRevision: 0x%02x, expect - 0x%02x\n", (UINTN)Image->AuthInfo.Hdr.wRevision, (UINTN)0x0200));
    return RETURN_INVALID_PARAMETER;
  }
  if (Image->AuthInfo.Hdr.wCertificateType != WIN_CERT_TYPE_EFI_GUID) {
    DEBUG((DEBUG_ERROR, "AuthenticateFmpImage - wCertificateType: 0x%02x, expect - 0x%02x\n", (UINTN)Image->AuthInfo.Hdr.wCertificateType, (UINTN)WIN_CERT_TYPE_EFI_GUID));
    return RETURN_INVALID_PARAMETER;
  }

  CertType = &Image->AuthInfo.CertType;
  DEBUG((DEBUG_INFO, "AuthenticateFmpImage - CertType: %g\n", CertType));

  if (CompareGuid (&gEfiCertPkcs7Guid, CertType)) {
    //
    // Call the match handler to extract raw data for the input section data.
    //
    Status = FmpAuthenticatedHandlerPkcs7 (
               Image,
               ImageSize,
               PublicKeyData,
               PublicKeyDataLength
               );
    return Status;
  }

  //
  // Not found, the input guided section is not supported.
  //
  return RETURN_UNSUPPORTED;
}

/**
  Init SMST if system in SMM.

**/
STATIC
VOID
InitSmstIfInSmmInSmm (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_BASE2_PROTOCOL         *SmmBase;
  BOOLEAN                        InSmm;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **) &SmmBase
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  InSmm = FALSE;
  SmmBase->InSmm (SmmBase, &InSmm);
  if (!InSmm) {
    return;
  }

  Status = SmmBase->GetSmstLocation(
                      SmmBase,
                      &mSmst
                      );
}

/**
  The constructor function to check current mode is protected mode or SMM mode.
  If system is in SMM mode, constructor will locate gEfiSmmVariableProtocolGuid
  instance for future usage.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_NOT_FOUND   Variable region does not exist.
  @retval EFI_SUCCESS     The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
FmpAuthenticationLibConstructor (
  IN      EFI_HANDLE                ImageHandle,
  IN      EFI_SYSTEM_TABLE          *SystemTable
  )
{
  InitSmstIfInSmmInSmm ();

  return EFI_SUCCESS;

}