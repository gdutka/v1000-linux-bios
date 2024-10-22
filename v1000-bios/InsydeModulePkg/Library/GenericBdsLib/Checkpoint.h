/** @file
  Header file of Checkpoint related functions

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
#ifndef _INTERNAL_CHECKPOINT_H_
#define _INTERNAL_CHECKPOINT_H_


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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

#endif