/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#ifndef _GET_CORE_COF_H_
#define _GET_CORE_COF_H_

#define AMD_MSR_PSTATE_STS        0xC0010063
#define CURRENT_PSTATE_MASK       0x00000007
#define AMD_MSR_PSTATE_0          0xC0010064
#define CPU_FID_MASK              0x000000000000003F // BIS[5:0] Family 16h
#define CPU_DID_MASK              0x00000000000001C0 // BIS[8:6] Family 16h
#define PSTATE_ENALBE_MASK        0x8000000000000000

#define AMD_PM_BUS        0
#define AMD_PM_DEV        0x18
#define AMD_PM_FUNC       4

#define AMD_PM_CORE_PERFORMANCE_BOOST_CONTROL 0x15C

#define AMD_PM_CORE_PERFORMANCE_BOOST_DISABLE 0
#define AMD_PM_CORE_PERFORMANCE_BOOST_ENABLE  1

/**  Core current operating frequency in MHz.

  @return The Core current operating frequency in MHz.

**/
UINT64
GetCoreCOF (
  VOID
  );

#endif
