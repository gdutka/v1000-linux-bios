/* $NoKeywords:$ */
/**
 * @file
 *
 * APOB service Library
 *
 * Contains interface of the APOB library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 317842 $   @e \$Date: 2015-04-29 15:06:52 +0800 (Wed, 29 Apr 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/ApobCommonServiceProtocol.h>
#include <Filecode.h>

#define FILECODE LIBRARY_APOBCOMMONSERVICELIBDXE_APOBCOMMONSERVICELIBDXE_FILECODE

APOB_COMMON_SERVICE_PROTOCOL *mApobServicePtr = NULL;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
GetApobServicePointer (
  )
{
  EFI_STATUS Status;
  Status = gBS->LocateProtocol (&gApobCommonServiceProtocolGuid, NULL, (VOID**)&mApobServicePtr);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return Status;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
ApobGetApcbInstance (
  IN OUT   UINT8  *ApcbInstance
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetApcbInstance (ApcbInstance);
}


EFI_STATUS
ApobGetApcbRecoveryFlag (
  IN OUT   BOOLEAN  *ApcbRecoveryFlag
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetApcbRecoveryFlag (ApcbRecoveryFlag);
}


EFI_STATUS
ApobGetDimmCfgUpdatedFlag (
  IN OUT   BOOLEAN  *DimmCfgUpdatedFlag
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetDimmCfgUpdatedFlag (DimmCfgUpdatedFlag);
}

EFI_STATUS
ApobGetPhysCcdNumber (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN OUT   UINT8 *PhysCcdNumber
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetPhysCcdNumber (ApobInstanceId, CcdIndex, PhysCcdNumber);
}


EFI_STATUS
ApobGetPhysComplexNumber (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN OUT   UINT8 *PhysComplexNumber
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetPhysComplexNumber (ApobInstanceId, CcdIndex, CcxIndex, PhysComplexNumber);
}

EFI_STATUS
ApobGetPhysCoreNumber (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN OUT   UINT8  *PhysCoreNumber
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetPhysCoreNumber (ApobInstanceId, CcdIndex, CcxIndex, CoreIndex, PhysCoreNumber);
}

EFI_STATUS
ApobGetIsThreadEnabled (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 CcdIndex,
  IN       UINT32 CcxIndex,
  IN       UINT32 CoreIndex,
  IN       UINT32 ThreadIndex,
  IN OUT   BOOLEAN *IsThreadEnabled
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetIsThreadEnabled (ApobInstanceId, CcdIndex, CcxIndex, CoreIndex, ThreadIndex, IsThreadEnabled);
}

EFI_STATUS
ApobGetEccEnable (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ChannelIndex,
  IN OUT   BOOLEAN *EccEnable
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetEccEnable (ApobInstanceId, ChannelIndex, EccEnable);
}

EFI_STATUS
ApobGetNvdimmPresentInSystem (
  IN       UINT32 ApobInstanceId,
  IN OUT   BOOLEAN *NvdimmPresentInSystem
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetNvdimmPresentInSystem (ApobInstanceId, NvdimmPresentInSystem);
}

EFI_STATUS
ApobGetNvdimmInfo (
  IN       UINT32 ApobInstanceId, 
  IN OUT   UINT32 **NvdimmInfo,
  IN OUT   UINT32 *NvdimmInfoSize
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetNvdimmInfo (ApobInstanceId, NvdimmInfo, NvdimmInfoSize);
}

EFI_STATUS
ApobGetMaxDimmsPerChannel (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxDimmsPerChannel
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetMaxDimmsPerChannel (ApobInstanceId, MaxDimmsPerChannel);
}
EFI_STATUS
ApobGetMaxChannelsPerDie (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerDie
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetMaxChannelsPerDie (ApobInstanceId, MaxChannelsPerDie);

}

EFI_STATUS
ApobGetMaxChannelsPerSocket (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT8 *MaxChannelsPerSocket
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetMaxChannelsPerSocket (ApobInstanceId, MaxChannelsPerSocket);
}


EFI_STATUS
ApobGetDimmSmbusInfo (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN OUT   APOB_SPD_STRUCT **DimmSmbusInfo
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetDimmSmbusInfo (ApobInstanceId, Index, DimmSmbusInfo);
}


EFI_STATUS
ApobGetMemClkFreq (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *MemClkFreq
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetMemoryFrequency (ApobInstanceId, MemClkFreq);
}


EFI_STATUS
ApobGetDdrMaxRate (
  IN       UINT32 ApobInstanceId,
  IN OUT   UINT16 *DdrMaxRate
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetDdrMaxRate (ApobInstanceId, DdrMaxRate);
}


EFI_STATUS
ApobGetMemGenInfoElement (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 ElementId,
  IN OUT   UINT16 *ElementSizeInByte,
  IN OUT   UINT64 *ElementValue
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetMemGenInfoElement (ApobInstanceId, ElementId, ElementSizeInByte, ElementValue);
}

EFI_STATUS
ApobGetNumberOfValidThresholds (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 *NumberOfValidThresholds
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetNumberOfValidThresholds (ApobInstanceId, NumberOfValidThresholds);
}

EFI_STATUS
ApobGetThresholds (
  IN       UINT32 ApobInstanceId,
  IN       UINT32 Index,
  IN       EDC_THROTTLE_THRESHOLD **Thresholds
  )
{
  EFI_STATUS Status;

  Status = GetApobServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mApobServicePtr->ApobGetThresholds (ApobInstanceId, Index, Thresholds);
}
