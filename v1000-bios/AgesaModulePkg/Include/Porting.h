/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _PORTING_H_
#define _PORTING_H_

#ifdef USE_EDKII_HEADER_FILE
  #include <ProcessorBind.h>

  #define   VOLATILE volatile
#else
  #include <Agesa-Porting.h>
#endif

#include <Compiler-Specs.h>

#endif
