/*****************************************************************************
 *
 * Copyright 20019- 2019 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software and documentation (if
 * any) (collectively, the "Materials") pursuant to the terms and conditions of
 * the Software License Agreement included with the Materials.  If you do not
 * have a copy of the Software License Agreement, contact your AMD
 * representative for a copy.
 *
 * You agree that you will not reverse engineer or decompile the Materials, in
 * whole or in part, except as allowed by applicable law.
 *
 * WARRANTY DISCLAIMER:  THE MATERIALS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE
 * MATERIALS WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM
 * CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE
 * OF THE MATERIAL IS ASSUMED BY YOU.  Some jurisdictions do not allow the
 * exclusion of implied warranties, so the above exclusion may not apply to
 * You.
 *
 * LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL
 * NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
 * INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
 * THE MATERIALS OR THIS AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  In no event shall AMD's total
 * liability to You for all damages, losses, and causes of action (whether in
 * contract, tort (including negligence) or otherwise) exceed the amount of
 * $100 USD. You agree to defend, indemnify and hold harmless AMD and its
 * licensors, and any of their directors, officers, employees, affiliates or
 * agents from and against any and all loss, damage, liability and other
 * expenses (including reasonable attorneys' fees), resulting from Your use of
 * the Materials or violation of the terms and conditions of this Agreement.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS:  The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgment of AMD's proprietary rights in them.
 *
 * EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as
 * stated in the Software License Agreement.
 *******************************************************************************
 */

#ifndef _PSP_ROM_ARMOR_LIB_H_
#define _PSP_ROM_ARMOR_LIB_H_

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Uefi.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define PSP_MAX_SPI_CMD_SUPPORT           (4)      ///< Max number of SPI command support
#define PSP_MAX_SPI_DATA_BUFFER_SIZE     (72)      ///< Max SPI Command Data Buffer Size
#define PSP_MAX_WHITE_LIST_CMD_NUM       (32)      ///< Max White list allowed command array number support
#define PSP_MAX_WHITE_LIST_REGION_NUM    (16)      ///< Max White list allowed region array number support

#define SPI_CHIP_SELECT_ALL               (0)      ///< Allowed on all chip selects
#define SPI_CHIP_SELECT_1                 (1)      ///< Chip Select 1
#define SPI_CHIP_SELECT_2                 (2)      ///< Chip Select 2

#define SPI_COMMAND_FREQUENCY_66_66MHZ    (0)      ///< 0=66.66MHz
#define SPI_COMMAND_FREQUENCY_33_33MHZ    (1)      ///< 1=33.33MHz
#define SPI_COMMAND_FREQUENCY_22_22MHZ    (2)      ///< 2=22.22MHz
#define SPI_COMMAND_FREQUENCY_16_66MHZ    (3)      ///< 3=16.66MHz
#define SPI_COMMAND_FREQUENCY_100MHZ      (4)      ///< 4=100MHz
#define SPI_COMMAND_FREQUENCY_800KHZ      (5)      ///< 5=800KHz

#define SPI_COMMAND_NOT_PROCEDDED         (0)      ///< 0 = Command not examined/processed
#define SPI_COMMAND_COMPLETED             (1)      ///< 1 = Command completed successfully
#define SPI_COMMAND_EXECUTION_ERROR       (2)      ///< 2 = Execution Error (i.e. timeout)
#define SPI_COMMAND_NOT_ALLOWED           (3)      ///< 3 = Command not allowed by Whitelist
#define SPI_COMMAND_MALFORMED             (4)      ///< 4 = Command malformed

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

#pragma pack (push, 1)

