/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe configuration data
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 312288 $   @e \$Date: 2015-02-04 00:39:01 -0600 (Wed, 04 Feb 2015) $
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
#include <PiDxe.h>
#include <GnbRegistersRV.h>
#include <GnbDxio.h>
#include <IdsHookId.h>
#include <Guid/HobList.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Protocol/AmdNbioPcieAerProtocol.h>
#include "AmdNbioPcieAerRV.h"
#include "Filecode.h"

#define FILECODE NBIO_PCIE_AMDNBIOPCIERVDXE_AMDNBIOPCIEAERRV_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
NbioSetPcieAerFeature (
  IN       AMD_NBIO_PCIE_AER_PROTOCOL  *This,
  IN       PCIe_PORT_AER_CONFIG        *PcieAerSetting
);

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
AMD_NBIO_PCIe_AER_CONFIG NbioPcieAerConfig = {
  NUMBER_OF_PORTS,
  {
    { 1,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing //<Embedded_Override>
      1,          ///< PciDev
      1,          ///< PciFunc
      {{0}},      ///< CorrectableMask
      {{0}},      ///< UncorrectableMask
      {{0}},      ///< UncorrectableSeverity
    },
    { 1,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing //<Embedded_Override>
      1,          ///< PciDev
      2,          ///< PciFunc
      {{0}},      ///< CorrectableMask
      {{0}},      ///< UncorrectableMask
      {{0}},      ///< UncorrectableSeverity
    },
    { 1,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing //<Embedded_Override>
      1,          ///< PciDev
      3,          ///< PciFunc
      {{0}},      ///< CorrectableMask
      {{0}},      ///< UncorrectableMask
      {{0}},      ///< UncorrectableSeverity
    },
    { 1,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing //<Embedded_Override>
      1,          ///< PciDev
      4,          ///< PciFunc
      {{0}},      ///< CorrectableMask
      {{0}},      ///< UncorrectableMask
      {{0}},      ///< UncorrectableSeverity
    },
    { 1,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing //<Embedded_Override>
      1,          ///< PciDev
      5,          ///< PciFunc
      {{0}},      ///< CorrectableMask
      {{0}},      ///< UncorrectableMask
      {{0}},      ///< UncorrectableSeverity
    },
    { 1,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing //<Embedded_Override>
      1,          ///< PciDev
      6,          ///< PciFunc
      {{0}},      ///< CorrectableMask
      {{0}},      ///< UncorrectableMask
      {{0}},      ///< UncorrectableSeverity
    },
    { 1,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing //<Embedded_Override>
      1,          ///< PciDev
      7,          ///< PciFunc
      {{0}},      ///< CorrectableMask
      {{0}},      ///< UncorrectableMask
      {{0}},      ///< UncorrectableSeverity
    },
  }
}
  ;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AMD_NBIO_PCIE_AER_PROTOCOL mAmdNbioPcieAerProtocol = {
  NbioSetPcieAerFeature
};

/*----------------------------------------------------------------------------------------*/

