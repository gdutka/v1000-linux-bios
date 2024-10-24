/* $NoKeywords:$ */
/**
 * @file
 *
 * Base Fabric MMIO map manager Lib implementation for DF1
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
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
#include "Filecode.h"
#include <FabricRegistersZP.h>
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdHeapLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Library/FabricResourceSizeForEachRbLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/FabricResourceReportToGcdLib.h>
#include "FabricResourceManager1.h"

#define FILECODE LIBRARY_FABRICRESOURCEMANAGERDF1LIB_FABRICRESOURCEMANAGER1_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
FabricGetRemainingSizeForThisRegion1 (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64             *Size,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  );

UINT64
FabricGetBaseAddress1 (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64              Length,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  );

EFI_STATUS
FabricGetUsedResourceSize (
  IN       FABRIC_RESOURCE_FOR_EACH_RB    *ResourceSizeForEachRb
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for allocate MMIO
 *
 * program MMIO base/limit/control registers
 *
 * @param[in, out]    BaseAddress        Starting address of the requested MMIO range.
 * @param[in, out]    Length             Length of the requested MMIO range.
 * @param[in]         Alignment          Alignment bit map.
 * @param[in]         Target             PCI bus number/Die number of the requestor.
 * @param[in, out]    Attributes         Attributes of the requested MMIO range indicating whether
 *                                       it is readable/writable/non-posted
 *
 * @retval            EFI_STATUS         EFI_OUT_OF_RESOURCES - The requested range could not be added because there are not
 *                                                              enough mapping resources.
 *                                       EFI_ABORTED          - One or more input parameters are invalid. For example, the
 *                                                              PciBusNumber does not correspond to any device in the system.
 *                                       EFI_SUCCESS          - Success to get an MMIO region
 */
