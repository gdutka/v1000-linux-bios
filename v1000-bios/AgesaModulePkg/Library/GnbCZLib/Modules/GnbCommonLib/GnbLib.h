/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB register access services.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
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
#ifndef _GNBLIB_H_
#define _GNBLIB_H_

#define IOC_WRITE_ENABLE 0x80

VOID
GnbLibPciIndirectWrite (
  IN      UINT32        Address,
  IN      UINT32        IndirectAddress,
  IN      ACCESS_WIDTH  Width,
  IN      VOID          *Value,
  IN      VOID          *Config
  );

VOID
GnbLibPciIndirectRead (
  IN       UINT32        Address,
  IN       UINT32        IndirectAddress,
  IN       ACCESS_WIDTH  Width,
     OUT   VOID          *Value,
  IN       VOID          *Config
  );

VOID
GnbLibPciIndirectReadField (
  IN       UINT32       Address,
  IN       UINT32       IndirectAddress,
  IN       UINT8        FieldOffset,
  IN       UINT8        FieldWidth,
     OUT   UINT32       *Value,
  IN       VOID         *Config
  );

VOID
GnbLibPciIndirectRMW (
  IN      UINT32        Address,
  IN      UINT32        IndirectAddress,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Value,
  IN      VOID          *Config
  );

VOID
GnbLibPciIndirectWriteField (
  IN      UINT32        Address,
  IN      UINT32        IndirectAddress,
  IN      UINT8         FieldOffset,
  IN      UINT8         FieldWidth,
  IN      UINT32        Value,
  IN      BOOLEAN       S3Save,
  IN      VOID          *Config
  );


VOID
GnbLibPciRMW (
  IN      UINT32        Address,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Value,
  IN      VOID          *Config
  );

VOID
GnbLibIoRMW (
  IN      UINT16        Address,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Value,
  IN      VOID          *Config
  );


UINT32
GnbLibPowerOf (
  IN      UINT32            Value,
  IN      UINT32            Power
  );

VOID*
GnbLibFind (
  IN      UINT8             *Buf1,
  IN      UINTN             Buf1Length,
  IN      UINT8             *Buf2,
  IN      UINTN             Buf2Length
  );

VOID
GnbLibIndirectIoBlockRead (
  IN      UINT16            IndexPort,
  IN      UINT16            DataPort,
  IN      ACCESS_WIDTH      Width,
  IN      UINT32            IndexAddress,
  IN      UINT32            Count,
  IN      VOID              *Buffer,
  IN      VOID              *Config
  );

UINT8
GnbLiGetIoapicId (
  IN      UINT64            IoApicBaseAddress,
  IN      VOID              *Config
  );

VOID
GnbLibDebugDumpBuffer (
  IN       VOID             *Buffer,
  IN       UINT32           Count,
  IN       UINT8            DataWidth,
  IN       UINT8            LineWidth
  );

AGESA_STATUS
GnbLibLocateService (
  IN       GNB_SERVICE_ID       ServiceId,
  IN       UINT8                SocketId,
  IN       VOID                 **ServiceProtocol,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

#endif

