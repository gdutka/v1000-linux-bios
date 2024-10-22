/** @file

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


#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <PiSmm.h>
#include <H2OStatusCode.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/VariableWrite.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/Variable.h>
#include <Protocol/VariableLock.h>
#include <Protocol/VarCheck.h>
#include <Protocol/SmmVarCheck.h>
#include <Protocol/Hash.h>
#include <Protocol/CryptoServices.h>
#include <Protocol/FaultTolerantWriteLite.h>
#include <Protocol/SmmFtw.h>
#include <Protocol/SmmFwBlockService.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/SmmAccess2.h>
#include <Protocol/NonVolatileVariable.h>
#include <Protocol/SmmReadyToBoot.h>
#include <Protocol/TcgService.h>
#include <Protocol/Tcg2Protocol.h>
#include <Protocol/H2OSmmVariableLockPolicy.h>

#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/VariableSupportLib.h>
#include <Library/ImageRelocationLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/MultiConfigBaseLib.h>
#include <Library/H2OCpLib.h>
#include <Library/ReportStatusCodeLib.h>

#include <Guid/GlobalVariable.h>
#include <Guid/EventGroup.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/AdmiSecureBoot.h>
#include <Guid/DebugMask.h>
#include <Guid/BootOrderHook.h>
#include <Guid/VarErrorFlag.h>
#include <Guid/H2OCp.h>


#include "Reclaim.h"
#include "Misc.h"
#include "SecureBoot.h"
#include "GlobalVariable.h"
#include <MultiConfigStructDef.h>


//
// SMI related definitions
//
#define SW_SMI_PORT                          PcdGet16(PcdSoftwareSmiPort)
#define SECURE_BOOT_SW_SMI                        0xEC
#define UPDATE_VARIABLE_PROPERTY_FUN_NUM          0xF8
#define SMM_VARIABLE_CHECK_FUN_NUM                0xF9
#define SMM_VARIABLE_LOCK_FUN_NUM                 0xFA
#define LEGACY_BOOT_SMI_FUN_NUM                   0xFB
#define SMM_SET_VARIABLE_SMI_FUN_NUM              0xFC
#define DISABLE_VARIABLE_CACHE_SMI_FUN_NUM        0xFE
#define DISABLE_SECURE_BOOT_SMI_FUN_NUM           0xFF

//
// Signature related definitions
//
#define SMM_VARIABLE_CHECK_SIGNATURE            SIGNATURE_32 ('s', 'V', 'C', 'L')
#define SMM_VARIABLE_LOCK_SIGNATURE             SIGNATURE_32 ('s', 'V', 'L', 'L')
#define SMM_LEGACY_BOOT_SIGNATURE               SIGNATURE_32 ('s', 'S', 'L', 'B')
#define SMM_VARIABLE_SIGNATURE                  SIGNATURE_32 ('s', 'm', 'v', 'a')
#define DISABLE_VARIABLE_CACHE_SIGNATURE        SIGNATURE_32 ('s', 'D', 'V', 'C')
#define DISABLE_SET_SENSITIVE_SIGNATURE         SIGNATURE_32 ('s', 'D', 'S', 'S')
#define ENABLE_SET_SENSITIVE_SIGNATURE          SIGNATURE_32 ('s', 'E', 'S', 'S')
#define UPDATE_VARIABLE_PROPERTY_SIGNATURE      SIGNATURE_32 ('s', 'U', 'V', 'P')
#define INSTANCE_FROM_EFI_SMM_VARIABLE_THIS(a)  CR (a, SMM_VARIABLE_INSTANCE, SmmVariable, SMM_VARIABLE_SIGNATURE)

//
// Size related definitions
//
#define VARIABLE_STORE_SIZE               (64 * 1024)
//
// To make sure SCRATCH_SIZE is large enough, set the SCRATCH_SIZE is MAX_VARIABLE_SIZE + 4KB.
//
#define SCRATCH_SIZE                       ((MAX_VARIABLE_SIZE) + (4 * 1024))
#define MAX_HARDWARE_ERROR_VARIABLE_SIZE   MAX_VARIABLE_SIZE
#define VARIABLE_RECLAIM_THRESHOLD         (1024)
#define APPEND_BUFF_SIZE                   MAX_VARIABLE_SIZE

//
// Attributes related definitions
//
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD 0x00000008
#define EFI_VARIABLE_ATTRIBUTES_MASK       (EFI_VARIABLE_NON_VOLATILE | \
                                            EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                                            EFI_VARIABLE_RUNTIME_ACCESS | \
                                            EFI_VARIABLE_HARDWARE_ERROR_RECORD | \
                                            EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS | \
                                            EFI_VARIABLE_APPEND_WRITE)
#define VARIABLE_ATTRIBUTE_NV_BS           (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS)
#define VARIABLE_ATTRIBUTE_BS_RT           (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)
#define VARIABLE_ATTRIBUTE_NV_BS_RT        (VARIABLE_ATTRIBUTE_BS_RT | EFI_VARIABLE_NON_VOLATILE)
#define VARIABLE_ATTRIBUTE_NV_BS_RT_AT     (VARIABLE_ATTRIBUTE_NV_BS_RT | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)
#define VARIABLE_ATTRIBUTE_NV_BS_RT_HR     (VARIABLE_ATTRIBUTE_NV_BS_RT | EFI_VARIABLE_HARDWARE_ERROR_RECORD)
#define VARIABLE_ATTRIBUTE_NV_BS_RT_AW     (VARIABLE_ATTRIBUTE_NV_BS_RT | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS)
//
// Misc definitions
//
#define MAX_TRY_SET_VARIABLE_TIMES         2
#define NEED_DO_RECLAIM_NAME               L"NeedDoReclaim"



#ifndef EFI_OS_INDICATIONS_START_OS_RECOVERY
#define EFI_OS_INDICATIONS_START_OS_RECOVERY 0x0000000000000020
#endif

typedef struct {
  VOID                  *BackupBuffer;
  EFI_LBA               VariableLba;
  UINTN                 VariableOffset;
  EFI_FTW_LITE_PROTOCOL *FtwLiteProtocol;
} VARIABLE_RECLAIM_INFO;

typedef struct {
  VARIABLE_HEADER *CurrPtr;
  VARIABLE_HEADER *EndPtr;
  VARIABLE_HEADER *StartPtr;
  BOOLEAN         Volatile;
} VARIABLE_POINTER_TRACK;

typedef struct {
  EFI_PHYSICAL_ADDRESS  VolatileVariableBase;
  EFI_PHYSICAL_ADDRESS  NonVolatileVariableBase;
  EFI_LOCK              VariableServicesLock;
  UINT32                ReentrantState;
} VARIABLE_GLOBAL;

typedef struct {
  EFI_STATUS                            Status;
  UINT32                                Signature;
  EFI_GUID                              VarGuid;
  UINT32                                Attributes;
  UINTN                                 VariableNameSize;
  UINTN                                 DataSize;
  UINT16                                VarChecksum;
//
//CHAR16      VarName[]; //Input buffer Variable's Name.
//                       //This is a Unicode string and end with Unicode NULL.
//
//UINT8       *VarData;
//
} SMM_VAR_BUFFER;

typedef struct {
  NON_VOLATILE_VARIABLE_PROTOCOL     NonVolatileVariableProtocol;
  VARIABLE_GLOBAL                    VariableBase;
  UINTN                              VolatileLastVariableOffset;
  UINTN                              NonVolatileLastVariableOffset;
  UINTN                              NonVolatileVariableCacheSize;
  UINTN                              CommonVariableSpace;
  UINTN                              CommonMaxUserVariableSpace;
  UINTN                              CommonUserVariableTotalSize;
  EFI_PHYSICAL_ADDRESS               FactoryDefaultBase;
  UINTN                              FactoryDefaultSize;
  EFI_PHYSICAL_ADDRESS               VariableDefaultBase;
  UINTN                              VariableDefaultSize;
  UINT8                              *NonVolatileVariableCache;
  UINT32                             NonVolatileVariableCacheCrc32;
  UINT8                              *FactoryDefaultCache;
  UINT8                              *VariableDefaultCache;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL *FvbInstance;
  LIST_ENTRY                         *VarCheckVariableList;
  BOOLEAN                            SmmCodeReady;
  BOOLEAN                            SecureBootCallbackEnabled;
  BOOLEAN                            BootOrderVariableHook;
  BOOLEAN                            AtRuntime;
  BOOLEAN                            EndOfDxe;
  BOOLEAN                            InsydeSecureVariableLocked;
  BOOLEAN                            VariableWriteReady;
  BOOLEAN                            TcgAvailable;
  PRESERVED_VARIABLE_TABLE           *PreservedVariableTable;
  UINT32                             PreservedTableSize;
  VOID                               *GlobalVariableList;
  EFI_SET_VARIABLE                   SmmSetVariable;
  VARIABLE_RECLAIM_INFO              *ReclaimInfo;
  SMM_VAR_BUFFER                     *SmmVarBuf;
  UINTN                              SmmVarBufSize;
} ESAL_VARIABLE_GLOBAL;


typedef struct {
  EFI_PHYSICAL_ADDRESS   FvbBaseAddress;
  EFI_LBA                Lba;
  UINTN                  Offset;
} VARIABLE_STORE_INFO;

typedef struct {
  UINT32                                Signature;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL     *SmmFwbService;
  EFI_SMM_CPU_PROTOCOL                  *SmmCpu;
  EFI_HANDLE                            SwHandle;
  UINTN                                 CurrentlyExecutingCpu;
  VARIABLE_STORE_INFO                   VariableStoreInfo;
  ESAL_VARIABLE_GLOBAL                  *ProtectedModeVariableModuleGlobal;
} SMM_VARIABLE_INSTANCE;

#define EFI_FREE_POOL(Addr) \
{  mSmst ? mSmst->SmmFreePool((VOID*) (Addr)) : gBS->FreePool ((VOID *) (Addr)); \
   Addr = NULL;}
/**
  Initializes variable store area for non-volatile and volatile variable.

  @retval EFI_SUCCESS           Function successfully executed.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate enough memory resource.

**/
EFI_STATUS
VariableCommonInitialize (
  VOID
  );

