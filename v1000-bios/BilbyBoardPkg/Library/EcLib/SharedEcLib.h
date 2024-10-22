/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SHARED_EC_LIB_H_
#define _SHARED_EC_LIB_H_

#include <Fch.h>
#include <Uefi.h>
#include <Library/EcLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/PciExpressLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseOemSvcKernelLib.h>

//
// EC command definition.
//
#define EC_CMD_STATUS_PORT        0x666
#define EC_CMD_DATA_PORT          0x662

#define EC_STATUS_INDEX           0xCF
#define EC_ACPI_ENABLE_BIT        0x04

#define TIEC_LIB_VERSION          0xB9
#define TIEC_VERSION              0xC2

#define EC_TIME_OUT               0x10000
#define EC_KEY_OBF                1
#define EC_KEY_IBF                2

//
// EC config definition.
//
#define EC_INDEX_PORT             0x4E
#define EC_DATA_PORT              0x4F

#define TIEC_CONFIG_ENABLE        0x00
#define TIEC_CONFIG_UNLOCK        0x55
#define TIEC_CONFIG_LOCK          0xAA

#define TIEC_ACTIVATE             0x30
#define TIEC_LDN_SELECT           0x07

#define TIEC_DEVID0               0x20
#define TIEC_DEVID1               0x21

#define TIEC_IOBASE_MSB           0x60
#define TIEC_IOBASE_LSB           0x61

#define TIEC_DC_TIMER             0xC7
#define TIEC_AC_TIMER             0xC8

//
// EC mailbox command and data.
//
#define EC_ENTER_WAIT_COMMAND     0xB4
#define EC_ENTER_WAIT_ACKNOWLEDGE 0xFA
#define EC_LEAVE_WAIT_COMMAND     0xB5
#define EC_LEAVE_WAIT_ACKNOWLEDGE 0xFA

typedef enum {
   LPCPnPChan = 0,
   LPCCOM0Chan,
   LPCACPIChan,
   LPCVI2CChan,
   LPCPort80Chan,
   LPCCOM1Chan,
   LPC8042Chan,
   LPCNOTHINGChan,
} EC_FEATURE_ENUM;

typedef struct {
  UINT8   Register;
  BOOLEAN Value;
} ENABLE_EC_TABLE;

UINT8
EcRead (
  IN UINT8                        Index
  );

VOID
EcWrite (
  IN UINT8                        Index,
  IN UINT8                        Value
  );

#endif
