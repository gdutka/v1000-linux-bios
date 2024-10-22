/** @file
  Helper functions for USB

;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbBus.h"
#include "Hub.h"
#include "UsbHelper.h"

/**

  Get the start position of next wanted descriptor.
  
  @param  Buffer                Buffer to parse
  @param  Length                Buffer length 
  @param  DescType              Descriptor type 
  @param  DescLength            Descriptor length
  @param  ParsedBytes           Parsed Bytes to return

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
GetExpectedDescriptor (
  IN  UINT8                             *Buffer,
  IN  UINTN                             Length,
  IN  UINT8                             DescType,
  IN  UINT8                             DescLength,
  OUT UINTN                             *ParsedBytes
  )
{
  UINT16  DescriptorHeader;
  UINT8   Len;
  UINT8   *Ptr;
  UINTN   Parsed;

  Parsed  = 0;
  Ptr     = Buffer;

  while (TRUE) {
    //
    // Buffer length should not less than Desc length
    //
    if (Length < DescLength) {
      return EFI_DEVICE_ERROR;
    }
    //
    // DescriptorHeader = *((UINT16 *)Ptr), compatible with IPF
    //
    DescriptorHeader  = (UINT16) ((*(Ptr + 1) << 8) | *Ptr);

    Len               = Ptr[0];

    //
    // Check to see if it is a start of expected descriptor
    //
    if ((UINT8) (DescriptorHeader >> 8) == DescType) {
      if (Len > DescLength && DescType != USB_DESC_TYPE_ENDPOINT && DescType != USB_DESC_TYPE_HID) {
        return EFI_DEVICE_ERROR;
      }
      break;
    }
    //
    // Descriptor length should be at least 2
    // and should not exceed the buffer length
    //
    if (Len < 2) {
      return EFI_DEVICE_ERROR;
    }

    if (Len > Length) {
      return EFI_DEVICE_ERROR;
    }
    //
    // Skip this mismatch descriptor
    //
    Length -= Len;
    Ptr += Len;
    Parsed += Len;
  }

  *ParsedBytes = Parsed;

  return EFI_SUCCESS;
}

/**

  Get the start position of next wanted endpoint descriptor.

  @param  EndpointEntry         ENDPOINT_DESC_LIST_ENTRY
  @param  Buffer                Buffer to parse 
  @param  BufferLength          Buffer Length
  @param  ParsedBytes           Parsed Bytes to return

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
ParseThisEndpoint (
  IN  ENDPOINT_DESC_LIST_ENTRY          *EndpointEntry,
  IN  UINT8                             *Buffer,
  IN  UINTN                             BufferLength,
  OUT UINTN                             *ParsedBytes
  )
{
  UINT8       *Ptr;
  EFI_STATUS  Status;
  UINTN       SkipBytes;

  //
  // Skip some data for this interface
  //
  Status = GetExpectedDescriptor (
            Buffer,
            BufferLength,
            USB_DESC_TYPE_ENDPOINT,
            sizeof (EFI_USB_ENDPOINT_DESCRIPTOR),
            &SkipBytes
            );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Ptr           = Buffer + SkipBytes;
  *ParsedBytes  = SkipBytes;

  CopyMem (
    &EndpointEntry->EndpointDescriptor,
    Ptr,
    sizeof (EFI_USB_ENDPOINT_DESCRIPTOR)
    );

  *ParsedBytes += *Ptr;

  return EFI_SUCCESS;
}

/**

  Get the start position of next wanted interface descriptor.

  @param  InterfaceEntry        INTERFACE_DESC_LIST_ENTRY
  @param  Buffer                Buffer to parse 
  @param  BufferLength          Buffer Length
  @param  ParsedBytes           Parsed Bytes to return

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
ParseThisInterface (
  IN  INTERFACE_DESC_LIST_ENTRY         *InterfaceEntry,
  IN  UINT8                             *Buffer,
  IN  UINTN                             BufferLen,
  OUT UINTN                             *ParsedBytes
  )
{
  UINT8                     *Ptr;
  UINTN                     SkipBytes;
  UINTN                     Index;
  UINTN                     Length;
  UINTN                     Parsed;
  ENDPOINT_DESC_LIST_ENTRY  *EndpointEntry;
  EFI_STATUS                Status;
  EFI_USB_HID_DESCRIPTOR    *HidDescriptor;

  Parsed = 0;

  //
  // Skip some data for this interface
  //
  Status = GetExpectedDescriptor (
             Buffer,
             BufferLen,
             USB_DESC_TYPE_INTERFACE,
             sizeof (EFI_USB_INTERFACE_DESCRIPTOR),
             &SkipBytes
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length        = BufferLen - SkipBytes;
  *ParsedBytes  = SkipBytes;
  Ptr           = Buffer + SkipBytes;

  //
  // Copy the interface descriptor
  //
  CopyMem (
    &InterfaceEntry->InterfaceDescriptor,
    Ptr,
    sizeof (EFI_USB_INTERFACE_DESCRIPTOR)
    );

  Length       -= *Ptr;
  *ParsedBytes += *Ptr;
  Ptr          += *Ptr;

  InitializeListHead (&InterfaceEntry->EndpointDescListHead);

  if (InterfaceEntry->InterfaceDescriptor.InterfaceClass == CLASS_HID) {
    //
    // Extract HID descriptor if available
    //
    Status = GetExpectedDescriptor (
               Ptr,
               Length,
               USB_DESC_TYPE_HID,
               sizeof (EFI_USB_HID_DESCRIPTOR),
               &SkipBytes
               );
    if (!EFI_ERROR (Status)) {
      //
      // Copy the HID descriptor
      //
      Length       -= SkipBytes;
      *ParsedBytes += SkipBytes;
      Ptr          += SkipBytes;

      HidDescriptor = (EFI_USB_HID_DESCRIPTOR *) Ptr;

      //
      // HID 1.1, section 6.2.1, The NumDescriptors must be at least one.
      //
      if (HidDescriptor->NumDescriptors != 0) {
        CopyMem (
          &InterfaceEntry->HidDescriptor,
          Ptr,
          *Ptr
          );
      }
      Length       -= *Ptr;
      *ParsedBytes += *Ptr;
      Ptr          += *Ptr;
    }
  }
  Status = EFI_SUCCESS;
  for (Index = 0; Index < InterfaceEntry->InterfaceDescriptor.NumEndpoints; Index++) {
    Status = mUsbCore->AllocateBuffer(
                         sizeof (ENDPOINT_DESC_LIST_ENTRY),
                         ALIGNMENT_32,
                         (VOID **)&EndpointEntry
                         );
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }
    //
    // Parses all the endpoint descriptors within this interface.
    //
    Status = ParseThisEndpoint (EndpointEntry, Ptr, Length, &Parsed);
    if (EFI_ERROR (Status)) {
      break;
    }
    InsertTailList (
      &InterfaceEntry->EndpointDescListHead,
      &EndpointEntry->Link
      );
    Length       -= Parsed;
    *ParsedBytes += Parsed;
    Ptr          += Parsed;
  }
  if (EFI_ERROR (Status)) {
    while (!IsListEmpty (&InterfaceEntry->EndpointDescListHead)) {
      EndpointEntry = (ENDPOINT_DESC_LIST_ENTRY*)GetFirstNode (&InterfaceEntry->EndpointDescListHead);
      RemoveEntryList ((LIST_ENTRY*)EndpointEntry);
      mUsbCore->FreeBuffer (
                  sizeof (ENDPOINT_DESC_LIST_ENTRY),
                  EndpointEntry
                  );
    }
  } else if (!IsListEmpty (&InterfaceEntry->EndpointDescListHead)) {
    EndpointEntry = (ENDPOINT_DESC_LIST_ENTRY*)GetFirstNode (&InterfaceEntry->EndpointDescListHead);
    do {
      mUsbCore->InsertAddressConvertTable (
                  ACT_INSTANCE_BODY,
                  EndpointEntry,
                  sizeof (ENDPOINT_DESC_LIST_ENTRY)
                  );
      EndpointEntry = (ENDPOINT_DESC_LIST_ENTRY*)GetFirstNode ((LIST_ENTRY*)EndpointEntry);
    } while (!IsNull (&InterfaceEntry->EndpointDescListHead, (LIST_ENTRY*)EndpointEntry));
  }
  return Status;
}

/**

  Parse the current configuration descriptior.

  @param  ConfigDescEntry       CONFIG_DESC_LIST_ENTRY
  @param  Buffer                Buffer to parse 
  @param  Length                Buffer Length

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
ParseThisConfig (
  IN CONFIG_DESC_LIST_ENTRY             *ConfigDescEntry,
  IN UINT8                              *Buffer,
  IN UINTN                              Length
  )
{
  UINT8                     *Ptr;
  UINT8                     NumInterface;
  UINT8                     ActualInterfaces;
  INTERFACE_DESC_LIST_ENTRY *InterfaceEntry;
  UINTN                     SkipBytes;
  INTN                      Parsed;
  EFI_STATUS                Status;
  INTN                      LengthLeft;

  Parsed = 0;
  //
  //  First skip the current config descriptor;
  //
  Status = GetExpectedDescriptor (
            Buffer,
            Length,
            USB_DESC_TYPE_CONFIG,
            sizeof (EFI_USB_CONFIG_DESCRIPTOR),
            &SkipBytes
            );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Ptr = Buffer + SkipBytes;
  ActualInterfaces = 0;
  CopyMem (
    &ConfigDescEntry->CongfigDescriptor,
    Ptr,
    sizeof (EFI_USB_CONFIG_DESCRIPTOR)
    );

  NumInterface = ConfigDescEntry->CongfigDescriptor.NumInterfaces;

  //
  // Skip size of Configuration Descriptor
  //
  Ptr += sizeof (EFI_USB_CONFIG_DESCRIPTOR);

  LengthLeft = Length - SkipBytes - sizeof (EFI_USB_CONFIG_DESCRIPTOR);

  do {
    //
    // Parse all Interface
    //
    Status = mUsbCore->AllocateBuffer(
                         sizeof (INTERFACE_DESC_LIST_ENTRY),
                         ALIGNMENT_32,
                         (VOID **)&InterfaceEntry
                         );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = ParseThisInterface (InterfaceEntry, Ptr, (UINTN)LengthLeft, (UINTN *)&Parsed);
    if (EFI_ERROR (Status)) {
      mUsbCore->FreeBuffer (
                  sizeof (INTERFACE_DESC_LIST_ENTRY),
                  InterfaceEntry
                  );
      return (ActualInterfaces >= NumInterface) ? EFI_SUCCESS : Status;
    }

    InsertTailList (
      &ConfigDescEntry->InterfaceDescListHead,
      &InterfaceEntry->Link
      );
    //
    // Setup Smm address convert table for Smm security policy
    //
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_BODY,
                InterfaceEntry,
                sizeof (INTERFACE_DESC_LIST_ENTRY)
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_LINKING_LIST,
                &InterfaceEntry->EndpointDescListHead,
                1
                );
    //
    // Parsed for next interface
    //
    LengthLeft -= Parsed;
    Ptr += Parsed;
    ActualInterfaces ++;
  } while (LengthLeft > 0);
  //
  // Parse for additional alt setting;
  //
  return EFI_SUCCESS;
}

/**

  Get the string table stored in a usb device.
  
  @param  UsbIoDev              USB_IO_DEVICE device structure.
  
  @retval EFI_SUCCESS
  @retval EFI_UNSUPPORTED
  @retval EFI_OUT_OF_RESOURCES
  
**/
EFI_STATUS
UsbGetStringtable (
  IN USB_IO_DEVICE                      *UsbIoDev
  )
{
  EFI_STATUS                    Status;
  UINT32                        Result;
  EFI_USB_SUPPORTED_LANGUAGES   *LanguageTable;
  UINT8                         *Buffer;
  UINT8                         *Ptr;
  UINTN                         Index;
  UINTN                         LangTableSize;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  UINT16                        TempBuffer;

  UsbIo   = &(UsbIoDev->UsbController[0]->UsbIo);

  //
  // We get first 2 byte of langID table,
  // so we can have the whole table length
  //
  Status = mUsbCore->UsbGetString (
                       UsbIo,
                       0,
                       0,
                       &TempBuffer,
                       2,
                       &Result
                       );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  LanguageTable = (EFI_USB_SUPPORTED_LANGUAGES *) &TempBuffer;

  if (LanguageTable->Length == 0) {
    return EFI_UNSUPPORTED;
  }
  //
  // If length is 2, then there is no string table
  //
  if (LanguageTable->Length == 2) {
    return EFI_UNSUPPORTED;
  }

  Status = mUsbCore->AllocateBuffer(
                       LanguageTable->Length,
                       ALIGNMENT_32,
                       (VOID **)&Buffer
                       );
  
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  //
  // Now we get the whole LangID table
  //
  Status = mUsbCore->UsbGetString (
                       UsbIo,
                       0,
                       0,
                       Buffer,
                       LanguageTable->Length,
                       &Result
                       );
  if (EFI_ERROR (Status)) {
    mUsbCore->FreeBuffer (
                LanguageTable->Length,
                Buffer
                );
    return EFI_UNSUPPORTED;
  }

  LanguageTable = (EFI_USB_SUPPORTED_LANGUAGES *) Buffer;

  //
  // Ptr point to the LangID table
  //
  Ptr           = Buffer + 2;
  LangTableSize = (LanguageTable->Length - 2) / 2;

  for (Index = 0; Index < LangTableSize && Index < USB_MAXLANID; Index++) {
    UsbIoDev->LangID[Index] = *((UINT16 *) Ptr);
    Ptr += 2;
  }

  mUsbCore->FreeBuffer (
              LanguageTable->Length,
              Buffer
              );
  return EFI_SUCCESS;
}

