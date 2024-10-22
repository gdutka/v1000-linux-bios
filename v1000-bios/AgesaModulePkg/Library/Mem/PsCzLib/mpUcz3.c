/* $NoKeywords:$ */
/**
 * @file
 *
 * mpUcz3.c
 *
 * Platform specific settings for CZ DDR3 UDIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/CZ)
 * @e \$Revision: 311790 $ @e \$Date: 2015-01-27 13:03:49 +0800 (Tue, 27 Jan 2015) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#include "AGESA.h"
#include "mport.h"
#include "ma.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_PSCZLIB_MPUCZ3_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */

//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqCZUDIMMD3[] = {
  {_1DIMM, 1, 1, 0, 0, DDR2133_FREQUENCY, DDR1866_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_1DIMM, 1, 0, 1, 0, DDR2133_FREQUENCY, DDR1866_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_2DIMM, 1, 1, 0, 0, DDR2133_FREQUENCY, DDR1866_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_2DIMM, 1, 0, 1, 0, DDR2133_FREQUENCY, DDR1866_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_2DIMM, 2, 2, 0, 0, DDR1866_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_2DIMM, 2, 1, 1, 0, DDR1866_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_2DIMM, 2, 0, 2, 0, DDR1866_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}
};
CONST PSC_TBL_ENTRY CZMaxFreqTblEntUD3 = {
   {PSCFG_MAXFREQ, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_ROUTING_ANY},
   sizeof (MaxFreqCZUDIMMD3) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqCZUDIMMD3
};

