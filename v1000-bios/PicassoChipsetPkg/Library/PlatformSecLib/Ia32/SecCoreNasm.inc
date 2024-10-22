;;******************************************************************************
;;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
;
; This file contains 'Framework Code' and is licensed as such
; under the terms of your license agreement with Intel or your
; vendor.  This file may not be modified, except as allowed by
; additional terms of your license agreement.
;
;------------------------------------------------------------------------------
;
; Copyright (c)  1999 - 2003 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
; 
; Module Name:
;
;   SecCore.inc
;
; Abstract:
;
;   SecCore constants and macros
;
;------------------------------------------------------------------------------

;[-start-110620-IB04170121-modify];
%ifdef   PLATFORM_DEBUG_PORT_NUMBER
  DEBUG_PORT EQU PLATFORM_DEBUG_PORT_NUMBER
%else
  DEBUG_PORT EQU 80h
%endif

%macro POSTCODE 1
;*AMD_EARLY_PORT80  IFDEF   H2O_PORT_80_DEBUG
    ror  eax, 10h
    mov  ax, %1
    out  DEBUG_PORT, ax
    rol  eax, 10h
;*AMD_EARLY_PORT80  ENDIF
%endmacro
;[-end-110620-IB04170121-modify];


L1_MIN_1_WAY_SIZE   EQU (16 * 1024)
TEST_BASE           EQU 0000CC000h

TEST_MASK           EQU (~ ((L1_MIN_1_WAY_SIZE) - 1))

IMAGE_BASE_ADDRESS EQU 0FFFE0000h
BFV_BASE_ADDRESS   EQU 0FFFFFFFCh
PEI_CORE_ENTRY     EQU 0FFFFFFE0h

;[-start-221116-IB148600862-remove];
  ;; only in new kernel
;MSR_SMMADDR_ADDRESS EQU 0C0010112h
;MSR_SMMMASK_ADDRESS EQU 0C0010113h
;  B_TVALID EQU 1
;
;; SMM_S3_RESUME_STATE matches structure in MdeModulePkg\Include\Guid\AcpiS3Context.h
;; PSP_SMM_RSM_MEM_INFO and PSP_SMM_HDR_DATA matches structure in AgesaModulePkg\Include\PspResumeMemInfo.h.
;
;struc SMM_S3_RESUME_STATE
;  .Signature             resq 1
;  .SmmS3ResumeEntryPoint resq 1
;  .SmmS3StackBase        resq 1
;  .SmmS3StackSize        resq 1
;  .SmmS3Cr0              resq 1
;  .SmmS3Cr3              resq 1
;  .SmmS3Cr4              resq 1
;  .ReturnCs              resw 1
;  .ReturnEntryPoint      resq 1
;  .ReturnContext1        resq 1
;  .ReturnContext2        resq 1
;  .ReturnStackPointer    resq 1
;  .Smst                  resq 1
;endstruc
;
;struc PSP_SMM_RSM_MEM_INFO
;  .BspStackSize    resd 1   ; BSP Stack Size for resume
;  .ApStackSize     resd 1   ; AP Stack Size for resume
;  .StackPtr        resq 1   ; Point to the base of Stack
;  .RsmHeapPtr      resq 1   ; Point to the base of Resume Heap
;  .HeapSize        resd 1   ; Reserved Heap Size
;  .TempRegionSize  resd 1   ; Reserved Temp Region Size
;  .TempRegionPtr   resq 1   ; Point to the base of Temporary Region (used for store the dynamic value during SMM SEC Phase)
;endstruc
;
;struc PSP_SMM_HDR_DATA
;  .SmmS3ResumeState      resb SMM_S3_RESUME_STATE_size
;  .PspSmmRsmMemInfo      resb PSP_SMM_RSM_MEM_INFO_size
;  .CcxDataRegion         resq 1
;  .CcxEarlyInit          resq 1
;  .ApStackTop            resq 1
;  .ApSyncFlag            resd 1
;  .CapuseUpdateSignature resd 1
;endstruc
;[-end-221116-IB148600862-remove];
