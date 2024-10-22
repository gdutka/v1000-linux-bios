/** @file
  BDS check related functions

;******************************************************************************
;* Copyright (c) 2017 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "BdsCheckFunctions.h"

/**
 Check the Boot#### number of device is EFI device or legacy device.

 @param[in] BootOptionNum       The boot option number of Boot#### variable

 @retval TRUE                   The device is EFI device.
 @retval FALSE                  The device is legacy device.
**/
BOOLEAN
IsEfiDevice (
  IN UINT16                     BootOptionNum
  )
{
  CHAR16                        BootOptionName[10];
  UINTN                         BootOptionVarSize;
  UINT8                         *BootOptionVar;
  UINT8                         *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  BOOLEAN                       IsEfiDevice;

  UnicodeSPrint (BootOptionName, sizeof (BootOptionName), L"Boot%04x", BootOptionNum);
  BootOptionVar = BdsLibGetVariableAndSize (
                    BootOptionName,
                    &gEfiGlobalVariableGuid,
                    &BootOptionVarSize
                    );
  ASSERT (BootOptionVar != NULL);
  if (BootOptionVar == NULL) {
    return FALSE;
  }

  WorkingPtr = BootOptionVar;
  WorkingPtr += sizeof (UINT32);
  WorkingPtr += sizeof (UINT16);
  WorkingPtr += (UINTN) StrSize ((CHAR16 *) WorkingPtr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;

  if ((BBS_DEVICE_PATH == DevicePath->Type) && (BBS_BBS_DP == DevicePath->SubType)) {
    IsEfiDevice = FALSE;
  } else {
    IsEfiDevice = TRUE;
  }

  FreePool (BootOptionVar);
  return IsEfiDevice;
}

/**
 Check whether option number is in the order array.

 @param[in] OrderArray          The pointer of order array
 @param[in] OrderCount          The count of order Array.
 @param[in] OptionNum           The number of option

 @retval TRUE   Option number is in the order array
 @retval FALSE  Option number is not in the order array
**/
BOOLEAN
IsOptionNumInOrderArray (
  IN UINT16                     *OrderArray,
  IN UINTN                      OrderCount,
  IN UINT16                     OptionNum
  )
{
  UINTN                         OrderIndex;

  return (BOOLEAN) (GetIndexInOrderArray (OrderArray, OrderCount, OptionNum, &OrderIndex) == EFI_SUCCESS);
}

/**
 According the Bus, Device, Function to check this controller is in Port Number Map table or not.

 @param[in] Bus                 PCI bus number
 @param[in] Device              PCI device number
 @param[in] Function            PCI function number

 @return TRUE                   This is a on board PCI device.
 @return FALSE                  Not on board device.
**/
BOOLEAN
IsOnBoardPciDevice (
  IN UINT32                     Bus,
  IN UINT32                     Device,
  IN UINT32                     Function
  )
{
  PORT_NUMBER_MAP               *PortMappingTable;
  PORT_NUMBER_MAP               EndEntry;
  UINTN                         NoPorts;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  NoPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    if ((PortMappingTable[NoPorts].Bus == Bus) &&
        (PortMappingTable[NoPorts].Device == Device) &&
        (PortMappingTable[NoPorts].Function == Function)) {
      return TRUE;
    }
    NoPorts++;
  }

  return FALSE;
}

/**
 Check the iput device path is wheter a USB device path or not.

 @param[in] DevicePath          Pointer to device path instance

 @retval TRUE                   It is a USB device path.
 @retval FALSE                  It isn't a USB device path.
**/
BOOLEAN
IsUsbDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  BOOLEAN                       IsEfiUsb;
  EFI_DEVICE_PATH_PROTOCOL      *FullDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *WorkingDevicePath;

  if (DevicePath == NULL) {
    return FALSE;
  }

  IsEfiUsb = FALSE;
  FullDevicePath = BdsExpandPartitionPartialDevicePathToFull ((HARDDRIVE_DEVICE_PATH *) DevicePath);
  WorkingDevicePath = (FullDevicePath == NULL) ? DevicePath : FullDevicePath;
  while (!IsDevicePathEnd (WorkingDevicePath)) {
    if (DevicePathType (WorkingDevicePath) == MESSAGING_DEVICE_PATH &&
        (DevicePathSubType (WorkingDevicePath) == MSG_USB_DP ||
         DevicePathSubType (WorkingDevicePath) == MSG_USB_WWID_DP)) {
      IsEfiUsb = TRUE;
      break;
    }

    WorkingDevicePath = NextDevicePathNode (WorkingDevicePath);
  }

  if (FullDevicePath != NULL) {
    FreePool (FullDevicePath);
  }

  return IsEfiUsb;
}

