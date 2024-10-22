/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Roles Library - VH
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include <Library/BaseLib.h>
#include "AMD.h"
#include "Filecode.h"
#include <Library/CcxBaseX86Lib.h>

#define FILECODE LIBRARY_CCXROLESVHLIB_CCXROLESVHLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

 /*----------------------------------------------------------------------------------------
  *                          E X P O R T E D    F U N C T I O N S
  *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  Is this the BSP core?
 *
 *  @param[in,out]   StdHeader        Header for library and services
 *
 *  @retval          TRUE             Is BSP core
 *  @retval          FALSE            Is not BSP Core
 *
 */
BOOLEAN
CcxIsBsp (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64   ApicBar;

  ApicBar = AsmReadMsr64 (0x0000001B);
  return (BOOLEAN) ((ApicBar & BIT8) != 0);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Is the current core a primary core of it's compute unit?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsComputeUnitPrimary (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  ThreadsPerCore;

  ThreadsPerCore = (UINT32) CcxGetThreadsPerCore ();
  if (ThreadsPerCore == 0) {
    ThreadsPerCore = 1;
  }
  return (BOOLEAN) ((CcxGetInitialApicID (StdHeader) % ThreadsPerCore) == 0);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Is the current core a primary core of its complex?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsComplexPrimary (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  ComplexMask;

  ComplexMask = (CcxGetThreadsPerCore () == 1) ? 0x00000003 : 0x00000007;
  return (BOOLEAN) ((CcxGetInitialApicID (StdHeader) & ComplexMask) == 0);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Is the current core a primary core of it's node?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsDiePrimary (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  DieMask;

  DieMask = (CcxGetThreadsPerCore () == 1) ? 0x00000007 : 0x0000000F;
  return (BOOLEAN) ((CcxGetInitialApicID (StdHeader) & DieMask) == 0);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Is the current core a primary core of it's node?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsSocketPrimary (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  SocketMask;

  SocketMask = (CcxGetThreadsPerCore () == 1) ? 0x0000003F : 0x0000007F;
  return (BOOLEAN) ((CcxGetInitialApicID (StdHeader) & SocketMask) == 0);
}

