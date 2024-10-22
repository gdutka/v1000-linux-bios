/* $NoKeywords:$ */
/**
 * @file
 *
 * CZ specific PSP
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 326857 $   @e \$Date: 2015-09-10 15:14:39 +0800 (Thu, 10 Sep 2015) $
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

#ifndef _GNBPSPCZ_H_
#define _GNBPSPCZ_H_

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
///< PSP entry points to SCS data
#define SCS_BIN_SIGNATURE         0x5F

AGESA_STATUS
GnbGetScsDataCZ (
  IN OUT   UINT64        *EntryAddress,
  IN OUT   UINT32        *EntrySize
  );

#endif


