/** @file
  Header file for LoadAmdAhciUefiDriver protocol.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _LOAD_AMD_AHCI_UEFI_DRIVER_H_
#define _LOAD_AMD_AHCI_UEFI_DRIVER_H_

//
// Load AMD AHCI UEFI Driver Protocol GUID
//
#define EFI_LOAD_AMD_AHCI_UEFI_DRIVER_PROTOCOL_GUID \
{ 0xe6a918dd, 0xc2ed, 0x485e, 0x91, 0x3d, 0xdd, 0xbf, 0xaf, 0x34, 0x9c, 0x6c }

extern EFI_GUID gEfiLoadAmdAhciUefiDriverProtocolGuid;

#endif

