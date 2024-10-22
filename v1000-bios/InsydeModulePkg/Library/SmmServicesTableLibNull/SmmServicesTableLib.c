/** @file
 SMM Services Table Library.


***************************************************************************
* Copyright (c) 2019, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <PiSmm.h>
#include <Library/SmmServicesTableLib.h>

EFI_SMM_SYSTEM_TABLE2   *gSmst                    = NULL;

/**
  This function allows the caller to determine if the driver is executing in
  System Management Mode(SMM).

  This function always return FALSE to indicate system isn't in SMM

  @retval  TRUE  The driver is executing in System Management Mode (SMM).
  @retval  FALSE The driver is not executing in System Management Mode (SMM).

**/
BOOLEAN
EFIAPI
InSmm (
  VOID
  )
{
  return FALSE;
}
