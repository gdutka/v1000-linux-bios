;;******************************************************************************
;;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************

%macro ReadPmio 1
         mov al, %1
         mov dx, 0cd6h
         out dx, al
         inc dx
         in al, dx
%endmacro

%macro WritePmio 2
         mov ah, %2
         mov al, %1
         mov dx, 0cd6h
         out dx, al
         inc dx
         mov al, ah
         out dx, al
%endmacro

%macro ModifyPmio 2
         mov dx, 0cd6h
         mov al, %1
         out dx, al
         inc dx
         in  al, dx
         or  al, %2
         out dx, al
%endmacro

%macro SEC_PLATFORM_ENABLE_DEBUG_PORT_MACRO 0
         ;
         ; Add enable debug port code in this macro according to H/W design.
         ;
         ; it's for PORT80 behind LPC
%ifdef   EMULATION_ONLY
         mov	 dx,   0cf8h
         mov	 eax,  8000A348h
         out	 dx,   eax
         add	 dl,   4 + 2
         in	 al,   dx
         or	 al,   20h
         out	 dx,   al
%endif

         ; SerialPortEnable0
         mov	 dx,   0cf8h
         mov	 eax,  8000A344h
         out	 dx,   eax
         add	 dl,   4
         in	 al,   dx
         or	 al,   40h
         out	 dx,   al
%endmacro


    bEnableCF8ExtCfgZp    EQU   00000001h   ; [1]
%macro SEC_PLATFORM_INIT_STAGE1_MACRO  0
        ;
        ; Add Platform required initial code here which will be executed very early stage.
        ;
        ;
        ; INIT handler
        ;

        ; Enable D18F4x44[EnableCf8ExtCfg]
        mov     dx, 0CF8h
        mov     eax, 8000C444h
        out     dx, eax
        mov     dl, 0FCh
        in      eax, dx
        or      eax, bEnableCF8ExtCfgZp
        out     dx, eax

        ; INIT handler
        mov   eax,  08300C680h     ; D18F6x380[0]
        mov   dx,   0CF8h
        out   dx,   eax
        add   dl,   4
        in    al,   dx
        test  al,   01h          ; InitDet
        jz    %%SetInitDetZp
        ;
        ; Issue PCI Reset
        ;
        mov   al,   06h
        mov   dx,   0CF9h
        out   dx,   al
        jmp   $

%%SetInitDetZp:
        or    al,   01h          ; Always set InitDet
        out   dx,   al
        jmp   %%AfterInitCmdWorkaournd
        ;
        ; Enable decode on Port2E/2F  Reg48 bit[0]
        ; Enable decode on Rom Rnage2 Reg48 bit[4]
        ; Don't Enable decode on Port70-73h Reg48 bit[6], Because none of our boards use SIO RTC.
        ; Programming bit-6 can cause a problem.
        ;
%%AfterInitCmdWorkaournd:
        mov   dx,   0cf8h
        mov   eax,  8000A348h
        out   dx,   eax

        add   dl,   4
        in    eax,  dx
        or    eax,  00000013h
        out   dx,   eax
        ;
        ; According to the BKDG, PMx00[LegacyIoEn] needs to be set to allow reads/writes to port 72/73.
        ;
        ModifyPmio     00h, 01h
        ;
        ; Enable ACPI MMIO decode
        ;
;;;Jeff         WritePmio     24h, 01h
        ModifyPmio     04h, 02h
        ModifyPmio     00h, 01h     ; set LegacyIOEn
%endmacro


%macro SEC_PLATFORM_GET_ACPI_TIMER_DATA_MACRO 0
        ;
        ; Get ACPI Timer when system just power on for POST time measurement
        ;
        mov      edx,  0fed80e40h
        mov      al,   byte ptr [edx]
        or       al,   40h
        mov      byte ptr [edx], al

        WritePmio      60h, 00h
        WritePmio      61h, 04h
        WritePmio      62h, 04h
     	  WritePmio      63h, 04h
        WritePmio      64h, 08h
    	  WritePmio      65h, 04h
        WritePmio      66h, 10h
        WritePmio      67h, 04h
        WritePmio      68h, 20h
    	  WritePmio      69h, 04h
        WritePmio      6Ah, 0B0h
        WritePmio      6Bh, 0h

        ReadPmio       074h
        mov      ah,    al
        or       ah,    01h
        WritePmio      074h, ah
    	  mov      dx,    0408h
        in       eax,   dx
        mov      ebx,   eax

        mov      ah,    BOOT_TIME_FLAG_SEC
WriteTimeToBiosRam:
        mov      ecx,   04h
%%label:
        mov      al,    ah
        mov      dx,    0cd4h
        out      dx,    al
        inc      ah
        out      0edh,  al
        mov      al,    bl
        inc      dx
        out      dx,    al
        ror      ebx,   08h
        loop     %%label
%endmacro