/**
 *---------------------------------------------------------------------------------------
 *
 *  NbioPcieAerFeatureEnable
 *
 *  Description:
 *     Enable/Disable AER.
 *
 *  Parameters:
 *    @param[in]     GnbHandle
 *    @param[in]     PortAerConfig
 *
 *    @retval         VOID
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
NbioPcieAerFeatureEnable (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       NBIO_PCIe_PORT_AER_CONFIG  *PortAerConfig
  )
{
  UINT32                   Value;
  UINT32                   AddrIndex;
  UINT32                   CorrMaskAddr;
  UINT32                   UnCorrMaskAddr;
  UINT32                   UnCorrSeverityAddr;
  UINT32                   PcieErrCntlAddr;
  PCIe_ENGINE_CONFIG       *PcieEngine;
  PCIe_WRAPPER_CONFIG      *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioPcieAerFeatureEnable Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    AddrIndex = (PcieWrapper->WrapId == 0) ? 0x00100000 : 0x00200000;
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine)) {
        if ((PcieEngine->Type.Port.PortData.DeviceNumber == PortAerConfig->PciDev) &&
            (PcieEngine->Type.Port.PortData.FunctionNumber == PortAerConfig->PciFunc)) {
          IDS_HDT_CONSOLE (PCIE_MISC, "Set AER feature: Dev%d Func%d %x\n", PortAerConfig->PciDev, PortAerConfig->PciFunc, PortAerConfig->PortAerEnable);
          AddrIndex |= (PcieEngine->Type.Port.PortId % 8) << 12;
          PcieErrCntlAddr = 0x110401A8 | AddrIndex;
          CorrMaskAddr = 0x11000164 | AddrIndex;
          UnCorrMaskAddr = 0x11000158 | AddrIndex;
          UnCorrSeverityAddr = 0x1100015c | AddrIndex;

          if (PortAerConfig->PortAerEnable == 0) {
            NbioRegisterRead ( GnbHandle, TYPE_SMN, PcieErrCntlAddr, &Value, 0);
            Value |= BIT0;
            NbioRegisterWrite (GnbHandle, TYPE_SMN, PcieErrCntlAddr, &Value, GNB_REG_ACC_FLAG_S3SAVE);
          } else if (PortAerConfig->PortAerEnable == 1) {
            //
            // PCIe Correctable Error Mask
            //
            NbioRegisterRead ( GnbHandle, TYPE_SMN, CorrMaskAddr, &Value, 0);
            Value &= ~(BIT6 | BIT7 | BIT8 | BIT12 | BIT13);
            if (PortAerConfig->CorrectableMask.Field.BadTLPMask == 1) {
              Value |= BIT6;
            }
            if (PortAerConfig->CorrectableMask.Field.BadDLLPMask == 1) {
              Value |= BIT7;
            }
            if (PortAerConfig->CorrectableMask.Field.ReplayNumberRolloverMask == 1) {
              Value |= BIT8;
            }
            if (PortAerConfig->CorrectableMask.Field.ReplayTimerTimeoutMask == 1) {
              Value |= BIT12;
            }
            if (PortAerConfig->CorrectableMask.Field.AdvisoryNonFatalErrorMask == 1) {
              Value |= BIT13;
            }
			Value |= (BIT13);
            NbioRegisterWrite (GnbHandle, TYPE_SMN, CorrMaskAddr, &Value, GNB_REG_ACC_FLAG_S3SAVE);

            //
            // PCIe Unorrectable Error Mask
            //
            NbioRegisterRead ( GnbHandle, TYPE_SMN, UnCorrMaskAddr, &Value, 0);
            Value &= ~(BIT4 | BIT12 | BIT14 | BIT15 | BIT16 | BIT18 | BIT19 | BIT20 | BIT21);
            if (PortAerConfig->UncorrectableMask.Field.DataLinkProtocolErrorMask == 1) {
              Value |= BIT4;
            }
            if (PortAerConfig->UncorrectableMask.Field.PoisonedTLPMask == 1) {
              Value |= BIT12;
            }
            if (PortAerConfig->UncorrectableMask.Field.CompletionTimeoutMask == 1) {
              Value |= BIT14;
            }
            if (PortAerConfig->UncorrectableMask.Field.CompleterAbortMask == 1) {
              Value |= BIT15;
            }
            if (PortAerConfig->UncorrectableMask.Field.UnexpectedCompletionMask == 1) {
              Value |= BIT16;
            }
            if (PortAerConfig->UncorrectableMask.Field.MalTlpMask == 1) {
              Value |= BIT18;
            }
            if (PortAerConfig->UncorrectableMask.Field.ECRCErrorMask == 1) {
              Value |= BIT19;
            }
            if (PortAerConfig->UncorrectableMask.Field.UnsupportedRequestErrorMask == 1) {
              Value |= BIT20;
            }
            if (PortAerConfig->UncorrectableMask.Field.AcsViolationMask == 1) {
              Value |= BIT21;
            }
            NbioRegisterWrite (GnbHandle, TYPE_SMN, UnCorrMaskAddr, &Value, GNB_REG_ACC_FLAG_S3SAVE);

            //
            // PCIe Unorrectable Error Severity
            //
            NbioRegisterRead ( GnbHandle, TYPE_SMN, UnCorrSeverityAddr, &Value, 0);
            Value &= ~(BIT4 | BIT12 | BIT14 | BIT15 | BIT16 | BIT18 | BIT19 | BIT20 | BIT21);
            if (PortAerConfig->UncorrectableSeverity.Field.DataLinkProtocolErrorSeverity == 1) {
              Value |= BIT4;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.PoisonedTLPSeverity == 1) {
              Value |= BIT12;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.CompletionTimeoutSeverity == 1) {
              Value |= BIT14;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.CompleterAbortSeverity == 1) {
              Value |= BIT15;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.UnexpectedCompletionSeverity == 1) {
              Value |= BIT16;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.MalTlpSeverity == 1) {
              Value |= BIT18;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.ECRCErrorSeverity == 1) {
              Value |= BIT19;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.UnsupportedRequestErrorSeverity == 1) {
              Value |= BIT20;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.AcsViolationSeverity == 1) {
              Value |= BIT21;
            }
            NbioRegisterWrite (GnbHandle, TYPE_SMN, UnCorrSeverityAddr, &Value, GNB_REG_ACC_FLAG_S3SAVE);
          }
        }
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioPcieAerFeatureEnable Exit\n");
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  DumpPcieAerSetting
 *
 *  Description:
 *     dump Nbio PCIe each port AER feature.
 *
 *  Parameters:
 *    @param[in]     PortAerConfig
 *
 *    @retval        VOID
 *
 *---------------------------------------------------------------------------------------
 **/
