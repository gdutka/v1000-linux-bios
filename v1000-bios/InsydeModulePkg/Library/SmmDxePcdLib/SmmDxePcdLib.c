/** @file
Implementation of PcdLib class library for DXE phase.

;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2006 - 2014, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


**/


#include <PiDxe.h>

#include <Protocol/Pcd.h>
#include <Protocol/PiPcd.h>
#include <Protocol/PcdInfo.h>
#include <Protocol/PiPcdInfo.h>

#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/LoadedImage.h>

STATIC PCD_PROTOCOL                  *mPcd          = NULL;
STATIC PCD_PROTOCOL                  *mSmmPcd       = NULL;
STATIC EFI_PCD_PROTOCOL              *mPiPcd        = NULL;
STATIC EFI_PCD_PROTOCOL              *mSmmPiPcd     = NULL;
STATIC GET_PCD_INFO_PROTOCOL         *mPcdInfo      = NULL;
STATIC GET_PCD_INFO_PROTOCOL         *mSmmPcdInfo   = NULL;
STATIC EFI_GET_PCD_INFO_PROTOCOL     *mPiPcdInfo    = NULL;
STATIC EFI_GET_PCD_INFO_PROTOCOL     *mSmmPiPcdInfo = NULL;
STATIC EFI_EVENT                     mVirtualNotifyEvent;
STATIC EFI_SMM_SYSTEM_TABLE2         *mSmst      = NULL;

/**
  Retrieves the PI PCD protocol from the handle database.

  @retval EFI_PCD_PROTOCOL * The pointer to the EFI_PCD_PROTOCOL.
**/
EFI_PCD_PROTOCOL *
EFIAPI
GetPiPcdProtocol (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mSmst != NULL) {
    if (mSmmPiPcd != NULL) {
      return mSmmPiPcd;
    }
    Status = mSmst->SmmLocateProtocol (&gEfiPcdProtocolGuid, NULL, (VOID **) &mSmmPiPcd);
    if (Status == EFI_SUCCESS) {
      return mSmmPiPcd;
    }
  }
  return mPiPcd;
}

/**
  Retrieves the PCD protocol from the handle database.

  @retval PCD_PROTOCOL * The pointer to the PCD_PROTOCOL.
**/
PCD_PROTOCOL *
EFIAPI
GetPcdProtocol (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mSmst != NULL) {
    if (mSmmPcd != NULL) {
      return mSmmPcd;
    }
    Status = mSmst->SmmLocateProtocol (&gPcdProtocolGuid, NULL, (VOID **) &mSmmPcd);
    if (Status == EFI_SUCCESS) {
      return mSmmPcd;
    }
  }
  return mPcd;
}

/**
  Retrieves the PI PCD info protocol from the handle database.

  @retval EFI_GET_PCD_INFO_PROTOCOL * The pointer to the EFI_GET_PCD_INFO_PROTOCOL defined in PI 1.2.1 Vol 3.
**/
EFI_GET_PCD_INFO_PROTOCOL *
GetPiPcdInfoProtocolPointer (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mSmst != NULL) {
    if (mSmmPiPcdInfo != NULL) {
      return mSmmPiPcdInfo;
    }
    Status = mSmst->SmmLocateProtocol (&gEfiGetPcdInfoProtocolGuid, NULL, (VOID **) &mSmmPiPcdInfo);
    if (Status == EFI_SUCCESS) {
      return mSmmPiPcdInfo;
    }
  }
  return mPiPcdInfo;
}

/**
  Retrieves the PCD info protocol from the handle database.

  @retval GET_PCD_INFO_PROTOCOL * The pointer to the GET_PCD_INFO_PROTOCOL.
**/
GET_PCD_INFO_PROTOCOL *
GetPcdInfoProtocolPointer (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mSmst != NULL) {
    if (mSmmPcdInfo != NULL) {
      return mSmmPcdInfo;
    }
    Status = mSmst->SmmLocateProtocol (&gGetPcdInfoProtocolGuid, NULL, (VOID **) &mSmmPcdInfo);
    if (Status == EFI_SUCCESS) {
      return mSmmPcdInfo;
    }
  }
  return mPcdInfo;
}


