/** @file
 IPMI Syn Bmc library header file.

 This file contains functions prototype that can easily access FUR data via
 using H2O IPMI FRU protocol.

;******************************************************************************
;* Copyright (c) 2016 - 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _IPMI_SYNC_BMC_LIB_2_H_
#define _IPMI_SYNC_BMC_LIB_2_H_

/**
 This function can sync power policy to and from BMC  

 @param[in]         PowerPolicy         Power policy. Reference IPMI_POWER_POLICY.
 @param[in]         SetToBmc            Set to Bmc or not.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI.
                                        FALSE: This command will send to BMC.

 @retval EFI_SUCCESS                    Update platform policy success.
 @return EFI_ERROR (Status)             Implement code execute status.
*/
EFI_STATUS
IpmiSyncBmcPowerPolicy2 (
  UINT8             *PowerPolicy,
  BOOLEAN           SetToBmc,
//[-start-161121-IB09330314-add]//
  IN  BOOLEAN       IsHeci
//[-end-161121-IB09330314-add]//
  );

#endif

