/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/
#include <Library/UefiBootServicesTableLib.h>

#include <Library/UefiRuntimeServicesTableLib.h>

#include <Library/FchDxeLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#define FILECODE UEFI_DXE_FCHDXELIB_FCHDXELIB_FILECODE

#define FCH_IOMAP_REGCD6  0xcd6
#define FCH_PMIOA_REG60   0x60


/*----------------------------------------------------------------------------------------*/
/**
 * I/O Read Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      IO address
 * @param[out]      Value        Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchIoRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   Address,
     OUT   VOID                                     *Value
  )
{
  switch (EfiWidth) {
  case EfiPciWidthUint8:
    *(UINT8 *) Value = IoRead8 (Address);
    break;
  case EfiPciWidthUint16:
    *(UINT16 *) Value = IoRead16 (Address);
    break;
  case EfiPciWidthUint32:
    *(UINT32 *) Value = IoRead32 (Address);
    break;
  default:
    ASSERT (FALSE);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * I/O Write Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      IO address
 * @param[in]       Value        Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchIoWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   Address,
     OUT   VOID                                     *Value
  )
{
  switch (EfiWidth) {
  case EfiPciWidthUint8:
    IoWrite8 (Address, *(UINT8 *) Value);
    break;
  case EfiPciWidthUint16:
    IoWrite16 (Address, *(UINT16 *) Value);
    break;
  case EfiPciWidthUint32:
    IoWrite32 (Address, *(UINT32 *) Value);
    break;
  default:
    ASSERT (FALSE);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Memory Read Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      Memory address
 * @param[out]      Value        Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchMemRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
     OUT   VOID                                     *Value
  )
{
  switch ( EfiWidth ) {
  case EfiPciWidthUint8:
    *((UINT8*)Value) = *((volatile UINT8*) ((UINTN)Address));
    break;

  case EfiPciWidthUint16:
    *((UINT16*)Value) = *((volatile UINT16*) ((UINTN)Address));
    break;

  case EfiPciWidthUint32:
    *((UINT32*)Value) = *((volatile UINT32*) ((UINTN)Address));
    break;

  default:
    ASSERT (FALSE);
    break;
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Memory Write Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      Memory address
 * @param[out]      Value        Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchMemWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *Value
  )
{
  switch ( EfiWidth ) {
  case EfiPciWidthUint8 :
    *((volatile UINT8*) ((UINTN)Address)) = *((UINT8*)Value);
    break;

  case EfiPciWidthUint16:
    *((volatile UINT16*) ((UINTN)Address)) = *((UINT16*)Value);
    break;

  case EfiPciWidthUint32:
    *((volatile UINT32*) ((UINTN)Address)) = *((UINT32*)Value);
    break;

  default:
    ASSERT (FALSE);
    break;
  }
}


UINT32
ModifyDataByWidth (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN OUT   VOID                                     *Data,
  IN       VOID                                     *AndMask,
  IN       VOID                                     *OrMask
  )
{
  UINT32   TempValue;

  switch (EfiWidth) {
  case EfiPciWidthUint8:
    TempValue = ((UINT32)*(UINT8 *)Data & (UINT32)*(UINT8 *) AndMask) | (UINT32)*(UINT8 *) OrMask;
    break;
  case EfiPciWidthUint16:
    TempValue = ((UINT32)*(UINT16 *)Data & (UINT32)*(UINT16 *) AndMask) | (UINT32)*(UINT16 *) OrMask;
    break;
  case EfiPciWidthUint32:
    TempValue = (*(UINT32 *)Data & *(UINT32 *) AndMask) | *(UINT32 *) OrMask;
    break;
  default:
    ASSERT (FALSE);
    TempValue = 0;
  }
  return TempValue;
}


/*----------------------------------------------------------------------------------------*/
/**
 * I/O Read-Modify-Write Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      IO address
 * @param[in]       *AndMask     Pointer to And Mask
 * @param[in]       *OrMask      Pointer to Or Mask
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchIoRw (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   Address,
  IN       VOID                                     *AndMask,
  IN       VOID                                     *OrMask
  )
{
  UINT32     FchDxeLibData32;

  LibFchIoRead (EfiWidth, Address, &FchDxeLibData32);
  FchDxeLibData32 = ModifyDataByWidth (EfiWidth, &FchDxeLibData32, AndMask, OrMask);
  LibFchIoWrite (EfiWidth, Address, &FchDxeLibData32);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Memory Read-Modify-Write Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      IO address
 * @param[in]       *AndMask     Pointer to And Mask
 * @param[in]       *OrMask      Pointer to Or Mask
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchMemRw (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *AndMask,
  IN       VOID                                     *OrMask
  )
{
  UINT32     FchDxeLibData32;

  LibFchMemRead (EfiWidth, Address, &FchDxeLibData32);
  FchDxeLibData32 = ModifyDataByWidth (EfiWidth, &FchDxeLibData32, AndMask, OrMask);
  LibFchMemWrite (EfiWidth, Address, &FchDxeLibData32);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Indirect I/O Read Access
 *
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchIndirectIoRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   IoBase,
  IN       UINT8                                    IndexAddress,
     OUT   VOID                                     *Value
  )
{
  UINT8    i;
  UINT8    ByteCount;

  ByteCount = (UINT8) EfiWidth + 1;
  for (i = 0; i < ByteCount; i++, IndexAddress++) {
    LibFchIoWrite (EfiPciWidthUint8, IoBase, &IndexAddress);
    LibFchIoRead (EfiPciWidthUint8, IoBase + 1, (UINT8 *)Value + i);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Indirect I/O Write Access
 *
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchIndirectIoWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   IoBase,
  IN       UINT8                                    IndexAddress,
  IN       VOID                                     *Value
  )
{
  UINT8    i;
  UINT8    ByteCount;

  ByteCount = (UINT8) EfiWidth + 1;
  for (i = 0; i < ByteCount; i++, IndexAddress++) {
    LibFchIoWrite (EfiPciWidthUint8, IoBase, &IndexAddress);
    LibFchIoWrite (EfiPciWidthUint8, IoBase + 1, (UINT8 *)Value + i);
  }
}


VOID
LibFchGetAcpiMmioBase (
  OUT   UINT32         *AcpiMmioBase
  )
{
  UINT32    Value32;

  Value32 = 0xFED80000;
  *AcpiMmioBase = Value32;
}


VOID
LibFchGetAcpiPmBase (
  OUT   UINT16         *AcpiPmBase
  )
{
  LibFchIndirectIoRead (EfiPciWidthUint16, FCH_IOMAP_REGCD6, FCH_PMIOA_REG60, AcpiPmBase);
}



/*----------------------------------------------------------------------------------------*/
/**
 * PCI Read Access
 *
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchPciRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *Value
  )
{
  UINTN  PciAddress;

  PciAddress = ((Address >> 4) & ~0xFFF) + (Address & 0xFFF);

  switch (EfiWidth) {
  case EfiPciWidthUint8:
    *((UINT8 *) Value) = PciRead8 (PciAddress);
    break;
  case EfiPciWidthUint16:
    *((UINT16 *) Value) = PciRead16 (PciAddress);
    break;
  case EfiPciWidthUint32:
    *((UINT32 *) Value) = PciRead32 (PciAddress);
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * PCI Write Access
 *
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchPciWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *Value
  )
{
  UINTN  PciAddress;

  PciAddress = ((Address >> 4) & ~0xFFF) + (Address & 0xFFF);

  switch (EfiWidth) {
  case EfiPciWidthUint8:
    PciWrite8 (PciAddress, *((UINT8 *) Value));
    break;
  case EfiPciWidthUint16:
    PciWrite16 (PciAddress, *((UINT16 *) Value));
    break;
  case EfiPciWidthUint32:
    PciWrite32 (PciAddress, *((UINT32 *) Value));
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}






