/** @file
    Header file of the H2O SMM variable lock policy protocol
;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Module Name:

  H2OSmmVariableLockPolicy.h

Abstract:

  This protocol is used to control the access policy of locked variables in SMM.

--*/

#ifndef _H2O_SMM_VARIABLE_LOCK_POLICY_PROTOCOL_H_
#define _H2O_SMM_VARIABLE_LOCK_POLICY_PROTOCOL_H_

#define H2O_SMM_VARIABLE_LOCK_POLICY_PROTOCOL_GUID  \
  {0x94be8611, 0x6425, 0x4ab4, 0x8d, 0xe5, 0xda, 0xd2, 0xfe, 0xbb, 0xb8, 0xf9}


typedef struct _H2O_SMM_VARIABLE_LOCK_POLICY_PROTOCOL H2O_SMM_VARIABLE_LOCK_POLICY_PROTOCOL;

/**
  Function to set variable lock check policy in SMM.

  @param[in]  EnableLock           Enable or disable variable lock check in SMM.

  @retval     EFI_SUCCESS           Change variable lock check policy in SMM successfully.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_ENABLE_VARIABLE_LOCK) (
  IN  BOOLEAN    EnableLock
  );

struct _H2O_SMM_VARIABLE_LOCK_POLICY_PROTOCOL {
  H2O_ENABLE_VARIABLE_LOCK              EnableVariableLock;
};

extern EFI_GUID gH2OSmmVariableLockPolicyProtocolGuid;


#endif
