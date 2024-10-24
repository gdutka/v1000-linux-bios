/** @file

;******************************************************************************
;* Copyright (c) 2015 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  Provides the basic interfaces to abstract a PCI Host Bridge Resource Allocation

Copyright (c) 2008 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are
licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PciHostBridge.h"
#include <Library/FabricResourceManagerLib.h>
#include "ApcbCommon.h"
#include "Protocol/AmdApcbProtocol.h"

extern EFI_GUID gAmdApcbDxeServiceProtocolGuid;

//
// Support 64 K IO space
//
#define RES_IO_BASE   0x2000
#define RES_IO_LIMIT  0x10000

//
// Support 4G address space
//
#define IommuCapId  0x0F

GLOBAL_REMOVE_IF_UNREFERENCED UINT8   mEnableAbove4GBMmioBiosAssignemnt = 0x0;

EFI_STATUS
UpdateLargeMemoryInfo (
  IN  EFI_PHYSICAL_ADDRESS  BaseAddress,
  IN  UINT64                Length
);

EFI_STATUS
UpdateLargeMemoryAcpiInfo (
  VOID
);

VOID
GetBaseSizeOfThisMmioRegion (
  IN  FABRIC_ADDR_APERTURE *Prefetch,
  IN  FABRIC_ADDR_APERTURE *NonPrefetch,
  IN  UINT64               *Base,
  IN  UINT64               *Size
  );

//
// Hard code: Root Bridge Number within the host bridge
//            Root Bridge's attribute
//            Root Bridge's device path
//            Root Bridge's resource appeture
//
UINTN RootBridgeNumber[1] = { 1 };

UINT64 RootBridgeAttribute[1][1] = { { EFI_PCI_HOST_BRIDGE_MEM64_DECODE } };

EFI_PCI_ROOT_BRIDGE_DEVICE_PATH mEfiPciRootBridgeDevicePath[1][1] = {
  {
    {{{ACPI_DEVICE_PATH,
    ACPI_DP,
    {(UINT8) (sizeof(ACPI_HID_DEVICE_PATH)),
    (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)}},
    EISA_PNP_ID(0x0A03),
    0},
    {END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {END_DEVICE_PATH_LENGTH,
    0}}
  }}
};

PCI_ROOT_BRIDGE_RESOURCE_APPETURE  mResAppeture[1][1] = {
  {{0, 0xff, 0x00000000, 0xffffffffffffffff, 0, 1 << 16}}
};

EFI_HANDLE mDriverImageHandle;

PCI_HOST_BRIDGE_INSTANCE mPciHostBridgeInstanceTemplate = {
  PCI_HOST_BRIDGE_SIGNATURE,  // Signature
  NULL,                       // HostBridgeHandle
  0,                          // RootBridgeNumber
  {NULL, NULL},               // Head
  FALSE,                      // ResourceSubiteed
  TRUE,                       // CanRestarted
  {
    NotifyPhase,
    GetNextRootBridge,
    GetAttributes,
    StartBusEnumeration,
    SetBusNumbers,
    SubmitResources,
    GetProposedResources,
    PreprocessController
  }
};

/**
  Entry point of this driver

  @param ImageHandle     Handle of driver image
  @param SystemTable     Point to EFI_SYSTEM_TABLE

  @retval EFI_OUT_OF_RESOURCES  Can not allocate memory resource
  @retval EFI_DEVICE_ERROR      Can not install the protocol instance
  @retval EFI_SUCCESS           Success to initialize the Pci host bridge.
**/
EFI_STATUS
EFIAPI
InitializePciHostBridge (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                  Status;
  UINTN                       Loop1;
  UINTN                       Loop2;
  PCI_HOST_BRIDGE_INSTANCE    *HostBridge;
  PCI_ROOT_BRIDGE_INSTANCE    *PrivateData;
  EFI_PHYSICAL_ADDRESS        BaseAddress;
  UINT64                      Length;
  FABRIC_RESOURCE_FOR_EACH_RB   MmioForEachDie;
  FABRIC_TARGET               Target;
  EFI_PHYSICAL_ADDRESS        TempAddress;

  TempAddress = 0;
  mDriverImageHandle = ImageHandle;
  PrivateData = NULL;

  if (PcdGet8 (PcdMmioMem64Enable)) {
    mEnableAbove4GBMmioBiosAssignemnt = 1;
  }
  //
  // Create Host Bridge Device Handle
  //
  for (Loop1 = 0; Loop1 < HOST_BRIDGE_NUMBER; Loop1++) {
    HostBridge = AllocateCopyPool (sizeof(PCI_HOST_BRIDGE_INSTANCE), &mPciHostBridgeInstanceTemplate);
    if (HostBridge == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    HostBridge->RootBridgeNumber = RootBridgeNumber[Loop1];
    InitializeListHead (&HostBridge->Head);

    Status = gBS->InstallMultipleProtocolInterfaces (
                    &HostBridge->HostBridgeHandle,
                    &gEfiPciHostBridgeResourceAllocationProtocolGuid, &HostBridge->ResAlloc,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      FreePool (HostBridge);
      return EFI_DEVICE_ERROR;
    }

    //
    // Create Root Bridge Device Handle in this Host Bridge
    //

    for (Loop2 = 0; Loop2 < HostBridge->RootBridgeNumber; Loop2++) {
      PrivateData = AllocateZeroPool (sizeof(PCI_ROOT_BRIDGE_INSTANCE));
      if (PrivateData == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      PrivateData->Signature = PCI_ROOT_BRIDGE_SIGNATURE;
      PrivateData->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&mEfiPciRootBridgeDevicePath[Loop1][Loop2];

      RootBridgeConstructor (
        &PrivateData->Io,
        HostBridge->HostBridgeHandle,
        RootBridgeAttribute[Loop1][Loop2],
        &mResAppeture[Loop1][Loop2]
        );

      Status = gBS->InstallMultipleProtocolInterfaces(
                      &PrivateData->Handle,
                      &gEfiDevicePathProtocolGuid,      PrivateData->DevicePath,
                      &gEfiPciRootBridgeIoProtocolGuid, &PrivateData->Io,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        FreePool(PrivateData);
        return EFI_DEVICE_ERROR;
      }

      InsertTailList (&HostBridge->Head, &PrivateData->Link);
    }
  }

  // Get available resource
  FabricGetAvailableResource (&MmioForEachDie);
  //
  // IO-Space mapping
  //
  if ((MmioForEachDie.IO[0][0].Base + MmioForEachDie.IO[0][0].Size) > 0x10000) {
    Length = 0x10000 - MmioForEachDie.IO[0][0].Base;
  } else {
    Length = MmioForEachDie.IO[0][0].Size;
  }
  DEBUG ((DEBUG_INFO, " Add IO space from 0x%X to 0x%X\n", MmioForEachDie.IO[0][0].Base, MmioForEachDie.IO[0][0].Base + Length - 1));
  Status = gDS->AddIoSpace ( EfiGcdIoTypeIo,
                             MmioForEachDie.IO[0][0].Base,
                             Length );
  ASSERT_EFI_ERROR (Status);

  //
  // MMIO-Space mapping
  //

  // Below 4G
  // 1st MMIO
  GetBaseSizeOfThisMmioRegion (&MmioForEachDie.PrefetchableMmioSizeBelow4G[0][0], &MmioForEachDie.NonPrefetchableMmioSizeBelow4G[0][0], &BaseAddress, &Length);
  if (Length != 0) {
    TempAddress = BaseAddress + Length;
    if (FeaturePcdGet (PcdH2ODdtSupported) && BaseAddress == 0xFC000000) {
      BaseAddress += 0x080000;
      Length -= 0x080000;
    }

    DEBUG ((DEBUG_INFO, " Add MMIO space from 0x%X to 0x%X\n", (UINT32)BaseAddress, (UINT32)(BaseAddress + Length - 1)));
    Status = gDS->AddMemorySpace ( EfiGcdMemoryTypeMemoryMappedIo,
                                   BaseAddress,
                                   Length,
                                   0 );
//[-start-231127-IB19090069-modify]//
    if (Status == EFI_ACCESS_DENIED) {
      //
      // Do nothing. AddMemorySpace return EFI_ACCESS_DENIED here means this space is already added before.
      //
    } else if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
    }
//[-end-231127-IB19090069-modify]//
  }
  // 2nd MMIO
  GetBaseSizeOfThisMmioRegion (&MmioForEachDie.PrimaryRbSecondPrefetchableMmioSizeBelow4G, &MmioForEachDie.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G, &BaseAddress, &Length);
  if (Length != 0) {
    if (FeaturePcdGet (PcdH2ODdtSupported) && BaseAddress == 0xFC000000) {
      BaseAddress += 0x080000;
      Length -= 0x080000;
    }

    DEBUG ((DEBUG_INFO, " Add MMIO space from 0x%X to 0x%X\n", (UINT32)BaseAddress, (UINT32)(BaseAddress + Length - 1)));
    Status = gDS->AddMemorySpace ( EfiGcdMemoryTypeMemoryMappedIo,
                                   BaseAddress,
                                   Length,
                                   0 );
//[-start-231127-IB19090069-modify]//
    if (Status == EFI_ACCESS_DENIED) {
      //
      // Do nothing. AddMemorySpace return EFI_ACCESS_DENIED here means this space is already added before.
      //
    } else if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
    }
//[-end-231127-IB19090069-modify]//
  }

  // Above 4G
  if (mEnableAbove4GBMmioBiosAssignemnt) {
    GetBaseSizeOfThisMmioRegion (&MmioForEachDie.PrefetchableMmioSizeAbove4G[0][0], &MmioForEachDie.NonPrefetchableMmioSizeAbove4G[0][0], &BaseAddress, &Length);
    if (Length != 0) {
      DEBUG ((DEBUG_INFO, " Add MMIO space from 0x%LX to 0x%LX\n", BaseAddress, BaseAddress + Length - 1));
      Status = gDS->AddMemorySpace ( EfiGcdMemoryTypeMemoryMappedIo,
                                     BaseAddress,
                                     Length,
                                     0 );
//[-start-231127-IB19090069-modify]//
      if (Status == EFI_ACCESS_DENIED) {
        //
        // Do nothing. AddMemorySpace return EFI_ACCESS_DENIED here means this space is already added before.
        //
      } else if (EFI_ERROR (Status)) {
        ASSERT_EFI_ERROR (Status);
      }
//[-end-231127-IB19090069-modify]//
    }
  }

  //
  // VGA
  //
  Target.TgtType = TARGET_RB;
  Target.SocketNum = 0;
  Target.RbNum = 0;
  FabricEnableVgaMmio (Target);

  return EFI_SUCCESS;
}


