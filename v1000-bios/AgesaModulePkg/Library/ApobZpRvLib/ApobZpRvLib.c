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
#include <Library/AmdSocBaseLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/AmdPspApobLib.h>
#include <RV/APOB.h>
#include <ApobCommonServicePub.h>
#include <Filecode.h>

#define FILECODE LIBRARY_APOBZPRVLIB_APOBZPRVLIB_FILECODE


/**
 *  Return ApobApcbRecoveryFlag
 *
 *  @param[in, out] APCB Recovery Flag
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
ApobGetApcbRecoveryFlagZpRv (
  IN OUT   BOOLEAN  *ApcbRecoveryFlag
  )
{
  EFI_STATUS Status;
  APOB_APCB_BOOT_INFO_STRUCT *ApobEntry;
  ApobEntry = NULL;
  *ApcbRecoveryFlag = FALSE;
  if (PcdGetBool (PcdAmdPspApcbRecoveryEnable) == FALSE) {
    // Alway return FALSE, if ApcbRecovery feature disabled
    return EFI_SUCCESS;
  }
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_APCB_BOOT_INFO_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get ApobApcbRecoveryFlag\n");
    return EFI_NOT_FOUND;
  }
  *ApcbRecoveryFlag = ApobEntry->ApcbRecoveryFlag;
  return Status;
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
ApobGetApcbInstanceZpRv (
  IN OUT   UINT8  *ApcbInstance
  )
{
  EFI_STATUS Status;
  APOB_APCB_BOOT_INFO_STRUCT *ApobEntry;
  ApobEntry = NULL;
  *ApcbInstance = 0;
  IDS_HDT_CONSOLE_PSP_TRACE ("GetApcbInstanceZpRv\n");

  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_APCB_BOOT_INFO_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get ApobApcbInstance\n");
    return EFI_NOT_FOUND;
  }
  *ApcbInstance = (UINT8) ApobEntry->ApcbActiveInstance;
  return Status;
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
ApobGetDimmCfgUpdatedFlagZpRv (
  IN OUT   BOOLEAN  *DimmCfgUpdatedFlag
  )
{
  EFI_STATUS Status;
  APOB_APCB_BOOT_INFO_STRUCT *ApobEntry;
  UINT16 PmemClrReason;

  PmemClrReason =0;
  ApobEntry = NULL;
  *DimmCfgUpdatedFlag = FALSE;

  IDS_HDT_CONSOLE_PSP_TRACE ("GetApcbInstanceZpRv\n");
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_APCB_BOOT_INFO_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get DimmCfgUpdatedFlag\n");
    return EFI_NOT_FOUND;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("DimmCfgUpdatedFlag %x\n", ApobEntry->DimmConfigurationUpdated);
  *DimmCfgUpdatedFlag = ApobEntry->DimmConfigurationUpdated;
    /*
  PersistentMemClrReason:
  1) MEM_CLEAR_STATE_CLEARED can happen for below reasons:
     a) MEM_CLEAR_RSN_COLDRESET
     b) MEM_CLEAR_RSN_MEMCLR_REQUESTED
     c) MEM_CLEAR_RSN_MEMCLR_UcERROR
  2) MEM_CLEAR_RSN_MEMCLR_REQUESTED mem clear is requested by user
  3) MEM_CLEAR_STATE_CLRDISABLED mem clear is disabled
  */
  PmemClrReason = (UINT16) ApobEntry->PersistentMemClrReason;
  IDS_HDT_CONSOLE_PSP_TRACE ("Persistent memory clear reason: 0x%04X\n", PmemClrReason);

  return Status;
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
ApobGetPhysComplexNumberZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN OUT   UINT8 *PhysComplexNumber
  )
{
  EFI_STATUS Status;

  APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE_STRUCT *ApobEntry;

  Status = AmdPspGetApobEntryInstance (APOB_CCX, APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    *PhysComplexNumber = ApobEntry->ComplexMap[CcxIndex].PhysComplexNumber;
  }
  return Status;
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
ApobGetPhysCoreNumberZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN OUT   UINT8  *PhysCoreNumber
  )
{
  EFI_STATUS Status;

  APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE_STRUCT *ApobEntry;

  Status = AmdPspGetApobEntryInstance (APOB_CCX, APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    *PhysCoreNumber = ApobEntry->ComplexMap[CcxIndex].CoreInfo[CoreIndex].PhysCoreNumber;
  }
  return Status;
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
ApobGetIsThreadEnabledZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN       UINT32 ThreadIndex,
  IN OUT   BOOLEAN *IsThreadEnabled
  )
{
  EFI_STATUS Status;

  APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE_STRUCT *ApobEntry;

  Status = AmdPspGetApobEntryInstance (APOB_CCX, APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    *IsThreadEnabled = ApobEntry->ComplexMap[CcxIndex].CoreInfo[CoreIndex].IsThreadEnabled[ThreadIndex];
  }
  return Status;
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
ApobGetEccEnableZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ChannelIndex,
  IN OUT   BOOLEAN *EccEnable
  )
{
  EFI_STATUS Status;
  APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT *ApobEntry;
  
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    *EccEnable = ApobEntry->EccEnable[ChannelIndex];
  }
  return Status;
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
ApobGetNvdimmPresentInSystemZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   BOOLEAN *NvdimmPresentInSystem
  )
{
  EFI_STATUS Status;
  APOB_MEM_NVDIMM_INFO_STRUCT *ApobEntry;
  
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_NVDIMM_INFO_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    *NvdimmPresentInSystem = ApobEntry->NvdimmPresentInSystem;
  }
  return Status;
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
ApobGetNvdimmInfoZpRv (
  IN       UINT32 ApobInstanceId, 
  IN OUT   UINT32 **NvdimmInfo,
  IN OUT   UINT32 *NvdimmInfoSize
  )
{
  EFI_STATUS Status;
  APOB_MEM_NVDIMM_INFO_STRUCT *ApobEntry;
  
 *NvdimmInfoSize = sizeof (ApobEntry->NvdimmInfo);

  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_NVDIMM_INFO_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
     *NvdimmInfo = &ApobEntry->NvdimmInfo[0][0];
  }
  return Status;
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
ApobGetMaxDimmsPerChannelZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxDimmsPerChannel
  )
{
  EFI_STATUS Status;
  APOB_MEM_DIMM_SPD_DATA_STRUCT *ApobEntry;
  
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_DIMM_SPD_DATA_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    *MaxDimmsPerChannel = ApobEntry->MaxDimmsPerChannel;
  }
  return Status;
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
ApobGetMaxChannelsPerDieZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerDie
  )
{
  *MaxChannelsPerDie = ABL_APOB_MAX_CHANNELS_PER_DIE;
  return EFI_SUCCESS;
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
ApobGetMaxChannelsPerSocketZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerSocket
  )
{
  EFI_STATUS Status;
  APOB_MEM_DIMM_SPD_DATA_STRUCT *ApobEntry;
  
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_DIMM_SPD_DATA_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    *MaxChannelsPerSocket = ApobEntry->MaxChannelsPerSocket;
  }
  return Status;
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
ApobGetDimmSmbusInfoZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN OUT   APOB_SPD_STRUCT **DimmSmbusInfo
  )
{
  EFI_STATUS Status;
  APOB_MEM_DIMM_SPD_DATA_STRUCT *ApobEntry;

  if (Index > ABL_APOB_MAX_CHANNELS_PER_DIE * ABL_APOB_MAX_DIMMS_PER_CHANNEL) {
    return EFI_NOT_FOUND;
  }

  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_DIMM_SPD_DATA_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    *DimmSmbusInfo = &ApobEntry->DimmSmbusInfo[Index];
  }
  return Status;
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
ApobGetMemClkFreqZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *MemClkFreq
  )
{
  EFI_STATUS Status;
  APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT *ApobEntry;
  
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    *MemClkFreq = ApobEntry->MemClkFreq;
  }
  return Status;
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
ApobGetDdrMaxRateZpRv (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *DdrMaxRate
  )
{
  EFI_STATUS Status;
  APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT *ApobEntry;
  
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    *DdrMaxRate = ApobEntry->DdrMaxRate;
  }
  return Status;
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
ApobGetMemGenInfoElementZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ElementId,
  IN OUT   UINT16 *ElementSizeInByte,
  IN OUT   UINT64 *ElementValue
  )
{
  EFI_STATUS Status;
  APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT *ApobEntry;
  
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE, ApobInstanceId, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    switch (ElementId) {
      case MemCtrllerProcOdtDddr4Current:
        *ElementSizeInByte=2;
        *ElementValue = ApobEntry->MemCtrllerProcOdtDddr4Current;
        break;

      case MemCtrllerRttNomDddr4Current:
        *ElementSizeInByte=1;
        *ElementValue = ApobEntry->MemCtrllerRttNomDddr4Current;
        break;

      case MemCtrllerRttWrDddr4Current:
        *ElementSizeInByte=1;
        *ElementValue = ApobEntry->MemCtrllerRttWrDddr4Current;
        break;

      case MemCtrllerRttParkDddr4Current:
        *ElementSizeInByte=1;
        *ElementValue = ApobEntry->MemCtrllerRttParkDddr4Current;
        break;

      case MemCtrllerAddrCmdSetupDddr4Current:
        *ElementSizeInByte=1;
        *ElementValue = ApobEntry->MemCtrllerAddrCmdSetupDddr4Current;
        break;

      case MemCtrllerCsOdtSetupDddr4Current:
        *ElementSizeInByte=1;
        *ElementValue = ApobEntry->MemCtrllerCsOdtSetupDddr4Current;
        break;

      case MemCtrllerCkeSetupDddr4Current:
        *ElementSizeInByte=1;
        *ElementValue = ApobEntry->MemCtrllerCkeSetupDddr4Current;
        break;

      case MemCtrllerCadBusAddrCmdDrvStrenDddr4Current:
        *ElementSizeInByte=1;
        *ElementValue = ApobEntry->MemCtrllerCadBusAddrCmdDrvStrenDddr4Current;
        break;

      case MemCtrllerCadBusClkDrvStrenDddr4Current:
        *ElementSizeInByte=1;
        *ElementValue = ApobEntry->MemCtrllerCadBusClkDrvStrenDddr4Current;
        break;

      case MemCtrllerCsOdtCmdDrvStrenDddr4Current:
        *ElementSizeInByte=1;
        *ElementValue = ApobEntry->MemCtrllerCsOdtCmdDrvStrenDddr4Current;
        break;

      case MemCtrllerCkeDrvStrenDddr4Current:
        *ElementSizeInByte=1;
        *ElementValue = ApobEntry->MemCtrllerCkeDrvStrenDddr4Current;
        break;

      case InterLeaveCapability:
        *ElementSizeInByte=4;
        *ElementValue = ApobEntry->InterLeaveCapability.Value;
        break;

      case InterLeaveCurrentMode:
        *ElementSizeInByte=4;
        *ElementValue = ApobEntry->InterLeaveCurrentMode.Value;
        break;

      case InterLeaveSize:
        *ElementSizeInByte=1;
        *ElementValue = ApobEntry->InterLeaveSize;
        break;
      default:
        ASSERT (FALSE);
        Status = EFI_NOT_FOUND;
        break;
    }
  }
  return Status;
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
ApobGetNumberOfValidThresholdsZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 *NumberOfValidThresholds
  )
{
  EFI_STATUS Status;
  APOB_EDC_THROTTLE_THRESHOLD_INFO_STRUCT *ApobEntry;

  Status = AmdPspGetApobEntryInstance (APOB_CCX, APOB_CCX_EDC_THROTTLE_THRESH_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);

  if (Status == EFI_SUCCESS) {
    *NumberOfValidThresholds = ApobEntry->NumberOfValidThresholds;
  }

  return Status;
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
ApobGetThresholdsZpRv (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN       EDC_THROTTLE_THRESHOLD **Thresholds
  )
{
  EFI_STATUS Status;
  APOB_EDC_THROTTLE_THRESHOLD_INFO_STRUCT *ApobEntry;

  if (Index > MAX_NUMBER_OF_THROTTLERS) {
    return EFI_NOT_FOUND;
  }

  Status = AmdPspGetApobEntryInstance (APOB_CCX, APOB_CCX_EDC_THROTTLE_THRESH_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (Status == EFI_SUCCESS) {
    *Thresholds = &ApobEntry->Thresholds[Index];
  }

  return Status;
}

