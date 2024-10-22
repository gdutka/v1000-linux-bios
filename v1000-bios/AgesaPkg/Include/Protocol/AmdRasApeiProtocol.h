/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _AMD_RAS_APEI_PROTOCOL_H_
#define _AMD_RAS_APEI_PROTOCOL_H_

#include "AmdRas.h"

extern EFI_GUID gAmdRasApeiProtocolGuid;

// current PPI revision
#define AMD_RAS_APEI_REV  0x01

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
typedef struct _AMD_RAS_APEI_PROTOCOL AMD_RAS_APEI_PROTOCOL;

// Protocol prototype
typedef
EFI_STATUS
(EFIAPI *AMD_ADD_BOOT_ERROR_RECORD_ENTRY) (
  IN    UINT8   *ErrorRecord,
  IN    UINT32  RecordLen,
  IN    UINT8   ErrorType,
  IN    UINT8   SeverityType
);

typedef
EFI_STATUS
(EFIAPI *ADD_HEST_ERROR_SOURCE_ENTRY) (
  IN    UINT8   *pErrorRecord,
  IN    UINT32  RecordLen
);

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
(EFIAPI *SEARCH_MCA_ERROR) (
  IN OUT   RAS_MCA_ERROR_INFO* RasMcaErrorInfo
);


/// RAS APEI Protocol Structure
struct _AMD_RAS_APEI_PROTOCOL {
  MCA_ERROR_ADDR_TRANSLATE         McaErrorAddrTranslate;    /// MCA_ADDR address Translate
  TRANSLATE_SYSADDR_TO_CS          TranslateSysAddrToCS;     /// System Address Translate
  AMD_ADD_BOOT_ERROR_RECORD_ENTRY  AddBootErrorRecordEntry;  /// Boot error record to be added
  ADD_HEST_ERROR_SOURCE_ENTRY      AddHestErrorSourceEntry;  /// HEST error source to be added
  SEARCH_MCA_ERROR                 SearchMcaError;           /// Search Mca Error
};

#endif //_AMD_RAS_APEI_PROTOCOL_H_

