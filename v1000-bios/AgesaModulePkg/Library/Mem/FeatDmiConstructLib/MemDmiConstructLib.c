/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdMemDmiConstruct.c
 *
 * Memory DMI table support.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 329870 $ @e \$Date: 2015-10-29 13:13:29 +0800 (Thu, 29 Oct 2015) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "Library/AmdHeapLib.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "Library/AmdMemDmiConstructLib.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_FEATDMICONSTRUCTLIB_MEMDMICONSTRUCTLIB_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
VOID
STATIC
IntToString (
  IN OUT   CHAR8 *String,
  IN       UINT8 *Integer,
  IN       UINT8 SizeInByte
);

/* -----------------------------------------------------------------------------*/
/**
 *  GetDmiInfoMain
 *
 *  Description:
 *     This is the common routine for getting Dmi type4 and type7 CPU related information.
 *
 *  Parameters:
 *    @param[in]        *StdHeader
 *    @param[in, out]  **DmiTable
 *
 *    @retval         AGESA_STATUS
 *
 */
AGESA_STATUS
MemConstructDmiInfo (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   DMI_INFO              **DmiTable
  )
{
  UINT8                         Socket;
  UINT8                         Channel;
  UINT8                         Dimm;
  UINT16                        DimmIndex;
  UINT8                         MaxPhysicalDimms;
  UINT8                         MaxLogicalDimms;
  UINT32                        EccCapable;
  UINT32                        MaxCapacity;
  UINT64                        Value64;
  UINT8                         TypeDetail;
  UINT32                       *TotalMemSizePtr;
  AGESA_STATUS                  Status;
  UINT8                        *MemInfo;
  DMI_T17_MEMORY_TYPE          *DmiType17MemTypePtr;
  DMI_T17_MEMORY_TYPE           MemType;
  MEM_DMI_PHYSICAL_DIMM_INFO   *DmiPhysicalDimmInfoTable;
  MEM_DMI_LOGICAL_DIMM_INFO    *DmiLogicalDimmInfoTable;
  DMI_INFO                     *DmiBufferPtr;
  ALLOCATE_HEAP_PARAMS          AllocateHeapParams;
  LOCATE_HEAP_PTR               LocateHeapParams;
  UINT8                         NumActiveDimms;

  MemInfo = NULL;
  DmiBufferPtr = *DmiTable;

  if (DmiBufferPtr == NULL) {
    //
    // Allocate a buffer by heap function
    //
    AllocateHeapParams.BufferHandle = AMD_DMI_INFO_BUFFER_HANDLE;
    AllocateHeapParams.RequestedBufferSize = sizeof (DMI_INFO);
    AllocateHeapParams.Persist = HEAP_SYSTEM_MEM;

    if (HeapAllocateBuffer (&AllocateHeapParams, StdHeader) != AGESA_SUCCESS) {
      return AGESA_ERROR;
    }

    DmiBufferPtr = (DMI_INFO *) AllocateHeapParams.BufferPtr;
    *DmiTable = DmiBufferPtr;
  }

  IDS_HDT_CONSOLE (MEM_FLOW, "  DMI is enabled\n");

  // Fill with 0x00
  LibAmdMemFill (DmiBufferPtr, 0x00, sizeof (DMI_INFO), StdHeader);

  //------------------------------
  // T Y P E 16 17 19 20
  //------------------------------

  LocateHeapParams.BufferHandle = AMD_DMI_MEM_DEV_INFO_HANDLE;
  Status = HeapLocateBuffer (&LocateHeapParams, StdHeader);
  if (Status != AGESA_SUCCESS) {
    return Status;
  } else {
    MemInfo = (UINT8 *) (LocateHeapParams.BufferPtr);
    // Max number of physical DIMMs
    MaxPhysicalDimms = *MemInfo;
    MemInfo ++;
    // Type Detail;
    TypeDetail = *MemInfo;
    MemInfo ++;
    // Pointer to total memory size
    TotalMemSizePtr = (UINT32 *) MemInfo;
    MemInfo += sizeof (UINT32);
    // ECC Type
    EccCapable = *((UINT32 *) MemInfo);
    MemInfo += sizeof (UINT32);
    // Memory Type
    DmiType17MemTypePtr = (DMI_T17_MEMORY_TYPE *) MemInfo;
    MemType = *DmiType17MemTypePtr;
    DmiType17MemTypePtr ++;
    MemInfo = (UINT8 *) DmiType17MemTypePtr;
    // Pointer to DMI info of Physical DIMMs
    DmiPhysicalDimmInfoTable = (MEM_DMI_PHYSICAL_DIMM_INFO *) MemInfo;
    // TYPE 16
    DmiBufferPtr->T16.Location = 0x03;
    DmiBufferPtr->T16.Use = 0x03;

    // TYPE 16 ECC Type
    if (EccCapable != 0) {
      DmiBufferPtr->T16.MemoryErrorCorrection = Dmi16MultiBitEcc;
    } else {
      DmiBufferPtr->T16.MemoryErrorCorrection = Dmi16NoneErrCorrection;
    }

    MaxCapacity = *TotalMemSizePtr;

    DmiBufferPtr->T16.NumberOfMemoryDevices = (UINT16) MaxPhysicalDimms;

    NumActiveDimms = 0;
    // TYPE 17 entries are organized by physical DIMMs
    for (DimmIndex = 0; DimmIndex < MaxPhysicalDimms; DimmIndex ++, DmiPhysicalDimmInfoTable ++) {
      Socket = DmiPhysicalDimmInfoTable->Socket;
      Channel = DmiPhysicalDimmInfoTable->Channel;
      Dimm = DmiPhysicalDimmInfoTable->Dimm;

      DmiBufferPtr->T17[Socket][Channel][Dimm].Handle = DmiPhysicalDimmInfoTable->Handle;
      DmiBufferPtr->T17[Socket][Channel][Dimm].TotalWidth = DmiPhysicalDimmInfoTable->TotalWidth;
      DmiBufferPtr->T17[Socket][Channel][Dimm].DataWidth = DmiPhysicalDimmInfoTable->DataWidth;
      DmiBufferPtr->T17[Socket][Channel][Dimm].MemorySize = DmiPhysicalDimmInfoTable->MemorySize;
      DmiBufferPtr->T17[Socket][Channel][Dimm].FormFactor = DmiPhysicalDimmInfoTable->FormFactor;
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceSet = 0;

      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[0] = 'D';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[1] = 'I';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[2] = 'M';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[3] = 'M';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[4] = ' ';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[5] = Dimm + 0x30;
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[6] = '\0';
      DmiBufferPtr->T17[Socket][Channel][Dimm].DeviceLocator[7] = '\0';

      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[0] = 'C';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[1] = 'H';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[2] = 'A';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[3] = 'N';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[4] = 'N';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[5] = 'E';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[6] = 'L';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[7] = ' ';
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[8] = Channel + 0x41;
      DmiBufferPtr->T17[Socket][Channel][Dimm].BankLocator[9] = '\0';

      DmiBufferPtr->T17[Socket][Channel][Dimm].MemoryType = UnknownMemType;
      if (DmiPhysicalDimmInfoTable->MemorySize != 0) {
        DmiBufferPtr->T17[Socket][Channel][Dimm].MemoryType = MemType;
        NumActiveDimms ++;
        DmiBufferPtr->T17[Socket][Channel][Dimm].TypeDetail.Synchronous = 1;
        if (TypeDetail == 1) {
          DmiBufferPtr->T17[Socket][Channel][Dimm].TypeDetail.Registered = 1;
        } else if  (TypeDetail == 2) {
          DmiBufferPtr->T17[Socket][Channel][Dimm].TypeDetail.Unbuffered = 1;
        } else {
          DmiBufferPtr->T17[Socket][Channel][Dimm].TypeDetail.Unknown = 1;
        }
      }
      DmiBufferPtr->T17[Socket][Channel][Dimm].Speed = DmiPhysicalDimmInfoTable->Speed;

      DmiBufferPtr->T17[Socket][Channel][Dimm].ManufacturerIdCode = DmiPhysicalDimmInfoTable->ManufacturerIdCode;

      IntToString (DmiBufferPtr->T17[Socket][Channel][Dimm].SerialNumber, DmiPhysicalDimmInfoTable->SerialNumber, (sizeof DmiBufferPtr->T17[Socket][Channel][Dimm].SerialNumber - 1) / 2);

      LibAmdMemCopy (&DmiBufferPtr->T17[Socket][Channel][Dimm].PartNumber, &DmiPhysicalDimmInfoTable->PartNumber, sizeof (DmiBufferPtr->T17[Socket][Channel][Dimm].PartNumber), StdHeader);

      DmiBufferPtr->T17[Socket][Channel][Dimm].Attributes = DmiPhysicalDimmInfoTable->Attributes;
      DmiBufferPtr->T17[Socket][Channel][Dimm].ExtSize = DmiPhysicalDimmInfoTable->ExtSize;
      DmiBufferPtr->T17[Socket][Channel][Dimm].ConfigSpeed = DmiPhysicalDimmInfoTable->ConfigSpeed;
      DmiBufferPtr->T17[Socket][Channel][Dimm].MinimumVoltage = DmiPhysicalDimmInfoTable->MinimumVoltage;
      DmiBufferPtr->T17[Socket][Channel][Dimm].MaximumVoltage = DmiPhysicalDimmInfoTable->MaximumVoltage;
      DmiBufferPtr->T17[Socket][Channel][Dimm].ConfiguredVoltage = DmiPhysicalDimmInfoTable->ConfiguredVoltage;
    }

    // Max number of logical DIMMs
    MemInfo = (UINT8 *) DmiPhysicalDimmInfoTable;
    MaxLogicalDimms = *MemInfo;
    MemInfo ++;

    // Pointer to DMI info of Logical DIMMs
    DmiLogicalDimmInfoTable = (MEM_DMI_LOGICAL_DIMM_INFO *) MemInfo;

    // TYPE 20 entries are organized by logical DIMMs
    for (DimmIndex = 0; DimmIndex < MaxLogicalDimms; DimmIndex ++, DmiLogicalDimmInfoTable ++) {
      Socket = DmiLogicalDimmInfoTable->Socket;
      Channel = DmiLogicalDimmInfoTable->Channel;
      Dimm = DmiLogicalDimmInfoTable->Dimm;
      DmiBufferPtr->T20[Socket][Channel][Dimm].PartitionRowPosition = 0xFF;
      if (DmiLogicalDimmInfoTable->Interleaved) {
        DmiBufferPtr->T20[Socket][Channel][Dimm].InterleavePosition = 0xFF;
        DmiBufferPtr->T20[Socket][Channel][Dimm].InterleavedDataDepth = 0xFF;
      }

      if (DmiLogicalDimmInfoTable->DimmPresent) {
        DmiBufferPtr->T20[Socket][Channel][Dimm].MemoryDeviceHandle = DmiLogicalDimmInfoTable->MemoryDeviceHandle;
        DmiBufferPtr->T20[Socket][Channel][Dimm].StartingAddr = DmiLogicalDimmInfoTable->StartingAddr;
        DmiBufferPtr->T20[Socket][Channel][Dimm].EndingAddr = DmiLogicalDimmInfoTable->EndingAddr;
        DmiBufferPtr->T20[Socket][Channel][Dimm].ExtStartingAddr = DmiLogicalDimmInfoTable->ExtStartingAddr;
        DmiBufferPtr->T20[Socket][Channel][Dimm].ExtEndingAddr = DmiLogicalDimmInfoTable->ExtEndingAddr;
      }
    }

    // TYPE 19
    DmiBufferPtr->T19[0].StartingAddr = 0;
    DmiBufferPtr->T19[0].ExtStartingAddr = 0;
    DmiBufferPtr->T19[0].ExtEndingAddr = 0;

    // If Ending Address >= 0xFFFFFFFF, update Starting Address (offset 04h) & Ending Address (offset 08h) to 0xFFFFFFFF,
    // and use the Extended Starting Address (offset 0Fh) & Extended Ending Address (offset 17h) instead.
    Value64 = (UINT64) ((MaxCapacity << 10) - 1);
    // Value64 = EndingAddr = MaxCapacity in KByte - 1
    if (Value64 >= ((UINT64) 0xFFFFFFFF)) {
      DmiBufferPtr->T19[0].StartingAddr = 0xFFFFFFFFUL;
      DmiBufferPtr->T19[0].EndingAddr = 0xFFFFFFFFUL;
      // In Byte
      DmiBufferPtr->T19[0].ExtEndingAddr = Value64 << 10;
    } else {
      // In KByte
      DmiBufferPtr->T19[0].EndingAddr = (UINT32) Value64;
    }

    DmiBufferPtr->T19[0].PartitionWidth = NumActiveDimms;
  }
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  IntToString
 *
 *  Description:
 *    Translate UINT array to CHAR array.
 *
 *  Parameters:
 *    @param[in, out]    *String       Pointer to CHAR array
 *    @param[in]         *Integer      Pointer to UINT array
 *    @param[in]         SizeInByte    The size of UINT array
 *
 *  Processing:
 *
 */
VOID
STATIC
IntToString (
  IN OUT   CHAR8 *String,
  IN       UINT8 *Integer,
  IN       UINT8 SizeInByte
  )
{
  UINT8 Index;

  for (Index = 0; Index < SizeInByte; Index++) {
    *(String + Index * 2) = (*(Integer + Index) >> 4) & 0x0F;
    *(String + Index * 2 + 1) = *(Integer + Index) & 0x0F;
  }
  for (Index = 0; Index < (SizeInByte * 2); Index++) {
    if (*(String + Index) >= 0x0A) {
      *(String + Index) += 0x37;
    } else {
      *(String + Index) += 0x30;
    }
  }
  *(String + SizeInByte * 2) = 0x0;
}
