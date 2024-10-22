/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH PEIM
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH PEIM
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */

/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "FchReset2.h"

UINT16
ReadPmio16 (
  IN       UINT8        Index
  )
{
  UINT8    bTemp;

  IoWrite8 (FCH_RESET_REGCD6, Index);
  bTemp = IoRead8 (FCH_RESET_REGCD7);
  IoWrite8 (FCH_RESET_REGCD6, Index + 1);
  return (UINT16) ((IoRead8 (FCH_RESET_REGCD7) << 8) + bTemp);
}

/*********************************************************************************
 * Name: FchPeiReset2
 *
 * Description
 *   Resets the entire platform.
 *
 * Input
 *   ResetType   - warm or cold
 *   ResetStatus - possible cause of reset
 *   DataSize    - Size of ResetData in bytes
 *   ResetData   - Optional Unicode string
 *  For details, see PiPeiCis.h
 *
 * Output
 *   none if the reset is successful
 *
 *********************************************************************************/
VOID
EFIAPI
FchPeiReset2 (
  IN EFI_RESET_TYPE     ResetType,
  IN EFI_STATUS         ResetStatus,
  IN UINTN              DataSize,
  IN VOID               *ResetData OPTIONAL
  )
{
  UINT8      InitialData;
  UINT8      OutputData;
  UINT8      PwrRsrCfg;
  UINT16     AcpiGpeBase;
  UINT16     AcpiPm1StsBase;
  UINT16     AcpiPm1CntBase;
  UINT32     Gpe0Enable;
  UINT16     PmCntl;
  UINT16     PwrSts;

  DEBUG ((DEBUG_INFO, "[FchPeiReset2] FchPeiReset2 invoked:  ResetType = %d\n", ResetType));
  switch (ResetType) {
  case EfiResetWarm:
    InitialData = HARDSTARTSTATE;
    OutputData  = HARDRESET;
    IoWrite8 (FCH_RESET_REGCD6, 0x10);
    PwrRsrCfg = IoRead8 (FCH_RESET_REGCD7);
    PwrRsrCfg = PwrRsrCfg & 0xFD; //clear ToggleAllPwrGoodOnCf9
    IoWrite8 (FCH_RESET_REGCD7, PwrRsrCfg);
    break;

  case EfiResetCold:
    InitialData = HARDSTARTSTATE;
    OutputData  = HARDRESET;
    IoWrite8 (FCH_RESET_REGCD6, 0x10);
    PwrRsrCfg = IoRead8 (FCH_RESET_REGCD7);
    PwrRsrCfg = PwrRsrCfg | BIT1; //set ToggleAllPwrGoodOnCf9
    IoWrite8 (FCH_RESET_REGCD7, PwrRsrCfg);
    break;

  case EfiResetShutdown:
    // Disable all GPE0 Event
    AcpiGpeBase = ReadPmio16 (FCH_RESET_PMIOA_REG68);
    AcpiGpeBase += 4; //Get enable base
    Gpe0Enable  = 0;
    IoWrite32 (AcpiGpeBase, Gpe0Enable);

    // Clear Power Button status.
    AcpiPm1StsBase = ReadPmio16 (FCH_RESET_PMIOA_REG60);
    PwrSts  = BIT8 | BIT15; //Clear WakeStatus with PwrBtnStatus
    IoWrite16 (AcpiPm1StsBase, PwrSts);

    // Transform system into S5 sleep state
    AcpiPm1CntBase = ReadPmio16 (FCH_RESET_PMIOA_REG62);
    PmCntl  = IoRead16 (AcpiPm1CntBase);
    PmCntl  = (PmCntl & ~SLP_TYPE) | SUS_S5 | SLP_EN;
    IoWrite16 (AcpiPm1CntBase, PmCntl);
    FCH_DEADLOOP();
    return ;

  default:
    return ;
  }

  IoWrite8 (FCH_RESET_REGCF9, InitialData);
  IoWrite8 (FCH_RESET_REGCF9, OutputData);

  //
  // Given we should have reset getting here would be bad
  //
  FCH_DEADLOOP();
}

