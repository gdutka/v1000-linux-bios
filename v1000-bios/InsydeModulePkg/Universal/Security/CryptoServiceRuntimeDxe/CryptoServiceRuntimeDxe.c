/** @file
  CryptoServices protocol and Hash protocol driver instance
  It will implement two phase function, runtime and boot service.

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiSmm.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/CryptoServices.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadFile.h>
#include <Protocol/LoadFile2.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/DebugLib.h>
#include <Library/ImageRelocationLib.h>

#include <Guid/FileInfo.h>
#include <Guid/EventGroup.h>

#include <IndustryStandard/PeImage.h>

#include "CryptoHash.h"
#include "CryptoServiceProtocol.h"
#include "CryptoServiceRuntimeDxe.h"

EFI_SMM_SYSTEM_TABLE2                   *mSmst;
HASH2_INSTANCE_DATA                     mRootHash2Instance;
extern EFI_PKCS7_VERIFY_PROTOCOL        mPkcs7Verify;
/**
  Constructor routine for runtime crypt library instance.

  The constructor function pre-allocates space for runtime cryptographic operation.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS          The construction succeeded.
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory.

**/
EFI_STATUS
EFIAPI
RuntimeCryptLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

CRYPTO_SERVICE                         *mCryptoService;
EFI_EVENT                              mVirtualAddrChange;
EFI_HANDLE                             mImageHandle;
EFI_HANDLE                             mNewImageHandle;

