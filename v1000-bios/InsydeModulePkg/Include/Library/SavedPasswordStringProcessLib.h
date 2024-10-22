/** @file

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
#include <Uefi.h>

#ifndef _SAVED_PASSWORD_STRING_PROCESS_LIB_H_
#define _SAVED_PASSWORD_STRING_PROCESS_LIB_H_

#define HDD_PASSWORD_IV_SIZE                             SHA256_DIGEST_SIZE
#define HDD_PASSWORD_NONCE_SIZE                          SHA256_DIGEST_SIZE
#define HDD_PASSWORD_SECURITY_KEY_SIZE                   32
#define HDD_PASSWORD_ENCRYPT_SIZE                        (HDD_PASSWORD_MAX_NUMBER * sizeof (CHAR16))
#define PasswordIterationCount                           10000
#define BITS_PER_BYTE                                    8

typedef enum {
  EncryptProcess,
  DecryptProcess
} PROCESS_TYPE;

/**
  Returns the next entry of the HDD password table.

  @param[in]        HddPasswordTable         Pointer to HddPasswordTable.

  @retval           The pointer to the next table.

**/
HDD_PASSWORD_TABLE *
GetNextTableEntry (
  IN  HDD_PASSWORD_TABLE                *HddPasswordTablePtr
  );

/**
  Calculate the number of Hdd password tables.

  @param[in]        HddPasswordTable         Pointer to HddPasswordTable.
  @param[in]        HddPasswordTableSize     HddPasswordTable size.

  @retval The number of tables.

**/
UINTN
NumOfHddPasswordTable (
  IN  HDD_PASSWORD_TABLE                  *HddPasswordTable,
  IN  UINTN                               HddPasswordTableSize
  );

/**
  Saved Password String Process.

  @param  ProcessType                       To do encrypt or decrypt.
  @param  HddPasswordTable                  Pointer to HddPasswordTable.
  @param  HddPasswordTableSize              Size of HddPasswordTable.

  @retval EFI_SUCCESS                       Function has completed successfully.
  @retval Others                            An unexpected error occurred.

**/
EFI_STATUS
SavedPasswordStringProcess (
  IN  UINT8                                 ProcessType,
  IN  HDD_PASSWORD_TABLE                    *HddPasswordTable,
  IN  UINTN                                 HddPasswordTableSize
  );

#endif
