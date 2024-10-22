/** @file
  Definitions for non-volatile variable store garbage collection

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _VAR_RECLAIM_H_
#define _VAR_RECLAIM_H_

#include "Variable.h"

/**
  Writes a buffer to variable storage space, in the working block.

  This function writes a buffer to variable storage space into a firmware
  volume block device. The destination is specified by the parameter
  VariableBase. Fault Tolerant Write protocol is used for writing.

  @param[in] VariableBase   Base address of the variable to write.
  @param[in] Buffer         Point to the data buffer.

  @retval EFI_SUCCESS       The function completed successfully.
  @retval EFI_NOT_FOUND     Fail to locate Fault Tolerant Write protocol.
  @retval EFI_ABORTED       The function could not complete successfully.
**/
EFI_STATUS
FtwVariableSpace (
  IN EFI_PHYSICAL_ADDRESS   VariableBase,
  IN UINT8                  *Buffer
  );

/**
  Variable store garbage collection and reclaim operation.

  @param[in]  VariableBase        Base address of variable store
  @param[in]  LastVariableOffset  Offset of last variable
  @param[in]  IsVolatile          The variable store is volatile or not,
                                  if it is non-volatile, need FTW
  @param[in]  CurrentVariable     If it is not NULL, it means not to process
                                  current variable for Reclaim.

  @return EFI_SUCCESS             Reclaim variable store successfully.
  @return EFI_OUT_OF_RESOURCES    Allocate pool failed or total variable size is large than variable store size.
  @return Others                  Other error causes reclaim failed.
**/
EFI_STATUS
EFIAPI
Reclaim (
  IN       EFI_PHYSICAL_ADDRESS  VariableBase,
  OUT      UINTN                 *LastVariableOffset,
  IN       BOOLEAN               IsVolatile,
  IN OUT   VARIABLE_HEADER       **CurrentVariable OPTIONAL
  );

/**
  non-volatile variable store garbage collection and reclaim operation

  @param[in]  Variable                Pointer to new added variable.
  @param[in]  VarSize                 The size of new added variable size.
  @param[in]  VariableBase            Base address of variable store
  @param[out] LastVariableOffset      Offset of last variable

  @retval EFI_SUCCESS             Reclaim non-volatile variable store successfully.
  @retval EFI_INVALID_PARAMETER   Any input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate pool failed or total variable size is large than variable store size.
  @retval EFI_DEVICE_ERROR        Unable to reclaim non-volatile variable caused by flash device error.
--*/
EFI_STATUS
EFIAPI
ReclaimNonVolatileVariable (
  IN   VARIABLE_HEADER        *NewVariable,
  IN   UINTN                  NewVarSize,
  IN   EFI_PHYSICAL_ADDRESS   VariableBase,
  OUT  UINTN                  *LastVariableOffset
  );

/**
  Make sure the remaining non-volatile variable space is larger than
  VARIABLE_RECLAIM_THRESHOLD before boot to OS
**/
VOID
EFIAPI
ReclaimForOS (
  VOID
  );

/**
  Internal function to check the existence of VARIABLE_RECLAIM_THRESHOLD variable.

  @retval TRUE    VARIABLE_RECLAIM_THRESHOLD variable exists.
  @retval FALSE   VARIABLE_RECLAIM_THRESHOLD variable doesn't exist.
**/
BOOLEAN
DoesNeedDoReclaimVariableExist (
  VOID
  );

/**
  Internal function to check VARIABLE_RECLAIM_THRESHOLD or set NEED_DO_RECLAIM_NAME variable to make sure system will
  do reclaim in next boot.

  @retval EFI_SUCCESS    Nonvolatile variable free space is smaller than VARIABLE_RECLAIM_THRESHOLD or
                         set NEED_DO_RECLAIM_NAME variable successfully.
  @return Other          Any error occurred while writing NEED_DO_RECLAIM_NAME variable.
**/
EFI_STATUS
SetDoReclaimNextBoot (
  VOID
  );

/**
  Convert all of preserved variable table relative pointers to virtual address.
**/
VOID
PreservedTableAddressChange (
  VOID
  );

/**
  Gets LBA of block and offset by given address.

  This function gets the Logical Block Address (LBA) of a firmware
  volume block containing the given address, and the offset of the
  address on the block.

  @param[in]  Address        Address which should be contained
                         by returned FVB handle.
  @param[out] Lba            Pointer to LBA for output.
  @param[out] Offset         Pointer to offset for output.

  @retval EFI_SUCCESS    LBA and offset successfully returned.
  @retval EFI_NOT_FOUND  Fail to find FVB handle by address.
  @retval EFI_ABORTED    Fail to find valid LBA and offset.
**/
EFI_STATUS
GetVariableStoreInfo (
  IN  EFI_PHYSICAL_ADDRESS   VariableBase,
  OUT EFI_PHYSICAL_ADDRESS   *FvbBaseAddress,
  OUT EFI_LBA                *Lba,
  OUT UINTN                  *Offset
  );

#endif
