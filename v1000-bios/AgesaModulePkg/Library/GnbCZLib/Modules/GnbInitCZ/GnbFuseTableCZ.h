/* $NoKeywords:$ */
/**
 * @file
 *
 * Fuse table initialization
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 328981 $   @e \$Date: 2015-10-14 19:44:43 +0800 (Wed, 14 Oct 2015) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
#ifndef _GNBFUSETABLECZ_H_
#define _GNBFUSETABLECZ_H_

#pragma pack (push, 1)

/// Fuse field entry
typedef struct {
  UINT8                    FieldOffset;              ///< Field offset in fuse register
  UINT8                    FieldWidth;               ///< Width of field
  UINT16                   FuseOffset;               ///< destination offset in translation table
} FUSE_REGISTER_ENTRY_CZ;

/// Fuse register entry
typedef struct {
  UINT8                    RegisterSpaceType;        ///< Register type
  UINT32                   Register;                 ///< FCR register address
  UINT8                    FuseRegisterTableLength;  ///< Length of field table for this register
  FUSE_REGISTER_ENTRY_CZ   *FuseRegisterTable;       ///< Pointer to field table
} FUSE_TABLE_ENTRY_CZ;

/// Fuse translation table
typedef struct {
  UINT8                    FuseTableLength;          ///< Length of translation table
  FUSE_TABLE_ENTRY_CZ      *FuseTable;               ///< Pointer to register table
} FUSE_TABLE_CZ;

#define OPN_CHAR_LENGTH  13
/// OPN strcuture
typedef struct {
  CHAR8 OpnChar [OPN_CHAR_LENGTH];                   ///< Opn string
} OPN_CHAR_STRUCT;

#pragma pack (pop)

BOOLEAN
CheckOpnCZ (
  IN      AMD_CONFIG_PARAMS       *StdHeader,
  IN      OPN_CHAR_STRUCT         *OpnPtr
  );

AGESA_STATUS
GnbLoadFuseTableCZ (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

AGESA_STATUS
GnbOverrideGpuFreqeuencyCZ (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

#endif

