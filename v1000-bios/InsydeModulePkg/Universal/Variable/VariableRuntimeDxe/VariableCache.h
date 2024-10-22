/** @file
  Definitions for Variable cache related code

;******************************************************************************
;* Copyright (c) 2015 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _VAR_VARAIBLE_CACHE_H_
#define _VAR_VARAIBLE_CACHE_H_

#include "Variable.h"
/**
  This function is used to disable variable whole cache mechanism in protected mode or SMM mode.
**/
VOID
DisableVariableCache (
  VOID
  );

/**
  This function uses to disable variable cache address

  @return EFI_SUCCESS    Disable all secure boot SMI functions successful.
  @return Other          Any error occurred while disabling all secure boot SMI functions successful.
**/
EFI_STATUS
SmmDisableVariableCache (
  VOID
  );

/**
  Convert memory address saves non-volatile data to real non-volatile data address.

  @param Global                  VARIABLE_GLOBAL pointer
  @param PtrTrack                [in]: Current pointer to Variable track pointer structure in cache or non-volatile that contains variable information.
                                 [out]: Current pointer to Variable track pointer structure in non-volatile that contains variable information.
  @retval EFI_INVALID_PARAMETER  Input parameter is invalid.
  @retval EFI_SUCCESS            Convert address successful.
**/
EFI_STATUS
ConvertCacheAddressToPhysicalAddress (
  IN     VARIABLE_GLOBAL          *Global,
  IN OUT VARIABLE_POINTER_TRACK   *PtrTrack
  );

/**
  This function uses to flush current non-volatile data to variable cache

  @param  CacheBaseAddress         pointer to variable cache base address.
  @param  NonVolatileBaseAddress  pointer to non-volatile base address.
  @param  CacheSize               Variable cache size.
  @param  LastVariableOffset      Pointer to save last variable offset.

  @return EFI_SUCCESS             Flush non-volatile data to variable cache successful.
  @return EFI_INVALID_PARAMETER   CacheBaseAddress or NonVolatileBaseAddress is NULL.
  @return EFI_BUFFER_TOO_SMALL    CacheSize is too small.

**/
EFI_STATUS
FlushVariableCache (
  IN  UINT8    *CacheBaseAddress,
  IN  UINT8    *NonVolatileBaseAddress,
  IN  UINTN    CacheSize,
  OUT UINTN    *LastVariableOffset
  );

#endif
