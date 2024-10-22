/* $NoKeywords:$ */
/**
 * @file
 *
 * mnS3cz.h
 *
 * S3 resume memory related function for CZ.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/CZ)
 * @e \$Revision: 327442 $ @e \$Date: 2015-09-22 00:15:18 +0800 (Tue, 22 Sep 2015) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#ifndef _MNS3CZ_H_
#define _MNS3CZ_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
/// ID for register list of CZ
typedef enum {
  PCI_LST_ESR_CZ,  ///< Assign 0x0000 for PCI register list for pre exit self refresh.
  PCI_LST_CZ,      ///< Assign 0x0001 for PCI register list for post exist self refresh.
  CPCI_LST_ESR_CZ, ///< Assign 0x0002 for conditional PCI register list for pre exit self refresh.
  CPCI_LST_CZ,     ///< Assign 0x0003 for conditional PCI register list for post exit self refresh.
  MSR_LST_ESR_CZ,  ///< Assign 0x0004 for MSR register list for pre exit self refresh.
  MSR_LST_CZ,      ///< Assign 0x0005 for MSR register list for post exit self refresh.
  CMSR_LST_ESR_CZ, ///< Assign 0x0006 for conditional MSR register list for pre exit self refresh.
  CMSR_LST_CZ      ///< Assign 0x0007 for conditional MSR register list for post exit self refresh.
} RegisterListIDCZ;

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */
#define SET_S3_NB_PSTATE_OFFSET(Offset, NBPstate) ((NBPstate << 10) | Offset)
#define SET_S3_MEM_PSTATE_OFFSET(Offset, MemPstate) ((MemPstate << 11) | Offset)

#define S3_DATABLOB_REV_CZ 0xA

#define GNB_D0F0x60_ADDRESS   0x60          ///<  Gnb Offset index for SMU mbox
#define GNB_D0F0x64_DATA 0x64
#define GNB_IOC_WRITE_ENABLE 0x80


/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

VOID
MemNS3ConfigureDisDllShutdownSrCZ (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );
#endif //_MNS3CZ_H_