/**
  Initializes read-only variable services.

  @retval EFI_SUCCESS           Function successfully executed.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate enough memory resource.
  @return Others                Ohter error occurred in this function.
**/
EFI_STATUS
VariableReadyOnlyInitialize (
  VOID
  );

/**
  Initialize SMM mode NV data

  @param  SmmBase        Pointer to EFI_SMM_BASE_PROTOCOL

  @retval EFI_SUCCESS    Initialize SMM mode NV data successful.
  @retval Other          Any error occurred during initialize SMM NV data.

**/
EFI_STATUS
EFIAPI
SmmNvsInitialize (
  IN  EFI_SMM_BASE2_PROTOCOL               *SmmBase
  );

/**
  Callback function for service administer secure boot

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.

**/
EFI_STATUS
EFIAPI
SecureBootCallback (
  IN EFI_HANDLE     DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommBuffer      OPTIONAL,
  IN OUT UINTN      *CommBufferSize  OPTIONAL
  );

/**
  Setup Variable driver for SMM

  @param    Event     Event whose notification function is being invoked.
  @param    Context   Pointer to the notification function's context.

  None

**/
VOID
EFIAPI
SetupSmmVariable (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

EFI_STATUS
EFIAPI
VariableServicesGetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          * VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data OPTIONAL
  );

