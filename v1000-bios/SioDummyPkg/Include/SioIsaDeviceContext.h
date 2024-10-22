/** @file
  H2O ISA Device Protocol

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

#ifndef _H2O_ISA_DEVICE_CONTEXT_H_
#define _H2O_ISA_DEVICE_CONTEXT_H_

#include <Protocol/H2OIsaDevice.h>

#define H2O_ISA_DEVICE_CONTEXT_SIGNATURE  SIGNATURE_32 ('I', 's', 'a', 'D')

typedef struct {
  UINT32                    Signature;
  EFI_HANDLE                Handle;
  H2O_ISA_DEVICE_PROTOCOL   DeviceProtocol;
  H2O_ISA_DEVICE_INFO       DeviceInfo;
  H2O_ISA_DEVICE_RESOURCE   DeviceResource;
} H2O_ISA_DEVICE_CONTEXT;

#define H2O_ISA_DEVICE_CONTEXT_FROM_PROTOCOL(a) \
  CR (a, \
      H2O_ISA_DEVICE_CONTEXT, \
      DeviceProtocol, \
      H2O_ISA_DEVICE_CONTEXT_SIGNATURE \
      )

#endif