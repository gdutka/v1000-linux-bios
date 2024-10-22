/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_LAKEPORT_MEMORY_CONTROLLER_H_
#define _PEI_LAKEPORT_MEMORY_CONTROLLER_H_

#define PEI_DUAL_CHANNEL_DDR_MEMORY_CONTROLLER_PPI_GUID \
  {0xDC2E5742, 0x9D14, 0x4195, 0x9E, 0xD5, 0x22, 0x63, 0xF7, 0x30, 0x8C, 0xF3}

typedef struct _GENERIC_MEMORY_CONTROLLER_PPI GENERIC_MEMORY_CONTROLLER_PPI;

//
// Maximum number of SDRAM channels supported by the memory controller
//
#define MAX_CHANNELS 2

//
// Maximum number of DIMM sockets supported by the memory controller
//
#ifdef  MAX_DIMM_SOCKET
#define MAX_SOCKETS MAX_DIMM_SOCKET
#else
#define MAX_SOCKETS 4
#endif
//
// Maximum number of sides supported per DIMM
//
#define   MAX_SIDES                         2

//
// Maximum number of "Socket Sets", where a "Socket Set is a set of matching
// DIMM's from the various channels
//
#define   MAX_SOCKET_SETS                   2

//
// Maximum number of rows supported by the memory controller
//
#define MAX_ROWS (MAX_SIDES * MAX_SOCKETS)

//BUGBUG
// Maximum number of memory ranges supported by the memory controller
//
#define MAX_RANGES (MAX_ROWS + 10)

//
// Maximum Number of Log entries
//
#define   PEI_MEMORY_LOG_MAX_INDEX          16


typedef struct _PEI_MEMORY_LOG_ENTRY {
  EFI_STATUS_CODE_VALUE                     Event;
  EFI_STATUS_CODE_TYPE                      Severity;
  UINT8                                     Data;
} PEI_MEMORY_LOG_ENTRY;

typedef struct _PEI_MEMORY_LOG {
  UINT8                                     Index;
  PEI_MEMORY_LOG_ENTRY                      Entry[PEI_MEMORY_LOG_MAX_INDEX];
} PEI_MEMORY_LOG;


#define SIZE_OF_SPD_DATA_BUFFER             65 // 44
//
// SPD data
//   SpdPresent indicates that the SPD data in Buffer is valid
//   Disabled indicates that the DIMM should not be used.
//
typedef struct _MEMINIT_SPD_DATA {
  BOOLEAN                                   SpdPresent;
  BOOLEAN                                   DimmDisabled;
  UINT8                                     Buffer[SIZE_OF_SPD_DATA_BUFFER];
} MEMINIT_SPD_DATA;

//
// DIMM data structure
//
typedef struct {
  UINT8   Present;      // DIMM is present
  UINT8   Sides;      // Number of sides on DIMM (SS/DS (0/1))
  UINT8   DataWidth;  // x8/x16 Data width (0/1)
  UINT8   Banks;      // Number of banks 4/8 (0/1). 8 banks is for DDR2 only.
  UINT8   DimmTechnology; // 0/1/2/3/4 128MBit/256MBit/512MBit/1GBit Technology
  
} PEI_DDR_DIMM_DATA;


//
// Timing data structure
//
typedef struct {
  UINT16  Frequency;     // Memory frequency for below timings 
                        //    0 - DDR/DDR2 333MHz
                        //    1 - DDR/DDR2 400MHz
                        //    2 - DDR/DDR2 533MHz
                        //    3 - DDR/DDR2 667MHz
                        //    4 - DDR/DDR2 800MHz
  UINT16 FrequencyMHz;  // Memory frequency in MHz  
  UINT8  FsbFrequency;  //  0/1/2/3/4 - 1067/533/800/667/1333MHz
  UINT8  RefreshRate;   // Fastest refresh rate 15.6uSec/7.8uSec (0/1)
  UINT8  Ecc;           // ECC supported by all memory sticks
  UINT8  Type;          // Memory type DDR/DDR2 (0/1)
  UINT8  Tcl;           // Cas Latency(s) DDR 3/2.5/2  DDR2 5/4/3  (0/1/2)
  UINT8  Trcd;          // Row to Col Delay 2/3/4/5 (0/1/2/3)
  UINT8  Trp;           // Ras Precharge 2/3/4/5 (0/1/2/3)
  UINT8  Tras;          // Ras Active Time 4/5/6/.../14/15
  UINT8  Twr;           // DRAM clocks 2/3/4/5 (0/1/2/3)
  UINT8  Trfc;          // DRAM clocks 0/1/2...1F (0/1/2/.../1F)
  UINT8  BurstLength;   // Burst Length
  UINT8  Interleaved;   // Non-Interleaved/Interleaved (0/1)
  UINT8  DimmConfigChannel[MAX_CHANNELS]; // DIMM config num for the two channels
  PEI_DDR_DIMM_DATA   DimmData[MAX_SOCKETS]; // Information about each individual DIMM MAX 4 DIMMS 

} PEI_DUAL_CHANNEL_DDR_TIMING_DATA;

