/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE Driver
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

#ifndef _FCH_DXE_H_
#define _FCH_DXE_H_

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include "FchPlatform.h"
#include <Library/FchInitHookLib.h>
#include <Library/FchDxeLib.h>

#include  <IdsHookId.h>
#include  <Library/AmdIdsHookLib.h>

#include <Protocol/FchInitProtocol.h>
#include <Protocol/FchInitDonePolicyProtocol.h>

//
// Module data structure
//
/// Private data and access defines
typedef struct _FCH_DXE_PRIVATE {
  UINTN                            Signature;           ///< Signature
  FCH_INIT_PROTOCOL                FchInit;              ///< Protocol data
  EFI_EVENT                        EventAfterPciInit;   ///< Event related data
  EFI_EVENT                        EventReadyToBoot;    ///< Event related data
} FCH_DXE_PRIVATE;

#define FCH_DXE_PRIVATE_DATA_SIGNATURE   SIGNATURE_32 ('S', 'B', 'i', 'D')

#define FCH_DXE_PRIVATE_INSTANCE_FROM_PPI_THIS(a) \
  CR (a, FCH_PEI_PRIVATE, FchInit, FCH_DXE_PRIVATE_DATA_SIGNATURE)


//
// Functions Prototypes
//
EFI_STATUS
EFIAPI
FchDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );

//
// Port Disable
// for RV:
//      USB3.1/3.0 port map: bit0-3 controller0, bit4 controller1
//      USB2.0 port map:     bit0-3 controller0, bit4-5 controller1
//
EFI_STATUS
EFIAPI
FchUsbDisablePort (
  IN       CONST FCH_INIT_PROTOCOL   *This,
  IN       UINT8                     Socket,
  IN       UINT32                    USB3DisableMap,
  IN       UINT32                    USB2DisableMap
  );

//
// Over Current Mapping
// for RV:
//      Port0-3: Xhci0
//      Port4-5: Xhci1
//
EFI_STATUS
EFIAPI
FchUsbOvercurrentControl (
  IN       CONST FCH_INIT_PROTOCOL   *This,
  IN       UINT8                     Socket,
  IN       UINT8                     Port,
  IN       UINT8                     OCPin
  );
#endif // _FCH_DXE_H_

