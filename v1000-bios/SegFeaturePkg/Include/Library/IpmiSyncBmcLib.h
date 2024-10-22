/** @file
 IPMI Syn Bmc library header file.

 This file contains functions prototype that can easily access FUR data via
 using H2O IPMI FRU protocol.

;******************************************************************************
;* Copyright (c) 2016 - 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _IPMI_SYNC_BMC_LIB_H_
#define _IPMI_SYNC_BMC_LIB_H_

//[-start-190419-IB09330600-modify]//
#include <Uefi/UefiBaseType.h>

/**
 This function can sync power policy to and from BMC  

 @param[in]         PowerPolicy         Power policy. Reference IPMI_POWER_POLICY.
 @param[in]         SetToBmc            Set to Bmc or not .

 @retval EFI_SUCCESS                    Update platform policy success.
 @return EFI_ERROR (Status)             Implement code execute status.
*/
EFI_STATUS
IpmiSyncBmcPowerPolicy (
  UINT8             *PowerPolicy,
  BOOLEAN           SetToBmc
  );

/**
 This is outdated and will be deleted and replaced by IpmiExternalLibGetPowerPolicy() 
 This function can get power policy that need to be set
 Chipset should call this function before setting StateAfterG3 to PCH

 @param[in out]       SetAfterG3On      in : Current caller's power policy.
                                        out: Return the power policy that need to be set.

 @retval EFI_SUCCESS                    Always return success.
*/
EFI_STATUS
IpmiSyncBmcPchControlPowerPolicy (
  IN OUT BOOLEAN        *SetAfterG3On
  );
//[-end-190419-IB09330600-modify]//

#endif

