/** @file

  Include file for xAPIC feature.

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

#ifndef _CPU_XAPIC_H_
#define _CPU_XAPIC_H_

#include "Cpu.h"

/**
  Sort the APIC ID of all processors.

  This function sorts the APIC ID of all processors so that processor number is assigned in the
  ascending order of APIC ID which eases MP debugging. SMBIOS logic also depends on this assumption.

**/
VOID
SortApicId (
  VOID
  );

/**
  Update collected APIC ID related info after socket IDs were reassigned.

**/
VOID
UpdateApicId (
  VOID
  );

/**
  Check if there is legacy APIC ID conflict among all processors.

  @retval EFI_SUCCESS      No coflict or conflict was resoved by force x2APIC mode
  @retval EFI_UNSUPPORTED  There is legacy APIC ID conflict and can't be rsolved in xAPIC mode
**/
EFI_STATUS
CheckApicId (
  VOID
  );

/**
  Assign a package scope BSP responsible for package scope programming.

  This functions assigns the processor with the least APIC ID within a processor package as
  the package BSP.

**/
VOID
AssignPackageBsp (
  VOID
  );

/**
  Re-program Local APIC for virtual wire mode after socket ID for the BSP has been changed.

**/
VOID
ReprogramVirtualWireMode (
  VOID
  );

#endif
