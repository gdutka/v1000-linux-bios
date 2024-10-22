/** @file
 DXE Chipset Services Library.

 This file contains only one function that is DxeCsSvcIoDecodeControl().
 The function DxeCsSvcIoDecodeControl() use chipset services to set
 IO decode region.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Protocol/H2ODxeChipsetServices.h>
#include <Library/PciLib.h>

#include <ChipsetSvc.h>
#include <Fch.h>

typedef struct {
  IO_DECODE_TYPE      IoType;
  IO_DECODE_ADDRESS   IoAddress;
  UINT8               IoDecReg;
  UINT16              IoDecEn;
} IO_DECODE_CONTROL_TABLE;

IO_DECODE_CONTROL_TABLE mIoDecConTable[] = {
  // {IoType, IoAddress, IoDecReg, IoDecEn}
  {IoDecodeFdd,   AddressFdd3F0,   FCH_LPC_REG47, BIT2},
  {IoDecodeFdd,   AddressFdd370,   FCH_LPC_REG47, BIT3},
  {IoDecodeLpt,   AddressLpt378,   FCH_LPC_REG44, BIT0},
  {IoDecodeLpt,   AddressLpt278,   FCH_LPC_REG44, BIT2},
  {IoDecodeLpt,   AddressLpt3BC,   FCH_LPC_REG44, BIT4},
  {IoDecodeComA,  AddressCom3F8,   FCH_LPC_REG44, BIT6},
  {IoDecodeComA,  AddressCom2F8,   FCH_LPC_REG44, BIT7},
  {IoDecodeComA,  AddressCom220,   FCH_LPC_REG45, BIT0},
  {IoDecodeComA,  AddressCom228,   FCH_LPC_REG45, BIT1},
  {IoDecodeComA,  AddressCom238,   FCH_LPC_REG45, BIT2},
  {IoDecodeComA,  AddressCom2E8,   FCH_LPC_REG45, BIT3},
  {IoDecodeComA,  AddressCom338,   FCH_LPC_REG45, BIT4},
  {IoDecodeComA,  AddressCom3E8,   FCH_LPC_REG45, BIT5},
  {IoDecodeComB,  AddressCom3F8,   FCH_LPC_REG44, BIT6},
  {IoDecodeComB,  AddressCom2F8,   FCH_LPC_REG44, BIT7},
  {IoDecodeComB,  AddressCom220,   FCH_LPC_REG45, BIT0},
  {IoDecodeComB,  AddressCom228,   FCH_LPC_REG45, BIT1},
  {IoDecodeComB,  AddressCom238,   FCH_LPC_REG45, BIT2},
  {IoDecodeComB,  AddressCom2E8,   FCH_LPC_REG45, BIT3},
  {IoDecodeComB,  AddressCom338,   FCH_LPC_REG45, BIT4},
  {IoDecodeComB,  AddressCom3E8,   FCH_LPC_REG45, BIT5},
  {IoDecodeCnf2,  AddressCnf24E4F, FCH_LPC_REG48, BIT1},
  {IoDecodeCnf1,  AddressCnf12E2F, FCH_LPC_REG48, BIT0},
#ifndef LEGACY_FREE_SUPPORT
  {IoDecodeMc,    AddressMc6266,   FCH_LPC_REG47, BIT6},
  {IoDecodeKbc,   AddressKbc6064,  FCH_LPC_REG47, BIT5},
#endif
  {IoDecodeGameh, AddressGameh208, FCH_LPC_REG47, BIT4},
  {IoDecodeGamel, AddressGamel200, FCH_LPC_REG47, BIT4},
};

typedef struct {
  UINT8               IoBaseReg;
  UINT8               IoDecReg;
  UINT16              IoDecEn;
  UINT8               IoDecAltEn;
} WIDE_IO_DECODE_CONTROL_TABLE;

WIDE_IO_DECODE_CONTROL_TABLE mWideIoDecConTable[] = {
  // {IoBaseReg, IoDecReg, IoDecEn, IoDecAltEn}
  {FCH_LPC_REG64, FCH_LPC_REG48, BIT2, BIT0},
  {FCH_LPC_REG66, FCH_LPC_REG4A, BIT8, BIT2},
  {FCH_LPC_REG66, FCH_LPC_REG4A, BIT9, BIT3},
};

/**
 Allocate Generic I/O.
 
 @param[in]         Address             Decode address
 @param[in]         Length              Decode length

 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER One of the parameters has an invalid value
                                          1. Address big then 0xFFFF
                                          2. Length need to be 16-byte aligned
 @retval            EFI_OUT_OF_RESOURCES  There are not enough resources available to set IO decode
*/
STATIC
EFI_STATUS
AllocateGenericIo (
  IN UINTN              Address,
  IN UINT16             Length
  )
{
  EFI_STATUS            Status;
  UINTN                 Index;
  UINT16                DecodeBaseAddress;

  Status = EFI_INVALID_PARAMETER;
  
  if (((Length & 0x000F) != 0) || (Address > 0xFFFF)) {
    return EFI_INVALID_PARAMETER;
  }
  if (Length > 0x200) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Update current decode settings
  for (Index = 0; Index < sizeof (mWideIoDecConTable) / sizeof (WIDE_IO_DECODE_CONTROL_TABLE); Index++) {
    DecodeBaseAddress = PciRead16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mWideIoDecConTable[Index].IoBaseReg));
    if (DecodeBaseAddress == Address) {
      if (Length == 0) {
        // Disable IO Decode
        PciWrite16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mWideIoDecConTable[Index].IoBaseReg), 0);
        PciAnd16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mWideIoDecConTable[Index].IoDecReg), ~mWideIoDecConTable[Index].IoDecEn);
        PciAnd8 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_REG74), ~mWideIoDecConTable[Index].IoDecAltEn);
      } else {
        PciOr16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mWideIoDecConTable[Index].IoDecReg), mWideIoDecConTable[Index].IoDecEn);
        if (Length == 0x10) {
          DecodeBaseAddress = (Address & 0xFFF0);
          PciWrite16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mWideIoDecConTable[Index].IoBaseReg), DecodeBaseAddress);
          PciOr16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mWideIoDecConTable[Index].IoDecReg), mWideIoDecConTable[Index].IoDecEn);
          PciOr8 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_REG74), mWideIoDecConTable[Index].IoDecAltEn);
        }
      }
      Status = EFI_SUCCESS;
    }
  }

  // Add new decode settings
  if ((Status != EFI_SUCCESS) && (Length != 0)) {
    for (Index = 0; Index < sizeof (mWideIoDecConTable) / sizeof (WIDE_IO_DECODE_CONTROL_TABLE); Index++) {
      DecodeBaseAddress = PciRead16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mWideIoDecConTable[Index].IoBaseReg));
      if (DecodeBaseAddress == 0 && Length == 0x10) {
        DecodeBaseAddress = (Address & 0xFFF0);
        PciWrite16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mWideIoDecConTable[Index].IoBaseReg), DecodeBaseAddress);
        PciOr16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mWideIoDecConTable[Index].IoDecReg), mWideIoDecConTable[Index].IoDecEn);
        PciOr8 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_REG74), mWideIoDecConTable[Index].IoDecAltEn);
        Status = EFI_SUCCESS;
        break;
      } else if (DecodeBaseAddress == 0) {
        DecodeBaseAddress = (Address & 0xFFFF);
        PciWrite16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mWideIoDecConTable[Index].IoBaseReg), DecodeBaseAddress);
        PciOr16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mWideIoDecConTable[Index].IoDecReg), mWideIoDecConTable[Index].IoDecEn);
        Status = EFI_SUCCESS;
        break;
      } else {
        Status = EFI_OUT_OF_RESOURCES;
      }
    }
  }

  return Status;
}

