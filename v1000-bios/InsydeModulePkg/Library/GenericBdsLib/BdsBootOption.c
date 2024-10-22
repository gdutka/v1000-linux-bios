/** @file
  BDS boot option related functions

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

#include "BdsBootOption.h"

#define SECURE_BOOT_ENABLED      0x00
#define SECURE_BOOT_DISABLED     0x01
#define MAX_BIOS_ENVIRONMENT     0x02
#define UNKNOWN_SECURE_SETTING   0x02

LIST_ENTRY                       mWindowsToGoDeviceList = INITIALIZE_LIST_HEAD_VARIABLE (mWindowsToGoDeviceList);
STATIC DEVICE_TYPE_INFORMATION   mDeviceTypeInfo[] = {
  {BDS_EFI_ACPI_FLOPPY_BOOT,        STRING_TOKEN (STR_DESCRIPTION_FLOPPY), 0, ATTR_ALL},
  {BDS_EFI_MESSAGE_SATA_BOOT,       STRING_TOKEN (STR_DESCRIPTION_CD_DVD), 0, ATTR_ALL},
  {BDS_EFI_MESSAGE_ATAPI_BOOT,      STRING_TOKEN (STR_DESCRIPTION_CD_DVD), 0, ATTR_ALL},
  {BDS_EFI_MEDIA_CDROM_BOOT,        STRING_TOKEN (STR_DESCRIPTION_CD_DVD), 0, ATTR_ALL},
  {BDS_EFI_MESSAGE_USB_DEVICE_BOOT, STRING_TOKEN (STR_DESCRIPTION_USB),    0, ATTR_ALL},
  {BDS_EFI_MESSAGE_SCSI_BOOT,       STRING_TOKEN (STR_DESCRIPTION_SCSI),   0, ATTR_ALL},
  {BDS_EFI_MESSAGE_ISCSI_BOOT,      STRING_TOKEN (STR_DESCRIPTION_ISCSI),  0, ATTR_ALL},
  {BDS_EFI_MESSAGE_MISC_BOOT,       STRING_TOKEN (STR_DESCRIPTION_MISC),   0, ATTR_ALL},
  {BDS_EFI_MEDIA_HD_BOOT,           STRING_TOKEN (STR_DESCRIPTION_HDD),    0, ATTR_ALL},
  {BDS_EFI_SDHC_BOOT,               STRING_TOKEN (STR_DESCRIPTION_EMMC),   0, ATTR_UNREMOVABLE_DEV},
  {BDS_EFI_SDHC_BOOT,               STRING_TOKEN (STR_DESCRIPTION_SDMMC),  0, ATTR_REMOVABLE_DEV},
};

/**
 Check if the target file is signed image or not

 @param[in] Handle             A pointer to a device handle.
 @param[in] FileName           Pointer to file name

 @retval TRUE                  This is a signed image
 @retval FALSE                 The file isn't exist, the image is not supported or this isn't a signed image
**/
STATIC
BOOLEAN
IsSignedImage (
  IN EFI_HANDLE                        Handle,
  IN CHAR16                            *FileName
  )
{
  EFI_STATUS                           Status;
  EFI_IMAGE_DOS_HEADER                 DosHeader;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION  Hdr;
  EFI_IMAGE_OPTIONAL_HEADER_UNION      HdrData;
  UINT16                               Magic;
  EFI_IMAGE_DATA_DIRECTORY             *SecDataDir;

  Hdr.Union = &HdrData;
  Status = BdsLibGetImageHeader (
             Handle,
             FileName,
             &DosHeader,
             Hdr
             );
  if (EFI_ERROR (Status) || !EFI_IMAGE_MACHINE_TYPE_SUPPORTED (Hdr.Pe32->FileHeader.Machine)) {
    return FALSE;
  }

  if (Hdr.Pe32->FileHeader.Machine == EFI_IMAGE_MACHINE_IA64 && Hdr.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
  } else {
    Magic = Hdr.Pe32->OptionalHeader.Magic;
  }

  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
  } else {
    //
    // Use PE32+ offset.
    //
    SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
  }

  if (SecDataDir->Size != 0) {
    return TRUE;
  }

  return FALSE;
}

