/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP Flash Access Library Routines declaration
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspStorage Library
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _PSP_FLASH_UPDATE_LIB_H_
#define _PSP_FLASH_UPDATE_LIB_H_

#include "Uefi.h"

EFI_STATUS
PspUpdateFlash (
  IN       UINT32 Address,
  IN       UINT32 Size,
  IN       VOID  *Buffer
  );

#endif  //_PSP_FLASH_UPDATE_LIB_H_