//
// Row characteristics data structure
//
typedef struct {
  UINT8 RowAddressLines;       // Number of row address lines
  UINT8 ColumnAddressLines;    // Number of column address lines
  UINT8 BankAddressLines;      // Number of bank address lines
  UINT8 DataWidth;             // Data width of SDRAM devices, x8/x16 data width (Not Installed(0)/8/16)
  UINT8 EccDataWidth;          // Data width of ECC devices (0 if not present)
} PEI_DUAL_CHANNEL_DDR_ROW_DATA;

//
// Defined ECC types
//
#define PEI_DUAL_CHANNEL_DDR_ECC_TYPE_NONE             0x01   // No error checking
#define PEI_DUAL_CHANNEL_DDR_ECC_TYPE_EC               0x02   // Error checking only
#define PEI_DUAL_CHANNEL_DDR_ECC_TYPE_SECC             0x04   // Software Scrubbing ECC
#define PEI_DUAL_CHANNEL_DDR_ECC_TYPE_HECC             0x08   // Hardware Scrubbing ECC
#define PEI_DUAL_CHANNEL_DDR_ECC_TYPE_CKECC            0x10   // Chip Kill ECC

//
// Row configuration status values
//
#define PEI_DUAL_CHANNEL_DDR_ROW_CONFIG_SUCCESS        0x00  // No error
#define PEI_DUAL_CHANNEL_DDR_ROW_CONFIG_UNKNOWN        0x01  // Pattern mismatch, no memory
#define PEI_DUAL_CHANNEL_DDR_ROW_CONFIG_UNSUPPORTED    0x02  // Memory type not supported
#define PEI_DUAL_CHANNEL_DDR_ROW_CONFIG_ADDRESS_ERROR  0x03  // Row/Col/Bnk mismatch
#define PEI_DUAL_CHANNEL_DDR_ROW_CONFIG_ECC_ERROR      0x04  // Received ECC error
#define PEI_DUAL_CHANNEL_DDR_ROW_CONFIG_NOT_PRESENT    0x05  // Row is not present
#define PEI_DUAL_CHANNEL_DDR_ROW_CONFIG_DISABLED       0x06  // Row is disabled

//
// Row configuration data structure
//
typedef struct {
  EFI_PHYSICAL_ADDRESS    BaseAddress;
  UINT64                  RowLength;    // Size of Row in bytes  
  PEI_DUAL_CHANNEL_DDR_ROW_DATA    RowData;
} PEI_DUAL_CHANNEL_DDR_ROW_CONFIG;

//
// Burst Length definitions
//
#define PEI_DUAL_CHANNEL_DDR_ROW_DATA_BL_01            0x01
#define PEI_DUAL_CHANNEL_DDR_ROW_DATA_BL_02            0x02
#define PEI_DUAL_CHANNEL_DDR_ROW_DATA_BL_04            0x04
#define PEI_DUAL_CHANNEL_DDR_ROW_DATA_BL_08            0x08

//
// Information about supported memory
//
typedef struct {
  UINT64                  MaxMemory;    // Maximum amount of memory supported
  UINT16                  ECCSupport;   // This memory controller supports ECC
  UINT8                   MemoryType;   // Support for SDR/DDR/Buffered/Registered
  UINT8                   MaxRows;      // Maximum number of memory rows 
                                        //   (MaxSlots * MaxSides)
                                        //
  UINT8                   MaxRanges;    // Maximum number of memory ranges
                                        //   that can exist in the memory map 
  UINT8                   MaxChannels;  // Maximum number of channels possible
  UINT8                   BurstLength;  // Burst lengths supported
} PEI_DUAL_CHANNEL_DDR_ROW_INFO;

//
// Memory range types
//
typedef enum {
  DualChannelDdrMainMemory,
  DualChannelDdrSmramCacheable,
  DualChannelDdrSmramNonCacheable,
  DualChannelDdrGraphicsMemoryCacheable,
  DualChannelDdrGraphicsMemoryNonCacheable,
  DualChannelDdrReservedMemory,
  DualChannelDdrMaxMemoryRangeType
} PEI_DUAL_CHANNEL_DDR_MEMORY_RANGE_TYPE;

