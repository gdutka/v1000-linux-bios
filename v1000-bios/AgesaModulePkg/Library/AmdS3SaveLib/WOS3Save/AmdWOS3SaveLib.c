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
#include "AMD.h"
#include "Filecode.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdS3SaveLib.h"
#include "Library/IdsLib.h"
#include <Library/PciLib.h>

#define FILECODE LIBRARY_AMDS3SAVELIB_WOS3SAVE_AMDWOS3SAVELIB_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdS3IoWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdIoWrite (Width, (UINT16) Address, Value, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3IoRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdIoRMW (Width, (UINT16) Address, Data, DataMask, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3IoPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdIoPoll (Width, (UINT16) Address, Data, DataMask, Delay, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3MemWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdMemWrite (Width, Address, Value, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3MemRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdMemRMW (Width, Address, Data, DataMask, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3MemPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdMemPoll (Width, Address, Data, DataMask, Delay, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3PciWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  switch (Width) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    PciWrite8 (PciAddress, *((UINT8 *) Value));
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    PciWrite16 (PciAddress, *((UINT16 *) Value));
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    PciWrite32 (PciAddress, *((UINT32 *) Value));
    break;
  default:
    IDS_ERROR_TRAP;
  }

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3PciRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  TempData;
  UINT32  TempMask;
  LibAmdGetDataFromPtr (Width, Data,  DataMask, &TempData, &TempMask);

  switch (Width) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    PciAndThenOr8 (PciAddress, (UINT8) (~TempMask), (UINT8) TempData);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    PciAndThenOr16 (PciAddress, (UINT16) (~TempMask), (UINT16) TempData);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    PciAndThenOr32 (PciAddress, (~TempMask), TempData);
    break;
  default:
    IDS_ERROR_TRAP;
  }

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptIoWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptIoRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptIoPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptMemWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptMemRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptMemPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptPciWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Value
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptPciRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptPciPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptDispatch (
  IN       VOID          *EntryPoint,
  IN       VOID          *Context
  )
{
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


