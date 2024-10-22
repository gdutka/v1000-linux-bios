/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _SPD_DATA_READ_PPI_H_
#define _SPD_DATA_READ_PPI_H_

#define SPD_DATA_READ_PPI_GUID \
  { \
    0xd18338dc, 0x65e8, 0x4819, {0xa6, 0xf7, 0x6e, 0x71, 0xcf, 0x03, 0x86, 0x1a } \
  }

typedef struct _SPD_DATA_READ_PPI SPD_DATA_READ_PPI;


typedef
BOOLEAN
(EFIAPI *SPD_DATA_READ_ROM_PPI) (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  UINT8               SlaveAddr,
  IN  UINT8               MemChannelId,   ///< Address of SPD - memory channel ID
  IN  UINT8               DimmId,         ///< Address of SPD - DIMM ID
  IN  UINT8               *Buffer         ///< Location where to place the SPD content
  );

typedef
EFI_STATUS
(EFIAPI *SPD_DATA_READ_SMBUS_PPI) (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  UINT8               SlaveAddr,
  IN  UINT8               *Buffer         ///< Location where to place the SPD content
  );

//
// SPD_DATA_READ_PPI
//
struct _SPD_DATA_READ_PPI {
  SPD_DATA_READ_ROM_PPI    SpdDataReadRom;
  SPD_DATA_READ_SMBUS_PPI  SpdDataReadSmbus;
};

extern EFI_GUID gSpdDataReadPpiGuid;

#endif
