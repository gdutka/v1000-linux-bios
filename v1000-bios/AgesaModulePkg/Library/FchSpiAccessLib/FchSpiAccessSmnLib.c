/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FchSpiAccessLib.h>
#include <Library/FchBaseLib.h>
#include "FchRegistersCommon.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_FCHSPIACCESSSMNLIB_FILECODE

#define IS_SPI_ROM2_OFFSET(a) (((a) < 0x1000000) ? TRUE : FALSE)
#define IS_IN_SPI_ROM2_WINDOW(a) ((((a) & ~(0xFFFFFF)) == 0xFF000000) ? TRUE : FALSE)
#define IS_SPI_ROM3_OFFSET(a) (((a) < 0x4000000) ? TRUE : FALSE)
#define IS_IN_SPI_ROM3_WINDOW(a) ((((a) & ~(0x3FFFFFF)) == 0xFD00000000) ? TRUE : FALSE)
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
* DetectRom2Page - detect the ROM page decoded into ROM2 decode range, available with default ROM2 16MB window (0xFF000000)
*
*
* @param VOID
*
* @retval UINT32 - the ROM page address (bit 24/25)
*/
UINT32
DetectRom2Page(
)
{
  UINT32    Value32;
  UINT8     SpiRomPageXor;
  UINT32    RomPage;

  RomPage = FCH_ROM_START_ADDRESS_2 >> 24;

  Value32 = MmioRead32 ((UINTN)(FCH_SPI_BASE_ADDRESS + FCH_SPI_MMIO_REG30));
  if (Value32 & FCH_SPI_R2MSK24){
    RomPage &= ~FCH_SPI_R2VAL24;
    RomPage |= Value32 & FCH_SPI_R2VAL24;
  }
  if (Value32 & FCH_SPI_R2MSK25){
    RomPage &= ~FCH_SPI_R2VAL25;
    RomPage |= Value32 & FCH_SPI_R2VAL25;
  }

  Value32 = MmioRead32 ((UINTN)(FCH_SPI_BASE_ADDRESS + FCH_SPI_MMIO_REG5C_Addr32_Ctrl3));

  SpiRomPageXor = (UINT8)(Value32 & FCH_SPI_SPIROM_PAGE_MASK);
  RomPage ^= SpiRomPageXor;

  return (RomPage << 24);
}

/**
* FchSpiRomSmnRead - Read the SPI ROM via SMN address
*
*
* @param[in] IohcBus - the IOHC Bus number
* @param[in] Offset - the offset address of host ROM3 address window
* @param[out]*Data - data to be read
* @param[in] Length - the size to read ROM
*
* @retval BOOLEAN - success read or not
*/
BOOLEAN
FchSpiRomSmnRead (
 IN      UINT32            IohcBus,
 IN      UINT32            Offset,
 OUT     UINT8             *Data,
 IN      UINT32            Length
)
{
  UINT32          Address;
  UINT32          Index, DataCount, ByteCount;
  UINTN           PciAddress;
  UINT32          Value;
  UINT32          StartAddr, EndAddr;
  UINT32          StartAlign, EndAlign;
  UINT32          FrontPad, BackPad;

  if ((Data == NULL) || (Length == 0)){
    return FALSE;
  }

  if((Offset >= FCH_ROM_SIZE_64M) || (Length > FCH_ROM_SIZE_64M) || ((Offset + Length) > FCH_ROM_SIZE_64M)){
    return FALSE;
  }

  StartAddr = Offset;
  EndAddr = Offset + Length - 1;

  if (StartAddr % BIT2){
    StartAlign = (StartAddr / BIT2) * BIT2;
    FrontPad = StartAddr - StartAlign;
  }else{
    StartAlign = StartAddr;
    FrontPad = BYTE0;
  }

  if (EndAddr % BIT2){
    EndAlign = (EndAddr / BIT2) * BIT2;
    BackPad = EndAddr - EndAlign;
  }else{
    EndAlign = EndAddr;
    BackPad = BYTE0;
  }

  DataCount = 0;
  for (Index = StartAlign; Index <= EndAlign; Index += sizeof(UINT32)){
    Address = FCH_ROM_SMN_START_ADDRESS + Index;
    PciAddress = ((UINTN)IohcBus << 20) + IOHC_NB_SMN_INDEX_2_BIOS;
    PciWrite32 (PciAddress, Address);
    PciAddress = ((UINTN)IohcBus << 20) + IOHC_NB_SMN_DATA_2_BIOS;
    Value = PciRead32 (PciAddress);

    if (StartAlign == EndAlign){
      for (ByteCount = FrontPad; ByteCount <= BackPad; ByteCount++, DataCount++){
        *(Data + DataCount) = (Value >> (ByteCount * BYTE_OFFSET)) & BYTE_MASK;
      }
    }else{
      if (Index == StartAlign){
        for (ByteCount = FrontPad; ByteCount <= BYTE3; ByteCount++, DataCount++){
          *(Data + DataCount) = (Value >> (ByteCount * BYTE_OFFSET)) & BYTE_MASK;
        }
      }else if (Index == EndAlign){
        for (ByteCount = BYTE0; ByteCount <= BackPad; ByteCount++, DataCount++){
          *(Data + DataCount) = (Value >> (ByteCount * BYTE_OFFSET)) & BYTE_MASK;
        }
      }else{
        *(UINT32 *)(Data + DataCount) = Value;
        DataCount += sizeof(UINT32)/sizeof(UINT8);
      }
    }
  }

  return TRUE;
}

