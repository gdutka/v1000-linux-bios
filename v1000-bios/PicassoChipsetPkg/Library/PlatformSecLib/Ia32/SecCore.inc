;;******************************************************************************
;;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
;
; This file contains 'Framework Code' and is licensed as such
; under the terms of your license agreement with Intel or your
; vendor.  This file may not be modified, except as allowed by
; additional terms of your license agreement.
;
;------------------------------------------------------------------------------
;
; Copyright (c)  1999 - 2003 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
; 
; Module Name:
;
;   SecCore.inc
;
; Abstract:
;
;   SecCore constants and macros
;
;------------------------------------------------------------------------------

;[-start-110620-IB04170121-modify];
IFDEF   PLATFORM_DEBUG_PORT_NUMBER
  DEBUG_PORT    EQU PLATFORM_DEBUG_PORT_NUMBER
ELSE
  DEBUG_PORT    EQU 80h
ENDIF

POSTCODE        MACRO   PostCode
;*AMD_EARLY_PORT80  IFDEF   H2O_PORT_80_DEBUG
    ror  eax, 10h
    mov  ax, PostCode
    out  DEBUG_PORT, ax
    rol  eax, 10h
;*AMD_EARLY_PORT80  ENDIF
ENDM
;[-end-110620-IB04170121-modify];


L1_MIN_1_WAY_SIZE   EQU (16 * 1024)
TEST_BASE           EQU 0000CC000h

TEST_MASK           EQU (NOT ((L1_MIN_1_WAY_SIZE) - 1))

IMAGE_BASE_ADDRESS EQU 0FFFE0000h
BFV_BASE_ADDRESS   EQU 0FFFFFFFCh
PEI_CORE_ENTRY     EQU 0FFFFFFE0h