typedef struct {
  UINT8    ChipSelect;     // 1 = CS1, 2 = CS2, all other values illegal

  UINT8    Frequency;      // 0=66.66MHz, 1=33.33MHz, 2=22.22MHz,
                           // 3=16.66MHz, 4=100Mhz, 5=800KHz, all others illegal

  UINT8    BytesToTx;      // Bytes to Transmit, valid range is 0-72. Does not include the
                           // SPI Opcode byte, but does include the address, dummy bytes, and
                           // data.

  UINT8    BytesToRx;      // Bytes to Receive from device, BytesToTx + BytesToRx <= 72

  UINT8    OpCode;         // The SPI Command OpCode (the first byte sent by the SPI controller)

  UINT8    Reserved[3];    // Reserved for future expansion

  UINT8    Buffer[PSP_MAX_SPI_DATA_BUFFER_SIZE];     // The remaining 0-72 bytes sent/received by the SPI controller.
                                                     //
                                                     // The SPI Controller will
                                                     //   1. Assert the ChipSelect
                                                     //   2. Send the one byte OpCode
                                                     //   3. Send Buffer[0] to Buffer[BytesToTx-1] to the SPI device
                                                     //   4. Read BytesToRx bytes from the device into Buffer[BytesToTx] to
                                                     //      Buffer[BytesToTx+BytesToRx-1]
                                                     //   5. Deassert the ChipSelect line
                                                     //
                                                     // SPI ROM Commands that include a target address send the address
                                                     // immediately after the OpCode (i.e. Buffer[0..2] or Buffer[0..3]
                                                     // depending if 24 or 32bit addresses are associated with the OpCode).
                                                     // See the SPI ROM Device’s datasheet for full details on your
                                                     // devices commands and formats.
} SPI_COMMAND;


typedef union {
  struct {                                  ///< SPI_COMMUNICATION_RESULT
    UINT16       Command0Result:4;          ///< Result[ 3: 0] The result of Command[0]
    UINT16       Command1Result:4;          ///< Result[ 7: 4] The result of Command[1]
    UINT16       Command2Result:4;          ///< Result[11: 8] The result of Command[2]
    UINT16       Command3Result:4;          ///< Result[15:12] The result of Command[3]
  } Field;
  UINT16  Value;
} SPI_COMMUNICATION_RESULT;

typedef struct {
  UINT8    ReadyToRun;     // Set to FALSE by x86 while the buffer is being constructed. Set to
                           // TRUE by x86 as the last step in building the communication buffer,
                           // just before x86 rings the PSP doorbell.
                           //
                           // Set to FALSE by PSP after the PSP copies the buffer from DRAM to
                           // private SRAM.

  UINT8    CommandCount;   // Number of commands to execute, Valid Values 1-4

  SPI_COMMUNICATION_RESULT   SpiCommunicationResult;
                           // Set to zero by x86 when the buffer is built. Atomically set to a
                           // non-zero value by the PSP to indicate the PSP has finished
                           // processing the requests in the communication buffer.  The specific
                           // value written by the PSP provides per command results
                           //
                           // Result values with special meaning:
                           // 0x0000 = (written by x86) PSP has not finished handling the request
                           // 0x1000 = PSP determined the request is malformed (invalid
                           //          CommandCount, chipselect, BytesToRx/Tx, etc)
                           // 0x2000, 0x3000, 0x4000, ... , 0xF000 = Reserved for future errors
                           //
                           // Generic Result values:
                           //   SpiCommunicationResult[ 3: 0] The result of Command[0]
                           //   SpiCommunicationResult[ 7: 4] The result of Command[1]
                           //   SpiCommunicationResult[11: 8] The result of Command[2]
                           //   SpiCommunicationResult[15:12] The result of Command[3]
                           //      0 = Command not examined/processed
                           //      1 = Command completed successfully
                           //      2 = Execution Error (i.e. timeout)
                           //      3 = Command not allowed by Whitelist
                           //      4 = Command malformed
                           //      5-15 = reserved for future use
                           //
                           // Examples of Generic Results:
                           //   0x0000 - PSP has not finished the request
                           //   0x0001 - PSP ran Command0 successfully, and is now idle
                           //   0x0111 - PSP ran Command0/1/2 successfully and is now idle
                           //   0x0031 - PSP ran Command0, but Command1 was blocked by whitelist

  SPI_COMMAND SpiCommand[PSP_MAX_SPI_CMD_SUPPORT];     // Spi command array
} SPI_COMMUNICATION_BUFFER;


