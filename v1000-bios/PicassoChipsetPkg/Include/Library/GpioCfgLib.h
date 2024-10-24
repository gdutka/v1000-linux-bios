/** @file

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _GPIO_CFG_LIB_H_
#define _GPIO_CFG_LIB_H_

//
//Inclues header files from reference code
//
#include <AmdCpmCommon.h>
#include <GpioCommon.h>
#include <GpioPin.h>

#pragma pack(push, 1)

//
// Defines GPIO_INIT_CONFIG to be the same as reference code, GPIO initialization struceure
//
typedef GPIO_CONFIG H2O_GPIO_INIT_STRUCT;

#pragma pack(pop)

//
// Defines H2O_GPIO_INIT macro to initialze all elements in the structure
//
#define H2O_GPIO_INIT(a,b,c,d,e,f,g) {c, GPIO_DEFINITION (a,b,f,g,0)}
//
// Defines H2O_GET_BOARD_GPIO macro that verifies that the board is supported and currently excuting
//

#define H2O_GET_BOARD_GPIO(NAME) \
{ \
  if(Board == H2O_BOARD_ID_##NAME) { \
  *GpioInitStruct = mGpioTable##NAME; \
  *GpioInitStructCount = mGpioTable##NAME##Count; \
  return EFI_SUCCESS; \
  } \
}

//
// To get build-time GPIO configuration
//
EFI_STATUS
GetGpioCfg(
  IN      UINT8                            Board,
  OUT     H2O_GPIO_INIT_STRUCT             **GpioInitStruct,
  OUT     UINT32                           *GpioInitStructCount
  );



#endif
