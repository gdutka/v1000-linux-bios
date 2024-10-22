/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Ccx Down Core Library
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


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuRegisters.h"
#include "Filecode.h"
#include "PiPei.h"
#include "Library/AmdBaseLib.h"
#include "Library/CcxDownCoreLib.h"
#include "Library/PeiServicesTablePointerLib.h"
#include <Library/PciLib.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_CCXDOWNCOREXVLIB_CCXDOWNCOREXVLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
#define CCX_XV_DOWN_CORE_REG_MASK 0xF

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *  Set down core register
 *
 *
 *  @param[in] CcxDownCoreMode - Down core mode
 *  @param[in] StdHeader      - Config handle for library and services.
 *
 */
VOID
CcxDownCore (
  IN       CCX_DOWN_CORE_MODE CcxDownCoreMode,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32    DownCoreReg;
  UINT32    LocalPciRegister;
  UINTN     PciAddress;
  EFI_PEI_SERVICES **PeiServices;

  IDS_HDT_CONSOLE (CPU_TRACE, "  Down Core Mode %d\n", CcxDownCoreMode);
  DownCoreReg = 0;
  switch (CcxDownCoreMode) {
  case CCX_DOWN_CORE_AUTO:
    return;
    break;
  case CCX_DOWN_CORE_1_0:
    DownCoreReg = 0xE;
    break;
  case CCX_DOWN_CORE_1_1:
    DownCoreReg = 0xA;
    break;
  case CCX_DOWN_CORE_2_0:
    DownCoreReg = 0xC;
    break;
  default:
    return;
    break;
  }

  PciAddress = MAKE_SBDFO (0, 0, 0x18, FUNC_3, DOWNCORE_CTRL);
  LocalPciRegister = PciRead32 (PciAddress);

  if ((LocalPciRegister & CCX_XV_DOWN_CORE_REG_MASK) == DownCoreReg) {
    return;
  } else {
    PciWrite32 (PciAddress, DownCoreReg);
    IDS_HDT_CONSOLE (CPU_TRACE, "  Set down core register %x, and issue warm reset\n", DownCoreReg);
    PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
    ASSERT (PeiServices != NULL);
    (**PeiServices).ResetSystem (PeiServices);
  }
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */


