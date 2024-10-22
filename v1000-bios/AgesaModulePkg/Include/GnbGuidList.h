/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD ALIB option API.
 *
 * Contains structures and values used to control the ALIB option code.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  OPTION
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _GNB_GUID_LIST_H_
#define _GNB_GUID_LIST_H_
#pragma pack(push, 1)
/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */
//extern EFI_GUID gGnbHobInfoGuid;

//[-start-240426-IB18160212-modify-]//
EFI_GUID GnbGuidListTable[] = {
//----------------------------------------------------------
//
  {0x3eb1d90, 0xce14, 0x40d8, {0xa6, 0xba, 0x10, 0x3a, 0x8d, 0x7b, 0xd3, 0x2d}}, // ///< Assign 0x0000    gGnbPcieHobInfoGuid
  {0xce3e2c94, 0x82e2, 0x4429, {0xba, 0x69, 0xcc, 0xf6, 0xb9, 0xba, 0x1, 0x4c}}   // ///< Assign 0x0001    gGnbNbioBaseZPHobInfoGuid
};
//[end-240426-IB18160212-modify]//






/*----------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *----------------------------------------------------------------------------------------
 */

#pragma pack(pop)
#endif  // _OPTION_GNB_H_

