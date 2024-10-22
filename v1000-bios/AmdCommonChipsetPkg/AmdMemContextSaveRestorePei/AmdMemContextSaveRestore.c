/** @file
  A PEIM for AMD memory context save and restore feature.

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include <PiPei.h>

#include <Guid/AmdMemoryContextSaveRestoreData.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/H2OPeiChipsetServices.h>
#include <Ppi/AmdMemContextSaveRestore.h>

#include <Library/PeiServicesLib.h>
#include <Library/BasememoryLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/CmosLib.h>
#include <Library/AmdExtCmosLib.h>
#include <Library/PciExpressLib.h>

#define RTC_ADDRESS_DAY_OF_THE_MONTH  7   // R/W  Range 1..31
#define RTC_ADDRESS_MONTH             8   // R/W  Range 1..12
#define RTC_ADDRESS_YEAR              9   // R/W  Range 0..99

BOOLEAN
PeiGetMemContextPolicy (
  IN  EFI_PEI_SERVICES   **PeiServices
  );

AMD_MEM_CONTEXT_SAVE_RESTORE_PPI   mAmdMemContextSaveRestorePpi = {
  PeiGetMemContextPolicy
};

//
// Ppis to be installed
//
EFI_PEI_PPI_DESCRIPTOR           mPpiList[] = { 
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmdMemContextSaveRestorePpiGuid,
    &mAmdMemContextSaveRestorePpi
  }
};

/**
  Converts BCD value to decimal value.

  @param [in]  BcdValue             The BCD value converted.

  @retval      UINT8                The decimal value which has been converted to decimal.

**/
UINT8
BcdToDecimal (
  IN  UINT8 BcdValue
  )
{
  UINTN High;
  UINTN Low;

  High  = BcdValue >> 4;
  Low   = BcdValue - (High << 4);

  return (UINT8) (Low + (High * 10));
}

/**
  Checks is this year is a leap year or not.

  @param [in]  Year                 The year needs to be checked.

  @retval BOOLEAN                   This year is leap year or not.

**/
BOOLEAN
IsLeapYear (
  UINT16  Year
  )
{
  if (Year % 4 == 0 && Year % 100 != 0 && Year % 400 == 0) {
    return TRUE;
  }

  return FALSE;
}

/**
  Calculates days.

  @param [in]  StartDay             StartDay EFI_TIME data.
  @param [in]  EndDay               EndDay EFI_TIME data.

  @retval UINTN                     TotalDays.

**/
UINTN
CalculateDays (
  EFI_TIME  StartDay,
  EFI_TIME  EndDay
  )
{
  UINTN    DurationDays;
  UINTN    TotalDays;
  UINT16   Index;
  UINT8    MonthDays[13] = {31,28,31,30,31,30,31,31,30,31,30,31};

  TotalDays = 0;
  for (Index = StartDay.Year; Index < EndDay.Year; Index++) {
    if (IsLeapYear(Index)) {
      TotalDays += 366;
    } else {
      TotalDays += 365;
    }
  }

  DurationDays = 0;
  for (Index = 1; Index < StartDay.Month; Index++) {
    DurationDays += MonthDays[Index];
  }

  DurationDays += StartDay.Day;

  if (IsLeapYear (StartDay.Year) && StartDay.Month > 2) {
    DurationDays++;
  }

  for (Index = 1; Index < EndDay.Month; Index++) {
    TotalDays += MonthDays[Index];
  }

  TotalDays += EndDay.Day;

  if (IsLeapYear (EndDay.Year) && EndDay.Month > 2) {
    TotalDays++;
  }

  TotalDays -= DurationDays;

  return TotalDays;
}