/**
   These are the notifications from the PCI bus driver that it is about to enter a certain
   phase of the PCI enumeration process.

   This member function can be used to notify the host bridge driver to perform specific actions,
   including any chipset-specific initialization, so that the chipset is ready to enter the next phase.
   Eight notification points are defined at this time. See belows:
   EfiPciHostBridgeBeginEnumeration       Resets the host bridge PCI apertures and internal data
                                          structures. The PCI enumerator should issue this notification
                                          before starting a fresh enumeration process. Enumeration cannot
                                          be restarted after sending any other notification such as
                                          EfiPciHostBridgeBeginBusAllocation.
   EfiPciHostBridgeBeginBusAllocation     The bus allocation phase is about to begin. No specific action is
                                          required here. This notification can be used to perform any
                                          chipset-specific programming.
   EfiPciHostBridgeEndBusAllocation       The bus allocation and bus programming phase is complete. No
                                          specific action is required here. This notification can be used to
                                          perform any chipset-specific programming.
   EfiPciHostBridgeBeginResourceAllocation
                                          The resource allocation phase is about to begin. No specific
                                          action is required here. This notification can be used to perform
                                          any chipset-specific programming.
   EfiPciHostBridgeAllocateResources      Allocates resources per previously submitted requests for all the PCI
                                          root bridges. These resource settings are returned on the next call to
                                          GetProposedResources(). Before calling NotifyPhase() with a Phase of
                                          EfiPciHostBridgeAllocateResource, the PCI bus enumerator is responsible
                                          for gathering I/O and memory requests for
                                          all the PCI root bridges and submitting these requests using
                                          SubmitResources(). This function pads the resource amount
                                          to suit the root bridge hardware, takes care of dependencies between
                                          the PCI root bridges, and calls the Global Coherency Domain (GCD)
                                          with the allocation request. In the case of padding, the allocated range
                                          could be bigger than what was requested.
   EfiPciHostBridgeSetResources           Programs the host bridge hardware to decode previously allocated
                                          resources (proposed resources) for all the PCI root bridges. After the
                                          hardware is programmed, reassigning resources will not be supported.
                                          The bus settings are not affected.
   EfiPciHostBridgeFreeResources          Deallocates resources that were previously allocated for all the PCI
                                          root bridges and resets the I/O and memory apertures to their initial
                                          state. The bus settings are not affected. If the request to allocate
                                          resources fails, the PCI enumerator can use this notification to
                                          deallocate previous resources, adjust the requests, and retry
                                          allocation.
   EfiPciHostBridgeEndResourceAllocation  The resource allocation phase is completed. No specific action is
                                          required here. This notification can be used to perform any chipsetspecific
                                          programming.

   @param[in] This                The instance pointer of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
   @param[in] Phase               The phase during enumeration

   @retval EFI_NOT_READY          This phase cannot be entered at this time. For example, this error
                                  is valid for a Phase of EfiPciHostBridgeAllocateResources if
                                  SubmitResources() has not been called for one or more
                                  PCI root bridges before this call
   @retval EFI_DEVICE_ERROR       Programming failed due to a hardware error. This error is valid
                                  for a Phase of EfiPciHostBridgeSetResources.
   @retval EFI_INVALID_PARAMETER  Invalid phase parameter
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.
                                  This error is valid for a Phase of EfiPciHostBridgeAllocateResources if the
                                  previously submitted resource requests cannot be fulfilled or
                                  were only partially fulfilled.
   @retval EFI_SUCCESS            The notification was accepted without any errors.

**/
EFI_STATUS
EFIAPI
NotifyPhase(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE    Phase
  )
{
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  PCI_RESOURCE_TYPE                     Index;
  LIST_ENTRY                            *List;
  EFI_PHYSICAL_ADDRESS                  BaseAddress;
  UINT64                                AddrLen;
  UINTN                                 BitsOfAlignment;
  EFI_STATUS                            Status;
  EFI_STATUS                            ReturnStatus;
  FABRIC_RESOURCE_FOR_EACH_RB          MmioForEachDie;
  UINT64                                Mem32ShrinkSize;
  UINT64                                Mem32ExpandSize;
  UINT64                                PcieBase;
  UINT64                                TopMem;
  UINT64                                TopMem2;
  UINT64                                Base;
  UINT64                                Size;
  UINT64                                MmioBaseBelowPcieCfg;
  UINT64                                MmioBaseAbovePcieCfg;
  UINT64                                MmioSizeBelowPcieCfg;
  UINT64                                MmioSizeAbovePcieCfg;
  UINT64                                MmioSizeBelowPcieCfgCopy;
  UINT64                                MmioSizeAbovePcieCfgCopy;
  UINT64                                TotalRequiredMmioSizeBelow4G;
  UINT64                                TotalRequiredMmioSizeAbove4G;
  AMD_APCB_SERVICE_PROTOCOL             *mApcbDxeServiceProtocol;
  UINT32                                SizeInByte;
  UINT64                                Value;
  BOOLEAN                               FirstBoot;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);

  RootBridgeInstance = NULL;
  Mem32ShrinkSize = 0;
  Mem32ExpandSize = 0;
  FirstBoot = FALSE;

  switch (Phase) {
  case EfiPciHostBridgeBeginEnumeration:
    if (HostBridgeInstance->CanRestarted) {
      //
      // Reset the Each Root Bridge
      //
      List = HostBridgeInstance->Head.ForwardLink;

      while (List != &HostBridgeInstance->Head) {
        RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
        for (Index = TypeIo; Index < TypeMax; Index++) {
          RootBridgeInstance->ResAllocNode[Index].Type      = Index;
          RootBridgeInstance->ResAllocNode[Index].Base      = 0;
          RootBridgeInstance->ResAllocNode[Index].Length    = 0;
          RootBridgeInstance->ResAllocNode[Index].Status    = ResNone;
        }

        List = List->ForwardLink;
      }

      HostBridgeInstance->ResourceSubmited = FALSE;
      HostBridgeInstance->CanRestarted     = TRUE;
      if (RootBridgeInstance != NULL) {
        if (mEnableAbove4GBMmioBiosAssignemnt){
          RootBridgeInstance->MemLimit = 0xFFFFFFFFFFFFFFFFUL;
        }
        else {
          RootBridgeInstance->MemLimit = 0xFFFFFFFF;
        }
      }
    } else {
      //
      // Can not restart
      //
      return EFI_NOT_READY;
    }
    break;

  case EfiPciHostBridgeBeginBusAllocation:
    //
    // No specific action is required here, can perform any chipset specific programing
    //
    HostBridgeInstance->CanRestarted = FALSE;
    return EFI_SUCCESS;
    break;

  case EfiPciHostBridgeEndBusAllocation:
    //
    // No specific action is required here, can perform any chipset specific programing
    //
    //HostBridgeInstance->CanRestarted = FALSE;
    return EFI_SUCCESS;
    break;

  case EfiPciHostBridgeBeginResourceAllocation:
    //
    // No specific action is required here, can perform any chipset specific programing
    //
    //HostBridgeInstance->CanRestarted = FALSE;
    return EFI_SUCCESS;
    break;

  case EfiPciHostBridgeAllocateResources:
    ReturnStatus = EFI_SUCCESS;
    if (HostBridgeInstance->ResourceSubmited) {
      Status = gBS->LocateProtocol (&gAmdApcbDxeServiceProtocolGuid, NULL, (VOID**)&mApcbDxeServiceProtocol);
      ASSERT_EFI_ERROR (Status);

      Status = FabricGetAvailableResource (&MmioForEachDie);
      TopMem = AsmReadMsr64 (0xC001001A);
      TopMem2 = AsmReadMsr64 (0xC001001D);
      PcieBase = PcdGet64 (PcdPciExpressBaseAddress);
      MmioBaseBelowPcieCfg = 0;
      MmioBaseAbovePcieCfg = 0;
      MmioSizeBelowPcieCfg = 0;
      MmioSizeAbovePcieCfg = 0;
      // There could be 2 MMIO regions below 4G. One is below PCIE configuration space and the other is above PCIE configuration space.
      // Below code try to get base/size of these 2 regions

      // Check 1st MMIO region
      GetBaseSizeOfThisMmioRegion (&MmioForEachDie.PrefetchableMmioSizeBelow4G[0][0], &MmioForEachDie.NonPrefetchableMmioSizeBelow4G[0][0], &Base, &Size);
      if (Size != 0) {
        if (Base < PcieBase) {
          // This region is below PCIE configuration space
          MmioBaseBelowPcieCfg = Base;
          MmioSizeBelowPcieCfg = Size;
        } else {
          MmioBaseAbovePcieCfg = Base;
          MmioSizeAbovePcieCfg = Size;
        }
      }

      // Check 2nd MMIO region
      GetBaseSizeOfThisMmioRegion (&MmioForEachDie.PrimaryRbSecondPrefetchableMmioSizeBelow4G, &MmioForEachDie.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G, &Base, &Size);
      if (Size != 0) {
        if (Base < PcieBase) {
          // This region is below PCIE configuration space
          MmioBaseBelowPcieCfg = Base;
          MmioSizeBelowPcieCfg = Size;
        } else {
          MmioBaseAbovePcieCfg = Base;
          MmioSizeAbovePcieCfg = Size;
        }
      }
      // Copy size to another variable, it will be used later
      MmioSizeBelowPcieCfgCopy = MmioSizeBelowPcieCfg;
      MmioSizeAbovePcieCfgCopy = MmioSizeAbovePcieCfg;

      //
      // Take care of the resource dependencies between the root bridges
      //
      List = HostBridgeInstance->Head.ForwardLink;

      while (List != &HostBridgeInstance->Head) {
        RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);

        // Calculate total required MMIO size (including below & above 4G)
        TotalRequiredMmioSizeBelow4G = (RootBridgeInstance->ResAllocNode[TypeMem32].Length + RootBridgeInstance->ResAllocNode[TypeMem32].Alignment) & (~RootBridgeInstance->ResAllocNode[TypeMem32].Alignment);
        TotalRequiredMmioSizeAbove4G = (RootBridgeInstance->ResAllocNode[TypePMem64].Length + RootBridgeInstance->ResAllocNode[TypePMem64].Alignment) & (~RootBridgeInstance->ResAllocNode[TypePMem64].Alignment);

        DEBUG ((DEBUG_INFO, "  ResAllocNode[TypeMem32].Length                     = 0x%08X\n", RootBridgeInstance->ResAllocNode[TypeMem32].Length));
        DEBUG ((DEBUG_INFO, "  ResAllocNode[TypeMem32].Alignment                  = 0x%08X\n", RootBridgeInstance->ResAllocNode[TypeMem32].Alignment));
        DEBUG ((DEBUG_INFO, "  ResAllocNode[TypePMem64].Length                    = 0x%08X\n", RootBridgeInstance->ResAllocNode[TypePMem64].Length));
        DEBUG ((DEBUG_INFO, "  ResAllocNode[TypePMem64].Alignment                 = 0x%08X\n", RootBridgeInstance->ResAllocNode[TypePMem64].Alignment));
        DEBUG ((DEBUG_INFO, "  TotalRequiredMmioSizeBelow4G                       = 0x%08X\n", TotalRequiredMmioSizeBelow4G));
        DEBUG ((DEBUG_INFO, "  TotalRequiredMmioSizeAbove4G                       = 0x%08X\n\n", TotalRequiredMmioSizeAbove4G));

        if (MmioForEachDie.NonPrefetchableMmioSizeAbove4G[0][0].Size != 0) {
          // First boot
          DEBUG ((DEBUG_INFO, "\tFirst Boot\n"));
          FirstBoot = TRUE;
        }
        // We only check if there's enough space below 4G.

        // If above 4G MMIO is disabled, then PMem64 (TotalRequiredMmioSizeAbove4G) would be put at below 4G, check if there's enough space
        if (mEnableAbove4GBMmioBiosAssignemnt == 0) {
          if (TotalRequiredMmioSizeAbove4G > MmioSizeAbovePcieCfg) {
            // @ below PCIE
            if (TotalRequiredMmioSizeAbove4G > MmioSizeBelowPcieCfg) {
              Mem32ExpandSize += TotalRequiredMmioSizeAbove4G - MmioSizeBelowPcieCfg;
              MmioSizeBelowPcieCfg = 0;
            } else {
              MmioSizeBelowPcieCfg -= TotalRequiredMmioSizeAbove4G;
            }
          } else {
            // @ above PCIE
            MmioSizeAbovePcieCfg -= TotalRequiredMmioSizeAbove4G;
          }
        }
        // Now MmioSizeBelowPcieCfg & MmioSizeAbovePcieCfg is the remaining size after TotalRequiredMmioSizeAbove4G has been satisfied.

        // Reserve more space under 4G for PEI GOP.
        if (0x20000000 > MmioSizeAbovePcieCfg) {
          // @ below PCIE
          if (0x20000000 > MmioSizeBelowPcieCfg) {
            Mem32ExpandSize += 0x20000000 - MmioSizeBelowPcieCfg;
            MmioSizeBelowPcieCfg = 0;
          } else {
            MmioSizeBelowPcieCfg -= 0x20000000;
          }
        } else {
          // @ above PCIE
          MmioSizeAbovePcieCfg -= 0x20000000;
        }

        // Check Mem32 (TotalRequiredMmioSizeBelow4G)
        if (TotalRequiredMmioSizeBelow4G > MmioSizeAbovePcieCfg) {
          // @ below PCIE
          if (TotalRequiredMmioSizeBelow4G > MmioSizeBelowPcieCfg) {
            Mem32ExpandSize += TotalRequiredMmioSizeBelow4G - MmioSizeBelowPcieCfg;
            MmioSizeBelowPcieCfg = 0;
          } else {
            Mem32ShrinkSize += MmioSizeBelowPcieCfg - TotalRequiredMmioSizeBelow4G;
            MmioSizeBelowPcieCfg -= TotalRequiredMmioSizeBelow4G;
          }
        } else {
          // @ above PCIE
          Mem32ShrinkSize += MmioSizeBelowPcieCfg;
          MmioSizeAbovePcieCfg -= TotalRequiredMmioSizeBelow4G;
        }
        // If TOM2 is 0, we can't upper TOM
        if (TopMem2 == 0) {
          Mem32ShrinkSize = 0;
        }
        Mem32ShrinkSize &= 0xF0000000;
        // One of ShrinkSize/ExpandSize must be 0
        ASSERT ((Mem32ShrinkSize == 0) || (Mem32ExpandSize == 0));

        if (GetBootModeHob () != BOOT_IN_RECOVERY_MODE) {
          // Now adjust TOM and call AGESA function FabricReallocateResourceForEachDie to adjust MMIO size
          if ((Mem32ShrinkSize != 0) || (Mem32ExpandSize != 0) || FirstBoot) {
            if (Mem32ShrinkSize != 0) {
              // Get the actual ShrinkSize
              Mem32ShrinkSize = ((TopMem + Mem32ShrinkSize) & 0xF0000000) - TopMem;
              TopMem += Mem32ShrinkSize;
              MmioSizeBelowPcieCfgCopy -= Mem32ShrinkSize;
            } else {
              // Get the actual ExpandSize
              Mem32ExpandSize = TopMem - ((TopMem - Mem32ExpandSize) & 0xF0000000);
              TopMem -= Mem32ExpandSize;
              MmioSizeBelowPcieCfgCopy += Mem32ExpandSize;
            }

            // By reporting the bigger size of MMIO above/below PCIE configuration space, it could make sure the smaller one would not be shunk by FabricReallocateResourceForEachDie
            MmioForEachDie.NonPrefetchableMmioSizeBelow4G[0][0].Size = (MmioSizeBelowPcieCfgCopy > MmioSizeAbovePcieCfgCopy) ? MmioSizeBelowPcieCfgCopy : MmioSizeAbovePcieCfgCopy;
            MmioForEachDie.PrefetchableMmioSizeBelow4G[0][0].Size = 0;
            MmioForEachDie.PrefetchableMmioSizeAbove4G[0][0].Size += MmioForEachDie.NonPrefetchableMmioSizeAbove4G[0][0].Size;
            MmioForEachDie.NonPrefetchableMmioSizeAbove4G[0][0].Size = 0;
            if (Mem32ExpandSize != 0) {
              // Lower TOM would cause TOM2 increased. So MMIO size above 4G must be shrunk.
              MmioForEachDie.PrefetchableMmioSizeAbove4G[0][0].Size -= Mem32ExpandSize;
            }
            DEBUG ((DEBUG_INFO, "  Reallocat MemoryCeiling                                = 0x%08X\n", TopMem));
            DEBUG ((DEBUG_INFO, "  Reallocat NonPrefetchableMmioSizeAbove4G[0][0].Size    = 0x%016LX\n", MmioForEachDie.NonPrefetchableMmioSizeAbove4G[0][0].Size));
            DEBUG ((DEBUG_INFO, "  Reallocat PrefetchableMmioSizeAbove4G[0][0].Size       = 0x%016LX\n", MmioForEachDie.PrefetchableMmioSizeAbove4G[0][0].Size));
            DEBUG ((DEBUG_INFO, "  Reallocat NonPrefetchableMmioSizeBelow4G[0][0].Size    = 0x%08X\n", MmioForEachDie.NonPrefetchableMmioSizeBelow4G[0][0].Size));
            DEBUG ((DEBUG_INFO, "  Reallocat PrefetchableMmioSizeBelow4G[0][0].Size       = 0x%08X\n", MmioForEachDie.PrefetchableMmioSizeBelow4G[0][0].Size));

            Status = gRT->SetVariable (MEMORY_CEILING_VARIABLE_NAME, &gMemoryCeilingVariableGuid, MEMORY_CEILING_VARIABLE_ATTRIBUTES, sizeof(EFI_PHYSICAL_ADDRESS), &TopMem);
            ASSERT_EFI_ERROR (Status);
            SizeInByte = 1;
            Value = RShiftU64 (TopMem, 24);
            DEBUG ((DEBUG_INFO, "\tSet APCB DF BottomIo to new value: 0x%08X\n", (UINT32) Value));
            Status = mApcbDxeServiceProtocol->ApcbSetConfigParameter (mApcbDxeServiceProtocol, APCB_ID_CONFIG_DF_BOTTOMIO, &SizeInByte, &Value);
            ASSERT_EFI_ERROR (Status);
            Status = mApcbDxeServiceProtocol->ApcbFlushData (mApcbDxeServiceProtocol);
            ASSERT_EFI_ERROR (Status);

          DEBUG ((DEBUG_INFO, "  Call FabricReallocateResourceForEachRb\n"));
          Status = FabricReallocateResourceForEachRb (&MmioForEachDie, NULL);
          DEBUG ((DEBUG_INFO, "  ResetSystem - EfiResetWarm\n"));
            gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
          }
        }

        // Allocate MMIO space for TypePMem64
        if ((RootBridgeInstance->RootBridgeAttrib & EFI_PCI_HOST_BRIDGE_MEM64_DECODE) != 0) {
          if (mEnableAbove4GBMmioBiosAssignemnt) {
            // TypePMem64 @ above 4G
            DEBUG ((DEBUG_INFO, "  UpdateLargeMemoryInfo Base = 0x%LX Size = 0x%LX\n", MmioForEachDie.PrefetchableMmioSizeAbove4G[0][0].Base, MmioForEachDie.PrefetchableMmioSizeAbove4G[0][0].Size));
            UpdateLargeMemoryInfo (MmioForEachDie.PrefetchableMmioSizeAbove4G[0][0].Base, MmioForEachDie.PrefetchableMmioSizeAbove4G[0][0].Size);
            DEBUG ((DEBUG_INFO, "  UpdateLargeMemoryAcpiInfo\n"));
            UpdateLargeMemoryAcpiInfo ();
//[-start-200108-IB06070076-add]//
            if (RootBridgeInstance->ResAllocNode[TypePMem64].Status != ResNone) {
//[-end-200108-IB06070076-add]//
              AddrLen = RootBridgeInstance->ResAllocNode[TypePMem64].Length;
              BitsOfAlignment = (UINTN) (HighBitSet64 (RootBridgeInstance->ResAllocNode[TypePMem64].Alignment) + 1);
              if (BitsOfAlignment >= 64) {
                DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypePMem64-if (BitsOfAlignment >= 64)\n"));
                BitsOfAlignment = 0;
              }
              DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypePMem64-AllocateMemorySpace-AddrLen 0x%LX BitsOfAlignment %X\n", AddrLen, BitsOfAlignment));
              Status = gDS->AllocateMemorySpace ( EfiGcdAllocateAnySearchTopDown,
                                                  EfiGcdMemoryTypeMemoryMappedIo,
                                                  BitsOfAlignment,
                                                  AddrLen,
                                                  &BaseAddress,
                                                  mDriverImageHandle,
                                                  NULL );
              if (!EFI_ERROR (Status)) {
                DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypePMem64-AllocateMemorySpace SUCESS Base = 0x%LX ResAllocated\n\n", BaseAddress));
                // We were able to allocate the PCI memory
                RootBridgeInstance->ResAllocNode[TypePMem64].Base   = (UINTN)BaseAddress;
                RootBridgeInstance->ResAllocNode[TypePMem64].Status = ResAllocated;
              } else {
                DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypePMem64-AllocateMemorySpace FAIL FAIL FAIL\n\n"));
                // Not able to allocate enough PCI memory
                ReturnStatus = Status;
                if (Status != EFI_OUT_OF_RESOURCES) {
                  RootBridgeInstance->ResAllocNode[TypePMem64].Length = 0;
                }
                ASSERT (FALSE);
              }
          }
          } else {
            // TypePMem64 @ below 4G
//[-start-200108-IB06070076-add]//
            if (RootBridgeInstance->ResAllocNode[TypePMem64].Status != ResNone) {
//[-end-200108-IB06070076-add]//
              AddrLen = RootBridgeInstance->ResAllocNode[TypePMem64].Length;
              BitsOfAlignment = (UINTN) (HighBitSet64 (RootBridgeInstance->ResAllocNode[TypePMem64].Alignment) + 1);
              if (BitsOfAlignment >= 32) {
                DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypePMem64-if (BitsOfAlignment >= 32)\n"));
                BitsOfAlignment = 0;
              }
              DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypePMem64-AllocateMemorySpace-AddrLen 0x%08X BitsOfAlignment %X\n", AddrLen, BitsOfAlignment));
              Status = gDS->AllocateMemorySpace ( EfiGcdAllocateAnySearchBottomUp,
                                                  EfiGcdMemoryTypeMemoryMappedIo,
                                                  BitsOfAlignment,
                                                  AddrLen,
                                                  &BaseAddress,
                                                  mDriverImageHandle,
                                                  NULL );
              if (!EFI_ERROR (Status)) {
                DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypePMem64-AllocateMemorySpace SUCESS Base = 0x%08X ResAllocated\n\n", BaseAddress));
                // We were able to allocate the PCI memory
                RootBridgeInstance->ResAllocNode[TypePMem64].Base   = (UINTN)BaseAddress;
                RootBridgeInstance->ResAllocNode[TypePMem64].Status = ResAllocated;
              } else {
                DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypePMem64-AllocateMemorySpace FAIL FAIL FAIL\n\n"));
                // Not able to allocate enough PCI memory
                ReturnStatus = Status;
                if (Status != EFI_OUT_OF_RESOURCES) {
                  RootBridgeInstance->ResAllocNode[TypePMem64].Length = 0;
                }
                ASSERT (FALSE);
              }
            }
          }
        }

        // Allocate MMIO space for TypeMem32
//[-start-200108-IB06070076-add]//
        if (RootBridgeInstance->ResAllocNode[TypeMem32].Status != ResNone) {
//[-end-200108-IB06070076-add]//
          AddrLen = RootBridgeInstance->ResAllocNode[TypeMem32].Length;
          BitsOfAlignment = (UINTN) (HighBitSet64 (RootBridgeInstance->ResAllocNode[TypeMem32].Alignment) + 1);
          if (BitsOfAlignment >= 32) {
            DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypeMem32-if (BitsOfAlignment >= 32)\n"));
            BitsOfAlignment = 0;
          }
          DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypeMem32-AllocateMemorySpace-AddrLen 0x%08X BitsOfAlignment %X\n", AddrLen, BitsOfAlignment));
          Status = gDS->AllocateMemorySpace ( EfiGcdAllocateAnySearchBottomUp,
                                              EfiGcdMemoryTypeMemoryMappedIo,
                                              BitsOfAlignment,
                                              AddrLen,
                                              &BaseAddress,
                                              mDriverImageHandle,
                                              NULL );
          if (!EFI_ERROR (Status)) {
            DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypeMem32-AllocateMemorySpace SUCESS Base = 0x%08X ResAllocated\n\n", BaseAddress));
            // We were able to allocate the PCI memory
            RootBridgeInstance->ResAllocNode[TypeMem32].Base   = (UINTN)BaseAddress;
            RootBridgeInstance->ResAllocNode[TypeMem32].Status = ResAllocated;
          } else {
            DEBUG ((DEBUG_INFO, "  ResourceSubmited-TypeMem32-AllocateMemorySpace FAIL FAIL FAIL\n\n"));
            // Not able to allocate enough PCI memory
            ReturnStatus = Status;
            if (Status != EFI_OUT_OF_RESOURCES) {
              RootBridgeInstance->ResAllocNode[TypeMem32].Length = 0;
            }
            ASSERT (FALSE);
          }
        }

        // case TypeIo:
        if (RootBridgeInstance->ResAllocNode[TypeIo].Status != ResNone) {
          AddrLen = RootBridgeInstance->ResAllocNode[TypeIo].Length;
          BitsOfAlignment = (UINTN) (HighBitSet64 (RootBridgeInstance->ResAllocNode[TypeIo].Alignment) + 1);
          //
          // It is impossible for this chipset to align 0xFFFF for IO16
          // So clear it
          //
          if (BitsOfAlignment >= 16) {
            BitsOfAlignment = 0;
          }
          Status = gDS->AllocateIoSpace ( EfiGcdAllocateAnySearchBottomUp,
                                          EfiGcdIoTypeIo,
                                          BitsOfAlignment,
                                          AddrLen,
                                          &BaseAddress,
                                          mDriverImageHandle,
                                          NULL );
          if (!EFI_ERROR (Status)) {
            RootBridgeInstance->ResAllocNode[TypeIo].Base   = (UINTN)BaseAddress;
            RootBridgeInstance->ResAllocNode[TypeIo].Status = ResAllocated;
          } else {
            ReturnStatus = Status;
            if (Status != EFI_OUT_OF_RESOURCES) {
              RootBridgeInstance->ResAllocNode[TypeIo].Length = 0;
            }
          }
        }

        List = List->ForwardLink;
      }
      return ReturnStatus;

    } else {
      return EFI_NOT_READY;
    }
    break;

  case EfiPciHostBridgeSetResources:
    break;

  case EfiPciHostBridgeFreeResources:
    ReturnStatus = EFI_SUCCESS;
    List = HostBridgeInstance->Head.ForwardLink;
    while (List != &HostBridgeInstance->Head) {
      RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
      for (Index = TypeIo; Index < TypeBus; Index++) {
        if (RootBridgeInstance->ResAllocNode[Index].Status == ResAllocated) {
          AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
          BaseAddress = RootBridgeInstance->ResAllocNode[Index].Base;
          switch (Index) {

          case TypeIo:
            Status = gDS->FreeIoSpace (BaseAddress, AddrLen);
            if (EFI_ERROR (Status)) {
              ReturnStatus = Status;
            }
            break;

          case TypeMem32:
            Status = gDS->FreeMemorySpace (BaseAddress, AddrLen);
            if (EFI_ERROR (Status)) {
              ReturnStatus = Status;
            }
            break;

          case TypePMem32:
            break;

          case TypeMem64:
            break;

          case TypePMem64:
            Status = gDS->FreeMemorySpace (BaseAddress, AddrLen);
            if (EFI_ERROR (Status)) {
              ReturnStatus = Status;
            }
            break;

          default:
            ASSERT (FALSE);
            break;

          }; //end switch
          RootBridgeInstance->ResAllocNode[Index].Type      = Index;
          RootBridgeInstance->ResAllocNode[Index].Base      = 0;
          RootBridgeInstance->ResAllocNode[Index].Length    = 0;
          RootBridgeInstance->ResAllocNode[Index].Status    = ResNone;
        }
      }

      List = List->ForwardLink;
    }

    HostBridgeInstance->ResourceSubmited = FALSE;
    HostBridgeInstance->CanRestarted     = TRUE;
    return ReturnStatus;
    break;

  case EfiPciHostBridgeEndResourceAllocation:
    HostBridgeInstance->CanRestarted = FALSE;
    break;

  case EfiPciHostBridgeEndEnumeration:
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }; // end switch

  return EFI_SUCCESS;
}

