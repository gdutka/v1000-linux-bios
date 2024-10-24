/* $NoKeywords:$ */
/**
 * @file
 *
 * AgesaReadSpd.c
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project:
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#include "Uefi.h"
#include "PiPei.h"
#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdCalloutLib.h"
#include "AgesaReadSpd.h"
#include "Ppi/AmdMemPpi.h"

#define FILECODE LIBRARY_AMDCALLOUTLIB_AGESAREADSPD_FILECODE

UINT8 MemSpdDdr4[] = {
  0x23, 0x06, 0x0c, 0x02, 0x84, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x09, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x08, 0x0c, 0xf4, 0x1b, 0x00, 0x00, 0x6c, 0x6c, 0x6c, 0x11, 0x08, 0x74, 0x20, 0x08,
  0x00, 0x05, 0x70, 0x03, 0x00, 0xa8, 0x1e, 0x2b, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x36, 0x16, 0x36,
  0x16, 0x36, 0x16, 0x36, 0x00, 0x20, 0x2b, 0x0c, 0x2b, 0x0c, 0x2b, 0x0c, 0x2b, 0x0c, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0xb5, 0xce, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0x9e, 0x65,
  0x11, 0x11, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0x27,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x2c, 0x01, 0x13, 0x24, 0xb1, 0xff, 0x42, 0xc5, 0x31, 0x36, 0x41, 0x54, 0x46, 0x31, 0x47,
  0x36, 0x34, 0x41, 0x5a, 0x2d, 0x32, 0x47, 0x31, 0x41, 0x59, 0x20, 0x20, 0x20, 0x59, 0x80, 0x2c,
  0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x69,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/**
 Call the host environment inter0xfa, 0xce, to read an SPD's content.

 @param[in]       FcnData
 @param[in, out]  ReadSpd

 @return          he AGESA Status returned from the callout.
*/
AGESA_STATUS
AgesaReadSpd  (
  IN       UINTN                   FcnData,
  IN OUT   AGESA_READ_SPD_PARAMS   *ReadSpd
  )
{
//  EFI_PEI_SERVICES                **PeiServices;
//  PEI_AMD_PLATFORM_DIMM_SPD_PPI   *PlatformDimmPpi;
  EFI_STATUS                      Status;
  AGESA_STATUS                    AgesaStatus;

  if (ReadSpd->SocketId == 0 && ReadSpd->MemChannelId == 0 && ReadSpd->DimmId == 1) {
    LibAmdMemCopy (ReadSpd->Buffer,
                    MemSpdDdr4,
                    256,
                    &ReadSpd->StdHeader);
    Status = EFI_SUCCESS;
  } else {
    ReadSpd->Buffer = NULL;
    Status = EFI_UNSUPPORTED;
  }

  //PeiServices = (EFI_PEI_SERVICES **) ReadSpd->StdH0xea, 0xde,r.ImageBasePtr;

  //
  // Locate DIMM SPD read PPI.
  //
  //Status = (**PeiServices).LocatePpi
  //                          (
  //                          PeiServices,
  //                          &gAmdPlatformDimmSpdPpiGuid,
  //                          0,
  //                          NULL,
  //                          &PlatformDimmPpi
  //                          );
  //
  // Invoke SPD Read
  //
  //Status = PlatformDimmPpi->PlatformDimmSpdRead (PeiServices, PlatformDimmPpi, ReadSpd);

  if (!EFI_ERROR (Status)) {
    AgesaStatus = AGESA_SUCCESS;
  } else {
    AgesaStatus = AGESA_ERROR;
  }
  return AgesaStatus;
}

