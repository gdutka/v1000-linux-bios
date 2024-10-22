/** @file
  The instance of Post Code Library

;******************************************************************************
;* Copyright (c) 2013 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>

#include <Library/PostCodeLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/H2OCpLib.h>
#include <Guid/H2OCp.h>

/**
  Internal function to initialize H2O_BASE_CP_POST_CODE_DATA data and trigger gH2OBaseCpPostCodeGuid checkpoint.

  @param[in] Value                The 32-bit value to write to the POST card.

  @retval The checkpoint status value.
**/
STATIC
UINT32
TriggerBaseCpPostCode (
  IN UINT32                             Value
  )
{
  H2O_BASE_CP_POST_CODE_DATA            PostCodeData;
  EFI_STATUS                            Status;

  PostCodeData.Size          = sizeof (H2O_BASE_CP_POST_CODE_DATA);
  PostCodeData.Status        = H2O_CP_TASK_NORMAL;
  PostCodeData.PostCodeValue = Value;
  DEBUG ((EFI_D_INFO, "Checkpoint Trigger: %g\n", &gH2OBaseCpPostCodeGuid));
  Status = H2OCpTrigger (&gH2OBaseCpPostCodeGuid, &PostCodeData);
  DEBUG ((EFI_D_INFO, "Checkpoint Result: %x\n", PostCodeData.Status));

  return PostCodeData.Status;
}

/**
  Sends an 32-bit value to a POST card.

  Sends the 32-bit value specified by Value to a POST card, and returns Value.
  Some implementations of this library function may perform I/O operations
  directly to a POST card device.  Other implementations may send Value to
  ReportStatusCode(), and the status code reporting mechanism will eventually
  display the 32-bit value on the status reporting device.

  PostCode() must actively prevent recursion.  If PostCode() is called while
  processing another any other Post Code Library function, then
  PostCode() must return Value immediately.

  @param  Value  The 32-bit value to write to the POST card.

  @return The 32-bit value to write to the POST card.

**/
UINT32
EFIAPI
PostCode (
  IN UINT32  Value
  )
{
  if (FeaturePcdGet (PcdH2OBaseCpPostCodeSupported)) {
    if (TriggerBaseCpPostCode (Value) == H2O_CP_TASK_SKIP) {
      return Value;
    }
  }

  IoWrite16 (PcdGet16(PcdH2ODebugPort), (UINT16)(Value));
  if (PcdGetBool (PcdPostCodeRedirectedToReportStatusCode)) {
    DEBUG((EFI_D_INFO, "POST CODE: %08x\n", Value));
  }

  return Value;
}


/**
  Sends an 32-bit value to a POST and associated ASCII string.

  Sends the 32-bit value specified by Value to a POST card, and returns Value.
  If Description is not NULL, then the ASCII string specified by Description is
  also passed to the handler that displays the POST card value.  Some
  implementations of this library function may perform I/O operations directly
  to a POST card device.  Other implementations may send Value to ReportStatusCode(),
  and the status code reporting mechanism will eventually display the 32-bit
  value on the status reporting device.

  PostCodeWithDescription()must actively prevent recursion.  If
  PostCodeWithDescription() is called while processing another any other Post
  Code Library function, then PostCodeWithDescription() must return Value
  immediately.

  @param  Value        The 32-bit value to write to the POST card.
  @param  Description  The pointer to an ASCII string that is a description of the
                       POST code value.  This is an optional parameter that may
                       be NULL.

  @return The 32-bit value to write to the POST card.

**/
UINT32
EFIAPI
PostCodeWithDescription (
  IN UINT32       Value,
  IN CONST CHAR8  *Description  OPTIONAL
  )
{
  if (FeaturePcdGet (PcdH2OBaseCpPostCodeSupported)) {
    if (TriggerBaseCpPostCode (Value) == H2O_CP_TASK_SKIP) {
      return Value;
    }
  }

  IoWrite16 (PcdGet16(PcdH2ODebugPort), (UINT16)(Value));
  if (PcdGetBool (PcdPostCodeRedirectedToReportStatusCode)) {
    DEBUG((EFI_D_INFO, "POST CODE: %08x - %s\n", Value, Description));
  }
  return Value;
}


/**
  Returns TRUE if POST Codes are enabled.

  This function returns TRUE if the POST_CODE_PROPERTY_POST_CODE_ENABLED
  bit of PcdPostCodePropertyMask is set.  Otherwise FALSE is returned.

  @retval  TRUE   The POST_CODE_PROPERTY_POST_CODE_ENABLED bit of
                  PcdPostCodeProperyMask is set.
  @retval  FALSE  The POST_CODE_PROPERTY_POST_CODE_ENABLED bit of
                  PcdPostCodeProperyMask is clear.

**/
BOOLEAN
EFIAPI
PostCodeEnabled (
  VOID
  )
{
  return (BOOLEAN) ((PcdGet8(PcdPostCodePropertyMask) & POST_CODE_PROPERTY_POST_CODE_ENABLED) != 0);
}


/**
  Returns TRUE if POST code descriptions are enabled.

  This function returns TRUE if the POST_CODE_PROPERTY_POST_CODE_DESCRIPTION_ENABLED
  bit of PcdPostCodePropertyMask is set.  Otherwise FALSE is returned.

  @retval  TRUE   The POST_CODE_PROPERTY_POST_CODE_DESCRIPTION_ENABLED bit of
                  PcdPostCodeProperyMask is set.
  @retval  FALSE  The POST_CODE_PROPERTY_POST_CODE_DESCRIPTION_ENABLED bit of
                  PcdPostCodeProperyMask is clear.

**/
BOOLEAN
EFIAPI
PostCodeDescriptionEnabled (
  VOID
  )
{
  return (BOOLEAN) ((PcdGet8(PcdPostCodePropertyMask) & POST_CODE_PROPERTY_POST_CODE_DESCRIPTION_ENABLED) != 0);
}
