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
#ifndef _APOB_ZPRV_LIB_H_
#define _APOB_ZPRV_LIB_H_

#include <Addendum/Apcb/Inc/APOBCMN.h>

EFI_STATUS
ApobGetApcbInstanceZpRv (
  IN OUT   UINT8  *ApcbInstance
  );


EFI_STATUS
ApobGetApcbRecoveryFlagZpRv (
  IN OUT   BOOLEAN  *ApcbRecoveryFlag
  );


EFI_STATUS
ApobGetDimmCfgUpdatedFlagZpRv (
  IN OUT   BOOLEAN  *DimmCfgUpdatedFlag
  );

EFI_STATUS
ApobGetPhysComplexNumberZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN OUT   UINT8 *PhysComplexNumber
  );

EFI_STATUS
ApobGetPhysCoreNumberZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN OUT   UINT8  *PhysCoreNumber
  );

EFI_STATUS
ApobGetIsThreadEnabledZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN       UINT32 ThreadIndex,
  IN OUT   BOOLEAN *IsThreadEnabled
  );

EFI_STATUS
ApobGetEccEnableZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ChannelIndex,
  IN OUT   BOOLEAN *EccEnable
  );

EFI_STATUS
ApobGetNvdimmPresentInSystemZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   BOOLEAN *NvdimmPresentInSystem
);

EFI_STATUS
ApobGetNvdimmInfoZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT32 **NvdimmInfo,
  IN OUT   UINT32 *NvdimmInfoSize
  );

EFI_STATUS
ApobGetMaxDimmsPerChannelZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxDimmsPerChannel
  );

EFI_STATUS
ApobGetMaxChannelsPerDieZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerDie
  );

EFI_STATUS
ApobGetMaxChannelsPerSocketZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerSocket
  );

EFI_STATUS
ApobGetDimmSmbusInfoZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN OUT   APOB_SPD_STRUCT **DimmSmbusInfo
  );

EFI_STATUS
ApobGetMemClkFreqZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *MemClkFreq
  );

EFI_STATUS
ApobGetDdrMaxRateZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *DdrMaxRate
  );

EFI_STATUS
ApobGetMemGenInfoElementZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ElementId,
  IN OUT   UINT16 *ElementSizeInByte,
  IN OUT   UINT64 *ElementValue
  );

EFI_STATUS
ApobGetNumberOfValidThresholdsZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 *NumberOfValidThresholds
  );

EFI_STATUS
ApobGetThresholdsZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN       EDC_THROTTLE_THRESHOLD **Thresholds
  );
#endif //_APOB_ZPRV_LIB_H_

