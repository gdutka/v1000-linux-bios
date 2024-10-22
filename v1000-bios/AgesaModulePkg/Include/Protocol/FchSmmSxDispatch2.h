/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD FCH SMM Sleep Type Dispacther Protocol
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
#ifndef _FCH_SMM_SX_DISPATCH2_PROTOCOL_H_
#define _FCH_SMM_SX_DISPATCH2_PROTOCOL_H_

#include <Protocol/SmmSxDispatch2.h>

extern EFI_GUID gFchSmmSxDispatch2ProtocolGuid;

typedef struct _FCH_SMM_SX_DISPATCH2_PROTOCOL FCH_SMM_SX_DISPATCH2_PROTOCOL;

///
/// AMD FCH SMM Sx Register Context
///
typedef struct {
  EFI_SLEEP_TYPE       Type;        ///< Sleep type
  EFI_SLEEP_PHASE      Phase;       ///< Sleep phase
  UINT8                Order;       ///< Sleep order
} FCH_SMM_SX_REGISTER_CONTEXT;


typedef
EFI_STATUS
(EFIAPI *FCH_SMM_SX_HANDLER_ENTRY_POINT2) (
  IN       EFI_HANDLE                       DispatchHandle,
  IN CONST FCH_SMM_SX_REGISTER_CONTEXT      *DispatchContext,
  IN OUT   VOID                             *CommBuffer OPTIONAL,
  IN OUT   UINTN                            *CommBufferSize  OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_SX_DISPATCH2_REGISTER) (
  IN CONST FCH_SMM_SX_DISPATCH2_PROTOCOL       *This,
  IN       FCH_SMM_SX_HANDLER_ENTRY_POINT2     DispatchFunction,
  IN CONST FCH_SMM_SX_REGISTER_CONTEXT         *RegisterContext,
     OUT   EFI_HANDLE                          *DispatchHandle
);

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_SX_DISPATCH2_UNREGISTER) (
  IN CONST FCH_SMM_SX_DISPATCH2_PROTOCOL     *This,
  IN       EFI_HANDLE                        DispatchHandle
);

struct  _FCH_SMM_SX_DISPATCH2_PROTOCOL {
  FCH_SMM_SX_DISPATCH2_REGISTER              Register;
  FCH_SMM_SX_DISPATCH2_UNREGISTER            UnRegister;
};
#endif

