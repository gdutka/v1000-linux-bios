/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD FCH POWER BUTTON Dispacther Protocol
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD FCH UEFI Drivers
 * @e sub-project:  Protocols
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/******************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
**/
#ifndef _FCH_SMM_PWRBTN_DISPATCH2_PROTOCOL_H_
#define _FCH_SMM_PWRBTN_DISPATCH2_PROTOCOL_H_

#include <Protocol/SmmPowerButtonDispatch2.h>

extern EFI_GUID gFchSmmPwrBtnDispatch2ProtocolGuid;

typedef struct _FCH_SMM_PWRBTN_DISPATCH2_PROTOCOL FCH_SMM_PWRBTN_DISPATCH2_PROTOCOL;

/// FCH Power Button SMM Register Context structure
typedef struct {
  EFI_POWER_BUTTON_PHASE           Phase;   ///< Power button phase
  UINT8                            Order;   ///< Priority 0-Highest (reserved), 0xFF-Lowest (reserved)
} FCH_SMM_PWRBTN_REGISTER_CONTEXT;


typedef
EFI_STATUS
(EFIAPI *FCH_SMM_PWRBTN_HANDLER_ENTRY_POINT2) (
  IN       EFI_HANDLE                               DispatchHandle,
  IN CONST FCH_SMM_PWRBTN_REGISTER_CONTEXT          *DispatchContext,
  IN OUT   VOID                                     *CommBuffer OPTIONAL,
  IN OUT   UINTN                                    *CommBufferSize  OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_PWRBTN_DISPATCH2_REGISTER) (
  IN CONST FCH_SMM_PWRBTN_DISPATCH2_PROTOCOL        *This,
  IN       FCH_SMM_PWRBTN_HANDLER_ENTRY_POINT2      DispatchFunction,
  IN       FCH_SMM_PWRBTN_REGISTER_CONTEXT          *RegisterContext,
     OUT   EFI_HANDLE                               *DispatchHandle
);

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_PWRBTN_DISPATCH2_UNREGISTER) (
  IN CONST FCH_SMM_PWRBTN_DISPATCH2_PROTOCOL        *This,
  IN       EFI_HANDLE                               DispatchHandle
);

struct  _FCH_SMM_PWRBTN_DISPATCH2_PROTOCOL {
  FCH_SMM_PWRBTN_DISPATCH2_REGISTER    Register;
  FCH_SMM_PWRBTN_DISPATCH2_UNREGISTER  UnRegister;
};
#endif

