/** @file
 DXE Chipset Services Library.

 This file contains only one function that is DxeCsSvcSataComReset().
 The function DxeCsSvcSataComReset() use chipset services to reset specified SATA port.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/TimerLib.h>
#include <Library/PciLib.h>

#include <Fch.h>

/**
 This routine issues SATA COM reset on the specified SATA port 

 @param[in]         PortNumber          The SATA port number to be reset

 @retval            EFI_SUCCESS         The SATA port has been reset successfully
 @retval            EFI_DEVICE_ERROR    1.SATA controller isn't in IDE, AHCI or RAID mode.
                                        2.Get error when getting PortNumberMapTable.
*/
EFI_STATUS
SataComReset (
  IN  UINTN         PortNumber
  )
{
  UINT8             BusB;
  UINT32            PortAddress;
  
  //
  // Internal PCIe GPP Bridge 0 to Bus B. (PPR.1 Table 13) Sata
  //
  BusB = PciRead8 (PCI_LIB_ADDRESS (0, 8, 2, 0x19));

  PortAddress = PciRead32 (PCI_LIB_ADDRESS (BusB, 0, 0, FCH_SATA_REG24)) & 0xFFFFFC00;

  *((UINT8 *)(PortAddress + FCH_SATA_BAR5_REG12C + PortNumber * 0x80)) |= 0x01;
  //
  // Software should leave this field set to 1h for a minimum of 1 millisecond 
  // to ensure COMRESET is sent on the interface.
  //
  MicroSecondDelay (1000);
  *((UINT8 *)(PortAddress + FCH_SATA_BAR5_REG12C + PortNumber * 0x80)) &= 0xF0;

  return EFI_SUCCESS;
}
