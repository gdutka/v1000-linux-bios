/* $NoKeywords:$ */
/**
 * @file
 *
 * Various Timer services.
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
#include  "Gnb.h"
#include  "GnbLib.h"
#include  "GnbTimerLib.h"
#include  "GnbFamServices.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBCOMMONLIB_GNBTIMERLIB_FILECODE




/*----------------------------------------------------------------------------------------*/
/*
 * Stall and save to script table
 *
 *
 *
 * @param[in] Microsecond     Stall time
 * @param[in] StdHeader       Standard configuration header
 */

VOID
GnbLibStallS3Save (
  IN      UINT32              Microsecond,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
//  S3_SAVE_DISPATCH (StdHeader, GnbLibStallS3Script_ID, sizeof (Microsecond), &Microsecond);
  GnbLibStall (Microsecond, StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/*
 * Stall
 *
 *
 *
 * @param[in] Microsecond     Stall time
 * @param[in] StdHeader       Standard configuration header
 */

VOID
GnbLibStall (
  IN      UINT32              Microsecond,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 TimeStampStart;
  UINT32 TimeStampDelta;
  UINT32 TimeStampCurrent;

  TimeStampStart = GnbFmTimeStamp (StdHeader);
  do {
    TimeStampCurrent = GnbFmTimeStamp (StdHeader);
    TimeStampDelta = ((TimeStampCurrent > TimeStampStart) ? (TimeStampCurrent - TimeStampStart) : (0xffffffffull - TimeStampStart + TimeStampCurrent));
  } while (TimeStampDelta < Microsecond);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Stall S3 script
 *
 *
 *
 * @param[in]  StdHeader      Standard configuration header
 * @param[in]  ContextLength  Context Length (not used)
 * @param[in]  Context        pointer to UINT32 number of us
 */
VOID
GnbLibStallS3Script (
  IN      AMD_CONFIG_PARAMS    *StdHeader,
  IN      UINT16               ContextLength,
  IN      VOID*                Context
  )
{
  GnbLibStall (* ((UINT32*) Context), StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Family specific time stamp function
 *
 *
 * @param[in]  StdHeader       Standard configuration header
 * @retval                     Count
 */
UINT32
GnbFmTimeStamp (
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  AGESA_STATUS               Status;
  GNB_FAM_TS_SERVICES       *GnbFamTsFunc;
  GNB_HANDLE                *GnbHandle;

  GnbHandle = GnbGetHandle (StdHeader);

  Status = GnbLibLocateService (GnbFamTsService, GnbGetSocketId (GnbHandle), &GnbFamTsFunc, StdHeader);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status == AGESA_SUCCESS) {
    return GnbFamTsFunc->GnbFmTimeStamp (StdHeader);
  }
  return 0;
}



