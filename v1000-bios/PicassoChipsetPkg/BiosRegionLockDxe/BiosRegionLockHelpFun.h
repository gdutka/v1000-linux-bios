//;******************************************************************************
//;* Copyright (c) 1983-2017, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _BIOS_REGION_LOCK_HELP_FUN_H_
#define _BIOS_REGION_LOCK_HELP_FUN_H_

EFI_STATUS
GetBiosRegionAddress (
  IN  BIOS_REGION_TYPE   Type,
  OUT UINTN             *Base,
  OUT UINTN             *Length
  );

EFI_STATUS
AddNewRegion (
  IN BIOS_REGION_LOCK_BASE  *BiosLock,
  IN UINTN                   Base,
  IN UINTN                   Length
  );

//[-start-130828-IB02740570-remove]//
//EFI_STATUS
//ProgramRegister (
//  IN BIOS_REGION_LOCK_BASE *BiosLock
//  );
//[-end-130828-IB02740570-remove]//

BOOLEAN
GetSpiProtectLockStatus (
  VOID
  );

//[-start-130828-IB02740570-remove]//
//EFI_STATUS
//LockSpiProtectLock (
//  VOID
//  );
//[-end-130828-IB02740570-remove]//

//[-start-130828-IB02740570-add]//
EFI_STATUS
CreateS3ResumeScriptTable (
  VOID
  );
//[-end-130828-IB02740570-add]//

#endif

