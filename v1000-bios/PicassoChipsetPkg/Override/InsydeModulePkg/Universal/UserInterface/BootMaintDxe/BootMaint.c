/** @file
  The boot maintenance implementation

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  The functions for Boot Maintainence Main menu.

Copyright (c) 2004 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BootMaint.h"

STATIC EFI_FORM_BROWSER2_PROTOCOL         *gBMFormBrowser2;

EFI_DEVICE_PATH_PROTOCOL  EndDevicePath[] = {
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH,
      0
    }
  }
};

STATIC CONST HII_VENDOR_DEVICE_PATH  mBmmHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    BOOT_MAINT_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

STATIC CONST HII_VENDOR_DEVICE_PATH  mFeHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    FILE_EXPLORE_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

CHAR16  mBootMaintStorageName[]     = L"BmmData";
CHAR16  mFileExplorerStorageName[]  = L"FeData";

BOOLEAN                     gConnectAllHappened = FALSE;
EFI_LOAD_FILE_PROTOCOL      mBootMaintLoadFile  = {BootMaintLoadFile};
BMM_CALLBACK_DATA           *mBmmCallbackInfo = NULL;
BOOLEAN                     mBootMaintSendFormBySelf = FALSE;

/**
  Init all memu.

  @param CallbackData    The BMM context data.

**/
VOID
InitAllMenu (
  IN  BMM_CALLBACK_DATA    *CallbackData
  );

/**
  Free up all Menu Option list.

**/
VOID
FreeAllMenu (
  VOID
  );

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Request         A null-terminated Unicode string in <ConfigRequest> format.
  @param Progress        On return, points to a character in the Request string.
                         Points to the string's null terminator if request was successful.
                         Points to the most recent '&' before the first failing name/value
                         pair (or the beginning of the string if the failure is in the
                         first name/value pair) if the request was not successful.
  @param Results         A null-terminated Unicode string in <ConfigAltResp> format which
                         has all values filled in for the names in the Request string.
                         String to be allocated by the called function.

  @retval  EFI_SUCCESS            The Results is filled with the requested values.
  @retval  EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval  EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
BMFakeExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Progress = Request;
  return EFI_NOT_FOUND;
}

/**
  This function processes the results of changes in configuration.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Configuration   A null-terminated Unicode string in <ConfigResp> format.
  @param Progress        A pointer to a string filled in with the offset of the most
                         recent '&' before the first failing name/value pair (or the
                         beginning of the string if the failure is in the first
                         name/value pair) or the terminating NULL if all was successful.

  @retval  EFI_SUCCESS            The Results is processed successfully.
  @retval  EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
BMFakeRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;
  if (!HiiIsConfigHdrMatch (Configuration, &gBootMaintFormSetGuid, mBootMaintStorageName)
      && !HiiIsConfigHdrMatch (Configuration, &gFileExploreFormSetGuid, mFileExplorerStorageName)) {
    return EFI_NOT_FOUND;
  }

  *Progress = Configuration + StrLen (Configuration);
  return EFI_SUCCESS;
}


/**
  Create string tokens for a menu from its help strings and display strings

  @param CallbackData       The BMM context data.
  @param HiiHandle          Hii Handle of the package to be updated.
  @param MenuOption         The Menu whose string tokens need to be created

  @retval  EFI_SUCCESS      String tokens created successfully
  @retval  others           contain some errors
**/
EFI_STATUS
CreateMenuStringToken (
  IN BMM_CALLBACK_DATA                *CallbackData,
  IN EFI_HII_HANDLE                   HiiHandle,
  IN BM_MENU_OPTION                   *MenuOption
  )
{
  BM_MENU_ENTRY *NewMenuEntry;
  UINTN         Index;

  for (Index = 0; Index < MenuOption->MenuNumber; Index++) {
    NewMenuEntry = BOpt_GetMenuEntry (MenuOption, Index);

    NewMenuEntry->DisplayStringToken = HiiSetString (
                                         HiiHandle,
                                         0,
                                         NewMenuEntry->DisplayString,
                                         NULL
                                         );

    if (NULL == NewMenuEntry->HelpString) {
      NewMenuEntry->HelpStringToken = NewMenuEntry->DisplayStringToken;
    } else {
      NewMenuEntry->HelpStringToken = HiiSetString (
                                        HiiHandle,
                                        0,
                                        NewMenuEntry->HelpString,
                                        NULL
                                        );
    }
  }

  return EFI_SUCCESS;
}

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Request         A null-terminated Unicode string in <ConfigRequest> format.
  @param Progress        On return, points to a character in the Request string.
                         Points to the string's null terminator if request was successful.
                         Points to the most recent '&' before the first failing name/value
                         pair (or the beginning of the string if the failure is in the
                         first name/value pair) if the request was not successful.
  @param Results         A null-terminated Unicode string in <ConfigAltResp> format which
                         has all values filled in for the names in the Request string.
                         String to be allocated by the called function.

  @retval  EFI_SUCCESS            The Results is filled with the requested values.
  @retval  EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval  EFI_INVALID_PARAMETER  Request is NULL, illegal syntax, or unknown name.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
BootMaintExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS         Status;
  UINTN              BufferSize;
  BMM_CALLBACK_DATA  *Private;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  BOOLEAN                          AllocatedRequest;
  UINTN                            Size;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;
  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &gBootMaintFormSetGuid, mBootMaintStorageName)) {
    return EFI_NOT_FOUND;
  }

  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Size             = 0;

  Private = BMM_CALLBACK_DATA_FROM_THIS (This);
  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  BufferSize = sizeof (BMM_FAKE_NV_DATA);
  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&gBootMaintFormSetGuid, mBootMaintStorageName, Private->BmmDriverHandle);
    if (ConfigRequestHdr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    if (ConfigRequest == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
  }

  Status = gHiiConfigRouting->BlockToConfig (
                                gHiiConfigRouting,
                                ConfigRequest,
                                (UINT8 *) &Private->BmmFakeNvData,
                                BufferSize,
                                Results,
                                Progress
                                );
  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;
}

