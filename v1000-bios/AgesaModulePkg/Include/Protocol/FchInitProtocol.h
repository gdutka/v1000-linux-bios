/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH DXE
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _FCH_INIT_PROTOCOL_H_
#define _FCH_INIT_PROTOCOL_H_


extern EFI_GUID gFchInitProtocolGuid;

typedef struct _FCH_INIT_PROTOCOL FCH_INIT_PROTOCOL;
/// FCH USB Over Current Data Block
typedef struct {
  UINT32       ChipType;                           ///< Bitfield: Applied chip types
  UINT8        Usb1OcPinMap[5];       ///< USB1 over current pin mapping
  UINT8        Usb2OcPinMap[5];       ///< USB2 over current pin mapping
  UINT8        Usb3OcPinMap[4];       ///< USB3 over current pin mapping
  UINT8        Usb4OcPinMap[2];       ///< USB4 over current pin mapping
  UINT8        Xhci0OcPinMap[2];      ///< XHCI0 over current pin mapping
  UINT8        Xhci1OcPinMap[2];      ///< XHCI1 over current pin mapping
} USB_OC;


//
// Protocol prototypes
//
typedef
VOID
(EFIAPI *FP_FCH_USB_OC) (
  USB_OC      *UsbOc
  );

//
// Rev2 for USB OC mapping after ZP
//
typedef EFI_STATUS (EFIAPI *FP_FCH_USB_OC2) (
  IN       CONST FCH_INIT_PROTOCOL   *This,              ///< FCH INIT Protocol
  IN       UINT8                     Socket,             ///< CPU/Socket number in system
  IN       UINT8                     Port,               ///< Bitmap (Bit0 - Port0, Bit1 - Port1, etc.) to disable USB3 ports
  IN       UINT8                     OCPin               ///< Bitmap (Bit0 - Port0, Bit1 - Port1, etc.) to disable USB2 ports
);

//
// USB port disable function
//
typedef EFI_STATUS (EFIAPI *FP_FCH_USB_PORT_DISABLE2) (
  IN       CONST FCH_INIT_PROTOCOL   *This,              ///< FCH INIT Protocol
  IN       UINT8                     Socket,             ///< CPU/Socket number in system
  IN       UINT32                    USB3DisableMap,     ///< Bitmap (Bit0 - Port0, Bit1 - Port1, etc.) to disable USB3 ports
  IN       UINT32                    USB2DisableMap      ///< Bitmap (Bit0 - Port0, Bit1 - Port1, etc.) to disable USB2 ports
);


/// FCH INIT Protocol
typedef struct _FCH_INIT_PROTOCOL {
  UINTN                     Revision;                 ///< Protocol Revision
  UINTN                     FchRev;                   ///< FCH Revision
  VOID                      *FchPolicy;               ///< Fch Config Data Block
  VOID                      *FchPtPolicy;             ///< PT Data Block
  FP_FCH_USB_OC             FpUsbOverCurrent;         ///< Service: USB Over Current
  FP_FCH_USB_OC2            FpUsbOverCurrent2;        ///< Service: USB Over Current2
  FP_FCH_USB_PORT_DISABLE2  FpUsbPortDisable2;        ///< Service: USB Port Disable2
} FCH_INIT_PROTOCOL;

// current PPI revision
#define FCH_INIT_REV  0x01

#endif // _FCH_INIT_PROTOCOL_H_

