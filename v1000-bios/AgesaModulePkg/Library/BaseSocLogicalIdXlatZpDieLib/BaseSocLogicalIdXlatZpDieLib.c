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
#include <Library/BaseLib.h>
#include "AGESA.h"
#include "cpuRegisters.h"
#include "SocLogicalId.h"
#include "BaseSocLogicalIdXlatZpDieLib.h"
#include "Filecode.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_BASESOCLOGICALIDXLATZPDIELIB_BASESOCLOGICALIDXLATZPDIELIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

STATIC CONST SOC_LOGICAL_REVISION_XLAT ROMDATA CcxZpLogicalRevisionTable[] =
{
  // NOTE: the highest supported stepping should be the first one
  {0x8, 0x3, AMD_REV_F17_PR_B3},
  {0x1, 0x3, AMD_REV_F17_ZP_B3},
  {0x8, 0x2, AMD_REV_F17_PR_B2},
  {0x1, 0x2, AMD_REV_F17_ZP_B2},
  {0x1, 0x1, AMD_REV_F17_ZP_B1},
  {0x1, 0x0, AMD_REV_F17_ZP_B0},
  {0x0, 0x0, AMD_REV_F17_ZP_A0},
  SOC_LOGICAL_REVISION_XLAT_TERMINATOR
};

STATIC CONST SOC_LOGICAL_FAMILY_XLAT ROMDATA CcxZpLogicalFamilyTable[] =
{
  {0x17, 0x0, AMD_FAMILY_17_ZP, CcxZpLogicalRevisionTable},  // Family 17h, Models 00h - 0Fh
  SOC_LOGICAL_FAMILY_XLAT_TERMINATOR
};

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
 * Translates the given raw ID into the appropriate logical family / logical revision
 *
 * @param[in]     RawId       Raw CPU ID to convert
 * @param[out]    LogicalId   Logical family and logical revision for the given raw ID
 *
 * @retval        TRUE        Logical ID was successfully found
 * @retval        FALSE       Family is unknown
 */
BOOLEAN
BaseSocConvertRawToLogical (
  IN       UINT32          RawId,
     OUT   SOC_LOGICAL_ID  *LogicalId
  )
{
  UINT8                      RawStepping;
  UINT8                      RawBaseModel;
  UINT8                      RawExtModel;
  UINT16                     RawFamily;
  SOC_LOGICAL_FAMILY_XLAT    *LogicalFamilyTable;
  SOC_LOGICAL_REVISION_XLAT  *LogicalRevisionTable;

  // initialize logical CPUID with UNKNOWN
  LogicalId->Family = AMD_FAMILY_UNKNOWN;
  LogicalId->Revision = AMD_REVISION_UNKNOWN;

  // get Raw CPUID
  RawFamily    = (UINT16) (((RawId & CPUID_BASE_FAMILY_MASK) >> CPUID_BASE_FAMILY_OFFSET) +
                           ((RawId & CPUID_EXT_FAMILY_MASK)  >> CPUID_EXT_FAMILY_OFFSET));
  RawBaseModel = (UINT8)   ((RawId & CPUID_BASE_MODEL_MASK)  >> CPUID_BASE_MODEL_OFFSET);
  RawExtModel  = (UINT8)   ((RawId & CPUID_EXT_MODEL_MASK)   >> CPUID_EXT_MODEL_OFFSET);
  RawStepping  = (UINT8)    (RawId & CPUID_STEPPING_MASK);

  // get logical CPUID
  LogicalFamilyTable = (SOC_LOGICAL_FAMILY_XLAT *) CcxZpLogicalFamilyTable;
  while (LogicalFamilyTable->LogicalRevisionTable != NULL) {
    if ((RawFamily == LogicalFamilyTable->RawFamily) &&
        (RawExtModel == LogicalFamilyTable->RawExtModel)) {
      LogicalRevisionTable = (SOC_LOGICAL_REVISION_XLAT *) LogicalFamilyTable->LogicalRevisionTable;  // get logical revision table
      LogicalId->Family = LogicalFamilyTable->LogicalFamily;         // get SOC_LOGICAL_ID.Family
      LogicalId->Revision = LogicalRevisionTable->LogicalRevision;     // initialize SOC_LOGICAL_ID.Revision with the first one (should be
                                                                          // the highest supported one)
      while (LogicalRevisionTable->LogicalRevision != AMD_REVISION_UNKNOWN) {
        if ((RawBaseModel == LogicalRevisionTable->RawBaseModel) &&
            (RawStepping == LogicalRevisionTable->RawStepping)) {
          LogicalId->Revision = LogicalRevisionTable->LogicalRevision;
        }
        LogicalRevisionTable++;
      }
    }
    LogicalFamilyTable++;
  }

  // check if logical family is UNKNOWN
  return ((BOOLEAN) (LogicalId->Family != AMD_FAMILY_UNKNOWN));
}

/*---------------------------------------------------------------------------------------*/
/**
 * Translates the given raw ID into the appropriate logical family / logical revision
 *
 * @param[in]     RawId       Raw CPU ID to convert
 * @param[out]    LogicalId   Logical family and logical revision for the given raw ID
 *
 * @retval        TRUE        Logical ID was successfully found
 * @retval        FALSE       Family is unknown
 */
BOOLEAN
BaseCoreConvertRawToLogical (
  IN       UINT32           RawId,
     OUT   CORE_LOGICAL_ID  *LogicalId
  )
{
  LogicalId->CoreFamily = AMD_FAMILY_UNKNOWN;
  LogicalId->CoreRevision = AMD_REVISION_UNKNOWN;

  return FALSE;
}