/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Aoac operation lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 312516 $   @e \$Date: 2015-02-06 17:37:45 -0800 (Fri, 06 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/

#ifndef _FCH_AOACLIB_H_
#define _FCH_AOACLIB_H_


#define FCH_AOAC_CLKGEN       0x00
#define FCH_AOAC_AB           0x01
#define FCH_AOAC_LPC          0x04
#define FCH_AOAC_I2C0         0x05
#define FCH_AOAC_I2C1         0x06
#define FCH_AOAC_I2C2         0x07
#define FCH_AOAC_I2C3         0x08
#define FCH_AOAC_I2C4         0x09
#define FCH_AOAC_I2C5         0x0A
#define FCH_AOAC_UART0        0x0B
#define FCH_AOAC_UART1        0x0C
#define FCH_AOAC_UART2        0x10
#define FCH_AOAC_AMBA         0x11
#define FCH_AOAC_SD0          0x18
#define FCH_AOAC_SD1          0x19
#define FCH_AOAC_UART3        0x1A
#define FCH_AOAC_ESPI         0x1B
#define FCH_AOAC_EMMC         0x1C
#define FCH_AOAC_IMC          0x1D



VOID
FchAoacPowerOnDev (
  IN  UINT8      Device,
  IN  UINT8      On
);

#endif
