/** @file

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef __ACPI_PLATFORM_
#define __ACPI_PLATFORM_

#include <Uefi.h>
#include <Protocol/AcpiSupport.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>

typedef UINT32  EFI_ACPI_TABLE_VERSION;
typedef VOID    *EFI_ACPI_HANDLE;

#define EFI_ACPI_TABLE_VERSION_NONE (1 << 0)
#define EFI_ACPI_TABLE_VERSION_1_0B (1 << 1)
#define EFI_ACPI_TABLE_VERSION_2_0  (1 << 2)
#define EFI_ACPI_TABLE_VERSION_3_0  (1 << 3)
#define EFI_ACPI_TABLE_VERSION_4_0  (1 << 4)

extern EFI_ACPI_SUPPORT_PROTOCOL *mAcpiSupport;

EFI_STATUS
UpdateOemTableID (
  VOID
  );

EFI_STATUS
CrbUpdateOemDsdt (
  VOID
  );

#endif