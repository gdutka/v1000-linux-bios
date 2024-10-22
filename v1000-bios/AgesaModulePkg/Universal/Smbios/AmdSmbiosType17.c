/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 17
 *
 * Contains code that generate SMBIOS type 17
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
#include <Library/AmdBaseLib.h>
#include <MemDmi.h>
#include "Library/UefiBootServicesTableLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Protocol/Smbios.h"
#include "Protocol/AmdSmbiosServicesProtocol.h"
#include "AmdSmbiosDxe.h"
#include "PiDxe.h"

#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSTYPE17_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
 #define MANUFACTURER_STRING_LENGTH             0x20

typedef struct {
  UINT16 DimmVendorWord;
  CONST CHAR8 *DimmVendorString;
} MANUFACTURER_STRING_PAIR;
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
ConvertManufacturer (
  IN     UINT8                                  MemoryType,
  IN     UINT64                                 ManufacturerIdCode,
     OUT CHAR8                                  *Manufacturer
  );

/* -----------------------------------------------------------------------------*/
/**
 *  DoubleMemSpeed
 *
 *  Description:
 *     Reports MT/s instead MHz for memory speed in compliance with SMBIOS 3.1.1
 *
 *  Parameters:
 *    @param[in]        Speed - Memory Speed
*
 *    @retval          DoubleSpeed
 *
 */