/**
  Gets periodic retraining policy.

  @param [in]  PeiServices          Describes the list of possible PEI Services.

  @retval BOOLEAN                   To do periodic retraining or not.

**/
BOOLEAN
GetPeriodicRetrainingPolicy (
  IN     CONST EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS                       Status;
  UINTN                            VariableSize;
  EFI_TIME                         CurrentTime;
  EFI_TIME                         LastTrainingTime;
  BOOLEAN                          DoPeriodicRetrain;
  UINTN                            DurationDays;

  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *Variable2Ppi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &Variable2Ppi
             );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }

  ZeroMem (&LastTrainingTime, sizeof (EFI_TIME));
  VariableSize = sizeof (EFI_TIME);

  Status = Variable2Ppi->GetVariable (
                           Variable2Ppi,
                           AMD_MEMORY_CONTEXT_LAST_TRAINING_TIME_NAME,
                           &gAmdMemContextSaveRestoreVariableGuid,
                           NULL,
                           &VariableSize,
                           &LastTrainingTime
                           );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }

  (**PeiServices).SetMem (&CurrentTime, sizeof (EFI_TIME), 0);
  CurrentTime.Year = BcdToDecimal (ReadCmos8 (RTC_ADDRESS_YEAR));
  CurrentTime.Year += 2000;
  CurrentTime.Month = BcdToDecimal (ReadCmos8 (RTC_ADDRESS_MONTH));
  CurrentTime.Day = BcdToDecimal (ReadCmos8 (RTC_ADDRESS_DAY_OF_THE_MONTH));

  DoPeriodicRetrain = FALSE;
  if ( (LastTrainingTime.Year > CurrentTime.Year) ||
      ((LastTrainingTime.Year == CurrentTime.Year) && (LastTrainingTime.Month > CurrentTime.Month)) ||
      ((LastTrainingTime.Year == CurrentTime.Year) && (LastTrainingTime.Month == CurrentTime.Month) && (LastTrainingTime.Day > CurrentTime.Day))) {
    DoPeriodicRetrain = TRUE;
  }

  if (!DoPeriodicRetrain) {
    DurationDays = CalculateDays (LastTrainingTime, CurrentTime);

    if (DurationDays >= 30) {
      DoPeriodicRetrain = TRUE;
    }
  }

  return DoPeriodicRetrain;
}

BOOLEAN
PeiGetMemContextPolicy (
  IN  EFI_PEI_SERVICES   **PeiServices
  )
{
  EFI_STATUS  Status;
  EFI_PEI_HOB_POINTERS  Hob;
  AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA  *MemoryStatusData;
  
  MemoryStatusData = NULL;
  Status = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION &&
        CompareGuid ( &Hob.Guid->Name, &gAmdMemoryContextSaveRestoreDataGuid)) {
      MemoryStatusData = (AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA *)(Hob.Raw +
                      sizeof (EFI_HOB_GENERIC_HEADER) +
                      sizeof (EFI_GUID));
      break;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  if (MemoryStatusData == NULL) {
    return FALSE;
  }

  return MemoryStatusData->DoMemContextSaveRestore;
}

