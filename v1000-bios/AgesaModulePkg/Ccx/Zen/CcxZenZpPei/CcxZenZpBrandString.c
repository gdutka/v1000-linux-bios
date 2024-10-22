/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX brand string
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Ccx
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Filecode.h"
#include "cpuRegisters.h"
#include "Library/PeiServicesTablePointerLib.h"
#include "Ppi/NbioSmuServicesPpi.h"
#include <Library/CcxBaseX86Lib.h>
#include <Library/IdsLib.h>
#include "PiPei.h"

#define FILECODE CCX_ZEN_CCXZENZPPEI_CCXZENZPBRANDSTRING_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// A structure containing brand string
typedef struct {
  CONST CHAR8 *Stringstart;  ///< The literal string
} CPU_F17_ZP_EXCEPTION_BRAND;

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// A structure containing brand string
typedef struct {
  CONST CHAR8 *Stringstart;  ///< The literal string
} CPU_F17_EXCEPTION_BRAND;

// This is an example, need to be updated once Processor Revision Guide define brand string exception
// Brand string is always 48 bytes
//CONST CHAR8 ROMDATA str_Exception_0[48] = "AMD PRO A12-8800B R7, 12 Compute Cores 4C+8G";
CONST CHAR8 ROMDATA str_Unprogrammed_Sample[48] = "AMD Unprogrammed Engineering Sample";

CONST CPU_F17_EXCEPTION_BRAND ROMDATA CpuF17ExceptionBrandIdString[] =
{
//  str_Exception_0
  NULL
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
STATIC
IsException (
     OUT   UINT32                 *ExceptionId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
* Set the Processor Name String register based on fuses.
*
*  @param[in]   StdHeader           Config handle for library and services.
*
*/
VOID
CcxZenZpSetBrandString (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32                         ExceptionId;
  UINT32                         MsrIndex;
  UINT64                         *MsrNameStringPtrPtr;
  EFI_STATUS                     Status;
  EFI_PEI_SERVICES               **PeiServices;
  PEI_AMD_NBIO_SMU_SERVICES_PPI  *GnbSmuService;
  UINT64                         BrandString[6];

  BrandString[5] = 0;
  MsrNameStringPtrPtr = (UINT64 *) str_Unprogrammed_Sample;
  if (IsException (&ExceptionId, StdHeader)) {
    ASSERT (ExceptionId < (sizeof (CpuF17ExceptionBrandIdString) / sizeof (CpuF17ExceptionBrandIdString[0])));
    MsrNameStringPtrPtr = (UINT64 *) CpuF17ExceptionBrandIdString[ExceptionId].Stringstart;
  } else {
    // Normally, we get Name String from the SMU
    IDS_HDT_CONSOLE (CPU_TRACE, "    Set brand string");
    PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
    Status = (**PeiServices).LocatePpi (
              PeiServices,
              &gAmdNbioSmuServicesPpiGuid,
              0,
              NULL,
              &GnbSmuService
              );
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE (CPU_TRACE, "    Can not locate SMU PPI");
    } else if (GnbSmuService->SmuReadBrandString (GnbSmuService, 0, ((sizeof (BrandString)) - 1), (UINT8 *) &BrandString[0]) != EFI_SUCCESS) {
      IDS_HDT_CONSOLE (CPU_TRACE, "    SMU PPI returned an error");
    } else {
      MsrNameStringPtrPtr = &BrandString[0];
    }
  }
  // Put values into name MSRs,  Always write the full 48 bytes
  for (MsrIndex = MSR_CPUID_NAME_STRING0; MsrIndex <= MSR_CPUID_NAME_STRING5; MsrIndex++) {
    AsmWriteMsr64 (MsrIndex, *MsrNameStringPtrPtr);
    MsrNameStringPtrPtr++;
  }
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
* Check if it's an exception
*
* For family 17h, brand string is obtained from D0F0xBC_xD021_108C, but there may be exceptions.
* This function checks if it's an exception.
*
*  @param[out]  ExceptionId         Id of exception
*  @param[in]   StdHeader           Config handle for library and services.
*
*  @retval      TRUE                It's an exception
*  @retval      FALSE               It's NOT an exception
*/
BOOLEAN
STATIC
IsException (
     OUT   UINT32             *ExceptionId,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  *ExceptionId = 0xFFFF;
  return FALSE;
}

