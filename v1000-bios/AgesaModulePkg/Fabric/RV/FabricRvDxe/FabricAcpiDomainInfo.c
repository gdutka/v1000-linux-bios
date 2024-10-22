/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric ACPI Domain infor for SRAT & CDIT.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "AMD.h"
#include "FabricAcpiDomainInfo.h"
#include "FabricAcpiTable.h"
#include <Library/BaseLib.h>
#include "Library/AmdBaseLib.h"
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersRV.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include "Library/UefiBootServicesTableLib.h"
#include <Protocol/FabricNumaServicesProtocol.h>
#include "Filecode.h"


#define FILECODE FABRIC_RV_FABRICRVDXE_FABRICACPIDOMAININFO_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
BuildDomainInfo (
  VOID
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

BOOLEAN      mDomainInfoValid = FALSE;
UINT32       mNumberOfDomains = 0;
DOMAIN_INFO  mDomainInfo[MAX_SOCKETS_RV * MAX_DIES_PER_SOCKET_RV];
DIE_INFO     mDieInfo[MAX_SOCKETS_RV][MAX_DIES_PER_SOCKET_RV];

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricRvGetDomainInfo (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
     OUT   UINT32                         *NumberOfDomainsInSystem,
     OUT   DOMAIN_INFO                   **DomainInfo
  );

EFI_STATUS
EFIAPI
FabricRvDomainXlat (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINTN                           Socket,
  IN       UINTN                           Die,
     OUT   UINT32                         *Domain
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
STATIC FABRIC_NUMA_SERVICES_PROTOCOL  mFabricNumaServicesProtocol = {
  0x1,
  FabricRvGetDomainInfo,
  FabricRvDomainXlat
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricRvNumaServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  BuildDomainInfo ();
  return (gBS->InstallProtocolInterface (
                &ImageHandle,
                &gAmdFabricNumaServicesProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mFabricNumaServicesProtocol
                ));
}

EFI_STATUS
EFIAPI
FabricRvGetDomainInfo (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
     OUT   UINT32                         *NumberOfDomainsInSystem,
     OUT   DOMAIN_INFO                   **DomainInfo
  )
{
  ASSERT (mDomainInfoValid);

  if ((NumberOfDomainsInSystem == NULL) && (DomainInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfDomainsInSystem != NULL) {
    *NumberOfDomainsInSystem = mNumberOfDomains;
  }

  if (DomainInfo != NULL) {
    *DomainInfo = &mDomainInfo[0];
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FabricRvDomainXlat (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINTN                           Socket,
  IN       UINTN                           Die,
     OUT   UINT32                         *Domain
  )
{
  ASSERT (mDomainInfoValid);

  if (Socket >= FabricTopologyGetNumberOfProcessorsPresent ()) {
    return EFI_INVALID_PARAMETER;
  }

  if (Die >= FabricTopologyGetNumberOfDiesOnSocket (Socket)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Domain == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Domain = mDieInfo[Socket][Die].Domain;
  return EFI_SUCCESS;
}

VOID
BuildDomainInfo (
  VOID
  )
{
  UINTN                        NumberOfSockets;
  UINTN                        NumberOfDiesOnSocket;
  UINT32                       i;
  UINT32                       j;
  UINT32                       k;
  UINT32                       MapPairIndex;
  UINT32                       DiesAccountedForInDomain[(MAX_SOCKETS_RV * MAX_DIES_PER_SOCKET_RV)];
  UINT32                       TotalEntities[(MAX_SOCKETS_RV * MAX_DIES_PER_SOCKET_RV)];
  DRAM_INFO                    DramInfo[NUMBER_OF_DRAM_REGIONS];
  DRAM_BASE_ADDRESS_REGISTER   DramBaseAddr;
  DRAM_LIMIT_ADDRESS_REGISTER  DramLimitAddr;

  ASSERT (!mDomainInfoValid);
  ASSERT (mNumberOfDomains == 0);

  // Collect raw data
  for (i = 0; i < NUMBER_OF_DRAM_REGIONS; i++) {
    DramBaseAddr.Value = FabricRegisterAccRead (0, 0, DRAMBASEADDR0_FUNC, (DRAMBASEADDR0_REG + (i * DRAM_REGION_REGISTER_OFFSET)), RV_IOMS0_INSTANCE_ID);
    if (DramBaseAddr.Field.AddrRngVal == 1) {
      DramLimitAddr.Value = FabricRegisterAccRead (0, 0, DRAMLIMITADDR0_FUNC, (DRAMLIMITADDR0_REG + (i * DRAM_REGION_REGISTER_OFFSET)), RV_IOMS0_INSTANCE_ID);
      DramInfo[i].Socket = ((DramLimitAddr.Field.DstFabricID >> FABRIC_ID_SOCKET_SHIFT) & FABRIC_ID_SOCKET_SIZE_MASK);
      DramInfo[i].Die = ((DramLimitAddr.Field.DstFabricID >> FABRIC_ID_DIE_SHIFT) & FABRIC_ID_DIE_SIZE_MASK);
      DramInfo[i].SocketMask = 0xFFFFFFFF << DramLimitAddr.Field.IntLvNumSockets;
      DramInfo[i].DieMask = 0xFFFFFFFF << DramLimitAddr.Field.IntLvNumDies;
      DramInfo[i].SocketCount = 1 << DramLimitAddr.Field.IntLvNumSockets;
      DramInfo[i].DieCount = 1 << DramLimitAddr.Field.IntLvNumDies;
    } else {
      DramInfo[i].SocketCount = 0;
      DramInfo[i].DieCount = 0;
      DramInfo[i].Socket = 0xFF;
      DramInfo[i].Die = 0xFF;
      DramInfo[i].SocketMask = 0;
      DramInfo[i].DieMask = 0;
    }
  }

  // Initialize domain info to none
  for (i = 0; i < (MAX_SOCKETS_RV * MAX_DIES_PER_SOCKET_RV); i++) {
    DiesAccountedForInDomain[i] = 0;
    TotalEntities[i] = 0;
    mDomainInfo[i].Type = MaxDomainType;
  }

  // Initialize die info to none
  for (i = 0; i < MAX_SOCKETS_RV; i++) {
    for (j = 0; j < MAX_DIES_PER_SOCKET_RV; j++) {
      mDieInfo[i][j].Domain = 0xFF;
      for (k = 0; k < MAX_CHANNELS_PER_DIE; k++) {
        mDieInfo[i][j].DramMapPair[k] = 0xFF;
      }
    }
  }

  // Build die and domain info
  NumberOfSockets = FabricTopologyGetNumberOfProcessorsPresent ();
  ASSERT (NumberOfSockets <= MAX_SOCKETS_RV);
  for (i = 0; i < NumberOfSockets; i++) {
    NumberOfDiesOnSocket = FabricTopologyGetNumberOfDiesOnSocket (i);
    ASSERT (NumberOfDiesOnSocket <= MAX_DIES_PER_SOCKET_RV);
    for (j = 0; j < NumberOfDiesOnSocket; j++) {
      mDieInfo[i][j].Domain = mNumberOfDomains;
      DiesAccountedForInDomain[mNumberOfDomains]++;
      MapPairIndex = 0;
      for (k = 0; k < NUMBER_OF_DRAM_REGIONS; k++) {
        if ((DramInfo[k].Socket == (i & DramInfo[k].SocketMask)) && (DramInfo[k].Die == (j & DramInfo[k].DieMask))) {
          ASSERT (MapPairIndex < MAX_CHANNELS_PER_DIE);
          mDieInfo[i][j].DramMapPair[MapPairIndex++] = k;
        }
      }
      if (MapPairIndex != 1)  {
        mDomainInfo[mNumberOfDomains].Type = NoIntlv;
        mDomainInfo[mNumberOfDomains].Intlv.None.Socket = i;
        mDomainInfo[mNumberOfDomains].Intlv.None.Die = j;
        TotalEntities[mNumberOfDomains] = 1;
      } else if (DiesAccountedForInDomain[mNumberOfDomains] == 1) {
        if (DramInfo[mDieInfo[i][j].DramMapPair[0]].SocketCount > 1) {
          mDomainInfo[mNumberOfDomains].Type = SocketIntlv;
          mDomainInfo[mNumberOfDomains].Intlv.Socket.SocketCount = DramInfo[mDieInfo[i][j].DramMapPair[0]].SocketCount;
          TotalEntities[mNumberOfDomains] = mDomainInfo[mNumberOfDomains].Intlv.Socket.SocketCount * DramInfo[mDieInfo[i][j].DramMapPair[0]].DieCount;
          mDomainInfo[mNumberOfDomains].Intlv.Socket.SocketMap |= (1 << i);
        } else if (DramInfo[mDieInfo[i][j].DramMapPair[0]].DieCount > 1) {
          mDomainInfo[mNumberOfDomains].Type = DieIntlv;
          mDomainInfo[mNumberOfDomains].Intlv.Die.DieCount = DramInfo[mDieInfo[i][j].DramMapPair[0]].DieCount;
          TotalEntities[mNumberOfDomains] = mDomainInfo[mNumberOfDomains].Intlv.Die.DieCount;
          mDomainInfo[mNumberOfDomains].Intlv.Die.DieMap |= (1 << j);
        } else {
          mDomainInfo[mNumberOfDomains].Type = NoIntlv;
          TotalEntities[mNumberOfDomains] = 1;
          mDomainInfo[mNumberOfDomains].Intlv.None.Socket = i;
          mDomainInfo[mNumberOfDomains].Intlv.None.Die = j;
        }
      }
      if (DiesAccountedForInDomain[mNumberOfDomains] == TotalEntities[mNumberOfDomains]) {
        mNumberOfDomains++;
      }
    }
  }

  mDomainInfoValid = TRUE;
}


