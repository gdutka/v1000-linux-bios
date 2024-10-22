/* $NoKeywords:$ */
/**
 * @file
 *
 * Report used MMIO region to GCD service
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
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
#include "Porting.h"
#include "AMD.h"
#include "Filecode.h"
#include <Uefi/UefiBaseType.h>
#include <Library/FabricResourceManagerLib.h>

#define FILECODE LIBRARY_FABRICRESOURCEREPORTTOGCDNULLLIB_FABRICRESOURCEREPORTTOGCDNULLLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
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

/*---------------------------------------------------------------------------------------*/
/**
 * FabricResourceReportToGcd
 *
 * Report MMIO regions to GCD
 *
 */
EFI_STATUS
FabricResourceReportToGcd (
  )
{
  return EFI_UNSUPPORTED;
}


/*---------------------------------------------------------------------------------------*/
/**
 * ReportMmioToGcd
 *
 *
 */
EFI_STATUS
ReportMmioToGcd (
  IN       UINT64 *BaseAddress,
  IN       UINT64 Length,
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager
  )
{
  return EFI_UNSUPPORTED;
}
