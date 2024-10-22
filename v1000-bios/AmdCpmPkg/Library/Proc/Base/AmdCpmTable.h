/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_TABLE_H_
#define _AMD_CPM_TABLE_H_

VOID *
EFIAPI
CpmAddTable (
  IN       VOID                    *This,
  IN       VOID                    *TablePtr
  );

VOID*
EFIAPI
CpmRemoveTable (
  IN       VOID                    *This,
  IN       VOID                    *TablePtr
  );

VOID *
EFIAPI
CpmGetTablePtr (
  IN       VOID                    *This,
  IN       UINT32                  TableId
  );

VOID *
EFIAPI
CpmGetTablePtr2 (
  IN       VOID                    *This,
  IN       UINT32                  TableId
  );

UINT16
EFIAPI
CpmGetTableSize (
  IN       VOID                    *TablePtr
  );

VOID
EFIAPI
CpmCopyMem (
  IN OUT   VOID                    *destination,
  IN       VOID                    *source,
  IN       UINTN                   size
  );

VOID
EFIAPI
CpmGenerateTableList (
  IN       VOID                    *TableListPtr,
  IN       UINT16                  PlatformId,
  IN OUT   AMD_CPM_TABLE_LIST      *TableInHobListPtr
  );

VOID *
EFIAPI
CpmCopyTableListToMemory (
  IN       AMD_CPM_TABLE_LIST      *TableInHobListPtr,
  IN OUT   VOID                    *HobTablePtr
  );

VOID
EFIAPI
CpmRelocateTableList (
  IN OUT   VOID                    *TablePtr
  );

VOID *
EFIAPI
CpmSearchAndUpdateTable (
  IN       VOID                    *This,
  IN       UINT32                  TableId,
  IN       UINT8                   Flag
  );

VOID
EFIAPI
CpmPostCode (
  IN       UINT32                  Postcode
  );

AMD_CPM_TABLE_LIST *
EFIAPI
CpmAdjustTableList (
  IN       VOID                *HobTablePtr
  );

#endif
