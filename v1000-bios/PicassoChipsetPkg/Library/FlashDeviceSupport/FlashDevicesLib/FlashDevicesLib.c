/** @file
  Collection of supported Flash devices

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

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/FdSupportLib.h>
#include <Library/SpiAccessLib.h>
#include <Library/IrsiRegistrationLib.h>
#include "SpiDeviceId.h"

LPC_FLASH_DEVICE_INFO LpcType1DeviceInfoA = {
  0xFFBC0000,
  0xFFF05555,
  0xFFF02AAA
};

LPC_FLASH_DEVICE_INFO LpcType1DeviceInfoB = {
  0xFFFF0000,
  0xFFF05555,
  0xFFF02AAA
};

LPC_FLASH_DEVICE_INFO LpcType2DeviceInfo = {
  0xFFFF0000,
  0,
  0
};

//
// 8 MB
//
FLASH_DEVICE SST49LF008A = {
  LpcFlashType1,
  {
    0x5ABF,
    0x0000,
    FLASH_SIZE_1024K,
    "SST",
    "49LF008A",
    {
      0x0010,          //size = 4k
      0x0100,          //mutilpe = 0x100
      0xFFFF
    },
  },
  &LpcType1DeviceInfoA
};

FLASH_DEVICE SST39VF080 = {
  LpcFlashType1,
  {
    0xD8BF,
    0x0000,
    FLASH_SIZE_1024K,
    "SST",
    "39VF080",
    {
      0x0010,          //size = 4k
      0x0100,          //mutilpe = 0x100
      0xFFFF
    },
  },
  &LpcType1DeviceInfoB
};

FLASH_DEVICE INTEL82802AC = {
  LpcFlashType2,
  {
    0xAC89,
    0x0000,
    FLASH_SIZE_1024K,
    "INTEL",
    "82802AC",
    {
      0x0100,   //size =64k
      0x0010,   //mutilpe = 0x10
      0xFFFF
    },
  },
  &LpcType2DeviceInfo
};

//
// 4 MB
//
FLASH_DEVICE SST49LF004A = {
  LpcFlashType1,
  {
    0x60BF,
    0x0000,
    FLASH_SIZE_512K,
    "SST",
    "49LF004A",
    {
      0x0100,   //size =64k
      0x0008,   //mutilpe = 0x08
      0xFFFF
    },
  },
  &LpcType1DeviceInfoA
};

FLASH_DEVICE PMC49FL004T = {
  LpcFlashType1,
  {
    0x6E9D,
    0x0000,
    FLASH_SIZE_512K,
    "PMC",
    "49FL004T",
    {
      0x0010,  //size = 4k
      0x0080,  //mutiple = 0x80
      0xFFFF
    },
  },
  &LpcType1DeviceInfoA
};

FLASH_DEVICE WINBOND39V040FAP = {
  LpcFlashType1,
  {
    0x34DA,
    0x0000,
    FLASH_SIZE_512K,
    "WIN",
    "39V040FPA",
    {
      0x0100,  //size = 64k
      0x0008,  //mutiple = 0x08
      0xFFFF
    },
  },
  &LpcType1DeviceInfoA
};

FLASH_DEVICE ST50FLW040A = {
  LpcFlashType2,
  {
    0x0820,
    0x0000,
    FLASH_SIZE_512K,
    "ST",
    "50FLW040A",
    {
      0x0010,  //size = 4k
      0x0080,  //mutiple = 0x100
      0xFFFF
    },
  },
  &LpcType2DeviceInfo
};


//
// SST 25LF040
//
SPI_CONFIG_BLOCK SST25LF040_Config =
{
  {
    0x90,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (ADR <<  0) |                 // Address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x50                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  0,                            // 0:Byte program,                1:Multibytes program
  2,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0x03 << 2,                    // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x80000,                      // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE SST25LF040 =
{
  SpiFlashType,
  {
    ID_SST_25LF040,             // ID
    0x0000,
    FLASH_SIZE_512K,
    "SST",
    "25LF040",
    {
      0x0010,                   //size = 4k
      0x0080,                   //mutiple = 0x80
      0xFFFF
    },
  },
  &SST25LF040_Config
};

//
// SST 25LF040A
//
SPI_CONFIG_BLOCK SST25LF040A_Config =
{
  {
    0x90,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (ADR <<  0) |                 // Address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x50                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  0,                            // 0:Byte program,                1:Multibytes program
  2,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0x03 << 2,                    // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x80000,                      // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE SST25LF040A =
{
  SpiFlashType,
  {
    ID_SST_25LF040A,            // ID
    0x0000,
    FLASH_SIZE_512K,
    "SST",
    "25LF040A",
    {
      0x0010,                   //size = 4k
      0x0080,                   //mutiple = 0x80
      0xFFFF
    },
  },
  &SST25LF040A_Config
};

//
// SST 25LF080A
//
SPI_CONFIG_BLOCK SST25LF080A_Config =
{
  {
    0x90,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (ADR <<  0) |                 // Address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x50                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  0,                            // 0:Byte program,                1:Multibytes program
  2,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0x07 << 2,                    // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x100000,                     // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE SST25LF080A =
{
  SpiFlashType,
  {
    ID_SST_25LF080A,            // ID
    0x0000,
    FLASH_SIZE_1024K,
    "SST",
    "25LF080A",
    {
      0x0010,                   //size = 4k
      0x0100,                   //mutilpe = 0x100
      0xFFFF
    },
  },
  &SST25LF080A_Config
};

//
// SST 25VF080B
//
SPI_CONFIG_BLOCK SST25VF080B_Config =
{
  {
    0x9F,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (NAR <<  0) |                 // Address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x50                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  0,                            // 0:Byte program,                1:Multibytes program
  3,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0x07 << 2,                    // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x100000,                     // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE SST25VF080B =
{
  SpiFlashType,
  {
    ID_SST_25VF080B,            // ID
    0x0000,
    FLASH_SIZE_1024K,
    "SST",
    "25VF080B",
    {
      0x0010,                   //size = 4k
      0x0100,                   //mutilpe = 0x100
      0xFFFF
    },
  },
  &SST25VF080B_Config
};

//
// SST 25VF016B
//
SPI_CONFIG_BLOCK SST25VF016B_Config =
{
  {
    0x90,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (ADR <<  0) |                 // Address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x50                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  0,                            // 0:Byte program,                1:Multibytes program
  2,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0x07 << 2,                    // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x200000,                     // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE SST25VF016B =
{
  SpiFlashType,
  {
    ID_SST_25VF016B,            // ID
    0x0000,
    FLASH_SIZE_2048K,
    "SST",
    "25VF016B",
    {
      0x0010,                   //size = 4k
      0x0200,                   //mutilpe = 0x100
      0xFFFF
    },
  },
  &SST25VF016B_Config
};

//
// SST 26VF064B
//
SPI_CONFIG_BLOCK SST26VF064B_Config =
{
  {
    0x9F,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (ADR <<  0) |                 // Address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x50                        // Write status enable prefix
  },
  0,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  0,                            // 0:Byte program,                1:Multibytes program
  3,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0,                            // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x800000,                     // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE SST26VF064B =
{
  SpiFlashType,
  {
    ID_SST_26VF064B,                // ID
    0x0000,
    FLASH_SIZE_8192K,
    "SST",
    "26VF064B",
    {
      0x0010,                      //size = 4k
      0x0800,                      //mutilpe = 0x800
      0xFFFF
    },
  },
  &SST26VF064B_Config
};


//
// Atmel 26DF041
//
SPI_CONFIG_BLOCK ATMEL26DF041_Config =
{
  {
    0x9f,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x00,                       // Write status command (unsupported in Atmel AT26DF041)
    0x03,                       // Read command
    0x00                        // Read status command (unsupported in Atmel AT26DF041)
  },
  (NAR <<  0) |                 // Non-address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x00,                       // Write enable prefix (unsupported in Atmel AT26DF041)
    0x00                        // Write status enable prefix (unsupported in Atmel AT26DF041)
  },
  0,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  0,                            // 0:Byte program,                1:Multibytes program
  2,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0,                            // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x80000,                      // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE ATMEL26DF041 =
{
  SpiFlashType,
  {
    ID_ATMEL_26DF041,           // ID
    0x0000,
    FLASH_SIZE_512K,
    "Atmel",
    "25DF041",
    {
      0x0010,                   //size = 4k
      0x0080,                   //mutilpe = 0x80
      0xFFFF
    },
  },
  &ATMEL26DF041_Config
};

//
// Atmel 26DF081A
//
SPI_CONFIG_BLOCK ATMEL26DF081A_Config =
{
  {
    0x9f,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (NAR <<  0) |                 // Non-address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x06                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                            // 0:Byte program,                1:Multibytes program
  2,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0x0f << 2,                    // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x100000,                     // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE ATMEL26DF081A =
{
  SpiFlashType,
  {
    ID_ATMEL_AT26DF081,         // ID
    0x0000,
    FLASH_SIZE_1024K,
    "Atmel",
    "25DF081A",
    {
      0x0010,                   //size = 4k
      0x0100,                   //mutilpe = 0x100
      0xFFFF
    },
  },
  &ATMEL26DF081A_Config
};

//
// Atmel 26DF161
//
SPI_CONFIG_BLOCK ATMEL26DF161_Config =
{
  {
    0x9f,                     // Read ID command
    0x20,                     // Erase command
    0x02,                     // Write command
    0x01,                     // Write status command (unsupported in Atmel AT26DF041)
    0x03,                     // Read command
    0x05                      // Read status command (unsupported in Atmel AT26DF041)
  },
  (NAR <<  0) |               // Non-address/read type for read ID command
  (ADW <<  2) |               // Address/write type for erase command
  (ADW <<  4) |               // Address/write type for write command
  (NAW <<  6) |               // Non-address/write type for write status command
  (ADR <<  8) |               // Address/read type for read command
  (NAR << 10) |               // Non-address/read type for read status command
  (ADW << 12) |               // Address/write type for block lock command
  (ADW << 14),                // Address/write type for block unlock command
  {
    0x06,                     // Write enable prefix (unsupported in Atmel AT26DF041)
    0x06                      // Write status enable prefix (unsupported in Atmel AT26DF041)
  },
  1,                          // 0:No global protect mechanism, 1:Global protect available
  0,                          // 0:No block protect mechanism,  1:Block protect available
  0,                          // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                          // 0:Byte program,                1:Multibytes program
  2,                          // Bytes of ID
  1,                          // Minimum bytes per program/read
  0,                          // 1:Has non-volatile bit in status register.
  0,                          // Reserve
  0x0F << 2,                  // Global protect code   (Used when global protect == 1)
  (UINT8)(~(0x0F << 2)),      // Global unprotect code (Used when global protect == 1)
  0,                          // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                          // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x200000,                   // Byte size of flash part
  0x1000,                     // Byte size of per erase
  0                           // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE ATMEL26DF161 =
{
  SpiFlashType,
  {
    ID_ATMEL_AT26DF161,       // ID
    0x0000,
    FLASH_SIZE_2048K,
    "Atmel",
    "26DF161",
    {
      0x0010,                 //size = 4k
      0x0200,                 //mutilpe = 0x100
      0xFFFF
    },
  },
  &ATMEL26DF161_Config
};

//
// Atmel 26DF321
//
SPI_CONFIG_BLOCK ATMEL26DF321_Config =
{
  {
    0x9f,                     // Read ID command
    0x20,                     // Erase command
    0x02,                     // Write command
    0x01,                     // Write status command (unsupported in Atmel AT26DF041)
    0x03,                     // Read command
    0x05                      // Read status command (unsupported in Atmel AT26DF041)
  },
  (NAR <<  0) |               // Non-address/read type for read ID command
  (ADW <<  2) |               // Address/write type for erase command
  (ADW <<  4) |               // Address/write type for write command
  (NAW <<  6) |               // Non-address/write type for write status command
  (ADR <<  8) |               // Address/read type for read command
  (NAR << 10) |               // Non-address/read type for read status command
  (ADW << 12) |               // Address/write type for block lock command
  (ADW << 14),                // Address/write type for block unlock command
  {
    0x06,                     // Write enable prefix (unsupported in Atmel AT26DF041)
    0x06                      // Write status enable prefix (unsupported in Atmel AT26DF041)
  },
  1,                          // 0:No global protect mechanism, 1:Global protect available
  0,                          // 0:No block protect mechanism,  1:Block protect available
  0,                          // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                          // 0:Byte program,                1:Multibytes program
  2,                          // Bytes of ID
  1,                          // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                          // Reserve
  0x0f << 2,                  // Global protect code   (Used when global protect == 1)
  0,                          // Global unprotect code (Used when global protect == 1)
  0,                          // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                          // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x400000,                   // Byte size of flash part
  0x1000,                     // Byte size of per erase
  0                           // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE ATMEL26DF321 =
{
  SpiFlashType,
  {
    ID_ATMEL_AT26DF321,       // ID
    0x0000,
    FLASH_SIZE_4096K,
    "Atmel",
    "26DF321",
    {
      0x0010,                 //size = 4k
      0x0400,                 //mutilpe = 0x400 (4096k/4k)
      0xFFFF
    },
  },
  &ATMEL26DF321_Config
};

//
// Winbond W25Q32BV
//
SPI_CONFIG_BLOCK WB25Q32BV_Config =
{
  {
    0x9f,                     // Read ID command
    0x20,                     // Erase command
    0x02,                     // Write command
    0x01,                     // Write status command
    0x03,                     // Read command
    0x05                      // Read status command
  },
  (NAR <<  0) |               // Non-address/read type for read ID command
  (ADW <<  2) |               // Address/write type for erase command
  (ADW <<  4) |               // Address/write type for write command
  (NAW <<  6) |               // Non-address/write type for write status command
  (ADR <<  8) |               // Address/read type for read command
  (NAR << 10) |               // Non-address/read type for read status command
  (ADW << 12) |               // Address/write type for block lock command
  (ADW << 14),                // Address/write type for block unlock command
  {
    0x06,                     // Write enable prefix
    0x06                      // Write status enable prefix
  },
  1,                          // 0:No global protect mechanism, 1:Global protect available
  0,                          // 0:No block protect mechanism,  1:Block protect available
  0,                          // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                          // 0:Byte program,                1:Multibytes program
  3,                          // Bytes of ID
  1,                          // Minimum bytes per program/read
  0,                          // 1:Has non-volatile bit in status register.
  0,                          // Reserve
  0x07 << 2,                  // Global protect code   (Used when global protect == 1)
  (UINT8)(~(0x07 << 2)),      // Global unprotect code (Used when global protect == 1)
  0,                          // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                          // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x400000,                   // Byte size of flash part
  0x1000,                     // Byte size of per erase
  0                           // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE WB25Q32BV =
{
  SpiFlashType,
  {
    ID_WINBOND_W25Q32BV,           // ID
    0x0000,
    FLASH_SIZE_4096K,
    "Winbond",
    "W25Q32BV",
    {
      0x0010,                      //size = 4k
      0x0400,                      //mutilpe = 0x800 (8196k/4k)
      0xFFFF
    },
  },
  &WB25Q32BV_Config
};

//
// Winbond W25Q64BV
//
SPI_CONFIG_BLOCK WB25Q64BV_Config =
{
  {
    0x9f,                     // Read ID command
    0x20,                     // Erase command
    0x02,                     // Write command
    0x01,                     // Write status command
    0x03,                     // Read command
    0x05                      // Read status command
  },
  (NAR <<  0) |               // Non-address/read type for read ID command
  (ADW <<  2) |               // Address/write type for erase command
  (ADW <<  4) |               // Address/write type for write command
  (NAW <<  6) |               // Non-address/write type for write status command
  (ADR <<  8) |               // Address/read type for read command
  (NAR << 10) |               // Non-address/read type for read status command
  (ADW << 12) |               // Address/write type for block lock command
  (ADW << 14),                // Address/write type for block unlock command
  {
    0x06,                     // Write enable prefix
    0x06                      // Write status enable prefix
  },
  0,                          // 0:No global protect mechanism, 1:Global protect available
  0,                          // 0:No block protect mechanism,  1:Block protect available
  0,                          // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                          // 0:Byte program,                1:Multibytes program
  3,                          // Bytes of ID
  1,                          // Minimum bytes per program/read
  1,                            // 1:Has non-volatile bit in status register.
  0,                          // Reserve
  0,                          // Global protect code   (Used when global protect == 1)
  0,                          // Global unprotect code (Used when global protect == 1)
  0,                          // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                          // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x800000,                   // Byte size of flash part
  0x1000,                     // Byte size of per erase
  0                           // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE WB25Q64BV =
{
  SpiFlashType,
  {
    ID_WINBOND_W25Q64BV,      // ID
    0x0000,
    FLASH_SIZE_8192K,
    "Winbond",
    "W25Q64BV",
    {
      0x0010,                 //size = 4k
      0x0800,                 //mutilpe = 0x800 (8196k/4k)
      0xFFFF
    },
  },
  &WB25Q64BV_Config
};

//
// Winbond W25Q64FV
//
SPI_CONFIG_BLOCK WB25Q64FV_Config =
{
  {
    0x9f,                     // Read ID command
    0x20,                     // Erase command
    0x02,                     // Write command
    0x01,                     // Write status command
    0x03,                     // Read command
    0x05                      // Read status command
  },
  (NAR <<  0) |               // Non-address/read type for read ID command
  (ADW <<  2) |               // Address/write type for erase command
  (ADW <<  4) |               // Address/write type for write command
  (NAW <<  6) |               // Non-address/write type for write status command
  (ADR <<  8) |               // Address/read type for read command
  (NAR << 10) |               // Non-address/read type for read status command
  (ADW << 12) |               // Address/write type for block lock command
  (ADW << 14),                // Address/write type for block unlock command
  {
    0x06,                     // Write enable prefix
    0x06                      // Write status enable prefix
  },
  0,                          // 0:No global protect mechanism, 1:Global protect available
  0,                          // 0:No block protect mechanism,  1:Block protect available
  0,                          // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                          // 0:Byte program,                1:Multibytes program
  3,                          // Bytes of ID
  1,                          // Minimum bytes per program/read
  0,                          // 1:Has non-volatile bit in status register.
  0,                          // Reserve
  0x07 << 2,                  // Global protect code   (Used when global protect == 1)
  (UINT8)(~(0x07 << 2)),      // Global unprotect code (Used when global protect == 1)
  0,                          // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                          // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x800000,                   // Byte size of flash part
  0x1000,                     // Byte size of per erase
  0                           // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE WB25Q64FV =
{
  SpiFlashType,
  {
    ID_WINBOND_W25Q64FV,           // ID
    0x0000,
    FLASH_SIZE_8192K,
    "Winbond",
    "W25Q64FV",
    {
      0x0010,                      //size = 4k
      0x0800,                      //mutilpe = 0x800 (8196k/4k)
      0xFFFF
    },
  },
  &WB25Q64FV_Config
};


//
// Atmel AT25DF641
//
SPI_CONFIG_BLOCK ATMEL25DF641_Config =
{
  {
    0x9f,                     // Read ID command
    0x20,                     // Erase command
    0x02,                     // Write command
    0x01,                     // Write status command (unsupported in Atmel AT26DF041)
    0x03,                     // Read command
    0x05                      // Read status command (unsupported in Atmel AT26DF041)
  },
  (NAR <<  0) |               // Non-address/read type for read ID command
  (ADW <<  2) |               // Address/write type for erase command
  (ADW <<  4) |               // Address/write type for write command
  (NAW <<  6) |               // Non-address/write type for write status command
  (ADR <<  8) |               // Address/read type for read command
  (NAR << 10) |               // Non-address/read type for read status command
  (ADW << 12) |               // Address/write type for block lock command
  (ADW << 14),                // Address/write type for block unlock command
  {
    0x06,                     // Write enable prefix (unsupported in Atmel AT26DF041)
    0x06                      // Write status enable prefix (unsupported in Atmel AT26DF041)
  },
  1,                          // 0:No global protect mechanism, 1:Global protect available
  0,                          // 0:No block protect mechanism,  1:Block protect available
  0,                          // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                          // 0:Byte program,                1:Multibytes program
  2,                          // Bytes of ID
  1,                          // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                          // Reserve
  0x0f << 2,                  // Global protect code   (Used when global protect == 1)
  0,                          // Global unprotect code (Used when global protect == 1)
  0,                          // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                          // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x800000,                   // Byte size of flash part
  0x1000,                     // Byte size of per erase
  0                           // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE ATMEL25DF641 =
{
  SpiFlashType,
  {
    ID_ATMEL_AT25DF641,       // ID
    0x0000,
    FLASH_SIZE_8192K,
    "Atmel",
    "25DF641",
    {
      0x0010,                 //size = 4k
      0x0800,                 //mutilpe = 0x800 (8192k/4k)
      0xFFFF
    },
  },
  &ATMEL25DF641_Config
};

//
// Mxic MX25L8005
//
SPI_CONFIG_BLOCK MX25L8005_Config =
{
  {
    0x9f,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (NAR <<  0) |                 // Non-address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x06                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                            // 0:Byte program,                1:Multibytes program
  3,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0x07 << 2,                    // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x100000,                     // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE MX25L8005 =
{
  SpiFlashType,
  {
    ID_MACRONIX_MX25L8005,      // ID
    0x0000,
    FLASH_SIZE_1024K,
    "Mxic",
    "25L8005",
    {
      0x0010,                   //size = 4k
      0x0100,                   //mutilpe = 0x100
      0xFFFF
    },
  },
  &MX25L8005_Config
};

//
// Mxic MX25L1605A
//
SPI_CONFIG_BLOCK MX25L1605A_Config =
{
  {
    0x9f,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (NAR <<  0) |                 // Non-address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x06                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                            // 0:Byte program,                1:Multibytes program
  3,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0x0F << 2,                    // Global protect code   (Used when global protect == 1)
  (UINT8)(~(0x0F << 2)),        // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x200000,                     // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE MX25L1605A =
{
  SpiFlashType,
  {
    ID_MACRONIX_MX25L1605A,     // ID
    0x0000,
    FLASH_SIZE_2048K,
    "Mxic",
    "25L1605A",
    {
      0x0010,                   //size = 4k
      0x0200,                   //mutilpe = 0x100
      0xFFFF
    },

  },
  &MX25L1605A_Config
};
//
// Mxic MX25L3236D
//
SPI_CONFIG_BLOCK MX25L3236D_Config =
{
  {
    0x9f,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (NAR <<  0) |                 // Non-address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x06                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                            // 0:Byte program,                1:Multibytes program
  3,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  1,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0x0F << 2,                    // Global protect code   (Used when global protect == 1)
  (UINT8)(~(0x0F << 2)),        // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x400000,                     // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE MX25L3236D =
{
  SpiFlashType,
  {
    ID_MACRONIX_MX25L3236D,     // ID
    0x0000,
    FLASH_SIZE_4096K,
    "Mxic",
    "25L3236D",
    {
      0x0010,                  //size = 4k
      0x0400,                  //mutilpe = 0x400
      0xFFFF
    },
  },
  &MX25L3236D_Config
};

//
// Mxic MX25L6436E
//
SPI_CONFIG_BLOCK MX25L6436E_Config =
{
  {
    0x9f,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (NAR <<  0) |                 // Non-address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x06                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                            // 0:Byte program,                1:Multibytes program
  3,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  1,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0x0F << 2,                    // Global protect code   (Used when global protect == 1)
  (UINT8)(~(0x0F << 2)),        // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x800000,                     // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE MX25L6436E =
{
  SpiFlashType,
  {
    ID_MACRONIX_MX25L6436E,        // ID
    0x0000,
    FLASH_SIZE_8192K,
    "Mxic",
    "25L6436E",
    {
      0x0010,                      //size = 4k
      0x0800,                      //mutilpe = 0x800
      0xFFFF
    },
  },
  &MX25L6436E_Config
};

//
// STMicro M25PE80
//
SPI_CONFIG_BLOCK M25PE80_Config =
{
  {
    0x9f,                       // Read ID command
    0xDB,                       // Erase command
    0x02,                       // Write command
    0x00,                       // Write status command (Unsupported for STMicro M25PE80)
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (NAR <<  0) |                 // Non-address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x00                        // Write status enable prefix (Unsupported for STMicro M25PE80)
  },
  0,                            // 0:No global protect mechanism, 1:Global protect available
  1,                            // 0:No block protect mechanism,  1:Block protect available
  1,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  1,                            // 0:Byte program,                1:Multibytes program
  3,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0,                            // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  1,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x100000,                     // Byte size of flash part
  0x100,                        // Byte size of per erase
  0x10000                       // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE M25PE80 =
{
  SpiFlashType,
  {
    ID_STMICRO_M25PE80,         // ID
    0x0000,
    FLASH_SIZE_1024K,
    "ST",
    "M25PE80",
    {
      0x0010,                   //size = 4k
      0x0100,                   //mutilpe = 0x100
      0xFFFF
    },
  },
  &M25PE80_Config
};

//
// PMC PM25LV040
//
SPI_CONFIG_BLOCK PM25LV040_Config =
{
  {
    0xAB,                       // Read ID command
    0xD7,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (NAR <<  0) |                 // Non-address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x06                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  0,                            // 0:Byte program,                1:Multibytes program
  2,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0x07 << 2,                    // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0x80000,                      // Byte size of flash part
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE PM25LV040 =
{
  SpiFlashType,
  {
    ID_PMC_25LV040,             // ID
    0x0000,
    FLASH_SIZE_512K,
    "PMC",
    "25LV040",
    {
      0x0010,                   //size = 4k
      0x0080,                   //mutilpe = 0x080
      0xFFFF
    },
  },
  &PM25LV040_Config
};

//
// Common Spi device (when the SPI device is not listed above, use default one)
//
SPI_CONFIG_BLOCK CommonSPI_Config =
{
  {
    0x00,                       // Read ID command
    0x20,                       // Erase command
    0x02,                       // Write command
    0x01,                       // Write status command
    0x03,                       // Read command
    0x05                        // Read status command
  },
  (NAR <<  0) |                 // Address/read type for read ID command
  (ADW <<  2) |                 // Address/write type for erase command
  (ADW <<  4) |                 // Address/write type for write command
  (NAW <<  6) |                 // Non-address/write type for write status command
  (ADR <<  8) |                 // Address/read type for read command
  (NAR << 10) |                 // Non-address/read type for read status command
  (ADW << 12) |                 // Address/write type for block lock command
  (ADW << 14),                  // Address/write type for block unlock command
  {
    0x06,                       // Write enable prefix
    0x06                        // Write status enable prefix
  },
  1,                            // 0:No global protect mechanism, 1:Global protect available
  0,                            // 0:No block protect mechanism,  1:Block protect available
  0,                            // 0:No block protect code,       1:Need block protect code for block lock/unlock
  0,                            // 0:Byte program,                1:Multibytes program
  0,                            // Bytes of ID
  1,                            // Minimum bytes per program/read
  0,                            // 1:Has non-volatile bit in status register.
  0,                            // Reserve
  0,                            // Global protect code   (Used when global protect == 1)
  0,                            // Global unprotect code (Used when global protect == 1)
  0,                            // Block protect code    (Used when block protect == 1 and block protect code == 1)
  0,                            // Block unprotect code  (Used when block protect == 1 and block protect code == 1)
  0xFFFFFFFF,                   // Byte size of flash part. This value wil be updated when RecognizeSPIDevice get called
  0x1000,                       // Byte size of per erase
  0                             // Byte size of block protect (Used when block protect == 1)
};

FLASH_DEVICE CommonSPI =
{
  SpiFlashType,
  {
    ID_COMMON_SPI_DEVICE,       // ID
    0x0000,
    0xFF,                       // This value wil be updated when RecognizeSPIDevice get called.
    "COMMONVID",
    "COMMONDID",
    {
      0x0010,                   // size = 4k
      0xFFFF,                   // mutilpe. This value wil be updated when RecognizeSPIDevice get called.
      0xFFFF
    },
  },
  &CommonSPI_Config
};


FLASH_DEVICE *gSupportedFlashDevices[] = {
  //
  // SPI Flash Devices
  //
  &SST25LF040,
  &SST25LF040A,
  &SST25LF080A,
  &SST25VF080B,
  &SST25VF016B,
  &SST26VF064B,
  &ATMEL26DF041,
  &ATMEL26DF081A,
  &ATMEL26DF161,
  &ATMEL26DF321,
  &WB25Q32BV,
  &WB25Q64BV,
  &WB25Q64FV,
  &ATMEL25DF641,
  &MX25L8005,
  &MX25L1605A,
  &MX25L3236D,
  &MX25L6436E,
  &M25PE80,
  &PM25LV040,
  &CommonSPI,

  //
  // LPC Flash Devices
  //
  &SST49LF008A,
  &SST39VF080,
  &INTEL82802AC,
  &SST49LF004A,
  &PMC49FL004T,
  &WINBOND39V040FAP,
  &ST50FLW040A

};

/**
  To retrieve total number of supported flash devices

  @param   none
  @return  Total number of supported flash devices
**/
UINTN
EFIAPI
NumberOfSupportedFlashDevices (
  VOID
)
{
  return sizeof(gSupportedFlashDevices) / sizeof(gSupportedFlashDevices[0]);
}

/**
  FlashDevicesLib Library Class Constructor

  @retval EFI_SUCCESS:   Module initialized successfully
  @retval Others     :   Module initialization failed

**/
EFI_STATUS
EFIAPI
FlashDevicesLibInit (
  VOID
  )
{
  UINTN         Index;
  EFI_STATUS    Status;
  
  Status = DetectFlashDevice (gSupportedFlashDevices, NumberOfSupportedFlashDevices());
  ASSERT_EFI_ERROR(Status);
  
  for (Index  = 0; Index < NumberOfSupportedFlashDevices(); Index++) {
     IrsiAddVirtualPointer((VOID **)&gSupportedFlashDevices[Index]->TypeSpecificInfo);
     IrsiAddVirtualPointer((VOID **)&gSupportedFlashDevices[Index]);
  }
  return EFI_SUCCESS;
}