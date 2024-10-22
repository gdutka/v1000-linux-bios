/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_CCX_PPI_H_
#define _AMD_CCX_PPI_H_

//----------------------------------------------------
//
//  AMD CCX PEI Initialization Complete PPI
//
//-------------------------------------------------------

// CCX init complete PPI
typedef struct _PEI_AMD_CCX_INIT_COMPLETE_PPI {
  UINTN    Revision;               ///< Revision Number
} PEI_AMD_CCX_INIT_COMPLETE_PPI;

// Current PPI revision
#define AMD_CCX_PPI_REVISION   0x00

extern EFI_GUID gAmdCcxPeiInitCompletePpiGuid;

#endif  //_AMD_CCX_PPI_H_

