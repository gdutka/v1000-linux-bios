/** @file
 The Header file of the SMM Access2 Driver.

***************************************************************************
* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#ifndef __SMM_ACCESS2_H__
#define __SMM_ACCESS2_H__

#include <PiDxe.h>

#include <Protocol/MpService.h>
#include <Protocol/SmmAccess2.h>

#include <Guid/SmramMemoryReserve.h>

#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <AGESA.h>
#include <PostCode.h>

//
// SmmAccess2 protocol instance signature definition.
//
#define EFI_SMM_ACCESS2_PRIVATE_SIGNATURE SIGNATURE_32 ('4', '5', 's', 'a')

//
// SmmAccess2 protocol instance data structure
//
typedef struct {
  UINTN                                 Signature;
  EFI_HANDLE                            Handle;
  EFI_SMM_ACCESS2_PROTOCOL              SmmAccess2;
  //
  // Local Data for SMM Access2 interface goes here
  //
  EFI_MP_SERVICES_PROTOCOL              *MpService;
  UINTN                                 NumberRegions;
  EFI_SMRAM_DESCRIPTOR                  *SmramDesc;
} EFI_SMM_ACCESS2_PRIVATE;

//
// SmmAccess2 protocol instance containing record macro
//
#define EFI_SMM_ACCESS2_PRIVATE_FROM_THIS(a) \
  CR (a, \
      EFI_SMM_ACCESS2_PRIVATE, \
      SmmAccess2, \
      EFI_SMM_ACCESS2_PRIVATE_SIGNATURE \
      );

#endif