UINT16
STATIC
DoubleMemSpeed (
  IN       UINT16 Speed
  )
{
  UINT16 DoubleSpeed;

  if (Speed == 1600) {
    DoubleSpeed = 3200;              // DDR4-3200
  } else if (Speed == 1467) {
    DoubleSpeed = 2933;              // DDR4-2933
  } else if (Speed == 1333) {
    DoubleSpeed = 2667;              // DDR4-2667
  } else if (Speed == 1200) {
    DoubleSpeed = 2400;              // DDR4-2400
  } else if (Speed == 1067 || Speed == 1066) {
    DoubleSpeed = 2133;              // DDR4-2133
  } else if (Speed == 1050) {
    DoubleSpeed = 2100;              // DDR4-2100
  } else if (Speed == 933) {
    DoubleSpeed = 1866;              // DDR4-1866
  } else if (Speed == 800) {
    DoubleSpeed = 1600;              // DDR4-1600
  } else if (Speed == 667) {
    DoubleSpeed = 1333;              // DDR4-1333
  } else if (Speed == 533) {
    DoubleSpeed = 1066;              // DDR4-1066
  } else if (Speed == 400) {
    DoubleSpeed = 800;               // DDR4-800
  } else if (Speed == 333) {
    DoubleSpeed = 667;               // DDR4-667
  } else if (Speed == 266) {
    DoubleSpeed = 533;               // DDR4-533
  } else if (Speed == 200) {
    DoubleSpeed = 400;               // DDR4-400
  } else {
    //<Embedded_Override_Start>
    DoubleSpeed = Speed * 2;         // DDR4-1333
	//<Embedded_Override_End>
  }

  return DoubleSpeed;
}

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate SMBIOS type 17
 *
 *  Parameters:
 *    @param[in]       Smbios                       Pointer to EfiSmbiosProtocol
 *    @param[in]       MemDmiInfo                   Pointer to Memory DMI information
 *    @param[in]       Socket                       Socket number
 *    @param[in]       Channel                      Channel number
 *    @param[in]       Dimm                         Dimm number
 *    @param[in]       MemoryArrayHandle            Handle of the array where the device is mapped to
 *    @param[in]       MemoryErrorInfoHandle        Handle of the device where error is detected
 *    @param[out]      MemoryDeviceHandle           Handle of the current device
 *    @param[in]       StdHeader                    Handle to config for library and services
 *
 *    @retval          EFI_SUCCESS                  The Type 17 entry is added successfully.
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdAddSmbiosType17 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       UINT8                 Socket,
  IN       UINT8                 Channel,
  IN       UINT8                 Dimm,
  IN       EFI_SMBIOS_HANDLE     MemoryArrayHandle,
  IN       EFI_SMBIOS_HANDLE     MemoryErrorInfoHandle,
     OUT   EFI_SMBIOS_HANDLE    *MemoryDeviceHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                             StructureSize;
  UINTN                             StringSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  EFI_SMBIOS_HANDLE                 DeviceErrorHandle;
  AMD_SMBIOS_TABLE_TYPE17          *SmbiosTableType17;
  CHAR8                             *StrPtr;
  CHAR8                             Manufacturer[MANUFACTURER_STRING_LENGTH];

  Status = EFI_SUCCESS;

  // Generate DMI type 17 --- Start

  if (MemDmiInfo->T17[Socket][Channel][Dimm].MemoryType != 0) {
  // Calculate size of DMI type 17
    ConvertManufacturer (MemDmiInfo->T17[Socket][Channel][Dimm].MemoryType,
                         MemDmiInfo->T17[Socket][Channel][Dimm].ManufacturerIdCode,
                         Manufacturer);
    StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE17);
    TotalSize = StructureSize + sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].DeviceLocator);
    TotalSize += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].BankLocator);
    TotalSize += sizeof (Manufacturer);
    TotalSize += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].SerialNumber);
    TotalSize += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].PartNumber);
    ++TotalSize; // Additional null (00h), End of strings

    // Allocate zero pool
    SmbiosTableType17 = NULL;
    SmbiosTableType17 = AllocateZeroPool (TotalSize);
    if (SmbiosTableType17 == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    CalledStatus = AmdAddSmbiosType18 (Smbios, &DeviceErrorHandle, StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
    if (EFI_ERROR (CalledStatus)) {
      return EFI_PROTOCOL_ERROR;
    }

    SmbiosTableType17->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
    SmbiosTableType17->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_MEMORY_DEVICE;
    SmbiosTableType17->Hdr.Length                        = (UINT8) StructureSize;

    SmbiosTableType17->MemoryArrayHandle                 = MemoryArrayHandle;
    SmbiosTableType17->MemoryErrorInformationHandle      = DeviceErrorHandle;
    SmbiosTableType17->TotalWidth                        = MemDmiInfo->T17[Socket][Channel][Dimm].TotalWidth;
    SmbiosTableType17->DataWidth                         = MemDmiInfo->T17[Socket][Channel][Dimm].DataWidth;
    SmbiosTableType17->Size                              = MemDmiInfo->T17[Socket][Channel][Dimm].MemorySize;
    SmbiosTableType17->FormFactor                        = MemDmiInfo->T17[Socket][Channel][Dimm].FormFactor;
    SmbiosTableType17->DeviceSet                         = MemDmiInfo->T17[Socket][Channel][Dimm].DeviceSet;
    SmbiosTableType17->DeviceLocator                     = 1;
    SmbiosTableType17->BankLocator                       = 2;
    SmbiosTableType17->MemoryType                        = MemDmiInfo->T17[Socket][Channel][Dimm].MemoryType;

    LibAmdMemCopy (&SmbiosTableType17->TypeDetail, &MemDmiInfo->T17[Socket][Channel][Dimm].TypeDetail, sizeof (AMD_MEMORY_DEVICE_TYPE_DETAIL), StdHeader);

    SmbiosTableType17->Speed                             = MemDmiInfo->T17[Socket][Channel][Dimm].Speed;
    if (AmdSmbiosVersionCheck (Smbios, 3, 1)) {
      SmbiosTableType17->Speed                           = DoubleMemSpeed (SmbiosTableType17->Speed);
    }
    SmbiosTableType17->Manufacturer                      = 3;
    SmbiosTableType17->SerialNumber                      = 4;
    SmbiosTableType17->AssetTag                          = 0x00;
    SmbiosTableType17->PartNumber                        = 5;
    SmbiosTableType17->Attributes                        = MemDmiInfo->T17[Socket][Channel][Dimm].Attributes;
    SmbiosTableType17->ExtendedSize                      = MemDmiInfo->T17[Socket][Channel][Dimm].ExtSize;
    SmbiosTableType17->ConfiguredMemoryClockSpeed        = MemDmiInfo->T17[Socket][Channel][Dimm].ConfigSpeed;
    if (AmdSmbiosVersionCheck (Smbios, 3, 1)) {
      SmbiosTableType17->ConfiguredMemoryClockSpeed      = DoubleMemSpeed (SmbiosTableType17->ConfiguredMemoryClockSpeed);
    }
    SmbiosTableType17->MinimumVoltage                    = MemDmiInfo->T17[Socket][Channel][Dimm].MinimumVoltage;
    SmbiosTableType17->MaximumVoltage                    = MemDmiInfo->T17[Socket][Channel][Dimm].MaximumVoltage;
    SmbiosTableType17->ConfiguredVoltage                 = MemDmiInfo->T17[Socket][Channel][Dimm].ConfiguredVoltage;

    StrPtr = (CHAR8 *) (((UINT8 *) SmbiosTableType17) + StructureSize);
    StringSize = TotalSize - StructureSize;
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        MemDmiInfo->T17[Socket][Channel][Dimm].DeviceLocator
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        MemDmiInfo->T17[Socket][Channel][Dimm].BankLocator
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        Manufacturer
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        MemDmiInfo->T17[Socket][Channel][Dimm].SerialNumber
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        MemDmiInfo->T17[Socket][Channel][Dimm].PartNumber
        );

    // Add DMI type 17
    CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType17->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType17);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
    *MemoryDeviceHandle = SmbiosTableType17->Hdr.Handle;

    // Free pool
    FreePool (SmbiosTableType17);

    return Status;
  } else {
    return EFI_SUCCESS;
  }
}



