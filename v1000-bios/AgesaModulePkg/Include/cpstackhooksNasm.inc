;****************************************************************************
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;****************************************************************************
;*****************************************************************************
; AMD Generic Encapsulated Software Architecture
;
; $Workfile:: cpstackhooksNasm.inc
; $Revision$    $Date$
;
; Description: cpstackhooksNasm.inc - AGESA stack setup family hooks
;

%include "NasmBase.inc"

%define STACK_SIZE_1M            100000h     ; 1MB
%define STACK_SIZE_256K          40000h      ; 256KB
%define STACK_SIZE_192K          30000h      ; 192KB
%define STACK_SIZE_128K          20000h      ; 128KB
%define STACK_SIZE_64K           10000h      ; 64KB
%define STACK_SIZE_32K           8000h       ; 32KB
%define STACK_SIZE_16K           4000h       ; 16KB
%define STACK_SIZE_4K            1000h       ; 4KB

%define CORE0_STACK_SIZE         STACK_SIZE_16K   ; 16KB for primary cores
%define CORE1_STACK_SIZE         STACK_SIZE_4K    ; 4KB for each AP cores

%define BSP_STACK_BASE_ADDR      30000h      ; Base address for core 0 stack
%define BSP_STACK_BASE_ADDR_4M   400000h     ; Base address at 4MB
%define CORE0_STACK_BASE_ADDR    80000h      ; Base address for primary cores stack
;CORE1_STACK_BASE_ADDR = BSP_STACK_BASE_ADDR + BSP_STACK_SIZE

%define BSP_CACHE_TYPE_POSITION  3

;============================================================================
;
; Define a  macro that allow the exclusion of processor families from
; the cache-as-ram code. This will reduce the size of the assembled file.
;
;============================================================================

;---------------------------------------------------
;
; AMD_ENABLE_STACK_FAMILY_HOOK Macro - Stackless
;
;   Set any family specific controls needed to enable the use of
;   cache as general storage before main memory is available.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;---------------------------------------------------
%macro AMD_ENABLE_STACK_FAMILY_HOOK 0

    AMD_ENABLE_STACK_FAMILY_HOOK_F15
    AMD_ENABLE_STACK_FAMILY_HOOK_F17
    AMD_ENABLE_STACK_FAMILY_HOOK_F19

%endmacro

;----------------------------------------------
;
; AMD_DISABLE_STACK_FAMILY_HOOK Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;----------------------------------------------
%macro AMD_DISABLE_STACK_FAMILY_HOOK 0

    AMD_DISABLE_STACK_FAMILY_HOOK_F15
    AMD_DISABLE_STACK_FAMILY_HOOK_F17
    AMD_DISABLE_STACK_FAMILY_HOOK_F19

%endmacro

;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID Macro - Stackless
;
;   Read family specific values to determine the node and core
;   numbers for the core executing this code.
;
; Inputs:
;     none
; Outputs:
;     SI[7:0] = Core# (0..N, relative to node)
;     SI[15:8]= Node# (0..N)
;     SI[23:16]= reserved
;     SI[24]=   flag: 1=Family Unrecognized
;     SI[25]=   flag: 1=Interface re-entry call
;     SI[26]=   flag: 1=Core is primary of compute unit
;     SI[31:27]= reserved, =0
;
; Destroyed:
;       eax, ebx, ecx, edx, esi
;---------------------------------------------------
%macro GET_NODE_ID_CORE_ID 0

    mov     si, -1
    GET_NODE_ID_CORE_ID_F15
    GET_NODE_ID_CORE_ID_F17
    GET_NODE_ID_CORE_ID_F19

    ;
    ; Check for unrecognized Family
    ;
    _if si, e, -1                      ; Has family (node/core) been discovered?
        mov     esi, ( (1 << FLAG_UNKNOWN_FAMILY)+(1 << FLAG_IS_PRIMARY) ) ; No, Set error code, Only let BSP continue
        mov     ecx, APIC_BASE_ADDRESS  ; MSR:0000_001B
        rdmsr
        bt      eax, APIC_BSC           ;   Is this the BSC?
        _if ncarry
            ; No, this is an AP
            hlt                         ;       Kill APs
        _endif
    _endif
%endmacro

