/** @file
  Project dependent initial code before transfering the control to the PEI core.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/SecOemSvcChipsetLib.h>
#include <Library/PciExpressLib.h>
#include <Fch.h>

VOID
EnableEcIoDecode (
  VOID
  );

/**
  Project dependent initial code before transfering the control to the PEI core..

  @param  Base on OEM design.

  @retval EFI_UNSUPPORTED    Returns unsupported by default.
  @retval EFI_SUCCESS        The service is customized in the project.
  @retval EFI_MEDIA_CHANGED  The value of IN OUT parameter is changed. 
  @retval Others             Depends on customization.
**/
EFI_STATUS
OemSvcOemSecPlatformMain (
  IN OUT  EFI_SEC_PEI_HAND_OFF    *SecCoreData,
  IN OUT  EFI_PEI_PPI_DESCRIPTOR  *PpiList
  )
{
  //
  // Enable IO662-666 and IO4C/4D decode.
  //
  EnableEcIoDecode ();
  return EFI_SUCCESS;
}

VOID
EnableEcIoDecode (
  VOID
  )
{
  //
  // Enable IO662-666 and IO4C/4D decode.
  //
  PciExpressAndThenOr32 (
            PCI_EXPRESS_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, 0x90),
            (UINT32) ~(0x0000FFFF),
            (UINT32) 0x662);
  PciExpressAndThenOr32 (
            PCI_EXPRESS_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, 0x64),
            (UINT32) ~(0xFFFF0000),
            (UINT32) 0x004C0000);
  PciExpressAndThenOr32 (
            PCI_EXPRESS_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, 0x74),
            (UINT32) ~(BIT3 | BIT2),
            (UINT32) (BIT3 | BIT2));
  PciExpressAndThenOr32 (
            PCI_EXPRESS_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, 0x48),
            (UINT32) ~(BIT25 | BIT24 | BIT1),
            (UINT32) (BIT25 | BIT24 | BIT1));
}


