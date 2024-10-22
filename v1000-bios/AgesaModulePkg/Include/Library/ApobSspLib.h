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
#ifndef _APOB_SSP_LIB_H_
#define _APOB_SSP_LIB_H_

#include <Addendum/Apcb/Inc/APOBCMN.h>

EFI_STATUS
ApobGetApcbInstanceSsp (
  IN OUT   UINT8  *ApcbInstance
  );


EFI_STATUS
ApobGetApcbRecoveryFlagSsp (
  IN OUT   BOOLEAN  *ApcbRecoveryFlag
  );


EFI_STATUS
ApobGetDimmCfgUpdatedFlagSsp (
  IN OUT   BOOLEAN  *DimmCfgUpdatedFlag
  );

EFI_STATUS
ApobGetPhysCcdNumberSsp (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN OUT   UINT8 *PhysCcdNumber
);

EFI_STATUS
ApobGetPhysComplexNumberSsp (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN OUT   UINT8 *PhysComplexNumber
  );

EFI_STATUS
ApobGetPhysCoreNumberSsp (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN OUT   UINT8  *PhysCoreNumber
  );

EFI_STATUS
ApobGetIsThreadEnabledSsp (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN       UINT32 ThreadIndex,
  IN OUT   BOOLEAN *IsThreadEnabled
  );

EFI_STATUS
ApobGetEccEnableSsp (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ChannelIndex,
  IN OUT   BOOLEAN *EccEnable
  );

EFI_STATUS
ApobGetNvdimmPresentInSystemSsp (
  IN       UINT32 ApobInstanceId,
  IN OUT   BOOLEAN *NvdimmPresentInSystem
  );

EFI_STATUS
ApobGetNvdimmInfoSsp (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT32 **NvdimmInfo,
  IN OUT   UINT32 *NvdimmInfoSize
  );

EFI_STATUS
ApobGetMaxDimmsPerChannelSsp (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxDimmsPerChannel
  );

EFI_STATUS
ApobGetMaxChannelsPerDieSsp (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerDie
  );

EFI_STATUS
ApobGetMaxChannelsPerDieZpMcm (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerDie
  );

EFI_STATUS
ApobGetMaxChannelsPerSocketSsp (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerSocket
  );

EFI_STATUS
ApobGetDimmSmbusInfoSsp (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN OUT   APOB_SPD_STRUCT **DimmSmbusInfo
  );

EFI_STATUS
ApobGetMemClkFreqSsp (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *MemClkFreq
  );

EFI_STATUS
ApobGetDdrMaxRateSsp (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *DdrMaxRate
  );

EFI_STATUS
ApobGetMemGenInfoElementSsp (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ElementId,
  IN OUT   UINT16 *ElementSizeInByte,
  IN OUT   UINT64 *ElementValue
  );

EFI_STATUS
ApobGetNumberOfValidThresholdsSsp (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 *NumberOfValidThresholds
  );

EFI_STATUS
ApobGetThresholdsSsp (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN       EDC_THROTTLE_THRESHOLD **Thresholds
  );

#endif //_APOB_SSP_LIB_H_