;;***************************************************************************
;;                      Family 15h MACROS
;;***************************************************************************
;---------------------------------------------------
;
; AMD_ENABLE_STACK_FAMILY_HOOK_F15 Macro - Stackless
;
;   Set any family specific controls needed to enable the use of
;   cache as general storage before main memory is available.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 15h CZ requirements:
;   * Paging must be disabled.
;   * MSRC001_0015[INVDWBINVD]=0
;   * MSRC001_101C[DisSS]=1
;   * MSRC001_1021[DisSpecTlb]=1
;   * MSRC001_101C[DisSpecTlb]=1
;   * MSRC001_101C[DisHwPf]=1
;   * No INVD or WBINVD, no exceptions, page faults or interrupts
;---------------------------------------------------
%macro AMD_ENABLE_STACK_FAMILY_HOOK_F15 0

    AMD_CPUID   CPUID_MODEL
    mov     ebx, eax                    ; Save revision info to EBX
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 06h                     ; Is this family 15h?
    jnz     %%fam15_enable_stack_hook_exit ; Br if no

    ; Set node to map the first 16MB to node 0; 0000_0000 to 00FF_FFFF as DRAM
    mov     ebx, esi                    ; Get my Node/Core info
    xor     bl, bl
    shl     bh, 3                       ; Isolate my node#, match alignment for PCI Dev#
    mov     eax, 8000C144h              ; D18F1x44:DRAM Base/Limit; N is Base, N+4 is Limit
    add     ah, bh
    mov     ebx, eax                    ; Save PCI address for Base/Limit pair

    mov     dx, 0CF8h
    out     dx, eax
    add     dx, 4
    xor     eax, eax                    ; Least Significant bit is AD24 so 0 sets mask of 00FF_FFFF (16MB)
    out     dx, eax                     ; DRAM Limit = node0, no interleave

    mov     eax, ebx
    sub     eax, 4                      ; Now point to the Base register
    mov     dx, 0CF8h
    out     dx, eax
    add     dx, 4
    mov     eax, 00000003h              ; Set the read and write enable bits
    out     dx, eax                     ; DRAM Base = 0x0000, R/W

    bt      esi, FLAG_STACK_REENTRY     ; Check if stack has already been set
    _if ncarry
        mov     ecx, HWCR               ; MSR C001_0015
        rdmsr
        btr     eax, INVD_WBINVD        ; disable INVD -> WBINVD conversion
        wrmsr
    _endif

    mov     ecx, LS_CFG3                ; MSR:C001_101C
    rdmsr
    bts     eax, DIS_SS_F15CZ           ; Turn on Streaming store functionality disabled bit
    wrmsr

    mov     ecx, IC_CFG                 ; MSR:C001_1021
    rdmsr
    bts     eax, IC_DIS_SPEC_TLB_RLD    ; Turn on Disable speculative IC-TLB reloads bit
    wrmsr

    mov     ecx, LS_CFG3                ; MSR:C001_101C
    rdmsr
    bts     eax, DC_DIS_SPEC_TLB_RLD_F15CZ  ; Turn on Disable speculative DC-TLB reloads bit
    bts     eax, DC_DIS_HW_PF_F15CZ         ; Turn on Disable hardware prefetches bit
    wrmsr

    ; Do CZ enable stack special
    mov     ecx, CU_CFG             ; MSR:C001_1023
    rdmsr
    bt      eax, L2_WAY_LOCK_EN     ; Check if way 15 of the L2 needs to be reserved
    _if ncarry
        bts     eax, L2_WAY_LOCK_EN
        or      eax, L2_FIRST_LOCKED_WAY_OR_MASK
        wrmsr
    _endif

    ; Do Standard Family 15 work
    mov     ecx, CU_CFG3                ; MSR:C001_102B
    rdmsr
    btr     edx, (COMBINE_CR0_CD - 32)  ; Clear CombineCr0Cd bit
    bts     eax, 18                     ; Set PfcDis bit
    wrmsr

    bt      esi, FLAG_STACK_REENTRY     ; Check if stack has already been set
    _if ncarry
        bt      esi, FLAG_IS_PRIMARY    ;   Is this core the primary in a compute unit?
        _if carry                       ;     Families using shared groups do not need to clear the MTRRs since that is done at power-on reset
            ; Set TOP_MEM (C001_001A) for non-shared cores to 16M. This will be increased at heap init.
            ;  - not strictly needed since the FixedMTRRs take presedence.
            mov     eax, (16 * 1024 * 1024)
            xor     edx, edx
            mov     ecx, TOP_MEM                ; MSR:C001_001A
            wrmsr
        _endif
    _endif

    xor     eax, eax
    xor     edx, edx
    mov     ecx, TOP_MEM2               ; MSR:C001_001D
    wrmsr

%%fam15_enable_stack_hook_exit:
%endmacro