/*
  Change address to virtual memory address

  This function will change CryptoService protocol function to Runtime Phase.
*/
VOID
EFIAPI
CryptoServicesVirtualAddressChange (
  EFI_EVENT                            Event,
  VOID                                 *Context
  )
{
  CRYPTO_SERVICE                       *ProtocolServices;
  UINTN                                Index;

  ProtocolServices = Context;

  //
  // Change CryptoService protocol function to Runtime phase
  //
  SetupCryptoService (
    ProtocolServices->Handle,
    ProtocolServices,
    CRYPTO_SERVICE_CHANGE_TO_RUNTIME,
    NULL,
    NULL,
    NULL,
    NULL
    );

  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->HashInstance.HashProtocol.GetHashSize));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->HashInstance.HashProtocol.Hash));
  for (Index = 0; Index < CRYPTO_ALGORITHM_MAX; Index++) {
    gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->HashInstance.Context[Index]));
  }
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->HashInstance));

  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->Hash2Instance.Hash2Protocol.GetHashSize));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->Hash2Instance.Hash2Protocol.Hash));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->Hash2Instance.Hash2Protocol.HashInit));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->Hash2Instance.Hash2Protocol.HashUpdate));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->Hash2Instance.Hash2Protocol.HashFinal));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->Hash2Instance));

  CONVERT_CRYPTO_FUNCTION (Sha1GetContextSize);
  CONVERT_CRYPTO_FUNCTION (Sha1Init);
  CONVERT_CRYPTO_FUNCTION (Sha1Duplicate);
  CONVERT_CRYPTO_FUNCTION (Sha1Update);
  CONVERT_CRYPTO_FUNCTION (Sha1Final);

  CONVERT_CRYPTO_FUNCTION (HmacSha1GetContextSize);
  CONVERT_CRYPTO_FUNCTION (HmacSha1Init);
  CONVERT_CRYPTO_FUNCTION (HmacSha1Duplicate);
  CONVERT_CRYPTO_FUNCTION (HmacSha1Update);
  CONVERT_CRYPTO_FUNCTION (HmacSha1Final);

  CONVERT_CRYPTO_FUNCTION (TdesGetContextSize);
  CONVERT_CRYPTO_FUNCTION (TdesInit);
  CONVERT_CRYPTO_FUNCTION (TdesEcbEncrypt);
  CONVERT_CRYPTO_FUNCTION (TdesEcbDecrypt);
  CONVERT_CRYPTO_FUNCTION (TdesCbcEncrypt);
  CONVERT_CRYPTO_FUNCTION (TdesCbcDecrypt);

  CONVERT_CRYPTO_FUNCTION (AesCbcDecrypt);
  CONVERT_CRYPTO_FUNCTION (AesCbcEncrypt);
  CONVERT_CRYPTO_FUNCTION (AesEcbDecrypt);
  CONVERT_CRYPTO_FUNCTION (AesEcbEncrypt);
  CONVERT_CRYPTO_FUNCTION (AesGetContextSize);
  CONVERT_CRYPTO_FUNCTION (AesInit);
  CONVERT_CRYPTO_FUNCTION (Pkcs7Verify);
  CONVERT_CRYPTO_FUNCTION (Pkcs7VerifyUsingPubKey);

  CONVERT_CRYPTO_FUNCTION (RsaGetPrivateKeyFromPem);
  CONVERT_CRYPTO_FUNCTION (RsaGetPublicKeyFromX509);

  CONVERT_CRYPTO_FUNCTION (X509GetSubjectName);
  CONVERT_CRYPTO_FUNCTION (X509VerifyCert);

  CONVERT_CRYPTO_FUNCTION (RsaFree);
  CONVERT_CRYPTO_FUNCTION (RsaNew);
  CONVERT_CRYPTO_FUNCTION (RsaSetKey);
  CONVERT_CRYPTO_FUNCTION (RsaPkcs1Sign);
  CONVERT_CRYPTO_FUNCTION (RsaPkcs1Verify);

  CONVERT_CRYPTO_FUNCTION (DhNew);
  CONVERT_CRYPTO_FUNCTION (DhFree);
  CONVERT_CRYPTO_FUNCTION (DhGenerateParameter);
  CONVERT_CRYPTO_FUNCTION (DhSetParameter);
  CONVERT_CRYPTO_FUNCTION (DhGenerateKey);
  CONVERT_CRYPTO_FUNCTION (DhComputeKey);

  CONVERT_CRYPTO_FUNCTION (RandomSeed);
  CONVERT_CRYPTO_FUNCTION (RandomBytes);
  CONVERT_CRYPTO_FUNCTION (AuthenticodeVerify);

  CONVERT_CRYPTO_FUNCTION (Pkcs7GetSigners);
  CONVERT_CRYPTO_FUNCTION (Pkcs7FreeSigners);
  CONVERT_CRYPTO_FUNCTION (RsaPkcs1Decrypt);
  CONVERT_CRYPTO_FUNCTION (X509GetIssuerName);
  CONVERT_CRYPTO_FUNCTION (ImageTimestampVerify);
  CONVERT_CRYPTO_FUNCTION (X509GetTBSCert);

  CONVERT_CRYPTO_FUNCTION (HmacSha256GetContextSize);
  CONVERT_CRYPTO_FUNCTION (HmacSha256Init);
  CONVERT_CRYPTO_FUNCTION (HmacSha256Duplicate);
  CONVERT_CRYPTO_FUNCTION (HmacSha256Update);
  CONVERT_CRYPTO_FUNCTION (HmacSha256Final);
  CONVERT_CRYPTO_FUNCTION (Pkcs7GetCertificatesList);
  CONVERT_CRYPTO_FUNCTION (Pbkdf2CreateKey);
  CONVERT_CRYPTO_FUNCTION (Sm3GetContextSize);
  CONVERT_CRYPTO_FUNCTION (Sm3Init);
  CONVERT_CRYPTO_FUNCTION (Sm3Duplicate);
  CONVERT_CRYPTO_FUNCTION (Sm3Update);
  CONVERT_CRYPTO_FUNCTION (Sm3Final);

  CONVERT_CRYPTO_FUNCTION (RsaPkcs1Encrypt);
  CONVERT_CRYPTO_FUNCTION (RsaPkcs1OaepEncrypt);
  CONVERT_CRYPTO_FUNCTION (RsaPkcs1OaepDecrypt);
  CONVERT_CRYPTO_FUNCTION (RsaPkcs1PssVerify);

  CONVERT_CRYPTO_FUNCTION (HkdfExtractAndExpand);

  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->CryptoServiceProtocol));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->CryptoService2Protocol));

  CryptHashVirtualAddressChange ();
}


