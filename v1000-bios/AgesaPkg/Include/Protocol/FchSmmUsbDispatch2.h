/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD FCH SMM USB Dispacther Protocol
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
#ifndef _FCH_SMM_USB_DISPATCH2_PROTOCOL_H_
#define _FCH_SMM_USB_DISPATCH2_PROTOCOL_H_

#include <Protocol/SmmUsbDispatch2.h>

extern EFI_GUID gFchSmmUsbDispatch2ProtocolGuid;

typedef struct _FCH_SMM_USB_DISPATCH2_PROTOCOL FCH_SMM_USB_DISPATCH2_PROTOCOL;

/// FCH USB SMM Register Context structure
typedef struct {
  EFI_USB_SMI_TYPE          Type;           ///< Legacy Type of support
  EFI_DEVICE_PATH_PROTOCOL  *Device;        ///< USB controllers
  UINT8                     Order;          ///< Priority 0-Highest (reserved), 0xFF-Lowest (reserved)
} FCH_SMM_USB_REGISTER_CONTEXT;


typedef
EFI_STATUS
(EFIAPI *FCH_SMM_USB_HANDLER_ENTRY_POINT2) (
  IN       EFI_HANDLE                         DispatchHandle,
  IN CONST FCH_SMM_USB_REGISTER_CONTEXT       *UsbRegisterContext,
  IN OUT   VOID                               *CommBuffer,
  IN OUT   UINTN                              *CommBufferSize
  );

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_USB_DISPATCH2_REGISTER) (
  IN CONST FCH_SMM_USB_DISPATCH2_PROTOCOL        *This,
  IN       FCH_SMM_USB_HANDLER_ENTRY_POINT2      DispatchFunction,
  IN CONST FCH_SMM_USB_REGISTER_CONTEXT          *UsbRegisterContext,
     OUT   EFI_HANDLE                            *DispatchHandle
);

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_USB_DISPATCH2_UNREGISTER) (
  IN CONST FCH_SMM_USB_DISPATCH2_PROTOCOL        *This,
  IN       EFI_HANDLE                            DispatchHandle
);

struct  _FCH_SMM_USB_DISPATCH2_PROTOCOL {
  FCH_SMM_USB_DISPATCH2_REGISTER    Register;
  FCH_SMM_USB_DISPATCH2_UNREGISTER  UnRegister;
};
#endif

