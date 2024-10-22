/*****************************************************************************
 *
 * Copyright (C) 2014-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * ***************************************************************************
 */

#ifndef _AMD_MEMORY_INFO_HOB_H_
#define _AMD_MEMORY_INFO_HOB_H_

// {45DC9E40-0336-4878-B5BB-AA7675C5C8EF}
#define AMD_MEMORY_INFO_HOB_GUID \
  { 0x45dc9e40, 0x336, 0x4878, 0xb5, 0xbb, 0xaa, 0x76, 0x75, 0xc5, 0xc8, 0xef }

extern EFI_GUID gAmdMemoryInfoHobGuid;

#pragma pack (push, 1)

/// Memory descriptor structure for each memory rang
typedef struct {
  UINT64  Base;                             ///< Base address of memory rang
  UINT64  Size;                             ///< Size of memory rang
  UINT32  Attribute;                        ///< Attribute of memory rang
} AMD_MEMORY_RANGE_DESCRIPTOR;

#define AMD_MEMORY_ATTRIBUTE_AVAILABLE      0x1
#define AMD_MEMORY_ATTRIBUTE_UMA            0x2
#define AMD_MEMORY_ATTRIBUTE_RESERVED       0x4

/// Memory info HOB structure
typedef struct  {
  UINT32  Version;                          ///< Version of HOB structure
  UINT32  NumberOfDescriptor;               ///< Number of memory range descriptor
  AMD_MEMORY_RANGE_DESCRIPTOR  Ranges[1];   ///< Memory ranges array
} AMD_MEMORY_INFO_HOB;

#pragma pack (pop)

#define AMD_MEMORY_INFO_HOB_VERISION        0x00000100ul  // Ver: 00.00.01.00

#endif // _AMD_MEMORY_INFO_HOB_H_