EFI_STATUS
EFIAPI
VariableServicesGetNextVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  );

EFI_STATUS
EFIAPI
VariableServicesSetVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINT32                  Attributes,
  IN UINTN                   DataSize,
  IN VOID                    *Data
  );

EFI_STATUS
EFIAPI
VariableServicesQueryVariableInfo (
  IN  UINT32                 Attributes,
  OUT UINT64                 *MaximumVariableStorageSize,
  OUT UINT64                 *RemainingVariableStorageSize,
  OUT UINT64                 *MaximumVariableSize
  );

/**
  This code finds variable in storage blocks (Volatile or Non-Volatile)

  @param VariableName                Name of the variable to be found
  @param VendorGuid                  Vendor GUID to be found.
  @param PtrTrack                    Variable Track Pointer structure that contains Variable Information.
  @param VariableCount               The number of found variable.
  @param Global                      VARIABLE_GLOBAL pointer.

  @retval EFI_INVALID_PARAMETER      If VariableName is not an empty string, while
                                     VendorGuid is NULL.
  @retval EFI_SUCCESS                Variable successfully found.
  @retval EFI_NOT_FOUND              Variable not found

**/
EFI_STATUS
FindVariable (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT       VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT       UINTN                   *VariableCount,
  IN        VARIABLE_GLOBAL         *Global
  );


