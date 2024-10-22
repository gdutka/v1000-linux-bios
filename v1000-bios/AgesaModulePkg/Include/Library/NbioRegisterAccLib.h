/* $NoKeywords:$ */
/**
 * @file
 *
 * Various register access service procedures
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
#ifndef _NBIOREGISTERACCLIB_H_
#define _NBIOREGISTERACCLIB_H_

#ifndef D0F0xB8_ADDRESS
  // **** D0F0xB8 Register Definition ****
  // Address
  #define D0F0xB8_ADDRESS                                         0xB8
#endif

#include <NbioRegisterTypes.h>


AGESA_STATUS
NbioRegisterWrite (
  IN       GNB_HANDLE          *NbioHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  );

AGESA_STATUS
NbioRegisterRead (
  IN       GNB_HANDLE          *NbioHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
     OUT   VOID                *Value,
  IN       UINT32              Flags
  );

VOID
NbioRegisterRMW (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       UINT32              Mask,
  IN       UINT32              Value,
  IN       UINT32              Flags
  );

#endif

