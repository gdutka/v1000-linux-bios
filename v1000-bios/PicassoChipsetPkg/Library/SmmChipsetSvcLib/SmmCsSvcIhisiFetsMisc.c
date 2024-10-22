/** @file
 SMM Chipset Services Library.

 This file contains Ihisi Fets Chipset service Lib function.

***************************************************************************
* Copyright (c) 2014 - 2015, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Uefi.h>
#include <H2OIhisi.h>
#include <Library/DebugLib.h>
#include <Library/SmmChipsetSvcLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/H2OSmmChipsetServices.h>

#define RST_CNT                      0xCF9
#define V_RST_CNT_FULLRESET          0x0E

extern H2O_SMM_CHIPSET_SERVICES_PROTOCOL *mSmmChipsetSvc;

/**
  Fets reset system function.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
EFIAPI
SmmCsSvcIhisiFetsReboot (
  VOID
  )
{
  UINT8        Buffer;

  Buffer = 0;

  Buffer = V_RST_CNT_FULLRESET;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, RST_CNT, 1, &Buffer );
  return EFI_SUCCESS;
}

/**
  Fets shutdown function.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
EFIAPI
SmmCsSvcIhisiFetsShutdown (
  VOID
  )
{
  SmmCsSvcIhisiFbtsShutDown();
  return EFI_SUCCESS;
}

