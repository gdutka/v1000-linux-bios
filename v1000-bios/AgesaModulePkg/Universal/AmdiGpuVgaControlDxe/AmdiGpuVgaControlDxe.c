/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB VGA decode control
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   GNB DXE Driver
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>

#include <Protocol\PciRootBridgeIo.h>
#include <Protocol\PciIo.h>
#include <Library\S3BootScriptLib.h>

#define FILECODE UEFI_DXE_AMDIGPUVGACONTROLDXE_AMDIGPUVGACONTROLDXE_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define VGA_ATTRIBUTES (EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO)


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

EFI_EVENT                             PciIoEvent;
VOID                                  *mRegistrationForPciIo;
EFI_PCI_IO_PROTOCOL_ATTRIBUTES        mGenericAttributes;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       *mPciRootBridgeIo = NULL;


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
EFIAPI
HookiGpuPciIo (
  IN      EFI_EVENT         Event,
  IN      VOID              *Context
  );

VOID
WritePci32 (
  IN      UINT64            Address,
  IN      UINT32            Value,
  IN      BOOLEAN           Save
  );

UINT32
ReadPci32 (
  IN      UINT64            Address
  );

VOID
SetVgaDecode (
  IN      BOOLEAN           VgaDecode
  );



/**
 *---------------------------------------------------------------------------------------
 *
 *  AmdiGpuVgaControlDxeInit
 *
 *  Description:
 *     Monitor attribute enablement request
 *
 *  Parameters:
 *    @param[in]     ImageHandle
 *    @param[in]     SystemTable
 *
 *    @retval         EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/

EFI_STATUS
EFIAPI
AmdiGpuVgaControlDxeInit (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                      Status;
  //
  // Initialize Global Variable
  //

  //
  // Locate Root Bridge IO
  //
  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &mPciRootBridgeIo
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Register the event handling function for PCIO install protocol
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  HookiGpuPciIo,
                  NULL,
                  NULL,
                  &PciIoEvent
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->RegisterProtocolNotify (
                  &gEfiPciIoProtocolGuid,
                  PciIoEvent,
                  &mRegistrationForPciIo
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Disable VGA decoding by default (enablement would have to be requested through PciIo protocol)
  //
  SetVgaDecode (FALSE);
  return Status;
}


/**
 *---------------------------------------------------------------------------------------
 *
 *  AmdiGpuAttributes
 *
 *  Description:
 *     Monitor attribute enablement request
 *
 *  Parameters:
 *    @param[in]     This
 *    @param[in]     Operation
 *    @param[in]     Attributes
 *    @param[out]    OPTIONAL Result
 *
 *    @retval        EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/

EFI_STATUS
AmdiGpuAttributes (
  IN       EFI_PCI_IO_PROTOCOL                       *This,
  IN       EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION  Operation,
  IN       UINT64                                   Attributes,
     OUT   UINT64                                   *Result OPTIONAL
  )
{
  if (Operation == EfiPciIoAttributeOperationSet || Operation == EfiPciIoAttributeOperationEnable) {
    if ((Attributes & VGA_ATTRIBUTES) != 0) {
      SetVgaDecode (TRUE);
    }
  } else if (Operation == EfiPciIoAttributeOperationDisable) {
    if ((Attributes & VGA_ATTRIBUTES) != 0) {
      SetVgaDecode (FALSE);
    }
  }
  return mGenericAttributes (This, Operation, Attributes, Result);
}


/**
 *---------------------------------------------------------------------------------------
 *
 *  HookiGpuPciIo
 *
 *  Description:
 *     Calls hook to PCIIO protocol for iGPU
 *
 *  Parameters:
 *    @param[in]     Event
 *    @param[in]     *Context
 *
 *    @retval         VOID
 *
 *---------------------------------------------------------------------------------------
 **/

VOID
EFIAPI
HookiGpuPciIo (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS           Status;
  EFI_HANDLE           Handle;
  UINTN                Size;
  EFI_PCI_IO_PROTOCOL  *PciIo;
  UINTN                Segment;
  UINTN                Bus;
  UINTN                Device;
  UINTN                Function;

  Size = sizeof (EFI_HANDLE);
  Status = gBS->LocateHandle (
                  ByRegisterNotify,
                  NULL,
                  mRegistrationForPciIo,
                  &Size,
                  &Handle
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Locate PCIIO protocol associated with registration
  //
  Status = gBS->HandleProtocol (Handle, &gEfiPciIoProtocolGuid, &PciIo);
  ASSERT_EFI_ERROR (Status);
  //
  // Determine device managed by protocol
  //
  Status = PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
  if ((Bus == 0) && (Device == 1) && (Function == 0)) {
    //
    // Hook to function that set attributed
    //
    mGenericAttributes = PciIo->Attributes;
    PciIo->Attributes = AmdiGpuAttributes;
    gBS->CloseEvent (Event);
  }
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  SetVgaDecode
 *
 *  Description:
 *     Control VGA decode
 *
 *  Parameters:
 *    @param[in]     VgaDecode
 *
 *
 *---------------------------------------------------------------------------------------
 **/

VOID
SetVgaDecode (
  IN    BOOLEAN   VgaDecode
  )
{
  UINT32 Value;

  WritePci32 (EFI_PCI_ADDRESS (0, 0, 0, 0x60), 0x9D, TRUE);
  Value = ReadPci32 (EFI_PCI_ADDRESS (0, 0, 0, 0x64));
  Value &= ~(0x2);
  if (VgaDecode) {
    Value |= 0x2;
  }
  WritePci32 (EFI_PCI_ADDRESS (0, 0, 0, 0x64), Value, TRUE);
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  WritePci32
 *
 *  Description:
 *    Write PCI DWORD
 *
 *  Parameters:
 *    @param[in]     Address
 *    @param[in]     Value
 *    @param[in]     Save
 *
 *    @retval         Content of PCI register
 *---------------------------------------------------------------------------------------
 **/
VOID
WritePci32 (
  IN      UINT64   Address,
  IN      UINT32   Value,
  IN      BOOLEAN  Save
  )
{

  mPciRootBridgeIo->Pci.Write (mPciRootBridgeIo, EfiPciIoWidthUint32, Address, 1, &Value);

  if (Save) {
    S3BootScriptSavePciCfgWrite (
                  EfiPciIoWidthUint32,
                  Address,
                  1,
                  &Value);
  }
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  ReadPci32
 *
 *  Description:
 *    Read PCI DWORD
 *
 *  Parameters:
 *    @param[in]     Address
 *
 *---------------------------------------------------------------------------------------
 **/
UINT32
ReadPci32 (
  IN      UINT64   Address
  )
{
  UINT32  Value;

  mPciRootBridgeIo->Pci.Read (mPciRootBridgeIo, EfiPciIoWidthUint32, Address, 1, &Value);

  return Value;
}

