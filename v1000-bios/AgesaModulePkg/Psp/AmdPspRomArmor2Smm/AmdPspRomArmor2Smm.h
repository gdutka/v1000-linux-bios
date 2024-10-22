/*****************************************************************************
 *
 * Copyright (C) 2020-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#ifndef PSP_ROMARMOR2_SMM_H_
#define PSP_ROMARMOR2_SMM_H_

#include <PiDxe.h>
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SpiSmmNorFlash.h>
#include <Protocol/PspMboxSmmBufferAddressProtocol.h>
#include <Library/AmdPspMboxLibV2.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Protocol/FchSmmSwDispatch2.h>

#pragma pack (push, 1)
typedef enum _REGION_ACCESS {
  READ_ACCESS_ONLY     = 0,
  READ_WRITE_ACCESS    = 1,
} REGION_ACCESS;

typedef struct _ROMARMOR_BIOS_NVSTORE {
  UINT8    *ShadowBase;                ///< Shadow copy of BIOS NvStore base
  UINT32    LABase;                    ///< Relative Base of BIOS NvStore
  UINT32    Size;                      ///< Size of BIOS NvStore
} ROMARMOR_BIOS_NVSTORE;

#pragma pack (pop)

#undef  GET_MASK64
#define GET_MASK64(HighBit, LowBit) ((((UINT64) 1 << (HighBit - LowBit + 1)) - 1) << LowBit)

#define MSR_SMMADDR                 0xC0010112ul
#define MSR_SMMADDR_TSEGBASE_BITS   GET_MASK64 (39, 17)

#define MSR_SMMMASK                 0xC0010113ul

#define ACPI_MMIO_BASE              0xFED80000ul
#define SMI_BASE                    0x200        // DWORD
#define PMIO_BASE                   0x300        // DWORD
#define FCH_PMIOA_REG6A             0x6A         // AcpiSmiCmd
#define FCH_SMI_REG84               0x84         // SmiStatus1
#define FCH_SMI_REG98               0x98         // SmiTrig
#define FCH_SMI_REGA8               0xA8

#endif // PSP_ROMARMOR2_SMM_H_
