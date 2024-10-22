/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _AMD_SMM_CPU_SAVE_STATE_H_
#define _AMD_SMM_CPU_SAVE_STATE_H_

#define EFI_SMM_CPU_SAVE_STATE_PROTOCOL_GUID \
  { \
    0x21f302ad, 0x6e94, 0x471b, {0x84, 0xbc, 0xb1, 0x48, 0x0, 0x40, 0x3a, 0x1d} \
  }

typedef struct _EFI_SMM_CPU_SAVE_STATE_PROTOCOL  EFI_SMM_CPU_SAVE_STATE_PROTOCOL;

//
// SMM-Revision Identifier for AMD64 Architecture.
//
#define SMM_MIN_REV_ID_x64     0x30064

#define IA32_PG_P                   1u
#define IA32_PG_RW                  (1u << 1)
#define IA32_PG_USR                 (1u << 2)
#define IA32_PG_WT                  (1u << 3)
#define IA32_PG_CD                  (1u << 4)
#define IA32_PG_A                   (1u << 5)
#define IA32_PG_D                   (1u << 6)
#define IA32_PG_PS                  (1u << 7)
#define IA32_PG_G                   (1u << 8)
#define IA32_PG_AVL1                (7u << 9)
#define IA32_PG_PAT_2M              (1u << 12)
#define IA32_PG_PAT_4K              IA32_PG_PS
#define IA32_PG_AVL2                (0x7ffull << 52)
#define SMM_DEFAULT_SMBASE          0x30000
#define SMM_HANDLER_OFFSET          0x8000
#define SMM_PSD_OFFSET              0xFDB0
#define SMM_CPU_STATE_OFFSET        0xFE00

#define SMM_CPU_INTERVAL            SIZE_2KB

#pragma pack (1)

typedef struct {
  UINT64                            Signature;              // Offset 0x00
  UINT16                            Reserved1;              // Offset 0x08
  UINT16                            Reserved2;              // Offset 0x0A
  UINT16                            Reserved3;              // Offset 0x0C
  UINT16                            SmmCs;                  // Offset 0x0E
  UINT16                            SmmDs;                  // Offset 0x10
  UINT16                            SmmSs;                  // Offset 0x12
  UINT16                            SmmOtherSegment;        // Offset 0x14
  UINT16                            Reserved4;              // Offset 0x16
  UINT64                            Reserved5;              // Offset 0x18
  UINT64                            Reserved6;              // Offset 0x20
  UINT64                            Reserved7;              // Offset 0x28
  UINT64                            SmmGdtPtr;              // Offset 0x30
  UINT32                            SmmGdtSize;             // Offset 0x38
  UINT32                            Reserved8;              // Offset 0x3C
  UINT64                            Reserved9;              // Offset 0x40
  UINT64                            Reserved10;             // Offset 0x48
  UINT16                            Reserved11;             // Offset 0x50
  UINT16                            Reserved12;             // Offset 0x52
  UINT32                            Reserved13;             // Offset 0x54
  UINT64                            MtrrBaseMaskPtr;        // Offset 0x58
} PROCESSOR_SMM_DESCRIPTOR;