EFI_STATUS
FabricAllocateMmio (
  IN OUT   UINT64 *BaseAddress,
  IN OUT   UINT64 *Length,
  IN       UINT64                 Alignment,
  IN       FABRIC_TARGET          Target,
  IN OUT   FABRIC_MMIO_ATTRIBUTE *Attributes
  )
{
  UINT8                     Socket;
  UINT8                     Rb;
  UINT8                     TempSocket;
  UINT8                     TempRb;
  UINT8                     DstFabricID;
  UINTN                     Register;
  UINT64                    SizeA;
  UINT64                    SizeB;
  UINT64                    TempSize;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_MMIO_MANAGER      *FabricMmioManager;
  FABRIC_MMIO_REGION       *MmioRegion;
  FABRIC_MMIO_REGION       *PrimaryRb2ndMmioRegion;
  EFI_STATUS                ReturnStatus;

  ReturnStatus = EFI_SUCCESS;
  *BaseAddress = 0;
  FabricMmioManager = NULL;
  // Find MMIO manager in heap
  LocateHeapParams.BufferHandle = AMD_MMIO_MANAGER;

  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *) LocateHeapParams.BufferPtr;
  }
  ASSERT (FabricMmioManager != NULL);

  if (FabricMmioManager == NULL) {
    return EFI_ABORTED;
  }
  // Check input parameters
  if (*Length == 0) {
    return EFI_SUCCESS;
  }

  if (Attributes->MmioType != NON_PCI_DEVICE_BELOW_4G) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  Warning: this function only support Attributes->MmioType = NON_PCI_DEVICE_BELOW_4G\n");
    return EFI_ABORTED;
  }

  // Find out Socket/Die
  DstFabricID = 0xFF;
  if (Target.TgtType == TARGET_PCI_BUS) {
    for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP7_REG; Register += 4) {
      BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
      if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (BusMap.Field.BusNumLimit >= Target.PciBusNum) && (BusMap.Field.BusNumBase <= Target.PciBusNum)) {
        DstFabricID = (UINT8) BusMap.Field.DstFabricID;
        DstFabricID &= FABRIC_ID_SOCKET_DIE_MASK;
        break;
      }
    }

    if (Register > CFGADDRESSMAP7_REG) {
      return EFI_ABORTED;
    }
    ASSERT (DstFabricID != 0xFF);
    Socket = (DstFabricID >> 7) & 1;
    Rb     = (DstFabricID >> 5) & 3;
  } else {
    Socket = (UINT8) (Target.SocketNum);
    Rb     = (UINT8) (Target.RbNum);
  }

  SizeA = 0;
  SizeB = 0;
  PrimaryRb2ndMmioRegion = NULL;
  if ((Attributes->MmioType == MMIO_BELOW_4G) ||
      (Attributes->MmioType == P_MMIO_BELOW_4G) ||
      (Attributes->MmioType == NON_PCI_DEVICE_BELOW_4G)) {
    //
    // Below 4G
    //

    // return EFI_ABORTED if no MMIO on this DIE
    if (FabricMmioManager->AllocateMmioBelow4GOnThisRb[Socket][Rb] == FALSE) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  ERROR: No below 4G MMIO on Socket %X RB %X\n", Socket, Rb);
      return EFI_ABORTED;
    }

    // Find out MmioRegion for this Die
    MmioRegion = &FabricMmioManager->MmioRegionBelow4G[Socket][Rb];
    FabricGetRemainingSizeForThisRegion1 (MmioRegion, &SizeA, Alignment, Attributes->MmioType);
    if (FabricMmioManager->PrimaryRbHas2ndMmioBelow4G && (Socket == 0) && (Rb == 0)) {
      // Die0 may have 2nd MmioRegion
      TempSocket = (FabricMmioManager->PrimaryRb2ndMmioPairBelow4G >> 4) & 0xF;
      TempRb = FabricMmioManager->PrimaryRb2ndMmioPairBelow4G & 0xF;
      PrimaryRb2ndMmioRegion = &FabricMmioManager->MmioRegionBelow4G[TempSocket][TempRb];
      FabricGetRemainingSizeForThisRegion1 (PrimaryRb2ndMmioRegion, &SizeB, Alignment, Attributes->MmioType);
      if ((SizeA > SizeB) && (SizeB != 0)) {
        // Make sure MmioRegion & SizeA points to the smaller one
        // PrimaryRb2ndMmioRegion & SizeB points to the larger one
        MmioRegion = PrimaryRb2ndMmioRegion;
        PrimaryRb2ndMmioRegion = &FabricMmioManager->MmioRegionBelow4G[Socket][Rb];

        TempSize = SizeA;
        SizeA = SizeB;
        SizeB = TempSize;
      }
    }
    // Try to allocate MMIO space from the smaller one first
    if (SizeA >= (*Length)) {
      *BaseAddress = FabricGetBaseAddress1 (MmioRegion, *Length, Alignment, Attributes->MmioType);
    } else if (SizeB >= (*Length)) {
      *BaseAddress = FabricGetBaseAddress1 (PrimaryRb2ndMmioRegion, *Length, Alignment, Attributes->MmioType);
    }
  } else if ((Attributes->MmioType == MMIO_ABOVE_4G) ||
             (Attributes->MmioType == P_MMIO_ABOVE_4G)) {
    //
    // Above 4G
    //
    // return EFI_ABORTED if no MMIO on this DIE
    if (FabricMmioManager->AllocateMmioAbove4GOnThisRb[Socket][Rb] == FALSE) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  ERROR: No above 4G MMIO on Socket %X RB %X\n", Socket, Rb);
      return EFI_ABORTED;
    }

    MmioRegion = &FabricMmioManager->MmioRegionAbove4G[Socket][Rb];
    FabricGetRemainingSizeForThisRegion1 (MmioRegion, &SizeA, Alignment, Attributes->MmioType);
    if (SizeA >= (*Length)) {
      // Have enough space
      *BaseAddress = FabricGetBaseAddress1 (MmioRegion, *Length, Alignment, Attributes->MmioType);
    }
  }

  if (*BaseAddress == 0) {
    *Length = (SizeA > SizeB) ? SizeA : SizeB;
    *BaseAddress = 0;
    ReturnStatus = EFI_OUT_OF_RESOURCES;
  }

  if (ReturnStatus == EFI_SUCCESS) {
    Attributes->NonPosted = 0;
    Attributes->ReadEnable = 1;
    Attributes->WriteEnable = 1;
    Attributes->CpuDis = 0;
    IDS_HDT_CONSOLE (CPU_TRACE, "  Allocate MMIO from 0x%lX ~ 0x%lX\n", *BaseAddress, (*BaseAddress + *Length - 1));
    // Check if we need to report it to GCD
    if (FabricMmioManager->ReportToGcd) {
      ReportMmioToGcd (BaseAddress, *Length, FabricMmioManager);
    }
  } else if (ReturnStatus == EFI_OUT_OF_RESOURCES) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  No enough space, the biggest MMIO size is 0x%X0000\n", (UINT32) (*Length >> 16));
    // FabricAllocateMmio is restricted for NON_PCI_DEVICE_BELOW_4G only
    // So if there's no enough space, ASSERT here
    if (Attributes->MmioType == NON_PCI_DEVICE_BELOW_4G) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Please increase PcdAmdMmioSizePerRbForNonPciDevice\n");
      ASSERT (FALSE);
    }
  }

  return ReturnStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for allocate IO
 *
 * program IO base/limit registers
 *
 * @param[in, out]    BaseAddress        Starting address of the requested MMIO range.
 * @param[in, out]    Length             Length of the requested MMIO range.
 * @param[in]         Target             PCI bus number/Die number of the requestor.
 *
 * @retval            EFI_STATUS         EFI_OUT_OF_RESOURCES - The requested range could not be added because there are not
 *                                                              enough mapping resources.
 *                                       EFI_ABORTED          - One or more input parameters are invalid. For example, the
 *                                                              PciBusNumber does not correspond to any device in the system.
 *                                       EFI_SUCCESS          - Success to get an IO region
 */
