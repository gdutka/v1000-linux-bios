/** @file
  UI Common Controls

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "H2ODisplayEngineLocalMetro.h"
#include "H2OPanels.h"
#include "MetroUi.h"
#include <Guid/H2ODisplayEngineType.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/LayoutLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>

#define OWNER_DRAW_TIMER_ID 1
STATIC H2O_OWNER_DRAW_PANEL_CLASS        *mH2OOwnerDrawPanelClass = NULL;
#define CURRENT_CLASS                    mH2OOwnerDrawPanelClass
#define FORCE_REFRESH_PANEL               TRUE
#define NO_FORCE_REFRESH_PANEL            FALSE

#if FixedPcdGet32(PcdH2OLmdeMultiLayout) == 0
CHAR16 *mOwnerDrawPanelChilds = L""
  L"<Control>"
    L"<Texture name='OwnerDrawBackground' float='true' height='-1' width='-1' background-image='@OwnerDrawBkg' background-image-style='stretch' background-color='0xFF333333'/>"
    L"<HorizontalLayout padding='13,15,13,15' name='OwnerDrawList'>"
      L"<VerticalLayout width='190'>"
        L"<Control width='180' height='50' background-image='@OwnerDrawInsyde' background-color='0x0'/>"
        L"<Control width='180' height='2'/>"
        L"<Label textcolor='0xFFFFFFFF' font-size='9' background-color='0x0' text-align='singleline' text-overflow='ellipsis' name='OwnerDrawSystemInfoProcessor'/>"
        L"<Label textcolor='0xFFFFFFFF' font-size='9' background-color='0x0' text-align='singleline' text-overflow='ellipsis' name='OwnerDrawSystemInfoMemFrequency'/>"
        L"<Label textcolor='0xFFFFFFFF' font-size='9' background-color='0x0' text-align='singleline' text-overflow='ellipsis' name='OwnerDrawSystemInfoMemSize'/>"
      L"</VerticalLayout>"
      L"<Control/>"
      L"<HorizontalLayout padding='20,0,10,0' width='700'>"
        L"<HorizontalLayout width='240'>"
          L"<Control width='33' background-image='@OwnerDrawTime' background-color='0x0' height='35'/>"
          L"<VerticalLayout width='95'>"
            L"<Label textcolor='0xFFF2F2F2' font-size='18' background-color='0x0' name='OwnerDrawDateValue'/>"
            L"<Label textcolor='0xFF999999' font-size='18' background-color='0x0' name='OwnerDrawWeekdayValue'/>"
          L"</VerticalLayout>"
          L"<Label textcolor='0xFFFFFFFF' font-size='28' background-color='0x0' name='OwnerDrawTimeValue'/>"
        L"</HorizontalLayout>"
        L"<Control/>"
        L"<HorizontalLayout width='230'>"
          L"<Control width='21' background-image='@OwnerDrawTemperature' background-color='0x0' height='42'/>"
          L"<VerticalLayout width='120'>"
            L"<Label textcolor='0xFFF2F2F2' font-size='18' background-color='0x0' name='OwnerDrawMainboardName'/>"
            L"<Label textcolor='0xFF999999' font-size='18' background-color='0x0' name='OwnerDrawSystemName'/>"
          L"</VerticalLayout>"
          L"<Label textcolor='0xFFFFFFFF' font-size='43' background-color='0x0' name='OwnerDrawSystemTemperature'/>"
        L"</HorizontalLayout>"
        L"<Control/>"
        L"<HorizontalLayout width='230'>"
          L"<Control width='21' background-image='@OwnerDrawTemperature' background-color='0x0' height='42'/>"
          L"<VerticalLayout width='120'>"
            L"<Label textcolor='0xFFF2F2F2' font-size='18' background-color='0x0' name='OwnerDrawCpuName'/>"
            L"<Label textcolor='0xFF999999' font-size='18' background-color='0x0' name='OwnerDrawTemperatureName'/>"
          L"</VerticalLayout>"
          L"<Label textcolor='0xFFFFFFFF' font-size='43' background-color='0x0' name='OwnerDrawCpuTemperature'/>"
        L"</HorizontalLayout>"
      L"</HorizontalLayout>"
      L"<Control width='100' height='84' background-image='@OwnerDrawInsydeH2O' background-color='0x0'/>"
    L"</HorizontalLayout>"
  L"</Control>";
#endif

CHAR16  mWeekdayStr[7][4] = {L"SUN", L"MON", L"TUE", L"WED", L"THU", L"FRI", L"SAT"};
UINT8 mOldSecond = 60;

CHAR16 *
GetWeekdayStr (
  IN EFI_TIME                             *Time
  )
{
  INTN                                    Adjustment;
  INTN                                    Month;
  INTN                                    Year;
  INTN                                    Weekday;

  ASSERT (Time != NULL);

  Adjustment = (14 - Time->Month) / 12;
  Month      = Time->Month + 12 * Adjustment - 2;
  Year       = Time->Year - Adjustment;

  Weekday = (Time->Day + (13 * Month - 1) / 5 + Year + Year / 4 - Year / 100 + Year / 400) % 7;

  return mWeekdayStr[Weekday];
}

/**
  Acquire the string associated with the Index from smbios structure and return it.
  The caller is responsible for free the string buffer.

  @param    OptionalStrStart  The start position to search the string
  @param    Index             The index of the string to extract
  @param    String            The string that is extracted

  @retval   EFI_SUCCESS            Get index string successfully.
  @retval   EFI_INVALID_PARAMETER  Index is zero. It is invalid value.
  @retval   EFI_ABORTED            Get missing string fail .
  @retval   EFI_OUT_OF_RESOURCES   Allocate memory fail.
  @retval   Other                  Get setup browser data fail.

**/
EFI_STATUS
GetOptionalStringByIndex (
  IN  CHAR8                        *OptionalStrStart,
  IN  UINT8                        Index,
  OUT CHAR16                       **String
  )
{
  UINTN                                 StrSize;
  CHAR16                                *StringBuffer;

  if (Index == 0) {
    return EFI_INVALID_PARAMETER;
  }

  StrSize = 0;
  do {
    Index--;
    OptionalStrStart += StrSize;
    StrSize           = AsciiStrSize (OptionalStrStart);
  } while (OptionalStrStart[StrSize] != 0 && Index != 0);

  if ((Index != 0) || (StrSize == 1)) {
    //
    // Meet the end of strings set but Index is non-zero
    //
    return EFI_ABORTED;
  }

  StringBuffer = AllocatePool (StrSize * sizeof (CHAR16));
  if (StringBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  AsciiStrToUnicodeStrS (OptionalStrStart, StringBuffer, StrSize);

  *String = StringBuffer;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
SetSystemInfoStr (
  IN UI_CONTROL               *Control
  )
{
  EFI_SMBIOS_TABLE_HEADER     *Record;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  EFI_SMBIOS_PROTOCOL         *Smbios;
  EFI_STATUS                  Status;
  SMBIOS_TABLE_TYPE4          *Type4Record;
  SMBIOS_TABLE_TYPE17         *Type17Record;
  SMBIOS_TABLE_TYPE19         *Type19Record;
  CHAR16                      *ProcessorVerStr;
  CHAR16                      *MemClockSpeedStr;
  CHAR16                      *MemSizeStr;
  UINT64                      MemorySizeInGB;
  UINT64                      MemorySizeInMB;
  UI_CONTROL                  *Child;

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  ProcessorVerStr  = NULL;
  MemClockSpeedStr = NULL;
  MemSizeStr       = NULL;
  MemorySizeInGB   = 0;
  MemorySizeInMB   = 0;

  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    if (Record->Type == EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION) {
      Type4Record = (SMBIOS_TABLE_TYPE4 *) Record;
      if (ProcessorVerStr != NULL) {
        FreePool (ProcessorVerStr);
      }
      GetOptionalStringByIndex ((CHAR8*)((UINT8*) Type4Record + Type4Record->Hdr.Length), Type4Record->ProcessorVersion, &ProcessorVerStr);
    }

    if (Record->Type == EFI_SMBIOS_TYPE_MEMORY_DEVICE) {
      Type17Record = (SMBIOS_TABLE_TYPE17 *) Record;

     if (Type17Record->ConfiguredMemoryClockSpeed != 0) {
        if (MemClockSpeedStr != NULL) {
          FreePool (MemClockSpeedStr);
        }
        MemClockSpeedStr = CatSPrint (NULL, L"DRAM Frequency: %d MHz", Type17Record->ConfiguredMemoryClockSpeed);
      }
    }

    if (Record->Type == EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS) {
      Type19Record = (SMBIOS_TABLE_TYPE19 *) Record;
      if (Type19Record->StartingAddress == 0xFFFFFFFF && Type19Record->EndingAddress == 0xFFFFFFFF) {
        MemorySizeInGB += (RShiftU64((Type19Record->ExtendedEndingAddress - Type19Record->ExtendedStartingAddress), 30) + 1);
      } else {
        MemorySizeInMB += (RShiftU64((Type19Record->EndingAddress - Type19Record->StartingAddress), 10) + 1);
      }
    }
  } while(Status == EFI_SUCCESS);

  if ((MemorySizeInGB != 0) && (MemorySizeInMB != 0)) {
    MemorySizeInMB += LShiftU64 (MemorySizeInGB, 10);
    MemSizeStr = CatSPrint (NULL, L"Memory Size: %d MB", MemorySizeInMB);
  } else if (MemorySizeInGB != 0) {
    MemSizeStr = CatSPrint (NULL, L"Memory Size: %d GB", MemorySizeInGB);
  } else if (MemorySizeInMB != 0) {
    MemSizeStr = CatSPrint (NULL, L"Memory Size: %d MB", MemorySizeInMB);
  }

  Child = UiFindChildByName (Control, L"OwnerDrawSystemInfoProcessor");
  if (Child != NULL) {
    if (ProcessorVerStr == NULL) {
      UiSetAttribute (Child, L"visibility", L"false");
    } else {
      UiSetAttribute (Child, L"visibility", L"true");
      UiSetAttribute (Child, L"text", ProcessorVerStr);
    }
  }
  Child = UiFindChildByName (Control, L"OwnerDrawSystemInfoMemFrequency");
  if (Child != NULL) {
    if (MemClockSpeedStr == NULL) {
      UiSetAttribute (Child, L"visibility", L"false");
    } else {
      UiSetAttribute (Child, L"visibility", L"true");
      UiSetAttribute (Child, L"text", MemClockSpeedStr);
    }
  }
  Child = UiFindChildByName (Control, L"OwnerDrawSystemInfoMemSize");
  if (Child != NULL) {
    if (MemSizeStr == NULL) {
      UiSetAttribute (Child, L"visibility", L"false");
    } else {
      UiSetAttribute (Child, L"visibility", L"true");
      UiSetAttribute (Child, L"text", MemSizeStr);
    }
  }

  if (ProcessorVerStr != NULL) {
    FreePool (ProcessorVerStr);
  }
  if (MemClockSpeedStr != NULL) {
    FreePool (MemClockSpeedStr);
  }
  if (MemSizeStr != NULL) {
    FreePool (MemSizeStr);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
UpdateOwnerDrawText (
  IN UI_CONTROL                            *PanelControl,
  IN CHAR16                                *ControlName,
  IN CHAR16                                *TextStr
  )
{
  UI_MANAGER                               *Manager;
  UI_CONTROL                               *Control;

  if (PanelControl == NULL || ControlName == NULL || TextStr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Manager = PanelControl->Manager;

  Control = Manager->FindControlByName (Manager, ControlName);
  if (Control != NULL) {
    UiSetAttribute (Control, L"text", TextStr);
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
OwnerDrawTimerFunc (
  IN UI_CONTROL                            *PanelControl,
  IN BOOLEAN                               ForceRefresh
  )
{
  EFI_STATUS                               Status;
  EFI_TIME                                 Time;
  CHAR16                                   String[20];

  Status = gRT->GetTime (&Time, NULL);
  if (!EFI_ERROR (Status)) {
    if ((Time.Second == mOldSecond) && !ForceRefresh) {
      return EFI_SUCCESS;
    }
    mOldSecond = Time.Second;
    UnicodeSPrint (String, sizeof (String), L"%04d/%02d/%02d", Time.Year, Time.Month, Time.Day);
    UpdateOwnerDrawText (PanelControl, L"OwnerDrawDateValue", String);

    UnicodeSPrint (String, sizeof (String), L"%s", GetWeekdayStr (&Time));
    UpdateOwnerDrawText (PanelControl, L"OwnerDrawWeekdayValue", String);

    UnicodeSPrint (String, sizeof (String), L"%02d:%02d:%02d", Time.Hour, Time.Minute, Time.Second);
    if (PcdGet32(PcdH2OLmdeMultiLayout) == 1) {
      UnicodeSPrint (String, sizeof (String), L"%02d:%02d", Time.Hour, Time.Minute);
    }
    UpdateOwnerDrawText (PanelControl, L"OwnerDrawTimeValue", String);
  }

  UpdateOwnerDrawText (PanelControl, L"OwnerDrawMainboardName"    , L"MAINBOARD");
  UpdateOwnerDrawText (PanelControl, L"OwnerDrawSystemName"       , L"SYSTEM");
  UpdateOwnerDrawText (PanelControl, L"OwnerDrawSystemTemperature", L"27\xB0" L"C");
  UpdateOwnerDrawText (PanelControl, L"OwnerDrawCpuName"          , L"CPU");
  UpdateOwnerDrawText (PanelControl, L"OwnerDrawTemperatureName"  , L"TEMPERATURE");
  UpdateOwnerDrawText (PanelControl, L"OwnerDrawCpuTemperature"   , L"30\xB0" L"C");

  return EFI_SUCCESS;
}

/**
  Set attribute to UI control data of UI owner draw

  @param[in] Control               Pointer to UI control
  @param[in] Name                  Attribute name
  @param[in] Value                 Attribute string

  @retval EFI_SUCCESS              Perform owner draw success
  @retval EFI_ABORTED              Control or manager data is not found
**/
BOOLEAN
EFIAPI
H2OOwnerDrawPanelSetAttribute (
  IN UI_CONTROL                 *Control,
  IN CHAR16                     *Name,
  IN CHAR16                     *Value
  )
{
  H2O_OWNER_DRAW_PANEL          *This;
  EFI_STATUS                    Status;

  This = (H2O_OWNER_DRAW_PANEL *) Control;

  if (StrCmp (Name, L"RefreshInterval") == 0) {
    This->RefreshInterval = (UINT32) StrToUInt (Value, 10, &Status);
    KillTimer (((UI_CONTROL *)This)->Wnd, OWNER_DRAW_TIMER_ID);
    if (GetWindowLongPtr (Control->Wnd, GWL_STYLE) & WS_VISIBLE) {
      if (This->RefreshInterval != 0) {
        SetTimer (Control->Wnd, OWNER_DRAW_TIMER_ID, (UINT) This->RefreshInterval, NULL);
        OwnerDrawTimerFunc (Control, FORCE_REFRESH_PANEL);
      }
    }
  } else if (StrCmp (Name, L"visibility") == 0) {
    KillTimer (((UI_CONTROL *)This)->Wnd, OWNER_DRAW_TIMER_ID);
    if (StrCmp (Value, L"true") == 0) {
      if (This->RefreshInterval != 0) {
        SetTimer (Control->Wnd, OWNER_DRAW_TIMER_ID, (UINT) This->RefreshInterval, NULL);
        OwnerDrawTimerFunc (Control, FORCE_REFRESH_PANEL);
      }
    }
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }

  return TRUE;
}


LRESULT
EFIAPI
H2OOwnerDrawPanelProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  H2O_OWNER_DRAW_PANEL          *This;
  UI_CONTROL                    *Control;
  UI_MANAGER                    *Manager;

  Manager = NULL;

  This = (H2O_OWNER_DRAW_PANEL *) GetWindowLongPtr (Hwnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *)This;

  switch (Msg) {

  case WM_CREATE:
    This = (H2O_OWNER_DRAW_PANEL *) AllocateZeroPool (sizeof (H2O_OWNER_DRAW_PANEL));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    Manager = Control->Manager;
    XmlCreateControl (mOwnerDrawPanelChilds, Control);
    This->OwnerDrawList = Manager->FindControlByName (Manager, L"OwnerDrawList");
    OwnerDrawTimerFunc (Control, FORCE_REFRESH_PANEL);
    SetSystemInfoStr (Control);
    break;

  case WM_TIMER:
    if (WParam == OWNER_DRAW_TIMER_ID) {
      OwnerDrawTimerFunc (Control, NO_FORCE_REFRESH_PANEL);
    }
    break;

  case WM_DESTROY:
    if (This->RefreshInterval != 0) {
      KillTimer (Hwnd, OWNER_DRAW_TIMER_ID);
    }
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
    break;

  case FB_NOTIFY_REPAINT:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
    CONTROL_CLASS_INVALIDATE (Control);
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
  }

  return 0;
}


H2O_OWNER_DRAW_PANEL_CLASS *
EFIAPI
GetH2OOwnerDrawPanelClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"H2OOwnerDrawPanel", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = H2OOwnerDrawPanelProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = H2OOwnerDrawPanelSetAttribute;

  return CURRENT_CLASS;
}

