/** @file

  Entry point of CPU DXE driver.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Cpu.h"
#include <Library/UefiCpuLib.h>

//
// CPU System Memory Map Definition
//
#define CPU_MSI_MEMORY_BASE                   0xFEE00000
#define CPU_MSI_MEMORY_SIZE                   0x100000

BOOLEAN                     mIsFlushingGCD;
EFI_HANDLE                  mHandle = NULL;
UINT64                      mTscTimerFrequency;

//
// Vector Table for user's interrupt handlers
//
EFI_CPU_INTERRUPT_HANDLER   mExternalVectorTable[INTERRUPT_VECTOR_NUMBER];

//
// The Cpu Architectural Protocol that this Driver produces
//
EFI_CPU_ARCH_PROTOCOL       mCpuArchProtocol = {
  FlushCpuDataCache,
  EnableInterrupt,
  DisableInterrupt,
  GetInterruptStateInstance,
  Init,
  RegisterInterruptHandler,
  GetTimerValue,
  SetMemoryAttributes,
  1,
  4,
};

/**
  This function flushes CPU data cache.

  This function implements FlushDataCache() service of CPU Architecture Protocol.
  It flushes a range of the processor's data cache.

  @param  This              The EFI_CPU_ARCH_PROTOCOL instance.
  @param  Start             Physical address to start flushing from.
  @param  Length            Number of bytes to flush. Round up to chipset granularity.
  @param  FlushType         Specifies the type of flush operation to perform.

  @retval EFI_SUCCESS       Cache is flushed.
  @retval EFI_UNSUPPORTED   Flush type is not supported.
  @retval EFI_DEVICE_ERROR  Requested range could not be flushed..

**/
EFI_STATUS
EFIAPI
FlushCpuDataCache (
  IN EFI_CPU_ARCH_PROTOCOL     *This,
  IN EFI_PHYSICAL_ADDRESS      Start,
  IN UINT64                    Length,
  IN EFI_CPU_FLUSH_TYPE        FlushType
  )
{
  if (FlushType == EfiCpuFlushTypeWriteBackInvalidate) {
    AsmWbinvd ();
    return EFI_SUCCESS;
  } else if (FlushType == EfiCpuFlushTypeInvalidate) {
    AsmInvd ();
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}

/**
  This function enables interrupt processing by the processor.

  This function implements EnableInterrupt() service of CPU Architecture Protocol.
  This function enables interrupt processing by the processor. This function is
  used to implement the Boot Services RaiseTPL() and RestoreTPL().

  @param  This              The EFI_CPU_ARCH_PROTOCOL instance.

  @retval EFI_SUCCESS       Interrupts are enabled on the processor.
  @retval EFI_DEVICE_ERROR  Interrupts could not be enabled on the processor.

**/
EFI_STATUS
EFIAPI
EnableInterrupt (
  IN EFI_CPU_ARCH_PROTOCOL          *This
  )
{
  EnableInterrupts ();
  return EFI_SUCCESS;
}

/**
  This function disables interrupt processing by the processor.

  This function implements DisableInterrupt() service of CPU Architecture Protocol.
  This function disables interrupt processing by the processor. This function is
  used to implement the Boot Services RaiseTPL() and RestoreTPL().

  @param  This              The EFI_CPU_ARCH_PROTOCOL instance.

  @retval EFI_SUCCESS       Interrupts are disabled on the processor.
  @retval EFI_DEVICE_ERROR  Interrupts could not be disabled on the processor.

**/
EFI_STATUS
EFIAPI
DisableInterrupt (
  IN EFI_CPU_ARCH_PROTOCOL     *This
  )
{
  DisableInterrupts ();
  return EFI_SUCCESS;
}

/**
  This function retrieves the processor's current interrupt state.

  This function implements GetInterruptState() service of CPU Architecture Protocol.
  This function retrieves the processor's current interrupt state.

  @param  This                   The EFI_CPU_ARCH_PROTOCOL instance.
  @param  State                  A pointer to the processor's current interrupt state.
                                 Set to TRUE if interrupts are enabled and FALSE if interrupts are disabled.

  @retval EFI_SUCCESS            The processor's current interrupt state was returned in State.
  @retval EFI_INVALID_PARAMETER  State is NULL.

**/
EFI_STATUS
EFIAPI
GetInterruptStateInstance (
  IN  EFI_CPU_ARCH_PROTOCOL     *This,
  OUT BOOLEAN                   *State
  )
{
  if (State == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *State = GetInterruptState ();
  return EFI_SUCCESS;
}

/**
  This function generates an INIT on the processor.

  This function implements Init() service of CPU Architecture Protocol.
  This function generates an INIT on the processor.

  @param  This                   The EFI_CPU_ARCH_PROTOCOL instance.
  @param  InitType               The type of processor INIT to perform.

  @retval EFI_SUCCESS            The processor INIT was performed. This return code should never be seen.
  @retval EFI_UNSUPPORTED        The processor INIT operation specified by InitType is not supported by this processor.
  @retval EFI_DEVICE_ERROR       The processor INIT failed.

**/
EFI_STATUS
EFIAPI
Init (
  IN EFI_CPU_ARCH_PROTOCOL      *This,
  IN EFI_CPU_INIT_TYPE          InitType
  )
{
  return EFI_UNSUPPORTED;
}

/**
  This function Registers a function to be called from the processor interrupt handler.

  This function implements RegisterInterruptHandler() service of CPU Architecture Protocol.
  This function Registers a function to be called from the processor interrupt handler.

  @param  This                   The EFI_CPU_ARCH_PROTOCOL instance.
  @param  InterruptType          Defines which interrupt or exception to hook.
  @param  InterruptHandler       A pointer to a function of type EFI_CPU_INTERRUPT_HANDLER
                                 that is called when a processor interrupt occurs.
                                 If this parameter is NULL, then the handler will be uninstalled.

  @retval EFI_SUCCESS            The handler for the processor interrupt was successfully installed or uninstalled.
  @retval EFI_ALREADY_STARTED    InterruptHandler is not NULL, and a handler for InterruptType was previously installed.
  @retval EFI_INVALID_PARAMETER  InterruptHandler is NULL, and a handler for InterruptType was not previously installed.
  @retval EFI_UNSUPPORTED        The interrupt specified by InterruptType is not supported.

**/
EFI_STATUS
EFIAPI
RegisterInterruptHandler (
  IN EFI_CPU_ARCH_PROTOCOL         *This,
  IN EFI_EXCEPTION_TYPE            InterruptType,
  IN EFI_CPU_INTERRUPT_HANDLER     InterruptHandler
  )
{
  if (InterruptType < 0 || InterruptType > 0xff) {
    return EFI_UNSUPPORTED;
  }

  if (InterruptHandler == NULL && mExternalVectorTable[InterruptType] == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (InterruptHandler != NULL && mExternalVectorTable[InterruptType] != NULL) {
    return EFI_ALREADY_STARTED;
  }

  if (InterruptHandler != NULL) {
    SetInterruptDescriptorTableHandlerAddress ((UINTN)InterruptType);
  } else {
    //
    // Restore the original IDT handler address if InterruptHandler is NULL.
    //
    RestoreInterruptDescriptorTableHandlerAddress ((UINTN)InterruptType);
  }

  mExternalVectorTable[InterruptType] = InterruptHandler;
  return EFI_SUCCESS;
}

/**
  This function returns a timer value from one of the processor's internal timers.

  This function implements GetTimerValue() service of CPU Architecture Protocol.
  This function returns a timer value from one of the processor's internal timers.

  @param  This                   The EFI_CPU_ARCH_PROTOCOL instance.
  @param  TimerIndex             Specifies which processor timer is to be returned in TimerValue.
                                 This parameter must be between 0 and EFI_CPU_ARCH_PROTOCOL.NumberOfTimers-1.
  @param  TimerValue             Pointer to the returned timer value.
  @param  TimerPeriod            A pointer to the amount of time that passes in femtoseconds for each
                                 increment of TimerValue. If TimerValue does not increment at a predictable
                                 rate, then 0 is returned.

  @retval EFI_SUCCESS            The processor timer value specified by TimerIndex was returned in TimerValue.
  @retval EFI_INVALID_PARAMETER  TimerValue is NULL.
  @retval EFI_INVALID_PARAMETER  TimerIndex is not valid.
  @retval EFI_UNSUPPORTEDT       The processor does not have any readable timers.
  @retval EFI_DEVICE_ERROR       An error occurred attempting to read one of the processor's timers.

**/
EFI_STATUS
EFIAPI
GetTimerValue (
  IN  EFI_CPU_ARCH_PROTOCOL     *This,
  IN  UINT32                    TimerIndex,
  OUT UINT64                    *TimerValue,
  OUT UINT64                    *TimerPeriod OPTIONAL
  )
{
  if (TimerValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  if (TimerIndex != 0) {
    return EFI_INVALID_PARAMETER;
  }
  
  *TimerValue = AsmReadTsc ();
  
  if (TimerPeriod != NULL) {
    *TimerPeriod = DivU64x32 (1000000000000000, (UINT32) mTscTimerFrequency);
  }
  
  return EFI_SUCCESS;
}

/**
  Implementation of SetMemoryAttributes() service of CPU Architecture Protocol.

  This function modifies the attributes for the memory region specified by BaseAddress and
  Length from their current attributes to the attributes specified by Attributes.

  @param  This             The EFI_CPU_ARCH_PROTOCOL instance.
  @param  BaseAddress      The physical address that is the start address of a memory region.
  @param  Length           The size in bytes of the memory region.
  @param  Attributes       The bit mask of attributes to set for the memory region.

  @retval EFI_SUCCESS           The attributes were set for the memory region.
  @retval EFI_ACCESS_DENIED     The attributes for the memory resource range specified by
                                BaseAddress and Length cannot be modified.
  @retval EFI_INVALID_PARAMETER Length is zero.
                                Attributes specified an illegal combination of attributes that
                                cannot be set together.
  @retval EFI_OUT_OF_RESOURCES  There are not enough system resources to modify the attributes of
                                the memory resource range.
  @retval EFI_UNSUPPORTED       The processor does not support one or more bytes of the memory
                                resource range specified by BaseAddress and Length.
                                The bit mask of attributes is not support for the memory resource
                                range specified by BaseAddress and Length.

**/
EFI_STATUS
EFIAPI
SetMemoryAttributes (
  IN EFI_CPU_ARCH_PROTOCOL     *This,
  IN EFI_PHYSICAL_ADDRESS      BaseAddress,
  IN UINT64                    Length,
  IN UINT64                    Attributes
  )
{
  RETURN_STATUS             Status;
  EFI_STATUS                MpStatus;
  MTRR_MEMORY_CACHE_TYPE    CacheType;
  EFI_MP_SERVICES_PROTOCOL  *MpService;

  //
  // If this function is called because GCD SetMemorySpaceAttributes () is called
  // by RefreshGcdMemoryAttributes (), then we are just synchronzing GCD memory 
  // map with MTRR values. So there is no need to modify MTRRs, just return immediately
  // to avoid unnecessary computing.
  //
  if (mIsFlushingGCD) {
    return EFI_SUCCESS;
  }

  switch (Attributes) {
  case EFI_MEMORY_UC:
    CacheType = CacheUncacheable;
    break;

  case EFI_MEMORY_WC:
    CacheType = CacheWriteCombining;
    break;

  case EFI_MEMORY_WT:
    CacheType = CacheWriteThrough;
    break;

  case EFI_MEMORY_WP:
    CacheType = CacheWriteProtected;
    break;

  case EFI_MEMORY_WB:
    CacheType = CacheWriteBack;
    break;

  case EFI_MEMORY_UCE:
  case EFI_MEMORY_RP:
  case EFI_MEMORY_XP:
  case EFI_MEMORY_RUNTIME:
    return EFI_UNSUPPORTED;

  default:
    return EFI_INVALID_PARAMETER;
  }
  //
  // call MTRR libary function
  //
  Status = MtrrSetMemoryAttribute(
             BaseAddress,
             Length,
             CacheType
           );

  if (!RETURN_ERROR (Status)) {
    //
    // Sync saved MTRR settings
    //
    MtrrGetAllMtrrs (mMtrrTable);
    //
    // Synchronize the update with all APs
    //
    MpStatus = gBS->LocateProtocol (
                      &gEfiMpServiceProtocolGuid,
                      NULL,
                      (VOID **) &MpService
                      );
    if (!EFI_ERROR (MpStatus)) {
      MpStatus = MpService->StartupAllAPs (
                              MpService,
                              LoadMtrrData,
                              TRUE,
                              NULL,
                              0,
                              NULL,
                              NULL
                              );
      if (MpStatus == EFI_NOT_STARTED) {
        //
        // If no enabled APs exit in the system, return success
        //
        MpStatus = EFI_SUCCESS;
      }
      ASSERT_EFI_ERROR (MpStatus);
    }
  }
  return (EFI_STATUS) Status;
}

/**
  Entrypoint of CPU DXE module.
  
  This function is the entrypoint of CPU DXE module. It initializes system and installs
  CPU Architecture Protocol. 

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS   The entrypoint always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
InitializeCpu (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_PHYSICAL_ADDRESS    BaseAddress;
  UINT64                  *TscFrequency;

  InitializeFloatingPointUnits ();

  //
  // Initialize Tsc Timer Frequency.
  // Get TSC frequency from system configuration table with TSC frequency GUID.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiTscFrequencyGuid, (VOID**) &TscFrequency);
  if (Status == EFI_SUCCESS) {
    mTscTimerFrequency = *TscFrequency;
  } else {
    Status = GetActualFrequency (&mTscTimerFrequency);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Initialize GDT and IDT
  //
  InitializeDescriptorTables ();

  //
  // Install CPU Architectural Protocol
  //
  Status = gBS->InstallProtocolInterface (
                  &mHandle,
                  &gEfiCpuArchProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mCpuArchProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Refresh GCD memory space map according to MTRR value.
  //
  RefreshGcdMemoryAttributes ();

  //
  // Report memory space from 0xFEE00000 to 0xFEEFFFFF as memory mapped space for MSI (Message Signaled Interrupt).
  // First, add memory space for MSI
  //
  BaseAddress = CPU_MSI_MEMORY_BASE;
  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeMemoryMappedIo, 
                  BaseAddress,
                  CPU_MSI_MEMORY_SIZE,
                  EFI_MEMORY_UC
                  );

  //
  // Second, allocate this new added memory space.
  //
  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateAddress,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  0,
                  CPU_MSI_MEMORY_SIZE,
                  &BaseAddress,
                  ImageHandle,
                  NULL
                  );

  //
  // If the default local APIC base address has been changed, we need to add/allocate
  // memory space for the 4K size of the local APIC memory range.
  //
  if (PcdGet32 (PcdCpuLocalApicBaseAddress) != CPU_MSI_MEMORY_BASE) {
    BaseAddress = (EFI_PHYSICAL_ADDRESS) PcdGet32 (PcdCpuLocalApicBaseAddress);
    Status = gDS->AddMemorySpace (
                    EfiGcdMemoryTypeMemoryMappedIo, 
                    BaseAddress,
                    SIZE_4KB,
                    EFI_MEMORY_UC
                    );
    ASSERT_EFI_ERROR (Status);

    Status = gDS->AllocateMemorySpace (
                    EfiGcdAllocateAddress,
                    EfiGcdMemoryTypeMemoryMappedIo,
                    0,
                    SIZE_4KB,
                    &BaseAddress,
                    ImageHandle,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }

  // Program Local APIC for virtual wire mode. Enable Spurious
  // Vector, program the LINT0 vector entry as ExtInt, and 
  // program the LINT1 vector entry as NMI
  //
  ProgramVirtualWireMode ();

  //
  // Allocates ACPI NVS memory for MTRR data.
  //
  InitializeMtrrData ();
  MtrrGetAllMtrrs (mMtrrTable);

  return EFI_SUCCESS;
}

/**
  Returns the actual CPU core frequency in MHz.

  This function returns the actual CPU core frequency in MHz.

  @param   Frequency              Pointer to the CPU core frequency.
  
  @retval  EFI_SUCCESS            The frequency is returned successfully.
  @retval  EFI_INVALID_PARAMETER  Frequency is NULL.            

**/
EFI_STATUS
GetActualFrequency (
  OUT UINT64                        *Frequency
  )
{
  UINT64  BeginValue;
  UINT64  EndValue;

  if (Frequency == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Read timestamp counter before and after delay of 100 microseconds
  //
  BeginValue = AsmReadTsc ();
  MicroSecondDelay (100);
  EndValue   = AsmReadTsc ();

  //
  // Calculate the actual frequency
  //
  *Frequency  = DivU64x32Remainder (EndValue - BeginValue, 100, NULL);

  return EFI_SUCCESS;
}