EFI_STATUS
FabricAllocateIo (
  IN OUT   UINT32 *BaseAddress,
  IN OUT   UINT32 *Length,
  IN       FABRIC_TARGET          Target
  )
{
  UINT8                     Socket;
  UINT8                     Rb;
  UINT8                     DstFabricID;
  UINTN                     Register;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_IO_MANAGER        *FabricIoManager;
  FABRIC_IO_REGION         *IoRegion;
  EFI_STATUS                ReturnStatus;

  ReturnStatus = EFI_SUCCESS;

  FabricIoManager = NULL;
  // Find MMIO manager in heap
  LocateHeapParams.BufferHandle = AMD_IO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricIoManager = (FABRIC_IO_MANAGER *) LocateHeapParams.BufferPtr;
  }
  ASSERT (FabricIoManager != NULL);

 if (FabricIoManager == NULL) {
   return EFI_OUT_OF_RESOURCES;
 }

  if (!FabricIoManager->GlobalCtrl) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Find out Socket/Die
  DstFabricID = 0xFF;
  if (Target.TgtType == TARGET_PCI_BUS) {
    for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP7_REG; Register += 4) {
      BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
      if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (BusMap.Field.BusNumLimit >= Target.PciBusNum) && (BusMap.Field.BusNumBase <= Target.PciBusNum)) {
        DstFabricID = (UINT8) BusMap.Field.DstFabricID;
        DstFabricID &= FABRIC_ID_SOCKET_DIE_MASK;
        break;
      }
    }

    if (Register > CFGADDRESSMAP7_REG) {
      return EFI_ABORTED;
    }
    ASSERT (DstFabricID != 0xFF);
    Socket = (DstFabricID >> 7) & 1;
    Rb     = (DstFabricID >> 5) & 3;
  } else {
    Socket = (UINT8) (Target.SocketNum);
    Rb     = (UINT8) (Target.RbNum);
  }

  //
  IoRegion = &FabricIoManager->IoRegion[Socket][Rb];
  if (IoRegion->IoSize >= (*Length + IoRegion->IoUsed)) {
    *BaseAddress = IoRegion->IoBase + IoRegion->IoSize - IoRegion->IoUsed - *Length;
    IoRegion->IoUsed += *Length;
    IDS_HDT_CONSOLE (CPU_TRACE, "  Allocate IO from 0x%X ~ 0x%X\n", *BaseAddress, (*BaseAddress + *Length - 1));
  } else {
    *Length = IoRegion->IoSize - IoRegion->IoUsed;
    ReturnStatus = EFI_OUT_OF_RESOURCES;
    IDS_HDT_CONSOLE (CPU_TRACE, "  No enough IO space, the biggest IO size is 0x%X\n", *Length);
  }

  return ReturnStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetAvailableResource
 *
 * Get available DF resource (MMIO/IO) for each Die
 *
 * @param[in, out]    ResourceForEachRb     Avaiable DF resource (MMIO/IO) for each Die
 *
 * @retval            EFI_SUCCESS           Success to get available resource
 *                    EFI_ABORTED           Can't get information of MMIO or IO
 */
