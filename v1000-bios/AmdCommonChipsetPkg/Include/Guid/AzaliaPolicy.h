/** @file
  Azalia policy structure and GUID declaration.

;*******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _AZALIA_POLICY_H_
#define _AZALIA_POLICY_H_

#define AZALIA_POLICY_GUID \
  { \
    0xE197BE68, 0x9AE2, 0x448E, 0xAC, 0x56, 0xA4, 0x70, 0x82, 0x5F, 0xD8, 0xD8 \
  }

#define AZALIA_POLICY_VARIABLE               L"AzaliaPolicyVariable"

typedef struct _AZALIAPIN AZALIAPIN;
struct _AZALIAPIN {
  UINT8   AzaliaSdin0:2;               /**< AzaliaSdin0
                                        *   @par
                                        * SDIN0 is define at BIT0 & BIT1
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  UINT8   AzaliaSdin1:2;               /**< AzaliaSdin1
                                        *   @par
                                        * SDIN0 is define at BIT2 & BIT3
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  UINT8   AzaliaSdin2:2;               /**< AzaliaSdin2
                                        *   @par
                                        * SDIN0 is define at BIT4 & BIT5
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  UINT8   AzaliaSdin3:2;               /**< AzaliaSdin3
                                        *   @par
                                        * SDIN0 is define at BIT6 & BIT7
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
};

typedef struct _AZALIA_POLICY AZALIA_POLICY;
struct _AZALIA_POLICY {
  UINT32                  AzaliaSsid;
  UINT8                   AzaliaController;
  UINT8                   AzaliaPinCfg;
  UINT8                   AzaliaFrontPanel;
  UINT8                   FrontPanelDetected;
  UINT8                   AzaliaSnoop;
  UINT8                   AzaliaDummy;
  union {
    UINT8                 AzaliaSdinPin;
    AZALIAPIN             AzaliaConfig;
  } AZALIACONFIGINFO;
};

extern EFI_GUID gAzaliaPolicyGuid;

#endif
