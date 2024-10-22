/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB function to create/locate PCIe configuration data area
 *
 * Contain code that create/locate and rebase configuration data area.
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <GnbDxio.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/AmdBaseLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/PcieInputParserLib.h>
#include  <Library/NbioHandleLib.h>
#include  <AmdPcieComplex.h>
#define FILECODE LIBRARY_PCIECONFIGLIB_PCIECONFIGLIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define DEBUG_HEX_DUMP  0

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * get Master Lane of PCIe port engine
 *
 *
 *
 * @param[in]  Engine      Pointer to engine descriptor
 * @retval                 Master Engine Lane Number
 */
UINT8
PcieConfigGetPcieEngineMasterLane (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  UINT8                 MasterLane;
  PCIe_WRAPPER_CONFIG   *Wrapper;
  ASSERT (PcieConfigIsPcieEngine (Engine));

  Wrapper = PcieConfigGetParentWrapper (Engine);
  if (Engine->EngineData.StartLane <= Engine->EngineData.EndLane) {
    MasterLane = (UINT8) (Engine->EngineData.StartLane - Wrapper->StartPhyLane);
  } else {
    MasterLane = (UINT8) (Engine->EngineData.EndLane - Wrapper->StartPhyLane);
  }
  return MasterLane;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get number of core lanes
 *
 *
 *
 * @param[in]  Engine      Pointer to engine descriptor
 * @retval                 Number of core lane
 */
UINT8
PcieConfigGetNumberOfCoreLane (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  if (Engine->Type.Port.StartCoreLane >= UNUSED_LANE_ID || Engine->Type.Port.EndCoreLane >= UNUSED_LANE_ID) {
    return 0;
  }
  return (UINT8) (Engine->Type.Port.EndCoreLane - Engine->Type.Port.StartCoreLane + 1);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Disable engine
 *
 *
 *
 * @param[in]   Engine              Pointer to engine config descriptor
 */
VOID
PcieConfigDisableEngine (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  if (PcieConfigIsSbPcieEngine (Engine)) {
    return;
  }
  PcieConfigResetDescriptorFlags (Engine, DESCRIPTOR_ALLOCATED);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Disable all engines on wrapper
 *
 *
 *
 * @param[in]  EngineTypeMask Engine type bitmap.
 * @param[in]  Wrapper        Pointer to wrapper config descriptor
 */
VOID
PcieConfigDisableAllEngines (
  IN      UINTN                          EngineTypeMask,
  IN      PCIe_WRAPPER_CONFIG            *Wrapper
  )
{
  PCIe_ENGINE_CONFIG        *EngineList;
  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if ((EngineList->EngineData.EngineType & EngineTypeMask) != 0) {
      PcieConfigDisableEngine (EngineList);
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get engine PHY lanes bitmap
 *
 *
 *
 * @param[in]   Engine              Pointer to engine config descriptor
 */
UINT32
PcieConfigGetEnginePhyLaneBitMap (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  UINT32  LaneBitMap;
  LaneBitMap = 0;
  if (PcieLibIsEngineAllocated (Engine)) {
    LaneBitMap = ((1 << PcieConfigGetNumberOfPhyLane (Engine)) - 1) <<  (PcieLibGetLoPhyLane (Engine) - PcieConfigGetParentWrapper (Engine)->StartPhyLane);
  }
  return LaneBitMap;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get number of phy lanes
 *
 *
 *
 * @param[in]  Engine      Pointer to engine config descriptor
 * @retval                 Number of Phy lane
 */
UINT8
PcieConfigGetNumberOfPhyLane (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  if (Engine->EngineData.StartLane >= UNUSED_LANE_ID || Engine->EngineData.EndLane >= UNUSED_LANE_ID) {
    return 0;
  }
  if (Engine->EngineData.StartLane > Engine->EngineData.EndLane) {
    return (UINT8) (Engine->EngineData.StartLane - Engine->EngineData.EndLane + 1);
  } else {
    return (UINT8) (Engine->EngineData.EndLane - Engine->EngineData.StartLane + 1);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get port configuration signature for given wrapper and core
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to wrapper config descriptor
 * @param[in]  CoreId      Core ID
 * @retval                 Configuration Signature
 */
UINT64
PcieConfigGetConfigurationSignature (
  IN      PCIe_WRAPPER_CONFIG            *Wrapper,
  IN      UINT8                          CoreId
  )
{
  UINT64              ConfigurationSignature;
  PCIe_ENGINE_CONFIG  *EngineList;
  ConfigurationSignature = 0;
  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if (PcieConfigIsPcieEngine (EngineList) && EngineList->Type.Port.CoreId == CoreId) {
      ConfigurationSignature = (ConfigurationSignature << 8) | PcieConfigGetNumberOfCoreLane (EngineList);
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
  IDS_HDT_CONSOLE (PCIE_MISC, "  CoreId - %x      ConfigurationSignature - %x\n", CoreId, ConfigurationSignature);
  return ConfigurationSignature;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check Port Status
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  PortStatus      Check if status asserted for port
 * @retval                     TRUE if status asserted
 */
BOOLEAN
PcieConfigCheckPortStatus (
  IN       PCIe_ENGINE_CONFIG            *Engine,
  IN       UINT32                        PortStatus
  )
{
  return (Engine->InitStatus & PortStatus) == 0 ? FALSE : TRUE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set/Reset port status
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  SetStatus       SetStatus
 * @param[in]  ResetStatus     ResetStatus
 *
 */
UINT16
PcieConfigUpdatePortStatus (
  IN       PCIe_ENGINE_CONFIG               *Engine,
  IN       PCIe_ENGINE_INIT_STATUS          SetStatus,
  IN       PCIe_ENGINE_INIT_STATUS          ResetStatus
  )
{
  Engine->InitStatus |= SetStatus;
  Engine->InitStatus &= (~ResetStatus);
  return Engine->InitStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Execute callback on all descriptor of specific type
 *
 *
 * @param[in]       InDescriptorFlags    Include descriptor flags
 * @param[in]       OutDescriptorFlags   Exclude descriptor flags
 * @param[in]       TerminationFlags     Termination flags
 * @param[in]       Callback             Pointer to callback function
 * @param[in, out]  Buffer               Pointer to buffer to pass information to callback
 * @param[in]       Pcie                 Pointer to global PCIe configuration
 */

AGESA_STATUS
PcieConfigRunProcForAllDescriptors (
  IN       UINT32                           InDescriptorFlags,
  IN       UINT32                           OutDescriptorFlags,
  IN       UINT32                           TerminationFlags,
  IN       PCIe_RUN_ON_DESCRIPTOR_CALLBACK  Callback,
  IN OUT   VOID                             *Buffer,
  IN       PCIe_PLATFORM_CONFIG             *Pcie
  )
{
  AGESA_STATUS            AgesaStatus;
  AGESA_STATUS            Status;
  PCIe_DESCRIPTOR_HEADER  *Descriptor;

  AgesaStatus = AGESA_SUCCESS;
  Descriptor = PcieConfigGetChild (InDescriptorFlags & DESCRIPTOR_ALL_TYPES, &Pcie->Header);
  while (Descriptor != NULL) {
    if ((InDescriptorFlags & Descriptor->DescriptorFlags) != 0 && (OutDescriptorFlags && Descriptor->DescriptorFlags) == 0) {
      Status = Callback (Descriptor, Buffer, Pcie);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
    }
    Descriptor = (PCIe_DESCRIPTOR_HEADER *) PcieConfigGetNextTopologyDescriptor (Descriptor, TerminationFlags);
  }
  return AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Execute callback on all wrappers in topology
 *
 *
 * @param[in]       DescriptorFlags   Wrapper Flags
 * @param[in]       Callback          Pointer to callback function
 * @param[in, out]  Buffer            Pointer to buffer to pass information to callback
 * @param[in]       Pcie              Pointer to global PCIe configuration
 */

AGESA_STATUS
PcieConfigRunProcForAllWrappers (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_WRAPPER_CALLBACK  Callback,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_WRAPPER_CONFIG   *Wrapper;

  AgesaStatus = AGESA_SUCCESS;
  Wrapper = (PCIe_WRAPPER_CONFIG *) PcieConfigGetChild (DESCRIPTOR_ALL_WRAPPERS, &Pcie->Header);
  while (Wrapper != NULL) {
    if ((DescriptorFlags & DESCRIPTOR_ALL_WRAPPERS & Wrapper->Header.DescriptorFlags) != 0) {
      Status = Callback (Wrapper, Buffer, Pcie);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
    }
    Wrapper = (PCIe_WRAPPER_CONFIG *) PcieConfigGetNextTopologyDescriptor (Wrapper, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }
  return AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Execute callback on all wrappers in NBIO
 *
 *
 * @param[in]       DescriptorFlags   Wrapper Flags
 * @param[in]       Callback          Pointer to callback function
 * @param[in, out]  Buffer            Pointer to buffer to pass information to callback
 * @param[in]       Pcie              Pointer to global PCIe configuration
 */

VOID
PcieConfigRunProcForAllWrappersInNbio (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_WRAPPER_CALLBACK2 Callback,
  IN OUT   VOID                          *Buffer,
  IN       GNB_HANDLE                    *GnbHandle
  )
{
  PCIe_WRAPPER_CONFIG   *Wrapper;

  Wrapper = PcieConfigGetChildWrapper (GnbHandle);
  while ((Wrapper != NULL) && (GnbHandle == (GNB_HANDLE *) PcieConfigGetParentSilicon (Wrapper))) {
    if ((DescriptorFlags & DESCRIPTOR_ALL_WRAPPERS & Wrapper->Header.DescriptorFlags) != 0) {
      Callback (Wrapper, Buffer, GnbHandle);
    }
    Wrapper = (PCIe_WRAPPER_CONFIG *) PcieConfigGetNextTopologyDescriptor (Wrapper, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }
  return;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Execute callback on all engine in topology
 *
 *
 * @param[in]       DescriptorFlags Engine flags.
 * @param[in]       Callback        Pointer to callback function
 * @param[in, out]  Buffer          Pointer to buffer to pass information to callback
 * @param[in]       Pcie            Pointer to global PCIe configuration
 */

VOID
PcieConfigRunProcForAllEngines (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_ENGINE_CALLBACK   Callback,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{

  PCIe_ENGINE_CONFIG  *Engine;
  Engine = (PCIe_ENGINE_CONFIG *) PcieConfigGetChild (DESCRIPTOR_ALL_ENGINES, &Pcie->Header);
  while (Engine != NULL) {
    if (!((DescriptorFlags & DESCRIPTOR_ALLOCATED) != 0 && !PcieLibIsEngineAllocated (Engine))) {
      if ((Engine->Header.DescriptorFlags & DESCRIPTOR_ALL_ENGINES & DescriptorFlags) != 0) {
        Callback (Engine, Buffer, Pcie);
      }
    }
    Engine = (PCIe_ENGINE_CONFIG *) PcieConfigGetNextTopologyDescriptor (Engine, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Execute callback on all engine in wrapper
 *
 *
 * @param[in]       DescriptorFlags Engine flags.
 * @param[in]       Callback        Pointer to callback function
 * @param[in, out]  Buffer          Pointer to buffer to pass information to callback
 * @param[in]       Pcie            Pointer to global PCIe configuration
 */

VOID
PcieConfigRunProcForAllEnginesInWrapper (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_ENGINE_CALLBACK2  Callback,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_WRAPPER_CONFIG           *Wrapper
  )
{

  PCIe_ENGINE_CONFIG  *Engine;
  Engine = PcieConfigGetChildEngine (Wrapper);
  while ((Engine != NULL) && (Wrapper == PcieConfigGetParentWrapper(Engine))) {
    if (!((DescriptorFlags & DESCRIPTOR_ALLOCATED) != 0 && !PcieLibIsEngineAllocated (Engine))) {
      if ((Engine->Header.DescriptorFlags & DESCRIPTOR_ALL_ENGINES & DescriptorFlags) != 0) {
        Callback (Engine, Buffer, Wrapper);
      }
    }
    Engine = (PCIe_ENGINE_CONFIG *) PcieConfigGetNextTopologyDescriptor (Engine, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get parent descriptor of specific type
 *
 *
 * @param[in]       Type            Descriptor type
 * @param[in]       Descriptor      Pointer to buffer to pass information to callback
 */
PCIe_DESCRIPTOR_HEADER *
PcieConfigGetParent (
  IN       UINT32                        Type,
  IN       PCIe_DESCRIPTOR_HEADER        *Descriptor
  )
{
  while ((Descriptor->DescriptorFlags & Type) == 0) {
    if (Descriptor->Parent != 0) {
      Descriptor = (PCIe_DESCRIPTOR_HEADER *) ((UINT8 *) Descriptor - Descriptor->Parent);
    } else {
      return NULL;
    }
  }
  return Descriptor;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get child descriptor of specific type
 *
 *
 * @param[in]       Type            Descriptor type
 * @param[in]       Descriptor      Pointer to buffer to pass information to callback
 */
PCIe_DESCRIPTOR_HEADER *
PcieConfigGetChild (
  IN       UINT32                        Type,
  IN       PCIe_DESCRIPTOR_HEADER        *Descriptor
  )
{
  while ((Descriptor->DescriptorFlags & Type) == 0) {
    if (Descriptor->Child != 0) {
      Descriptor = (PCIe_DESCRIPTOR_HEADER *) ((UINT8 *) Descriptor + Descriptor->Child);
    } else {
      return NULL;
    }
  }
  return Descriptor;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get peer descriptor of specific type
 *
 *
 * @param[in]       Type            Descriptor type
 * @param[in]       Descriptor      Pointer to buffer to pass information to callback
 */
PCIe_DESCRIPTOR_HEADER *
PcieConfigGetPeer (
  IN       UINT32                        Type,
  IN       PCIe_DESCRIPTOR_HEADER        *Descriptor
  )
{
  ASSERT (Descriptor != NULL);
  while ((Descriptor->DescriptorFlags & Type) == 0) {
    if (Descriptor->Peer != 0) {
      Descriptor = (PCIe_DESCRIPTOR_HEADER *) ((UINT8 *) Descriptor + Descriptor->Peer);
    } else {
      return NULL;
    }
  }
  return Descriptor;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check is engine is active or potentially active
 *
 *
 *
 * @param[in]  Engine      Pointer to engine descriptor
 * @retval                 TRUE  - engine active
 * @retval                 FALSE - engine not active
 */
BOOLEAN
PcieConfigIsActivePcieEngine (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  BOOLEAN Result;
  ASSERT (PcieConfigIsPcieEngine (Engine));
  Result = FALSE;
  if (PcieConfigIsEngineAllocated (Engine)) {
    if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS) ||
      (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled && Engine->Type.Port.PortData.LinkHotplug != HotplugInboard)) {
      Result = TRUE;
    }
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Locate SB engine on wrapper
 *
 *
 *
 * @param[in]  Wrapper     Pointer to wrapper config descriptor
 * @retval                 SB engine pointer or NULL
 */
PCIe_ENGINE_CONFIG *
PcieConfigLocateSbEngine (
  IN      PCIe_WRAPPER_CONFIG            *Wrapper
  )
{
  PCIe_ENGINE_CONFIG  *EngineList;
  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if (PcieConfigIsSbPcieEngine (EngineList)) {
      return EngineList;
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
  return NULL;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump engine configuration
 *
 *
 * @param[in]  EngineList           Engine Configuration
 */
VOID
PcieConfigEngineDebugDump (
  IN      PCIe_ENGINE_CONFIG        *EngineList
  )
{
  if (EngineList->EngineData.StartLane != 255) {
    IDS_HDT_CONSOLE (PCIE_MISC, "        Descriptor Flags - 0x%08x\n", EngineList->Header.DescriptorFlags);
    IDS_HDT_CONSOLE (PCIE_MISC, "        Engine Type - PCIe Port\n        Start Lane - %d  |||  End Lane - %d\n",
      EngineList->EngineData.StartLane,
      EngineList->EngineData.EndLane
      );
    IDS_HDT_CONSOLE(PCIE_MISC, "        Scratch        - %d\n", EngineList->Scratch);
    IDS_HDT_CONSOLE (PCIE_MISC, "        Init Status    - 0x%08x\n", EngineList->InitStatus);
    IDS_HDT_CONSOLE (PCIE_MISC, "        PCIe port configuration:\n");
    IDS_HDT_CONSOLE (PCIE_MISC, "          Port Training - %a\n",
      (EngineList->Type.Port.PortData.PortPresent == PortDisabled) ? "Disable" : "Enabled"
      );
    IDS_HDT_CONSOLE (PCIE_MISC, "          Requested PCI Dev Number   - %d\n",EngineList->Type.Port.PortData.DeviceNumber);
    IDS_HDT_CONSOLE (PCIE_MISC, "          Requested PCI Func Number  - %d\n",EngineList->Type.Port.PortData.FunctionNumber);
    IDS_HDT_CONSOLE (PCIE_MISC, "          PCI Address - %d:%d:%d\n",
      EngineList->Type.Port.Address.Address.Bus,
      EngineList->Type.Port.Address.Address.Device,
      EngineList->Type.Port.Address.Address.Function
      );
    IDS_HDT_CONSOLE (PCIE_MISC, "          EngineId - %d\n",
      EngineList->Type.Port.PcieBridgeId
      );
    IDS_HDT_CONSOLE (PCIE_MISC, "          Misc Control  - 0x%02x\n", EngineList->Type.Port.PortData.MiscControls);
    IDS_HDT_CONSOLE (PCIE_MISC, "          Hotplug - %a\n",
      (EngineList->Type.Port.PortData.LinkHotplug == HotplugDisabled) ? "Disabled" : (
      (EngineList->Type.Port.PortData.LinkHotplug == HotplugBasic) ? "Basic" : (
      (EngineList->Type.Port.PortData.LinkHotplug == HotplugServer) ? "Server" : (
      (EngineList->Type.Port.PortData.LinkHotplug == HotplugEnhanced) ? "Enhanced" : (
      (EngineList->Type.Port.PortData.LinkHotplug == HotplugServerSsd) ? "ServerSsd" : (
      (EngineList->Type.Port.PortData.LinkHotplug == HotplugInboard) ? "Inboard" : "Unknown")))))
      );
    IDS_HDT_CONSOLE (PCIE_MISC, "          ASPM    - %a\n",
      (EngineList->Type.Port.PortData.LinkAspm == AspmDisabled) ? "Disabled" : (
      (EngineList->Type.Port.PortData.LinkAspm == AspmL0s) ? "L0s" : (
      (EngineList->Type.Port.PortData.LinkAspm == AspmL1) ? "L1" :  (
      (EngineList->Type.Port.PortData.LinkAspm == AspmL0sL1) ? "L0s & L1" : "Unknown")))
      );
    IDS_HDT_CONSOLE (PCIE_MISC, "          Speed   - %d\n",
      EngineList->Type.Port.PortData.LinkSpeedCapability
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump wrapper configuration
 *
 *
 * @param[in]  WrapperList           Wrapper Configuration
 */
VOID
PcieConfigWrapperDebugDump (
  IN      PCIe_WRAPPER_CONFIG        *WrapperList
  )
{
  PCIe_ENGINE_CONFIG    *EngineList;
  IDS_HDT_CONSOLE (PCIE_MISC, "      <---------Wrapper -  Config -------->\n");
  IDS_HDT_CONSOLE (PCIE_MISC, "        Start  PHY lane - %02d\n", WrapperList->StartPhyLane);
  IDS_HDT_CONSOLE (PCIE_MISC, "        End    PHY lane - %02d\n", WrapperList->EndPhyLane);
  IDS_HDT_CONSOLE (PCIE_MISC, "        Start DXIO lane - %02d\n", WrapperList->StartDxioLane);
  IDS_HDT_CONSOLE (PCIE_MISC, "        Descriptor Flags - 0x%08x\n", WrapperList->Header.DescriptorFlags);
  IDS_HDT_CONSOLE (PCIE_MISC, "      <---------Wrapper -  Config End----->\n");
  EngineList = PcieConfigGetChildEngine (WrapperList);
  while (EngineList != NULL) {
//    if (PcieLibIsEngineAllocated (EngineList)) {
      PcieConfigEngineDebugDump (EngineList);
//    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump configuration to debug out
 *
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieConfigDebugDump (
  IN      PCIe_PLATFORM_CONFIG        *Pcie
  )
{
  PCIe_SILICON_CONFIG   *SiliconList;
  PCIe_WRAPPER_CONFIG   *WrapperList;
  PCIe_COMPLEX_CONFIG   *ComplexList;
  ComplexList = (PCIe_COMPLEX_CONFIG *) PcieConfigGetChild (DESCRIPTOR_COMPLEX, &Pcie->Header);
  IDS_HDT_CONSOLE (PCIE_MISC, "<-------------- PCIe Config Start------------>\n");
#if DEBUG_HEX_DUMP
  GnbLibDebugDumpBuffer ((VOID*) ComplexList, 0x1800, 1, 16);
#endif
  IDS_HDT_CONSOLE (PCIE_MISC, "  PSPP Policy - %a\n",
   (Pcie->PsppPolicy == AgesaPsppPowerSaving) ? "Power Saving" :
   (Pcie->PsppPolicy == AgesaPsppBalance) ? "Balanced Mode" : (
   (Pcie->PsppPolicy == AgesaPsppPerformance) ? "Performance" : (
   (Pcie->PsppPolicy == AgesaPsppDisabled) ? "Disabled" : "Unknown"))
    );

  while (ComplexList != NULL) {
    IDS_HDT_CONSOLE (PCIE_MISC, "  <---------- Complex Config Start ---------->\n");
    IDS_HDT_CONSOLE (PCIE_MISC, "    Descriptor Flags - 0x%08x\n", ComplexList->Header.DescriptorFlags);
    IDS_HDT_CONSOLE (PCIE_MISC, "    Node ID - %d\n", ComplexList->NodeId);
    SiliconList = PcieConfigGetChildSilicon (ComplexList);
    while (SiliconList != NULL) {
      IDS_HDT_CONSOLE (PCIE_MISC, "    <---------- Silicon Config Start -------->\n");
      IDS_HDT_CONSOLE (PCIE_MISC, "      Descriptor Flags - 0x%08x\n", SiliconList->Header.DescriptorFlags);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Socket Number  - %d\n", SiliconList->SocketId);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Die Number     - %d\n", SiliconList->DieNumber);
      IDS_HDT_CONSOLE (PCIE_MISC, "      RBIndex        - %d\n", SiliconList->RBIndex);
      IDS_HDT_CONSOLE (PCIE_MISC, "      LogicalRBIndex - %d\n", SiliconList->LogicalRBIndex);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Instance ID    - %d\n", SiliconList->InstanceId);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Start Lane     - %d\n", SiliconList->StartLane);
      IDS_HDT_CONSOLE (PCIE_MISC, "      End Lane       - %d\n", SiliconList->EndLane);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Start Bus      - %d\n", SiliconList->Address.Address.Bus);
      IDS_HDT_CONSOLE (PCIE_MISC, "      End Bus        - %d\n", SiliconList->BusNumberLimit);
      IDS_HDT_CONSOLE (PCIE_MISC, "      SmuPresent     - %d\n", SiliconList->SmuPresent);
      IDS_HDT_CONSOLE (PCIE_MISC, "      SbPresent      - %d\n", SiliconList->SbPresent);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Host PCI Address - %d:%d:%d\n",
        SiliconList->Address.Address.Bus,
        SiliconList->Address.Address.Device,
        SiliconList->Address.Address.Function
        );

      WrapperList = PcieConfigGetChildWrapper (SiliconList);
      while (WrapperList != NULL) {
        PcieConfigWrapperDebugDump (WrapperList);
        WrapperList = PcieLibGetNextDescriptor (WrapperList);
      }
      IDS_HDT_CONSOLE (PCIE_MISC, "    <---------- Silicon Config End ---------->\n");
      SiliconList = PcieLibGetNextDescriptor (SiliconList);
    }
    IDS_HDT_CONSOLE (PCIE_MISC, "  <---------- Complex Config End ------------>\n");
    if ((ComplexList->Header.DescriptorFlags & DESCRIPTOR_TERMINATE_TOPOLOGY) == 0) {
      ComplexList++;
    } else {
      ComplexList = NULL;
    }
  }
  IDS_HDT_CONSOLE (PCIE_MISC, "<-------------- PCIe Config End-------------->\n");
}


/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump input configuration to user engine descriptor
 *
 *
 * @param[in]  EngineDescriptor   Pointer to engine descriptor
 */
VOID
PcieUserDescriptorConfigDump (
  IN      PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor
  )
{

  IDS_HDT_CONSOLE (PCIE_MISC, "  Engine Type - %a\n",
    (EngineDescriptor->EngineData.EngineType == PciePortEngine) ? "PCIe Port" : (
    (EngineDescriptor->EngineData.EngineType == PcieDdiEngine) ? "DDI Link" : (
    (EngineDescriptor->EngineData.EngineType == PcieUnusedEngine) ? "Unused" : "Invalid"))
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "    Start Phy Lane - %d\n    End   Phy Lane - %d\n",
    EngineDescriptor->EngineData.StartLane,
    EngineDescriptor->EngineData.EndLane
  );
  if (EngineDescriptor->EngineData.EngineType == PciePortEngine) {
    IDS_HDT_CONSOLE (PCIE_MISC, "    PortPresent - %d\n    ChannelType  - %d\n    DeviceNumber - %d\n    FunctionNumber - %d\n    LinkSpeedCapability - %d\n    LinkAspm - %d\n    LinkHotplug - %d\n    ResetId - %d\n    SB link - %d\n    MiscControls - 0x%02x\n" ,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.PortPresent,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.ChannelType,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.DeviceNumber,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.FunctionNumber,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkSpeedCapability,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkAspm,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkHotplug,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.ResetId,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.MiscControls.SbLink,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.MiscControls
    );
  }
  if (EngineDescriptor->EngineData.EngineType == PcieDdiEngine) {
    IDS_HDT_CONSOLE (PCIE_MISC, "    ConnectorType - %d\n    AuxIndex  - %d\n    HdpIndex - %d\n" ,
      ((PCIe_DDI_DESCRIPTOR *) EngineDescriptor)->Ddi.ConnectorType,
      ((PCIe_DDI_DESCRIPTOR *) EngineDescriptor)->Ddi.AuxIndex,
      ((PCIe_DDI_DESCRIPTOR *) EngineDescriptor)->Ddi.HdpIndex
    );
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump input configuration to debug out
 *
 *
 * @param[in]  ComplexDescriptor   Pointer to user defined complex descriptor
 */
VOID
PcieUserConfigConfigDump (
  IN      PCIe_COMPLEX_DESCRIPTOR     *ComplexDescriptor
  )
{
  PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor;
  PCIe_COMPLEX_DESCRIPTOR     *CurrentComplexDescriptor;
  UINTN                       ComplexIndex;
  UINTN                       Index;
  UINTN                       NumberOfEngines;
  UINTN                       NumberOfComplexes;

  IDS_HDT_CONSOLE (PCIE_MISC, "<---------- PCIe User Config Start------------->\n");

  NumberOfComplexes = PcieInputParserGetNumberOfComplexes (ComplexDescriptor);
  for (ComplexIndex = 0; ComplexIndex < NumberOfComplexes; ++ComplexIndex) {
    CurrentComplexDescriptor = PcieInputParserGetComplexDescriptor (ComplexDescriptor, ComplexIndex);
    NumberOfEngines = PcieInputParserGetNumberOfEngines (CurrentComplexDescriptor);
    IDS_HDT_CONSOLE (PCIE_MISC, "  ComplexDescriptor Node - %d\n  NumberOfEngines - %d\n",
        ComplexDescriptor->SocketId,
        NumberOfEngines
      );

    for (Index = 0; Index < NumberOfEngines; Index++) {
      EngineDescriptor = PcieInputParserGetEngineDescriptor (ComplexDescriptor, Index);
      PcieUserDescriptorConfigDump (EngineDescriptor);
    }
  }
  IDS_HDT_CONSOLE (PCIE_MISC, "<---------- PCIe User Config End-------------->\n");
}