/**
 This function uses device handle to check the EFI boot option is Windows To Go device or not

 @param[in] Handle     A pointer to a device handle.

 @retval TRUE          This is a Windows To Go device
 @retval FALSE         This isn't a Windows To Go device
**/
BOOLEAN
IsWindowsToGo (
  IN EFI_HANDLE                 Handle
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;

  DevicePath = DevicePathFromHandle (Handle);

  for (TempDevicePath = DevicePath; !IsDevicePathEndType (TempDevicePath); TempDevicePath = NextDevicePathNode (TempDevicePath)) {
    if (DevicePathType (TempDevicePath) == MEDIA_DEVICE_PATH &&
        DevicePathSubType (TempDevicePath) == MEDIA_CDROM_DP) {
      return FALSE;
    }
  }

  if (BdsLibGetBootTypeFromDevicePath (DevicePath) == BDS_EFI_MESSAGE_USB_DEVICE_BOOT &&
      IsSignedImage (Handle, EFI_REMOVABLE_MEDIA_FILE_NAME)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Add a BDS_COMMON_OPTION to portabl boot option list

 @param[in]      DevicePath           A poitner to device path instance.
 @param[in, out] BdsBootOptionList    The header of the link list which indexed all current boot options

 @retval EFI_SUCCESS            Finished all the boot device enumerate and create the boot option base on that boot device
 @retval EFI_OUT_OF_RESOURCES   Fail to allocate pool
**/
STATIC
EFI_STATUS
RegisterPortalbeBootOption (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN OUT LIST_ENTRY             *BdsBootOptionList
 )
{
  BDS_COMMON_OPTION             *Option;

  Option = AllocateZeroPool (sizeof (BDS_COMMON_OPTION));
  if (Option == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Option->Signature   = BDS_LOAD_OPTION_SIGNATURE;
  Option->DevicePath  = AllocateZeroPool (GetDevicePathSize (DevicePath));
  if (Option->DevicePath == NULL) {
    FreePool (Option);
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (Option->DevicePath, DevicePath, GetDevicePathSize (DevicePath));
  Option->Attribute   = 0;
  Option->Description = NULL;
  Option->LoadOptions = NULL;
  Option->LoadOptionsSize = 0;
  InsertTailList (BdsBootOptionList, &Option->Link);

  return EFI_SUCCESS;
}

/**
 Enumernate all Windows To Go devices and return device number.

 @param[out] DeviceNum          Pointer to device number

 @retval EFI_SUCCESS            Enumernate all Windows To Go devices success
 @retval EFI_NOT_FOUND          Windows To Go device is not found
**/
STATIC
EFI_STATUS
EnumerateAllWindowsToGoDevice (
  OUT UINT16                    *DeviceNum
  )
{
  UINTN                         Index;
  UINTN                         NumberFileSystemHandles;
  EFI_HANDLE                    *FileSystemHandles;
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;
  UINT16                        Count;

  Count                   = 0;
  NumberFileSystemHandles = 0;

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &NumberFileSystemHandles,
         &FileSystemHandles
         );
  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    if (IsWindowsToGo (FileSystemHandles[Index])) {
      //
      // Add to Windows To Go list if this is USB Entry for Windows To Go
      //
      FilePath = FileDevicePath (FileSystemHandles[Index], EFI_REMOVABLE_MEDIA_FILE_NAME);
      RegisterPortalbeBootOption (FilePath, &mWindowsToGoDeviceList);
      FreePool (FilePath);
      Count++;
    }
  }

  if (NumberFileSystemHandles != 0) {
    FreePool (FileSystemHandles);
  }

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  *DeviceNum = Count;

  return EFI_SUCCESS;
}

/**
 Set Windows To Go variable

 @param[in] DeviceNum          Windows To Go device number

 @retval EFI_SUCCESS           Set Windows To Go device variable success
 @retval EFI_NOT_FOUND         Windows To Go device is not found
 @retval EFI_OUT_OF_RESOURCES  Allocate memory fail
**/
STATIC
EFI_STATUS
SetWindowsToGoVariable (
  IN UINT16                     DeviceNum
  )
{
  EFI_STATUS                    Status;
  BDS_COMMON_OPTION             *Option;
  UINT8                         *Ptr;
  UINT8                         *VarData;
  UINTN                         Size;
  UINTN                         DevPathSize;

  if (IsListEmpty (&mWindowsToGoDeviceList)) {
    return EFI_NOT_FOUND;
  }

  Option      = CR (mWindowsToGoDeviceList.ForwardLink, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);
  DevPathSize = GetDevicePathSize (Option->DevicePath);
  Size        = DevPathSize + sizeof(UINT16);
  VarData     = AllocatePool (Size);
  if (VarData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Ptr = VarData;
  CopyMem (Ptr, Option->DevicePath, DevPathSize);
  Ptr += DevPathSize;
  CopyMem (Ptr, &DeviceNum, sizeof(UINT16));
  Status = gRT->SetVariable (
                  L"WindowsToGo",
                  &gEfiGenericVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  Size,
                  VarData
                  );
  FreePool (VarData);

  return Status;
}

/**
 Enumernate all Windows To Go devices and update them to list.
**/
VOID
UpdateWindowsToGoList (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT16                        DeviceNum;
  KERNEL_CONFIGURATION          SystemConfiguration;

  Status = GetKernelConfiguration (&SystemConfiguration);
  if (Status == EFI_SUCCESS && SystemConfiguration.UsbBoot != 0) {
    return;
  }
  Status = EnumerateAllWindowsToGoDevice (&DeviceNum);
  if (Status == EFI_SUCCESS) {
    SetWindowsToGoVariable (DeviceNum);
  }
}

/**
  Check the MAC address is empty

  @param  MacAddr

  @retval TRUE
  @retval FALSE
**/
BOOLEAN
MacEmpty (
  IN UINT8                      *MacAddr,
  IN UINTN                      MACLength
  )
{
  UINTN       Index;
  for (Index = 0; Index < MACLength; Index++ ) {
    if (MacAddr[Index] != 0) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
  Check two MAC address is equal

  @param  MacAddr1
  @param  MacAddr2

  @retval TRUE
  @retval FALSE
**/
BOOLEAN
MacCompare (
  IN UINT8                      *MacAddr1,
  IN UINT8                      *MacAddr2,
  IN UINTN                      MACLength
  )
{
  UINTN       Index;
  for (Index = 0; Index < MACLength; Index++) {
    if (MacAddr1[Index] != MacAddr2[Index]) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
  Translate MAC address to index of remembered MAC address list;
  This function will recoder MAC addresses is not scaned, max records of UEFI_NETWORK_BOOT_OPTION_MAX

  @param  DeviceMacAddr    Your Devices MAC Address
  @param  MacAddressList   Mac Address list buffer
  @param  DeviceId         Result

  @retval EFI_SUCCESS     Found
  @retval EFI_NOT_FOUND   Not found or buffer full, check with UEFI_NETWORK_BOOT_OPTION_MAX
                          Id = UEFI_NETWORK_BOOT_OPTION_MAX
**/
EFI_STATUS
MacToIndex (
  IN     EFI_MAC_ADDRESS        *DeviceMacAddr,
  IN OUT EFI_MAC_ADDRESS        *MacAddressList,
  OUT    UINTN                  *DeviceId
  )
{
  EFI_STATUS   Status;
  UINTN        Index;
  UINTN        MACLength;

  MACLength = sizeof (EFI_MAC_ADDRESS);
  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < UEFI_NETWORK_BOOT_OPTION_MAX; Index++) {
    if (MacEmpty ((VOID *) &(MacAddressList[Index]), MACLength)) {
      CopyMem (&(MacAddressList[Index]), DeviceMacAddr, MACLength);
      Status = EFI_SUCCESS;
    } else if (MacCompare ((VOID *) &(MacAddressList[Index]), (VOID *)(DeviceMacAddr), MACLength)) {
      Status = EFI_SUCCESS;
    }
    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  *DeviceId = Index;
  return Status;
}

/**
  Build Boot option for UEFI PXE.
  The boot option will follow style.
    "EFI Network (Index) for IPv(4/6) (MAC Address)"

  @param[in] BdsBootOptionList    Your Devices MAC Address

  @retval EFI_SUCCESS     Boot option build success
  @retval Other           Memory allocate failed.
                          PXE boot option not found.
                          System configuration not found.
**/
STATIC
EFI_STATUS
BuildNetworkBootOption (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  KERNEL_CONFIGURATION          SystemConfiguration;
  EFI_DEVICE_PATH_PROTOCOL      *NetworkDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  UINTN                         NumberLoadFileHandles;
  EFI_HANDLE                    *LoadFileHandles;
  CHAR16                        Buffer[UEFI_NETWORK_BOOT_OPTION_MAX_CHAR];
  MAC_ADDR_DEVICE_PATH          *MAC;
  UINTN                         DeviceId;
  EFI_MAC_ADDRESS               *MacAddressList = NULL;
  BOOLEAN                       NeedBuild;
  UINT8                         IpType;
  CHAR16                        VlanString[UEFI_NETWORK_VLAN_STRING_LENGTH];
  UINT16                        VlanID;
  BOOLEAN                       IsIPv4BootOption;
  BOOLEAN                       IsIPv6BootOption;
  BOOLEAN                       IsHttpBoot;
  CHAR8                         *UriString;
  URI_DEVICE_PATH               *UriDevicePath;

  NetworkDevicePath   = NULL;
  TempDevicePath      = NULL;
  MacAddressList      = NULL;
  LoadFileHandles     = NULL;

  Status = GetKernelConfiguration (&SystemConfiguration);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // If Pxe disabled then do not build Network boot variables
  //
  if (SystemConfiguration.PxeBootToLan == 0) {
    return EFI_SUCCESS;
  }

  ZeroMem (Buffer, sizeof (Buffer));

  MacAddressList = AllocateZeroPool (sizeof (EFI_MAC_ADDRESS) * UEFI_NETWORK_BOOT_OPTION_MAX);
  if (MacAddressList == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ERROR_HANDLE;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadFileProtocolGuid,
                  NULL,
                  &NumberLoadFileHandles,
                  &LoadFileHandles
                  );
  if (EFI_ERROR (Status)) {
    goto ERROR_HANDLE;
  }

  for (Index = 0; Index < NumberLoadFileHandles; Index++) {
    NeedBuild = FALSE;
    //
    // VlanID is Range 0 ~ 4094, If VlanID == 0xFFFF means not Vlan device
    //
    VlanID              = 0xFFFF;
    MAC                 = NULL;
    IsIPv4BootOption    = FALSE;
    IsIPv6BootOption    = FALSE;
    IsHttpBoot          = FALSE;
    IpType              = 0;
    UriString           = "";

    Status = gBS->HandleProtocol (
                    LoadFileHandles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &TempDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Parsing DevicePath to identifier MAC address or IP capability or HTTP URI
    //
    NetworkDevicePath = TempDevicePath;
    while (!IsDevicePathEnd (TempDevicePath)) {
      if (TempDevicePath->Type == MESSAGING_DEVICE_PATH) {
        switch (TempDevicePath->SubType) {
        case MSG_MAC_ADDR_DP:
          MAC = (MAC_ADDR_DEVICE_PATH *)TempDevicePath;
          break;

        case MSG_VLAN_DP:
          VlanID = ((VLAN_DEVICE_PATH *)TempDevicePath)->VlanId;
          break;

        case MSG_IPv4_DP:
          IsIPv4BootOption = TRUE;
          break;

        case MSG_IPv6_DP:
          IsIPv6BootOption = TRUE;
          break;

        case MSG_URI_DP:
          IsHttpBoot = TRUE;
          if (DevicePathNodeLength (TempDevicePath) > sizeof (EFI_DEVICE_PATH)) {
            UriDevicePath = (URI_DEVICE_PATH *) TempDevicePath;
            UriString = UriDevicePath->Uri;
          }
          break;
        }
      }
      TempDevicePath = NextDevicePathNode (TempDevicePath);
    }

    //
    // IPv4 and IPv6 both in DevicePath is incorrect
    //
    if (IsIPv4BootOption && IsIPv6BootOption) {
      continue;
    }

    //
    // If MAC has not found, Item cannot display
    //
    if (MAC == NULL) {
      continue;
    }

    //
    // Build Policy:
    //   Device Path is:
    //     MAC\IPv4    : "EFI Network X for IPv4 (MAC Address)"
    //     MAC\IPv6    : "EFI Network X for IPv6 (MAC Address)"
    //     MAC         : "EFI Network X for IPv4 (MAC Address)"
    //     Other       : "Others: (Device Path)"
    //
    if (MAC != NULL && (!IsIPv4BootOption && !IsIPv6BootOption)) {
      IsIPv4BootOption = TRUE;
    }

    if (IsIPv4BootOption && !IsIPv6BootOption) {
      IpType = 4;
    } else if (!IsIPv4BootOption && IsIPv6BootOption) {
      IpType = 6;
    } else {
      /// Shouldn't have this case
      continue;
    }

    switch (SystemConfiguration.NetworkProtocol) {
      case UEFI_NETWORK_BOOT_OPTION_PXE_IPV4:
      case UEFI_NETWORK_BOOT_OPTION_PXE_IPV6:
      case UEFI_NETWORK_BOOT_OPTION_PXE_BOTH:
        if (IsHttpBoot) {
          break;
        }
        if (((SystemConfiguration.NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_PXE_IPV4) && (IpType == 4)) ||
            ((SystemConfiguration.NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_PXE_IPV6) && (IpType == 6)) ||
             (SystemConfiguration.NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_PXE_BOTH)) {
          NeedBuild = TRUE;
        }
        break;

      case UEFI_NETWORK_BOOT_OPTION_HTTP_IPV4:
      case UEFI_NETWORK_BOOT_OPTION_HTTP_IPV6:
      case UEFI_NETWORK_BOOT_OPTION_HTTP_BOTH:
        if (!IsHttpBoot) {
          break;
        }
        if (((SystemConfiguration.NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_HTTP_IPV4) && (IpType == 4)) ||
            ((SystemConfiguration.NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_HTTP_IPV6) && (IpType == 6)) ||
            (SystemConfiguration.NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_HTTP_BOTH)) {
          NeedBuild = TRUE;
        }
        break;

      case UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_IPV4:
      case UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_IPV6:
      case UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_BOTH:
        if (((SystemConfiguration.NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_IPV4) && (IpType == 4)) ||
            ((SystemConfiguration.NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_IPV6) && (IpType == 6)) ||
            (SystemConfiguration.NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_BOTH)) {
          NeedBuild = TRUE;
        }
        break;

      default:
        break;
    }

    if (!NeedBuild) {
      continue;
    }
    if (IsIPv4BootOption || IsIPv6BootOption) {
      MacToIndex (&(MAC->MacAddress), MacAddressList, &DeviceId);
      //
      // Select Network boot type string.
      //
      if (!IsHttpBoot) {
        UnicodeSPrint (
          Buffer,
          sizeof (Buffer),
          L"EFI PXE %d for IPv%d (%02x-%02x-%02x-%02x-%02x-%02x) ",
          DeviceId,
          IpType,
          MAC->MacAddress.Addr[0],
          MAC->MacAddress.Addr[1],
          MAC->MacAddress.Addr[2],
          MAC->MacAddress.Addr[3],
          MAC->MacAddress.Addr[4],
          MAC->MacAddress.Addr[5]
          );
      } else {
        if (AsciiStrLen (UriString) != 0) {
          UnicodeSPrint (
            Buffer,
            sizeof (Buffer),
            L"[IPv%d](%02x-%02x-%02x-%02x-%02x-%02x) %a",
            IpType,
            MAC->MacAddress.Addr[0],
            MAC->MacAddress.Addr[1],
            MAC->MacAddress.Addr[2],
            MAC->MacAddress.Addr[3],
            MAC->MacAddress.Addr[4],
            MAC->MacAddress.Addr[5],
            UriString
            );
        } else {
          UnicodeSPrint (
            Buffer,
            sizeof (Buffer),
            L"EFI HTTP[IPv%d](%02x-%02x-%02x-%02x-%02x-%02x) ",
            IpType,
            MAC->MacAddress.Addr[0],
            MAC->MacAddress.Addr[1],
            MAC->MacAddress.Addr[2],
            MAC->MacAddress.Addr[3],
            MAC->MacAddress.Addr[4],
            MAC->MacAddress.Addr[5]
            );
        }
      }
      if (VlanID != 0xFFFF) {
        UnicodeSPrint (
          VlanString,
          sizeof (VlanString),
          L"VLAN(%d)",
          VlanID
          );
          StrCatS (Buffer, UEFI_NETWORK_BOOT_OPTION_MAX_CHAR, VlanString);
        }
        BdsLibBuildOptionFromHandle (LoadFileHandles[Index], NULL, Buffer);
      } else {
        //
        // Build unknown load file option
        //
        StrCatS (Buffer, UEFI_NETWORK_BOOT_OPTION_MAX_CHAR, L"Others: ");
        StrCatS (Buffer, UEFI_NETWORK_BOOT_OPTION_MAX_CHAR, DevicePathToStr (NetworkDevicePath));
        BdsLibBuildOptionFromHandle (LoadFileHandles[Index], NULL, Buffer);
      }
    }

ERROR_HANDLE:
  if (LoadFileHandles) {
    FreePool (LoadFileHandles);
  }
  if (MacAddressList) {
    FreePool (MacAddressList);
  }

  return Status;
}

/**
 Check whether the input device path is a valid network boot option or not.

 @param[in] SystemConfiguration   Pointer to system configuration
 @param[in] OptionDevicePath      Pointer to device path of boot option

 @retval TRUE    The input device path is a valid network boot option
 @retval FALSE   The input device path is not a valid network boot option
**/
BOOLEAN
IsValidNetworkBootOption (
  IN KERNEL_CONFIGURATION          *SystemConfiguration,
  IN EFI_DEVICE_PATH_PROTOCOL      *OptionDevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL         *TempDevicePath;
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  BOOLEAN                          IsValid;
  BOOLEAN                          IsIPv4BootOption;
  BOOLEAN                          IsIPv6BootOption;
  BOOLEAN                          IsHttpBootOption;

  TempDevicePath = OptionDevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiLoadFileProtocolGuid,
                  &TempDevicePath,
                  &Handle
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // Focus on removable media and network boot device
  //
  if (!FeaturePcdGet (PcdH2ONetworkSupported)) {
    return TRUE;
  }

  IsValid          = FALSE;
  IsIPv4BootOption = FALSE;
  IsIPv6BootOption = FALSE;
  IsHttpBootOption = FALSE;

  //
  // Scan Build Option device path, if is IPv4 or IPv6 will trigger boolean
  //
  TempDevicePath = OptionDevicePath;
  while (!IsDevicePathEnd (TempDevicePath)) {
    if (TempDevicePath->Type == MESSAGING_DEVICE_PATH) {
      if (TempDevicePath->SubType == MSG_IPv4_DP) {
        IsIPv4BootOption = TRUE;
      } else if (TempDevicePath->SubType == MSG_IPv6_DP) {
        IsIPv6BootOption = TRUE;
      } else if (TempDevicePath->SubType == MSG_URI_DP) {
        IsHttpBootOption = TRUE;
      }
    }
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  //
  // Detect user set Network is IPv4/IPv6/Both
  // Will delete delete not selected item
  //
  switch (SystemConfiguration->NetworkProtocol) {
  case UEFI_NETWORK_BOOT_OPTION_PXE_IPV4:
    if (!IsHttpBootOption && IsIPv4BootOption) {
      IsValid = TRUE;
    }
    break;

  case UEFI_NETWORK_BOOT_OPTION_PXE_IPV6:
    if (!IsHttpBootOption && IsIPv6BootOption) {
      IsValid = TRUE;
    }
    break;

  case UEFI_NETWORK_BOOT_OPTION_PXE_BOTH:
    if (!IsHttpBootOption) {
      IsValid = TRUE;
    }
    break;

  case UEFI_NETWORK_BOOT_OPTION_HTTP_IPV4:
    if (IsHttpBootOption && IsIPv4BootOption) {
      IsValid = TRUE;
    }
    break;

  case UEFI_NETWORK_BOOT_OPTION_HTTP_IPV6:
    if (IsHttpBootOption && IsIPv6BootOption) {
      IsValid = TRUE;
    }
    break;

  case UEFI_NETWORK_BOOT_OPTION_HTTP_BOTH:
    if (IsHttpBootOption) {
      IsValid = TRUE;
    }
    break;

  case UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_IPV4:
    if (IsIPv4BootOption) {
      IsValid = TRUE;
    }
    break;

  case UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_IPV6:
    if (IsIPv6BootOption) {
      IsValid = TRUE;
    }
    break;

  case UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_BOTH:
    IsValid = TRUE;
    break;
  }

  //
  // If Boot option is vaild, will not delete
  // Include Unknown LoadFile build option
  //
  if (!IsIPv4BootOption && !IsIPv6BootOption) {
    IsValid = TRUE;
  }

  return IsValid;
}

/**
 Enumernate all network boot options.

 @retval EFI_SUCCESS                 Enumernate all network boot options success
**/
EFI_STATUS
EnumerateAllNetworkBootOption (
  VOID
  )
{
  EFI_STATUS                   Status;
  UINTN                        NumberLoadFileHandles;
  EFI_HANDLE                   *LoadFileHandles;
  UINTN                        Index;
  VOID                         *ProtocolInstance;
  CHAR16                        Buffer[128];

  if (FeaturePcdGet (PcdH2ONetworkSupported)) {
    BuildNetworkBootOption ();
  } else {
    gBS->LocateHandleBuffer (
          ByProtocol,
          &gEfiSimpleNetworkProtocolGuid,
          NULL,
          &NumberLoadFileHandles,
          &LoadFileHandles
          );
    for (Index = 0; Index < NumberLoadFileHandles; Index++) {
      Status = gBS->HandleProtocol (
                      LoadFileHandles[Index],
                      &gEfiLoadFileProtocolGuid,
                      (VOID **) &ProtocolInstance
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      if (Index == 0) {
        UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_NETWORK)));
      } else {
        UnicodeSPrint (Buffer, sizeof (Buffer), L"%s %d", BdsLibGetStringById (STRING_TOKEN (STR_DESCRIPTION_NETWORK)), Index);
      }
      BdsLibBuildOptionFromHandle (LoadFileHandles[Index], NULL, Buffer);
    }

    if (NumberLoadFileHandles) {
      FreePool (LoadFileHandles);
    }
  }

  return EFI_SUCCESS;
}

/**
  Compare device path to check if this boot device is already exist in BootOrder.

  @param[in] DevicePath        Device path of the query boot device

  @retval TRUE                 The device is already in BootOrder variable.
  @retval FALSE                The device is not in BootOrder variable.
**/
BOOLEAN
IsAlreadyInBootOrder (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  UINTN                         DevicePathSize;
  UINT16                        *BootOrder;
  UINTN                         BootOrderSize;
  UINTN                         BootDevNum;
  UINTN                         Index;
  UINT16                        BootOptionName[20];
  UINTN                         VariableSize;
  UINT8                         *BootOption;
  UINT8                         *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL      *BootOptionDevicePath;
  UINTN                         BootOptionDevicePathSize;

  DevicePathSize = GetDevicePathSize (DevicePath);

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    return FALSE;
  }

  BootDevNum = BootOrderSize / sizeof (UINT16);
  for (Index = 0; Index < BootDevNum; Index++) {
    UnicodeSPrint (BootOptionName, sizeof (BootOptionName), L"Boot%04x", BootOrder[Index]);
    BootOption = BdsLibGetVariableAndSize (
                   BootOptionName,
                   &gEfiGlobalVariableGuid,
                   &VariableSize
                   );
    if (BootOption == NULL) {
      continue;
    }

    //
    // Find device path in Bootxxxx variable
    //
    WorkingPtr = BootOption;
    WorkingPtr += sizeof (UINT32);
    WorkingPtr += sizeof (UINT16);
    WorkingPtr += (UINTN) StrSize ((UINT16 *) WorkingPtr);
    BootOptionDevicePath     = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;
    BootOptionDevicePathSize = GetDevicePathSize (BootOptionDevicePath);

    if (DevicePathSize == BootOptionDevicePathSize &&
        CompareMem (DevicePath, BootOptionDevicePath, BootOptionDevicePathSize) == 0) {
      FreePool (BootOption);
      FreePool (BootOrder);
      return TRUE;
    }

    FreePool (BootOption);
  }

  FreePool (BootOrder);

  return FALSE;
}

/**
 Get the hash value of shell file

 @param[in]  Handle             FV handle value
 @param[out] HashValue          Output hash value of file

 @retval EFI_SUCCESS            Get hash value of file successfully.
 @retval EFI_ABORTED            Read file fail.
 @retval Other                  HandleProtocol or AllocatePage or CalculateCrc32 fail.
**/
EFI_STATUS
GetShellFileHashValue (
  IN  EFI_HANDLE                Handle,
  OUT UINT32                    *HashValue
  )
{
  EFI_STATUS                    Status;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINT8                         *File;
  UINTN                         Size;
  EFI_FV_FILETYPE               Type;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;
  UINTN                         PageNum;

  PageNum = 0;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  (VOID **) &Fv
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  File = NULL;
  Status = Fv->ReadFile (
                 Fv,
                 PcdGetPtr(PcdShellFile),
                 (VOID **) &File,
                 &Size,
                 &Type,
                 &Attributes,
                 &AuthenticationStatus
                 );
  if (Status == EFI_WARN_BUFFER_TOO_SMALL || Status == EFI_BUFFER_TOO_SMALL) {
    PageNum = EFI_SIZE_TO_PAGES(Size);
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiBootServicesData,
                    PageNum,
                    (EFI_PHYSICAL_ADDRESS *) &File
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Size = EFI_PAGES_TO_SIZE(PageNum);
    Status = Fv->ReadFile (
                   Fv,
                   PcdGetPtr(PcdShellFile),
                   (VOID **) &File,
                   &Size,
                   &Type,
                   &Attributes,
                   &AuthenticationStatus
                   );
    if (Status != EFI_SUCCESS) {
      gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) File, PageNum);
      return EFI_ABORTED;
    }
  }

  if (Status != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  Status = gBS->CalculateCrc32 (File, Size, HashValue);

  if (PageNum != 0) {
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) File, PageNum);
  } else {
    FreePool (File);
  }

  return Status;
}

/**
 Remove console out variable

 @param[in] OptionName           Boot option string
 @param[in] Attribute            Attributes of boot option
 @param[in] Description          Description of boot option
 @param[in] BootOptionHashValue  Hase value of shell boot option

 @retval EFI_SUCCESS             Success to update shell device path in BootXXXX variable.
 @retval EFI_NOT_FOUND           Can not find the corresponding device path by hash value.
 @retval Other                   Update variable fail.
**/
EFI_STATUS
UpdateShellDevicePath (
  IN CHAR16                            *OptionName,
  IN UINT32                            Attribute,
  IN CHAR16                            *Description,
  IN UINT32                            BootOptionHashValue
  )
{
  EFI_STATUS                           Status;
  UINTN                                Index;
  UINTN                                FvHandleCount;
  EFI_HANDLE                           *FvHandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH    ShellNode;
  UINT8                                OptionalData[SHELL_OPTIONAL_DATA_SIZE];
  UINT32                               HashValue;

  Status = EFI_NOT_FOUND;
  FvHandleCount = 0;
  FvHandleBuffer = NULL;

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiFirmwareVolume2ProtocolGuid,
         NULL,
         &FvHandleCount,
         &FvHandleBuffer
         );
  for (Index = 0; Index < FvHandleCount; Index++) {
    Status = GetShellFileHashValue (FvHandleBuffer[Index], &HashValue);
    if (EFI_ERROR (Status) || (BootOptionHashValue != HashValue)) {
      continue;
    }

    DevicePath = DevicePathFromHandle (FvHandleBuffer[Index]);
    EfiInitializeFwVolDevicepathNode (&ShellNode, PcdGetPtr(PcdShellFile));
    DevicePath = AppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &ShellNode);

    if (IsAlreadyInBootOrder (DevicePath)) {
      FreePool (DevicePath);
      continue;
    }

    gBS->CopyMem (OptionalData    , "RC"      , 2);
    gBS->CopyMem (&OptionalData[2], &HashValue, 4);

    Status = BdsLibUpdateOptionVar (
               OptionName,
               Attribute,
               DevicePath,
               Description,
               OptionalData,
               SHELL_OPTIONAL_DATA_SIZE
               );
    FreePool (DevicePath);
    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  if (FvHandleCount != 0) {
    FreePool (FvHandleBuffer);
  }

  if (Index == FvHandleCount) {
    return EFI_NOT_FOUND;
  }

  return Status;
}

/**
  Build the on flash shell boot option with the handle parsed in.

  @param[in]      Handle             The handle which present the device path to create on flash shell boot option
  @param[in, out] BdsBootOptionList  The header of the link list which indexed all current boot options. Deprecated.
  @param[in]      Description        Pointer to description string of boot option
**/
VOID
EFIAPI
BdsLibBuildOptionFromShell (
  IN EFI_HANDLE                        Handle,
  IN OUT LIST_ENTRY                    *BdsBootOptionList OPTIONAL,
  IN CHAR16                            *Description
  )
{
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH    ShellNode;
  EFI_STATUS                           Status;
  UINT32                               HashValue;
  UINT8                                OptionalData[SHELL_OPTIONAL_DATA_SIZE];
  EFI_DEVICE_PATH_PROTOCOL             *FilePath;
  CHAR16                               *CustomizedDescription;
  EFI_STATUS                           OemSvcStatus;

  DEBUG_CODE (
    if (BdsBootOptionList != NULL) {
      DEBUG ((DEBUG_INFO, "BdsLibBuildOptionFromShell(): BdsBootOptionList is deprecated. Please upgrade caller code to input NULL.\n"));
    }
  );

  FilePath              = NULL;
  CustomizedDescription = NULL;
  DevicePath = DevicePathFromHandle (Handle);

  //
  // Build the shell device path
  //
  EfiInitializeFwVolDevicepathNode (&ShellNode, PcdGetPtr(PcdShellFile));
  DevicePath = AppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &ShellNode);

  if (IsAlreadyInBootOrder (DevicePath)) {
    goto Exit;
  }

  FilePath = AllocateZeroPool (DevicePathNodeLength (&ShellNode) + END_DEVICE_PATH_LENGTH);
  if (FilePath != NULL) {
    CopyMem (FilePath, &ShellNode, DevicePathNodeLength (&ShellNode));
    SetDevicePathEndNode (NextDevicePathNode (FilePath));
  }

  CustomizedDescription = AllocateCopyPool (StrSize (Description), Description);
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDxeUpdateDescriptionOfBootOption \n"));
  OemSvcStatus = OemSvcDxeUpdateDescriptionOfBootOption (DevicePath, FilePath, NULL, &CustomizedDescription);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDxeUpdateDescriptionOfBootOption Status %r\n", OemSvcStatus));
  if (CustomizedDescription != NULL) {
    Description = CustomizedDescription;
  }

  Status = GetShellFileHashValue (Handle, &HashValue);
  if (EFI_ERROR(Status)) {
    BdsLibRegisterNewOption (NULL, DevicePath, Description, L"BootOrder", (UINT8 *) "RC", 2);
    goto Exit;
  }

  gBS->CopyMem (OptionalData    , "RC"                , 2);
  gBS->CopyMem (&OptionalData[2], (UINT8 *) &HashValue, 4);

  //
  // Create and register the shell boot option
  //
  BdsLibRegisterNewOption (NULL, DevicePath, Description, L"BootOrder", OptionalData, SHELL_OPTIONAL_DATA_SIZE);

Exit:
  FreePool (DevicePath);
  if (FilePath != NULL) {
    FreePool (FilePath);
  }
  if (CustomizedDescription != NULL) {
    FreePool (CustomizedDescription);
  }
}

/**
 Enumernate all shell boot options.

 @retval EFI_SUCCESS                 Enumernate all shell boot options success
**/
EFI_STATUS
EnumerateAllShellBootOption (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         FvHandleCount;
  EFI_HANDLE                    *FvHandleBuffer;
  UINTN                         Index;
  UINT16                        InternalShellNumber;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  EFI_FV_FILETYPE               Type;
  UINTN                         Size;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;
  CHAR16                        Buffer[128];

  gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiFirmwareVolume2ProtocolGuid,
        NULL,
        &FvHandleCount,
        &FvHandleBuffer
        );
  for (Index = 0, InternalShellNumber = 0; Index < FvHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    FvHandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = Fv->ReadFile (
                  Fv,
                  PcdGetPtr(PcdShellFile),
                  NULL,
                  &Size,
                  &Type,
                  &Attributes,
                  &AuthenticationStatus
                  );
    if (EFI_ERROR (Status)) {
      //
      // Skip if no shell file in the FV
      //
      continue;
    }
    //
    // Build the shell boot option
    //
    if (InternalShellNumber == 0) {
      UnicodeSPrint (Buffer, sizeof (Buffer), L"Internal EFI Shell");
    } else {
      UnicodeSPrint (Buffer, sizeof (Buffer), L"Internal EFI Shell %d", InternalShellNumber);
    }
    InternalShellNumber++;
    BdsLibBuildOptionFromShell (FvHandleBuffer[Index], NULL, Buffer);
  }

  if (FvHandleCount) {
    FreePool (FvHandleBuffer);
  }

  return EFI_SUCCESS;
}