EFI_STATUS
FabricGetAvailableResource (
  IN       FABRIC_RESOURCE_FOR_EACH_RB    *ResourceForEachRb
  )
{
  UINT8  i;
  UINT8  j;
  UINT8  SocketNumber;
  UINT8  RbNumberPerSocket;
  UINT8  TempSocket;
  UINT8  TempRb;
  UINTN  PciBase;
  UINTN  PciLimit;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_MMIO_MANAGER      *FabricMmioManager;
  FABRIC_MMIO_REGION       *MmioRegion;
  FABRIC_IO_MANAGER        *FabricIoManager;
  FABRIC_IO_REGION         *IoRegion;

  FabricMmioManager = NULL;
  FabricIoManager = NULL;

  LocateHeapParams.BufferHandle = AMD_MMIO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *) LocateHeapParams.BufferPtr;
  }

  LocateHeapParams.BufferHandle = AMD_IO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricIoManager = (FABRIC_IO_MANAGER *) LocateHeapParams.BufferPtr;
  }

  if ((FabricMmioManager == NULL) && (FabricIoManager == NULL)) {
    return EFI_ABORTED;
  }

  SocketNumber = (UINT8) FabricTopologyGetNumberOfProcessorsPresent ();
  RbNumberPerSocket = (UINT8) FabricTopologyGetNumberOfRootBridgesOnSocket (0);

  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_DIES_PER_SOCKET; j++) {
      ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
      ResourceForEachRb->PrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
      ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
      ResourceForEachRb->PrefetchableMmioSizeBelow4G[i][j].Alignment = 0;

      if ((i >= SocketNumber) || (j >= RbNumberPerSocket)) {
        // Die doesn't exist
        ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceForEachRb->PrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceForEachRb->PrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Base = 0;
        ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Base = 0;
        ResourceForEachRb->PrefetchableMmioSizeAbove4G[i][j].Base = 0;
        ResourceForEachRb->PrefetchableMmioSizeBelow4G[i][j].Base = 0;

        ResourceForEachRb->IO[i][j].Size = 0;
        ResourceForEachRb->IO[i][j].Base = 0;
      } else {
        if ((FabricMmioManager->AllocateMmioAbove4GOnThisRb[i][j]) == FALSE) {
          // No MMIO on this DIE
          ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Size = 0;
          ResourceForEachRb->PrefetchableMmioSizeAbove4G[i][j].Size = 0;
          ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Base = 0;
          ResourceForEachRb->PrefetchableMmioSizeAbove4G[i][j].Base = 0;
        } else {
          // Only report prefetchable & non-prefetchable MMIO size
          MmioRegion = &FabricMmioManager->MmioRegionAbove4G[i][j];
          ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachRb->PrefetchableMmioSizeAbove4G[i][j].Size = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
          ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachRb->PrefetchableMmioSizeAbove4G[i][j].Base = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x RB%x has non-prefetch base 0x%lX size 0x%lX align %x MMIO above 4G available\n", i, j, ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Base, ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Size, ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Alignment);
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x RB%x has prefetchable base 0x%lX size 0x%lX align %x MMIO above 4G available\n", i, j, ResourceForEachRb->PrefetchableMmioSizeAbove4G[i][j].Base, ResourceForEachRb->PrefetchableMmioSizeAbove4G[i][j].Size, ResourceForEachRb->PrefetchableMmioSizeAbove4G[i][j].Alignment);
        }

        if ((FabricMmioManager->AllocateMmioBelow4GOnThisRb[i][j]) == FALSE) {
          // No MMIO on this DIE
          ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Size = 0;
          ResourceForEachRb->PrefetchableMmioSizeBelow4G[i][j].Size = 0;
          ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Base = 0;
          ResourceForEachRb->PrefetchableMmioSizeBelow4G[i][j].Base = 0;
        } else {
          // Only report prefetchable & non-prefetchable MMIO size
          MmioRegion = &FabricMmioManager->MmioRegionBelow4G[i][j];
          ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachRb->PrefetchableMmioSizeBelow4G[i][j].Size =  MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
          ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachRb->PrefetchableMmioSizeBelow4G[i][j].Base =  MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x RB%x has non-prefetch base 0x%lX size 0x%lX align %x MMIO below 4G available\n", i, j, ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Base, ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Size, ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Alignment);
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x RB%x has prefetchable base 0x%lX size 0x%lX align %x MMIO below 4G available\n", i, j, ResourceForEachRb->PrefetchableMmioSizeBelow4G[i][j].Base, ResourceForEachRb->PrefetchableMmioSizeBelow4G[i][j].Size, ResourceForEachRb->PrefetchableMmioSizeBelow4G[i][j].Alignment);
        }

        IoRegion = &FabricIoManager->IoRegion[i][j];
        ResourceForEachRb->IO[i][j].Base = IoRegion->IoBase;
        ResourceForEachRb->IO[i][j].Size = IoRegion->IoSize - IoRegion->IoUsed;
        IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x RB%x has IO base 0x%X size 0x%X\n", i, j, ResourceForEachRb->IO[i][j].Base, ResourceForEachRb->IO[i][j].Size);

        PciBase = FabricTopologyGetHostBridgeBusBase (i, j, 0);
        PciLimit = FabricTopologyGetHostBridgeBusLimit (i, j, 0);
        ResourceForEachRb->PciBusNumber[i][j] = (UINT16) (PciLimit - PciBase + 1);
        IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x RB%x has PCI bus 0x%lX ~ 0x%lX size 0x%X\n", i, j, PciBase, PciLimit, ResourceForEachRb->PciBusNumber);
      }
    }
  }

  // Die0's 2nd MMIO
  if (FabricMmioManager->PrimaryRbHas2ndMmioBelow4G) {
    TempSocket = (FabricMmioManager->PrimaryRb2ndMmioPairBelow4G >> 4) & 0xF;
    TempRb = FabricMmioManager->PrimaryRb2ndMmioPairBelow4G & 0xF;
    MmioRegion = &FabricMmioManager->MmioRegionBelow4G[TempSocket][TempRb];

    ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
    ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
    ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
    ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Base = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
    IDS_HDT_CONSOLE (CPU_TRACE, "  Primary RB's 2nd MMIO non-prefetch base 0x%lX size 0x%lX align %x MMIO\n", ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Base, ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size, ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Alignment);
    IDS_HDT_CONSOLE (CPU_TRACE, "  Primary RB's 2nd MMIO prefetchable base 0x%lX size 0x%lX align %x MMIO\n", ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Base, ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size, ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Alignment);
  } else {
    ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size = 0;
    ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size = 0;
    ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Base = 0;
    ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Base = 0;
  }
  ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Alignment = 0;
  ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Alignment = 0;

  FabricIoManager->GlobalCtrl = FALSE;

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricReallocateResourceForEachRb
 *
 * ResourceSize = UsedSize + UserRequestSize (input)
 * Save ResourceSize to NV variable
 *
 * @param[in, out]    ResourceSizeForEachRb     Avaiable DF resource (MMIO/IO) size for each root bridge
 * @param[in, out]    SpaceStatus               Current status
 *
 * @retval            EFI_SUCCESS               Save user's request to NV variable successfully
 *                    EFI_OUT_OF_RESOURCES      No enough resource
 */
