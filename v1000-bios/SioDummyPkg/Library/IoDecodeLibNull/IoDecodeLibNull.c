/** @file
 Null library of the IO address decoding.

;******************************************************************************
;* Copyright 2017 Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include <Uefi.h>

/**
 Null library of the IO address decoding.

 This is based on the Chip vendor which will be implemented differently.

 @param[in]          Address            IO base Address
 @param[in]          Length             The size of IO Space

 @retval             EFI_SUCCESS                 Function complete successfully.
 @retval             EFI_INVALID_PARAMETER       One of the parameters has an invalid value.
 @retval             EFI_OUT_OF_RESOURCES        There are not enough resources available to set IO decode.
 @retval             EFI_UNSUPPORTED             Null library.
*/
EFI_STATUS
IoDecodeControl (
  IN UINT16    Address,
  IN UINT16    Length
  )
{
  return EFI_UNSUPPORTED;
}