/**
 Check if the input device path contains the query type and subtype or not.

 @param[in] DevicePath          Pointer to device path instance
 @param[in] QueryType           Query type of device path
 @param[in] QuerySubType        Query subtype of device path

 @retval TRUE                   Remove variable was success.
 @retval FALSE                  DevicePath doesn't contain the query type and subtype.
**/
BOOLEAN
CheckDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN UINT8                      QueryType,
  IN UINT8                      QuerySubType
  )
{
  BOOLEAN                       Match;
  EFI_DEVICE_PATH_PROTOCOL      *FullDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *WorkingDevicePath;

  Match = FALSE;

  if (DevicePath == NULL) {
    return FALSE;
  }

  FullDevicePath = BdsExpandPartitionPartialDevicePathToFull ((HARDDRIVE_DEVICE_PATH *) DevicePath);
  WorkingDevicePath = (FullDevicePath == NULL) ? DevicePath : FullDevicePath;

  while (!IsDevicePathEnd (WorkingDevicePath)) {
    if (DevicePathType (WorkingDevicePath) == QueryType &&
        DevicePathSubType (WorkingDevicePath) == QuerySubType) {
      Match = TRUE;
      break;
    }

    WorkingDevicePath = NextDevicePathNode (WorkingDevicePath);
  }

  if (FullDevicePath != NULL) {
    FreePool (FullDevicePath);
  }

  return Match;
}

