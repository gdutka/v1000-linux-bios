/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcSetPlatformHardwareSwitch().
 The function PeiCsSvcSetPlatformHardwareSwitch() use chipset services to feedback 
 its proprietary settings of the hardware switches.

***************************************************************************
* Copyright (c) 2013 - 2017, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/ChipsetConfigLib.h>

#include <Guid/PlatformHardwareSwitch.h>

#include <ChipsetSetupConfig.h>

/**
  To feedback its proprietary settings of the hardware switches.
  if you wants it to be not auto detected while it is set to 'PORT_ENABLE'. 
  That means, you has 3 choices to a port of PCI-e Ports - 'PORT_DISABLE', 'PCIE_PORT_AUTO', 
  and 'PORT_ENABLE'. For Usb and Sata ports, only two options are available -'PORT_DISABLE' and 'PORT_ENABLE'.

  @param[out]  PlatformHardwareSwitch    A Pointer to PLATFORM_HARDWARE_SWITCH.

  @retval      EFI_SUCCESS                Set hardware switch success.
  @retval      EFI_NOT_FOUND              Locate ReadOnlyVariablePpi failed.
**/
EFI_STATUS
SetPlatformHardwareSwitch (
  OUT PLATFORM_HARDWARE_SWITCH          **PlatformHardwareSwitchDptr
  )
{
  //
  // Not use this fucntion due to chipset service is too late.
  // Move the function to PEIM [EarlySetPlatformHardwareSwitchPei]
  //

  return EFI_SUCCESS;
}

