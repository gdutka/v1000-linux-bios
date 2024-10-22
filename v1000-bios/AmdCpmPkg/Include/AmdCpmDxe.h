/*****************************************************************************
 *
 * Copyright (C) 2014-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_DXE_H_
#define _AMD_CPM_DXE_H_

#include <AmdCpmBase.h>
#include CPM_PROTOCOL_DEFINITION (AmdCpmTableProtocol)
#include CPM_PROTOCOL_DEFINITION (AmdCpmNvDataProtocol)
#include CPM_PROTOCOL_DEFINITION (AmdCpmBootTimeRecordProtocol)
#include CPM_PROTOCOL_DEFINITION (AmdCpmDisplayFeatureProtocol)
#include CPM_PROTOCOL_DEFINITION (AmdCpmAllPciIoProtocolsInstalled)
#include CPM_GUID_DEFINITION (AmdCpmTableHob)
#include CPM_GUID_DEFINITION (AmdCpmBootTimeRecordHob)
#include EFI_PROTOCOL_DEFINITION (CpmFirmwareVolume)
#include EFI_PROTOCOL_DEFINITION (PciIo)
#include EFI_PROTOCOL_DEFINITION (PciEnumerationComplete)

#ifndef AMD_CPM_EDKII
  #include <EfiDriverLib.h>
  #include <EfiHobLib.h>
  #include <AcpiCommon.h>
  #include <pci22.h>
  #include EFI_PROTOCOL_DEFINITION (Smbus)
  #include EFI_PROTOCOL_DEFINITION (AcpiSupport)
  #include EFI_PROTOCOL_DEFINITION (LegacyBios)
  #include EFI_PROTOCOL_DEFINITION (LegacyRegion)
  #include EFI_GUID_DEFINITION (Acpi)
  #include EFI_GUID_DEFINITION (Hob)
#else
  #include <PiDxe.h>
  #include CPM_LIBRARY_DEFINITION (BaseLib)
  #include CPM_LIBRARY_DEFINITION (BaseMemoryLib)
  #include CPM_LIBRARY_DEFINITION (DebugLib)
  #include CPM_LIBRARY_DEFINITION (HobLib)
  #include CPM_LIBRARY_DEFINITION (PcdLib)
  #include CPM_LIBRARY_DEFINITION (UefiLib)
  #include CPM_LIBRARY_DEFINITION (UefiBootServicesTableLib)
  #include CPM_INDUSTRYSTANDARD_DEFINITION (Acpi)
  #include CPM_INDUSTRYSTANDARD_DEFINITION (Pci22)
  #include CPM_INDUSTRYSTANDARD_DEFINITION (SmBus)
  #include EFI_PROTOCOL_DEFINITION (SmbusHc)
  #include EFI_PROTOCOL_DEFINITION (AcpiTable)
  #include EFI_GUID_DEFINITION (EventGroup)
  #include EFI_GUID_DEFINITION (HobList)
#endif

#endif
