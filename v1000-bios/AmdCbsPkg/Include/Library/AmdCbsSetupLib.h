/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef __CBS_CALLBACK_LIB_H__
#define __CBS_CALLBACK_LIB_H__

//
// This is the generated IFR binary data for each formset defined in VFR.
// This data array is ready to be used as input of HiiAddPackages() to
// create a packagelist (which contains Form packages, String packages, etc).
//
extern UINT8  AmdCbsFormBin[];

//
// This is the generated String package data for all .UNI files.
// This data array is ready to be used as input of HiiAddPackages() to
// create a packagelist (which contains Form packages, String packages, etc).
//
extern UINT8  CbsSetupLibInstanceStrings[];

extern UINTN  CbsVariableSize;

EFI_STATUS
AmdCbsSetupCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT UINT8                                  *IfrData,
  IN  EFI_HII_HANDLE                         HiiHandle
  );

VOID
CbsWriteDefalutValue (
  IN UINT8   *IfrData
  );

VOID
AmdSaveCbsConfigData (
  IN UINT8   *IfrData
  );

VOID
CbsComboIdentify (
  IN UINT8   *IfrData
  );

EFI_STATUS
EFIAPI
CbsSetupLoadDefaultFunc (
  );

BOOLEAN
CbsFamilyIdentify (
  );

typedef struct {
  UINT16 RawFamily;                ///< CPUID Fn0000_0001_EAX[BaseFamily] + [ExtFamily]
  UINT8  RawExtModel;              ///< CPUID Fn0000_0001_EAX[ExtModel]
  UINT8  RawBaseModel;             ///< CPUID Fn0000_0001_EAX[BaseModel]
  UINT8  RawStepping;              ///< CPUID Fn0000_0001_EAX[Stepping]
  UINT8  RawPkgType;               ///< CPUID Fn0000_0001_EBX[PkgType]
  UINT8  ComboFlag;
} COMBO_FLAG_XLAT;

#define COMBO_FLAG_UNKNOWN  254
#define COMBO_FLAG_RESERVED 255
#define COMBO_FLAG_XLAT_TERMINATOR   {'x', 'x', 'x', 'x', 'x', COMBO_FLAG_UNKNOWN}

#endif //__CBS_CALLBACK_LIB_H__

