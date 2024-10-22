/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH BaseReset Lib
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH DXE Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include <Uefi.h>
#include "AMD.h"
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/FchBaseLib.h>
#include <Protocol/Reset.h>
#include <Library/ResetSystemLib.h>
#include <FchRegistersCommon.h>

//
// Reset control register values
//
#define FULLRESET       0x0E
#define HARDRESET       0x06
#define SOFTRESET       0x04
#define HARDSTARTSTATE  0x02
#define SOFTSTARTSTATE  0x00

#define SUS_S3          0x0C00U     // S3
#define SUS_S5          0x1400U     // S4
#define SLP_TYPE        0x1C00U     // MASK
#define SLP_EN          0x2000U     // BIT13

/**
  This function causes a system-wide reset (cold reset), in which
  all circuitry within the system returns to its initial state. This type of reset
  is asynchronous to system operation and operates without regard to
  cycle boundaries.

  If this function returns, it means that the system does not support cold reset.
**/
VOID
EFIAPI
ResetCold (
  VOID
)
{
  UINT8      InitialData;
  UINT8      OutputData;
  UINT8      PwrRsrCfg;
  BOOLEAN    IsFullHardReset = FALSE;

  // DEBUG(( DEBUG_INFO, __FUNCTION__" - enter..\n" ));

  InitialData = HARDSTARTSTATE;
  IsFullHardReset = PcdGetBool (PcdFchFullHardReset);
  if (IsFullHardReset){
    OutputData  = FULLRESET;
  }else{
    OutputData  = HARDRESET;
  }
  IoWrite8 (FCH_IOMAP_REGCD6, 0x10);
  PwrRsrCfg = IoRead8 (FCH_IOMAP_REGCD7);
  PwrRsrCfg = PwrRsrCfg | BIT1;                          //set ToggleAllPwrGoodOnCf9
  IoWrite8 (FCH_IOMAP_REGCD7, PwrRsrCfg);

  IoWrite8 (FCH_IOMAP_REGCF9, InitialData);
  IoWrite8 (FCH_IOMAP_REGCF9, OutputData);

  //
  // Given we should have reset getting here would be bad
  //
  // DEBUG(( DEBUG_ERROR, __FUNCTION__" - should never be here!!!\n" ));
  FCH_DEADLOOP();
}

/**
  This function causes a system-wide initialization (warm reset), in which all processors
  are set to their initial state. Pending cycles are not corrupted.

  If this function returns, it means that the system does not support warm reset.
**/
VOID
EFIAPI
ResetWarm (
  VOID
)
{
  UINT8      InitialData;
  UINT8      OutputData;
  UINT8      PwrRsrCfg;

  // DEBUG(( DEBUG_INFO, __FUNCTION__" - enter..\n" ));

  InitialData = HARDSTARTSTATE;
  OutputData  = HARDRESET;
  IoWrite8 (FCH_IOMAP_REGCD6, 0x10);
  PwrRsrCfg = IoRead8 (FCH_IOMAP_REGCD7);
  PwrRsrCfg = PwrRsrCfg & 0xFD;                          //clear ToggleAllPwrGoodOnCf9
  IoWrite8 (FCH_IOMAP_REGCD7, PwrRsrCfg);

  IoWrite8 (FCH_IOMAP_REGCF9, InitialData);
  IoWrite8 (FCH_IOMAP_REGCF9, OutputData);

  //
  // Given we should have reset getting here would be bad
  //
  // DEBUG(( DEBUG_ERROR, __FUNCTION__" - should never be here!!!\n" ));
  FCH_DEADLOOP();
}

/**
  This function causes the system to enter a power state equivalent
  to the ACPI G2/S5 or G3 states.

  If this function returns, it means that the system does not support shutdown reset.
**/
VOID
EFIAPI
ResetShutdown (
  VOID
)
{
  UINT16     AcpiGpeBase;
  UINT16     AcpiPm1StsBase;
  UINT16     AcpiPm1CntBase;
  UINT32     Gpe0Enable;
  UINT16     PmCntl;
  UINT16     PwrSts;

  // DEBUG(( DEBUG_INFO, __FUNCTION__" - enter..\n" ));

  // Disable all GPE0 Event
  FchReadPmio (FCH_PMIOA_REG68, AccessWidth16, &AcpiGpeBase);
  AcpiGpeBase += 4; //Get enable base
  Gpe0Enable  = 0;
  IoWrite32 (AcpiGpeBase, Gpe0Enable);

  // Clear Power Button status.
  FchReadPmio (FCH_PMIOA_REG60, AccessWidth16, &AcpiPm1StsBase);
  PwrSts  = BIT8 | BIT15; //Clear WakeStatus with PwrBtnStatus
  IoWrite16 (AcpiPm1StsBase, PwrSts);

  // Transform system into S5 sleep state
  FchReadPmio (FCH_PMIOA_REG62, AccessWidth16, &AcpiPm1CntBase);
  PmCntl  = IoRead16 (AcpiPm1CntBase);
  PmCntl  = (PmCntl & ~SLP_TYPE) | SUS_S5 | SLP_EN;
  IoWrite16 (AcpiPm1CntBase, PmCntl);

  //
  // Given we should have shutdown getting here would be bad
  //
  // DEBUG(( DEBUG_ERROR, __FUNCTION__" - should never be here!!!\n" ));
  FCH_DEADLOOP();
}

/**
  This function causes the system to enter S3 and then wake up immediately.

  If this function returns, it means that the system does not support S3 feature.
**/
VOID
EFIAPI
EnterS3WithImmediateWake (
  VOID
)
{
  // DEBUG(( DEBUG_ERROR, __FUNCTION__" - No S3WithImmediateWake handler found!\n" ));
  ASSERT (FALSE);
}

/**
  This function causes a systemwide reset. The exact type of the reset is
  defined by the EFI_GUID that follows the Null-terminated Unicode string passed
  into ResetData. If the platform does not recognize the EFI_GUID in ResetData
  the platform must pick a supported reset type to perform.The platform may
  optionally log the parameters from any non-normal reset that occurs.

  @param[in]  DataSize   The size, in bytes, of ResetData.
  @param[in]  ResetData  The data buffer starts with a Null-terminated string,
                         followed by the EFI_GUID.
**/
VOID
EFIAPI
ResetPlatformSpecific (
  IN UINTN   DataSize,
  IN VOID    *ResetData
)
{
  // DEBUG(( DEBUG_ERROR, __FUNCTION__" - No PlatformSpecific reset handler found!\n" ));
  ASSERT( FALSE );
  ResetCold();
}
