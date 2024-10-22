/** @file

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _MULTI_LAYOUT_H
#define _MULTI_LAYOUT_H

extern CHAR16 *mDialogPanelChilds;
extern CHAR16 *mScreenPanelChilds;
extern CHAR16 *mOwnerDrawPanelChilds;
extern CHAR16 *mFormTitlePanelChilds;
extern CHAR16 *mHelpTextPanelChilds;
extern CHAR16 *mHotkeyItemChilds;
extern CHAR16 *mFrontPageItemChilds;
extern CHAR16 *mSetupMenuItemChilds;
extern CHAR16 *mSubtitleChilds;
extern CHAR16 *mH2OCheckBoxOpChilds;
extern CHAR16 *mStatementChilds;
extern CHAR16 *mH2OTextOpChilds;
extern CHAR16 *mCommonDialogChilds;
extern CHAR16 *mCommonDialogWithHelpChilds;
extern CHAR16 *mDialogWithoutSendFormChilds;
extern CHAR16 *mHelpDialogChilds;
extern CHAR16 *mConfirmPageDialogChilds;
extern CHAR16 *mOneOfDialogChilds;
extern CHAR16 *mOneOfDialogWithoutSendFormChilds;
extern CHAR16 *mOneOfOptionChilds;
extern CHAR16 *mOneOfOptionChildsWithoutSendForm;
extern CHAR16 *mNumericDialogChilds;
extern CHAR16 *mNumericDialogWithoutSendFormChilds;
extern CHAR16 *mPasswordDialogChilds;
extern CHAR16 *mConfirmPasswordDialogChilds;
extern CHAR16 *mStringDialogChilds;
extern CHAR16 *mOrderedListDialogChilds;
extern CHAR16 *mOrderedListOptionChilds;
extern CHAR16 *mTimeDialogChilds;
extern CHAR16 *mDateDialogChilds;
extern CHAR16 *mScrollbarChilds;
extern CHAR16 *mSwitchChilds;

INT32
GetStatementHeight (
  IN VOID                                 *Statement
  );

#endif

