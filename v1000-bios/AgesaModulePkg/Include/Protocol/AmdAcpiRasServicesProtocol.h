/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD RAS Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_ACPI_RAS_SERVICES_PROTOCOL_H_
#define _AMD_ACPI_RAS_SERVICES_PROTOCOL_H_
#pragma pack (push, 1)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *                            System Resource Affinity Table
 *----------------------------------------------------------------------------------------
 */
/// Initial data of AMD_MC_BANK
typedef struct _AMD_MC_BANK_INIT_DATA {
  UINT64 CtrlInitData;            ///< Initial data of CtrlInitDataLSD
  UINT32 CtrlRegMSRAddr;          ///< Initial data of CtrlRegMSRAddr
  UINT32 StatRegMSRAddr;          ///< Initial data of StatRegMSRAddr
  UINT32 AddrRegMSRAddr;          ///< Initial data of AddrRegMSRAddr
  UINT32 MiscRegMSRAddr;          ///< Initial data of MiscRegMSRAddr
} AMD_MC_BANK_INIT_DATA;

/// Initial data of Machine Check
typedef struct _AMD_MC_INIT_DATA {
  UINT64 GlobCapInitData;         ///< Check global capability register
  UINT64 GlobCtrlInitData;        ///< Check global control register
  UINT8  ClrStatusOnInit;         ///< Indicates if the status information in this machine check
                                  ///< bank is to be cleared during system initialization
  UINT8  StatusDataFormat;        ///< Indicates the format of the data in the status register
  UINT8  ConfWriteEn;             ///< This field indicates whether configuration parameters may be
                                  ///< modified by the OS. If the bit for the associated parameter is
                                  ///< set, the parameter is writable by the OS.
  UINT8  McBankNum;               ///< Number of MCA Bank
  AMD_MC_BANK_INIT_DATA *McBankInitData;  ///< Pointer to Initial data of Mc Bank
} AMD_MC_INIT_DATA;

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */
///
/// Forward declaration for the AMD_FABRIC_ACPI_SRAT_SERVICES_PROTOCOL.
///
typedef struct _AMD_CCX_ACPI_RAS_SERVICES_PROTOCOL AMD_CCX_ACPI_RAS_SERVICES_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AMD_RAS_SERVICES_GET_MC_BANK_INFO) (
  IN       AMD_CCX_ACPI_RAS_SERVICES_PROTOCOL         *This,
     OUT   AMD_MC_INIT_DATA                          **McBankInfo
  );

///
/// When installed, the SRAT Services Protocol produces a collection of
/// services that return various information to generate SRAT
///
struct _AMD_CCX_ACPI_RAS_SERVICES_PROTOCOL {
  UINTN                                Revision;       ///< Revision Number
  AMD_RAS_SERVICES_GET_MC_BANK_INFO    GetMcBankInfo;  ///< Get Machine Check bank infomation
};

extern EFI_GUID gAmdCcxAcpiRasServicesProtocolGuid;

#pragma pack (pop)
#endif // _AMD_ACPI_RAS_SERVICES_PROTOCOL_H_
