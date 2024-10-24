/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  Supporting functions declaration for PCI devices management.

Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_PCI_DEVICE_SUPPORT_H_
#define _EFI_PCI_DEVICE_SUPPORT_H_


#pragma pack (1)
typedef struct _BUS_MASTER_DISABLE_DATA {
  UINT8         DeviceNumber;
  UINT8         FunctionNumber;
} BUS_MASTER_DISABLE_DATA;
#pragma pack ()

/**
  Initialize the PCI devices pool.

**/
VOID
InitializePciDevicePool (
  VOID
  );

/**
  Insert a root bridge into PCI device pool.

  @param RootBridge     A pointer to the PCI_IO_DEVICE.

**/
VOID
InsertRootBridge (
  IN PCI_IO_DEVICE      *RootBridge
  );

/**
  This function is used to insert a PCI device node under
  a bridge.

  @param Bridge         The PCI bridge.
  @param PciDeviceNode  The PCI device needs inserting.

**/
VOID
InsertPciDevice (
  IN PCI_IO_DEVICE      *Bridge,
  IN PCI_IO_DEVICE      *PciDeviceNode
  );

/**
  Destroy root bridge and remove it from deivce tree.

  @param RootBridge     The bridge want to be removed.

**/
VOID
DestroyRootBridge (
  IN PCI_IO_DEVICE      *RootBridge
  );

/**
  Destroy all the pci device node under the bridge.
  Bridge itself is not included.

  @param Bridge         A pointer to the PCI_IO_DEVICE.

**/
VOID
DestroyPciDeviceTree (
  IN PCI_IO_DEVICE      *Bridge
  );

/**
  Destroy all device nodes under the root bridge
  specified by Controller.

  The root bridge itself is also included.

  @param  Controller    Root bridge handle.

  @retval EFI_SUCCESS   Destory all devcie nodes successfully.
  @retval EFI_NOT_FOUND Cannot find any PCI device under specified
                        root bridge.

**/
EFI_STATUS
DestroyRootBridgeByHandle (
  IN EFI_HANDLE        Controller
  );

/**
  This function registers the PCI IO device.

  It creates a handle for this PCI IO device (if the handle does not exist), attaches
  appropriate protocols onto the handle, does necessary initialization, and sets up
  parent/child relationship with its bus controller.

  @param Controller     An EFI handle for the PCI bus controller.
  @param PciIoDevice    A PCI_IO_DEVICE pointer to the PCI IO device to be registered.
  @param Handle         A pointer to hold the returned EFI handle for the PCI IO device.

  @retval EFI_SUCCESS   The PCI device is successfully registered.
  @retval other         An error occurred when registering the PCI device.

**/
EFI_STATUS
RegisterPciDevice (
  IN  EFI_HANDLE          Controller,
  IN  PCI_IO_DEVICE       *PciIoDevice,
  OUT EFI_HANDLE          *Handle      OPTIONAL
  );

/**
  This function is used to remove the whole PCI devices on the specified bridge from
  the root bridge.

  @param RootBridgeHandle   The root bridge device handle.
  @param Bridge             The bridge device to be removed.

**/
VOID
RemoveAllPciDeviceOnBridge (
  EFI_HANDLE               RootBridgeHandle,
  PCI_IO_DEVICE            *Bridge
  );

/**
  This function is used to de-register the PCI IO device.

  That includes un-installing PciIo protocol from the specified PCI
  device handle.

  @param Controller    An EFI handle for the PCI bus controller.
  @param Handle        PCI device handle.

  @retval EFI_SUCCESS  The PCI device is successfully de-registered.
  @retval other        An error occurred when de-registering the PCI device.

**/
EFI_STATUS
DeRegisterPciDevice (
  IN  EFI_HANDLE                     Controller,
  IN  EFI_HANDLE                     Handle
  );

