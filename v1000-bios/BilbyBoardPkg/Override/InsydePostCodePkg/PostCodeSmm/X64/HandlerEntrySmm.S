## @file
#  This file is the PostCodeFeatureHandlerEntry () implementation
#  It is X64 architecture implement
#
#******************************************************************************
#* Copyright (c) 2018, Insyde Software Corporation. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

ASM_GLOBAL ASM_PFX(PostCodeFeatureHandler);


#---------------------------------------;
# PostCodeFeatureHandlerEntry           ;
#---------------------------------------;
#This entry will be registered to the debug exception entry of IDT by PostCodeSmm driver
#And it will call PostCodeFeatureHandler () to perform PostCode features 

#
# +---------------------+ <-- 16-byte aligned ensured by processor
# +    Old SS           +
# +---------------------+
# +    Old RSP          +
# +---------------------+
# +    RFlags           +
# +---------------------+
# +    CS               +
# +---------------------+
# +    RIP              +
# +---------------------+
# +    RBP              +
# +---------------------+ <-- RBP, 16-byte aligned
#
ASM_GLOBAL ASM_PFX(PostCodeFeatureHandlerEntry);
ASM_PFX(PostCodeFeatureHandlerEntry):
  pushq   %rbp
  movq    %rsp, %rbp

  #
  # Since here the stack pointer is 16-byte aligned, so
  # EFI_FX_SAVE_STATE_X64 of EFI_SYSTEM_CONTEXT_x64
  # is 16-byte aligned
  #

## UINT64  Rdi, Rsi, Rbp, Rsp, Rbx, Rdx, Rcx, Rax;
## UINT64  R8, R9, R10, R11, R12, R13, R14, R15;
  pushq %r15
  pushq %r14
  pushq %r13
  pushq %r12
  pushq %r11
  pushq %r10
  pushq %r9
  pushq %r8
  pushq %rax
  pushq %rcx
  pushq %rdx
  pushq %rbx
  pushq 32(%rbp)
  pushq (%rbp)
  pushq %rsi
  pushq %rdi

## UINT64  Gs, Fs, Es, Ds, Cs, Ss;  insure high 16 bits of each is zero
  movzwq   40(%rbp), %rax
  pushq    %rax
  movzwq   16(%rbp), %rax
  pushq    %rax
  movq     %ds, %rax
  pushq    %rax
  movq     %es, %rax
  pushq    %rax
  movq     %fs, %rax
  pushq    %rax
  movq     %gs, %rax
  pushq    %rax

## UINT64  Rip;
  pushq    8(%rbp)

## UINT64  Gdtr[2], Idtr[2];
  subq     $16, %rsp
  sidt    (%rsp)
  subq     $16, %rsp
  sgdt    (%rsp)

## UINT64  Ldtr, Tr;
  xorq     %rax, %rax
  strw     %ax
  pushq    %rax
  sldtw    %ax
  pushq    %rax

## UINT64  RFlags;
  pushq    24(%rbp)

## UINT64  Cr0, Cr1, Cr2, Cr3, Cr4, Cr8;
  movq     %cr8, %rax
  pushq    %rax
  movq     %cr4, %rax
  orq      $0x208, %rax
  movq     %rax, %cr4
  pushq    %rax
  movq     %cr3, %rax
  pushq    %rax
  movq     %cr2, %rax
  pushq    %rax
  xorq     %rax, %rax
  pushq    %rax
  movq     %cr0, %rax
  pushq    %rax

## UINT64  Dr0, Dr1, Dr2, Dr3, Dr6, Dr7;
  movq     %dr7, %rax
  pushq    %rax
  movq     %dr6, %rax
  pushq    %rax
  movq     %dr3, %rax
  pushq    %rax
  movq     %dr2, %rax
  pushq    %rax
  movq     %dr1, %rax
  pushq    %rax
  movq     %dr0, %rax
  pushq    %rax

## FX_SAVE_STATE_X64 FxSaveState;

  subq     $512, %rsp
  movq     %rsp, %rdi
  .byte    0x0f, 0xae, 0b00000111

## UEFI calling convention for x64 requires that Direction flag in EFLAGs is clear
  cld

## UINT32  ExceptionData;
  pushq    16(%rbp)
   

## Prepare parameter and call
  movq     $1, %rcx
  movq     %rsp, %rdx
  #
  # Per X64 calling convention, allocate maximum parameter stack space
  # and make sure RSP is 16-byte aligned
  #
  subq     $(4 * 8 + 8), %rsp
  call     ASM_PFX(PostCodeFeatureHandler)
  addq     $(4 * 8 + 8), %rsp


## UINT64  ExceptionData;
  addq    $8, %rsp

## FX_SAVE_STATE_X64 FxSaveState;

  movq    %rsp, %rsi
  .byte   0x0f, 0xae, 0b00001110
  addq    $512, %rsp

## UINT64  Dr0, Dr1, Dr2, Dr3, Dr6, Dr7;
## Skip restoration of DRx registers to support in-circuit emualators
## or debuggers set breakpoint in interrupt/exception context
  addq    $48, %rsp

## UINT64  Cr0, Cr1, Cr2, Cr3, Cr4, Cr8;
  popq    %rax
  movq    %rax, %cr0
  addq    $8, %rsp
  popq    %rax
  movq    %rax, %cr2
  popq    %rax
  movq    %rax, %cr3
  popq    %rax
  movq    %rax, %cr4
  popq    %rax
  movq    %rax, %cr8

## UINT64  RFlags;
  popq    24(%rbp)

## UINT64  Ldtr, Tr;
## UINT64  Gdtr[2], Idtr[2];
## Best not let anyone mess with these particular registers...
  addq    $48, %rsp

## UINT64  Rip;
  popq    8(%rbp)

## UINT64  Gs, Fs, Es, Ds, Cs, Ss;
  popq    %rax
  # mov     gs, rax ; not for gs
  popq    %rax
  # mov     fs, rax ; not for fs
  # (X64 will not use fs and gs, so we do not restore it)
  popq    %rax
  mov     %rax, %es
  popq    %rax
  mov     %rax, %ds
  popq    16(%rbp)
  popq    40(%rbp)

## UINT64  Rdi, Rsi, Rbp, Rsp, Rbx, Rdx, Rcx, Rax;
## UINT64  R8, R9, R10, R11, R12, R13, R14, R15;
  popq    %rdi
  popq    %rsi
  addq    $8, %rsp
  popq    32(%rbp)
  popq    %rbx
  popq    %rdx
  popq    %rcx
  popq    %rax
  popq    %r8
  popq    %r9
  popq    %r10
  popq    %r11
  popq    %r12
  popq    %r13
  popq    %r14
  popq    %r15

  movq    %rbp, %rsp
  popq    %rbp
  iretq

