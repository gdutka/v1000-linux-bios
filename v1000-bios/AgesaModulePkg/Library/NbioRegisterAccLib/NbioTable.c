/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access PCI config space registers
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision$   @e \$Date$
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <Library/BaseLib.h>
#include  <AGESA.h>
#include  <Library/IdsLib.h>
#include  <Library/AmdBaseLib.h>
//#include  "cpuFamilyTranslation.h"
#include  <Gnb.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbLib.h>
#include  <Library/NbioTable.h>
#include  <Library/NbioRegisterAccLib.h>
#include  <Filecode.h>

#define FILECODE LIBRARY_NBIOREGISTERACCLIB_NBIOTABLE_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
STATIC
GnbProcessTableRegisterRmw (
  IN      GNB_HANDLE                      *GnbHandle,
  IN      GNB_RMW_BLOCK                   *Data,
  IN      UINT32                          Flags
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Process table
 *
 * @param[in] GnbHandle       Gnb handle
 * @param[in] Table           Table pointer
 * @param[in] Property        Property
 * @param[in] Flags           Flags
 * @param[in] StdHeader       Standard configuration header
 */

AGESA_STATUS
GnbProcessTable (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GNB_TABLE             *Table,
  IN      UINT32                Property,
  IN      UINT32                Flags,
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8                            *EntryPointer;
  UINT64                           Data;
  UINT64                           Temp;
  UINT64                           Mask;
  UINT32                           WriteAccFlags;
//  CPU_LOGICAL_ID                   LogicalId;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbProcessTableExt Enter\n");
  IDS_HDT_CONSOLE (GNB_TRACE, " Property - 0x%08x\n", Property);

//  LogicalId.Family = 0;
//  LogicalId.Revision = 0;
  EntryPointer = (UINT8 *) Table;
  WriteAccFlags = 0;
  if ((Flags & NBIO_TABLE_FLAGS_FORCE_S3_SAVE) != 0) {
    WriteAccFlags |= GNB_REG_ACC_FLAG_S3SAVE;
  }

  while (*EntryPointer != GnbEntryTerminate) {
    Data = 0;
    Temp = 0;
    switch (*EntryPointer) {
    case GnbEntryWr:
      NbioRegisterWrite (
        GnbHandle,
        ((GNB_TABLE_ENTRY_WR*) EntryPointer)->RegisterSpaceType,
        ((GNB_TABLE_ENTRY_WR*) EntryPointer)->Address,
        &((GNB_TABLE_ENTRY_WR*) EntryPointer)->Value,
        WriteAccFlags
        );
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_WR);
      break;
    case GnbEntryPropertyWr:
      if ((Property & ((GNB_TABLE_ENTRY_PROPERTY_WR *) EntryPointer)->Property) != 0) {
        NbioRegisterWrite (
          GnbHandle,
          ((GNB_TABLE_ENTRY_PROPERTY_WR *) EntryPointer)->RegisterSpaceType,
          ((GNB_TABLE_ENTRY_PROPERTY_WR *) EntryPointer)->Address,
          &((GNB_TABLE_ENTRY_PROPERTY_WR *) EntryPointer)->Value,
          WriteAccFlags
          );
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_PROPERTY_WR);
      break;
    case GnbEntryFullWr:
      if ((Property & ((GNB_TABLE_ENTRY_FULL_WR*) EntryPointer)->Property) != 0) {
//        if ((LogicalId.Revision & ((GNB_TABLE_ENTRY_FULL_WR*) EntryPointer)->Revision) != 0) {
          NbioRegisterWrite (
            GnbHandle,
            ((GNB_TABLE_ENTRY_FULL_WR*) EntryPointer)->RegisterSpaceType,
            ((GNB_TABLE_ENTRY_FULL_WR*) EntryPointer)->Address,
            &((GNB_TABLE_ENTRY_FULL_WR*) EntryPointer)->Value,
            WriteAccFlags
            );
//        }
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_FULL_WR);
      break;
    case GnbEntryRmw:
      GnbProcessTableRegisterRmw (
        GnbHandle,
        &((GNB_TABLE_ENTRY_RMW *) EntryPointer)->Data,
        WriteAccFlags
        );
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_RMW);
      break;
    case GnbEntryPropertyRmw:
      if ((Property & ((GNB_TABLE_ENTRY_PROPERTY_RMW *) EntryPointer)->Property) != 0) {
        GnbProcessTableRegisterRmw (
          GnbHandle,
          &((GNB_TABLE_ENTRY_PROPERTY_RMW *) EntryPointer)->Data,
          WriteAccFlags
          );
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_PROPERTY_RMW);
      break;
    case GnbEntryRevRmw:
//      if ((LogicalId.Revision & ((GNB_TABLE_ENTRY_REV_RMW *) EntryPointer)->Revision) != 0) {
        GnbProcessTableRegisterRmw (
          GnbHandle,
          &((GNB_TABLE_ENTRY_REV_RMW *) EntryPointer)->Data,
          WriteAccFlags
          );
//      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_REV_RMW);
      break;
    case GnbEntryFullRmw:
      if ((Property & ((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->Property) != 0) {
//        if ((LogicalId.Revision & ((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->Revision) != 0) {
          GnbProcessTableRegisterRmw (
            GnbHandle,
            &((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->Data,
            WriteAccFlags
            );
//        }
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_FULL_RMW);
      break;
    case GnbEntryPoll:
      do {
        NbioRegisterRead (
          GnbHandle,
          ((GNB_TABLE_ENTRY_POLL *) EntryPointer)->RegisterSpaceType,
          ((GNB_TABLE_ENTRY_POLL *) EntryPointer)->Address,
          &Data,
          0
          );
      } while ((Data & ((GNB_TABLE_ENTRY_POLL*) EntryPointer)->AndMask) != ((GNB_TABLE_ENTRY_POLL*) EntryPointer)->CompareValue);
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_POLL);
      break;
    case GnbEntryPropertyPoll:
      if ((Property & ((GNB_TABLE_ENTRY_PROPERTY_POLL *) EntryPointer)->Property) != 0) {
        do {
          NbioRegisterRead (
            GnbHandle,
            ((GNB_TABLE_ENTRY_PROPERTY_POLL *) EntryPointer)->RegisterSpaceType,
            ((GNB_TABLE_ENTRY_PROPERTY_POLL *) EntryPointer)->Address,
            &Data,
            0
            );
        } while ((Data & ((GNB_TABLE_ENTRY_PROPERTY_POLL *) EntryPointer)->AndMask) != ((GNB_TABLE_ENTRY_PROPERTY_POLL *) EntryPointer)->CompareValue);
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_PROPERTY_POLL);
      break;
    case GnbEntryFullPoll:
      if ((Property & ((GNB_TABLE_ENTRY_FULL_POLL *) EntryPointer)->Property) != 0) {
        do {
          NbioRegisterRead (
            GnbHandle,
            ((GNB_TABLE_ENTRY_FULL_POLL *) EntryPointer)->RegisterSpaceType,
            ((GNB_TABLE_ENTRY_FULL_POLL *) EntryPointer)->Address,
            &Data,
            0
            );
        } while ((Data & ((GNB_TABLE_ENTRY_FULL_POLL *) EntryPointer)->AndMask) != ((GNB_TABLE_ENTRY_FULL_POLL *) EntryPointer)->CompareValue);
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_FULL_POLL);
      break;
    case GnbEntryCopy:
      NbioRegisterRead (
        GnbHandle,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcRegisterSpaceType,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcAddress,
        &Data,
        0
        );
      Mask = LShiftU64 (1ull, ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcFieldWidth) - 1;
      Data = RShiftU64 (Data, ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcFieldOffset) & Mask;
      NbioRegisterRead (
        GnbHandle,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestRegisterSpaceType,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestAddress,
        &Temp,
        0
        );
      Mask = LShiftU64 (1ull, ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestFieldWidth) - 1;
      Temp = Temp & ( ~ LShiftU64 (Mask, ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestFieldOffset));
      Temp = Temp | LShiftU64 ((Data & Mask), ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestFieldOffset);
      NbioRegisterWrite (
        GnbHandle,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestRegisterSpaceType,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestAddress,
        &Temp,
        WriteAccFlags
        );
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_COPY);
      break;
    default:
      ASSERT (FALSE);
      IDS_HDT_CONSOLE (NB_MISC, "  ERROR!!! Register table parse\n");
      return AGESA_ERROR;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbProcessTableExt Exit\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Supporting function for register read modify write
 *
 * @param[in] GnbHandle                 Gnb handle
 * @param[in] Data                      Data pointer
 * @param[in] Flags                     Flags
 */

VOID
STATIC
GnbProcessTableRegisterRmw (
  IN      GNB_HANDLE                     *GnbHandle,
  IN      GNB_RMW_BLOCK                  *Data,
  IN      UINT32                         Flags
  )
{
  UINT64  Value;
  Value = 0;
  NbioRegisterRead (
    GnbHandle,
    Data->RegisterSpaceType,
    Data->Address,
    &Value,
    0
    );
  Value = (Value & (~ (UINT64) Data->AndMask)) | Data->OrMask;
  NbioRegisterWrite (
    GnbHandle,
    Data->RegisterSpaceType,
    Data->Address,
    &Value,
    Flags
    );
}

