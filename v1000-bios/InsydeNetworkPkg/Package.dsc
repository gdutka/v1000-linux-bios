## @file
# Network Package Description file
#
#******************************************************************************
#* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************
#

[Defines]

[PcdsFeatureFlag]
  gInsydeTokenSpaceGuid.PcdH2ONetworkHttpSupported|FALSE
  gInsydeTokenSpaceGuid.PcdH2ONetworkTlsSupported|FALSE

[PcdsFixedAtBuild]
  gEfiNetworkPkgTokenSpaceGuid.PcdAllowHttpConnections|TRUE

[LibraryClasses.IA32, LibraryClasses.X64]
!if gInsydeTokenSpaceGuid.PcdH2ONetworkTlsSupported
  OpensslLib|InsydeNetworkPkg/Library/OpensslLib/OpensslLib.inf
  TlsLib|InsydeNetworkPkg/Library/TlsLib/TlsLib.inf
!endif

[LibraryClasses]
  InsydeNetworkLib|InsydeNetworkPkg/Library/InsydeNetworkLib/InsydeNetworkLib.inf
  HttpLib|InsydeNetworkPkg/Library/DxeHttpLib/DxeHttpLib.inf
  IpIoLib|InsydeNetworkPkg/Library/DxeIpIoLib/DxeIpIoLib.inf
  UdpIoLib|InsydeNetworkPkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  TcpIoLib|InsydeNetworkPkg/Library/DxeTcpIoLib/DxeTcpIoLib.inf
  DpcLib|InsydeNetworkPkg/Library/DxeDpcLib/DxeDpcLib.inf
  NetLib|InsydeNetworkPkg/Library/DxeNetLib/DxeNetLib.inf

[PcdsDynamicExDefault]
  #
  # PXE procedure cancel hot key definition
  # Key struct definition {KeyCode, ScanCode, [Next hot key], 0x00, 0x00[END]}
  #
  gInsydeTokenSpaceGuid.PcdPxeCancelHotKeyDefinition|{ \
    0x00, 0x17, \ # ESC
    0x00, 0x00  \ # List end
    }
  gInsydeTokenSpaceGuid.PcdPxeCancelHotKeyString|"[ESC]"

  #
  # This Feature will display DEVICE information when PXE start.
  # Default is disable.
  # It can be enabled by gH2oPxeDeviceInfoProtocolGuid, you can refer it in document "InsydeH2O Network Technical Reference".
  #
  gInsydeTokenSpaceGuid.PcdH2oPxeDisplayDeviceInformation|FALSE
  
  #
  # Define how much retry times to detect network media.
  # Set to 0, it will detect network media once. 
  #
  gInsydeTokenSpaceGuid.PcdH2OPxeCheckMediaRetryCount|2

[Components.$(DXE_ARCH)]
!if gInsydeTokenSpaceGuid.PcdH2ONetworkSupported
  InsydeNetworkPkg/Drivers/NetworkLockerDxe/NetworkLockerDxe.inf
  InsydeNetworkPkg/Drivers/DhcpDummyDxe/DhcpDummyDxe.inf
  InsydeNetworkPkg/Drivers/PxeDummyDxe/PxeDummyDxe.inf
  InsydeNetworkPkg/Drivers/DpcDxe/DpcDxe.inf
  InsydeNetworkPkg/Drivers/MnpDxe/MnpDxe.inf
  InsydeNetworkPkg/Drivers/ArpDxe/ArpDxe.inf
  InsydeNetworkPkg/Drivers/SnpDxe/SnpDxe.inf
  InsydeNetworkPkg/Drivers/Ip4Dxe/Ip4Dxe.inf
  InsydeNetworkPkg/Drivers/Udp4Dxe/Udp4Dxe.inf
  InsydeNetworkPkg/Drivers/Dhcp4Dxe/Dhcp4Dxe.inf
  InsydeNetworkPkg/Drivers/Mtftp4Dxe/Mtftp4Dxe.inf
!if gInsydeTokenSpaceGuid.PcdH2ONetworkIpv6Supported
  InsydeNetworkPkg/Drivers/Ip6Dxe/Ip6Dxe.inf
  InsydeNetworkPkg/Drivers/Udp6Dxe/Udp6Dxe.inf
  InsydeNetworkPkg/Drivers/Dhcp6Dxe/Dhcp6Dxe.inf
  InsydeNetworkPkg/Drivers/Mtftp6Dxe/Mtftp6Dxe.inf
!endif
  InsydeNetworkPkg/Drivers/UefiPxeBcDxe/UefiPxeBcDxe.inf
!if gInsydeTokenSpaceGuid.PcdH2ONetworkIscsiSupported OR gInsydeTokenSpaceGuid.PcdH2ONetworkHttpSupported
  InsydeNetworkPkg/Drivers/TcpDxe/TcpDxe.inf
!if gInsydeTokenSpaceGuid.PcdH2ONetworkIscsiSupported
  InsydeNetworkPkg/Drivers/IScsiDxe/IScsiDxe.inf
!endif
!if gInsydeTokenSpaceGuid.PcdH2ONetworkHttpSupported
  InsydeNetworkPkg/Drivers/HttpDxe/HttpDxe.inf
  InsydeNetworkPkg/Drivers/HttpUtilitiesDxe/HttpUtilitiesDxe.inf
  InsydeNetworkPkg/Drivers/HttpBootDxe/HttpBootDxe.inf {
    ### This Library override will avoid UefiBootManagerLib to link unnecessary Libraries.
    <LibraryClasses>
      UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  }
  InsydeNetworkPkg/Drivers/DnsDxe/DnsDxe.inf
  InsydeNetworkPkg/Drivers/RestDxe/RestDxe.inf 
!if gInsydeTokenSpaceGuid.PcdH2ONetworkTlsSupported
  InsydeNetworkPkg/Drivers/TlsDxe/TlsDxe.inf
!endif

  MdeModulePkg/Universal/Disk/RamDiskDxe/RamDiskDxe.inf

!endif
!endif
!endif
