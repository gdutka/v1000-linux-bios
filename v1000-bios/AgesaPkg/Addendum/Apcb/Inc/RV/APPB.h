/* $NoKeywords:$ */
/**
 * @file
 *
 * APCB.h
 *
 * AGESA PSP PMU Block
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 192403 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#ifndef _APPB_H_
#define _APPB_H_
//#include "psp_fw_image.h"

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
///===============================================================================
/// _PSP_PMU_STRUCT
/// This data structure is used to pass wrapper parameters to the memory configuration code
///
typedef struct _PSP_PMU_STRUCT {
  PMU_FIRMWARE PmuFirmwareImage;              ///< PMU Firmware Image
} PSP_PMU_STRUCT;

// AGESA PSP PMU Signature.
#define APPB_SIGNATURE           {'!', '!', 'B', 'P', 'P', 'A'}   ///< APPB Signature with 6 characters limited.
#define APPB_VERSION     0x0001  // APPB Version
/// AMD APCB_HEADER Header
typedef struct {
  CHAR8   Signature[6];                       ///< APPB signature
  CHAR8   TerminatorNull;                     ///< null terminated string
  CHAR8   TerminatorNull1;                    ///< null terminated string
  UINT32   Version;                           ///< Version
  UINT32   Family;                            ///< Family
  UINT32   DddrType;                          ///< DDR Type
} APPB_HEADER;

///===============================================================================
/// AGESA PSP PMU BLOCK
/// This data structure is used to pass PMU information to the PSP
///
typedef struct _AGESA_PSP_PMU_BLOCK_STRUCT {
  PSP_FW_IMAGE_HEADER     PspFwImageHeader;       ///< PSP FW Image Header
  IN APPB_HEADER          Header;                 ///< APPB Header
  IN PSP_PMU_STRUCT       memPspPmu;              ///< PMU Image Buffer
} AGESA_PSP_PMU_BLOCK_STRUCT;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

AGESA_STATUS
VerifyAppb (
  IN OUT   UINT32 AgesaAppbVersion,
  IN OUT   UINT32 BiosAppbVersion,
  IN       UINT32 Family,
  IN       UINT32 DDRType,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#endif  /* _APPB_H_ */