/**
  This code finds variable in all of storage blocks (Volatile, Non-Volatile variable store and
  variable default store)

  @param VariableName                Name of the variable to be found
  @param VendorGuid                  Vendor GUID to be found.
  @param PtrTrack                    Variable Track Pointer structure that contains Variable Information.
  @param VariableCount               The number of found variable.
  @param Global                      VARIABLE_GLOBAL pointer.

  @retval EFI_INVALID_PARAMETER      If VariableName is not an empty string, while
                                     VendorGuid is NULL.
  @retval EFI_SUCCESS                Variable successfully found.
  @retval EFI_NOT_FOUND              Variable not found

**/
EFI_STATUS
FindVariableInAllRegions (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT       VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT       UINTN                   *VariableCount,
  IN        VARIABLE_GLOBAL         *Global
  );

/**
  Finds variable in storage blocks of volatile and non-volatile storage areas.

  This code finds variable in storage blocks of volatile and non-volatile storage areas.
  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.
  Otherwise, VariableName and VendorGuid are compared.

  @param  VariableName                Name of the variable to be found.
  @param  VendorGuid                  Vendor GUID to be found.
  @param  PtrTrack                    VARIABLE_POINTER_TRACK structure for output,
                                      including the range searched and the target position.
  @param  PtrTrack                    Variable count  for output.
  @param  Global                      Pointer to VARIABLE_GLOBAL structure, including
                                      base of volatile variable storage area, base of
                                      NV variable storage area, and a lock.

  @retval EFI_INVALID_PARAMETER       If VariableName is not an empty string, while
                                      VendorGuid is NULL.
  @retval EFI_SUCCESS                 Variable successfully found.
  @retval EFI_INVALID_PARAMETER       Variable not found.

**/
EFI_STATUS
FindVariableByLifetime (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT       VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT       UINTN                   *VariableCount,
  IN        VARIABLE_GLOBAL         *Global
  );

/**
  This code finds variable in all of storage blocks (Volatile, Non-Volatile variable store and
  variable default store)

  @param VariableName                Name of the variable to be found
  @param VendorGuid                  Vendor GUID to be found.
  @param PtrTrack                    Variable Track Pointer structure that contains Variable Information.
  @param VariableCount               The number of found variable.
  @param Global                      VARIABLE_GLOBAL pointer.

  @retval EFI_INVALID_PARAMETER      If VariableName is not an empty string, while
                                     VendorGuid is NULL.
  @retval EFI_SUCCESS                Variable successfully found.
  @retval EFI_NOT_FOUND              Variable not found

**/
EFI_STATUS
FindVariableByLifetimeInAllRegions (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT       VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT       UINTN                   *VariableCount,
  IN        VARIABLE_GLOBAL         *Global
  );

/**
  Update the variable region with Variable information. If EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS is set,
  index of associated public key is needed.

  @param[in] VariableName       Name of variable.
  @param[in] VendorGuid         Guid of variable.
  @param[in] Data               Variable data.
  @param[in] DataSize           Size of data. 0 means delete.
  @param[in] Attributes         Attributes of the variable.
  @param[in] KeyIndex           Index of associated public key.
  @param[in] MonotonicCount     Value of associated monotonic count.
  @param[in] Variable           The variable information that is used to keep track of variable usage.

  @param[in] TimeStamp          Value of associated TimeStamp.
  @param[in] Global             Pointer to VARIABLE_GLOBAL

  @retval EFI_SUCCESS           The update operation is success.
  @retval EFI_OUT_OF_RESOURCES  Variable region is full, cannot write other data into this region.

**/
EFI_STATUS
UpdateVariable (
  IN CONST CHAR16                 *VariableName,
  IN CONST EFI_GUID               *VendorGuid,
  IN CONST VOID                   *Data,
  IN       UINTN                  DataSize,
  IN       UINT32                 Attributes,
  IN       UINT32                 KeyIndex        OPTIONAL,
  IN       UINT64                 MonotonicCount  OPTIONAL,
  IN       VARIABLE_POINTER_TRACK *Variable,
  IN       EFI_TIME               *TimeStamp      OPTIONAL,
  IN       VARIABLE_GLOBAL        *Global
  );

