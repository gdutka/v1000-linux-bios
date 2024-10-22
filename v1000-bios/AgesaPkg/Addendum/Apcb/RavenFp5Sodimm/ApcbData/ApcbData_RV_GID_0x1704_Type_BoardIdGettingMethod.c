/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "MyPorting.h"
#include <RV/APCB.h>
#include <ApcbCustomizedDefinitions.h>
#include <ApcbDefaults.h>

CHAR8 mDummyBuf;

APCB_TYPE_DATA_START_SIGNATURE();
// From EEPROM
//APCB_TYPE_HEADER       ApcbTypeHeader = {APCB_GROUP_PSP, APCB_PSP_TYPE_BOARD_ID_GETTING_METHOD, (sizeof(APCB_TYPE_HEADER) + sizeof (PSP_GET_BOARD_ID_FROM_EEPROM_STRUCT)), 0, 0, 0 };  // SizeOfType will be fixed up by tool
//PSP_GET_BOARD_ID_FROM_EEPROM_STRUCT  ApcbGettingMethod =
//{
//  BOARD_ID_METHOD_EEPROM,      // EEPROM
//  5,                           // ZP I2C controller
//  0xa0                         // SmbusAddr
//};

// From Smbus device
APCB_TYPE_HEADER       ApcbTypeHeader = {APCB_GROUP_PSP, APCB_PSP_TYPE_BOARD_ID_GETTING_METHOD, (sizeof(APCB_TYPE_HEADER) + sizeof (PSP_GET_BOARD_ID_FROM_SMBUS_STRUCT)), 0, 0, 0 };  // SizeOfType will be fixed up by tool
PSP_GET_BOARD_ID_FROM_SMBUS_STRUCT  BoardIdGettingMethod =
{
  BOARD_ID_METHOD_SMBUS,      // Smbus device method
  0,                          // ZP I2C controller
  0xff,                       ///< I2C Mux Address
  0xff,                       ///< Mux Control Address
  0xff,                       ///< I2C Mux Channel assocaited iwth this SPD
  0x4e,                       // Smbus address
  0,                          // register index
  {                           // Id_Apcb_Mapping
    {0x7, 0, 0},
    {0x7, 1, 1}
  }
};

// From FCH GPIO
//APCB_TYPE_HEADER       ApcbTypeHeader = {APCB_GROUP_PSP, APCB_PSP_TYPE_BOARD_ID_GETTING_METHOD, (sizeof(APCB_TYPE_HEADER) + sizeof (PSP_GET_BOARD_ID_FROM_GPIO_STRUCT)), 0, 0, 0 };  // SizeOfType will be fixed up by tool
//PSP_GET_BOARD_ID_FROM_GPIO_STRUCT  BoardIdGettingMethod =
//{
//  BOARD_ID_METHOD_GPIO,      // GPIO method
//  2,                         // Gpio0
//  0,                         // Gpio0IoMux
//  0,                         // Gpio0BankCtl
//
//  2,                         // Gpio1
//  0,                         // Gpio1IoMux
//  0,                         // Gpio1BankCtl
//
//  2,                         // Gpio2
//  0,                         // Gpio2IoMux
//  0,                         // Gpio2BankCtl
//
//  0xff,                      // Gpio3
//  0xff,                      // Gpio3IoMux
//  0xff,                      // Gpio3BankCtl
//  
//  {       // Id_Apcb_Mapping
//    {0x7, 0, 0},
//    {0x7, 1, 1}
//  }
//};

APCB_TYPE_DATA_END_SIGNATURE();

int main (IN int argc, IN char * argv [ ])
{
  return 0;
}





