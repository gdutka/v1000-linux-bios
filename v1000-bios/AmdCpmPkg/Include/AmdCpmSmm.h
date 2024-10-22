/*****************************************************************************
 *
 * Copyright (C) 2014-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_SMM_H_
#define _AMD_CPM_SMM_H_

#include <AmdCpmBase.h>
#include CPM_PROTOCOL_DEFINITION (AmdCpmTableProtocol)
#include CPM_PROTOCOL_DEFINITION (AmdCpmNvDataProtocol)
#include CPM_PROTOCOL_DEFINITION (AmdCpmDisplayFeatureProtocol)
#include CPM_PROTOCOL_DEFINITION (AmdCpmBootTimeRecordProtocol)

#ifndef AMD_CPM_EDKII
  #include <EfiSmmDriverLib.h>
  #include <EdkIIGlueDevicePathLib.h>
  #include EFI_PROTOCOL_DEFINITION (PciIo)
  #include EFI_PROTOCOL_DEFINITION (SmmBase)
  #include EFI_PROTOCOL_DEFINITION (LoadedImage)
  #include EFI_PROTOCOL_DEFINITION (SmmSxDispatch)
  #include EFI_PROTOCOL_DEFINITION (SmmSwDispatch)
#else
  #include <PiSmm.h>
  #include CPM_LIBRARY_DEFINITION (BaseLib)
  #include CPM_LIBRARY_DEFINITION (BaseMemoryLib)
  #include CPM_LIBRARY_DEFINITION (DebugLib)
  #include CPM_LIBRARY_DEFINITION (SmmServicesTableLib)
  #include CPM_LIBRARY_DEFINITION (UefiBootServicesTableLib)
  #include EFI_PROTOCOL_DEFINITION (SmmBase2)
  #include EFI_PROTOCOL_DEFINITION (SmmSxDispatch2)
  #include EFI_PROTOCOL_DEFINITION (SmmSwDispatch2)
#endif

#endif
