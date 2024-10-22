/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH DXE
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _FCH_MULTI_FCH_INIT_PROTOCOL_H_
#define _FCH_MULTI_FCH_INIT_PROTOCOL_H_


extern EFI_GUID gFchMultiFchInitProtocolGuid;

typedef struct _FCH_MULTI_FCH_INIT_PROTOCOL FCH_MULTI_FCH_INIT_PROTOCOL;


typedef struct _FCH_MULITI_FCH_DATA_BLOCK {
  UINT64                FchAcpiMmioBase[8];            ///< FCH ACPI MMIO Base
} FCH_MULITI_FCH_DATA_BLOCK;


/// MULTI FCH INIT Protocol
typedef struct _FCH_MULTI_FCH_INIT_PROTOCOL {
  UINTN                     Revision;                 ///< Protocol Revision
  FCH_MULITI_FCH_DATA_BLOCK FchMfData;               ///< Data Block
} FCH_MULTI_FCH_INIT_PROTOCOL;

// current Protocol revision
#define FCH_MULTI_FCH_INIT_REV  0x01

#endif // _FCH_MULTI_FCH_INIT_PROTOCOL_H_

