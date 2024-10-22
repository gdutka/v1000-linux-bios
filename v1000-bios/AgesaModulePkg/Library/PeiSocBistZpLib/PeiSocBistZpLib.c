/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD SOC BIST Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "Uefi.h"
#include <Library/BaseLib.h>
#include "AMD.h"
#include "Filecode.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Ppi/NbioSmuServicesPpi.h>

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_PEISOCBISTZPLIB_PEISOCBISTZPLIB_FILECODE

 /*---------------------------------------------------------------------------------------*/
 /**
  *
  * Reads the status of CCX BIST
  *
  * @param[in]      CcxNumber              Ccx number to read BIST data from
  * @param[in]      SystemDieCount         System based die number
  * @param[in]      PeiServices            Pointer to PEI services
  *
  *
  * @retval         AGESA_ALERT            A BIST error has occurred
  * @retval         AGESA_SUCCESS          No BIST errors have occured
  */
AGESA_STATUS
ReadCcxBistData (
  IN       UINTN            CcxNumber,
  IN       UINTN            SystemDieCount,
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  UINT8                         BistEn;
  UINT8                         ColdResetMBistEn;
  UINT8                         BistDone;
  UINT8                         BistPF;
  UINT32                        BistStatus;
  UINT32                        BistEnFuse;
  UINT32                        ColdResetMBistEnFuse;
  EFI_STATUS                    CalledStatus;
  PEI_AMD_NBIO_SMU_SERVICES_PPI *NbioSmuServices;


  CalledStatus = (**PeiServices).LocatePpi (
                                 PeiServices,
                                 &gAmdNbioSmuServicesPpiGuid,
                                 0,
                                 NULL,
                                 &NbioSmuServices
                                 );

  ASSERT (CalledStatus == EFI_SUCCESS);

  NbioSmuServices->SmuRegisterRead (NbioSmuServices, SystemDieCount, 0x5D0CC, &BistEnFuse);
  BistEn = (BistEnFuse >> 4) & 0x1;

  NbioSmuServices->SmuRegisterRead (NbioSmuServices, SystemDieCount, 0x5D0DC, &ColdResetMBistEnFuse);
  ColdResetMBistEn = (ColdResetMBistEnFuse >> 25) & 0x1;

  // Check BIST status only if BIST is fuse enabled
  if ((BistEn == 0x1) || (ColdResetMBistEn == 0x1)){
    NbioSmuServices->SmuRegisterRead (NbioSmuServices, SystemDieCount, 0x18080064 | (CcxNumber << 22), &BistStatus);

    BistDone = (BistStatus >> 9) & 0x1;
    if (BistDone == 0x1) {
      BistPF = (BistStatus >> 8) & 0x1;
      if (BistPF == 0) {
        return AGESA_ALERT;
      }
    }
  }
  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
 /**
  *
  * Reads the status of non-CCX BIST
  *
  * @param[in]      DieNumber              Die number to read data from
  * @param[out]     BistData               Value of BIST status register
  * @param[in]      PeiServices            Pointer to PEI services
  *
  * @retval         AGESA_ALERT            A BIST error has occurred
  * @retval         AGESA_SUCCESS          No BIST errors have occured
  */
AGESA_STATUS
ReadNonCcxBistData (
  IN       UINTN            DieNumber,
       OUT UINTN            *BistData,
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  UINT8                         RunMBist;
  UINT32                        RunMBistFuse;
  EFI_STATUS                    CalledStatus;
  PEI_AMD_NBIO_SMU_SERVICES_PPI *NbioSmuServices;

  CalledStatus = (**PeiServices).LocatePpi (
                                 PeiServices,
                                 &gAmdNbioSmuServicesPpiGuid,
                                 0,
                                 NULL,
                                 &NbioSmuServices
                                 );

  ASSERT (CalledStatus == EFI_SUCCESS);

  NbioSmuServices->SmuRegisterRead (NbioSmuServices, DieNumber, 0x5D258, &RunMBistFuse);
  RunMBist = (RunMBistFuse >> 6) & 0x1;

  if (RunMBist == 0x1) {
    NbioSmuServices->SmuRegisterRead (NbioSmuServices, DieNumber, 0x03810038, BistData);

    if (*BistData != 0) {
      return AGESA_ALERT;
    }
  }

  return AGESA_SUCCESS;
}
