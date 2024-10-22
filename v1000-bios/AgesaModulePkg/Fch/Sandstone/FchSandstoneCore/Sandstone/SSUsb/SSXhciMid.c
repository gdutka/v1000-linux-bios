/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch USB3 controller
 *
 * Init USB3 features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*;********************************************************************************
;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************
;*/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_SANDSTONE_SSUSB_SSXHCIMID_FILECODE

/**
 * FchInitMidUsbXhci - Config USB3 controller after PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidUsbXhci (
  IN  VOID     *FchDataPtr
  )
{
//USB_CMD trap
  FchRVXhciSmuUsbCmdTrap (FchDataPtr);
}

