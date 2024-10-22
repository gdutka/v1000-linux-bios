/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory API, and related functions.
 *
 * Contains code that initializes memory
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/*++
Module Name:

  AmdMemAm4Pei.c
  Init Memory interface

Abstract:
--*/

#include "PiPei.h"
//#include <Library/IdsLib.h>
#include <Ppi/AmdMemPpi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Addendum/Apcb/Inc/ZP/APOB.h>
#include <Library/AmdPspApobLib.h>
#include <Library/HobLib.h>
#include "Filecode.h"

#define FILECODE MEM_AMDMEMAM4PEI_AMDMEMAM4PEI_FILECODE
extern EFI_GUID gAmdNvdimmInfoHobGuid;

EFI_STATUS
GetSystemMemoryMapInternal (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN UINT32                     *NumberOfHoles,
  IN UINT64                     *TopOfSystemMemory,
  IN  VOID                      **MemHoleDescPtr
  )
{
  EFI_STATUS                          Status;
  UINT32                              NumOfMemHoleMap;
  APOB_TYPE_HEADER                    *ApobEntry;
  APOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT  *SystemMemoryMapPtr;
  APOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT  SystemMemoryMap;

  Status = AmdPspGetApobEntryInstance (APOB_FABRIC, APOB_SYS_MAP_INFO_TYPE, 0, FALSE, &ApobEntry);
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  CopyMem (&SystemMemoryMap, ApobEntry, sizeof (APOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT));
  //
  // Get the number of holes so that you can allocate sufficient buffer for all entries
  //
  NumOfMemHoleMap = SystemMemoryMap.ApobSystemMap.NumberOfHoles;
  //
  // Allocate some buffer for Memory Hole Map Descriptor
  //
  if (NumOfMemHoleMap != 0) {
    Status = (*PeiServices)->AllocatePool (PeiServices,
                      (((NumOfMemHoleMap -1 ) * sizeof (MEMORY_HOLE_DESCRIPTOR)) + sizeof (APOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT)),
                                         (VOID**)&SystemMemoryMapPtr);
    if (Status != EFI_SUCCESS) {
      return Status;
    }
    CopyMem (SystemMemoryMapPtr, ApobEntry, (((NumOfMemHoleMap -1 ) * sizeof (MEMORY_HOLE_DESCRIPTOR)) + sizeof (APOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT)));
    *MemHoleDescPtr =  (VOID *)&SystemMemoryMapPtr->ApobSystemMap.HoleInfo[0];
    *TopOfSystemMemory = SystemMemoryMapPtr->ApobSystemMap.TopOfSystemMemory;
    *NumberOfHoles = NumOfMemHoleMap;
  }

  return EFI_SUCCESS;
}
/*++

Routine Description:

  PSP Driver Entry. Initialize PSP device and report DRAM info to PSP once found

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdMemAm4PeimZp (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                      Status;
  AMD_MEMORY_INIT_COMPLETE_PPI    *MemInitCompleteData;
  EFI_PEI_PPI_DESCRIPTOR          *MemInitPpiList;
  APOB_TYPE_HEADER                *ApobEntry;
  APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT   ApobMemGenConfig;
  APOB_MEM_NVDIMM_INFO_STRUCT         NvdimmMemoryInfo;

  ApobEntry = NULL;
  //----------------------------------------------------------------
  //
  // Allocate space for AmdMemoryInitCompletePpi
  //
  //----------------------------------------------------------------
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (AMD_MEMORY_INIT_COMPLETE_PPI),
                                         (VOID**)&MemInitCompleteData);

  if (Status != EFI_SUCCESS) {
    return Status;
  }
  ZeroMem (MemInitCompleteData, sizeof (AMD_MEMORY_INIT_COMPLETE_PPI));
  MemInitCompleteData->Revision = AMD_MEMORY_INIT_COMPLETE_REVISION;

  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE, 0, FALSE, &ApobEntry);

  if ((Status != EFI_SUCCESS) || (ApobEntry == NULL)) {
    MemInitCompleteData->AmdMemoryFrequency = 0;
    MemInitCompleteData->DdrMaxRate = 0;
  } else {
    CopyMem (&ApobMemGenConfig, ApobEntry, sizeof (APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT));
    MemInitCompleteData->AmdMemoryFrequency = ApobMemGenConfig.MemClkFreq;
    MemInitCompleteData->DdrMaxRate = ApobMemGenConfig.DdrMaxRate;

  }

  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), (VOID**)&MemInitPpiList);
  MemInitPpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  MemInitPpiList->Guid  = &gAmdMemoryInitCompletePpiGuid;
  MemInitPpiList->Ppi   = MemInitCompleteData;
  ((AMD_MEMORY_INIT_COMPLETE_PPI *)MemInitPpiList->Ppi)->GetSystemMemoryMap = GetSystemMemoryMapInternal;

  Status = (*PeiServices)->InstallPpi (PeiServices, MemInitPpiList);

  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_NVDIMM_INFO_TYPE, 0, FALSE, &ApobEntry);
  if (Status == EFI_SUCCESS) {
    CopyMem(&NvdimmMemoryInfo, ApobEntry, sizeof(APOB_MEM_NVDIMM_INFO_STRUCT));
    if (BuildGuidDataHob (&gAmdNvdimmInfoHobGuid, &NvdimmMemoryInfo.NvdimmInfo[0][0], sizeof (NvdimmMemoryInfo.NvdimmInfo)) == NULL) {
      Status = EFI_NOT_FOUND;
    }
  }

  return (Status);
}