/**
  Start to manage the PCI device on the specified root bridge or PCI-PCI Bridge.

  @param Controller          The root bridge handle.
  @param RootBridge          A pointer to the PCI_IO_DEVICE.
  @param RemainingDevicePath A pointer to the EFI_DEVICE_PATH_PROTOCOL.
  @param NumberOfChildren    Children number.
  @param ChildHandleBuffer   A pointer to the child handle buffer.

  @retval EFI_NOT_READY   Device is not allocated.
  @retval EFI_UNSUPPORTED Device only support PCI-PCI bridge.
  @retval EFI_NOT_FOUND   Can not find the specific device.
  @retval EFI_SUCCESS     Success to start Pci devices on bridge.

**/
EFI_STATUS
StartPciDevicesOnBridge (
  IN EFI_HANDLE                          Controller,
  IN PCI_IO_DEVICE                       *RootBridge,
  IN EFI_DEVICE_PATH_PROTOCOL            *RemainingDevicePath,
  IN OUT UINT8                           *NumberOfChildren,
  IN OUT EFI_HANDLE                      *ChildHandleBuffer
  );

/**
  Start to manage all the PCI devices it found previously under
  the entire host bridge.

  @param Controller          The root bridge handle.

  @retval EFI_NOT_READY   Device is not allocated.
  @retval EFI_SUCCESS     Success to start Pci device on host bridge.

**/
EFI_STATUS
StartPciDevices (
  IN EFI_HANDLE                         Controller
  );

/**
  Create root bridge device.

  @param RootBridgeHandle    Specified root bridge hanle.

  @return The crated root bridge device instance, NULL means no
          root bridge device instance created.

**/
PCI_IO_DEVICE *
CreateRootBridge (
  IN EFI_HANDLE                   RootBridgeHandle
  );

/**
  Get root bridge device instance by specific root bridge handle.

  @param RootBridgeHandle    Given root bridge handle.

  @return The root bridge device instance, NULL means no root bridge
          device instance found.

**/
PCI_IO_DEVICE *
GetRootBridgeByHandle (
  EFI_HANDLE RootBridgeHandle
  );


/**
  Judege whether Pci device existed.

  @param Bridge       Parent bridege instance.
  @param PciIoDevice  Device instance.

  @retval TRUE        Pci device existed.
  @retval FALSE       Pci device did not exist.

**/
BOOLEAN
PciDeviceExisted (
  IN PCI_IO_DEVICE    *Bridge,
  IN PCI_IO_DEVICE    *PciIoDevice
  );

/**
  Get the active VGA device on the specified Host Bridge.

  @param HostBridgeHandle    Host Bridge handle.

  @return The active VGA device on the specified Host Bridge.

**/
PCI_IO_DEVICE *
LocateVgaDeviceOnHostBridge (
  IN EFI_HANDLE           HostBridgeHandle
  );

/**
  Locate the active VGA device under the bridge.

  @param Bridge  PCI IO instance for the bridge.

  @return The active VGA device.

**/
PCI_IO_DEVICE *
LocateVgaDevice (
  IN PCI_IO_DEVICE        *Bridge
  );

/**
  Destroy a pci device node.

  All direct or indirect allocated resource for this node will be freed.

  @param PciIoDevice  A pointer to the PCI_IO_DEVICE to be destoried.

**/
VOID
FreePciDevice (
  IN PCI_IO_DEVICE    *PciIoDevice
  );

BOOLEAN
IsUnsafeBusMasterDev (
  IN PCI_IO_DEVICE  *PciIoDevice
  );

/**
  This function initializes H2OPciIoDevice information from PciIoDevice instance

  @param[out] H2OPciIoDevice     Pointer to H2O_PCI_IO_DEVICE instance.
  @param[in]  PciIoDevice        Pointer to PCI_IO_DEVICE instance.
 **/
VOID
SetH2OPciIoDeviceInfo (
  OUT H2O_PCI_IO_DEVICE   *H2OPciIoDevice,
  IN  PCI_IO_DEVICE       *PciIoDevice
  );

/**
  This function restores the information from H2OPciIoDevice to PciIoDevice.

  @param[out]  PciIoDevice        Pointer to PCI_IO_DEVICE instance.
  @param[in] H2OPciIoDevice     Pointer to H2O_PCI_IO_DEVICE instance.
 **/
VOID
SetPciIoDeviceInfo (
  OUT  PCI_IO_DEVICE       *PciIoDevice,
  IN   H2O_PCI_IO_DEVICE   *H2OPciIoDevice
  );

#endif
