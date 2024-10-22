/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_XGBE_WORKAROUND_H_
#define _AMD_XGBE_WORKAROUND_H_

#define ENET_PORT_PROPERTY_3  0x1D00Cul

#define NUMBER_OF_ETH_PORTS 2

#define PCA9535_ADDRESS       0x20
#define PORT_I2C_MUTEX        0x1D080ul
#define INT_EN                0x1D078ul

typedef struct { 
    UINT32 Xgmac0BaseAdd;
    UINT32 Xgmac1BaseAdd;
} XGBE_DATA;


typedef struct XGBE_DATA XgbeData;

#define FCH_IOMUXx28_AGPIO40_SGPIO_DATAIN_MDIO0_SDA           0xFED80D28ul   //
#define FCH_IOMUXx28_AGPIO10_SGPIO_DATAIN_MDIO0_SDA           0xFED80D0Aul   //

#endif 
