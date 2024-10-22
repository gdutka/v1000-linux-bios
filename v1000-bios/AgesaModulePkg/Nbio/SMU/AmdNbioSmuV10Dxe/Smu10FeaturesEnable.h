/**
 * @file
 *
 * SMU Features enablement
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _SMU_FEATURES_ENABLE_H_
#define _SMU_FEATURES_ENABLE_H_

#include <smu10.h>

#define SMU_FEATURES_ENABLE_DEFAULT (1 << FEATURE_CCLK_CONTROLLER_BIT) | \
                                    (1 << FEATURE_FAN_CONTROLLER_BIT) | \
                                    (1 << FEATURE_DATA_CALCULATION_BIT) | \
                                    (1 << FEATURE_PPT_BIT) | \
                                    (1 << FEATURE_TDC_BIT) | \
                                    (1 << FEATURE_THERMAL_BIT) | \
                                    (1 << FEATURE_FIT_BIT) | \
                                    (1 << FEATURE_EDC_BIT) | \
                                    (1 << FEATURE_PLL_POWER_DOWN_BIT) | \
                                    (1 << FEATURE_ULV_BIT) | \
                                    (1 << FEATURE_VDDOFF_BIT) | \
                                    (1 << FEATURE_VCN_DPM_BIT) | \
                                    (1 << FEATURE_ACP_DPM_BIT) | \
                                    (1 << FEATURE_ISP_DPM_BIT) | \
                                    (0 << FEATURE_FCLK_DPM_BIT) | \
                                    (1 << FEATURE_SOCCLK_DPM_BIT) | \
                                    (1 << FEATURE_MP0CLK_DPM_BIT) | \
                                    (1 << FEATURE_LCLK_DPM_BIT) | \
                                    (1 << FEATURE_SHUBCLK_DPM_BIT) | \
                                    (1 << FEATURE_DCEFCLK_DPM_BIT) | \
                                    (1 << FEATURE_GFX_DPM_BIT) | \
                                    (1 << FEATURE_DS_GFXCLK_BIT) | \
                                    (1 << FEATURE_DS_SOCCLK_BIT) | \
                                    (1 << FEATURE_DS_LCLK_BIT) | \
                                    (1 << FEATURE_DS_DCEFCLK_BIT) | \
                                    (1 << FEATURE_DS_SHUBCLK_BIT) | \
                                    (1 << FEATURE_RM_BIT) | \
                                    (0 << FEATURE_S0i2_BIT) | \
                                    (1 << FEATURE_WHISPER_MODE_BIT) | \
                                    (1 << FEATURE_DS_FCLK_BIT) | \
                                    (1 << FEATURE_DS_SMNCLK_BIT) | \
                                    (1 << FEATURE_DS_MP1CLK_BIT)

#define SMU_FEATURES_ENABLE_EXT_DEFAULT  (1 << (FEATURE_DS_MP0CLK_BIT % 32)) | \
                                         (1 << (FEATURE_MGCG_BIT % 32)) | \
                                         (1 << (FEATURE_DS_FUSE_SRAM_BIT % 32)) | \
                                         (0 << (FEATURE_GFX_CKS % 32)) | \
                                         (1 << (FEATURE_PSI0_BIT % 32)) | \
                                         (1 << (FEATURE_PROCHOT_BIT % 32)) | \
                                         (1 << (FEATURE_CPUOFF_BIT % 32)) | \
                                         (1 << (FEATURE_STAPM_BIT % 32)) | \
                                         (1 << (FEATURE_CORE_CSTATES_BIT % 32)) | \
                                         (0 << (FEATURE_GFX_DUTY_CYCLE_BIT % 32)) | \
                                         (0 << (FEATURE_AA_MODE_BIT % 32)) | \
                                         (0 << (FEATURE_S0i3_EN_BIT % 32))

#define RV2_SMU_FEATURES_ENABLE_DEFAULT (1 << FEATURE_CCLK_CONTROLLER_BIT) | \
                                        (1 << FEATURE_FAN_CONTROLLER_BIT) | \
                                        (1 << FEATURE_DATA_CALCULATION_BIT) | \
                                        (1 << FEATURE_PPT_BIT) | \
                                        (1 << FEATURE_TDC_BIT) | \
                                        (1 << FEATURE_THERMAL_BIT) | \
                                        (1 << FEATURE_FIT_BIT) | \
                                        (1 << FEATURE_EDC_BIT) | \
                                        (1 << FEATURE_PLL_POWER_DOWN_BIT) | \
                                        (1 << FEATURE_ULV_BIT) | \
                                        (1 << FEATURE_VDDOFF_BIT) | \
                                        (1 << FEATURE_VCN_DPM_BIT) | \
                                        (1 << FEATURE_ACP_DPM_BIT) | \
                                        (1 << FEATURE_ISP_DPM_BIT) | \
                                        (0 << FEATURE_FCLK_DPM_BIT) | \
                                        (1 << FEATURE_SOCCLK_DPM_BIT) | \
                                        (1 << FEATURE_MP0CLK_DPM_BIT) | \
                                        (1 << FEATURE_LCLK_DPM_BIT) | \
                                        (1 << FEATURE_SHUBCLK_DPM_BIT) | \
                                        (1 << FEATURE_DCEFCLK_DPM_BIT) | \
                                        (1 << FEATURE_GFX_DPM_BIT) | \
                                        (1 << FEATURE_DS_GFXCLK_BIT) | \
                                        (1 << FEATURE_DS_SOCCLK_BIT) | \
                                        (1 << FEATURE_DS_LCLK_BIT) | \
                                        (1 << FEATURE_DS_DCEFCLK_BIT) | \
                                        (1 << FEATURE_DS_SHUBCLK_BIT) | \
                                        (1 << FEATURE_RM_BIT) | \
                                        (0 << FEATURE_S0i2_BIT) | \
                                        (1 << FEATURE_WHISPER_MODE_BIT) | \
                                        (1 << FEATURE_DS_FCLK_BIT) | \
                                        (1 << FEATURE_DS_SMNCLK_BIT) | \
                                        (1 << FEATURE_DS_MP1CLK_BIT)

#define RV2_SMU_FEATURES_ENABLE_EXT_DEFAULT  (1 << (FEATURE_DS_MP0CLK_BIT % 32)) | \
                                             (1 << (FEATURE_MGCG_BIT % 32)) | \
                                             (1 << (FEATURE_DS_FUSE_SRAM_BIT % 32)) | \
                                             (0 << (FEATURE_GFX_CKS % 32)) | \
                                             (1 << (FEATURE_PSI0_BIT % 32)) | \
                                             (1 << (FEATURE_PROCHOT_BIT % 32)) | \
                                             (1 << (FEATURE_CPUOFF_BIT % 32)) | \
                                             (1 << (FEATURE_STAPM_BIT % 32)) | \
                                             (1 << (FEATURE_CORE_CSTATES_BIT % 32)) | \
                                             (0 << (FEATURE_GFX_DUTY_CYCLE_BIT % 32)) | \
                                             (0 << (FEATURE_AA_MODE_BIT % 32)) | \
                                             (0 << (FEATURE_LIVMIN_BIT % 32)) | \
                                             (0 << (FEATURE_RLC_PACE_BIT % 32))

#define PCO_SMU_FEATURES_ENABLE_DEFAULT (1 << FEATURE_CCLK_CONTROLLER_BIT) | \
                                        (1 << FEATURE_FAN_CONTROLLER_BIT) | \
                                        (1 << FEATURE_DATA_CALCULATION_BIT) | \
                                        (1 << FEATURE_PPT_BIT) | \
                                        (1 << FEATURE_TDC_BIT) | \
                                        (1 << FEATURE_THERMAL_BIT) | \
                                        (1 << FEATURE_FIT_BIT) | \
                                        (1 << FEATURE_EDC_BIT) | \
                                        (1 << FEATURE_PLL_POWER_DOWN_BIT) | \
                                        (1 << FEATURE_ULV_BIT) | \
                                        (1 << FEATURE_VDDOFF_BIT) | \
                                        (1 << FEATURE_VCN_DPM_BIT) | \
                                        (1 << FEATURE_ACP_DPM_BIT) | \
                                        (1 << FEATURE_ISP_DPM_BIT) | \
                                        (0 << FEATURE_FCLK_DPM_BIT) | \
                                        (1 << FEATURE_SOCCLK_DPM_BIT) | \
                                        (1 << FEATURE_MP0CLK_DPM_BIT) | \
                                        (1 << FEATURE_LCLK_DPM_BIT) | \
                                        (1 << FEATURE_SHUBCLK_DPM_BIT) | \
                                        (1 << FEATURE_DCEFCLK_DPM_BIT) | \
                                        (1 << FEATURE_GFX_DPM_BIT) | \
                                        (1 << FEATURE_DS_GFXCLK_BIT) | \
                                        (1 << FEATURE_DS_SOCCLK_BIT) | \
                                        (1 << FEATURE_DS_LCLK_BIT) | \
                                        (1 << FEATURE_DS_DCEFCLK_BIT) | \
                                        (1 << FEATURE_DS_SHUBCLK_BIT) | \
                                        (1 << FEATURE_RM_BIT) | \
                                        (0 << FEATURE_S0i2_BIT) | \
                                        (1 << FEATURE_WHISPER_MODE_BIT) | \
                                        (1 << FEATURE_DS_FCLK_BIT) | \
                                        (1 << FEATURE_DS_SMNCLK_BIT) | \
                                        (1 << FEATURE_DS_MP1CLK_BIT)

#define PCO_SMU_FEATURES_ENABLE_EXT_DEFAULT  (1 << (FEATURE_DS_MP0CLK_BIT % 32)) | \
                                             (1 << (FEATURE_MGCG_BIT % 32)) | \
                                             (1 << (FEATURE_DS_FUSE_SRAM_BIT % 32)) | \
                                             (0 << (FEATURE_GFX_CKS % 32)) | \
                                             (1 << (FEATURE_PSI0_BIT % 32)) | \
                                             (1 << (FEATURE_PROCHOT_BIT % 32)) | \
                                             (1 << (FEATURE_CPUOFF_BIT % 32)) | \
                                             (1 << (FEATURE_STAPM_BIT % 32)) | \
                                             (1 << (FEATURE_CORE_CSTATES_BIT % 32)) | \
                                             (0 << (FEATURE_GFX_DUTY_CYCLE_BIT % 32)) | \
                                             (0 << (FEATURE_AA_MODE_BIT % 32)) | \
                                             (0 << (FEATURE_S0i3_EN_BIT % 32)) | \
                                             (0 << (FEATURE_OSCSTATE_BIT % 32))

#define AM4_SMU_FEATURES_ENABLE_DEFAULT (1 << FEATURE_CCLK_CONTROLLER_BIT) | \
                                        (1 << FEATURE_FAN_CONTROLLER_BIT) | \
                                        (1 << FEATURE_DATA_CALCULATION_BIT) | \
                                        (1 << FEATURE_PPT_BIT) | \
                                        (1 << FEATURE_TDC_BIT) | \
                                        (1 << FEATURE_THERMAL_BIT) | \
                                        (1 << FEATURE_FIT_BIT) | \
                                        (1 << FEATURE_EDC_BIT) | \
                                        (1 << FEATURE_PLL_POWER_DOWN_BIT) | \
                                        (1 << FEATURE_ULV_BIT) | \
                                        (1 << FEATURE_VDDOFF_BIT) | \
                                        (1 << FEATURE_VCN_DPM_BIT) | \
                                        (1 << FEATURE_ACP_DPM_BIT) | \
                                        (1 << FEATURE_ISP_DPM_BIT) | \
                                        (0 << FEATURE_FCLK_DPM_BIT) | \
                                        (1 << FEATURE_SOCCLK_DPM_BIT) | \
                                        (1 << FEATURE_MP0CLK_DPM_BIT) | \
                                        (1 << FEATURE_LCLK_DPM_BIT) | \
                                        (1 << FEATURE_SHUBCLK_DPM_BIT) | \
                                        (1 << FEATURE_DCEFCLK_DPM_BIT) | \
                                        (1 << FEATURE_GFX_DPM_BIT) | \
                                        (1 << FEATURE_DS_GFXCLK_BIT) | \
                                        (1 << FEATURE_DS_SOCCLK_BIT) | \
                                        (1 << FEATURE_DS_LCLK_BIT) | \
                                        (1 << FEATURE_DS_DCEFCLK_BIT) | \
                                        (1 << FEATURE_DS_SHUBCLK_BIT) | \
                                        (1 << FEATURE_RM_BIT) | \
                                        (0 << FEATURE_S0i2_BIT) | \
                                        (0 << FEATURE_WHISPER_MODE_BIT) | \
                                        (1 << FEATURE_DS_FCLK_BIT) | \
                                        (1 << FEATURE_DS_SMNCLK_BIT) | \
                                        (1 << FEATURE_DS_MP1CLK_BIT)

#define AM4_SMU_FEATURES_ENABLE_EXT_DEFAULT  (1 << (FEATURE_DS_MP0CLK_BIT % 32)) | \
                                             (1 << (FEATURE_MGCG_BIT % 32)) | \
                                             (1 << (FEATURE_DS_FUSE_SRAM_BIT % 32)) | \
                                             (0 << (FEATURE_GFX_CKS % 32)) | \
                                             (1 << (FEATURE_PSI0_BIT % 32)) | \
                                             (1 << (FEATURE_PROCHOT_BIT % 32)) | \
                                             (1 << (FEATURE_CPUOFF_BIT % 32)) | \
                                             (1 << (FEATURE_STAPM_BIT % 32)) | \
                                             (1 << (FEATURE_CORE_CSTATES_BIT % 32)) | \
                                             (0 << (FEATURE_GFX_DUTY_CYCLE_BIT % 32)) | \
                                             (0 << (FEATURE_AA_MODE_BIT % 32))
#endif

