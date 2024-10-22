;/**
; * @file
; *
; * PSP SMM ASSEMBLY library
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  PSP
; * @e \$Revision$   @e \$Date$
; */
;*****************************************************************************
;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************


.code
;
; The below code will resume outside SMM
S3ExitToBiosV2   PROC  PUBLIC
  rsm
S3ExitToBiosV2   ENDP

END