/**
  This function processes the results of changes in configuration.


  @param This               Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action             Specifies the type of action taken by the browser.
  @param QuestionId         A unique value which is sent to the original exporting driver
                            so that it can identify the type of data to expect.
  @param Type               The type of value for the question.
  @param Value              A pointer to the data being sent to the original exporting driver.
  @param ActionRequest      On return, points to the action requested by the callback function.

  @retval EFI_SUCCESS           The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES  Not enough storage is available to hold the variable and its data.
  @retval EFI_DEVICE_ERROR      The variable could not be saved.
  @retval EFI_UNSUPPORTED       The specified Action is not supported by the callback.
  @retval EFI_INVALID_PARAMETER The parameter of Value or ActionRequest is invalid.
**/
EFI_STATUS
EFIAPI
BootMaintCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL         *This,
  IN        EFI_BROWSER_ACTION                     Action,
  IN        EFI_QUESTION_ID                        QuestionId,
  IN        UINT8                                  Type,
  IN        EFI_IFR_TYPE_VALUE                     *Value,
  OUT       EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  BMM_CALLBACK_DATA *Private;
  BM_MENU_ENTRY     *NewMenuEntry;
  BMM_FAKE_NV_DATA  *CurrentFakeNVMap;
  EFI_STATUS        Status;
  UINTN             OldValue;
  UINTN             NewValue;
  UINTN             Number;
  UINTN             Pos;
  UINTN             Bit;
  UINT16            NewValuePos;
  UINT16            Index3;
  UINT16            Index2;
  UINT16            Index;
  UINT8             *OldLegacyDev;
  UINT8             *NewLegacyDev;
  UINT8             *DisMap;

  if (Action != EFI_BROWSER_ACTION_CHANGING && Action != EFI_BROWSER_ACTION_CHANGED) {
    //
    // All other action return unsupported.
    //
    return EFI_UNSUPPORTED;
  }

  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status       = EFI_SUCCESS;
  OldValue     = 0;
  NewValue     = 0;
  Number       = 0;
  OldLegacyDev = NULL;
  NewLegacyDev = NULL;
  NewValuePos  = 0;
  DisMap       = NULL;

  Private      = BMM_CALLBACK_DATA_FROM_THIS (This);
  //
  // Retrive uncommitted data from Form Browser
  //
  CurrentFakeNVMap = &Private->BmmFakeNvData;
  HiiGetBrowserData (&gBootMaintFormSetGuid, mBootMaintStorageName, sizeof (BMM_FAKE_NV_DATA), (UINT8 *) CurrentFakeNVMap);
  if (Action == EFI_BROWSER_ACTION_CHANGING) {
    UpdatePageId (Private, QuestionId);

    //
    // need to be subtituded.
    //
    // Update Select FD/HD/CD/NET/BEV Order Form
    //
    if ((QuestionId >= LEGACY_FD_QUESTION_ID) && (QuestionId < LEGACY_BEV_QUESTION_ID + MAX_MENU_NUMBER)) {

      DisMap  = Private->BmmOldFakeNVData.DisableMap;

      if (QuestionId >= LEGACY_FD_QUESTION_ID && QuestionId < LEGACY_FD_QUESTION_ID + MAX_MENU_NUMBER) {
        Number        = (UINT16) LegacyFDMenu.MenuNumber;
        OldLegacyDev  = Private->BmmOldFakeNVData.LegacyFD;
        NewLegacyDev  = CurrentFakeNVMap->LegacyFD;
      } else if (QuestionId >= LEGACY_HD_QUESTION_ID && QuestionId < LEGACY_HD_QUESTION_ID + MAX_MENU_NUMBER) {
        Number        = (UINT16) LegacyHDMenu.MenuNumber;
        OldLegacyDev  = Private->BmmOldFakeNVData.LegacyHD;
        NewLegacyDev  = CurrentFakeNVMap->LegacyHD;
      } else if (QuestionId >= LEGACY_CD_QUESTION_ID && QuestionId < LEGACY_CD_QUESTION_ID + MAX_MENU_NUMBER) {
        Number        = (UINT16) LegacyCDMenu.MenuNumber;
        OldLegacyDev  = Private->BmmOldFakeNVData.LegacyCD;
        NewLegacyDev  = CurrentFakeNVMap->LegacyCD;
      } else if (QuestionId >= LEGACY_NET_QUESTION_ID && QuestionId < LEGACY_NET_QUESTION_ID + MAX_MENU_NUMBER) {
        Number        = (UINT16) LegacyNETMenu.MenuNumber;
        OldLegacyDev  = Private->BmmOldFakeNVData.LegacyNET;
        NewLegacyDev  = CurrentFakeNVMap->LegacyNET;
      } else if (QuestionId >= LEGACY_BEV_QUESTION_ID && QuestionId < LEGACY_BEV_QUESTION_ID + MAX_MENU_NUMBER) {
        Number        = (UINT16) LegacyBEVMenu.MenuNumber;
        OldLegacyDev  = Private->BmmOldFakeNVData.LegacyBEV;
        NewLegacyDev  = CurrentFakeNVMap->LegacyBEV;
      }
      //
      //  First, find the different position
      //  if there is change, it should be only one
      //
      for (Index = 0; Index < Number; Index++) {
        if (OldLegacyDev[Index] != NewLegacyDev[Index]) {
          OldValue  = OldLegacyDev[Index];
          NewValue  = NewLegacyDev[Index];
          break;
        }
      }

      if (Index != Number) {
        //
        // there is change, now process
        //
        if (0xFF == NewValue) {
          //
          // This item will be disable
          // Just move the items behind this forward to overlap it
          //
          Pos = OldValue / 8;
          Bit = 7 - (OldValue % 8);
          DisMap[Pos] = (UINT8) (DisMap[Pos] | (UINT8) (1 << Bit));
          for (Index2 = Index; Index2 < Number - 1; Index2++) {
            NewLegacyDev[Index2] = NewLegacyDev[Index2 + 1];
          }

          NewLegacyDev[Index2] = 0xFF;
        } else {
          for (Index2 = 0; Index2 < Number; Index2++) {
            if (Index2 == Index) {
              continue;
            }

            if (OldLegacyDev[Index2] == NewValue) {
              //
              // If NewValue is in OldLegacyDev array
              // remember its old position
              //
              NewValuePos = Index2;
              break;
            }
          }

          if (Index2 != Number) {
            //
            // We will change current item to an existing item
            // (It's hard to describe here, please read code, it's like a cycle-moving)
            //
            for (Index2 = NewValuePos; Index2 != Index;) {
              if (NewValuePos < Index) {
                NewLegacyDev[Index2] = OldLegacyDev[Index2 + 1];
                Index2++;
              } else {
                NewLegacyDev[Index2] = OldLegacyDev[Index2 - 1];
                Index2--;
              }
            }
          } else {
            //
            // If NewValue is not in OldlegacyDev array, we are changing to a disabled item
            // so we should modify DisMap to reflect the change
            //
            Pos = NewValue / 8;
            Bit = 7 - (NewValue % 8);
            DisMap[Pos] = (UINT8) (DisMap[Pos] & (~ (UINT8) (1 << Bit)));
            if (0xFF != OldValue) {
              //
              // Because NewValue is a item that was disabled before
              // so after changing the OldValue should be disabled
              // actually we are doing a swap of enable-disable states of two items
              //
              Pos = OldValue / 8;
              Bit = 7 - (OldValue % 8);
              DisMap[Pos] = (UINT8) (DisMap[Pos] | (UINT8) (1 << Bit));
            }
          }
        }
        //
        // To prevent DISABLE appears in the middle of the list
        // we should perform a re-ordering
        //
        Index3 = Index;
        Index = 0;
        while (Index < Number) {
          if (0xFF != NewLegacyDev[Index]) {
            Index++;
            continue;
          }

          Index2 = Index;
          Index2++;
          while (Index2 < Number) {
            if (0xFF != NewLegacyDev[Index2]) {
              break;
            }

            Index2++;
          }

          if (Index2 < Number) {
            NewLegacyDev[Index]   = NewLegacyDev[Index2];
            NewLegacyDev[Index2]  = 0xFF;
          }

          Index++;
        }

        CopyMem (
          OldLegacyDev,
          NewLegacyDev,
          Number
          );

        //
        //  Return correct question value.
        //
        Value->u8 = NewLegacyDev[Index3];
      }
    }

    if (QuestionId < FILE_OPTION_OFFSET) {
      if (QuestionId < CONFIG_OPTION_OFFSET) {
        switch (QuestionId) {
        case KEY_VALUE_BOOT_FROM_FILE:
          Private->FeCurrentState = FileExplorerStateBootFromFile;
          break;

        case FORM_BOOT_ADD_ID:
          Private->FeCurrentState = FileExplorerStateAddBootOption;
          break;

        case FORM_DRV_ADD_FILE_ID:
          Private->FeCurrentState = FileExplorerStateAddDriverOptionState;
          break;

        case FORM_DRV_ADD_HANDLE_ID:
          CleanUpPage (FORM_DRV_ADD_HANDLE_ID, Private);
          UpdateDrvAddHandlePage (Private);
          break;

        case FORM_BOOT_DEL_ID:
          CleanUpPage (FORM_BOOT_DEL_ID, Private);
          UpdateBootDelPage (Private);
          break;

        case FORM_BOOT_CHG_ID:
        case FORM_DRV_CHG_ID:
          UpdatePageBody (QuestionId, Private);
          break;

        case FORM_DRV_DEL_ID:
          CleanUpPage (FORM_DRV_DEL_ID, Private);
          UpdateDrvDelPage (Private);
          break;

        case FORM_BOOT_NEXT_ID:
          CleanUpPage (FORM_BOOT_NEXT_ID, Private);
          UpdateBootNextPage (Private);
          break;

        case FORM_TIME_OUT_ID:
          CleanUpPage (FORM_TIME_OUT_ID, Private);
          UpdateTimeOutPage (Private);
          break;

        case FORM_CON_IN_ID:
        case FORM_CON_OUT_ID:
        case FORM_CON_ERR_ID:
          UpdatePageBody (QuestionId, Private);
          break;

        case FORM_CON_MODE_ID:
          CleanUpPage (FORM_CON_MODE_ID, Private);
          UpdateConModePage (Private);
          break;

        case FORM_CON_COM_ID:
          CleanUpPage (FORM_CON_COM_ID, Private);
          UpdateConCOMPage (Private);
          break;

        case FORM_SET_FD_ORDER_ID:
        case FORM_SET_HD_ORDER_ID:
        case FORM_SET_CD_ORDER_ID:
        case FORM_SET_NET_ORDER_ID:
        case FORM_SET_BEV_ORDER_ID:
          CleanUpPage (QuestionId, Private);
          UpdateSetLegacyDeviceOrderPage (QuestionId, Private);
          break;

        default:
          break;
        }
      } else if ((QuestionId >= TERMINAL_OPTION_OFFSET) && (QuestionId < CONSOLE_OPTION_OFFSET)) {
        Index2                    = (UINT16) (QuestionId - TERMINAL_OPTION_OFFSET);
        Private->CurrentTerminal  = Index2;

        CleanUpPage (FORM_CON_COM_SETUP_ID, Private);
        UpdateTerminalPage (Private);

      } else if (QuestionId >= HANDLE_OPTION_OFFSET) {
        Index2                  = (UINT16) (QuestionId - HANDLE_OPTION_OFFSET);

        NewMenuEntry            = BOpt_GetMenuEntry (&DriverMenu, Index2);
        ASSERT (NewMenuEntry != NULL);
        Private->HandleContext  = (BM_HANDLE_CONTEXT *) NewMenuEntry->VariableContext;

        CleanUpPage (FORM_DRV_ADD_HANDLE_DESC_ID, Private);

        Private->MenuEntry                  = NewMenuEntry;
        Private->LoadContext->FilePathList  = Private->HandleContext->DevicePath;

        UpdateDriverAddHandleDescPage (Private);
      }
    }
  } else if (Action == EFI_BROWSER_ACTION_CHANGED) {

    switch (QuestionId) {
    case KEY_VALUE_SAVE_AND_EXIT:
    case KEY_VALUE_NO_SAVE_AND_EXIT:
      if (QuestionId == KEY_VALUE_SAVE_AND_EXIT) {
        Status = ApplyChangeHandler (Private, CurrentFakeNVMap, Private->BmmPreviousPageId);
        if (EFI_ERROR (Status)) {
          return Status;
        }
      } else if (QuestionId == KEY_VALUE_NO_SAVE_AND_EXIT) {
        DiscardChangeHandler (Private, CurrentFakeNVMap);
      }

      //
      // Tell browser not to ask for confirmation of changes,
      // since we have already applied or discarded.
      //
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_FORM_SUBMIT_EXIT;
      break;

    case FORM_RESET:
      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
      return EFI_UNSUPPORTED;

    default:
      break;
    }
  }

  //
  // Pass changed uncommitted data back to Form Browser
  //
  HiiSetBrowserData (&gBootMaintFormSetGuid, mBootMaintStorageName, sizeof (BMM_FAKE_NV_DATA), (UINT8 *) CurrentFakeNVMap, NULL);
  return EFI_SUCCESS;
}

