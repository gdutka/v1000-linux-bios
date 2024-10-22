/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _APCB_MEM_OVERCLOCK_CONFIG_H_
#define _APCB_MEM_OVERCLOCK_CONFIG_H_
//
// @APCB_START
//


///===============================================================================
/// Mempory Overclock configuration
#define MEM_OC_SAFE_BOOT_CONTROL_METHOD_DIS  0      ///< Safe Boot is disabled
#define MEM_OC_SAFE_BOOT_CONTROL_METHOD_G3   1      ///< Safe Boot is applied only when mechanical off
#define MEM_OC_SAFE_BOOT_CONTROL_METHOD_G2   2      ///< Safe Boot is applied only when soft-off or suspend to disk

typedef struct _MEM_OC_SAFE_BOOT_CONTROL_INDEX_IO_STRUCT {
  UINT32                          Size;
  UINT8                           Method;
  UINT8                           IndexPort;    ///< 72
  UINT8                           DataPort;     ///< 73
  UINT8                           Address;      ///< 0x04
  UINT8                           UnlockValue;  ///< 0x55
  UINT8                           LockValue;    ///< 0xAA
} MEM_OC_SAFE_BOOT_CONTROL_INDEX_IO_STRUCT;

typedef struct _MEM_OC_CONFIG_STRUCT {
  UINT32                                    Size;
  MEM_OC_SAFE_BOOT_CONTROL_INDEX_IO_STRUCT  SafeBootCtrlIndexIo;
} MEM_OC_CONFIG_STRUCT;



#endif // _APCB_MEM_OVERCLOCK_CONFIG_H_


