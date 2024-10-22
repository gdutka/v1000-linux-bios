/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */


/**** This is a platform specific reference only driver, the implemenatation depends on OEM design***/

#include <AmdCpmDxe.h>
#include <Protocol/PspPlatformProtocol.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/AmdPspBaseLibV2.h>

STATIC EFI_HANDLE mPspPlatformHandle = NULL;

STATIC PSP_PLATFORM_PROTOCOL mPspPlatformProtocol = {
  TRUE,                   //Support CPU Context Resume
  0xDD,                   //SW SMI Command for Build Context
  0x40000,                //BSP Stack Size
  0x4000,                 //AP Stack Size
  NULL                    //Address of PEI Info Structure
};

EFI_STATUS
PspPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINT64            RsmInfoLocation;
  EFI_STATUS        Status;
  TYPE_ATTRIB       TypeAttrib;
  UINT64            EntryAddress;
  UINT32            EntrySize;
  UINT64            EntryDest;
  UINT64            ResetVectorIp;
  UINT64            ResetVectorSeg;

  DEBUG((DEBUG_INFO, "OEM-DXE-PspPlatformEntryPoint-Start\n"));
  EntrySize = 0;
  EntryDest = 0;

/*Search for Resume HandOff Infomation structure*/
/*This should based on Resume SEC phase design.*/
  if (BIOSEntryInfo (BIOS_FIRMWARE, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest) == FALSE) {
    DEBUG((DEBUG_INFO, "OEM-DXE-PspPlatformEntryPoint-End-1-EFI_UNSUPPORTED\n"));
    return EFI_UNSUPPORTED;
  }
  ResetVectorSeg = EntryDest + EntrySize - 0x10000;
  ResetVectorIp = ResetVectorSeg + 0xFFF0;

  DEBUG((DEBUG_INFO, "  EntrySize =       0x%x\n", EntrySize));
  DEBUG((DEBUG_INFO, "  EntryDest =       0x%x\n", EntryDest));
  DEBUG((DEBUG_INFO, "  ResetVectorSeg =  0x%x\n", ResetVectorSeg));
  DEBUG((DEBUG_INFO, "  ResetVectorIp =   0x%x\n", ResetVectorIp));

  RsmInfoLocation = *((UINT16 *)(UINTN) (ResetVectorIp + 0x3));
  RsmInfoLocation += 0xFFF5;
  RsmInfoLocation = (RsmInfoLocation & 0xFFFF) | ResetVectorSeg;
  RsmInfoLocation -= sizeof (RSM_HANDOFF_INFO);
  mPspPlatformProtocol.SwSmiCmdtoBuildContext = PcdGet8 (PcdSwSmiCmdtoBuildContext);
  mPspPlatformProtocol.BspStackSize = PcdGet32 (PcdBspStackSize);
  mPspPlatformProtocol.ApStackSize = PcdGet32 (PcdApStackSize);
  mPspPlatformProtocol.RsmHandOffInfo = (RSM_HANDOFF_INFO *)RsmInfoLocation;

  DEBUG((DEBUG_INFO, "  mPspPlatformProtocol.CpuContextResumeEnable = %x\n", mPspPlatformProtocol.CpuContextResumeEnable ? 1:0));
  DEBUG((DEBUG_INFO, "  mPspPlatformProtocol.SwSmiCmdtoBuildContext = 0x%x\n", mPspPlatformProtocol.SwSmiCmdtoBuildContext));
  DEBUG((DEBUG_INFO, "  mPspPlatformProtocol.BspStackSize =           0x%x\n", mPspPlatformProtocol.BspStackSize));
  DEBUG((DEBUG_INFO, "  mPspPlatformProtocol.ApStackSize =            0x%x\n", mPspPlatformProtocol.ApStackSize));
  DEBUG((DEBUG_INFO, "  mPspPlatformProtocol.RsmHandOffInfo =         0x%x\n", mPspPlatformProtocol.RsmHandOffInfo));
/*===================================================*/

  //Install Protocol
  DEBUG((DEBUG_INFO, "OEM-DXE-PspPlatformEntryPoint-InstallProtocolInterface-gPspPlatformProtocolGuid\n"));
  Status = gBS->InstallProtocolInterface (
                              &mPspPlatformHandle,
                              &gPspPlatformProtocolGuid,
                              EFI_NATIVE_INTERFACE,
                              &mPspPlatformProtocol
                              );

  DEBUG((DEBUG_INFO, "OEM-DXE-PspPlatformEntryPoint-End-EFI_SUCCESS\n"));
  return EFI_SUCCESS;
}