/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU prefetch functions.
 *
 * This funtion provides for performance tuning to optimize for specific
 * workloads. For general performance use the recommended settings.
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Ccx
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _CCX_ZEN_ZP_PREFETCH_DXE_H_
#define _CCX_ZEN_ZP_PREFETCH_DXE_H_

#pragma pack (push, 1)

///  The possible hardware prefetch mode settings.
typedef enum  {
  HARDWARE_PREFETCHER_AUTO,                     ///< Use the recommended setting for the processor. In most cases, the recommended setting is enabled.
  DISABLE_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES,  ///< Use the recommended setting for the hardware prefetcher, but disable training on software prefetches.
  DISABLE_L1_PREFETCHER,                        ///< Use the recommended settings for the hardware prefetcher, but disable L1 prefetching and above.
  DISABLE_L2_STRIDE_PREFETCHER,                 ///< Use the recommended settings for the hardware prefetcher, but disable the L2 stride prefetcher and above
  DISABLE_HARDWARE_PREFETCH,                    ///< Disable hardware prefetching.
  MAX_HARDWARE_PREFETCH_MODE                    ///< Not a hardware prefetch mode, use for limit checking.
} HARDWARE_PREFETCH_MODE;

///  The possible software prefetch mode settings.
typedef enum  {
  SOFTWARE_PREFETCHES_AUTO,                     ///< Use the recommended setting for the processor. In most cases, the recommended setting is enabled.
  DISABLE_SOFTWARE_PREFETCHES,                  ///< Disable software prefetches (convert software prefetch instructions to NOP).
  MAX_SOFTWARE_PREFETCH_MODE                    ///< Not a software prefetch mode, use for limit checking.
} SOFTWARE_PREFETCH_MODE;

/// Advanced performance tunings, prefetchers.
/// These settings provide for performance tuning to optimize for specific workloads.
typedef struct {
  IN HARDWARE_PREFETCH_MODE  HardwarePrefetchMode; ///< This value provides for advanced performance tuning by controlling the hardware prefetcher setting.
  IN SOFTWARE_PREFETCH_MODE  SoftwarePrefetchMode; ///< This value provides for advanced performance tuning by controlling the software prefetch instructions.
} CCX_PREFETCH_MODE;

AGESA_STATUS
CcxZenZpInitializePrefetchMode (
  IN       AMD_CONFIG_PARAMS                *StdHeader
  );

#pragma pack (pop)
#endif // _CCX_ZEN_ZP_PREFETCH_DXE_H_

