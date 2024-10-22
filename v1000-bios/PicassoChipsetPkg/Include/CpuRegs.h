/** @file
  CpuRegs.h.

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/


#ifndef _CPU_REG_
#define _CPU_REG_

#define AMD_LDT_BUS       0
#define AMD_LDT_DEV       0x18
#define AMD_LDT_FUNC      0

#define AMD_LDT_REG60     0x60
#define AMD_LDT_REG68     0x68
#define CPU1EN            BIT5
#define AMD_LDT_REG84     0x84


#define AMD_ADDR_BUS      0
#define AMD_ADDR_DEV      0x18
#define AMD_ADDR_FUNC     1

#define AMD_ADDR_REGC0    0xC0
#define AMD_ADDR_REGC4    0xC4
#define AMD_ADDR_REGC8    0xC8
#define AMD_ADDR_REGCC    0xCC
#define AMD_ADDR_REGD0    0xD0
#define AMD_ADDR_REGD4    0xD4
#define AMD_ADDR_REGD8    0xD8
#define AMD_ADDR_REGDC    0xDC

#define AMD_MEM_BUS       0
#define AMD_MEM_DEV       0x18
#define AMD_MEM_FUNC      2
#define AMD_MEM_REG118    0x118

#define AMD_MISC_BUS      0
#define AMD_MISC_DEV      0x18
#define AMD_MISC_FUNC     3


#define AMD_LDT_REG84     0x84

#define AMD_MISC_REG80    0x80
#define AMD_MISC_REGD8    0xD8
#define AMD_MISC_REG64    0x64
#define ENABLE_HTC        0x01
#define AMD_MISC_REGE8    0xE8
#define HTC_CAPABLE       0x400
#define AMD_MISC_REG190   0x190

#define AMD_PM_BUS        0
#define AMD_PM_DEV        0x18
#define AMD_PM_FUNC       4

#define AMD_PM_C6_BASE    0x12C

#define AMD_PM_CORE_PERFORMANCE_BOOST_CONTROL 0x15C
typedef struct {
  UINT32 BoostSrc:2;                 ///< Boost source
  UINT32 NumBoostStates:3;           ///< Number of boosted states
  UINT32 :2;                         ///< Reserved
  UINT32 ApmMasterEn:1;              ///< APM master enable
  UINT32 CstatePwrEn:1;              ///< C-State power enable
  UINT32 :22;                        ///< Reserved
  UINT32 BoostLock:1;                ///<
} F16_CPB_CTRL_REGISTER;

typedef union {
  UINT32 CpbCtrlData;                ///< SMAF raw value
  F16_CPB_CTRL_REGISTER CpbCtrl;     ///< SMAF structure
} F16_CPB_CTRL_DEFINE;
#define AMD_PM_CORE_PERFORMANCE_BOOST_DISABLE 0
#define AMD_PM_CORE_PERFORMANCE_BOOST_ENABLE  1

#define AMD_MSR_MC0_STATUS    0x401
#define AMD_MSR_MC4_STATUS    0x411
#define AMD_MSR_MC5_STATUS    0x415
#define AMD_MSR_MC5_MISC      0x417

#define AMD_MSR_EFER          0xC0000080
#define AMD_MSR_HWCR          0xC0010015
#define SMM_LOCK              0x1
#define AMD_MSR_VM_CR         0xC0010114
#define AMD_MSR_SVM_LOCK_KEY  0xC0010118

#define AMD_MSR_PSTATE_CTRL         0xC0010062
#define AMD_MSR_PSTATE_STS          0xC0010063
#define CURRENT_PSTATE_MASK         0x00000007
#define AMD_MSR_PSTATE_0            0xC0010064
#define AMD_MSR_PSTATE_1            0xC0010065
#define AMD_MSR_PSTATE_2            0xC0010066
#define AMD_MSR_PSTATE_3            0xC0010067
#define AMD_MSR_PSTATE_4            0xC0010068
#define AMD_MSR_PSTATE_5            0xC0010069
#define AMD_MSR_PSTATE_6            0xC001006A
#define AMD_MSR_PSTATE_7            0xC001006B
#define   PSTATE_EN                 0x8000000000000000

#define   CPU_FID_MASK              0x000000000000003F // BIS[5:0] Family 16h
#define   CPU_DID_MASK              0x00000000000001C0 // BIS[8:6] Family 16h

#define   CPU_VID_MASK              0x000000000000FE00

#define AMD_MSR_SMI_ON_IO_TRAP_0               0xC0010050
#define AMD_MSR_SMI_ON_IO_TRAP_1               0xC0010051
#define AMD_MSR_SMI_ON_IO_TRAP_2               0xC0010052
#define AMD_MSR_SMI_ON_IO_TRAP_3               0xC0010053
#define AMD_MSR_SMI_ON_IO_TRAP_CTL_STS         0xC0010054
#define AMD_MSR_SMI_TRIGGER_IO_CYCLE_REGISTER  0xC0010056

#define AMD_MSR_MMIO_Cfg_Base       0xC0010058      // MMIO Configuration Base Address Register

#define AMD_MSR_SMI_MMIO_CONFIG_TRAP_CONTROL  0xC0010059
#define AMD_MSR_SMI_ON_MMIO_TRAP_0            0xC001005A
#define AMD_MSR_SMI_ON_MMIO_TRAP_1            0xC001005B
#define AMD_MSR_SMI_ON_MMIO_TRAP_2            0xC001005C
#define AMD_MSR_SMI_ON_MMIO_TRAP_3            0xC001005D

//
// CPUID related registers
//
#define AMD_CPUID_ASIZE_PCCOUNT     0x80000008  // Address Size Identifiers, Physical Core Count

#endif
