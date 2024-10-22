/*
 ******************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _BASE_SOC_LOGICAL_ID_XLAT_LIB_H_
#define _BASE_SOC_LOGICAL_ID_XLAT_LIB_H_

#include <SocLogicalId.h>

/*---------------------------------------------------------------------------------------*/
/**
 * Translates the given raw ID into the appropriate logical family / logical revision
 *
 * @param[in]     RawId       Raw CPU ID to convert
 * @param[out]    LogicalId   Logical family and logical revision for the given raw ID
 *
 * @retval        TRUE        Logical ID was successfully found
 * @retval        FALSE       Family is unknown
 */
BOOLEAN
BaseSocConvertRawToLogical (
  IN       UINT32          RawId,
     OUT   SOC_LOGICAL_ID  *LogicalId
  );

/*---------------------------------------------------------------------------------------*/
/**
 * Translates the given raw ID into the appropriate logical Core family / logical Core revision
 *
 * @param[in]     RawId       Raw CPU ID to convert
 * @param[out]    LogicalId   Logical Core family and logical revision for the given raw ID
 *
 * @retval        TRUE        Logical Core ID was successfully found
 * @retval        FALSE       Family is unknown
 */
BOOLEAN
BaseCoreConvertRawToLogical (
  IN       UINT32           RawId,
     OUT   CORE_LOGICAL_ID  *CoreLogicalId
  );

#endif // _BASE_SOC_LOGICAL_ID_XLAT_LIB_H_

