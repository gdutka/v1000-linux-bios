/** @file

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _GSMI_
#define _GSMI_

Method(GSMI, 1)
{
  Store(ARG0, APMD)
  Store(0xE4, APMC)
  Sleep(2)
}

Method(S80H, 1)
{
  Store(ARG0, P80H)
}

Method(BSMI, 1)
{
  Store(ARG0, APMD)
  Store(0xBE, APMC)
  Sleep(1)
}

#endif
