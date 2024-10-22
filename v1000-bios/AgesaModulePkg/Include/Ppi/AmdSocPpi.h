/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_SOC_PPI_H_
#define _AMD_SOC_PPI_H_

//----------------------------------------------------
//
//  Amd SOC Service Functions
//
//-------------------------------------------------------

/// AMD SOC Identification Check PPI
typedef struct _PEI_AMD_SOC_PPI {
  UINTN    Revision;               ///< Revision Number
} PEI_AMD_SOC_PPI;

// Current PPI revision
#define AMD_SOC_PPI_REVISION   0x00

#endif

