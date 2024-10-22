/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 4
 *
 * Contains code that generate SMBIOS type 4
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Universal
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "AmdSmBios.h"
#include "AMD.h"
#include "Porting.h"
#include "Filecode.h"
#include "Library/IdsLib.h"
#include "Library/BaseLib.h"
#include "Library/AmdBaseLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Protocol/Smbios.h"
#include "Protocol/AmdSmbiosServicesProtocol.h"
#include "PiDxe.h"

#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSTYPE4_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
CHAR8 ROMDATA str_ProcManufacturer[] = "Unknown";
CHAR8 ROMDATA str_ProcVersion[] = "Not Installed";

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate SMBIOS type 4
 *
 *  Parameters:
 *    @param[in]       Socket             Socket number
 *    @param[in]       CacheHandle        Handle of L1 L2 & L3 cache
 *    @param[in]       Smbios             Pointer to EfiSmbiosProtocol
 *    @param[in]       StdHeader          Handle to config for library and services
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdAddSmbiosType4 (
  IN       UINTN                 Socket,
  IN       UINT16                CacheHandle[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                             StructureSize;
  UINTN                             StringSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  AMD_SMBIOS_TABLE_TYPE4           *SmbiosTableType4;
  CHAR8                             *StrPtr;
  AMD_CCX_CORE_DMI_INFO             AmdCoreDmiInfo;
  AMD_CCX_SMBIOS_SERVICES_PROTOCOL *AmdSmbiosServices;

  Status = EFI_SUCCESS;

  // Calculate size of DMI type 4
  StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE4);
  TotalSize = StructureSize;
  switch (Socket) {
  case 0:
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket0));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosSerialNumberSocket0));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosAssetTagSocket0));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosPartNumberSocket0));
    break;
  case 1:
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket1));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosSerialNumberSocket1));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosAssetTagSocket1));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosPartNumberSocket1));
    break;
  default:
    // Only support up to 2 sockets
    ASSERT (FALSE);
  }
  TotalSize += sizeof (AmdCoreDmiInfo.ProcessorVersion);
  TotalSize += sizeof (AmdCoreDmiInfo.ProcessorManufacturer);
  ++TotalSize; // Additional null (00h), End of strings

  // Allocate zero pool
  SmbiosTableType4 = NULL;
  SmbiosTableType4 = AllocateZeroPool (TotalSize);
  if (SmbiosTableType4 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Generate DMI type 4 --- Start
  CalledStatus = gBS->LocateProtocol (&gAmdCcxSmbiosServicesProtocolGuid, NULL, (VOID **)&AmdSmbiosServices);
  if (EFI_ERROR (CalledStatus)) {
    return EFI_PROTOCOL_ERROR;
  }
  AmdSmbiosServices->GetCoreDmiInfo (AmdSmbiosServices, Socket, &AmdCoreDmiInfo);

  SmbiosTableType4->Hdr.Type             = AMD_EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  SmbiosTableType4->Hdr.Length           = (UINT8) StructureSize;
  SmbiosTableType4->Hdr.Handle           = AMD_SMBIOS_HANDLE_PI_RESERVED;

  SmbiosTableType4->Socket               = 1;
  SmbiosTableType4->ProcessorManufacture = 2;
  SmbiosTableType4->ProcessorVersion     = 3;
  SmbiosTableType4->SerialNumber         = 4;
  SmbiosTableType4->AssetTag             = 5;
  SmbiosTableType4->PartNumber           = 6;

  SmbiosTableType4->ProcessorType        = AmdCoreDmiInfo.ProcessorType;
  if (AmdCoreDmiInfo.ProcessorFamily < 0xFE) {
    SmbiosTableType4->ProcessorFamily    = (UINT8) AmdCoreDmiInfo.ProcessorFamily;
    SmbiosTableType4->ProcessorFamily2   = SmbiosTableType4->ProcessorFamily;
  } else {
    SmbiosTableType4->ProcessorFamily    = 0xFE;
    SmbiosTableType4->ProcessorFamily2   = AmdCoreDmiInfo.ProcessorFamily;
  }
  *((UINT32 *) &SmbiosTableType4->ProcessorId.Signature)    = AmdCoreDmiInfo.ProcessorID.EaxFamilyId;
  *((UINT32 *) &SmbiosTableType4->ProcessorId.FeatureFlags) = AmdCoreDmiInfo.ProcessorID.EdxFeatureId;
  *((UINT8  *) &SmbiosTableType4->Voltage)                  = AmdCoreDmiInfo.Voltage;
  SmbiosTableType4->ExternalClock        = AmdCoreDmiInfo.ExternalClock;
  SmbiosTableType4->MaxSpeed             = AmdCoreDmiInfo.MaxSpeed;
  SmbiosTableType4->CurrentSpeed         = AmdCoreDmiInfo.CurrentSpeed;
  SmbiosTableType4->Status               = AmdCoreDmiInfo.Status;
  SmbiosTableType4->ProcessorUpgrade     = AmdCoreDmiInfo.ProcessorUpgrade;
  SmbiosTableType4->L1CacheHandle        = CacheHandle[CpuL1Cache];
  SmbiosTableType4->L2CacheHandle        = CacheHandle[CpuL2Cache];
  SmbiosTableType4->L3CacheHandle        = CacheHandle[CpuL3Cache];
  if (AmdCoreDmiInfo.CoreCount >= 0xFF) {
    SmbiosTableType4->CoreCount          = 0xFF;
    SmbiosTableType4->CoreCount2         = AmdCoreDmiInfo.CoreCount;
  } else {
    SmbiosTableType4->CoreCount          = (UINT8) AmdCoreDmiInfo.CoreCount;
    SmbiosTableType4->CoreCount2         = AmdCoreDmiInfo.CoreCount;
  }
  if (AmdCoreDmiInfo.CoreEnabled >= 0xFF) {
    SmbiosTableType4->EnabledCoreCount   = 0xFF;
    SmbiosTableType4->EnabledCoreCount2  = AmdCoreDmiInfo.CoreEnabled;
  } else {
    SmbiosTableType4->EnabledCoreCount   = (UINT8) AmdCoreDmiInfo.CoreEnabled;
    SmbiosTableType4->EnabledCoreCount2  = AmdCoreDmiInfo.CoreEnabled;
  }
  if (AmdCoreDmiInfo.ThreadCount >= 0xFF) {
    SmbiosTableType4->ThreadCount        = 0xFF;
    SmbiosTableType4->ThreadCount2       = AmdCoreDmiInfo.ThreadCount;
  } else {
    SmbiosTableType4->ThreadCount        = (UINT8) AmdCoreDmiInfo.ThreadCount;
    SmbiosTableType4->ThreadCount2       = AmdCoreDmiInfo.ThreadCount;
  }
  SmbiosTableType4->ProcessorCharacteristics = AmdCoreDmiInfo.ProcessorCharacteristics;
  // Generate DMI type 4 --- End


  // Append all strings
  StrPtr = (((CHAR8 *) SmbiosTableType4) + StructureSize);
  StringSize = TotalSize - StructureSize;
  switch (Socket) {
  case 0:
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket0)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        AmdCoreDmiInfo.ProcessorManufacturer
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        AmdCoreDmiInfo.ProcessorVersion
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosSerialNumberSocket0)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosAssetTagSocket0)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosPartNumberSocket0)
        );
    break;
  case 1:
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket1)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        AmdCoreDmiInfo.ProcessorManufacturer
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        AmdCoreDmiInfo.ProcessorVersion
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosSerialNumberSocket1)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosAssetTagSocket1)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosPartNumberSocket1)
        );
    break;
  default:
    // Only support up to 2 sockets
    ASSERT (FALSE);
  }
  // Add DMI type 4
  CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType4->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType4);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  // Free pool
  FreePool (SmbiosTableType4);

  return Status;
}


