/** @file
  Protocol produced by platform memory test driver.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __PLATFORM_MEMTEST_H__
#define __PLATFORM_MEMTEST_H__

#include <Protocol/HiiDatabase.h>
#include <Protocol/GenericMemoryTest.h>

#define EFI_PLATFORM_MEMTEST_PROTOCOL_GUID \
  { 0x859ba18, 0x7dd7, 0x4ed7, 0xa8, 0x8e, 0x10, 0x9c, 0x63, 0x91, 0x7b, 0xdd }

#define EFI_COMPATIBLE_MEMORY_TESTED_PROTOCOL_GUID \
  { 0x64c475ef, 0x344b, 0x492c, 0x93, 0xad, 0xab, 0x9e, 0xb4, 0x39, 0x50, 0x4 }

//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_PLATFORM_MEMTEST_PROTOCOL EFI_PLATFORM_MEMTEST_PROTOCOL;

//
// Basic type definitions
//
typedef UINT16  EFI_DIMM_ID;
#define EFI_INVALID_DIMM_ID 0xFFFF

//
// This describes the capabilities
//
typedef struct {
  UINT32                HwCap;
  EFI_PHYSICAL_ADDRESS  InitBlockSize;
  EFI_PHYSICAL_ADDRESS  TestBlockSize;
} EFI_PLATFORM_MEMTEST_CAPABILITIES;

//
// This defines the platform driver phases to use in notify phase
//
typedef enum {
  EfiMemtestPhaseBegin,
  EfiMemtestPhaseEnd,
  EfiMemtestPhaseMaximum
} EFI_MEMTEST_PHASE;


//
// Definitions of Capability bits
//
#define EFI_MEMTEST_CAP_NONE           0X00000000
#define EFI_MEMTEST_CAP_ECC            0X00000001
#define EFI_MEMTEST_CAP_HW_TEST        0X00000002
#define EFI_MEMTEST_CAP_HW_INIT        0X00000004

//
// This describes the pattern for testing all memory cells
//
typedef struct {
  UINT32         Count;      // Number of patterns
  UINT32         Width;      // Width of patterns
  VOID          *Pattern;    // Array of patterns
}EFI_MEMTEST_DATA_PATTERN;

//
// This describes the data and address line patterns
//
typedef struct {
  UINT32                 Count;    // Number of patterns
  EFI_PHYSICAL_ADDRESS  *Address;  // Array of addresses
  EFI_PHYSICAL_ADDRESS  *Pattern;  // Array of patterns
} EFI_MEMTEST_ADDRESS_PATTERN;


#define   EFI_INVALID_DIMM_ID   0xFFFF

//
// DIMM information exported by the platform driver
//

typedef struct {
  EFI_DIMM_ID          DimmId;
  EFI_STRING_ID        DimmReference;
} EFI_MEMTEST_DIMM_INFO;


//
// Various types of errors
//
typedef enum {
  EfiMemtestCorrectableError = 1,
  EfiMemtestUncorrectableError,
  EfiMemtestErrorMaximum,
} EFI_DIMM_ERROR;

//
// Actions to be taken.
//
typedef enum {
  EfiMemtestActionReport = 1,
  EfiMemtestActionDisableMem,
  EfiMemtestActionMaximum,
} EFI_MEMTEST_ERROR_ACTION;


//
// Structure indicating a policy entry.
//

typedef struct {
  EFI_DIMM_ERROR            ErrorType;
  UINT32                    Threshold;
  EFI_MEMTEST_ERROR_ACTION  Action;
} EFI_MEMTEST_POLICY_ENTRY;



//
// Error Information.
//
typedef struct {
  EFI_DIMM_ERROR  Error;
  EFI_DIMM_ID     DimmId;
} EFI_PLATFORM_MEMTEST_ERROR;


typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_MEMTEST_NOTIFY_PHASE) (
  IN EFI_PLATFORM_MEMTEST_PROTOCOL            *This,
  IN EFI_MEMTEST_PHASE                        Phase
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_MEMTEST_GET_PLATFORM_INFO) (
  IN EFI_PLATFORM_MEMTEST_PROTOCOL            *This,
  OUT  EFI_PLATFORM_MEMTEST_CAPABILITIES      *Capabilities,
  OUT  EFI_MEMTEST_ADDRESS_PATTERN            *AddressPattern,
  OUT  EFI_MEMTEST_DATA_PATTERN               *DataPattern,
  OUT  EFI_MEMTEST_DATA_PATTERN               *MemoryPattern,
  OUT  UINT32                                 *EntryCount,
  OUT  EFI_MEMTEST_POLICY_ENTRY               **PolicyGrid
  );

typedef
EFI_STATUS
(EFIAPI * EFI_PLATFORM_MEMTEST_GET_NEXT_DIMM_INFO) (
  IN EFI_PLATFORM_MEMTEST_PROTOCOL                *This,
  IN OUT EFI_MEMTEST_DIMM_INFO                    *DimmInfo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_MEMTEST_INITIALIZE_MEMORY) (
  IN EFI_PLATFORM_MEMTEST_PROTOCOL            *This,
  IN EFI_PHYSICAL_ADDRESS                     Start,
  IN EFI_PHYSICAL_ADDRESS                     Size
  );

typedef
EFI_STATUS
(EFIAPI * EFI_PLATFORM_MEMTEST_HW_MEMORY_TEST) (
  IN EFI_PLATFORM_MEMTEST_PROTOCOL          *This,
  IN EXTENDMEM_COVERAGE_LEVEL               Level,
  IN EFI_PHYSICAL_ADDRESS                   Start,
  IN EFI_PHYSICAL_ADDRESS                   Size,
  OUT UINTN                                 *ErrorCount,
  OUT EFI_PLATFORM_MEMTEST_ERROR            **ErrorInfo
  );

typedef
EFI_STATUS
(EFIAPI * EFI_PLATFORM_MEMTEST_FLUSH_BUFFER) (
  IN EFI_PLATFORM_MEMTEST_PROTOCOL            *This
  );

typedef
EFI_STATUS
(EFIAPI * EFI_PLATFORM_MEMTEST_CHECK_FOR_ERROR)  (
  IN EFI_PLATFORM_MEMTEST_PROTOCOL           *This,
  OUT  UINT32                                *ErrorCount,
  OUT EFI_PLATFORM_MEMTEST_ERROR             **ErrorInfo

  );

typedef
EFI_STATUS
(EFIAPI * EFI_PLATFORM_MEMTEST_LOCATE_BAD_DIMM) (
  IN EFI_PLATFORM_MEMTEST_PROTOCOL          *This,
  IN  EFI_PHYSICAL_ADDRESS                  FailingAddress,
  IN  UINT8                                 Expected,
  IN  UINT8                                 Found,
  OUT  UINT32                               *ErrorInfoCount,
  OUT EFI_PLATFORM_MEMTEST_ERROR            **ErrorInfoBuffer
  );

typedef
EFI_STATUS
(EFIAPI * EFI_PLATFORM_MEMTEST_CLEAR_ERROR) (
  IN EFI_PLATFORM_MEMTEST_PROTOCOL          *This
  );

typedef
EFI_STATUS
(EFIAPI * EFI_PLATFORM_MEMTEST_DISABLE_DIMM) (
  IN EFI_PLATFORM_MEMTEST_PROTOCOL          *This,
  IN UINT16                                 *DimmCount,
  IN EFI_DIMM_ID                            *DimmIds,
  IN BOOLEAN                                 RebootRequired
  );

typedef
EFI_STATUS
(EFIAPI * EFI_PLATFORM_MEMTEST_OVERLAP) (
  IN EFI_PLATFORM_MEMTEST_PROTOCOL          *This,
  IN  EFI_DIMM_ID                           DimmId,
  IN  EFI_PHYSICAL_ADDRESS                  start,
  IN  EFI_PHYSICAL_ADDRESS                  Size,
  OUT BOOLEAN                               *Overlap
  );

struct _EFI_PLATFORM_MEMTEST_PROTOCOL {
  EFI_PLATFORM_MEMTEST_NOTIFY_PHASE       NotifyPhase;
  EFI_PLATFORM_MEMTEST_GET_PLATFORM_INFO  GetPlatformInfo;
  EFI_PLATFORM_MEMTEST_GET_NEXT_DIMM_INFO GetNextDimmInfo;
  EFI_PLATFORM_MEMTEST_INITIALIZE_MEMORY  InitializeMem;
  EFI_PLATFORM_MEMTEST_HW_MEMORY_TEST     HwTest;
  EFI_PLATFORM_MEMTEST_FLUSH_BUFFER       FlushBuffer;
  EFI_PLATFORM_MEMTEST_CHECK_FOR_ERROR    CheckError;
  EFI_PLATFORM_MEMTEST_LOCATE_BAD_DIMM    LocateBadDimm;
  EFI_PLATFORM_MEMTEST_CLEAR_ERROR        ClearError;
  EFI_PLATFORM_MEMTEST_DISABLE_DIMM       DisableDimm;
  EFI_PLATFORM_MEMTEST_OVERLAP            Overlap;
};



extern EFI_GUID gEfiPlatformMemTestGuid;
extern EFI_GUID gEfiCompatibleMemoryTestedGuid;
#endif