/**
 Free the file path list.

 @param[in] FilePathList          Pointer to the file path list
 @param[in] FilePathCount         File path count
**/
STATIC
VOID
FreeFilePathList (
  IN EFI_DEVICE_PATH_PROTOCOL     **FilePathList,
  IN UINTN                        FilePathCount
  )
{
  UINTN                           Index;

  if (FilePathList == NULL) {
    return;
  }

  for (Index = 0; Index < FilePathCount; Index++) {
    FreePool (FilePathList[Index]);
  }
  FreePool (FilePathList);
}

/**
 Get whole file path from gH2OBdsDefaultBootListGenericOsTokenSpaceGuid token space PCD.

 This function allocates space for a pointer list which record the new copy of the device paths. If DevicePath is not
 found or allocate memory failed, then NULL is returned. The memory for the new device
 path is allocated from EFI boot services memory. It is the responsibility of the caller
 to free the memory allocated.

 @param[in]  Handle           A pointer to a device handle.
 @param[in]  TokenNum         Token number in gH2OBdsDefaultBootListGenericOsTokenSpaceGuid token space.
 @param[out] FilePathCount    Count of file path in the list.

 @return pointer to list of device path pointer or NULL if not found or allocate memory failed.
**/
EFI_DEVICE_PATH_PROTOCOL **
GetFilePathFromBootListGenericOsPcd (
  IN  EFI_HANDLE                Handle,
  IN  UINTN                     TokenNum,
  OUT UINTN                     *FilePathCount
  )
{
  CHAR16                        *GenericOs;
  CHAR16                        *FilePath;
  CHAR16                        *FilePathPtr;
  CHAR16                        *SingleFilePath;
  CHAR16                        *CpuPath;
  EFI_DEVICE_PATH_PROTOCOL      **FilePathList;
  EFI_DEVICE_PATH_PROTOCOL      *FullFilePath;
  UINTN                         Count;
  UINTN                         FilePathSize;
  UINTN                         Index;
  UINTN                         CurrentIndex;

  GenericOs = (CHAR16 *) LibPcdGetExPtr(&gH2OBdsDefaultBootListGenericOsTokenSpaceGuid, TokenNum);
  if (GenericOs == NULL) {
    return NULL;
  }

  //
  // paring result should be as below:
  // L"\\EFI\\android\\bootx64.efi\tAndroid"           -> "\\EFI\\android\\bootx64.efi"
  // L"\\EFI\\tencent\\grubx64.efi\ttencent\tNORMAL"   -> "\\EFI\\tencent\\grubx64.efi"
  // L"\\EFI\\\tencent\\grubx64.efi\ttencent\tNORMAL"  -> "\\EFI\\"
  //

  for (Index = 0; ; Index++) {
    if (GenericOs[Index] == L'\0') {
      return NULL;
    }
    if (GenericOs[Index] == L'\\' && GenericOs[Index + 1] == L't') {
      break;
    }
    if (GenericOs[Index] == L'\\' && GenericOs[Index + 1] == L'\\') {
      Index += 1;
      continue;
    }
  }
  FilePathSize = (Index + 1) * sizeof (CHAR16);
  FilePath = AllocateZeroPool (FilePathSize);
  if (FilePath == NULL) {
    return NULL;
  }
  CopyMem (FilePath, GenericOs, FilePathSize - sizeof (CHAR16));
  if (sizeof (ARCHITECTURE_NAME) > sizeof (L"$cpu$")) {
    Index = 0;
    CpuPath = FilePath;
    while ((CpuPath = StrStr (CpuPath, L"$cpu$")) != NULL) {
      Index++;
      CpuPath++;
    }
    if (Index != 0) {
      FilePath = ReallocatePool (
                   FilePathSize,
                   FilePathSize + Index * (StrSize (ARCHITECTURE_NAME) - StrSize (L"$cpu$")),
                   FilePath
                   );
      if (FilePath == NULL) {
        return NULL;
      }
    }

  }
  //
  // Convert string "\\" to string "\"
  //
  for (Index = 0, CurrentIndex = 0; FilePath[Index] != L'\0'; Index++, CurrentIndex++) {
    if (FilePath[Index] ==  L'\\' && FilePath[Index + 1] ==  L'\\') {
      Index++;
    }
    FilePath[CurrentIndex] = FilePath[Index];
  }
  FilePath[CurrentIndex] = L'\0';
  CpuPath = FilePath;
  while ((CpuPath = StrStr (CpuPath, L"$cpu$")) != NULL) {
    if (sizeof (ARCHITECTURE_NAME) > sizeof (L"$cpu$")) {
      CopyMem (
        CpuPath + StrLen (ARCHITECTURE_NAME),
        CpuPath + StrLen (L"$cpu$"),
        StrSize (CpuPath) - sizeof (L"$cpu$") + sizeof (CHAR16)
        );
      CopyMem (CpuPath, ARCHITECTURE_NAME, StrLen (ARCHITECTURE_NAME) * sizeof (CHAR16));
    } else {
      CopyMem (CpuPath, ARCHITECTURE_NAME, StrLen (ARCHITECTURE_NAME) * sizeof (CHAR16));
      CopyMem (
        CpuPath + StrLen (ARCHITECTURE_NAME),
        CpuPath + StrLen (L"$cpu$"),
        StrSize (CpuPath) - sizeof (L"$cpu$") + sizeof (CHAR16)
        );
    }
  }

  //
  // Parsing file path string to get each file path which is delimited by '|' character.
  // Create a list to record all file path.
  //
  Count        = 0;
  FilePathList = NULL;
  FilePathPtr  = FilePath;
  while (FilePathPtr != NULL) {
    SingleFilePath = FilePathPtr;

    FilePathPtr = StrStr (FilePathPtr, L"|");
    if (FilePathPtr != NULL) {
      *FilePathPtr = L'\0';
      FilePathPtr++;
    }

    if (*SingleFilePath == L'\0') {
      continue;
    }

    FullFilePath = FileDevicePath (Handle, SingleFilePath);
    if (FullFilePath == NULL) {
      continue;
    }

    FilePathList = ReallocatePool (
                     sizeof(EFI_DEVICE_PATH_PROTOCOL **) * Count,
                     sizeof(EFI_DEVICE_PATH_PROTOCOL **) * (Count + 1),
                     FilePathList
                     );
    if (FilePathList == NULL) {
      return NULL;
    }

    FilePathList[Count++] = FullFilePath;
  }

  FreePool (FilePath);

  *FilePathCount = Count;
  return FilePathList;

}

