/* $NoKeywords:$ */
/**
 * @file
 *
 * Local Function Prototype Header File
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 312288 $   @e \$Date: 2015-02-04 00:39:01 -0600 (Wed, 04 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
#ifndef _AMDPSPPRV_H_
#define _AMDPSPPRV_H_

#define SmartDevice         0
#define NonSmartDevice      1
#define DumbDevice          2



/// Pcie PSPP Modes
typedef enum {
  DxioPsppPerformance = 0,   ///< @todo Style_Analyzer: Add Doxygen comments to enum entry
  DxioPsppBalanced,          ///< @todo Style_Analyzer: Add Doxygen comments to enum entry
  DxioPsppPowerSaving,       ///< @todo Style_Analyzer: Add Doxygen comments to enum entry
} DXIO_PSPP_POLICY;


#define DXIO_MSG_RNTM_CHANGESPEEDPERPSPPPOLICY  74
#define DXIO_MSG_PHYS_TARGETFREQUPDATE          0x10


#define DXIO_freqUpdType_restoreOnly            0 /*!< use previously saved values and do not apply per-lane resets. */
#define DXIO_freqUpdType_all                    1 /*!< update all engines of the given type */
#define DXIO_freqUpdType_single                 2 /*!< update single engine of the given type */

/// @todo Style_Analyzer: Add Doxygen comment to union construct
typedef union {
  struct {
    UINT16    VendorId;                                               ///< @todo Style_Analyzer: Add Doxygen comments to enum entry 
    UINT16    DeviceId;                                               ///< @todo Style_Analyzer: Add Doxygen comments to enum entry 
  } Field;                                                            ///< @todo Style_Analyzer: Add Doxygen comments to enum entry 
  UINT32 Value;
} VEN_DEV_ID;


VOID
AmdPsppConfig (
  IN       PCIe_PLATFORM_CONFIG       *Pcie
  );

#endif

