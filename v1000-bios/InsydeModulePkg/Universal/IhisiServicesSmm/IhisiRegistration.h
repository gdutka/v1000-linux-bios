/** @file
  Type definition for the IHISI Registration functions

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

#ifndef _IHISI_REGISTRATION_H_
#define _IHISI_REGISTRATION_H_

#include <Uefi.h>
#include <H2OIhisi.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/RngLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/H2OIhisi.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/SmmFwBlockService.h>

#define IHISI_EBX_SIGNATURE        SIGNATURE_32 ('O', '2', 'H', '$')

#define IHISI_SIGNATURE            SIGNATURE_32 ('I','H','I','S')

typedef struct {
  UINT32                        Signature;
  LIST_ENTRY                    Link;
  IHISI_FUNCTION                Function;
  UINT8                         Priority;
} IHISI_FUNCTION_ENTRY;

typedef struct _IHISI_FUNCTION_ENTRY {
  UINT32                        Signature;
  LIST_ENTRY                    Link;
  UINT32                        CmdNumber;
  LIST_ENTRY                    FunctionChain;
} IHISI_COMMAND_ENTRY;

typedef struct _IHISI_REGISTRATION_CONTEXT {
  UINT32                        Signature;
  H2O_IHISI_PROTOCOL            Ihisi;
  LIST_ENTRY                    CommandList;
  UINTN                         IhisiCpuIndex;
  EFI_SMM_CPU_PROTOCOL         *SmmCpu;
  UINT32                        IhisiStatus;
} IHISI_CONTEXT;

#define IHISI_CONTEXT_FROM_THIS(a)                 CR(a, IHISI_CONTEXT, Ihisi, IHISI_SIGNATURE)
#define IHISI_COMMAND_ENTRY_FROM_LINK(a)           CR(a, IHISI_COMMAND_ENTRY, Link, IHISI_SIGNATURE)
#define IHISI_FUNCTION_ENTRY_FROM_LINK(a)          CR(a, IHISI_FUNCTION_ENTRY, Link, IHISI_SIGNATURE)

#define UTILITY_ALLOCATE_BLOCK_SIZE                0x80000

#define FLASH_COMPLETE_IN_RECOVERY                 0x01
#define RESET_IN_RECOVERY                          0x02

EFI_STATUS
EFIAPI
InitIhisi (
  VOID
  );

EFI_STATUS
AllocateIhisiBuffer (
  VOID
  );

/**
  IHISI Registration Protocol function for registering IHISI functions

  @param[in] FunctionNumber        Command code of the IHISI function
  @param[in] IhisiFunction         The IHISI function pointer
  @param[in] Priority              The priority level of this function.

  @retval EFI_SUCCESS              IHISI function successfully registered
  @retval EFI_ABORTED              faile to register IHISI function
  @retval EFI_ALREADY_STARTED      IHISI function already registered
**/
EFI_STATUS
EFIAPI
IhisiProtRegisterCommand (
  IN  UINT32                            CmdCode,
  IN  IHISI_FUNCTION                    IhisiFunction,
  IN  UINT8                             Priority
  );

/**
  IHISI Registration Protocol function for registering IHISI functions

  @param[in] CmdCode            Command code of the IHISI function
  @param[in] FromPriority       Functions ranges from Priority
  @param[in] ToPriority         Functions ranges to Priority

  @retval EFI_SUCCESS           IHISI function successfully unregistered
  @retval EFI_ABORTED           Failed to unregister IHISI function
**/
EFI_STATUS
EFIAPI
IhisiProtRemoveFunctions (
  IN  UINT32                            CmdCode,
  IN  UINT8                             FromPriority,
  IN  UINT8                             ToPriority
  );

/**
  Execute IHISI Command

  @param[in]  CommandCode       Command code of IHISI function

  @retval EFI_SUCCESS           The given IHISI command is successfully executed.
  @return Other                 Error status returned from  executed IHISI command.
**/
EFI_STATUS
EFIAPI
IhisiProtExecuteCommand (
  IN UINT32                     CommandCode
  );

/**
  This function is a helper function for IHISI functions to get CPU
  registers of the active CPU in order to get data parameters or buffer
  passed from the application.

  @param[in]  RegisterNum       The corresponding IHIS status code to be handled.

  @return                       The content of specfiic CPU register.
**/
UINT32
EFIAPI
IhisiProtReadCpuReg32 (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum
  );

/**
  This function is a helper function for IHISI functions to write CPU
  registers of the active CPU.

  @param[in]  RegisterNum       The corresponding IHIS status code to be
                                handled.
  @param[in]  RegisterValue     The value to be written to the index of the CPU
                                registered specified by RegisterNum.

  @return                       The content of specfiic CPU register.
**/
EFI_STATUS
EFIAPI
IhisiProtWriteCpuReg32 (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINT32                            Value
  );

EFI_STATUS
EFIAPI
IhisiRuntimeProtect (
  VOID
  );

/**
  This function check if the buffer overlaps with SMRAM.

  @param[in]  Buffer            Pointer to the first byte in a buffer.
  @param[in]  BufferSize        Unsigned integer that specifies the size of the Buffer, in bytes.

  @retval TRUE                  Buffer overlaps with SMRAM.
  @return FALSE                 Buffer does not overlap with SMRAM.
**/
BOOLEAN
EFIAPI
IhisiProtBufferOverlapSmram (
  IN  VOID                              *Buffer,
  IN  UINTN                             BufferSize
  );

