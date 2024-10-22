/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP APOB related functions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309172 $   @e \$Date: 2014-12-10 21:13:24 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#include "Uefi.h"
#include <Pi/PiMultiPhase.h>
#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Addendum/Apcb/Inc/APOBCMN.h>


/**
 *  Return ApobApcbRecoveryFlag
 *
 *  @param[in, out] APCB Recovery Flag
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetApcbRecoveryFlagDummy (
  IN OUT   BOOLEAN  *ApcbRecoveryFlag
  )
{
  return EFI_UNSUPPORTED;
}

/*
/**
 *  Return ApobApcbRecoveryFlag
 *
 *  @param[in, out] APCB Active Instance
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetApcbInstanceDummy (
  IN OUT   UINT8  *ApcbInstance
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Return DimmCfgUpdatedFlag
 *
 *  @param[in, out] DimmCfgUpdatedFlag
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetDimmCfgUpdatedFlagDummy (
  IN OUT   BOOLEAN  *DimmCfgUpdatedFlag
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Get the physical complex number by reading APOB APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in] CcxIndex used access ComplexMap
 *  @param[in, out] PhysComplexNumber
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetPhysComplexNumberDummy (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN OUT   UINT8 *PhysComplexNumber
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Get the physical core number by reading APOB APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in] CcxIndex used access ComplexMap
 *  @param[in] CoreIndex used access CoreInfo
 *  @param[in, out] PhysCoreNumber
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetPhysCoreNumberDummy (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN OUT   UINT8  *PhysCoreNumber
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Check if the thread is enable by reading APOB APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in] CcxIndex used access ComplexMap
 *  @param[in] CoreIndex used access CoreInfo
 *  @param[in] ThreadIndex used access IsThreadEnabled
 *  @param[in, out] IsThreadEnabled
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetIsThreadEnabledDummy (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN       UINT32 ThreadIndex,
  IN OUT   BOOLEAN *IsThreadEnabled
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Check EccEnable by reading APOB APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in] ChannelIndex used access EccEnable arrary
 *  @param[in, out] EccEnable
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetEccEnableDummy (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ChannelIndex,
  IN OUT   BOOLEAN *EccEnable
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Get NvdimmPresentInSystem by reading APOB APOB_MEM_NVDIMM_INFO_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in, out] NvdimmPresentInSystem
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetNvdimmPresentInSystemDummy (
  IN       UINT32 ApobInstanceId,
  IN OUT   BOOLEAN *NvdimmPresentInSystem
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Get NvdimmPresentInSystem by reading APOB APOB_MEM_NVDIMM_INFO_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in, out] NvdimmInfo
 *  @param[in, out] NvdimmInfoSize, on Input size used for hold NvdimmInfo, on Output the real size used in APOB
 *
 *  @retval       EFI_SUCCESS             Function succeed
 *  @retval       EFI_BUFFER_TOO_SMALL    NvdimmInfoSize not enough size
 *  @retval       other NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetNvdimmInfoDummy (
  IN       UINT32 ApobInstanceId, 
  IN OUT   UINT32 **NvdimmInfo,
  IN OUT   UINT32 *NvdimmInfoSize
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Get MaxDimmsPerChannel of SPD structure by reading APOB APOB_MEM_DIMM_SPD_DATA_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in, out] MaxDimmsPerChannel
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetMaxDimmsPerChannelDummy (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxDimmsPerChannel
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Get MaxChannelsPerDie
 *
 *  @param[in] ApobInstanceId not used
 *  @param[in, out] MaxDimmsPerChannel
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetMaxChannelsPerDieDummy (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerDie
  )
{
  return EFI_UNSUPPORTED;
}
/**
 *  Get MaxChannelsPerSocket of SPD structure by reading APOB APOB_MEM_DIMM_SPD_DATA_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in, out] MaxChannelsPerSocket
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetMaxChannelsPerSocketDummy (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerSocket
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Get DimmSmbusInfo of SPD structure by reading APOB APOB_MEM_DIMM_SPD_DATA_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in] Index used to access DimmSmbusInfo structure
 *  @param[in, out] DimmSmbusInfo
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetDimmSmbusInfoDummy (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN OUT   APOB_SPD_STRUCT **DimmSmbusInfo
  )
{
  return EFI_UNSUPPORTED;
}


/**
 *  Get MemClkFreq of SPD structure by reading APOB APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in, out] MemClkFreq
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetMemClkFreqDummy (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *MemClkFreq
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Get DdrMaxRate of SPD structure by reading APOB APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in, out] DdrMaxRate
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetDdrMaxRateDummy (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *DdrMaxRate
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Get MemGenInfoElement by reading APOB APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE
 *
 *  @param[in] ApobInstanceId
 *  @param[in] ElementId
 *  @param[in, out] ElementSizeInByte
 *  @param[in, out] ElementValue
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetMemGenInfoElementDummy (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ElementId,
  IN OUT   UINT16 *ElementSizeInByte,
  IN OUT   UINT64 *ElementValue
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Get NumberOfValidThresholds by reading APOB APOB_EDC_THROTTLE_THRESHOLD_INFO_STRUCT
 *
 *  @param[in] ApobInstanceId
 *  @param[in, out] NumberOfValidThresholds
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetNumberOfValidThresholdsDummy (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 *NumberOfValidThresholds
  )
{
  return EFI_UNSUPPORTED;
}

/**
 *  Get Thresholds by reading APOB APOB_EDC_THROTTLE_THRESHOLD_INFO_STRUCT
 *
 *  @param[in] ApobInstanceId
 *  @param[in] Index
 *  @param[in, out] Thresholds
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetThresholdsDummy (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN       EDC_THROTTLE_THRESHOLD **Thresholds
  )
{
  return EFI_UNSUPPORTED;
}

