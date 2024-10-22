/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init HwAcpi Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"
#include "FchAoacLinkListData.h"
#define FILECODE PROC_FCH_SANDSTONE_SSHWACPI_SSHWACPIENV_FILECODE

extern VOID  ProgramEnvPFchAcpiMmio            (IN VOID  *FchDataPtr);
extern VOID  ProgramFchEnvHwAcpiPciReg         (IN VOID  *FchDataPtr);
extern VOID  ProgramSpecificFchInitEnvAcpiMmio (IN VOID  *FchDataPtr);
extern VOID  ProgramFchEnvSpreadSpectrum       (IN VOID  *FchDataPtr);
extern VOID  ValidateFchVariant                (IN VOID  *FchDataPtr);

VOID
FchInitSmiEnv (
  IN  VOID     *FchDataPtr
  );
/**
 * FchInitEnvHwAcpiP - Config HwAcpi controller preliminary
 *   (Special)
 *   Acpi S3 resume won't execute this procedure (POST only)
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvHwAcpiP (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  AGESA_TESTPOINT (TpFchInitEnvHwAcpiP, NULL);

  RecordFchConfigPtr ( (UINT32) ((UINTN) (LocalCfgPtr)));

  ProgramEnvPFchAcpiMmio (FchDataPtr);

  ProgramFchEnvClkCntl (FchDataPtr);

  ProgramFchEnvSpreadSpectrum (FchDataPtr);

  ProgramFchEnvAoacInit (FchDataPtr);
}

/**
 * FchInitEnvHwAcpi - Config HwAcpi controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvHwAcpi (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  AGESA_TESTPOINT (TpFchInitEnvHwAcpi, NULL);

  ProgramFchEnvHwAcpiPciReg (FchDataPtr);
  //
  // FCH Specific Function programming
  //
  ProgramSpecificFchInitEnvAcpiMmio (FchDataPtr);
  HpetInit (LocalCfgPtr);
  FchAl2ahbInit (LocalCfgPtr);
  FchI2cUartInit (LocalCfgPtr);
}

VOID
FchInitSmiEnv (
  IN  VOID     *FchDataPtr
  )
{
  UINT16                  FchSmiData;
  UINT8                   SwSmiCmdAddress;
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  FchSmiData = (FCH_SMI_DATA_POST_ENV << 8) | FCH_AOAC_SW_VALUE;
  SwSmiCmdAddress = ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A);
  LibAmdIoWrite (AccessWidth16, SwSmiCmdAddress, &FchSmiData, LocalCfgPtr->StdHeader);

}

