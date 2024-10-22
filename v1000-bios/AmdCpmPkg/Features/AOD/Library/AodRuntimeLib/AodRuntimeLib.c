/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>

#include <Features/Aod/Include/AodRuntimeLib.h>

static UINT16 MemFreqTranslateTable [][2] = {
  {0x04, 333},
  {0x06, 400},
  {0x0A, 533},
  {0x14, 667},
  {0x18, 800},
  {0x19, 1050},
  {0x1A, 1066},
  {0x1C, 933},
  {0x20, 1067},
  {0x24, 1200},
  {0x28, 1333},
  {0x29, 1367},
  {0x2A, 1400},
  {0x2B, 1433},
  {0x2C, 1467},
  {0x2D, 1500},
  {0x2E, 1533},
  {0x2F, 1567},
  {0x30, 1600},
  {0x31, 1633},
  {0x32, 1667},
  {0x33, 1700},
  {0x34, 1733},
  {0x35, 1767},
  {0x36, 1800},
  {0x37, 1833},
  {0x38, 1867},
  {0x39, 1900},
  {0x3A, 1933},
  {0x3B, 1967},
  {0x3C, 2000},
  {0x3D, 2033},
  {0x3E, 2067},
  {0x3F, 2100},
  {0x40, 2133},
  {0x41, 2167},
  {0x42, 2200},
  {0xFF, 0xFF}
};

VOID
AodSmnRead (
  IN       UINT8               DieBusNum,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value
  )
{
  UINT32    RegIndex;
  UINTN     PciAddress;

  RegIndex = SmnAddress;
  PciAddress = (DieBusNum << 20) + 0xB8;
  PciWrite32 (PciAddress, RegIndex);
  PciAddress = (DieBusNum << 20) + 0xBC;
  *Value = PciRead32 (PciAddress);
}

VOID
AodSmnWrite (
  IN       UINT8               DieBusNum,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value
  )
{
  UINT32    RegIndex;
  UINTN     PciAddress;

  RegIndex = SmnAddress;
  PciAddress = (DieBusNum << 20) + 0xB8;
  PciWrite32 (PciAddress, RegIndex);
  PciAddress = (DieBusNum << 20) + 0xBC;
  PciWrite32 (PciAddress, *Value);
}


BOOLEAN
VerifyAcpiSmiTransferBuffer (
  AodNvAreaStruct *AodNvAreaPtr
  )
{
  if (AodNvAreaPtr->AodMemBuffer.Version == AOD_MEMORY_BUFFER_VERSION) {
    AodNvAreaPtr->Reserved1 += 0x100;  // Just for test purpose to record function execution times
    return TRUE;
  } else {
    return FALSE;
  }
}


VOID
FinalizeTransferBuffer (
  AodNvAreaStruct *AodNvAreaPtr,
  EFI_STATUS  Status
  )
{
  AodNvAreaPtr->AodSmiCmd = 0;
  AodNvAreaPtr->AodSmiStatus = (UINT32) Status;
  AodNvAreaPtr->AodMemBuffer.Version = 0;     // Clear the version field to avoid multiple entries of SMI handler
  AodNvAreaPtr->AodMemBuffer.Command = 0;
  AodNvAreaPtr->Reserved1 += 1;  // Just for test purpose to record the finalize calling times
  DisableSmMasterDecode ();
}


EFI_STATUS
GetMemFreqIndexByClk (
  UINT16    MemFreqClk,
  UINT8     *FreqIndex
  )
{
  UINT8 Index = 0;
  if (FreqIndex == NULL) {
    return EFI_UNSUPPORTED;
  }

  while (MemFreqTranslateTable[Index][0] != 0xFF) {
    if (MemFreqTranslateTable[Index][1] == MemFreqClk) {
      *FreqIndex = (UINT8) MemFreqTranslateTable[Index][0];
      return EFI_SUCCESS;
    }
    Index++;
  }

  return EFI_NOT_FOUND;
}


EFI_STATUS
GetMemFreqClkByIndex (
  UINT8       FreqIndex,
  UINT16     *MemFreqClk
  )
{
  UINT8 Index = 0;
  if (MemFreqClk == NULL) {
    return EFI_UNSUPPORTED;
  }

  while (MemFreqTranslateTable[Index][0] != 0xFF) {
    if (MemFreqTranslateTable[Index][0] == FreqIndex) {
      *MemFreqClk = MemFreqTranslateTable[Index][1];
      return EFI_SUCCESS;
    }
    Index++;
  }

  return EFI_NOT_FOUND;
}

UINT8
HammingWeight (
  IN UINT32 Value
  )
{
   UINT8 Count;
   UINT32 Temp;

   Count = 0;
   Temp = Value;

   while (Temp > 0) {
      Temp &= Temp - 1;
      Count++;
   }
   return Count;
}