/**
   Return the device handle of the next PCI root bridge that is associated with this Host Bridge.

   This function is called multiple times to retrieve the device handles of all the PCI root bridges that
   are associated with this PCI host bridge. Each PCI host bridge is associated with one or more PCI
   root bridges. On each call, the handle that was returned by the previous call is passed into the
   interface, and on output the interface returns the device handle of the next PCI root bridge. The
   caller can use the handle to obtain the instance of the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL
   for that root bridge. When there are no more PCI root bridges to report, the interface returns
   EFI_NOT_FOUND. A PCI enumerator must enumerate the PCI root bridges in the order that they
   are returned by this function.
   For D945 implementation, there is only one root bridge in PCI host bridge.

   @param[in]       This              The instance pointer of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
   @param[in, out]  RootBridgeHandle  Returns the device handle of the next PCI root bridge.

   @retval EFI_SUCCESS            If parameter RootBridgeHandle = NULL, then return the first Rootbridge handle of the
                                  specific Host bridge and return EFI_SUCCESS.
   @retval EFI_NOT_FOUND          Can not find the any more root bridge in specific host bridge.
   @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not an EFI_HANDLE that was
                                  returned on a previous call to GetNextRootBridge().
**/
EFI_STATUS
EFIAPI
GetNextRootBridge(
  IN       EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN OUT   EFI_HANDLE                                       *RootBridgeHandle
  )
{
  BOOLEAN                               NoRootBridge;
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;

  NoRootBridge = TRUE;
  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;


  while (List != &HostBridgeInstance->Head) {
    NoRootBridge = FALSE;
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (*RootBridgeHandle == NULL) {
      //
      // Return the first Root Bridge Handle of the Host Bridge
      //
      *RootBridgeHandle = RootBridgeInstance->Handle;
      return EFI_SUCCESS;
    } else {
      if (*RootBridgeHandle == RootBridgeInstance->Handle) {
        //
        // Get next if have
        //
        List = List->ForwardLink;
        if (List!=&HostBridgeInstance->Head) {
          RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
          *RootBridgeHandle = RootBridgeInstance->Handle;
          return EFI_SUCCESS;
        } else {
          return EFI_NOT_FOUND;
        }
      }
    }

    List = List->ForwardLink;
  } //end while

  if (NoRootBridge) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}

