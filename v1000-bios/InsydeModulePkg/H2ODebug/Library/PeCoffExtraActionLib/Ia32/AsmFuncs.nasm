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

; .stack
SECTION .text

;==============================================================================
;  VOID AsmSendInfo(PE_COFF_LOADER_IMAGE_CONTEXT*)
;==============================================================================

global ASM_PFX(AsmSendInfo)
ASM_PFX(AsmSendInfo):
		sub	esp, 0x10
		sidt	[esp]
		mov	eax, [esp + 2]
		or	eax, eax
		jz	.0
		mov	edx, [eax + 28]
		mov	dx, [eax + 24]
		cmp	dword [edx - 16], 0x44656267
		jne	.0
		push	dword [esp + 0x14]
		push	ebx
		mov	ebx, [eax + 20]
		mov	bx, [eax + 16]
		mov	eax, ebx
		pop	ebx
		push	eax
		mov	edx, [edx - 32]
		call	edx
		add	esp, 8
.0:
		add	esp, 0x10
		ret

