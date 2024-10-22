/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory Info Hob PPI prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AgesaPkg
 * @e sub-project:  Memory
 */
/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_MEMORY_INFO_HOB_PPI_H_
#define _AMD_MEMORY_INFO_HOB_PPI_H_

//----------------------------------------------------
//
//  Amd Memory Service Functions
//
//-------------------------------------------------------


//----------------------------------------------------
//
//  Amd Memory Init Complete Ppi
//
//-------------------------------------------------------
extern EFI_GUID gAmdMemoryInfoHobPpiGuid;

typedef struct _AMD_MEMORY_INFO_HOB_PPI {
  UINT32    Revision;                                              ///< revision
} AMD_MEMORY_INFO_HOB_PPI;

#define AMD_MEMORY_INFO_HOB_PPI_REVISION   0x01
#define AMD_MEMORY_INFO_HOB_PPI_REV_0400   0x0400

#endif

