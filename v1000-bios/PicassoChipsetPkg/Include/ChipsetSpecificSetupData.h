/** @file
  Chipset Setup Configuration Data

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifdef _IMPORT_CHIPSET_SPECIFIC_SETUP_

//==========================================================================================================
//==========================================================================================================
//----------------------------------------------------------------------------------------------------------
// Start area for Platform. The following area is used by Chipset team to modify.
// The total size of variable in this part are fixed (500bytes). That means if you need to add or remove
// variables, please modify the PlatformRSV buffer size as well.
//----------------------------------------------------------------------------------------------------------
//Platform_Start
//Offset(700);

  UINT8         PlatformRSV[500];                      //Offset 700 - 1199
//Platform_End
//----------------------------------------------------------------------------------------------------------
// End of area for Chipset team use.
//----------------------------------------------------------------------------------------------------------

#endif