/**
   Returns the allocation attributes of a PCI root bridge.

   The function returns the allocation attributes of a specific PCI root bridge. The attributes can vary
   from one PCI root bridge to another. These attributes are different from the decode-related
   attributes that are returned by the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.GetAttributes() member function. The
   RootBridgeHandle parameter is used to specify the instance of the PCI root bridge. The device
   handles of all the root bridges that are associated with this host bridge must be obtained by calling
   GetNextRootBridge(). The attributes are static in the sense that they do not change during or
   after the enumeration process. The hardware may provide mechanisms to change the attributes on
   the fly, but such changes must be completed before EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL is
   installed. The permitted values of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ATTRIBUTES are defined in
   "Related Definitions" below. The caller uses these attributes to combine multiple resource requests.
   For example, if the flag EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM is set, the PCI bus enumerator needs to
   include requests for the prefetchable memory in the nonprefetchable memory pool and not request any
   prefetchable memory.
      Attribute                                 Description
   ------------------------------------         ----------------------------------------------------------------------
   EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM         If this bit is set, then the PCI root bridge does not support separate
                                                windows for nonprefetchable and prefetchable memory. A PCI bus
                                                driver needs to include requests for prefetchable memory in the
                                                nonprefetchable memory pool.

   EFI_PCI_HOST_BRIDGE_MEM64_DECODE             If this bit is set, then the PCI root bridge supports 64-bit memory
                                                windows. If this bit is not set, the PCI bus driver needs to include
                                                requests for a 64-bit memory address in the corresponding 32-bit
                                                memory pool.

   @param[in]   This               The instance pointer of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
   @param[in]   RootBridgeHandle   The device handle of the PCI root bridge in which the caller is interested. Type
                                   EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.0 Specification.
   @param[out]  Attributes         The pointer to attribte of root bridge, it is output parameter

   @retval EFI_INVALID_PARAMETER   Attribute pointer is NULL
   @retval EFI_INVALID_PARAMETER   RootBridgehandle is invalid.
   @retval EFI_SUCCESS             Success to get attribute of interested root bridge.

**/
EFI_STATUS
EFIAPI
GetAttributes(
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN  EFI_HANDLE                                       RootBridgeHandle,
  OUT UINT64                                           *Attributes
  )
{
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;

  if (Attributes == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      *Attributes = RootBridgeInstance->RootBridgeAttrib;
      return EFI_SUCCESS;
    }
    List = List->ForwardLink;
  }

  //
  // RootBridgeHandle is not an EFI_HANDLE
  // that was returned on a previous call to GetNextRootBridge()
  //
  return EFI_INVALID_PARAMETER;
}

