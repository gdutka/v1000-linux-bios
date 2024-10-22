/** @file
  Definitions for SPI access Library Instance

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

#ifndef _SPI_ACCESS_H_
#define _SPI_ACCESS_H_

//
// Operation index
//
#define SPI_OPCODE_INDEX_READ_ID        0
#define SPI_OPCODE_INDEX_ERASE          1
#define SPI_OPCODE_INDEX_WRITE          2
#define SPI_OPCODE_INDEX_WRITE_S        3
#define SPI_OPCODE_INDEX_READ           4
#define SPI_OPCODE_INDEX_READ_S         5
#define SPI_OPCODE_INDEX_LOCK           6
#define SPI_OPCODE_INDEX_UNLOCK         7
#define SPI_PREFIX_INDEX_WRITE_EN       0
#define SPI_PREFIX_INDEX_WRITE_S_EN     1

//
// Block lock bit definitions
//
#define SPI_WRITE_LOCK                  0x01
#define SPI_FULL_ACCESS                 0x00

#define MAX_SPI_WIRTE_BYTES_PER_TIME    64
#define MAX_SPI_FIFO_SIZE  (MAX_SPI_WIRTE_BYTES_PER_TIME + 3) // CZ - SPIx[C6:80] FIFO[70:0]
#endif
