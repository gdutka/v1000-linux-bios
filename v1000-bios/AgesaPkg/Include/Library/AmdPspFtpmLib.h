/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP TPM related functions Prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _PSP_FTPM_LIB_H_
#define _PSP_FTPM_LIB_H_
#include <IndustryStandard/Acpi30.h>

//
/// 1MB @todo check. Also move this to common header file and make it customizable
/// perhaps fed it via build option etc
#define FTPM_COMMAND_BUFFER_SIZE  (16*1024)
#define FTPM_RESPONSE_BUFFER_SIZE (16*1024)

/* SYSTEM_TPM_CONFIG_VAL
 *  Value range for APCB_TOKEN_UID_PSP_SYSTEM_TPM_CONFIG
 */
typedef enum {
  SYSTEM_TPM_CONFIG_DTPM     = 0x00, ///< dTPM
  SYSTEM_TPM_CONFIG_PSP_FTPM = 0x01, ///< PSP FTPM
  SYSTEM_TPM_CONFIG_HSP_FTPM = 0x02, ///< HSP FTPM
  SYSTEM_TPM_CONFIG_NONE     = 0xFF, ///< None of TPM
} SYSTEM_TPM_CONFIG_VAL;
#pragma  pack (push, 1)
/// Define TPM_2_CONTROL_AREA
typedef struct {
    // Interface Identifier
  volatile UINT64                InterfaceIdentifier; ///< Interface Identifier

  // TPM2 Control Area Extension
  volatile UINT32                 Clear;            ///< Clear
  volatile UINT32                 RemainingBytes;   ///< RemainingBytes
  volatile UINT32                 StatusReserved;   ///< StatusReserved
  volatile UINT32                 StatusError;      ///< StatusError
  volatile UINT32                 StatusCancel;     ///< StatusCancel
  volatile UINT32                 StatusStart;      ///< StatusStart
  UINT64                          InterruptControl; ///< InterruptControl
  UINT32                          CommandSize;      ///< CommandSize
  EFI_PHYSICAL_ADDRESS            CommandAddress;   ///< CommandAddress
  UINT32                          ResponseSize;     ///< ResponseSize
  EFI_PHYSICAL_ADDRESS            ResponseAddress;  ///< ResponseAddress
// Memory Absent command/response buffer
  volatile UINT32                 CmdRespHWBuffer;  ///< Cmd/Rsp HW Buffer

} TPM2_CONTROL_AREA;


#pragma  pack (pop)

/**
 *  Set Command,Response buffer registers, Buffer address equal to NULL will skip the setting
 *
 * @param[in]    CommandBuffer              Point to the TPM command buffer
 * @param[in]    CommandSize                Size of the TPM command buffer
 * @param[in]    ResponseBuffer             Point to the TPM response buffer
 * @param[in]    ResponseSize               Size of the TPM response buffer
 *
 * @return       EFI_SUCCESS                Command executed successfully
 * @return       EFI_UNSUPPORTED            Device unsupported
 *
 */
EFI_STATUS
FtpmAssignMemory (
  IN     UINTN                CommandBuffer,
  IN     UINT32               CommandSize,
  IN     UINTN                ResponseBuffer,
  IN     UINT32               ResponseSize
  );

/**
 *  GET TPM related Info
 *
 * @param[in,out] FtpmStatus              Used to hold more detail info (Unused Currently)
 *
 * @return       EFI_SUCCESS              Ftpm function supported
 * @return       EFI_UNSUPPORTED          Ftpm function unsupported
 *
 */
EFI_STATUS
FtpmGetInfo (
  IN OUT UINTN                *FtpmStatus
  );

/**
 *  Send a TPM command
 *
 * @param[in]    CommandBuffer              Point to the TPM command buffer
 * @param[in]    CommandSize                Size of the TPM command buffer
 *
 * @return       EFI_SUCCESS                Command executed successfully
 * @return       EFI_UNSUPPORTED            Device unsupported
 * @return       EFI_TIMEOUT                Command fail due the time out
 * @return       EFI_DEVICE_ERROR           Command fail due the error status set
 * @return       EFI_BUFFER_TOO_SMALL       Response buffer too small to hold the response
 *
 */
EFI_STATUS
FtpmSendCommand (
  IN     VOID                 *CommandBuffer,
  IN     UINT32                CommandSize
  );

/**
 *  Get a TPM command's response
 *
 * @param[in]    ResponseBuffer             Point to the TPM response buffer
 * @param[in]    ResponseSize               Size of the TPM response buffer
 *
 * @return       EFI_SUCCESS                Command executed successfully
 * @return       EFI_UNSUPPORTED            Device unsupported
 * @return       EFI_TIMEOUT                Command fail due the time out
 * @return       EFI_DEVICE_ERROR           Command fail due the error status set
 * @return       EFI_BUFFER_TOO_SMALL       Response buffer too small to hold the response
 *
 */
EFI_STATUS
FtpmGetResponse (
  IN OUT VOID                  *ResponseBuffer,
  IN OUT UINT32                *ResponseSize
  );

/**
 *  Execute a TPM command
 *
 * @param[in]    CommandBuffer              Point to the TPM command buffer
 * @param[in]    CommandSize                Size of the TPM command buffer
 * @param[in]    ResponseBuffer             Point to the TPM response buffer
 * @param[in]    ResponseSize               Size of the TPM response buffer
 *
 * @return       EFI_SUCCESS                Command executed successfully
 * @return       EFI_UNSUPPORTED            Device unsupported
 * @return       EFI_TIMEOUT                Command fail due the time out
 * @return       EFI_DEVICE_ERROR           Command fail due the error status set
 * @return       EFI_BUFFER_TOO_SMALL       Response buffer too small to hold the response
 *
 */
EFI_STATUS
FtpmExecuteCommand (
  IN     VOID                 *CommandBuffer,
  IN     UINT32               CommandSize,
  IN OUT VOID                 *ResponseBuffer,
  IN OUT UINT32               *ResponseSize
  );
#endif //_PSP_FTPM_LIB_H_
