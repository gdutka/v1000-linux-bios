;*****************************************************************************
; AMD Generic Encapsulated Software Architecture
;
;  $Workfile:: mu.asm   $ $Revision:: 309#$  $Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
; Description: Main memory controller system configuration for AGESA
;
;
;*****************************************************************************
;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************
;============================================================================


    .XLIST
    .LIST

    .CODE

;----------------------------------------------------------------------------
; _MFENCE();
;
_MFENCE macro
    db  0Fh,0AEh,0F0h
    endm

;===============================================================================
;MemUFlushPattern:
;
; Flush a pattern of 72 bit times (per DQ) from cache.  This procedure is used
;to ensure cache miss on the next read training.
;
;             In: Address   - Physical address to be flushed
;                 ClCount   - number of cachelines to be flushed
;            Out:
;
;All registers preserved.
;===============================================================================
MemUFlushPattern PROC PUBLIC Address:DWORD, ClCount:WORD
        push rdi
        push rcx
        mov edi,Address
        movzx ecx,ClCount
        @@:
        _MFENCE                     ; Force strong ordering of clflush
        db  64h,0Fh,0AEh,3Fh        ; MemUClFlush fs:[edi]
        _MFENCE
        add edi,64
        loop @B
        pop rcx
        pop rdi
        ret
MemUFlushPattern ENDP

;===============================================================================
;MemUReadCachelines:
;
; Read a pattern of 72 bit times (per DQ), to test dram functionality.  The
;pattern is a stress pattern which exercises both ISI and crosstalk.  The number
;of cache lines to fill is dependent on DCT width mode and burstlength.
;
;             In: Buffer    - pointer to a buffer where read data will be stored
;                 Address   - Physical address to be read
;                 ClCount   - number of cachelines to be read
;            Out:
;
;All registers preserved.
;===============================================================================
MemUReadCachelines PROC PUBLIC
        ret
MemUReadCachelines ENDP

;===============================================================================
;MemUWriteCachelines:
;   Write a test pattern to DRAM
;
;             In: Pattern   - pointer to the write pattern
;                 Address   - Physical address to be read
;                 ClCount   - number of cachelines to be read
;            Out:
;
;All registers preserved.
;===============================================================================
MemUWriteCachelines PROC PUBLIC
        ret
MemUWriteCachelines ENDP

;===============================================================================
;AlignPointerTo16Byte:
;   Modifies BufferPtr to be 16 byte aligned
;
;             In: BufferPtrPtr - Pointer to buffer pointer
;            Out: BufferPtrPtr - Pointer to buffer pointer that has been 16 byte aligned
;
;All registers preserved.
;===============================================================================
AlignPointerTo16Byte PROC PUBLIC BufferPtrPtr:NEAR PTR DWORD
        push rdx
        push rax
        mov rdx, BufferPtrPtr
        mov rax, [rdx]
        add rax, 16
        and ax, 0FFF0h
        mov [rdx], rax
        pop rax
        pop rdx
        ret
AlignPointerTo16Byte ENDP

;===============================================================================
;MemUMFenceInstr:
;   Serialize instruction
;
;             In:
;            Out:
;
;All registers preserved.
;===============================================================================
MemUMFenceInstr PROC PUBLIC
        _MFENCE
        ret
MemUMFenceInstr ENDP

    END

