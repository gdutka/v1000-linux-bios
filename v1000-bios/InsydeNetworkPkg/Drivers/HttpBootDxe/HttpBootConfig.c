/** @file
  HTTP boot configuration related functions.

;******************************************************************************
;* Copyright (c) 2019 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/** @file
  Helper functions for configuring or getting the parameters relating to HTTP Boot.

Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "HttpBootDxe.h"
#include <Library/UefiBootManagerLib.h>
#include <Library/DevicePathLib.h>
#include <Library/KernelConfigLib.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/ZeroGuid.h>

STATIC BOOLEAN mgH2OFormBrowserProtocoSupported = FALSE;

STATIC
EFI_STATUS
InitializeHttpBootOptionForm (
  IN   HTTP_BOOT_PRIVATE_DATA          *Private,
  IN   HTTP_BOOT_FORM_CALLBACK_INFO    *CallbackInfo
  );

CHAR16  mHttpBootConfigStorageName[]     = L"HTTP_BOOT_CONFIG_IFR_NVDATA";

VENDOR_DEVICE_PATH   mVendorDevicePath = {{HARDWARE_DEVICE_PATH, HW_VENDOR_DP, 0x14, 0x00}, EFI_CALLER_ID_GUID};

/**
  Add new boot option for HTTP boot.

  @param[in]  Private             Pointer to the driver private data.
  @param[in]  UsingIpv6           Set to TRUE if creating boot option for IPv6.
  @param[in]  Description         The description text of the boot option.
  @param[in]  Uri                 The URI string of the boot file.

  @retval EFI_SUCCESS             The boot option is created successfully.
  @retval Others                  Failed to create new boot option.

**/
STATIC
EFI_STATUS
HttpBootAddBootOption (
  IN   HTTP_BOOT_PRIVATE_DATA   *Private,
  IN   BOOLEAN                  UsingIpv6,
  IN   CHAR16                   *Uri
  )
{
  EFI_DEV_PATH                      *Node;
  EFI_DEVICE_PATH_PROTOCOL          *TmpDevicePath;
  EFI_DEVICE_PATH_PROTOCOL          *NewDevicePath;
  UINTN                             Length;
  CHAR8                             AsciiUri[URI_STR_MAX_SIZE];
  EFI_STATUS                        Status;
  UINTN                             Index;
  HTTP_BOOT_VIRTUAL_NIC             *NewVirtualNic;

  NewDevicePath = NULL;
  Node          = NULL;
  TmpDevicePath = NULL;

  //
  // Convert the scheme to all lower case.
  //
  for (Index = 0; Index < StrLen (Uri); Index++) {
    if (Uri[Index] == L':') {
      break;
    }
    if (Uri[Index] >= L'A' && Uri[Index] <= L'Z') {
      Uri[Index] -= (CHAR16)(L'A' - L'a');
    }
  }

  //
  // Only accept empty URI, or http and https URI.
  //
  if ((StrLen (Uri) != 0) && (StrnCmp (Uri, L"http://", 7) != 0) && (StrnCmp (Uri, L"https://", 8) != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Create a new device path by appending the IP node and URI node to
  // the driver's parent device path
  //
  if (!UsingIpv6) {
    Node = AllocateZeroPool (sizeof (IPv4_DEVICE_PATH));
    if (Node == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }
    Node->Ipv4.Header.Type    = MESSAGING_DEVICE_PATH;
    Node->Ipv4.Header.SubType = MSG_IPv4_DP;
    SetDevicePathNodeLength (Node, sizeof (IPv4_DEVICE_PATH));
  } else {
    Node = AllocateZeroPool (sizeof (IPv6_DEVICE_PATH));
    if (Node == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }
    Node->Ipv6.Header.Type     = MESSAGING_DEVICE_PATH;
    Node->Ipv6.Header.SubType  = MSG_IPv6_DP;
    SetDevicePathNodeLength (Node, sizeof (IPv6_DEVICE_PATH));
  }
  TmpDevicePath = AppendDevicePathNode (Private->ParentDevicePath, (EFI_DEVICE_PATH_PROTOCOL*) Node);
  FreePool (Node);
  if (TmpDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Update the URI node with the input boot file URI.
  //
  UnicodeStrToAsciiStrS (Uri, AsciiUri, sizeof (AsciiUri));
  Length = sizeof (EFI_DEVICE_PATH_PROTOCOL) + AsciiStrSize (AsciiUri);
  Node = AllocatePool (Length);
  if (Node == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    FreePool (TmpDevicePath);
    goto ON_EXIT;
  }
  Node->DevPath.Type    = MESSAGING_DEVICE_PATH;
  Node->DevPath.SubType = MSG_URI_DP;
  SetDevicePathNodeLength (Node, Length);
  CopyMem ((UINT8*) Node + sizeof (EFI_DEVICE_PATH_PROTOCOL), AsciiUri, AsciiStrSize (AsciiUri));
  NewDevicePath = AppendDevicePathNode (TmpDevicePath, (EFI_DEVICE_PATH_PROTOCOL*) Node);
  FreePool (Node);
  FreePool (TmpDevicePath);
  if (NewDevicePath == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  NewVirtualNic = AllocateZeroPool (sizeof (HTTP_BOOT_VIRTUAL_NIC));
  if (NewVirtualNic == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  NewVirtualNic->Signature   = HTTP_BOOT_VIRTUAL_NIC_SIGNATURE;
  NewVirtualNic->ImageHandle = gImageHandle;
  CopyMem ((VOID *) &NewVirtualNic->LoadFile, &gHttpBootDxeLoadFile, sizeof (EFI_LOAD_FILE_PROTOCOL));
  NewVirtualNic->DevicePath  = NewDevicePath;
  NewVirtualNic->Private     = Private;
  NewVirtualNic->IsIPv6      = UsingIpv6;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &NewVirtualNic->Controller,
                  &gEfiLoadFileProtocolGuid,
                  &NewVirtualNic->LoadFile,
                  &gEfiDevicePathProtocolGuid,
                  NewVirtualNic->DevicePath,
                  NULL
                  );

ON_EXIT:

  if (EFI_ERROR (Status)) {
    if (NewDevicePath != NULL) {
      FreePool (NewDevicePath);
    }
  }
  return Status;
}

/**
   
  This function allows the caller to request the current
  configuration for one or more named elements. The resulting
  string is in <ConfigAltResp> format. Also, any and all alternative
  configuration strings shall be appended to the end of the
  current configuration string. If they are, they must appear
  after the current configuration. They must contain the same
  routing (GUID, NAME, PATH) as the current configuration string.
  They must have an additional description indicating the type of
  alternative configuration the string represents,
  "ALTCFG=<StringToken>". That <StringToken> (when
  converted from Hex UNICODE to binary) is a reference to a
  string in the associated string pack.

  @param[in]  This       Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.

  @param[in]  Request    A null-terminated Unicode string in
                         <ConfigRequest> format. Note that this
                         includes the routing information as well as
                         the configurable name / value pairs. It is
                         invalid for this string to be in
                         <MultiConfigRequest> format.

  @param[out] Progress   On return, points to a character in the
                         Request string. Points to the string's null
                         terminator if request was successful. Points
                         to the most recent "&" before the first
                         failing name / value pair (or the beginning
                         of the string if the failure is in the first
                         name / value pair) if the request was not successful.

  @param[out] Results    A null-terminated Unicode string in
                         <ConfigAltResp> format which has all values
                         filled in for the names in the Request string.
                         String to be allocated by the called function.

  @retval EFI_SUCCESS             The Results string is filled with the
                                  values corresponding to all requested
                                  names.

  @retval EFI_OUT_OF_RESOURCES    Not enough memory to store the
                                  parts of the results that must be
                                  stored awaiting possible future
                                  protocols.

  @retval EFI_INVALID_PARAMETER   For example, passing in a NULL
                                  for the Request parameter
                                  would result in this type of
                                  error. In this case, the
                                  Progress parameter would be
                                  set to NULL.

  @retval EFI_NOT_FOUND           Routing data doesn't match any
                                  known driver. Progress set to the
                                  first character in the routing header.
                                  Note: There is no requirement that the
                                  driver validate the routing data. It
                                  must skip the <ConfigHdr> in order to
                                  process the names.

  @retval EFI_INVALID_PARAMETER   Illegal syntax. Progress set
                                  to most recent "&" before the
                                  error or the beginning of the
                                  string.

  @retval EFI_INVALID_PARAMETER   Unknown name. Progress points
                                  to the & before the name in
                                  question.

**/
EFI_STATUS
EFIAPI
HttpBootFormExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  HTTP_BOOT_FORM_CALLBACK_INFO     *CallbackInfo;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  BOOLEAN                          AllocatedRequest;
  UINTN                            Size;

  if (This == NULL || Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CallbackInfo = HTTP_BOOT_FORM_CALLBACK_INFO_FROM_CONFIG_ACCESS (This);

  *Progress = Request;
  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, CallbackInfo->FormGuid, mHttpBootConfigStorageName)) {
    return EFI_NOT_FOUND;
  }

  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Size             = 0;
  
  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  BufferSize = sizeof (HTTP_BOOT_CONFIG_IFR_NVDATA);
  ZeroMem (&CallbackInfo->HttpBootNvData, BufferSize);
  StrCpyS (CallbackInfo->HttpBootNvData.Description, DESCRIPTION_STR_MAX_SIZE / sizeof (CHAR16), HTTP_BOOT_DEFAULT_DESCRIPTION_STR);

  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (CallbackInfo->FormGuid, mHttpBootConfigStorageName, CallbackInfo->RegisteredHandle);
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    if (ConfigRequest == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
  }

  Status = gHiiConfigRouting->BlockToConfig (
                                gHiiConfigRouting,
                                ConfigRequest,
                                (UINT8 *) &CallbackInfo->HttpBootNvData,
                                BufferSize,
                                Results,
                                Progress
                                );

  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;
}

/**

  This function applies changes in a driver's configuration.
  Input is a Configuration, which has the routing data for this
  driver followed by name / value configuration pairs. The driver
  must apply those pairs to its configurable storage. If the
  driver's configuration is stored in a linear block of data
  and the driver's name / value pairs are in <BlockConfig>
  format, it may use the ConfigToBlock helper function (above) to
  simplify the job.

  @param[in]  This           Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.

  @param[in]  Configuration  A null-terminated Unicode string in
                             <ConfigString> format.

  @param[out] Progress       A pointer to a string filled in with the
                             offset of the most recent '&' before the
                             first failing name / value pair (or the
                             beginning of the string if the failure
                             is in the first name / value pair) or
                             the terminating NULL if all was
                             successful.

  @retval EFI_SUCCESS             The results have been distributed or are
                                  awaiting distribution.

  @retval EFI_OUT_OF_RESOURCES    Not enough memory to store the
                                  parts of the results that must be
                                  stored awaiting possible future
                                  protocols.

  @retval EFI_INVALID_PARAMETERS  Passing in a NULL for the
                                  Results parameter would result
                                  in this type of error.

  @retval EFI_NOT_FOUND           Target for the specified routing data
                                  was not found.

**/
EFI_STATUS
EFIAPI
HttpBootFormRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  HTTP_BOOT_FORM_CALLBACK_INFO     *CallbackInfo;
  HTTP_BOOT_PRIVATE_DATA           *Private;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CallbackInfo = HTTP_BOOT_FORM_CALLBACK_INFO_FROM_CONFIG_ACCESS (This);
  if (CallbackInfo->IsRootForm){
    return EFI_SUCCESS;
  }
  Private      = HTTP_BOOT_PRIVATE_DATA_FROM_CALLBACK_INFO (CallbackInfo);


  if (Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Progress = Configuration;

  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check routing data in <ConfigHdr>.
  // Note: there is no name for Name/Value storage, only GUID will be checked
  //
  if (!HiiIsConfigHdrMatch (Configuration, CallbackInfo->FormGuid, mHttpBootConfigStorageName)) {
    return EFI_NOT_FOUND;
  }

  //
  // Get NV configuration data.
  //
  BufferSize = sizeof (HTTP_BOOT_CONFIG_IFR_NVDATA);
  ZeroMem (&CallbackInfo->HttpBootNvData, BufferSize);
  Status = gHiiConfigRouting->ConfigToBlock (
                            gHiiConfigRouting,
                            Configuration,
                            (UINT8 *) &CallbackInfo->HttpBootNvData,
                            &BufferSize,
                            Progress
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (StrLen (CallbackInfo->HttpBootNvData.Uri) == 0) {
    return EFI_SUCCESS;
  }

  //
  // Create a new boot option according to the configuration data.
  //
  HttpBootAddBootOption (
    Private,
    (CallbackInfo->HttpBootNvData.IpVersion == HTTP_BOOT_IP_VERSION_6) ? TRUE : FALSE,
    CallbackInfo->HttpBootNvData.Uri
    );
  InitializeHttpBootOptionForm (Private, CallbackInfo);

  return EFI_SUCCESS;
}

/**
  Find MAC address node and URI node from handle's device path.
  And Indicate the IPv4 or IPv6 capability from DevicePath IPv4 or IPv6 node.

  @param[in]  BootDeviceHandle        BootDeviceHandle
  @param[out] DevicePathMacAddress    Output the pointer of MAC address in DevicePath.
  @param[out] IsIPv6                  Indicate IP capability in this DevicePath.
  @param[out] HttpBootUrlString       Output the pointer of URI string in DevicePath.

  @retval EFI_SUCCESS                 MacAddress and URI node are found.
  @retval EFI_NOT_FOUND               Not found MAC or URI node.
  @retval others                      Error status from HandleProtocol function.
**/
STATIC
EFI_STATUS
GetMacAddrAndUrlFromHttpBootHandle (
  IN  EFI_HANDLE                BootDeviceHandle,
  OUT EFI_MAC_ADDRESS           **DevicePathMacAddress,
  OUT BOOLEAN                   *IsIPv6,
  OUT CHAR8                     **HttpBootUrlString
  )
{
  EFI_STATUS                    Status;
  BOOLEAN                       IsHttpBootDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *TmpDevicePath;
  BOOLEAN                       TmpIsIPv6;
  MAC_ADDR_DEVICE_PATH          *MacDevicePath;
  EFI_MAC_ADDRESS               *MacAddrPtr;
  CHAR8                         *UrlString;
  URI_DEVICE_PATH               *UriDevicePath;

  if (BootDeviceHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (
                       BootDeviceHandle,
                       &gEfiDevicePathProtocolGuid,
                       &DevicePath
                       );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Find out DevicePath like .../MAC/IP/URI
  //
  MacAddrPtr           = NULL;
  IsHttpBootDevicePath = FALSE;
  TmpDevicePath        = DevicePath;
  UrlString            = NULL;
  TmpIsIPv6            = FALSE;
  while (!IsDevicePathEnd (TmpDevicePath)) {
    if (TmpDevicePath->Type == MESSAGING_DEVICE_PATH) {
      if (TmpDevicePath->SubType == MSG_MAC_ADDR_DP) {
        MacDevicePath = (VOID *)TmpDevicePath;
        MacAddrPtr = &MacDevicePath->MacAddress;
      } else if (TmpDevicePath->SubType == MSG_IPv6_DP) {
        TmpIsIPv6 = TRUE;
      } else if (MacAddrPtr != NULL && (TmpDevicePath->SubType == MSG_URI_DP)) {
        IsHttpBootDevicePath = TRUE;
        if (DevicePathNodeLength (TmpDevicePath) > sizeof (EFI_DEVICE_PATH)) {
          UriDevicePath = (URI_DEVICE_PATH *) TmpDevicePath;
          UrlString = UriDevicePath->Uri;
        }
        break;
      }
    }
    TmpDevicePath = NextDevicePathNode (TmpDevicePath);
  }

  if (!IsHttpBootDevicePath) {
    return EFI_NOT_FOUND;    
  }

  *IsIPv6               = TmpIsIPv6;
  *DevicePathMacAddress = MacAddrPtr;
  *HttpBootUrlString    = UrlString;
  return EFI_SUCCESS;
}

/**
  Wake UNDI device from PxeDummy.

  @param[in]  UndiDeviceHandle        Handle need content Ip4Config2 protocol.

  @retval EFI_SUCCESS                 MacAddress and URI node are found.
  @retval EFI_INVALID_PARAMETER       Handle without Ip4Config2 protocol.
**/
STATIC
EFI_STATUS
WakeUpUndiDeviceFromPxeDummy (
  IN    EFI_HANDLE                     UndiDeviceHandle
  )
{
  EFI_STATUS                           Status;
  EFI_IP4_CONFIG2_PROTOCOL             *Ip4Config2;

  Status = gBS->HandleProtocol (
                  UndiDeviceHandle,
                  &gEfiIp4Config2ProtocolGuid,
                  (VOID **) &Ip4Config2
                  );
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Issue a dummy call to wake up network device from PXE dummy.
  //
  Ip4Config2->GetData (Ip4Config2, Ip4Config2DataTypeDnsServer, NULL, NULL);
  return EFI_SUCCESS;
}

/**
  Wake UNDI device from PxeDummy.
**/
STATIC
VOID
WakeAllUndiDevicesFromPxeDummy (
  VOID
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           *UndiHandles;
  EFI_HANDLE                           UndiHandle;
  UINTN                                DeviceCounter;
  UINTN                                Index;

  DeviceCounter = 0;
  UndiHandles   = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleNetworkProtocolGuid,
                  NULL,
                  &DeviceCounter,
                  &UndiHandles
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  for (Index = 0; Index < DeviceCounter; Index++) {
    UndiHandle = UndiHandles[Index];
    WakeUpUndiDeviceFromPxeDummy (UndiHandle);
  }
  FreePool (UndiHandles);
}

/**
  HTTP boot option form initialize.
  Provides Boot options in selected Device.

  @param[in]  HttpUriBootConfig        BootDeviceHandle
  @param[out] CallbackInfo             Output the pointer of MAC address in DevicePath.
  @param[out] HttpBootUrlString       Output the pointer of URI string in DevicePath.

  @retval EFI_SUCCESS                 MacAddress and URI node are found.
  @retval EFI_NOT_FOUND               Not found MAC or URI node.
  @retval others                      Error status from HandleProtocol function.
**/
STATIC
EFI_STATUS
InitializeHttpBootOptionForm (
  IN   HTTP_BOOT_PRIVATE_DATA          *Private,
  IN   HTTP_BOOT_FORM_CALLBACK_INFO    *CallbackInfo
  )
{
  EFI_STATUS              Status;
  EFI_IFR_GUID_LABEL      *HttpBootOptionLabel;
  EFI_IFR_GUID_LABEL      *HttpBootOptionEndLabel;

  VOID                    *OpCodeHandle;
  VOID                    *OpEndCodeHandle;

  UINTN                   Index;
  UINTN                   HttpDeviceCnt;
  EFI_HANDLE              *HttpDeviceHandles;
  EFI_MAC_ADDRESS         DeviceMacAddress;
  EFI_MAC_ADDRESS         *MacPtr;
  UINTN                   MacAddrSize;
  CHAR8                   *UrlString;
  CHAR16                  UnicodeUrlString[200];
  UINTN                   StringIdListIndex;
  HTTP_DEVICE_ENTRY       *StringIdList;
  UINT16                  StringId;
  BOOLEAN                 IsIpv6;

  //
  // Root form shouldn't process function.
  //
  if (CallbackInfo->IsRootForm) {
    return EFI_SUCCESS;
  }

  MacAddrSize = 0;
  ZeroMem (&DeviceMacAddress, sizeof (EFI_MAC_ADDRESS));
  Status = NetLibGetMacAddress (Private->Controller, &DeviceMacAddress, &MacAddrSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HttpDeviceCnt     = 0;
  HttpDeviceHandles = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadFileProtocolGuid,
                  NULL,
                  &HttpDeviceCnt,
                  &HttpDeviceHandles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  OpCodeHandle    = HiiAllocateOpCodeHandle ();
  OpEndCodeHandle = HiiAllocateOpCodeHandle ();
  if ((OpCodeHandle == NULL) || (OpEndCodeHandle == NULL)) {
    goto ON_EXIT;
  }

  if (CallbackInfo->HttpDeviceList == NULL) {
    CallbackInfo->HttpDeviceList = AllocateZeroPool (sizeof (HTTP_DEVICE_ENTRY) * HTTP_BOOT_DEVICE_MAX);
    if (CallbackInfo->HttpDeviceList == NULL) {
      goto ON_EXIT;
    }
  }

  HttpBootOptionLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (OpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  HttpBootOptionLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  HttpBootOptionLabel->Number       = HTTP_BOOT_URL_LIST_START_ID;

  HttpBootOptionEndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (OpEndCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  HttpBootOptionEndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  HttpBootOptionEndLabel->Number       = HTTP_BOOT_URL_LIST_END_ID;

  StringIdListIndex = 0;
  StringIdList      = CallbackInfo->HttpDeviceList;
  for (Index = 0; Index < HttpDeviceCnt; Index++) {
    Status = GetMacAddrAndUrlFromHttpBootHandle (HttpDeviceHandles[Index], &MacPtr, &IsIpv6, &UrlString);
    if (EFI_ERROR (Status)) {
      continue;
    }
    //
    // If MAC is not the same, skip it.
    //
    if (CompareMem ((VOID *) MacPtr, (VOID *) &DeviceMacAddress, MacAddrSize) != 0) {
      continue;
    }
    //
    // Set URL as HII String.
    //
    UnicodeSPrint (UnicodeUrlString, 200, L"%s%a", (IsIpv6) ? L"[IPv6]" : L"", UrlString);
    StringId = StringIdList[StringIdListIndex].StringId;
    StringId = HiiSetString (
                 CallbackInfo->RegisteredHandle,
                 StringId,
                 UnicodeUrlString,
                 NULL
                 );
    StringIdList[StringIdListIndex].StringId = StringId;
    HiiCreateTextOpCode (OpCodeHandle, StringId, StringId, 0);
    StringIdListIndex++;
    if (StringIdListIndex >= HTTP_BOOT_DEVICE_MAX) {
      break;
    }
  }

  HiiUpdateForm (
    CallbackInfo->RegisteredHandle,
    &gHttpBootConfigGuid,
    FORMID_MAIN_FORM,
    OpCodeHandle,
    OpEndCodeHandle
    );

ON_EXIT:
  if (HttpDeviceHandles != NULL) {
    FreePool (HttpDeviceHandles);
  }
  if (OpCodeHandle != NULL) {
    HiiFreeOpCodeHandle (OpCodeHandle);
  }
  if (OpEndCodeHandle) {
    HiiFreeOpCodeHandle (OpEndCodeHandle);
  }
  return EFI_SUCCESS;
}

/**
  HTTP boot form initialize.
  Provides MAC address as list.

  @param[in]  HttpUriBootConfig        BootDeviceHandle
  @param[out] CallbackInfo             Output the pointer of MAC address in DevicePath.
  @param[out] HttpBootUrlString       Output the pointer of URI string in DevicePath.

  @retval EFI_SUCCESS                 MacAddress and URI node are found.
  @retval EFI_NOT_FOUND               Not found MAC or URI node.
  @retval others                      Error status from HandleProtocol function.
**/
STATIC
EFI_STATUS
InitializeHttpBootForm (
  IN   HTTP_BOOT_FORM_CALLBACK_INFO    *CallbackInfo
  )
{
  EFI_STATUS                           Status;
  EFI_IFR_GUID_LABEL                   *HttpDeviceHandleLabel;
  EFI_IFR_GUID_LABEL                   *HttpDeviceHandleEndLabel;

  UINTN                                Index;
  UINTN                                HttpDeviceCnt;
  EFI_HANDLE                           *HttpDeviceHandles;
  EFI_HANDLE                           HttpDeviceHandle;
  UINT32                               *Id;
  HTTP_BOOT_PRIVATE_DATA               *Private;
  HTTP_BOOT_FORM_CALLBACK_INFO         *DeviceCallbackInfo;

  CHAR16                               *MacString;
  VOID                                 *OpCodeHandle;
  VOID                                 *OpEndCodeHandle;
  EFI_DEVICE_PATH_PROTOCOL             *HiiVendorDevicePath;
  CHAR16                               *HiiVendorDevicePathString;
  HTTP_DEVICE_ENTRY                    *HttpDevice;

  OpCodeHandle              = NULL;
  OpEndCodeHandle           = NULL;
  HiiVendorDevicePathString = NULL;
  HttpDeviceCnt             = 0;
  HttpDeviceHandles         = NULL;

  //
  // This function will wake all of network device from PxeDummy.
  //
  WakeAllUndiDevicesFromPxeDummy ();

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiCallerIdGuid,
                  NULL,
                  &HttpDeviceCnt,
                  &HttpDeviceHandles
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  if (CallbackInfo->HttpDeviceList == NULL) {
    CallbackInfo->HttpDeviceList = AllocateZeroPool (sizeof (HTTP_DEVICE_ENTRY) * HTTP_BOOT_FORM_SUPPORTED_DEVICES_MAX);
    if (CallbackInfo->HttpDeviceList == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }
  }

  OpCodeHandle    = HiiAllocateOpCodeHandle ();
  OpEndCodeHandle = HiiAllocateOpCodeHandle ();
  if ((OpCodeHandle == NULL) || (OpEndCodeHandle == NULL)) {
    goto ON_EXIT;
  }

  HttpDeviceHandleLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (OpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  HttpDeviceHandleLabel->ExtendOpCode    = EFI_IFR_EXTEND_OP_LABEL;
  HttpDeviceHandleLabel->Number          = HTTP_BOOT_DEVICE_LIST_START_ID;

  HttpDeviceHandleEndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (OpEndCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  HttpDeviceHandleEndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  HttpDeviceHandleEndLabel->Number       = HTTP_BOOT_DEVICE_LIST_END_ID;

  for (Index = 0; Index < HttpDeviceCnt; Index++) {
    HttpDeviceHandle    = HttpDeviceHandles[Index];
    MacString           = NULL;

    if (HiiVendorDevicePathString != NULL) {
      FreePool (HiiVendorDevicePathString);
      HiiVendorDevicePathString = NULL;
    }

    Status = gBS->HandleProtocol (
                    HttpDeviceHandle,
                    &gEfiCallerIdGuid,
                    (VOID **) &Id
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    Private = HTTP_BOOT_PRIVATE_DATA_FROM_ID(Id);
    DeviceCallbackInfo = &Private->CallbackInfo;

    Status = NetLibGetMacString (HttpDeviceHandle, NULL, &MacString);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    DeviceCallbackInfo->ChildHandle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &HiiVendorDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    HiiVendorDevicePathString = ConvertDevicePathToText (HiiVendorDevicePath, TRUE, FALSE);
    if (HiiVendorDevicePathString == NULL) {
      continue;
    }

    HttpDevice = CallbackInfo->HttpDeviceList + Index;
    HttpDevice->HiiDevPathStringId = HiiSetString (
                                       CallbackInfo->RegisteredHandle,
                                       HttpDevice->HiiDevPathStringId,
                                       HiiVendorDevicePathString,
                                       NULL
                                       );
    HttpDevice->StringId = HiiSetString (
                             CallbackInfo->RegisteredHandle,
                             HttpDevice->StringId,
                             MacString,
                             NULL
                             );
    HiiCreateGotoExOpCode (
      OpCodeHandle,
      FORMID_MAIN_FORM,
      HttpDevice->StringId,
      HttpDevice->StringId,
      0,
      (EFI_QUESTION_ID) (UINT16)(HTTP_BOOT_DEVICE_LIST_START_ID + Index),
      0,
      &gHttpBootConfigGuid,
      HttpDevice->HiiDevPathStringId
      );
    FreePool (MacString);
  }

  HiiUpdateForm (
    CallbackInfo->RegisteredHandle,
    NULL,
    FORMID_MAIN_FORM,
    OpCodeHandle,
    OpEndCodeHandle
    );

  Status = EFI_SUCCESS;

ON_EXIT:
  if (HiiVendorDevicePathString != NULL) {
    FreePool (HiiVendorDevicePathString);
  }
  if (HttpDeviceHandles != NULL) {
    FreePool (HttpDeviceHandles);
  }
  if (OpCodeHandle != NULL) {
    HiiFreeOpCodeHandle (OpCodeHandle);
  }
  if (OpEndCodeHandle) {
    HiiFreeOpCodeHandle (OpEndCodeHandle);
  }
  return Status;
}

/**
  Register HTTP URL boot via CP gH2OBdsCpBootCreateHttpUrlGuid when user submit.

  @param[in]  HttpUriConfigData       Form browser internal data.
  @param[out] HttpUriConfig           User input data from form browser.

  @retval EFI_SUCCESS                 Register HTTP boot option successful.
  @retval EFI_INVALID_PARAMETER       HttpBootUrl is too long.
  @retval EFI_NOT_READY               CP function feedback failed.
  @retval EFI_NOT_FOUND               Private data not found in SelectNicHandle.
  @retval others                      Error status from HandleProtocol function.
**/
EFI_STATUS
RegisterHttpBootUriOption (
  IN   HTTP_BOOT_PRIVATE_DATA                    *Private,
  IN   HTTP_BOOT_FORM_CALLBACK_INFO              *CallbackInfo
  )
{
  EFI_STATUS                                     Status;
  
  Status = WakeUpUndiDeviceFromPxeDummy (CallbackInfo->UndiHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get HII NV data
  //
  HiiGetBrowserData (&gHttpBootConfigGuid, L"HTTP_BOOT_CONFIG_IFR_NVDATA", sizeof (HTTP_BOOT_CONFIG_IFR_NVDATA), (UINT8 *) &CallbackInfo->HttpBootNvData);

  //
  // Set NV data to create new HTTP boot handle. NV data will be clean if function return SUCCESS.
  //
  Status = HttpBootAddBootOption (
             Private,
             (CallbackInfo->HttpBootNvData.IpVersion == HTTP_BOOT_IP_VERSION_6) ? TRUE : FALSE,
             CallbackInfo->HttpBootNvData.Uri
             );
   if (!EFI_ERROR (Status)) {
     ZeroMem (&CallbackInfo->HttpBootNvData, sizeof (HTTP_BOOT_CONFIG_IFR_NVDATA));
     HiiSetBrowserData (&gHttpBootConfigGuid, L"HTTP_BOOT_CONFIG_IFR_NVDATA", sizeof (HTTP_BOOT_CONFIG_IFR_NVDATA), (UINT8 *) &CallbackInfo->HttpBootNvData, NULL);
   }
  return Status;
}

/**
  This function is called to provide results data to the driver.
  This data consists of a unique key that is used to identify
  which data is either being passed back or being asked for.

  @param[in]       This          Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]       Action        Specifies the type of action taken by the browser.
  @param[in]       QuestionId    A unique value which is sent to the original
                                 exporting driver so that it can identify the type
                                 of data to expect. The format of the data tends to
                                 vary based on the opcode that generated the callback.
  @param[in]       Type          The type of value for the question.
  @param[in, out]  Value         A pointer to the data being sent to the original
                                 exporting driver.
  @param[out]      ActionRequest On return, points to the action requested by the
                                 callback function.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the
                                 variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the
                                 callback.
**/
EFI_STATUS
EFIAPI
HttpBootFormCallback (
  IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN        EFI_BROWSER_ACTION               Action,
  IN        EFI_QUESTION_ID                  QuestionId,
  IN        UINT8                            Type,
  IN OUT    EFI_IFR_TYPE_VALUE               *Value,
  OUT       EFI_BROWSER_ACTION_REQUEST       *ActionRequest
  )
{
  CHAR16                          *Uri;
  UINTN                           UriLen;
  CHAR8                           *AsciiUri;
  HTTP_BOOT_FORM_CALLBACK_INFO    *CallbackInfo;
  HTTP_BOOT_PRIVATE_DATA          *Private;
  EFI_STATUS                      Status;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Uri      = NULL;
  UriLen   = 0;
  AsciiUri = NULL;
  Status   = EFI_SUCCESS;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CallbackInfo = HTTP_BOOT_FORM_CALLBACK_INFO_FROM_CONFIG_ACCESS (This);
  if (mgH2OFormBrowserProtocoSupported && Action == EFI_BROWSER_ACTION_CHANGING) {
    //
    // For compatible to old form browser which only use EFI_BROWSER_ACTION_CHANGING action,
    // change action to EFI_BROWSER_ACTION_CHANGED to make it workable.
    //
    Action = EFI_BROWSER_ACTION_CHANGED;
  }

  //
  // Provide items of UNDI device handles, it will use MAC address as name.
  //
  if ((Action == EFI_BROWSER_ACTION_FORM_OPEN) && (QuestionId == 0)) {
    if (CallbackInfo->IsRootForm) {
      return InitializeHttpBootForm (CallbackInfo);
    } else {
      Private = HTTP_BOOT_PRIVATE_DATA_FROM_CALLBACK_INFO (CallbackInfo);
      return InitializeHttpBootOptionForm (Private, CallbackInfo);
    }
  }

  if (Value == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  Private = HTTP_BOOT_PRIVATE_DATA_FROM_CALLBACK_INFO (CallbackInfo);
  switch (QuestionId) {
  case KEY_INITIATOR_URI:
    break;

  case HTTP_BOOT_KEY_INITIATOR_URI:
    /// Check URI correction.
    Status = RegisterHttpBootUriOption (Private, CallbackInfo);
    if (EFI_ERROR (Status)) {
      break;
    }
    if (CallbackInfo->FormBrowserEx2 == NULL) {
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_SUBMIT;
    } else {
      CallbackInfo->FormBrowserEx2->ExecuteAction (BROWSER_ACTION_SUBMIT, 0);
    }
    Status = InitializeHttpBootOptionForm (Private, CallbackInfo);
    break;

  default:
    break;
  };
  return Status;
}

/**
  Initialize the configuration form.

  @param[in]  Private             Pointer to the driver private data.

  @retval EFI_SUCCESS             The configuration form is initialized.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.

**/
EFI_STATUS
HttpBootConfigFormInit (
  IN HTTP_BOOT_PRIVATE_DATA     *Private
  )
{
  EFI_STATUS                        Status;
  HTTP_BOOT_FORM_CALLBACK_INFO      *CallbackInfo;
  CHAR16                            *MacString;
  CHAR16                            *OldMenuString;
  CHAR16                            MenuString[128];

  CallbackInfo = &Private->CallbackInfo;

  if (CallbackInfo->Initialized) {
    return EFI_SUCCESS;
  }

  CallbackInfo->Signature  = HTTP_BOOT_FORM_CALLBACK_INFO_SIGNATURE;
  CallbackInfo->UndiHandle = Private->Controller;
  CallbackInfo->HiiVendorDevicePath = AppendDevicePathNode (
                                        Private->ParentDevicePath,
                                        (EFI_DEVICE_PATH_PROTOCOL *) &mVendorDevicePath
                                        );
  if (CallbackInfo->HiiVendorDevicePath == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  CallbackInfo->ConfigAccess.ExtractConfig = HttpBootFormExtractConfig;
  CallbackInfo->ConfigAccess.RouteConfig   = HttpBootFormRouteConfig;
  CallbackInfo->ConfigAccess.Callback      = HttpBootFormCallback;

  //
  // Install Device Path Protocol and Config Access protocol to driver handle.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &CallbackInfo->ChildHandle,
                  &gEfiDevicePathProtocolGuid,
                  CallbackInfo->HiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &CallbackInfo->ConfigAccess,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  //
  // Publish our HII data.
  //
  CallbackInfo->RegisteredHandle = HiiAddPackages (
                                     &gHttpBootConfigGuid,
                                     CallbackInfo->ChildHandle,
                                     HttpBootDxeStrings,
                                     HttpBootConfigVfrBin,
                                     NULL
                                     );
  if (CallbackInfo->RegisteredHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  //
  // Locate gEdkiiFormBrowserEx2ProtocolGuid to resolve HII submit by CREATE button
  // System will pop up dialog to notify somethings doesn't save.
  //
  gBS->LocateProtocol (&gEdkiiFormBrowserEx2ProtocolGuid, NULL, (VOID **) &CallbackInfo->FormBrowserEx2);

  //
  // Append MAC string in the menu help string
  //
  Status = NetLibGetMacString (Private->Controller, NULL, &MacString);
  if (!EFI_ERROR (Status)) {
    OldMenuString = HiiGetString (
                      CallbackInfo->RegisteredHandle,
                      STRING_TOKEN (STR_HTTP_BOOT_CONFIG_FORM_HELP),
                      NULL
                      );
    UnicodeSPrint (MenuString, 128, L"%s (MAC:%s)", OldMenuString, MacString);
    HiiSetString (
      CallbackInfo->RegisteredHandle,
      STRING_TOKEN (STR_HTTP_BOOT_CONFIG_FORM_HELP),
      MenuString,
      NULL
      );

    FreePool (MacString);
    FreePool (OldMenuString);

    CallbackInfo->Initialized = TRUE;
    CallbackInfo->FormGuid    = &gHttpBootConfigGuid;
    return EFI_SUCCESS;
  }

Error:

  HttpBootConfigFormUnload (Private);
  return Status;
}

/**
  Unload the configuration form, this includes: delete all the configuration
  entries, uninstall the form callback protocol, and free the resources used.
  The form will only be unload completely when both IP4 and IP6 stack are stopped.

  @param[in]  Private             Pointer to the driver private data.

  @retval EFI_SUCCESS             The configuration form is unloaded.
  @retval Others                  Failed to unload the form.

**/
EFI_STATUS
HttpBootConfigFormUnload (
  IN HTTP_BOOT_PRIVATE_DATA     *Private
  )
{
  HTTP_BOOT_FORM_CALLBACK_INFO      *CallbackInfo;

  if (Private->Ip4Nic != NULL || Private->Ip6Nic != NULL) {
    //
    // Only unload the configuration form when both IP4 and IP6 stack are stopped.
    //
    return EFI_SUCCESS;
  }

  CallbackInfo = &Private->CallbackInfo;
  if (CallbackInfo->ChildHandle != NULL) {
    //
    // Uninstall EFI_HII_CONFIG_ACCESS_PROTOCOL
    //
    gBS->UninstallMultipleProtocolInterfaces (
           CallbackInfo->ChildHandle,
           &gEfiDevicePathProtocolGuid,
           CallbackInfo->HiiVendorDevicePath,
           &gEfiHiiConfigAccessProtocolGuid,
           &CallbackInfo->ConfigAccess,
           NULL
           );
    CallbackInfo->ChildHandle = NULL;
  }

  if (CallbackInfo->HiiVendorDevicePath != NULL) {
    FreePool (CallbackInfo->HiiVendorDevicePath);
    CallbackInfo->HiiVendorDevicePath = NULL;
  }

  if (CallbackInfo->RegisteredHandle != NULL) {
    //
    // Remove HII package list
    //
    HiiRemovePackages (CallbackInfo->RegisteredHandle);
    CallbackInfo->RegisteredHandle = NULL;
  }

  return EFI_SUCCESS;
}

/**
  Initialize the configuration form to SCU Boot\Link.

  @param[in]  Private             Pointer to the driver private data.

  @retval EFI_SUCCESS             The configuration form is initialized.
  @retval EFI_INVALID_PARAMETER   HiiHandle is NULL.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.

**/
EFI_STATUS
HttpBootConfigLinkFormUnload (
  IN   HTTP_BOOT_FORM_CALLBACK_INFO    *CallbackInfo
  )
{
  return EFI_SUCCESS;
}

/**
  Callback for Protocol notify.

  @param[in]     Event       Notify event
  @param[in]     Context     Context for this event.

**/
STATIC
VOID
H2oFormBrowserCheckCallback (
  IN   EFI_EVENT                       Event,
  IN   VOID                            *Context
  )
{
  gBS->CloseEvent (Event);
  mgH2OFormBrowserProtocoSupported = TRUE;
}

/**
  Check the H2OFormBrowser protocol exist or not, it will make HiiCallback function compatibility with old FormBrowser engine.

**/
STATIC
VOID
IsH2OFormBrowserExist (
  )
{
  EFI_STATUS                           Status;
  VOID                                 *H2oFormBrowser;
  EFI_EVENT                            H2OFormBrowserCheckEvent;
  VOID                                 *Registration;

  mgH2OFormBrowserProtocoSupported = FALSE;
  H2OFormBrowserCheckEvent         = NULL;
  Registration                     = NULL;
  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, &H2oFormBrowser);
  if (!EFI_ERROR (Status)) {
    mgH2OFormBrowserProtocoSupported = TRUE;
  } else {
    Status = gBS->CreateEvent (EVT_NOTIFY_SIGNAL, TPL_CALLBACK, H2oFormBrowserCheckCallback, NULL, &H2OFormBrowserCheckEvent);
    if (EFI_ERROR (Status)) {
      return;
    }
    Registration = NULL;
    gBS->RegisterProtocolNotify (&gH2OFormBrowserProtocolGuid, H2OFormBrowserCheckEvent, &Registration);
  }
}

/**
  Initialize the link configuration form for Link.
  This form will be install to SCU\Boot\Link

  @param[out] Private             Initilailze the driver private data for HII.

  @retval EFI_SUCCESS             The configuration form is initialized.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.

**/
EFI_STATUS
HttpBootConfigLinkFormInit (
  OUT HTTP_BOOT_FORM_CALLBACK_INFO    *CallbackInfo
  )
{
  EFI_STATUS                           Status;
  VENDOR_DEVICE_PATH                   *VendorDeviceNode;
  UINTN                                SizeOfBuffer;
  KERNEL_CONFIGURATION                 KernelConfiguration;

  ZeroMem (CallbackInfo, sizeof (HTTP_BOOT_FORM_CALLBACK_INFO));
  VendorDeviceNode = AllocateZeroPool (sizeof (VENDOR_DEVICE_PATH) + sizeof (EFI_DEVICE_PATH_PROTOCOL));
  if (VendorDeviceNode == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Set VENDOR device path content.
  //
  VendorDeviceNode->Header.Type    = HARDWARE_DEVICE_PATH;
  VendorDeviceNode->Header.SubType = HW_VENDOR_DP;
  CopyGuid (&VendorDeviceNode->Guid, &gEfiCallerIdGuid);
  SetDevicePathNodeLength (VendorDeviceNode, sizeof (VENDOR_DEVICE_PATH));

  //
  // Set DevicePath end node.
  //
  SetDevicePathEndNode (VendorDeviceNode + 1);

  CallbackInfo->Signature                  = HTTP_BOOT_FORM_CALLBACK_INFO_SIGNATURE;
  CallbackInfo->HiiVendorDevicePath        = (EFI_DEVICE_PATH_PROTOCOL *) VendorDeviceNode;
  CallbackInfo->ConfigAccess.ExtractConfig = HttpBootFormExtractConfig;
  CallbackInfo->ConfigAccess.RouteConfig   = HttpBootFormRouteConfig;
  CallbackInfo->ConfigAccess.Callback      = HttpBootFormCallback;

  //
  // Install Device Path Protocol and Config Access protocol to driver handle.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &CallbackInfo->ChildHandle,
                  &gEfiDevicePathProtocolGuid,
                  CallbackInfo->HiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &CallbackInfo->ConfigAccess,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  //
  // Publish our HII data.
  //
  CallbackInfo->RegisteredHandle = HiiAddPackages (
                                     &gHttpBootConfigLinkFormGuid,
                                     CallbackInfo->ChildHandle,
                                     HttpBootDxeStrings,
                                     HttpBootConfigLinkFormVfrBin,
                                     NULL
                                     );
  if (CallbackInfo->RegisteredHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  CallbackInfo->Initialized = TRUE;
  CallbackInfo->IsRootForm  = TRUE;
  CallbackInfo->FormGuid    = &gHttpBootConfigLinkFormGuid;

  SizeOfBuffer = sizeof (EFI_GUID);
  Status = GetKernelConfiguration (&KernelConfiguration);
  if (!EFI_ERROR (Status) && (KernelConfiguration.NetworkProtocol <= UEFI_NETWORK_BOOT_OPTION_DISABLE)) {
    Status = LibPcdSetExPtrS (&gLinkBootTokenSpaceGuid, PcdToken (PcdH2OHttpBootCreateUrlFormsetGuid), &SizeOfBuffer, &gZeroGuid);  // HTTP Boot is disabled; don't link HTTP Boot Configuration form.
    ASSERT_EFI_ERROR (Status);
  } else {
    Status = LibPcdSetExPtrS (&gLinkBootTokenSpaceGuid, PcdToken (PcdH2OHttpBootCreateUrlFormsetGuid), &SizeOfBuffer, &gHttpBootConfigLinkFormGuid);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;

Error:
  HttpBootConfigLinkFormUnload (CallbackInfo);
  return Status;
}