/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD FCH GPI Dispacther Protocol
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
#ifndef _FCH_SMM_GPI_DISPATCH2_PROTOCOL_H_
#define _FCH_SMM_GPI_DISPATCH2_PROTOCOL_H_

#include <Protocol/SmmGpiDispatch2.h>

extern EFI_GUID gFchSmmGpiDispatch2ProtocolGuid;

typedef struct _FCH_SMM_GPI_DISPATCH2_PROTOCOL FCH_SMM_GPI_DISPATCH2_PROTOCOL;


typedef EFI_STATUS (EFIAPI *FCH_SMM_GPI_HANDLER_ENTRY_POINT2) (
  IN       EFI_HANDLE                         DispatchHandle,
  IN CONST EFI_SMM_GPI_REGISTER_CONTEXT       *GpiRegisterContext,
  IN OUT   EFI_SMM_GPI_REGISTER_CONTEXT       *CurrentContext,
  IN OUT   UINTN                              *SizeOfCurrentContext
  );

typedef EFI_STATUS (EFIAPI *FCH_SMM_GPI_DISPATCH2_REGISTER) (
  IN CONST FCH_SMM_GPI_DISPATCH2_PROTOCOL     *This,
  IN       FCH_SMM_GPI_HANDLER_ENTRY_POINT2   DispatchFunction,
  IN CONST EFI_SMM_GPI_REGISTER_CONTEXT       *GpiRegisterContext,
     OUT   EFI_HANDLE                         *DispatchHandle
);

typedef EFI_STATUS (EFIAPI *FCH_SMM_GPI_DISPATCH2_UNREGISTER) (
  IN CONST FCH_SMM_GPI_DISPATCH2_PROTOCOL     *This,
  IN       EFI_HANDLE                         DispatchHandle
);

struct  _FCH_SMM_GPI_DISPATCH2_PROTOCOL {
  FCH_SMM_GPI_DISPATCH2_REGISTER      Register;
  FCH_SMM_GPI_DISPATCH2_UNREGISTER    UnRegister;
  UINTN                               NumSupportedGpis;
};

#endif

