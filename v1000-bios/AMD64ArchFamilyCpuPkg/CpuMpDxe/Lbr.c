/** @file

  Code for LBR (last branch record) feature.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Lbr.h"

BOOLEAN    EnableLbr[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of LBR for specified processor.
  
  This function detects capability of LBR for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
LbrDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32                FamilyId;
  UINT32                ModelId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

  //
  // LBR feature is supported after family 10h processors
  //
  if (FamilyId > 0x0F) {
    SetProcessorFeatureCapability (ProcessorNumber, Lbr, NULL);
  } else {
    ClearProcessorFeatureCapability (ProcessorNumber, Lbr);
  }
}

/**
  Configures Processor Feature Lists for LBR for all processors.
  
  This function configures Processor Feature Lists for LBR for all processors.

**/
VOID
LbrConfigFeatureList (
  VOID
  )
{
  UINTN                ProcessorNumber;
  BOOLEAN              UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_LBR_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports LBR
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, Lbr, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of machine check capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_LBR_BIT);
        //
        // Set the bit of machine check setting according to BSP setting
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_LBR_BIT);
        }
      }

      //
      // If this logical processor supports LBR, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableLbr[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, Lbr, &(EnableLbr[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of LBR in Register Table for specified processor.
  
  This function produces entry of LBR in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Machine check Attribute.

**/
VOID
LbrReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;
  UINT32     FamilyId;
  UINT32     ModelId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);
  
  //
  // If Attribute is TRUE, then write 1 to MSR_IA32_DBG_CTL[0].
  // Otherwise, write 0 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 0;
  if (*Enable) {
    Value = 1;
  }

  WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_DBG_CTL, N_MSR_DBG_CTL_LBR_ENABLE, 1, Value);
}