/**

  This function is to parse all the configuration descriptor.
  
  @param  UsbIoDev              USB_IO_DEVICE device structure.
  
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_OUT_OF_RESOURCES  

**/
EFI_STATUS
UsbGetAllConfigurations (
  IN USB_IO_DEVICE                      *UsbIoDev
  )
{
  EFI_STATUS                    Status;
  UINT32                        Result;
  UINTN                         Index;
  UINTN                         TotalLength;
  UINT8                         *Buffer;
  CONFIG_DESC_LIST_ENTRY        *ConfigDescEntry;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  BOOLEAN                       Retried;

  InitializeListHead (&UsbIoDev->ConfigDescListHead);
  UsbIo   = &(UsbIoDev->UsbController[0]->UsbIo);

  for (Index = 0; Index < UsbIoDev->DeviceDescriptor.NumConfigurations; Index++) {
    Status = mUsbCore->AllocateBuffer(
                         sizeof (CONFIG_DESC_LIST_ENTRY),
                         ALIGNMENT_32,
                         (VOID **)&ConfigDescEntry
                         );
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      goto exit;
    }
    //
    // Stall 1ms between two GetDescriptor command for specific device compatibility(0.1ms minimum)
    //
    mUsbCore->Stall (1000);
    //
    // 1st only get 1st 8 bytes config descriptor, so we can know the whole length.
    // Don't set the length less then 8 due to some device may return incorrect content
    //
    Status = mUsbCore->UsbGetDescriptor (
                         UsbIo,
                         (UINT16) ((USB_DESC_TYPE_CONFIG << 8) | Index),
                         0,
                         sizeof (EFI_USB_CONFIG_DESCRIPTOR),
                         &ConfigDescEntry->CongfigDescriptor,
                         &Result
                         );
    if (EFI_ERROR (Status)) {
      DEBUG ((gUSBErrorLevel, "First get config descriptor error\n"));
      Status = EFI_DEVICE_ERROR;
      goto free_config_desc_entry;
    }
    TotalLength = ConfigDescEntry->CongfigDescriptor.TotalLength;
    //
    // Reject if TotalLength even cannot cover itself.
    //
    if (TotalLength < OFFSET_OF (EFI_USB_CONFIG_DESCRIPTOR, TotalLength) + sizeof (ConfigDescEntry->CongfigDescriptor.TotalLength)) {
      Status = EFI_DEVICE_ERROR;
      goto free_config_desc_entry;
    }
    Retried     = FALSE;
GET_CONFIG_RETRY:    
    Status = mUsbCore->AllocateBuffer(
                         TotalLength,
                         ALIGNMENT_32,
                         (VOID **)&Buffer
                         );
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      goto free_config_desc_entry;
    }
    //
    // Stall 1ms between two GetDescriptor command for specific device compatibility(0.1ms minimum)
    //
    mUsbCore->Stall (1000);
    //
    // Then we get the total descriptors for this configuration
    //
    Status = mUsbCore->UsbGetDescriptor (
                         UsbIo,
                         (UINT16) ((USB_DESC_TYPE_CONFIG << 8) | Index),
                         0,
                         (UINT16) TotalLength,
                         Buffer,
                         &Result
                         );
    if (EFI_ERROR (Status)) {
      DEBUG ((gUSBErrorLevel, "Get whole config descriptor error\n"));
      Status = EFI_DEVICE_ERROR;
      mUsbCore->FreeBuffer (
                  TotalLength,
                  Buffer
                  );
      goto free_config_desc_entry;
    }
    InitializeListHead (&ConfigDescEntry->InterfaceDescListHead);
    //
    // Parse this whole configuration
    //
    Status = ParseThisConfig (ConfigDescEntry, Buffer, TotalLength);

    if (EFI_ERROR (Status)) {
      //
      // Ignore this configuration, parse next one
      //
      mUsbCore->FreeBuffer (
                  TotalLength,
                  Buffer
                  );
      if (Status == EFI_DEVICE_ERROR && !Retried) {
        //
        // Double check is it a vaild config descriptor as some device will
        // report either wrong total length or total endpoints
        //
        TotalLength = (TotalLength + 0xff) & ~0xff;
        Retried = TRUE;
        goto GET_CONFIG_RETRY;
      }
      mUsbCore->FreeBuffer (
                  sizeof (CONFIG_DESC_LIST_ENTRY),
                  ConfigDescEntry
                  );
      continue;
    }
    InsertTailList (&UsbIoDev->ConfigDescListHead, &ConfigDescEntry->Link);
    mUsbCore->FreeBuffer (
                TotalLength,
                Buffer
                );
    //
    // Setup Smm address convert table for Smm security policy
    //
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_BODY,
                ConfigDescEntry,
                sizeof (CONFIG_DESC_LIST_ENTRY)
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_LINKING_LIST,
                &ConfigDescEntry->InterfaceDescListHead,
                1
                );
  }
  Status = EFI_SUCCESS;
  goto exit;
free_config_desc_entry:
  mUsbCore->FreeBuffer (
              sizeof (CONFIG_DESC_LIST_ENTRY),
              ConfigDescEntry
              );
exit:
  return Status;
}

/**

  Set the device to a configuration value.
  
  @param  UsbIoDev              USB_IO_DEVICE to be set configuration
  @param  ConfigrationValue     The configuration value to be set to that device
  
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  
**/
EFI_STATUS
UsbSetConfiguration (
  IN USB_IO_DEVICE                      *UsbIoDev,
  IN UINTN                              ConfigurationValue
  )
{
  LIST_ENTRY              *NextEntry;
  CONFIG_DESC_LIST_ENTRY  *ConfigEntry;
  UINT32                  Result;
  EFI_STATUS              Status;
  EFI_USB_IO_PROTOCOL     *UsbIo;

  UsbIo     = &(UsbIoDev->UsbController[0]->UsbIo);
  NextEntry = UsbIoDev->ConfigDescListHead.ForwardLink;

  while (NextEntry != &UsbIoDev->ConfigDescListHead) {
    //
    // Get one entry
    //
    ConfigEntry = (CONFIG_DESC_LIST_ENTRY *) NextEntry;
    if (ConfigEntry->CongfigDescriptor.ConfigurationValue == ConfigurationValue) {
      //
      // Find one, set to the active configuration
      //
      UsbIoDev->ActiveConfig = ConfigEntry;
      break;
    }

    NextEntry = NextEntry->ForwardLink;
  }
  //
  // Next Entry should not be null
  //
  Status = mUsbCore->UsbSetDeviceConfiguration (
                       UsbIo,
                       (UINT16) ConfigurationValue,
                       &Result
                       );
  return Status;
}

