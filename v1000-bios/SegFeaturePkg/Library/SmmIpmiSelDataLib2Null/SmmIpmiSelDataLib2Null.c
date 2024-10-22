/** @file
 H2O IPMI SEL Data library implement code.

 This c file contains SEL Data library instance in SMM phase.

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

#include <Library/IpmiSelDataLib.h>

/**
 Get Sel Data of SelId.

 @param[in]         SelId               The SEL record ID to get which sel data.
 @param[out]        SelData             A buffer to IPMI_GET_SEL_ENTRY_RESPONSE.

 @retval EFI_UNSUPPORTED                Function Unsupported..
*/
EFI_STATUS
IpmiLibGetSelData (
  IN  UINT16                            SelId,
  IN  BOOLEAN                           IsHeci,
  OUT IPMI_GET_SEL_ENTRY_RESPONSE       *SelData
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Get All Sel entry data.

 @param[out]        SelCount            Number of log entries in SEL.
 @param[out]        SelData             A IPMI_GET_SEL_ENTRY_RESPONSE array to store all IPMI_GET_SEL_ENTRY_RESPONSE.

 @retval EFI_UNSUPPORTED                Function Unsupported.
*/
EFI_STATUS
IpmiLibGetAllSelData (
  IN  BOOLEAN                           IsHeci,
  OUT UINT16                            *SelCount,
  OUT IPMI_GET_SEL_ENTRY_RESPONSE       **SelData
  )
{
  return EFI_UNSUPPORTED;
}

