//;******************************************************************************
//;* Copyright (c) 2015 - 2018, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#ifndef _OEM_GLOBAL_NVS_AREA_H_
#define _OEM_GLOBAL_NVS_AREA_H_

//
// OEM Global NVS Area definition
//
#pragma pack (1)
typedef struct {

  //
  // The definitions below need to be matched OGNS definitions in OemGlobalNvs.ASL
  // and can be modified by OEM
  //
  UINT8       OemGlobalNvsData00;    // 00
  UINT8       OemGlobalNvsData01;    // 01 OG01
  UINT8       M2WlanEn;              // 02 M2WL
  UINT8       TouchPanel;            // 03
  UINT8       PcieBar;               // 04
  UINT8       TouchPad;              // 05
  UINT8       DtSlotEn;              // 06 DTEN
  UINT8       SerialDbgMsgUnderOs;   // 07 SDMO
  UINT8       OemGlobalNvsData08;    // 08
  UINT8       InternalBuild;         // 09
  UINT8       RV2Identifier;         // 10

} EFI_OEM_GLOBAL_NVS_AREA;
#pragma pack ()

#endif