/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 Convert the manufactureId into an ASCII string.

 @param[in]     MemoryType                      The type of memory used in this device.
 @param[in]     ManufacturerIdCode              Manufacturer ID code.
 @param[out]    Manufacturer                    Return macufacturer.

 @retval        VOID
*/
VOID
STATIC
ConvertManufacturer (
  IN       UINT8                                  MemoryType,
  IN       UINT64                                 ManufacturerIdCode,
     OUT   CHAR8                                  *Manufacturer
  )
{
  UINT16                                        i;
  UINT8                                         LowId;
  UINT8                                         HighId;
  UINT16                                        DimmVendorWord;
  BOOLEAN                                       ManufacturerFound;

  MANUFACTURER_STRING_PAIR ManufacturerStrPairs[] = {
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
  };

  LowId = 0;
  HighId = 0;
  ManufacturerFound = FALSE;

  // Initialize the DIMM Vendor Words and Strings
  ManufacturerStrPairs[0].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord0);
  ManufacturerStrPairs[1].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord1);
  ManufacturerStrPairs[2].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord2);
  ManufacturerStrPairs[3].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord3);
  ManufacturerStrPairs[4].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord4);
  ManufacturerStrPairs[5].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord5);
  ManufacturerStrPairs[6].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord6);
  ManufacturerStrPairs[7].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord7);
  ManufacturerStrPairs[8].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord8);
  ManufacturerStrPairs[9].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord9);
  ManufacturerStrPairs[10].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord10);
  ManufacturerStrPairs[11].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord11);
  ManufacturerStrPairs[12].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord12);
  ManufacturerStrPairs[13].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord13);
  ManufacturerStrPairs[14].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord14);
  ManufacturerStrPairs[15].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord15);

  ManufacturerStrPairs[0].DimmVendorString  = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr0);
  ManufacturerStrPairs[1].DimmVendorString  = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr1);
  ManufacturerStrPairs[2].DimmVendorString  = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr2);
  ManufacturerStrPairs[3].DimmVendorString  = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr3);
  ManufacturerStrPairs[4].DimmVendorString  = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr4);
  ManufacturerStrPairs[5].DimmVendorString  = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr5);
  ManufacturerStrPairs[6].DimmVendorString  = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr6);
  ManufacturerStrPairs[7].DimmVendorString  = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr7);
  ManufacturerStrPairs[8].DimmVendorString  = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr8);
  ManufacturerStrPairs[9].DimmVendorString  = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr9);
  ManufacturerStrPairs[10].DimmVendorString = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr10);
  ManufacturerStrPairs[11].DimmVendorString = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr11);
  ManufacturerStrPairs[12].DimmVendorString = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr12);
  ManufacturerStrPairs[13].DimmVendorString = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr13);
  ManufacturerStrPairs[14].DimmVendorString = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr14);
  ManufacturerStrPairs[15].DimmVendorString = (CHAR8 *)PcdGetPtr (PcdAmdSmbiosDimmVendorStr15);

  switch (MemoryType) {
  case AmdMemoryTypeDdr3:
  case AmdMemoryTypeDdr4:
  case AmdMemoryTypeLpddr3:
  case AmdMemoryTypeLpddr4:
    if (ManufacturerIdCode != 0) {
      LowId = (UINT8) (ManufacturerIdCode & 0x7F);
      HighId = (UINT8) RShiftU64 (ManufacturerIdCode, 8);

      for (i = 0; i < (sizeof (ManufacturerStrPairs) / sizeof (MANUFACTURER_STRING_PAIR)); ++i) {
        DimmVendorWord = ManufacturerStrPairs[i].DimmVendorWord;
        if ((DimmVendorWord & 0xFF) == LowId &&
            ((DimmVendorWord >> 8) & 0xFF) == HighId) {
          AsciiStrCpyS (Manufacturer, MANUFACTURER_STRING_LENGTH, ManufacturerStrPairs[i].DimmVendorString);
          ManufacturerFound = TRUE;
          break;
        }
      }
    }
    break;
  }
  if (!ManufacturerFound) {
    AsciiStrCpyS (Manufacturer, MANUFACTURER_STRING_LENGTH, "Unknown");
  }
}

