/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric watchdog timer library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <AMD.h>
#include <FabricRegistersDf3.h>
#include <Library/IdsLib.h>
#include <Library/FabricWdtLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_FABRICWDTDF3LIB_FABRICWDTDF3LIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
UINT32 Df3WdtCntSelDecode [] =
{
  31,
  127,
  255,
  511,
  1023,
  2047,
  4095,
  16383
};
#define NumberOfDfWdtCntSel (sizeof (Df3WdtCntSelDecode) / sizeof (Df3WdtCntSelDecode[0]))

UINT64 Df3WdtBaseSelDecode [] =
{
  1310000,
  1280,
  10,
  0
};
#define NumberOfDfWdtBaseSel (sizeof (Df3WdtBaseSelDecode) / sizeof (Df3WdtBaseSelDecode[0]))

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  FabricGetCcmWdtInfo
 *
 *  @param[in, out] DfCcmTimeout        DF CCM WDT time in ns
 *
 *  @retval TRUE  WDT is enabled
 *  @retval FALSE WDT is disabled
 *
 */
BOOLEAN
FabricGetCcmWdtInfo (
  IN       UINT64 *DfCcmTimeout
  )
{
  BOOLEAN                DfWdtEn;
  DF_GLBL_CTRL_REGISTER  DfGlblCtrl;

  DfGlblCtrl.Value = FabricRegisterAccRead (0, 0, DF_GLBL_CTRL_FUNC, DF_GLBL_CTRL_REG, FABRIC_REG_ACC_BC);
  ASSERT (DfGlblCtrl.Field.WDTBaseSel < NumberOfDfWdtBaseSel);
  ASSERT (DfGlblCtrl.Field.CCMWDTCntSel < NumberOfDfWdtCntSel);
  if (DfGlblCtrl.Field.WDTBaseSel < 3) {
    DfWdtEn = TRUE;
    *DfCcmTimeout = (UINT64) (Df3WdtBaseSelDecode[DfGlblCtrl.Field.WDTBaseSel] * Df3WdtCntSelDecode[DfGlblCtrl.Field.CCMWDTCntSel]);
  } else {
    DfWdtEn = FALSE;
    *DfCcmTimeout = 0;
  }
  return DfWdtEn;
}

