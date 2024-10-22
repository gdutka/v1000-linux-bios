/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to Set PCIe topology segment ASPM L1SS
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2020-01-31 11:02:43 +0800 (Fri, 31 Jan 2020) $
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
#include <GnbDxio.h>
#include <Filecode.h>
#include <Library/IdsLib.h>
#include <Library/GnbPcieConfigLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/NbioSmuV10Lib.h>
#include <Library/PcieMiscCommLib.h>
#include <Library/SmnAccessLib.h>
#include <GnbRegistersRV.h>

//#define FILECODE        NBIO_PCIE_RN_DXE_AMDNBIOPCIEDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define DXIO_MSG_RNTM_MBOXROUTINES         58
#define FUSE_ACCESS_LOCATION               0xFFFE0000
#define FUSE_INSTANCE_MULTIPLIER           0x1000
#define HOLD_TRAINING_A_INDEX              173  ///< HOLD_TRAINING_A
#define HOLD_TRAINING_B_INDEX              254  ///< HOLD_TRAINING_B
#define PORT_STRAP_INDEX(INDEX, PORT) (INDEX + (PORT * (HOLD_TRAINING_B_INDEX - HOLD_TRAINING_A_INDEX)))
#ifndef PORT_SPACE
  #define  PORT_SPACE(HANDLE, WRAPPER, PORTINDEX, ADDRESS)   (ADDRESS + (HANDLE->RBIndex << 20) + (WRAPPER->WrapId << 22) + (PORTINDEX << 12))
#endif
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN CheckRvOpn(VOID);

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to write pcie soft straps
 *
 *
 *
 * @param[in]  GnbHandle         The associated Gnb Handle
 * @param[in]  StrapIndex        Strap index
 * @param[out] Value             Pointer to byte
 * @param[in]  Wrapper           Pcie wrapper number
 */
VOID
WritePcieStrap (
  IN   GNB_HANDLE   *GnbHandle,
  IN   UINT16        StrapIndex,
  IN   UINT32       Value,
  IN   UINT8        Wrapper
  )
{
  UINT32 Index;
  UINT8  InstanceNumber;

  Index = FUSE_ACCESS_LOCATION;
  Index += StrapIndex;
  InstanceNumber = GnbHandle->RBIndex + Wrapper * 4; /// RBIndex is the NBIO #

  SmnRegisterWrite (
    GnbHandle->Address.Address.Bus,
    PCIE0_STRAP_INDEX_ADDRESS+ FUSE_INSTANCE_MULTIPLIER * InstanceNumber,
    &Index,
    0
    );

  SmnRegisterWrite (
    GnbHandle->Address.Address.Bus,
     PCIE0_STRAP_DATA_ADDRESS + FUSE_INSTANCE_MULTIPLIER * InstanceNumber,
     &Value,
     0
     );
}


/**----------------------------------------------------------------------------------------*/
/**
 * Interface to Disable ASPM
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieAspmBlackListInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;
  UINT32                         AspmData;
  UINT16                         Index;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmBlackListInterface Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine)) {
        if (PcieConfigCheckPortStatus (PcieEngine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
          AspmData = PcieDisableASPMCallback (PcieEngine);
          if (AspmData != 0xFFFF) {
            // Configure PM Support
            //<Embedded_Override_Start>
            if(CheckRvOpn()) {
              Index = PORT_STRAP_INDEX(STRAP_BIF_PM_SUPPORT_A_INDEX_RV1, PcieEngine->Type.Port.PortId);
            }
            else {
              Index = PORT_STRAP_INDEX(STRAP_BIF_PM_SUPPORT_A_INDEX_RV2, PcieEngine->Type.Port.PortId);
            }
            //<Embedded_Override_End>
            IDS_HDT_CONSOLE (GNB_TRACE, " - writing strap 0x%x to ASPM %d\n", AspmData);
            WritePcieStrap (GnbHandle, Index, AspmData & 0x3, PcieWrapper->WrapId);
          }

          if ((PcieEngine->Type.Port.LinkAspmL1_1 == 1) || (PcieEngine->Type.Port.LinkAspmL1_2 == 1)) {
            if (PcieDisableL1SSCallback (PcieEngine)) {
              // PCIE_CNTL::HWINIT_WR_LOCK = 0
              SmnRegisterRMW (GnbHandle->Address.Address.Bus,
                    PCIE0_CNTL_ADDRESS,
                    (UINT32) ~(PCIE_CNTL_HWINIT_WR_LOCK_MASK),
                    0 << PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
                    GNB_REG_ACC_FLAG_S3SAVE
                    );
              if (PcieEngine->Type.Port.LinkAspmL1_1 == 0) {
                // PCIE_L1_PM_SUB_CAP_funcX::PCI_PM_L1_1_SUPPORTED = 0
                // PCIE_L1_PM_SUB_CAP_funcX::ASPM_L1_1_SUPPORTED = 0
               SmnRegisterRMW (GnbHandle->Address.Address.Bus,
                         PORT_SPACE (GnbHandle, PcieWrapper, (PcieEngine->Type.Port.PortId % 8), SMN_PCIE0FUNC0_PCIE_L1_PM_SUB_CAP_ADDRESS),
                         (UINT32) ~(PCIE_L1_PM_SUB_CNTL_PCI_PM_L1_1_EN_MASK | PCIE_L1_PM_SUB_CNTL_ASPM_L1_1_EN_MASK),
                         (0x0 << PCIE_L1_PM_SUB_CNTL_PCI_PM_L1_1_EN_OFFSET) | (0x0 << PCIE_L1_PM_SUB_CNTL_ASPM_L1_1_EN_OFFSET),
                         GNB_REG_ACC_FLAG_S3SAVE
                         );
              }
              if (PcieEngine->Type.Port.LinkAspmL1_2 == 0) {
                // PCIE_L1_PM_SUB_CAP_funcX::PCI_PM_L1_2_SUPPORTED = 0
                // PCIE_L1_PM_SUB_CAP_funcX::ASPM_L1_2_SUPPORTED = 0
               SmnRegisterRMW (GnbHandle->Address.Address.Bus,
                         PORT_SPACE (GnbHandle, PcieWrapper, (PcieEngine->Type.Port.PortId % 8), SMN_PCIE0FUNC0_PCIE_L1_PM_SUB_CAP_ADDRESS),
                         (UINT32) ~(PCIE_L1_PM_SUB_CNTL_PCI_PM_L1_2_EN_MASK | PCIE_L1_PM_SUB_CNTL_ASPM_L1_2_EN_MASK),
                         (0x0 << PCIE_L1_PM_SUB_CNTL_PCI_PM_L1_2_EN_OFFSET) | (0x0 << PCIE_L1_PM_SUB_CNTL_ASPM_L1_2_EN_OFFSET),
                         GNB_REG_ACC_FLAG_S3SAVE
                         );
              }
              // PCIE_CNTL::HWINIT_WR_LOCK = 1
              SmnRegisterRMW (GnbHandle->Address.Address.Bus,
                    PCIE0_CNTL_ADDRESS,
                    (UINT32) ~(PCIE_CNTL_HWINIT_WR_LOCK_MASK),
                    1 << PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
                    GNB_REG_ACC_FLAG_S3SAVE
                    );
            }
          }
        }
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmBlackListInterface Exit\n");
}
