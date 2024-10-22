/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_CPU_H_
#define _AMD_CPM_CPU_H_

UINT32
CpmGetSbTsiAddr (
  IN       VOID                        *This
  );

VOID
EFIAPI
CpmRegisterCpu (
  IN       VOID                        *This
  );

#endif // _AMD_CPM_FCH_H_
