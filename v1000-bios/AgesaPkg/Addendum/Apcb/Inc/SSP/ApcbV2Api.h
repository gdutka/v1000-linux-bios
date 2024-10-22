/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _APCB_V2_API_H
#define _APCB_V2_API_H

BOOLEAN ApcbGetConfigParameter (
  IN     UINT32             ApcbParamToken,
  IN     UINT8              ApcbTimePoint,
  IN OUT UINT8              *Buffer,
  IN OUT UINT8              *Size,
  IN     AMD_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN ApcbGetCbsCmnParameter (
  IN     UINT32             ApcbParamToken,
  IN     UINT8              ApcbTimePoint,
  IN OUT UINT8              *Buffer,
  IN OUT UINT8              *Size,
  IN     AMD_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN ApcbGetCbsDbgParameter (
  IN     UINT32             ApcbParamToken,
  IN     UINT8              ApcbTimePoint,
  IN OUT UINT8              *Buffer,
  IN OUT UINT8              *Size,
  IN     AMD_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN ApcbGetParameter (
  IN     APCB_PARAM_TYPE    ApcbParamType,
  IN     UINT32             ApcbParamToken,
  IN     UINT8              ApcbTimePoint,
  IN OUT UINT8              *Buffer,
  IN OUT UINT8              *Size,
  IN     AMD_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN ApcbAblIdsOverrideCbsCmnParameter (
  IN     UINT32             ApcbParamToken,
  IN     UINT8              ApcbTimePoint,
  IN OUT UINT8              *Buffer,
  IN OUT UINT8              *Size,
  IN     AMD_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN ApcbAblIdsOverrideCbsDbgParameter (
  IN     UINT32             ApcbParamToken,
  IN     UINT8              ApcbTimePoint,
  IN OUT UINT8              *Buffer,
  IN OUT UINT8              *Size,
  IN     AMD_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN ApcbAblIdsOverrideCbsCmnChildParameter (
  IN     UINT32             *ApcbParamTokenList,
  IN     UINT8              ApcbTimePoint,
  IN OUT UINT8              *Buffer,
  IN OUT UINT8              *Size,
  IN     BOOLEAN            Numeric,
  IN     AMD_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN ApcbAblIdsOverrideCbsDbgChildParameter (
  IN     UINT32             *ApcbParamTokenList,
  IN     UINT8              ApcbTimePoint,
  IN OUT UINT8              *Buffer,
  IN OUT UINT8              *Size,
  IN     BOOLEAN            Numeric,
  IN     AMD_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN ApcbLocateType (
  IN     UINT16   GroupId,
  IN     UINT16   TypeId,
  IN     UINT16   InstanceId,
  IN     UINT32   *TableSize,
  IN OUT VOID     **TypeData
  );

#endif // _APCB_V2_API_H