/**
 Get OS description string from gH2OBdsDefaultBootListGenericOsTokenSpaceGuid token space PCD.

 This function allocates space for OS description string. If description is not found or allocate
 memory failed, then NULL is returned. The memory for the description is allocated from EFI boot
 services memory. It is the responsibility of the caller to free the memory allocated.

 @param[in] TokenNum     Token number in gH2OBdsDefaultBootListGenericOsTokenSpaceGuid token space.

 @return pointer to description or NULL if not found.
**/
STATIC
CHAR16 *
GetDescriptionFromBootListGenericOsPcd (
  IN UINTN                      TokenNum
  )
{
  CHAR16                        *GenericOs;
  CHAR16                        *TempPtr;
  CHAR16                        *Description;
  UINTN                         DescriptionSize;
  UINTN                         Index;

  GenericOs = (CHAR16 *) LibPcdGetExPtr(&gH2OBdsDefaultBootListGenericOsTokenSpaceGuid, TokenNum);
  if (GenericOs == NULL) {
    return NULL;
  }

  for (Index = 0; !(GenericOs[Index] == L'\\' && GenericOs[Index + 1] == L't'); Index++) {
    if (GenericOs[Index] == L'\0') {
      return NULL;
    }
  }
  TempPtr = &GenericOs[Index];
  Index++;
  while (!(GenericOs[Index] == L'\\' && GenericOs[Index + 1] == L't') && GenericOs[Index] != L'\0') {
    Index++;
  }

  DescriptionSize = (UINTN) (&GenericOs[Index]) - (UINTN) TempPtr - sizeof (CHAR16);
  Description = AllocateZeroPool (DescriptionSize);
  if (Description == NULL) {
    return NULL;
  }
  CopyMem (Description, TempPtr + 2, DescriptionSize - sizeof (CHAR16));
  return Description;
}

/**
 Get OS attribute flag from from gH2OBdsDefaultBootListGenericOsTokenSpaceGuid token space PCD.

 @param[in] TokenNum            Token number in gH2OBdsDefaultBootListGenericOsTokenSpaceGuid token space.

 @retval SECURE_BOOT_ENABLED    The generic OS PCD is for secure boot enabled or doesn't specific
                                secure is enabled or not.
 @retval SECURE_BOOT_DISABLED   The generic OS PCD is for secure boot disabled.
 @retval UNKNOWN_SECURE_SETTING Cannot find generic OS from input token.
**/
STATIC
UINTN
GetBootFlagFromBootListGenericOsPcd (
  IN UINTN                       TokenNum
  )
{
  CHAR16                         *GenericOs;
  UINTN                          Index;
  UINTN                          TabIndex;

  GenericOs = (CHAR16 *) LibPcdGetExPtr(&gH2OBdsDefaultBootListGenericOsTokenSpaceGuid, TokenNum);
  if (GenericOs == NULL) {
    return UNKNOWN_SECURE_SETTING;
  }

  Index = 0;
  for (TabIndex = 0; TabIndex < MAX_BIOS_ENVIRONMENT; TabIndex++) {
    for ( ;!(GenericOs[Index] == L'\\' && GenericOs[Index + 1] == L't'); Index++) {
      if (GenericOs[Index] == L'\0') {
        return SECURE_BOOT_ENABLED;
      }
    }
    Index += 2;
  }
  return StrCmp (&GenericOs[Index], L"SECURE") == 0 ? SECURE_BOOT_ENABLED : SECURE_BOOT_DISABLED;
}

