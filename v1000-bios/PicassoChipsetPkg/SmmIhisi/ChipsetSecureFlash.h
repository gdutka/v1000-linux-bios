/** @file
  This driver provides IHISI interface in SMM mode

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _IHISI_CHIPSET_SECURE_FLASH_H_
#define _IHISI_CHIPSET_SECURE_FLASH_H_
#include "IhisiSmm.h"
#include <H2OIhisi.h>
#include <ChipsetSetupConfig.h>
#include <Library/FdSupportLib.h>
#include <Protocol/SmmFwBlockService.h>
#include <Library/VariableLib.h>

#define SECURE_FLASH_SIGNATURE        SIGNATURE_32 ('$', 'S', 'E', 'C')
#define EXTEND_FLAG_INPUT_SIGNATURE   SIGNATURE_32 ('$', 'E', 'X', 'T')
#define EXTEND_FLAG_OUTPUT_SIGNATURE  SIGNATURE_32 ('$', 'E', 'X', 'O')

EFI_STATUS
ChipsetSecureFlashInit (
  VOID
  );
#endif
