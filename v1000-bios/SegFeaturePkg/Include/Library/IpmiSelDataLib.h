/** @file
 H2O IPMI SEL Data library header file.

 This file contains functions prototype that can easily retrieve SEL Data via
 using H2O IPMI SEL Data protocol.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _IPMI_SEL_DATA_H_
#define _IPMI_SEL_DATA_H_


//[-start-171108-IB09330400-modify]//
#include <Library/IpmiLibDefine.h>
//[-end-171108-IB09330400-modify]//

/**
 Get Sel Data of SelId.

 @param[in]         SelId               The SEL record ID to get which sel data.
 @param[out]        SelData             A buffer to IPMI_GET_SEL_ENTRY_RESPONSE.

 @retval EFI_SUCCESS                    Get SEL data success.
 @retval EFI_UNSUPPORTED                Get SEL data fail or speificed Id does not exist.
*/
EFI_STATUS
IpmiLibGetSelData (
  IN  UINT16                            SelId,
  OUT IPMI_GET_SEL_ENTRY_RESPONSE       *SelData
  );


/**
 Get All Sel entry data.

 @param[out]        SelCount            Number of log entries in SEL.
 @param[out]        SelData             A IPMI_GET_SEL_ENTRY_RESPONSE array to store all IPMI_GET_SEL_ENTRY_RESPONSE.

 @retval EFI_SUCCESS                    Get SEL data success.
 @retval EFI_UNSUPPORTED                Get SEL data fail.
*/
EFI_STATUS
IpmiLibGetAllSelData (
  OUT UINT16                            *SelCount,
  OUT IPMI_GET_SEL_ENTRY_RESPONSE       **SelData
  );


#endif

