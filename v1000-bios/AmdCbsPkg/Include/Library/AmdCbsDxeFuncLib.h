/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef __CBS_FUNC_LIB_H__
#define __CBS_FUNC_LIB_H__

#define CBS_DEADLOOP()    { volatile int __iii; __iii = 1; while (__iii); }

EFI_STATUS CbsInitEntry(
  IN  EFI_BOOT_SERVICES *BootServices
  );

EFI_STATUS
CbsInterfaceFunc (
  IN  EFI_BOOT_SERVICES      *BootServices,
  IN  OUT VOID               *AMD_PARAMS,
  IN      UINTN              TimePoint
  );

BOOLEAN
CbsFamilyIdentify (
  );

#endif //__CBS_FUNC_LIB_H__

