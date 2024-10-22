;;******************************************************************************
;;* Copyright (c) 1983-2017, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************

%include "AmdUefiStackNasm.inc"
%include "SecCoreNasm.inc"
%include "SecPlatformInitNasm.inc"

SECTION .text

global   ASM_PFX(TopOfCar)
extern   ASM_PFX(SecStartup)
extern   ASM_PFX(OemSvcHookBeforePeiCoreEntryPoint)
extern   ASM_PFX(PcdGet64 (PcdPciExpressBaseAddress))
extern   ASM_PFX(PcdGet32 (PcdPciExpressSize))
extern   ASM_PFX(PcdGet32 (PcdFlashFvRecoverySize))
extern   ASM_PFX(PcdGet32 (PcdFlashFvRecoveryBase))
extern   ASM_PFX(PcdGet32 (PcdBootFirmwareVolumeBase))

%define SMM_RESUME_SIGNATURE 0x055AABB66 ;When SMM rsm here on resume take alternate path
%define CAPSULE_SIGNATURE    0x0CA9501EF

BITS    16
;------------------------------------------------------------------------------
;
;  SEC "Security" Code module.
;
;  Transition to non-paged flat-model protected mode from a
;  hard-coded GDT that provides exactly two descriptors.
;  This is a bare bones transition to protected mode only
;  used for while in PEI and possibly DXE.
;
;  IA32 specific cache as RAM modules
;
;  After enabling protected mode, a far jump is executed to
;  TransferToPEI using the newly loaded GDT.
;  This code also enables the Cache-as-RAM
;
;  RETURNS:    none
;
;------------------------------------------------------------------------------

align 4

