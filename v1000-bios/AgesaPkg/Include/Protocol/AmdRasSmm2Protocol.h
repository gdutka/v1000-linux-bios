/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _AMD_RAS_SMM2_PROTOCOL_H_
#define _AMD_RAS_SMM2_PROTOCOL_H_

#include "AmdRas.h"

extern EFI_GUID gAmdRasSmm2ProtocolGuid;

// current PPI revision
#define AMD_RAS_SMM_REV  0x02

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
typedef struct _AMD_RAS_SMM2_PROTOCOL AMD_RAS_SMM2_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *MCA_ERROR_ADDR_TRANSLATE) (
  IN       NORMALIZED_ADDRESS *NormalizedAddress,
  OUT      UINT64             *SystemMemoryAddress,
  OUT      DIMM_INFO          *DimmInfo
);

typedef
EFI_STATUS
(EFIAPI *TRANSLATE_SYSADDR_TO_CS) (
  IN       UINT64             *SystemMemoryAddress,
  OUT      NORMALIZED_ADDRESS *NormalizedAddress,
  OUT      DIMM_INFO          *DimmInfo
);

typedef
EFI_STATUS
(EFIAPI *SET_SMI_TRIG_IOCYCLE) (
  IN       UINT64 SmiTrigIoCycleData
);


typedef
EFI_STATUS
(EFIAPI *GET_ALL_LOCAL_SMI_STATUS) (
  IN       LOCAL_SMI_STATUS* pLocalSmiStatusList
);

typedef
EFI_STATUS
(EFIAPI *SEARCH_MCA_ERROR) (
  IN OUT   RAS_MCA_ERROR_INFO_V2* RasMcaErrorInfo
);

typedef
EFI_STATUS
(EFIAPI *RAS_SMM_EXIT_TYPE) (
  IN       UINTN ProcessorNumber,
  IN       UINTN SmiExitType
);

typedef
EFI_STATUS
(EFIAPI *GET_SMM_SAVE_STATE_BASE) (
  IN       UINTN ProcessorNumber,
  OUT      UINT64* SmmSaveStateBase
);

typedef
EFI_STATUS
(EFIAPI *SET_MCA_CLOAK_CFG) (
   IN       UINTN ProcessorNumber,
   IN       UINT64 CloakValue,
   IN       UINT64 UnCloakValue
);

typedef
EFI_STATUS
(EFIAPI *CLR_MCA_STATUS) (
  IN       UINTN    ProcessorNumber,
  IN       UINTN    McaBankNumber,
  IN       BOOLEAN  IsWrMsr
);

typedef
EFI_STATUS
(EFIAPI *MAP_SYMBOL_TO_DRAM_DEVICE) (
  IN       AMD_RAS_SMM2_PROTOCOL *This,
  IN       RAS_MCA_ERROR_INFO_V2 *RasMcaErrorInfo,
  IN       NORMALIZED_ADDRESS    *NormalizedAddress,
  IN       UINT8                 BankIndex,
  OUT      UINT32                *DeviceStart,
  OUT      UINT32                *DeviceEnd,
  OUT      UINT8                 *DeviceType
);

typedef
EFI_STATUS
(EFIAPI *SET_MCA_THRESHOLD) (
  IN       UINTN                 *ProcessorNumber,
  IN       UINTN                 *McaBankNumber,
  IN       RAS_THRESHOLD_CONFIG  *RasThresholdConfig,
  IN       BOOLEAN               OvrflwChk
);

/// Protocol Structure
struct _AMD_RAS_SMM2_PROTOCOL {
  MCA_ERROR_ADDR_TRANSLATE         McaErrorAddrTranslate;    /// MCA_ADDR address Translate
  TRANSLATE_SYSADDR_TO_CS          TranslateSysAddrToCS;     /// System Address Translate
  SET_SMI_TRIG_IOCYCLE             SetSmiTrigIoCycle;        /// Set SmiTrigIoCycle
  GET_ALL_LOCAL_SMI_STATUS         GetAllLocalSmiStatus;     /// Get all Local Smi Status
  SEARCH_MCA_ERROR                 SearchMcaError;           /// Search Mca Error
  RAS_SMM_EXIT_TYPE                RasSmmExitType;           /// Set SMM exit interrupt type
  GET_SMM_SAVE_STATE_BASE          GetSmmSaveStateBase;      /// Get CPU SMM Save State Base Address
  SET_MCA_CLOAK_CFG                SetMcaCloakCfg;           ///
  CLR_MCA_STATUS                   ClrMcaStatus;             /// Clear MCA_STATUS MSR
  MAP_SYMBOL_TO_DRAM_DEVICE        MapSymbolToDramDevice;    /// Map the DRAM device to symbol
  SET_MCA_THRESHOLD                SetMcaThreshold;          /// Set MCA Thershold count
};

#endif //_AMD_RAS_SMM2_PROTOCOL_H_

