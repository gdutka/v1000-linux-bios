/** @file
  Project dependent initial code for Insyde Debugger.

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
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/PlatformHookLib.h>

/**
  Project dependent initial code for Insyde Debugger.

  @param  Base on OEM design.

  @retval EFI_UNSUPPORTED    Returns unsupported by default.
  @retval EFI_SUCCESS        The service is customized in the project.
  @retval EFI_MEDIA_CHANGED  The value of IN OUT parameter is changed.
  @retval Others             Depends on customization.
**/
EFI_STATUS
 OemSvcInitializeInsydeDebugger (
  VOID
  )
{
//  UINT32    Value32;
//  UINT32    *Ptr;
//
//  //
//  // 0. Enable EHCI0.
//  //
//  Value32 = 0xFED80304;
//  Ptr = (UINT32 *)(UINTN)Value32;
//  *Ptr = (*Ptr | BIT1);
//
//  Value32 = 0xFED81E64;
//  Ptr = (UINT32 *)(UINTN)Value32;
//  *Ptr = (*Ptr | BIT3 | BIT0);
//  for (Value32 = 0; Value32 < 10000; Value32 ++) {
//    IoRead8(0xED);
//  }
//  //
//  // 1. Assign a temporary BAR0 to B:0x00/D:0x12/F:0x00 (EHCI0)
//  //
//  PciWrite32 (PCI_LIB_ADDRESS (FCH_EHCI1_BUS, FCH_EHCI1_DEV, 0, FCH_EHCI_REG10), 0xFC000000);
//
//  //
//  // 2. Enable MMIO decode on B:0x00/D:0x12/F:0x00 (EHCI0)
//  //
//  PciWrite32 (PCI_LIB_ADDRESS (FCH_EHCI1_BUS, FCH_EHCI1_DEV, 0, FCH_EHCI_REG04), 0x06);
//
//  Value32 = 0;
//  Value32 = PciRead32 (PCI_LIB_ADDRESS (FCH_EHCI1_BUS, FCH_EHCI1_DEV, 0, FCH_EHCI_REG90));
//  Value32 &= ~(BIT16+BIT17);
//  PciWrite32 (PCI_LIB_ADDRESS (FCH_EHCI1_BUS, FCH_EHCI1_DEV, 0, FCH_EHCI_REG90), Value32 | BIT18);
//  Value32 = PciRead32 (PCI_LIB_ADDRESS (FCH_EHCI1_BUS, FCH_EHCI1_DEV, 0, FCH_EHCI_REG90));
//
//  //
//  // 3. Set Bit27 on EHCI BAR 0xA0 (EOR+80h) and then EHCI BAR 0xA0 [31:28] gonna mapping to HCSPARAMS[23:20].
//  //
//  Value32 = 0xFC0000A0;
//  Ptr = (UINT32 *)(UINTN)Value32;
//
//  *Ptr = (*Ptr | BIT27);
//
//  //
//  // 4. Change the USB debug port number (One-based. 1 = Port0, 2 = Port1...)
//  //
//  *Ptr &= ~(BIT28+BIT29+BIT30+BIT31);
//  *Ptr |= (2 << 28);  // USB debug port = Port 0
//
//  //
//  // 5. Disable MMIO decode on B:0x00/D:0x12/F:0x00 (EHCI0)
//  //
//  PciWrite32 (PCI_LIB_ADDRESS (FCH_EHCI1_BUS, FCH_EHCI1_DEV, 0, FCH_EHCI_REG04), 0x00);
//
//  //
//  // 6. Revert BAR0 B:0x00/D:0x12/F:0x00 (EHCI0) to 0x00000000
//  //
//  PciWrite32 (PCI_LIB_ADDRESS (FCH_EHCI1_BUS, FCH_EHCI1_DEV, 0, FCH_EHCI_REG10), 0xFC000000);
  PlatformHookSerialPortInitialize ();
  return EFI_SUCCESS;
}
