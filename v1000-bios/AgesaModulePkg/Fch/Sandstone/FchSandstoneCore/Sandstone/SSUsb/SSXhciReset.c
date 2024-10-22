/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Xhci controller
 *
 * Init Xhci Controller features (PEI phase).
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
#include "GnbDxio.h"
#include <Library/NbioSmuV10Lib.h>

#define FILECODE FCH_SANDSTONE_FCHSANDSTONECORE_SANDSTONE_SSUSB_SSXHCIRESET_FILECODE

VOID
FchInitRecoveryXhci (
  IN  VOID     *FchDataPtr
  );

/**
 * FchInitResetXhci - Config Xhci controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetXhci (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                       SleepType;
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  UINT32                      UsbId;

  UsbId = 0;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  AGESA_TESTPOINT (TpFchInitResetUsb, NULL);

  SleepType = (ReadFchSleepType (LocalCfgPtr->StdHeader));

  if (LocalCfgPtr->FchReset.Xhci0Enable == FALSE) {
    FchSmnRW (0, FCH_SS_USB_NBIF_STRAP0, ~(UINT32) BIT28, 0, LocalCfgPtr->StdHeader);
    UsbId |= BIT0;
  }

  if (LocalCfgPtr->FchReset.Xhci1Enable == FALSE) {
    FchSmnRW (0, FCH_SS_USB1_NBIF_STRAP0, ~(UINT32) BIT28, 0, LocalCfgPtr->StdHeader);
    UsbId |= BIT1;
  }

  FchRVXhciSmuUsbPowerService (UsbId);

  if (LocalCfgPtr->FchReset.Xhci0Enable | LocalCfgPtr->FchReset.Xhci1Enable) {
    if (SleepType != ACPI_SLPTYP_S3) {
      //FchRVXhci0InitBootProgram (FchDataPtr);
      //FchRVXhci1InitBootProgram (FchDataPtr);
      FchRVUsb3InitBootProgram (FchDataPtr);
      //FchRVXhciSmuService (SMC_MSG_UsbInit);
    } else {
      //FchRVXhci0InitS3ExitProgram (FchDataPtr);
      //FchRVXhci1InitS3ExitProgram (FchDataPtr);
      FchRVUsb3InitS3ExitProgram (FchDataPtr);
      //FchRVXhciSmuService (SMC_MSG_UsbD3Exit);
    }
  }


}

/**
 * FchInitRecoveryLpc - Config Xhci controller during Crisis
 * Recovery
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitRecoveryXhci (
  IN  VOID     *FchDataPtr
  )
{
}