/**
  Write sub function number to SMI port to trigger SMI.

  @param[in] InPutBuff   Pointer to input buffer
  @param[in] DataSize    Data size by bytes in input buffer.
  @param[in] SubFunNum   SMI sub function number.
  @param[in] SmiPort     SMI port number.

  @retval 0              Always return 0.
**/
UINT8
EFIAPI
SmmSecureBootCall (
  IN     UINT8            *InPutBuff,
  IN     UINTN            DataSize,
  IN     UINT8            SubFunNum,
  IN     UINT16           SmiPort
  );


/**
  This function uses to do specific action when legacy boot event is signaled.

  @retval EFI_SUCCESS      All of action for legacy boot event in SMM mode is successful.
  @retval Other            Any error occurred.
--*/
EFI_STATUS
SmmLegacyBootEvent (
  VOID
  );

/**
  This function uses to invoke SMM mode SetVariable ()

  @retval EFI_SUCCESS     Calling SMM mode SetVariable () successful.
  @return Other           Any error occurred while setting variable.
**/
EFI_STATUS
SmmInternalSetVariable (
  VOID
  );

/**
  Get the proper fvb handle and/or fvb protocol by the given Flash address.

  @param[in]  Address      The Flash address.
  @param[out] FvbHandle    In output, if it is not NULL, it points to the proper FVB handle.
  @param[out] FvbProtocol  In output, if it is not NULL, it points to the proper FVB protocol.

  @retval EFI_SUCCESS      Get fvb handle and/or fvb protocol successfully.
  @retval EFI_NOT_FOUND    Cannot find fvb handle and/or fvb protocol.
**/
EFI_STATUS
GetFvbInfoByAddress (
  IN  EFI_PHYSICAL_ADDRESS                Address,
  OUT EFI_HANDLE                          *FvbHandle OPTIONAL,
  OUT EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  **FvbProtocol OPTIONAL
  );


/**
  This code gets the size of non-volatile variable store.

  @return UINTN           The size of non-volatile variable store.

**/
UINTN
GetNonVolatileVariableStoreSize (
  VOID
  );

/**
  Function to get current nonvolatile offset from physical hardware device.

  @return The offset of the first free nonvolatile variable space.
**/
UINTN
GetCurrentNonVolatileOffset (
  VOID
  );

/**
 This code gets the pointer to the last variable memory pointer byte

 @param[in] VarStoreHeader   Pointer to the Variable Store Header.

 @return                     Pointer to last unavailable Variable Header.
**/
VARIABLE_HEADER *
EFIAPI
GetNonVolatileEndPointer (
  IN VARIABLE_STORE_HEADER       *VarStoreHeader
  );

/**
  This function writes data to the FWH at the correct LBA even if the LBAs
  are fragmented.

  @param[in] Global              Pointer to VARIABLE_GLOBAL structure.
  @param[in] Volatile            If the Variable is Volatile or Non-Volatile
  @param[in] SetByIndex          TRUE: Target pointer is given as index.
                                 FALSE: Target pointer is absolute.
  @param[in] DataPtrIndex        Pointer to the Data from the end of VARIABLE_STORE_HEADER
                                 structure.
  @param[in] DataSize            Size of data to be written.
  @param[in] Buffer              Pointer to the buffer from which data is written.

  @retval EFI_SUCCESS            Variable store successfully updated.
  @retval EFI_INVALID_PARAMETER  Parameters not valid.
--*/
EFI_STATUS
UpdateVariableStore (
  IN  VARIABLE_GLOBAL         *Global,
  IN  BOOLEAN                 Volatile,
  IN  BOOLEAN                 SetByIndex,
  IN  UINTN                   DataPtrIndex,
  IN  UINT32                  DataSize,
  IN  UINT8                   *Buffer
  );

