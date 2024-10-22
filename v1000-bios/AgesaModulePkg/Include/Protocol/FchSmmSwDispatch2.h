/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD FCH SMM SW Dispacther Protocol
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
#ifndef _FCH_SMM_SW_DISPATCH2_PROTOCOL_H_
#define _FCH_SMM_SW_DISPATCH2_PROTOCOL_H_

#include <Pi/PiSmmCis.h>

extern EFI_GUID gFchSmmSwDispatch2ProtocolGuid;

typedef struct _FCH_SMM_SW_DISPATCH2_PROTOCOL FCH_SMM_SW_DISPATCH2_PROTOCOL;

/// FCH SW SMM Register Context structure
typedef struct {
  UINTN AmdSwValue;                         ///< Soft SMI value
  UINT8 Order;                              ///< Priority 0-Highest (reserved), 0xFF-Lowest (reserved)
} FCH_SMM_SW_REGISTER_CONTEXT;

/// SMM Software SMI Context
typedef struct {
  UINTN    SwSmiCpuIndex;                   ///< 0-based CPU ID
  UINT8    CommandPort;                     ///< SMI Command port
  UINT8    DataPort;                        ///< SMI Data Port
} FCH_SMM_SW_CONTEXT;


typedef
EFI_STATUS
(EFIAPI *FCH_SMM_SW_HANDLER_ENTRY_POINT2) (
  IN       EFI_HANDLE                          DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT         *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                  *SwContext,
  IN OUT   UINTN                               *SizeOfSwContext
);

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_SW_DISPATCH2_REGISTER) (
  IN CONST FCH_SMM_SW_DISPATCH2_PROTOCOL       *This,
  IN       FCH_SMM_SW_HANDLER_ENTRY_POINT2     DispatchFunction,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT         *SwRegisterContext,
     OUT   EFI_HANDLE                          *DispatchHandle
);

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_SW_DISPATCH2_UNREGISTER) (
  IN CONST FCH_SMM_SW_DISPATCH2_PROTOCOL       *This,
  IN       EFI_HANDLE                          DispatchHandle
);

struct  _FCH_SMM_SW_DISPATCH2_PROTOCOL {
  FCH_SMM_SW_DISPATCH2_REGISTER    Register;
  FCH_SMM_SW_DISPATCH2_UNREGISTER  UnRegister;
  UINTN                            MaximumSwiValue;
};


#endif