;----------------------------------------------
;
; AMD_DISABLE_STACK_FAMILY_HOOK_F15 Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - [31:24] flags; [15:8]= Node#; [7:0]= core#
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 15h CZ requirements:
;   * INVD or WBINVD
;   * MSRC001_0015[INVD_WBINVD]=1
;   * MSRC001_101C[DisSS]=0
;   * MSRC001_1021[DisSpecTlb]=0
;   * MSRC001_101C[DisSpecTlb]=0
;   * MSRC001_101C[DisHwPf]=0
;---------------------------------------------------
%macro AMD_DISABLE_STACK_FAMILY_HOOK_F15 0

    AMD_CPUID   CPUID_MODEL
    mov     ebx, eax                        ; Save revision info to EBX
    shr     eax, 20                         ; AL = cpu extended family
    cmp     al, 06h                         ; Is this family 15h?
    jnz     %%fam15_disable_stack_hook_exit ; Br if no

    mov     edi, ebx                        ; Save revision info to EDI
    AMD_CPUID   AMD_CPUID_APIC
    mov     al, cl                          ; AL = number of cores - 1
    shr     cx, APIC_ID_CORE_ID_SIZE        ; CL = ApicIdCoreIdSize
    mov     bx, 1
    shl     bl, cl                          ; BL = theoretical number of cores on socket
    dec     bx                              ; BL = core number on socket mask
    mov     ah, bl                          ; AH = core number on socket mask
    mov     ebx, edi                        ; Restore revision info to EBX
    mov     di, ax                          ; DI[15:8] = core number mask, DI[7:0] = number of cores - 1

    and     ebx, 0F00FFh
    mov     eax, ebx
    shr     eax, 8
    or      bx, ax                          ; Save Extended Model, Model and Stepping to BX
                                            ; [11:8] = Extended Model, [7:4] = Model, [3:0] = Stepping

    ;; A handshake is required to ensure that all cores on a node invalidate in sync.
    ;    Entry:
    ;      BX: [11:8] = Extended Model,   [7:4] = Model, [3:0] = Stepping
    ;      DI: [15:8] = core number mask, [7:0] = number of cores - 1
    HANDSHAKE_F15

    ;    Exit:
    ;     BX:
    ;       HANDSHAKE_F15      - [11:8] = Extended Model,   [7:4] = Model, [3:0] = Stepping
    ;       HANDSHAKE_F15_PCI  - [7:0]  = Core ID
    ;     EDI: CU flag
    ;       bit[31] - AMD_CU_NEED_TO_WAIT
    ;       bit[30] - AMD_CU_SEND_INVD_MSG
    ;       bit[29] - AMD_CU_RESTORE_ES

    ;; Handshaking complete.  Continue tearing down CAR.
    mov     ecx, LS_CFG3                    ; MSR:C001_101C
    rdmsr
    btr     eax, DIS_SS_F15CZ               ; Turn on Streaming store functionality
    wrmsr

    mov     ecx, IC_CFG                     ; MSR:C001_1021
    rdmsr
    btr     eax, IC_DIS_SPEC_TLB_RLD        ; Turn on speculative TLB reloads
    wrmsr

    mov     ecx, LS_CFG3                    ; MSR:C001_101C
    rdmsr
    btr     eax, DC_DIS_SPEC_TLB_RLD_F15CZ  ; Turn on speculative TLB reloads
    btr     eax, DC_DIS_HW_PF_F15CZ         ; Turn on hardware prefetches
    wrmsr

    mov     ecx, HWCR                       ; MSR:C001_0015h
    rdmsr
    btr     eax, INVD_WBINVD                ; Disable INVD -> WBINVD conversion
    wrmsr
    invd                                    ; Clear the cache tag RAMs

    ; Do CZ disable stack special
    mov     ecx, CU_CFG                     ; MSR:C001_1023
    rdmsr
    shr      eax, L2_FIRST_LOCKED_WAY
    and      eax, 01Fh
    _if eax, e, 01Fh                        ; Check if way 15 of the L2 needs to be reserved
        rdmsr                               ; ECX = CU_CFG, MSR:C001_1023
        btr     eax, L2_WAY_LOCK_EN
        wrmsr
    _endif

    ; Do Standard Family 15 work
    mov     ecx, HWCR                       ; MSR:C001_0015h
    rdmsr
    bts     eax, INVD_WBINVD                ; Turn on INVD -> WBINVD conversion
    wrmsr

    mov     ecx, CU_CFG3                    ; MSR:C001_102B
    rdmsr
    bts     edx, (COMBINE_CR0_CD - 32)      ; Set CombineCr0Cd bit
    btr     eax, 18                         ; Clear PfcDis bit
    wrmsr

    bt      edi, AMD_CU_SEND_INVD_MSG
    _if carry
        ;; Non core zero needs to signal to core 0 to proceed
        SIGNAL_OTHER_CORE_F15
    _endif

