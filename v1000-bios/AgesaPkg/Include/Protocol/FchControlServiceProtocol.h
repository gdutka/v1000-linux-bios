/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH CONTROL SERVICE PROTOCOL
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

#ifndef _FCH_CONTROL_SERVICE_PROTOCOL_H_
#define _FCH_CONTROL_SERVICE_PROTOCOL_H_


extern EFI_GUID gFchControlServiceProtocolGuid;

typedef struct _FCH_CONTROL_SERVICE_PROTOCOL FCH_CONTROL_SERVICE_PROTOCOL;

//
// Protocol prototypes
//


//
// Get Fch Acpi Mmio Base Address
//
typedef EFI_STATUS (EFIAPI *FP_FCH_GET_ACPI_MMIO) (
  IN       CONST FCH_CONTROL_SERVICE_PROTOCOL   *This,              ///< FCH CONTROL SERVICE Protocol
  IN       UINT8                                FchDie,             ///< Fch Die number
     OUT   UINT64                               *AcpiMmioBase       ///< Fch Acpi Mmio Base
);


/// FCH Control Service Protocol
typedef struct _FCH_CONTROL_SERVICE_PROTOCOL {
  UINTN                     Revision;                 ///< Protocol Revision
  UINTN                     FchRev;                   ///< FCH Revision
  FP_FCH_GET_ACPI_MMIO      FpGetAcpiMmioBase;        ///< Service: Get Fch Acpi Mmio Base
} FCH_CONTROL_SERVICE_PROTOCOL;

// current Protocol revision
#define FCH_CONTROL_SERVICE_PROTOCOL_REV  0x01

#endif // _FCH_CONTROL_SERVICE_PROTOCOL_H_