/**
 Enumerate all PCDs in gH2OBdsDefaultBootListGenericOsTokenSpaceGuid to find out the PCD token number which
 input device path match the file path list.

 @param[in] DevPath             Pointer to device path

 @return PCD token number or zero if PCD is not found or DevPath is NULL.
**/
UINTN
GetGenericOsPcdByFilePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevPath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *DevPathNode;
  UINTN                         Index;
  EFI_DEVICE_PATH_PROTOCOL      **FilePathList;
  UINTN                         FilePathCount;
  UINTN                         FilePathIndex;
  UINTN                         TokenNum;

  if (DevPath == NULL) {
    return 0;
  }

  for (DevPathNode = DevPath; !IsDevicePathEnd (DevPathNode); DevPathNode = NextDevicePathNode (DevPathNode)) {
    if ((DevicePathType (DevPathNode) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (DevPathNode) == MEDIA_FILEPATH_DP)) {
      break;
    }
  }
  if (IsDevicePathEnd (DevPathNode)) {
    return 0;
  }

  for (Index = 0; Index < MAX_BIOS_ENVIRONMENT; Index++) {
    for (TokenNum = LibPcdGetNextToken(&gH2OBdsDefaultBootListGenericOsTokenSpaceGuid, 0);
         TokenNum != 0;
         TokenNum = LibPcdGetNextToken(&gH2OBdsDefaultBootListGenericOsTokenSpaceGuid, TokenNum)) {
      if (GetBootFlagFromBootListGenericOsPcd (TokenNum) != Index) {
        continue;
      }

      FilePathList = GetFilePathFromBootListGenericOsPcd (NULL, TokenNum, &FilePathCount);
      if (FilePathList == NULL) {
        continue;
      }

      for (FilePathIndex = 0; FilePathIndex < FilePathCount; FilePathIndex++) {
        if (BdsLibMatchFilePathDevicePathNode (FilePathList[FilePathIndex], DevPathNode)) {
          FreeFilePathList (FilePathList, FilePathCount);
          return TokenNum;
        }
      }

      FreeFilePathList (FilePathList, FilePathCount);
    }
  }

  return 0;
}

/**
 According to block IO and device path protocol which installed on the input handle to get media type

 If input handle doesn't contain block IO or device path protocol this function will return MaxMediaType
 to indicate we cannot recognize the media type.

 @param[in] Handle   The handle which contains block IO and device path protocol.

 @return   Media type
**/
STATIC
BDS_MEDIA_TYPE
GetMediaType (
  IN EFI_HANDLE                 Handle
  )
{
  EFI_STATUS                    Status;
  EFI_BLOCK_IO_PROTOCOL         *BlkIo;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  UINTN                         DevicePathType;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiBlockIoProtocolGuid,
                  (VOID **) &BlkIo
                  );
  if (Status != EFI_SUCCESS) {
    return MaxMediaType;
  }
  DevicePath = DevicePathFromHandle (Handle);
  if (DevicePath == NULL) {
    return MaxMediaType;
  }

  DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
  if (!BlkIo->Media->RemovableMedia &&
      DevicePathType != BDS_EFI_MESSAGE_USB_DEVICE_BOOT &&
      CheckDevicePath (DevicePath, MEDIA_DEVICE_PATH, MEDIA_HARDDRIVE_DP)) {
    return NonRemovableHdd;
  } else if (DevicePathType == BDS_EFI_MESSAGE_USB_DEVICE_BOOT &&
             CheckDevicePath (DevicePath, MEDIA_DEVICE_PATH, MEDIA_HARDDRIVE_DP)) {
    return UsbHdd;
  } else {
    return MaxMediaType;
  }
}

/**
 According to block IO and device path protocol which installed on the input handle to determine
 this is non-removable HDD or not

 If input handle doesn't contain block IO or device path protocol this function will return FALSE
 to indicate this media isn't a non-removable HDD.

 @param[in] Handle   The handle which contains block IO and device path protocol.

 @retval  TRUE       The media is a non-removable HDD.
 @retval  FALSE      The media isn't a removable HDD.
**/
BOOLEAN
IsNonRemovableHdd (
  IN EFI_HANDLE                 Handle
  )
{
  return GetMediaType (Handle) == NonRemovableHdd;
}

/**
  Internal function to check whether the media can install generic UEFI OS.

  @param[in] Handle   The handle which contains block IO and device path protocol.

  @retval  TRUE       The media can install generic UEFI OS.
  @retval  FALSE      The media cannot install generic UEFI OS.
**/
STATIC
BOOLEAN
IsGenericUefiOsMedia (
  IN EFI_HANDLE                 Handle
  )
{
  BDS_MEDIA_TYPE                MediaType;

  MediaType = GetMediaType (Handle);
  if (MediaType == NonRemovableHdd || MediaType == UsbHdd) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
 Check if it is a generic UEFI bootable OS or not.

 @param[in] Handle              A pointer to a device handle.
 @param[in] WindowsToGoEnabled  If True, Windows to Go is enabled.
                                If FALSE, Windows to Go is disabled.

 @retval TRUE                   This is a generic UEFI Bootable OS.
 @retval FALSE                  This is not a generic UEFI Bootable OS.
**/
BOOLEAN
IsGenericUefiBootOs (
  IN EFI_HANDLE                 Handle,
  IN BOOLEAN                    WindowsToGoEnabled
  )
{
  EFI_STATUS                    Status;
  UINTN                         TokenNum;
  UINTN                         Index;
  UINTN                         FilePathCount;
  EFI_DEVICE_PATH_PROTOCOL      **FilePath;
  EFI_FILE_HANDLE               FileHandle;

  if (!IsGenericUefiOsMedia (Handle)) {
    return FALSE;
  }

  for (TokenNum = LibPcdGetNextToken(&gH2OBdsDefaultBootListGenericOsTokenSpaceGuid, 0); TokenNum != 0; \
       TokenNum = LibPcdGetNextToken(&gH2OBdsDefaultBootListGenericOsTokenSpaceGuid, TokenNum)) {
    if (!WindowsToGoEnabled && PcdToken (MicrosoftOS) == TokenNum && GetMediaType (Handle) == UsbHdd) {
      continue;
    }

    FilePath = GetFilePathFromBootListGenericOsPcd (Handle, TokenNum, &FilePathCount);
    if (FilePath == NULL) {
      continue;
    }

    for (Index = 0; Index < FilePathCount; Index++) {
      Status = BdsLibOpenFileFromDevicePath (
                 FilePath[Index],
                 EFI_FILE_MODE_READ,
                 0,
                 &FileHandle
                 );
      if (Status == EFI_SUCCESS) {
        if (FileHandle != NULL) {
          FileHandle->Close (FileHandle);
        }
        FreeFilePathList (FilePath, FilePathCount);
        return TRUE;
      }
    }
    FreeFilePathList (FilePath, FilePathCount);
  }

  return FALSE;
}

/**
 Check whether the input device path is in the file path list of input PCD.

 @param[in] PcdTokenNum         PCD token number in gH2OBdsDefaultBootListGenericOsTokenSpaceGuid.
 @param[in] DevPath             Pointer to device path

 @retval TRUE                   The input device path is in the file path list of input PCD.
 @retval FALSE                  The input device path is not in the file path list of input PCD.
**/
BOOLEAN
IsFilePathInGenericOsPcd (
  IN UINTN                      PcdTokenNum,
  IN EFI_DEVICE_PATH_PROTOCOL   *DevPath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      **FilePathList;
  UINTN                         FilePathCount;
  UINTN                         FilePathIndex;

  if (DevPath == NULL || PcdTokenNum == 0) {
    return FALSE;
  }

  FilePathList = GetFilePathFromBootListGenericOsPcd (NULL, PcdTokenNum, &FilePathCount);
  if (FilePathList == NULL) {
    return FALSE;
  }

  for (FilePathIndex = 0; FilePathIndex < FilePathCount; FilePathIndex++) {
    if (BdsLibMatchFilePathDevicePathNode (FilePathList[FilePathIndex], DevPath)) {
      FreeFilePathList (FilePathList, FilePathCount);
      return TRUE;
    }
  }

  FreeFilePathList (FilePathList, FilePathCount);
  return FALSE;
}

/**
 Check if the hard drive file path is specified in the boot option variable or not.

 @param[in] FilePath              A pointer to hard drive file path

 @retval TRUE                     The hard drive file path is specified in the boot option variable
 @retval FALSE                    The hard drive file path is not specified in the boot option variable
**/
STATIC
BOOLEAN
IsHardDriveFilePathInBootOptionVariables (
  IN EFI_DEVICE_PATH_PROTOCOL     *FilePath
  )
{
  UINT16                          *BootOrder;
  UINTN                           BootOrderSize;
  UINTN                           BootDevNum;
  UINTN                           Index;
  CHAR16                          OptionName[10];
  UINT8                           *OptionPtr;
  UINTN                           OptionSize;
  UINT8                           *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL        *OptionDevicePath;

  BootOrder = BdsLibGetVariableAndSize (L"BootOrder", &gEfiGlobalVariableGuid, &BootOrderSize);
  if (BootOrder == NULL) {
    return FALSE;
  }
  BootDevNum = BootOrderSize / sizeof (UINT16);

  for (Index = 0; Index < BootDevNum; Index++) {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrder[Index]);
    OptionPtr = BdsLibGetVariableAndSize (OptionName, &gEfiGlobalVariableGuid, &OptionSize);
    if (OptionPtr == NULL) {
      continue;
    }

    TempPtr = OptionPtr;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

    if (MatchPartitionDevicePathNode (OptionDevicePath, (HARDDRIVE_DEVICE_PATH *) FilePath) &&
        BdsLibMatchFilePathDevicePathNode (OptionDevicePath, FilePath)) {
      FreePool (OptionPtr);
      FreePool (BootOrder);
      return TRUE;
    }

    FreePool (OptionPtr);
  }

  FreePool (BootOrder);
  return FALSE;
}