EFI_STATUS
FabricReallocateResourceForEachRb (
  IN       FABRIC_RESOURCE_FOR_EACH_RB       *ResourceSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE            *SpaceStatus
  )
{
  UINT8       i;
  UINT8       j;
  UINT8       SocketNumber;
  UINT8       RbNumberPerSocket;
  EFI_STATUS  Status;
  EFI_STATUS  CalledStatus;
  FABRIC_RESOURCE_FOR_EACH_RB CurrentUsed;

  SocketNumber = (UINT8) FabricTopologyGetNumberOfProcessorsPresent ();
  RbNumberPerSocket = (UINT8) FabricTopologyGetNumberOfDiesOnSocket (0);
  if (RbNumberPerSocket == 0) {
    RbNumberPerSocket = 1;
  }

  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_DIES_PER_SOCKET; j++) {
      if ((i >= SocketNumber) || (j >= RbNumberPerSocket)) {
        // Die doesn't exist
        ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceSizeForEachRb->IO[i][j].Size = 0;

        ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
        ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
        ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
        ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
        continue;
      }
      if (ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Size == 0) {
        ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
      }
      if (ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Size == 0) {
        ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
      }
      if (ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j].Size == 0) {
        ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
      }
      if (ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j].Size == 0) {
        ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
      }
      if (ResourceSizeForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size == 0) {
        ResourceSizeForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Alignment = 0;
      }
      if (ResourceSizeForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size == 0) {
        ResourceSizeForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Alignment = 0;
      }
    }
  }

  // Get how much space is used
  FabricGetUsedResourceSize (&CurrentUsed);

  Status = FabricSetResourceSizeForEachRb (&CurrentUsed, ResourceSizeForEachRb);

  LibAmdMemFill (SpaceStatus, 0, sizeof (FABRIC_ADDR_SPACE_SIZE), NULL);
  CalledStatus = FabricInitMmioBasedOnNvVariable1 (NULL, ResourceSizeForEachRb, SpaceStatus, SocketNumber, RbNumberPerSocket, FALSE);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  CalledStatus = FabricInitIoBasedOnNvVariable1 (NULL, ResourceSizeForEachRb, SpaceStatus, SocketNumber, RbNumberPerSocket, FALSE);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * FabricResourceRestoreDefault
 *
 * Restore default MMIO/IO distribution strategy by clearing NV variable
 *
 * @retval            EFI_SUCCESS               Success to clear NV variable
 */
