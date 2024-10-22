/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcGetSleepState().
 The function PeiCsSvcGetSleepState() use chipset services to identify
 sleep state.

***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

//
// Libraries
//
#include <Library/IoLib.h>

#include <YangtzeReg.h>

/**
 To identify sleep state.

 @param[in, out]    SleepState          3 - This is an S3 restart
                                        4 - This is an S4 restart

 @retval            EFI_SUCCESS         Function always returns successfully
*/
EFI_STATUS
GetSleepState (
  IN OUT  UINT8              *SleepState
  )
{
  UINTN                 AcpiPm1CntBlk;
  UINT16                PmControl;

  AcpiPm1CntBlk = MmioRead16 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG62);

  PmControl = IoRead16 (AcpiPm1CntBlk);

  switch (PmControl & B_SLP_TYPE) {

  case V_SLP_TYPE_S4:
    *SleepState = 4;
    break;

  case V_SLP_TYPE_S3:
    *SleepState = 3;
    break;

  default:
    //
    // Clear Sleep Type.Typically maps to S0.
    //
    *SleepState = 0;
    PmControl &= (~B_SLP_TYPE);
    IoWrite16 (AcpiPm1CntBlk, PmControl);
    break;
  }

  return EFI_SUCCESS;
}