;/**
; * @file
; *
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  Ccx
; * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
; */
;*****************************************************************************
;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************

.code

EXTRN BspMsrLocation:DWORD
EXTRN ApSyncLocation:DWORD
EXTRN AllowToLaunchNextThreadLocation:DWORD
EXTRN ApStackBasePtr:NEAR PTR QWORD
EXTRN RegSettingBeforeLaunchingNextThread:NEAR
EXTRN ApEntryPointInC:NEAR
EXTRN ApGdtDescriptor:WORD

ApAsmCode PROC

  mov ax, 30h
  mov ds, ax
  mov es, ax
  mov ss, ax

  ; Reset RSP
  mov eax, 1
  cpuid
  shr ebx, 24

  ; Use 1-based APIC ID to index into the top of this AP's stack
  xor rax, rax
  mov eax, ebx
  mov ecx, 0400h
  mul ecx

  ; Make space for the first qword
  sub eax, 8

  mov rsi, ApStackBasePtr
  add rax, rsi
  mov rsp, rax

  ; Enable Fixed MTRR modification
  mov ecx, 0C0010010h
  rdmsr
  or  eax, 00080000h
  wrmsr

  ; Setup MSRs to BSP values
  mov esi, BspMsrLocation
MsrStart:
  mov ecx, [esi]
  cmp ecx, 0FFFFFFFFh
  jz MsrDone
  add esi, 4
  mov eax, [esi]
  add esi, 4
  mov edx, [esi]
  wrmsr
  add esi, 4
  jmp MsrStart

MsrDone:
  ; Disable Fixed MTRR modification and enable MTRRs
  mov ecx, 0C0010010h
  rdmsr
  and eax, 0FFF7FFFFh
  or  eax, 00140000h
  bt  eax, 21
  jnc Tom2Disabled
  bts eax, 22
Tom2Disabled:
  wrmsr

  ; Enable caching
  mov rax, cr0
  btr eax, 30
  btr eax, 29
  mov cr0, rax

  ; Call into C code before next thread is launched
  call RegSettingBeforeLaunchingNextThread

  ; Increment call count to allow to launch next thread
  mov esi, AllowToLaunchNextThreadLocation
  lock inc WORD PTR [esi]

  ; Call into C code
  call ApEntryPointInC

  ; Set up resident GDT
  lea rsi, ApGdtDescriptor
  lgdt FWORD PTR [rsi]
  lea rbx, NewGdtAddress
  lea rsi, NewGdtOffset
  mov [rsi], ebx
  lea rsi, NewGdtOffset
  jmp far ptr [rsi]
NewGdtAddress:

  ; Increment call count to indicate core is done running
  mov esi, ApSyncLocation
  lock inc WORD PTR [esi]

  ; Hlt
@@:
  cli
  hlt
  jmp @B

ApAsmCode ENDP

NewGdtOffset LABEL DWORD
  DD  ?
NewGdtSelector LABEL WORD
  DW  0038h

END