/**
 IO address decode

 @param[in]         Type                Decode type
 @param[in]         Address             Decode address
 @param[in]         Length              Decode length
                                        If length is 0 then exist I/O decode "Type" and "address" will be disabled.

 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER One of the parameters has an invalid value
                                          1. Type has invalid value
                                          2. Address big then 0xFFFF
                                          3. Length need to be 16-byte aligned
 @retval            EFI_OUT_OF_RESOURCES  There are not enough resources available to set IO decode
*/
EFI_STATUS
IoDecodeControl (
  IN IO_DECODE_TYPE     Type,
  IN IO_DECODE_ADDRESS  Address,
  IN UINT16             Length
  )
{
  EFI_STATUS            Status;
  UINTN                 Index;

  if (Type >= IoDecodeTypeMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  if (Type != IoDecodeGenericIo && Address >= IoDecodeAddressMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  if (Type == IoDecodeGenericIo) {
    Status = AllocateGenericIo ((UINTN)Address, Length);
    return Status;
  }

  for (Index = 0; Index < sizeof (mIoDecConTable) / sizeof (IO_DECODE_CONTROL_TABLE); Index++) {
    if ((Type == mIoDecConTable[Index].IoType) && (Address == mIoDecConTable[Index].IoAddress)) {
      if (Length == 0) {
        PciAnd16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mIoDecConTable[Index].IoDecReg), ~mIoDecConTable[Index].IoDecEn);
      } else {
        PciOr16 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, mIoDecConTable[Index].IoDecReg), mIoDecConTable[Index].IoDecEn);
      }
    }
  }

  return EFI_SUCCESS;
}