/**
  This code checks if variable header is valid or not and is whether whole data in variable region.

  @param[in] Variable          Pointer to the Variable Header.
  @param[in] VariableStoreEnd  Pointer to the Variable store end.

  @retval TRUE                 Variable header is valid and in variable region.
  @retval FALSE                Variable header is not valid or isn't in variable region.
**/
BOOLEAN
IsValidVariableHeaderInVarRegion (
  IN CONST VARIABLE_HEADER   *Variable,
  IN CONST VARIABLE_HEADER   *VariableStoreEnd
  );

/**
  Function to check is whether PK variable exist.

  @retval TRUE    PK variable exist.
  @retval FALSE   PK variable doesn't exist.
**/
BOOLEAN
DoesPkExist (
  VOID
  );

/**
  Returns if this is MOR related variable.

  @param[in]  VariableName the name of the vendor's variable, it's a Null-Terminated Unicode String
  @param[in]  VendorGuid   Unify identifier for vendor.

  @retval  TRUE            The variable is MOR related.
  @retval  FALSE           The variable is NOT MOR related.
**/
BOOLEAN
IsAnyMorVariable (
  IN CHAR16                                 *VariableName,
  IN EFI_GUID                               *VendorGuid
  );

/**
  Initialization for MOR Lock Control.

  @retval EFI_SUCCESS     MorLock initialization success.
  @return Others          Some error occurs.
**/
EFI_STATUS
MorLockInit (
  VOID
  );


/**
  Initialization for MOR Lock Control at ready to boot event in SMM.
**/
VOID
MorLockInitAtSmmReadyToBoot (
  VOID
  );

/**
  This service is an MOR/MorLock checker handler for the SetVariable().

  @param[in]  VariableName the name of the vendor's variable, as a
                       Null-Terminated Unicode String
  @param[in]  VendorGuid   Unify identifier for vendor.
  @param[in]  Attributes   Point to memory location to return the attributes of variable. If the point
                       is NULL, the parameter would be ignored.
  @param[in]  DataSize     The size in bytes of Data-Buffer.
  @param[in]  Data         Point to the content of the variable.

  @retval  EFI_SUCCESS            The MOR/MorLock check pass, and Variable driver can store the variable data.
  @retval  EFI_INVALID_PARAMETER  The MOR/MorLock data or data size or attributes is not allowed for MOR variable.
  @retval  EFI_ACCESS_DENIED      The MOR/MorLock is locked.
  @retval  EFI_ALREADY_STARTED    The MorLock variable is handled inside this function.
                                  Variable driver can just return EFI_SUCCESS.
**/
EFI_STATUS
SetVariableCheckHandlerMor (
  IN CHAR16     *VariableName,
  IN EFI_GUID   *VendorGuid,
  IN UINT32     Attributes,
  IN UINTN      DataSize,
  IN VOID       *Data
  );

/**
  Check the variable which pointed by CurrPtr is whether a most value variable.

  @param[in, out] CurrPtr      Pointer to the header of current variable
  @param[in, out] StartPtr     Pointer to the start address of variable header.
  @param[in, out] EndPtr       Pointer to the end address of variable header.

  @retval TRUE       The input variable is most value variable.
  @retval FALSE      The input variable isn't most value variable.
**/
BOOLEAN
IsMostValueVariable (
  IN  VARIABLE_HEADER        *CurrPtr,
  IN  VARIABLE_HEADER        *StartPtr,
  IN  VARIABLE_HEADER        *EndPtr
  );

/**
  Allocate SMM confidential pool for MOR lock key.
**/
VOID
InitMorLockKey (
  VOID
  );

extern ESAL_VARIABLE_GLOBAL         *mVariableModuleGlobal;
extern SMM_VARIABLE_INSTANCE        *mSmmVariableGlobal;
extern EFI_SMM_SYSTEM_TABLE2        *mSmst;
extern VOID                         *mStorageArea;
extern VOID                         *mCertDbList;
extern VARIABLE_RECLAIM_INFO        *mVariableReclaimInfo;
extern SMM_VAR_BUFFER               *mSmmPhyVarBuf;
extern BOOLEAN                      mEnableLocking;
extern LIST_ENTRY                   mVarCheckVariableList;
extern SMM_VAR_BUFFER               *mSmmVarBuf;
#endif
