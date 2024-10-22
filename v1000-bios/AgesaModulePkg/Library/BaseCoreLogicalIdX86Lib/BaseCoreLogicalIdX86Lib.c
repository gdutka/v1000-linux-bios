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
#include <Library/AmdBaseLib.h>
#include <Library/BaseSocLogicalIdXlatLib.h>
#include "cpuRegisters.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_BASECORELOGICALIDX86LIB_BASECORELOGICALIDX86LIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the raw family / model / stepping for the executing core
 *
 * @returns       Raw CPU ID for the executing core
 */
UINT32
BaseGetRawIdOnExecutingCore (
  VOID
  )
{
  CPUID_DATA CpuId;

  AsmCpuid (
      AMD_CPUID_FMF,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );

  return CpuId.EAX_Reg;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the logical family / revision for the executing core
 *
 * @param[out]    LogicalId   Executing core's logical faily and logical revision
 *
 * @retval        TRUE        Executing core's logical ID was successfully found
 * @retval        FALSE       The executing core's family is unknown
 */
BOOLEAN
BaseGetLogicalIdOnExecutingCore (
     OUT SOC_LOGICAL_ID  *LogicalId
  )
{
  return (BaseSocConvertRawToLogical (BaseGetRawIdOnExecutingCore (), LogicalId));
}

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the logical Core family / revision for the executing core
 *
 * @param[out]    LogicalCoreId   Executing core's logical family and logical revision
 *
 * @retval        TRUE            Executing core's logical ID was successfully found
 * @retval        FALSE           The executing core's family is unknown
 */
BOOLEAN
BaseGetCoreLogicalIdOnExecutingCore (
     OUT CORE_LOGICAL_ID  *LogicalCoreId
  )
{
  return (BaseCoreConvertRawToLogical (BaseGetRawIdOnExecutingCore (), LogicalCoreId));
}

