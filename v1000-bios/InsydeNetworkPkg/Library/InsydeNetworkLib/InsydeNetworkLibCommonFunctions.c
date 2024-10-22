/** @file

;******************************************************************************
;* Copyright (c) 2012-2019, Insyde Software Corporation. All Rights Reserved.
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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/InsydeNetworkLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/NetLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

#include <Protocol/ServiceBinding.h>
#include <Protocol/DevicePath.h>
#include <Protocol/Hash.h>
#include <Protocol/Hash2.h>

EFI_HANDLE                           mHashHandle  = NULL;
EFI_HANDLE                           mHash2Handle = NULL;

EFI_STATUS
EFIAPI
CreateServiceChild (
  IN  EFI_GUID              *ServiceBindingGuid,
  IN  OUT EFI_HANDLE        *ChildHandle
  )
{
  EFI_STATUS                    Status;
  EFI_SERVICE_BINDING_PROTOCOL  *ServiceBinding;

  //
  // Get the ServiceBinding Protocol
  //
  Status = gBS->LocateProtocol (ServiceBindingGuid, NULL, (VOID **) &ServiceBinding);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Create a child
  //
  Status = ServiceBinding->CreateChild (ServiceBinding, ChildHandle);
  return Status;
}

EFI_STATUS
EFIAPI
DestroyServiceChild (
  IN  EFI_GUID              *ServiceBindingGuid,
  IN  OUT EFI_HANDLE        ChildHandle
  )
{
  EFI_STATUS                    Status;
  EFI_SERVICE_BINDING_PROTOCOL  *ServiceBinding;

  Status = gBS->LocateProtocol (ServiceBindingGuid, NULL, (VOID **) &ServiceBinding);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return ServiceBinding->DestroyChild (ServiceBinding,  ChildHandle);
}


EFI_STATUS
InitializeHashProtocol (
  )
{
  EFI_STATUS                           Status;

  if ((mHash2Handle != NULL) || (mHashHandle != NULL)) {
    return EFI_SUCCESS;
  }

  Status = EFI_NOT_READY;
  if (mHash2Handle == NULL) {
    Status = CreateServiceChild (&gEfiHash2ServiceBindingProtocolGuid, &mHash2Handle);
    if (!EFI_ERROR (Status)) {
      return EFI_SUCCESS;;
    }

  }

  if (mHashHandle == NULL) {
    Status = CreateServiceChild (&gEfiHashServiceBindingProtocolGuid, &mHashHandle);
    if (!EFI_ERROR (Status)) {
      return EFI_SUCCESS;;
    }

  }
  return Status;
}

VOID
UninitializeHashProtocol (
  )
{
  if ((mHash2Handle == NULL) && (mHashHandle == NULL)) {
    return;
  }

  if (mHash2Handle != NULL) {
    DestroyServiceChild (&gEfiHash2ServiceBindingProtocolGuid, mHash2Handle);
    mHash2Handle = NULL;

  }

  if (mHashHandle != NULL) {
    DestroyServiceChild (&gEfiHashServiceBindingProtocolGuid, mHashHandle);
    mHashHandle = NULL;

  }
}

#define VARIAB_NAME_BUFFER_LENGTH      100

EFI_STATUS
RemoveUnnessaryConfigureVariable (
  IN   EFI_GUID                        *VariableGuid,
  IN   UINT32                          VariableAttribute
  )
{
  EFI_STATUS                           Status;
  UINTN                                VariableNameSize;
  CHAR16                               *VariableName;
  UINTN                                BufferSize;
  EFI_GUID                             SearchVariableNameGuid;
  //
  // To remove configure variable, the Variable name length is 12 (24 bytes).
  //
  BufferSize   = sizeof (CHAR16) * VARIAB_NAME_BUFFER_LENGTH;
  VariableName = AllocateZeroPool (BufferSize);
  if (VariableName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (&SearchVariableNameGuid, sizeof (EFI_GUID));
  while (TRUE) {
    VariableNameSize = BufferSize;
    Status = gRT->GetNextVariableName (&VariableNameSize, VariableName, &SearchVariableNameGuid);
    if (Status == EFI_BUFFER_TOO_SMALL) {
      VariableName = ReallocatePool (BufferSize, VariableNameSize, VariableName);
      if (VariableName == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      BufferSize = VariableNameSize;
      Status = gRT->GetNextVariableName (&VariableNameSize, VariableName, &SearchVariableNameGuid);
    }
    if (EFI_ERROR (Status)) {
      break;
    }
    if (!CompareGuid (&SearchVariableNameGuid, VariableGuid)) {
      continue;
    }
    if (VariableNameSize != sizeof (CHAR16) * (12+1)) {
      continue;
    }
    Status = gRT->SetVariable (VariableName, VariableGuid, VariableAttribute, 0, NULL);
  }
  if (VariableName != NULL) {
    FreePool (VariableName);
  }
  return Status;
}

EFI_STATUS
GetHashSize (
  IN   EFI_GUID                        *HashAlgorithm,
  OUT  UINTN                           *HashSize
  )
{
  EFI_STATUS                           Status;
  EFI_HASH2_PROTOCOL                   *Hash2;
  EFI_HASH_PROTOCOL                    *Hash;

  Status = EFI_NOT_READY;
  if (mHash2Handle != NULL) {
    Status = gBS->HandleProtocol (mHash2Handle, &gEfiHash2ProtocolGuid, (VOID **) &Hash2);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = Hash2->GetHashSize (Hash2, HashAlgorithm, HashSize);

  } else if (mHashHandle != NULL) {
    Status = gBS->HandleProtocol (mHashHandle, &gEfiHashProtocolGuid, (VOID **) &Hash);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = Hash->GetHashSize (Hash, HashAlgorithm, HashSize);

  } else {
    /// CRC32, output is about 4 bytes.
    *HashSize = 4;
    Status = EFI_SUCCESS;
  }
  return Status;
}

EFI_STATUS
CalculateHashResult (
  IN   EFI_GUID                        *HashAlgorithm,
  IN   UINT8                           *InputBuffer,
  IN   UINTN                           InputBufferLen,
  OUT  UINT8                           *OutputBuffer,
  IN OUT UINTN                         *OutputBufferLen
  )
{
  EFI_STATUS                           Status;
  EFI_HASH2_PROTOCOL                   *Hash2;
  EFI_HASH_PROTOCOL                    *Hash;
  UINTN                                HashSize;
  EFI_HASH_OUTPUT                      HashOutput;

  HashSize = 0;
  Status = GetHashSize (HashAlgorithm, &HashSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (HashSize > *OutputBufferLen) {
    *OutputBufferLen = HashSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  if (mHash2Handle != NULL) {
    Status = gBS->HandleProtocol (mHash2Handle, &gEfiHash2ProtocolGuid, (VOID **) &Hash2);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = Hash2->Hash (
                      Hash2,
                      HashAlgorithm,
                      InputBuffer,
                      InputBufferLen,
                      (EFI_HASH2_OUTPUT *) OutputBuffer
                      );

  } else if (mHashHandle != NULL) {
    Status = gBS->HandleProtocol (mHashHandle, &gEfiHashProtocolGuid, (VOID **) &Hash);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    HashOutput.Sha1Hash = (EFI_SHA1_HASH *) OutputBuffer;
    Status = Hash->Hash (
                     Hash,
                     HashAlgorithm,
                     FALSE,
                     InputBuffer,
                     InputBufferLen,
                     &HashOutput
                     );

  } else {
    Status = gBS->CalculateCrc32 (
                    InputBuffer,
                    InputBufferLen,
                    (UINT32 *) OutputBuffer
                    );
  }
  return Status;
}

CHAR16
Binary16BitsToChar16 (
  IN    UINT8           Value
  )
{
  if (Value >= 0x10) {
    Value = Value % 0x10;
  }
  if ((Value >= 0x00) && (Value <= 0x09)) {
    return Value + '0';
  } else if ((Value >= 0x0A) && (Value <= 0x0F)) {
    return Value - 0x0A + 'A';
  }
  return '0';
}

/**
  Convert binary to hex format string.

  @param[in]  Buffer            The binary data.
  @param[in]  BufferSize        The size in bytes of the binary data.
  @param[in, out] HexString     Hex format string.
  @param[in]      HexStringSize The size in bytes of the string.

  @return The hex format string.
**/
CHAR16*
BinaryToHexString (
  IN     VOID    *Buffer,
  IN     UINTN   BufferSize,
  IN OUT CHAR16  *HexString,
  IN     UINTN   HexStringSize
  )
{
  UINTN Index;
  UINTN StringIndex;
  UINT8 *HexValue;
  UINT8 HighValue;
  UINT8 LowValue;

  ASSERT (Buffer != NULL);
  ASSERT ((BufferSize * 2 + 1) * sizeof (CHAR16) <= HexStringSize);

  HexValue = Buffer;
  StringIndex = 0;
  for (Index = 0; Index < BufferSize; Index += 1) {
    HighValue = HexValue[Index] / 16;
    LowValue  = HexValue[Index] % 16;
    HexString[StringIndex]   = Binary16BitsToChar16 (HighValue);
    HexString[StringIndex+1] = Binary16BitsToChar16 (LowValue);
    StringIndex += 2;
  }
  return HexString;
}

