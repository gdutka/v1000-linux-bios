/* $NoKeywords:$ */
/**
 * @file
 *
 * Smn register access service procedures
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
#ifndef _SMNACCESSLIB_H_
#define _SMNACCESSLIB_H_

#ifndef D0F0xB8_ADDRESS
  // **** D0F0xB8 Register Definition ****
  // Address
  #define D0F0xB8_ADDRESS      0xB8
#endif


VOID
SmnRegisterRead (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
     OUT   VOID                *Value
  );

VOID
SmnRegisterWrite (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  );

VOID
SmnRegisterRMW (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue,
  IN       UINT32              Flags
  );

#endif