%%fam15_disable_stack_hook_exit:
%endmacro

;---------------------------------------------------
;
; HANDSHAKE_F15 Macro - Stackless
;
;   A handshake is required to ensure that all cores
;   on a node invalidate in sync.
;
; Inputs:
;     BX:  [11:8] = Extended Model,   [7:4] = Model, [3:0] = Stepping
;     DI:  [15:8] = core number mask, [7:0] = number of cores - 1
; Outputs:
;     BX:  [11:8] = Extended Model,   [7:4] = Model, [3:0] = Stepping
;     EDI: CU flag
;       bit[31] - AMD_CU_NEED_TO_WAIT
;       bit[30] - AMD_CU_SEND_INVD_MSG
;       bit[29] - AMD_CU_RESTORE_ES
; Destroyed:
;       eax, ebx, ecx, edx
;---------------------------------------------------
%macro HANDSHAKE_F15 0

    mov     ecx, APIC_BASE_ADDRESS
    rdmsr
    mov     dx, bx                      ; Save Extended Model, Model and Stepping to DX
    shl     edx, 16                     ; EDX[31:16] = Extended Model, Model and Stepping
    mov     ebx, eax                    ; EBX = LAPIC base
    xor     ecx, ecx                    ; Zero out CU flags
    bts     ecx, AMD_CU_NEED_TO_WAIT    ; Default to waiting
    bts     ecx, AMD_CU_SEND_INVD_MSG   ; Default to signaling
    mov     eax, CR0
    bt      ax, CR0_PE                  ; Are we in protected mode?
    _if ncarry
        bts     ecx, AMD_CU_RESTORE_ES  ; Indicate ES restore is required
        mov     cx, es                  ; Save ES segment register to CX
        xor     ax, ax
        mov     es, ax                  ; Set ES to big real mode selector for 4GB access
    _endif

    and     bx, 0F000h                  ; EBX = LAPIC base, offset 0
    or      bl, APIC_ID_REG
    mov     eax, [es:ebx]               ; EAX[31:24] = APIC ID
    shr     eax, APIC20_APICID          ; AL = APIC ID
    mov     ah, al                      ; AH = APIC ID
    mov     dx, di                      ; DH = core mask
    and     ah, dh                      ; AH = core number
    _if zero
        ;; Core 0 of a socket
        btr     ecx, AMD_CU_SEND_INVD_MSG ; No need to signal after INVD
        _if dl, ne, 0
            ;; This socket has multiple cores
            and     bx, 0F000h          ; EBX = LAPIC base, offset 0
            or      bx, APIC_MSG_REG
            mov     edi, APIC_MSG
            mov     [es:ebx], edi       ; Signal for non core 0s to complete CAR breakdown
        _else
            btr     ecx, AMD_CU_NEED_TO_WAIT ; No need to wait on a single core CPU
        _endif
    _endif

    bt     ecx, AMD_CU_NEED_TO_WAIT
    _if carry
        _if ah, e, dl
            ;; This is the highest numbered core on this socket -- wait on core 0
            not     dh                  ; Flip the mask to determine local core 0's APIC ID
            and     al, dh              ; AL = target APIC ID
        _else
            ;; All other cores (including core 0) wait on the next highest core.
            ;; In this way, cores will halt in a cascading fashion down to 0.
            inc     al
        _endif

        shl     eax, APIC20_APICID
        and     bx, 0F000h
        or      bx, APIC_CMD_HI_REG
        mov     [es:ebx], eax           ; Set target APIC ID

        ;; Use bits 23:16 as a timeout for unresponsive cores
        ror     ecx, 8
        mov     ch, 0FFh
        stc
        _while carry
            and     bx, 0F000h          ; EBX = LAPIC base, offset 0
            or      bx, APIC_CMD_LO_REG
            mov     eax, CMD_REG_TO_READ_DATA
            mov     [es:ebx], eax       ; Fire remote read IPI
            inc     ch                  ; Pre increment the timeout
            stc
            _while carry
                dec     ch              ; Check the timeout
                jz      %%fam15_disable_stack_remote_read_exit   ; Branch if there is an unresponsive core
                mov     eax, [es:ebx]
                bt      eax, DELIVERY_STS_BIT
            _endw
            stc
            _while carry
                mov     eax, [es:ebx]
                and     eax, REMOTE_READ_STS
                _if eax, e, REMOTE_DELIVERY_PEND
                    dec     ch          ; Check the timeout
                    jz      %%fam15_disable_stack_remote_read_exit   ; Branch if there is an unresponsive core
                    stc
                _else
                    clc
                _endif
            _endw
            _if eax, e, REMOTE_DELIVERY_DONE
                and     bx, 0F000h      ; EBX = LAPIC base, offset 0
                or      bl, APIC_REMOTE_READ_REG
                mov     eax, [es:ebx]
                _if eax, e, APIC_MSG
                    clc
                _else
                    stc
                _endif
            _else
                dec     ch              ; Check the timeout
                jz      %%fam15_disable_stack_remote_read_exit   ; Branch if there is an unresponsive core
                stc
            _endif
        _endw

