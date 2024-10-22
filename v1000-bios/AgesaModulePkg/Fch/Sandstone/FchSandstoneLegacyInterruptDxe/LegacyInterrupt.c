/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   Legacy Interrupt DXE Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "LegacyInterrupt.h"
#define FILECODE UEFI_DXE_LEGACYINTERRUPT_LEGACYINTERRUPT_FILECODE


// Get the number of PIRQs supported.
EFI_STATUS
AmdGetNumberPirqs (
  IN       EFI_LEGACY_INTERRUPT_PROTOCOL *This,
     OUT   UINT8                         *NumberPirqs
  )
{
  if (NULL == NumberPirqs) {
    return EFI_INVALID_PARAMETER;
  }

  *NumberPirqs = MAX_NUMBER_PIRQS;
  return EFI_SUCCESS;
}


// Get the PCI bus#, device# and function# that associated with this protocol.
EFI_STATUS
AmdGetLocation (
  IN       EFI_LEGACY_INTERRUPT_PROTOCOL *This,
     OUT   UINT8                         *Bus,
     OUT   UINT8                         *Device,
     OUT   UINT8                         *Function
  )
{
  if ((NULL == Bus) || (NULL == Device) || (NULL == Function)) {
    return EFI_INVALID_PARAMETER;
  }

  *Bus      = FCH_PCI_BUS;
  *Device   = FCH_PCI_DEV;
  *Function = FCH_PCI_FUNC;

  return EFI_SUCCESS;
}


// Read the given PIRQ register and return the IRQ value that is assigned to it.
EFI_STATUS
AmdReadPirq (
  IN       EFI_LEGACY_INTERRUPT_PROTOCOL *This,
  IN       UINT8                         PirqNumber,
     OUT   UINT8                         *PirqData
  )
{
  if (NULL == PirqData) {
    return EFI_INVALID_PARAMETER;
  }
  LibFchIndirectIoRead (EfiPciWidthUint8, FCH_IOMAP_REGC00, PirqNumber, PirqData);

  return EFI_SUCCESS;
}


// Write an IRQ value to the specified PIRQ register.
EFI_STATUS
AmdWritePirq (
  IN       EFI_LEGACY_INTERRUPT_PROTOCOL *This,
  IN       UINT8                         PirqNumber,
  IN       UINT8                         PirqData
  )
{
  //if ((PirqNumber >= MAX_NUMBER_PIRQS) || (PirqData > 15)) {
  //  return EFI_INVALID_PARAMETER;
  //}
  LibFchIndirectIoWrite (EfiPciWidthUint8, FCH_IOMAP_REGC00, PirqNumber, &PirqData);

  return EFI_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Name: AmdInstallLegacyInterrupt
 *
 * This DXE driver produces the legacy interrupt protocol as part of the EfiCompatibility
 * component in CSM. This protocol will manage the programming of PCI IRQ assignment.
 *
 * @param[in]    ImageHandle     Pointer to the image handle
 * @param[in]    SystemTable     Pointer to the EFI system table
 *
 * @retval       EFI_SUCCESS     Driver initialized successfully
 * @retval       EFI_ERROR       Driver initialization failed
 *
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
AmdInstallLegacyInterrupt (
  IN       EFI_HANDLE              ImageHandle,
  IN       EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_LEGACY_INTERRUPT_PROTOCOL    *LegacyInterrupt;
  VOID                             *Instance;
  UINT8                            i;
  UINT8                            bValue;
  EFI_STATUS                       Status;

  //
  // Initialize PCI IRQ routing registers for INTA#-INTH#
  //
  for (i = 0; i < 8; i++) {
    bValue = i | BIT7;                                 // Select IRQ routing to APIC
    LibFchIoWrite (EfiPciWidthUint8, FCH_IOMAP_REGC00, &bValue);
    bValue = i | BIT4;
    LibFchIoWrite (EfiPciWidthUint8, FCH_IOMAP_REGC01, &bValue);
  }

  //
  // Check and see if the protocol has been installed already
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyInterruptProtocolGuid,
                  NULL,
                  &Instance
                  );
  ASSERT (EFI_ERROR (Status));

  //
  // Allocate memory and initialize the data structure
  //
//[-start-240426-IB18160212-modify-]//
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (EFI_LEGACY_INTERRUPT_PROTOCOL),
                  (VOID**)&LegacyInterrupt
                  );
//[end-240426-IB18160212-modify]//
  ASSERT_EFI_ERROR (Status);

  LegacyInterrupt->GetNumberPirqs = AmdGetNumberPirqs;
  LegacyInterrupt->GetLocation    = AmdGetLocation;
  LegacyInterrupt->ReadPirq       = AmdReadPirq;
  LegacyInterrupt->WritePirq      = AmdWritePirq;


  //
  // Finally install the protocol
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiLegacyInterruptProtocolGuid,
                  LegacyInterrupt,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return (Status);
}