/**
  This function provides a means by which SKU support can be established in the PCD infrastructure.

  Sets the current SKU in the PCD database to the value specified by SkuId.  SkuId is returned.

  @param  SkuId   The SKU value that will be used when the PCD service retrieves and sets values
                  associated with a PCD token.

  @return  Return the SKU ID that just be set.

**/
UINTN
EFIAPI
LibPcdSetSku (
  IN UINTN   SkuId
  )
{
  GetPcdProtocol()->SetSku (SkuId);

  return SkuId;
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 8-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 8-bit value for the token specified by TokenNumber.

**/
UINT8
EFIAPI
LibPcdGet8 (
  IN UINTN             TokenNumber
  )
{
  return GetPcdProtocol()->Get8 (TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 16-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 16-bit value for the token specified by TokenNumber.

**/
UINT16
EFIAPI
LibPcdGet16 (
  IN UINTN             TokenNumber
  )
{
  return GetPcdProtocol()->Get16 (TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 32-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 32-bit value for the token specified by TokenNumber.

**/
UINT32
EFIAPI
LibPcdGet32 (
  IN UINTN             TokenNumber
  )
{
  return GetPcdProtocol()->Get32 (TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 64-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 64-bit value for the token specified by TokenNumber.

**/
UINT64
EFIAPI
LibPcdGet64 (
  IN UINTN             TokenNumber
  )
{
  return GetPcdProtocol()->Get64 (TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the pointer to the buffer of the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the pointer to the token specified by TokenNumber.

**/
VOID *
EFIAPI
LibPcdGetPtr (
  IN UINTN             TokenNumber
  )
{
  return GetPcdProtocol()->GetPtr (TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the Boolean value of the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the Boolean value of the token specified by TokenNumber.

**/
BOOLEAN
EFIAPI
LibPcdGetBool (
  IN UINTN             TokenNumber
  )
{
  return GetPcdProtocol()->GetBool (TokenNumber);
}



/**
  This function provides a means by which to retrieve the size of a given PCD token.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the size of the token specified by TokenNumber.

**/
UINTN
EFIAPI
LibPcdGetSize (
  IN UINTN             TokenNumber
  )
{
  return GetPcdProtocol()->GetSize (TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 8-bit value for the token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid         The pointer to a 128-bit unique value that designates
                           which namespace to retrieve a value from.
  @param[in]  TokenNumber  The PCD token number to retrieve a current value for.

  @return Return the UINT8.

**/
UINT8
EFIAPI
LibPcdGetEx8 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  ASSERT (Guid != NULL);

  return GetPiPcdProtocol()->Get8 (Guid, TokenNumber);
}


/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 16-bit value for the token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid         The pointer to a 128-bit unique value that designates
                           which namespace to retrieve a value from.
  @param[in]  TokenNumber  The PCD token number to retrieve a current value for.

  @return Return the UINT16.

**/
UINT16
EFIAPI
LibPcdGetEx16 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  ASSERT (Guid != NULL);

  return GetPiPcdProtocol()->Get16 (Guid, TokenNumber);
}


/**
  Returns the 32-bit value for the token specified by TokenNumber and Guid.
  If Guid is NULL, then ASSERT().

  @param[in]  Guid         The pointer to a 128-bit unique value that designates
                           which namespace to retrieve a value from.
  @param[in]  TokenNumber  The PCD token number to retrieve a current value for.

  @return Return the UINT32.

**/
UINT32
EFIAPI
LibPcdGetEx32 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  ASSERT (Guid != NULL);

  return GetPiPcdProtocol()->Get32 (Guid, TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 64-bit value for the token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that designates
                            which namespace to retrieve a value from.
  @param[in]  TokenNumber   The PCD token number to retrieve a current value for.

  @return Return the UINT64.

**/
UINT64
EFIAPI
LibPcdGetEx64 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  ASSERT (Guid != NULL);

  return GetPiPcdProtocol()->Get64 (Guid, TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the pointer to the buffer of token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that designates
                            which namespace to retrieve a value from.
  @param[in]  TokenNumber   The PCD token number to retrieve a current value for.

  @return Return the VOID* pointer.

**/
VOID *
EFIAPI
LibPcdGetExPtr (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  ASSERT (Guid != NULL);
  return GetPiPcdProtocol()->GetPtr (Guid, TokenNumber);
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the Boolean value of the token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that designates
                            which namespace to retrieve a value from.
  @param[in]  TokenNumber   The PCD token number to retrieve a current value for.

  @return Return the BOOLEAN.

**/
BOOLEAN
EFIAPI
LibPcdGetExBool (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  ASSERT (Guid != NULL);

  return GetPiPcdProtocol()->GetBool (Guid, TokenNumber);
}



/**
  This function provides a means by which to retrieve the size of a given PCD token.

  Returns the size of the token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that designates
                            which namespace to retrieve a value from.
  @param[in]  TokenNumber   The PCD token number to retrieve a current value for.

  @return Return the size.

**/
UINTN
EFIAPI
LibPcdGetExSize (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  ASSERT (Guid != NULL);

  return GetPiPcdProtocol()->GetSize (Guid, TokenNumber);
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 8-bit value for the token specified by TokenNumber
  to the value specified by Value.  Value is returned.

  If the set operation was not correctly performed, then ASSERT().

  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 8-bit value to set.

  @return Return the value that was set.

**/
UINT8
EFIAPI
LibPcdSet8 (
  IN UINTN             TokenNumber,
  IN UINT8             Value
  )
{
  EFI_STATUS Status;

  Status = GetPcdProtocol()->Set8 (TokenNumber, Value);
  ASSERT_EFI_ERROR (Status);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 16-bit value for the token specified by TokenNumber
  to the value specified by Value.  Value is returned.

  If the set operation was not correctly performed, then ASSERT().

  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 16-bit value to set.

  @return Return the value that was set.

**/
UINT16
EFIAPI
LibPcdSet16 (
  IN UINTN             TokenNumber,
  IN UINT16            Value
  )
{
  EFI_STATUS Status;

  Status = GetPcdProtocol()->Set16 (TokenNumber, Value);
  ASSERT_EFI_ERROR (Status);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 32-bit value for the token specified by TokenNumber
  to the value specified by Value.  Value is returned.

  If the set operation was not correctly performed, then ASSERT().

  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 32-bit value to set.

  @return Return the value that was set.

**/
UINT32
EFIAPI
LibPcdSet32 (
  IN UINTN             TokenNumber,
  IN UINT32            Value
  )
{
  EFI_STATUS Status;

  Status = GetPcdProtocol()->Set32 (TokenNumber, Value);
  ASSERT_EFI_ERROR (Status);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 64-bit value for the token specified by TokenNumber
  to the value specified by Value.  Value is returned.

  If the set operation was not correctly performed, then ASSERT().

  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 64-bit value to set.

  @return Return the value that was set.

**/
UINT64
EFIAPI
LibPcdSet64 (
  IN UINTN             TokenNumber,
  IN UINT64            Value
  )
{
  EFI_STATUS Status;

  Status = GetPcdProtocol()->Set64 (TokenNumber, Value);
  ASSERT_EFI_ERROR (Status);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets a buffer for the token specified by TokenNumber to the value
  specified by Buffer and SizeOfBuffer.  Buffer is returned.
  If SizeOfBuffer is greater than the maximum size support by TokenNumber,
  then set SizeOfBuffer to the maximum size supported by TokenNumber and
  return NULL to indicate that the set operation was not actually performed,
  or ASSERT() if the set operation was not correctly performed.

  If SizeOfBuffer is set to MAX_ADDRESS, then SizeOfBuffer must be set to the
  maximum size supported by TokenName and NULL must be returned.

  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[in]      TokenNumber   The PCD token number to set a current value for.
  @param[in, out] SizeOfBuffer  The size, in bytes, of Buffer.
  @param[in]      Buffer        A pointer to the buffer to set.

  @return Return the pointer for the buffer been set.

**/
VOID *
EFIAPI
LibPcdSetPtr (
  IN        UINTN             TokenNumber,
  IN OUT    UINTN             *SizeOfBuffer,
  IN CONST  VOID              *Buffer
  )
{
  EFI_STATUS Status;
  UINTN      InputSizeOfBuffer;

  ASSERT (SizeOfBuffer != NULL);

  if (*SizeOfBuffer > 0) {
    ASSERT (Buffer != NULL);
  }

  InputSizeOfBuffer = *SizeOfBuffer;
  Status = GetPcdProtocol()->SetPtr (TokenNumber, SizeOfBuffer, (VOID *) Buffer);
  if (EFI_ERROR (Status) && (*SizeOfBuffer < InputSizeOfBuffer)) {
    return NULL;
  }
  ASSERT_EFI_ERROR (Status);

  return (VOID *)Buffer;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the Boolean value for the token specified by TokenNumber
  to the value specified by Value.  Value is returned.

  If the set operation was not correctly performed, then ASSERT().

  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The boolean value to set.

  @return Return the value that was set.

**/
BOOLEAN
EFIAPI
LibPcdSetBool (
  IN UINTN             TokenNumber,
  IN BOOLEAN           Value
  )
{
  EFI_STATUS Status;

  Status = GetPcdProtocol()->SetBool (TokenNumber, Value);
  ASSERT_EFI_ERROR (Status);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 8-bit value for the token specified by TokenNumber and
  Guid to the value specified by Value. Value is returned.

  If Guid is NULL, then ASSERT().
  If the set operation was not correctly performed, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 8-bit value to set.

  @return Return the value that was set.

**/
UINT8
EFIAPI
LibPcdSetEx8 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber,
  IN UINT8             Value
  )
{
  EFI_STATUS Status;

  ASSERT (Guid != NULL);

  Status = GetPiPcdProtocol()->Set8 (Guid, TokenNumber, Value);
  ASSERT_EFI_ERROR (Status);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 16-bit value for the token specified by TokenNumber and
  Guid to the value specified by Value. Value is returned.

  If Guid is NULL, then ASSERT().
  If the set operation was not correctly performed, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 16-bit value to set.

  @return Return the value that was set.

**/
UINT16
EFIAPI
LibPcdSetEx16 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber,
  IN UINT16            Value
  )
{
  EFI_STATUS Status;

  ASSERT (Guid != NULL);

  Status = GetPiPcdProtocol()->Set16 (Guid, TokenNumber, Value);
  ASSERT_EFI_ERROR (Status);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 32-bit value for the token specified by TokenNumber and
  Guid to the value specified by Value. Value is returned.

  If Guid is NULL, then ASSERT().
  If the set operation was not correctly performed, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 32-bit value to set.

  @return Return the value that was set.

**/
UINT32
EFIAPI
LibPcdSetEx32 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber,
  IN UINT32            Value
  )
{
  EFI_STATUS Status;

  ASSERT (Guid != NULL);

  Status = GetPiPcdProtocol()->Set32 (Guid, TokenNumber, Value);
  ASSERT_EFI_ERROR (Status);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 64-bit value for the token specified by TokenNumber and
  Guid to the value specified by Value. Value is returned.

  If Guid is NULL, then ASSERT().
  If the set operation was not correctly performed, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The 64-bit value to set.

  @return Return the value that was set.

**/
UINT64
EFIAPI
LibPcdSetEx64 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber,
  IN UINT64            Value
  )
{
  EFI_STATUS Status;

  ASSERT (Guid != NULL);

  Status = GetPiPcdProtocol()->Set64 (Guid, TokenNumber, Value);
  ASSERT_EFI_ERROR (Status);

  return Value;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets a buffer for the token specified by TokenNumber to the value specified by
  Buffer and SizeOfBuffer.  Buffer is returned.  If SizeOfBuffer is greater than
  the maximum size support by TokenNumber, then set SizeOfBuffer to the maximum size
  supported by TokenNumber and return NULL to indicate that the set operation
  was not actually performed, or ASSERT() if the set operation was not corretly performed.

  If Guid is NULL, then ASSERT().
  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[in]  Guid              The pointer to a 128-bit unique value that
                                designates which namespace to set a value from.
  @param[in]  TokenNumber       The PCD token number to set a current value for.
  @param[in, out] SizeOfBuffer  The size, in bytes, of Buffer.
  @param[in]  Buffer            A pointer to the buffer to set.

  @return Return the pointer to the buffer been set.

**/
VOID *
EFIAPI
LibPcdSetExPtr (
  IN      CONST GUID        *Guid,
  IN      UINTN             TokenNumber,
  IN OUT  UINTN             *SizeOfBuffer,
  IN      VOID              *Buffer
  )
{
  EFI_STATUS  Status;
  UINTN       InputSizeOfBuffer;

  ASSERT (Guid != NULL);

  ASSERT (SizeOfBuffer != NULL);

  if (*SizeOfBuffer > 0) {
    ASSERT (Buffer != NULL);
  }

  InputSizeOfBuffer = *SizeOfBuffer;
  Status = GetPiPcdProtocol()->SetPtr (Guid, TokenNumber, SizeOfBuffer, Buffer);
  if (EFI_ERROR (Status) && (*SizeOfBuffer < InputSizeOfBuffer)) {
    return NULL;
  }
  ASSERT_EFI_ERROR (Status);

  return Buffer;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the Boolean value for the token specified by TokenNumber and
  Guid to the value specified by Value. Value is returned.

  If Guid is NULL, then ASSERT().
  If the set operation was not correctly performed, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in]  TokenNumber   The PCD token number to set a current value for.
  @param[in]  Value         The Boolean value to set.

  @return Return the value that was set.

**/
BOOLEAN
EFIAPI
LibPcdSetExBool (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber,
  IN BOOLEAN           Value
  )
{
  EFI_STATUS Status;

  ASSERT (Guid != NULL);

  Status = GetPiPcdProtocol()->SetBool (Guid, TokenNumber, Value);
  ASSERT_EFI_ERROR (Status);

  return Value;
}



/**
  Set up a notification function that is called when a specified token is set.

  When the token specified by TokenNumber and Guid is set,
  then notification function specified by NotificationFunction is called.
  If Guid is NULL, then the default token space is used.
  If NotificationFunction is NULL, then ASSERT().

  @param[in]  Guid                  The pointer to a 128-bit unique value that designates which
                                    namespace to set a value from.  If NULL, then the default
                                    token space is used.
  @param[in]  TokenNumber           The PCD token number to monitor.
  @param[in]  NotificationFunction  The function to call when the token
                                    specified by Guid and TokenNumber is set.

**/
VOID
EFIAPI
LibPcdCallbackOnSet (
  IN CONST GUID               *Guid,       OPTIONAL
  IN UINTN                    TokenNumber,
  IN PCD_CALLBACK             NotificationFunction
  )
{
  EFI_STATUS Status;

  if (mSmst != NULL) {
    return;
  }

  ASSERT (NotificationFunction != NULL);

  Status = GetPiPcdProtocol()->CallbackOnSet (Guid, TokenNumber, (EFI_PCD_PROTOCOL_CALLBACK) NotificationFunction);
  ASSERT_EFI_ERROR (Status);

  return;
}



/**
  Disable a notification function that was established with LibPcdCallbackonSet().

  Disable a notification function that was previously established with LibPcdCallbackOnSet().
  If NotificationFunction is NULL, then ASSERT().
  If LibPcdCallbackOnSet() was not previously called with Guid, TokenNumber,
  and NotificationFunction, then ASSERT().

  @param[in]  Guid                 Specify the GUID token space.
  @param[in]  TokenNumber          Specify the token number.
  @param[in]  NotificationFunction The callback function to be unregistered.

**/
VOID
EFIAPI
LibPcdCancelCallback (
  IN CONST GUID               *Guid,       OPTIONAL
  IN UINTN                    TokenNumber,
  IN PCD_CALLBACK             NotificationFunction
  )
{
  EFI_STATUS Status;

  if (mSmst != NULL) {
    return;
  }

  ASSERT (NotificationFunction != NULL);

  Status = GetPiPcdProtocol()->CancelCallback (Guid, TokenNumber, (EFI_PCD_PROTOCOL_CALLBACK) NotificationFunction);
  ASSERT_EFI_ERROR (Status);

  return;
}



/**
  Retrieves the next token in a token space.

  Retrieves the next PCD token number from the token space specified by Guid.
  If Guid is NULL, then the default token space is used.  If TokenNumber is 0,
  then the first token number is returned.  Otherwise, the token number that
  follows TokenNumber in the token space is returned.  If TokenNumber is the last
  token number in the token space, then 0 is returned.

  If TokenNumber is not 0 and is not in the token space specified by Guid, then ASSERT().

  @param[in]  Guid        The pointer to a 128-bit unique value that designates which namespace
                          to set a value from.  If NULL, then the default token space is used.
  @param[in]  TokenNumber The previous PCD token number.  If 0, then retrieves the first PCD
                          token number.

  @return The next valid token number.

**/
UINTN
EFIAPI
LibPcdGetNextToken (
  IN CONST GUID               *Guid,       OPTIONAL
  IN UINTN                    TokenNumber
  )
{
  EFI_STATUS    Status;

  Status = GetPiPcdProtocol()->GetNextToken (Guid, &TokenNumber);
  ASSERT (!EFI_ERROR (Status) || TokenNumber == 0);

  return TokenNumber;
}



/**
  Used to retrieve the list of available PCD token space GUIDs.

  Returns the PCD token space GUID that follows TokenSpaceGuid in the list of token spaces
  in the platform.
  If TokenSpaceGuid is NULL, then a pointer to the first PCD token spaces returned.
  If TokenSpaceGuid is the last PCD token space GUID in the list, then NULL is returned.

  @param  TokenSpaceGuid  The pointer to the a PCD token space GUID.

  @return The next valid token namespace.

**/
GUID *
EFIAPI
LibPcdGetNextTokenSpace (
  IN CONST GUID  *TokenSpaceGuid
  )
{
  GetPiPcdProtocol()->GetNextTokenSpace (&TokenSpaceGuid);

  return (GUID *)TokenSpaceGuid;
}


/**
  Sets a value of a patchable PCD entry that is type pointer.

  Sets the PCD entry specified by PatchVariable to the value specified by Buffer
  and SizeOfBuffer.  Buffer is returned.  If SizeOfBuffer is greater than
  MaximumDatumSize, then set SizeOfBuffer to MaximumDatumSize and return
  NULL to indicate that the set operation was not actually performed.
  If SizeOfBuffer is set to MAX_ADDRESS, then SizeOfBuffer must be set to
  MaximumDatumSize and NULL must be returned.

  If PatchVariable is NULL, then ASSERT().
  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[in] PatchVariable      A pointer to the global variable in a module that is
                                the target of the set operation.
  @param[in] MaximumDatumSize   The maximum size allowed for the PCD entry specified by PatchVariable.
  @param[in, out] SizeOfBuffer  A pointer to the size, in bytes, of Buffer.
  @param[in] Buffer             A pointer to the buffer to used to set the target variable.

  @return Return the pointer to the buffer been set.

**/
VOID *
EFIAPI
LibPatchPcdSetPtr (
  IN        VOID        *PatchVariable,
  IN        UINTN       MaximumDatumSize,
  IN OUT    UINTN       *SizeOfBuffer,
  IN CONST  VOID        *Buffer
  )
{
  ASSERT (PatchVariable != NULL);
  ASSERT (SizeOfBuffer  != NULL);

  if (*SizeOfBuffer > 0) {
    ASSERT (Buffer != NULL);
  }

  if ((*SizeOfBuffer > MaximumDatumSize) ||
      (*SizeOfBuffer == MAX_ADDRESS)) {
    *SizeOfBuffer = MaximumDatumSize;
    return NULL;
  }

  CopyMem (PatchVariable, Buffer, *SizeOfBuffer);

  return (VOID *) Buffer;
}


/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 8-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 8-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet8S (
  IN UINTN          TokenNumber,
  IN UINT8          Value
  )
{
  return GetPcdProtocol()->Set8 (TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 16-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 16-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet16S (
  IN UINTN          TokenNumber,
  IN UINT16         Value
  )
{
  return GetPcdProtocol()->Set16 (TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 32-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 32-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet32S (
  IN UINTN          TokenNumber,
  IN UINT32         Value
  )
{
  return GetPcdProtocol()->Set32 (TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 64-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 64-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet64S (
  IN UINTN          TokenNumber,
  IN UINT64         Value
  )
{
  return GetPcdProtocol()->Set64 (TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets a buffer for the token specified by TokenNumber to the value specified
  by Buffer and SizeOfBuffer. If SizeOfBuffer is greater than the maximum size
  support by TokenNumber, then set SizeOfBuffer to the maximum size supported by
  TokenNumber and return EFI_INVALID_PARAMETER to indicate that the set operation
  was not actually performed.

  If SizeOfBuffer is set to MAX_ADDRESS, then SizeOfBuffer must be set to the
  maximum size supported by TokenName and EFI_INVALID_PARAMETER must be returned.

  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[in]      TokenNumber   The PCD token number to set a current value for.
  @param[in, out] SizeOfBuffer  The size, in bytes, of Buffer.
  @param[in]      Buffer        A pointer to the buffer to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetPtrS (
  IN       UINTN    TokenNumber,
  IN OUT   UINTN    *SizeOfBuffer,
  IN CONST VOID     *Buffer
  )
{
  ASSERT (SizeOfBuffer != NULL);

  if (*SizeOfBuffer > 0) {
    ASSERT (Buffer != NULL);
  }

  return GetPcdProtocol()->SetPtr (TokenNumber, SizeOfBuffer, (VOID *) Buffer);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the boolean value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The boolean value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetBoolS (
  IN UINTN          TokenNumber,
  IN BOOLEAN        Value
  )
{
  return GetPcdProtocol()->SetBool (TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 8-bit value for the token specified by TokenNumber
  to the value specified by Value.

  If Guid is NULL, then ASSERT().

  @param[in] Guid           The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 8-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetEx8S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT8          Value
  )
{
  ASSERT (Guid != NULL);

  return GetPiPcdProtocol()->Set8 (Guid, TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 16-bit value for the token specified by TokenNumber
  to the value specified by Value.

  If Guid is NULL, then ASSERT().

  @param[in] Guid           The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 16-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetEx16S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT16         Value
  )
{
  ASSERT (Guid != NULL);

  return GetPiPcdProtocol()->Set16 (Guid, TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 32-bit value for the token specified by TokenNumber
  to the value specified by Value.

  If Guid is NULL, then ASSERT().

  @param[in] Guid           The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 32-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetEx32S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT32         Value
  )
{
  ASSERT (Guid != NULL);

  return GetPiPcdProtocol()->Set32 (Guid, TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 64-bit value for the token specified by TokenNumber
  to the value specified by Value.

  If Guid is NULL, then ASSERT().

  @param[in] Guid           The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 64-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetEx64S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT64         Value
  )
{
  ASSERT (Guid != NULL);

  return GetPiPcdProtocol()->Set64 (Guid, TokenNumber, Value);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets a buffer for the token specified by TokenNumber to the value specified by
  Buffer and SizeOfBuffer. If SizeOfBuffer is greater than the maximum size
  support by TokenNumber, then set SizeOfBuffer to the maximum size supported by
  TokenNumber and return EFI_INVALID_PARAMETER to indicate that the set operation
  was not actually performed.

  If Guid is NULL, then ASSERT().
  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[in]      Guid          Pointer to a 128-bit unique value that
                                designates which namespace to set a value from.
  @param[in]      TokenNumber   The PCD token number to set a current value for.
  @param[in, out] SizeOfBuffer  The size, in bytes, of Buffer.
  @param[in]      Buffer        A pointer to the buffer to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetExPtrS (
  IN CONST GUID     *Guid,
  IN       UINTN    TokenNumber,
  IN OUT   UINTN    *SizeOfBuffer,
  IN       VOID     *Buffer
  )
{
  ASSERT (Guid != NULL);

  ASSERT (SizeOfBuffer != NULL);

  if (*SizeOfBuffer > 0) {
    ASSERT (Buffer != NULL);
  }

  return GetPiPcdProtocol()->SetPtr (Guid, TokenNumber, SizeOfBuffer, Buffer);
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the boolean value for the token specified by TokenNumber
  to the value specified by Value.

  If Guid is NULL, then ASSERT().

  @param[in] Guid           The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The boolean value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetExBoolS (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN BOOLEAN        Value
  )
{
  ASSERT (Guid != NULL);

  return GetPiPcdProtocol()->SetBool (Guid, TokenNumber, Value);
}

/**
  Retrieve additional information associated with a PCD token.

  This includes information such as the type of value the TokenNumber is associated with as well as possible
  human readable name that is associated with the token.

  If TokenNumber is not in the default token space specified, then ASSERT().

  @param[in]    TokenNumber The PCD token number.
  @param[out]   PcdInfo     The returned information associated with the requested TokenNumber.
                            The caller is responsible for freeing the buffer that is allocated by callee for PcdInfo->PcdName.
**/
VOID
EFIAPI
LibPcdGetInfo (
  IN        UINTN           TokenNumber,
  OUT       PCD_INFO        *PcdInfo
  )
{
  EFI_STATUS                  Status;
  GET_PCD_INFO_PROTOCOL       *PcdInfoProtocol;

  PcdInfoProtocol = GetPcdInfoProtocolPointer();
  if (PcdInfoProtocol == NULL) {
    ASSERT (PcdInfoProtocol != NULL);
    return;
  }
  Status = PcdInfoProtocol->GetInfo (TokenNumber, (EFI_PCD_INFO *) PcdInfo);
  ASSERT_EFI_ERROR (Status);
}

/**
  Retrieve additional information associated with a PCD token.

  This includes information such as the type of value the TokenNumber is associated with as well as possible
  human readable name that is associated with the token.

  If TokenNumber is not in the token space specified by Guid, then ASSERT().

  @param[in]    Guid        The 128-bit unique value that designates the namespace from which to extract the value.
  @param[in]    TokenNumber The PCD token number.
  @param[out]   PcdInfo     The returned information associated with the requested TokenNumber.
                            The caller is responsible for freeing the buffer that is allocated by callee for PcdInfo->PcdName.
**/
VOID
EFIAPI
LibPcdGetInfoEx (
  IN CONST  GUID            *Guid,
  IN        UINTN           TokenNumber,
  OUT       PCD_INFO        *PcdInfo
  )
{
  EFI_STATUS                    Status;
  EFI_GET_PCD_INFO_PROTOCOL     *PiPcdInfoProtocol;

  PiPcdInfoProtocol = NULL;
  PiPcdInfoProtocol = GetPiPcdInfoProtocolPointer();
  if (PiPcdInfoProtocol == NULL) {
    ASSERT (PiPcdInfoProtocol != NULL);
    return;
  }

  Status = PiPcdInfoProtocol->GetInfo (Guid, TokenNumber, (EFI_PCD_INFO *) PcdInfo);
  ASSERT_EFI_ERROR (Status);
}

/**
  Retrieve the currently set SKU Id.


  @return   The currently set SKU Id. If the platform has not set at a SKU Id, then the
            default SKU Id value of 0 is returned. If the platform has set a SKU Id, then the currently set SKU
            Id is returned.
**/
UINTN
EFIAPI
LibPcdGetSku (
  VOID
  )
{
  UINTN SkuId;

  SkuId = GetPiPcdInfoProtocolPointer()->GetSku ();

  return SkuId;
}

/**
  Notification function of EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context

  @retval None
**/
VOID
EFIAPI
VirtualPointerConversionNotify (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  gRT->ConvertPointer (0, (VOID **)&mPcd);
  gRT->ConvertPointer (0, (VOID **)&mPiPcd);
  gRT->ConvertPointer (0, (VOID **)&mPcdInfo);
  gRT->ConvertPointer (0, (VOID **)&mPiPcdInfo);
}

/**
  Notification function of gPcdProtocolGuid.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context

  @retval None
**/
VOID
EFIAPI
PcdProtocolCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  gBS->LocateProtocol (&gPcdProtocolGuid, NULL, (VOID **) &mPcd);
}


/**
  Notification function of gEfiPcdProtocolGuid.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context

  @retval None
**/
VOID
EFIAPI
EfiPcdProtocolCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  gBS->LocateProtocol (&gEfiPcdProtocolGuid, NULL, (VOID **) &mPiPcd);
}

/**
  Notification function of gGetPcdInfoProtocolGuid.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context

  @retval None
**/
VOID
EFIAPI
GetPcdInfoProtocolCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  gBS->LocateProtocol (&gGetPcdInfoProtocolGuid, NULL, (VOID **) &mPcdInfo);
}

/**
  Notification function of gEfiGetPcdInfoProtocolGuid.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context

  @retval None
**/
VOID
EFIAPI
EfiGetPcdInfoProtocolCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  gBS->LocateProtocol (&gEfiGetPcdInfoProtocolGuid, NULL, (VOID **) &mPiPcdInfo);
}

/**
  Function uses image handle to check image is whther EfiRuntimeServicesCode image code type.

  @param[in] ImageHandle   Input Image handle.

  @retval TRUE             This is a EfiRuntimeServicesCode image code type iamge
  @retval FALSE            This isn't a EfiRuntimeServicesCode image code type iamge
**/
STATIC
BOOLEAN
IsRuntimeCodeImageType (
  IN EFI_HANDLE       ImageHandle
  )
{
  EFI_STATUS                       Status;
  EFI_LOADED_IMAGE_PROTOCOL        *LoadedImage;

  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **)&LoadedImage
                  );
  if (!EFI_ERROR (Status) && LoadedImage->ImageCodeType == EfiRuntimeServicesCode) {
    return TRUE;
  }

  return FALSE;
}


/**
  Creates and returns a notification event and registers that event with all the protocol
  instances specified by ProtocolGuid.


  @param  ProtocolGuid    Supplies GUID of the protocol upon whose installation the event is fired.
  @param  NotifyTpl       Supplies the task priority level of the event notifications.
  @param  NotifyFunction  Supplies the function to notify when the event is signaled.
  @param  NotifyContext   The context parameter to pass to NotifyFunction.
  @param  Registration    A pointer to a memory location to receive the registration value.
                          This value is passed to LocateHandle() to obtain new handles that
                          have been added that support the ProtocolGuid-specified protocol.

  @return The notification event that was created.

**/
STATIC
EFI_EVENT
PcdCreateProtocolNotifyEvent (
  IN  EFI_GUID          *ProtocolGuid,
  IN  EFI_TPL           NotifyTpl,
  IN  EFI_EVENT_NOTIFY  NotifyFunction,
  IN  VOID              *NotifyContext,  OPTIONAL
  OUT VOID              **Registration
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;

  ASSERT (ProtocolGuid != NULL);
  ASSERT (NotifyFunction != NULL);
  ASSERT (Registration != NULL);

  //
  // Create the event
  //

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  NotifyTpl,
                  NotifyFunction,
                  NotifyContext,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for protocol notifications on this event
  //

  Status = gBS->RegisterProtocolNotify (
                  ProtocolGuid,
                  Event,
                  Registration
                  );

  ASSERT_EFI_ERROR (Status);

  return Event;
}

/**
  PcdLib library class constructor for setting up PCD protocols during POST, at runtime and in SMM mode

  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
SmmDxePcdLibInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_SMM_BASE2_PROTOCOL  *SmmBase2;
  BOOLEAN                 InSmmMode;
  VOID                    *Registration;

  Status = gBS->LocateProtocol (&gPcdProtocolGuid, NULL, (VOID **) &mPcd);
  Status = gBS->LocateProtocol (&gEfiPcdProtocolGuid, NULL, (VOID **) &mPiPcd);

  //
  // Ignore the status of PCD Info protocols, because they might not be installed
  //
  gBS->LocateProtocol (&gGetPcdInfoProtocolGuid, NULL, (VOID **) &mPcdInfo);
  gBS->LocateProtocol (&gEfiGetPcdInfoProtocolGuid, NULL, (VOID **) &mPiPcdInfo);

  InSmmMode = FALSE;
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase2
                  );
  if (Status == EFI_SUCCESS) {
    SmmBase2->InSmm (SmmBase2, &InSmmMode);
  }
  if (!InSmmMode) {
    //
    // COMBINED_SMM_DXE modules can also be DXE_RUNTIME_DRIVER modules
    //
    Status = EFI_SUCCESS;
    if (IsRuntimeCodeImageType (ImageHandle)) {
      Status = gBS->CreateEventEx (
                      EVT_NOTIFY_SIGNAL,
                      TPL_NOTIFY,
                      VirtualPointerConversionNotify,
                      NULL,
                      &gEfiEventVirtualAddressChangeGuid,
                      &mVirtualNotifyEvent
                      );
      ASSERT_EFI_ERROR (Status);
    }
    //
    // Create related notifies to get related protocols if they are not avaiable at entry point.
    //
    if (mPcd == NULL) {
      PcdCreateProtocolNotifyEvent (
        &gPcdProtocolGuid,
        TPL_CALLBACK,
        PcdProtocolCallback,
        NULL,
        &Registration
        );
    }
    if (mPiPcd == NULL) {
      PcdCreateProtocolNotifyEvent (
        &gEfiPcdProtocolGuid,
        TPL_CALLBACK,
        EfiPcdProtocolCallback,
        NULL,
        &Registration
        );
    }
    if (mPcdInfo == NULL) {
      PcdCreateProtocolNotifyEvent (
        &gGetPcdInfoProtocolGuid,
        TPL_CALLBACK,
        GetPcdInfoProtocolCallback,
        NULL,
        &Registration
        );
    }
    if (mPiPcdInfo == NULL) {
      PcdCreateProtocolNotifyEvent (
        &gEfiGetPcdInfoProtocolGuid,
        TPL_CALLBACK,
        EfiGetPcdInfoProtocolCallback,
        NULL,
        &Registration
        );
    }
  } else {
    Status = SmmBase2->GetSmstLocation (SmmBase2, &mSmst);
    ASSERT_EFI_ERROR (Status);
    //
    // Note: Do not use SmmRegisterProtocolNotify here, because there is no CloseEvent() equivalent function in SMST
    //       to close the event in the Library destructor if the drivers linking this module fail.
    //
  }
  return Status;
}

/**
  PcdLib library class destructor
  The destructor function is required for closing the virtual pointer conversion event
  if dispatch fails to dispatch the module.

  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
SmmDxePcdLibDestruct (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  if (mSmst == NULL && IsRuntimeCodeImageType (ImageHandle)) {
    gBS->CloseEvent (mVirtualNotifyEvent);
  }
  return EFI_SUCCESS;
}