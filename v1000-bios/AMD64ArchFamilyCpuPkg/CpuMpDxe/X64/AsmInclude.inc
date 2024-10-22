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

VacantFlag                    Equ   00h
NotVacantFlag                 Equ   0ffh
StartupApSignal               Equ   6E750000h
MonitorFilterSize             Equ   10h
ApInHltLoop                   Equ   1
ApInMwaitLoop                 Equ   2
ApInRunLoop                   Equ   3

LockLocation                  equ        (RendezvousFunnelProcEnd - RendezvousFunnelProcStart)
StackStartAddressLocation     equ        (LockLocation + 08h)
StackSizeLocation             equ        (LockLocation + 10h)
CProcedureLocation            equ        (LockLocation + 18h)
GdtrLocation                  equ        (LockLocation + 20h)
IdtrLocation                  equ        (LockLocation + 2Ah)
BufferStartLocation           equ        (LockLocation + 34h)
Cr3OffsetLocation             equ        (LockLocation + 38h)
InitFlagLocation              equ        (LockLocation + 3Ch)
ApCountLocation               equ        (LockLocation + 40h)
DcuModeSelectFlagLocation     equ        (LockLocation + 44h)
DcuModeLocation               equ        (LockLocation + 45h)
ApLoopModeLocation            equ        (LockLocation + 48h)
BistBuffer                    equ        (LockLocation + 4Ch)

;-------------------------------------------------------------------------------
