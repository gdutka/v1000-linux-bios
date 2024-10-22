/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Platform Protocol
 *
 * Contains definitions for AMD_PSP_PLATFORM_PROTOCOL_GUID
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  UEFI
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ****************************************************************************
 * AMD Generic Encapsulated Software Architecture
 *
 * Description: AmdAgesaDxeProtocol.h - DXE Protocol and GUID Declaration.
 *
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _PSP_PLATFORM_PROTOCOL_H_
#define _PSP_PLATFORM_PROTOCOL_H_

// !!! NOTE Please don't change the GUID defined below
// {CCF14A29-37E0-48ad-9005-1F89622FB798}
#define AMD_PSP_PLATFORM_PROTOCOL_GUID \
  { 0xccf14a29, 0x37e0, 0x48ad, { 0x90, 0x5, 0x1f, 0x89, 0x62, 0x2f, 0xb7, 0x98 } }

/// Resume Handoff Structure
typedef struct {
  UINT32  GdtOffset;                              // GDT table offset for RSM
  UINT16  CodeSelector;                           // CODE Segment Selector
  UINT16  DataSelector;                           // DATA Segment Selector
  UINT32  RsmEntryPoint;                          // IP Address after executing rsm command
  UINT32  EdxResumeSignature;                     // Value keep in EDX after executing rsm command
} RSM_HANDOFF_INFO;


/// PSP Platform Protocol, provide PSP platform customized information
typedef struct _PSP_PLATFORM_PROTOCOL {
  BOOLEAN                 CpuContextResumeEnable; // TRUE:Enable CPU Context Resume, FALSE:Disable CPU Context Resume
  UINT8                   SwSmiCmdtoBuildContext; // SW SMI number for build Cpu Context
  UINT32                  BspStackSize;           // BSP Stack Size for resume
  UINT32                  ApStackSize;            // AP Stack Size for resume
  RSM_HANDOFF_INFO        *RsmHandOffInfo;        // Resume Handoff structure include GDTTable Offset, CS&DS Selector, Entrypoint for Resume
                                                  // Also a signature kept in EDX for identification
} PSP_PLATFORM_PROTOCOL;

extern EFI_GUID gPspPlatformProtocolGuid;

#endif //_PSP_PLATFORM_PROTOCOL_H_

