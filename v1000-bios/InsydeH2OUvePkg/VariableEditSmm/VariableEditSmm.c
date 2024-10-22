/** @file
  Content file contains function definitions for Variable Edit Smm driver

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include "VariableEditSmm.h"

//
// Factory default Support Function
//
STATIC
IHISI_REGISTER_TABLE
H2OUVE_SIMSERVICE_REGISTER_TABLE[] = {
  //
  // AH=52h, H2O UEFI variable edit SMI service - Confirm the legality of the variables.
  //
  { UveVariableConfirm, "S52OemH2OSmiCallBac", H2OSmiCallBack                 }, \
  { UveVariableConfirm, "S52OemSetupCBSync00", SetupCallbackSyncCallbackIhisi }, \

  //
  // AH=53h, H2O UEFI variable edit SMI service - Boot information-related services.
  //
  { UveBootInfoService, "S53OemBootInfoSvc00", BootInfoService         }
};

H2O_IHISI_PROTOCOL                      *mH2OIhisi           = NULL;

/**
  Register IHISI sub function if SubFuncTable CmdNumber/AsciiFuncGuid define in PcdIhisiRegisterTable list.

  @param[out] SubFuncTable        Pointer to ihisi register table.
  @param[out] TableCount          SubFuncTable count

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
RegisterIhisiSubFunction (
  IN IHISI_REGISTER_TABLE         *SubFuncTable,
  IN UINT16                        TableCount
  )
{
  EFI_STATUS                    Status;
  UINT8                         *PcdTable;
  UINT8                         PcdPriority;
  UINT8                         EndChar;
  UINTN                         Index;
  UINTN                         PcdCount;
  UINTN                         PcdMaxCount;
  UINTN                         SignatureSize;
  BOOLEAN                       PcdFoundRegistered;
  UINTN                         WholeTableSize;
  UINTN                         TableSize;

  Status             = EFI_SUCCESS;
  EndChar            = 0;
  PcdFoundRegistered = FALSE;

  PcdTable    = (UINT8 *)PcdGetPtr (PcdH2OUveIhisiRegisterTable);
  PcdMaxCount = PcdGetSize (PcdH2OUveIhisiRegisterTable) / sizeof (UINT8);
  WholeTableSize = FixedPcdGetPtrSize (PcdH2OUveIhisiRegisterTable);
  TableSize = sizeof (PCD_IHISI_REGISTER_TABLE);
  if ((WholeTableSize % TableSize) != 0) {
    DEBUG ((DEBUG_ERROR, "PcdIhisiRegisterTable of description not follow PCD_IHISI_REGISTER_TABLE definition, \
                          it may cause some of IHISI function register fail \n"));
  }

  for (Index = 0; Index < TableCount; Index ++) {
    PcdCount   = 0;
    PcdPriority = 0x80;
    PcdFoundRegistered = FALSE;
    SignatureSize = AsciiStrLen (SubFuncTable[Index].FuncSignature);

    //
    // Caculate PCD of address to find 1. CmdNumber 2. FuncSignature 3. Priority
    //
    do {
      if (SubFuncTable[Index].CmdNumber == *(PcdTable + PcdCount)) {
        PcdCount++;
        if (AsciiStrnCmp (SubFuncTable[Index].FuncSignature, (CHAR8 *) (PcdTable + PcdCount), SignatureSize) == 0) {
          if (EndChar == *(PcdTable + PcdCount + SignatureSize)) {
            PcdPriority = *(PcdTable + PcdCount + SignatureSize + 1);
            PcdFoundRegistered = TRUE;
            break;
          }
        }
      }
      PcdCount++;
    } while (PcdCount < PcdMaxCount);

    if (PcdFoundRegistered) {
      Status = mH2OIhisi->RegisterCommand (SubFuncTable[Index].CmdNumber, SubFuncTable[Index].IhisiFunction, PcdPriority);
      if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "IHISI command :0x%X, priority : 0x%X, that already has a registered function\n", SubFuncTable[Index].CmdNumber, PcdPriority));
        ASSERT (FALSE);
      }
    }
  }

  return Status;
}

/**
  Register IHISI sub function for H2OUvePkg.

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
InstallH2OUveIhisiServices (
  VOID
  )
{
  EFI_STATUS                            Status;
  IHISI_REGISTER_TABLE                  *SubFuncTable;
  UINT16                                TableCount;

  Status = gSmst->SmmLocateProtocol (
                    &gH2OIhisiProtocolGuid,
                    NULL,
                    (VOID **) &mH2OIhisi
                    );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "[%a:%d] Status:%r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }


  SubFuncTable = H2OUVE_SIMSERVICE_REGISTER_TABLE;
  TableCount = sizeof(H2OUVE_SIMSERVICE_REGISTER_TABLE)/sizeof(H2OUVE_SIMSERVICE_REGISTER_TABLE[0]);
  Status = RegisterIhisiSubFunction (SubFuncTable, TableCount);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "[%a:%d] Status:%r\n", __FUNCTION__, __LINE__, Status));
  }

  return Status;
}

/**
  Check status of function disabled variable, install IHISI sub function and initialize SysPassword Driver.

 @param [in]   ImageHandle      Pointer to the loaded image protocol for this driver
 @param [in]   SystemTable      Pointer to the EFI System Table

 @retval EFI_SUCCESS
 @retval EFI_UNSUPPORTED        VarEditFunDis variable is exist and has been seted TRUE.
 @return Other                  Error occurred in this function.

**/
EFI_STATUS
EFIAPI
SetupCallbackSyncEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                                Status;
  VARIABLE_EDIT_FUNCTION_DISABLE_SETTING    VarEditFunDis;
  UINTN                                     VarEditFunDisSize;
  EFI_SMM_VARIABLE_PROTOCOL                 *SmmVariable;

  DEBUG ((DEBUG_INFO, "->SetupCallbackSyncEntryPoint\n"));

  ZeroMem (&VarEditFunDis, sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING));

  VarEditFunDisSize = sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING);

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID **)&SmmVariable
                    );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "[%a:%d] Status:%r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }
  Status = SmmVariable->SmmGetVariable (
                          H2OUVE_VARSTORE_NAME,
                          &gH2OUveVarstoreGuid,
                          NULL,
                          &VarEditFunDisSize,
                          &VarEditFunDis
                          );

  if ( VarEditFunDisSize != sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING)) {
    DEBUG ((DEBUG_INFO, "Please check sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING) in Variable Store."));
  }
  if ( (!EFI_ERROR (Status)) &&
       (VarEditFunDis.VariableEditFunDis == TRUE) &&
       (VarEditFunDisSize == sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING))
     ) {
    DEBUG ((DEBUG_INFO, "[%a:%d] VariableEditor Function Disabled.\n", __FUNCTION__, __LINE__));
    return EFI_UNSUPPORTED;
  }

  UpdateVariableEditBootTypeInfo();

  Status = InstallH2OUveIhisiServices ();
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d] Fail to install the H2OUVE IHISI service.\n", __FUNCTION__, __LINE__));
    return Status;
  }

  return EFI_SUCCESS;
}

