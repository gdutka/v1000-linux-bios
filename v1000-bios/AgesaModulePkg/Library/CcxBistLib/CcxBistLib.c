/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX BIST Library
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
#include <Library/CcxBistLib.h>

#define FILECODE LIBRARY_CCXBISTLIB_CCXBISTLIB_FILECODE

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
  * Reads the BIST register and if the lower 32 bits is non-zero, returns AGESA_ALERT.
  *
  * @param[in]      StdHeader              Header for library and services
  *
  * @retval         AGESA_ALERT            A BIST error has occurred
  * @retval         AGESA_SUCCESS          No BIST errors have occured
  */
AGESA_STATUS
CheckBistStatus (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64   MsrValue;
  UINT32   BistResults;

  // Make sure that Standard Header is valid
  ASSERT (StdHeader != NULL);
  //ASSERT (IsBsp (StdHeader));

  // Read MSRC001_0060 BIST Results Register
  MsrValue = AsmReadMsr64 (MSR_BIST);
  BistResults = (MsrValue & 0xFFFFFFFF);

  if (BistResults != 0) {
    return AGESA_ALERT;
  }

  return AGESA_SUCCESS;
}

