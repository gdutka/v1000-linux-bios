/** @file

Header file for all SegFeature modules.

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _H2O_SEG_FEATURE_INFO_PROTOCOL_H_
#define _H2O_SEG_FEATURE_INFO_PROTOCOL_H_

#define H2O_SEG_FEATURE_INFO_PROTOCOL_VERSION      0x100

#define H2O_SEG_FEATURE_INFO_PROTOCOL_GUID \
  { 0x996E5CAE, 0xF918, 0x4314, { 0x97, 0xC5, 0xBD, 0x7D, 0x0C, 0x1B, 0x8B, 0xF6 } }

typedef struct _H2O_SEG_FEATURE_INFO_PROTOCOL H2O_SEG_FEATURE_INFO_PROTOCOL;

#pragma pack(1)

typedef enum {
  RedfishFirst  = 0,
  IpmiFirst,
  BootOptionPriorityEnd = 0xF
} H2O_BOOT_OPTION_PRIORITY;

//
// Default is 0, if feature has handled this protocol, turn on the corresponding bit.
//
typedef struct {
  UINT16      RefishHasHandled    :1;
  UINT16      IpmiHasHandled      :1;
  UINT16      Reserved            :14;
} H2O_SEG_FEATURE_ACKNOWLEDGE_DATA;

//
// Capability of RedFish.
//
typedef struct {
  UINT16      BootSourceOverride  :1;
  UINT16      Reserved            :15;
} H2O_SEG_FEATURE_REDFISH_CAPABILITY;

//
// Capability of IPMI.
//
typedef struct {
  UINT16      BootOptionSupport   :1;
  UINT16      Reserved            :15;
} H2O_SEG_FEATURE_IPMI_CAPABILITY;

typedef struct {
  H2O_SEG_FEATURE_IPMI_CAPABILITY        IpmiCapability;
  H2O_SEG_FEATURE_IPMI_CAPABILITY        IpmiSupportStatus;
} H2O_SEG_FEATURE_IPMI_INFO;

typedef struct {
  H2O_SEG_FEATURE_REDFISH_CAPABILITY     RedFishCapability;
  H2O_SEG_FEATURE_REDFISH_CAPABILITY     RedFishSupportStatus;
} H2O_SEG_FEATURE_REDFISH_INFO;

//
// This is the definition of the Protocol structure.
//
struct _H2O_SEG_FEATURE_INFO_PROTOCOL {
  UINT16                                 Version;
  H2O_SEG_FEATURE_ACKNOWLEDGE_DATA       FeatureAck;
  H2O_SEG_FEATURE_REDFISH_INFO           RedFishInfo;
  H2O_SEG_FEATURE_IPMI_INFO              IpmiInfo;
};

#pragma pack()

extern EFI_GUID gH2OSegFeatureInfoProtocolGuid;

#endif

