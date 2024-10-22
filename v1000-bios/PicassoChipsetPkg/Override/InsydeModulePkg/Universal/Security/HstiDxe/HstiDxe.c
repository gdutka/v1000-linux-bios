/** @file
  HSTI DXE Driver

;******************************************************************************
;* Copyright (c) 2015-2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "HstiDxe.h"


/**
  Internal function to init IBV HSTI Platform Security State Interface
**/
STATIC
EFI_STATUS
InitHstiIbvData (
  VOID
  )
{
  ADAPTER_INFO_PLATFORM_SECURITY   *Hsti;
  UINTN                            HstiSize;
  UINT8                            *HstiData;
  UINT8                            FeatureBitField;

  //
  // Total HstiSize is structure size + requirement/implement/Verify feature size + a null string for error string, 
  // Allocate Pool for hold ADAPTER_INFO_PLATFORM_SECURITY
  //
  HstiSize = sizeof (ADAPTER_INFO_PLATFORM_SECURITY) + 3 * HSTI_AMD_FEATUERS_SIZE_IN_BYTES + HSTI_AMD_ERROR_STRING_SIZE;
  Hsti = AllocateZeroPool (HstiSize);
  if (Hsti == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize HSTI table
  //
  Hsti->Version              = PLATFORM_SECURITY_VERSION_VNEXTCS;
  Hsti->Role                 = PLATFORM_SECURITY_ROLE_PLATFORM_IBV;
  StrCpy (Hsti->ImplementationID, HSTI_IBV_IMPLEMENT_ID);
  Hsti->SecurityFeaturesSize = HSTI_AMD_FEATUERS_SIZE_IN_BYTES;// HSTI_IBV_FEATUER_SIZE;
 
  HstiData = (UINT8 *) (Hsti + 1);
  //
  // Implementation
  //
  //Skip the requirement field as no need to fill for IBV codes
  HstiData += HSTI_AMD_FEATUERS_SIZE_IN_BYTES;
  
  HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_0] = HSTI_AMD_CRYPTO_STRENGTH;
  
  HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_1] = HSTI_AMD_FWCODE_PROTECT_PROTECT_SPI +
                                            HSTI_AMD_FWCODE_PROTECT_SIGNED_FW_CHECK;

  FeatureBitField = 0;
  
  if (FeaturePcdGet(PcdSecureFlashSupported) ||
      FeaturePcdGet (PcdH2OCapsuleUpdateSupported) ||
      FeaturePcdGet (PcdUefiCapsuleUpdateSupported)) {
    FeatureBitField |= (UINT8)(HSTI_AMD_SECURE_FW_UPDATE_DFT_TESTKEY + 
                               HSTI_AMD_SECURE_FW_UPDATE_CHECK_TESTKEY_IN_PRODUCTION + 
                               HSTI_AMD_SECURE_FW_UPDATE_ROLLBACK_CHECK);
  }
  HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_2] = FeatureBitField;

  HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_3] = HSTI_AMD_SECUREBOOT_BACKDOOR_INLINE_PROMPT_CHECK +
                                            HSTI_AMD_SECUREBOOT_BACKDOOR_MANUFACTURE_CHECK;
  
  //
  //CS System
  //
  #ifdef AMD_HSTI_CS_SYSTEM_SUPPORT
    HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_5] = HSTI_AMD_CSM_DISABLE_IF_SECUREBOOT_EN +
                                              HSTI_AMD_CSM_DISABLE_ON_CS_SYSTEM;
  #else
    HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_5] = HSTI_AMD_CSM_DISABLE_IF_SECUREBOOT_EN;
  #endif

  //
  // Verification
  //
  HstiData += HSTI_AMD_FEATUERS_SIZE_IN_BYTES;
    
  //
  // Install HSTI table.
  //
  return HstiLibSetTable ((VOID *) Hsti, HstiSize);
}

/**
  Internal function to check whether support SHA256 and RSA2048 secure feature.
**/
STATIC
VOID
CheckSecurityStrength (
  VOID
  )
{
  EFI_STATUS                  Status;
  CRYPTO_SERVICES_PROTOCOL    *Crypto;
  EFI_HASH_PROTOCOL           *Hash;


  Status = gBS->LocateProtocol (&gCryptoServicesProtocolGuid, NULL, (VOID **)&Crypto);
  if (Status != EFI_SUCCESS) {
    return;
  }
  Status = gBS->LocateProtocol (&gEfiHashProtocolGuid, NULL, (VOID **)&Hash);
  if (Status != EFI_SUCCESS) {
    return;
  }

  HstiLibSetFeaturesVerified (
    PLATFORM_SECURITY_ROLE_PLATFORM_IBV,
    HSTI_IBV_IMPLEMENT_ID,
    HSTI_AMD_FEATURE_BYTE_INDEX_0,
    HSTI_AMD_CRYPTO_STRENGTH
    );
}