/**
  Convert DevicePath binary data to HASH (SHA-1) string.

  @param[in]  DevicePath        The binary data of device path.
  @param[out] HashString        The HEX base string.

  @return The hex format string in UNICODE.
**/
EFI_STATUS
STATIC
DevicePathToHashString (
  IN   EFI_DEVICE_PATH_PROTOCOL        *DevicePath,
  OUT  CHAR16                          **HashString
  )
{
  EFI_STATUS                           Status;
  UINTN                                HashSize;
  UINT8                                *HashResult;
  UINTN                                HashHexStringLength;
  UINT16                               *HashHexString;
  UINTN                                DevicePathSize;

  Status = InitializeHashProtocol ();
  if (EFI_ERROR (Status)) {
    /// If Hash and Hash2 not exist, use CRC32 to do hash function.
    // return Status;
  }

  Status = GetHashSize (&gEfiHashAlgorithmSha1Guid, &HashSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DevicePathSize      = GetDevicePathSize (DevicePath);  
  HashHexStringLength = (HashSize * 2 + 1) * sizeof (CHAR16);
  HashResult          = AllocateZeroPool (HashSize);
  HashHexString       = AllocateZeroPool (HashHexStringLength);
  if ((HashResult == NULL) || (HashHexString == NULL)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto FUNCTION_EXIT;
  }

  Status = CalculateHashResult (
             &gEfiHashAlgorithmSha1Guid,
             (UINT8 *) DevicePath,
             DevicePathSize,
             HashResult,
             &HashSize
             );
  if (EFI_ERROR (Status)) {
    goto FUNCTION_EXIT;
  }
  
  BinaryToHexString (HashResult, HashSize, HashHexString, HashHexStringLength);
  /// Output the correct HashHexString.
  *HashString   = HashHexString;
  HashHexString = NULL;
  Status        = EFI_SUCCESS;
  
FUNCTION_EXIT:
  if (HashResult != NULL) {
    FreePool (HashResult);
  }
  if (HashHexString != NULL) {
    FreePool (HashHexString);
  }
  return Status;
}

/**
  Truncate the last Node of Device.

  @param[in, out] DevicePath    DevicePath instance.

  @no return values.
**/
VOID
STATIC
TruncateLastNode (
  IN OUT EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL    *LastNode;
  EFI_DEVICE_PATH_PROTOCOL    *DevPath;

  if (DevicePath == NULL) {
    return;
  }

  LastNode = NULL;
  DevPath  = DevicePath;
  while (!IsDevicePathEnd(DevPath)) {
      LastNode = DevPath;
      DevPath = NextDevicePathNode (DevPath);
  }

  if (LastNode != NULL) {
    CopyMem( LastNode, DevPath, sizeof(EFI_DEVICE_PATH_PROTOCOL));
  }

  return;
}

/**
  Get DevicePath protocol from handle, generate the unique SHA-1 HEX string.

  @param[in]  DeviceHandle      The handle of device.
  @param[out] UniqueString      String generate by DevicePath, if DevicePath is not duplicated and Modified.
                                This string should be unique in POST time.

  @return EFI_INVAILD_PARAMETER        DeviceHandle or UniqueString is NULL value.
  @return EFI_INVAILD_PARAMETER        DevicePath protocol not exist in input handle.
  @return EFI_NOT_READY                gEfiDevicePathToTextProtocolGuid protocol is not ready.
  @return EFI_OUT_OF_RESOURCES         Memory allocate pool failed.
  @return others                       Status from gBS->HandleProtocol.
**/
EFI_STATUS
GetUniqueStringByDevicePath (
  EFI_HANDLE                           *DeviceHandle,
  CHAR16                               **UniqueString
  )
{
  EFI_STATUS                           Status;
  EFI_DEVICE_PATH                      *ControllerDevicePath;
  EFI_DEVICE_PATH                      *ParentDevicePath;
  CHAR16                               *HashHexString;
  
  if ((DeviceHandle == NULL) || (UniqueString == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ParentDevicePath     = NULL;
  HashHexString        = NULL;
  Status = gBS->HandleProtocol (DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID **) &ControllerDevicePath);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ParentDevicePath = DuplicateDevicePath (ControllerDevicePath);
  if (ParentDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  TruncateLastNode (ParentDevicePath);
  Status = DevicePathToHashString (ParentDevicePath, &HashHexString);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  } else {
    if (*UniqueString != NULL) {
	  FreePool (*UniqueString); 
	}
    *UniqueString = HashHexString;
  }
  Status = EFI_SUCCESS;

ON_EXIT:
  if (ParentDevicePath != NULL) {
    FreePool (ParentDevicePath);
  }
  return Status;
}
