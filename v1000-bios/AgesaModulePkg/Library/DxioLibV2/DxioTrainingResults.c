/* $NoKeywords:$ */
/**
 * @file
 *
 * DXIO initialization services
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <PiPei.h>
#include <AmdPcieComplex.h>
#include <GnbDxio.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/GnbLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/DxioLibV2.h>
#include <Library/OemGpioResetControlLib.h>
#include "DxioLibLocal.h"

#define FILECODE LIBRARY_DXIOLIBV2_DXIOTRAININGRESULTS_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

///Port information needed for callback
typedef struct {
  UINT8           SocketNumber;       ///< SocketNumber
  UINT8           StartLane;          ///< Start Lane from User Config
  UINT8           MacPortID;          ///< Translated Start Lane
  BOOLEAN         TrainingSuccess;    ///< Flag for lanes found
} PORT_INFO_CALLBACK;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to fixup Pcie Platform Configuration for package specific values
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */
AGESA_STATUS
STATIC
DxioSetPortStateCallback (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      VOID                  *Buffer,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GNB_HANDLE              *GnbHandle;
  PORT_INFO_CALLBACK      *PortInfo;
  PCIe_ENGINE_CONFIG      *Engine;

  PortInfo = (PORT_INFO_CALLBACK *) Buffer;
  GnbHandle = (GNB_HANDLE *) PcieConfigGetParentSilicon (Wrapper);
  if (GnbHandle->SocketId == PortInfo->SocketNumber) {
    if ((PortInfo->StartLane >= Wrapper->StartDxioLane) && (PortInfo->StartLane <= Wrapper->EndDxioLane)) {
      // Find the PCIe structure entry for this DXIO engine
      Engine = PcieConfigGetChildEngine (Wrapper);
      while (Engine != NULL) {
        if (Engine->Type.Port.PortId == PortInfo->MacPortID) {
          Engine->InitStatus = INIT_STATUS_PCIE_TRAINING_SUCCESS;
          break;
        }
        Engine = PcieLibGetNextDescriptor (Engine);
      }
    }
  }
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * DXIO User Config Parser
 *
 *
 *
 * @param[in]  Pcie           PCIe configuration info
 * @param[in]  SocketNumber   SocketNumber for this port from User Config
 * @param[in]  StartLane      StartLane for this port from User Config
 * @param[in]          EndLane for this port from User Config
 * @retval     BOOLEAN
 */
VOID
DxioFindPortAndSetState (
  IN       PCIe_PLATFORM_CONFIG      *Pcie,
  IN       UINT8                     SocketNumber,
  IN       UINT8                     StartLane,
  IN       UINT8                     MacPortID,
  IN       BOOLEAN                   TrainingSuccess
  )
{
  PORT_INFO_CALLBACK    PortInfo;
  AGESA_STATUS          AgesaStatus;

  PortInfo.SocketNumber = SocketNumber;
  PortInfo.StartLane = StartLane;
  PortInfo.MacPortID = MacPortID;
  PortInfo.TrainingSuccess = TrainingSuccess;
  AgesaStatus = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, DxioSetPortStateCallback, &PortInfo, Pcie);
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * DXIO Update Port Training Status
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  ThisNode       Pointer to the platform descriptor for this node
 * @retval     AGESA_STATUS
 */
VOID
DxioUpdatePortTrainingStatus (
  IN       GNB_HANDLE                *GnbHandle,
  IN       DXIO_COMPLEX_DESCRIPTOR   *UserConfig,
  OUT      DXIO_PLATFORM_DESCRIPTOR  *ThisNode
  )
{
  // DXIO Firmware Structures
  DXIO_PLATFORM_DESCRIPTOR  *DxioNodePlatformDesc;
  DXIO_ENGINE_DESCRIPTOR    *DxioEnginePointer;
  UINT32                    DxioNodeCount;
  UINT8                     SocketNumber;
  UINT32                    Index;
  BOOLEAN                   TrainingSuccess;
  PCIe_PLATFORM_CONFIG      *Pcie;

  IDS_HDT_CONSOLE (PCIE_MISC, "%a : Enter\n", __FUNCTION__);

  Pcie = PcieConfigGetPlatform(GnbHandle);
  // Get Socket Number
  SocketNumber = GnbHandle->SocketId;

  DxioNodePlatformDesc = ThisNode;
  DxioNodeCount = DxioNodePlatformDesc->NumEngineDesc;
  DxioNodePlatformDesc ++;                                              // Point to first engine descriptor
  DxioEnginePointer = (DXIO_ENGINE_DESCRIPTOR *)DxioNodePlatformDesc;   // Initialize engine pointer to first engine

  for (Index = 0;Index < DxioNodeCount;Index++) {
    TrainingSuccess = FALSE;
    if (DxioEnginePointer->EngineType == DxioPcieEngine) {
      IDS_HDT_CONSOLE (PCIE_MISC, "  Training State for Device %d Function %d is 0x%x\n",
                       DxioEnginePointer->LinkDesc.Pcie.DevFunNumber >> 3,
                       DxioEnginePointer->LinkDesc.Pcie.DevFunNumber & 0x7,
                       DxioEnginePointer->LinkDesc.Pcie.linkTrainingState
                       );
      if (DxioEnginePointer->LinkDesc.Pcie.linkTrainingState == DXIO_LinkStateTrainingSuccess) {
        TrainingSuccess = TRUE;
        DxioFindPortAndSetState (Pcie, SocketNumber, (UINT8)DxioEnginePointer->StartLane, (UINT8)DxioEnginePointer->LinkDesc.Pcie.MacPortID, TrainingSuccess);
        IDS_HDT_CONSOLE (PCIE_MISC, "  linkTrainingState indicates SUCCESS");
      } else {
        TrainingSuccess = FALSE;
        IDS_HDT_CONSOLE (PCIE_MISC, "  linkTrainingState indicates FAILURE");
      }
    }
    if ((TrainingSuccess != TRUE) || (DxioEnginePointer->LinkDesc.Pcie.Capabilities.portPresent == 0)) {
      IDS_HDT_CONSOLE (PCIE_MISC, "Training failed on Device %x Function %x\n",
                       DxioEnginePointer->LinkDesc.Pcie.DevFunNumber >> 3,
                       DxioEnginePointer->LinkDesc.Pcie.DevFunNumber & 0x7
                       );
    } else {
      IDS_HDT_CONSOLE (PCIE_MISC, "Training SUCCESS on Device %x Function %x\n",
                       DxioEnginePointer->LinkDesc.Pcie.DevFunNumber >> 3,
                       DxioEnginePointer->LinkDesc.Pcie.DevFunNumber & 0x7
                       );
    }
    DxioEnginePointer++;
  }

  IDS_HDT_CONSOLE (PCIE_MISC, "%a : Exit\n", __FUNCTION__);
  return;
}


