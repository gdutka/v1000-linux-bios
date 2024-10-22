/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Roles Library - x86
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

#define FILECODE LIBRARY_CCXROLESX86LIB_CCXROLESX86LIB_FILECODE

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
  UINT32      ThreadsPerCore;
  CPUID_DATA  CpuId;

  AsmCpuid (
      0x8000001E,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  ThreadsPerCore = (((CpuId.EBX_Reg >> 8) & 0xFF) + 1);

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
  return (BOOLEAN) ((CcxGetInitialApicID (StdHeader) & 0x07) == 0);
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
  return (BOOLEAN) ((CcxGetInitialApicID (StdHeader) & 0x0F) == 0);
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
  return (BOOLEAN) ((CcxGetInitialApicID (StdHeader) & 0x3F) == 0);
}

