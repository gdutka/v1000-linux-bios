/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific GFX library
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309572 $   @e \$Date: 2014-12-17 16:39:45 +0800 (Wed, 17 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbGfx.h"
#include  "GfxLibCZ.h"
#include  "GnbCommonLib.h"
#include  "GnbRegisterAccCZ.h"
#include  "GnbRegistersCZ.h"
#include  "Library/AmdHeapLib.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBINITCZ_GFXLIBCZ_FILECODE
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

/*----------------------------------------------------------------------------------------*/
/**
 * Disable GFX controller
 *
 *
 *
 * @param[in] StdHeader  Standard configuration header
 */

VOID
GfxDisableControllerCZ (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  GnbLibPciRMW (
    MAKE_SBDFO (0, 0, 0, 0,D0F0x7C_ADDRESS),
    AccessS3SaveWidth32,
    0xffffffff,
    1 << D0F0x7C_ForceIntGfxDisable_OFFSET,
    StdHeader
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get system PLL COF
 *
 *
 *
 * @param[in] StdHeader  Standard configuration header
 * @retval    System PLL COF
 */
UINT32
GfxLibGetSystemPllCofCZ (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  D0F0xBC_xC001400C_STRUCT  D0F0xBC_xC001400C;
  GnbRegisterReadCZ (GnbGetHandle (StdHeader), D0F0xBC_xC001400C_TYPE,  D0F0xBC_xC001400C_ADDRESS, &D0F0xBC_xC001400C.Value, 0, StdHeader);
  return 100 * (D0F0xBC_xC001400C.Field.MainPllOpFreqIdStartup + 0x10);

}

/*----------------------------------------------------------------------------------------*/
/**
 * Calculate COF for DFS out of Main PLL
 *
 *
 *
 * @param[in] Did         Did
 * @param[in] StdHeader   Standard Configuration Header
 * @retval                COF in 10khz
 */

UINT32
GfxCalculateClockCZ (
  IN       UINT8                       Did,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  UINT32  Divider;
  UINT32  SystemPllCof;
  SystemPllCof = GfxLibGetSystemPllCofCZ (StdHeader) * 100;
  if (Did >= 8 && Did <= 0x3F) {
    Divider = Did * 25;
  } else if (Did > 0x3F && Did <= 0x5F) {
    Divider = (Did - 64) * 50 + 1600;
  } else if (Did > 0x5F && Did <= 0x7E) {
    Divider = (Did - 96) * 100 + 3200;
  } else if (Did == 0x7f) {
    Divider = 128 * 100;
  } else {
///@todo    ASSERT (FALSE);
    return 200 * 100;
  }
  ASSERT (Divider != 0);
  return (((SystemPllCof * 100) + (Divider - 1)) / Divider);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if Video BIOS has posted or not
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 */

BOOLEAN
GfxIsVbiosPostedCZ (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GMMx1740_STRUCT GMMx1740;
  GnbRegisterReadCZ (GnbGetHandle (GnbLibGetHeader (Gfx)), GMMx1740_TYPE, GMMx1740_ADDRESS, &GMMx1740.Value, 0, GnbLibGetHeader (Gfx));

  return ((GMMx1740.Value & ATOM_S7_ASIC_INIT_COMPLETEb1) != 0) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------------------*/
/**
 * This function will add HSA Processing Unit entry for Gnb.
 *
 *    @param[in, out] StdHeader            Standard Head Pointer
 *    @retval         AGESA_ERROR          Crat table entry is not created successfully
 *    @retval         AGESA_SUCCESS        Crat table entry is created successfully
 */
AGESA_STATUS
GfxMakeHSAProcUnitEntryCZ (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  CRAT_HSA_PROCESSING_UNIT   *HsaProcUnit;

  IDS_HDT_CONSOLE (GNB_TRACE, "Make HSA Proc entry\n");

  HsaProcUnit = (CRAT_HSA_PROCESSING_UNIT *) GnbAllocateHeapBuffer (AMD_GNB_CRAT_HSA_TABLE_HANDLE, sizeof (CRAT_HSA_PROCESSING_UNIT), StdHeader);
  if (HsaProcUnit == NULL) {
    return AGESA_ERROR;
  }
  HsaProcUnit->Length = 40;
  HsaProcUnit->Flags.Enabled = 1;
  HsaProcUnit->Flags.GpuPresent = 1;
  HsaProcUnit->Flags.CpuPresent = 0;
  HsaProcUnit->Flags.IommuPresent = 1;
  HsaProcUnit->ProcessorIdLow = 0x80000000;
  HsaProcUnit->NumSIMDPerCU = 4;
  HsaProcUnit->NumSIMDCores = GPU__GC__NUM_SE * GPU__GC__NUM_SH_PER_SE * GPU__GC__NUM_CU_PER_SH * (HsaProcUnit->NumSIMDPerCU);
  HsaProcUnit->MaxWavesSIMD = 40;
  HsaProcUnit->LDSSizeInKB = 64;
  HsaProcUnit->WaveFrontSize = 64;
  HsaProcUnit->NumBanks = GPU__GC__NUM_SE;
  HsaProcUnit->NumArrays = GPU__GC__NUM_SH_PER_SE;
  HsaProcUnit->NumCUPerArray = GPU__GC__NUM_SH_PER_SE * GPU__GC__NUM_CU_PER_SH;
  HsaProcUnit->MaxSlotsScratchCU = 32;

  IDS_HDT_CONSOLE (GNB_TRACE, "Make HSA Proc exit\n");
  return AGESA_SUCCESS;
}

