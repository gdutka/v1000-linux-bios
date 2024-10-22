/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB API definition.
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

#ifndef _GNBINTERFACE_H_
#define _GNBINTERFACE_H_

AGESA_STATUS
GnbInitAtReset (
  IN      AMD_CONFIG_PARAMS       *StdHeader
  );

AGESA_STATUS
GnbInitAtEarly (
  IN OUT  AMD_EARLY_PARAMS        *EarlyParamsPtr
  );

VOID
GnbInitDataStructAtPostDef (
  IN OUT  GNB_POST_CONFIGURATION  *GnbPostConfigPtr,
  IN      AMD_POST_PARAMS         *PostParamsPtr
  );

AGESA_STATUS
GnbInitAtPost (
  IN OUT  AMD_POST_PARAMS         *PostParamsPtr
  );

VOID
GnbInitDataStructAtEnvDef (
  IN OUT  GNB_ENV_CONFIGURATION   *GnbEnvConfigPtr,
  IN      AMD_ENV_PARAMS          *EnvParamsPtr
  );

VOID
GnbInitDataStructAtMidDef (
  IN OUT  GNB_MID_CONFIGURATION   *GnbMidConfigPtr,
  IN      AMD_MID_PARAMS          *MidParamsPtr
  );

VOID
GnbInitDataStructAtLateDef (
  IN OUT  GNB_LATE_CONFIGURATION  *GnbLateConfigPtr,
  IN      AMD_LATE_PARAMS         *LateParamsPtr
  );

AGESA_STATUS
GnbInitAtEnv (
  IN       AMD_ENV_PARAMS          *EnvParamsPtr
  );

AGESA_STATUS
GnbInitAtMid (
  IN OUT   AMD_MID_PARAMS          *MidParamsPtr
  );

AGESA_STATUS
GnbInitAtMidLate (
  IN OUT   AMD_MID_PARAMS          *MidParamsPtr
  );

AGESA_STATUS
GnbInitAtLate (
  IN OUT   AMD_LATE_PARAMS         *LateParamsPtr
  );

AGESA_STATUS
GnbInitAtPostAfterDram (
  IN OUT   AMD_POST_PARAMS         *PostParamsPtr
  );

AGESA_STATUS
AmdGnbRecovery (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GnbInitAtEarlier (
  IN OUT   AMD_EARLY_PARAMS        *EarlyParamsPtr
  );

AGESA_STATUS
GnbInitAtRtb (
  IN OUT   AMD_RTB_PARAMS          *AmdRtbParams
  );

#endif

