/* $NoKeywords:$ */
/**
 * @file
 *
 * The file used to declare structure of resume memory info
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspSmm Driver
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _PSP_RESUME_MEM_INFO_H_
#define _PSP_RESUME_MEM_INFO_H_

#pragma pack(push, 1)
/// Used to keep SMM Resume Memory informtion
typedef struct _PSP_SMM_RSM_MEM_INFO {
  UINT32                  BspStackSize;           // BSP Stack Size for resume
  UINT32                  ApStackSize;            // AP Stack Size for resume
  EFI_PHYSICAL_ADDRESS    StackPtr;              // Point to the base of Stack
  EFI_PHYSICAL_ADDRESS    RsmHeapPtr;            // Point to the base of Resume Heap
  UINT32                  HeapSize;               // Reserved Heap Size, Change the Order to avoid Warning 4366
  UINT32                  TempRegionSize;         // Reserved Temp Region Size
  EFI_PHYSICAL_ADDRESS    TempRegionPtr;         // Point to the base of Temporary Region (used for store the dynamic value during SMM SEC Phase)
} PSP_SMM_RSM_MEM_INFO;
#pragma pack(pop)

#endif  //_PSP_RESUME_MEM_INFO_H_

