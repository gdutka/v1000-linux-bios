/** @file

  Definitions for CPU S3 data.

;******************************************************************************
;* Copyright (c) 2013 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _ACPI_CPU_DATA_H_
#define _ACPI_CPU_DATA_H_

typedef struct {
  BOOLEAN               APState;
  EFI_PHYSICAL_ADDRESS  StartupVector;
  EFI_PHYSICAL_ADDRESS  GdtrProfile;
  EFI_PHYSICAL_ADDRESS  IdtrProfile;
  EFI_PHYSICAL_ADDRESS  StackAddress;
  UINT32                StackSize;
  UINT32                NumberOfCpus;
  EFI_PHYSICAL_ADDRESS  MtrrTable;
  EFI_PHYSICAL_ADDRESS  PreSmmInitRegisterTable;
  EFI_PHYSICAL_ADDRESS  RegisterTable;
  EFI_PHYSICAL_ADDRESS  ApMachineCheckHandlerBase;
  UINT32                ApMachineCheckHandlerSize;
} ACPI_CPU_DATA;

//
// Register types in register table
//
typedef enum _REGISTER_TYPE {
  Msr,
  ControlRegister,
  MemoryMapped,
  CacheControl
} REGISTER_TYPE;

//
// Element of register table entry
//
typedef struct {
  REGISTER_TYPE RegisterType;
  UINT32        Index;
  UINT8         ValidBitStart;
  UINT8         ValidBitLength;
  UINT64        Value;
} CPU_REGISTER_TABLE_ENTRY;

//
// Register table definition, including current table length,
// allocated size of this table, and pointer to the list of table entries.
//
typedef struct {
  UINT32                   TableLength;
  UINT32                   NumberBeforeReset;
  UINT32                   AllocatedSize;
  UINT32                   InitialApicId;
  CPU_REGISTER_TABLE_ENTRY *RegisterTableEntry;
} CPU_REGISTER_TABLE;

#endif

