/*****************************************************************************
 *
 * Copyright 2008 - 2019 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software and documentation (if
 * any) (collectively, the "Materials") pursuant to the terms and conditions of
 * the Software License Agreement included with the Materials.  If you do not
 * have a copy of the Software License Agreement, contact your AMD
 * representative for a copy.
 *
 * You agree that you will not reverse engineer or decompile the Materials, in
 * whole or in part, except as allowed by applicable law.
 *
 * WARRANTY DISCLAIMER:  THE MATERIALS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE
 * MATERIALS WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM
 * CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE
 * OF THE MATERIAL IS ASSUMED BY YOU.  Some jurisdictions do not allow the
 * exclusion of implied warranties, so the above exclusion may not apply to
 * You.
 *
 * LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL
 * NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
 * INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
 * THE MATERIALS OR THIS AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  In no event shall AMD's total
 * liability to You for all damages, losses, and causes of action (whether in
 * contract, tort (including negligence) or otherwise) exceed the amount of
 * $100 USD. You agree to defend, indemnify and hold harmless AMD and its
 * licensors, and any of their directors, officers, employees, affiliates or
 * agents from and against any and all loss, damage, liability and other
 * expenses (including reasonable attorneys' fees), resulting from Your use of
 * the Materials or violation of the terms and conditions of this Agreement.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS:  The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgment of AMD's proprietary rights in them.
 *
 * EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as
 * stated in the Software License Agreement.
 ***************************************************************************/


#ifndef _AMD_CBS_FORM_ID_H_
#define _AMD_CBS_FORM_ID_H_

  #define CBS_CONFIGURATION_VARSTORE_ID                                           0x5000
// Section ID
  #define SETUP_CBS_ROOT_LABLE                                                    0x7000
  #define SETUP_ZEN_COMMON_OPTIONS_LABLE                                          0x7001
  #define SETUP_DF_COMMON_OPTIONS_LABLE                                           0x7002
  #define SETUP_UMC_COMMON_OPTIONS_LABLE                                          0x7003
  #define SETUP_NBIO_COMMON_OPTIONS_LABLE                                         0x7004
  #define SETUP_FCH_COMMON_OPTIONS_LABLE                                          0x7005
  #define SETUP_PROMONTORY_COMMON_OPTIONS_LABLE                                   0x7006
  #define SETUP_CUSTOM_PSTATES_THROTTLING_LABLE                                   0x7008
  #define SETUP_CORE_THREAD_ENABLEMENT_LABLE                                      0x7009
  #define SETUP_DECLINE_LABLE                                                     0x700A
  #define SETUP_ACCEPT_LABLE                                                      0x700B
  #define SETUP_DISAGREE_LABLE                                                    0x702D
  #define SETUP_AGREE_LABLE                                                       0x702E
  #define SETUP_DDR4_COMMON_OPTIONS_LABLE                                         0x702F
  #define SETUP_DRAM_MEMORY_MAPPING_LABLE                                         0x7030
  #define SETUP_NVDIMM_LABLE                                                      0x7031
  #define SETUP_MEMORY_MBIST_LABLE                                                0x7032
  #define SETUP_DRAM_TIMING_CONFIGURATION_LABLE                                   0x7033
  #define SETUP_DRAM_CONTROLLER_CONFIGURATION_LABLE                               0x7034
  #define SETUP_CAD_BUS_CONFIGURATION_LABLE                                       0x7035
  #define SETUP_DATA_BUS_CONFIGURATION_LABLE                                      0x7036
  #define SETUP_COMMON_RAS_LABLE                                                  0x7037
  #define SETUP_SECURITY_LABLE                                                    0x7038
  #define SETUP_I_DECLINE_LABLE                                                   0x7039
  #define SETUP_I_ACCEPT_LABLE                                                    0x703A
  #define SETUP_DRAM_POWER_OPTIONS_LABLE                                          0x703B
  #define SETUP_ECC_CONFIGURATION_LABLE                                           0x703C
  #define SETUP_GFX_CONFIGURATION_LABLE                                           0x703E
  #define SETUP_NB_CONFIGURATION_LABLE                                            0x703F
  #define SETUP_PCIE_CONFIGURATION_LABLE                                          0x7040
  #define SETUP_FAN_CONTROL_LABLE                                                 0x7042
  #define SETUP_SATA_CONFIGURATION_OPTIONS_LABLE                                  0x7043
  #define SETUP_USB_CONFIGURATION_OPTIONS_LABLE                                   0x7044
  #define SETUP_AC_POWER_LOSS_OPTIONS_LABLE                                       0x7045
  #define SETUP_I2C_CONFIGURATION_OPTIONS_LABLE                                   0x7046
  #define SETUP_UART_CONFIGURATION_OPTIONS_LABLE                                  0x7047
  #define SETUP_ESPI_CONFIGURATION_OPTIONS_LABLE                                  0x7048
  #define SETUP_XGBE_CONFIGURATION_OPTIONS_LABLE                                  0x7049
  #define SETUP_EMMC_OPTIONS_LABLE                                                0x704A
  #define SETUP_LPC_OPTIONS_LABLE                                                 0x704B
  #define SETUP_SYSTEM_CONTROL_LABLE                                              0x704C
  #define SETUP_PT_SATA_CONFIGURATION_OPTIONS_LABLE                               0x704F
  #define SETUP_PT_SATA_PORT_CONFIGURATION_OPTION_LABLE                           0x7050
  #define SETUP_PT_USB_CONFIGURATION_OPTIONS_LABLE                                0x7051
  #define SETUP_PROM3_4_5_6_7_USB_PORT_CONFIGURATION_OPTIONS_LABLE                0x7052
  #define SETUP_PROM2_8_9_USB_PORT_CONFIGURATION_OPTIONS_LABLE                    0x7053
  #define SETUP_PROM1_USB_PORT_CONFIGURATION_OPTIONS_LABLE                        0x7054
  #define SETUP_PT_PCIE_PORT_CONFIGURATION_OPTION_LABLE                           0x7055

