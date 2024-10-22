/*****************************************************************************
 *
 * Copyright (C) 2014-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_PEI_H_
#define _AMD_CPM_PEI_H_

#include <AmdCpmBase.h>

#ifndef AMD_CPM_EDKII
  #include <Pei.h>
  #include <PeiLib.h>
  #include EFI_PPI_DEFINITION (Smbus)
  #include EFI_PPI_DEFINITION (MemoryDiscovered)
#else
  #include <PiPei.h>
  #include CPM_LIBRARY_DEFINITION (BaseMemoryLib)
  #include CPM_LIBRARY_DEFINITION (BaseLib)
  #include CPM_LIBRARY_DEFINITION (HobLib)
  #include CPM_LIBRARY_DEFINITION (PcdLib)
  #include EFI_PPI_DEFINITION (Smbus2)
  #include EFI_PPI_DEFINITION (MemoryDiscovered)
#endif

#include CPM_PPI_DEFINITION (AmdCpmOemTablePpi)
#include CPM_PPI_DEFINITION (AmdCpmOverrideTablePpi)
#include CPM_PPI_DEFINITION (AmdCpmTablePpi)
#include CPM_PPI_DEFINITION (AmdCpmTableHobPpi)
#include CPM_PPI_DEFINITION (AmdCpmGpioInitFinishedPpi)
#include CPM_PPI_DEFINITION (AmdCpmBootTimeRecordPpi)
#include CPM_PPI_DEFINITION (PcieTopologyTablePpi)
#include CPM_GUID_DEFINITION (AmdCpmTableHob)
#include CPM_GUID_DEFINITION (AmdCpmBootTimeRecordHob)

#endif
