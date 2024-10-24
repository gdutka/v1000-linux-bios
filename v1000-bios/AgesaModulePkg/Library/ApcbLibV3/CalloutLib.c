/* $NoKeywords:$ */
/**
 * @file
 *
 * core APCB interface
 *
 * Contains core interface to manipulate APCB V2
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * AMD processors and its use is subject to the terms and conditions of the
 * AMD Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an AMD processor outside of
 * the applicable AMD product specifications will void any AMD warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and AMD disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of AMD processors.
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SmmBase2.h>
#include "CalloutLib.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          G L O B A L        V A L U E S
 *----------------------------------------------------------------------------------------
 */

static  CORE_APCB_TYPE_ENTRY       *coreApcbTypeOrigin = NULL;
static  CORE_APCB_TOKEN_ENTRY      *coreApcbTokenOrigin = NULL;

extern BOOLEAN  mInSmm;
extern BOOLEAN  mAtRuntime;
extern EFI_SMM_SYSTEM_TABLE2 *mSmst;

/*---------------------------------------------------------------------------------------*/
/**
 *      This functions retrieves the header of the link list of meta data of APCB types
 *
 *      In PEI the header can be saved in a PCD
 *      In DXE or runtime code the header can be saved in a static variable
 *
 */
VOID
customGetTypeCoreDb (
  IN        CORE_APCB_TYPE_ENTRY      **apcbTypeOrigin
  )
{
  *apcbTypeOrigin = coreApcbTypeOrigin;
}

/*---------------------------------------------------------------------------------------*/
/**
 *      This functions retrieves the header of the link list of meta data of APCB tokens
 *
 *      In PEI the header can be saved in a PCD
 *      In DXE or runtime code the header can be saved in a static variable
 *
 */
VOID
customGetTokenCoreDb (
  IN        CORE_APCB_TOKEN_ENTRY     **apcbTokenOrigin
  )
{
  *apcbTokenOrigin = coreApcbTokenOrigin;
}

/*---------------------------------------------------------------------------------------*/
/**
 *      This functions sets the header of the link list of meta data of APCB types
 *
 *      In PEI the header can be saved in a PCD
 *      In DXE or runtime code the header can be saved in a static variable
 *
 */
VOID
customSetTypeCoreDb (
  IN        CORE_APCB_TYPE_ENTRY      *apcbTypeOrigin
  )
{
  coreApcbTypeOrigin = apcbTypeOrigin;
}

/*---------------------------------------------------------------------------------------*/
/**
 *      This functions sets the header of the link list of meta data of APCB tokens
 *
 *      In PEI the header can be saved in a PCD
 *      In DXE or runtime code the header can be saved in a static variable
 *
 */
VOID
customSetTokenCoreDb (
  IN        CORE_APCB_TOKEN_ENTRY     *apcbTokenOrigin
  )
{
  coreApcbTokenOrigin = apcbTokenOrigin;
}

/*---------------------------------------------------------------------------------------*/
/**
 *      This functions needs to be implemented by the host to allocate a buffer
 *
 */
VOID *
customAlloc (
    IN          UINT32      sizeBuf
    )
{
  EFI_STATUS                Status;
  VOID                      *Buffer;
  EFI_SMM_BASE2_PROTOCOL    *SmmBase2;

  Buffer = NULL;
  mInSmm = FALSE;

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID**) &SmmBase2);
  if (!EFI_ERROR (Status)) {
    SmmBase2->InSmm (SmmBase2, &mInSmm);
  }

  if (!mInSmm) {
    Status = gBS->AllocatePool (EfiBootServicesData,
                                sizeBuf,
                                &Buffer
                                );
  } else {
    Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData,
                                     sizeBuf,
                                     &Buffer
                                    );
  }
  if (EFI_ERROR (Status)) {
    customPrint ("[APCB Lib V3] Fail to allocate a buffer\n");
  }

  return Buffer;
}

/*---------------------------------------------------------------------------------------*/
/**
 *      This functions needs to be implemented by the host to free a buffer
 *
 */
VOID
customFree (
    IN          VOID        *buffer
    )
{
  EFI_STATUS                Status;
  EFI_SMM_BASE2_PROTOCOL    *SmmBase2;

  mInSmm = FALSE;

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID**) &SmmBase2);
  if (!EFI_ERROR (Status)) {
    SmmBase2->InSmm (SmmBase2, &mInSmm);
  }

  if (!mInSmm) {
    Status = gBS->FreePool (buffer);
  } else {
    Status = mSmst->SmmFreePool (buffer);
  }
  if (EFI_ERROR (Status)) {
    customPrint ("[APCB Lib V3] Fail to free the buffer\n");
  }

  return;
}

