/** @file
  Header file for H2O Console Redirection Callback

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

#ifndef _CR_CALLBACK_H_
#define _CR_CALLBACK_H_

EFI_STATUS
EFIAPI
CrDriverCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL     *This,
  IN EFI_BROWSER_ACTION                       Action,
  IN EFI_QUESTION_ID                          QuestionId,
  IN UINT8                                    Type,
  IN EFI_IFR_TYPE_VALUE                       *Value,
  OUT EFI_BROWSER_ACTION_REQUEST              *ActionRequest
  );

EFI_STATUS
EFIAPI
CrSubmit (
  VOID
  );

#endif