typedef struct {
  UINT8 ChipSelect;        // 0= Allowed on all chip selects, 1= CS1, 2= CS2, all else invalid

  UINT8 Frequency;         // The allowed frequency for the command
                           // 0=66.66MHz, 1=33.33MHz, 2=22.22MHz,
                           // 3=16.66MHz, 4=100Mhz, 5=800KHz, all others illegal

  UINT8 OpCode;            // The allowed commands opcode

  UINT8 MinTx, MaxTx;      // The range of allowed transmit byte counts for this command
                           // (does not include opcode)

  UINT8 MinRx, MaxRx;      // The range of allowed Rx byte counts

  UINT8 AddrChkMethod;     // 0=No address verification performed
                           // 1=Treat Buffer[0-2] as a 24-bit address, and verify the entire
                           //   ImpactZone of the command falls within one of the allowed regions
                           // 2=Treat Buffer[0-3] as a 32-bit address, and verify the entire
                           //   ImpactZone of the command falls within one of the allowed regions

  UINT32 ImpactSize;       // The Impact Zone is the naturally aligned power of two sized block
                           // of addresses that may be impacted by a given SPI Command.  For
                           // example, a sector erase command targeted at an address within a
                           // 64K block will impact every byte within that 64K block. Likewise
                           // a page program SPI command (i.e. write) may impact many bytes
                           // within the targeted 256/512 byte page due to page wrap-around, but
                           // no bytes outside the page. The ImapctSize field specifies the power
                           // of two size of the ImpactZone for this command. If VerifyAddress is
                           // zero (no checking) this field must also be zero, otherwise this
                           // field must be a power of two between 256 and 64MB (256, 512, ...,
                           // 67108864). NOTE: When setting this field, carefully examine your
                           // device's datasheet.
                           //
} WHITE_LIST_ALLOWED_COMMAND;


typedef struct  { // 8 bytes
  UINT32 StartAddress;        // LSB must be 0x00, bit31 identifies a chipselect: 0=CS1, 1=CS2
  UINT32 EndAddress;          // LSB must be 0xFF, StartAddress must be less than EndAddress
} WHITE_LIST_ALLOWED_REGION;

typedef struct {
  UINT8 AllowedCmdCount;
  UINT8 AllowedRegionCount;
  WHITE_LIST_ALLOWED_COMMAND WhitelistAllowedCommands[PSP_MAX_WHITE_LIST_CMD_NUM];
  WHITE_LIST_ALLOWED_REGION  WhitelistAllowedRegions[PSP_MAX_WHITE_LIST_REGION_NUM];
} SPI_WHITE_LIST;


#pragma pack (pop)

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 * Request secures the SPI Controller
 *
 * @param[in, out]  SpiCommunicationBuffer    Spi Communication Structure Buffer pointer
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspEnterSmmOnlyMode (
  IN     SPI_COMMUNICATION_BUFFER  *SpiCommunicationBuffer
);

/**
 * Loads the whitelist into the PSP.
 *
 * @param[in]       SpiWhitelist              SPI white list structure buffer pointer.
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspEnforceWhitelist (
  IN     SPI_WHITE_LIST  *SpiWhitelist
  );

/**
 * Request execute SPI command provide in TSEG comm buffer.
 *
 * @param[in]       N/A
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspExecuteSpiCommand (VOID);

/**
 * Request PSP firmware swtich SPI controller chip select.
 *
 *
 * @param[in]       Chipset                   SPI controller chip select, 0= Allowed on all chip selects, 1= CS1, 2= CS2, all else invalid
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspSwitchChipSelect (
  IN     UINT8      ChipSelect
  );

#endif  //_PSP_ROM_ARMOR_LIB_H_


