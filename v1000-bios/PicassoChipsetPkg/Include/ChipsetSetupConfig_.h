/** @file
  Chipset Setup Configuration Definitions

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CHIPSET_SETUP_CONFIG_H_
#define _CHIPSET_SETUP_CONFIG_H_

#include <KernelSetupConfig.h>

#pragma pack(1)
//
//  Chipset Setup Configuration Structure
//
typedef struct {
  //
  // Import chipset setup configuration definition from ChipsetSetupData.h
  //
  #define _IMPORT_CHIPSET_SETUP_
  #include <ChipsetSetupData.h>
  #undef _IMPORT_CHIPSET_SETUP_

} CHIPSET_CONFIGURATION;

#pragma pack()

#endif
