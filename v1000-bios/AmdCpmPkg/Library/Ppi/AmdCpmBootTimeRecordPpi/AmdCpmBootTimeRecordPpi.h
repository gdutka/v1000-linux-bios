/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * ***************************************************************************
 */

#ifndef _AMD_CPM_BOOT_TIME_RECORD_PPI_H_
#define _AMD_CPM_BOOT_TIME_RECORD_PPI_H_
#include <AmdCpmBase.h>

#define AMD_BOOT_TIME_RECORD_PPI_GUID \
{ \
  0x90599780, 0x50b0, 0x43d1, 0x9f, 0x22, 0x43, 0xd1, 0xeb, 0xc, 0x0, 0x3f \
}

#define AMD_S3_RESUME_BEFORE_WAK_PPI_GUID \
{ \
  0x90599782, 0x50c0, 0x63d1, 0x9f, 0x22, 0x48, 0xd6, 0xeb, 0xc, 0x0, 0x3f \
}

#pragma pack (push, 1)
///Boot Time Data
typedef struct _AMD_BOOT_TIME_DATA {
  #ifdef TSC_TIMER_TICK_ENABLE
    UINT64    TscTicks;       ///< Cpu Tsc Ticks
  #endif
  #ifdef ACPI_TIMER_TICK_ENABLE
    UINT32    AcpiTimerTicks; ///< ACPI Timer Ticks
  #endif
  UINT32    Version;          ///< Boot Time Record Version
  UINT64    Description;      ///< Test Points description
}  AMD_BOOT_TIME_DATA;

///Boot Time Record data holder
typedef struct _AMD_BOOT_TIME_RECORD {
  UINT16               IndexToWrite;          ///< Last Index + 1;
  AMD_BOOT_TIME_DATA   data[MAXIMUM_RECORDS]; ///< Boot Time Data Holder
} AMD_BOOT_TIME_RECORD;

#ifdef IDS_PERFORMANCE_ENABLE
  #define MAX_PERFORMANCE_UNIT_NUM MAX_IDS_PERFORMANCE_UNIT_NUM
  /// Data Structure of Parameters for TestPoint_TSC.
  typedef struct {
    UINT32 LineInFile;                    ///< Line of current time counter
    UINT64 Description;                   ///< Description ID
    UINT64 StartTsc;                      ///< The StartTimer of TestPoint_TSC
    #ifdef ACPI_TIMER_TICK_ENABLE
      UINT32 StartAcpiTimer;                ///< Acpi Timer
    #endif
  } TestPoint_TSC;

  /// Data Structure of Parameters for TP_Perf_STRUCT.
  typedef struct {
    UINT32 Signature;                                 ///< "TIME"
    UINT32 Index;                                     ///< The Index of TP_Perf_STRUCT
    UINT32 TscInMhz;                                  ///< Tsc counter in 1 mhz
    UINT32 Version;                                   ///< Version number
    #ifdef ACPI_TIMER_TICK_ENABLE
      UINT32 AcpiTimer;                                 ///< Acpi Timer
    #endif
    TestPoint_TSC TP[MAX_PERFORMANCE_UNIT_NUM];       ///< The TP of TP_Perf_STRUCT
  } TP_Perf_STRUCT;
#endif

#pragma pack (pop)
typedef
EFI_STATUS
(EFIAPI *AMD_BOOT_TIME_RECORD_PPI_ADD) (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN AMD_BOOT_TIME_RECORD      *Record,
  IN UINT64                     desc
  );

#ifdef  IDS_PERFORMANCE_ENABLE
  typedef
  EFI_STATUS
  (EFIAPI *AMD_BOOT_TIME_RECORD_PPI_IDS) (
    IN EFI_PEI_SERVICES          **PeiServices,
    IN TP_Perf_STRUCT            *Record,
    IN  OUT VOID *IdsCalloutData
    );
#endif

///Boot Time Record PPI
typedef struct _AMD_BOOT_TIME_RECORD_PPI {
  AMD_BOOT_TIME_RECORD            *Record;                            ///<Boot time record for bios post
  AMD_BOOT_TIME_RECORD            *S3Record;                          ///<Boot time record for Standby resume
  #ifdef IDS_PERFORMANCE_ENABLE
    TP_Perf_STRUCT                  *IdsS3Record;                       ///<Boot time record for AGESA
    AMD_BOOT_TIME_RECORD_PPI_IDS    CopyIdsPerformanceDataToIdsRecord;  ///<Function for transfer AGESA IDS points to Boot time record
  #endif
  AMD_BOOT_TIME_RECORD_PPI_ADD    AddRecord;                          ///<Function to add record
} AMD_BOOT_TIME_RECORD_PPI;

extern EFI_GUID gBootTimeRecordPpiGuid;
extern EFI_GUID gPeiLastS3RestorePointPpiGuid;

extern EFI_STATUS RecordTimeTicks (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN UINT64                    desc
);

extern EFI_STATUS RecordTimeTicksS3Resume (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN UINT64                     desc
);


#ifdef IDS_PERFORMANCE_ENABLE
  extern EFI_STATUS RecordTimeTicksAGESAS3Resume (
    IN  EFI_PEI_SERVICES          **PeiServices,
    IN  OUT VOID                  *IdsCalloutData
  );
#endif

#ifdef AMD_BIOS_BOOT_TIME_RECORD
  #define RECORD_TIME_PEI(x) RecordTimeTicks((EFI_PEI_SERVICES**)PeiServices, x);
  #define RECORD_TIME_S3RESUME(x) RecordTimeTicksS3Resume((EFI_PEI_SERVICES**)PeiServices, x);
  #ifdef IDS_PERFORMANCE_ENABLE
    #define RECORD_TIME_AGESA_S3RESUME(x) RecordTimeTicksAGESAS3Resume((EFI_PEI_SERVICES**)PeiServices, (VOID *)x);
  #else
    #define RECORD_TIME_AGESA_S3RESUME(x)
  #endif
#else
  #define RECORD_TIME_PEI(x)
  #define RECORD_TIME_S3RESUME(x)
  #define RECORD_TIME_AGESA_S3RESUME(x)
#endif


#endif
