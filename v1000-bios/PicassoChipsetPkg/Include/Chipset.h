//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEM_SERVICES_CHIPSET_SUPPORT_INC_H_
#define _OEM_SERVICES_CHIPSET_SUPPORT_INC_H_

//#include "Kernel.h"

//[-start-130208-IB10040023-add]//
#define  COMMON_SMM_OEM_DEVICE_REG_TABLE_ARG_COUNT        1
//[-end-130208-IB10040023-add]//

typedef enum {
  AMD_OVERRIDE_SPD_DATA,
  PEI_MAX_NUM_CHIPSET_EXTENDABLE_SUPPORT
} OEM_SERVICES_PEI_CHIPSET_EXTENDABLE_SUPPORT;

typedef enum {
  CHIPSET_CHANGE_CPU_FREQUENCY,
  RUNTIME_MAX_NUM_CHIPSET_EXTENDABLE_SUPPORT
} OEM_SERVICES_RUNTIME_CHIPSET_EXTENDABLE_SUPPORT;

typedef enum {
//[-start-130208-IB10040023-add]//
  SMM_OEM_UPDATE_DEVICE_REG_TABLE,
//[-end-130208-IB10040023-add]//
  SMM_MAX_NUM_CHIPSET_EXTENDABLE_SUPPORT        
} OEM_SERVICES_SMM_CHIPSET_EXTENDABLE_SUPPORT;

#endif
