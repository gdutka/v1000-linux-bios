/*
 ******************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _BASE_CORE_LOGICAL_ID_LIB_H_
#define _BASE_CORE_LOGICAL_ID_LIB_H_

#include <SocLogicalId.h>

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the raw family / model / stepping for the executing core
 *
 * @returns       Raw CPU ID for the executing core
 */
UINT32
BaseGetRawIdOnExecutingCore (
  VOID
  );

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the logical family / revision for the executing core
 *
 * @param[out]    LogicalId   Executing core's logical family and logical revision
 *
 * @retval        TRUE        Executing core's logical ID was successfully found
 * @retval        FALSE       The executing core's family is unknown
 */
BOOLEAN
BaseGetLogicalIdOnExecutingCore (
     OUT SOC_LOGICAL_ID  *LogicalId
  );

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the logical Core family / revision for the executing core
 *
 * @param[out]    CoreLogicalId   Executing core's logical family and logical revision
 *
 * @retval        TRUE            Executing core's logical ID was successfully found
 * @retval        FALSE           The executing core's family is unknown
 */
BOOLEAN
BaseGetCoreLogicalIdOnExecutingCore (
     OUT CORE_LOGICAL_ID  *CoreLogicalId
  );

#endif // _BASE_CORE_LOGICAL_ID_LIB_H_

