/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe ALIB
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
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

#ifndef _PCIEALIBV2_H_
#define _PCIEALIBV2_H_

#pragma pack (push, 1)

/// _ALIB_PORT_DATA structure
typedef struct {
  UINT8                   PciePortMaxSpeed;                 ///< Maximum speed for PCIe Port
  UINT8                   PciePortAcSpeed;                  ///< AC power speed for PCIe Port
  UINT8                   PciePortDcSpeed;                  ///< DC power speed for PCIe Port
  UINT8                   PciePortCurSpeed;                 ///< Current speed for PCIe Port
  UINT8                   PcieSbPort;                       ///< Sb Port
  UINT8                   PcieLinkSafeMode;                 ///< Link Safe Mode
  UINT8                   PcieLocalOverrideSpeed;           ///< Local Override Speed
  UINT8                   StartPhyLane;                     ///< Start PHY lane
  UINT8                   EndPhyLane;                       ///< End PHY lane
  UINT8                   StartCoreLane;                    ///< Start CORE lane
  UINT8                   EndCoreLane;                      ///< End Core lane
  UINT8                   PortId;                           ///< Logical Port ID
  UINT8                   LinkHotplug;                      ///< Hotplug Flags
  UINT8                   PciDev;                           ///< PCI Device Number
  UINT8                   PciFun;                           ///< PCI Function Number
} _ALIB_PORT_DATA;

/// Set size for Port Data
typedef union {
  _ALIB_PORT_DATA         PortData;                         ///< Port data for ALIB
  UINT8                   Padding[20];                      ///< Padding size
} ALIB_PORT_DATA;

/// _ALIB_GLOBAL_DATA structure
typedef  struct {
  UINT8                 PsppPolicy;                       ///< PSPP Policy
  UINT8                 PcieVidGen1;                      ///< VID for Gen1
  UINT8                 PcieVidGen2;                      ///< VID for Gen2
  UINT8                 PcieVidGen3;                      ///< VID for Gen3
  UINT8                 DpmMask;                          ///< DPM Mask
  UINT8                 NumBoostStates;                   ///< DPM States
  UINT8                 DockedTdpHeadroom;                ///< Docked TDP Headroom
  UINT8                 DcTdpEnable;                      ///< Dc TDP Enable
} _ALIB_GLOBAL_DATA;

/// Set size for Global Data
typedef union {
  _ALIB_GLOBAL_DATA       Data;                             ///< Global data struct for ALIB
  UINT8                   Padding[32];                      ///< Padding size
} ALIB_GLOBAL_DATA;

/// ALIB_GLOBAL_DATA structure
typedef struct {
  ALIB_GLOBAL_DATA        Data;                             ///< Global data struct for ALIB
  ALIB_PORT_DATA          PortData[12];                     ///< Padding size
} ALIB_DATA;


#pragma pack (pop)

AGESA_STATUS
PcieAlibV2Feature (
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

#endif