/// 
/// CPU save-state strcuture for IA32 and X64.
///
typedef struct _AMD_SMM_CPU_SAVE_STATE32 {
  UINT8   Reserved[0xF8];  // FE00h - FEF7h , Reserved, 248 Bytes, --
  UINT32  SMBASE;           // FEF8h, SMBASE, Doubleword, Read/Write
  UINT32  SMMRevId;         // FEFCh, SMM-Revision identifier, Doubleword, Read-Only
  UINT16  IORestart;        // FF00h, I/O Instruction Restart, Word, Read/Write
  UINT16  AutoHALTRestart;  // FF02h, Auti-Halt Restart, Word, Read/Write
  UINT8   Reserved1[0x84];  // FF04h - FF87h, Reserved, 132 Bytes --
  UINT32  GDTBase;          // FF88h, GDT Base, Doubleword, Read-Only
  UINT64  Reserved2;        // FF8Ch - FF93h, Quadword, --
  UINT8   Reserved3[0x10];  // FF98 - FFA7h, Reserved, 16Bytes, -- 
  UINT32  _ES;               // FFA8h, ES, Doubleword, Read-Only
  UINT32  _CS;               // FFACh, CS, Doubleword, Read-Only
  UINT32  _SS;               // FFB0h, SS, Doubleword, Read-Only
  UINT32  _DS;               // FFB4h, DS, Doubleword, Read-Only
  UINT32  _FS;               // FFB8h, FS, Doubleword, Read-Only
  UINT32  _GS;               // FFBCh, GS, Doubleword, Read-Only
  UINT32  LDTBase;          // FFC0h, LDT Base, Doubleword, Read-Only
  UINT32  _TR;              // FFC4h, TR, Doubleword, Read-Only
  UINT32  _DR7;             // FFC8h, DR7, Doubleword, Read-Only
  UINT32  _DR6;             // FFCCh, DR6, Doubleword, Read-Only
  UINT32  _EAX;             // FFD0h, EAX, Doubleword, Read/Write
  UINT32  _ECX;             // FFD4h, ECX, Doubleword, Read/Write
  UINT32  _EDX;             // FFD8h, EDX, Doubleword, Read/Write
  UINT32  _EBX;             // FFDCh, EBX, Doubleword, Read/Write
  UINT32  _ESP;             // FFE0h, ESP, Doubleword, Read/Write
  UINT32  _EBP;             // FFE4h, EBP, Doubleword, Read/Write
  UINT32  _ESI;             // FFE8h, ESI, Doubleword, Read/Write
  UINT32  _EDI;             // FFECh, EDI, Doubleword, Read/Write
  UINT32  _EIP;             // FFF0h, EIP, Doubleword, Read/Write
  UINT32  _EFLAGS;          // FFF4h, EFLAGS, Doubleword, Read/Write
  UINT32  _CR3;             // FFF8h, CR3, Doubleword, Read/Write
  UINT32  _CR0;             // FFFCh, CR0, Doubleword, Read/Write
} AMD_SMM_CPU_SAVE_STATE32;

