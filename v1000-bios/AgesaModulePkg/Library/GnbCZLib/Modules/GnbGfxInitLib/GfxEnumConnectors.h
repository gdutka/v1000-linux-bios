/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to initialize Integrated Info Table
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
#ifndef _GFXENUMCONNECTORS_H_
#define _GFXENUMCONNECTORS_H_

/// External connector info
typedef struct {
  PCIE_CONNECTOR_TYPE ConnectorType;          ///<
  UINT8               DisplayDeviceEnum;      ///<
  UINT16              ConnectorEnum;          ///<
  UINT16              EncoderEnum;            ///<
  UINT8               ConnectorIndex;         ///<
} EXT_CONNECTOR_INFO;

/// External display device info
typedef struct {
  UINT8               DisplayDeviceEnum;      ///<
  UINT8               DeviceIndex;            ///<
  UINT16              DeviceTag;              ///<
  UINT16              DeviceAcpiEnum;         ///<
} EXT_DISPLAY_DEVICE_INFO;

/// Connector enumeration info
typedef struct {
  AGESA_STATUS        Status;                 ///<
  UINT8               DisplayDeviceEnum;      ///<
  UINT8               RequestedPriorityIndex; ///<
  UINT8               CurrentPriorityIndex;   ///<
  PCIe_ENGINE_CONFIG  *Engine;                ///<
} CONNECTOR_ENUM_INFO;

VOID
GfxIntegratedCopyDisplayInfo (
  IN       PCIe_ENGINE_CONFIG             *Engine,
     OUT   EXT_DISPLAY_PATH               *DisplayPath,
     OUT   EXT_DISPLAY_PATH               *SecondaryDisplayPath,
  IN       GFX_PLATFORM_CONFIG            *Gfx
  );

AGESA_STATUS
GfxIntegratedEnumerateAllConnectors (
     OUT   EXT_DISPLAY_PATH                 *DisplayPathList,
  IN OUT   PCIe_PLATFORM_CONFIG             *Pcie,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  );

EXT_CONNECTOR_INFO*
GfxIntegratedExtConnectorInfo (
  IN       UINT8                         ConnectorType
  );

EXT_DISPLAY_DEVICE_INFO*
GfxIntegratedExtDisplayDeviceInfo (
  IN       UINT8                         DisplayDeviceEnum,
  IN       UINT8                         DisplayDeviceIndex
  );


#endif

