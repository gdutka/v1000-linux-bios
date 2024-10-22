/** @file
 H2O IPMI SEL Data library header file.

 This file contains functions prototype that can easily retrieve SEL Data via
 using H2O IPMI SEL Data protocol.

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _IPMI_SEL_DATA2_H_
#define _IPMI_SEL_DATA2_H_

#include <Library/IpmiLibDefine.h>

/**
 Get Sel Data of SelId.

 @param[in]         SelId               The SEL record ID to get which sel data.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI.
                                        FALSE: This command will send to BMC.
 @param[out]        SelData             A buffer to IPMI_GET_SEL_ENTRY_RESPONSE.

 @retval EFI_SUCCESS                    Get SEL data success.
 @retval EFI_UNSUPPORTED                Get SEL data fail or speificed Id does not exist.
*/
EFI_STATUS
IpmiLibGetSelData2 (
  IN  UINT16                            SelId,
  IN  BOOLEAN                           IsHeci,
  OUT IPMI_GET_SEL_ENTRY_RESPONSE       *SelData
  );


/**
 Get All Sel entry data.

 @param[in]         IsHeci              TRUE : This command will send to IE by HECI.
                                        FALSE: This command will send to BMC.
 @param[out]        SelCount            Number of log entries in SEL.
 @param[out]        SelData             A IPMI_GET_SEL_ENTRY_RESPONSE array to store all IPMI_GET_SEL_ENTRY_RESPONSE.

 @retval EFI_SUCCESS                    Get SEL data success.
 @retval EFI_UNSUPPORTED                Get SEL data fail.
*/
EFI_STATUS
IpmiLibGetAllSelData2 (
  IN  BOOLEAN                           IsHeci,
  OUT UINT16                            *SelCount,
  OUT IPMI_GET_SEL_ENTRY_RESPONSE       **SelData
  );


#endif

