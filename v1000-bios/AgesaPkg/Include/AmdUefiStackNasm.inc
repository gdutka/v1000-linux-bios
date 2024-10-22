;*****************************************************************************
; AMD Generic Encapsulated Software Architecture
;
;  Workfile: AmdUefiStackNasm.inc
;  $Revision$    $Date$
;
; Description: Code to setup temporary memory access for stack usage. This code
; is to be used on memory present systems that do not need CAR
;
;*****************************************************************************
;
; Copyright 2008 - 2021 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
;
; AMD is granting You permission to use this software and documentation (if
; any) (collectively, the "Software") pursuant to the terms and conditions of
; the Software License Agreement included with the Software. If You do not have
; a copy of the Software License Agreement, contact Your AMD representative for
; a copy.
;
; You agree that You will not reverse engineer or decompile the Software, in
; whole or in part, except as allowed by applicable law.
;
; WARRANTY DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
; KIND. AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
; BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, QUALITY,
; FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT AND WARRANTIES
; ARISING FROM CUSTOM OF TRADE OR COURSE OF USAGE WITH RESPECT TO THE SOFTWARE,
; INCLUDING WITHOUT LIMITATION, THAT THE SOFTWARE WILL RUN UNINTERRUPTED OR
; ERROR-FREE. THE ENTIRE RISK ASSOCIATED WITH THE USE OF THE SOFTWARE IS
; ASSUMED BY YOU. Some jurisdictions do not allow the exclusion of implied
; warranties, so the above exclusion may not apply to You, but only to the
; extent required by law.
;
; LIMITATION OF LIABILITY AND INDEMNIFICATION: TO THE EXTENT NOT PROHIBITED BY
; APPLICABLE LAW, AMD AND ITS LICENSORS WILL NOT, UNDER ANY CIRCUMSTANCES BE
; LIABLE TO YOU FOR ANY PUNITIVE, DIRECT, INCIDENTAL, INDIRECT, SPECIAL OR
; CONSEQUENTIAL DAMAGES ARISING FROM POSSESSION OR USE OF THE SOFTWARE OR
; OTHERWISE IN CONNECTION WITH ANY PROVISION OF THIS AGREEMENT EVEN IF AMD AND
; ITS LICENSORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. THIS
; INCLUDES, WITHOUT LIMITATION, DAMAGES DUE TO LOST OR MISAPPROPRIATED DATA,
; LOST PROFITS OR CONFIDENTIAL OR OTHER INFORMATION, FOR BUSINESS INTERRUPTION,
; FOR PERSONAL INJURY, FOR LOSS OF PRIVACY, FOR FAILURE TO MEET ANY DUTY
; INCLUDING OF GOOD FAITH OR REASONABLE CARE, FOR NEGLIGENCE AND FOR ANY OTHER
; PECUNIARY OR OTHER LOSS WHTSOEVER. In no event shall AMD's total liability to
; You for all damages, losses, and causes of action (whether in contract, tort
; (including negligence) or otherwise) exceed the amount of $50 USD. You agree
; to defend, indemnify and hold harmless AMD, its subsidiaries and affiliates
; and their respective licensors, directors, officers, employees, affiliates or
; agents from and against any and all loss, damage, liability and other
; expenses (including reasonable attorneys' fees), resulting from Your
; possession or use of the Software or violation of the terms and conditions of
; this Agreement.
;
; U.S. GOVERNMENT RESTRICTED RIGHTS: Notice to U.S. Government End Users. The
; Software and related documentation are "commercial items", as that term is
; defined at 48 C.F.R. Section 2.101, consisting of "commercial computer
; software" and "commercial computer software documentation", as such terms are
; used in 48 C.F.R. Section 12.212 and 48 C.F.R. Section 227.7202,
; respectively. Consistent with 48 C.F.R. Section 12.212 or 48 C.F.R. Sections
; 227.7202-1 through 227.7202-4, as applicable, the commercial computer
; software and commercial computer software documentation are being licensed to
; U.S. Government end users: (a) only as commercial items, and (b) with only
; those rights as are granted to all other end users pursuant to the terms and
; conditions set forth in this Agreement. Unpublished rights are reserved under
; the copyright laws of the United States.
;
; EXPORT RESTRICTIONS:  You shall adhere to all applicable U.S. import/export
; laws and regulations, as well as the import/export control laws and
; regulations of other countries as applicable. You further agree You will not
; export, re-export, or transfer, directly or indirectly, any product,
; technical data, software or source code received from AMD under this license,
; or the direct product of such technical data or software to any country for
; which the United States or any other applicable government requires an export
; license or other governmental approval without first obtaining such licenses
; or approvals, or in violation of any applicable laws or regulations of the
; United States or the country where the technical data or software was
; obtained. You acknowledges the technical data and software received will not,
; in the absence of authorization from U.S. or local law and regulations as
; applicable, be used by or exported, re-exported or transferred to: (i) any
; sanctioned or embargoed country, or to nationals or residents of such
; countries; (ii) any restricted end-user as identified on any applicable
; government end-user list; or (iii) any party where the end-use involves
; nuclear, chemical/biological weapons, rocket systems, or unmanned air
; vehicles.  For the most current Country Group listings, or for additional
; information about the EAR or Your obligations under those regulations, please
; refer to the website of the U.S. Bureau of Industry and Security at
; http://www.bis.doc.gov/.
;******************************************************************************

;============================================================================
;
; Define a  macro that allow the OEM to specify supported solutions in the
; cache-as-ram code. This will reduce the size of the assembled file.
; The macro will convert solutions into supported families.
;
;============================================================================

%include "cpstackNasm.inc"

;======================================================================
;   Reference: UEFI PI v1.2 definition:
;
;   typedef struct _UEFI_SEC_PEI_HAND_OFF {
;       UINT16  DataSize;
;       VOID    *BootFirmwareVolumeBase;
;       UINTN   BootFirmwareVolumeSize;
;       VOID    *TemporaryRamBase;
;       UINTN   TemporaryRamSize;
;       VOID    *PeiTemporaryRamBase;
;       UINTN   PeiTemporaryRamSize;
;       VOID    *StackBase;
;       UINTN   StackSize;
;   } UEFI_SEC_PEI_HAND_OFF;
;

struc UEFI_SEC_PEI_HAND_OFF
.DATA_SIZE                   resw 1
                             alignb 4
.BOOT_FIRMWARE_VOLUME_BASE   resd 1
.BOOT_FIRMWARE_VOLUME_SIZE   resd 1
.TEMPORARY_RAM_BASE          resd 1
.TEMPORARY_RAM_SIZE          resd 1
.PEI_TEMPORARY_RAM_BASE      resd 1
.PEI_TEMPORARY_RAM_SIZE      resd 1
.STACK_BASE                  resd 1
.STACK_SIZE                  resd 1
.size:
endstruc

; Assure build option is defined, default is BIST only storage
%IFNDEF AMD_STACK_FRAME_PAD
    AMD_STACK_FRAME_PAD     EQU     0
%ENDIF


;======================================================================
;======================================================================
; AMD_ENABLE_UEFI_STACK2:  Setup a stack, heap & UEFI stack frame
;
;   Input condition requirements:
;       32bit protected 'flat addressing' mode
;       SS, DS, ES = segment descriptor defining 0x00000000 as the base.
;
;   Build time options:
;       AMD_STACK_FRAME_PAD EQU 00h
;              used to create a Host Env stack frame for pseudo
;              global variables - a build time option. Incremented
;              by 4 to cover the BIST storage.
;
;   Input Parameter:
;       StackLocation
;         STACK_AT_TOP
;                Indicate stack is on the top of cache as RAM.
;         STACK_AT_BOTTOM (default)
;                Indicate stack is at the bottom of cache as RAM.
;       BspStackSize = Stack size for BSP
;       BspStackAddr = Stack base address for BSP
;
;   In:
;       EAX  = BIST value collected after reset by host env
;       EBX  = Return address (preserved)
;       ECX  = size, in bytes, of the region to cache for execution.
;       EDX  = base address of region to cache, or zero for (4GB - size).
;
;   Out:
;       SS:ESP - Our new private stack location
;
;       EAX = AGESA_STATUS
;       EDX = Return status code if EAX contains a return code of higher
;             severity than AGESA_SUCCESS
;       ECX = Stack size in bytes
;       EDI  = pointer to stack frame location. Points to the
;               beginning of the UEFI structure defined by the
;               PI v1.2 spec. The Host Env stack frame follows
;               this structure.
;               [EDI]UEFI_SEC_PEI_HAND_OFF.BOOT_FIRMWARE_VOLUME_BASE = OEM_BFV_BASE
;               [EDI]UEFI_SEC_PEI_HAND_OFF.BOOT_FIRMWARE_VOLUME_SIZE = OEM_BFV_SIZE
;               [EDI+sizeof(UEFI_SEC_PEI_HAND_OFF) + OEM_DATA_DWORD[0] = BIST
;
;   Preserved:
;       EBX, EBP, DS, ES, SS
;
;   Destroyed:
;       EAX, ECX, EDX, EDI, ESI, ESP
;       MMX0, MMX1, MMX2, MMX3, MMX4, MMX5  ... MMX[0..7] are used as save/restore storage
;
;   Known Limitations:
;       *!* This routine presently is limited to a max of 64 processor cores
;
;   Description:
;       This procedure will do the following:
;       - allocate pre-defined address space for use as a stack for C code
;       - allocate pre-defined address space for use as a UEFI heap
;       - enable execution cache for a specified region
;       - create an instance of the UEFI structure UEFI_SEC_PEI_HAND_OFF on the
;           stack and populate it with values.
;
;     Stack Allocation:
;       Note: At present, the stack allocation is the same as described above in AMD_ENABLE_STACK_PRIVATE.
;           In fact, this macro uses that macro to perform the allocation.
;           The same 64 core limit applies to this implementation.
;       Future versions of this macro will expand support to 80+ cores.
;           Stack allocation will be 64k for the BSP, 16K for all APs.
;       ESP is set to point below the HostEnv stack frame.
;
;     Heap Allocation:
;       Note: At present, only the BSP will be allocated space for a UEFI heap.
;       Future versions of this macro will allcate 48K for each AP and the
;           allocation for the BSP will vary for the size of the L2 present and
;           the number of cores sharing the L2; maximizing the BSP allocation.
;
;     Execution cache:
;       The code will use Variable MTRRs 6 and 7 to define an area of memory
;       enabled for execution cache. This is presumed to include the PEI core
;       code area. The allocation is presummed to be at top-of-4G address range
;       so the smaller block, if one exists, will be allocated at the base
;       parameter (edx) and the larger block just after (edx+sizeof(smaller block))
;
;      HostEnv UEFI stack frame:
;       The code will create a stack data frame containing:
;       * a Host Env specific area for pseudo global variables.
;           o This area is at 'bottom (defalult)' so as to be retained if the PEI core decides
;               to reset the stack.
;           o The first entry in this area is the BIST value.
;       * an SEC-PEI hand-off structure (PI v1.2)
;           o populated with the stack and Temporary RAM entries.
;           o A processor's stack and heap are contiguous, with stack on 'top'.
;
;======================================================================
%macro AMD_ENABLE_UEFI_STACK2 3
;%1 = StackPosition, %2 = BspStackSize, %3 = BspStackAddr
    movd    mm1, ebp                    ; Save user requested register
    movd    mm0, ebx                    ; Logically 'push' the input parameters ( return address )
    movd    mm2, eax                    ;   ( BIST )
    movd    mm3, ecx                    ;   ( cache zone size )
    movd    mm4, edx                    ;   ( cache zone base )

    ; Short term method - need to accommodate existance of UEFI heap AND the AGESA heap.
    ;   So, use the old stack allocation process for stack, then mimick current UEFI (~v0.9)
    ;   operation to fill in the data stack frame.
    AMD_ENABLE_STACK_PRIVATE %1, %2, %3
    cmp     eax, AGESA_SUCCESS          ; Abort if not first entry; future versions will not allow multi-entry
    jne     %%AmdEnableUefiStackAbort

    ; review:
    ;       EAX = AGESA_STATUS
    ;       EDX = Return status code if EAX contains a return code of higher
    ;             severity than AGESA_SUCCESS
    ;       ECX = Stack size in bytes
    ;       ebx - return address parameter
    ;       ebp - user preserved register
    ;       ss:esp - stack pointer
    ;
    ;       esi -  address of start of stack block
    ;       [esp] - stack base address
    ;       [esp+4] - size of stack
    ;       [esp+8] - Marker for top-of-stack
    ;       mm0 - user return address
    ;       mm1 - user saved EBP register content
    ;       mm2 - BIST value
    ;       mm3 - cache zone size
    ;       mm5 - 32b pointer to family info struc. Set by GET_NODE_ID_CORE_ID_Fxx macros

    ; calculate stack frame pointer

    mov     ebp, [esp]
    mov     edx, ebp                            ; save stack base to edx

    ; for BSC, we divide the memory allocation zone in half and allocate 1/2 to each of stack & UEFI heap
    ; for APs, we allocate whole allocation to stack
    IS_BSC
;    .if (carry?)
    jnc %%AEUS_1
    %ifidni %1, STACK_AT_BOTTOM
        shr ecx, 1
        add ebp, ecx
        shl ecx, 1
    %else
        add ebp, ecx
    %endif
    jmp     %%AEUS_2
;    .else
%%AEUS_1:
    add     ebp, ecx
;    .endif
%%AEUS_2:
    
    sub     ebp, (4 + AMD_STACK_FRAME_PAD)      ; space for BIST and additional OEM data
    movd    eax, mm2                            ; retrieve BIST data OEM acquired after reset
    mov     [ebp], eax                          ; place BIST data into first OEM data DWORD
    sub     ebp, UEFI_SEC_PEI_HAND_OFF.size     ; space for UEFI structure storage
    mov     eax, edx                            ; retrieve memory base address for passing on
    mov     esp, ebp                            ; now can update the esp
    ; fill the UEFI stack frame
    mov     [ebp + UEFI_SEC_PEI_HAND_OFF.TEMPORARY_RAM_BASE], eax
    mov     [ebp + UEFI_SEC_PEI_HAND_OFF.TEMPORARY_RAM_SIZE], ecx
    mov     word [ebp + UEFI_SEC_PEI_HAND_OFF.DATA_SIZE], UEFI_SEC_PEI_HAND_OFF.size

    ; for BSC, we divide the memory zone in half and allocate 1/2 to each of stack & UEFI heap
    IS_BSC
;    .if (carry?)
    jnc %%AEUS_3
    push    ecx
    shr     ecx, 1                          ; divide the memory zone in half and allocate 1/2 to each of stack & UEFI heap
    mov     [ebp + UEFI_SEC_PEI_HAND_OFF.PEI_TEMPORARY_RAM_SIZE], ecx
    mov     [ebp + UEFI_SEC_PEI_HAND_OFF.STACK_SIZE], ecx

    %ifidni %1, STACK_AT_BOTTOM
        mov     [ebp + UEFI_SEC_PEI_HAND_OFF.STACK_BASE], eax
        add     eax, ecx                ; put PEI temporary RAM base in upper half
        mov     [ebp + UEFI_SEC_PEI_HAND_OFF.PEI_TEMPORARY_RAM_BASE], eax
    %else
        mov     [ebp + UEFI_SEC_PEI_HAND_OFF.PEI_TEMPORARY_RAM_BASE], eax
        add     eax, ecx                ; put stack base in upper half
        mov     [ebp + UEFI_SEC_PEI_HAND_OFF.STACK_BASE], eax
    %endif
    pop     ecx
    jmp     %%AEUS_4
;    .else
%%AEUS_3:
    ; for APs, we allocate whole memory to stack
    mov     dword [ebp + UEFI_SEC_PEI_HAND_OFF.PEI_TEMPORARY_RAM_BASE], 0
    mov     dword [ebp + UEFI_SEC_PEI_HAND_OFF.PEI_TEMPORARY_RAM_SIZE], 0
    mov     [ebp + UEFI_SEC_PEI_HAND_OFF.STACK_SIZE], ecx
    mov     [ebp + UEFI_SEC_PEI_HAND_OFF.STACK_BASE], eax
;    .endif
%%AEUS_4:

    ; we will use the cache zone as implied BFV,
    ; The OEM is free to override this from their code that follows
    movd    eax, mm3                            ; cache zone size
    mov     [ebp + UEFI_SEC_PEI_HAND_OFF.BOOT_FIRMWARE_VOLUME_SIZE], eax

    ; calculate the base from size
    movd    ebx, mm4
;    .if (ebx == 0)
    cmp     ebx, 0
    jne     %%AEUS_5
    sub     ebx, eax
    movd    mm4, ebx
;    .endif
%%AEUS_5:
    mov     [ebp + UEFI_SEC_PEI_HAND_OFF.BOOT_FIRMWARE_VOLUME_BASE], ebx

    ; Round up the size if there are more than 2 bits set in the given cache zone size
    push    edx
    push    ecx
    push    eax

    bsr     ecx, eax
;    .if (!ZERO?)
    jz      %%AEUS_6
    btr     eax, ecx                        ; there is one bit set in the given cache zone size
    bsr     ecx, eax
;        .if (!ZERO?)
    jz     %%AEUS_7
    push    ecx
    btr     eax, ecx                    ; there are two bits set in the given cache zone size

    bsr     ecx, eax
;            .if (!ZERO?)
    jz   %%AEUS_8
    pop     ecx                     ; ecx is the index of second bit set from most-significant
    pop     eax                     ; eax is the given cache zone size

    xor     edx, edx
    bts     edx, ecx
    add     eax, edx                ; round up the size
    dec     edx
    bts     edx, ecx                ; former 2nd bit spot should now be =0, clear it also
    not     edx
    and     eax, edx                ; now, eax has two bits set at most, could have only one

    push    eax
    jmp %%AEUS_9
;            .else
%%AEUS_8:
    pop     ecx                     ; balance the stack
;            .endif
%%AEUS_9:
;        .endif
%%AEUS_7:
;    .endif
%%AEUS_6:

    pop     eax
    pop     ecx
    pop     edx
    movd    mm3, eax                            ; update cache zone size

    ; Check for and apply any family size limits.
    movd    edi, mm5
    mov     bx, [edi + CPU_FAMILY_INFO.L2_ALLOC_EXE]

;    .if (bx > 0)                                ; if there is a family limit
    cmp     bx, 0
    jbe     %%AEUS_10                                ; if there is a family limit
    ; CPUID will destroyed EAX, EBX, ECX, EDX
    ; but we only want to preserve EAX, ECX, EDX
    push    eax
    push    ecx
    push    edx

    ; get L2 allocate execution cache = CPU_FAMILY_INFO.L2_ALLOC_EXE + (L2 cache size - CPU_FAMILY_INFO.L2_MIN_SIZE)
    AMD_CPUID   AMD_CPUID_L2Cache
    shr     ecx, 16                         ; CX = L2 cache size
    sub     cx, [edi + CPU_FAMILY_INFO.L2_MIN_SIZE]  ; CX = additional L2 size to the family limit
    mov     bx, [edi + CPU_FAMILY_INFO.L2_ALLOC_EXE] ; use the additional L2 for exe cache
    add     bx,  cx

    ; restore EAX, ECX, EDX
    pop     edx
    pop     ecx
    pop     eax

    movzx   ebx, bx                         ;   convert the limit from K to Bytes
    shl     ebx, 10
;        .if     (eax > ebx)                     ;   enforce the family limit
    cmp     eax, ebx
    jbe     %%AEUS_11                       ;   enforce the family limit
    ; note: SEC-PEI data is NOT updated on purpose, to allow the PEI
    ;   to see the full intended zone as the BFV

    mov     eax, ebx                    ;   set size to family limit
    movd    mm3, eax                    ;   update cache zone size
;        .endif
%%AEUS_11:
;    .endif
%%AEUS_10:

    ; base = 4G - size
;    push    edx
;    xor     edx, edx
;    sub     edx, eax
;    movd    mm4, edx
;    pop     edx
    ; review:
    ;       eax - Cache zone size
    ;       ebx -
    ;       ecx - Stack size in bytes
    ;       edx - Return status code if EAX contains a return code of higher
    ;             severity than AGESA_SUCCESS
    ;       ebp - Stack Frame pointer
    ;
    ;       esi -  address of start of stack block
    ;       mm0 - user return address
    ;       mm1 - user saved EBP register content
    ;       mm3 - cache zone size
    ;       mm4 - cache zone base
    ;       mm5 - 32b pointer to family info struc. Set by GET_NODE_ID_CORE_ID_Fxx macros

    ; Cross check the cache zone for proper base/length values,
    push    edx
    push    ecx

    and     eax, 0FFFF8000h                     ; size must be >= 32K

    ; Size a Power of Two? We can pull the two largest blocks from the size
    ; then set first avaible vMTRR to cover those blocks of the zone. The zone is presumed
    ; to be at the top of 4G memory space, so the blocks are allocated in a
    ; 'top down' manner, smaller first at base address then the larger.
    bsr     ecx, eax                            ; Is parameter non-zero?
;    .if (!ZERO?)                                ; Is parameter non-zero?;
    jz      %%AEUS_12
    push    ecx                             ; save size of larger block
    btr     eax, ecx                        ; reduce zone size by 1st 2**N
    push    eax

    ; skip vMTRR setting if it's not a primary thread
    pushad
    AMD_CPUID   CPUID_MODEL
    shr     ebx, LOCAL_APIC_ID
    and     ebx, 0FFh ; ebx - initial local APIC physical ID
    push    ebx

    AMD_CPUID   AMD_CPUID_EXT_APIC

    pop     eax ; eax - initial local APIC physical ID
    shr     ebx, 8
    and     ebx, 0FFh
    inc     bl ; bl - ThreadsPerCore
    div     bl
    cmp     ah, 0
    popad
    jnz %%AmdSkipvMtrrSetting

    ; calculate upper mask value - needs to match the CPU address bus size
    movzx   ax, [edi + CPU_FAMILY_INFO.SIZE_ADDRESS_BUS]
    movzx   eax, ax
    xor     edx, edx

    ; find out the first vMTRR which is available
    push eax
    push ecx
    push edx
    mov ecx, AMD_MTRR_VARIABLE_MASK0
%%AEUS_14:
;        .while (ecx <= AMD_MTRR_VARIABLE_MASK7)
    cmp ecx, AMD_MTRR_VARIABLE_MASK7
    ja %%AEUS_13  

     _RDMSR
;            .if (eax == 0)
    cmp eax, 0
    jne %%AEUS_15
    mov edi, ecx
    dec edi                         ; now edi points to AMD_MTRR_VARIABLE_BASEx
    jmp %%AmdvMtrrFound
;            .endif
%%AEUS_15:
    add ecx, 2
    jmp %%AEUS_14
;        .endw
%%AEUS_13:

%%AmdvMtrrFound:
    pop edx
    pop ecx
    pop eax
    cmp edi, AMD_MTRR_VARIABLE_BASE7
    ja %%AmdSkipvMtrrSetting            ; There's no enough vMTRR register pairs for ROM cache

;        .if (al <= 64)
    cmp al, 64
    ja %%AEUS_16
    bts     edx, eax
;        .endif
%%AEUS_16:
    dec     edx                             ; edx = upper mask (e.g. 0x000FFFFF)
    pop     eax                             ; retrieve zone size (minus large block)
    bsr     ecx, eax
;        .if (!zero?)
    jz      %%AEUS_17
    push    edx                             ; save upper mask, make room to calc new base
    ; set vMTRR[x] for Smaller block, if it exists
    xor     ebx, ebx
    dec     ebx                             ; ebx = all ones
    btr     ebx, ecx
    inc     ebx                             ; ebx = MTRR mask ( e.g 0xFFF80000)
    movd    eax, mm4                        ; cache zone base
    and     eax, ebx                        ; use mask to align base
    xor     edx, edx
    bts     edx, ecx                        ; edx = block size
    add     edx, eax                        ; add block size to base - for next block's base
    movd    mm4, edx                        ; update stored base value
    mov     al,  MTRR_TYPE_WP
    mov     ecx, edi                        ; use vMTRR pair # which is found above
    add     edi, 2                          ; point to the next vMTRR
    xor     edx, edx                        ; clear upper base
    wrmsr                                   ; set the vMTRR[6] Base
    mov     eax, ebx                        ; now build the mask
    pop     edx                             ; retrieve upper mask value
    bts     eax, VMTRR_VALID
    inc     ecx
    wrmsr                                   ; set the vMTRR[6] Mask + Valid
;        .endif                                  ; Any remaining size is abandoned. We can only use 2 vMTRRs
%%AEUS_17:                                      ; Any remaining size is abandoned. We can only use 2 vMTRRs
    pop     ecx                             ; retrieve size of larger block
    push    edx                             ; save upper mask value
    ; set vMTRR[x + 1] for Larger block, if it exists
    cmp     edi, AMD_MTRR_VARIABLE_BASE7 
    ja      %%AmdSkipvMtrrSetting           ; There's no enough vMTRR register pairs for ROM cache

    xor     ebx, ebx
    dec     ebx                             ; ebx=all ones
    btr     ebx, ecx
    inc     ebx                             ; ebx = MTRR mask ( e.g 0xFFF00000)
    movd    eax, mm4                        ; cache zone base
    and     eax, ebx                        ; use mask to align base
    xor     edx, edx                        ; clear upper base
    mov     al,  MTRR_TYPE_WP
    mov     ecx, edi
    wrmsr                                   ; set the vMTRR[7] Base
    mov     eax, ebx                        ; now build the mask
    bts     eax, VMTRR_VALID
    pop     edx                             ; retrieve upper mask value
    inc     ecx
    wrmsr                                   ; set the vMTRR[7] Mask + Valid
;    .endif
%%AEUS_12:

    ; prepare to exit
%%AmdSkipvMtrrSetting:
    mov     edi, ebp                    ; place stack frame pointer for return
    movd    ebp, mm1                    ; Restore saved user requested register
    movd    ebx, mm0                    ;   and the return address

    pop     ecx
    pop     edx
    mov     eax, AGESA_SUCCESS
%%AmdEnableUefiStackAbort:
%endmacro


;======================================================================
; AMD_DISABLE_UEFI_STACK2:  Dismantle the pre-memory cache-as-RAM mode.
;
;   In:
;       EBX  = Return address (preserved)
;
;   Out:
;       EAX = AGESA_SUCCESS
;
;   Description:
;       It is expected that the UEFI PEI core has reloacted the stack to main
;       RAM by this time and the MTRR map has been sync'd. Therefore, this
;       routine will not modify the MTRR settings; but rather, just disable
;       the CAR mode. Cache tags will be invalidated.
;
;   Preserved:
;       ebx, esp
;   Destroyed:
;       eax, ebx, ecx, edx, esi, ebp
;======================================================================
%macro AMD_DISABLE_UEFI_STACK2 0
    mov     ebp, ebx                    ; Save return address

    ; get node/core/flags of current executing core
    GET_NODE_ID_CORE_ID                 ; Sets ESI[15,8]= Node#; ESI[7,0]= core# (relative to node); flags

    AMD_DISABLE_STACK_FAMILY_HOOK       ; Re-Enable 'normal' cache operations

    mov     ebx, ebp                    ; restore return address (ebx)
    xor     eax, AGESA_SUCCESS
%endmacro

;======================================================================
; IS_BSC:  Determine if this is Boot Strap Core
;
;   In:
;       NULL
;
;   Out:
;       CF = 1, it's BSC
;       CF = 0, it's AP
;
;   Destroyed:
;       CF
;======================================================================
%macro IS_BSC 0
    pushad
    mov     ecx, APIC_BASE_ADDRESS      ; MSR:0000_001B
    rdmsr
    bt      eax, APIC_BSC               ; Is this the BSC?
    popad
%endmacro

