/** @file
  InstallVerbTablePei peim will produces the InstallVerbTable Ppi for Pei driver.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <RecoveryCpuInitPei.h>

/**
  Init the CPU Register

  @param   FfsHeader                   
  @param   PeiServices                  
  
  @retval  NONE 
  
**/
EFI_STATUS
RecoveryCpuInitEntryPoint (
  IN EFI_PEI_FILE_HANDLE                                    FileHandle,
  IN CONST EFI_PEI_SERVICES                                 **PeiServices
  )
{
  EFI_STATUS              Status;
  EFI_PEI_PCI_CFG2_PPI    *PciCfgPpi;
  UINT32                  Reg32;
  UINT32                  TopMem;
  UINT64                  PciAddress;

  UINT64                  BaseAddress;
  UINT64                  Length;
  UINT32                  Value32;
  Status = EFI_SUCCESS;
  TopMem = (UINT32)AsmReadMsr32 (0xC001001a);

  PciCfgPpi = (**PeiServices).PciCfg;
  //
  // set pci memory top
  //
  Reg32 = TopMem;// - (GraphicsMemorySize);
  PciAddress = PEI_PCI_CFG_ADDRESS(0, 0, 0, 0x90);
  PciCfgPpi->Write (PeiServices,
                    PciCfgPpi,
                    //PeiPciCfgWidthUint32,
					EfiPeiPciCfgWidthUint32,
                    PciAddress,
                    &Reg32
                    );


  //
  //  Reserved MMIO resource for external device used.
  //
  BaseAddress = (UINT64) RES_MEM_BASE_2;
  Length = RES_MEM_LIMIT_2 - BaseAddress;

  if (Length != 0) {
    PciAddress = PEI_PCI_CFG_ADDRESS(0, 0x18, 0x1, 0x84);
    Value32 = (UINT32)(BaseAddress + Length - 0x100) >> 8;
    Value32 &= 0xFFFFFF00;

    PciCfgPpi->Write (PeiServices,
      PciCfgPpi,
      EfiPeiPciCfgWidthUint32,
      PciAddress,
      &Value32
      );

    PciAddress = PEI_PCI_CFG_ADDRESS (0, 0x18, 0x01, 0x80);
    Value32 = ((UINT32)(BaseAddress) >> 8) | 0x03;

    PciCfgPpi->Write (PeiServices,
      PciCfgPpi,
      EfiPeiPciCfgWidthUint32,
      PciAddress,
      &Value32
      );
  }

  

  return Status;
}               


