//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; 
//; Module Name:
//; 
//;   CpuSmmCounterProtocol.h
//; 
//; Abstract:
//; 
//;   Protocol definition for EFI_SMM_CPU_COUNTER protocol.
//; 

#ifndef _CPU_SMM_COUNTER_PROTOCOL_H_
#define _CPU_SMM_COUNTER_PROTOCOL_H_
//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_SMM_CPU_COUNTER_PROTOCOL EFI_SMM_CPU_COUNTER_PROTOCOL;

//
// Global ID for the Sx SMI Protocol
//
#define EFI_SMM_CPU_COUNTER_GUID  \
  { 0x21f302ae, 0x6e95, 0x471a, 0x84, 0xbc, 0xb1, 0x48, 0x0, 0x40, 0x3a, 0x1d }

typedef struct _EFI_SMM_CPU_COUNTER_PROTOCOL {
  volatile UINT32                  *CpuSmmCounter;
} EFI_SMM_CPU_COUNTER_PROTOCOL;

extern EFI_GUID gEfiSmmCpuCounterProtocolGuid;

#endif