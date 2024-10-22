/** @file
 FlashRpmcAccessLibSmm.c headfile.

;******************************************************************************
;* Copyright (c) 2018 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************

*/

#ifndef _FLASH_RPMC_ACCESS_LIB_SMM_H_
#define _FLASH_RPMC_ACCESS_LIB_SMM_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>

#include <Library/DebugLib.h>

extern
EFI_STATUS
EFIAPI
SendSpiCmd (
  IN     UINT8                  OpCode,
  IN     UINT8                  *BufferIn,
  IN     UINT8                  SizeIn,
  IN OUT UINT8                  *BufferOut,
  IN OUT UINT8                  *SizeOut
  );

//
// Normal SPI OpCode
//
#define RDSR                            0x05        // Read Status Register.

//
// SFDP
//
#define SFDP_SIGNATURE                  0x50444653
#define RDSFDP                          0x5A

//
// RPMC Table
//
#define RPMC_OP1                        0x9B
#define RPMC_OP2                        0X96
#define RPMC_PARAMETER_ID               0x03

#define COMMAND_TYPE_RESERVED           0x04        // CmdType = 04-FFH. These OP1 commands are reserved and
                                                    // cannont be used.

//
// Define extended Status Register Error message.
//
#define RPMC_CMD_BUSY                   0x01
#define RPMC_CMD_SUCCESSFUL_COMPLETION  0x80        // Bit7.

typedef enum {
  CmdWriteRootKeyReg,
  CmdUpdateHmacKeyReg,
  CmdIncMc,
  CmdReqMc,
  CmdRsvd                        // CmdType = 04-0FFH. These OP1 commands are reserved and cannont be used.
} RPMC_CMD_TYPE;

#pragma pack(1)
typedef struct {
  UINT32  SfdpSignature;
  UINT8   SfdpMinorRevNum;
  UINT8   SfdpMajorRevNum;
  UINT8   NumOfParameterHeader;  // n: n + 1 parameter header.
  UINT8   Rsvd;
} SPI_SFDP_HEADER;

typedef struct {
  UINT8  ID;
  UINT8  ParameterTableMinorRev;
  UINT8  ParameterTableMajorRev;
  UINT8  ParameterTableLen;            // In double word
  UINT8  ParameterTablePointer[3];  
  UINT8  Reserve0;
} SPI_SFDP_PARAMETER_HEADER;

typedef union {
  struct {
    UINT8  FlashHardening:1;
    UINT8  McSize:1;
    UINT8  BusyPollingMethod:1;
    UINT8  Rsvd:1;
    UINT8  NumCounter:4;               // Number of supported counters-1.
  } Byte0;
  UINT8  Byte0Raw;
} SPI_RPMC_PARAMETER;

//
// RPMC Command: Increment Monotonic Counter.
//
typedef struct {
  UINT8   CmdType;               // Byte 1.
  UINT8   CounterAddr;           // Byte 2.
  UINT8   Rsvd;                  // Byte 3.
  UINT32  CounterData;           // Byte 4-7.
  UINT8   Signature[32];         // Byte 8-39.
} RPMC_INCREMENT_MC_CMD_INFO;

//
// RPMC Command: Request Monotonic Counter.
//
typedef struct {
  UINT8   CmdType;               // Byte 1.
  UINT8   CounterAddr;           // Byte 2.
  UINT8   Rsvd;                  // Byte 3.
  UINT8   Tag[12];               // Byte 4-15.
  UINT8   Signature[32];         // Byte 16-47.
} RPMC_REQUEST_MC_CMD_INFO;

//
// RPMC Command: Read Data.
//
typedef struct {
  UINT8   ExtendedStatus;        // Byte 2.
  UINT8   Tag[12];               // Byte 3-14.
  UINT32  CounterReadData;       // Byte 15-18
  UINT8   Signature[32];         // Byte 19-50.  
} RPMC_READ_DATA_CMD_INFO;
#pragma pack()

/**
  Check the ROM part whether supports the RPMC function.

  @retval TRUE                  The ROM part supported RPMC function.
  @retval FALSE                 The ROM part unsupported RPMC function.
**/
BOOLEAN
EFIAPI
RpmcSupportStatus (
  VOID
  );

/**
  Increment the Monotonic counter by 1 inside the Serial Flash Device.

  @param[in] CounterAddr        Target Monotonic counter address.
  @param[in] CounterData        Current Monotonic counter value.
  @param[in] Signature          Point to the 32 bytes Signature.

  @retval EFI_SUCCESS           Function successfully returned.
  @retval Others                Some error occurs when executing this function.
**/
EFI_STATUS
EFIAPI
RpmcIncMc (
  IN UINT8   CounterAddr,
  IN UINT32  CounterData,
  IN UINT8*  Signature
  );

/**
  Request the Monotonic counter value inside the Serial Flash Devicee.

  @param[in]      CounterAddr   Target Monotonic counter address.
  @param[in]      Tag           Point to the 12 bytes payload to be added.
  @param[in]      Signature     Point to the 32 bytes Signature.
  @param[in, out] CounterData   The latest Monotonic counter value.

  @retval EFI_SUCCESS           Function successfully returned.
  @retval Others                Some error occurs when executing this function.
**/
EFI_STATUS
EFIAPI
RpmcReqMc (
  IN     UINT8   CounterAddr,
  IN     UINT8   *Tag,
  IN     UINT8   *Signature,
  IN OUT UINT32  *CounterData,
  IN OUT UINT8   *OutputSignature
  );
#endif