/**
* FchSpiRomMmioRead24b - Read the SPI ROM via ROM2 24b address
*
*
* @param[in] Offset - the offset address of host ROM2 address window
* @param[out]*Data - buffer to save the data
* @param[in] Length - the size to read from ROM
*
* @retval BOOLEAN - success read or not
*/
BOOLEAN
FchSpiRomMmioRead24b (
  IN      UINT32            Offset,
  OUT     UINT8             *Data,
  IN      UINT32            Length
)
{
  UINT64          Value;
  UINT32          Index, DataCount, ByteCount;
  UINT32          StartAddr, EndAddr;
  UINT32          StartAlign, EndAlign;
  UINT32          FrontPad, BackPad;

  if ((Data == NULL) || (Length == 0)){
    return FALSE;
  }

  if((Offset >= FCH_ROM_SIZE_16M) || (Length > FCH_ROM_SIZE_16M) || ((Offset + Length) > FCH_ROM_SIZE_16M)){
    return FALSE;
  }

  StartAddr = Offset;
  EndAddr = Offset + Length - 1;

  if (StartAddr % BIT3){
    StartAlign = (StartAddr / BIT3) * BIT3;
    FrontPad = StartAddr - StartAlign;
  }else{
    StartAlign = StartAddr;
    FrontPad = BYTE0;
  }

  if (EndAddr % BIT3){
    EndAlign = (EndAddr / BIT3) * BIT3;
    BackPad = EndAddr - EndAlign;
  }else{
    EndAlign = EndAddr;
    BackPad = BYTE0;
  }

  DataCount = 0;
  for (Index = StartAlign; Index <= EndAlign; Index += sizeof(UINT64)){
    Value = MmioRead64 (FCH_ROM_START_ADDRESS_2 + Index);
    if (StartAlign == EndAlign){
      for (ByteCount = FrontPad; ByteCount <= BackPad; ByteCount++, DataCount++){
        *(Data + DataCount) = (Value >> (ByteCount * BYTE_OFFSET)) & BYTE_MASK;
      }
    }else{
      if (Index == StartAlign){
        for (ByteCount = FrontPad; ByteCount <= BYTE7; ByteCount++, DataCount++){
          *(Data + DataCount) = (Value >> (ByteCount * BYTE_OFFSET)) & BYTE_MASK;
        }
      }else if (Index == EndAlign){
        for (ByteCount = BYTE0; ByteCount <= BackPad; ByteCount++, DataCount++){
          *(Data + DataCount) = (Value >> (ByteCount * BYTE_OFFSET)) & BYTE_MASK;
        }
      }else{
        *(UINT64 *)(Data + DataCount) = Value;
        DataCount += sizeof(UINT64)/sizeof(UINT8);
      }
    }
  }

  return TRUE;
}

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 * IsRom2Decoded - check Offset 
 *
 *
 * @param[in] Offset - The offset of ROM
 *
 * @retval BOOLEAN Is in SPI ROM2 decode window or not, available with default ROM2 16MB window (0xFF000000)
 */
BOOLEAN
IsRom2Decoded (
  IN       UINT64        Offset
  )
{
  UINT32                    Bank;

  Bank = DetectRom2Page();

  if ((Bank & BIT24) != (Offset & BIT24)){
    return FALSE;
  }else {
    return TRUE;
  }
}

/**
 * FchSpiRomRead - Read the SPI ROM
 *
 *
 * @param[in] Offset - Offset of host SPI ROM3 address window
 * @param[out]*Data - Buffer address to save the data
 * @param[in] Length - The number of byte to read
 *
 * @retval BOOLEAN success read or not
 */
BOOLEAN
FchSpiRomRead (
    IN      UINT32            Offset,
    OUT     UINT8             *Data,
    IN      UINT32            Length
  )
{
  BOOLEAN         Status = TRUE;
  UINT32          Family, ExtModel;

  Family = FchGetFamilyId();
  ExtModel = FchGetModelId() & 0xF0;

  if ((Family == 0x17 && ExtModel == 0x30) || (Family == 0x17 && ExtModel >= 0x60) || (Family == 0x19)){  //SSP, RN and later
    Status = FchSpiRomSmnRead(0x00, Offset, Data, Length);
  }else{
    Status = FchSpiRomMmioRead24b(Offset & FCH_ROM_RANGE_MASK, Data, Length);
  }
  return Status;
}

/**
 * FchSpiRomReadEx - Extended function to read the SPI ROM
 *
 *
 * @param[in] Location - Offset of host SPI ROM3 address window
 *                     - Host address in ROM2 window to SPI ROM
 * @param[out]*Data - Buffer address to save the data
 * @param[in] Length - The number of byte to read
 *
 * @retval BOOLEAN success read or not
 */
BOOLEAN
FchSpiRomReadEx (
    IN      UINTN             Location,
    OUT     UINT8             *Data,
    IN      UINT32            Length
  )
{
  BOOLEAN    Status = TRUE;

  if(IS_SPI_ROM3_OFFSET(Location)){
    Status = FchSpiRomRead((UINT32)Location, Data, Length);
  }else if(IS_IN_SPI_ROM2_WINDOW(Location)){
    CopyMem(Data, (VOID *)Location, Length);
    Status = TRUE;
  }else{
    Status = FALSE;
  }

  return Status;
}
