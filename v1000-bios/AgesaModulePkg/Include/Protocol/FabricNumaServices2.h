/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric NUMA Services Protocol V2 prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 */
/*****************************************************************************
 *
 * Copyright (C) 2018-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _FABRIC_NUMA_SERVICES2_H_
#define _FABRIC_NUMA_SERVICES2_H_

#pragma pack (push, 1)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// Domain type
typedef enum {
  NumaDram,
  NumaSLink,
  MaxNumaDomainType2
} DOMAIN_TYPE2;

/// Reported Domain Info
typedef struct {
  DOMAIN_TYPE2  Type;            ///< Type
  UINT32        SocketMap;       ///< Bitmap indicating physical socket location
  UINT32        PhysicalDomain;  ///< Physical domain number
} DOMAIN_INFO2;

/// Physical Dram Info
typedef struct {
  UINT32   NormalizedCsMap;                   ///< Bitmap of CSs comprising this physical domain
  UINT32   SharingEntityCount;                ///< Number of virtual domains sharing this physical domain
  UINT32   SharingEntityMap;                  ///< Bitmap of reported domains that share this physical domain
  UINT32   Reserved;                          ///< Reserved
} PHYS_DOMAIN_INFO;

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

///
/// Forward declaration for the FABRIC_NUMA_SERVICES2_PROTOCOL
///
typedef struct _FABRIC_NUMA_SERVICES2_PROTOCOL FABRIC_NUMA_SERVICES2_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *FABRIC_NUMA_SERVICES2_GET_DOMAIN_INFO) (
  IN       FABRIC_NUMA_SERVICES2_PROTOCOL  *This,
     OUT   UINT32                          *NumberOfDomainsInSystem,
     OUT   DOMAIN_INFO2                   **DomainInfo,
     OUT   BOOLEAN                         *CcxAsNumaDomain
  );

typedef
EFI_STATUS
(EFIAPI *FABRIC_NUMA_SERVICES2_DOMAIN_XLAT) (
  IN       FABRIC_NUMA_SERVICES2_PROTOCOL  *This,
  IN       UINTN                            Socket,
  IN       UINTN                            Die,
  IN       UINTN                            Ccd,
  IN       UINTN                            Ccx,
     OUT   UINT32                          *Domain
  );

typedef
EFI_STATUS
(EFIAPI *FABRIC_NUMA_SERVICES2_GET_PHYSICAL_DOMAIN_INFO) (
  IN       FABRIC_NUMA_SERVICES2_PROTOCOL  *This,
     OUT   UINT32                          *NumberOfPhysDomainsInSystem,
     OUT   PHYS_DOMAIN_INFO               **PhysDomainInfo,
     OUT   UINT32                          *PhysNodesPerSocket,
     OUT   UINT32                          *NumberOfSystemSLinkDomains
  );

///
/// When installed, the Fabric NUMA Services 2 Protocol produces a collection of
/// services that return various information associated with non-uniform memory
/// architecture.
///
struct _FABRIC_NUMA_SERVICES2_PROTOCOL {
  UINTN                                           Revision;           ///< Revision Number
  FABRIC_NUMA_SERVICES2_GET_DOMAIN_INFO           GetDomainInfo;      ///< Get Domain Info
  FABRIC_NUMA_SERVICES2_DOMAIN_XLAT               DomainXlat;         ///< Domain Translation
  FABRIC_NUMA_SERVICES2_GET_PHYSICAL_DOMAIN_INFO  GetPhysDomainInfo;  ///< Get Physical Domain Info
};

extern EFI_GUID gAmdFabricNumaServices2ProtocolGuid;

#pragma pack (pop)
#endif // _FABRIC_NUMA_SERVICES2_H_