EFI_STATUS
FabricResourceRestoreDefault (
  )
{
  FabricClearResourceSizeForEachRb ();

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricEnableVgaMmio
 *
 * Set VGA Enable register
 *
 * @param[in]         Target                    PCI bus number/Die number of the requestor.
 *
 * @retval            EFI_SUCCESS               Success to set VGA enable registers
 *                    EFI_ABORTED               Can't find destination
 */
EFI_STATUS
FabricEnableVgaMmio (
  IN       FABRIC_TARGET          Target
  )
{
  UINT8  i;
  UINT8  j;
  UINT8  SocketNumber;
  UINT8  RbNumberPerSocket;
  UINT8  DstFabricIDSysOffset;
  UINTN  Register;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  VGA_EN_REGISTER           VgaEn;
  MMIO_ADDR_CTRL            MmioAddrCtrlReg;

  SocketNumber = (UINT8) FabricTopologyGetNumberOfProcessorsPresent ();
  RbNumberPerSocket = (UINT8) FabricTopologyGetNumberOfDiesOnSocket (0);

  // Find out DstFabricIDSysOffset
  DstFabricIDSysOffset = 0xFF;
  if (Target.TgtType == TARGET_PCI_BUS) {
    for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP7_REG; Register += 4) {
      BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
      if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (BusMap.Field.BusNumLimit >= Target.PciBusNum) && (BusMap.Field.BusNumBase <= Target.PciBusNum)) {
        DstFabricIDSysOffset = (UINT8) BusMap.Field.DstFabricID;
        DstFabricIDSysOffset &= FABRIC_ID_SOCKET_DIE_MASK;
        break;
      }
    }

    if (Register > CFGADDRESSMAP7_REG) {
      return EFI_ABORTED;
    }
    ASSERT (DstFabricIDSysOffset != 0xFF);
  } else {
    DstFabricIDSysOffset = (UINT8) FabricTopologyGetDieSystemOffset (Target.SocketNum, Target.RbNum);
  }

  // Reset value of VgaEn_DstFabricID is 0
  // We have to get FabricID of IOMS from MMIO control register
  MmioAddrCtrlReg.Value = FabricRegisterAccRead (0, 0, 0x0, (MMIO_ADDRESS_CONTROL_REG_0), FABRIC_REG_ACC_BC);
  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < RbNumberPerSocket; j++) {
      VgaEn.Value = FabricRegisterAccRead (i, j, VGA_EN_FUNC, VGA_EN_REG, FABRIC_REG_ACC_BC);
      VgaEn.Field.VgaEn_VE = 1;
      VgaEn.Field.VgaEn_CpuDis = 0;
      VgaEn.Field.VgaEn_DstFabricID = MmioAddrCtrlReg.Field.DstFabricID & 0x1F;
      VgaEn.Field.VgaEn_DstFabricID |= DstFabricIDSysOffset;
      FabricRegisterAccWrite (i, j, VGA_EN_FUNC, VGA_EN_REG, FABRIC_REG_ACC_BC, VgaEn.Value, TRUE);
    }
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *           L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetRemainingSizeForThisRegion1
 *
 * Sub-routine of FabricAllocateMmio
 * Get remaining size of one MMIO region
 *
 * @param[in]         MmioRegion                  Point to a data structure which contains MMIO space information
 * @param[in]         Size                        Remaining size
 * @param[in]         Alignment                   Alignment bit map
 * @param[in]         MmioType                    Mmio type
 *
 */