;; Offset 0xFFFFFFF0 (reset address has code below
;; nop; nop; jmp Flat32Start
;; So we can read offset 0xFFFFFFF3 to find the location of Flat32Start
;; Subtract that with 12 we will know this SMMResumeInfo location
SMMResumeInfo: ;; This offset can bie found as 0xFFFFFFF5 + word [ 0xFFFFFFF3] - 12
      DD BootGDTtable              ; GDT base address
      DW LINEAR_CODE_SEL                  ; code segment
      DW SYS_DATA_SEL                     ; data segment
      DD ASM_PFX(ProtectedModeEntryPoint) ; Offset of our 32 bit code
      DD SMM_RESUME_SIGNATURE

;Flat32Start         PROC NEAR C PUBLIC
global ASM_PFX(_ModuleEntryPoint)
ASM_PFX(_ModuleEntryPoint):

        ;
        ; Save BIST state in EAX
        ;
        mov     ebp,  eax

        ;
        ; Enable LBR
        ;
       ;;; mov     ecx, 1D9h       ; DBG_CTL_MSR
       ;;; rdmsr
       ;;; or      eax, 01h        ; LBR
       ;;; wrmsr

        ;
        ; BspApCheck
        ;
        mov     ecx, APIC_BASE_ADDRESS  ; MSR:0000_001B
        rdmsr
        bt      eax, APIC_BSC           ; Is this the BSC?
        _if ncarry
           ; This is AP
           jmp     Ap_skip_1M_ROM
        _endif

        SEC_PLATFORM_ENABLE_DEBUG_PORT_MACRO

        POSTCODE (0201h);SEC_SYSTEM_POWER_ON

        SEC_PLATFORM_INIT_STAGE1_MACRO

Ap_skip_1M_ROM:
        ;
        ; Switch to Protected mode.
        ;
        mov     esi, GdtDesc
        DB      66h
        lgdt    [cs:si]
        mov     eax, cr0                           ; Get control register 0
        or      eax, 00000003h                     ; Set PE bit (bit #0) & MP bit (bit #1)
        mov     cr0, eax                           ; Activate protected mode
        mov     eax, cr4                           ; Get control register 4
        or      eax, 00000600h                     ; Set OSFXSR bit (bit #9) & OSXMMEXCPT bit (bit #10)
        mov     cr4, eax

        ;
        ; Now we're in Protected16
        ; Set up the selectors for protected mode entry
        ;
        mov     ax, SYS_DATA_SEL
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
        mov     ss, ax

        ;
        ; Go to Protected32
        ;
        mov esi, ASM_PFX(NemInitLinearAddress)
        jmp     far dword [cs:si]
;Flat32Start         ENDP
;_TEXT_REALMODE      ENDS

;_TEXT_PROTECTED_MODE      SEGMENT PARA PUBLIC USE32 'CODE'
;                          ASSUME  CS:_TEXT_PROTECTED_MODE, DS:_TEXT_PROTECTED_MODE

BITS 32

;ProtectedModeEntryPoint PROC NEAR PUBLIC
global ASM_PFX(ProtectedModeEntryPoint)
ASM_PFX(ProtectedModeEntryPoint):

        cmp     edx, SMM_RESUME_SIGNATURE
        je      SmmS3Resume
        jmp     ASM_PFX(InitializeNem)

SmmS3Resume:
        push    eax
        push    ebx
        cmp     EBX, CAPSULE_SIGNATURE
        jnz     capsule_skip
         ; Rebase smm to default
        mov     ecx, 0C0010111h ; SMM_BASE
        rdmsr
        mov     eax, 30000h
        wrmsr

capsule_skip:
        ;
        ; BspApCheck
        ;
        mov     ecx, APIC_BASE_ADDRESS  ; MSR:0000_001B
        rdmsr
        bt      eax, APIC_BSC           ; Is this the BSC?
        _if ncarry
           ; This is AP
           jmp     Ap_skip_Init
        _endif

        SEC_PLATFORM_ENABLE_DEBUG_PORT_MACRO

        POSTCODE (0201h);SEC_SYSTEM_POWER_ON

        SEC_PLATFORM_INIT_STAGE1_MACRO

Ap_skip_Init:
        ;Clear Long Mode Enable
        mov     ecx, 0c0000080h ; EFER MSR number.
        rdmsr                   ; Read EFER.
        btr     eax, 8          ; Set LME=0
        wrmsr                   ; Write EFER.
        ;Open smm ram
        mov     ecx, 0c0010113h ; SMMMask
        rdmsr
        btr     eax, 0          ; Set AValid=0
        btr     eax, 1          ; Set TValid=0
        wrmsr
;         ; Rebase smm to default
;         mov     ecx, 0c0010111h ; SMM_BASE
;         rdmsr
;         mov     eax, 30000h
;         wrmsr
        ;
        ; BspApCheck
        ;
        mov     ecx, APIC_BASE_ADDRESS  ; MSR:0000_001B
        rdmsr
        bt      eax, APIC_BSC           ; Is this the BSC?
        _if ncarry
           ; This is AP
           jmp     ASM_PFX(StartUpApS3)
        _endif
        pop     ebx
        pop     eax

SmmS3ResumeBsp:
        push    eax             ;Address of mPspSmmRsmMemInfo, initialized by PSP SMM Resume code
        push    ebx             ;Capsule update signature

        ;
        ; Pass BFV into the PEI Core
        ;
;        mov  edi, 0FFFFFFFCh
;        push DWORD PTR ds:[edi]
        mov edi, [ASM_PFX(PcdGet32(PcdBootFirmwareVolumeBase))]
        push edi
        ;
        ; Pass stack base into the PEI Core
        ;
        mov  edi, eax
        push dword [ds:edi+08h]

        ;
        ; Pass stack size into the PEI Core
        ;
        push    _PCD_VALUE_PcdBspStackSize
        ;
        ; Pass Control into the PEI Core
        ;
        POSTCODE (0209h);SEC_GO_TO_SECSTARTUP
        call ASM_PFX(SecStartup)

;ProtectedModeEntryPoint ENDP

;StartUpApS3     PROC    NEAR    PUBLIC
global ASM_PFX(StartUpApS3)
ASM_PFX(StartUpApS3):
 .0:
         cli                    ; Family 17h AP just halt here
         hlt
         jmp .0

        ;
        ; Get ApInit Address from CMOS
        ;
        mov     ah, 0A0h ; AGESA_CPU_INIT
        mov     ecx, 04h
GetApInitAddressLoop:
        mov     al, ah
        out     072h, al
        inc     ah
        out     0edh, al
        in      al, 073h
        mov     bl, al
        ror     ebx, 08h
        loop    GetApInitAddressLoop

        ; Go AP Init
        pop     eax
        push    0ABCDABCDh      ;  AP Top of Stack Signature
        push    eax             ;  Address of mPspSmmRsmMemInfo, initialized by PSP SMM Resume code
        push    dword [0]     ; IN EFI_PEI_SERVICES           **PeiServices
        call    ebx             ; AmdCpuInitialize
        ;Should never come here
        jmp     $

;StartUpApS3     ENDP



;InitializeNem   PROC    NEAR    PRIVATE
global ASM_PFX(InitializeNem)
ASM_PFX(InitializeNem):

        ;
        ; Determine processor family
        ;
        mov     eax, 1                           ; Get version information
        cpuid                                    ;
        mov     ebx, eax                         ; Save version information in eax

;        mov     ecx, NB_CFG
;        rdmsr
;        or      edx, bEnableCF8ExtCfg
;        wrmsr

        ; using Pcd instead
        mov     eax, DWORD [ASM_PFX(PcdGet64 (PcdPciExpressBaseAddress))]
        mov     ecx, [ASM_PFX(PcdGet32 (PcdPciExpressSize))]
        or      eax, 1dh
        cmp     ecx, 8000000h
        je      SetPcieMmioMsr

        and     eax, 0FFFFFF00h
        or      eax, 19h
        cmp     ecx, 4000000h
        je      SetPcieMmioMsr

        and     eax, 0FFFFFF00h
        or      eax, 21h

SetPcieMmioMsr:
        xor     edx, edx
        mov     ecx, 0C0010058h
        wrmsr

        ;
        ; Passing parameters into AMD_ENABLE_UEFI_STACK2
        ;
        xor     eax, eax
        mov     eax, ebp
        xor     ecx, ecx
;        mov     ecx, PcdGet32 (PcdFlashFvRecoverySize)
        xor     edx, edx
;        mov     edx, PcdGet32 (PcdFlashFvRecoveryBase)
        AMD_ENABLE_UEFI_STACK2 STACK_AT_TOP, _PCD_VALUE_PcdBspStackSize, 30000h

%define CAR_TEST_KEY 0x012345678
        xor     eax, eax
        push    CAR_TEST_KEY
        pop     eax
        cmp     eax, CAR_TEST_KEY
        je      CarTestOk
        mov     al, 0C2h
        out     80h, al
        jmp $
CarTestOk:
        ;
        ; BspApCheck
        ;
        mov     ecx, APIC_BASE_ADDRESS  ; MSR:0000_001B
        rdmsr
        bt      eax, APIC_BSC           ; Is this the BSC?
        _if ncarry
           ; This is AP
           jmp     ASM_PFX(StartUpAp)
        _endif

        POSTCODE (0207h);SEC_SETUP_CAR_OK

        jmp     ASM_PFX(CallPeiCoreEntryPoint)

;InitializeNem   ENDP


;CallPeiCoreEntryPoint   PROC    NEAR    PRIVATE
;global ASM_PFX(CallPeiCoreEntryPoint)
ASM_PFX(CallPeiCoreEntryPoint):
        ;
        ; Call SEC OEM Function
        ;
        CALL    ASM_PFX(OemSvcHookBeforePeiCoreEntryPoint)
        cmp     al, 0                         ; Return status = Sucess
        je      TransferToSecStartup
        ;
        ; Push CPU count to stack first, then AP's (if there is one)
        ; BIST status, and then BSP's
        ;
        mov     eax, 1
        cpuid
        shr     ebx, 16
        and     ebx, 0000000FFh
        push    ebx
        cmp     bl, 1
        jbe     PushBspBist

;         mov     ecx, MTRR_PHYS_MASK_1
;         rdmsr
;         shr     eax, 12
;         push    ax
;         mov     ecx, MTRR_PHYS_BASE_1
;         rdmsr
;         shr     eax, 12
;         push    ax

PushBspBist:
        push    ebp

        ;
        ; Pass BFV into the PEI Core
        ;
;        mov  edi, 0FFFFFFFCh
;        push DWORD PTR ds:[edi]
        mov edi, [ASM_PFX(PcdGet32(PcdBootFirmwareVolumeBase))]
        push edi

        ;
        ; Pass stack base into the PEI Core
        ;
        push    BSP_STACK_BASE_ADDR

        ;
        ; Pass stack size into the PEI Core
        ;
        push    _PCD_VALUE_PcdBspStackSize
        ;
        ; Pass Control into the PEI Core
        ;
        POSTCODE (0209h);SEC_GO_TO_SECSTARTUP
TransferToSecStartup:
        call ASM_PFX(SecStartup)
;CallPeiCoreEntryPoint   ENDP

;StartUpAp       PROC    NEAR    PUBLIC
global ASM_PFX(StartUpAp)
ASM_PFX(StartUpAp):
        ;
        ; Get ApInit Address from CMOS
        ;
        mov     ah, 0A0h ; AGESA_CPU_INIT
        mov     ecx, 04h
GetApInitAddressLoop2:
        mov     al, ah
        out     072h, al
        inc     ah
        out     0edh, al
        in      al, 073h
        mov     bl, al
        ror     ebx, 08h
        loop    GetApInitAddressLoop2

        ; Go AP Init
        push    strict dword 0  ; IN EFI_PEI_SERVICES **PeiServices
        call    ebx             ; AmdCpuInitialize

;StartUpAp       ENDP

align 10h
                    ;PUBLIC  BootGDTtable

;
; GDT[0]: 0x00: Null entry, never used.
;
NULL_SEL            equ     $ - GDT_BASE               ; Selector [0]
GDT_BASE:
BootGDTtable        DD      0
                    DD      0
;
; Linear data segment descriptor
;
LINEAR_SEL          equ     $ - GDT_BASE               ; Selector [0x8]
                    DW      0FFFFh                     ; limit 0xFFFFF
                    DW      0                          ; base 0
                    DB      0
                    DB      092h                       ; present, ring 0, data, expand-up, writable
                    DB      0CFh                       ; page-granular, 32-bit
                    DB      0
;
; Linear code segment descriptor
;
LINEAR_CODE_SEL     equ     $ - GDT_BASE               ; Selector [0x10]
                    DW      0FFFFh                     ; limit 0xFFFFF
                    DW      0                          ; base 0
                    DB      0
                    DB      09Bh                       ; present, ring 0, data, expand-up, not-writable
                    DB      0CFh                       ; page-granular, 32-bit
                    DB      0
;
; System data segment descriptor
;
SYS_DATA_SEL        equ     $ - GDT_BASE               ; Selector [0x18]
                    DW      0FFFFh                     ; limit 0xFFFFF
                    DW      0                          ; base 0
                    DB      0
                    DB      093h                       ; present, ring 0, data, expand-up, not-writable
                    DB      0CFh                       ; page-granular, 32-bit
                    DB      0

;
; System code segment descriptor
;
SYS_CODE_SEL        equ     $ - GDT_BASE               ; Selector [0x20]
                    DW      0FFFFh                     ; limit 0xFFFFF
                    DW      0                          ; base 0
                    DB      0
                    DB      09Ah                       ; present, ring 0, data, expand-up, writable
                    DB      0CFh                       ; page-granular, 32-bit
                    DB      0
;
; Spare segment descriptor
;
SYS16_CODE_SEL      equ     $ - GDT_BASE               ; Selector [0x28]
                    DW      0FFFFh                     ; limit 0xFFFF
                    DW      0                          ; base 0xF000
                    DB      0Fh
                    DB      09Bh                       ; present, ring 0, code, expand-up, writable
                    DB      00h                        ; byte-granular, 16-bit
                    DB      0
;
; Spare segment descriptor
;
SYS16_DATA_SEL      equ     $ - GDT_BASE               ; Selector [0x30]
                    DW      0FFFFh                     ; limit 0xFFFF
                    DW      0                          ; base 0
                    DB      0
                    DB      093h                       ; present, ring 0, data, expand-up, not-writable
                    DB      00h                        ; byte-granular, 16-bit
                    DB      0

;
; Spare segment descriptor
;
SPARE5_SEL          equ     $ - GDT_BASE               ; Selector [0x38]
                    DW      0                          ; limit 0
                    DW      0                          ; base 0
                    DB      0
                    DB      0                          ; present, ring 0, data, expand-up, writable
                    DB      0                          ; page-granular, 32-bit
                    DB      0
GDT_SIZE            EQU     $ - BootGDTtable           ; Size, in bytes

GdtDesc:                                               ; GDT descriptor
OffsetGDTDesc       EQU     $ - ASM_PFX(_ModuleEntryPoint)
                    DW      GDT_SIZE - 1               ; GDT limit
                    DD      BootGDTtable               ; GDT base address

ASM_PFX(NemInitLinearAddress):
NemInitLinearOffset:
                    DD      ASM_PFX(ProtectedModeEntryPoint)              ; Offset of our 32 bit code
                    DW      LINEAR_CODE_SEL

ASM_PFX(TopOfCar):
                    DD      BSP_STACK_BASE_ADDR + _PCD_VALUE_PcdBspStackSize -4
;_TEXT_PROTECTED_MODE    ENDS
;        END

