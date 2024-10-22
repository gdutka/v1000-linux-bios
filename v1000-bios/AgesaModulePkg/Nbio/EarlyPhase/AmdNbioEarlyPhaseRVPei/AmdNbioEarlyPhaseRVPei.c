/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioEarlyPhaseRVPei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioEarlyPhaseRVPei
 * @e \$Revision: 312065 $   @e \$Date: 2018-08-10 10:09:05 +0800 (Fri, 10 Aug 2018) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <AMD.h>
#include <Filecode.h>
#include <PiPei.h>
#include <GnbDxio.h>
#include <Library/NbioSmuV10Lib.h>
#include <Library/PcdLib.h>

#define FILECODE  NBIO_EARLYPHASE_AMDNBIOEARLYPHASERVPEI_AMDNBIOEARLYPHASERVPEI_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */




/*----------------------------------------------------------------------------------------*/
/**
 * AmdNbioEarlyPhase driver entry point for RV
 *
 *
 *
 * @param[in]  FileHandle  Standard configuration header
 * @param[in]  PeiServices Pointer to EFI_PEI_SERVICES pointer
 * @retval     EFI_STATUS
 */

EFI_STATUS
EFIAPI
AmdNbioEarlyPhaseRVPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  PCI_ADDR      NbioPciAddress;
  UINT32        SmuArg[6];

  AGESA_TESTPOINT (TpEarlyEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioEarlyPhaseRVPeiEntry Entry\n");


  NbioSmuServiceCommonInitArgumentsV10 (SmuArg);
  NbioPciAddress.AddressValue = 0;
  if (NbioSmuServiceRequestV10 (NbioPciAddress, 0x51, SmuArg,0)) {  // SMC_MSG_GetOpnSpareFuse
    IDS_HDT_CONSOLE (NB_MISC, "  SmuArg 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", SmuArg[0], SmuArg[1], SmuArg[2], SmuArg[3], SmuArg[4], SmuArg[5]);
  }

  PcdSet32 (PcdGetRVFamilyOPN, SmuArg[0]);

  SmuArg[0] = SmuArg[0] >> 30;
  IDS_HDT_CONSOLE (NB_MISC, "SmuArg 0x%x\n", SmuArg[0]);
  if (SmuArg[0] == 2 || SmuArg[0] == 3) {
    PcdSetBool (PcdPCIeExactMatchEnable, TRUE);
    IDS_HDT_CONSOLE (MAIN_FLOW, "Enable Exact Match\n");
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioEarlyPhaseRVPeiEntry Exit\n");
  AGESA_TESTPOINT (TpEarlyExit, NULL);
  return EFI_SUCCESS;
}

