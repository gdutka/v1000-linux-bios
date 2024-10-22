/* $NoKeywords:$ */
/**
 * @file
 *
 * CCX Initialization Complete Protocol definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_CCX_PROTOCOL_H_
#define _AMD_CCX_PROTOCOL_H_

//----------------------------------------------------
//
//  AMD CCX DXE Initialization Complete Protocol
//
//-------------------------------------------------------

// CCX init complete protocol
typedef struct _DXE_AMD_CCX_INIT_COMPLETE_PROTOCOL {
  UINTN    Revision;               ///< Revision Number
} DXE_AMD_CCX_INIT_COMPLETE_PROTOCOL;

// CCX mid init complete protocol
typedef struct _DXE_AMD_CCX_MID_INIT_COMPLETE_PROTOCOL {
  UINTN    Revision;               ///< Revision Number
} DXE_AMD_CCX_MID_INIT_COMPLETE_PROTOCOL;

// CCX late init complete protocol
typedef struct _DXE_AMD_CCX_LATE_INIT_COMPLETE_PROTOCOL {
  UINTN    Revision;               ///< Revision Number
} DXE_AMD_CCX_LATE_INIT_COMPLETE_PROTOCOL;

// CCX OC function complete protocol
typedef struct _DXE_AMD_CCX_OC_COMPLETE_PROTOCOL {
  UINTN    Revision;               ///< Revision Number
} DXE_AMD_CCX_OC_COMPLETE_PROTOCOL;

// Current Protocol revision
#define AMD_CCX_PROTOCOL_REVISION   0x00

extern EFI_GUID gAmdCcxDxeInitCompleteProtocolGuid;
extern EFI_GUID gAmdCcxDxeMidInitCompleteProtocolGuid;
extern EFI_GUID gAmdCcxDxeLateInitCompleteProtocolGuid;
extern EFI_GUID gAmdCcxOcCompleteProtocolGuid;

#endif // _AMD_CCX_PROTOCOL_H_

