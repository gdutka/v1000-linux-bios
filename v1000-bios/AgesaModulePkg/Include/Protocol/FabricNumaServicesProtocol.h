/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric NUMA Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 */
/*****************************************************************************
 *
 * Copyright (C) 2017-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _FABRIC_NUMA_SERVICES_PROTOCOL_H_
#define _FABRIC_NUMA_SERVICES_PROTOCOL_H_

#pragma pack (push, 1)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
 /// Domain type
typedef enum {
  SocketIntlv,
  DieIntlv,
  NoIntlv,
  MaxDomainType
} DOMAIN_TYPE;

/// Socket interleave domain
typedef struct {
  UINT32   SocketCount;   ///< Socket Count
  UINT32   SocketMap;     ///< Socket Map
} SOCKET_INTLV_DOMAIN;

/// Die interleave domain
typedef struct {
  UINT32   DieCount;      ///< Die Count
  UINT32   DieMap;        ///< Die Map
} DIE_INTLV_DOMAIN;

/// No interleave domain
typedef struct {
  UINT32   Socket;        ///< Socket
  UINT32   Die;           ///< Die
} NO_INTLV_DOMAIN;

/// Domain Info
typedef struct {
  DOMAIN_TYPE  Type;              ///< Type
  union {                         ///< Interleave
    SOCKET_INTLV_DOMAIN  Socket;  ///< Socket
    DIE_INTLV_DOMAIN     Die;     ///< Die
    NO_INTLV_DOMAIN      None;    ///< None
  } Intlv;
} DOMAIN_INFO;

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

///
/// Forward declaration for the FABRIC_NUMA_SERVICES_PROTOCOL
///
typedef struct _FABRIC_NUMA_SERVICES_PROTOCOL FABRIC_NUMA_SERVICES_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *FABRIC_NUMA_SERVICES_GET_DOMAIN_INFO) (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
     OUT   UINT32                         *NumberOfDomainsInSystem,
     OUT   DOMAIN_INFO                   **DomainInfo
  );

typedef
EFI_STATUS
(EFIAPI *FABRIC_NUMA_SERVICES_DOMAIN_XLAT) (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINTN                           Socket,
  IN       UINTN                           Die,
     OUT   UINT32                         *Domain
  );

typedef
UINT8
(EFIAPI *FABRIC_NUMA_SERVICES_GET_MAX_DOMAINS) (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This
  );

///
/// When installed, the Fabric NUMA Services Protocol produces a collection of
/// services that return various information associated with non-uniform memory
/// architecture.
///
struct _FABRIC_NUMA_SERVICES_PROTOCOL {
  UINTN                                     Revision;       ///< Revision Number
  FABRIC_NUMA_SERVICES_GET_DOMAIN_INFO      GetDomainInfo;  ///< Get Domain Info
  FABRIC_NUMA_SERVICES_DOMAIN_XLAT          DomainXlat;     ///< Domain Translation
  FABRIC_NUMA_SERVICES_GET_MAX_DOMAINS      GetMaxDomains;  ///< Get maximum proximity domain
};

extern EFI_GUID gAmdFabricNumaServicesProtocolGuid;

#pragma pack (pop)
#endif // _FABRIC_NUMA_SERVICES_PROTOCOL_H_

