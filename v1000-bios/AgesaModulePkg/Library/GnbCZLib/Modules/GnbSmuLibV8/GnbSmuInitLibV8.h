/* $NoKeywords:$ */
/**
 * @file
 *
 * SMU services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 317069 $   @e \$Date: 2015-04-21 02:25:50 +0800 (Tue, 21 Apr 2015) $
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
#ifndef _GNBNBINITLIBV8_H_
#define _GNBNBINITLIBV8_H_

#pragma pack (push, 1)
#include "GnbPcie.h"

// Address
#define D0F0xB8_ADDRESS                                         0xb8
#define D0F0xBC_x13000000_ADDRESS                               0x13000000
#define D0F0xBC_x13000010_ADDRESS                               0x13000010
#define D0F0xBC_x13000020_ADDRESS                               0x13000020
#define D0F0xBC_x13000024_ADDRESS                               0x13000024
#define D0F0xBC_x13000028_ADDRESS                               0x13000028
#define D0F0xBC_x1300002C_ADDRESS                               0x1300002C
#define D0F0xBC_x13000030_ADDRESS                               0x13000030
#define D0F0xBC_x13000034_ADDRESS                               0x13000034

/// SMU service request contect
typedef struct {
  PCI_ADDR          GnbPciAddress;    ///< PCIe address of GNB
  UINT32            RequestId;        ///< Request/Msg ID
  UINT32            RequestArgument[6]; ///< Request/Msg Argument
} SMU_MSG_CONTEXT_V8;

#define SIZE_OF_APMWEIGHTS      (sizeof (UINT32))
/// SCS result struct
typedef struct {
  UINT64        EnergyCountConstant;  ///< copied from SCS table
  UINT64        BoostTimeConstant;    ///< copied from SCS table
  UINT32        CountApmWeights;      ///< length of APMWeights array
//  UINT32        ApmWeights[1];        ///< ApmWeights
} SMU_RAM_CPU_INFO;

/// SMU PCIe Phy/Core PG struct
typedef union {
  struct {                                                              ///<
    UINT32                                                       Rx:1 ; ///<
    UINT32                                                       Tx:1 ; ///<
    UINT32                                                     Core:1 ; ///<
    UINT32                                                  SkipPhy:1 ; ///<
    UINT32                                                 SkipCore:1 ; ///<
    UINT32                                            Reserved_15_5:11; ///<
    UINT32                                              LowerLaneID:8 ; ///<
    UINT32                                              UpperLaneID:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMU_PCIE_PHY_LN_STRUCT;
/// SMU TDP contect
typedef struct {
  UINT32            SmuTdp;        ///< TDP returned by SMU
  UINT32            cTdp;          ///< Config TDP
} SMU_TDP_INFO;

typedef struct {
  UINT32  Pstate;
  UINT32  VidAdjustment;
} F15CZ_VID_WA_PSTATE_ADJ;

#pragma pack (pop)

//BIOSSMC_MSG_LoadPort80MonitorParameters
struct BIOSSMC_MSG_LoadPort80MonitorParameters_t
{
  UINT32 MmioAddress;
  UINT32 MemoryBaseHi;
  UINT32 MemoryBaseLo;
  UINT32 MemoryBufferSize;
  UINT16 PollingInterval; // ms
  UINT8  EnableCsrShadow;
  UINT8  EnableDramShadow;
};
typedef struct BIOSSMC_MSG_LoadPort80MonitorParameters_t GNB_SMU_PORT80_PARAMS;

#define SMU_SCS_SIGNATURE STRING_TO_UINT32 ('!', 'S', 'C', 'S')
#ifndef TYPE_D0F0xBC
  #define  TYPE_D0F0xBC               0x4
#endif

/// SMU V8 basic lib
/// Generic device object
typedef struct {
  PCI_ADDR              DevPciAddress;  ///< DevPciAddress
  GNB_HANDLE            *GnbHandle;     ///< GnbHandle
  AMD_CONFIG_PARAMS     *StdHeader;     ///<
} DEV_OBJECT;

VOID
GnbSmuServiceCommonInitArgumentsV8 (
  IN       GNB_HANDLE               *GnbHandle,
  IN       AMD_CONFIG_PARAMS        *StdHeader,
  IN OUT   DEV_OBJECT               *DevObject,
  IN OUT   UINT32                   *SmuArg
  );


VOID
GnbSmuServiceRequestV8 (
  IN       DEV_OBJECT               *DevObject,
  IN       UINT32                   RequestId,
  IN OUT   UINT32                   *RequestArgument,
  IN       UINT32                   AccessFlags
  );

AGESA_STATUS
GnbSmuLoadScsDataV8 (
  IN       GNB_HANDLE           *GnbHandle,
  IN       UINT8                *ScsDataPtr,
  IN       UINT32               RequestId,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

VOID
GnbSmuServiceRequestV8S3Script (
  IN      AMD_CONFIG_PARAMS     *StdHeader,
  IN      UINT16                ContextLength,
  IN      VOID                  *Context
  );

AGESA_STATUS
GnbSmuGetScsResultV8 (
  IN       GNB_HANDLE           *GnbHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );


/// SMU V8 message interface

AGESA_STATUS
GnbSmuFirmwareTestV8 (
  IN       GNB_HANDLE               *GnbHandle,
  IN       UINT32                   RequestId,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GnbSmuRequestGpuPowerV8 (
  IN       GNB_HANDLE               *GnbHandle,
  IN       UINT32                   RequestId,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GnbSmuAcpPowerGatingV8 (
  IN       GNB_HANDLE               *GnbHandle,
  IN       UINT32                   RequestId,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

BOOLEAN
IsF15CzVidAdjustmentRequired (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#endif

