/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA PSP Resume Service Protocol Initialization
 *
 * Contains code to initialize GUID for AMD_PSP_RESUME_SERVICE_PROTOCOL_GUID
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision$   @e \$Date$
 *
 */
/****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_PSP_RESUME_SERVICE_PROTOCOL_H_
#define _AMD_PSP_RESUME_SERVICE_PROTOCOL_H_

/// PSP resume callback priority level
#define PSP_RESUME_CALLBACK_LOWEST_LEVEL  0x1
#define PSP_RESUME_CALLBACK_LOW_LEVEL     0x40
#define PSP_RESUME_CALLBACK_MEDIUM_LEVEL  0x80
#define PSP_RESUME_CALLBACK_HIGH_LEVEL    0xC0
#define PSP_RESUME_CALLBACK_HIGHEST_LEVEL 0xFF

/// Define enum PSP_RESUME_CALLBACK_FLAG
typedef enum {
  PspResumeCallBackFlagBspOnly           = 0x01, ///< Call Back function will only be executed on BSP
  PspResumeCallBackFlagCorePrimaryOnly   = 0x02, ///< Call Back function will only be executed on CorePrimary
  PspResumeCallBackFlagCcxPrimaryOnly    = 0x03, ///< Call Back function will only be executed on CcxPrimary
  PspResumeCallBackFlagDiePrimaryOnly    = 0x04, ///< Call Back function will only be executed on DiePrimary
  PspResumeCallBackFlagSocketPrimaryOnly = 0x05, ///< Call Back function will only be executed on SocketPrimary
  PspResumeCallBackFlagAllCores          = 0x06, ///< Call Back function will be executed on AllCores
} PSP_RESUME_CALLBACK_FLAG;

/// Define enum RESUME_TYPE
typedef enum {
  ResumeFromConnectedStandby  = 0x01, ///< When resume from CS
  ResumeFromS3                = 0x02, ///< When resuming from S3
  InvalidResumeValue          = 0xFF  ///< Invalid Resume Value
} RESUME_TYPE;

typedef struct _PSP_RESUME_SERVICE_PROTOCOL PSP_RESUME_SERVICE_PROTOCOL;

typedef EFI_STATUS (EFIAPI *PSP_RESUME_CALLBACK) (
  IN  RESUME_TYPE   ResumeType,
  IN  VOID          *Context
  );

typedef EFI_STATUS (EFIAPI *PSP_RESUME_REGISTER) (
  IN       PSP_RESUME_SERVICE_PROTOCOL        *This,
  IN       PSP_RESUME_CALLBACK                CallBackFunction,
  IN OUT   VOID                               *Context,
  IN       UINTN                              CallbackPriority,
  IN       PSP_RESUME_CALLBACK_FLAG           Flag,
  OUT      EFI_HANDLE                         *DispatchHandle
);

typedef EFI_STATUS (EFIAPI *PSP_RESUME_UNREGISTER) (
  IN       PSP_RESUME_SERVICE_PROTOCOL        *This,
  IN       EFI_HANDLE                         DispatchHandle
);

/// Structure for PSP_RESUME_SERVICE_PROTOCOL protocol
typedef struct  _PSP_RESUME_SERVICE_PROTOCOL {
  PSP_RESUME_REGISTER    Register;        ///< Register fn
  PSP_RESUME_UNREGISTER  UnRegister;      ///< Unregister fn
} PSP_RESUME_SERVICE_PROTOCOL ;

extern EFI_GUID gAmdPspResumeServiceProtocolGuid;
#endif //_AMD_PSP_RESUME_SERVICE_PROTOCOL_H_

