/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _APCB_PSP_BOARD_ID_GETTING_METHOD_H_
#define _APCB_PSP_BOARD_ID_GETTING_METHOD_H_
//
// @APCB_START
//




///===============================================================================
// Get board ID retrieve method, then load the APCB instance based on the board ID


#define BOARD_ID_METHOD_SMBUS     1
#define BOARD_ID_METHOD_EEPROM    2
#define BOARD_ID_METHOD_GPIO      3
#define BOARD_ID_METHOD_IO        4
#define METHOD_USER_CONFIG        0xF

#define ID_FEATURE_MASK           0x80   ///< This is Bit 7 of

typedef struct {
  IN       UINT8    IdAndFeatureMask;    ///< Mask to the value read from board ID register
                                         ///< Bit 6:0 - Id Mask Bits
                                         ///< Bit7 - detemines between normal or feature controlled Instance
                                         ///< Bit7,   1 = User Controlled Feature Enabled, 0 - Normal Mode
  IN       UINT8    IdAndFeatureValue;   ///< IdValue after mask
  IN       UINT8    ApcbInstance;        ///< The corresponding APCB instance
} ID_APCB_MAPPING;

/// For IO method
typedef struct _PSP_GET_BOARD_ID_FROM_IO_STRUCT {
  IN       UINT16 AccessMethod; ///< 0: EEPROM, 1: Smbus, 2: FCH GPIO, 4: IO, 0xF: USER_CONFIG
  IN       UINT16 ConfigPortWidth; ///< Width of Config Port
  IN       UINT16 ConfigValue; ///< Value write to Configure IO Port for accessing the data
  IN       UINT32 ConfigPortAddress; ///< ConfigPort Address
  IN       UINT16 DataPortWidth; ///< DataPort Width
  IN       UINT32 DataPortAddress; ///< Data Port Address
  IN       ID_APCB_MAPPING IdApcbMapping[];
} PSP_GET_BOARD_ID_FROM_IO_STRUCT;

/// For EEPROM method
typedef struct _PSP_GET_BOARD_ID_FROM_EEPROM_STRUCT {
  IN       UINT16   AccessMethod;   ///< 0: EEPROM, 1: Smbus, 2: FCH GPIO. 0xF: USER_CONFIG
  IN       UINT16   I2cCtrlr;       ///< ZP I2C controller: 0: I2C_0, 1: I2C_1, 2: I2C_2, 3: I2C_3, 4: I2C_4, 5: I2C_5                                   ///
  IN       UINT16   SmbusAddr;      ///< Smbus address of the EEPROM
} PSP_GET_BOARD_ID_FROM_EEPROM_STRUCT;

/// For Smbus device method
typedef struct _PSP_GET_BOARD_ID_FROM_SMBUS_STRUCT {
  IN       UINT16   AccessMethod;   ///< 0: EEPROM, 1: Smbus, 2: FCH GPIO F: None
  IN       UINT16   I2cCtrlr;       ///< ZP I2C controller: 0: I2C_0, 1: I2C_1, 2: I2C_2, 3: I2C_3, 4: I2C_4, 5: I2C_5                                   ///
  IN       UINT16   SmbusAddr;      ///< Smbus address of the device to get the board ID
  IN       UINT16   RegIndex;       ///< Register index of the Smbus device to get the board ID
  IN       ID_APCB_MAPPING   IdApcbMapping[]; ///< Board ID to APCB Instance mapping
} PSP_GET_BOARD_ID_FROM_SMBUS_STRUCT;

/// For FCH GPIO method
typedef struct _PSP_GET_BOARD_ID_FROM_GPIO_STRUCT {
  IN       UINT16   AccessMethod;   ///< 0: EEPROM, 1: Smbus, 2: FCH GPIO, 0xF: USER_CONFIG
  IN       UINT8    Gpio0;          ///< FCH GPIO number of the board ID bit 0
  IN       UINT8    Gpio0IoMUX;     ///< Value write to IOMUX to configure this GPIO pin
  IN       UINT8    Gpio0BankCtl;   ///< Value write to GPIOBankCtl[23:16] to configure this GPIO pin
  IN       UINT8    Gpio1;          ///< FCH GPIO number of the board ID bit 1
  IN       UINT8    Gpio1IoMUX;     ///< Value write to IOMUX to configure this GPIO pin
  IN       UINT8    Gpio1BankCtl;   ///< Value write to GPIOBankCtl[23:16] to configure this GPIO pin
  IN       UINT8    Gpio2;          ///< FCH GPIO number of the board ID bit 2, put 0xff if does not exist
  IN       UINT8    Gpio2IoMUX;     ///< Value write to IOMUX to configure this GPIO pin
  IN       UINT8    Gpio2BankCtl;   ///< Value write to GPIOBankCtl[23:16] to configure this GPIO pin
  IN       UINT8    Gpio3;          ///< FCH GPIO number of the board ID bit 3, put 0xff if does not exist
  IN       UINT8    Gpio3IoMUX;     ///< Value write to IOMUX to configure this GPIO pin
  IN       UINT8    Gpio3BankCtl;   ///< Value write to GPIOBankCtl[23:16] to configure this GPIO pin
  IN       ID_APCB_MAPPING   IdApcbMapping[]; ///< Board ID to APCB Instance mapping
} PSP_GET_BOARD_ID_FROM_GPIO_STRUCT;

/// For User Controlled Method to Load Specific APCB Instance
typedef struct _PSP_METHOD_USER_CONTROLLED_FEATURE_STRUCT {
  IN       UINT16   AccessMethod;           ///< 0: EEPROM, 1: Smbus, 2: FCH GPIO,  0xF: USER_CONFIG
  IN       UINT16   FeatureMaskData;        ///< Data Pattern defined by user which can be used to
                                            ///< mask between normal and feature enabled Instance selection
  IN       ID_APCB_MAPPING   IdApcbMapping[]; ///< Board ID to APCB Instance mapping
} PSP_METHOD_USER_CONTROLLED_FEATURE_STRUCT;

/** PSP Board ID Getting Method
* @GROUPID: APCB_GROUP_PSP
* @TYPEID: APCB_PSP_TYPE_BOARD_ID_GETTING_METHOD
* Board Id getting methods, could be from one of below.
* EEPROM, SMBus, GPIO, IO, User Config
*/
typedef union {
  PSP_GET_BOARD_ID_FROM_EEPROM_STRUCT         MethodByEeprom;       ///< @SELECTION: AccessMethod = 0
  PSP_GET_BOARD_ID_FROM_SMBUS_STRUCT          MethodBySmbus;        ///< @SELECTION: AccessMethod = 1
  PSP_GET_BOARD_ID_FROM_GPIO_STRUCT           MethodByGpio;         ///< @SELECTION: AccessMethod = 2
  PSP_GET_BOARD_ID_FROM_IO_STRUCT             MethodByIo;           ///< @SELECTION: AccessMethod = 4
  PSP_METHOD_USER_CONTROLLED_FEATURE_STRUCT   MethodByUserFeature;  ///< @SELECTION: AccessMethod = 0xF
} PSP_BOARD_ID_GETTING_METHOD;



#endif // _APCB_PSP_BOARD_ID_GETTING_METHOD_H_

