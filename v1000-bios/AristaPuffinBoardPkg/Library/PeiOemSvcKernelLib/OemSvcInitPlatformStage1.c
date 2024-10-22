/** @file
  Project dependent initial code in PlatformStage1.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/PeiOemSvcKernelLib.h>
#include <Library/IoLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include "SetupConfig.h"
#include <Library/EcLib.h>
#include <Library/PeiServicesLib.h>
#include "Fch.h"
//>>EnableAbove4GBMmio++
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
//<<EnableAbove4GBMmio++

#define TIEC_RAM_GPO2_OFFSET           0xAC

EFI_STATUS
OemEcAcDcSwitch (
  IN SYSTEM_CONFIGURATION                  *SystemConfiguration
  )
{
  if (!FeaturePcdGet (PcdEmulationOnly)) {  //EMULATION_ONLY
    EcAcDcSwitch (SystemConfiguration->amdAcDcSwitch, SystemConfiguration->amdDcTimer, SystemConfiguration->amdAcTimer );
  }  //EMULATION_ONLY
  return EFI_SUCCESS;
}

/**
  Project dependent initial code in PlatformStage1.

  @param  Base on OEM design.

  @retval EFI_UNSUPPORTED    Returns unsupported by default.
  @retval EFI_SUCCESS        The service is customized in the project.
  @retval EFI_MEDIA_CHANGED  The value of IN OUT parameter is changed.
  @retval Others             Depends on customization.
**/
EFI_STATUS
OemSvcInitPlatformStage1 (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *Variable;
//[-start-230504-IB14860915-modify]//
//  SYSTEM_CONFIGURATION                  *SystemConfiguration = NULL;
  SYSTEM_CONFIGURATION                  SystemConfiguration = {0};
//[-end-230504-IB14860915-modify]//
  UINTN                                 VariableSize;
  BOOLEAN                               SetupVariableExist;

  SetupVariableExist = FALSE;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &Variable
             );
  if (!EFI_ERROR (Status)) {
    VariableSize = sizeof (SYSTEM_CONFIGURATION);
//[-start-230504-IB14860915-modify]//
//    Status = Variable->GetVariable (
//                         Variable,
//                         SETUP_VARIABLE_NAME,
//                         &gSystemConfigurationGuid,
//                         NULL,
//                         &VariableSize,
//                         SystemConfiguration
//                         );
    Status = Variable->GetVariable (
                         Variable,
                         SETUP_VARIABLE_NAME,
                         &gSystemConfigurationGuid,
                         NULL,
                         &VariableSize,
                         &SystemConfiguration
                         );
//[-end-230504-IB14860915-modify]//
  }
  if (!EFI_ERROR (Status)) {
    SetupVariableExist = TRUE;
  }


 // OemEcAcDcSwitch (&SystemConfiguration);

//>>EnableAbove4GBMmio++
   PcdSet8 (PcdMmioMem64Enable, 0x00);
   if (FeaturePcdGet (Pcd64BitAmlSupported)) {
     if (SetupVariableExist) {
//[-start-230504-IB14860915-modify]//
//       if(SystemConfiguration->EnableAbove4GBMmio == 1) {
//         if(SystemConfiguration->BootType == 2) {
       if(SystemConfiguration.EnableAbove4GBMmio == 1) {
         if(SystemConfiguration.BootType == 2) {
//[-end-230504-IB14860915-modify]//
           PcdSet8 (PcdMmioMem64Enable, 0x01);
         }
       }
     }
   }
//<<EnableAbove4GBMmio++

  return EFI_SUCCESS;
}
