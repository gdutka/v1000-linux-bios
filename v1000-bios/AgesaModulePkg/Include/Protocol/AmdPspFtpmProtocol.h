/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD PSP Ftpm Protocol Initialization
 *
 * Contains code to declare Ftpm protocol related definition
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _FTPM_PROTOCOL_H_
#define _FTPM_PROTOCOL_H_

//
// GUID definition
//
extern EFI_GUID gAmdPspFtpmProtocolGuid;

typedef struct _PSP_FTPM_PROTOCOL PSP_FTPM_PROTOCOL;

///Define function prototype: Execute a TPM command
typedef
EFI_STATUS
(EFIAPI *FTPM_EXECUTE) (
  IN     PSP_FTPM_PROTOCOL    *This,
  IN     VOID                 *CommandBuffer,
  IN     UINT32                CommandSize,
  IN OUT VOID                 *ResponseBuffer,
  IN OUT UINT32               *ResponseSize
  );

///Define function prototype: GET TPM related Info
typedef
EFI_STATUS
(EFIAPI *FTPM_CHECK_STATUS) (
  IN     PSP_FTPM_PROTOCOL    *This,
  IN OUT UINTN                *FtpmStatus
  );


///Define function prototype: Send a TPM command
typedef
EFI_STATUS
(EFIAPI *FTPM_SEND_COMMAND) (
  IN     PSP_FTPM_PROTOCOL    *This,
  IN     VOID                 *CommandBuffer,
  IN     UINT32                CommandSize
  );

///Define function prototype: Get a TPM command's response
typedef
EFI_STATUS
(EFIAPI *FTPM_GET_RESPONSE) (
  IN     PSP_FTPM_PROTOCOL     *This,
  IN OUT VOID                  *ResponseBuffer,
  IN OUT UINT32                *ResponseSize
  );


/// Defines PSP_FTPM_PROTOCOL. This protocol is used to get Ftpm info
/// Send TPM command, Get TPM command's response, Execute TPM command(Include send & get response)

typedef struct _PSP_FTPM_PROTOCOL {
  FTPM_EXECUTE                          Execute;          ///< Execute TPM command, include send & get response
  FTPM_CHECK_STATUS                     CheckStatus;      ///< Check TPM Status
  FTPM_SEND_COMMAND                     SendCommand;      ///< Send TPM command
  FTPM_GET_RESPONSE                     GetResponse;      ///< Get Last TPM command response
} PSP_FTPM_PROTOCOL;

#endif //_FTPM_PROTOCOL_H_
