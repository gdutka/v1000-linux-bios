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
#include <PiPei.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/AmdPspCommonServicePpi.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDPSPCOMMONLIBPEI_AMDPSPCOMMONLIBPEI_FILECODE

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
  AMD_PSP_COMMON_SERVICE_PPI **PspServicePtr
  )
{
//[-start-240426-IB18160212-modify-]//
  CONST EFI_PEI_SERVICES    **PeiServices;
  EFI_STATUS                Status;

  PeiServices = GetPeiServicesTablePointer ();
  ASSERT (PeiServices != NULL);

  if (PeiServices != NULL) {
    Status  = (*PeiServices)->LocatePpi (
              PeiServices,
              &gAmdPspCommonServicePpiGuid,
              0,
              NULL,
              (VOID**) PspServicePtr
              );
//[-end-240426-IB18160212-modify-]//
    return Status;
  }
  return EFI_UNSUPPORTED;
}

BOOLEAN
GetFtpmControlArea (
  IN OUT   VOID **FtpmControlArea
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  PspService = NULL;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return PspService->GetFtpmControlArea (FtpmControlArea);
}

VOID
SwitchPspMmioDecode (
  IN       BOOLEAN SwitchFlag,
  IN OUT   UINT32 *RegisterCopy
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  PspService = NULL;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return;
  }
  PspService->SwitchPspMmioDecode (SwitchFlag, RegisterCopy);
}

BOOLEAN
CheckPspDevicePresent (
  VOID
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  PspService = NULL;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }
  return PspService->CheckPspDevicePresent ();
}

EFI_STATUS
CheckFtpmCaps (
  IN OUT   UINT32 *Caps
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  PspService = NULL;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  return PspService->CheckFtpmCaps (Caps);
}

EFI_STATUS
CheckPspCaps (
  IN OUT   UINT32 *PspCaps
  )
{
  EFI_STATUS                  Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;
  PspService = NULL;

  Status = GetPspServicePointer (&PspService);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  return PspService->CheckPspCaps (PspCaps);
}

BOOLEAN
PSPEntryInfo (
  IN       UINT32                       EntryType,
  IN OUT   UINT64                      *EntryAddress,
  IN       UINT32                      *EntrySize
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return PspService->PSPEntryInfo (EntryType, EntryAddress, EntrySize);
}

BOOLEAN
PspLibTimeOut (
  IN       UINT64              uSec,
  IN       FP_CONDITIONER      Conditioner,
  IN       VOID                *Context
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  PspService = NULL;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return PspService->PspLibTimeOut (uSec, Conditioner, Context);

}

VOID
AcquirePspSmiRegMutex (
  VOID
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return;
  }

  PspService->AcquirePspSmiRegMutex ();
}

VOID
ReleasePspSmiRegMutex (
  VOID
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return;
  }

  PspService->ReleasePspSmiRegMutex ();
}

