/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA PSP Resume Service Protocol Initialization
 *
 * Contains code to initialize GUID for AMD_PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL_GUID
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

#ifndef _PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL_H_
#define _PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL_H_



/// Structure for PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL protocol
typedef struct  _PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL {
  UINT8                           *PspMboxSmmBuffer;        //Address for PspMboxSmmBuffer
  BOOLEAN                         *PspMboxSmmFlagAddr;      //Address for PSP mbox insmm flag
} PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL ;

extern EFI_GUID gPspMboxSmmBufferAddressProtocolGuid;
#endif //_PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL_H_

