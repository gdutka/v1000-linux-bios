/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/


#ifndef _FCHPLATFORMLIB_H_
#define _FCHPLATFORMLIB_H_

EFI_STATUS
EFIAPI
FchPlatformImcDxeInit (
  IN      VOID                          *FchImcData
  );

EFI_STATUS
EFIAPI
FchPlatformPTPeiInit (
  IN      VOID                          *FchPTData
  );

EFI_STATUS
EFIAPI
FchPlatformPTDxeInit (
  IN      VOID                          *FchPTData
  );

EFI_STATUS
EFIAPI
FchPlatformOemPeiInit (
  IN      VOID                          *FchData
  );

EFI_STATUS
EFIAPI
FchPlatformOemDxeInit (
  IN      VOID                          *FchData
  );

#endif // _FCHPLATFORMLIB_H_