/**
  This function check if the whole buffer lies within the command buffer.

  @param[in]  Buffer            Pointer to the first byte in a buffer.
  @param[in]  BufferSize        Unsigned integer that specifies the size of the Buffer, in bytes.

  @retval TRUE                  Whole buffer lies within the command buffer.
  @return FALSE                 Any of byte in buffer lies outside the command buffer.
**/
BOOLEAN
EFIAPI
IhisiProtBufferInCmdBuffer (
  IN  VOID                              *Buffer,
  IN  UINTN                             BufferSize
  );

/**
  This function returns the current possible security levels and the current security level (if any).

  @param[out]  SecurityLevelsSupported    Pointer to a bitmask that indicates the security levels supported by
                                          IHISI on this platform.
  @param[out]  CurrentSecurityLevel       Pointer to a bitmask that indicates which security level was used when the
                                          IHISI session was opened.

  @retval EFI_SUCCESS                     This function completed successfully.
  @return EFI_INVALID_PARAMETER           SecurityLevelsSupported or CurrentSecurityLevel are NULL.
**/
EFI_STATUS
EFIAPI
IhisiProtGetAuthStatus (
  OUT UINT32                    *SecurityLevelsSupported,
  OUT UINT32                    *CurrentSecurityLevel
  );

/**
  This function gets the command buffer and size in bytes of the command buffer.

  @param[out]  Buffer               The address to return a pointer to the command buffer.
  @param[out]  BufferSize           The size in bytes of the return command buffer.

  @retval EFI_SUCCESS               This function completed successfully.
  @return EFI_INVALID_PARAMETER     Buffer or BufferSize is NULL.
**/
EFI_STATUS
EFIAPI
IhisiProtGetCmdBuffer (
  OUT VOID                      **Buffer,
  OUT UINTN                     *BufferSize
  );

/**
  This function gets the image buffer and size in bytes of the image buffer.

  @param[out]  Buffer               The address to return a pointer to the image buffer.
  @param[out]  BufferSize           The size in bytes of the return image buffer.

  @retval EFI_SUCCESS               This function completed successfully.
  @return EFI_INVALID_PARAMETER     Buffer or BufferSize is NULL.
**/
EFI_STATUS
EFIAPI
IhisiProtGetImageBuffer (
  OUT VOID                      **Buffer,
  OUT UINTN                     *BufferSize
  );


/**
  This function returns whether the bytes from Buffer through Buffer + BufferSize - 1
  lie entirely within the image buffer (TRUE) or whether one or more bytes lie outside the
  image buffer (FALSE).
  If PcdH2OIhisiImageBufferSupported is set to FALSE, then this function will always
  return TRUE.

  @param[in]  Buffer            Pointer to the first byte in a buffer.
  @param[in]  BufferSize        Unsigned integer that specifies the size of the Buffer, in bytes.

  @retval TRUE                  Whole buffer lies within the image buffer.
  @return FALSE                 Any of byte in buffer lies outside the image buffer.
**/
BOOLEAN
EFIAPI
IhisiProtBufferInImageBuffer (
  IN  VOID                              *Buffer,
  IN  UINTN                             BufferSize
  );

/**
  Handle all registered handlers for the specified IHISI command with priorities greater than
  or equal to FromPriority and less than or equal to ToPriority, or until one of the handlers
  returns IHISI_END_FUNCTION_CHAIN.

  Functions ranges from FromPriority to ToPriority (inclusive) in the IHISI command function
  chain will be executed.

  @param[in]  CmdCode            Pointer to the first byte in a buffer.
  @param[in]  FromPriority       Vlaue of From Priority
  @param[in]  ToPriority         Vlaue of To Priority

  @retval TRUE                  Whole buffer lies within the image buffer.
  @return FALSE                 Any of byte in buffer lies outside the image buffer.
**/
EFI_STATUS
EFIAPI
IhisiProtExecuteCommandByPriority (
  IN UINT32         CmdCode,
  IN UINT8          FromPriority,
  IN UINT8          ToPriority
  );

/**
  IHISI Protocol installation routine

  @retval EFI_SUCCESS:          IHISI Protocol is successfully installed
  @retval Others                Failed to install IHISI Protocol
**/
EFI_STATUS
EFIAPI
InstallIhisiProtocol(
  VOID
  );

/**
  Returned error code in AL.

  @param[in] IhisiStatus  Returned error code in AL.
**/
VOID
IhisiErrorCodeHandler (
  IN  UINT32                            IhisiStatus
  );

/**
  Register IHISI sub function if SubFuncTable CmdNumber/AsciiGuid define in PcdIhisiRegisterTable list.

  @param[out] SubFuncTable        Pointer to ihisi register table.
  @param[out] TableCount          SubFuncTable count

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
RegisterIhisiSubFunction (
  IHISI_REGISTER_TABLE         *SubFuncTable,
  UINT16                        TableCount
  );

extern IHISI_CONTEXT                       *mIhisiContext;
extern EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL   *mSmmFwBlockService;
extern EFI_SMM_VARIABLE_PROTOCOL           *mSmmVariable;
extern EFI_PHYSICAL_ADDRESS                mIhisiCmdBuffer;
extern UINTN                               mIhisiCmdBufferSize;
extern EFI_PHYSICAL_ADDRESS                mIhisiImageBuffer;
extern UINTN                               mIhisiImageBufferSize;
extern BOOLEAN                             mIhisiLock;
extern UINT32                              mIhisiSessionToken;
extern BOOLEAN                             mPurifyVariable;
extern BOOLEAN                             mSharedBufferInSmRam;
#endif