/**
  PeiInitAmdMemContextSaveRestoreEntryPoint.

  @param [in]  FileHandle           Handle of the file being invoked.
  @param [in]  PeiServices          Describes the list of possible PEI Services.

  @retval EFI_SUCCESS               The driver installed without error.

**/
EFI_STATUS
EFIAPI
PeiInitAmdMemContextSaveRestoreEntryPoint (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                            Status;
  UINT8                                 CmosData8;
  UINT16                                Data16;
  BOOLEAN                               MemContextExist;
  UINTN                                 Size;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *Variable2Ppi;
  AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA  *MemoryStatusData;
  UINT8                                 SleepState;
  H2O_CHIPSET_SERVICES_PPI              *ChipsetSvcPpi;

  //
  // Initializea the memory status data structure.
  //
  Status = PeiServicesAllocatePool (
             sizeof (AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA),
             &MemoryStatusData
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  ZeroMem (MemoryStatusData, sizeof (AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA));

  //
  // 2.1 Cold boot detection.
  //
  Data16 = PciExpressRead16 (
             PCI_EXPRESS_LIB_ADDRESS (
               0,
               0x18,
               0,
               0x6C
               )
             );
  if ((Data16 & BIT4) != 0) {  //ColdRstDet == 1, lase reset is warm reset
    MemoryStatusData->IsSystemCfgGood = TRUE;
    MemoryStatusData->IsWarmBoot = TRUE;
  } else {                     //ColdRstDet == 0, last reset is cold reset
    MemoryStatusData->IsSystemCfgGood = FALSE;
    MemoryStatusData->IsWarmBoot = FALSE;

    //
    // Sets ColdRstDet = 1.
    //
    PciExpressOr16 (
      PCI_EXPRESS_LIB_ADDRESS (
        0,
        0x18,
        0,
        0x6C
        ),
      (UINT16) BIT4
      );
  }

  //
  // 2.2 S4 detection.
  //
  Status = PeiServicesLocatePpi (
             &gH2OChipsetServicesPpiGuid,
             0,
             NULL,
             (VOID **)&ChipsetSvcPpi
             );

  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "H2O PEI Chipset Services can not be found!\n"));
    return EFI_UNSUPPORTED;
  }

  ChipsetSvcPpi->GetSleepState (&SleepState);

  if (SleepState == 4) {
    MemoryStatusData->IsS4Resume = TRUE;
  }

  //
  // 2.4 MemContext detection.
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &Variable2Ppi
             );
  ASSERT_EFI_ERROR (Status);

  MemContextExist = FALSE;
  Size = sizeof (BOOLEAN);
  Status = Variable2Ppi->GetVariable (
                           Variable2Ppi,
                           AMD_MEMORY_CONTEXT_DATA_EXIST_VARIABLE_NAME,
                           &gAmdMemContextSaveRestoreVariableGuid,
                           NULL,
                           &Size,
                           &MemContextExist
                           );
  if (!EFI_ERROR(Status) && (MemContextExist == TRUE)) {
    MemoryStatusData->HasMemContext = TRUE;
  }

  //
  // 2.3 Last boot status detection.
  // 2.5 Setup Options changed detection.
  //
  CmosData8 = AmdReadExtCmos8 (
                PcdGet8 (CmosAmdMemoryContextSaveRestore)
                );
  AmdWriteExtCmos8 (
    PcdGet8 (CmosAmdMemoryContextSaveRestore),
    0
    );

  if ((CmosData8 & PcdGet8 (PostPassFlag)) != 0) { //POST reach readytoboot event.
    MemoryStatusData->IsPostPass = TRUE;
  }
 
  if ((CmosData8 & PcdGet8 (SetupChangedFlag)) == 0) { //Setup variable is no changed.
    MemoryStatusData->IsSetupNoChange = TRUE;
  }

  //
  // TODO:
  // 2.6 Periodic retraining.
  //
  MemoryStatusData->DoPeriodicRetraining = GetPeriodicRetrainingPolicy (PeiServices);

  //
  // Builds memory context save restore data into Hob.
  //
  if (((MemoryStatusData->IsWarmBoot == TRUE) ||      // 2.1 Cold boot detection
      (MemoryStatusData->IsS4Resume == TRUE)) &&      // 2.2 S4 detection
      (MemoryStatusData->IsPostPass == TRUE) &&       // 2.3 Last boot status detection
      (MemoryStatusData->HasMemContext == TRUE) &&    // 2.4 MemContext detection
      (MemoryStatusData->IsSetupNoChange == TRUE) &&  // 2.5 Setup Options changed detection
      (MemoryStatusData->DoPeriodicRetraining == FALSE)) {  // 2.6 Periodic retraining
    MemoryStatusData->DoMemContextSaveRestore = TRUE;
  }

  BuildGuidDataHob (
    &gAmdMemoryContextSaveRestoreDataGuid,
    MemoryStatusData,
    sizeof (AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA)
    );

  Status = PeiServicesInstallPpi (&mPpiList[0]);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
