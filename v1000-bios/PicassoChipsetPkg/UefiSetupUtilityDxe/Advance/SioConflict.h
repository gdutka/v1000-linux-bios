/** @file

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SETUP_SIOCONFLICT_H
#define _SETUP_SIOCONFLICT_H

#include "SetupUtility.h"

#define MAXPCISLOT 3

BOOLEAN
CheckSioConflict (
  IN  CHIPSET_CONFIGURATION                   *MyIfrNVData
  );

BOOLEAN
CheckSioAndPciSoltConflict (
  IN  CHIPSET_CONFIGURATION                   *MyIfrNVData,
  IN  UINT8                                   *PciIrqData,
  OUT UINT8                                   *DeviceKind
  );
  
UINT8
CheckLptAndPciSoltConflict (
  IN  CHIPSET_CONFIGURATION                   *MyIfrNVData,
  IN  UINT8                                   *PciIrqData
  );

BOOLEAN
CheckPciSioConflict (
  IN  CHIPSET_CONFIGURATION                   *MyIfrNVData,
  IN  UINT8                                   *PciIrqData,
  OUT UINT8                                   *DeviceKind
  );
#endif