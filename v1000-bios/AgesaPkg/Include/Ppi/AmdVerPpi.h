/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_VER_PPI_H_
#define _AMD_VER_PPI_H_

//----------------------------------------------------
//
//  Amd Version String Service Functions
//
//-------------------------------------------------------

/// AMD AGESA Version String PPI
typedef struct _PEI_AMD_VER_PPI {
  CONST CHAR8*    Signature;               ///< AGESA version string signature
  CONST CHAR8*    VersionString;           ///< AGESA PI name & version
  } PEI_AMD_VER_PPI;

// Current PPI revision
#define AMD_VER_PPI_REVISION   0x00

extern EFI_GUID gAmdVersionStringPpiGuid;

#endif

