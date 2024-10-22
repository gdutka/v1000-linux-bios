/** @file
 The Header file of the AMD Fch SMM Library.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#ifndef _FCH_SMM_LIB_INTERNAL_H_
#define _FCH_SMM_LIB_INTERNAL_H_

#include <Base.h>

#include <Library/SmmLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include <Fch.h>

#define BOOT_SERVICE_SOFTWARE_SMI_DATA          0
#define RUNTIME_SOFTWARE_SMI_DATA               1

#define BASE_TICKS              (3.579545)

#endif