/**
 Check whether the input device path is for legacy boot option.

 @param[in] DevicePath          Pointer to device path instance

 @retval TRUE   The input device path is for legacy boot option
 @retval FALSE  The input device path is not for legacy boot option
**/
BOOLEAN
IsLegacyBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  if ((DevicePathType (DevicePath) == BBS_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == BBS_BBS_DP)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Check whether the input device path is for hardware vendor boot option.

 @param[in] DevicePath          Pointer to device path instance

 @retval TRUE   The input device path is for hardware vendor boot option
 @retval FALSE  The input device path is not for hardware vendor boot option
**/
BOOLEAN
IsValidHwVendorBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_HANDLE                    Handle;
  EFI_STATUS                    Status;

  if ((DevicePathType (OptionDevicePath) == HARDWARE_DEVICE_PATH) &&
      (DevicePathSubType (OptionDevicePath) == HW_VENDOR_DP)) {
    if ((CompareGuid (&((VENDOR_DEVICE_PATH *) OptionDevicePath)->Guid, &gH2OBdsBootDeviceGroupGuid)) &&
        (((H2O_BDS_BOOT_GROUP_DEVICE_PATH *) OptionDevicePath)->Flags & H2O_BDS_BOOT_GROUP_NO_DELETE) != 0) {
      //
      // skip boot option if it is H2O_BDS_BOOT_GROUP_GUID and H2O_BDS_BOOT_GROUP_NO_DELETE bit is set
      //
      return TRUE;
    }

    TempDevicePath = OptionDevicePath;
    Status = gBS->LocateDevicePath (
                    &gEfiLoadFileProtocolGuid,
                    &TempDevicePath,
                    &Handle
                    );
    if (!EFI_ERROR (Status)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 Check whether the input device path is for FV file boot option.

 @param[in] DevicePath          Pointer to device path instance

 @retval TRUE   The input device path is for FV file boot option
 @retval FALSE  The input device path is not for FV file boot option
**/
BOOLEAN
IsFvFileBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;

  TempDevicePath = OptionDevicePath;
  LastDeviceNode = OptionDevicePath;
  while (!IsDevicePathEnd (TempDevicePath)) {
    LastDeviceNode = TempDevicePath;
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  if (EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode) != NULL) {
    return TRUE;
  }

  return FALSE;
}

/**
 Check whether the input device path is a valid FV file boot option.

 @param[in] DevicePath          Pointer to device path instance

 @retval TRUE   The input device path is a valid FV file boot option
 @retval FALSE  The input device path is not a valid FV file boot option
**/
BOOLEAN
IsValidFvFileBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_GUID                      *FvFileName;
  EFI_STATUS                    Status;
  EFI_HANDLE                    Handle;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINTN                         Size;
  EFI_FV_FILETYPE               Type;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;

  TempDevicePath = OptionDevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiFirmwareVolume2ProtocolGuid,
                  &TempDevicePath,
                  &Handle
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = gBS->HandleProtocol (Handle, &gEfiFirmwareVolume2ProtocolGuid, (VOID **) &Fv);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  FvFileName = EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) TempDevicePath);
  if (FvFileName == NULL) {
    return FALSE;
  }

  Status = Fv->ReadFile (
                 Fv,
                 FvFileName,
                 NULL,
                 &Size,
                 &Type,
                 &Attributes,
                 &AuthenticationStatus
                 );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  return TRUE;
}

