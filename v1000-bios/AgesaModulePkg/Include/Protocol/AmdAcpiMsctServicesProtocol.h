/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD MSCT Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Protocol
 * @e \$Revision:  $   @e \$Date:  $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_ACPI_MSCT_SERVICES_PROTOCOL_H_
#define _AMD_ACPI_MSCT_SERVICES_PROTOCOL_H_

#pragma pack (push, 1)

/// Format for MSCT Header
typedef struct {
  UINT8  Signature[4];        ///< 'MSCT' Signature for the Maximum System Characteristics Table
  UINT32 Length;              ///< Length, in bytes, of the entire MSCT
  UINT8  Revision;            ///< 1
  UINT8  Checksum;            ///< Entire table must sum to zero
  UINT8  OemId[6];            ///< OEM ID
  UINT8  OemTableId[8];       ///< For the MSCT, the table ID is the manufacturer model ID
  UINT32 OemRevision;         ///< OEM revision of MSCT for supplied OEM Table ID
  UINT8  CreatorId[4];        ///< Vendor ID of utility that created the table
  UINT32 CreatorRev;          ///< Revision of utility that created the table
  UINT32 OffsetProxDomInfo;   ///< Offset in bytes to the Proximity Domain Information Structure table entry
  UINT32 MaxProxDom;          ///< Maximum number of Proximity Domains ever possible in the system
  UINT32 MaxClkDom;           ///< Maximum number of Clock Domains ever possible in the system
  UINT64 MaxPhysAddr;         ///< Maximum Physical Address ever possible in the system
} MSCT_HEADER;

/// Format for Maximum Proximity Domain Information Structure
typedef struct {
  UINT8  Revision;            ///< 1
  UINT8  Length;              ///< 22
  UINT32 ProximityLow;        ///< Starting proximity domain for the range that this structure is providing information
  UINT32 ProximityHigh;       ///< Ending proximity domain for the range that this structure is providing information
  UINT32 MaxProcCap;          ///< Maximum Procesor Capacity of each of the Proximity Domains specified in the range
  UINT64 MaxMemCap;           ///< Maximum Memory Capacity (in bytes) of the Proximity Domains specified in the range
} MSCT_PROX_DOMAIN_INFO_STRUCT;

/// Format for Maximum Proximity Domain Structure
typedef struct {
  UINT32        ProxDomain;   ///< Proximity Domain
  UINT32        MaxProcCap;   ///< Maximum (Actual) Processor Capacity
  UINT64        MaxMemCap;    ///< Maximum (Actual) Memory Capacity
} MSCT_PROX_DOMAIN_INFO;

/// Forward declaration for AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL
typedef struct _AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AMD_MSCT_SERVICES_GET_MSCT_INFO) (
  IN       AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL  *This,
     OUT   UINT32                                  *HighestProxDomain,
     OUT   UINT32                                  *MaxNumProxDomains,
     OUT   MSCT_PROX_DOMAIN_INFO                  **MsctDomainInfo
  );

/// When installed, the MSCT Services Protocol produces a collection of
/// services related to MSCT
struct _AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL {
  UINTN                                                Revision;                    ///< Revision Number
  AMD_MSCT_SERVICES_GET_MSCT_INFO                      GetMsctInfo;                 ///< Returns information to populate MSCT maximum proximity structures
};

extern EFI_GUID gAmdFabricAcpiMsctServicesProtocolGuid;

#pragma pack (pop)
#endif // _AMD_ACPI_MSCT_SERVICES_PROTOCOL_H_
