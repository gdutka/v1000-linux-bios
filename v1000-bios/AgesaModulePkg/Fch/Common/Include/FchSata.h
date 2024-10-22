/* $NoKeywords:$ */
/**
 * @file
 *
 * Agesa structures and definitions
 *
 * Contains AMD AGESA core interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Include
 * @e \$Revision: 317069 $   @e \$Date: 2015-04-20 11:25:50 -0700 (Mon, 20 Apr 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/


#ifndef _FCHSATA_H_
#define _FCHSATA_H_

#pragma pack (push, 1)

///
/// SATA controller Structure
///
typedef struct {
  BOOLEAN               SataEnable;                     ///< SATA enable
  BOOLEAN               SataSetMaxGen2;                 ///< SATA enable
  BOOLEAN               SataMsiEnable;                  ///< SATA MSI capability
  UINT32                SataRaidSsid;                   ///< SATA RAID mode SSID
  UINT32                SataRaid5Ssid;                  ///< SATA RAID 5 mode SSID
  UINT32                SataAhciSsid;                   ///< SATA AHCI mode SSID

  SATA_CLASS            SataClass;                      ///< SataClass - SATA Controller mode [2:0]

  UINT8                 SataEspPort;                    ///<  SataEspPort - SATA port is external accessible on a signal only connector (eSATA:)

  UINT8                 SataPortPower;                  ///<  SataPortPower - Port Power configuration

  UINT16                SataPortMd;                     ///<  SataPortMd - Port Mode

  UINT8                 SataAggrLinkPmCap;              /// SataAggrLinkPmCap - 0:OFF   1:ON
  UINT8                 SataPortMultCap;                /// SataPortMultCap - 0:OFF   1:ON
  UINT8                 SataClkAutoOff;                 /// SataClkAutoOff - AutoClockOff 0:Disabled, 1:Enabled
  UINT8                 SataPscCap;                     /// SataPscCap 1:Enable PSC, 0:Disable PSC capability
  UINT8                 BiosOsHandOff;                  /// BiosOsHandOff - Reserved
  UINT8                 SataFisBasedSwitching;          /// SataFisBasedSwitching - Reserved
  UINT8                 SataCccSupport;                 /// SataCccSupport - Reserved
  UINT8                 SataSscCap;                     /// SataSscCap - 1:Enable, 0:Disable SSC capability
  UINT8                 SataTargetSupport8Device;       /// SataTargetSupport8Device - Reserved
  UINT8                 SataDisableGenericMode;         /// SataDisableGenericMode - Reserved
  BOOLEAN               SataAhciEnclosureManagement;    /// SataAhciEnclosureManagement - Reserved
  UINT8                 SataSgpio0;                     /// SataSgpio0 - Reserved
  UINT8                 SataSgpio1;                     /// SataSgpio1 - Reserved
  UINT8                 SataPhyPllShutDown;             /// SataPhyPllShutDown - Reserved
  BOOLEAN               SataOobDetectionEnh;            /// SataOobDetectionEnh - TRUE
  BOOLEAN               SataRasSupport;                 /// SataRasSupport - Support RAS function TRUE: Enable FALSE: Disable
  BOOLEAN               SataAhciDisPrefetchFunction;    /// SataAhciDisPrefetchFunction - Disable AHCI Prefetch Function Support
  BOOLEAN               SataDevSlpPort0;                /// SataDevSlpPort0 - Reserved
  BOOLEAN               SataDevSlpPort1;                /// SataDevSlpPort1 - Reserved
  UINT8                 SataDevSlpPort0Num;             /// SataDevSlpPort0Num - Reserved
  UINT8                 SataDevSlpPort1Num;             /// SataDevSlpPort1Num - Reserved
  UINT32                TempMmio;                       /// TempMmio - Reserved
  UINT8                 Reserved[19];
} FCH_SATA2;

#pragma pack (pop)

#endif // _FCHSATA_H_