/**
   Sets up the specified PCI root bridge for the bus enumeration process.

   This member function sets up the root bridge for bus enumeration and returns the PCI bus range
   over which the search should be performed in ACPI 2.0 resource descriptor format.

   @param[in]   This              The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
   @param[in]   RootBridgeHandle  The PCI Root Bridge to be set up.
   @param[out]  Configuration     Pointer to the pointer to the PCI bus resource descriptor.

   @retval EFI_INVALID_PARAMETER Invalid Root bridge's handle
   @retval EFI_OUT_OF_RESOURCES  Fail to allocate ACPI resource descriptor tag.
   @retval EFI_SUCCESS           Sucess to allocate ACPI resource descriptor.

**/
EFI_STATUS
EFIAPI
StartBusEnumeration(
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN  EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                             **Configuration
  )
{
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  VOID                                  *Buffer;
  UINT8                                 *Temp;
  UINT64                                BusStart;
  UINT64                                BusEnd;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      //
      // Set up the Root Bridge for Bus Enumeration
      //
      BusStart = RootBridgeInstance->BusBase;
      BusEnd   = RootBridgeInstance->BusLimit;
      BusEnd   = (UINT64) ((PcdGet32 (PcdPciExpressSize) >> 20) - 1);
      //
      // Program the Hardware(if needed) if error return EFI_DEVICE_ERROR
      //

      Buffer = AllocatePool (sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof(EFI_ACPI_END_TAG_DESCRIPTOR));
      if (Buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      Temp = (UINT8 *)Buffer;

      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->Desc = 0x8A;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->Len  = 0x2B;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->ResType = 2;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->GenFlag = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->SpecificFlag = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrSpaceGranularity = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrRangeMin = BusStart;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrRangeMax = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrTranslationOffset = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Temp)->AddrLen = BusEnd - BusStart + 1;

      Temp = Temp + sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Desc = 0x79;
      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Checksum = 0x0;

      *Configuration = Buffer;
      return EFI_SUCCESS;
    }
    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
   Programs the PCI root bridge hardware so that it decodes the specified PCI bus range.

   This member function programs the specified PCI root bridge to decode the bus range that is
   specified by the input parameter Configuration.
   The bus range information is specified in terms of the ACPI 2.0 resource descriptor format.

   @param[in] This              The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
   @param[in] RootBridgeHandle  The PCI Root Bridge whose bus range is to be programmed
   @param[in] Configuration     The pointer to the PCI bus resource descriptor

   @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not a valid root bridge handle.
   @retval EFI_INVALID_PARAMETER  Configuration is NULL.
   @retval EFI_INVALID_PARAMETER  Configuration does not point to a valid ACPI 2.0 resource descriptor.
   @retval EFI_INVALID_PARAMETER  Configuration does not include a valid ACPI 2.0 bus resource descriptor.
   @retval EFI_INVALID_PARAMETER  Configuration includes valid ACPI 2.0 resource descriptors other than
                                  bus descriptors.
   @retval EFI_INVALID_PARAMETER  Configuration contains one or more invalid ACPI resource descriptors.
   @retval EFI_INVALID_PARAMETER  "Address Range Minimum" is invalid for this root bridge.
   @retval EFI_INVALID_PARAMETER  "Address Range Length" is invalid for this root bridge.
   @retval EFI_DEVICE_ERROR       Programming failed due to a hardware error.
   @retval EFI_SUCCESS            The bus range for the PCI root bridge was programmed.

