/** @file

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

/*++ 
  Module Name:
  
    OEMBadgingString.h
  
  Abstract:
  
    Header file for OEM badging support driver
    
--*/
#ifndef _EFI_OEM_BADGING_STRING_H
#define _EFI_OEM_BADGING_STRING_H

#include <Library/BadgingSupportLib.h>
#include <Library/PcdLib.h>

EFI_STATUS
GetId(
  IN OUT UINTN    *CpuId
);

BOOLEAN
GetCpuId(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
);

#define BADGING_STRINGS_COUNT                      0//1
#define BADGING_AFTER_SELECT_COUNT                 2
#define BADGING_STRINGS_COUNT_TEXT_MODE                      2
#define BADGING_STRINGS_INCLUDE_ME_INFORM_COUNT_TEXT_MODE    3
#define OEM_STRING_LOCATION_X       50
#define OEM_STRING_GRAPHICAL_LOCATION_X                (UINTN) -1
#define OEM_STRING_GRAPHICAL_ESC_SELECT_LOCATION_X     (UINTN) -1
#define OEM_STRING_GRAPHICAL_LOCATION_Y                (UINTN) -1
#define OEM_STRING1_LOCATION_Y      500
#define OEM_STRING2_LOCATION_Y      575
#endif
