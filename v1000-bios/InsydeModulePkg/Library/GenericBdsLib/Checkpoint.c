/** @file
  Checkpoint related functions
;******************************************************************************
;* Copyright (c) 2014 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "InternalBdsLib.h"

/**
  Internal function to only initialize size member in checkpoint data and
  use this checkpoint data to trigger checkpoint.

  @param[in] BdsCheckpoint        The number ID of checkpoint.
  @param[in] DataSize             Checkpoint data size by bytes.

  @retval EFI_SUCCESS             Trigger checkpoint successfully.
  @retval EFI_INVALID_PARAMETER   BdsCheckpoint is NULL or DataSize is smaller than 4 bytes.
  @retval EFI_OUT_OF_RESOURCES    Allocate chekcpoint data failed.
**/
STATIC
EFI_STATUS
CommonTriggerCpFunction (
  IN   EFI_GUID   *BdsCheckpoint,
  IN   UINT32     DataSize
  )
{
  VOID         *Interface;
  EFI_STATUS   Status;

  if (BdsCheckpoint == NULL || DataSize < sizeof (UINT32)) {
    return EFI_INVALID_PARAMETER;
  }
  Interface = AllocateZeroPool (DataSize);
  if (Interface == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (Interface, &DataSize, sizeof (UINT32));
  DEBUG ((EFI_D_INFO, "Checkpoint Trigger: %g\n", BdsCheckpoint));
  Status = H2OCpTrigger (BdsCheckpoint, Interface);
  DEBUG ((EFI_D_INFO, "Checkpoint Result: %x\n", ((H2O_BDS_CP_READY_TO_BOOT_BEFORE_DATA *)Interface)->Status));
  FreePool (Interface);
  return Status;
}

/**
  Initalize H2O_BDS_CP_CON_IN_CONNECT_AFTER_DATA
  data and trigger gH2OBdsCpConInConnectAfterGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpConInConnectAfterGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpConInConnectAfterGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpConInConnectAfter (
  VOID
  )
{
  return CommonTriggerCpFunction (
           &gH2OBdsCpConInConnectAfterGuid,
           sizeof (H2O_BDS_CP_CON_IN_CONNECT_AFTER_DATA)
           );
}

/**
  Initalize H2O_BDS_CP_CON_IN_CONNECT_BEFORE_DATA
  data and trigger gH2OBdsCpConInConnectBeforeGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpConInConnectBeforeGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpConInConnectBeforeGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpConInConnectBefore (
  VOID
  )
{
  return CommonTriggerCpFunction (
           &gH2OBdsCpConInConnectBeforeGuid,
           sizeof (H2O_BDS_CP_CON_IN_CONNECT_BEFORE_DATA)
           );
}

/**
  Initalize H2O_BDS_CP_CON_OUT_CONNECT_AFTER_DATA
  data and trigger gH2OBdsCpConOutConnectAfterGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpConOutConnectAfterGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpConOutConnectAfterGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpConOutConnectAfter (
  VOID
  )
{
  return CommonTriggerCpFunction (
           &gH2OBdsCpConOutConnectAfterGuid,
           sizeof (H2O_BDS_CP_CON_OUT_CONNECT_AFTER_DATA)
           );
}

/**
  Initalize H2O_BDS_CP_CON_OUT_CONNECT_BEFORE_DATA
  data and trigger gH2OBdsCpConOutConnectBeforeGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpConOutConnectBeforeGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpConOutConnectBeforeGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpConOutConnectBefore (
  VOID
  )
{
  return CommonTriggerCpFunction (
           &gH2OBdsCpConOutConnectBeforeGuid,
           sizeof (H2O_BDS_CP_CON_OUT_CONNECT_BEFORE_DATA)
           );
}

/**
  Initalize H2O_BDS_CP_READY_TO_BOOT_AFTER_DATA
  data and trigger gH2OBdsCpReadyToBootAfterGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpReadyToBootAfterGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpReadyToBootAfterGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpReadyToBootAfter (
  VOID
  )
{
  return CommonTriggerCpFunction (
           &gH2OBdsCpReadyToBootAfterGuid,
           sizeof (H2O_BDS_CP_READY_TO_BOOT_AFTER_DATA)
           );
}

/**
  Initalize H2O_BDS_CP_READY_TO_BOOT_BEFORE_DATA
  data and trigger gH2OBdsCpReadyToBootBeforeGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpReadyToBootBeforeGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpReadyToBootBeforeGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpReadyToBootBefore (
  VOID
  )
{
  return CommonTriggerCpFunction (
           &gH2OBdsCpReadyToBootBeforeGuid,
           sizeof (H2O_BDS_CP_READY_TO_BOOT_BEFORE_DATA)
           );
}

/**
  Internal function to initialize H2O_BDS_CP_CONNECT_ALL_AFTER_DATA data and trigger
  gH2OBdsCpConnectAllAfterGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpConnectAllAfterGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpConnectAllAfterGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpConnectAllAfter (
  VOID
  )
{
  return CommonTriggerCpFunction (
           &gH2OBdsCpConnectAllAfterGuid,
           sizeof (H2O_BDS_CP_CONNECT_ALL_AFTER_DATA)
           );
}

/**
  Internal function to initialize H2O_BDS_CP_CONNECT_ALL_BEFORE_DATA data and trigger
  gH2OBdsCpConnectAllBeforeGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpConnectAllBeforeGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpConnectAllBeforeGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpConnectAllBefore (
  VOID
  )
{
  return CommonTriggerCpFunction (
           &gH2OBdsCpConnectAllBeforeGuid,
           sizeof (H2O_BDS_CP_CONNECT_ALL_BEFORE_DATA)
           );
}

/**
  Internal function to initialize H2O_BDS_CP_DXE_SMM_READY_TO_LOCK_BEFORE_DATA data and trigger
  gH2OBdsCpDxeSmmReadyToLockBeforeGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpDxeSmmReadyToLockBeforeGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpDxeSmmReadyToLockBeforeGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpDxeSmmReadyToLockBefore (
  VOID
  )
{
  return CommonTriggerCpFunction (
           &gH2OBdsCpDxeSmmReadyToLockBeforeGuid,
           sizeof (H2O_BDS_CP_DXE_SMM_READY_TO_LOCK_BEFORE_DATA)
           );
}

/**
  Internal function to initialize H2O_BDS_CP_DXE_SMM_READY_TO_LOCK_AFTER_DATA data and trigger
  gH2OBdsCpDxeSmmReadyToLockAfterGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpDxeSmmReadyToLockAfterGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpDxeSmmReadyToLockAfterGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpDxeSmmReadyToLockAfter (
  VOID
  )
{
  return CommonTriggerCpFunction (
           &gH2OBdsCpDxeSmmReadyToLockAfterGuid,
           sizeof (H2O_BDS_CP_DXE_SMM_READY_TO_LOCK_AFTER_DATA)
           );
}
