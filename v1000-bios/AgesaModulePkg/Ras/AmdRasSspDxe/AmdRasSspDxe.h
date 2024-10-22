/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_RAS_SSP_DXE_H_
#define _AMD_RAS_SSP_DXE_H_

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern EFI_GUID gAmdRasInitDataProtocolGuid;

//
// Functions Prototypes
//
EFI_STATUS
EFIAPI
AmdRasZpDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );


#endif // _AMD_RAS_SSP_DXE_H_

