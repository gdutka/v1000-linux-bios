/* $NoKeywords:$ */
/**
 * @file
 *
 * Nbio PCIe AER Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Nbio
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _NBIO_PCIE_AER_PROTOCOL_H_
#define _NBIO_PCIE_AER_PROTOCOL_H_


// TODO : Potentially remove these defines if they are not used, and leave them to the architecture
// specific GNB PPR defines. Otherwise they are duplicates...
/// PCIe Correctable Error Mask
typedef union {
  struct {                                                            ///<
    UINT32                  BadTLPMask :1;                            ///<
    UINT32                  BadDLLPMask :1;                           ///<
    UINT32                  ReplayNumberRolloverMask :1;              ///<
    UINT32                  ReplayTimerTimeoutMask :1;                ///<
    UINT32                  AdvisoryNonFatalErrorMask :1;             ///<
  } Field;                                                            ///<
  UINT32 Value;                                                       ///<
} PCIe_AER_CORRECTABLE_MASK;

/// PCIe Unorrectable Error Mask
typedef union {
  struct {                                                            ///<
    UINT32                  DataLinkProtocolErrorMask :1;             ///<
    UINT32                  PoisonedTLPMask :1;                       ///<
    UINT32                  CompletionTimeoutMask :1;                 ///<
    UINT32                  CompleterAbortMask :1;                    ///<
    UINT32                  UnexpectedCompletionMask :1;              ///<
    UINT32                  MalTlpMask:1 ;                            ///<
    UINT32                  ECRCErrorMask :1;                         ///<
    UINT32                  UnsupportedRequestErrorMask :1;           ///<
    UINT32                  AcsViolationMask:1 ;                      ///<
  } Field;                                                            ///<
  UINT32 Value;                                                       ///<
} PCIe_AER_UNCORRECTABLE_MASK;

/// PCIe Unorrectable Error Severity
typedef union {
  struct {                                                            ///<
    UINT32                  DataLinkProtocolErrorSeverity :1;         ///<
    UINT32                  PoisonedTLPSeverity :1;                   ///<
    UINT32                  CompletionTimeoutSeverity :1;             ///<
    UINT32                  CompleterAbortSeverity :1;                ///<
    UINT32                  UnexpectedCompletionSeverity :1;          ///<
    UINT32                  MalTlpSeverity:1 ;                        ///<
    UINT32                  ECRCErrorSeverity :1;                     ///<
    UINT32                  UnsupportedRequestErrorSeverity :1;       ///<
    UINT32                  AcsViolationSeverity:1 ;                  ///<
  } Field;                                                            ///<
  UINT32 Value;                                                       ///<
} PCIe_AER_UNCORRECTABLE_SEVERITY;

/// PCIe AER Port Configuration
typedef struct {
  UINT8                           AerEnable;                          ///< General per-port enable, 0=disable 1=enable
  UINT8                           PciBus;                             ///< PCI Bus Number
  UINT8                           PciDev;                             ///< PCI Device Number
  UINT8                           PciFunc;                            ///< PCI Function Number
  PCIe_AER_CORRECTABLE_MASK       CorrectableMask;                    ///< Per-port mask for correctable errors
  PCIe_AER_UNCORRECTABLE_MASK     UncorrectableMask;                  ///< Per-port mask for uncorrectable errors
  PCIe_AER_UNCORRECTABLE_SEVERITY UncorrectableSeverity;              ///< Per-port severity configuration for uncorrectable errors
} PCIe_PORT_AER_CONFIG;

///
/// Forward declaration for the AMD_NBIO_PCIE_AER_PROTOCOL
///
typedef struct _AMD_NBIO_PCIE_AER_PROTOCOL AMD_NBIO_PCIE_AER_PROTOCOL;


//
// Protocol Definitions
//
/**
  Enable or disable Advanced Error Reporting feature
  This
    A pointer to the AMD_NBIO_PCIE_AER_PROTOCOL instance.
  PcieAerSetting
    A pointer to store the address of the PCIe port AER config structure
**/
typedef
EFI_STATUS
(EFIAPI * AMD_NBIO_PCIE_AER_FEATURE) (
  IN  AMD_NBIO_PCIE_AER_PROTOCOL  *This,
  IN  PCIe_PORT_AER_CONFIG        *PcieAerSetting
);

///
/// The Dxe of PCIE AER Services
///
struct _AMD_NBIO_PCIE_AER_PROTOCOL {
  AMD_NBIO_PCIE_AER_FEATURE     SetPcieAerFeature; ///<
};

extern EFI_GUID gAmdNbioPcieAerProtocolGuid;

#endif

