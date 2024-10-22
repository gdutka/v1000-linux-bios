/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_SOC_PROTOCOL_H_
#define _AMD_SOC_PROTOCOL_H_

//
// PPI prototype
//
typedef struct _AMD_SOC_PROTOCOL {
  UINTN    Revision;               ///< Revision Number
} AMD_SOC_PROTOCOL;

// Current PPI revision
#define AMD_SOC_PROTOCOL_REVISION   0x00

#endif //_AMD_SOC_PROTOCOL_H_

