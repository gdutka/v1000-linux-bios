/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Library
 *
 * Contains interface to the AMD S3 save library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _AMD_S3_SAVE_LIB_H_
#define _AMD_S3_SAVE_LIB_H_
///AmdS3<Func> will set register and save to S3 Script
///AmdS3SaveScript<Func> will save to S3 Script Only
AGESA_STATUS
AmdS3IoWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
AmdS3IoRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
AmdS3IoPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
AmdS3MemWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
AmdS3MemRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
AmdS3MemPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
AmdS3PciWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
AmdS3PciRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
AmdS3SaveScriptIoWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value
  );

AGESA_STATUS
AmdS3SaveScriptIoRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  );

AGESA_STATUS
AmdS3SaveScriptIoPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  );

AGESA_STATUS
AmdS3SaveScriptMemWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value
  );

AGESA_STATUS
AmdS3SaveScriptMemRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  );

AGESA_STATUS
AmdS3SaveScriptMemPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  );

AGESA_STATUS
AmdS3SaveScriptPciWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Value
  );

AGESA_STATUS
AmdS3SaveScriptPciRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  );

AGESA_STATUS
AmdS3SaveScriptPciPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  );

AGESA_STATUS
AmdS3SaveScriptDispatch (
  IN       VOID          *EntryPoint,
  IN       VOID          *Context
  );

#endif // _AMD_S3_SAVE_LIB_H_