// KEY function ID
  #define KEY_CBS_CMN_CPU_OC_MODE                                                 0x7007  //CbsCmnCpuOcMode
  #define KEY_CBS_CPU_PST_CUSTOM_P0                                               0x700C  //CbsCpuPstCustomP0
  #define KEY_CBS_CPU_PST0_FID                                                    0x700D  //CbsCpuPst0Fid
  #define KEY_CBS_CPU_PST0_DID                                                    0x700E  //CbsCpuPst0Did
  #define KEY_CBS_CPU_PST0_VID                                                    0x700F  //CbsCpuPst0Vid
  #define KEY_CBS_CPU_PST_CUSTOM_P1                                               0x7010  //CbsCpuPstCustomP1
  #define KEY_CBS_CPU_PST1_FID                                                    0x7011  //CbsCpuPst1Fid
  #define KEY_CBS_CPU_PST1_DID                                                    0x7012  //CbsCpuPst1Did
  #define KEY_CBS_CPU_PST1_VID                                                    0x7013  //CbsCpuPst1Vid
  #define KEY_CBS_CPU_PST_CUSTOM_P2                                               0x7014  //CbsCpuPstCustomP2
  #define KEY_CBS_CPU_PST2_FID                                                    0x7015  //CbsCpuPst2Fid
  #define KEY_CBS_CPU_PST2_DID                                                    0x7016  //CbsCpuPst2Did
  #define KEY_CBS_CPU_PST2_VID                                                    0x7017  //CbsCpuPst2Vid
  #define KEY_CBS_CPU_PST_CUSTOM_P3                                               0x7018  //CbsCpuPstCustomP3
  #define KEY_CBS_CPU_PST3_FID                                                    0x7019  //CbsCpuPst3Fid
  #define KEY_CBS_CPU_PST3_DID                                                    0x701A  //CbsCpuPst3Did
  #define KEY_CBS_CPU_PST3_VID                                                    0x701B  //CbsCpuPst3Vid
  #define KEY_CBS_CPU_PST_CUSTOM_P4                                               0x701C  //CbsCpuPstCustomP4
  #define KEY_CBS_CPU_PST4_FID                                                    0x701D  //CbsCpuPst4Fid
  #define KEY_CBS_CPU_PST4_DID                                                    0x701E  //CbsCpuPst4Did
  #define KEY_CBS_CPU_PST4_VID                                                    0x701F  //CbsCpuPst4Vid
  #define KEY_CBS_CPU_PST_CUSTOM_P5                                               0x7020  //CbsCpuPstCustomP5
  #define KEY_CBS_CPU_PST5_FID                                                    0x7021  //CbsCpuPst5Fid
  #define KEY_CBS_CPU_PST5_DID                                                    0x7022  //CbsCpuPst5Did
  #define KEY_CBS_CPU_PST5_VID                                                    0x7023  //CbsCpuPst5Vid
  #define KEY_CBS_CPU_PST_CUSTOM_P6                                               0x7024  //CbsCpuPstCustomP6
  #define KEY_CBS_CPU_PST6_FID                                                    0x7025  //CbsCpuPst6Fid
  #define KEY_CBS_CPU_PST6_DID                                                    0x7026  //CbsCpuPst6Did
  #define KEY_CBS_CPU_PST6_VID                                                    0x7027  //CbsCpuPst6Vid
  #define KEY_CBS_CPU_PST_CUSTOM_P7                                               0x7028  //CbsCpuPstCustomP7
  #define KEY_CBS_CPU_PST7_FID                                                    0x7029  //CbsCpuPst7Fid
  #define KEY_CBS_CPU_PST7_DID                                                    0x702A  //CbsCpuPst7Did
  #define KEY_CBS_CPU_PST7_VID                                                    0x702B  //CbsCpuPst7Vid
  #define KEY_CBS_CPU_RELAXED_EDC_THROTTLING                                      0x702C  //CbsCpuRelaxedEdcThrottling
  #define KEY_CBS_CMN_MEM_TSME_DDR4                                               0x703D  //CbsCmnMemTsmeDdr4
  #define KEY_CBS_CMN_GNB_SMU_SYSTEM_CONFIG                                       0x7041  //CbsCmnGnbSMUSystemConfig
  #define KEY_CBS_CMN_FCH_SATA_ENABLE                                             0x704D  //CbsCmnFchSataEnable
  #define KEY_CBS_CMN_FCH_SATA_CLASS                                              0x704E  //CbsCmnFchSataClass

#endif // _AMD_CBS_FORM_ID_H_