EFI_STATUS
EFIAPI
AmdAddSmbiosType4ForNoProcInstalled (
  IN       UINTN                 Socket,
  IN       UINT16                CacheHandle[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                             StructureSize;
  UINTN                             StringSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  AMD_SMBIOS_TABLE_TYPE4           *SmbiosTableType4;
  CHAR8                             *StrPtr;

  Status = EFI_SUCCESS;

  // Calculate size of DMI type 4
  StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE4);
  TotalSize = StructureSize;
  switch (Socket) {
  case 0:
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket0));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosSerialNumberSocket0));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosAssetTagSocket0));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosPartNumberSocket0));
    break;
  case 1:
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket1));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosSerialNumberSocket1));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosAssetTagSocket1));
    TotalSize += AsciiStrSize ((CHAR8 *)PcdGetPtr (PcdAmdSmbiosPartNumberSocket1));
    break;
  default:
    // Only support up to 2 sockets
    ASSERT (FALSE);
  }
  TotalSize += sizeof (str_ProcVersion);
  TotalSize += sizeof (str_ProcManufacturer);
  ++TotalSize; // Additional null (00h), End of strings

  // Allocate zero pool
  SmbiosTableType4 = NULL;
  SmbiosTableType4 = AllocateZeroPool (TotalSize);
  if (SmbiosTableType4 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Generate DMI type 4 --- Start

  SmbiosTableType4->Hdr.Type             = AMD_EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  SmbiosTableType4->Hdr.Length           = (UINT8) StructureSize;
  SmbiosTableType4->Hdr.Handle           = AMD_SMBIOS_HANDLE_PI_RESERVED;

  SmbiosTableType4->Socket               = 1;
  SmbiosTableType4->ProcessorManufacture = 2;
  SmbiosTableType4->ProcessorVersion     = 3;
  SmbiosTableType4->SerialNumber         = 4;
  SmbiosTableType4->AssetTag             = 5;
  SmbiosTableType4->PartNumber           = 6;

  SmbiosTableType4->ProcessorType        = 0x2;
  SmbiosTableType4->ProcessorFamily      = 0x2;
  SmbiosTableType4->ProcessorFamily2     = 0x2;
  SmbiosTableType4->ProcessorUpgrade     = 0x2;
  SmbiosTableType4->L1CacheHandle        = 0xFFFF;
  SmbiosTableType4->L2CacheHandle        = 0xFFFF;
  SmbiosTableType4->L3CacheHandle        = 0xFFFF;
  SmbiosTableType4->ProcessorCharacteristics = 0x2;
  // Generate DMI type 4 --- End


  // Append all strings
  StrPtr = (((CHAR8 *) SmbiosTableType4) + StructureSize);
  StringSize = TotalSize - StructureSize;
  switch (Socket) {
  case 0:
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket0)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        str_ProcManufacturer
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        str_ProcVersion
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosSerialNumberSocket0)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosAssetTagSocket0)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosPartNumberSocket0)
        );
    break;
  case 1:
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket1)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        str_ProcManufacturer
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        str_ProcVersion
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosSerialNumberSocket1)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosAssetTagSocket1)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        (CHAR8 *)PcdGetPtr (PcdAmdSmbiosPartNumberSocket1)
        );
    break;
  default:
    // Only support up to 2 sockets
    ASSERT (FALSE);
  }
  // Add DMI type 4
  CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType4->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType4);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  // Free pool
  FreePool (SmbiosTableType4);

  return Status;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


