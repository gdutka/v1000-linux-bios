/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe family specific services.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 312448 $   @e \$Date: 2015-02-06 10:57:34 +0800 (Fri, 06 Feb 2015) $
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
#ifndef _GNBFAMSERVICES_H_
#define _GNBFAMSERVICES_H_

#include "Gnb.h"
#include "GnbPcie.h"
#include "GnbIommu.h"

typedef AGESA_STATUS (F_GNB_REGISTER_ACCESS) (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       UINT8                      RegisterSpaceType,
  IN       UINT32                     Address,
  IN       VOID                       *Value,
  IN       UINT32                     Flags,
  IN       AMD_CONFIG_PARAMS          *StdHeader
);

/// Register Read/Write protocol
typedef struct {
  F_GNB_REGISTER_ACCESS               *Read;    ///< Read Register
  F_GNB_REGISTER_ACCESS               *Write;   ///< Write Register
} GNB_REGISTER_SERVICE;

AGESA_STATUS
GnbFmCreateIvrsEntry (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

typedef AGESA_STATUS F_GNBFMCREATEIVRSENTRY (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

BOOLEAN
GnbFmCheckIommuPresent (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

typedef BOOLEAN F_GNBFMCHECKIOMMUPRESENT (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

/// GNB IOMMU services
typedef struct {
  F_GNBFMCHECKIOMMUPRESENT             *GnbFmCheckIommuPresent;       ///< GnbFmCheckIommuPresent
  F_GNBFMCREATEIVRSENTRY               *GnbFmCreateIvrsEntry;         ///< GnbFmCreateIvrsEntry
} GNB_FAM_IOMMU_SERVICES;


typedef UINT8 (F_PCIE_MAXPAYLOAD_SETTING) (
  IN       PCIe_ENGINE_CONFIG    *Engine
);

/// MaxPayload service protocol
typedef struct {
  F_PCIE_MAXPAYLOAD_SETTING           *SetMaxPayload;   ///< Write Register
} PCIE_MAXPAYLOAD_SERVICE;



PCI_ADDR
GnbFmGetPciAddress (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

AGESA_STATUS
GnbFmGetBusDecodeRange (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   UINT8                      *StartBusNumber,
     OUT   UINT8                      *EndBusNumber,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

AGESA_STATUS
GnbFmGetLinkId (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   UINT8                      *LinkId,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

typedef UINT32 F_GNBTIMESTAMP (
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

/// GNB Timestamp services
typedef struct {
  F_GNBTIMESTAMP                      *GnbFmTimeStamp;         ///< GnbFmTimeStamp
} GNB_FAM_TS_SERVICES;

VOID *
GnbFmAlibGetBaseTable (
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

typedef VOID * (F_GNBFMALIBGETBASETABLE) (
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

/// GNB Alib services
typedef struct {
  F_GNBFMALIBGETBASETABLE             *GnbFmAlibGetBaseTable;         ///< GnbFmAlibGetBaseTable
} GNB_FAM_ALIB_SERVICES;

VOID *
GnbFmGnbBuildOptions (
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

typedef VOID * (F_GNBFMALIBGETBUILDOPTIONS) (
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

/// GNB Build Options services
typedef struct {
  F_GNBFMALIBGETBUILDOPTIONS          *GnbFmGetBuildOptions;            ///< GnbFmBuildOptionsPtr
} GNB_FAM_BUILDOPTIONS_POINTERS;

#endif

