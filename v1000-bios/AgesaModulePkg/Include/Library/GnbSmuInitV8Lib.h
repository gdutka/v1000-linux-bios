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
#ifndef _GNBNBINITV8LIB_H_
#define _GNBNBINITV8LIB_H_

#pragma pack (push, 1)
#include  <AMD.h>
#include  <GnbPcie.h>
#include  <Library/GnbUraServicesLib.h>

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

#pragma pack (pop)

#define SMU_SCS_SIGNATURE STRING_TO_UINT32 ('!', 'S', 'C', 'S')
#ifndef TYPE_D0F0xBC
  #define  TYPE_D0F0xBC               0x4
#endif

/// SMU V8 basic lib

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

#endif

