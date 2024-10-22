/** @file
  Content file contains function definitions for ApcbTokensUpdate.

;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#ifndef _APCB_TOKENS_UPDATE_H_
#define _APCB_TOKENS_UPDATE_H_

//[-start-221013-IB14860855-add]//
extern EFI_GUID gAodSystemConfigurationGuid;

#define AOD_SETUP_VARIABLE_NAME  L"AOD_SETUP"
//[-end-221013-IB14860855-add]//

#define STRING_COUNT_MAX                      0x80
#define VARIABLE_BUFFER_TO_DATA(p)            ((VOID *)(((UINT8 *)(&p->VariableName[0])) + (((p)->VariableNameLength + 1) * sizeof(CHAR16))))

#define field_size_of(s,m)                    (sizeof(((s *)0)->m))
#define RESULT_SIZE(c)                        (field_size_of(VATS_RESULT_LIST_STRUCTURE ,ListCount) + c * sizeof(RESULT_LIST_RESULT_DATA) )

enum {
  UveVariableConfirm = 0x52,
  UveBootInfoService = 0x53,
};

/**
  Callback function for the modifying of APCB tokens.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER          Rsi/Rcx is 0, variable name or GUID is mismatched with variable.

**/
EFI_STATUS
EFIAPI
ApcbSyncCallback (
  VOID
);

#endif
