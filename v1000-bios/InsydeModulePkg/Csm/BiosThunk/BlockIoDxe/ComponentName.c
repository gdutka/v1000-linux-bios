/** @file

;******************************************************************************
;* Copyright (c) 2012-2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**

Copyright (c) 1999 - 2011, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions
of the BSD License which accompanies this distribution.  The
full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BiosBlkIo.h"

/**
  Retrieves a Unicode string that is the user readable name of the driver.

  This function retrieves the user readable name of a driver in the form of a
  Unicode string. If the driver specified by This has a user readable name in
  the language specified by Language, then a pointer to the driver name is
  returned in DriverName, and EFI_SUCCESS is returned. If the driver specified
  by This does not support the language specified by Language,
  then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language. This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified
                                in RFC 4646 or ISO 639-2 language code format.

  @param  DriverName[out]       A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                driver specified by This in the language
                                specified by Language.

  @retval EFI_SUCCESS           The Unicode string for the Driver specified by
                                This and the language specified by Language was
                                returned in DriverName.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER DriverName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
BiosBlockIoComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  );

/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by a driver.

  This function retrieves the user readable name of the controller specified by
  ControllerHandle and ChildHandle in the form of a Unicode string. If the
  driver specified by This has a user readable name in the language specified by
  Language, then a pointer to the controller name is returned in ControllerName,
  and EFI_SUCCESS is returned.  If the driver specified by This is not currently
  managing the controller specified by ControllerHandle and ChildHandle,
  then EFI_UNSUPPORTED is returned.  If the driver specified by This does not
  support the language specified by Language, then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  ControllerHandle[in]  The handle of a controller that the driver
                                specified by This is managing.  This handle
                                specifies the controller whose name is to be
                                returned.

  @param  ChildHandle[in]       The handle of the child controller to retrieve
                                the name of.  This is an optional parameter that
                                may be NULL.  It will be NULL for device
                                drivers.  It will also be NULL for a bus drivers
                                that wish to retrieve the name of the bus
                                controller.  It will not be NULL for a bus
                                driver that wishes to retrieve the name of a
                                child controller.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language.  This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified in
                                RFC 4646 or ISO 639-2 language code format.

  @param  ControllerName[out]   A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                controller specified by ControllerHandle and
                                ChildHandle in the language specified by
                                Language from the point of view of the driver
                                specified by This.

  @retval EFI_SUCCESS           The Unicode string for the user readable name in
                                the language specified by Language for the
                                driver specified by This was returned in
                                DriverName.

  @retval EFI_INVALID_PARAMETER ControllerHandle is NULL.

  @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid
                                EFI_HANDLE.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER ControllerName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This is not currently
                                managing the controller specified by
                                ControllerHandle and ChildHandle.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
BiosBlockIoComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  );


//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL     gBiosBlockIoComponentName = {
  BiosBlockIoComponentNameGetDriverName,
  BiosBlockIoComponentNameGetControllerName,
  "eng"
};

//
// EFI Component Name 2 Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL    gBiosBlockIoComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME) BiosBlockIoComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) BiosBlockIoComponentNameGetControllerName,
  "en"
};


GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mBiosBlockIoDriverNameTable[] = {
  {
    "eng;en",
    L"BIOS[INT13] Block Io Driver"
  },
  {
    NULL,
    NULL
  }
};

 //
 // The maximum model name in ATA identify data
 //
 #define MAX_MODEL_NAME_LEN            40

 CHAR16                                mModelName[MAX_MODEL_NAME_LEN + 1];


/**
  Retrieves a Unicode string that is the user readable name of the driver.

  This function retrieves the user readable name of a driver in the form of a
  Unicode string. If the driver specified by This has a user readable name in
  the language specified by Language, then a pointer to the driver name is
  returned in DriverName, and EFI_SUCCESS is returned. If the driver specified
  by This does not support the language specified by Language,
  then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language. This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified
                                in RFC 4646 or ISO 639-2 language code format.

  @param  DriverName[out]       A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                driver specified by This in the language
                                specified by Language.

  @retval EFI_SUCCESS           The Unicode string for the Driver specified by
                                This and the language specified by Language was
                                returned in DriverName.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER DriverName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
BiosBlockIoComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  )
{
  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mBiosBlockIoDriverNameTable,
           DriverName,
           (BOOLEAN)(This == &gBiosBlockIoComponentName)
           );
}


VOID
PrintAtaModelName (
  IN ATA_IDENTIFY_DATA  *IdentifyData
  )
{
  UINTN   Index;
  CHAR8   *Source;
  CHAR16  *Destination;

  Source = IdentifyData->ModelName;
  Destination = mModelName;

  //
  // Swap the byte order in the original module name.
  //
  for (Index = 0; Index < MAX_MODEL_NAME_LEN; Index += 2) {
    Destination[Index]      = Source[Index + 1];
    Destination[Index + 1]  = Source[Index];
  }

  //
  // add terminate character at the end
  //
  mModelName[MAX_MODEL_NAME_LEN] = L'\0';
}




/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by a driver.

  This function retrieves the user readable name of the controller specified by
  ControllerHandle and ChildHandle in the form of a Unicode string. If the
  driver specified by This has a user readable name in the language specified by
  Language, then a pointer to the controller name is returned in ControllerName,
  and EFI_SUCCESS is returned.  If the driver specified by This is not currently
  managing the controller specified by ControllerHandle and ChildHandle,
  then EFI_UNSUPPORTED is returned.  If the driver specified by This does not
  support the language specified by Language, then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  ControllerHandle[in]  The handle of a controller that the driver
                                specified by This is managing.  This handle
                                specifies the controller whose name is to be
                                returned.

  @param  ChildHandle[in]       The handle of the child controller to retrieve
                                the name of.  This is an optional parameter that
                                may be NULL.  It will be NULL for device
                                drivers.  It will also be NULL for a bus drivers
                                that wish to retrieve the name of the bus
                                controller.  It will not be NULL for a bus
                                driver that wishes to retrieve the name of a
                                child controller.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language.  This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified in
                                RFC 4646 or ISO 639-2 language code format.

  @param  ControllerName[out]   A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                controller specified by ControllerHandle and
                                ChildHandle in the language specified by
                                Language from the point of view of the driver
                                specified by This.

  @retval EFI_SUCCESS           The Unicode string for the user readable name in
                                the language specified by Language for the
                                driver specified by This was returned in
                                DriverName.

  @retval EFI_INVALID_PARAMETER ControllerHandle is NULL.

  @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid
                                EFI_HANDLE.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER ControllerName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This is not currently
                                managing the controller specified by
                                ControllerHandle and ChildHandle.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
BiosBlockIoComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  )
{
  EFI_DISK_INFO_PROTOCOL                *DiskInfo;
  ATA_IDENTIFY_DATA                     *IdentifyData;
  UINT32                                IdentifyDataSize;
  EFI_STATUS                            Status;
  EFI_UNICODE_STRING_TABLE              *ControllerNameTable;
  EFI_HANDLE                            *DiskInfoHandleBuffer;
  EFI_HANDLE                            DiskInfoHandle;
  EFI_HANDLE                            AtaPassthruHandle;
  EFI_HANDLE                            PciIOHandle;
  UINTN                                 HandleCount;
  UINT32                                HandleIndex;
  BIOS_BLOCK_IO_DEV                     *BiosBlockIoPrivate;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *ControllerDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *DiskInfoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *ParentDevicePath;
  UINT32                                PortNumebr;
  UINT32                                MultiPortNumber;
  UINT8                                 BiosBlkPortMap;
  UINT8                                 BiosBlkPortNumber;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  PCI_TYPE00                            Pci;
  EFI_BLOCK_IO_PROTOCOL                 *BlockIo;

  IdentifyDataSize = sizeof(ATA5_IDENTIFY_DATA);
  DiskInfoHandle = NULL;
  AtaPassthruHandle = NULL;
  PciIOHandle = NULL;
  PortNumebr = 0;
  MultiPortNumber = 0;
  BiosBlkPortMap = 0;
  BiosBlkPortNumber = 0;

  Status = gBS->HandleProtocol (
                  ControllerHandle,
                  &gEfiBlockIoProtocolGuid,
                  (VOID**)&BlockIo
                  );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Skip handles that does not belong here.
  //
  if (BASE_CR (BlockIo, BIOS_BLOCK_IO_DEV, BlockIo)->Signature != BIOS_CONSOLE_BLOCK_IO_DEV_SIGNATURE) {
    return Status;
  }
  BiosBlockIoPrivate = BIOS_BLOCK_IO_FROM_THIS(BlockIo);

  //
  //  the code flow is
  //  1. Retrieve the device path of Controller handle passed by core code.
  //
  Status = gBS->HandleProtocol (
                  ControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**)&ControllerDevicePath
                  );

  if (Status != EFI_SUCCESS) {
    return Status;
  }


  //
  //  2. Find the handles with PCIIO and AtaPassthru
  //

  DevicePath = ControllerDevicePath;
  Status = gBS->LocateDevicePath(&gEfiAtaPassThruProtocolGuid, &DevicePath, &AtaPassthruHandle);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                  AtaPassthruHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**)&ParentDevicePath
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  //  3. Get PCI config data and blockIO private data
  //
  DevicePath = ControllerDevicePath;
  Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &DevicePath, &PciIOHandle);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                  PciIOHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID**)&PciIo
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, (sizeof (Pci) / sizeof (UINT32)), &Pci);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // 4. Find the diskinfo instance that installed under same parent handle as the blockIO handle
  //

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDiskInfoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &DiskInfoHandleBuffer
                  );
  if (Status != EFI_SUCCESS || DiskInfoHandleBuffer == NULL) {
    return Status;
  }

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->HandleProtocol (
                    DiskInfoHandleBuffer[HandleIndex],
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&DiskInfoDevicePath
                    );
    if (CompareMem(ParentDevicePath, DiskInfoDevicePath, GetDevicePathSize(ParentDevicePath) - END_DEVICE_PATH_LENGTH)) {
      continue;
    }

    DiskInfoHandle = DiskInfoHandleBuffer[HandleIndex];

    //
    // 5. compare port number we get from private data with the one we get from diskinfo
    //
    Status = gBS->HandleProtocol (
                    DiskInfoHandle,
                    &gEfiDiskInfoProtocolGuid,
                    (VOID**)&DiskInfo
                    );
    if (EFI_ERROR(Status)) {
      FreePool (DiskInfoHandleBuffer);
      return Status;
    }

    DiskInfo->WhichIde(
                DiskInfo,
                &PortNumebr,
                &MultiPortNumber
                );

    if ((Pci.Hdr.VendorId == 0x8086) && (Pci.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID)) {
      //
      // Intel RAID OpRom ONLY.
      // If BIT0 set, the SATA port number of this device is 0, it BIT1 set port number is 1 and so on.
      //
      BiosBlkPortMap =  BiosBlockIoPrivate->Bios.Parameters.DevicePath.Atapi.Reserved[2];
      for (BiosBlkPortNumber = 0xFF; BiosBlkPortMap != 0; BiosBlkPortNumber++, BiosBlkPortMap >>= 1);
      //
      // 5. check if the diskinfo handle and the blockio handle, with same parent handle, that
      //    have the same port number
      //
        if (BiosBlkPortNumber == PortNumebr) {
          DiskInfoHandle = DiskInfoHandleBuffer[HandleIndex];
        } else {
          //
          // this is not the diskinfo we are look for, wrong port number
          //
          continue;
        }
    } else {
      //
      // For Non-Intel RAID controller
      //
      BiosBlkPortNumber = (UINT8)BiosBlockIoPrivate->Bios.Parameters.DevicePath.RAID.RAIDArrayNumber;
      if (BiosBlkPortNumber == PortNumebr) {
        DiskInfoHandle = DiskInfoHandleBuffer[HandleIndex];
      } else {
         //
         // this is not the diskinfo we are look for, wrong port number
         //
         continue;
      }
    }

      //
      //6. prepare ControllerName
      //
      IdentifyData = AllocatePool (IdentifyDataSize);
      if (IdentifyData == NULL) {
       FreePool (DiskInfoHandleBuffer);
       return EFI_UNSUPPORTED;
      }

      Status = DiskInfo->Identify(
                 DiskInfo,
                 IdentifyData,
                 &IdentifyDataSize
                 );

      if (EFI_ERROR(Status)) {
       FreePool (DiskInfoHandleBuffer);
       FreePool (IdentifyData);
       return EFI_UNSUPPORTED;
      }

      PrintAtaModelName(IdentifyData);

      ControllerNameTable = mBiosBlockIoDriverNameTable;
      AddUnicodeString (
        Language,
        This->SupportedLanguages,
        &ControllerNameTable,
        mModelName
        );

      //
      // free buffer before leave
      //
      FreePool (DiskInfoHandleBuffer);
      FreePool (IdentifyData);


      LookupUnicodeString (
        Language,
        This->SupportedLanguages,
        ControllerNameTable,
        ControllerName
        );

      //
      // if a new table allocate for ControllerNameTable, relesae it before leave
      //
      if (ControllerNameTable != mBiosBlockIoDriverNameTable) {
        FreePool (ControllerNameTable);
      }

	    return Status;
  }

  if (DiskInfoHandleBuffer != NULL) {
    FreePool (DiskInfoHandleBuffer);
  }

  return EFI_UNSUPPORTED;
}
