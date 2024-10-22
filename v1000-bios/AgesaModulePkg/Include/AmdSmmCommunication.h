/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdSmmCommunication.h
 *
 * Contains definition for AmdSmmCommunication
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _AMD_SMM_COMMUNICATION_H_
#define _AMD_SMM_COMMUNICATION_H_

extern EFI_GUID gAmdSmmCommunicationHandleGuid;

#define AMD_SMM_COMMUNICATION_ID_S3SCRIPT       0x1     ///< ID for S3 script

#define AMD_SMM_COMMUNICATION_CMN_BUFFER_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data)  + sizeof (AMD_SMM_COMMUNICATION_CMN))
/// SMM communication common buffer
typedef struct _AMD_SMM_COMMUNICATION_CMN {
  UINT32                       id;         ///< ID of smm communication buffer
} AMD_SMM_COMMUNICATION_CMN;

#define AMD_SMM_COMMUNICATION_S3SCRIPT_BUFFER_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data)  + sizeof (AMD_SMM_COMMUNICATION_S3SCRIPT))

typedef struct _AMD_SMM_COMMUNICATION_S3SCRIPT {
  UINT32                       id;         ///< ID of smm communication buffer
  VOID                         *PrivateDataAddress;  ///< Point to AmdS3LibPrivateData
} AMD_SMM_COMMUNICATION_S3SCRIPT;

//Sample for new added command
// #define AMD_SMM_COMMUNICATION_ID_XX       0x2     ///< ID for XX
// #define AMD_SMM_COMMUNICATION_XX_BUFFER_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data)  + sizeof (_AMD_SMM_COMMUNICATION_XX))
// typedef struct _AMD_SMM_COMMUNICATION_XX {
//   UINT32                       id;         ///< ID of smm communication buffer
//   XX_STRUCT                    xx;         ///< Structure for XX
// } _AMD_SMM_COMMUNICATION_XX;

#endif // _AMD_SMM_COMMUNICATION_H_