/**
  Function handling request to apply changes for BMM pages.

  @param Private            Pointer to callback data buffer.
  @param CurrentFakeNVMap   Pointer to buffer holding data of various values used by BMM
  @param FormId             ID of the form which has sent the request to apply change.

  @retval  EFI_SUCCESS       Change successfully applied.
  @retval  Other             Error occurs while trying to apply changes.

**/
EFI_STATUS
ApplyChangeHandler (
  IN  BMM_CALLBACK_DATA               *Private,
  IN  BMM_FAKE_NV_DATA                *CurrentFakeNVMap,
  IN  EFI_FORM_ID                     FormId
  )
{
  BM_CONSOLE_CONTEXT  *NewConsoleContext;
  BM_TERMINAL_CONTEXT *NewTerminalContext;
  BM_LOAD_CONTEXT     *NewLoadContext;
  BM_MENU_ENTRY       *NewMenuEntry;
  EFI_STATUS          Status;
  UINT16              Index;
  UINT16              Timeout;

  Status = EFI_SUCCESS;

  switch (FormId) {
  case FORM_SET_FD_ORDER_ID:
  case FORM_SET_HD_ORDER_ID:
  case FORM_SET_CD_ORDER_ID:
  case FORM_SET_NET_ORDER_ID:
  case FORM_SET_BEV_ORDER_ID:
    Var_UpdateBBSOption (Private);
    break;

  case FORM_BOOT_DEL_ID:
    for (Index = 0;
         ((Index < BootOptionMenu.MenuNumber) && (Index < (sizeof (CurrentFakeNVMap->OptionDel) / sizeof (CurrentFakeNVMap->OptionDel[0]))));
         Index ++) {
      NewMenuEntry            = BOpt_GetMenuEntry (&BootOptionMenu, Index);
      NewLoadContext          = (BM_LOAD_CONTEXT *) NewMenuEntry->VariableContext;
      NewLoadContext->Deleted = CurrentFakeNVMap->OptionDel[Index];
    }

    Var_DelBootOption ();
    break;

  case FORM_DRV_DEL_ID:
    for (Index = 0;
         ((Index < DriverOptionMenu.MenuNumber) && (Index < (sizeof (CurrentFakeNVMap->OptionDel) / sizeof (CurrentFakeNVMap->OptionDel[0]))));
         Index++) {
      NewMenuEntry            = BOpt_GetMenuEntry (&DriverOptionMenu, Index);
      NewLoadContext          = (BM_LOAD_CONTEXT *) NewMenuEntry->VariableContext;
      NewLoadContext->Deleted = CurrentFakeNVMap->OptionDel[Index];
    }

    Var_DelDriverOption ();
    break;

  case FORM_BOOT_CHG_ID:
    Status = Var_UpdateBootOrder (Private);
    break;

  case FORM_DRV_CHG_ID:
    Status = Var_UpdateDriverOrder (Private);
    break;

  case FORM_TIME_OUT_ID:
    Timeout = CurrentFakeNVMap->BootTimeOut;
    gRT->SetVariable (
           L"Timeout",
           &gEfiGlobalVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           sizeof (Timeout),
           &Timeout
           );

    Private->BmmOldFakeNVData.BootTimeOut = CurrentFakeNVMap->BootTimeOut;
    break;

  case FORM_BOOT_NEXT_ID:
    Status = Var_UpdateBootNext (Private);
    break;

  case FORM_CON_MODE_ID:
    Status = Var_UpdateConMode (Private);
    break;

  case FORM_CON_COM_SETUP_ID:
    NewMenuEntry                      = BOpt_GetMenuEntry (&TerminalMenu, Private->CurrentTerminal);

    ASSERT (NewMenuEntry != NULL);
    if (NewMenuEntry == NULL) {
      return EFI_ABORTED;
    }

    NewTerminalContext                = (BM_TERMINAL_CONTEXT *) NewMenuEntry->VariableContext;

    NewTerminalContext->BaudRateIndex = CurrentFakeNVMap->COMBaudRate;
    ASSERT (CurrentFakeNVMap->COMBaudRate < (sizeof (BaudRateList) / sizeof (BaudRateList[0])));
    if (CurrentFakeNVMap->COMBaudRate >= (sizeof (BaudRateList) / sizeof (BaudRateList[0]))) {
      return EFI_ABORTED;
    }
    NewTerminalContext->BaudRate      = BaudRateList[CurrentFakeNVMap->COMBaudRate].Value;
    NewTerminalContext->DataBitsIndex = CurrentFakeNVMap->COMDataRate;
    ASSERT (CurrentFakeNVMap->COMDataRate < (sizeof (DataBitsList) / sizeof (DataBitsList[0])));
    if (CurrentFakeNVMap->COMDataRate >= (sizeof (DataBitsList) / sizeof (DataBitsList[0]))) {
      return EFI_ABORTED;
    }
    NewTerminalContext->DataBits      = (UINT8) DataBitsList[CurrentFakeNVMap->COMDataRate].Value;
    NewTerminalContext->StopBitsIndex = CurrentFakeNVMap->COMStopBits;
    ASSERT (CurrentFakeNVMap->COMStopBits < (sizeof (StopBitsList) / sizeof (StopBitsList[0])));
    if (CurrentFakeNVMap->COMStopBits >= (sizeof (StopBitsList) / sizeof (StopBitsList[0]))) {
      return EFI_ABORTED;
    }
    NewTerminalContext->StopBits      = (UINT8) StopBitsList[CurrentFakeNVMap->COMStopBits].Value;
    NewTerminalContext->ParityIndex   = CurrentFakeNVMap->COMParity;
    ASSERT (CurrentFakeNVMap->COMParity < (sizeof (ParityList) / sizeof (ParityList[0])));
    if (CurrentFakeNVMap->COMParity >= (sizeof (ParityList) / sizeof (ParityList[0]))) {
      return EFI_ABORTED;
    }
    NewTerminalContext->Parity        = (UINT8) ParityList[CurrentFakeNVMap->COMParity].Value;
    NewTerminalContext->TerminalType  = CurrentFakeNVMap->COMTerminalType;
    NewTerminalContext->FlowControl   = CurrentFakeNVMap->COMFlowControl;

    ChangeTerminalDevicePath (
      &(NewTerminalContext->DevicePath),
      FALSE
      );

    Var_UpdateConsoleInpOption ();
    Var_UpdateConsoleOutOption ();
    Var_UpdateErrorOutOption ();
    break;

  case FORM_CON_IN_ID:
    for (Index = 0; Index < ConsoleInpMenu.MenuNumber; Index++) {
      NewMenuEntry                = BOpt_GetMenuEntry (&ConsoleInpMenu, Index);
      NewConsoleContext           = (BM_CONSOLE_CONTEXT *) NewMenuEntry->VariableContext;
      ASSERT (Index < MAX_MENU_NUMBER);
      if (Index >= MAX_MENU_NUMBER) {
        return EFI_ABORTED;
      }
      NewConsoleContext->IsActive = CurrentFakeNVMap->ConsoleCheck[Index];
    }

    for (Index = 0; Index < TerminalMenu.MenuNumber; Index++) {
      NewMenuEntry                = BOpt_GetMenuEntry (&TerminalMenu, Index);
      NewTerminalContext          = (BM_TERMINAL_CONTEXT *) NewMenuEntry->VariableContext;
      ASSERT (Index + ConsoleInpMenu.MenuNumber < MAX_MENU_NUMBER);
      if (Index + ConsoleInpMenu.MenuNumber >= MAX_MENU_NUMBER) {
        return EFI_ABORTED;
      }
      NewTerminalContext->IsConIn = CurrentFakeNVMap->ConsoleCheck[Index + ConsoleInpMenu.MenuNumber];
    }

    Var_UpdateConsoleInpOption ();
    break;

  case FORM_CON_OUT_ID:
    for (Index = 0; Index < ConsoleOutMenu.MenuNumber; Index++) {
      NewMenuEntry                = BOpt_GetMenuEntry (&ConsoleOutMenu, Index);
      NewConsoleContext           = (BM_CONSOLE_CONTEXT *) NewMenuEntry->VariableContext;
      ASSERT (Index < MAX_MENU_NUMBER);
      if (Index >= MAX_MENU_NUMBER) {
        return EFI_ABORTED;
      }
      NewConsoleContext->IsActive = CurrentFakeNVMap->ConsoleCheck[Index];
    }

    for (Index = 0; Index < TerminalMenu.MenuNumber; Index++) {
      NewMenuEntry                  = BOpt_GetMenuEntry (&TerminalMenu, Index);
      NewTerminalContext            = (BM_TERMINAL_CONTEXT *) NewMenuEntry->VariableContext;
      ASSERT (Index + ConsoleOutMenu.MenuNumber < MAX_MENU_NUMBER);
      if (Index + ConsoleOutMenu.MenuNumber >= MAX_MENU_NUMBER) {
        return EFI_ABORTED;
      }
      NewTerminalContext->IsConOut  = CurrentFakeNVMap->ConsoleCheck[Index + ConsoleOutMenu.MenuNumber];
    }

    Var_UpdateConsoleOutOption ();
    break;

  case FORM_CON_ERR_ID:
    for (Index = 0; Index < ConsoleErrMenu.MenuNumber; Index++) {
      NewMenuEntry                = BOpt_GetMenuEntry (&ConsoleErrMenu, Index);
      NewConsoleContext           = (BM_CONSOLE_CONTEXT *) NewMenuEntry->VariableContext;
      ASSERT (Index < MAX_MENU_NUMBER);
      if (Index >= MAX_MENU_NUMBER) {
        return EFI_ABORTED;
      }
      NewConsoleContext->IsActive = CurrentFakeNVMap->ConsoleCheck[Index];
    }

    for (Index = 0; Index < TerminalMenu.MenuNumber; Index++) {
      NewMenuEntry                  = BOpt_GetMenuEntry (&TerminalMenu, Index);
      NewTerminalContext            = (BM_TERMINAL_CONTEXT *) NewMenuEntry->VariableContext;
      ASSERT (Index + ConsoleErrMenu.MenuNumber < MAX_MENU_NUMBER);
      if (Index + ConsoleErrMenu.MenuNumber >= MAX_MENU_NUMBER) {
        return EFI_ABORTED;
      }
      NewTerminalContext->IsStdErr  = CurrentFakeNVMap->ConsoleCheck[Index + ConsoleErrMenu.MenuNumber];
    }

    Var_UpdateErrorOutOption ();
    break;

  case FORM_DRV_ADD_HANDLE_DESC_ID:
    Status = Var_UpdateDriverOption (
               Private,
               Private->BmmHiiHandle,
               CurrentFakeNVMap->DriverAddHandleDesc,
               sizeof(CurrentFakeNVMap->DriverAddHandleDesc),
               CurrentFakeNVMap->DriverAddHandleOptionalData,
               CurrentFakeNVMap->DriverAddForceReconnect
               );
    if (EFI_ERROR (Status)) {
      goto Error;
    }

    BOpt_GetDriverOptions (Private);
    CreateMenuStringToken (Private, Private->BmmHiiHandle, &DriverOptionMenu);
    break;

  default:
    break;
  }

Error:
  return Status;
}

