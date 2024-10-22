;** @file
;
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

VacantFlag          Equ   00h
NotVacantFlag       Equ   0ffh
StartupApSignal     Equ   6E750000h
MonitorFilterSize   Equ   10h
ApInHltLoop         Equ   1
ApInMwaitLoop       Equ   2
ApInRunLoop         Equ   3

LockLocation        equ        (RendezvousFunnelProcEnd - RendezvousFunnelProcStart)
StackStart          equ        (LockLocation + 4h)
StackSize           equ        (LockLocation + 8h)
RendezvousProc      equ        (LockLocation + 0Ch)
GdtrProfile         equ        (LockLocation + 10h)
IdtrProfile         equ        (LockLocation + 16h)
BufferStart         equ        (LockLocation + 1Ch)
Cr3Location         equ        (LockLocation + 20h)
InitFlag            equ        (LockLocation + 24h)
ApCountLocation     equ        (LockLocation + 28h)
DcuModeSelectFlag   equ        (LockLocation + 2Ch)
DcuMode             equ        (LockLocation + 2Dh)
ApLoopModeLocation  equ        (LockLocation + 30h)
BistBuffer          equ        (LockLocation + 34h)

PAUSE32   MACRO
            DB      0F3h
            DB      090h
            ENDM

;-------------------------------------------------------------------------------
