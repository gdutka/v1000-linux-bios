/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CbsDxe.h"
#include "CbsIDSCommonDxe.h"
extern DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(IdsDxeFunTable,_Common)[];

#ifdef AMD_CBS_ZEN_RV
#include "CBSZenFunDXE_RV.h"
extern DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(ZenDxeFunTable,_RV)[];
#endif
#ifdef AMD_CBS_DF_RV
#include "CBSDfFunDXE_RV.h"
extern DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(DfDxeFunTable,_RV)[];
#endif
#ifdef AMD_CBS_UMC_RV
#include "CBSUmcFunDXE_RV.h"
extern DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(UmcDxeFunTable,_RV)[];
#endif
#ifdef AMD_CBS_NBIO_RV
#include "CBSNbioFunDXE_RV.h"
extern DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(NbioDxeFunTable,_RV)[];
#endif
#ifdef AMD_CBS_FCH_RV
#include "CBSFchFunDXE_RV.h"
extern DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(FchDxeFunTable,_RV)[];
#endif
#ifdef AMD_CBS_PROMONTORY_RV
#include "CBSPromontoryFunDXE_RV.h"
extern DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(PromontoryDxeFunTable,_RV)[];
#endif
#ifdef AMD_CBS_SMU_RV
#include "CBSSmuFunDXE_RV.h"
extern DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(SmuDxeFunTable,_RV)[];
#endif
#ifdef AMD_CBS_SOC_RV
#include "CBSSocFunDXE_RV.h"
extern DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(SocDxeFunTable,_RV)[];
#endif

DXE_CBS_COMPONENT_STRUCT CbsCommonFuncTable[] =
{
  {0xFFFFFFFF,    AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(IdsDxeFunTable,_Common) },

#ifdef AMD_CBS_ZEN_RV
  {F17_RV_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(ZenDxeFunTable,_RV) },
  {F17_RV2_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(ZenDxeFunTable,_RV) },
  {F17_PIC_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(ZenDxeFunTable,_RV) },
#endif
#ifdef AMD_CBS_DF_RV
  {F17_RV_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(DfDxeFunTable,_RV) },
  {F17_RV2_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(DfDxeFunTable,_RV) },
  {F17_PIC_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(DfDxeFunTable,_RV) },
#endif
#ifdef AMD_CBS_UMC_RV
  {F17_RV_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(UmcDxeFunTable,_RV) },
  {F17_RV2_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(UmcDxeFunTable,_RV) },
  {F17_PIC_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(UmcDxeFunTable,_RV) },
#endif
#ifdef AMD_CBS_NBIO_RV
  {F17_RV_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(NbioDxeFunTable,_RV) },
  {F17_RV2_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(NbioDxeFunTable,_RV) },
  {F17_PIC_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(NbioDxeFunTable,_RV) },
#endif
#ifdef AMD_CBS_FCH_RV
  {F17_RV_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(FchDxeFunTable,_RV) },
  {F17_RV2_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(FchDxeFunTable,_RV) },
  {F17_PIC_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(FchDxeFunTable,_RV) },
#endif
#ifdef AMD_CBS_PROMONTORY_RV
  {F17_RV_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(PromontoryDxeFunTable,_RV) },
  {F17_RV2_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(PromontoryDxeFunTable,_RV) },
  {F17_PIC_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(PromontoryDxeFunTable,_RV) },
#endif
#ifdef AMD_CBS_SMU_RV
  {F17_RV_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(SmuDxeFunTable,_RV) },
  {F17_RV2_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(SmuDxeFunTable,_RV) },
  {F17_PIC_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(SmuDxeFunTable,_RV) },
#endif
#ifdef AMD_CBS_SOC_RV
  {F17_RV_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(SocDxeFunTable,_RV) },
  {F17_RV2_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(SocDxeFunTable,_RV) },
  {F17_PIC_RAW_ID, AMD_RESERVE_ALL, (DXE_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(SocDxeFunTable,_RV) },
#endif

  //
  // end of table.
  //
  {0, 0, NULL}
};

