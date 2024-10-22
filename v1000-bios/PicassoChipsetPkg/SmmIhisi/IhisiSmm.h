/** @file
  This driver provides IHISI interface in SMM mode

;*******************************************************************************
;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _IHISI_SMM_H_
#define _IHISI_SMM_H_

#include <Uefi.h>
#include <YangtzeReg.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseOemSvcKernelLib.h>
#include <Library/SmmOemSvcChipsetLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/H2OIhisi.h>
#include <Library/BaseLib.h>

#include <H2OIhisi.h>
#include "Fbts.h"
#include "ChipsetSecureFlash.h"

extern H2O_IHISI_PROTOCOL              *mH2OIhisi;

/**
  Register IHISI sub function if SubFuncTable CmdNumber/AsciiFuncGuid define in PcdIhisiRegisterTable list.

  @param[out] SubFuncTable        Pointer to ihisi register table.
  @param[out] TableCount          SubFuncTable count

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
RegisterIhisiSubFunction (
  IHISI_REGISTER_TABLE         *SubFuncTable,
  UINT16                        TableCount
  );



#endif
