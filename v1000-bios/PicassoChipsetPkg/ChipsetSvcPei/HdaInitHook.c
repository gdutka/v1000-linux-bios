/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcHdaInitHook().
 The function PeiCsSvcHdaInitHook() use chipset services to provide
 a interface for chipset porting on different HDA controller.

***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

/**
 To provide a interface for chipset porting on different HDA controller.

 @param[in]         HdaBar              Base address for HDA

 @return            EFI_SUCCESS         This function always return successfully
*/
EFI_STATUS
HdaInitHook (
  IN UINT32          HdaBar
  )
{
  //
  // To add your chipset code at here.
  //
  return EFI_SUCCESS;
}