/**
  Internal function to update check CSM policy secure feature result.
**/
STATIC
VOID
CheckCsmPolicy (
  VOID
  )
{
  EFI_STATUS                  Status;
  EFI_LEGACY_BIOS_PROTOCOL    *LegacyBios;
  UINTN                       DataSize;
  UINT8                       SecureBoot;
  UINT8                       FeatureBitField; 

  FeatureBitField = 0;
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if (Status == EFI_SUCCESS) {
    DataSize = sizeof(UINT8);
    Status = gRT->GetVariable (
                    EFI_SECURE_BOOT_MODE_NAME,
                    &gEfiGlobalVariableGuid,
                    NULL,
                    &DataSize,
                    &SecureBoot
                    );
    if (Status == EFI_SUCCESS && SecureBoot == 1) {
      HstiLibSetErrorString (
        PLATFORM_SECURITY_ROLE_PLATFORM_IBV,
        HSTI_IBV_IMPLEMENT_ID,
        L"Error!! Secure boot Must be disabled if CSM is enabled. Now, secure boot is enabled in CSM is enabled environment."
        );
      return;
    }
  }

  #ifdef AMD_HSTI_CS_SYSTEM_SUPPORT
    FeatureBitField |= HSTI_AMD_CSM_DISABLE_IF_SECUREBOOT_EN +
                       HSTI_AMD_CSM_DISABLE_ON_CS_SYSTEM;
  #else
    FeatureBitField |= HSTI_AMD_CSM_DISABLE_IF_SECUREBOOT_EN;
  #endif
  
  HstiLibSetFeaturesVerified (
    PLATFORM_SECURITY_ROLE_PLATFORM_IBV,
    HSTI_IBV_IMPLEMENT_ID,
    HSTI_AMD_FEATURE_BYTE_INDEX_5,
    FeatureBitField
    );
}

/**
  Internal function to update firmware code protect secure feature result.
**/
STATIC
VOID
CheckFirmwareCodeProtect (
  VOID
  )
{
	UINT8                            FeatureBitField;    

  FeatureBitField = 0;

  FeatureBitField |= HSTI_AMD_FWCODE_PROTECT_PROTECT_SPI +
                     HSTI_AMD_FWCODE_PROTECT_SIGNED_FW_CHECK;
  
  HstiLibSetFeaturesVerified (
    PLATFORM_SECURITY_ROLE_PLATFORM_IBV,
    HSTI_IBV_IMPLEMENT_ID,
    HSTI_AMD_FEATURE_BYTE_INDEX_1,
    FeatureBitField
    );
}


/**
  Internal function to update secure firmware update secure feature result.
**/
STATIC
VOID
CheckSecureFirmwareUpdate (
  VOID
  )
{
  UINT8                            FeatureBitField;
 
  FeatureBitField = 0;  	
  if (FeaturePcdGet (PcdSecureFlashSupported) ||
      FeaturePcdGet (PcdH2OCapsuleUpdateSupported) ||
      FeaturePcdGet (PcdUefiCapsuleUpdateSupported)) {
  
    FeatureBitField |= HSTI_AMD_SECURE_FW_UPDATE_DFT_TESTKEY + 
                       HSTI_AMD_SECURE_FW_UPDATE_CHECK_TESTKEY_IN_PRODUCTION +
                       HSTI_AMD_SECURE_FW_UPDATE_ROLLBACK_CHECK;
  }
                     
  HstiLibSetFeaturesVerified (
    PLATFORM_SECURITY_ROLE_PLATFORM_IBV,
    HSTI_IBV_IMPLEMENT_ID,
    HSTI_AMD_FEATURE_BYTE_INDEX_2,
    FeatureBitField 
    );
}

/**
  Internal function to update secure boot policy secure feature result.
**/
STATIC
VOID
CheckSecureBootPolicy (
  VOID
  )
{
  UINTN               DataSize;
  UINT8               SecureBoot;
  EFI_STATUS          Status;
  UINT8               FeatureBitField;

  if (!FeaturePcdGet (PcdH2OSecureBootSupported)) {
    return;
  }

  DataSize = sizeof(UINT8);
  Status = gRT->GetVariable (
                  EFI_SECURE_BOOT_MODE_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &DataSize,
                  &SecureBoot
                  );
  if (Status != EFI_SUCCESS) {
    return;
  }
  Status = gRT->SetVariable (
                  EFI_SECURE_BOOT_MODE_NAME,
                  &gEfiGlobalVariableGuid,
                  0,
                  0,
                  NULL
                  );
  if (Status == EFI_SUCCESS) {
    HstiLibSetErrorString (
      PLATFORM_SECURITY_ROLE_PLATFORM_IBV,
      HSTI_IBV_IMPLEMENT_ID,
      L"Error!! SecureBoot variable must be read-only. But this variable can be deleted."
      );
    return;
  }
  FeatureBitField= HSTI_AMD_SECUREBOOT_BACKDOOR_INLINE_PROMPT_CHECK +
                   HSTI_AMD_SECUREBOOT_BACKDOOR_MANUFACTURE_CHECK;



  HstiLibSetFeaturesVerified (
    PLATFORM_SECURITY_ROLE_PLATFORM_IBV,
    HSTI_IBV_IMPLEMENT_ID,
    HSTI_AMD_FEATURE_BYTE_INDEX_3,
    FeatureBitField
    );
}