/**
  Discard all changes done to the BMM pages such as Boot Order change,
  Driver order change.

  @param Private            The BMM context data.
  @param CurrentFakeNVMap   The current Fack NV Map.

**/
VOID
DiscardChangeHandler (
  IN  BMM_CALLBACK_DATA               *Private,
  IN  BMM_FAKE_NV_DATA                *CurrentFakeNVMap
  )
{
  UINT16  Index;

  switch (Private->BmmPreviousPageId) {
  case FORM_BOOT_CHG_ID:
  case FORM_DRV_CHG_ID:
    CopyMem (CurrentFakeNVMap->OptionOrder, Private->BmmOldFakeNVData.OptionOrder, sizeof (CurrentFakeNVMap->OptionOrder));
    break;

  case FORM_BOOT_DEL_ID:
    ASSERT (BootOptionMenu.MenuNumber <= (sizeof (CurrentFakeNVMap->OptionDel) / sizeof (CurrentFakeNVMap->OptionDel[0])));
    for (Index = 0; Index < BootOptionMenu.MenuNumber; Index++) {
      CurrentFakeNVMap->OptionDel[Index] = FALSE;
    }
    break;

  case FORM_DRV_DEL_ID:
    ASSERT (DriverOptionMenu.MenuNumber <= (sizeof (CurrentFakeNVMap->OptionDel) / sizeof (CurrentFakeNVMap->OptionDel[0])));
    for (Index = 0; Index < DriverOptionMenu.MenuNumber; Index++) {
      CurrentFakeNVMap->OptionDel[Index] = FALSE;
    }
    break;

  case FORM_BOOT_NEXT_ID:
    CurrentFakeNVMap->BootNext = Private->BmmOldFakeNVData.BootNext;
    break;

  case FORM_TIME_OUT_ID:
    CurrentFakeNVMap->BootTimeOut = Private->BmmOldFakeNVData.BootTimeOut;
    break;

  case FORM_DRV_ADD_HANDLE_DESC_ID:
  case FORM_DRV_ADD_FILE_ID:
  case FORM_DRV_ADD_HANDLE_ID:
    CurrentFakeNVMap->DriverAddHandleDesc[0]          = 0x0000;
    CurrentFakeNVMap->DriverAddHandleOptionalData[0]  = 0x0000;
    break;

  default:
    break;
  }
}