typedef struct _AMD_SMM_CPU_SAVE_STATE64 {
// FE00h ES Selector Word
  UINT16  _ES;                    // FE00h, ES Selector, Word, Read-Only
// FE02h ES Attributes Word
  UINT16  ES_Attributes;         // FE02h, ES Attributes, Word, Read-Only
// FE04h ES Limit Doubleword
  UINT32  ES_Limit;               // FE04h, ES Limit, Doubleword, Read-Only
// FE08h ES Base Quadword
  UINT64  ES_Base;                // FE08h, ES Base, Quadword, Read-Only
// FE10h CS Selector Word
  UINT16  _CS;                     // FE10h, CS Selector, Word, Read-Only
// FE12h CS Attributes Word
  UINT16  CS_Attributes;          // FE12h, CS Attributes, Word, Read-Only
// FE14h CS Limit Doubleword
  UINT32  CS_Limit;                // FE14h, CS Limit, Doubleword, Read-Only
// FE18h CS Base Quadword
  UINT64  CS_Base;                 // FE18h, CS Base, Quadword, Read-Only
// FE20h SS Selector Word
  UINT16  _SS;                      // FE20h, SS Selector, Word, Read-Only
// FE22h SS Attributes Word
  UINT16  SS_Attributes;           // FE22h, SS Attributes, Word, Read-Only
// FE24h SS Limit Doubleword
  UINT32  SS_Limit;                 // FE24h, SS Limit, Doubleword, Read-Only
// FE28h SS Base Quadword
  UINT64  SS_Base;                  // FE28h, SS Base, Quadword, Read-Only
// FE30h DS Selector Word
  UINT16  _DS;                      // FE30h, DS Selector, Word, Read-Only
// FE32h DS Attributes Word
  UINT16  DS_Attributes;           // FE32h, DS Attributes, Word, Read-Only
// FE34h DS Limit Doubleword
  UINT32  DS_Limit;                 // FE34h, DS Limit, Doubleword, Read-Only
// FE38h DS Base Quadword
  UINT64  DS_Base;                  // FE38h, DS Base, Quadword, Read-Only
// FE40h FS Selector Word
  UINT16  _FS;                      // FE40h, FS Selector, Word, Read-Only
// FE42h FS Attributes Word
  UINT16  FS_Attributes;           // FE42h, FS Attributes, Word, Read-Only
// FE44h FS Limit Doubleword
  UINT32  FS_Limit;                 // FE44h, FS Limit, Doubleword, Read-Only
// FE48h FS Base Quadword
  UINT64  FS_Base;                  // FE48h, FS Base, Quadword, Read-Only
// FE50h GS Selector Word
  UINT16  _GS;                      // FE50h, GS Selector, Word, Read-Only
// FE52h GS Attributes Word
  UINT16  GS_Attributes;           // FE52h, GS Attributes, Word, Read-Only
// FE54h GS Limit Doubleword
  UINT32  GS_Limit;                // FE54h, GS Limit, Doubleword, Read-Only
  UINT64  GS_Base;                 // FE58h, GS Base, Quadword, Read-Only
  UINT8   Reserved1[4];            // FE60h - FE63h, GDTR Reserved, 4 Bytes, Read-Only 
// FE64h GDTR Limit Word
  UINT16  GDTR_Limit;              // FE64h, GDTR Limit, Word, Read-Only
// FE66h reserved 2 Bytes
  UINT8   Reserved2[2];            // FE66h - FE67h, Reserved, 2 Bytes, Read-Only
// FE68h GDTR Base Quadword
//  UINT64  GDTR_Base;               // FE68h, GDTR Base, Quadword, Read-Only
  UINT32  GDTR_Base_LoDword;
  UINT32  GDTR_Base_HiDword;
// FE70h LDTR Selector Word
  UINT16  LDTR_Selector;           // FE70h, LDTR Selector, Word, Read-Only
// FE72h Attributes Word
  UINT16  LDTR_Attributes;        // FE72h, LDTR Attributes, Word, Read-Only
// FE74h Limit Doubleword 
  UINT32  LDTR_Limit;              // FE74h, LDTR Limit, Doubleword, Read-Only
// FE78h Base Quadword
//  UINT64  LDTR_Base;               // FE78h, LDTR Base, Quadword, Read-Only
  UINT32  LDTR_Base_LoDword;
  UINT32  LDTR_Base_HiDword;
// FE80h¡VFEB1h IDTR reserved 2 Bytes Read-Only
  UINT8   Reserved3[4];            // FE80h - FE83h (PID: 24593 (PUB) 3.20 p.279 has a technical errors), IDTR Reserved, 4 Bytes, Read-Only
// FE84h Limit Word
  UINT16  IDTR_Limit;              // FE84h, IDTR Limit, Word, Read-Only
// FEB6h¡VFEB7h reserved 2 Bytes
  UINT8   Reserved4[2];            // FE86h - FE87h (PID: 24593 (PUB) 3.20 p.279 has a technical errors), IDTR Reserved, 2 Bytes, Read-Only
// FE88h Base Quadword
//  UINT64  IDTR_Base;               // FE88h, IDTR Base, Quadword, Read-Only
  UINT32  IDTR_Base_LoDword;
  UINT32  IDTR_Base_HiDword;
// FE90h TR Selector Word
  UINT16  TR_Selector;            // FE90h, TR Selector, Word, Read-Only
// FE92h Attributes Word
  UINT16  TR_Attributes;          // FE92h, TR Attributes, Word, Read-Only
// FE94h Limit Doubleword
  UINT32  TR_Limit;                // FE94h, TR Limit, Doubleword, Read-Only
// FE98h Base Quadword
  UINT64  TR_Base;                 // FE98h, TR Base, Quadword, Read-Only
// FEA0h¡VFEBFh reserved 32 Bytes ¡X
//  UINT8   Reserved14[32];  //
  UINT64  IO_RESTART_RIP;         // FEA0h, I/O Instruction Restart RIP, Quadword, Read-Only
  UINT64  IO_RESTART_RCX;         // FEA8h, I/O Instruction Restart RCX, Quadword, Read-Only
  UINT64  IO_RESTART_RSI;         // FEB0h, I/O Instruction Restart RSI, Quadword, Read-Only
  UINT64  IO_RESTART_RDI;         // FEB8h, I/O Instruction Restart RDI, Quadword, Read-Only
  UINT32  SMM_IO_TRAP;            // FEC0h, I/O Instruction Restart Dword SMMFEC0 [SMM IO Trap Offset], Read-Only
  UINT32  LocalSmiStatus;         // FEC4h, SMMFEC4 [Local SMI Status], Doubleword, Read-Only
  UINT8   SMM_IO_RESTART;         // FEC8h, SMMFEC8 [SMM IO Restart Byte], Byte, Read/Write
  UINT8   AutoHALTRestart; // FEC9h, SMMFEC9 [Auto Halt Restart Offset], Byte, Read/Write
  UINT8   NMI_MASK;               // FECAh, SMMFECA [NMI Mask], Byte, Read/Write
  UINT8   Reserved5[5];           // FECBh - FECFh, Reserved, 5 Bytes, --
  UINT64  EFER;                    // FED0h, EFER, Quadword, Read-Only
  UINT64  SMM_SVM_State;          // FED8h, SMMFED8 [SMM SVM State], Read-Only
  UINT64  Guest_VMCB_PHY_ADDR;   // FEE0h, Guest VMCB physical address, Read-Only
  UINT64  SVM_Virtual_INT_CTRL;  // FEE8h, SVM Virtual Interrupt Control, Read-Only
  UINT8   Reserved6[12];          // FEF0h - FEFBh, Reserved, 12 Bytes (PID: 42300 Family 15h BKDG (NDA) 1.11 p.49 has a technial error), -- 
  UINT32  SMM_REVID;              // FEFCh, SMMFEFC [SMM-Revision-Indentifier], Doubleword, Read/Write
  UINT32  SMBASE;                 // FF00h, SMMFF00 [SMM Base Address (SMM_BASE)], Read/Write
// FF04h¡XFF47h reserved 68 Bytes ¡X
  UINT8   Reserved7[28];          // FF04h, Reserved, 24 Bytes, -- 
  UINT64  Guest_PAT;              // FF20h, Guest PAT, Quadword, Read-Only
  UINT64  Host_EFER;              // FF28h, Host EFER, Quadword, Read-Only
  UINT64  Host_CR4;               // FF30h, Host CR4, Quadword, Read-Only
  UINT64  Nested_CR3;             // FF38h, Nested CR3, Quadword, Read-Only
  UINT64  Host_CR0;               // FF40h, Host CR0, Quadword, Read-Only 
// FF48h CR4 Quadword
  UINT64  _CR4;                   // FF48h, CR4, Quadword, Read-Only
// FF50h CR3 Quadword
  UINT64  _CR3;                   // FF50h, CR3, Quadword, Read-Only
// FF58h CR0 Quadword
  UINT64  _CR0;                   // FF58h, CR0, Quadword, Read-Only
// FF60h DR7 Quadword
  UINT64  _DR7;                   // FF60h, DR7, Quadword, Read-Only
// FF68h DR6 Quadword
  UINT64  _DR6;                   // FF68h, DR6, Quadword, Read-Only
// FF70h RFLAGS Quadword
  UINT64  _RFLAGS;                // FF70h, RFLAGS, Quadword, Read/Write
// FF78h RIP
  UINT64  _RIP;                   // FF78h, RIP, Quadword, Read/Write
// FF80h R15
  UINT64  _R15;                    // FF80h, R15, Quadword, Read/Write
// FF88h R14
  UINT64  _R14;                    // FF88h, R14, Quadword, Read/Write
// FF90h R13
  UINT64  _R13;                    // FF90h, R13, Quadword, Read/Write
// FF98h R12
  UINT64  _R12;                    // FF98h, R12, Quadword, Read/Write
// FFA0h R11
  UINT64  _R11;                    // FFA0h, R11, Quadword, Read/Write
// FFA8h R10
  UINT64  _R10;                    // FFA8h, R10, Quadword, Read/Write
// FFB0h R9 
  UINT64  _R9;                     // FFB0h, R9, Quadword, Read/Write
// FFB8h R8 
  UINT64  _R8;                     // FFB8, R8, Quadword, Read/Write
// FFC0h RDI
  UINT64  _RDI;                   // FFC0h, RDI, Quadword, Read/Write
// FFC8h RSI
  UINT64  _RSI;                   // FFD8h, RSI, Quadword, Read/Write
// FFD0h RBP
  UINT64  _RBP;                   // FFD0h, RBP, Quadword, Read/Write
// FFD8h RSP
  UINT64  _RSP;                   // FFE0h, RSP, Quadword, Read/Write
// FFE0h RBX
  UINT64  _RBX;                   // FFE0h, RBX, Quadword, Read/Write
// FFE8h RDX
  UINT64  _RDX;                   // FFE8h, RDX, Quadword, Read/Write
// FFF0h RCX
  UINT64  _RCX;                   // FFF0h, RCX, Quadword, Read/Write
// FFF8h RAX
  UINT64  _RAX;                   // FFF8h, RAX, Quadword, Read/Write
} AMD_SMM_CPU_SAVE_STATE64;

typedef union _AMD_SMM_CPU_STATE {
  AMD_SMM_CPU_SAVE_STATE32          x86;
  AMD_SMM_CPU_SAVE_STATE64          x64;
} AMD_SMM_CPU_STATE;


#pragma pack ()

///
/// Provides a programatic means to access SMM save state.
///
struct _EFI_SMM_CPU_SAVE_STATE_PROTOCOL {
  ///
  /// Reference to a list of save states.
  ///
  AMD_SMM_CPU_STATE  **CpuSaveState;
};

extern EFI_GUID gEfiSmmCpuSaveStateProtocolGuid;

#endif
