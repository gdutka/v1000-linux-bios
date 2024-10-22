/** @file
 DXE Chipset Services Library.
	
 This file contains only one function that is DxeCsSvcSetSmbiosMemoryData().
 The function DxeCsSvcSetSmbiosMemoryData() use chipset services to collection
 information of memory and set to SMBIOS.
	
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

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <CommonSmbiosMemory.h>

/**
 To collection information of memory and set to SMBIOS.

 @param[in, out]    ChipsetSmbiosMemData Point to SMBIOS memory data

 @retval            EFI_SUCCESS          This function always return successfully.
*/
EFI_STATUS
SetSmbiosMemoryData (
  IN OUT H2O_SMBIOS_MEM_CONFIG_DATA        *ChipsetSmbiosMemData
  )
{
  return EFI_SUCCESS;
}