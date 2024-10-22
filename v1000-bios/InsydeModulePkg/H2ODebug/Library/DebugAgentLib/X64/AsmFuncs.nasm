;******************************************************************************
;* Copyright (c) 1983-2019, Insyde Software Corporation. All Rights Reserved.
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
;   AsmFuncs for DebugAgentLib
;

DEFAULT REL

SEGMENT data

SmmInstance	dq	0

SEGMENT text

;==============================================================================
;  VOID AsmSaveIdt()
;==============================================================================

global ASM_PFX(AsmSaveIdt)
ASM_PFX(AsmSaveIdt):
		sub	rsp, 0x10
		sidt	[rsp]
		mov	rax, [rsp + 2]
		or	rax, rax
		jz	.0
		mov	rdx, [rax + 0x28]
		shl     rdx, 32
		mov     edx, [rax + 0x24]
		mov     dx, [rax + 0x20]
		cmp	dword [rdx], 0x67626544
		jne	.0
		mov	[SmmInstance], rdx
.0:
		add	rsp, 0x10
		ret

;==============================================================================
;  VOID AsmSmmSetup(BOOLEAN)
;==============================================================================

global ASM_PFX(AsmSmmSetup)
ASM_PFX(AsmSmmSetup):
		sub	rsp, 0x10
		mov	ax, cs
		test	ax, 0x03
		jz	.1
		mov	rax, 0x6762654444656267
		int	3
		jmp	.5
.1:
		sidt	[rsp]
		mov	rax, [rsp + 2]
		or	rax, rax
		jz	.5
		mov	rdx, [rax + 0x28]
		shl	rdx, 32
		mov	edx, [rax + 0x24]
		mov	dx, [rax + 0x20]
		cmp	dword [rdx], 0x67626544
		je	.2
		mov	rdx, [SmmInstance]
		or	rdx, rdx
		jz	.5
		cmp	dword [rdx], 0x67626544
		jne	.5
.2:
		cmp	rdx, [SmmInstance]
		je	.3
		mov	[SmmInstance], rdx
.3:		
		mov	rax, rsp
		or	cl, cl
		jnz	.4
		xor	rax, rax
.4:
		mov	rdx, [rdx + 0x80]
		call	rdx
.5:
		add	rsp, 0x10
		ret

;==============================================================================
;  VOID AsmThunkX64Setup()
;==============================================================================

global ASM_PFX(AsmSetupDxeIdt)
ASM_PFX(AsmSetupDxeIdt):
		xor	rax, rax
		mov	eax, dword [0x3fc]
		or	rax, rax
		jz	quit
		cmp	dword [rax], 0x67626544
		jne	quit
		sub	rsp, 0x10
		mov	word [rsp], 0x3f
		add	rax, 0x18
		mov	[rsp + 2], rax
		cmp	dword [rax - 4], 0
		jne	.6
		push	rbx
		sidt	[rax - 0x10]
		mov	rbx, [rax - 0xe]
		mov	rbx, [rbx - 4]
		mov	[rax - 4], ebx
		pop	rbx
.6:
		lidt	[rsp]
		add	rsp, 0x10
quit:
		ret