**/
EFI_STATUS
EFIAPI
SetBusNumbers(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  )
{
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  UINT8                                 *Ptr;
  UINTN                                 BusStart;
  UINTN                                 BusEnd;
  UINTN                                 BusLen;

  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Ptr = Configuration;

  //
  // Check the Configuration is valid
  //
  if(*Ptr != ACPI_ADDRESS_SPACE_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  if (((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->ResType != 2) {
    return EFI_INVALID_PARAMETER;
  }

  Ptr += sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
  if (*Ptr != ACPI_END_TAG_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  Ptr = Configuration;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      BusStart = (UINTN)((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrRangeMin;
      BusLen = (UINTN)((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrLen;
      BusEnd = BusStart + BusLen - 1;

      if (BusStart > BusEnd) {
        return EFI_INVALID_PARAMETER;
      }

      if ((BusStart < RootBridgeInstance->BusBase) || (BusEnd > RootBridgeInstance->BusLimit)) {
        return EFI_INVALID_PARAMETER;
      }

      //
      // Update the Bus Range
      //
      RootBridgeInstance->ResAllocNode[TypeBus].Base   = BusStart;
      RootBridgeInstance->ResAllocNode[TypeBus].Length = BusLen;
      RootBridgeInstance->ResAllocNode[TypeBus].Status = ResAllocated;

      //
      // Program the Root Bridge Hardware
      //

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}


/**
   Submits the I/O and memory resource requirements for the specified PCI root bridge.

   This function is used to submit all the I/O and memory resources that are required by the specified
   PCI root bridge. The input parameter Configuration is used to specify the following:
   - The various types of resources that are required
   - The associated lengths in terms of ACPI 2.0 resource descriptor format

   @param[in] This              Pointer to the EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance.
   @param[in] RootBridgeHandle  The PCI root bridge whose I/O and memory resource requirements are being submitted.
   @param[in] Configuration     The pointer to the PCI I/O and PCI memory resource descriptor.

   @retval EFI_SUCCESS            The I/O and memory resource requests for a PCI root bridge were accepted.
   @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not a valid root bridge handle.
   @retval EFI_INVALID_PARAMETER  Configuration is NULL.
   @retval EFI_INVALID_PARAMETER  Configuration does not point to a valid ACPI 2.0 resource descriptor.
   @retval EFI_INVALID_PARAMETER  Configuration includes requests for one or more resource types that are
                                  not supported by this PCI root bridge. This error will happen if the caller
                                  did not combine resources according to Attributes that were returned by
                                  GetAllocAttributes().
   @retval EFI_INVALID_PARAMETER  Address Range Maximum" is invalid.
   @retval EFI_INVALID_PARAMETER  "Address Range Length" is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  "Address Space Granularity" is invalid for this PCI root bridge.

**/
EFI_STATUS
EFIAPI
SubmitResources(
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  )
{
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  UINT8                                 *Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     *Ptr;
  UINT64                                AddrLen;
  UINT64                                Alignment;

  //
  // Check the input parameter: Configuration
  //
  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  Temp = (UINT8 *)Configuration;
  while ( *Temp == 0x8A) {
    Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) ;
  }
  if (*Temp != 0x79) {
    return EFI_INVALID_PARAMETER;
  }

  Temp = (UINT8 *)Configuration;
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      while ( *Temp == 0x8A) {
        Ptr = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp ;

        if (mEnableAbove4GBMmioBiosAssignemnt == 0x0) {
          //
          // Check Address Length
          //
          if (Ptr->AddrLen > 0xffffffff) {
            return EFI_INVALID_PARAMETER;
          }

          //
          // Check address range alignment
          //
          if (Ptr->AddrRangeMax >= 0xffffffff || Ptr->AddrRangeMax != (GetPowerOfTwo64 (Ptr->AddrRangeMax + 1) - 1)) {
            return EFI_INVALID_PARAMETER;
          }
        }
        else {
          //
          // Check address range alignment
          //
          if (Ptr->AddrRangeMax >= 0xffffffffFFFFFFFFUL || Ptr->AddrRangeMax != (GetPowerOfTwo64 (Ptr->AddrRangeMax + 1) - 1)) {
            return EFI_INVALID_PARAMETER;
          }
        }

        switch (Ptr->ResType) {

        case 0:

          //
          // Check invalid Address Sapce Granularity
          //

          //
          // check the memory resource request is supported by PCI root bridge
          //

          AddrLen = Ptr->AddrLen;
          Alignment = Ptr->AddrRangeMax;
          if (Ptr->AddrSpaceGranularity == 32) {
            if (Ptr->SpecificFlag == 0x06) {
              //
              // Apply from GCD
              //
              RootBridgeInstance->ResAllocNode[TypePMem32].Status = ResSubmitted;
            } else {
              RootBridgeInstance->ResAllocNode[TypeMem32].Length = AddrLen;
              RootBridgeInstance->ResAllocNode[TypeMem32].Alignment = Alignment;
              RootBridgeInstance->ResAllocNode[TypeMem32].Status = ResRequested;
              HostBridgeInstance->ResourceSubmited = TRUE;
            }
          }

          if (Ptr->AddrSpaceGranularity == 64) {
            if (Ptr->SpecificFlag == 0x06) {
              RootBridgeInstance->ResAllocNode[TypePMem64].Length = AddrLen;
              RootBridgeInstance->ResAllocNode[TypePMem64].Alignment = Alignment;
              RootBridgeInstance->ResAllocNode[TypePMem64].Status = ResRequested;
              HostBridgeInstance->ResourceSubmited = TRUE;
            } else {
              RootBridgeInstance->ResAllocNode[TypeMem64].Status = ResSubmitted;
            }
          }
          break;

        case 1:
          AddrLen = (UINTN) Ptr->AddrLen;
          Alignment = (UINTN) Ptr->AddrRangeMax;
          RootBridgeInstance->ResAllocNode[TypeIo].Length  = AddrLen;
          RootBridgeInstance->ResAllocNode[TypeIo].Alignment = Alignment;
          RootBridgeInstance->ResAllocNode[TypeIo].Status  = ResRequested;
          HostBridgeInstance->ResourceSubmited = TRUE;
          break;

        default:
            break;
        };

        Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) ;
      }

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
   Returns the proposed resource settings for the specified PCI root bridge.

   This member function returns the proposed resource settings for the specified PCI root bridge. The
   proposed resource settings are prepared when NotifyPhase() is called with a Phase of
   EfiPciHostBridgeAllocateResources. The output parameter Configuration
   specifies the following:
   - The various types of resources, excluding bus resources, that are allocated
   - The associated lengths in terms of ACPI 2.0 resource descriptor format

   @param[in]  This              Pointer to the EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance.
   @param[in]  RootBridgeHandle  The PCI root bridge handle. Type EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.0 Specification.
   @param[out] Configuration     The pointer to the pointer to the PCI I/O and memory resource descriptor.

   @retval EFI_SUCCESS            The requested parameters were returned.
   @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not a valid root bridge handle.
   @retval EFI_DEVICE_ERROR       Programming failed due to a hardware error.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI
GetProposedResources(
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN  EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                             **Configuration
  )
{
  LIST_ENTRY                            *List;
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  UINTN                                 Index;
  UINTN                                 Number;
  VOID                                  *Buffer;
  UINT8                                 *Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     *Ptr;
  UINT64                                ResStatus;

  Buffer = NULL;
  Number = 0;
  //
  // Get the Host Bridge Instance from the resource allocation protocol
  //
  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  //
  // Enumerate the root bridges in this host bridge
  //
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      for (Index = 0; Index < TypeBus; Index ++) {
        if (RootBridgeInstance->ResAllocNode[Index].Status != ResNone) {
          Number ++;
        }
      }

      if (Number ==  0) {
        return EFI_INVALID_PARAMETER;
      }

      Buffer = AllocateZeroPool (Number * sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof(EFI_ACPI_END_TAG_DESCRIPTOR));
      if (Buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      Temp = Buffer;
      for (Index = 0; Index < TypeBus; Index ++) {
        if (RootBridgeInstance->ResAllocNode[Index].Status != ResNone) {
          Ptr = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp ;
          ResStatus = RootBridgeInstance->ResAllocNode[Index].Status;

          switch (Index) {

          case TypeIo:
            //
            // Io
            //
            Ptr->Desc = 0x8A;
            Ptr->Len  = 0x2B;
            Ptr->ResType = 1;
            Ptr->GenFlag = 0;
            Ptr->SpecificFlag = 0;
            Ptr->AddrRangeMin = RootBridgeInstance->ResAllocNode[Index].Base;
            Ptr->AddrRangeMax = 0;
            Ptr->AddrTranslationOffset = \
                 (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
            Ptr->AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
            break;

          case TypeMem32:
            //
            // Memory 32
            //
            Ptr->Desc = 0x8A;
            Ptr->Len  = 0x2B;
            Ptr->ResType = 0;
            Ptr->GenFlag = 0;
            Ptr->SpecificFlag = 0;
            Ptr->AddrSpaceGranularity = 32;
            Ptr->AddrRangeMin = RootBridgeInstance->ResAllocNode[Index].Base;
            Ptr->AddrRangeMax = 0;
            Ptr->AddrTranslationOffset = \
                 (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
            Ptr->AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
            break;

          case TypePMem32:
            //
            // Prefetch memory 32
            //
            Ptr->Desc = 0x8A;
            Ptr->Len  = 0x2B;
            Ptr->ResType = 0;
            Ptr->GenFlag = 0;
            Ptr->SpecificFlag = 6;
            Ptr->AddrSpaceGranularity = 32;
            Ptr->AddrRangeMin = 0;
            Ptr->AddrRangeMax = 0;
            Ptr->AddrTranslationOffset = EFI_RESOURCE_NONEXISTENT;
            Ptr->AddrLen = 0;
            break;

          case TypeMem64:
            //
            // Memory 64
            //
            Ptr->Desc = 0x8A;
            Ptr->Len  = 0x2B;
            Ptr->ResType = 0;
            Ptr->GenFlag = 0;
            Ptr->SpecificFlag = 0;
            Ptr->AddrSpaceGranularity = 64;
            Ptr->AddrRangeMin = 0;
            Ptr->AddrRangeMax = 0;
            Ptr->AddrTranslationOffset = EFI_RESOURCE_NONEXISTENT;
            Ptr->AddrLen = 0;
            break;

          case TypePMem64:
            //
            // Prefetch memory 64
            //
            Ptr->Desc = 0x8A;
            Ptr->Len  = 0x2B;
            Ptr->ResType = 0;
            Ptr->GenFlag = 0;
            Ptr->SpecificFlag = 6;
            Ptr->AddrSpaceGranularity = 64;
            Ptr->AddrRangeMin = RootBridgeInstance->ResAllocNode[Index].Base;
            Ptr->AddrRangeMax = 0;
            Ptr->AddrTranslationOffset = \
                 (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
            Ptr->AddrLen = RootBridgeInstance->ResAllocNode[Index].Length;
            break;
          };

          Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
        }
      }

      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Desc = 0x79;
      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Temp)->Checksum = 0x0;

      *Configuration = Buffer;

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
   Provides the hooks from the PCI bus driver to every PCI controller (device/function) at various
   stages of the PCI enumeration process that allow the host bridge driver to preinitialize individual
   PCI controllers before enumeration.

   This function is called during the PCI enumeration process. No specific action is expected from this
   member function. It allows the host bridge driver to preinitialize individual PCI controllers before
   enumeration.

   @param This              Pointer to the EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance.
   @param RootBridgeHandle  The associated PCI root bridge handle. Type EFI_HANDLE is defined in
                            InstallProtocolInterface() in the UEFI 2.0 Specification.
   @param PciAddress        The address of the PCI device on the PCI bus. This address can be passed to the
                            EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL member functions to access the PCI
                            configuration space of the device. See Table 12-1 in the UEFI 2.0 Specification for
                            the definition of EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS.
   @param Phase             The phase of the PCI device enumeration.

   @retval EFI_SUCCESS              The requested parameters were returned.
   @retval EFI_INVALID_PARAMETER    RootBridgeHandle is not a valid root bridge handle.
   @retval EFI_INVALID_PARAMETER    Phase is not a valid phase that is defined in
                                    EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE.
   @retval EFI_DEVICE_ERROR         Programming failed due to a hardware error. The PCI enumerator should
                                    not enumerate this device, including its child devices if it is a PCI-to-PCI
                                    bridge.

**/
EFI_STATUS
EFIAPI
PreprocessController (
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL          *This,
  IN  EFI_HANDLE                                                RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS               PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE              Phase
  )
{
  PCI_HOST_BRIDGE_INSTANCE              *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridgeInstance;
  LIST_ENTRY                            *List;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List = HostBridgeInstance->Head.ForwardLink;

  //
  // Enumerate the root bridges in this host bridge
  //
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      break;
    }
    List = List->ForwardLink;
  }
  if (List == &HostBridgeInstance->Head) {
    return EFI_INVALID_PARAMETER;
  }

  if ((UINT32)Phase > EfiPciBeforeResourceCollection) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

#define  LARGE_MEMORY_REGION_COUNT  1

GLOBAL_REMOVE_IF_UNREFERENCED UINT64  mMmio64Base[LARGE_MEMORY_REGION_COUNT]   = {0x0};
GLOBAL_REMOVE_IF_UNREFERENCED UINT64  mMmio64Length[LARGE_MEMORY_REGION_COUNT] = {0x0};
GLOBAL_REMOVE_IF_UNREFERENCED UINT64  mMmio64Limit[LARGE_MEMORY_REGION_COUNT]  = {0x0};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA_PROTOCOL *mGlobalNvsAreaProtocol = NULL;

VOID
UpdateGnvsForMmioResourceBaseLength (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINTN                         TableOffet = 0x0;

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &mGlobalNvsAreaProtocol);

  if (Status != EFI_SUCCESS) {
    return;
  }

  gBS->CloseEvent (Event);

  mGlobalNvsAreaProtocol->Area->Mmio64AslSupport = mEnableAbove4GBMmioBiosAssignemnt;
  for (TableOffet = 0x0; TableOffet < LARGE_MEMORY_REGION_COUNT; TableOffet++) {
    mGlobalNvsAreaProtocol->Area->MmioMem64Base[TableOffet]    = mMmio64Base[TableOffet];
    mGlobalNvsAreaProtocol->Area->MmioMem64Length[TableOffet]  = mMmio64Length[TableOffet];
    mGlobalNvsAreaProtocol->Area->MmioMem64Limit[TableOffet]   = mMmio64Limit[TableOffet];
  }

  return;
}

EFI_STATUS
UpdateLargeMemoryAcpiInfo (
  VOID
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_EVENT  GlobalNvsUpdateEvent = NULL;
  VOID       *GlobalNvsInstalledRegistration = NULL;

  if (!PcdGet8 (PcdMmioMem64Enable)) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// If Global NVS protocol not installed yet, register Global Nvs protocol callback event
  ///
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &mGlobalNvsAreaProtocol);

  if (Status == EFI_NOT_FOUND) {

    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    (EFI_EVENT_NOTIFY) UpdateGnvsForMmioResourceBaseLength,
                    NULL,
                    &GlobalNvsUpdateEvent
                   );

     ASSERT_EFI_ERROR (Status);

     Status = gBS->RegisterProtocolNotify (
                     &gEfiGlobalNvsAreaProtocolGuid,
                     GlobalNvsUpdateEvent,
                     &GlobalNvsInstalledRegistration
                   );

     ASSERT_EFI_ERROR (Status);
  }
  else {
    UpdateGnvsForMmioResourceBaseLength (NULL, NULL);
  }

  return  EFI_SUCCESS;
}

EFI_STATUS
UpdateLargeMemoryInfo (
  IN  EFI_PHYSICAL_ADDRESS  BaseAddress,
  IN  UINT64                Length
)
{
  UINTN  TableCount  = 0x0;
  UINTN  TableOffset = 0x0;

  if (!PcdGet8 (PcdMmioMem64Enable)) {
    return EFI_UNSUPPORTED;
  }

  for (TableCount = 0x0; TableCount < LARGE_MEMORY_REGION_COUNT; TableCount++) {

    TableOffset = LARGE_MEMORY_REGION_COUNT - TableCount - 1;

    if ((mMmio64Base[TableOffset] == 0x0) && (mMmio64Length[TableOffset] == 0x0)) {

      mMmio64Base[TableOffset]   = BaseAddress;
      mMmio64Length[TableOffset] = Length;
      mMmio64Limit[TableOffset]  = BaseAddress + Length - 1;

      return  EFI_SUCCESS;

    }
  }

  return  EFI_SUCCESS;
}

VOID
GetBaseSizeOfThisMmioRegion (
  IN  FABRIC_ADDR_APERTURE *Prefetch,
  IN  FABRIC_ADDR_APERTURE *NonPrefetch,
  IN  UINT64               *Base,
  IN  UINT64               *Size
  )
{
  *Base = 0;
  *Size = Prefetch->Size + NonPrefetch->Size;
  if (*Size != 0) {
    if ((Prefetch->Size != 0) && (NonPrefetch->Size != 0)) {
      *Base = (Prefetch->Base <= NonPrefetch->Base) ? Prefetch->Base : NonPrefetch->Base;
    } else if (Prefetch->Size != 0) {
      *Base = Prefetch->Base;
    } else if (NonPrefetch->Size != 0) {
      *Base = NonPrefetch->Base;
    }
  }
  return;
}
