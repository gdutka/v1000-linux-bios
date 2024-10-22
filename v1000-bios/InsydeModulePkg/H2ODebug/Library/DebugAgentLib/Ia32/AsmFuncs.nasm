;******************************************************************************
;* Copyright (c) 1983-2018, Insyde Software Corporation. All Rights Reserved.
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

; .stack
SECTION .text

;==============================================================================
;  VOID AsmSaveIdt()
;==============================================================================

global ASM_PFX(AsmSaveIdt)
ASM_PFX(AsmSaveIdt):
		sub	esp, 0x10
		sidt	[esp]
		mov	eax, [esp + 2]
		or	eax, eax
		jz	.0
		mov	edx, [eax + 0x14]
		mov	dx, [eax + 0x10]
		cmp	dword [edx], 0x67626544
		jne	.0
		mov	dword [ds:0x3fc], edx
.0:
		add	esp, 0x10
		ret

;==============================================================================
;  VOID AsmSmmSetup(BOOLEAN)
;==============================================================================

global ASM_PFX(AsmSmmSetup)
ASM_PFX(AsmSmmSetup):
		sub	esp, 0x10
		sidt	[esp]
		mov	eax, [esp + 2]
		or	eax, eax
		jz	.3
		mov	edx, [eax + 0x14]
		mov	dx, [eax + 0x10]
		cmp	dword [edx], 0x67626544
		je	.1
		mov	edx, dword [ds:0x3fc]
		or	edx, edx
		jz	.3
		cmp	dword [edx], 0x67626544
		jne	.3
.1:
		mov	eax, esp
		test	byte [esp + 0x14], 0xff
		jnz	.2
		xor	eax, eax
.2:
		mov	edx, [edx + 0x6C]
		call	edx
.3:
		add	esp, 0x10
		ret

;==============================================================================
;  VOID AsmThunkX64Setup()
;==============================================================================

global ASM_PFX(AsmSetupDxeIdt)
ASM_PFX(AsmSetupDxeIdt):
		mov	eax, dword [ds:0x3fc]
		or	eax, eax
		jz	.5
		cmp	dword [eax], 0x67626544
		jne	.5
		sub	esp, 0x10
		mov	word [esp], 0x3f
		add	eax, 0x18
		mov	[esp + 2], eax
		cmp	dword [eax - 4], 0
		jne	.4
		push	ebx
		sidt	[eax - 0x10]
		mov	ebx, [eax - 0xe]
		mov	ebx, [ebx - 4]
		mov	[eax - 4], ebx
		pop	ebx
.4:
		lidt	[esp]
		add	esp, 0x10
.5:
		ret

