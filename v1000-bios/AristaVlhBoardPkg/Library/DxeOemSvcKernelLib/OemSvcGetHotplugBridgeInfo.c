/** @file
  Provides an opportunity for OEM to get the HotPlugBridgeInfo table.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

HOT_PLUG_BRIDGE_INFO  HotPlugBridgeInfo [] = {
// NodeUID,    DevNum,     FuncNum,    ReservedBusCount,     ReservedIoRange,   ReservedNonPrefetchableMmio,  AlignemntOfReservedNonPrefetchableMmio, ReservedPrefetchableMmio, AlignemntOfReservedPrefetchableMmio
  { 0x0,           1,           1,                   1,             0x2000,                    0x200000,                    0x100000,                    0x1000000,               0x100000},
  { 0x0,           1,           2,                   1,             0x2000,                    0x200000,                    0x100000,                    0x1000000,               0x100000},
  { 0x0,           1,           3,                   1,             0x2000,                    0x200000,                    0x100000,                    0x1000000,               0x100000},
  { 0x0,           1,           4,                   1,             0x2000,                    0x200000,                    0x100000,                    0x1000000,               0x100000},
  { 0x0,           1,           5,                   1,             0x2000,                    0x200000,                    0x100000,                    0x1000000,               0x100000},
  { 0x0,           1,           6,                   1,             0x2000,                    0x200000,                    0x100000,                    0x1000000,               0x100000},
  { 0x0,           1,           7,                   1,             0x2000,                    0x200000,                    0x100000,                    0x1000000,               0x100000},
  { END_OF_TABLE,  0,           0,                   0,                   0,                             0,                         0,                             0,                         0}, //End of Table, Cannot be removed.
};

/**
  Provides an opportunity for OEM to get the HotPlugBridgeInfo table. 
  This table contains the hot plug bridge address info and defines reserved BUS, I/O and memory range for bridge device.

  @param[out]  HotPlugBridgeInfoTable   Points to the structure that indicates the HOT_PLUG_BRIDGE_INFO Table

  @retval      EFI_UNSUPPORTED          Returns unsupported by default.
  @retval      EFI_MEDIA_CHANGED        Get hot plug bridge info success.
  @retval      Others                   Depends on customization.
**/
EFI_STATUS
OemSvcGetHotplugBridgeInfo (
  OUT HOT_PLUG_BRIDGE_INFO           **HotPlugBridgeInfoTable
  )
{

  *HotPlugBridgeInfoTable = HotPlugBridgeInfo;

  return EFI_MEDIA_CHANGED;
}
