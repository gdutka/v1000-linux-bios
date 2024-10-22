/** @file
   Header file for SmmLockBoxSmmDxeLib
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Uefi.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/LockBoxLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>

#include <Protocol/SmmCommunication.h>

#include <Guid/SmmLockBox.h>

/**
  This function will save confidential information to lockbox.

  @param Guid       the guid to identify the confidential information
  @param Buffer     the address of the confidential information
  @param Length     the length of the confidential information

  @retval RETURN_SUCCESS            the information is saved successfully.
  @retval RETURN_INVALID_PARAMETER  the Guid is NULL, or Buffer is NULL, or Length is 0
  @retval RETURN_ALREADY_STARTED    the requested GUID already exist.
  @retval RETURN_OUT_OF_RESOURCES   no enough resource to save the information.
  @retval RETURN_ACCESS_DENIED      it is too late to invoke this interface
  @retval RETURN_NOT_STARTED        it is too early to invoke this interface
  @retval RETURN_UNSUPPORTED        the service is not supported by implementaion.
**/
RETURN_STATUS
EFIAPI
DxeSaveLockBox (
  IN  GUID                        *Guid,
  IN  VOID                        *Buffer,
  IN  UINTN                       Length
  );

/**
  This function will set lockbox attributes.

  @param Guid       the guid to identify the confidential information
  @param Attributes the attributes of the lockbox

  @retval RETURN_SUCCESS            the information is saved successfully.
  @retval RETURN_INVALID_PARAMETER  attributes is invalid.
  @retval RETURN_NOT_FOUND          the requested GUID not found.
  @retval RETURN_ACCESS_DENIED      it is too late to invoke this interface
  @retval RETURN_NOT_STARTED        it is too early to invoke this interface
  @retval RETURN_UNSUPPORTED        the service is not supported by implementaion.
**/
RETURN_STATUS
EFIAPI
DxeSetLockBoxAttributes (
  IN  GUID                        *Guid,
  IN  UINT64                      Attributes
  );

/**
  This function will update confidential information to lockbox.

  @param Guid   the guid to identify the original confidential information
  @param Offset the offset of the original confidential information
  @param Buffer the address of the updated confidential information
  @param Length the length of the updated confidential information

  @retval RETURN_SUCCESS            the information is saved successfully.
  @retval RETURN_INVALID_PARAMETER  the Guid is NULL, or Buffer is NULL, or Length is 0.
  @retval RETURN_NOT_FOUND          the requested GUID not found.
  @retval RETURN_BUFFER_TOO_SMALL   the original buffer to too small to hold new information.
  @retval RETURN_ACCESS_DENIED      it is too late to invoke this interface
  @retval RETURN_NOT_STARTED        it is too early to invoke this interface
  @retval RETURN_UNSUPPORTED        the service is not supported by implementaion.
**/
RETURN_STATUS
EFIAPI
DxeUpdateLockBox (
  IN  GUID                        *Guid,
  IN  UINTN                       Offset,
  IN  VOID                        *Buffer,
  IN  UINTN                       Length
  );

/**
  This function will restore confidential information from lockbox.

  @param Guid   the guid to identify the confidential information
  @param Buffer the address of the restored confidential information
                NULL means restored to original address, Length MUST be NULL at same time.
  @param Length the length of the restored confidential information

  @retval RETURN_SUCCESS            the information is restored successfully.
  @retval RETURN_INVALID_PARAMETER  the Guid is NULL, or one of Buffer and Length is NULL.
  @retval RETURN_WRITE_PROTECTED    Buffer and Length are NULL, but the LockBox has no
                                    LOCK_BOX_ATTRIBUTE_RESTORE_IN_PLACE attribute.
  @retval RETURN_BUFFER_TOO_SMALL   the Length is too small to hold the confidential information.
  @retval RETURN_NOT_FOUND          the requested GUID not found.
  @retval RETURN_NOT_STARTED        it is too early to invoke this interface
  @retval RETURN_ACCESS_DENIED      not allow to restore to the address
  @retval RETURN_UNSUPPORTED        the service is not supported by implementaion.
**/
RETURN_STATUS
EFIAPI
DxeRestoreLockBox (
  IN  GUID                        *Guid,
  IN  VOID                        *Buffer, OPTIONAL
  IN  OUT UINTN                   *Length  OPTIONAL
  );

/**
  This function will restore confidential information from all lockbox which have RestoreInPlace attribute.

  @retval RETURN_SUCCESS            the information is restored successfully.
  @retval RETURN_NOT_STARTED        it is too early to invoke this interface
  @retval RETURN_UNSUPPORTED        the service is not supported by implementaion.
**/
RETURN_STATUS
EFIAPI
DxeRestoreAllLockBoxInPlace (
  VOID
  );

