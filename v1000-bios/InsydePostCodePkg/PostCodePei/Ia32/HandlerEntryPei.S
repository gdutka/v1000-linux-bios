## @file
#  This file is the PostCodeFeatureHandlerEntry () implementation
#  It is Ia32 architecture implement
#
#******************************************************************************
#* Copyright (c) 2018, Insyde Software Corporation. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

ASM_GLOBAL ASM_PFX(PostCodeFeatureHandler)

#---------------------------------------;
# PostCodeFeatureHandlerEntry           ;
#---------------------------------------;
#This entry will be registered to the debug exception entry of IDT by PostCodePei module
#And it will call PostCodeFeatureHandler () to perform PostCode feature 

# +---------------------+ 
# +    Old SS           +
# +---------------------+
# +    Old ESP          +
# +---------------------+
# +    EFlags           +
# +---------------------+
# +    CS               +
# +---------------------+
# +    EIP              +
# +---------------------+
# +    EBP              +
# +---------------------+<-- EBP
# 
ASM_GLOBAL ASM_PFX(PostCodeFeatureHandlerEntry)
ASM_PFX(PostCodeFeatureHandlerEntry):
  pushl   %ebp                # C prolog
  movl    %esp,%ebp
  
#Push scratch registers
#The processor will enter this exception when it writes PostCode to port 80
#And Eax is the PostCode data because of the Out instruction (ex. Out Dx,Eax)
  pushl   %edx
  pushl   %ecx
  pushl   4(%ebp)
  pushl   %eax 

#Call PostCodeFeatureHandler ()
  call    ASM_PFX(PostCodeFeatureHandler)

#Pop scratch registers
  popl    %eax
  add     $0x4, %esp
  popl    %ecx
  popl    %edx

  popl    %ebp
  
  iret
