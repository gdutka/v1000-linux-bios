/** @file

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef SIO_ISA_ACPI_INITIALIZED
#define SIO_ISA_ACPI_INITIALIZED

#define SIO_ISA_ACPI_INITIALIZED_PROTOCOL_GUID \
  { \
     0x41b4d617, 0x701e, 0x4ab7, {0xa0, 0x19, 0x5b, 0x9b, 0x1c, 0x2f, 0x26, 0xd1}  \
  }

extern EFI_GUID  gH2OSioIsaAcpiInitializedProtocolGuid;

#endif
