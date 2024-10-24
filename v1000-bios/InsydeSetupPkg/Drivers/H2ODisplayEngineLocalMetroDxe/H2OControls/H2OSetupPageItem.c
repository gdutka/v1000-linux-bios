/** @file
  UI Common Controls

;******************************************************************************
;* Copyright (c) 2014 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "H2OControls.h"
#include "H2ODisplayEngineLocalMetro.h"
#include "MetroUi.h"

STATIC H2O_SETUP_PAGE_ITEM_CLASS    *mSetupPageItemClass = NULL;
#define CURRENT_CLASS              mSetupPageItemClass

#if FixedPcdGet32(PcdH2OLmdeMultiLayout) == 0
CHAR16 *mSubtitleChilds = L""
  L"<VerticalLayout height='35' width='match_parent'>"
    L"<Label name='OptionPrompt' text-align='singleline' padding='0,0,0,20' font-size='20' textcolor='0xFF4D4D4D' text-overflow='ellipsis'/>"
    L"<Control height='1' width='match_parent' background-image='@OptionBkg' background-image-style='stretch'/>"
  L"</VerticalLayout>";

CHAR16 *mH2OCheckBoxOpChilds = L""
  L"<Texture float='true' background-image='@OptionBkg' name='OptionBkg' background-image-style='stretch'/>"
  L"<HorizontalLayout>"
    L"<Control width='60' name='OptionImagePadding'>"
      L"<Texture name='OptionImage' background-image-style='stretch'/>"
    L"</Control>"
    L"<Label text-align='singleline' textcolor='0xFF666666' font-size='21' name='OptionPrompt' text-overflow='ellipsis'/>"
    L"<HorizontalLayout padding='17,0,17,0' width='120' height='70'>"
      L"<Switch switchcolor='@menucolor' name='CheckBox'/>"
    L"</HorizontalLayout>"
    L"<Label width='30' />"
  L"</HorizontalLayout>";

CHAR16 *mStatementChilds = L""
  L"<Texture float='true' background-image='@OptionBkg' name='OptionBkg' background-image-style='stretch'/>"
  L"<HorizontalLayout>"
    L"<Control width='60' name='OptionImagePadding'>"
      L"<Texture name='OptionImage' background-image-style='stretch'/>"
    L"</Control>"
    L"<HorizontalLayout name='OptionLayout'>"
      L"<Label text-align='singleline|left' textcolor='0xFF666666' font-size='21' name='OptionPrompt' text-overflow='ellipsis'/>"
      L"<Label text-align='singleline' name='OptionSparator' width='10'/>"
      L"<Label text-align='singleline|right' width='160' textcolor='0xFF666666' font-size='21' name='OptionValue' text-overflow='ellipsis'/>"
      L"<Label name='OptionEnd' text='>' text-align='singleline|right' text-align='center' width='20' font-size='21' textcolor='0xFF666666'/>"
      L"<Label text-align='singleline' name='EndSparator' width='20'/>"
    L"</HorizontalLayout>"
  L"</HorizontalLayout>";

CHAR16 *mH2OTextOpChilds = L""
  L"<HorizontalLayout child-padding='2' width='match_parent' min-height='35' height='wrap_content'>"
    L"<HorizontalLayout child-padding='2' float='true' width='match_parent' height='match_parent'>"
      L"<Texture scale9grid='1,1,1,1' background-image='@SetupMenuTextOpBkg' name='OptionPromptBackground' width='258' height='match_parent'/>"
      L"<Texture scale9grid='1,1,1,1' background-image='@SetupMenuTextOpBkg' name='OptionValueBackground' height='match_parent'/>"
    L"</HorizontalLayout>"
    L"<Label name='OptionPrompt' min-height='35' height='wrap_content' padding='7,0,7,60' width='258' font-size='20' textcolor='0xFF4D4D4D'/>"
    L"<Label name='OptionValue' text-align='center' min-height='35' height='wrap_content' padding='7,0,7,0' width='258' font-size='20' textcolor='@menucolor'/>"
    L"<Texture float='true' background-color='@menucolor' pos='30,11,38,19'/>"
  L"</HorizontalLayout>";

INT32
GetStatementHeight (
  IN VOID                                 *Statement
  )
{
  H2O_FORM_BROWSER_S                      *St;

  if (Statement == NULL) {
    return 35;
  }

  St = (H2O_FORM_BROWSER_S *)Statement;
  if (St->Operand == EFI_IFR_SUBTITLE_OP ||
    St->Operand == EFI_IFR_TEXT_OP) {
    return 35;
  }

  return 70;
}
#endif

#define  OPTION_FONT_SIZE         21
#define  OPTION_IMAGE_SIZE        25

#define  OPTION_LEFT_PADDING      60
#define  OPTION_MIN_PROMPT_WIDTH  200
#define  OPTION_VALUE_WIDTH       105
#define  OPTION_RIGHT_PADDING     30

EFI_STATUS
MarqueeByName (
  IN UI_CONTROL             *Control,
  IN CHAR16                 *ChildName,
  IN CHAR16                 *Value
  )
{
  UI_CONTROL                *Child;

  Child = UiFindChildByName (Control, ChildName);
  if (Child == NULL) {
    return EFI_NOT_FOUND;
  }

  UiSetAttribute (Child, L"marquee", Value);
  return EFI_SUCCESS;
}

VOID
UpdateSetupPageItem (
  UI_CONTROL                    *Control
  )
{
  H2O_SETUP_PAGE_ITEM           *This;
  H2O_FORM_BROWSER_S            *Statement;
  CHAR16                        Str[20];
  UI_CONTROL                    *Child;
  UINT32                        Index;
  INTN                          Result;
  EFI_STATUS                    Status;
  INT32                         Height;
  UI_CONTROL                    *CheckBox;
  UI_LABEL                      *Label;
  CHAR16                        ValueTextColor[20];

  This = (H2O_SETUP_PAGE_ITEM *) Control;
  if (This->Statement == NULL) {
    return ;
  }

  Statement = This->Statement;
  ASSERT (Statement != NULL);
  if (Statement == NULL) {
    return;
  }

  //
  // Get Value Text color
  //
  Label = (UI_LABEL *)UiFindChildByName (This, L"ValueText");
  UnicodeSPrint (ValueTextColor, sizeof (ValueTextColor), L"%s", L"@menucolor");
  if (Label != NULL) {
    if (Label->TextColor == 0) {
      UnicodeSPrint (ValueTextColor, sizeof (ValueTextColor), L"%s", L"@menulightcolor");
    } else {
      UnicodeSPrint (ValueTextColor, sizeof (ValueTextColor), L"0x%08x", Label->TextColor);
    }
  }

  //
  // Add Statement image if need
  //
  Child = UiFindChildByName (This, L"OptionImage");
  if (Child != NULL && Statement->Image != NULL) {
    Height = GetStatementHeight (Statement);
    UnicodeSPrint (Str, sizeof (Str), L"0x%p", Statement->Image);
    UiSetAttribute (Child, L"background-image", Str);
    UnicodeSPrint (
      Str,
      sizeof (Str),
      L"%d,%d,%d,%d",
      (Height - OPTION_IMAGE_SIZE) / 2 + (Height - OPTION_IMAGE_SIZE) % 2,
      17,
      (Height - OPTION_IMAGE_SIZE) / 2,
      18
      );
    Child = UiFindChildByName (This, L"OptionImagePadding");
    UiSetAttribute (Child, L"padding", Str);
  }

  Child = UiFindChildByName (This, L"RefImage");
  if (Child != NULL && Statement->Operand == EFI_IFR_REF_OP) {
    UiSetAttribute (Child, L"visibility", L"true");
  }

  //
  // Adjust text and text color
  //
  if (Statement->Prompt != NULL) {
    Child = UiFindChildByName (This, L"OptionPrompt");
    UiSetAttribute (Child, L"text", Statement->Prompt);
  }

  if (Statement->Operand == EFI_IFR_CHECKBOX_OP) {
    CheckBox = UiFindChildByName (This, L"CheckBox");

    if (Statement->HiiValue.Value.b) {
      UiSetAttribute (CheckBox, L"checkboxvalue", L"true");
    } else {
      UiSetAttribute (CheckBox, L"checkboxvalue", L"false");
    }

    if (Statement->Selectable) {
      UiSetAttribute (CheckBox, L"disabled", L"false");
      if (Statement->HiiValue.Value.b) {
        UiSetAttribute (CheckBox, L"switchcolor", L"@menucolor");
        UiSetAttribute (CheckBox, L"switch-textcolor", L"0xFFFFFFFF");
      } else {
        UiSetAttribute (CheckBox, L"switchcolor", L"gray");
        UiSetAttribute (CheckBox, L"switch-textcolor", L"@menucolor");
        if (PcdGet32(PcdH2OLmdeMultiLayout) == 1) {
          UiSetAttribute (CheckBox, L"switch-textcolor", L"0xFF008000");
        }
      }
    } else {
      UiSetAttribute (CheckBox, L"switchcolor", L"gray");
      UiSetAttribute (CheckBox, L"switch-textcolor", L"0xFF666666");
      if (PcdGet32(PcdH2OLmdeMultiLayout) == 1) {
        UiSetAttribute (CheckBox, L"switch-textcolor", L"0xFF333333");
      }
      UiSetAttribute (CheckBox, L"disabled", L"true");
    }

  } else if (Statement->Operand == EFI_IFR_TEXT_OP) {
    if (Statement->TextTwo != NULL && Statement->TextTwo[0] != '\0') {
      Child = UiFindChildByName (This, L"OptionValue");
      UiSetAttribute (Child, L"text", Statement->TextTwo);
    } else {
      Child = UiFindChildByName (This, L"OptionPrompt");
      UiSetAttribute (Child, L"width", L"0");
      UiSetAttribute (Child, L"height", L"0");
      UiSetAttribute (Child, L"text-align", L"singleline|left");
      UiSetAttribute (Child, L"text-overflow", L"ellipsis");

      Child = UiFindChildByName (This, L"OptionValue");
      UiSetAttribute (Child, L"visibility", L"false");

      Child = UiFindChildByName (This, L"OptionPromptBackground");
      UiSetAttribute (Child, L"width", L"0");

      Child = UiFindChildByName (This, L"OptionValueBackground");
      UiSetAttribute (Child, L"visibility", L"false");
    }
  } else if (Statement->NumberOfOptions != 0) {
    for (Index = 0; Index < Statement->NumberOfOptions; Index++) {
      Status = CompareHiiValue (&Statement->Options[Index].HiiValue, &Statement->HiiValue, &Result);
      if (!EFI_ERROR (Status) && Result == 0) {
        Child = UiFindChildByName (This, L"OptionValue");
        UiSetAttribute (Child, L"text", Statement->Options[Index].Text);
        if (Statement->Selectable) {
          UiSetAttribute (Child, L"textcolor", ValueTextColor);
          Child = UiFindChildByName (This, L"OptionEnd");
          UiSetAttribute (Child, L"textcolor", ValueTextColor);
        }
        break;
      }
    }
  } else if (Statement->Operand == EFI_IFR_NUMERIC_OP) {
    Child = UiFindChildByName (This, L"OptionValue");
    if ((Statement->Flags & EFI_IFR_DISPLAY) == EFI_IFR_DISPLAY_UINT_HEX) {
      UnicodeSPrint (Str, sizeof (Str), L"0x%lX", Statement->HiiValue.Value.u64);
    } else {
      UnicodeSPrint (Str, sizeof (Str), L"%ld", Statement->HiiValue.Value.u64);
    }
    UiSetAttribute (Child, L"text", Str);

    if (Statement->Selectable) {
      UiSetAttribute (Child, L"textcolor", ValueTextColor);
      Child = UiFindChildByName (This, L"OptionEnd");
      UiSetAttribute (Child, L"textcolor", ValueTextColor);
    }
  } else if (Statement->Operand == EFI_IFR_ACTION_OP) {
    Child = UiFindChildByName (This, L"OptionValue");
    if (Child != NULL) {
      if (Statement->TextTwo != NULL && Statement->TextTwo[0] != '\0') {
        UiSetAttribute (Child, L"text", Statement->TextTwo);
        if (Statement->Selectable) {
          UiSetAttribute (Child, L"textcolor", ValueTextColor);
        } else {
          UiSetAttribute (Child, L"textcolor", L"0xFF666666");
        }
      } else {
        UiSetAttribute (Child, L"visibility", L"false");
      }
      Child = UiFindChildByName (This, L"OptionEnd");
      UiSetAttribute (Child, L"visibility", L"false");
    }
  } else if (Statement->Operand == EFI_IFR_STRING_OP) {
    Child = UiFindChildByName (This, L"OptionValue");
    if (Child != NULL && Statement->HiiValue.Buffer != NULL) {
      UiSetAttribute (Child, L"text", (CHAR16 *)Statement->HiiValue.Buffer);
    }

    if (Statement->Selectable) {
      if (Child != NULL) {
        UiSetAttribute (Child, L"textcolor", ValueTextColor);
      }
      Child = UiFindChildByName (This, L"OptionEnd");
      UiSetAttribute (Child, L"textcolor", ValueTextColor);
    }
  } else {
    Child = UiFindChildByName (This, L"OptionValue");
    if (Child != NULL) {
      UiSetAttribute (Child, L"visibility", L"false");
      Child = UiFindChildByName (This, L"OptionEnd");
      UiSetAttribute (Child, L"visibility", L"false");
    }
  }
}

BOOLEAN
EFIAPI
H2OSetupPageItemSetAttribute (
  UI_CONTROL             *Control,
  CHAR16                 *Name,
  CHAR16                 *Value
  )
{
  H2O_SETUP_PAGE_ITEM    *This;
  EFI_STATUS             Status;
  UI_CONTROL             *Switch;

  This = (H2O_SETUP_PAGE_ITEM *) Control;

  if (StrCmp (Name, L"statement") == 0) {
    CONTROL_CLASS (This)->RemoveAllChild (Control);

    This->Statement = (H2O_FORM_BROWSER_S *)(UINTN) StrToUInt (Value, 16, &Status);
    if (This->Statement->Operand == EFI_IFR_SUBTITLE_OP) {
      CONTROL_CLASS_SET_STATE(Control, UISTATE_DISABLED, 0);
      XmlCreateControl (mSubtitleChilds, Control);
    } else if (This->Statement->Operand == EFI_IFR_CHECKBOX_OP) {
      XmlCreateControl (mH2OCheckBoxOpChilds, Control);
      Switch = UiFindChildByName (This, L"CheckBox");
      SetWindowLongPtr (Switch->Wnd, GWLP_USERDATA, This->Statement->StatementId);
    } else if (This->Statement->Operand == EFI_IFR_TEXT_OP) {
      XmlCreateControl (mH2OTextOpChilds, Control);
    } else {
      switch (This->Statement->Operand) {

      case EFI_IFR_REF_OP:
      case EFI_IFR_ACTION_OP:
      case EFI_IFR_PASSWORD_OP:
      case EFI_IFR_NUMERIC_OP:
      case EFI_IFR_ONE_OF_OP:
      case EFI_IFR_TIME_OP:
      case EFI_IFR_DATE_OP:
      case EFI_IFR_ORDERED_LIST_OP:
      case EFI_IFR_RESET_BUTTON_OP:
      case EFI_IFR_STRING_OP:
        XmlCreateControl (mStatementChilds, Control);
        break;

      default:
        DEBUG ((EFI_D_ERROR, "Unsupported opcode : %d\n", This->Statement->Operand));
        This->Statement = NULL;
        ASSERT (FALSE);
        break;
      }
    }
    Control->MinSize.cy = GetStatementHeight (This->Statement);
    UpdateSetupPageItem (Control);
    CONTROL_CLASS_INVALIDATE (This);
    return TRUE;
  }

  return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);

}

VOID
EFIAPI
H2OSetupPageItemSetState (
  UI_CONTROL                    *Control,
  UI_STATE                      SetState,
  UI_STATE                      ClearState
  )
{
  STATIC UI_CONTROL             *OldFocusedControl = NULL;
  UI_CONTROL                    *Child;

  PARENT_CONTROL_CLASS(CURRENT_CLASS)->SetState (Control, SetState, ClearState);

  if ((SetState & UISTATE_FOCUSED) == UISTATE_FOCUSED) {
    if (Control != OldFocusedControl) {
      MarqueeByName (Control, L"OptionPrompt", L"true");
      MarqueeByName (Control, L"OptionValue",  L"true");
      OldFocusedControl = Control;

      Child = UiFindChildByName (Control, L"HotBkg");
      if (Child != NULL) {
        UiSetAttribute (Child, L"visibility", L"true");
      }
    }
  } else if ((ClearState & UISTATE_FOCUSED) == UISTATE_FOCUSED) {
    if (Control == OldFocusedControl) {
      MarqueeByName (Control, L"OptionPrompt", L"false");
      MarqueeByName (Control, L"OptionValue",  L"false");
      OldFocusedControl = NULL;

      Child = UiFindChildByName (Control, L"HotBkg");
      if (Child != NULL) {
        UiSetAttribute (Child, L"visibility", L"false");
      }
    }
  }
}

LRESULT
EFIAPI
H2OSetupPageItemProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  H2O_SETUP_PAGE_ITEM      *This;
  UI_CONTROL               *Control;

  This = (H2O_SETUP_PAGE_ITEM *) GetWindowLongPtr (Hwnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *)This;

  switch (Msg) {

  case WM_CREATE:
    This = (H2O_SETUP_PAGE_ITEM *) AllocateZeroPool (sizeof (H2O_SETUP_PAGE_ITEM));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    Control->FixedSize.cy = WRAP_CONTENT;
    Control->MinSize.cy = 35;
    break;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_BKCOLOR || WParam == PAINT_STATUSIMAGE) {
      break;
    }
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

    if (WParam == PAINT_ALL &&
        (CONTROL_CLASS_GET_STATE (This) & (UISTATE_SELECTED | UISTATE_FOCUSED)) == (UISTATE_SELECTED | UISTATE_FOCUSED)) {
      COLORREF                  Color;
      UI_MANAGER                *Manager;
      RECT                      Rc;
      HDC                       Hdc;

      if (PcdGet32(PcdH2OLmdeMultiLayout) == 0) {
        Color = 0xFFFF0000;
      } else {
        Color = 0xFFFFFFFF;
      }
      Manager = Control->Manager;
      Hdc     = Manager->PaintDC;

      Manager->GetControlRect (Manager, Control, &Rc);

      SelectObject (Hdc, GetStockObject (PS_NULL));
      SelectObject (Hdc, GetStockObject (DC_PEN));
      SetDCPenColor (Hdc, Color);
      Rectangle (Hdc, Rc.left, Rc.top, Rc.right, Rc.bottom);
   }
   break;

  case WM_NCHITTEST:
    return HTTRANSPARENT;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

H2O_SETUP_PAGE_ITEM_CLASS *
EFIAPI
GetSetupPageItemClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"SetupPageItem", (UI_CONTROL_CLASS *) GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = H2OSetupPageItemProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = H2OSetupPageItemSetAttribute;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetState     = H2OSetupPageItemSetState;

  return CURRENT_CLASS;
}


