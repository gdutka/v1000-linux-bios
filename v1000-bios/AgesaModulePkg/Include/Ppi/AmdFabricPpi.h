/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_FABRIC_PPI_H_
#define _AMD_FABRIC_PPI_H_

//----------------------------------------------------
//
//  AMD Fabric PEI Initialization Complete PPI
//
//-------------------------------------------------------

/// Fabric Init Complete PPI
typedef struct _PEI_AMD_FABRIC_INIT_COMPLETE_PPI {
  UINTN    Revision;               ///< Revision Number
} PEI_AMD_FABRIC_INIT_COMPLETE_PPI;

// Current PPI revision
#define AMD_FABRIC_PPI_REVISION   0x00

extern EFI_GUID gAmdFabricPeiInitCompletePpiGuid;

#endif  //_AMD_FABRIC_PPI_H_
