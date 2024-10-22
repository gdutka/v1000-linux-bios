/** @file

  Header for the status code data hub logging component

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

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

#include <PiDxe.h>

#include <Protocol/MpService.h>

#include <Library/CpuConfigLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/AMD64ArchLib.h>

/**
  Worker function to create a new feature entry.
  
  This is a worker function to create a new feature entry. The new entry is added to
  the feature list by other functions in the library.

  @param  FeatureID   The ID of the feature.
  @param  Attribute   Feature-specific data.
  
  @return  The address of the created entry.

**/
CPU_FEATURE_ENTRY *
CreateFeatureEntry (
  IN  CPU_FEATURE_ID      FeatureID,
  IN  VOID                *Attribute
  );

/**
  Worker function to search for a feature entry in processor feature list.
  
  This is a worker function to search for a feature entry in processor feature list.

  @param  ProcessorNumber Handle number of specified logical processor
  @param  FeatureIndex    The index of the new node in feature list.
  
  @return Pointer to the feature node. If not found, NULL is returned.

**/
CPU_FEATURE_ENTRY *
SearchFeatureEntry (
  IN  UINTN               ProcessorNumber,
  IN  UINTN               FeatureIndex
  );

#endif
