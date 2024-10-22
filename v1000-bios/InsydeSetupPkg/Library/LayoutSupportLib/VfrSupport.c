/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "LayoutSupportLib.h"

EFI_STATUS
GetImportInfoList (
  IN  EFI_GUID                                *FormsetGuid,
  OUT IMPORT_INFO                             **ImportInfoList,
  OUT UINT16                                  *ImportInfoCount
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *VfrLink;
  LIST_ENTRY                                   *FormSetLink;
  LIST_ENTRY                                   *FormLink;
  LIST_ENTRY                                   *ImportLink;


  H2O_VFR_INFO                                 *VfrInfo;
  H2O_FORMSET_INFO                             *FormSetInfo;
  H2O_FORM_INFO                                *FormInfo;
  H2O_IMPORT_INFO                              *ImportInfo;


  IMPORT_INFO                                  *ImportList;
  UINT16                                       ImportCount;

  LIST_ENTRY                                   *LayoutListHead;
  LIST_ENTRY                                   *VfrListHead;

  Status = GetLayoutTreeAndVfrTree (&LayoutListHead, &VfrListHead);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status) || VfrListHead == NULL) {
    return EFI_NOT_FOUND;
  }

  ASSERT (FormsetGuid != NULL);
  ASSERT (ImportInfoCount != NULL);
  if ((FormsetGuid == NULL) || (ImportInfoCount == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ImportCount = 0;
  VfrLink = GetFirstNode (VfrListHead);
  while (!IsNull (VfrListHead, VfrLink)) {
    VfrInfo = H2O_VFR_INFO_NODE_FROM_LINK (VfrLink);
    FormSetLink = GetFirstNode ( &VfrInfo->FormsetListHead);
    while (!IsNull (&VfrInfo->FormsetListHead, FormSetLink)) {
      FormSetInfo = H2O_FORMSET_INFO_NODE_FROM_LINK (FormSetLink);
      if (CompareGuid (&(FormSetInfo->FormsetId), FormsetGuid)) {
        FormLink = GetFirstNode (&FormSetInfo->FormListHead);
        while (!IsNull (&FormSetInfo->FormListHead, FormLink)) {
          FormInfo = H2O_FORM_INFO_NODE_FROM_LINK (FormLink);
          ImportLink = GetFirstNode (&FormInfo->ImportListHead);
          while (!IsNull (&FormInfo->ImportListHead, ImportLink)) {
            ImportCount++;
            ImportLink = GetNextNode (&FormInfo->ImportListHead, ImportLink);
          }
          FormLink = GetNextNode (&FormSetInfo->FormListHead, FormLink);
        }
      }
      FormSetLink = GetNextNode (&VfrInfo->FormsetListHead, FormSetLink);
    }
    VfrLink = GetNextNode (VfrListHead, VfrLink);
  }

  if (ImportCount == 0) {
    return EFI_NOT_FOUND;
  }

  ImportList = AllocateZeroPool (ImportCount * sizeof (IMPORT_INFO));
  if (ImportList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *ImportInfoList  = ImportList;
  *ImportInfoCount = ImportCount;

  VfrLink = GetFirstNode (VfrListHead);
  while (!IsNull (VfrListHead, VfrLink)) {
    VfrInfo = H2O_VFR_INFO_NODE_FROM_LINK (VfrLink);
    VfrLink = GetNextNode (VfrListHead, VfrLink);

    FormSetLink = GetFirstNode (&VfrInfo->FormsetListHead);
    while (!IsNull (&VfrInfo->FormsetListHead, FormSetLink)) {
      FormSetInfo = H2O_FORMSET_INFO_NODE_FROM_LINK (FormSetLink);
      FormSetLink = GetNextNode (&VfrInfo->FormsetListHead, FormSetLink);

      if (CompareGuid (&(FormSetInfo->FormsetId), FormsetGuid)) {
        FormLink = GetFirstNode (&FormSetInfo->FormListHead);

        while (!IsNull (&FormSetInfo->FormListHead, FormLink)) {
          FormInfo = H2O_FORM_INFO_NODE_FROM_LINK (FormLink);
          FormLink = GetNextNode (&FormSetInfo->FormListHead, FormLink);

          ImportLink = GetFirstNode (&FormInfo->ImportListHead);
          while (!IsNull (&FormInfo->ImportListHead, ImportLink)) {
            ImportInfo = H2O_IMPORT_INFO_NODE_FROM_LINK (ImportLink);

            ImportList->Flags     = ImportInfo->Flags;
            CopyGuid (&ImportList->SrcFormSetGuid, &ImportInfo->TargetFormsetGuid);
            ImportList->SrcFormId = ImportInfo->TargetFormId;
            ImportList->SrcId     = ImportInfo->TargetId;
            ImportList->DstFormId = (UINT16)FormInfo->FormBlock->FormId;
            ImportList->DstId     = ImportInfo->LocalId;
            ImportList++;

            ImportLink = GetNextNode (&FormInfo->ImportListHead, ImportLink);
          }
        }
      }
    }

  }

  return EFI_SUCCESS;
}

UINT32
GetStyleTypeByOpCode (
  IN    UINT8                                  OpCode
  )
{
  switch (OpCode) {

  case EFI_IFR_SUBTITLE_OP:
    return H2O_IFR_STYLE_TYPE_SUBTITLE;

  case EFI_IFR_CHECKBOX_OP:
    return H2O_IFR_STYLE_TYPE_CHECKBOX;

  case EFI_IFR_TEXT_OP:
    return H2O_IFR_STYLE_TYPE_TEXT;

  case EFI_IFR_ACTION_OP:
    return H2O_IFR_STYLE_TYPE_ACTION;

  case EFI_IFR_REF_OP:
    return H2O_IFR_STYLE_TYPE_GOTO;

  case EFI_IFR_PASSWORD_OP:
    return H2O_IFR_STYLE_TYPE_PASSWORD;

  case EFI_IFR_NUMERIC_OP:
    return H2O_IFR_STYLE_TYPE_NUMERIC;

  case EFI_IFR_ONE_OF_OP:
    return H2O_IFR_STYLE_TYPE_ONEOF;

  case EFI_IFR_TIME_OP:
    return H2O_IFR_STYLE_TYPE_TIME;

  case EFI_IFR_DATE_OP:
    return H2O_IFR_STYLE_TYPE_DATE;

  case EFI_IFR_ORDERED_LIST_OP:
    return H2O_IFR_STYLE_TYPE_ORDEREDLIST;

  case EFI_IFR_RESET_BUTTON_OP:
    return H2O_IFR_STYLE_TYPE_RESETBUTTON;

  case EFI_IFR_STRING_OP:
    return H2O_IFR_STYLE_TYPE_STRING;

  default:
    break;
  }

  ASSERT (FALSE);
  return H2O_IFR_STYLE_TYPE_SHEET;
}

EFI_STATUS
GetVfcfFormsetLayoutId (
  IN  EFI_GUID                                 *FormsetGuid,
  OUT UINT32                                   *FormsetLayoutId
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *LayoutListHead;
  LIST_ENTRY                                   *VfrListHead;
  LIST_ENTRY                                   *VfrLink;
  H2O_VFR_INFO                                 *VfrInfo;
  LIST_ENTRY                                   *FormsetListHead;
  LIST_ENTRY                                   *FormsetLink;
  H2O_FORMSET_INFO                             *FormSetInfo;
  UINT32                                       LayoutId;

  Status = GetLayoutTreeAndVfrTree (&LayoutListHead, &VfrListHead);
  if (EFI_ERROR (Status) || VfrListHead == NULL || IsListEmpty (VfrListHead)) {
    return EFI_NOT_FOUND;
  }

  LayoutId = 0;
  for (VfrLink = GetFirstNode (VfrListHead); !IsNull (VfrListHead, VfrLink); VfrLink = GetNextNode (VfrListHead, VfrLink)) {
    VfrInfo = H2O_VFR_INFO_NODE_FROM_LINK (VfrLink);

    FormsetListHead = &VfrInfo->FormsetListHead;
    for (FormsetLink = GetFirstNode (FormsetListHead); !IsNull (FormsetListHead, FormsetLink); FormsetLink = GetNextNode (FormsetListHead, FormsetLink)) {
      FormSetInfo = H2O_FORMSET_INFO_NODE_FROM_LINK (FormsetLink);
      if (FormSetInfo->FormsetBlock->LayoutId != 0 &&
          CompareGuid (&FormSetInfo->FormsetId, FormsetGuid)) {
        LayoutId = FormSetInfo->FormsetBlock->LayoutId;
      }
    }
  }
  if (LayoutId == 0) {
    return EFI_NOT_FOUND;
  }

  *FormsetLayoutId = LayoutId;
  return EFI_SUCCESS;
}

EFI_STATUS
GetVfcfFormsetPropertyValue (
  IN  EFI_GUID                                 *FormsetGuid,
  IN  UINT32                                   StyleType,
  IN  UINT32                                   PseudoClass,
  IN  CHAR8                                    *IdentifierStr,
  IN  RECT                                     *RelativeField OPTIONAL,
  OUT H2O_PROPERTY_INFO                        **PropertyInfo,
  OUT H2O_PROPERTY_VALUE                       *PropertyValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *LayoutListHead;
  LIST_ENTRY                                   *VfrListHead;
  LIST_ENTRY                                   *VfrLink;
  H2O_VFR_INFO                                 *VfrInfo;
  LIST_ENTRY                                   *FormsetListHead;
  LIST_ENTRY                                   *FormsetLink;
  H2O_FORMSET_INFO                             *FormsetInfo;
  EFI_STATUS                                   ResultStatus;
  H2O_PROPERTY_INFO                            *FoundProperty;
  H2O_PROPERTY_VALUE                           FoundPropertyValue;

  if (FormsetGuid == NULL || IdentifierStr == NULL || PropertyInfo == NULL || PropertyValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetLayoutTreeAndVfrTree (&LayoutListHead, &VfrListHead);
  if (EFI_ERROR (Status) || VfrListHead == NULL) {
    return EFI_NOT_FOUND;
  }

  ResultStatus = EFI_NOT_FOUND;
  for (VfrLink = GetFirstNode (VfrListHead); !IsNull (VfrListHead, VfrLink); VfrLink = GetNextNode (VfrListHead, VfrLink)) {
    VfrInfo = H2O_VFR_INFO_NODE_FROM_LINK (VfrLink);

    FormsetListHead = &VfrInfo->FormsetListHead;
    for (FormsetLink = GetFirstNode (FormsetListHead); !IsNull (FormsetListHead, FormsetLink); FormsetLink = GetNextNode (FormsetListHead, FormsetLink)) {
      FormsetInfo = H2O_FORMSET_INFO_NODE_FROM_LINK (FormsetLink);
      if (!CompareGuid (&FormsetInfo->FormsetId, FormsetGuid)) {
        continue;
      }

      Status = GetPropertyFromStyleList (
                 &FormsetInfo->StyleListHead,
                 StyleType,
                 PseudoClass,
                 IdentifierStr,
                 RelativeField,
                 &FoundProperty,
                 &FoundPropertyValue
                 );
      if (!EFI_ERROR (Status)) {
        ResultStatus = EFI_SUCCESS;
        *PropertyInfo = FoundProperty;
        CopyMem (PropertyValue, &FoundPropertyValue, sizeof (H2O_PROPERTY_VALUE));
      }
    }
  }

  return ResultStatus;
}

EFI_STATUS
GetVfcfFormPropertyValue (
  IN  EFI_GUID                                 *FormsetGuid,
  IN  UINT16                                   FormId,
  IN  UINT32                                   StyleType,
  IN  UINT32                                   PseudoClass,
  IN  CHAR8                                    *IdentifierStr,
  IN  RECT                                     *RelativeField OPTIONAL,
  OUT H2O_PROPERTY_INFO                        **PropertyInfo,
  OUT H2O_PROPERTY_VALUE                       *PropertyValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *LayoutListHead;
  LIST_ENTRY                                   *VfrListHead;
  LIST_ENTRY                                   *VfrLink;
  H2O_VFR_INFO                                 *VfrInfo;
  LIST_ENTRY                                   *FormsetListHead;
  LIST_ENTRY                                   *FormsetLink;
  H2O_FORMSET_INFO                             *FormsetInfo;
  LIST_ENTRY                                   *FormListHead;
  LIST_ENTRY                                   *FormLink;
  H2O_FORM_INFO                                *FormInfo;
  EFI_STATUS                                   ResultStatus;
  H2O_PROPERTY_INFO                            *FoundProperty;
  H2O_PROPERTY_VALUE                           FoundPropertyValue;

  if (FormsetGuid == NULL || IdentifierStr == NULL || PropertyInfo == NULL || PropertyValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetLayoutTreeAndVfrTree (&LayoutListHead, &VfrListHead);
  if (EFI_ERROR (Status) || VfrListHead == NULL) {
    return EFI_NOT_FOUND;
  }

  ResultStatus = EFI_NOT_FOUND;
  for (VfrLink = GetFirstNode (VfrListHead); !IsNull (VfrListHead, VfrLink); VfrLink = GetNextNode (VfrListHead, VfrLink)) {
    VfrInfo = H2O_VFR_INFO_NODE_FROM_LINK (VfrLink);

    FormsetListHead = &VfrInfo->FormsetListHead;
    for (FormsetLink = GetFirstNode (FormsetListHead); !IsNull (FormsetListHead, FormsetLink); FormsetLink = GetNextNode (FormsetListHead, FormsetLink)) {
      FormsetInfo = H2O_FORMSET_INFO_NODE_FROM_LINK (FormsetLink);
      if (!CompareGuid (&FormsetInfo->FormsetId, FormsetGuid)) {
        continue;
      }

      FormListHead = &FormsetInfo->FormListHead;
      for (FormLink = GetFirstNode (FormListHead); !IsNull (FormListHead, FormLink); FormLink = GetNextNode (FormListHead, FormLink)) {
        FormInfo = H2O_FORM_INFO_NODE_FROM_LINK (FormLink);
        if (FormInfo->FormBlock->FormId != FormId) {
          continue;
        }

        Status = GetPropertyFromStyleList (
                   &FormInfo->StyleListHead,
                   StyleType,
                   PseudoClass,
                   IdentifierStr,
                   RelativeField,
                   &FoundProperty,
                   &FoundPropertyValue
                   );
        if (!EFI_ERROR (Status)) {
          ResultStatus = EFI_SUCCESS;
          *PropertyInfo = FoundProperty;
          CopyMem (PropertyValue, &FoundPropertyValue, sizeof (H2O_PROPERTY_VALUE));
        }
      }
    }
  }

  return ResultStatus;
}

EFI_STATUS
GetVfcfStatementPropertyValue (
  IN  EFI_GUID                                 *FormsetGuid,
  IN  BOOLEAN                                  IsQuestion,
  IN  UINT16                                   Id,
  IN  UINT32                                   StyleType,
  IN  UINT32                                   PseudoClass,
  IN  CHAR8                                    *IdentifierStr,
  IN  RECT                                     *RelativeField OPTIONAL,
  OUT H2O_PROPERTY_INFO                        **PropertyInfo,
  OUT H2O_PROPERTY_VALUE                       *PropertyValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *LayoutListHead;
  LIST_ENTRY                                   *VfrListHead;
  LIST_ENTRY                                   *VfrLink;
  H2O_VFR_INFO                                 *VfrInfo;
  LIST_ENTRY                                   *FormsetListHead;
  LIST_ENTRY                                   *FormsetLink;
  H2O_FORMSET_INFO                             *FormsetInfo;
  LIST_ENTRY                                   *FormListHead;
  LIST_ENTRY                                   *FormLink;
  H2O_FORM_INFO                                *FormInfo;
  LIST_ENTRY                                   *StatementListHead;
  LIST_ENTRY                                   *StatementLink;
  H2O_STATEMENT_INFO                           *StatementInfo;
  EFI_STATUS                                   ResultStatus;
  H2O_PROPERTY_INFO                            *FoundProperty;
  H2O_PROPERTY_VALUE                           FoundPropertyValue;

  if (FormsetGuid == NULL || IdentifierStr == NULL || PropertyInfo == NULL || PropertyValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetLayoutTreeAndVfrTree (&LayoutListHead, &VfrListHead);
  if (EFI_ERROR (Status) || VfrListHead == NULL) {
    return EFI_NOT_FOUND;
  }

  ResultStatus = EFI_NOT_FOUND;
  for (VfrLink = GetFirstNode (VfrListHead); !IsNull (VfrListHead, VfrLink); VfrLink = GetNextNode (VfrListHead, VfrLink)) {
    VfrInfo = H2O_VFR_INFO_NODE_FROM_LINK (VfrLink);

    FormsetListHead = &VfrInfo->FormsetListHead;
    for (FormsetLink = GetFirstNode (FormsetListHead); !IsNull (FormsetListHead, FormsetLink); FormsetLink = GetNextNode (FormsetListHead, FormsetLink)) {
      FormsetInfo = H2O_FORMSET_INFO_NODE_FROM_LINK (FormsetLink);
      if (!CompareGuid (&FormsetInfo->FormsetId, FormsetGuid)) {
        continue;
      }

      FormListHead = &FormsetInfo->FormListHead;
      for (FormLink = GetFirstNode (FormListHead); !IsNull (FormListHead, FormLink); FormLink = GetNextNode (FormListHead, FormLink)) {
        FormInfo = H2O_FORM_INFO_NODE_FROM_LINK (FormLink);

        StatementListHead = &FormInfo->StatementListHead;
        for (StatementLink = GetFirstNode (StatementListHead); !IsNull (StatementListHead, StatementLink); StatementLink = GetNextNode (StatementListHead, StatementLink)) {
          StatementInfo = H2O_STATEMENT_INFO_NODE_FROM_LINK (StatementLink);
          if (StatementInfo->IsQuestion != IsQuestion ||
              StatementInfo->QuestionId != Id) {
            continue;
          }

          Status = GetPropertyFromStyleList (
                     &StatementInfo->StyleListHead,
                     StyleType,
                     PseudoClass,
                     IdentifierStr,
                     RelativeField,
                     &FoundProperty,
                     &FoundPropertyValue
                     );
          if (!EFI_ERROR (Status)) {
            ResultStatus = EFI_SUCCESS;
            *PropertyInfo = FoundProperty;
            CopyMem (PropertyValue, &FoundPropertyValue, sizeof (H2O_PROPERTY_VALUE));
          }
        }
      }
    }
  }

  return ResultStatus;
}

BOOLEAN
HaveVfcfFormsetInfo (
  IN EFI_GUID                                  *FormsetGuid
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *LayoutListHead;
  LIST_ENTRY                                   *VfrListHead;
  LIST_ENTRY                                   *VfrLink;
  H2O_VFR_INFO                                 *VfrInfo;
  LIST_ENTRY                                   *FormsetListHead;
  LIST_ENTRY                                   *FormsetLink;
  H2O_FORMSET_INFO                             *FormsetInfo;

  Status = GetLayoutTreeAndVfrTree (&LayoutListHead, &VfrListHead);
  if (EFI_ERROR (Status) || VfrListHead == NULL || IsListEmpty (VfrListHead)) {
    return FALSE;
  }

  for (VfrLink = GetFirstNode (VfrListHead); !IsNull (VfrListHead, VfrLink); VfrLink = GetNextNode (VfrListHead, VfrLink)) {
    VfrInfo = H2O_VFR_INFO_NODE_FROM_LINK (VfrLink);

    FormsetListHead = &VfrInfo->FormsetListHead;
    for (FormsetLink = GetFirstNode (FormsetListHead); !IsNull (FormsetListHead, FormsetLink); FormsetLink = GetNextNode (FormsetListHead, FormsetLink)) {
      FormsetInfo = H2O_FORMSET_INFO_NODE_FROM_LINK (FormsetLink);
      if (CompareGuid (&FormsetInfo->FormsetId, FormsetGuid)) {
        return TRUE;
      }
    }
  }

  return FALSE;
}