/*++

  Set the device to a default configuration value.
  
  @param  UsbIoDev              USB_IO_DEVICE to be set configuration
  
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  
--*/
EFI_STATUS
UsbSetDefaultConfiguration (
  IN  USB_IO_DEVICE                     *UsbIoDev
  )
{
  CONFIG_DESC_LIST_ENTRY  *ConfigEntry;
  UINT16                  ConfigValue;
  LIST_ENTRY              *NextEntry;

  if (IsListEmpty (&UsbIoDev->ConfigDescListHead)) {
    return EFI_DEVICE_ERROR;
  }

  NextEntry   = UsbIoDev->ConfigDescListHead.ForwardLink;

  ConfigEntry = (CONFIG_DESC_LIST_ENTRY *) NextEntry;
  ConfigValue = ConfigEntry->CongfigDescriptor.ConfigurationValue;

  return UsbSetConfiguration (UsbIoDev, ConfigValue);
}

/**

  Delete all configuration data when device is not used.
  
  @param  UsbIoDev              USB_IO_DEVICE to be set configuration

**/
VOID
UsbDestroyAllConfiguration (
  IN  USB_IO_DEVICE                     *UsbIoDev
  )
{
  CONFIG_DESC_LIST_ENTRY        *ConfigEntry;
  INTERFACE_DESC_LIST_ENTRY     *InterfaceEntry;
  ENDPOINT_DESC_LIST_ENTRY      *EndpointEntry;
  LIST_ENTRY                    *NextEntry;

  //
  // Delete all configuration descriptor data
  //
  ConfigEntry = (CONFIG_DESC_LIST_ENTRY *) UsbIoDev->ConfigDescListHead.ForwardLink;

  while (ConfigEntry && ConfigEntry != (CONFIG_DESC_LIST_ENTRY *) &UsbIoDev->ConfigDescListHead) {
    //
    // Delete all its interface descriptors
    //
    InterfaceEntry = (INTERFACE_DESC_LIST_ENTRY *) ConfigEntry->InterfaceDescListHead.ForwardLink;

    while (InterfaceEntry != (INTERFACE_DESC_LIST_ENTRY *) &ConfigEntry->InterfaceDescListHead) {
      //
      // Delete all its endpoint descriptors
      //
      EndpointEntry = (ENDPOINT_DESC_LIST_ENTRY *) InterfaceEntry->EndpointDescListHead.ForwardLink;
      while (EndpointEntry != (ENDPOINT_DESC_LIST_ENTRY *) &InterfaceEntry->EndpointDescListHead) {
        NextEntry = ((LIST_ENTRY *) EndpointEntry)->ForwardLink;
        RemoveEntryList ((LIST_ENTRY *) EndpointEntry);
        mUsbCore->FreeBuffer (
                    sizeof (ENDPOINT_DESC_LIST_ENTRY),
                    EndpointEntry
                    );
        //
        // Remove Smm address convert table
        //
        mUsbCore->RemoveAddressConvertTable (
                    ACT_INSTANCE_BODY,
                    EndpointEntry
                    );
        EndpointEntry = (ENDPOINT_DESC_LIST_ENTRY *) NextEntry;
      }

      NextEntry = ((LIST_ENTRY *) InterfaceEntry)->ForwardLink;
      RemoveEntryList ((LIST_ENTRY *) InterfaceEntry);
      mUsbCore->FreeBuffer (
                  sizeof (INTERFACE_DESC_LIST_ENTRY),
                  InterfaceEntry
                  );
      //
      // Remove Smm address convert table
      //
      mUsbCore->RemoveAddressConvertTable (
                  ACT_INSTANCE_BODY,
                  InterfaceEntry
                  );
      mUsbCore->RemoveAddressConvertTable (
                  ACT_LINKING_LIST,
                  &InterfaceEntry->EndpointDescListHead
                  );
      InterfaceEntry = (INTERFACE_DESC_LIST_ENTRY *) NextEntry;
    }

    NextEntry = ((LIST_ENTRY *) ConfigEntry)->ForwardLink;
    RemoveEntryList ((LIST_ENTRY *) ConfigEntry);
    mUsbCore->FreeBuffer (
                sizeof (CONFIG_DESC_LIST_ENTRY),
                ConfigEntry
                );
    //
    // Remove Smm address convert table
    //
    mUsbCore->RemoveAddressConvertTable (
                ACT_INSTANCE_BODY,
                ConfigEntry
                );
    mUsbCore->RemoveAddressConvertTable (
                ACT_LINKING_LIST,
                &ConfigEntry->InterfaceDescListHead
                );
    ConfigEntry = (CONFIG_DESC_LIST_ENTRY *) NextEntry;
  }
}

