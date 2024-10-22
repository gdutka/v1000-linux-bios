/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_XGBE_AND_DISPLAY_CONFIGURE_H_
#define _AMD_XGBE_AND_DISPLAY_CONFIGURE_H_

#define PCA9535_ADDRESS       0x21

#define I2C2_MMIO_ADDR 0xFEDC4000  //I2C2 maps to SMN space as D4000 - D4FFF and SDP address as FEDC4000 - FEDC4FFF

#define PCA9535_I2C_BASE I2C2_MMIO_ADDR

typedef enum { 
 HDMI_DISPLAY = 0,
 DP_DISPLAY = 1,
} DisplayType;

#endif