%%fam15_disable_stack_remote_read_exit:
        rol     ecx, 8                  ; Restore ECX

    _endif
    bt      ecx, AMD_CU_RESTORE_ES
    _if carry
        mov     es, cx
    _endif
    mov     edi, ecx                    ; EDI = CU flags
    shr     edx, 16
    mov     bx,  dx
%endmacro

;---------------------------------------------------
;
; SIGNAL_OTHER_CORE_F15 Macro - Stackless
;
;   Non core zero needs to signal to other cores to proceed
;
; Inputs:
;     BX:  Core ID
;     DI:  [15:8] = core number mask, [7:0] = number of cores - 1
; Outputs:

; Destroyed:
;       eax, ebx, ecx, edx
;---------------------------------------------------
%macro SIGNAL_OTHER_CORE_F15 0
    mov     ecx, APIC_BASE_ADDRESS
    rdmsr
    mov     ebx, eax                ; EBX = LAPIC base
    and     bx, 0F000h              ; EBX = LAPIC base, offset 0
    or      bx, APIC_MSG_REG
    mov     eax, APIC_MSG
    mov     [es:ebx], eax           ; Signal for core 0 to complete CAR breakdown
%endmacro

;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID_F15 Macro - Stackless
;
;   Read family specific values to determine the node and core
;   numbers for the core executing this code.
;
; Inputs:
;     none
; Outputs:
;     ESI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
;     MM5 = 32b pointer to family info structure
; Destroyed:
;       eax, ebx, ecx, edx, esi, mm5
;---------------------------------------------------
%macro GET_NODE_ID_CORE_ID_F15 0

        jmp     %%end_of_f15h_data
        ; Family 15h Info Structure:          L2Size,  #SharedCores, AllocMem, AllocExe, SzAddrBus, pad
        %%FAM15H_INFO_STRUCT istruc CPU_FAMILY_INFO
        at  CPU_FAMILY_INFO.L2_MIN_SIZE,      dw 512
        at  CPU_FAMILY_INFO.NUM_SHARED_CORES, db 2
        at  CPU_FAMILY_INFO.L2_ALLOC_MEM,     db 0
        at  CPU_FAMILY_INFO.L2_ALLOC_EXE,     dw 0
        at  CPU_FAMILY_INFO.SIZE_ADDRESS_BUS, db 48
        at  CPU_FAMILY_INFO.FAMILY_RESERVED,  db 0
        iend
%%end_of_f15h_data:

    cmp     si, -1                      ; Has node/core already been discovered?
    jnz     %%node_core_f15_exit        ; Br if yes

    AMD_CPUID   CPUID_MODEL
    shr     eax, 12                     ; AH = cpu extended family
    cmp     ah, 06h                     ; Is this family 15h?
    jnz     %%node_core_f15_exit        ; Br if no
    shr     al, 4                       ; AL = cpu extended model
    shr     ebx, 16                     ; BH = LocalApicId
    mov     bl, al                      ; BL = cpu extended model

    LoadTableAddress %%FAM15H_INFO_STRUCT
    movd    mm5, eax                    ; load pointer to Family Info Struc

    xor     esi, esi                    ; Assume BSC, clear local flags
    mov     ecx, APIC_BASE_ADDRESS      ; MSR:0000_001B
    rdmsr
    bt      eax, APIC_BSC               ; Is this the BSC?
    _if carry
        ; This is the BSP.
        ; Enable routing tables on BSP (just in case the topology init code has not yet enabled them)
        mov     eax, 8000C06Ch          ;   PCI address for D18F0x6C Link Initialization Control Register
        mov     dx, 0CF8h
        out     dx, eax
        add     dx, 4
        in      eax, dx
        btr     eax, 0                  ;   Set LinkInitializationControl[RouteTblDis] = 0
        out     dx, eax
    _else                               ;
        mov     al, bl                  ;   AL = cpu extended model
        shr     bx, 8                   ;   BL = CPUID Fn0000_0001_EBX[LocalApicId]
        mov si, bx                      ;   SI = [15:8]= Node# = 0; [7:0]= core#
    _endif                              ; end

    ;
    ;   determine if this core shares MTRRs
    ;
    mov     eax, 8000C580h              ; Compute Unit Status
    mov     bx, si                      ; load node#(bh), core#(bl)
    shl     bh, 3                       ; Move node# to PCI Dev# field
    add     ah, bh                      ; Adjust PCI address for node number
    mov     dx, 0CF8h
    out     dx, eax
    add     dx, 4
    in      eax, dx                     ; [3:0]=Enabled; [19:16]=DualCore

                                        ; BL is MyCore#  , BH is primary flag
    mov     cx, 08h                     ; Use CH as 'first of pair' core#
    jmp     %%WhileCheck