/**
  Initialize the Boot Maintenance Utitliy.


  @retval  EFI_SUCCESS      utility ended successfully
  @retval  others           contain some errors

**/
EFI_STATUS
InitializeBM (
  VOID
  )
{
  EFI_LEGACY_BIOS_PROTOCOL    *LegacyBios;
  BMM_CALLBACK_DATA           *BmmCallbackInfo;
  EFI_STATUS                  Status;
  UINT8                       *Ptr;

  Status = EFI_SUCCESS;
  BmmCallbackInfo = mBmmCallbackInfo;

  //
  // Connect all prior to entering the platform setup menu.
  //
  if (!gConnectAllHappened) {
    BdsLibConnectAllDriversToAllControllers ();
    gConnectAllHappened = TRUE;
  }
  //
  // Have chance to enumerate boot device
  //
  BdsLibEnumerateAllBootOption (FALSE, NULL);

  //
  // Create LoadOption in BmmCallbackInfo for Driver Callback
  //
  Ptr = AllocateZeroPool (sizeof (BM_LOAD_CONTEXT) + sizeof (BM_FILE_CONTEXT) + sizeof (BM_HANDLE_CONTEXT) + sizeof (BM_MENU_ENTRY));
  if (Ptr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize Bmm callback data.
  //
  BmmCallbackInfo->LoadContext = (BM_LOAD_CONTEXT *) Ptr;
  Ptr += sizeof (BM_LOAD_CONTEXT);

  BmmCallbackInfo->FileContext = (BM_FILE_CONTEXT *) Ptr;
  Ptr += sizeof (BM_FILE_CONTEXT);

  BmmCallbackInfo->HandleContext = (BM_HANDLE_CONTEXT *) Ptr;
  Ptr += sizeof (BM_HANDLE_CONTEXT);

  BmmCallbackInfo->MenuEntry      = (BM_MENU_ENTRY *) Ptr;

  BmmCallbackInfo->Signature                     = BMM_CALLBACK_DATA_SIGNATURE;
  BmmCallbackInfo->BmmPreviousPageId             = FORM_MAIN_ID;
  BmmCallbackInfo->BmmCurrentPageId              = FORM_MAIN_ID;
  BmmCallbackInfo->FeCurrentState                = FileExplorerStateInActive;
  BmmCallbackInfo->FeDisplayContext              = FileExplorerDisplayUnknown;
  BmmCallbackInfo->TerminalContext               = NULL;
  BmmCallbackInfo->CurrentTerminal               = 0;
  BmmCallbackInfo->BbsType                       = 0;
  ZeroMem (&BmmCallbackInfo->BmmFakeNvData   , sizeof(BmmCallbackInfo->BmmFakeNvData));
  ZeroMem (&BmmCallbackInfo->BmmOldFakeNVData, sizeof(BmmCallbackInfo->BmmOldFakeNVData));
  ZeroMem (&BmmCallbackInfo->FeFakeNvData    , sizeof(BmmCallbackInfo->FeFakeNvData));

  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &gBMFormBrowser2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Init OpCode Handle and Allocate space for creation of Buffer
  //
  mStartOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (mStartOpCodeHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mEndOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (mEndOpCodeHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  mStartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (mStartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  mStartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  mEndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (mEndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  mEndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  mEndLabel->Number       = LABEL_END;

  InitializeStringDepository ();

  InitAllMenu (BmmCallbackInfo);

  CreateMenuStringToken (BmmCallbackInfo, BmmCallbackInfo->BmmHiiHandle, &ConsoleInpMenu);
  CreateMenuStringToken (BmmCallbackInfo, BmmCallbackInfo->BmmHiiHandle, &ConsoleOutMenu);
  CreateMenuStringToken (BmmCallbackInfo, BmmCallbackInfo->BmmHiiHandle, &ConsoleErrMenu);
  CreateMenuStringToken (BmmCallbackInfo, BmmCallbackInfo->BmmHiiHandle, &BootOptionMenu);
  CreateMenuStringToken (BmmCallbackInfo, BmmCallbackInfo->BmmHiiHandle, &DriverOptionMenu);
  CreateMenuStringToken (BmmCallbackInfo, BmmCallbackInfo->BmmHiiHandle, &TerminalMenu);
  CreateMenuStringToken (BmmCallbackInfo, BmmCallbackInfo->BmmHiiHandle, &DriverMenu);

  UpdateBootDelPage (BmmCallbackInfo);
  UpdateDrvDelPage (BmmCallbackInfo);

  if (TerminalMenu.MenuNumber > 0) {
    BmmCallbackInfo->CurrentTerminal = 0;
    UpdateTerminalPage (BmmCallbackInfo);
  }

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (!EFI_ERROR (Status)) {
    RefreshUpdateData ();
    mStartLabel->Number = FORM_BOOT_LEGACY_DEVICE_ID;

    //
    // If LegacyBios Protocol is installed, add 3 tags about legacy boot option
    // in BootOption form: legacy FD/HD/CD/NET/BEV
    //
    HiiCreateGotoOpCode (
      mStartOpCodeHandle,
      FORM_SET_FD_ORDER_ID,
      STRING_TOKEN (STR_FORM_SET_FD_ORDER_TITLE),
      STRING_TOKEN (STR_FORM_SET_FD_ORDER_TITLE),
      EFI_IFR_FLAG_CALLBACK,
      FORM_SET_FD_ORDER_ID
      );

    HiiCreateGotoOpCode (
      mStartOpCodeHandle,
      FORM_SET_HD_ORDER_ID,
      STRING_TOKEN (STR_FORM_SET_HD_ORDER_TITLE),
      STRING_TOKEN (STR_FORM_SET_HD_ORDER_TITLE),
      EFI_IFR_FLAG_CALLBACK,
      FORM_SET_HD_ORDER_ID
      );

    HiiCreateGotoOpCode (
      mStartOpCodeHandle,
      FORM_SET_CD_ORDER_ID,
      STRING_TOKEN (STR_FORM_SET_CD_ORDER_TITLE),
      STRING_TOKEN (STR_FORM_SET_CD_ORDER_TITLE),
      EFI_IFR_FLAG_CALLBACK,
      FORM_SET_CD_ORDER_ID
      );

    HiiCreateGotoOpCode (
      mStartOpCodeHandle,
      FORM_SET_NET_ORDER_ID,
      STRING_TOKEN (STR_FORM_SET_NET_ORDER_TITLE),
      STRING_TOKEN (STR_FORM_SET_NET_ORDER_TITLE),
      EFI_IFR_FLAG_CALLBACK,
      FORM_SET_NET_ORDER_ID
      );

    HiiCreateGotoOpCode (
      mStartOpCodeHandle,
      FORM_SET_BEV_ORDER_ID,
      STRING_TOKEN (STR_FORM_SET_BEV_ORDER_TITLE),
      STRING_TOKEN (STR_FORM_SET_BEV_ORDER_TITLE),
      EFI_IFR_FLAG_CALLBACK,
      FORM_SET_BEV_ORDER_ID
      );

    HiiUpdateForm (
      BmmCallbackInfo->BmmHiiHandle,
      &gBootMaintFormSetGuid,
      FORM_BOOT_SETUP_ID,
      mStartOpCodeHandle, // Label FORM_BOOT_LEGACY_DEVICE_ID
      mEndOpCodeHandle    // LABEL_END
      );
  }

  return EFI_SUCCESS;
}

/**
  Release resource to shutdown Boot Maintenance Utility.
**/
VOID
ShutdownBM (
  VOID
  )
{
  mBmmCallbackInfo->FeCurrentState    = FileExplorerStateInActive;
  mBmmCallbackInfo->FeDisplayContext  = FileExplorerDisplayUnknown;

  if (FileOptionStrDepository != NULL) {
    CleanUpStringDepository ();
    FreeAllMenu ();
    FileOptionStrDepository = NULL;
  }

  if (mStartOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle (mStartOpCodeHandle);
    mStartOpCodeHandle = NULL;
  }

  if (mEndOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle (mEndOpCodeHandle);
    mEndOpCodeHandle = NULL;
  }

  if (mBmmCallbackInfo->LoadContext != NULL) {
    FreePool (mBmmCallbackInfo->LoadContext);
    mBmmCallbackInfo->LoadContext = NULL;
  }
}

/**
  Initialized all Menu Option List.

  @param CallbackData    The BMM context data.

**/
VOID
InitAllMenu (
  IN  BMM_CALLBACK_DATA    *CallbackData
  )
{
  InitializeListHead (&BootOptionMenu.Head);
  InitializeListHead (&DriverOptionMenu.Head);
  BOpt_GetBootOptions (CallbackData);
  BOpt_GetDriverOptions (CallbackData);
  BOpt_GetLegacyOptions ();
  InitializeListHead (&FsOptionMenu.Head);
  BOpt_FindDrivers ();
  InitializeListHead (&DirectoryMenu.Head);
  InitializeListHead (&ConsoleInpMenu.Head);
  InitializeListHead (&ConsoleOutMenu.Head);
  InitializeListHead (&ConsoleErrMenu.Head);
  InitializeListHead (&TerminalMenu.Head);
  LocateSerialIo ();
  GetAllConsoles ();
}

/**
  Free up all Menu Option list.

**/
VOID
FreeAllMenu (
  VOID
  )
{
  BOpt_FreeMenu (&DirectoryMenu);
  BOpt_FreeMenu (&FsOptionMenu);
  BOpt_FreeMenu (&BootOptionMenu);
  BOpt_FreeMenu (&DriverOptionMenu);
  BOpt_FreeMenu (&DriverMenu);
  BOpt_FreeLegacyOptions ();
  FreeAllConsoles ();
}

/**
  Initialize all the string depositories.

**/
VOID
InitializeStringDepository (
  VOID
  )
{
  STRING_DEPOSITORY *StringDepository;
  StringDepository              = AllocateZeroPool (sizeof (STRING_DEPOSITORY) * STRING_DEPOSITORY_NUMBER);
  FileOptionStrDepository       = StringDepository++;
  ConsoleOptionStrDepository    = StringDepository++;
  BootOptionStrDepository       = StringDepository++;
  BootOptionHelpStrDepository   = StringDepository++;
  DriverOptionStrDepository     = StringDepository++;
  DriverOptionHelpStrDepository = StringDepository++;
  TerminalStrDepository         = StringDepository;
}

/**
  Fetch a usable string node from the string depository and return the string token.

  @param CallbackData       The BMM context data.
  @param StringDepository   The string repository.

  @retval  EFI_STRING_ID           String token.

**/
EFI_STRING_ID
GetStringTokenFromDepository (
  IN   BMM_CALLBACK_DATA     *CallbackData,
  IN   STRING_DEPOSITORY     *StringDepository
  )
{
  STRING_LIST_NODE  *CurrentListNode;
  STRING_LIST_NODE  *NextListNode;

  CurrentListNode = StringDepository->CurrentNode;

  if ((NULL != CurrentListNode) && (NULL != CurrentListNode->Next)) {
    //
    // Fetch one reclaimed node from the list.
    //
    NextListNode = StringDepository->CurrentNode->Next;
  } else {
    //
    // If there is no usable node in the list, update the list.
    //
    NextListNode = AllocateZeroPool (sizeof (STRING_LIST_NODE));
    ASSERT (NextListNode != NULL);
    if (NextListNode == NULL) {
      return (EFI_STRING_ID) 0;
    }
    NextListNode->StringToken = HiiSetString (CallbackData->BmmHiiHandle, 0, L" ", NULL);
    ASSERT (NextListNode->StringToken != 0);

    StringDepository->TotalNodeNumber++;

    if (NULL == CurrentListNode) {
      StringDepository->ListHead = NextListNode;
    } else {
      CurrentListNode->Next = NextListNode;
    }
  }

  StringDepository->CurrentNode = NextListNode;

  return StringDepository->CurrentNode->StringToken;
}

/**
  Reclaim string depositories by moving the current node pointer to list head..

**/
VOID
ReclaimStringDepository (
  VOID
  )
{
  UINTN             DepositoryIndex;
  STRING_DEPOSITORY *StringDepository;

  StringDepository = FileOptionStrDepository;
  for (DepositoryIndex = 0; DepositoryIndex < STRING_DEPOSITORY_NUMBER; DepositoryIndex++) {
    StringDepository->CurrentNode = StringDepository->ListHead;
    StringDepository++;
  }
}

/**
  Release resource for all the string depositories.

**/
VOID
CleanUpStringDepository (
  VOID
  )
{
  UINTN             NodeIndex;
  UINTN             DepositoryIndex;
  STRING_LIST_NODE  *CurrentListNode;
  STRING_LIST_NODE  *NextListNode;
  STRING_DEPOSITORY *StringDepository;

  //
  // Release string list nodes.
  //
  StringDepository = FileOptionStrDepository;
  for (DepositoryIndex = 0; DepositoryIndex < STRING_DEPOSITORY_NUMBER; DepositoryIndex++) {
    CurrentListNode = StringDepository->ListHead;
    for (NodeIndex = 0; NodeIndex < StringDepository->TotalNodeNumber; NodeIndex++) {
      NextListNode = CurrentListNode->Next;
      FreePool (CurrentListNode);
      CurrentListNode = NextListNode;
    }

    StringDepository++;
  }
  //
  // Release string depository.
  //
  FreePool (FileOptionStrDepository);
}

/**
  Enter boot maintenance manager.

  @param[in]      This          Protocol instance pointer.
  @param[in]      FilePath      The device specific path of the file to load.
  @param[in]      BootPolicy    If TRUE, indicates that the request originates from the boot manager is attempting to
                                load FilePath as a boot selection. If FALSE, then FilePath must match as exact file to be loaded.
  @param[in, out] BufferSize    On input the size of Buffer in bytes. On output with a return code of EFI_SUCCESS,
                                the amount of data transferred to Buffer. On output with a return code of EFI_BUFFER_TOO_SMALL,
                                the size of Buffer required to retrieve the requested file.
  @param[in]  Buffer            The memory buffer to transfer the file to. IF Buffer is NULL, then no the size of the
                                requested file is returned in BufferSize.

  @retval EFI_NOT_FOUND         Always return not found.
**/
EFI_STATUS
EFIAPI
BootMaintLoadFile (
  IN EFI_LOAD_FILE_PROTOCOL           *This,
  IN EFI_DEVICE_PATH_PROTOCOL         *FilePath,
  IN BOOLEAN                          BootPolicy,
  IN OUT UINTN                        *BufferSize,
  IN VOID                             *Buffer OPTIONAL
  )
{
  EFI_STATUS                          Status;
  EFI_BROWSER_ACTION_REQUEST          ActionRequest;

  Status = InitializeBM ();
  if (!EFI_ERROR (Status)) {
    //
    // Dispatch File Explorer formset.
    //
    mBmmCallbackInfo->FeCurrentState = FileExplorerStateBootFromFile;
    UpdateFileExplorer (mBmmCallbackInfo, 0);

    mBootMaintSendFormBySelf = TRUE;
    ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
    Status = gBMFormBrowser2->SendForm (
                                gBMFormBrowser2,
                                &mBmmCallbackInfo->FeHiiHandle,
                                1,
                                &gFileExploreFormSetGuid,
                                0,
                                NULL,
                                &ActionRequest
                                );
    if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
      EnableResetRequired ();
    }
    mBootMaintSendFormBySelf = FALSE;
  }
  ShutdownBM ();

  return EFI_NOT_FOUND;
}

/**
 Initialize HII data.

 @param[in] Event          A pointer to the Event that triggered the callback.
 @param[in] Handle         Checkpoint handle.
**/
VOID
EFIAPI
BootMaintCpInitNotifyFunc (
  IN EFI_EVENT                        Event,
  IN H2O_CP_HANDLE                    Handle
  )
{
  EFI_STATUS                          Status;

  H2OCpUnregisterHandler (Handle);

  mBmmCallbackInfo = AllocateZeroPool (sizeof (BMM_CALLBACK_DATA));
  if (mBmmCallbackInfo == NULL) {
    return;
  }

  mBmmCallbackInfo->Signature                     = BMM_CALLBACK_DATA_SIGNATURE;
  mBmmCallbackInfo->BmmConfigAccess.ExtractConfig = BootMaintExtractConfig;
  mBmmCallbackInfo->BmmConfigAccess.RouteConfig   = BMFakeRouteConfig;
  mBmmCallbackInfo->BmmConfigAccess.Callback      = BootMaintCallback;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mBmmCallbackInfo->BmmDriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mBmmHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mBmmCallbackInfo->BmmConfigAccess,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  mBmmCallbackInfo->FeConfigAccess.ExtractConfig = BMFakeExtractConfig;
  mBmmCallbackInfo->FeConfigAccess.RouteConfig   = BMFakeRouteConfig;
  mBmmCallbackInfo->FeConfigAccess.Callback      = FileExplorerCallback;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mBmmCallbackInfo->FeDriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mFeHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mBmmCallbackInfo->FeConfigAccess,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Post our Boot Maint VFR binary to the HII database.
  //
  mBmmCallbackInfo->BmmHiiHandle = HiiAddPackages (
                                     &gBootMaintFormSetGuid,
                                     mBmmCallbackInfo->BmmDriverHandle,
                                     BmBin,
                                     BootMaintDxeStrings,
                                     BootMaintDxeImages,
                                     NULL
                                     );
  ASSERT (mBmmCallbackInfo->BmmHiiHandle != NULL);

  //
  // Post our File Explorer VFR binary to the HII database.
  //
  mBmmCallbackInfo->FeHiiHandle = HiiAddPackages (
                                    &gFileExploreFormSetGuid,
                                    mBmmCallbackInfo->FeDriverHandle,
                                    FEBin,
                                    BootMaintDxeStrings,
                                    BootMaintDxeImages,
                                    NULL
                                    );
  ASSERT (mBmmCallbackInfo->FeHiiHandle != NULL);
}

/**
 Install the instance of the EFI_LOAD_FILE_PROTOCOL and EFI_DEVICE_PATH_PROTOCOL on new handle.

 @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 @param[in] SystemTable    A pointer to the EFI System Table.

 @retval EFI_SUCCESS       Install protocol successfully.
 @retval other             Fail to install protocol instance.
**/
EFI_STATUS
EFIAPI
BootMaintEntry (
  IN EFI_HANDLE                       ImageHandle,
  IN EFI_SYSTEM_TABLE                 *SystemTable
  )
{
  EFI_HANDLE                          DriverHandle;
  EFI_STATUS                          Status;

  if (FeaturePcdGet (PcdH2OBdsCpInitSupported)) {
    H2O_CP_HANDLE                     CpHandle;
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpInitGuid,
               BootMaintCpInitNotifyFunc,
               H2O_CP_MEDIUM,
               &CpHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpInitGuid, Status));
      return Status;
    }
    DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpInitGuid, Status));
  }

  DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &gH2OBootMaintenanceDevicePath,
                  &gEfiLoadFileProtocolGuid,
                  &mBootMaintLoadFile,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
