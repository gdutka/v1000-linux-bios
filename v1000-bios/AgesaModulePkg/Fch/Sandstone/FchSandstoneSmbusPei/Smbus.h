/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH PEI driver.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  FCH PEIM
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _FCH_SMBUS_H_
#define _FCH_SMBUS_H_

#include <Ppi/Smbus2.h>
#include "FchPlatform.h"
//
// EQUate Definitions
//
#define SMB_IOREG00         0x00        // SMBusStatus
#define SMB_IOREG01         0x01        // SMBusSlaveStatus
#define SMB_IOREG02         0x02        // SMBusControl
#define SMB_IOREG03         0x03        // SMBusHostCmd
#define SMB_IOREG04         0x04        // SMBusAddress
#define SMB_IOREG05         0x05        // SMBusData0
#define SMB_IOREG06         0x06        // SMBusData1
#define SMB_IOREG07         0x07        // SMBusBlockData
#define SMB_IOREG08         0x08        // SMBusSlaveControl

#define SMB_CMD_QUICK       0x00 << 2   // Quick Read or Write
#define SMB_CMD_BYTE        0x01 << 2   // Byte Read or Write
#define SMB_CMD_BYTE_DATA   0x02 << 2   // Byte Data Read or Write
#define SMB_CMD_WORD_DATA   0x03 << 2   // Word Data Read or Write
#define SMB_CMD_BLOCK       0x05 << 2   // Block Read or Write
#define SMB_ALL_HOST_STATUS 0x1f        // HostBusy+SMBInterrupt+DeviceErr+BusCollision+Failed

#define TRY_TIMES           0x03
#define SMBUS_READ_ENABLE   BIT0

typedef struct  _AMD_SMBUS_PPI_PRIVATE AMD_SMBUS_PPI_PRIVATE;

//
typedef
UINT8
(EFIAPI *SMBUS_IO_READ) (
  IN       AMD_SMBUS_PPI_PRIVATE  *Private,
  IN       UINTN                  Address
  );


typedef
VOID
(EFIAPI *SMBUS_IO_WRITE) (
  IN       AMD_SMBUS_PPI_PRIVATE  *Private,
  IN       UINTN                  Address,
  IN       UINT8                  Data
  );


typedef
EFI_STATUS
(EFIAPI *SMBUS_STALL) (
  IN       EFI_PEI_SERVICES   **PeiServices,
  IN       EFI_PEI_STALL_PPI      *This,
  IN       UINTN              Microseconds
  );


//
// Module data structure
//
/// Private SMBUS PPI Data Structures
typedef struct _AMD_SMBUS_PPI_PRIVATE {
  EFI_PEI_SMBUS2_PPI           SmbusPpi;           ///< SMBUS PPI
  EFI_PEI_SERVICES        **PeiServices;      ///< Pointer to PeiServices
  UINTN                   BaseAddress;        ///< SMBUS Base Address
  UINT8                   RegisterIndex;      ///< SMBUS Base Register Index
  SMBUS_IO_READ           IoRead8;            ///< Local Function Pointer
  SMBUS_IO_WRITE          IoWrite8;           ///< Local Function Pointer
} AMD_SMBUS_PPI_PRIVATE;


/// Private SMBUS Data Block Structure
typedef struct _AMD_PEI_FCH_SMBUS_PRIVATE {
  AMD_SMBUS_PPI_PRIVATE   SmbusPpi0;          ///< SMBUS0 Data Block
  AMD_SMBUS_PPI_PRIVATE   SmbusPpi1;          ///< SMBUS1 Data Block
} AMD_PEI_FCH_SMBUS_PRIVATE;

#endif // _FCH_SMBUS_H_

