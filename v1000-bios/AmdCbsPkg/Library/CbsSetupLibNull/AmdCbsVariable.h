/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef __CBS_VARIABLE_H__
#define __CBS_VARIABLE_H__
#pragma pack(1)

typedef struct _CBS_CONFIG {
  UINT8 CbsCmnFchSataClass;
  UINT8 CbsCmnFchUsbXHCI0Enable;
  UINT8 reserve;                                      ///< reserve
} CBS_CONFIG;

#pragma pack()

#endif //__CBS_VARIABLE_H__
