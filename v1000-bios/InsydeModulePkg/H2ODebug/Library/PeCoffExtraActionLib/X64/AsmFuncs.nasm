;******************************************************************************
;* Copyright (c) 1983-2016, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
;
; Module Name:
;
;   AsmFuncs.nasm
;
; Abstract:
;
;   AsmFuncs for PeCoffExtraActionLib
;

SEGMENT .text

;==============================================================================
;  VOID AsmSendInfo(PE_COFF_LOADER_IMAGE_CONTEXT*)
;==============================================================================

global ASM_PFX(AsmSendInfo)
ASM_PFX(AsmSendInfo):
		sub	rsp, 0x10
		sidt	[rsp]
		mov	rax, [rsp + 2]
		or	rax, rax
		jz	.0
		mov	rdx, [rax + 0x38]
		shl     rdx, 32
		mov     rdx, [rax + 0x34]
		mov     dx, [rax + 0x30]
		cmp	dword [rdx - 16], 0x44656267
		jne	.0
		mov	rax, [rdx - 32]
		mov	rdx, rcx
		call	rax
.0:
		add	rsp, 0x10
		ret