/**
  Install CryptoServices protocol.
  The Runtime boolean is indicate install to Runtime or BootService phase

  In BootService driver, it will install Hash and CryptoSerice with CryptoServiceBootService GUID.
  In Runtime driver, it will locate BootService protocol, and replce to CryptoService protocol.

  @param  Runtime                    Phase indicator

  @retval EFI_SUCCESS                Protocol install success
  @retval others                     Failed from another driver
**/
EFI_STATUS
CryptoServiceInstallProtocol (
  BOOLEAN                              Runtime
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;
  EFI_HASH_PROTOCOL                    *HashProtocol;
  EFI_HASH2_PROTOCOL                   *Hash2Protocol;
  CRYPTO_SERVICES_PROTOCOL             *CryptoService;
  CRYPTO_SERVICES2_PROTOCOL            *CryptoService2;

  EFI_GUID  CryptoServiceBootServiceProtocolGuid      = CRYPTO_SERVICE_BOOT_SERVICES_CRYPTO_PROTOCOL_GUID;
  EFI_GUID  CryptoServiceBootService2ProtocolGuid     = CRYPTO_SERVICE_BOOT_SERVICES_CRYPTO2_PROTOCOL_GUID;
  EFI_GUID  CryptoServiceBootServiceHashProtocolGuid  = CRYPTO_SERVICE_BOOT_SERVICES_HASH_PROTOCOL_GUID;
  EFI_GUID  CryptoServiceBootServiceHash2ProtocolGuid = CRYPTO_SERVICE_BOOT_SERVICES_HASH2_PROTOCOL_GUID;

  HashProtocol   = NULL;
  Hash2Protocol  = NULL;
  CryptoService  = NULL;
  CryptoService2 = NULL;

  if (Runtime) {
    mCryptoService = AllocateRuntimePool (sizeof (CRYPTO_SERVICE));
  } else {
    mCryptoService = AllocatePool (sizeof (CRYPTO_SERVICE));
  }

  if (mCryptoService == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (mCryptoService, sizeof (CRYPTO_SERVICE));

  if (Runtime) {
    Status = gBS->LocateProtocol (&CryptoServiceBootServiceHashProtocolGuid, NULL, (VOID **)&HashProtocol);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = gBS->LocateProtocol (&CryptoServiceBootServiceHash2ProtocolGuid, NULL, (VOID **)&Hash2Protocol);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = gBS->LocateProtocol (&CryptoServiceBootServiceProtocolGuid,     NULL, (VOID **)&CryptoService);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = gBS->LocateProtocol (&CryptoServiceBootService2ProtocolGuid,     NULL, (VOID **)&CryptoService2);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  SetupCryptoService (mImageHandle, mCryptoService, Runtime, HashProtocol, Hash2Protocol, CryptoService, CryptoService2);

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  (Runtime) ? &gEfiHashProtocolGuid        : &CryptoServiceBootServiceHashProtocolGuid,
                  (VOID *)  &(mCryptoService->HashInstance.HashProtocol),
                  (Runtime) ? &gEfiHash2ProtocolGuid        : &CryptoServiceBootServiceHash2ProtocolGuid,
                  (VOID *)  &(mCryptoService->Hash2Instance.Hash2Protocol),
                  (Runtime) ? &gCryptoServicesProtocolGuid : &CryptoServiceBootServiceProtocolGuid,
                  (VOID *) &(mCryptoService->CryptoServiceProtocol),
                  (Runtime) ? &gCryptoServices2ProtocolGuid : &CryptoServiceBootService2ProtocolGuid,
                  (VOID *) &(mCryptoService->CryptoService2Protocol),
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (mCryptoService);
    ASSERT_EFI_ERROR (Status);
  }
  return Status;
}

/*
  The entry point for PxeBc driver which install the driver
  binding and component name protocol on its image.

  @param  ImageHandle                The Image handle of the driver
  @param  SystemTable                The system table

  @retval EFI_SUCCESS                Driver initialize in RuntimeServices memory success
  @retval EFI_ALREADY_STARTED        Driver unload from BootServices memory
  @retval EFI_OUT_OF_RESOURCES       Allocate memory failed
  @retval Others                     Failed

*/
EFI_STATUS
EFIAPI
CryptoServiceSmmRuntimeEntry (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_SMM_BASE2_PROTOCOL               *SmmBase;
  BOOLEAN                              InSmm;
  EFI_HANDLE                           Handle;
  EFI_EVENT                            CryptoServicesVirtualAddressChangeEvent;

  mImageHandle   = ImageHandle;
  mSmst          = NULL;
  mCryptoService = NULL;

  //
  // SMM check
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  } else {
    InSmm = FALSE;
  }

  if (!InSmm) {
    gRT = SystemTable->RuntimeServices;

    if (!IsRuntimeDriver (ImageHandle)) {
      //
      // This section is BootService driver
      // Install CryptoService protocol for BootServices phase
      //
      Status = CryptoServiceInstallProtocol (CRYPTO_SERVICE_BOOT_SERVICES_PROTOCOL);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      Status = RelocateImageToRuntimeDriver (ImageHandle);
      ASSERT_EFI_ERROR (Status);
      return EFI_SUCCESS;
    }

    //
    // Image location is Runtime Services
    //

    //
    // Initialize BaseCryptLib Memory services
    //
    Status = RuntimeCryptLibConstructor (ImageHandle, SystemTable);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = CryptoServiceInstallProtocol (CRYPTO_SERVICE_RUNTIME_PROTOCOL);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
    //
    // Install EFI_HASH_SERVICE_BINDING_PROTOCOL
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &mImageHandle,
                    &gEfiHashServiceBindingProtocolGuid,
                    (VOID *) &mHashServiceBindingProtocol,
                    &gEfiHash2ServiceBindingProtocolGuid,
                    (VOID *) &mHash2ServiceBindingProtocol,
                    &gEfiPkcs7VerifyProtocolGuid,
                    (VOID *) &mPkcs7Verify,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
    //
    // Create Event for change Runtime Function Pointer
    //
    CryptoServicesVirtualAddressChangeEvent = NULL;
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    CryptoServicesVirtualAddressChange,
                    (VOID *) mCryptoService,
                    &gEfiEventVirtualAddressChangeGuid,
                    &CryptoServicesVirtualAddressChangeEvent
                    );
  } else {
    //
    // Get Smm Syatem Table
    //
    Status = SmmBase->GetSmstLocation(
                        SmmBase,
                        &mSmst
                        );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    mCryptoService = AllocatePool (sizeof (CRYPTO_SERVICE));
    if (mCryptoService == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    SetupCryptoService (ImageHandle, mCryptoService, FALSE, NULL, NULL, NULL, NULL);

    //
    // SMM Entry
    //
    Handle = NULL;
    Status = mSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gEfiHashProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      (VOID *) &(mCryptoService->HashInstance.HashProtocol)
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = mSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gEfiHash2ProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      (VOID *) &(mCryptoService->Hash2Instance.Hash2Protocol)
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = mSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gCryptoServicesProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      (VOID *) &(mCryptoService->CryptoServiceProtocol)
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = mSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gCryptoServices2ProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      (VOID *) &(mCryptoService->CryptoService2Protocol)
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    return EFI_SUCCESS;
  }

  return Status;
}