VOID DumpPcieAerSetting (
  IN       NBIO_PCIe_PORT_AER_CONFIG  *PortAerConfig
  )
{
  IDS_HDT_CONSOLE (PCIE_MISC, "Dev%d Func%d AER - %a\n", PortAerConfig->PciDev, PortAerConfig->PciFunc, PortAerConfig->PortAerEnable == 0? "Disable":"Enable");
  IDS_HDT_CONSOLE (PCIE_MISC, "  BadTLPMask - %d\n", PortAerConfig->CorrectableMask.Field.BadTLPMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  BadDLLPMask - %d\n", PortAerConfig->CorrectableMask.Field.BadDLLPMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  ReplayNumberRolloverMask - %d\n", PortAerConfig->CorrectableMask.Field.ReplayNumberRolloverMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  ReplayTimerTimeoutMask - %d\n", PortAerConfig->CorrectableMask.Field.ReplayTimerTimeoutMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  AdvisoryNonFatalErrorMask - %d\n", PortAerConfig->CorrectableMask.Field.AdvisoryNonFatalErrorMask);

  IDS_HDT_CONSOLE (PCIE_MISC, "  DataLinkProtocolErrorMask - %d\n", PortAerConfig->UncorrectableMask.Field.DataLinkProtocolErrorMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  PoisonedTLPMask - %d\n", PortAerConfig->UncorrectableMask.Field.PoisonedTLPMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  CompletionTimeoutMask - %d\n", PortAerConfig->UncorrectableMask.Field.CompletionTimeoutMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  CompleterAbortMask - %d\n", PortAerConfig->UncorrectableMask.Field.CompleterAbortMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  UnexpectedCompletionMask - %d\n", PortAerConfig->UncorrectableMask.Field.UnexpectedCompletionMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  MalTlpMask - %d\n", PortAerConfig->UncorrectableMask.Field.MalTlpMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  ECRCErrorMask - %d\n", PortAerConfig->UncorrectableMask.Field.ECRCErrorMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  UnsupportedRequestErrorMask - %d\n", PortAerConfig->UncorrectableMask.Field.UnsupportedRequestErrorMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  AcsViolationMask - %d\n", PortAerConfig->UncorrectableMask.Field.AcsViolationMask);

  IDS_HDT_CONSOLE (PCIE_MISC, "  DataLinkProtocolErrorSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.DataLinkProtocolErrorSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  PoisonedTLPSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.PoisonedTLPSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  CompletionTimeoutSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.CompletionTimeoutSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  CompleterAbortSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.CompleterAbortSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  UnexpectedCompletionSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.UnexpectedCompletionSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  MalTlpSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.MalTlpSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  ECRCErrorSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.ECRCErrorSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  UnsupportedRequestErrorSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.UnsupportedRequestErrorSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  AcsViolationSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.AcsViolationSeverity);
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  NbioPcieAerInitRV
 *
 *  Description:
 *     Initializer Nbio PCIe each port AER feature.
 *
 *  Parameters:
 *    @param[in]     GnbHandle
 *
 *    @retval        VOID
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
NbioPcieAerInitRV (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  UINT32                   Index;

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioPcieAerInitRV Enter\n");
  NbioPcieAerConfig.NumberOfPorts = NUMBER_OF_PORTS;
  IDS_HDT_CONSOLE (GNB_TRACE, "Socket:%x, Die:%x, Instance:%x, Number of ports in NBIO: %d\n",  GnbHandle->SocketId, GnbHandle->DieNumber, GnbHandle->InstanceId, NbioPcieAerConfig.NumberOfPorts);
  IDS_HOOK (IDS_HOOK_NBIO_PCIE_AER_INIT, NULL, (VOID *)&NbioPcieAerConfig);

  for (Index = 0; Index < NbioPcieAerConfig.NumberOfPorts; Index++) {
    DumpPcieAerSetting ((NBIO_PCIe_PORT_AER_CONFIG *) &NbioPcieAerConfig.PortAerConfig[Index]);
    NbioPcieAerFeatureEnable (GnbHandle, (NBIO_PCIe_PORT_AER_CONFIG *) &NbioPcieAerConfig.PortAerConfig[Index]);
    // disable the reporting of PCIe correctible errors.
	//<Embedded_Override_Start>
    /*NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     PCIE_PORT0_1A8_ADDRESS | (Index << 12),
                     (UINT32) ~(BIT0),
                     0x1,
                     GNB_REG_ACC_FLAG_S3SAVE
                     );
    */
	//<Embedded_Override_End>
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioPcieAerInitRV Exit\n");
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  NbioSetPcieAerFeature
 *
 *  Description:
 *     Enable/Disable AER feature.
 *
 *  Parameters:
 *    @param[in]     GnbHandle
 *    @param[in]     PortAerConfig
 *
 *    @retval        EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS
EFIAPI
NbioSetPcieAerFeature (
  IN       AMD_NBIO_PCIE_AER_PROTOCOL  *This,
  IN       PCIe_PORT_AER_CONFIG        *PcieAerSetting
  )
{
  EFI_STATUS        Status;
  UINT16            PcieCapPtr;
  UINT32            Value;
  PCI_ADDR          Device;

  // IDS_HDT_CONSOLE (MAIN_FLOW, "NbioSetPcieAerFeature Entry\n");
  Status = EFI_UNSUPPORTED;
  Device.AddressValue = 0;

  if (PcieAerSetting->AerEnable == 1) {
    Device.Address.Bus = PcieAerSetting->PciBus;
    Device.Address.Device = PcieAerSetting->PciDev;
    Device.Address.Function = PcieAerSetting->PciFunc;

    PcieCapPtr = GnbLibFindPcieExtendedCapability (Device.AddressValue, PCIE_EXT_AER_CAP_ID, NULL);
    if (PcieCapPtr != 0) {
      // Uncorrectable Error Mask Register (Offset 08h)
      GnbLibPciRead (Device.AddressValue | (PcieCapPtr + 0x08), AccessWidth32, &Value, NULL);
      if (PcieAerSetting->UncorrectableMask.Field.DataLinkProtocolErrorMask == 1) {
        Value |= BIT4;
      }
      if (PcieAerSetting->UncorrectableMask.Field.PoisonedTLPMask == 1) {
        Value |= BIT12;
      }
      if (PcieAerSetting->UncorrectableMask.Field.CompletionTimeoutMask == 1) {
        Value |= BIT14;
      }
      if (PcieAerSetting->UncorrectableMask.Field.CompleterAbortMask == 1) {
        Value |= BIT15;
      }
      if (PcieAerSetting->UncorrectableMask.Field.UnexpectedCompletionMask == 1) {
        Value |= BIT16;
      }
      if (PcieAerSetting->UncorrectableMask.Field.MalTlpMask == 1) {
        Value |= BIT18;
      }
      if (PcieAerSetting->UncorrectableMask.Field.ECRCErrorMask == 1) {
        Value |= BIT19;
      }
      if (PcieAerSetting->UncorrectableMask.Field.UnsupportedRequestErrorMask == 1) {
        Value |= BIT20;
      }
      if (PcieAerSetting->UncorrectableMask.Field.AcsViolationMask == 1) {
        Value |= BIT21;
      }
      GnbLibPciWrite (Device.AddressValue | (PcieCapPtr + 0x08), AccessS3SaveWidth32, &Value, NULL);

      // Uncorrectable Error Severity Register (Offset 0Ch)
      GnbLibPciRead (Device.AddressValue | (PcieCapPtr + 0x0C), AccessWidth32, &Value, NULL);
      if (PcieAerSetting->UncorrectableSeverity.Field.DataLinkProtocolErrorSeverity == 1) {
        Value |= BIT4;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.PoisonedTLPSeverity == 1) {
        Value |= BIT12;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.CompletionTimeoutSeverity == 1) {
        Value |= BIT14;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.CompleterAbortSeverity == 1) {
        Value |= BIT15;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.UnexpectedCompletionSeverity == 1) {
        Value |= BIT16;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.MalTlpSeverity == 1) {
        Value |= BIT18;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.ECRCErrorSeverity == 1) {
        Value |= BIT19;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.UnsupportedRequestErrorSeverity == 1) {
        Value |= BIT20;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.AcsViolationSeverity == 1) {
        Value |= BIT21;
      }
      GnbLibPciWrite (Device.AddressValue | (PcieCapPtr + 0x0C), AccessS3SaveWidth32, &Value, NULL);

      // Correctable Error Mask Register (Offset 14h)
      GnbLibPciRead (Device.AddressValue | (PcieCapPtr + 0x14), AccessWidth32, &Value, NULL);
      if (PcieAerSetting->CorrectableMask.Field.BadTLPMask == 1) {
        Value |= BIT6;
      }
      if (PcieAerSetting->CorrectableMask.Field.BadDLLPMask == 1) {
        Value |= BIT7;
      }
      if (PcieAerSetting->CorrectableMask.Field.ReplayNumberRolloverMask == 1) {
        Value |= BIT8;
      }
      if (PcieAerSetting->CorrectableMask.Field.ReplayTimerTimeoutMask == 1) {
        Value |= BIT12;
      }
      if (PcieAerSetting->CorrectableMask.Field.AdvisoryNonFatalErrorMask == 1) {
        Value |= BIT13;
      }
      GnbLibPciWrite (Device.AddressValue | (PcieCapPtr + 0x14), AccessS3SaveWidth32, &Value, NULL);
      Status = EFI_SUCCESS;
    }
  }

  // IDS_HDT_CONSOLE (MAIN_FLOW, "NbioSetPcieAerFeature Exit(%x)\n", Status);
  return Status;
}


