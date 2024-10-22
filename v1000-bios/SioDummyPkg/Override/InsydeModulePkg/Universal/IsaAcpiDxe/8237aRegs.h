/** @file
  Declaration for DmaControllerInit.c

;******************************************************************************
;* Copyright (c) 2013 - 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _8237A_REGS_H_
#define _8237A_REGS_H_

#include <Uefi.h>

//
// Direct I/O Registers
//
#define R_8237A_DMA1_STS_CMD              0x08
#define R_8237A_DMA1_REQ                  0x09
#define R_8237A_DMA1_SINGLE_MASK          0x0A
#define R_8237A_DMA1_MODE                 0x0B
#define R_8237A_DMA1_CLEAR_BYTE_POINTER   0x0C

#define R_8237A_DMA2_STS_CMD              0xD0
#define R_8237A_DMA2_REQ                  0xD2
#define R_8237A_DMA2_SINGLE_MASK          0xD4
#define R_8237A_DMA2_MODE                 0xD6
#define R_8237A_DMA2_CLEAR_BYTE_POINTER   0xD8

//
// Bit and value definitions in Mode Register
// For Channel Select Bits, the bit position is same as in Request/Mask Register
//
#define  B_8237A_CHANNEL_SELECT           (BIT1 | BIT0)
#define   V_8237A_CHANNEL_0               0x00
#define   V_8237A_CHANNEL_1               0x01
#define   V_8237A_CHANNEL_2               0x02
#define   V_8237A_CHANNEL_3               0x03

#define  B_8237A_OPERATION                (BIT3 | BIT2)
#define  N_8237A_OPERATION                2
#define   V_8237A_VERIFY_TRANSFER         0x00
#define   V_8237A_WRITE_TRANSFER          0x01
#define   V_8237A_READ_TRANSFER           0x02

#define  B_8237A_AUTO_INIT                BIT4
#define  N_8237A_AUTO_INIT                4
#define   V_8237A_AUTO_INIT_DISABLE       0x00
#define   V_8237A_AUTO_INIT_ENABLE        0x01

#define  B_8237A_ADDRESS_DIRECTION        BIT5
#define  N_8237A_ADDRESS_DIRECTION        5
#define   V_8237A_ADDRESS_INCREMENT       0x00
#define   V_8237A_ADDRESS_DECREMENT       0x01

#define  B_8237A_MODE_SELECT              (BIT7 | BIT6)
#define  N_8237A_MODE_SELECT              6
#define   V_8237A_DEMAND_MODE             0x00
#define   V_8237A_SINGLE_MODE             0x01
#define   V_8237A_BLOCK_MODE              0x02
#define   V_8237A_CASCADE_MODE            0x03

#endif
