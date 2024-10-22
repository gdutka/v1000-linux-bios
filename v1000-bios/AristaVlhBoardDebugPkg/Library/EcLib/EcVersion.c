//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   EcVersion.c
//;
//; Abstract:
//;

//#include "Tiano.h"
//#include "EfiCommonLib.h"
//#include "EcLib.h"
//#include "IoAccess.h"
#include "SharedEcLib.h"

/*-----------------------------------------------------------------------------*/
/**
*  EcVersion
*
*  Description:
*    This is the stub function to get EC version
*    by command 0xAC
*
*  Parameters:
*    @param[in]      None
*    @param[in/out]  *EcVersion
*                    Format: x x x x . x x (x: 0 ~ 9)
*
*    @retval         EFI_SUCESS, EFI_ERROR()*
**/
/*-----------------------------------------------------------------------------*/
EFI_STATUS
EcVersion(
  IN OUT  CHAR16    *EcVersion
  )
{
  UINT8 Value8;
  UINT8 Data8;
  UINT8 TempVersion;
  
  Value8 = 0;
  Data8 = 0;
  TempVersion = 0;
  
  for (Value8=0;Value8<5;Value8++) {
    Data8 = TIEC_VERSION + Value8;
    TempVersion = EcRead(Data8);
    *EcVersion = (CHAR16) TempVersion; 
    EcVersion++;
  }  
  
  return EFI_SUCCESS;
}

EFI_STATUS
EcLibVersion(
  IN OUT  CHAR16    *EcLib_Version
  )
{
  UINT8 Value8;
  UINT8 Data8;
  UINT8 TempVersion;
  
  Value8 = 0;
  Data8 = 0;
  TempVersion = 0;
  
  for (Value8=0;Value8<5;Value8++) {
    Data8 = TIEC_LIB_VERSION + Value8;
    TempVersion = EcRead(Data8);
    *EcLib_Version = (CHAR16) TempVersion; 
    EcLib_Version++;
  }  
  
  return EFI_SUCCESS;
}