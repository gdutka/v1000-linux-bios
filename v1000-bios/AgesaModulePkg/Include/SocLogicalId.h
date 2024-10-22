/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _SOC_LOGICAL_ID_H_
#define _SOC_LOGICAL_ID_H_
#pragma pack (push, 1)

/*
 *--------------------------------------------------------------
 *
 *                      M O D U L E S    U S E D
 *
 *---------------------------------------------------------------
 */

/*
 *--------------------------------------------------------------
 *
 *                      D E F I N I T I O N S  /  M A C R O S
 *
 *---------------------------------------------------------------
 */
/// SOC logical ID structure
typedef struct _SOC_LOGICAL_ID {
  IN OUT   UINT32 Family;             ///< Indicates logical ID Family
  IN OUT   UINT16 Revision;           ///< Indicates logical ID Revision
} SOC_LOGICAL_ID;

/// Core logical ID structure
typedef struct _CORE_LOGICAL_ID {
  IN OUT   UINT32 CoreFamily;         ///< Indicates logical ID Core (silicon) Family
  IN OUT   UINT16 CoreRevision;       ///< Indicates logical ID Core (silicon) Revision
} CORE_LOGICAL_ID;

// UNKNOWN equates
// -------------------------------------
// This define should be equal to the total number of families
// in the cpuFamily enum.
#define MAX_CPU_FAMILIES    31
#define MAX_CPU_REVISIONS   15    // Max Cpu Revisions Per Family
// Family Unknown
#define AMD_FAMILY_UNKNOWN   0x80000000ul
#define AMD_REVISION_UNKNOWN 0x8000u

// --------------------------------------------------------------------------
//   S O C   F A M I L Y & R E V I S I O N S
// --------------------------------------------------------------------------

// SOC_LOGICAL_ID.Family equates
// -------------------------------------

// Family 15h CZ equates
#define AMD_FAMILY_15_CZ  0x00000001ul
#define AMD_FAMILY_CZ     (AMD_FAMILY_15_CZ)

#define AMD_FAMILY_15     (AMD_FAMILY_15_CZ)

// Family 17h ZP equates
#define AMD_FAMILY_17_ZP  0x00000002ul
#define AMD_FAMILY_ZP     (AMD_FAMILY_17_ZP)

// Family 17h RV equates
#define AMD_FAMILY_17_RV  0x00000004ul
#define AMD_FAMILY_RV     (AMD_FAMILY_17_RV)

// Family 17h RV2 equates
#define AMD_FAMILY_17_RV2 0x00000008ul
#define AMD_FAMILY_RV2    (AMD_FAMILY_17_RV2)

// Family 17h SSP equates
#define AMD_FAMILY_17_SSP 0x00000010ul
#define AMD_FAMILY_SSP    (AMD_FAMILY_17_SSP)

// Family 17h PIC equates
#define AMD_FAMILY_17_PIC 0x00000020ul
#define AMD_FAMILY_PIC    (AMD_FAMILY_17_PIC)

// Family 17h MTS equates
#define AMD_FAMILY_17_MTS 0x0000004ul
#define AMD_FAMILY_MTS    (AMD_FAMILY_17_MTS)

#define AMD_FAMILY_17     (AMD_FAMILY_ZP | AMD_FAMILY_RV | AMD_FAMILY_RV2 | AMD_FAMILY_PIC | AMD_FAMILY_SSP | AMD_FAMILY_MTS)

// Family 15h SOC_LOGICAL_ID.Revision equates
// -------------------------------------
  // Family 15h CZ steppings
#define AMD_REV_F15_CZ_A0    0x0001u
#define AMD_REV_F15_CZ_A1    0x0002u
#define AMD_REV_F15_BR_A1    0x0004u

#define AMD_REV_F15_BR_Ax    (AMD_REV_F15_BR_A1)
#define AMD_REV_F15_BR_ALL   (AMD_REV_F15_BR_Ax)

#define AMD_REV_F15_CZ_Ax    (AMD_REV_F15_CZ_A0 | AMD_REV_F15_CZ_A1 | AMD_REV_F15_BR_Ax)
#define AMD_REV_F15_CZ_ALL   (AMD_REV_F15_CZ_Ax | AMD_REV_F15_BR_ALL)

#define AMD_REV_F15_ALL      (AMD_REV_F15_CZ_ALL)

// Family 17h SOC_LOGICAL_ID.Revision equates
// -------------------------------------
  // Family 17h ZP core steppings
#define AMD_REV_F17_ZP_A0    0x0001u
#define AMD_REV_F17_ZP_B0    0x0002u
#define AMD_REV_F17_ZP_B1    0x0004u
#define AMD_REV_F17_ZP_B2    0x0008u
#define AMD_REV_F17_PR_B2    0x0010u
#define AMD_REV_F17_PR_B3    0x0020u
#define AMD_REV_F17_ZP_B3    0x0040u

#define AMD_REV_F17_ZP_Ax    (AMD_REV_F17_ZP_A0)
#define AMD_REV_F17_ZP_Bx    (AMD_REV_F17_ZP_B0 | AMD_REV_F17_ZP_B1 | AMD_REV_F17_ZP_B2 | AMD_REV_F17_ZP_B3)
#define AMD_REV_F17_PR_Bx    (AMD_REV_F17_PR_B2 | AMD_REV_F17_PR_B3)
#define AMD_REV_F17_ZP_PR_Bx (AMD_REV_F17_ZP_Bx | AMD_REV_F17_PR_Bx)

#define AMD_REV_F17_ZP_ALL   (AMD_REV_F17_ZP_Ax | AMD_REV_F17_ZP_Bx)
#define AMD_REV_F17_PR_ALL   (AMD_REV_F17_PR_Bx)
#define AMD_REV_F17_ZP_PR_ALL (AMD_REV_F17_ZP_ALL | AMD_REV_F17_PR_ALL)

  // Family 17h ZP derived steppings
