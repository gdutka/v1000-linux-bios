/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Hudson-2 SMI Dispatcher Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  UEFI
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/******************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
**/
#include "FchSmmDispatcher.h"

FCH_SMM_DISPATCHER_TABLE   FchSmmDispatcherTable[] = {
  {
    FCH_SMI_REG84,
    UsbSmi,
    FchSmmUsbDispatchHandler
  },
  {
    FCH_SMI_REG88,
    BIT12,
    FchSmmUsbDispatchHandler2
  },
  {
    FCH_SMI_REG88,
    Slp_Type,
    FchSmmSxDispatchHandler
  },
  {
    FCH_SMI_REG88,
    SmiCmdPort,
    FchSmmSwDispatchHandler
  },
  {
    FCH_SMI_REG84,
    PwrBtn,
    FchSmmPwrBtnDispatchHandler
  },
  {
    FCH_SMI_REG90,
    IoTrapping0,
    FchSmmIoTrapDispatchHandler
  },
  {
    FCH_SMI_REG90,
    IoTrapping1,
    FchSmmIoTrapDispatchHandler
  },
  {
    FCH_SMI_REG90,
    IoTrapping2,
    FchSmmIoTrapDispatchHandler
  },
  {
    FCH_SMI_REG90,
    IoTrapping3,
    FchSmmIoTrapDispatchHandler
  },
  {
    FCH_SMI_REG90,
    ShortTimer | LongTimer,
    FchSmmPeriodicalDispatchHandler
  },
  {
    FCH_SMI_REG10,
    Gpe,
    FchSmmGpiDispatchHandler
  },
  {
    FCH_SMI_REG84,
    Smbus0 | BIT13 | BIT14,
    FchSmmMiscDispatchHandler
  },
  {
    FCH_SMI_REG80,
    AllGevents,
    FchSmmMiscDispatchHandler
  },
#ifdef FCH_TIMER_TICK_INTERVAL_WA
  {
    FCH_SMI_REG90,
    BIT24,
    FchSmmMiscDispatchHandler
  },
  {
    FCH_SMI_REG8C,
    BIT2,
    FchSmmMiscDispatchHandler
  },
#endif
  {
    FCH_SMI_REG88,
    FchGppSerr0 | FchGppSerr1 | FchGppSerr2 | FchGppSerr3,
    FchSmmMiscDispatchHandler
  },
  {
    FCH_SMI_REG84,
    FchFakeSts0,
    FchSmmMiscDispatchHandler
  },
};
UINT8 NumOfDispatcherTableEntry = sizeof (FchSmmDispatcherTable) / sizeof (FCH_SMM_DISPATCHER_TABLE);

SAVE_B2B_IO       B2bIoList[] = {
  {CFG_ADDR_PORT, SMM_IO_UINT32, 0},
  {0x70,  SMM_IO_UINT8, 0},
  {0x72,  SMM_IO_UINT8, 0},
  {FCH_IOMAP_REGC00, SMM_IO_UINT8, 0},
  {FCH_IOMAP_REGCD0, SMM_IO_UINT8, 0},
  {FCH_IOMAP_REGCD4, SMM_IO_UINT8, 0},
  {FCH_IOMAP_REGCD6, SMM_IO_UINT8, 0},
};
UINT8 NumOfB2bIoListEntry = sizeof (B2bIoList) / sizeof (SAVE_B2B_IO);

SAVE_PCI      SavePciList[] = {
  {0, 0, 0, 0x60, SMM_IO_UINT32, 0},
  {0, 0, 0, 0x94, SMM_IO_UINT32, 0},
  {0, 0, 0, 0xE8, SMM_IO_UINT32, 0},
  {0, 0, 0, 0xE0, SMM_IO_UINT32, 0},
  {0, 0, 0, 0xB8, SMM_IO_UINT32, 0},
};
UINT8 NumOfSavePciListEntry = sizeof (SavePciList) / sizeof (SAVE_PCI);

