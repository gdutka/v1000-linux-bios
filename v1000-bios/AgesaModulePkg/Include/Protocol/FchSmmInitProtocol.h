/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH SMM
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _FCH_SMM_INIT_PROTOCOL_H_
#define _FCH_SMM_INIT_PROTOCOL_H_


extern EFI_GUID gFchSmmInitProtocolGuid;

typedef struct _FCH_SMM_INIT_PROTOCOL FCH_SMM_INIT_PROTOCOL;


//
// Protocol prototypes
//


/// FCH INIT Protocol
typedef struct _FCH_SMM_INIT_PROTOCOL {
  UINTN                     Revision;                 ///< Protocol Revision
  UINTN                     FchRev;                   ///< FCH Revision
  VOID                      *FchSmmPolicy;            ///< Fch Config Data Block
} FCH_SMM_INIT_PROTOCOL;

#endif // _FCH_INIT_PROTOCOL_H_

