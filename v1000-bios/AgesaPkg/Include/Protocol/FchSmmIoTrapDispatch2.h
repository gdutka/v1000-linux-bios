/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD FCH IO Trap dispacther Protocol
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
#ifndef _FCH_SMM_IOTRAP_DISPATCH2_PROTOCOL_H_
#define _FCH_SMM_IOTRAP_DISPATCH2_PROTOCOL_H_

#include <Protocol/SmmIoTrapDispatch2.h>

extern EFI_GUID gFchSmmIoTrapDispatch2ProtocolGuid;

typedef struct _FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL;


/// SMM I/O trap register context
typedef struct {
  UINT16                          Address;     ///< 16 bit address
  UINT16                          Length;      ///< Length
  EFI_SMM_IO_TRAP_DISPATCH_TYPE   Type;        ///< I/O trap dispatch type
} FCH_SMM_IO_TRAP_REGISTER_CONTEXT;


typedef
EFI_STATUS
(EFIAPI *FCH_SMM_IO_TRAP_HANDLER_ENTRY_POINT2) (
  IN       EFI_HANDLE                              DispatchHandle,
  IN CONST FCH_SMM_IO_TRAP_REGISTER_CONTEXT        *RegisterContext,
  IN OUT   EFI_SMM_IO_TRAP_CONTEXT                 *IoTrapContext,
  IN OUT   UINTN                                   *SizeOfIoTrapContext
  );

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_IO_TRAP_DISPATCH2_REGISTER) (
  IN CONST FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL      *This,
  IN       FCH_SMM_IO_TRAP_HANDLER_ENTRY_POINT2    DispatchFunction,
  IN OUT   FCH_SMM_IO_TRAP_REGISTER_CONTEXT        *RegisterContext,
     OUT   EFI_HANDLE                              *DispatchHandle
);

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_IO_TRAP_DISPATCH2_UNREGISTER) (
  IN CONST FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL      *This,
  IN       EFI_HANDLE                              DispatchHandle
);

/// FCH IO trap protocol
struct  _FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL {
  FCH_SMM_IO_TRAP_DISPATCH2_REGISTER        Register;
  FCH_SMM_IO_TRAP_DISPATCH2_UNREGISTER      UnRegister;
};

#endif

