/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP APOB related functions Prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 312133 $   @e \$Date: 2015-02-03 02:47:45 +0800 (Tue, 03 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMDPSP_APOBLIB_H_
#define _AMDPSP_APOBLIB_H_
#define APOB_ENTRY_INSTANCE_MAX 0xFF
#include <Addendum/Apcb/Inc/APOBCMN.h>

///The data struct used by APOBLIB as global variable
typedef struct _APOBLIB_INFO {
  BOOLEAN  Supported;                ///<  Specify if APOB supported
  UINT32    ApobSize;                 ///<  ApobSize
  UINT64    ApobAddr;                 ///<  The Address of APOB
} APOBLIB_INFO;

EFI_STATUS
AmdPspGetApobInfo (
  IN OUT   APOBLIB_INFO  **ApobInfo
  );

EFI_STATUS
AmdPspGetApobEntry (
  IN       UINT32  GroupID,
  IN       UINT32  DataTypeID,
  IN       BOOLEAN  ReadFromSpiCopy,
     OUT   UINT32  *NumofEntry,
     OUT   APOB_TYPE_HEADER **ApobEntries
  );

EFI_STATUS
AmdPspGetApobEntryInstance (
  IN       UINT32  GroupID,
  IN       UINT32  DataTypeID,
  IN       UINT32  InstanceID,
  IN       BOOLEAN  ReadFromSpiCopy,
     OUT   APOB_TYPE_HEADER **ApobEntry
  );

#endif //_AMDPSP_APOBLIB_H_

