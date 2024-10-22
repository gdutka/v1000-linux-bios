/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP V1 Base Library
 *
 * Contains interface to the PSP library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 317842 $   @e \$Date: 2015-04-29 15:06:52 +0800 (Wed, 29 Apr 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmdPspCommonServiceProtocol.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDPSPCOMMONLIBDXE_AMDPSPCOMMONLIBDXE_FILECODE

AMD_PSP_COMMON_SERVICE_PROTOCOL *mPspServicePtr = NULL;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
GetPspServicePointer (
  )
{
  EFI_STATUS Status;
  //[-start-240426-IB18160212-modify-]//
  if (mPspServicePtr == NULL) {
    Status = gBS->LocateProtocol (&gAmdPspCommonServiceProtocolGuid, NULL, (VOID**)&mPspServicePtr);
    if (EFI_ERROR (Status)) {
      ASSERT (FALSE);
      return Status;
    }
  }
  //[-end-240426-IB18160212-modify-]//
  return EFI_SUCCESS;
}

BOOLEAN
GetFtpmControlArea (
  IN OUT   VOID **FtpmControlArea
  )
{
  EFI_STATUS Status;

  Status = GetPspServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mPspServicePtr->GetFtpmControlArea (FtpmControlArea);
}

VOID
SwitchPspMmioDecode (
  IN       BOOLEAN SwitchFlag,
  IN OUT   UINT32 *RegisterCopy
  )
{
  EFI_STATUS Status;

  Status = GetPspServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return;
  }
  mPspServicePtr->SwitchPspMmioDecode (SwitchFlag, RegisterCopy);
}

BOOLEAN
CheckPspDevicePresent (
  VOID
  )
{
  EFI_STATUS Status;

  Status = GetPspServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }
  return mPspServicePtr->CheckPspDevicePresent ();
}

EFI_STATUS
CheckFtpmCaps (
  IN OUT   UINT32 *Caps
  )
{
  EFI_STATUS Status;

  Status = GetPspServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  return mPspServicePtr->CheckFtpmCaps (Caps);
}

BOOLEAN
PSPEntryInfo (
  IN       UINT32                       EntryType,
  IN OUT   UINT64                      *EntryAddress,
  IN       UINT32                      *EntrySize
  )
{
  EFI_STATUS Status;

  Status = GetPspServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mPspServicePtr->PSPEntryInfo (EntryType, EntryAddress, EntrySize);
}

BOOLEAN
PspLibTimeOut (
  IN       UINT64              uSec,
  IN       FP_CONDITIONER      Conditioner,
  IN       VOID                *Context
  )
{
  EFI_STATUS Status;

  Status = GetPspServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return mPspServicePtr->PspLibTimeOut (uSec, Conditioner, Context);

}

VOID
AcquirePspSmiRegMutex (
  VOID
  )
{
  EFI_STATUS Status;

  Status = GetPspServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return;
  }

  mPspServicePtr->AcquirePspSmiRegMutex ();
}

VOID
ReleasePspSmiRegMutex (
  VOID
  )
{
  EFI_STATUS Status;

  Status = GetPspServicePointer ();

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return;
  }

  mPspServicePtr->ReleasePspSmiRegMutex ();
}