//
// Memory map range information
//
typedef struct {
  UINT8                                         RowNumber;
  EFI_PHYSICAL_ADDRESS                          PhysicalAddress;
  EFI_PHYSICAL_ADDRESS                          CpuAddress;
  EFI_PHYSICAL_ADDRESS                          RangeLength;
  PEI_DUAL_CHANNEL_DDR_MEMORY_RANGE_TYPE                 Type;
} PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE;

typedef struct _EFI_MEMINIT_CONFIG_DATA {
  PEI_DUAL_CHANNEL_DDR_ROW_INFO             RowInfo;
  PEI_DUAL_CHANNEL_DDR_TIMING_DATA          TimingData;
  PEI_DUAL_CHANNEL_DDR_ROW_CONFIG           RowConfArray[MAX_ROWS];
  MEMINIT_SPD_DATA                          SpdData[MAX_SOCKETS];
} EFI_MEMINIT_CONFIG_DATA;

//
// PPI function declarations
//

typedef
EFI_STATUS
(EFIAPI *PEI_MEMORY_CONTROLLER_RESET) (
  IN EFI_PEI_SERVICES                           **PeiServices,
  IN struct _GENERIC_MEMORY_CONTROLLER_PPI  *This
  );


typedef
EFI_STATUS
(EFIAPI *PEI_GET_DUAL_CHANNEL_DDR_SAFE_MODE_TIMINGS) (
  IN EFI_PEI_SERVICES                           **PeiServices,
  IN struct _GENERIC_MEMORY_CONTROLLER_PPI  *This,
  IN OUT PEI_DUAL_CHANNEL_DDR_TIMING_DATA                **DdrTimingData
  );

typedef
EFI_STATUS
(EFIAPI *PEI_GET_ROW_INFO) (
  IN EFI_PEI_SERVICES                           **PeiServices,
  IN struct _GENERIC_MEMORY_CONTROLLER_PPI  *This,
  OUT PEI_DUAL_CHANNEL_DDR_ROW_INFO                      **RowInfo
  );

typedef
EFI_STATUS
(EFIAPI *PEI_GET_MEMORY_MAP) (
  IN EFI_PEI_SERVICES                           **PeiServices,
  IN struct _GENERIC_MEMORY_CONTROLLER_PPI  *This,
  IN     PEI_DUAL_CHANNEL_DDR_TIMING_DATA                *TimingData,
  IN OUT  PEI_DUAL_CHANNEL_DDR_ROW_CONFIG                *RowConfArray,
  IN OUT PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE           *MemoryMap,
  IN OUT UINT8                                  *NumRanges
  );

typedef
EFI_STATUS
(EFIAPI *PEI_DETECT_MEMORY) (
  IN      EFI_PEI_SERVICES                      **PeiServices,
  IN struct _GENERIC_MEMORY_CONTROLLER_PPI  *This,
  IN      UINT8                                 DimmSockets,
  IN      MEMINIT_SPD_DATA                      *SpdData,
  IN OUT  PEI_DUAL_CHANNEL_DDR_TIMING_DATA               *TimingData,
  IN OUT  PEI_DUAL_CHANNEL_DDR_ROW_CONFIG                *RowConfArray,
  IN OUT  PEI_MEMORY_LOG                        *MemoryLog

  );

typedef
EFI_STATUS
(EFIAPI *PEI_CONFIGURE_MEMORY) (
  IN      EFI_PEI_SERVICES                      **PeiServices,
  IN struct _GENERIC_MEMORY_CONTROLLER_PPI  *This,
  IN      UINT8                                 DimmSockets,
  IN OUT  PEI_DUAL_CHANNEL_DDR_TIMING_DATA               *TimingData,
  IN OUT  PEI_DUAL_CHANNEL_DDR_ROW_CONFIG                *RowConfArray,
  IN OUT  PEI_MEMORY_LOG                        *MemoryLog

  );

typedef struct _GENERIC_MEMORY_CONTROLLER_PPI {
  BOOLEAN                                     SoftReset;
  PEI_MEMORY_CONTROLLER_RESET                 Reset;
  PEI_GET_ROW_INFO                            RowInfo;
  PEI_GET_MEMORY_MAP                          GetMemoryMap;
  PEI_DETECT_MEMORY                           DetectMemory;
  PEI_CONFIGURE_MEMORY                        ConfigureMemory;
} GENERIC_MEMORY_CONTROLLER_PPI;

extern EFI_GUID gGenericMemoryControllerPpiGuid;

#endif