%%StartOfWhile:
        bt      eax, 0                  ;   Is pair enabled?
        _if carry                       ;
            mov     bh, 01h             ;   flag core as primary
            bt      eax, 16             ;   Is there a 2nd in the pair?
            _if carry                   ;
                cmp     ch, bl          ;     Does 1st match MyCore#?
                je      %%EndOfWhile
                inc     ch
                xor     bh, bh          ;     flag core as NOT primary
                cmp     ch, bl          ;     Does 2nd match MyCore#?
                je      %%EndOfWhile
            _else                       ;   No 2nd core
                cmp     ch, bl          ;     Does 1st match MyCore#?
                je      %%EndOfWhile
            _endif
            inc     ch
        _endif
        shr     eax, 1
        dec     cl
%%WhileCheck:
    cmp     cl, 0
    ja      %%StartOfWhile
%%EndOfWhile:
    _if cl, e, 0
        ;Error - core# didn't match Compute Unit Status content
        bts     esi, FLAG_CORE_NOT_IDENTIFIED
        bts     esi, FLAG_IS_PRIMARY    ;   Set Is_Primary for unknowns
    _endif
    _if bh, ne, 0                       ; Check state of primary for the matched core
        bts     esi, FLAG_IS_PRIMARY    ;   Set shared flag into return value
    _endif
%%node_core_f15_exit:
%endmacro

;;***************************************************************************
;;                      Family 17h MACROS
;;***************************************************************************
;---------------------------------------------------
;
; AMD_ENABLE_STACK_FAMILY_HOOK_F17 Macro - Stackless
;
;   Set any family specific controls needed to enable the use of
;   cache as general storage before main memory is available.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 15h CZ requirements:
;   * Paging must be disabled.
;   * MSRC001_0015[INVDWBINVD]=0
;   * MSRC001_101C[DisSS]=1
;   * MSRC001_1021[DisSpecTlb]=1
;   * MSRC001_101C[DisSpecTlb]=1
;   * MSRC001_101C[DisHwPf]=1
;   * No INVD or WBINVD, no exceptions, page faults or interrupts
;---------------------------------------------------
%macro AMD_ENABLE_STACK_FAMILY_HOOK_F17 0

    AMD_CPUID   CPUID_MODEL
    mov     ebx, eax                       ; Save revision info to EBX
    shr     eax, 20                        ; AL = cpu extended family
    cmp     al, 08h                        ; Is this family 17h?
    jnz     %%fam17_enable_stack_hook_exit ; Br if no

    ; Set TOP_MEM (C001_001A) for non-shared cores to 16M. This will be increased at heap init.
    ;  - not strictly needed since the FixedMTRRs take presedence.
    mov     ecx, TOP_MEM                   ; MSR:C001_001A
    rdmsr
    test    eax, eax
    _if zero
        dec     eax
        wrmsr
    _endif

%%fam17_enable_stack_hook_exit:
%endmacro


;----------------------------------------------
;
; AMD_DISABLE_STACK_FAMILY_HOOK_F17 Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - [31:24] flags; [15:8]= Node#; [7:0]= core#
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 15h CZ requirements:
;   * INVD or WBINVD
;   * MSRC001_0015[INVD_WBINVD]=1
;   * MSRC001_101C[DisSS]=0
;   * MSRC001_1021[DisSpecTlb]=0
;   * MSRC001_101C[DisSpecTlb]=0
;   * MSRC001_101C[DisHwPf]=0
;---------------------------------------------------
%macro AMD_DISABLE_STACK_FAMILY_HOOK_F17 0

    AMD_CPUID   CPUID_MODEL
    mov     ebx, eax                        ; Save revision info to EBX
    shr     eax, 20                         ; AL = cpu extended family
    cmp     al, 08h                         ; Is this family 17h?
    jnz     %%fam17_disable_stack_hook_exit ; Br if no

