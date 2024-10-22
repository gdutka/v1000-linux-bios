/*****************************************************************************
 *
 * Copyright (C) 2014-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_BASE_H_
#define _AMD_CPM_BASE_H_

#include <AmdCpmPreDefine.h>

#ifndef AMD_CPM_EDKII
  #include <Tiano.h>
#else
  #include <Base.h>
  #include <Uefi.h>
  #include <PiPei.h>
  #include CPM_LIBRARY_DEFINITION (BaseLib)
#endif

#include <AmdCpmCommon.h>
#include <Library/DebugLib.h>

#endif