/**
  This function will save confidential information to lockbox.

  @param Guid       the guid to identify the confidential information
  @param Buffer     the address of the confidential information
  @param Length     the length of the confidential information

  @retval RETURN_SUCCESS            the information is saved successfully.
  @retval RETURN_INVALID_PARAMETER  the Guid is NULL, or Buffer is NULL, or Length is 0
  @retval RETURN_ALREADY_STARTED    the requested GUID already exist.
  @retval RETURN_OUT_OF_RESOURCES   no enough resource to save the information.
  @retval RETURN_ACCESS_DENIED      it is too late to invoke this interface
  @retval RETURN_NOT_STARTED        it is too early to invoke this interface
  @retval RETURN_UNSUPPORTED        the service is not supported by implementaion.
**/
RETURN_STATUS
EFIAPI
SmmSaveLockBox (
  IN  GUID                        *Guid,
  IN  VOID                        *Buffer,
  IN  UINTN                       Length
  );

/**
  This function will set lockbox attributes.

  @param Guid       the guid to identify the confidential information
  @param Attributes the attributes of the lockbox

  @retval RETURN_SUCCESS            the information is saved successfully.
  @retval RETURN_INVALID_PARAMETER  attributes is invalid.
  @retval RETURN_NOT_FOUND          the requested GUID not found.
  @retval RETURN_ACCESS_DENIED      it is too late to invoke this interface
  @retval RETURN_NOT_STARTED        it is too early to invoke this interface
  @retval RETURN_UNSUPPORTED        the service is not supported by implementaion.
**/
RETURN_STATUS
EFIAPI
SmmSetLockBoxAttributes (
  IN  GUID                        *Guid,
  IN  UINT64                      Attributes
  );

/**
  This function will update confidential information to lockbox.

  @param Guid   the guid to identify the original confidential information
  @param Offset the offset of the original confidential information
  @param Buffer the address of the updated confidential information
  @param Length the length of the updated confidential information

  @retval RETURN_SUCCESS            the information is saved successfully.
  @retval RETURN_INVALID_PARAMETER  the Guid is NULL, or Buffer is NULL, or Length is 0.
  @retval RETURN_NOT_FOUND          the requested GUID not found.
  @retval RETURN_BUFFER_TOO_SMALL   the original buffer to too small to hold new information.
  @retval RETURN_ACCESS_DENIED      it is too late to invoke this interface
  @retval RETURN_NOT_STARTED        it is too early to invoke this interface
  @retval RETURN_UNSUPPORTED        the service is not supported by implementaion.
**/
RETURN_STATUS
EFIAPI
SmmUpdateLockBox (
  IN  GUID                        *Guid,
  IN  UINTN                       Offset,
  IN  VOID                        *Buffer,
  IN  UINTN                       Length
  );

/**
  This function will restore confidential information from lockbox.

  @param Guid   the guid to identify the confidential information
  @param Buffer the address of the restored confidential information
                NULL means restored to original address, Length MUST be NULL at same time.
  @param Length the length of the restored confidential information

  @retval RETURN_SUCCESS            the information is restored successfully.
  @retval RETURN_INVALID_PARAMETER  the Guid is NULL, or one of Buffer and Length is NULL.
  @retval RETURN_WRITE_PROTECTED    Buffer and Length are NULL, but the LockBox has no
                                    LOCK_BOX_ATTRIBUTE_RESTORE_IN_PLACE attribute.
  @retval RETURN_BUFFER_TOO_SMALL   the Length is too small to hold the confidential information.
  @retval RETURN_NOT_FOUND          the requested GUID not found.
  @retval RETURN_NOT_STARTED        it is too early to invoke this interface
  @retval RETURN_ACCESS_DENIED      not allow to restore to the address
  @retval RETURN_UNSUPPORTED        the service is not supported by implementaion.
**/
RETURN_STATUS
EFIAPI
SmmRestoreLockBox (
  IN  GUID                        *Guid,
  IN  VOID                        *Buffer, OPTIONAL
  IN  OUT UINTN                   *Length  OPTIONAL
  );

/**
  This function will restore confidential information from all lockbox which have RestoreInPlace attribute.

  @retval RETURN_SUCCESS            the information is restored successfully.
  @retval RETURN_NOT_STARTED        it is too early to invoke this interface
  @retval RETURN_UNSUPPORTED        the service is not supported by implementaion.
**/
RETURN_STATUS
EFIAPI
SmmRestoreAllLockBoxInPlace (
  VOID
  );


/**
  Constructor for SmmLockBox library.
  This is used to set SmmLockBox context, which will be used in PEI phase in S3 boot path later.

  @param[in] ImageHandle  Image handle of this driver.
  @param[in] SystemTable  A Pointer to the EFI System Table.

  @retval EFI_SUCEESS
  @return Others          Some error occurs.
**/
EFI_STATUS
EFIAPI
SmmLockBoxSmmConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );


/**
  Destructor for SmmLockBox library.
  This is used to uninstall SmmLockBoxCommunication configuration table
  if it has been installed in Constructor.

  @param[in] ImageHandle    Image handle of this driver.
  @param[in] SystemTable    A Pointer to the EFI System Table.

  @retval EFI_SUCEESS       The destructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
SmmLockBoxSmmDestructor (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );