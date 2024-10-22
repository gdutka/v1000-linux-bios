/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Initialization.
 *
 * Init IOAPIC/IOMMU/Misc NB features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*;********************************************************************************
;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************
;*/

#include "FchPlatform.h"
#include "SSFchTaskLauncher.h"
#define FILECODE PROC_FCH_SANDSTONE_SSINTERFACE_SSFCHINITS3_FILECODE

FCH_TASK_ENTRY *FchInitS3EarlyTasksTable[] = {
        &FchInitEnvHwAcpi,
        &FchInitEnvSpi,
        &FchInitEnvSd,
        &FchInitEnvUsbXhci,
        &FchInitEnvSata,
        &FchInitEnvXGbe,
        &FchInitEnvEmmc,
        &FchInitEnvAb,
        NULL
};


FCH_TASK_ENTRY *FchInitS3LateTasksTable[] = {
        &FchInitMidUsbXhci,
        &FchInitLateSpi,
        &FchInitLateSata,
        &FchInitLateHwAcpi,
        &FchInitLateEmmc,
        NULL
};


VOID
FchInitS3EarlyRestore (
  IN      FCH_DATA_BLOCK     *FchDataPtr
  );

VOID
FchInitS3LateRestore (
  IN      FCH_DATA_BLOCK     *FchDataPtr
  );
/*----------------------------------------------------------------------------------------*/
/**
 * FchInitS3EarlyRestore - Config Fch before ACPI S3 resume PCI config device restore
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */

VOID
FchInitS3EarlyRestore (
  IN      FCH_DATA_BLOCK     *FchDataPtr
  )
{
  FchDataPtr->Misc.S3Resume = 1;
  FchTaskLauncher (&FchInitS3EarlyTasksTable[0], FchDataPtr, TpFchInitS3EarlyDispatching);
  FchDataPtr->Misc.S3Resume = 0;
  AGESA_TESTPOINT (TpFchInitS3EarlyDispatchingEnd, NULL);
}

/*----------------------------------------------------------------------------------------*/
/**
 * FchInitS3LateRestore - Config Fch after ACPI S3 resume PCI config device restore
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */

VOID
FchInitS3LateRestore (
  IN      FCH_DATA_BLOCK     *FchDataPtr
  )
{
  FchDataPtr->Misc.S3Resume = 1;
  FchTaskLauncher (&FchInitS3LateTasksTable[0], FchDataPtr, TpFchInitS3LateDispatching);
  FchDataPtr->Misc.S3Resume = 0;
  AGESA_TESTPOINT (TpFchInitS3LateDispatchingEnd, NULL);
}


