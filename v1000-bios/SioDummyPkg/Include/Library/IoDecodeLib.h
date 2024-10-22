/** @file
 This file contains the interface of IoDecodeLib

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

#ifndef _IO_DECODE_LIB_H_
#define _IO_DECODE_LIB_H_

/**
  Decode a given IO space of given address and Length

  This is based on the Chip vendor which will be implemented differently.

  @param[in]       Address                  IO base Address
  @param[in]       Length                   The size of IO Space

  @retval          EFI_SUCCESS              Function complete successfully.
  @retval          EFI_INVALID_PARAMETER    One or More of the parameters are invalid values.
  @retval          EFI_OUT_OF_RESOURCES     There are not enough resources available to set IO decode.
  @retval          EFI_UNSUPPORTED          Null library.

**/
EFI_STATUS
IoDecodeControl (
  IN UINT16    Address,
  IN UINT16    Length
  );

#endif