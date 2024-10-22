/** @file
  This library class provides services to non-SMM drivers to access
  IHISI services.

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
#include "InternalH2OIhisiLib.h"

STATIC  UINT32       mSessionToken = SIGNATURE_32 ('O', '2', 'H', '$');
STATIC  BOOLEAN      mAuthLock;
STATIC  VOID        *mInternalBuffer;
STATIC  EFI_GUID     mInternalH2OIhisiLibGuid = {0x5ee58bdc, 0x686c, 0x4d0c, 0xa9, 0xd2, 0x30, 0xba, 0x7b, 0xb4, 0xcf, 0x2e};

STATIC  UINT8        mAtpSignature[6] = {0x24, 0x49, 0x48, 0x49, 0x53, 0x49};     // $IHISI for ATP signature.

/**
  Generic function to trigger SMI

  @param  [in,out] SmiData   Pointer to IHISI_LIB_SMI_DATA data.
**/
VOID
IhisiLibGenericSmi (
  IN OUT  IHISI_LIB_SMI_DATA      *SmiData
  );

/**
  This function sets the security level to the specified level. If the specified security level is not available
  , then an error is returned. If PcdH2OIhisiAuthSupported is set to FALSE, then this function will always return
  IhisiSuccess.

  @param  [in] SecurityLevelRequested   Bitmask that specifies the IHISI security level requested. Only
                                        a single bit may be specified. Valid values are:
                                        IHISI_AUTH_NONE - Minimum security level possible.
                                        IHISI_AUTH_POST_ONLY - After Ready-To-Boot, this security level
                                                              will always fail. This lock should not persist
                                                              past the Ready-To-Lock event.
                                        IHISI_AUTH_DXE_ONLY - After End-of-DXE, this security level will always
                                                              fail. This lock should not persist past the End-Of-DXE
                                                              event.
                                        IHISI_AUTH_SECURE_ENVIRONMENT - When outside of a secure environment, sessions
                                                                        cannot be opened with this security level.
                                        IHISI_AUTH_PASSWORD - Without a valid password, this security level will always
                                                              fail. The password is specified by CredentialData.
  @param  [in] CredentialData           Pointer to Handler array with has been free by caller.

  @retval IhisiSuccess                  Function completed successfully or PcdH2OIhisiAuthSupported is set to FALSE.
  @retval IhisiInvalidParamFatalError   SecurityLevelRequested specified a security level that is not supported.
  @retval IhisiInvalidParamFatalError   CredentialData extends outside the IHISI Command Buffer.
  @return IhisiInterfaceClosed          The IHISI interface is closed.
  @retval IhisiBufferRangeError         The Flash Part Information or Flash Part Block Structure lies outside the
                                        Command Buffer.
  @return IhisiInvalidPassword          The password specified does not match.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiAuth (
  IN UINT32             SecurityLevelRequested,
  IN IHISI_AUTH_LOCK    *CredentialData           OPTIONAL
  )
{
  IHISI_STATUS_CODE         IhisiStatus;
  IHISI_LIB_SMI_DATA        SmiData;

  if (!PcdGetBool (PcdH2OIhisiAuthSupported)) {
    return IhisiSuccess;
  }
  if (!mAuthLock) {
    return IhisiInterfaceClosed;
  }

  SmiData.Eax = (IHISI_SW_SMI | (((UINTN)IhisiAuth) << 8));
  SmiData.Ebx = mSessionToken;
  SmiData.Ecx = SecurityLevelRequested;
  SmiData.Edx = PcdGet16 (PcdSoftwareSmiPort);
  SmiData.Esi = (UINT32)(UINTN)CredentialData;
  SmiData.Edi = 0;

  IhisiLibGenericSmi (&SmiData);
  IhisiStatus   = (UINT8)SmiData.Eax;
  mSessionToken = SmiData.Ebx;
  if (IhisiStatus == IhisiBufferRangeError) {
    mAuthLock = FALSE;
  }
  return IhisiStatus;
}

/**
  This function locks the IHISI interface and returns the first session token. The session token is a random number. If
  PcdH2OIhisiAuthSupported is set to FALSE, then this function will always return IhisiSuccess. The security level is
  set to IHISI_AUTH_NONE.

  This function is a wrapper for the IHISI function IhisiAuthLock.

  @retval IhisiSuccess              Function completed successfully or PcdH2OIhisiAuthSupported is set to FALSE.
  @retval IhisiInterfaceClosed      IHISI interface already open.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiAuthLock (
  VOID
  )
{
  IHISI_STATUS_CODE           IhisiStatus;
  IHISI_LIB_SMI_DATA          SmiData;

  if (!PcdGetBool (PcdH2OIhisiAuthSupported)) {
    return IhisiSuccess;
  }
  if (mAuthLock) {
    return IhisiInterfaceClosed;
  }

  SmiData.Eax = (IHISI_SW_SMI | (((UINTN)IhisiAuthLock) << 8));
  SmiData.Ebx = mSessionToken;
  SmiData.Ecx = 0;
  SmiData.Edx = PcdGet16 (PcdSoftwareSmiPort);
  SmiData.Esi = 0;
  SmiData.Edi = 0;

  IhisiLibGenericSmi (&SmiData);
  IhisiStatus   = (UINT8)SmiData.Eax;
  mSessionToken = SmiData.Ebx;
  if (IhisiStatus == IhisiSuccess) {
    mAuthLock = TRUE;
  }
  return IhisiStatus;
}

/**
  This function returns whether the IHISI interface is already locked and, if not, returns the types of security levels
  supported by the interface. This function can be called when the interface is open or closed.

  @param  [out] SecurityLevelRequested  On return, a bitmask that indicates the security levels supported by IHISI on
                                        this platform. Zero or more may be set.
                                        IHISI_AUTH_NONE - Minimum security level possible.
                                        IHISI_AUTH_POST_ONLY - After Ready-To-Boot, this security level
                                                              will always fail. This lock should not persist
                                                              past the Ready-To-Lock event.
                                        IHISI_AUTH_DXE_ONLY - After End-of-DXE, this security level will always
                                                              fail. This lock should not persist past the End-Of-DXE
                                                              event.
                                        IHISI_AUTH_SECURE_ENVIRONMENT - When outside of a secure environment, sessions
                                                                        cannot be opened with this security level.
                                        IHISI_AUTH_PASSWORD - Without a valid password, this security level will always
                                                              fail. The password is specified by CredentialData.
  @param  [out] CurrentSecurityLevel    On return, a bitmask that indicates which of the security level that was used
                                        when IHISI was locked, or 0 if the IHISI session is currently closed. Only a
                                        single bit may be set. The bit values are the same as those used in
                                        SecurityLevelsSupported.

  @retval IhisiSuccess                  Function completed successfully.
  @retval IhisiInvalidParamFatalError   CurrentSecurityLevel or SecurityLevelsSupported is NULL.
  @retval IhisiFunNotSupported          IHISI authentication is not supported.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiAuthStatus (
  OUT UINT32 *SecurityLevelsSupported,
  OUT UINT32 *CurrentSecurityLevel
  )
{
  IHISI_STATUS_CODE         IhisiStatus;
  IHISI_LIB_SMI_DATA        SmiData;

  if (!mAuthLock) {
    mSessionToken = SIGNATURE_32 ('O', '2', 'H', '$');
  }
  SmiData.Eax = (IHISI_SW_SMI | (((UINTN)IhisiAuthStatus) << 8));
  SmiData.Ebx = mSessionToken;
  SmiData.Ecx = 0;
  SmiData.Edx = PcdGet16 (PcdSoftwareSmiPort);
  SmiData.Esi = 0;
  SmiData.Edi = 0;

  IhisiLibGenericSmi (&SmiData);
  IhisiStatus              = (UINT8)SmiData.Eax;
  mSessionToken            = SmiData.Ebx;
  *SecurityLevelsSupported = SmiData.Ecx;
  *CurrentSecurityLevel    = SmiData.Esi;
  return IhisiStatus;
}

/**
  This function unlocks the IHISI interface. This function can be called when the interface is open or closed.
  This function is a wrapper for the IHISI function IhisiAuthUnlock .

  @retval IhisiSuccess              Function completed successfully or PcdH2OIhisiAuthSupported is set to FALSE.
  @retval IhisiInterfaceClosed      IHISI session is closed.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiAuthUnlock(
  VOID
  )
{
  IHISI_STATUS_CODE         IhisiStatus;
  IHISI_LIB_SMI_DATA        SmiData;

  if (!PcdGetBool (PcdH2OIhisiAuthSupported)) {
    return IhisiSuccess;
  }
  if (!mAuthLock) {
    return IhisiInterfaceClosed;
  }

  SmiData.Eax = (IHISI_SW_SMI | (((UINTN)IhisiAuthUnlock) << 8));
  SmiData.Ebx = IHISI_EBX_SIGNATURE;
  SmiData.Ecx = 0;
  SmiData.Edx = PcdGet16 (PcdSoftwareSmiPort);
  SmiData.Esi = 0;
  SmiData.Edi = 0;

  IhisiLibGenericSmi (&SmiData);
  IhisiStatus   = (UINT8)SmiData.Eax;
  mSessionToken = SmiData.Ebx;
  if (IhisiStatus == IhisiSuccess) {
    mAuthLock = FALSE;
  }
  return IhisiStatus;
}


/**
  This function returns whether the IHISI interface is already locked and, if not, returns the types of security levels
  supported by the interface. This function can be called when the interface is open or closed.

  @param  [out] CommandBuffer       On return, pointer to the first byte of the Command Buffer.
  @param  [out] CommandBufferSize   On return, pointer to the unsigned integer that holds the size of the Command
                                    Buffer, in bytes.
  @param  [out] ImageBuffer         On return, optional pointer to the first byte of the Image Buffer, if supported. If
                                    not supported, then this must set to 0. It is illegal for this to be non-zero if the
                                    Image Buffer Size is 0.
  @param  [out] ImageBufferSize     On return, optional pointer to the unsigned integer that specifies the size of the
                                    Image Buffer, in bytes, if supported. If not supported, then this must be set to 0.

  @retval IhisiSuccess                  Function completed successfully.
  @retval IhisiFunNotSupported          IHISI command buffer not supported.
  @retval IhisiInterfaceClosed          The IHISI interface is closed.
  @retval IhisiInvalidParamFatalError   CommandBuffer, CommandBufferSize is NULL.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiGetCmdBuffer (
  OUT  VOID                   **CommandBuffer,
  OUT  UINT32                 *CommandBufferSize,
  OUT  VOID                   **ImageBuffer,         OPTIONAL
  OUT  UINT32                 *ImageBufferSize      OPTIONAL
  )
{
  IHISI_STATUS_CODE         IhisiStatus;
  IHISI_LIB_SMI_DATA        SmiData;

  if (CommandBuffer == NULL || CommandBufferSize == NULL) {
    return IhisiInvalidParamFatalError;
  }
  if (!mAuthLock && PcdGetBool (PcdH2OIhisiAuthSupported)) {
    return IhisiInterfaceClosed;
  }
  if (!PcdGetBool (PcdH2OIhisiAuthSupported)) {
    mSessionToken = SIGNATURE_32 ('O', '2', 'H', '$');
  }
  SmiData.Eax = (IHISI_SW_SMI | (((UINTN)IhisiGetCmdBuffer) << 8));
  SmiData.Ebx = mSessionToken;
  SmiData.Ecx = 0;
  SmiData.Edx = PcdGet16 (PcdSoftwareSmiPort);
  SmiData.Esi = 0;
  SmiData.Edi = 0;

  IhisiLibGenericSmi (&SmiData);
  IhisiStatus   = (UINT8)SmiData.Eax;
  mSessionToken = SmiData.Ebx;
  if (IhisiStatus == IhisiSuccess) {
    *CommandBuffer     = (VOID *)(UINTN)SmiData.Ecx;
    *CommandBufferSize = SmiData.Edx;
    if (ImageBuffer != NULL) {
      *ImageBuffer = (VOID *)(UINTN)SmiData.Esi;
    }
    if (ImageBufferSize != NULL) {
      *ImageBufferSize = SmiData.Edi;
    }
  }
  return IhisiStatus;
}

/**
  This function returns platform information for the specified tool. This information is returned in CPU registers and
  in a memory buffer.

  @param  [in]  Version                 Pointer to the four ASCII characters representing the XXX version.
  @param  [in]  ATpVersion              Pointer to the desired IHISI version.
  @param  [out] Permission              On return, points to the returned flags that indicate whether the specified
                                        version is supported or not. This is related with the AH register returned from
                                        IHISI function 0x10. Valid values are 0 (permission denied) and 1 (permission
                                        allowed).
  @param  [out] FBTSVersion             Unsigned 16-bit integer that indicates the version of the FBTS interface. The
                                        major version is in bits 15:8 and the minor version is in bits 7:0. This is
                                        related to the CL and CH register returned from IHISI function 0x10.
  @param  [out] PlatformStatus          Points to the returned platform status in the FBTS_PLATFORM_STATUS_BUFFER
                                        structure.

  @retval IhisiSuccess                  This function completed successfully. Permission, FBTSVersion, PlatformStatus
                                        is updated.
  @retval IhisiInvalidParamFatalError   Version, ATpVersion, FBTSVersion, Permission or PlatformStatus is NULL.
                                        The IHISI function was not invoked.
  @retval IhisiAccessProhibited         Incorrect session token or the security level was not sufficient to perform this
                                        action. The IHISI session is closed.
  @retval IhisiInterfaceClosed          IHISI session is closed.
  @retval IhisiFunNotSupported          IHISI command buffer not supported.
  @retval IhisiBufferRangeError         The FBTS tools version buffer Structure lies outside the Command Buffer.
  @retval IhisiObLenTooSmall            Buffer is too small for output.
  @retval FbtsPermissionDeny            Unable to access flash block services.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiFbtsGetSupportVersion (
  IN  CONST UINT8                 *Version,
  IN  CONST UINT8                 *ATpVersion,
  OUT UINT8                       *Permission,
  OUT UINT16                      *FBTSVersion,
  OUT FBTS_PLATFORM_STATUS_BUFFER *PlatformStatus
  )
{
  IHISI_STATUS_CODE            IhisiStatus;
  IHISI_LIB_SMI_DATA           SmiData;
  VOID                        *CmdBuffer;
  UINT32                       CommandBufferSize;
  FBTS_TOOLS_VERSION_BUFFER    *VersionBuffer;


  if (Version == NULL || ATpVersion == NULL || Permission == NULL ||
      FBTSVersion == NULL || PlatformStatus == NULL) {
    return IhisiInvalidParamFatalError;
  }

  if (PcdGetBool (PcdH2OIhisiCmdBufferSupported)) {
    IhisiStatus = H2OIhisiGetCmdBuffer (&CmdBuffer, &CommandBufferSize, NULL, NULL);
    if (IhisiStatus != IhisiSuccess) {
      return IhisiStatus;
    }
  } else {
    CmdBuffer = mInternalBuffer;
  }
  VersionBuffer = (FBTS_TOOLS_VERSION_BUFFER *)CmdBuffer;

  VersionBuffer->Signature = FBTS_VERSION_SIGNATURE;
  CopyMem (VersionBuffer->Version, Version, sizeof (VersionBuffer->Version));
  CopyMem (VersionBuffer->AtpSignature, mAtpSignature, sizeof (mAtpSignature));
  CopyMem (VersionBuffer->ATpVersion, ATpVersion, sizeof (VersionBuffer->ATpVersion));

  SmiData.Eax = (IHISI_SW_SMI | (((UINTN)FBTSGetSupportVersion) << 8));
  SmiData.Ebx = mSessionToken;
  SmiData.Ecx = 0;
  SmiData.Edx = PcdGet16 (PcdSoftwareSmiPort);
  SmiData.Esi = 0;
  SmiData.Edi = (UINT32)(UINTN)CmdBuffer;
  IhisiLibGenericSmi (&SmiData);

  IhisiStatus   = (UINT8)SmiData.Eax;
  mSessionToken = SmiData.Ebx;
  if (IhisiStatus == IhisiSuccess) {
    *Permission = ((UINTN)SmiData.Eax >> 0x8) & 0xFF;
    *FBTSVersion = (UINT16)SmiData.Ecx;
    CopyMem (PlatformStatus, CmdBuffer, sizeof (FBTS_PLATFORM_STATUS_BUFFER));
  }
  return IhisiStatus;
}

/**
  This function returns information about the primary flash device. This function must be called after setting a
  security level other than IHISI_AUTH_NONE using H2OIhisiAuth().
  If the function completes successfully, then FlashDeviceInfo and FlashDeviceBlocks are updated with pointers to the
  flash device information.

  NOTE:	The flash device information is located in the shared Command Buffer. This information must be copied or used
  before the next IHISI command.

  @param  [out] FlashDeviceInfo     On return, pointer to the first byte of the Command Buffer.
  @param  [out] FlashDeviceBlocks   On return, pointer to the unsigned integer that holds the size of the Command
                                    Buffer, in bytes.

  @retval IhisiSuccess                  This function completed successfully. FlashDeviceInfo and FlashDeviceBlocks are
                                        updated.
  @retval IhisiInvalidParamFatalError   FlashDeviceInfo or FlashDeviceBlocks is NULL.
  @retval IhisiAccessProhibited         Incorrect session token or the security level was not sufficient to perform this
                                        action. The IHISI session is closed.
  @retval IhisiInterfaceClosed          IHISI session is closed.
  @retval IhisiFunNotSupported          IHISI command buffer not supported.
  @retval IhisiBufferRangeError         The Flash Part Information or Flash Part Block Structure lies outside the
                                        Command Buffer.
  @retval IhisiObLenTooSmall            Buffer is too small for output.
  @retval FbtsPermissionDeny            Unable to access flash block services.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiFbtsGetFlashPartInfo (
  OUT  FBTS_FLASH_DEVICE      **FlashDeviceInfo,
  OUT  FD_BLOCK_MAP           **FlashDeviceBlocks
  )
{
  VOID                      *CmdBuffer;
  UINT32                    CommandBufferSize;
  IHISI_STATUS_CODE         IhisiStatus;
  IHISI_LIB_SMI_DATA        SmiData;

  if (FlashDeviceInfo == NULL || FlashDeviceBlocks == NULL) {
    return IhisiInvalidParamFatalError;
  }
  if (!mAuthLock && PcdGetBool (PcdH2OIhisiAuthSupported)) {
    return IhisiInterfaceClosed;
  }

  if (PcdGetBool (PcdH2OIhisiCmdBufferSupported)) {
    IhisiStatus = H2OIhisiGetCmdBuffer (&CmdBuffer, &CommandBufferSize, NULL, NULL);
    if (IhisiStatus != IhisiSuccess) {
      return IhisiStatus;
    }
  } else {
    CmdBuffer = mInternalBuffer;
  }

  if (!PcdGetBool (PcdH2OIhisiAuthSupported)) {
    mSessionToken = SIGNATURE_32 ('O', '2', 'H', '$');
  }

  SmiData.Eax = (IHISI_SW_SMI | (((UINTN)FBTSGetPlatformInfo) << 8));
  SmiData.Ebx = mSessionToken;
  SmiData.Ecx = 0;
  SmiData.Edx = PcdGet16 (PcdSoftwareSmiPort);
  SmiData.Esi = (UINT32)(UINTN)CmdBuffer + 0x100;
  SmiData.Edi = (UINT32)(UINTN)CmdBuffer;

  IhisiLibGenericSmi (&SmiData);
  IhisiStatus   = (UINT8)SmiData.Eax;
  mSessionToken = SmiData.Ebx;
  if (IhisiStatus == IhisiSuccess) {

    *FlashDeviceInfo   = (FBTS_FLASH_DEVICE *)(UINTN)SmiData.Edi;
    *FlashDeviceBlocks = (FD_BLOCK_MAP *)(UINTN)SmiData.Esi;
  }
  if (IhisiStatus == IhisiBufferRangeError) {
    mAuthLock = FALSE;
  }

  return IhisiStatus;
}

/**
  This function writes the specified number of bytes of data to the flash device at the specified linear address from
  DataToWrite. This function must be called after setting a security level other than IHISI_AUTH_NONE using
  H2OIhisiAuth()
  This function is a wrapper for FBTSWrite. This function copies SizeToWrite bytes from DataToWrite into the Command
  Buffer. If the number of bytes exceeds the size of the Command Buffer, then the function will return
  IhisiBufferRangeError.

  @param  [in] SizeToWrite      Unsigned integer that specifies the number of bytes to write to the flash device, in
                                bytes.
  @param  [in] AddresstoWrite   Unsigned integer that specifies the linear address of the within the flash device to
                                write. This address must lie within the flash device. This address must be at least
                                SizeToWrite bytes below the end of memory.
  @param  [in] DataToWrite      Pointer to the buffer that holds the data to write to the flash device data.

  @retval IhisiSuccess                 This function completed successfully.
  @retval IhisiInvalidParamFatalError  DataToWrite is NULL or SizeToWrite is zero or The AddressToWrite lies
                                       outside the flash device.
  @retval IhisiAccessProhibited        Incorrect session token or the security level was not sufficient to perform
                                       this action. The IHISI session is closed and the session token is reset to
                                       the default value of $H2O.
  @retval IhisiFunNotSupported         Unable to access flash block services.
  @retval IhisiInterfaceClosed         IHISI session is closed.
  @retval FbtsSkipThisWriteBlock       Block was write-protected.
  @retval FbtsWriteFail                Write failure outside of POST or Hardware write failure during flash block write.
  @retval FbtsEraseFail                Hardware erase failure during flash block erase.
  @retval IhisiBufferRangeError        The DataToWrite would extend outside the Command Buffer.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiFbtsWrite (
  IN   UINT32     SizeToWrite,
  IN   UINT32     AddresstoWrite,
  IN   UINT8      *DataToWrite
  )
{
  IHISI_STATUS_CODE         IhisiStatus;
  IHISI_LIB_SMI_DATA        SmiData;

  if (!mAuthLock && PcdGetBool (PcdH2OIhisiAuthSupported)) {
    return IhisiInterfaceClosed;
  }
  if (!PcdGetBool (PcdH2OIhisiAuthSupported)) {
    mSessionToken = SIGNATURE_32 ('O', '2', 'H', '$');
  }

  SmiData.Eax = (IHISI_SW_SMI | (((UINTN)FBTSWrite) << 8));
  SmiData.Ebx = mSessionToken;
  SmiData.Ecx = SizeToWrite;
  SmiData.Edx = PcdGet16 (PcdSoftwareSmiPort);
  SmiData.Esi = (UINT32)(UINTN)DataToWrite;
  SmiData.Edi = AddresstoWrite;

  IhisiLibGenericSmi (&SmiData);
  IhisiStatus   = (UINT8)SmiData.Eax;
  mSessionToken = SmiData.Ebx;
  if (IhisiStatus == IhisiBufferRangeError) {
    mAuthLock = FALSE;
  }
  return IhisiStatus;
}

/**
  This function is called by the flash application at the end of the flashing process with information about the status
  of the overall process. This function must be called after setting a security level other than IHISI_AUTH_NONE using
  H2OIhisiAuth().

  This function is a wrapper for the IHISI function FBTSComplete. This function creates the structures in the Command
  Buffer.


  @param  [in] FlashCompleteActionRequest  Enumerated value that specifies the flash application's requested action.
                                           Valid values are:
                                             FlashCompleteDoNothing - Do nothing.
                                             FlashCompleteShutdown - Shut down the system.
                                             FlashCompleteReboot - Reboot
                                             FlashCompleteS3 - Enter S3.
  @param  [in] FlashCosmpleteStatuss       Enumerated value that specifies the type of flash that the application is
                                           performing. Valid values are:
                                             ApTerminated - Application has terminated.
                                             NormalFlash - Complete flash device update has completed normally.
                                             PartialFlash - Partial flash device update has completed normally.

  @retval IhisiSuccess                 This function completed successfully.
  @retval IhisiInvalidParamFatalError  DataToWrite is NULL or SizeToWrite is zero or The AddressToWrite lies
                                       outside the flash device.
  @retval IhisiAccessProhibited        Incorrect session token or the security level was not sufficient to perform
                                       this action.The IHISI session is closed and the session token is reset to
                                       the default value of $H2O.
  @retval IhisiFunNotSupported         Unable to access flash block services.
  @retval IhisiInterfaceClosed         IHISI session is closed.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiFbtsFlashComplete (
  IN   FLASH_COMPLETE_APREQUEST    FlashCompleteActionRequest,
  IN   FLASH_COMPLETE_STATUS       FlashCosmpleteStatuss
  )
{
  IHISI_STATUS_CODE             IhisiStatus;
  VOID                          *CmdBuffer;
  UINT32                        CommandBufferSize;
  FBTS_FLASH_COMPLETE_STATUS    *FbtsFlashCompleteBuffer;
  IHISI_LIB_SMI_DATA            SmiData;

  if (!mAuthLock && PcdGetBool (PcdH2OIhisiAuthSupported)) {
    return IhisiInterfaceClosed;
  }

  if (PcdGetBool (PcdH2OIhisiCmdBufferSupported)) {
    IhisiStatus = H2OIhisiGetCmdBuffer (&CmdBuffer, &CommandBufferSize, NULL, NULL);
    if (IhisiStatus != IhisiSuccess) {
      return IhisiStatus;
    }
  } else {
    CmdBuffer = mInternalBuffer;
  }

  if (!PcdGetBool (PcdH2OIhisiAuthSupported)) {
    mSessionToken = SIGNATURE_32 ('O', '2', 'H', '$');
  }
  FbtsFlashCompleteBuffer = (FBTS_FLASH_COMPLETE_STATUS *) ((UINTN) CmdBuffer);
  FbtsFlashCompleteBuffer->Signature     = FLASH_COMPLETE_STATUS_SIGNATURE;
  FbtsFlashCompleteBuffer->StructureSize = sizeof(FBTS_FLASH_COMPLETE_STATUS);
  FbtsFlashCompleteBuffer->CompleteStatus = FlashCosmpleteStatuss;

  SmiData.Eax = (IHISI_SW_SMI | (((UINTN)FBTSComplete) << 8));
  SmiData.Ebx = mSessionToken;
  SmiData.Ecx = FlashCompleteActionRequest;
  SmiData.Edx = PcdGet16 (PcdSoftwareSmiPort);
  SmiData.Esi = (UINT32)(UINTN)FbtsFlashCompleteBuffer;
  SmiData.Edi = 0;

  IhisiLibGenericSmi (&SmiData);
  IhisiStatus   = (UINT8)SmiData.Eax;
  mSessionToken = SmiData.Ebx;
  if (IhisiStatus == IhisiBufferRangeError) {
    mAuthLock = FALSE;
  }
  return IhisiSuccess;
}

/**
  This function initiates the secure flash from the specified image across reset.
  This function is a wrapper for the IHISI function FBTSOEMCapsuleSecureFlash. This function creates the structures in
  the Command Buffer and copies the image to the Image Buffer. The type of reset is returned in FlashToolAction.

  @param  [in] CapsuleSize        Unsigned integer that specifies the size of the capsule image, in bytes.
  @param  [in] CapsuleData        Pointer to the capsule data.
  @param  [out] FlashToolAction   Pointer to enumerated value that tells the tool what action to take after this call.
                                  Valid values are below. The default is 1 (Reboot).
                                    WINFLASH_DO_S3 - Perform S3 suspend.
                                    WINFLASH_DO_REBOOT - Reboot
                                    WINFLASH_DO_SHUT_DOWN - Shutdown
                                    WINFLASH_DO_NOTHING - Nothing.

  @retval IhisiSuccess                  This function completed successfully.
  @retval IhisiInvalidParamFatalError   CapsuleSize is 0 or SECURE_FLASH_SIGNATURE or SECURE_FLASH_LEGACY_SIGN
  @retval IhisiAccessProhibited         Incorrect session token or the security level was not sufficient to perform
                                        this action.
  @retval IhisiFunNotSupported          Function not supported.
  @retval IhisiInterfaceClosed          IHISI session is closed.
  @retval IhisiBufferRangeError         Command Buffer or Image Buffer was too small or Image Buffer could not hold the
                                        entire image.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiFbtsOemCapsuleSecureFlashAcrossReset (
  IN   UINT32                 CapsuleSize,
  IN   VOID                   *CapsuleData,
  OUT  WINFLASH_ACTION_TYPE   *FlashToolAction
  )
{
  //
  // TODO: No any code need use this function so not implement this function for now.
  //
  return IhisiFunNotSupported;
}

/**
  Trigger SMI to call IHISI OEM Capsule Secure Flash function (0x48)

  @param[in]       Command        Input IHISI command for IHISI function 0x48.
  @param[in]       SmiPort        IO port number to trigger software SMI.
  @param[in, out]  ExtendFlag     On input, points to an enumerated value that describes the capsule capabilities being
  	                              queried.
  	                              On exit, points to the result of the query.

  @retval IhisiSuccess           Function completed successfully.
  @return Other                  Any error occurred in this function.
**/
STATIC
UINT8
IhisiLibCallFbtsOemCapsuleSecureFlash (
  IN     UINT32	                        Command,
  IN     UINT16                         SmiPort,
  IN OUT FBTS_EXTEND_FLAG_STRUCTURE     *ExtendFlag
  )
{
  IHISI_STATUS_CODE         IhisiStatus;
  IHISI_LIB_SMI_DATA        SmiData;

  SmiData.Eax = (IHISI_SW_SMI | (((UINTN)FBTSOEMCapsuleSecureFlash) << 8));
  SmiData.Ebx = mSessionToken;
  SmiData.Ecx = Command;
  SmiData.Edx = PcdGet16 (PcdSoftwareSmiPort);
  SmiData.Esi = 0;
  SmiData.Edi = (UINT32)(UINTN)ExtendFlag;

  IhisiLibGenericSmi (&SmiData);

  IhisiStatus   = (UINT8)SmiData.Eax;
  mSessionToken = SmiData.Ebx;

  return (UINT8)IhisiStatus;
}