#define AMD_REV_F17_ZP_LT_B1  (AMD_REV_F17_ZP_Ax | AMD_REV_F17_ZP_B0)
#define AMD_REV_F17_ZP_GTE_B1 (AMD_REV_F17_ZP_ALL & ~AMD_REV_F17_ZP_LT_B1)
#define AMD_REV_F17_ZP_LTE_B1 (AMD_REV_F17_ZP_Ax | AMD_REV_F17_ZP_B0 | AMD_REV_F17_ZP_B1)
#define AMD_REV_F17_ZP_PR_GTE_B1 ((AMD_REV_F17_ZP_ALL & ~AMD_REV_F17_ZP_LT_B1) | AMD_REV_F17_PR_ALL)

#define AMD_REV_F17_ZP_LT_B2  (AMD_REV_F17_ZP_Ax | AMD_REV_F17_ZP_B0 | AMD_REV_F17_ZP_B1)
#define AMD_REV_F17_ZP_GTE_B2 (AMD_REV_F17_ZP_ALL & ~AMD_REV_F17_ZP_LT_B2)
#define AMD_REV_F17_ZP_PR_GTE_B2 ((AMD_REV_F17_ZP_ALL & ~AMD_REV_F17_ZP_LT_B2) | AMD_REV_F17_PR_ALL)

  // Family 17h RV steppings
#define AMD_REV_F17_RV_A0    0x0001u
#define AMD_REV_F17_RV_B0    0x0002u

#define AMD_REV_F17_RV_Ax    (AMD_REV_F17_RV_A0)
#define AMD_REV_F17_RV_Bx    (AMD_REV_F17_RV_B0)

#define AMD_REV_F17_RV_LT_B0  (AMD_REV_F17_RV_Ax)
#define AMD_REV_F17_RV_GTE_B0 (AMD_REV_F17_RV_ALL & ~AMD_REV_F17_RV_LT_B0)

#define AMD_REV_F17_RV_ALL   (AMD_REV_F17_RV_Ax | AMD_REV_F17_RV_Bx)

  // Family 17h RV2 steppings
#define AMD_REV_F17_RV2_A0   0x0001u
#define AMD_REV_F17_RV2_A1   0x0002u

#define AMD_REV_F17_RV2_Ax   (AMD_REV_F17_RV2_A0 | AMD_REV_F17_RV2_A1)

#define AMD_REV_F17_RV2_ALL  (AMD_REV_F17_RV2_Ax)

  // Family 17h SSP steppings
#define AMD_REV_F17_SSP_A0   0x0001u
#define AMD_REV_F17_SSP_B0   0x0010u

#define AMD_REV_F17_SSP_Ax   (AMD_REV_F17_SSP_A0)
#define AMD_REV_F17_SSP_Bx   (AMD_REV_F17_SSP_B0)

#define AMD_REV_F17_SSP_ALL  (AMD_REV_F17_SSP_Ax | AMD_REV_F17_SSP_Bx)

#define AMD_REV_F17_SSP_GTE_B0 (AMD_REV_F17_SSP_ALL & ~AMD_REV_F17_SSP_Ax)

  // Family 17h PIC steppings
#define AMD_REV_F17_PIC_B0   0x0001u
#define AMD_REV_F17_PIC_B1   0x0002u

#define AMD_REV_F17_PIC_Bx   (AMD_REV_F17_PIC_B0 | AMD_REV_F17_PIC_B1)

#define AMD_REV_F17_PIC_GTE_B0 (AMD_REV_F17_PIC_Bx)

#define AMD_REV_F17_PIC_ALL  (AMD_REV_F17_PIC_Bx)

  // Family 17h MTS steppings
#define AMD_REV_F17_MTS_A0   0x0001u

#define AMD_REV_F17_MTS_Ax   (AMD_REV_F17_MTS_A0)

#define AMD_REV_F17_MTS_ALL  (AMD_REV_F17_MTS_Ax)

// --------------------------------------------------------------------------
//   C O R E   F A M I L Y & R E V I S I O N S
// --------------------------------------------------------------------------

// -------------------------------------
// CORE_LOGICAL_ID.CoreFamily equates
// -------------------------------------
  // Core Family 17h VH equates
#define AMD_CORE_FAMILY_17_VH  0x00000001ul
#define AMD_CORE_FAMILY_VH     (AMD_CORE_FAMILY_17_VH)


// -------------------------------------
// CORE_LOGICAL_ID.LogicalCoreRev equates
// -------------------------------------
  // Family 17h VH core steppings
#define AMD_CORE_F17_VH_A0   0x0001u
#define AMD_CORE_F17_VH_B0   0x0010u

#define AMD_CORE_F17_VH_Ax   (AMD_CORE_F17_VH_A0)
#define AMD_CORE_F17_VH_Bx   (AMD_CORE_F17_VH_B0)

#define AMD_CORE_F17_VH_ALL  (AMD_CORE_F17_VH_Ax | AMD_CORE_F17_VH_Bx)

#define AMD_CORE_F17_VH_GTE_B0 (AMD_CORE_F17_VH_ALL & ~AMD_CORE_F17_VH_Ax)

  // All
#define AMD_REV_F17_ALL      (AMD_REV_F17_ZP_ALL | AMD_REV_F17_RV_ALL | AMD_REV_F17_RV2_ALL | AMD_REV_F17_SSP_ALL | AMD_REV_F17_PIC_ALL | AMD_REV_F17_MTS_ALL)

#pragma pack (pop)
#endif      // _SOC_LOGICAL_ID_H_


