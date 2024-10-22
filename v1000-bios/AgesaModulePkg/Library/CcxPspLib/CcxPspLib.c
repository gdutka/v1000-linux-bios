/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX PSP Library
 *
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


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "Uefi.h"
#include <Library/BaseLib.h>
#include "AMD.h"
#include "Filecode.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/CcxPspLib.h>

#define FILECODE LIBRARY_CCXPSPLIB_CCXPSPLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

 /*----------------------------------------------------------------------------------------
  *                          E X P O R T E D    F U N C T I O N S
  *----------------------------------------------------------------------------------------
 */

 /*---------------------------------------------------------------------------------------*/
 /**
  *
  * Programs the Psp Address register to the desired value.
  *
  * @param[in]      PspAddr                Value written to Psp Bar address
  *
  */
VOID
UpdatePspAddr (
  IN       UINT64 PspAddr
  )
{
  AsmWriteMsr64 (0xC00110A2, PspAddr);
  PspAddr = AsmReadMsr64 (0xC00110A2);
}

/*---------------------------------------------------------------------------------------*/
 /**
  *
  * Programs the S3 Resume register with the desired value.
  *
  * @param[in]      ResumeVector            Value written to S3 Resume register
  *
  */

VOID
SetupReleaseVector (
  IN OUT   VOID *ResumeVector
  )
{
  AsmWriteMsr64 (0xC00110E0, (UINT64) ResumeVector);
  AsmWbinvd ();    // Invalid the cache to make sure DRAM contents has been updated
}

