/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcSetSetupVariableDefault().
 The function PeiCsSvcSetSetupVariableDefault() use chipset services to set defaul
 variables when variable not found

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
 When variable not found, to set default variables.

 @param[in]         SystemConfiguration A pointer to setup variables

 @return            EFI_SUCCESS         Always return successfully
*/
EFI_STATUS
SetSetupVariableDefault (
  IN VOID                      *SystemConfiguration
  )
{

  return EFI_SUCCESS;
}