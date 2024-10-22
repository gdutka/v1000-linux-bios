/** @file

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_VERB_TABLE_CONFIG_LIB_H_
#define _PEI_VERB_TABLE_CONFIG_LIB_H_

#include <Ppi/VerbTable.h>




EFI_STATUS
GetVerbTable (
  OUT COMMON_CHIPSET_AZALIA_VERB_TABLE      **VerbTableHeaderDataAddress
  );

#endif