/**
  This function initiates the secure flash from the specified image from the EFI System Partition.
  This function is a wrapper for the IHISI function FBTSOEMCapsuleSecureFlash. This function creates the structures in
  the Command Buffer. The type of reset is returned in FlashToolAction.

  @param  [in]  CapsuleSize        Unsigned integer that specifies the size of the capsule image, in bytes.
  @param  [out] FlashToolAction    Pointer to enumerated value that tells the tool what action to take after this call.
                                   Valid values are below. The default is 1 (Reboot).
                                    WINFLASH_DO_S3 - Perform S3 suspend.
                                    WINFLASH_DO_REBOOT - Reboot
                                    WINFLASH_DO_SHUT_DOWN - Shutdown
                                    WINFLASH_DO_NOTHING - Nothing.

  @retval IhisiSuccess                  This function completed successfully.
  @retval IhisiInvalidParamFatalError   CapsuleSize is 0 or SECURE_FLASH_SIGNATURE or SECURE_FLASH_LEGACY_SIGN.
  @retval IhisiAccessProhibited         Incorrect session token or the security level was not sufficient to perform this
                                        action.
  @retval IhisiFunNotSupported          Function not supported.
  @retval IhisiInterfaceClosed          IHISI session is closed.
  @retval IhisiBufferRangeError         Command Buffer was too small.
  @retval IhisiOutOfResources           Image Buffer could not hold the entire image.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiFbtsOemCapsuleSecureFlashFromEsp (
  IN    UINT32                  CapsuleSize,
  OUT   WINFLASH_ACTION_TYPE    *FlashToolAction
  )
{
  //
  // TODO: No any code need use this function so not implement this function for now.
  //
  return IhisiFunNotSupported;
}

/**
  This function disables secure flash from the EFI System Partition. This will cause
  H2OIhisiFbtsOemCapsuleSecureFlashFromEsp to be disabled. There is no way to re-enable ESP support without restarting
  the system.
  This function is a wrapper for the IHISI function FBTSOEMCapsuleSecureFlash.

  @retval IhisiSuccess                  This function completed successfully.
  @retval IhisiAccessProhibited         Incorrect session token or the security level was not sufficient to perform this
                                        action.
  @retval IhisiFunNotSupported          Function not supported.
  @retval IhisiInterfaceClosed          IHISI session is closed.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiFbtsOemCapsuleSecureFlashDisableEsp  (
  VOID
  )
{
  if (!mAuthLock && PcdGetBool (PcdH2OIhisiAuthSupported)) {
    return IhisiInterfaceClosed;
  }
  if (!PcdGetBool (PcdH2OIhisiAuthSupported)) {
    mSessionToken = SIGNATURE_32 ('O', '2', 'H', '$');
  }
  return IhisiLibCallFbtsOemCapsuleSecureFlash (
           SECURE_FLASH_LEGACY_SIGN,
           PcdGet16 (PcdSoftwareSmiPort),
           NULL
           );
}

/**
  This function marks the system as no longer being "in POST". There is no way to mark the system as being "in POST"
  without restarting the system.
  This function is a wrapper for the IHISI function FBTSOEMCapsuleSecureFlash.

  @retval IhisiSuccess                  This function completed successfully.
  @retval IhisiAccessProhibited         Incorrect session token or the security level was not sufficient to perform this
                                        action.
  @retval IhisiFunNotSupported          Function not supported.
  @retval IhisiInterfaceClosed          IHISI session is closed.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiFbtsOemCapsuleSecureFlashResetPostFlag  (
  VOID
  )
{
  if (!mAuthLock && PcdGetBool (PcdH2OIhisiAuthSupported)) {
    return IhisiInterfaceClosed;
  }
  if (!PcdGetBool (PcdH2OIhisiAuthSupported)) {
    mSessionToken = SIGNATURE_32 ('O', '2', 'H', '$');
  }
  return IhisiLibCallFbtsOemCapsuleSecureFlash (
           SECURE_FLASH_SIGNATURE,
           PcdGet16 (PcdSoftwareSmiPort),
           NULL
           );

}

/**
  This function marks the system as no longer being "in POST". There is no way to mark the system as being "in POST"
  without restarting the system.
  This function is a wrapper for the IHISI function FBTSOEMCapsuleSecureFlash.

  @param  [in]  InFlag          On entry, points to an enumerated value that describes the capsule capabilities being
  	                            queried. 1 = Support ESP
  @param  [out] OutFlag         On exit, points to the result of the query: 1 = ESP supported.

  @retval IhisiSuccess                  This function completed successfully.
  @retval IhisiInvalidParamFatalError   InFlag or OutFlag is NULL.
  @retval IhisiAccessProhibited         Incorrect session token or the security level was not sufficient to perform this
                                        action.
  @retval IhisiFunNotSupported          Function not supported.
  @retval IhisiInterfaceClosed          IHISI session is closed.
**/
IHISI_STATUS_CODE
EFIAPI
H2OIhisiFbtsOemCapsuleSecureFlashQueryCapabilities (
  IN    FBTS_EXTEND_FLAG   *InFlag,
  OUT   FBTS_EXTEND_FLAG   *OutFlag
  )
{
  IHISI_STATUS_CODE                   IhisiStatus;
  FBTS_EXTEND_FLAG_STRUCTURE          *ExtendFlag;
  VOID                                *CmdBuffer;
  UINT32                              CommandBufferSize;

  if (!mAuthLock && PcdGetBool (PcdH2OIhisiAuthSupported)) {
    return IhisiInterfaceClosed;
  }

  if (PcdGetBool (PcdH2OIhisiCmdBufferSupported)) {
    IhisiStatus = H2OIhisiGetCmdBuffer (&CmdBuffer, &CommandBufferSize, NULL, NULL);
    if (IhisiStatus != IhisiSuccess) {
      return IhisiStatus;
    }
  } else {
    CmdBuffer =  mInternalBuffer;
  }

  if (!PcdGetBool (PcdH2OIhisiAuthSupported)) {
    mSessionToken = SIGNATURE_32 ('O', '2', 'H', '$');
  }
  ExtendFlag = (FBTS_EXTEND_FLAG_STRUCTURE *)CmdBuffer;

  ExtendFlag->Signature     = EXTEND_FLAG_INPUT_SIGNATURE;
  ExtendFlag->StructureSize = sizeof (FBTS_EXTEND_FLAG_STRUCTURE);
  CopyMem (&ExtendFlag->Flag, InFlag, sizeof (FBTS_EXTEND_FLAG));
  IhisiStatus = IhisiLibCallFbtsOemCapsuleSecureFlash (
                  0,
                  PcdGet16 (PcdSoftwareSmiPort),
                  ExtendFlag
                  );
  if (IhisiStatus == IhisiSuccess) {
    CopyMem (OutFlag, &ExtendFlag->Flag, sizeof (FBTS_EXTEND_FLAG));
  }

  if (IhisiStatus == IhisiBufferRangeError) {
    mAuthLock = FALSE;
  }

  return IhisiStatus;
}


/**
  The constructor function allocate internal buffer if PcdH2OIhisiCmdBufferSupported
  is FALSE. If PcdH2OIhisiCmdBufferSupported is TRUE, return EFI_SUCCESS directly.

    @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   This function completes  successfully.
  @return Others        Any error occurred in this function
**/
EFI_STATUS
EFIAPI
H2OIhisiLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_HANDLE                     Handle;

  if (PcdGetBool (PcdH2OIhisiCmdBufferSupported)) {
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (
                    &mInternalH2OIhisiLibGuid,
                    NULL,
                    &mInternalBuffer
                    );
  if (EFI_ERROR (Status)) {
    mInternalBuffer = AllocatePool (H2O_IHISI_LIB_BUFFER_SIZE);
    ASSERT (mInternalBuffer != NULL);
    if (mInternalBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &mInternalH2OIhisiLibGuid,
                    EFI_NATIVE_INTERFACE,
                    mInternalBuffer
                    );
  }
  return Status;
}