%%fam17_disable_stack_hook_exit:
%endmacro

;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID_F17 Macro - Stackless
;
;   Read family specific values to determine the node and core
;   numbers for the core executing this code.
;
; Inputs:
;     none
; Outputs:
;     ESI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
;     MM5 = 32b pointer to family info structure
; Destroyed:
;       eax, ebx, ecx, edx, esi, mm5
;---------------------------------------------------
%macro GET_NODE_ID_CORE_ID_F17 0

        jmp     %%end_of_f17h_data
        %%FAM17H_INFO_STRUCT istruc CPU_FAMILY_INFO
        at  CPU_FAMILY_INFO.L2_MIN_SIZE,      dw 2048
        at  CPU_FAMILY_INFO.NUM_SHARED_CORES, db 2
        at  CPU_FAMILY_INFO.L2_ALLOC_MEM,     db 0
        at  CPU_FAMILY_INFO.L2_ALLOC_EXE,     dw 0
        at  CPU_FAMILY_INFO.SIZE_ADDRESS_BUS, db 48
        at  CPU_FAMILY_INFO.FAMILY_RESERVED,  db 0
        iend
%%end_of_f17h_data:

    cmp     si, -1                      ; Has node/core already been discovered?
    jnz     %%node_core_f17_exit        ; Br if yes

    AMD_CPUID   CPUID_MODEL
    shr     eax, 12                     ; AH = cpu extended family
    cmp     ah, 08h                     ; Is this family 17h?
    jnz     %%node_core_f17_exit        ; Br if no
    shr     al, 4                       ; AL = cpu extended model
    shr     ebx, 16                     ; BH = LocalApicId
    mov     bl, al                      ; BL = cpu extended model

    LoadTableAddress %%FAM17H_INFO_STRUCT
    movd    mm5, eax                    ; load pointer to Family Info Struc

    xor     esi, esi                    ; Assume BSC, clear local flags
    mov     ecx, APIC_BASE_ADDRESS      ; MSR:0000_001B
    rdmsr
    bt      eax, APIC_BSC               ; Is this the BSC?
    _if ncarry
        shr     bx, 4
        shr     bl, 4
        mov     si, bx                  ;   SI = [15:8]= Node# = 0; [7:0]= core#
    _endif                              ; end

    ;
    ;   determine if this core shares MTRRs
    ;

    AMD_CPUID   AMD_CPUID_EXT_APIC
    _if bh, ne, 0
        bt     si, 0
        _if ncarry
            bts     esi, FLAG_IS_PRIMARY    ;   Set shared flag into return value
        _endif
    _else
        bts     esi, FLAG_IS_PRIMARY    ;   Set shared flag into return value
    _endif

    bts    esi, FLAG_DRAM_AVAILABLE
%%node_core_f17_exit:
%endmacro

;;***************************************************************************
;;                      Family 19h MACROS
;;***************************************************************************
;---------------------------------------------------
;
; AMD_ENABLE_STACK_FAMILY_HOOK_F19 Macro - Stackless
;
;   Set any family specific controls needed to enable the use of
;   cache as general storage before main memory is available.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;---------------------------------------------------
%macro AMD_ENABLE_STACK_FAMILY_HOOK_F19 0

    AMD_CPUID   CPUID_MODEL
    mov     ebx, eax                       ; Save revision info to EBX
    shr     eax, 20                        ; AL = cpu extended family
    cmp     al, 0Ah                        ; Is this family 19h?
    jnz     %%fam19_enable_stack_hook_exit ; Br if no

    ; Set TOP_MEM (C001_001A) for non-shared cores to 16M. This will be increased at heap init.
    ;  - not strictly needed since the FixedMTRRs take presedence.
    mov     ecx, TOP_MEM                   ; MSR:C001_001A
    rdmsr
    test    eax, eax
    _if zero
        dec     eax
        wrmsr
    _endif

%%fam19_enable_stack_hook_exit:
%endmacro


;----------------------------------------------
;
; AMD_DISABLE_STACK_FAMILY_HOOK_F19 Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - [31:24] flags; [15:8]= Node#; [7:0]= core#
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;---------------------------------------------------
%macro AMD_DISABLE_STACK_FAMILY_HOOK_F19 0

    AMD_CPUID   CPUID_MODEL
    mov     ebx, eax                        ; Save revision info to EBX
    shr     eax, 20                         ; AL = cpu extended family
    cmp     al, 0Ah                         ; Is this family 19h?
    jnz     %%fam19_disable_stack_hook_exit ; Br if no

%%fam19_disable_stack_hook_exit:
%endmacro