/**
 Check whether the input device path is a generic UEFI OS boot option or not.

 @param[in] OptionDevicePath      Pointer to device path of boot option

 @retval TRUE    The input device path is a generic UEFI OS boot option
 @retval FALSE   The input device path is not a generic UEFI OS boot option
**/
BOOLEAN
IsGenericUefiBootOsDevPath (
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

  if ((DevicePathType (LastDeviceNode) == MEDIA_DEVICE_PATH) &&
      (DevicePathSubType (LastDeviceNode) == MEDIA_FILEPATH_DP)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Create generic UEFI bootable OS.

 @param[in] Handle              A pointer to a device handle.


 @retval EFI_SUCCESS            Create generic UEFI OS success
 @retval Other                  Register new boot option fail
**/
EFI_STATUS
CreateGenericUefiBootOs (
  IN  EFI_HANDLE                Handle
  )
{
  UINTN                         Index;
  UINTN                         TokenNum;
  EFI_DEVICE_PATH_PROTOCOL      **FilePath;
  UINTN                         FilePathCount;
  UINTN                         FilePathIndex;
  EFI_FILE_HANDLE               FileHandle;
  EFI_STATUS                    Status;
  EFI_STATUS                    RegisterStatus;
  EFI_DEVICE_PATH_PROTOCOL      *TempFilePath;
  EFI_DEVICE_PATH_PROTOCOL      *OptionFilePath;
  CHAR16                        *Description;
  CHAR16                        **Descriptions;
  UINTN                         DescriptionCount;
  UINTN                         AddedSecureCount;
  UINTN                         OsIndex;
  EFI_STATUS                    OemSvcStatus;

  RegisterStatus = EFI_SUCCESS;

  DescriptionCount = 0;
  for (TokenNum = LibPcdGetNextToken(&gH2OBdsDefaultBootListGenericOsTokenSpaceGuid, 0); TokenNum != 0; \
       TokenNum = LibPcdGetNextToken(&gH2OBdsDefaultBootListGenericOsTokenSpaceGuid, TokenNum)) {
    DescriptionCount++;
  }

  Descriptions = AllocateZeroPool (DescriptionCount * sizeof (CHAR16 *));
  if (Descriptions == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  AddedSecureCount = 0;
  for (Index = 0; Index < MAX_BIOS_ENVIRONMENT; Index++) {
    for (TokenNum = LibPcdGetNextToken(&gH2OBdsDefaultBootListGenericOsTokenSpaceGuid, 0); TokenNum != 0; \
         TokenNum = LibPcdGetNextToken(&gH2OBdsDefaultBootListGenericOsTokenSpaceGuid, TokenNum)) {
      if (GetBootFlagFromBootListGenericOsPcd (TokenNum) != Index) {
        continue;
      }

      FilePath = GetFilePathFromBootListGenericOsPcd (Handle, TokenNum, &FilePathCount);
      if (FilePath == NULL) {
        continue;
      }

      //
      // Check all file paths in PCD. If the file exist and is defined in Boot#### variable, use this file path to register boot option.
      // Otherwise, use the first one found file path to register boot option.
      //
      OptionFilePath = NULL;
      for (FilePathIndex = 0; FilePathIndex < FilePathCount; FilePathIndex++) {
        Status = BdsLibOpenFileFromDevicePath (
                   FilePath[FilePathIndex],
                   EFI_FILE_MODE_READ,
                   0,
                   &FileHandle
                   );
        if (Status == EFI_SUCCESS) {
          //
          // If open generic OS loader file success, register a new boot option.
          //
          if (FileHandle != NULL) {
            FileHandle->Close (FileHandle);
          }

          TempFilePath = FilePath[FilePathIndex];
          while (!IsDevicePathEnd (TempFilePath) &&
                 (TempFilePath->Type != MEDIA_DEVICE_PATH || TempFilePath->SubType != MEDIA_HARDDRIVE_DP)) {
            TempFilePath = NextDevicePathNode (TempFilePath);
          }
          if (IsDevicePathEnd (TempFilePath)) {
            continue;
          }

          if (OptionFilePath == NULL) {
            OptionFilePath = AllocateCopyPool (GetDevicePathSize (TempFilePath), TempFilePath);
            continue;
          }

          if (IsHardDriveFilePathInBootOptionVariables (TempFilePath)) {
            FreePool (OptionFilePath);
            OptionFilePath = AllocateCopyPool (GetDevicePathSize (TempFilePath), TempFilePath);
            break;
          }
        }
      }
      FreeFilePathList (FilePath, FilePathCount);
      if (OptionFilePath == NULL) {
        continue;
      }

      Description = GetDescriptionFromBootListGenericOsPcd (TokenNum);
      if (Description == NULL) {
        continue;
      }
      //
      // Don't create Microsoft OS which installed on removable media. They will handle by
      // windows to go mechanism.
      //
      if (PcdToken (MicrosoftOS) == TokenNum && !IsNonRemovableHdd (Handle)) {
        continue;
      }
      if (Index == SECURE_BOOT_ENABLED) {
        //
        // Record added OS for secure boot enabled.
        //
        Descriptions[AddedSecureCount++] = AllocateCopyPool (StrSize (Description), Description);
      } else {
        //
        // Don't add OS boot option for secure boot disabled if corresponding option for secure boot enabled is added
        //
        for (OsIndex = 0; OsIndex < AddedSecureCount; OsIndex++) {
          if (StrCmp (Descriptions[OsIndex], Description) == 0) {
            break;
          }
        }
        if (OsIndex != AddedSecureCount) {
          continue;
        }
      }

      DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDxeUpdateDescriptionOfBootOption \n"));
      OemSvcStatus = OemSvcDxeUpdateDescriptionOfBootOption (DevicePathFromHandle (Handle), OptionFilePath, NULL, &Description);
      DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDxeUpdateDescriptionOfBootOption Status: %r\n", OemSvcStatus));

      Status = BdsLibRegisterNewOption (
                 NULL,
                 OptionFilePath,
                 Description,
                 L"BootOrder",
                 (UINT8 *) "RC",
                 2
                 );
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) {
        RegisterStatus = Status;
      }
      FreePool (Description);
      FreePool (OptionFilePath);
    }
  }

  //
  // Free allocated strings
  //
  for (OsIndex = 0; OsIndex < AddedSecureCount; OsIndex++) {
    FreePool (Descriptions[OsIndex]);
  }
  FreePool (Descriptions);
  return RegisterStatus;
}

/**
 Initalize module device type information.
**/
STATIC
VOID
InitDeviceTypeInfo (
  VOID
  )
{
  UINTN                         TableCount;
  UINTN                         Index;

  TableCount = sizeof (mDeviceTypeInfo) / sizeof (DEVICE_TYPE_INFORMATION);
  for (Index = 0; Index < TableCount; Index++) {
    mDeviceTypeInfo[Index].DeviceNum = 0;
  }
}

/**
 Check whether two boot options are the same or not.

 @param[in] FirstBootOption       Pointer to first boot option data.
 @param[in] FirstBootOptionSize   First boot option data size in bytes.
 @param[in] SecondBootOption      Pointer to second boot option data.
 @param[in] SecondBootOptionSize  Second boot option data size in bytes.

 @retval TRUE                     Two boot options are the same.
 @retval FALSE                    Two boot options aren't the same.
**/
BOOLEAN
IsSameBootOption (
  IN UINT8                      *FirstBootOption,
  IN UINTN                      FirstBootOptionSize,
  IN UINT8                      *SecondBootOption,
  IN UINTN                      SecondBootOptionSize
  )
{
  CHAR16                        *FirstDescription;
  CHAR16                        *SecondeDescription;
  UINTN                         FirstDevicePathOffset;
  UINTN                         SecondDevicePathOffset;
  UINTN                         TableCount;
  UINTN                         Index;
  UINTN                         CompareResult;
  CHAR16                        *DeviceStr;


  //
  // Whole boot option data are the indentical indicates these two boot options are the same.
  //
  if (FirstBootOptionSize == SecondBootOptionSize &&
    CompareMem (FirstBootOption, SecondBootOption, FirstBootOptionSize) == 0) {
    return TRUE;
  }
  //
  // return FALSE if device path or optional data in boot option are different.
  //
  FirstDescription       = (CHAR16 *) (FirstBootOption + sizeof (UINT32) + sizeof (UINT16));
  FirstDevicePathOffset  = sizeof (UINT32) + sizeof (UINT16) + StrSize (FirstDescription);
  SecondeDescription     = (CHAR16 *) (SecondBootOption + sizeof (UINT32) + sizeof (UINT16));
  SecondDevicePathOffset = sizeof (UINT32) + sizeof (UINT16) + StrSize (SecondeDescription);
  if (FirstBootOptionSize - FirstDevicePathOffset != SecondBootOptionSize - SecondDevicePathOffset) {
    return FALSE;
  }
  CompareResult = CompareMem (
                    FirstBootOption + FirstDevicePathOffset,
                    SecondBootOption + SecondDevicePathOffset,
                    FirstBootOptionSize - FirstDevicePathOffset
                    );
  if (CompareResult != 0) {
    return FALSE;
  }

  //
  // Only return TRUE if the input description is the same type and created by BIOS.
  //
  TableCount = sizeof (mDeviceTypeInfo) / sizeof (DEVICE_TYPE_INFORMATION);
  for (Index = 0; Index < TableCount; Index++) {
    DeviceStr = BdsLibGetStringById (mDeviceTypeInfo[Index].StringToken);
    ASSERT (DeviceStr != NULL);
    if (DeviceStr == NULL) {
      continue;
    }
    if (StrnCmp (FirstDescription, DeviceStr, StrLen (DeviceStr)) == 0 &&
        StrnCmp (SecondeDescription, DeviceStr, StrLen (DeviceStr)) == 0) {
      FreePool (DeviceStr);
      return TRUE;
    }
    FreePool (DeviceStr);
  }
  return FALSE;
}

/**
 Check if it has the default removable file or not.

 @param[in] Handle     A pointer to a device handle.

 @retval TRUE          It has the default removable file.
 @retval FALSE         It does not has the default removable file.
**/
BOOLEAN
HaveDefaultRemovableFile (
  IN EFI_HANDLE                 Handle
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;
  EFI_FILE_HANDLE               FileHandle;
  EFI_STATUS                    Status;

  FilePath = FileDevicePath (Handle, EFI_REMOVABLE_MEDIA_FILE_NAME);
  Status = BdsLibOpenFileFromDevicePath (
             FilePath,
             EFI_FILE_MODE_READ,
             0,
             &FileHandle
             );
  FreePool (FilePath);
  if (Status == EFI_SUCCESS) {
    if (FileHandle != NULL) {
      FileHandle->Close (FileHandle);
    }
    return TRUE;
  }

  return FALSE;
}

/**
 Check whether the input device path is a removable boot option or not.

 @param[in] OptionDevicePath      Pointer to device path of boot option

 @retval TRUE    The input device path is a removable boot option
 @retval FALSE   The input device path is not a removable boot option
**/
BOOLEAN
IsValidRemovableBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_STATUS                    Status;
  EFI_HANDLE                    Handle;

  TempDevicePath = OptionDevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &TempDevicePath,
                  &Handle
                  );
  if (!EFI_ERROR (Status) && IsDevicePathEnd (TempDevicePath)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Create removable boot option form according to input handle.

 @param[in]     Handle             The handle which present the device path to create
                                   boot option.

 @retval EFI_SUCCESS               Create removable boot option successful.
 @retval EFI_NOT_FOUND             Cannot find device path from input handle.
**/
STATIC
EFI_STATUS
CreateRemovableBootOption (
  IN     EFI_HANDLE             Handle
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  UINTN                         DevicePathType;
  HARDWARE_BOOT_DEVICE_INFO     *HwBootDeviceInfo;
  UINTN                         HwBootDeviceCount;
  UINTN                         HwNum;
  UINTN                         TableCount;
  UINTN                         Index;
  CHAR16                        Buffer[128];
  CHAR16                        *DeviceStr;
  UINT32                        Attribute;
  EFI_STATUS                    Status;
  EFI_BLOCK_IO_PROTOCOL         *BlkIo;

  Attribute = 0;
  Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **) &BlkIo);
  if (!EFI_ERROR (Status)) {
    Attribute |= (BlkIo->Media->RemovableMedia ? ATTR_REMOVABLE_DEV : ATTR_UNREMOVABLE_DEV);
  }

  DevicePath = DevicePathFromHandle (Handle);
  if (DevicePath == NULL) {
    return EFI_NOT_FOUND;
  }

  ZeroMem (Buffer, sizeof (Buffer));
  DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
  TableCount = sizeof (mDeviceTypeInfo) / sizeof (DEVICE_TYPE_INFORMATION);
  //
  // Get Device type string.
  //
  for (Index = 0; Index < TableCount; Index++) {
    if ((DevicePathType == mDeviceTypeInfo[Index].DevicePathType) &&
        (Attribute      == (mDeviceTypeInfo[Index].Attribute & Attribute))) {
      DeviceStr = BdsLibGetStringById (mDeviceTypeInfo[Index].StringToken);
      ASSERT (DeviceStr != NULL);
      if (DeviceStr == NULL) {
        continue;
      }
      if (mDeviceTypeInfo[Index].DeviceNum == 0) {
        UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", DeviceStr);
      } else {
        UnicodeSPrint (
          Buffer,
          sizeof (Buffer),
          L"%s %d",
          DeviceStr,
          mDeviceTypeInfo[Index].DeviceNum
          );
      }
      FreePool (DeviceStr);
      mDeviceTypeInfo[Index].DeviceNum++;
      break;
    }
  }

  if (Index == TableCount) {
    BdsLibBuildOptionFromHandle (Handle, NULL, NULL);
  } else {
    //
    // Get hardware device name for current boot option.
    //
    HwBootDeviceCount = 0;
    BdsLibGetAllHwBootDeviceInfo (&HwBootDeviceCount, &HwBootDeviceInfo);
    for (HwNum = 0; HwNum < HwBootDeviceCount; HwNum++) {
      if (HwBootDeviceInfo != NULL && BdsLibCompareBlockIoDevicePath (HwBootDeviceInfo[HwNum].BlockIoDevicePath, DevicePath)) {
        break;
      }
    }
    if (HwNum == HwBootDeviceCount || HwBootDeviceInfo == NULL ) {
      BdsLibBuildOptionFromHandle (Handle, NULL, Buffer);
    } else {
      UnicodeSPrint (Buffer, sizeof(Buffer), L"%s (%s)", Buffer, HwBootDeviceInfo[HwNum].HwDeviceName);
      BdsLibBuildOptionFromHandle (Handle, NULL, Buffer);
    }
    //
    // Free ardware device name.
    //
    if (HwBootDeviceCount != 0 && HwBootDeviceInfo != NULL ) {
      for (Index = 0; Index < HwBootDeviceCount; Index++) {
        FreePool (HwBootDeviceInfo[Index].HwDeviceName);
      }
      FreePool (HwBootDeviceInfo);
    }
  }

  return EFI_SUCCESS;
}

