/* $NoKeywords:$ */
/**
 * @file
 *
 * NB services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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
#ifndef _GNBNBINITV4LIB_H_
#define _GNBNBINITV4LIB_H_

#pragma pack (push, 1)

/// Firmware header
typedef struct {
  UINT32  Version;                  ///< Version
  UINT32  HeaderLength;             ///< Header length
  UINT32  FirmwareLength;           ///< Firmware length
  UINT32  EntryPoint;               ///< Entry point
  UINT32  MessageDigest[5];         ///< Message digest
  UINT32  Reserved_A[3];            ///< Reserved
  UINT32  CurrentSystemState;       ///< Current system state
  UINT32  DpmCacHistory;            ///< DpmCac History
  UINT32  DpmResidencyCounters;     ///< DPM recidency counters
  UINT32  Reserved_B[16];           ///< Reserved
  UINT32  Reserved_C[16];           ///< Reserved
  UINT32  Reserved_D[16];           ///< Reserved
  UINT32  HeaderEnd;                ///< Header end signature
} FIRMWARE_HEADER_V4;

/// SMU service request contect
typedef struct {
  PCI_ADDR          GnbPciAddress;   ///< PCIe address of GNB
  UINT32            RequestId;       ///< Request/Msg ID
} SMU_MSG_CONTEXT;

#pragma pack (pop)

BOOLEAN
GnbCheckPhantomFuncSupport (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GnbGetTopologyInfoV4 (
  IN       PCI_ADDR                  StartPciAddress,
  IN       PCI_ADDR                  EndPciAddress,
     OUT   GNB_TOPOLOGY_INFO         *TopologyInfo,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  );

VOID
GnbSmuServiceRequestV4 (
  IN       PCI_ADDR                 GnbPciAddress,
  IN       UINT32                   RequestId,
  IN       UINT32                   AccessFlags,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

VOID
GnbSmuServiceRequestV4S3Script (
  IN      AMD_CONFIG_PARAMS     *StdHeader,
  IN      UINT16                ContextLength,
  IN      VOID                  *Context
  );

AGESA_STATUS
GnbSmuFirmwareLoadV4 (
  IN       PCI_ADDR                 GnbPciAddress,
  IN       FIRMWARE_HEADER_V4       *Firmware,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

PCI_ADDR
GnbGetIommuPciAddressV4 (
  IN       GNB_HANDLE               *GnbHandle,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GnbEnableIommuMmioV4 (
  IN       GNB_HANDLE           *GnbHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

#endif