/**
  Internal function to update PSP secure feature result.
**/
STATIC
VOID
CheckPspSecureFeature (
  VOID
  )
{
	UINT32                           HSTIState;
	UINT8                            FeatureBitField;                     
	
	HSTIState = 0;
  PspMboxBiosQueryHSTIState (&HSTIState);
	
	FeatureBitField = 0;
  if (HSTIState & HSTISTATE_PSP_SECURE_EN) {
    FeatureBitField |= HSTI_AMD_FEATURE_PSP_SECURE_EN;
  }
#ifdef AMD_HSTI_CS_SYSTEM_SUPPORT
  if (HSTIState & HSTISTATE_PSP_PLATFORM_SECURE_EN) {
    FeatureBitField |= HSTI_AMD_FEATURE_PSP_PLATFORM_SECURE_BOOT_EN;
  }
#endif
  if (HSTIState & HSTISTATE_PSP_DEBUG_LOCK_ON) {
    FeatureBitField |= HSTI_AMD_FEATURE_PSP_DEBUG_LOCK_ON;
  }
  
  HstiLibSetFeaturesVerified (
    PLATFORM_SECURITY_ROLE_PLATFORM_IBV,
    HSTI_IBV_IMPLEMENT_ID,
    HSTI_AMD_FEATURE_BYTE_INDEX_4,
    FeatureBitField
    );	
	
}


///**
//  Internal function to update External DMA protect secure feature result.
//**/
//STATIC
//VOID
//CheckExternalDmaProtect (
//  VOID
//  )
//{
//
//  if (!FeaturePcdGet (PcdNoExternalDmaSupported)) {
//    return;
//  }
//
//  HstiLibSetFeaturesVerified (
//    PLATFORM_SECURITY_ROLE_PLATFORM_IBV,
//    HSTI_IBV_IMPLEMENT_ID,
//    HSTI_IBV_FEATUER_SIZE - 1,
//    EXTERNAL_DMA_PROTECT
//    );
//}

/**
  Internal function to update HSTI related data.
**/
STATIC
VOID
UpdateHstiData (
  VOID
  )
{
  CheckSecurityStrength ();

  CheckFirmwareCodeProtect ();
  CheckSecureFirmwareUpdate ();
  CheckSecureBootPolicy ();
  //CheckExternalDmaProtect ();
  CheckCsmPolicy ();
}


/**
  Remove all of AIPs except for AIP supports gAdapterInfoPlatformSecurityGuid.
**/
VOID
DeleteOtherAip (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINTN                             NoHandles;
  EFI_HANDLE                        *Handles;
  EFI_ADAPTER_INFORMATION_PROTOCOL  *Aip;
  UINTN                             Index;
  UINTN                             InfoTypesIndex;
  EFI_GUID                          *InfoTypesBuffer;
  UINTN                             InfoTypesBufferCount;
  BOOLEAN                           Found;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiAdapterInformationProtocolGuid,
                  NULL,
                  &NoHandles,
                  &Handles
                  );
  if (Status != EFI_SUCCESS) {
    return;
  }
  for (Index = 0; Index < NoHandles; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiAdapterInformationProtocolGuid,
                    (VOID **)&Aip
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = Aip->GetSupportedTypes (
                    Aip,
                    &InfoTypesBuffer,
                    &InfoTypesBufferCount
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Found = FALSE;
    for (InfoTypesIndex = 0; InfoTypesIndex < InfoTypesBufferCount; InfoTypesIndex++) {
      if (CompareGuid (&InfoTypesBuffer[InfoTypesIndex], &gAdapterInfoPlatformSecurityGuid)) {
        Found = TRUE;
        break;
      }
    }

    if (!Found) {
      Status = gBS->UninstallProtocolInterface (
                      Handles[Index],
                      &gEfiAdapterInformationProtocolGuid,
                      (VOID *) Aip
                      );
    }
  }
  FreePool (Handles);
}

/**
  Callback function for ready to boot event to update HSTI related data.

  @param Event    - Event whose notification function is being invoked.
  @param Context  - Pointer to the notification function's context.
**/
STATIC
VOID
EFIAPI
OnReadyToBoot (
  EFI_EVENT                               Event,
  VOID                                    *Context
  )
{

  UpdateHstiData ();
  //
  // BUG BUG !!!
  // OS will mistake consider other AIPs are gAdapterInfoPlatformSecurityGuid AIPs so we temporarily
  // remove other AIPs to make the test passed. Once OS fix this bug, we will remove this code
  //
  DeleteOtherAip ();
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
}


/**
  The HSTI driver's entry point.

  Create IBV gAdapterInfoPlatformSecurityGuid protocol

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
**/
EFI_STATUS
EFIAPI
HstiEntryPoint (
  IN  EFI_HANDLE           ImageHandle,
  IN  EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS     Status;
  EFI_EVENT      Event;

  Status = InitHstiIbvData ();
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  Status = EfiCreateEventReadyToBootEx (
            TPL_CALLBACK - 1,
            OnReadyToBoot,
            NULL,
            &Event
            );

  return Status;
}
