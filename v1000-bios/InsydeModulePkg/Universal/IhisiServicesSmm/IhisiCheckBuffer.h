/** @file
  This driver provides IHISI interface in SMM mode

;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _IHISI_CHECK_BUFFER_H_
#define _IHISI_CHECK_BUFFER_H_

#include <Library/FdSupportLib.h>

#pragma pack(1)
typedef struct _H2O_IHISI_PARAMS {
  UINT32 Size;        // Size of struct
  UINT32 Reserved;    // Must be 0

  UINT64 Param1;      // IhisiRax
  UINT64 Param2;      // IhisiRbx
  UINT64 Param3;      // IhisiRcx
  UINT64 Param4;      // IhisiRdx
  UINT64 Param5;      // IhisiRsi
  UINT64 Param6;      // IhisiRdi
  UINT64 Param7;      // IhisiFs
  UINT64 Param8;      // IhisiGs
} H2O_IHISI_PARAMS;
#pragma pack()

/**
  According to input IHISI command to check shared buffer is whether valid.

  @param[in] Cmd                Input IHISI SMI function command.

  @retval EFI_SUCCESS            All of shared data buffer is valid.
  @retval EFI_INVALID_PARAMETER  Any of shared buffer is invalid.
**/
EFI_STATUS
CheckSharedBuffer (
  IHISI_SMI      Cmd
  );

/**
  According to input IHISI command to store shared buffer to SMRAM.

  @param[in] Cmd                 Input IHISI SMI function command.

  @retval EFI_SUCCESS            Copy shared buffer to SMRAM successfully.
  @retval EFI_OUT_OF_RESOURCES   Unable allocate SMRAM to store share buffer.
**/
EFI_STATUS
StoreSharedBufferToSmram (
  IHISI_SMI      Cmd
  );

/**
  Based on input IHISI command to check shared buffer is whether valid.

   @param[in] Cmd                Input IHISI SMI function command.

  @retval EFI_SUCCESS            All of shared data buffer is valid.
  @retval EFI_INVALID_PARAMETER  Any of shared buffer is invalid.
**/
EFI_STATUS
RestoreSharedBufferFromSmram (
  VOID
  );


/**
  Get default BIOS ROM map

  @param[out] BiosRomMap              Pointer to the returned (FBTS_INTERNAL_BIOS_ROM_MAP *) data
  @param[out] NumberOfRegions         The total number of regions in BiosRomMap

  @retval EFI_SUCCESS                 FBTS get BIOS ROM map success.
  @return Others                      FBTS get BIOS ROM map failed.
**/
EFI_STATUS
GetDefaultBiosRomMap (
  OUT FBTS_INTERNAL_BIOS_ROM_MAP    **BiosRomMap,
  OUT UINTN                          *NumberOfRegions
  );


#define TOTAL_PARAMETERS  8

#endif
