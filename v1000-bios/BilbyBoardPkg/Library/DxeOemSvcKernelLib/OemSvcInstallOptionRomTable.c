/** @file
  Provide OEM to install the PCI Option ROM table and Non-PCI Option ROM table.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/PciExpressLib.h>
#include <Fch.h>
#include <SSFch.h>
#include <GNbReg.h>

//[-start-240426-IB18160212-modify-]//
//[-start-141031-IB06720273-add]//
//@ OPROM defines
//[-end-141031-IB06720273-add]//
//
// Global variables for PCI Option Roms
//
#define NULL_ROM_FILE_GUID \
  { \
    0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} \
  }

#define ONBOARD_PCO_VIDEO_OPTION_ROM_FILE_GUID \
  { \
    0x8dfae5d4, 0xb50e, 0x4c10, {0x96, 0xe6, 0xf2, 0xc2, 0x66, 0xca, 0x56, 0x78} \
  }

#define ONBOARD_RV2_VIDEO_OPTION_ROM_FILE_GUID \
  { \
    0x8dfae5d4, 0xb50e, 0x4c10, {0x96, 0xe6, 0xf2, 0xc2, 0x66, 0xca, 0xac, 0xd7} \
  }

#define IDE_RAID_OPTION_ROM_FILE_GUID \
  { \
    0x501737ab, 0x9d1a, 0x4856, {0x86, 0xd3, 0x7f, 0x12, 0x87, 0xfa, 0x5a, 0x55} \
  }

#define AMD_AHCI_OPTION_ROM_FILE_GUID \
  { \
    0x52948e6f, 0x137e, 0x4adc, {0xbb, 0xb7, 0xd1, 0x7d, 0xc0, 0xad, 0x9e, 0xc4} \
  }

#define PXE_UNDI_OPTION_ROM_FILE_GUID \
  { \
    0xC015FC87, 0x3B1F, 0x421A, {0xA8, 0xFB, 0xDA, 0xBD, 0xCC, 0xBE, 0x98, 0xFC} \
  }

#define RTL_8153_ON_XHCI_OPTION_ROM_FILE_GUID \
  { \
    0xc778953d, 0xd04d, 0x4ae9, {0x94, 0xa8, 0x19, 0xdc, 0xf5, 0xa9, 0x64, 0xef} \
  }
#define PXE_BCM5762_OPTION_ROM_FILE_GUID \
  { \
    0xb7c21de4, 0x5a33, 0x46bb, {0xa2, 0x2e, 0xed, 0x6c, 0xd9, 0x9f, 0x5e, 0xdc} \
  }
#define PXE_RTL8111EP_OPTION_ROM_FILE_GUID \
  { \
    0xb591816f, 0xd392, 0x477b, {0x95, 0x33, 0xd, 0xd0, 0xd3, 0x9d, 0xf6, 0x69} \
  }

//
// Global variables for Non-PCI Option Roms
//
#define SYSTEM_ROM_FILE_GUID \
  { \
    0x1547B4F3, 0x3E8A, 0x4FEF, {0x81, 0xC8, 0x32, 0x8E, 0xD6, 0x47, 0xAB, 0x1A} \
  }

//
// STMicro AT19Wp18 TPM1.2 chip
//
#define TPM12_STM19WP18_GUID \
  { \
    0x4B8D2F76, 0xD9E7, 0x46dc, {0xB5, 0xED, 0xEF, 0xEF, 0x94, 0x76, 0xAF, 0x4A} \
  }
//[-end-240426-IB18160212-modify-]//

//=======================================
//         Pci Option Rom Table
//=======================================
//[-start-141031-IB06720273-add]//
//@ PCI OPROM table
//[-end-141031-IB06720273-add]//
PCI_OPTION_ROM_TABLE      PciOptionRomTable[] = {
  //
  // Video Option Rom
  //
  {
    ONBOARD_PCO_VIDEO_OPTION_ROM_FILE_GUID,
    INTERNAL_VGA_VID,
    INTERNAL_VGA_DID14
  },
  {
    ONBOARD_RV2_VIDEO_OPTION_ROM_FILE_GUID,
    INTERNAL_VGA_VID,
    DEFAULT_IGP_DID
  },
  //
  // AHCI Option Rom
  //
  {
    AMD_AHCI_OPTION_ROM_FILE_GUID,
    AMD_FCH_VID,
    SANDSTONE_FCH_SATA_AHCI_DID
  },
  {
    AMD_AHCI_OPTION_ROM_FILE_GUID,
    AMD_FCH_VID,
    SANDSTONE_FCH_SATA_AMDAHCI_DID
  },
  //
  // PXE UNDI Option Rom
  //
  {
    RTL_8153_ON_XHCI_OPTION_ROM_FILE_GUID,
    AMD_FCH_VID,
    SANDSTONE_FCH_USB_XHCI_DID
  },
  //
  // BCM5762 PXE Option Rom
  //
  {
    PXE_BCM5762_OPTION_ROM_FILE_GUID,
    0x14E4,
    0x1687
  },
  //
  // RTL8111EP PXE ROM
  //
  {
    PXE_RTL8111EP_OPTION_ROM_FILE_GUID,
    0x10EC,
    0x8168
  },
  {
    NULL_ROM_FILE_GUID,
    0xffff,
    0xffff
  }
};


//=======================================
//        Non Pci Option Rom Table
//=======================================
//
// System Rom table
//
//[-start-141031-IB06720273-add]//
//@ Non-PCI OPROM table
//[-end-141031-IB06720273-add]//
SYSTEM_ROM_TABLE    SystemRomTable[] = {
  {
    //
    // CSM16 binary
    //
    SYSTEM_ROM_FILE_GUID,
    TRUE,
    SYSTEM_ROM
  },
  {
    NULL_ROM_FILE_GUID,
    FALSE,
    MAX_NUM
  }
};


//
// Oem Int table
//
SYSTEM_ROM_TABLE    SystemOemIntTable[] = {
  {
    NULL_ROM_FILE_GUID,
    FALSE,
    MAX_NUM
  }
};

//
//  Service ROM Table
//
SERVICE_ROM_TABLE  ServiceRomTable[] = {
  {
    NULL_ROM_FILE_GUID,
    FALSE,
    FALSE,
    MAX_NUM
  }
};

//
// TPM Rom table
//
SYSTEM_ROM_TABLE    TpmTable[] = {
  {
    //
    // TPMMP binary
    //
    TPM12_STM19WP18_GUID,
    TRUE,
    TPM_ROM
  },
  {
    NULL_ROM_FILE_GUID,
    FALSE,
    MAX_NUM
  }
};

/*---------------------------------------------------------------------------------------*/
/**
 * Check PCO or RV2
 *
 * @retval     1    This is 15DD (RV2)
 *                 2    This is 15D8/A4 (PCO)
 *                 3    This is 15D8/A4 (PCO-RV2)
 */
