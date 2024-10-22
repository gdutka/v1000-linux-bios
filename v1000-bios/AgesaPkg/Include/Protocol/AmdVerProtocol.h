/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_VER_PROTOCOL_H_
#define _AMD_VER_PROTOCOL_H_

///
///  AMD Version String Protocol
///
typedef struct _AMD_VER_PROTOCOL {
  CONST CHAR8*    Signature;               ///< AGESA version string signature
  CONST CHAR8*    VersionString;           ///< AGESA PI name & version
} AMD_VER_PROTOCOL;

#endif //_AMD_VER_PROTOCOL_H_

