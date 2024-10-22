/** @file
  Define External CMOS value.

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


#ifndef _EXTENED_CMOS_USAGE_H_
#define _EXTENED_CMOS_USAGE_H_

#define EXT_CMOS_INDEX                0x72
#define EXT_CMOS_DATA                 0x73

/*
Offset 0x80~0x8F : Customer
Offset 0x90~0xBF : Chipset
Offset 0xC0~0xD7 : Customer
Offset 0xD8~0xDF : Chipset
offset 0xE0~0xFF : Customer
*/

//CmosAmdMemoryContextSaveRestore     0x9E
//CMOS_LOCATION_OFFSET                0xA0~0xA3

#endif