/**
 Check whether the input device path is for Windows To Go boot option.

 @param[in] DevicePath          Pointer to device path instance

 @retval TRUE   The input device path is for Windows To Go boot option
 @retval FALSE  The input device path is not for Windows To Go boot option
**/
BOOLEAN
IsWindowsToGoBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  )
{
  if ((DevicePathType (OptionDevicePath) == MESSAGING_DEVICE_PATH) &&
      (DevicePathSubType (OptionDevicePath) == MSG_USB_CLASS_DP)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Check if option variable is created by BIOS or not.

 @param[in] Variable            Pointer to option variable
 @param[in] VariableSize        Option variable size

 @retval TRUE                   Option varible is created by BIOS
 @retval FALSE                  Option varible is not created by BIOS
**/
BOOLEAN
BdsLibIsBiosCreatedOption (
  IN UINT8                      *Variable,
  IN UINTN                      VariableSize
  )
{
  UINT8                         *Ptr;
  UINT32                        Attribute;
  UINT16                        FilePathSize;
  CHAR16                        *Description;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  VOID                          *LoadOptions;
  UINT32                        LoadOptionsSize;

  if (Variable == NULL || VariableSize == 0) {
    return FALSE;
  }

  Ptr = Variable;
  Attribute = *(UINT32 *) Variable;
  Ptr += sizeof (UINT32);
  FilePathSize = *(UINT16 *) Ptr;
  Ptr += sizeof (UINT16);
  Description = (CHAR16 *) Ptr;
  Ptr += StrSize ((CHAR16 *) Ptr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
  Ptr += FilePathSize;

  LoadOptions     = Ptr;
  LoadOptionsSize = (UINT32) (VariableSize - (UINTN) (Ptr - Variable));

  //
  // RC signature in optional data means it is created by BIOS.
  //
  if ((LoadOptionsSize == 2 || LoadOptionsSize == SHELL_OPTIONAL_DATA_SIZE) &&
      (AsciiStrnCmp (LoadOptions, "RC", 2) == 0)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Check whether the input boot option variable data is legacy boot option or not.

 @param[in]  BootOptionVar      Pointer to boot option variable
 @param[out] BootOptBbsEntry    Double pointer to BBS table data in boot option variable
 @param[out] BbsIndex           Pointer to BBS table index

 @retval TRUE                   Input boot option variable data is legacy boot option
 @retval FALSE                  Input boot option variable data is not legacy boot option
**/
BOOLEAN
BdsLibIsLegacyBootOption (
  IN  UINT8                     *BootOptionVar,
  OUT BBS_TABLE                 **BbsEntry,
  OUT UINT16                    *BbsIndex
  )
{
  UINT8                         *Ptr;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  BOOLEAN                       Ret;
  UINT16                        DevPathLen;

  Ptr = BootOptionVar;
  Ptr += sizeof (UINT32);
  DevPathLen = *(UINT16 *) Ptr;
  Ptr += sizeof (UINT16);
  Ptr += StrSize ((UINT16 *) Ptr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
  if ((BBS_DEVICE_PATH == DevicePath->Type) && (BBS_BBS_DP == DevicePath->SubType)) {
    Ptr += DevPathLen;
    *BbsEntry = (BBS_TABLE *) Ptr;
    Ptr += sizeof (BBS_TABLE);
    *BbsIndex = *(UINT16 *) Ptr;
    Ret       = TRUE;
  } else {
    *BbsEntry = NULL;
    Ret       = FALSE;
  }

  return Ret;
}

/**
 Check the two device paths whether have the same file path.

 @param[in] FirstDevicePath     First multi device path instances which need to check
 @param[in] SecondDevicePath    Second multi device path instances which need to check

 @retval TRUE   There is a matched device path instance
 @retval FALSE  There is no matched device path instance
**/
BOOLEAN
BdsLibMatchFilePathDevicePathNode (
  IN EFI_DEVICE_PATH_PROTOCOL   *FirstDevicePath,
  IN EFI_DEVICE_PATH_PROTOCOL   *SecondDevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *FirstFileDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *SecondFileDevicePath;


  if ((FirstDevicePath == NULL) || (SecondDevicePath == NULL)) {
    return FALSE;
  }
  FirstFileDevicePath  = NULL;
  SecondFileDevicePath = NULL;
  //
  // find the file device path node
  //
  DevicePath = FirstDevicePath;
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == MEDIA_FILEPATH_DP)
        ) {
      FirstFileDevicePath = DevicePath;
      break;
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  if (FirstFileDevicePath == NULL) {
    return FALSE;
  }

  DevicePath = SecondDevicePath;
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == MEDIA_FILEPATH_DP)
        ) {
      SecondFileDevicePath = DevicePath;
      break;
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  if (SecondFileDevicePath == NULL) {
    return FALSE;
  }

  if (BdsLibStriCmp ((CHAR16 *) (FirstFileDevicePath + 1),  (CHAR16 *) (SecondFileDevicePath + 1)) == 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