VOID
FabricGetRemainingSizeForThisRegion1 (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64             *Size,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  )
{
  UINT64 Base;
  UINT64 BaseAligned;
  UINT64 AlignMask;

  Base = 0;

  if ((MmioType == MMIO_BELOW_4G) || (MmioType == MMIO_ABOVE_4G)) {
    Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
    *Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
  } else if ((MmioType == P_MMIO_BELOW_4G) || (MmioType == P_MMIO_ABOVE_4G)) {
    Base = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
    *Size = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
  } else if (MmioType == NON_PCI_DEVICE_BELOW_4G) {
    Base = MmioRegion->BaseNonPci + MmioRegion->UsedSizeNonPci;
    *Size = MmioRegion->SizeNonPci - MmioRegion->UsedSizeNonPci;
  } else {
    ASSERT (FALSE);
    *Size = 0;
  }

  AlignMask = Alignment;
  BaseAligned = (Base + AlignMask) & (~AlignMask);
  if ((*Size) >= (BaseAligned - Base)) {
    *Size -= BaseAligned - Base;
  } else {
    *Size = 0;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetBaseAddress1
 *
 * Sub-routine of FabricAllocateMmio
 * Get base address of a vailable MMIO space, and assign it to caller
 *
 * @param[in]         MmioRegion                  Point to a data structure which contains MMIO space information
 * @param[in]         Length                      Length of MMIO region which is required by user
 * @param[in]         Alignment                   Alignment bit map
 * @param[in]         MmioType                    Mmio type
 *
 */
UINT64
FabricGetBaseAddress1 (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64              Length,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  )
{
  UINT64 MmioBaseAddress;
  UINT64 MmioBaseAddressAligned;
  UINT64 MmioRemainingSize;
  UINT64 AlignMask;

  MmioBaseAddress = 0;
  MmioBaseAddressAligned = 0;
  MmioRemainingSize = 0;

  if ((MmioType == MMIO_BELOW_4G) || (MmioType == MMIO_ABOVE_4G)) {
    MmioBaseAddress = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
    MmioRemainingSize = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
  } else if ((MmioType == P_MMIO_BELOW_4G) || (MmioType == P_MMIO_ABOVE_4G)) {
    MmioBaseAddress = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
    MmioRemainingSize = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
  } else if (MmioType == NON_PCI_DEVICE_BELOW_4G) {
    MmioBaseAddress = MmioRegion->BaseNonPci + MmioRegion->UsedSizeNonPci;
    MmioRemainingSize = MmioRegion->SizeNonPci - MmioRegion->UsedSizeNonPci;
  }

  if ((MmioBaseAddress != 0) && (MmioRemainingSize != 0)) {
    AlignMask = Alignment;
    MmioBaseAddressAligned = (MmioBaseAddress + AlignMask) & (~AlignMask);
    if (MmioRemainingSize >= (Length + MmioBaseAddressAligned - MmioBaseAddress)) {
      if ((MmioType == MMIO_BELOW_4G) || (MmioType == MMIO_ABOVE_4G)) {
        MmioRegion->UsedSizeNonPrefetch += Length + MmioBaseAddressAligned - MmioBaseAddress;
      } else if ((MmioType == P_MMIO_BELOW_4G) || (MmioType == P_MMIO_ABOVE_4G)) {
        MmioRegion->UsedSizePrefetch += Length + MmioBaseAddressAligned - MmioBaseAddress;
      } else if (MmioType == NON_PCI_DEVICE_BELOW_4G) {
        MmioRegion->UsedSizeNonPci += Length + MmioBaseAddressAligned - MmioBaseAddress;
      }
    } else {
      MmioBaseAddressAligned = 0;
    }
  }

  return MmioBaseAddressAligned;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetUsedResourceSize
 *
 * Sub-routine of FabricReallocateResourceForEachRb
 * Get used DF resource size for each Die
 *
 * @param[in, out]    ResourceSizeForEachRb    Used Mmio size for each Die
 *
 */
EFI_STATUS
FabricGetUsedResourceSize (
  IN       FABRIC_RESOURCE_FOR_EACH_RB    *ResourceSizeForEachRb
  )
{
  UINT8  i;
  UINT8  j;
  UINT8  TempSocket;
  UINT8  TempRb;
  UINT8  SocketNumber;
  UINT8  RbNumberPerSocket;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_MMIO_MANAGER      *FabricMmioManager;
  FABRIC_MMIO_REGION       *MmioRegion;
  FABRIC_IO_MANAGER        *FabricIoManager;
  FABRIC_IO_REGION         *IoRegion;

  FabricMmioManager = NULL;
  FabricIoManager = NULL;
  LibAmdMemFill (ResourceSizeForEachRb, 0, sizeof (FABRIC_RESOURCE_FOR_EACH_RB), NULL);

  LocateHeapParams.BufferHandle = AMD_MMIO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *) LocateHeapParams.BufferPtr;
  }

  LocateHeapParams.BufferHandle = AMD_IO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricIoManager = (FABRIC_IO_MANAGER *) LocateHeapParams.BufferPtr;
  }

  if (FabricMmioManager == NULL) {
    return EFI_ABORTED;
  }
  SocketNumber = (UINT8) FabricTopologyGetNumberOfProcessorsPresent ();
  RbNumberPerSocket = (UINT8) FabricTopologyGetNumberOfDiesOnSocket (0);

  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_DIES_PER_SOCKET; j++) {
      if ((i >= SocketNumber) || (j >= RbNumberPerSocket)) {
        // Die doesn't exist
        ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceSizeForEachRb->IO[i][j].Size = 0;
      } else {
        if ((FabricMmioManager->AllocateMmioAbove4GOnThisRb[i][j]) == FALSE) {
          // No MMIO on this DIE
          ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Size = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j].Size = 0;
        } else {
          MmioRegion = &FabricMmioManager->MmioRegionAbove4G[i][j];
          ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Size = MmioRegion->UsedSizeNonPrefetch;
          ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j].Size = MmioRegion->UsedSizePrefetch;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x RB%x has non-prefetch 0x%lX, prefetchable 0x%lX MMIO above 4G used\n", i, j, ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j].Size, ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j].Size);
        }

        if ((FabricMmioManager->AllocateMmioBelow4GOnThisRb[i][j]) == FALSE) {
          // No MMIO on this DIE
          ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Size = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j].Size = 0;
        } else {
          MmioRegion = &FabricMmioManager->MmioRegionBelow4G[i][j];
          ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Size = MmioRegion->UsedSizeNonPrefetch;
          ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j].Size = MmioRegion->UsedSizePrefetch;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x RB%x has non-prefetch 0x%lX, prefetchable 0x%lX MMIO below 4G used\n", i, j, ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j].Size, ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j].Size);
        }

        IoRegion = &FabricIoManager->IoRegion[i][j];
        ResourceSizeForEachRb->IO[i][j].Size = IoRegion->IoUsed;
        IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x RB%x has 0x%X IO space used\n", i, j, ResourceSizeForEachRb->IO[i][j].Size);
      }
    }
  }

  // Die0's 2nd MMIO
  if (FabricMmioManager->PrimaryRbHas2ndMmioBelow4G) {
    TempSocket = (FabricMmioManager->PrimaryRb2ndMmioPairBelow4G >> 4) & 0xF;
    TempRb = FabricMmioManager->PrimaryRb2ndMmioPairBelow4G & 0xF;
    MmioRegion = &FabricMmioManager->MmioRegionBelow4G[TempSocket][TempRb];

    ResourceSizeForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size = MmioRegion->UsedSizeNonPrefetch;
    ResourceSizeForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size = MmioRegion->UsedSizePrefetch;
  } else {
    ResourceSizeForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size = 0;
    ResourceSizeForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size = 0;
  }

  return EFI_SUCCESS;
}


