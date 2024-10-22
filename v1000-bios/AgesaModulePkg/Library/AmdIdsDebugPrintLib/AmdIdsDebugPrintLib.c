/**
 * @file
 *
 * AMD Integrated Debug Print Routines
 *
 * Contains all functions related to IDS Debug Print
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Uefi.h>
#include <Pi/PiMultiPhase.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/AmdBaseLib.h>
#include <AmdIdsDebugPrintLocal.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDIDSDEBUGPRINTLIB_AMDIDSDEBUGPRINTLIB_FILECODE

extern AMD_IDS_DEBUG_PRINT_INSTANCE AmdIdsDebugPrintRedirectIoInstance;
extern AMD_IDS_DEBUG_PRINT_INSTANCE AmdIdsDebugPrintHdtoutInstance;
extern AMD_IDS_DEBUG_PRINT_INSTANCE AmdIdsDebugPrintSerialInstance;

#define MAX_LOCAL_BUFFER_SIZE   512

CONST AMD_IDS_DEBUG_PRINT_INSTANCE* mAmdIdsDebugPrintInstances[] =
{
  &AmdIdsDebugPrintRedirectIoInstance,
  &AmdIdsDebugPrintHdtoutInstance,
  &AmdIdsDebugPrintSerialInstance,
  NULL,
};
/**
 *
 *  @param[in,out] Value  - Hex value to convert to a string in Buffer.
 *
 *
 */
VOID
GetAmdIdsDebugPrintInstances (
  IN OUT   CONST AMD_IDS_DEBUG_PRINT_INSTANCE   ***pIdsAmdIdsDebugPrintInstancesPtr
  )
{
  *pIdsAmdIdsDebugPrintInstancesPtr = &mAmdIdsDebugPrintInstances[0];
}


/**
 *  Check If any print service is enabled.
 *
 *  @param[in] AmdIdsDebugPrintInstances    The Pointer to print service list
 *
 *  @retval       TRUE    At least on print service is enabled
 *  @retval       FALSE   All print service is disabled
 *
 **/
BOOLEAN
AmdIdsDebugPrintCheckSupportAny (
  IN      CONST AMD_IDS_DEBUG_PRINT_INSTANCE **AmdIdsDebugPrintInstances
  )
{
  BOOLEAN IsSupported;
  UINTN i;
  IsSupported = FALSE;
  for (i = 0; AmdIdsDebugPrintInstances[i] != NULL; i++) {
    if (AmdIdsDebugPrintInstances[i]->support ()) {
      IsSupported = TRUE;
    }
  }
  return IsSupported;
}

RETURN_STATUS
EFIAPI
AmdIdsDebugPrintLibConstructor (
  VOID
  )
{
  UINT8 i;
  if (AmdIdsDebugPrintCheckSupportAny (mAmdIdsDebugPrintInstances)) {
    //init input
    for (i = 0; mAmdIdsDebugPrintInstances[i] != NULL; i++) {
      if (mAmdIdsDebugPrintInstances[i]->support ()) {
        mAmdIdsDebugPrintInstances[i]->constructor ();
      }
    }
  }
  return RETURN_SUCCESS;
}


/**
 *  Process debug string and print to the final Physical Layer
 *
 *  @param[in] FilterFlag    - Filter Flag
 *  @param[in] *Format - format string
 *  @param[in] Marker  - Variable parameter
 *
**/
VOID
AmdIdsDebugPrintf (
  IN      UINT64      FilterFlag,
  IN      CONST CHAR8 *Format,
  IN      VA_LIST     Marker
  )
{
  CHAR8 LocalBuffer[MAX_LOCAL_BUFFER_SIZE];
  UINTN OutPutStringLen;
  UINT64 Filter;
  AMD_IDS_DEBUG_PRINT_INSTANCE **AmdIdsDebugPrintInstances;
  UINT8 i;

  if (PcdGetBool (PcdAmdIdsDebugPrintEnable) == FALSE) {
    return;
  }

  OutPutStringLen = AsciiVSPrint (LocalBuffer, sizeof (LocalBuffer), Format, Marker);

  GetAmdIdsDebugPrintInstances (
      (const AMD_IDS_DEBUG_PRINT_INSTANCE ***)&AmdIdsDebugPrintInstances);
  if (AmdIdsDebugPrintCheckSupportAny (
        (const AMD_IDS_DEBUG_PRINT_INSTANCE **)AmdIdsDebugPrintInstances)) {
    //init input
    for (i = 0; AmdIdsDebugPrintInstances[i] != NULL; i++) {
      if (AmdIdsDebugPrintInstances[i]->support ()) {
        Filter = PcdGet64 (PcdAmdIdsDebugPrintFilter);
        //Get Customize filter if implemented(Option)
        AmdIdsDebugPrintInstances[i]->customfilter (&Filter);
        if (FilterFlag & Filter) {
          //Print Physical Layer
          AmdIdsDebugPrintInstances[i]->print (&LocalBuffer[0], OutPutStringLen);
        }
      }
    }
  }
}

