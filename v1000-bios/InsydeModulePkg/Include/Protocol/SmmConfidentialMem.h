/** @file
  SMM confidential memory interface

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*//** @file
  SMM confidential memory interface

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2019 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains 'Framework Code' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may not be
  modified, except as allowed by additional terms of your license agreement.

**/

#ifndef __SMM_CONFIDENTIAL_MEM_H__
#define __SMM_CONFIDENTIAL_MEM_H__

#include <PiSmm.h>

#define EDKII_SMM_CONF_MEM_PROTOCOL_GUID \
  { \
    0xeeb82377, 0x5fda, 0x4453, { 0xaa, 0x10, 0x8c, 0x03, 0x1c, 0xa8, 0xef, 0xea } \
  };

typedef struct _EDKII_SMM_CONF_MEM_PROTOCOL EDKII_SMM_CONF_MEM_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI * EDKII_SMM_ALLOCATE_CONFIDENTIAL_POOL) (
  IN      UINTN   Size,
  IN OUT  VOID    **Buffer
    );

typedef
EFI_STATUS
(EFIAPI * EDKII_SMM_FREE_CONFIDENTIAL_POOL) (
  IN OUT  VOID   **Buffer
    );

struct _EDKII_SMM_CONF_MEM_PROTOCOL {
  EDKII_SMM_ALLOCATE_CONFIDENTIAL_POOL SmmAllocateConfPool;
  EDKII_SMM_FREE_CONFIDENTIAL_POOL     SmmFreeConfPool;
};

extern EFI_GUID gEdkiiSmmConfMemProtocolGuid;

#endif
