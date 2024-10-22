/** @file
 Spd Read PEI. 
 
 It produces an instance of the DIMM PPI to provide the SPD Data Read from ROM, SMBUS.

***************************************************************************
* Copyright (c) 2013 - 2017, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <PiPei.h>
#include <DimmSmbusAddrHobData.h>

#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/SmbusLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>

#include <Guid/DimmSmbusAddrHob.h>

#include <Ppi/SpdDataRead.h>

#include <IndustryStandard/SdramSpd.h>


BOOLEAN
SpdDataReadRom (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  UINT8               SlaveAddr,
  IN  UINT8               MemChannelId,   ///< Address of SPD - memory channel ID
  IN  UINT8               DimmId,         ///< Address of SPD - DIMM ID
  IN  UINT8               *Buffer         ///< Location where to place the SPD content
  );

EFI_STATUS
SpdDataReadSmbus (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  UINT8               SlaveAddr,
  IN  UINT8               *Buffer         ///< Location where to place the SPD content
  );

SPD_DATA_READ_PPI         mSpdDataReadPpi = {
  SpdDataReadRom,
  SpdDataReadSmbus
};

//
// Ppis to be installed
//
EFI_PEI_PPI_DESCRIPTOR    mPpiList[] = { 
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gSpdDataReadPpiGuid,
    &mSpdDataReadPpi
  }
};

BOOLEAN
SpdDataReadRom (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  UINT8               SlaveAddr,
  IN  UINT8               MemChannelId,   ///< Address of SPD - memory channel ID
  IN  UINT8               DimmId,         ///< Address of SPD - DIMM ID
  IN OUT UINT8            *Buffer         ///< Location where to place the SPD content
  )
{
  EFI_STATUS              Status;
  EFI_STATUS              DummyStatus;
  UINT8                   *Buffer2;
  UINTN                   Index;
  UINTN                   DimmIndex;
  UINTN                   SmbusOffset;
  BOOLEAN                 SameAsPrevious;
  UINT8                   *SpdInRom;
  UINT8                   Value8;
  UINT8                   SpdMemoryType;
  UINT8                   *SpdChkList;
  UINT16                  *SpdDDR4ChkList;
  UINT8                   SpdChkListSize;
  DIMM_INFO_SMBUS *       DimmInfoSmbusArray;
  EFI_PEI_HOB_POINTERS    Hob;

  Status = EFI_SUCCESS;
  Buffer2 = Buffer;
  SameAsPrevious = FALSE;
  Value8 = 0;
  DimmInfoSmbusArray = NULL;

  Status = (*PeiServices)->GetHobList ((CONST EFI_PEI_SERVICES**)PeiServices, (VOID**)&Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION &&
      CompareGuid (&Hob.Guid->Name, &gDimmSmbusAddrHobGuid)) {
      UINT8 *TempBuffer;
      TempBuffer = (UINT8 *)(Hob.Raw) +
               sizeof (EFI_HOB_GENERIC_HEADER) +
               sizeof (EFI_GUID) +
               PcdGet8 (PcdMaxDimms);
      DimmInfoSmbusArray = (DIMM_INFO_SMBUS*) (UINTN) TempBuffer;
      break;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  for (DimmIndex = 0; DimmIndex < PcdGet8 (PcdMaxDimms); DimmIndex++) {
    if ((MemChannelId == DimmInfoSmbusArray[DimmIndex].ChannelId) && (DimmId == DimmInfoSmbusArray[DimmIndex].DimmId)) {
      break;
    }
  }

  //
  // Read Spd data from PcdFlashSpdDataStoreBase
  //
  SpdInRom = (UINT8*)(UINTN)(PcdGet32 (PcdFlashSpdDataStoreBase) + (PcdGet16 (PcdSizeOfSpd) * DimmIndex));

  SmbusOffset = SPD_MEMORY_TYPE;
  SpdMemoryType = SmBusReadDataByte (SMBUS_LIB_ADDRESS (SlaveAddr, SmbusOffset, 0, FALSE), &Status);
  if (!EFI_ERROR (Status)) {
    Index = 0;
    SameAsPrevious = TRUE;

    switch (SpdMemoryType) { //DDR2 SDRAM

      case  8: // DDR2 SDRAM
      case  9: // DDR2 SDRAM FB-DIMM
      case 10: // DDR2 SDRAM FB-DIMM PROBE
        SpdChkList = PcdGetPtr (PcdSpdDdr2ChkList);
        SpdChkListSize = 18;
        do {
          SmbusOffset = SpdChkList[Index];
          Value8 = SmBusReadDataByte (SMBUS_LIB_ADDRESS (SlaveAddr, SmbusOffset, 0, FALSE), &Status);
          if ((Value8 != SpdInRom[SmbusOffset]) || EFI_ERROR (Status)) {
            SameAsPrevious = FALSE;
          }
          Index++;
        } while (SameAsPrevious && (Index < SpdChkListSize));
        break;

      case 11: //DDR3 SDRAM
        SpdChkList = PcdGetPtr (PcdSpdDdr3ChkList);
        SpdChkListSize = 11;
        do {
          SmbusOffset = SpdChkList[Index];
          Value8 = SmBusReadDataByte (SMBUS_LIB_ADDRESS (SlaveAddr, SmbusOffset, 0, FALSE), &Status);
          if ((Value8 != SpdInRom[SmbusOffset]) || EFI_ERROR (Status)) {
            SameAsPrevious = FALSE;
          }
          Index++;
        } while (SameAsPrevious && (Index < SpdChkListSize));
        break;

      case 12: //DDR4 SDRAM
        SpdDDR4ChkList = PcdGetPtr (PcdSpdDdr4ChkList);
        SpdChkListSize = 11;

        do {
          SmbusOffset = SpdDDR4ChkList[Index];
          if(SmbusOffset <=255) {
            Value8 = SmBusReadDataByte (SMBUS_LIB_ADDRESS (SlaveAddr, SmbusOffset, 0, FALSE), &Status);
            if ((Value8 != SpdInRom[SmbusOffset]) || EFI_ERROR (Status)) {
              SameAsPrevious = FALSE;
            }
          }else {
            SmBusWriteDataByte (SMBUS_LIB_ADDRESS (SMBUS_LIB_SLAVE_ADDRESS (0x6E), 0, 0, FALSE), 0, &DummyStatus); //SPA1
            Value8 = SmBusReadDataByte (SMBUS_LIB_ADDRESS (SlaveAddr, SmbusOffset - 256, 0, FALSE), &Status);
            if ((Value8 != SpdInRom[SmbusOffset]) || EFI_ERROR (Status)) {
              SameAsPrevious = FALSE;
            }
            SmBusWriteDataByte (SMBUS_LIB_ADDRESS (SMBUS_LIB_SLAVE_ADDRESS (0x6C), 0, 0, FALSE), 0, &DummyStatus); //SPA0
          }
          Index++;
        } while (SameAsPrevious && (Index < SpdChkListSize));
        break;
    }
  }

  if (SameAsPrevious) {
    CopyMem (Buffer2, &SpdInRom[0], PcdGet16 (PcdSizeOfSpd));
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
SpdDataReadSmbus (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  UINT8               SlaveAddr,
  IN OUT UINT8            *Buffer         ///< Location where to place the SPD content
  )
{
  EFI_STATUS              Status;
  EFI_STATUS              DummyStatus;
  UINT8                   *Buffer2;
  UINTN                   Index;
  UINTN                   SmbusOffset;
  UINT8                   SpdMemoryType;
  
  Status = EFI_SUCCESS;
  Buffer2 = Buffer;

  //
  // Read Spd data from Smbus
  //
  SmbusOffset = 0;
  for (Index = 0; Index < 256; Index++) {
    *Buffer2 = SmBusReadDataByte (SMBUS_LIB_ADDRESS (SlaveAddr, SmbusOffset, 0, FALSE), &Status);
    if (EFI_ERROR (Status)) {
      break;
    }
    SmbusOffset += 1;
    Buffer2++;
  }
  
  SmbusOffset = SPD_MEMORY_TYPE;
  SpdMemoryType = SmBusReadDataByte (SMBUS_LIB_ADDRESS (SlaveAddr, SmbusOffset, 0, FALSE), &Status);
  if (SpdMemoryType == 12) { //DDR4 SDRAM
    SmbusOffset = 0;
    SmBusWriteDataByte (SMBUS_LIB_ADDRESS (SMBUS_LIB_SLAVE_ADDRESS (0x6E), SmbusOffset, 0, FALSE), 0, &DummyStatus); //SPA1
    for (Index = 0; Index < 256; Index++) {
      *Buffer2 = SmBusReadDataByte (SMBUS_LIB_ADDRESS (SlaveAddr, SmbusOffset, 0, FALSE), &Status);
      if (EFI_ERROR (Status)) {
        break;
      }
      SmbusOffset += 1;
      Buffer2++;
    }
    SmBusWriteDataByte (SMBUS_LIB_ADDRESS (SMBUS_LIB_SLAVE_ADDRESS (0x6C), 0, 0, FALSE), 0, &DummyStatus); //SPA0
  }

  return Status;
}

/**
 This is the declaration of Spd Data Read PEI entry point. 
 
 @param[in]         FileHandle          Handle of the file being invoked.
 @param[in]         PeiServices         Describes the list of possible PEI Services.
                    
 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
EFIAPI
SpdDataReadPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  EFI_STATUS                         Status;

  Status = PeiServicesInstallPpi (&mPpiList[0]);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