;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID_F19 Macro - Stackless
;
;   Read family specific values to determine the node and core
;   numbers for the core executing this code.
;
; Inputs:
;     none
; Outputs:
;     ESI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
;     MM5 = 32b pointer to family info structure
; Destroyed:
;       eax, ebx, ecx, edx, esi, mm5
;---------------------------------------------------
%macro GET_NODE_ID_CORE_ID_F19 0

        jmp     %%end_of_f19h_data
        ; Family 19h Info Structure:          L2Size,  #SharedCores, AllocMem, AllocExe, SzAddrBus, pad
        %%FAM19H_INFO_STRUCT istruc CPU_FAMILY_INFO
        at  CPU_FAMILY_INFO.L2_MIN_SIZE,      dw 2048
        at  CPU_FAMILY_INFO.NUM_SHARED_CORES, db 2
        at  CPU_FAMILY_INFO.L2_ALLOC_MEM,     db 0
        at  CPU_FAMILY_INFO.L2_ALLOC_EXE,     dw 0
        at  CPU_FAMILY_INFO.SIZE_ADDRESS_BUS, db 48
        at  CPU_FAMILY_INFO.FAMILY_RESERVED,  db 0
        iend
%%end_of_f19h_data:
        jmp     %%end_of_f19h_zen4_data
        ; Family 19h Info Structure:              L2Size,  #SharedCores, AllocMem, AllocExe, SzAddrBus, pad
        %%FAM19H_ZEN4_INFO_STRUCT istruc CPU_FAMILY_INFO
        at  CPU_FAMILY_INFO.L2_MIN_SIZE,      dw 2048
        at  CPU_FAMILY_INFO.NUM_SHARED_CORES, db 2
        at  CPU_FAMILY_INFO.L2_ALLOC_MEM,     db 0
        at  CPU_FAMILY_INFO.L2_ALLOC_EXE,     dw 0
        at  CPU_FAMILY_INFO.SIZE_ADDRESS_BUS, db 52
        at  CPU_FAMILY_INFO.FAMILY_RESERVED,  db 0
        iend
%%end_of_f19h_zen4_data:

    cmp     si, -1                      ; Has node/core already been discovered?
    jnz     %%node_core_f19_exit        ; Br if yes

    AMD_CPUID   CPUID_MODEL
    shr     ebx, 16                     ; BH = LocalApicId
    mov      bl, al
    shr      bl, 4                      ; BL = cpu basic model
    shr     eax, 12                     ; AH = cpu extended family
    cmp     ah, 0Ah                     ; Is this family 19h?
    jnz     %%node_core_f19_exit        ; Br if no
    or      bl, al                      ; BL = cpu model

    cmp    bl, 010h                     ; Is this RS
    jz     %%load_family_19_zen4_info   ; Br if yes
    cmp    bl, 011h                     ; Is this RS B0
    jz     %%load_family_19_zen4_info   ; Br if yes
    cmp    bl, 018h                     ; Is this STP
    jz     %%load_family_19_zen4_info   ; Br if yes
    cmp    bl, 0A0h                     ; Is this Bergamo
    jz     %%load_family_19_zen4_info   ; Br if yes
    jmp    %%load_family_19_info

%%load_family_19_zen4_info:
    mov    ecx, TW_CFG
    rdmsr
    mov    eax, edx
    shr    eax, 31
    cmp    eax, 1
    jne    %%load_family_19_info
    LoadTableAddress %%FAM19H_ZEN4_INFO_STRUCT
    jmp %%load_family_19_end
%%load_family_19_info:
    LoadTableAddress %%FAM19H_INFO_STRUCT
%%load_family_19_end:
    movd    mm5, eax                    ; load pointer to Family Info Struc

    xor     esi, esi                    ; Assume BSC, clear local flags
    mov     ecx, APIC_BASE_ADDRESS      ; MSR:0000_001B
    rdmsr
    bt      eax, APIC_BSC               ; Is this the BSC?
    _if ncarry
        shr     bx, 4
        shr     bl, 4
        mov     si, bx                  ;   SI = [15:8]= Node# = 0; [7:0]= core#
    _endif                              ; end

    ;
    ;   determine if this core shares MTRRs
    ;

    AMD_CPUID   AMD_CPUID_EXT_APIC
    _if bh, ne, 0
        bt     si, 0
        _if ncarry
            bts     esi, FLAG_IS_PRIMARY    ;   Set shared flag into return value
        _endif
    _else
        bts     esi, FLAG_IS_PRIMARY    ;   Set shared flag into return value
    _endif

    bts    esi, FLAG_DRAM_AVAILABLE
%%node_core_f19_exit:
%endmacro


