/** @file

Header file for all SegFeature modules.

;******************************************************************************
;* Copyright (c) 2013 - 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _H2O_IPMI_FEATURE_INFO_PROTOCOL_H_
#define _H2O_IPMI_FEATURE_INFO_PROTOCOL_H_

// {4863555C-E36F-4c93-BC09-C704D0F55074}
//-static const GUID <<name>> =
//-{ 0x4863555c, 0xe36f, 0x4c93, { 0xbc, 0x9, 0xc7, 0x4, 0xd0, 0xf5, 0x50, 0x74 } };

#define H2O_IPMI_FEATURE_INFO_PROTOCOL_VERSION      0x100

#define H2O_IPMI_FEATURE_INFO_PROTOCOL_GUID \
  { 0x4863555c, 0xe36f, 0x4c93, { 0xbc, 0x9, 0xc7, 0x4, 0xd0, 0xf5, 0x50, 0x74 } }

typedef struct _H2O_IPMI_FEATURE_INFO_PROTOCOL H2O_IPMI_FEATURE_INFO_PROTOCOL;

typedef union _H2O_IPMI_CAPABILITY_FLAGS   H2O_IPMI_CAPABILITY_FLAGS;
union _H2O_IPMI_CAPABILITY_FLAGS {
    UINT32    Raw;
    struct {
      UINT32    BootSourceOverride : 1;
      UINT32    Reserved           : 31;
    } Flags;
};

//
// This is the definition of the HOB structure.
//
struct _H2O_IPMI_FEATURE_INFO_PROTOCOL {
  UINTN                           Version;
  H2O_IPMI_CAPABILITY_FLAGS       Capabilities;
  H2O_IPMI_CAPABILITY_FLAGS       ActionStatus;
};

extern EFI_GUID gH2OIpmiFeatureInfoProtocolGuid;

#endif  //_H2O_IPMI_FEATURE_INFO_PROTOCOL_H_

