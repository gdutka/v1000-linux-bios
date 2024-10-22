/** @file
  Header file for SMM Access Ppi.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#ifndef _SMM_ACCESS_DRIVER_H_
#define _SMM_ACCESS_DRIVER_H_

#include <PiPei.h>

#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>

#include <Guid/SmramMemoryReserve.h>

#include <Ppi/SmmAccess.h>

#define SMM_ACCESS_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('4', '5', 's', 'a')

//
// Private data
//
typedef struct {
  UINTN                 Signature;
  EFI_HANDLE            Handle;
  PEI_SMM_ACCESS_PPI    SmmAccess;
  //
  // Local Data for SMM Access interface goes here
  //
  UINTN                 NumberRegions;
  EFI_SMRAM_DESCRIPTOR  *SmramDesc;
} SMM_ACCESS_PRIVATE_DATA;

#define SMM_ACCESS_PRIVATE_DATA_FROM_THIS(a) \
  CR (a, \
      SMM_ACCESS_PRIVATE_DATA, \
      SmmAccess, \
      SMM_ACCESS_PRIVATE_DATA_SIGNATURE \
      )

#endif
