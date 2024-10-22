/** @file
  Definition for Smm OEM Services H2OUve Lib.

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

#ifndef _SMM_OEM_SVC_H2OUVE_LIB_H_
#define _SMM_OEM_SVC_H2OUVE_LIB_H_

#include <Protocol/VariableEditBootTypeInfo.h>

#define STRING_COUNT_MAX                      0x80

#define DONT__CARE                            (0xFFFFFFFF)
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr)                       (sizeof(arr) / sizeof((arr)[0]))
#endif
#define CHECK_CONDITION(reg, mask, val)       (((DONT__CARE) == (mask)) || (((reg) & (mask)) == (val)))
#define BS_RT_NV_VAR_ATTR                     (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)
#define VARIABLE_BUFFER_TO_DATA(p)            ((VOID *)(((UINT8 *)(&p->VariableName[0])) + (((p)->VariableNameLength + 1) * sizeof(CHAR16))))

//
// The return code for the Result of RESULT_LIST_RESULT_DATA which was defined in InsydeModulePkg/Include/H2OIhisi.h.
//
enum {
  CALLBACK_RET_SUCCESS = 0,
  CALLBACK_RET_FAIL,
  CALLBACK_RET_SKIP_WRITE_VAR, // Success but do NOT write variable
  CALLBACK_RET_NOT_SUPPORT
};

#define field_size_of(s,m)                    (sizeof(((s *)0)->m))
#define RESULT_SIZE(c)                        (field_size_of(VATS_RESULT_LIST_STRUCTURE ,ListCount) + c * sizeof(RESULT_LIST_RESULT_DATA) )

typedef enum {
  _UVE__52H_ = 0x00005200, // AH=52h, H2O UEFI Variable Edit SMI service - Confirm the legality of the variables
  _UVE__53H_ = 0x00005300, // AH=53h, H2O UEFI Variable Edit SMI service - Boot information-related services
  _AH__MASK_ = 0x0000FF00,

  _ECX__01H_ = 0x00000001, // ECH=01h, Get Current Boot option order information
  _ECX__02H_ = 0x00000002, // ECH=02h, Set Current Boot option order sequence
  _ECX__03H_ = 0x00000003, // ECH=03h, Get Current Boot type order information
  _ECX__04H_ = 0x00000004, // ECH=04h, Set Current Boot type order sequence
  _ECX__05H_ = 0x00000005, // ECH=05h, Get Current Boot type name
  _ECX_MASK_ = 0x000000FF,
} H2OUVE_RAX_MASK;

#pragma pack(1)

typedef
EFI_STATUS
(EFIAPI * H2OUVE_SERVICE_FUNCTION_POINT) (
  IN CONST UINT64               Rax,
  IN CONST UINT64               Rcx,
  IN CONST UINT64               Rsi,
  IN CONST UINT64               Rdi
  );

typedef struct _H2OUVE_SERVICE_FUNCTION_MAP_TABLE {
  UINT64                        MaskRax;
  UINT64                        ValueRax;
  UINT64                        MaskRcx;
  UINT64                        ValueRcx;
  UINT64                        MaskRsi;
  UINT64                        ValueRsi;
  UINT64                        MaskRdi;
  UINT64                        ValueRdi;
  H2OUVE_SERVICE_FUNCTION_POINT FuncPtr;
} H2OUVE_SERVICE_FUNCTION_MAP_TABLE;

#pragma pack()

VOID
EFIAPI
UpdateVariableEditBootTypeInfo  (
  VOID
);

EFI_STATUS 
EFIAPI 
H2OSmiCallBack (
  VOID
  );


EFI_STATUS
EFIAPI
SetupCallbackSyncCallbackIhisi (
  VOID
);

EFI_STATUS
BootInfoService (
  VOID
  );

#endif