/**

  Tell if there is a device connected to that port according to
  the Port Status.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortConnect (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 0 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_CONNECTION) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if Port is enabled.

  @param  PortStatus            The status value of that port.

  @retval TRUE                  Port is enable
  @retval FALSE                 Port is disable

**/
BOOLEAN
IsPortEnable (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 1 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_ENABLE) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if the port is being reset.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortInReset (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 4 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_RESET) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if there is power applied to that port.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortPowerApplied (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 8 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_POWER) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if the connected device is a low device.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortLowSpeedDeviceAttached (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 9 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_LOW_SPEED) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if the port is suspend.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortSuspend (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 2 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_SUSPEND) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if there is a Connect Change status in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortConnectChange (
  IN UINT16  PortChangeStatus
  )
{
  //
  // return the bit 0 value of PortChangeStatus
  //
  if ((PortChangeStatus & USB_PORT_STAT_C_CONNECTION) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if there is a Enable/Disable change in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortEnableDisableChange (
  IN UINT16  PortChangeStatus
  )
{
  //
  // return the bit 1 value of PortChangeStatus
  //
  if ((PortChangeStatus & USB_PORT_STAT_C_ENABLE) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if there is a Port Reset Change status in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortResetChange (
  IN UINT16  PortChangeStatus
  )
{
  //
  // return the bit 4 value of PortChangeStatus
  //
  if ((PortChangeStatus & (USB_PORT_STAT_C_RESET | USB_PORT_STAT_C_BH_RESET)) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if there is a Suspend Change Status in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortSuspendChange (
  IN UINT16  PortChangeStatus
  )
{
  //
  // return the bit 2 value of PortChangeStatus
  //
  if ((PortChangeStatus & USB_PORT_STAT_C_SUSPEND) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Reset if the device is disconencted or not

  @param  UsbIoController       Indicating the Usb Controller Device.
  @param  Disconnected          Indicate whether the device is disconencted or not

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
EFI_STATUS
IsDeviceDisconnected (
  IN USB_IO_CONTROLLER_DEVICE    *UsbIoController,
  IN OUT BOOLEAN                 *Disconnected
  )
{
  USB_IO_DEVICE             *ParentIoDev;
  USB_IO_DEVICE             *UsbIoDev;
  USB_IO_CONTROLLER_DEVICE  *ParentController;
  UINT8                     HubPort;
  EFI_STATUS                Status;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  EFI_USB_PORT_STATUS       PortStatus;

  ParentController  = UsbIoController->Parent;
  ParentIoDev       = ParentController->UsbDevice;
  UsbIoDev          = UsbIoController->UsbDevice;
  HubPort           = UsbIoController->ParentPort;

  if (ParentIoDev->DeviceAddress == 1) {
    //
    // Connected to the root hub
    //
    GetRootHubPortStatus (
      ParentIoDev->BusController->Usb3HCInterface,
      HubPort,
      &PortStatus
      );

  } else {
    UsbIo = &UsbIoController->UsbIo;
    Status = HubGetPortStatus (
              &ParentController->UsbIo,
              HubPort + 1,
              (UINT32 *) &PortStatus
              );

    if (EFI_ERROR (Status)) {
      return IsDeviceDisconnected (ParentController, Disconnected);
    }
  }

  *Disconnected = FALSE;

  if (!IsPortConnect (PortStatus.PortStatus)) {
    *Disconnected = TRUE;
  }

  return EFI_SUCCESS;
}

/**

  Get the first AlternateSetting in the Interface ListEntry

  @param  This                  EFI_USB_IO_PROTOCOL

  @retval The first AlternateSetting value

**/
UINT8
GetFirstAlternateSetting (
  IN EFI_USB_IO_PROTOCOL                *This
  )
{
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  USB_IO_DEVICE             *UsbIoDev;
  LIST_ENTRY                *InterfaceListHead;
  INTERFACE_DESC_LIST_ENTRY *InterfaceListEntry;

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;
  if (!UsbIoDev->IsConfigured) {
    return 0;
  }
  InterfaceListHead   = &UsbIoDev->ActiveConfig->InterfaceDescListHead;
  InterfaceListEntry  = (INTERFACE_DESC_LIST_ENTRY *) (InterfaceListHead->ForwardLink);
  //
  // Loop all interface descriptor to get match one.
  //
  while (InterfaceListEntry != (INTERFACE_DESC_LIST_ENTRY *) InterfaceListHead) {
    if (InterfaceListEntry->InterfaceDescriptor.InterfaceNumber == UsbIoController->InterfaceNumber) {
      return InterfaceListEntry->InterfaceDescriptor.AlternateSetting;
    }
    InterfaceListEntry = (INTERFACE_DESC_LIST_ENTRY *) InterfaceListEntry->Link.ForwardLink;
  }
  return 0;
}

/**

  Set the first AlternateSetting in the Interface ListEntry

  @param  This                  EFI_USB_IO_PROTOCOL

**/
VOID
SetFirstAlternateSetting (
  IN EFI_USB_IO_PROTOCOL                *This
  )
{
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  USB_IO_DEVICE             *UsbIoDev;
  LIST_ENTRY                *InterfaceListHead;
  INTERFACE_DESC_LIST_ENTRY *InterfaceListEntry;
  UINTN                     NumberAlterSetting;
  EFI_USB_DEVICE_REQUEST    DevReq;
  UINT32                    RetStatus;

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;
  if (!UsbIoDev->IsConfigured) {
    return;
  }
  InterfaceListHead   = &UsbIoDev->ActiveConfig->InterfaceDescListHead;
  InterfaceListEntry  = (INTERFACE_DESC_LIST_ENTRY *) (InterfaceListHead->ForwardLink);
  //
  // Loop all interface descriptor to get match one.
  //
  NumberAlterSetting = 0;
  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));
  while (InterfaceListEntry != (INTERFACE_DESC_LIST_ENTRY *) InterfaceListHead) {
    if (InterfaceListEntry->InterfaceDescriptor.InterfaceNumber == UsbIoController->InterfaceNumber) {
      if (NumberAlterSetting == 0) {
      	DevReq.Index = InterfaceListEntry->InterfaceDescriptor.InterfaceNumber;
      	DevReq.Value = InterfaceListEntry->InterfaceDescriptor.AlternateSetting;
      }
      NumberAlterSetting ++;
    }
    InterfaceListEntry = (INTERFACE_DESC_LIST_ENTRY *) InterfaceListEntry->Link.ForwardLink;
  }
  if (NumberAlterSetting > 1) {
    DevReq.Request     = USB_DEV_SET_INTERFACE;
    DevReq.RequestType = USB_DEV_SET_INTERFACE_REQ_TYPE;
    UsbControlTransfer (
      This,
      &DevReq,
      EfiUsbNoData,
      1000,
      NULL,
      0,
      &RetStatus
      );
  }
}

/**

  Get the HID descriptor in the Interface ListEntry

  @param  This                  EFI_USB_IO_PROTOCOL

  @retval The HID descriptor

**/
EFI_USB_HID_DESCRIPTOR*
GetHidDescriptor (
  IN EFI_USB_IO_PROTOCOL                *This
  )
{
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  USB_IO_DEVICE             *UsbIoDev;
  LIST_ENTRY                *InterfaceListHead;
  INTERFACE_DESC_LIST_ENTRY *InterfaceListEntry;

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;
  if (!UsbIoDev->IsConfigured) {
    return NULL;
  }
  InterfaceListHead   = &UsbIoDev->ActiveConfig->InterfaceDescListHead;
  InterfaceListEntry  = (INTERFACE_DESC_LIST_ENTRY *) (InterfaceListHead->ForwardLink);
  //
  // Loop all interface descriptor to get match one.
  //
  while (InterfaceListEntry != (INTERFACE_DESC_LIST_ENTRY *) InterfaceListHead) {
    if (InterfaceListEntry->InterfaceDescriptor.InterfaceNumber == UsbIoController->InterfaceNumber &&
        InterfaceListEntry->InterfaceDescriptor.AlternateSetting == UsbIoController->AlternateSetting) {
      return (InterfaceListEntry->HidDescriptor.Length != 0) ? &InterfaceListEntry->HidDescriptor : NULL;
    }
    InterfaceListEntry = (INTERFACE_DESC_LIST_ENTRY *) InterfaceListEntry->Link.ForwardLink;
  }
  return NULL;
}

INTERFACE_DESC_LIST_ENTRY*
FindInterfaceListEntry (
  IN EFI_USB_IO_PROTOCOL        *This
  )
{
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  USB_IO_DEVICE             *UsbIoDev;
  LIST_ENTRY                *InterfaceListHead;
  INTERFACE_DESC_LIST_ENTRY *InterfaceListEntry;

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;

  if (!UsbIoDev->IsConfigured) {
    return NULL;
  }

  InterfaceListHead   = &UsbIoDev->ActiveConfig->InterfaceDescListHead;
  InterfaceListEntry  = (INTERFACE_DESC_LIST_ENTRY *) (InterfaceListHead->ForwardLink);

  //
  // Loop all interface descriptor to get match one.
  //
  while (InterfaceListEntry != (INTERFACE_DESC_LIST_ENTRY *) InterfaceListHead) {
    if (InterfaceListEntry->InterfaceDescriptor.InterfaceNumber == UsbIoController->InterfaceNumber &&
        InterfaceListEntry->InterfaceDescriptor.AlternateSetting == UsbIoController->AlternateSetting) {
      return InterfaceListEntry;
    }

    InterfaceListEntry = (INTERFACE_DESC_LIST_ENTRY *) InterfaceListEntry->Link.ForwardLink;
  }

  return NULL;
}

/**

  Find EndPoint ListEntry.

  @param  This                  EFI_USB_IO_PROTOCOL   
  @param  EndPointAddress       Endpoint address.

  @retval ENDPOINT_DESC_LIST_ENTRY pointer

**/
ENDPOINT_DESC_LIST_ENTRY *
FindEndPointListEntry (
  IN EFI_USB_IO_PROTOCOL    *This,
  IN UINT8                  EndPointAddress
  )
{
  INTERFACE_DESC_LIST_ENTRY *InterfaceListEntry;
  LIST_ENTRY                *EndpointDescListHead;
  ENDPOINT_DESC_LIST_ENTRY  *EndPointListEntry;

  InterfaceListEntry = FindInterfaceListEntry (This);
  if (InterfaceListEntry != NULL) {
    EndpointDescListHead  = &InterfaceListEntry->EndpointDescListHead;
    EndPointListEntry     = (ENDPOINT_DESC_LIST_ENTRY *) (EndpointDescListHead->ForwardLink);

    //
    // Loop all interface descriptor to get match one.
    //
    while (EndPointListEntry != (ENDPOINT_DESC_LIST_ENTRY *) EndpointDescListHead) {
      if (EndPointListEntry->EndpointDescriptor.EndpointAddress == EndPointAddress) {
        return EndPointListEntry;
      }

      EndPointListEntry = (ENDPOINT_DESC_LIST_ENTRY *) EndPointListEntry->Link.ForwardLink;
    }
  }

  return NULL;
}

/**

  Set the datatoggle of a specified endpoint

  @param  UsbIo                 Given Usb Controller device.
  @param  EndpointAddr          Given Endpoint address.
  @param  DataToggle            The current data toggle of that endpoint to be set

**/
VOID
SetDataToggleBit (
  IN EFI_USB_IO_PROTOCOL    *UsbIo,
  IN UINT8                  EndpointAddr,
  IN UINT8                  DataToggle
  )
{

  ENDPOINT_DESC_LIST_ENTRY  *EndpointListEntry;

  EndpointListEntry = FindEndPointListEntry (UsbIo, EndpointAddr);
  if (EndpointListEntry == NULL) {
    return ;
  }

  EndpointListEntry->Toggle = DataToggle;
  return ;
}

/**

  Usb Set Device Address

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  AddressValue          Device address 
  @param  Status                Transfer status

  @retval EFI_SUCCESS           Success
  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
UsbSetDeviceAddress (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  AddressValue,
  OUT UINT32                  *Status
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  DevReq.RequestType  = USB_DEV_SET_ADDRESS_REQ_TYPE;
  DevReq.Request      = USB_DEV_SET_ADDRESS;
  DevReq.Value        = AddressValue;

  return UsbControlTransfer (
           UsbIo,
           &DevReq,
           EfiUsbNoData,
           3 * 1000,
           NULL,
           0,
           Status
           );
}

/**

  Submits control transfer to a target USB device.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  DeviceAddress         Represents the address of the target device on the USB,
                                which is assigned during USB enumeration.
  @param  DeviceSpeed           Indicates target device speed.
  @param  MaximumPacketLength   Indicates the maximum packet size that the
                                default control transfer endpoint is capable of
                                sending or receiving.
  @param  Request               A pointer to the USB device request that will be sent
                                to the USB device.
  @param  TransferDirection     Specifies the data direction for the transfer.
                                There are three values available, DataIn, DataOut
                                and NoData.
  @param  Data                  A pointer to the buffer of data that will be transmitted
                                to USB device or received from USB device.
  @param  DataLength            Indicates the size, in bytes, of the data buffer
                                specified by Data.
  @param  TimeOut               Indicates the maximum time, in milliseconds,
                                which the transfer is allowed to complete.
  @param  Translator            A pointr to the transaction translator data.
  @param  TransferResult        A pointer to the detailed result information generated
                                by this control transfer.

  @retval EFI_SUCCESS           The control transfer was completed successfully.
  @retval EFI_OUT_OF_RESOURCES  The control transfer could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER Some parameters are invalid.
  @retval EFI_TIMEOUT           The control transfer failed due to timeout.
  @retval EFI_DEVICE_ERROR      The control transfer failed due to host controller or device error.
                                Caller should check TranferResult for detailed error information.

**/
EFI_STATUS
EFIAPI
ControlTransfer (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN     EFI_USB_DEVICE_REQUEST                 *Request,
  IN     EFI_USB_DATA_DIRECTION                 TransferDirection,
  IN OUT VOID                                   *Data       OPTIONAL,
  IN OUT UINTN                                  *DataLength OPTIONAL,
  IN     UINTN                                  TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  OUT    UINT32                                 *TransferResult
  )
{
  EFI_STATUS                            Status;
  EFI_USB3_HC_PROTOCOL_CONTROL_TRANSFER ControlTransferFunc = This->ControlTransfer;
  
  if (mInSmram) {
    Status = mUsbCore->AddressConvert (
                         SMM_ADDRESS,
                         (VOID*)(UINTN)ControlTransferFunc,
                         (VOID**)&ControlTransferFunc
                         );
    if (Status == EFI_NOT_FOUND) return Status;
  }
  return ControlTransferFunc (
           This,
           DeviceAddress,
           DeviceSpeed,
           MaximumPacketLength,
           Request,
           TransferDirection,
           Data,
           DataLength,
           TimeOut,
           Translator,
           TransferResult
           );
}

/**

  Submits bulk transfer to a bulk endpoint of a USB device.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  DeviceAddress         Represents the address of the target device on the USB,
                                which is assigned during USB enumeration.
  @param  EndPointAddress       The combination of an endpoint number and an
                                endpoint direction of the target USB device.
                                Each endpoint address supports data transfer in
                                one direction except the control endpoint
                                (whose default endpoint address is 0).
                                It is the caller's responsibility to make sure that
                                the EndPointAddress represents a bulk endpoint.
  @param  DeviceSpeed           Indicates device speed. The supported values are EFI_USB_SPEED_FULL
                                and EFI_USB_SPEED_HIGH.
  @param  MaximumPacketLength   Indicates the maximum packet size the target endpoint
                                is capable of sending or receiving.
  @param  DataBuffersNumber     Number of data buffers prepared for the transfer.
  @param  Data                  Array of pointers to the buffers of data that will be transmitted
                                to USB device or received from USB device.
  @param  DataLength            When input, indicates the size, in bytes, of the data buffer
                                specified by Data. When output, indicates the actually
                                transferred data size.
  @param  DataToggle            A pointer to the data toggle value. On input, it indicates
                                the initial data toggle value the bulk transfer should adopt;
                                on output, it is updated to indicate the data toggle value
                                of the subsequent bulk transfer.
  @param  TimeOut               Indicates the maximum time, in milliseconds, which the
                                transfer is allowed to complete.
  @param  Translator            A pointr to the transaction translator data.
  @param  CallBackFunction      The Callback function.This function is called at the
                                rate specified by PollingInterval.This parameter is
                                only required when request asynchronous interrupt transfer.
  @param  Context               The context that is passed to the CallBackFunction.
                                This is an optional parameter and may be NULL.
  @param  PollingInterval       Indicates the interval, in milliseconds, that the
                                asynchronous interrupt transfer is polled.
                                This parameter is required request asynchronous interrupt
                                transfer.
  @param  TransferResult        A pointer to the detailed result information of the
                                bulk transfer.

  @retval EFI_SUCCESS           The bulk transfer was completed successfully.
  @retval EFI_OUT_OF_RESOURCES  The bulk transfer could not be submitted due to lack of resource.
  @retval EFI_INVALID_PARAMETER Some parameters are invalid.
  @retval EFI_TIMEOUT           The bulk transfer failed due to timeout.
  @retval EFI_DEVICE_ERROR      The bulk transfer failed due to host controller or device error.
                                Caller should check TranferResult for detailed error information.

**/
EFI_STATUS
EFIAPI
BulkTransfer (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN     UINT8                                  DataBuffersNumber,
  IN OUT VOID                                   *Data[EFI_USB_MAX_BULK_BUFFER_NUM],
  IN OUT UINTN                                  *DataLength,
  IN OUT UINT8                                  *DataToggle,
  IN     UINTN                                  TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK        CallBackFunction OPTIONAL,
  IN     VOID                                   *Context         OPTIONAL,
  OUT    UINT32                                 *TransferResult
  )
{
  EFI_STATUS                            Status;
  EFI_USB3_HC_PROTOCOL_BULK_TRANSFER    BulkTransferFunc = This->BulkTransfer;
  
  if (mInSmram) {
    Status = mUsbCore->AddressConvert (
                         SMM_ADDRESS,
                         (VOID*)(UINTN)BulkTransferFunc,
                         (VOID**)&BulkTransferFunc
                         );
    if (Status == EFI_NOT_FOUND) return Status;
  }
  return BulkTransferFunc (
           This,
           DeviceAddress,
           EndPointAddress,
           DeviceSpeed,
           MaximumPacketLength,
           DataBuffersNumber,
           Data,
           DataLength,
           DataToggle,
           TimeOut,
           Translator,
           CallBackFunction,
           Context,
           TransferResult
           );
}

/**

  Submits synchronous interrupt transfer to an interrupt endpoint
  of a USB device.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  DeviceAddress         Represents the address of the target device on the USB,
                                which is assigned during USB enumeration.
  @param  EndPointAddress       The combination of an endpoint number and an endpoint
                                direction of the target USB device. Each endpoint
                                address supports data transfer in one direction
                                except the control endpoint (whose default
                                endpoint address is 0). It is the caller's responsibility
                                to make sure that the EndPointAddress represents
                                an interrupt endpoint.
  @param  DeviceSpeed           Indicates device speed.
  @param  MaximumPacketLength   Indicates the maximum packet size the target endpoint
                                is capable of sending or receiving.
  @param  Data                  A pointer to the buffer of data that will be transmitted
                                to USB device or received from USB device.
  @param  DataLength            On input, the size, in bytes, of the data buffer specified
                                by Data. On output, the number of bytes transferred.
  @param  DataToggle            A pointer to the data toggle value. On input, it indicates
                                the initial data toggle value the synchronous interrupt
                                transfer should adopt;
                                on output, it is updated to indicate the data toggle value
                                of the subsequent synchronous interrupt transfer.
  @param  TimeOut               Indicates the maximum time, in milliseconds, which the
                                transfer is allowed to complete.
  @param  Translator            A pointr to the transaction translator data.
  @param  CallBackFunction      The Callback function.This function is called at the
                                rate specified by PollingInterval.This parameter is
                                only required when request asynchronous interrupt transfer.
  @param  Context               The context that is passed to the CallBackFunction.
                                his is an optional parameter and may be NULL.
  @param  PollingInterval       Indicates the interval, in milliseconds, that the
                                asynchronous interrupt transfer is polled.
                                This parameter is required request asynchronous interrupt
                                transfer.
  @param  TransferResult        A pointer to the detailed result information from
                                the synchronous interrupt transfer.

  @retval EFI_SUCCESS           The synchronous interrupt transfer was completed successfully.
  @retval EFI_OUT_OF_RESOURCES  The synchronous interrupt transfer could not be submitted due
                                to lack of resource.
  @retval EFI_INVALID_PARAMETE  Some parameters are invalid.
  @retval EFI_TIMEOUT           The synchronous interrupt transfer failed due to timeout.
  @retval EFI_DEVICE_ERROR      The synchronous interrupt transfer failed due to host controller
                                or device error. Caller should check TranferResult for detailed
                                error information.

**/
EFI_STATUS
EFIAPI
InterruptTransfer (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN OUT VOID                                   *Data,
  IN OUT UINTN                                  *DataLength,
  IN OUT UINT8                                  *DataToggle,
  IN     UINTN                                  TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK        CallBackFunction OPTIONAL,
  IN     VOID                                   *Context         OPTIONAL,
  IN     UINTN                                  PollingInterval  OPTIONAL,
  OUT    UINT32                                 *TransferResult
  )
{
  EFI_STATUS                                    Status;
  EFI_USB3_HC_PROTOCOL_INTERRUPT_TRANSFER       InterruptTransferFunc = This->InterruptTransfer;

  if (mInSmram) {
    Status = mUsbCore->AddressConvert (
                         SMM_ADDRESS,
                         (VOID*)(UINTN)InterruptTransferFunc,
                         (VOID**)&InterruptTransferFunc
                         );
    if (Status == EFI_NOT_FOUND) return Status;
  }
  return InterruptTransferFunc (
           This,
           DeviceAddress,
           EndPointAddress,
           DeviceSpeed,
           MaximumPacketLength,
           Data,
           DataLength,
           DataToggle,
           TimeOut,
           Translator,
           CallBackFunction,
           Context,
           PollingInterval,
           TransferResult
           );
}

/**

  Cancel the interrupt transfer allocated by AsyncInterruptTransfer.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  DeviceAddress         Represents the address of the target device on the USB,
                                which is assigned during USB enumeration.
  @param  EndPointAddress       Endpoint address. Value 0 indicates to stop the Slot
  @param  PollingInterval       Indicates the interval, in milliseconds, that the
                                asynchronous interrupt transfer is polled.
                                This parameter is required request asynchronous interrupt
                                transfer.
  @param  DataToggle            Unused in XHC, always set to 0
  
  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
CancelIo (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINTN                                  PollingInterval,
  IN OUT UINT8                                  *DataToggle
  )
{
  EFI_STATUS                            Status;
  EFI_USB3_HC_PROTOCOL_CANCEL_IO        CancelIoFunc = This->CancelIo;

  if (mInSmram) {
    Status = mUsbCore->AddressConvert (
                         SMM_ADDRESS,
                         (VOID*)(UINTN)CancelIoFunc,
                         (VOID**)&CancelIoFunc
                         );
    if (Status == EFI_NOT_FOUND) return Status;
  }
  return CancelIoFunc (
           This,
           DeviceAddress,
           EndPointAddress,
           PollingInterval,
           DataToggle
           );
}

/**

  Retrieves the current status of a USB root hub port.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL.
  @param  PortNumber            Specifies the root hub port from which the status
                                is to be retrieved.  This value is zero-based. For example,
                                if a root hub has two ports, then the first port is numbered 0,
                                and the second port is numbered 1.
  @param  PortStatus            A pointer to the current port status bits and
                                port status change bits.

  @retval EFI_SUCCESS           The status of the USB root hub port specified
                                by PortNumber was returned in PortStatus.
  @retval EFI_INVALID_PARAMETER PortNumber is invalid.
  @retval EFI_DEVICE_ERROR      Can't read register

**/
EFI_STATUS
EFIAPI
GetRootHubPortStatus (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  PortNumber,
  OUT    EFI_USB_PORT_STATUS                    *PortStatus
  )
{
  EFI_STATUS                                    Status;
  EFI_USB3_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS  GetRootHubPortStatusFunc = This->GetRootHubPortStatus;
  
  if (mInSmram) {
    Status = mUsbCore->AddressConvert (
                         SMM_ADDRESS,
                         (VOID*)(UINTN)GetRootHubPortStatusFunc,
                         (VOID**)&GetRootHubPortStatusFunc
                         );
    if (Status == EFI_NOT_FOUND) return Status;
  }
  return GetRootHubPortStatusFunc (
           This,
           PortNumber,
           PortStatus
           );
}

/**

  Sets a feature for the specified root hub port.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL.
  @param  PortNumber            Specifies the root hub port whose feature
                                is requested to be set.
  @param  PortFeature           Indicates the feature selector associated
                                with the feature set request.

  @retval EFI_SUCCESS           The feature specified by PortFeature was set for the
                                USB root hub port specified by PortNumber.
  @retval EFI_INVALID_PARAMETER PortNumber is invalid or PortFeature is invalid.
  @retval EFI_DEVICE_ERROR      Can't read register

**/
EFI_STATUS
EFIAPI
SetRootHubPortFeature (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  PortNumber,
  IN     EFI_USB_PORT_FEATURE                   PortFeature
  )
{
  EFI_STATUS                                    Status;
  EFI_USB3_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE SetRootHubPortFeatureFunc = This->SetRootHubPortFeature;
  
  if (mInSmram) {
    Status = mUsbCore->AddressConvert (
                         SMM_ADDRESS,
                         (VOID*)(UINTN)SetRootHubPortFeatureFunc,
                         (VOID**)&SetRootHubPortFeatureFunc
                         );
    if (Status == EFI_NOT_FOUND) return Status;
  }
  return SetRootHubPortFeatureFunc (
           This,
           PortNumber,
           PortFeature
           );
}

/**

  Clears a feature for the specified root hub port.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  PortNumber            Specifies the root hub port whose feature
                                is requested to be cleared.
  @param  PortFeature           Indicates the feature selector associated with the
                                feature clear request.

  @retval EFI_SUCCESS           The feature specified by PortFeature was cleared for the
                                USB root hub port specified by PortNumber.
  @retval EFI_INVALID_PARAMETER PortNumber is invalid or PortFeature is invalid.
  @retval EFI_DEVICE_ERROR      Can't read register

**/
EFI_STATUS
EFIAPI
ClearRootHubPortFeature (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  PortNumber,
  IN     EFI_USB_PORT_FEATURE                   PortFeature
  )
{
  EFI_STATUS                                            Status;
  EFI_USB3_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE       ClearRootHubPortFeatureFunc = This->ClearRootHubPortFeature;
  
  if (mInSmram) {
    Status = mUsbCore->AddressConvert (
                         SMM_ADDRESS,
                         (VOID*)(UINTN)ClearRootHubPortFeatureFunc,
                         (VOID**)&ClearRootHubPortFeatureFunc
                         );
    if (Status == EFI_NOT_FOUND) return Status;
  }
  return ClearRootHubPortFeatureFunc (
           This,
           PortNumber,
           PortFeature
           );
}

/**

  Retrieves the capablility of root hub ports.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  MaxSpeed              A pointer to the number of the host controller.
  @param  PortNumber            A pointer to the number of the root hub ports.
  @param  Is64BitCapable        A pointer to the flag for whether controller supports
                                64-bit memory addressing.

  @retval EFI_SUCCESS           host controller capability were retrieved successfully.
  @retval EFI_INVALID_PARAMETER MaxSpeed or PortNumber or Is64BitCapable is NULL.
  @retval EFI_DEVICE_ERROR      An error was encountered while attempting to retrieve the capabilities.

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcGetCapability (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  OUT    UINT8                                  *MaxSpeed,
  OUT    UINT8                                  *PortNumber,
  OUT    UINT8                                  *Is64BitCapable
  )
{
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  return UsbHcDev->Usb2Hc->GetCapability (
                             UsbHcDev->Usb2Hc,
                             MaxSpeed,
                             PortNumber,
                             Is64BitCapable
                             );
}

/**

  Provides software reset for the USB host controller.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  Attributes            A bit mask of the reset operation to perform.
                                See below for a list of the supported bit mask values.

                                #define EFI_USB_HC_RESET_GLOBAL  0x0001
                                #define EFI_USB_HC_RESET_HOST_CONTROLLER  0x0002
                                #define EFI_USB_HC_RESET_GLOBAL_WITH_DEBUG  0x0004
                                #define EFI_USB_HC_RESET_HOST_WITH_DEBUG  0x0008

                                EFI_USB_HC_RESET_GLOBAL
                                      If this bit is set, a global reset signal will be sent to the USB bus.
                                      This resets all of the USB bus logic, including the USB host
                                      controller hardware and all the devices attached on the USB bus.
                                EFI_USB_HC_RESET_HOST_CONTROLLER
                                      If this bit is set, the USB host controller hardware will be reset.
                                      No reset signal will be sent to the USB bus.
                                EFI_USB_HC_RESET_GLOBAL_WITH_DEBUG
                                      If this bit is set, a global reset signal will be sent to the USB bus.
                                      This resets all of the USB bus logic, including the USB host
                                      controller hardware and all the devices attached on the USB bus.
                                      If this is an EHCI controller and the debug port has configured, then
                                      this is will still reset the host controller.
                                EFI_USB_HC_RESET_HOST_WITH_DEBUG
                                      If this bit is set, the USB host controller hardware will be reset.
                                      If this is an EHCI controller and the debug port has been configured,
                                      then this will still reset the host controller.

  @retval EFI_SUCCESS           The reset operation succeeded.
  @retval EFI_INVALID_PARAMETER Attributes is not valid.
  @retval EFI_UNSUPPOURTED      The type of reset specified by Attributes is not currently supported by
                                the host controller hardware.
  @retval EFI_ACCESS_DENIED     Reset operation is rejected due to the debug port being configured and
                                active; only EFI_USB_HC_RESET_GLOBAL_WITH_DEBUG or
                                EFI_USB_HC_RESET_HOST_WITH_DEBUG reset Atrributes can be used to
                                perform reset operation for this host controller.
  @retval EFI_DEVICE_ERROR      An error was encountered while attempting to perform
                                the reset operation.

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcReset (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT16                                 Attributes
  )
{
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  return UsbHcDev->Usb2Hc->Reset (
                             UsbHcDev->Usb2Hc,
                             Attributes
                             );
}

/**

  Retrieves current state of the USB host controller.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  State                 A pointer to the EFI_USB_HC_STATE data structure that
                                indicates current state of the USB host controller.
                                Type EFI_USB_HC_STATE is defined below.
                                typedef enum {
                                  EfiUsbHcStateHalt,
                                  EfiUsbHcStateOperational,
                                  EfiUsbHcStateSuspend,
                                  EfiUsbHcStateMaximum
                                } EFI_USB_HC_STATE;
                               
  @retval EFI_SUCCESS           The state information of the host controller was returned in State.
  @retval EFI_INVALID_PARAMETER State is NULL.
  @retval EFI_DEVICE_ERROR      An error was encountered while attempting to retrieve the
                                host controller's current state.
**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcGetState (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  OUT    EFI_USB_HC_STATE                       *State
  )
{
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  return UsbHcDev->Usb2Hc->GetState (
                             UsbHcDev->Usb2Hc,
                             State
                             );
}

/**

  Sets the USB host controller to a specific state.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  State                 Indicates the state of the host controller that will be set.

  @retval EFI_SUCCESS           The USB host controller was successfully placed in the state
                                specified by State.
  @retval EFI_INVALID_PARAMETER State is invalid.
  @retval EFI_DEVICE_ERROR      Failed to set the state specified by State due to device error.

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcSetState (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     EFI_USB_HC_STATE                       State
  )
{
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  return UsbHcDev->Usb2Hc->SetState (
                             UsbHcDev->Usb2Hc,
                             State
                             );
}

/**

  Submits control transfer to a target USB device.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  DeviceAddress         Represents the address of the target device on the USB,
                                which is assigned during USB enumeration.
  @param  DeviceSpeed           Indicates target device speed.
  @param  MaximumPacketLength   Indicates the maximum packet size that the
                                default control transfer endpoint is capable of
                                sending or receiving.
  @param  Request               A pointer to the USB device request that will be sent
                                to the USB device.
  @param  TransferDirection     Specifies the data direction for the transfer.
                                There are three values available, DataIn, DataOut
                                and NoData.
  @param  Data                  A pointer to the buffer of data that will be transmitted
                                to USB device or received from USB device.
  @param  DataLength            Indicates the size, in bytes, of the data buffer
                                specified by Data.
  @param  TimeOut               Indicates the maximum time, in milliseconds,
                                which the transfer is allowed to complete.
  @param  Translator            A pointr to the transaction translator data.
  @param  TransferResult        A pointer to the detailed result information generated
                                by this control transfer.

  @retval EFI_SUCCESS           The control transfer was completed successfully.
  @retval EFI_OUT_OF_RESOURCES  The control transfer could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER Some parameters are invalid.
  @retval EFI_TIMEOUT           The control transfer failed due to timeout.
  @retval EFI_DEVICE_ERROR      The control transfer failed due to host controller or device error.
                                Caller should check TranferResult for detailed error information.

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcControlTransfer (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN     EFI_USB_DEVICE_REQUEST                 *Request,
  IN     EFI_USB_DATA_DIRECTION                 TransferDirection,
  IN OUT VOID                                   *Data       OPTIONAL,
  IN OUT UINTN                                  *DataLength OPTIONAL,
  IN     UINTN                                  TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  OUT    UINT32                                 *TransferResult
  )
{
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  return UsbHcDev->Usb2Hc->ControlTransfer (
                             UsbHcDev->Usb2Hc,
                             DeviceAddress,
                             DeviceSpeed,
                             MaximumPacketLength,
                             Request,
                             TransferDirection,
                             Data,
                             DataLength,
                             TimeOut,
                             (EFI_USB2_HC_TRANSACTION_TRANSLATOR*) Translator,
                             TransferResult
                             );
}

/**

  Submits bulk transfer to a bulk endpoint of a USB device.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  DeviceAddress         Represents the address of the target device on the USB,
                                which is assigned during USB enumeration.
  @param  EndPointAddress       The combination of an endpoint number and an
                                endpoint direction of the target USB device.
                                Each endpoint address supports data transfer in
                                one direction except the control endpoint
                                (whose default endpoint address is 0).
                                It is the caller's responsibility to make sure that
                                the EndPointAddress represents a bulk endpoint.
  @param  DeviceSpeed           Indicates device speed. The supported values are EFI_USB_SPEED_FULL
                                and EFI_USB_SPEED_HIGH.
  @param  MaximumPacketLength   Indicates the maximum packet size the target endpoint
                                is capable of sending or receiving.
  @param  DataBuffersNumber     Number of data buffers prepared for the transfer.
  @param  Data                  Array of pointers to the buffers of data that will be transmitted
                                to USB device or received from USB device.
  @param  DataLength            When input, indicates the size, in bytes, of the data buffer
                                specified by Data. When output, indicates the actually
                                transferred data size.
  @param  DataToggle            A pointer to the data toggle value. On input, it indicates
                                the initial data toggle value the bulk transfer should adopt;
                                on output, it is updated to indicate the data toggle value
                                of the subsequent bulk transfer.
  @param  TimeOut               Indicates the maximum time, in milliseconds, which the
                                transfer is allowed to complete.
  @param  Translator            A pointr to the transaction translator data.
  @param  CallBackFunction      The Callback function.This function is called at the
                                rate specified by PollingInterval.This parameter is
                                only required when request asynchronous interrupt transfer.
  @param  Context               The context that is passed to the CallBackFunction.
                                This is an optional parameter and may be NULL.
  @param  PollingInterval       Indicates the interval, in milliseconds, that the
                                asynchronous interrupt transfer is polled.
                                This parameter is required request asynchronous interrupt
                                transfer.
  @param  TransferResult        A pointer to the detailed result information of the
                                bulk transfer.

  @retval EFI_SUCCESS           The bulk transfer was completed successfully.
  @retval EFI_OUT_OF_RESOURCES  The bulk transfer could not be submitted due to lack of resource.
  @retval EFI_INVALID_PARAMETER Some parameters are invalid.
  @retval EFI_TIMEOUT           The bulk transfer failed due to timeout.
  @retval EFI_DEVICE_ERROR      The bulk transfer failed due to host controller or device error.
                                Caller should check TranferResult for detailed error information.

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcBulkTransfer (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN     UINT8                                  DataBuffersNumber,
  IN OUT VOID                                   *Data[EFI_USB_MAX_BULK_BUFFER_NUM],
  IN OUT UINTN                                  *DataLength,
  IN OUT UINT8                                  *DataToggle,
  IN     UINTN                                  TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK        CallBackFunction OPTIONAL,
  IN     VOID                                   *Context         OPTIONAL,
  OUT    UINT32                                 *TransferResult
  )
{
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  return UsbHcDev->Usb2Hc->BulkTransfer (
                             UsbHcDev->Usb2Hc,
                             DeviceAddress,
                             EndPointAddress,
                             DeviceSpeed,
                             MaximumPacketLength,
                             DataBuffersNumber,
                             Data,
                             DataLength,
                             DataToggle,
                             TimeOut,
                             (EFI_USB2_HC_TRANSACTION_TRANSLATOR*) Translator,
                             TransferResult
                             );
}

/**

  Submits synchronous interrupt transfer to an interrupt endpoint
  of a USB device.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  DeviceAddress         Represents the address of the target device on the USB,
                                which is assigned during USB enumeration.
  @param  EndPointAddress       The combination of an endpoint number and an endpoint
                                direction of the target USB device. Each endpoint
                                address supports data transfer in one direction
                                except the control endpoint (whose default
                                endpoint address is 0). It is the caller's responsibility
                                to make sure that the EndPointAddress represents
                                an interrupt endpoint.
  @param  DeviceSpeed           Indicates device speed.
  @param  MaximumPacketLength   Indicates the maximum packet size the target endpoint
                                is capable of sending or receiving.
  @param  Data                  A pointer to the buffer of data that will be transmitted
                                to USB device or received from USB device.
  @param  DataLength            On input, the size, in bytes, of the data buffer specified
                                by Data. On output, the number of bytes transferred.
  @param  DataToggle            A pointer to the data toggle value. On input, it indicates
                                the initial data toggle value the synchronous interrupt
                                transfer should adopt;
                                on output, it is updated to indicate the data toggle value
                                of the subsequent synchronous interrupt transfer.
  @param  TimeOut               Indicates the maximum time, in milliseconds, which the
                                transfer is allowed to complete.
  @param  Translator            A pointr to the transaction translator data.
  @param  CallBackFunction      The Callback function.This function is called at the
                                rate specified by PollingInterval.This parameter is
                                only required when request asynchronous interrupt transfer.
  @param  Context               The context that is passed to the CallBackFunction.
                                his is an optional parameter and may be NULL.
  @param  PollingInterval       Indicates the interval, in milliseconds, that the
                                asynchronous interrupt transfer is polled.
                                This parameter is required request asynchronous interrupt
                                transfer.
  @param  TransferResult        A pointer to the detailed result information from
                                the synchronous interrupt transfer.

  @retval EFI_SUCCESS           The synchronous interrupt transfer was completed successfully.
  @retval EFI_OUT_OF_RESOURCES  The synchronous interrupt transfer could not be submitted due
                                to lack of resource.
  @retval EFI_INVALID_PARAMETE  Some parameters are invalid.
  @retval EFI_TIMEOUT           The synchronous interrupt transfer failed due to timeout.
  @retval EFI_DEVICE_ERROR      The synchronous interrupt transfer failed due to host controller
                                or device error. Caller should check TranferResult for detailed
                                error information.

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcInterruptTransfer (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN OUT VOID                                   *Data,
  IN OUT UINTN                                  *DataLength,
  IN OUT UINT8                                  *DataToggle,
  IN     UINTN                                  TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK        CallBackFunction OPTIONAL,
  IN     VOID                                   *Context         OPTIONAL,
  IN     UINTN                                  PollingInterval  OPTIONAL,
  OUT    UINT32                                 *TransferResult
  )
{
  EFI_STATUS                    Status;
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  if (CallBackFunction) {
    Status = UsbHcDev->Usb2Hc->AsyncInterruptTransfer (
                                 UsbHcDev->Usb2Hc,
                                 DeviceAddress,
                                 EndPointAddress,
                                 DeviceSpeed,
                                 MaximumPacketLength,
                                 TRUE,
                                 DataToggle,
                                 PollingInterval,
                                 *DataLength,
                                 (EFI_USB2_HC_TRANSACTION_TRANSLATOR*) Translator,
                                 CallBackFunction,
                                 Context
                                 );
  } else {
    Status = UsbHcDev->Usb2Hc->SyncInterruptTransfer (
                                 UsbHcDev->Usb2Hc,
                                 DeviceAddress,
                                 EndPointAddress,
                                 DeviceSpeed,
                                 MaximumPacketLength,
                                 Data,
                                 DataLength,
                                 DataToggle,
                                 TimeOut,
                                 (EFI_USB2_HC_TRANSACTION_TRANSLATOR*) Translator,
                                 TransferResult
                                 );
  }
  return Status;
}

/**

  Submits Async isochronous transfer to a target USB device.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  DeviceAddress         Represents the address of the target device on the USB,
                                which is assigned during USB enumeration.
  @param  EndPointAddress       End point address
  @param  DeviceSpeed           Indicates device speed.
  @param  MaximumPacketLength   Indicates the maximum packet size that the
                                default control transfer endpoint is capable of
                                sending or receiving.
  @param  DataBuffersNumber     Number of data buffers prepared for the transfer.
  @param  Data                  Array of pointers to the buffers of data that will be transmitted
                                to USB device or received from USB device.
  @param  Translator            A pointr to the transaction translator data.
  @param  IsochronousCallBack   When the transfer complete, the call back function will be called
  @param  Context               Pass to the call back function as parameter

  @retval EFI_UNSUPPORTED

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcIsochronousTransfer (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN     UINT8                                  DataBuffersNumber,
  IN OUT VOID                                   *Data[EFI_USB_MAX_ISO_BUFFER_NUM],
  IN     UINTN                                  *DataLength,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK        CallBackFunction OPTIONAL,
  IN     VOID                                   *Context         OPTIONAL,
  OUT    UINT32                                 *TransferResult
  )
{
  return EFI_UNSUPPORTED;
}

/**

  Retrieves the current status of a USB root hub port.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL.
  @param  PortNumber            Specifies the root hub port from which the status
                                is to be retrieved.  This value is zero-based. For example,
                                if a root hub has two ports, then the first port is numbered 0,
                                and the second port is numbered 1.
  @param  PortStatus            A pointer to the current port status bits and
                                port status change bits.

  @retval EFI_SUCCESS           The status of the USB root hub port specified
                                by PortNumber was returned in PortStatus.
  @retval EFI_INVALID_PARAMETER PortNumber is invalid.
  @retval EFI_DEVICE_ERROR      Can't read register

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcGetRootHubPortStatus (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  PortNumber,
  OUT    EFI_USB_PORT_STATUS                    *PortStatus
  )
{
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  return UsbHcDev->Usb2Hc->GetRootHubPortStatus (
                             UsbHcDev->Usb2Hc,
                             PortNumber,
                             PortStatus
                             );
}

/**

  Sets a feature for the specified root hub port.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL.
  @param  PortNumber            Specifies the root hub port whose feature
                                is requested to be set.
  @param  PortFeature           Indicates the feature selector associated
                                with the feature set request.

  @retval EFI_SUCCESS           The feature specified by PortFeature was set for the
                                USB root hub port specified by PortNumber.
  @retval EFI_INVALID_PARAMETER PortNumber is invalid or PortFeature is invalid.
  @retval EFI_DEVICE_ERROR      Can't read register

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcSetRootHubPortFeature (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  PortNumber,
  IN     EFI_USB_PORT_FEATURE                   PortFeature
  )
{
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  return UsbHcDev->Usb2Hc->SetRootHubPortFeature (
                             UsbHcDev->Usb2Hc,
                             PortNumber,
                             PortFeature
                             );
}

/**

  Clears a feature for the specified root hub port.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  PortNumber            Specifies the root hub port whose feature
                                is requested to be cleared.
  @param  PortFeature           Indicates the feature selector associated with the
                                feature clear request.

  @retval EFI_SUCCESS           The feature specified by PortFeature was cleared for the
                                USB root hub port specified by PortNumber.
  @retval EFI_INVALID_PARAMETER PortNumber is invalid or PortFeature is invalid.
  @retval EFI_DEVICE_ERROR      Can't read register

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcClearRootHubPortFeature (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  PortNumber,
  IN     EFI_USB_PORT_FEATURE                   PortFeature
  )
{
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  return UsbHcDev->Usb2Hc->ClearRootHubPortFeature (
                             UsbHcDev->Usb2Hc,
                             PortNumber,
                             PortFeature
                             );
}

/**

  Cancel the interrupt transfer allocated by AsyncInterruptTransfer.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  DeviceAddress         Represents the address of the target device on the USB,
                                which is assigned during USB enumeration.
  @param  EndPointAddress       End point address
  @param  PollingInterval       Indicates the interval, in milliseconds, that the
                                asynchronous interrupt transfer is polled.
                                This parameter is required request asynchronous interrupt
                                transfer.
  @param  DataToggle            A pointer to the data toggle value. On input, it indicates
                                the initial data toggle value the synchronous interrupt
                                transfer should adopt;
                                on output, it is updated to indicate the data toggle value
                                of the subsequent synchronous interrupt transfer.
  
  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcCancelIo (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINTN                                  PollingInterval,
  IN OUT UINT8                                  *DataToggle
  )
{
  USB_HOST_CONTROLLER_DEVICE     *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  return UsbHcDev->Usb2Hc->AsyncInterruptTransfer (
                             UsbHcDev->Usb2Hc,
                             DeviceAddress,
                             EndPointAddress,
                             0,
                             0,
                             FALSE,
                             DataToggle,
                             PollingInterval,
                             0,
                             NULL,
                             NULL,
                             NULL
                             );
}

/**

  Query the AsyncInterruptTransfer exist or not.

  @param  This                  A pointer to the EFI_USB3_HC_PROTOCOL instance.
  @param  DeviceAddress         Represents the address of the target device on the USB,
                                which is assigned during USB enumeration.
  @param  EndPointAddress       End point address
  @param  PollingInterval       Indicates the interval, in milliseconds, that the
                                asynchronous interrupt transfer is polled.
                                This parameter is required request asynchronous interrupt
                                transfer.

  @retval EFI_SUCCESS           Exist
  @retval EFI_NOT_FOUND         Not found

**/
STATIC
EFI_STATUS
EFIAPI
Usb3HcQueryIo (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINTN                                  PollingInterval
  )
{
  return EFI_UNSUPPORTED;
}

/**

  Non-Smm polling callback 

  @param  Event                 Event.
  @param  Context               Context.

  @retval EFI_SUCCESS

**/
STATIC
VOID
EFIAPI
Usb3HcTimerCallback (
  IN    UINTN        Event,
  IN    VOID         *Context
  )
{
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;
  //
  // Port connection changed detection
  //
  if (mInSmram) return;
  UsbHcDev = (USB_HOST_CONTROLLER_DEVICE*)Context;
  if (Event == LEGACY_FREE_HC_TIMER_EVENT && UsbHcDev->TimerCallback != NULL) {
    UsbHcDev->TimerCallback (
                0,
                UsbHcDev->TimerContext
                );
  }
}

STATIC
EFI_STATUS
EFIAPI
Usb3HcRegisterEvent (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINTN                                  EventType,
  IN     EFI_USB3_HC_CALLBACK                   Callback,
  IN     VOID                                   *Context,
  IN OUT VOID                                   **Handle
  )
{
  USB_HOST_CONTROLLER_DEVICE     *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  if ((EventType & HC_EVENT_CONNECT_CHANGE) && UsbHcDev->TimerEvent) {
    //
    // Create timer event for root port connection changed detection
    //
    UsbHcDev->TimerCallback = Callback;
    UsbHcDev->TimerContext  = Context;
    //
    // Always use DXE callback for Usb2Hc 
    //
    mUsbCore->AddressConvert (
                NON_SMM_ADDRESS,
                (VOID*)(UINTN)UsbHcDev->TimerCallback,
                (VOID**)&UsbHcDev->TimerCallback
                );
    if (Handle) *Handle = UsbHcDev->TimerEvent;
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
Usb3HcUnregisterEvent (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     VOID                                   *Handle
  )
{
  USB_HOST_CONTROLLER_DEVICE     *UsbHcDev;

  if (mInSmram) return EFI_UNSUPPORTED;
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (This);
  if (UsbHcDev->TimerEvent == Handle) {
    UsbHcDev->TimerCallback = NULL;
    UsbHcDev->TimerContext  = NULL;
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
Usb3HcLegacyHidSupport (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINTN                                  Command,
  IN     VOID                                   *Param
  )
{
  return EFI_UNSUPPORTED;
}

/**

  Install Usb3Hc for Usb2Hc only driver

  @param  Usb2Hc                EFI_USB2_HC_PROTOCOL

  @retval The installed EFI_USB3_HC_PROTOCOL

**/
EFI_USB3_HC_PROTOCOL*
InstallUsb3HcThunk (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo,
  IN  EFI_USB2_HC_PROTOCOL      *Usb2Hc
  )
{
  EFI_STATUS                            Status;
  USB_HOST_CONTROLLER_DEVICE            *UsbHcDev;
  
  Status = mUsbCore->AllocateBuffer (
                       sizeof (USB_HOST_CONTROLLER_DEVICE),
                       ALIGNMENT_32,
                       (VOID **)&UsbHcDev
                       );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  UsbHcDev->Signature                        = USB_HOST_CONTROLLER_DEVICE_SIGNATURE;
  UsbHcDev->UsbIo                            = UsbIo;
  UsbHcDev->Usb2Hc                           = Usb2Hc;
  UsbHcDev->Usb3Hc.GetCapability             = Usb3HcGetCapability;
  UsbHcDev->Usb3Hc.Reset                     = Usb3HcReset;
  UsbHcDev->Usb3Hc.GetState                  = Usb3HcGetState;
  UsbHcDev->Usb3Hc.SetState                  = Usb3HcSetState;
  UsbHcDev->Usb3Hc.ControlTransfer           = Usb3HcControlTransfer;
  UsbHcDev->Usb3Hc.BulkTransfer              = Usb3HcBulkTransfer;
  UsbHcDev->Usb3Hc.InterruptTransfer         = Usb3HcInterruptTransfer;
  UsbHcDev->Usb3Hc.IsochronousTransfer       = Usb3HcIsochronousTransfer;
  UsbHcDev->Usb3Hc.GetRootHubPortStatus      = Usb3HcGetRootHubPortStatus;
  UsbHcDev->Usb3Hc.SetRootHubPortFeature     = Usb3HcSetRootHubPortFeature;
  UsbHcDev->Usb3Hc.ClearRootHubPortFeature   = Usb3HcClearRootHubPortFeature;
  UsbHcDev->Usb3Hc.CancelIo                  = Usb3HcCancelIo;
  UsbHcDev->Usb3Hc.QueryIo                   = Usb3HcQueryIo;
  UsbHcDev->Usb3Hc.RegisterEvent             = Usb3HcRegisterEvent;
  UsbHcDev->Usb3Hc.UnregisterEvent           = Usb3HcUnregisterEvent;
  UsbHcDev->Usb3Hc.LegacyHidSupport          = Usb3HcLegacyHidSupport;
  UsbHcDev->Usb3Hc.MajorRevision             = 0x3;
  UsbHcDev->Usb3Hc.MinorRevision             = 0x0;
  mUsbCore->InsertAddressConvertTable (
              ACT_INSTANCE_BODY,
              UsbHcDev,
              sizeof(USB_HOST_CONTROLLER_DEVICE)
              );
  mUsbCore->InsertAddressConvertTable (
              ACT_FUNCTION_POINTER,
              &UsbHcDev->Usb3Hc,
              (sizeof(EFI_USB3_HC_PROTOCOL) - (sizeof (UINT16) * 2)) / sizeof(VOID*)
              );
  mUsbCore->RegisterLegacyFreeHcCallback (
              Usb3HcTimerCallback,
              0,
              UsbHcDev,
              &UsbHcDev->TimerEvent
              );
  return &UsbHcDev->Usb3Hc;
}

/**

  Uninstall Usb3Hc for Usb2Hc only driver

  @param  Usb2Hc                EFI_USB2_HC_PROTOCOL

  @retval The installed EFI_USB3_HC_PROTOCOL

**/
VOID
UninstallUsb3HcThunk (
  IN  EFI_USB3_HC_PROTOCOL      *Usb3Hc
  )
{
  USB_HOST_CONTROLLER_DEVICE    *UsbHcDev;
  
  UsbHcDev = USB_HOST_CONTROLLER_DEVICE_FROM_THIS (Usb3Hc);
  if (UsbHcDev->Signature == USB_HOST_CONTROLLER_DEVICE_SIGNATURE) {
    if (UsbHcDev->TimerEvent) {
      Usb3HcUnregisterEvent (
        &UsbHcDev->Usb3Hc,
        UsbHcDev->TimerEvent
        );
      mUsbCore->UnregisterLegacyFreeHcCallback (
                  UsbHcDev->TimerEvent
                  );
      UsbHcDev->TimerEvent = NULL;
    }
    mUsbCore->RemoveAddressConvertTable (
                ACT_INSTANCE_BODY,
                UsbHcDev
                );
    mUsbCore->RemoveAddressConvertTable (
                ACT_FUNCTION_POINTER,
                &UsbHcDev->Usb3Hc
                );
    mUsbCore->FreeBuffer (
                sizeof (USB_HOST_CONTROLLER_DEVICE),
                UsbHcDev
                );
  }
}