/**
 Delete boot option which device path and input device path is matched

 @param[in] DevicePath    Pointer to device path instance

 @retval EFI_SUCCESS      Finished all the boot device enumerate and create
                          the boot option base on that boot device
 @retval Other
**/
STATIC
EFI_STATUS
DeleteRecoveryOption (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  UINT16                        *BootOrder;
  UINT8                         *BootOptionVar;
  UINTN                         BootOrderSize;
  UINTN                         BootOptionSize;
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINT16                        BootOption[BOOT_OPTION_MAX_CHAR];
  EFI_DEVICE_PATH_PROTOCOL      *OptionDevicePath;
  UINTN                         OptionDevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL      *ExpandedDevPath;
  BOOLEAN                       Found;
  UINT8                         *TempPtr;
  CHAR16                        *Description;
  UINTN                         DevPathSize;

  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    return EFI_NOT_FOUND;
  }

  DevPathSize = GetDevicePathSize (DevicePath);
  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      //
      //If the Boot Device is not exit, we should dynamically adjust the BootOrder
      //
      BdsLibUpdateInvalidBootOrder (&BootOrder, Index, &BootOrderSize);
      if (BootOrder == NULL) {
        break;
      }
      continue;
    }

    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32);
    OptionDevicePathSize = (UINTN) (*((UINT16 *) TempPtr));
    TempPtr += sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

    ExpandedDevPath = IS_USB_SHORT_FORM_DEVICE_PATH(OptionDevicePath) ? BdsLibExpandUsbShortFormDevPath (OptionDevicePath) : NULL;
    if (ExpandedDevPath != NULL) {
      OptionDevicePath     = ExpandedDevPath;
      OptionDevicePathSize = GetDevicePathSize (ExpandedDevPath);
    }

    if (OptionDevicePathSize == DevPathSize && CompareMem (OptionDevicePath, DevicePath, OptionDevicePathSize) == 0) {
      Found = TRUE;
    } else {
      Found = FALSE;
    }

    FreePool (BootOptionVar);
    if (ExpandedDevPath != NULL) {
      FreePool (ExpandedDevPath);
    }

    if (Found) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      continue;
    }
    Index++;
  }

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );

  if (BootOrder != NULL) {
    FreePool (BootOrder);
  }
  return Status;
}

/**
 Enumernate all simple file system boot options.

 @param[in]      WindowsToGoBootVarExist   Flag to indicate Windows To Go boot option is existence or not
 @param[in]      IsUsbBootSupported        Flag to indicate USB boot is supported or not

 @retval EFI_SUCCESS                       Enumernate all simple file system boot options success
**/
EFI_STATUS
EnumerateAllSimpleFileSysBootOption (
  IN     BOOLEAN                WindowsToGoBootVarExist,
  IN     BOOLEAN                IsUsbBootSupported
  )
{
  EFI_STATUS                    Status;
  UINTN                         NumberFileSystemHandles;
  EFI_HANDLE                    *FileSystemHandles;
  UINTN                         Size;
  OPROM_STORAGE_DEVICE_INFO     *OpromStorageDev;
  UINTN                         OpromStorageDevCount;
  UINT8                         *DisableOpromStorageDevBoot;
  UINTN                         Index;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_BLOCK_IO_PROTOCOL         *BlkIo;

  InitDeviceTypeInfo ();

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberFileSystemHandles,
                  &FileSystemHandles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  OpromStorageDev = NULL;
  OpromStorageDevCount = 0;
  DisableOpromStorageDevBoot = BdsLibGetVariableAndSize (
                                 L"DisableOpromStorageDevBoot",
                                 &gEfiGenericVariableGuid,
                                 &Size
                                 );
  if (DisableOpromStorageDevBoot != NULL) {
    BdsLibGetOpromStorageDevInfo (&OpromStorageDev, &OpromStorageDevCount);
  }

  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    DevicePath = DevicePathFromHandle (FileSystemHandles[Index]);

    //
    // If need, skip the option ROM storage device.
    //
    if (IsOpromStorageDev (DevicePath, OpromStorageDev, OpromStorageDevCount)) {
      continue;
    }

    //
    // If system doesn't support USB boot, needn't add USB EFI boot option
    //
    if (!IsUsbBootSupported && IsUsbDevicePath (DevicePath)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    FileSystemHandles[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlkIo
                    );
    if (!EFI_ERROR (Status)) {
      //
      // If find UEFI OS option in unremovable device, delete exist recovery boot option.
      // If cannot find UEFI OS option, try to find default EFI boot option as recovery boot option.
      //
      if (FeaturePcdGet (PcdH2OBdsDefaultBootListGenericOsSupported) &&
          IsGenericUefiBootOs (FileSystemHandles[Index], WindowsToGoBootVarExist)) {
        CreateGenericUefiBootOs (FileSystemHandles[Index]);
        if (IsNonRemovableHdd (FileSystemHandles[Index])) {
          DeleteRecoveryOption (DevicePath);
        }
        if (WindowsToGoBootVarExist && IsWindowsToGo (FileSystemHandles[Index])) {
          BdsLibDeleteOptionFromHandle (FileSystemHandles[Index]);
        }
        continue;
      }
    }

    if (WindowsToGoBootVarExist && IsWindowsToGo (FileSystemHandles[Index])) {
      BdsLibDeleteOptionFromHandle (FileSystemHandles[Index]);
      continue;
    }

    //
    // Do the removable Media thing. \EFI\BOOT\boot{machinename}.EFI
    //  machinename is ia32, ia64, x64, ...
    //
    if (!HaveDefaultRemovableFile (FileSystemHandles[Index])) {
      //
      // No such file or the file is not a EFI application, delete this boot option
      //
      BdsLibDeleteOptionFromHandle (FileSystemHandles[Index]);
    } else {
      CreateRemovableBootOption (FileSystemHandles[Index]);
    }
  }

  if (FileSystemHandles != NULL) {
    FreePool (FileSystemHandles);
  }
  if (DisableOpromStorageDevBoot != NULL) {
    FreePool (DisableOpromStorageDevBoot);

    if (OpromStorageDevCount) {
      FreePool (OpromStorageDev);
    }
  }
  return EFI_SUCCESS;
}

