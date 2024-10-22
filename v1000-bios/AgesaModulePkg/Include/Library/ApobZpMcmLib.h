/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP APOB related functions Prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 312133 $   @e \$Date: 2015-02-03 02:47:45 +0800 (Tue, 03 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _APOB_ZPMCM_LIB_H_
#define _APOB_ZPMCM_LIB_H_

#include <Addendum/Apcb/Inc/APOBCMN.h>

EFI_STATUS
ApobGetApcbInstanceZpMcm (
  IN OUT   UINT8  *ApcbInstance
  );


EFI_STATUS
ApobGetApcbRecoveryFlagZpMcm (
  IN OUT   BOOLEAN  *ApcbRecoveryFlag
  );


EFI_STATUS
ApobGetDimmCfgUpdatedFlagZpMcm (
  IN OUT   BOOLEAN  *DimmCfgUpdatedFlag
  );

EFI_STATUS
ApobGetPhysComplexNumberZpMcm (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN OUT   UINT8 *PhysComplexNumber
  );

EFI_STATUS
ApobGetPhysCoreNumberZpMcm (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN OUT   UINT8  *PhysCoreNumber
  );

EFI_STATUS
ApobGetIsThreadEnabledZpMcm (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN       UINT32 ThreadIndex,
  IN OUT   BOOLEAN *IsThreadEnabled
  );

EFI_STATUS
ApobGetEccEnableZpMcm (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ChannelIndex,
  IN OUT   BOOLEAN *EccEnable
  );

EFI_STATUS
ApobGetNvdimmPresentInSystemZpMcm (
  IN       UINT32 ApobInstanceId,
  IN OUT   BOOLEAN *NvdimmPresentInSystem
  );

EFI_STATUS
ApobGetNvdimmInfoZpMcm (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT32 **NvdimmInfo,
  IN OUT   UINT32 *NvdimmInfoSize
  );

EFI_STATUS
ApobGetMaxDimmsPerChannelZpMcm (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxDimmsPerChannel
  );

EFI_STATUS
ApobGetMaxChannelsPerDieZpMcm (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerDie
  );

EFI_STATUS
ApobGetMaxChannelsPerSocketZpMcm (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerSocket
  );

EFI_STATUS
ApobGetDimmSmbusInfoZpMcm (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN OUT   APOB_SPD_STRUCT **DimmSmbusInfo
  );

EFI_STATUS
ApobGetMemClkFreqZpMcm (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *MemClkFreq
  );

EFI_STATUS
ApobGetDdrMaxRateZpMcm (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *DdrMaxRate
  );

EFI_STATUS
ApobGetMemGenInfoElementZpMcm (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ElementId,
  IN OUT   UINT16 *ElementSizeInByte,
  IN OUT   UINT64 *ElementValue
  );

EFI_STATUS
ApobGetNumberOfValidThresholdsZpMcm (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 *NumberOfValidThresholds
  );

EFI_STATUS
ApobGetThresholdsZpMcm (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN       EDC_THROTTLE_THRESHOLD **Thresholds
  );

#endif //_APOB_ZPMCM_LIB_H_