// CAD Bus configuration for UDIMM (DQS routed in tree)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,  Slow, AddrCmdTmg, CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusCZUDIMMTree3[] = {
  {1, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x003B0000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x003B0000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x00380000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x00360000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1866, V1_35 + V1_5        , DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1866, V1_35 + V1_5        , DIMM_DR, NP     , 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR2133, V1_5                , DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR2133, V1_5                , DIMM_DR, NP     , 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 0, 0x003B0000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x003B0000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 0, 0x003B0000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x003B0000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 0, 0x00380000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x00380000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, 0, 0x00350037, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, 0, 0x00350037, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, 0, 0x00350037, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, 0, 0x00350037, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 0, 0x00360000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x00360000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1600, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1600, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_SR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_DR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_SR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_DR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1866, V1_35 + V1_5        , NP     , DIMM_SR, 0, 0x003C3C3C, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1866, V1_35 + V1_5        , NP     , DIMM_DR, 1, 0x00003C3C, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1866, V1_35 + V1_5        , DIMM_SR, NP     , 0, 0x003C3C3C, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1866, V1_35 + V1_5        , DIMM_DR, NP     , 1, 0x00003C3C, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1866, V1_5                , DIMM_SR, DIMM_SR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1866, V1_5                , DIMM_DR, DIMM_DR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1866, V1_5                , DIMM_DR, DIMM_SR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1866, V1_5                , DIMM_SR, DIMM_DR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR2133, V1_5                , NP     , DIMM_SR, 0, 0x003B3B3B, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR2133, V1_5                , NP     , DIMM_DR, 1, 0x00003B3B, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR2133, V1_5                , DIMM_SR, NP     , 0, 0x003B3B3B, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR2133, V1_5                , DIMM_DR, NP     , 1, 0x00003B3B, 0x1F, 0x1F, 0x1F, 0x1F},
};
CONST PSC_TBL_ENTRY CZCadBusTblEnUTree3 = {
   {PSCFG_CADBUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (CadBusCZUDIMMTree3) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusCZUDIMMTree3
};

// Data Bus configuration for UDIMM (DQS routed in tree)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,   RttNom,  RttWr,   DQ,   DQS,  ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D3 ROMDATA DataBusCZUDIMMTree3[] = {
  {1, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {1, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {1, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {1, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {1, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x04},
  {1, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x04},
  {1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_60 , ODT_OFF, 0x70, 0x70, 0x05},
  {1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_60 , ODT_OFF, 0x70, 0x70, 0x05},
  {1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_60 , ODT_OFF, 0x70, 0x70, 0x0C},
  {1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {1, DDR1866, V1_35 + V1_5        , DIMM_SR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {1, DDR1866, V1_35 + V1_5        , DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {1, DDR2133, V1_5                , DIMM_SR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {1, DDR2133, V1_5                , DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR667 , V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR667 , V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x04},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x04},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x04},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x04},
  {2, DDR800 , V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR800 , V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x05},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x05},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x05},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x05},
  {2, DDR1066, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_120, ODT_OFF, 0x70, 0x70, 0x04},
  {2, DDR1066, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_120, ODT_OFF, 0x70, 0x70, 0x04},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x04},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x04},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_60 , ODT_OFF, 0x70, 0x70, 0x05},
  {2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_60 , ODT_OFF, 0x70, 0x70, 0x05},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_60 , ODT_OFF, 0x70, 0x70, 0x05},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_60 , ODT_OFF, 0x70, 0x70, 0x05},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1600, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1600, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1866, V1_35 + V1_5        , NP     , DIMM_SR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1866, V1_35 + V1_5        , NP     , DIMM_DR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1866, V1_35 + V1_5        , DIMM_SR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1866, V1_35 + V1_5        , DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1866, V1_5                , DIMM_SR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1866, V1_5                , DIMM_DR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1866, V1_5                , DIMM_DR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1866, V1_5                , DIMM_SR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR2133, V1_5                , NP     , DIMM_SR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR2133, V1_5                , NP     , DIMM_DR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR2133, V1_5                , DIMM_SR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR2133, V1_5                , DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
};
CONST PSC_TBL_ENTRY CZDataBusTblEnUTree3 = {
   {PSCFG_DATABUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (DataBusCZUDIMMTree3) / sizeof (PSCFG_DATABUS_ENTRY_D3),
   (VOID *)&DataBusCZUDIMMTree3
};

// CAD Bus configuration for UDIMM (DQS routed in daisy chain)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,  Slow, AddrCmdTmg, CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusCZUDIMMDC3[] = {
  {1, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x003B0000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x003B0000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x00380000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x00360000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1866, V1_35 + V1_5        , DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR1866, V1_35 + V1_5        , DIMM_DR, NP     , 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR2133, V1_5                , DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {1, DDR2133, V1_5                , DIMM_DR, NP     , 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 0, 0x003B0000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 0, 0x003B0000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, 0, 0x00390039, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 0, 0x00380000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, 0, 0x00350037, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, 0, 0x00350037, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, 0, 0x00350037, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, 0, 0x00350037, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 0, 0x00360000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_SR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_DR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_SR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_DR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1866, V1_35 + V1_5        , NP     , DIMM_SR, 0, 0x003C3C3C, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1866, V1_35 + V1_5        , NP     , DIMM_DR, 1, 0x00003C3C, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR1866, V1_5                , DIMM_SR, DIMM_SR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1866, V1_5                , DIMM_DR, DIMM_DR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1866, V1_5                , DIMM_DR, DIMM_SR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR1866, V1_5                , DIMM_SR, DIMM_DR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F},
  {2, DDR2133, V1_5                , NP     , DIMM_SR, 0, 0x003B3B3B, 0x1F, 0x1F, 0x1F, 0x1F},
  {2, DDR2133, V1_5                , NP     , DIMM_DR, 1, 0x00003B3B, 0x1F, 0x1F, 0x1F, 0x1F},
};
CONST PSC_TBL_ENTRY CZCadBusTblEnUDC3 = {
   {PSCFG_CADBUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (CadBusCZUDIMMDC3) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusCZUDIMMDC3
};

// Data Bus configuration for SO-DIMM (DQS routed in daisy chain)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,   RttNom,  RttWr,   DQ,   DQS,  ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D3 ROMDATA DataBusCZUDIMMDC3[] = {
  {1, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {1, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {1, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {1, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {1, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x04},
  {1, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_120, ODT_OFF, 0x70, 0x70, 0x04},
  {1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_60 , ODT_OFF, 0x70, 0x70, 0x05},
  {1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_60 , ODT_OFF, 0x70, 0x70, 0x05},
  {1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_60 , ODT_OFF, 0x70, 0x70, 0x0C},
  {1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {1, DDR1866, V1_35 + V1_5        , DIMM_SR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {1, DDR1866, V1_35 + V1_5        , DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {1, DDR2133, V1_5                , DIMM_SR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {1, DDR2133, V1_5                , DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR667 , V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR667 , V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x04},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x04},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x04},
  {2, DDR667 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x04},
  {2, DDR800 , V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR800 , V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_120, ODT_OFF, 0x70, 0x70, 0x01},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x05},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x05},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x05},
  {2, DDR800 , V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x05},
  {2, DDR1066, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_120, ODT_OFF, 0x70, 0x70, 0x04},
  {2, DDR1066, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_120, ODT_OFF, 0x70, 0x70, 0x04},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, ODT_40 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1066, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, ODT_40 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_60 , ODT_OFF, 0x70, 0x70, 0x05},
  {2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_60 , ODT_OFF, 0x70, 0x70, 0x05},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C},
  {2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1866, V1_35 + V1_5        , NP     , DIMM_SR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1866, V1_35 + V1_5        , NP     , DIMM_DR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR1866, V1_5                , DIMM_SR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1866, V1_5                , DIMM_DR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1866, V1_5                , DIMM_DR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR1866, V1_5                , DIMM_SR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C},
  {2, DDR2133, V1_5                , NP     , DIMM_SR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
  {2, DDR2133, V1_5                , NP     , DIMM_DR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C},
};
CONST PSC_TBL_ENTRY CZDataBusTblEnUDC3 = {
   {PSCFG_DATABUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (DataBusCZUDIMMDC3) / sizeof (PSCFG_DATABUS_ENTRY_D3),
   (VOID *)&DataBusCZUDIMMDC3
};

