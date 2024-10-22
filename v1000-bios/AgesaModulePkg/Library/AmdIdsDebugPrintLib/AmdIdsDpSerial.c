/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains the functions related to serial port output
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Uefi.h>
#include <Pi/PiMultiPhase.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdIdsDebugPrintLib.h>
#include <AmdIdsDebugPrintLocal.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDIDSDEBUGPRINTLIB_AMDIDSDPSERIAL_FILECODE

#define IDS_SERIAL_PORT (PcdGet64(PcdAmdIdsDebugPrintSerialPort))

#define IDS_SERIAL_PORT_LSR \
(IDS_SERIAL_PORT > 0xFFFF ? \
(IDS_SERIAL_PORT + 5*4) : \
(IDS_SERIAL_PORT + 5))

#define IDS_LSR_TRANSMIT_HOLDING_REGISTER_EMPTY_MASK BIT5

#define IDS_SERIAL_PORT_LSR_READ(Value) \
(IDS_SERIAL_PORT > 0xFFFF ? \
LibAmdMemRead (AccessWidth8, (UINT64) IDS_SERIAL_PORT_LSR, &Value, NULL) : \
LibAmdIoRead (AccessWidth8, (UINT16) IDS_SERIAL_PORT_LSR, &Value, NULL))

#define IDS_SERIAL_PORT_WRITE(ByteSended) \
(IDS_SERIAL_PORT > 0xFFFF ? \
LibAmdMemWrite (AccessWidth8, (UINT64) IDS_SERIAL_PORT, &ByteSended, NULL) : \
LibAmdIoWrite (AccessWidth8, (UINT16) IDS_SERIAL_PORT, &ByteSended, NULL))

#define IDS_SERIAL_PORT_MSR \
(IDS_SERIAL_PORT > 0xFFFF ? \
(IDS_SERIAL_PORT + 6*4) : \
(IDS_SERIAL_PORT + 6))

#define IDS_SERIAL_PORT_MSR_MASK     (BIT4 + BIT5)

#define IDS_SERIAL_PORT_MSR_READ(Value) \
(IDS_SERIAL_PORT > 0xFFFF ? \
LibAmdMemRead(AccessWidth8, (UINT64) IDS_SERIAL_PORT_MSR, &Value, NULL) : \
LibAmdIoRead(AccessWidth8, (UINT16) IDS_SERIAL_PORT_MSR, &Value, NULL))

/**
 *  Determine whether IDS console is enabled.
 *
 *
 *  @retval       TRUE    Alway return true
 *
 **/
BOOLEAN
AmdIdsDpSerialSupport (
  VOID
  )
{
  UINT8 Value;

  if (PcdGetBool (PcdAmdIdsDebugPrintSerialPortEnable)) {
    if (PcdGetBool (PcdAmdIdsDebugPrintSerialPortDetectCableConnection) ) {
      // Wait for both DSR and CTS to be set
      //   DSR is set if a cable is connected.
      //   CTS is set if it is ok to transmit data
      IDS_SERIAL_PORT_MSR_READ (Value);
      if ((Value == 0xFF) || ((Value & IDS_SERIAL_PORT_MSR_MASK) != IDS_SERIAL_PORT_MSR_MASK)) {
        return FALSE;
      }
    }
    return TRUE;
  }
  return FALSE;
}

VOID
AmdIdsDpSerialConstructor (
  VOID
  )
{
  return;
}

/**
 *  Get Serial customize Filter
 *
 *  @param[in,out] Filter    Filter do be filled
 *
 *  @retval       FALSE    Alway return FALSE
 *
 **/
BOOLEAN
AmdIdsDpSerialGetFilter (
  IN OUT   UINT64 *Filter
  )
{
  return FALSE;
}

/**
 *  Send byte to  Serial Port
 *
 *  Before use this routine, please make sure Serial Communications Chip have been initialed
 *
 *  @param[in] ByteSended   Byte to be sended
 *
 *  @retval       TRUE    Byte sended successfully
 *  @retval       FALSE   Byte sended failed
 *
 **/
BOOLEAN
AmdIdsDpSerialSendByte (
  IN      CHAR8   ByteSended
  )
{
  UINT32 RetryCount;
  UINT8 Value;

  // Wait until LSR.Bit5 (Transmitter holding register Empty)
  RetryCount = 2000;
  do {
    IDS_SERIAL_PORT_LSR_READ (Value);
    RetryCount--;
  } while (((Value & IDS_LSR_TRANSMIT_HOLDING_REGISTER_EMPTY_MASK) == 0) && (RetryCount > 0));

  if (RetryCount == 0) {
    // Time expired
    return FALSE;
  } else {
    IDS_SERIAL_PORT_WRITE (ByteSended);
    return TRUE;
  }
}


/**
 *  Print formated string
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *
**/
VOID
AmdIdsDpSerialPrint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize
  )
{
  BOOLEAN SendStatus;
  while (BufferSize--) {
    SendStatus = AmdIdsDpSerialSendByte (*Buffer);
    Buffer ++;
  }
}


CONST AMD_IDS_DEBUG_PRINT_INSTANCE   AmdIdsDebugPrintSerialInstance =
{
  AmdIdsDpSerialSupport,
  AmdIdsDpSerialConstructor,
  AmdIdsDpSerialGetFilter,
  AmdIdsDpSerialPrint
};




