/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD FCH APU RAS SMI Dispacther Protocol
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
#ifndef _FCH_SMM_APURAS_DISPATCH_PROTOCOL_H_
#define _FCH_SMM_APURAS_DISPATCH_PROTOCOL_H_

extern EFI_GUID gFchSmmApuRasDispatchProtocolGuid;

typedef struct _FCH_SMM_APURAS_DISPATCH_PROTOCOL FCH_SMM_APURAS_DISPATCH_PROTOCOL;

/// AMD FCH SMM APU RAS Register Context
typedef struct {
  UINT8                     Socket;         ///< Socket number
  UINT8                     Die;            ///< Die number
  UINT8                     Bus;            ///< Bus number
  UINT8                     NbioNumber;     ///< NBIO number
  UINT8                     Order;          ///< Priority 0-Highest (reserved), 0xFF-Lowest (reserved)
} FCH_SMM_APURAS_REGISTER_CONTEXT;

typedef EFI_STATUS (EFIAPI *FCH_SMM_APURAS_HANDLER_ENTRY_POINT) (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       FCH_SMM_APURAS_REGISTER_CONTEXT   *MiscRegisterContext
  );

typedef EFI_STATUS (EFIAPI *FCH_SMM_APURAS_DISPATCH_REGISTER) (
  IN CONST FCH_SMM_APURAS_DISPATCH_PROTOCOL        *This,
  IN       FCH_SMM_APURAS_HANDLER_ENTRY_POINT      CallBackFunction,
  IN OUT   FCH_SMM_APURAS_REGISTER_CONTEXT         *MiscRegisterContext,
  OUT      EFI_HANDLE                              *DispatchHandle
);

typedef EFI_STATUS (EFIAPI *FCH_SMM_APURAS_DISPATCH_UNREGISTER) (
  IN       CONST FCH_SMM_APURAS_DISPATCH_PROTOCOL   *This,
  IN       EFI_HANDLE                               DispatchHandle
);

struct  _FCH_SMM_APURAS_DISPATCH_PROTOCOL {
  FCH_SMM_APURAS_DISPATCH_REGISTER    Register;
  FCH_SMM_APURAS_DISPATCH_UNREGISTER  UnRegister;
};

#endif