UINT8
CheckPcoOpnB4 (
 
  )
{
  UINT32        TempData32;

  TempData32 =  (PcdGet32 (PcdGetRVFamilyOPN) >> 30) & 0x3;
  return (UINT8)TempData32;
}

UINT16
GetIGpuDid (
  VOID
  )
{
  UINT16                    IGPU_DID;
  
  IGPU_DID = (UINT16) PcdGet32 (PcdGetRVFamilyOPN);
  
  return IGPU_DID;
}

/**
  Provide OEM to install the PCI Option ROM table and Non-PCI Option ROM table.
  The detail refers to the document "OptionRomTable Restructure User Guide".

  @param[in]   *RomType              The type of option rom. This parameter decides which kind of option ROM table will be access.
  @param[out]  **RomTable            A pointer to the option ROM table.

  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_SUCCESS           Get Option ROM Table info success.
  @retval      EFI_NOT_FOUND         Get Option ROM Table info failed.
  @retval      EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed.
  @retval      Others                Depends on customization.
**/
EFI_STATUS
OemSvcInstallOptionRomTable (
  IN  UINT8                                 RomType,
  OUT VOID                                  **mOptionRomTable
  )
{
  VOID                                  *OptionRomTable;
  UINTN                                 OptionRomTableSize;
  UINT16                                OptionCount;
  UINT16                                Index;
  EFI_GUID                              OnboardPCOVideoOptionRomFileGuid = ONBOARD_PCO_VIDEO_OPTION_ROM_FILE_GUID;
  EFI_GUID                              OnboardRV2VideoOptionRomFileGuid = ONBOARD_RV2_VIDEO_OPTION_ROM_FILE_GUID;
  UINT8                                 PCOtype;

  switch (RomType) {

  case PCI_OPROM:

    PCOtype = CheckPcoOpnB4();

    OptionCount = sizeof (PciOptionRomTable) / sizeof (PCI_OPTION_ROM_TABLE);


    for(Index = 0; Index < OptionCount; Index++) {
      if(CompareGuid (&PciOptionRomTable[Index].FileName, &OnboardPCOVideoOptionRomFileGuid)) {
        if( (PCOtype == 0) ||(PCOtype == 2) ){ //PCO
          PciOptionRomTable[Index].DeviceId = GetIGpuDid();
        } else {
          PciOptionRomTable[Index].DeviceId = 0x0000;
        }
        break;
      }
    }

    for(Index = 0; Index < OptionCount; Index++) {
      if(CompareGuid (&PciOptionRomTable[Index].FileName, &OnboardRV2VideoOptionRomFileGuid)) {
        if( (PCOtype == 1) ||(PCOtype == 3) ){ //RV2
          PciOptionRomTable[Index].DeviceId = GetIGpuDid();
        } else {
          PciOptionRomTable[Index].DeviceId = 0x0000;
        }
        break;
      }
    }

    OptionRomTable      = (void *)PciOptionRomTable;
    OptionRomTableSize  = sizeof(PciOptionRomTable);
    break;

  case SYSTEM_ROM:
    OptionRomTable      = (void *)SystemRomTable;
    OptionRomTableSize  = sizeof(SystemRomTable);
    break;

  case SYSTEM_OEM_INT_ROM:
    OptionRomTable      = (void *)SystemOemIntTable;
    OptionRomTableSize  = sizeof(SystemOemIntTable);
    break;

  case SERVICE_ROM:
    OptionRomTable      = (void *)ServiceRomTable;
    OptionRomTableSize  = sizeof(ServiceRomTable);
    break;

  case TPM_ROM:
    OptionRomTable      = (void *)TpmTable;
    OptionRomTableSize  = sizeof(TpmTable);
    break;

  default:
    (*mOptionRomTable)  = NULL;
    return EFI_SUCCESS;
  }

  (*mOptionRomTable)   = AllocateZeroPool (OptionRomTableSize);
  CopyMem ((*mOptionRomTable), OptionRomTable, OptionRomTableSize);

  return EFI_MEDIA_CHANGED;
}

