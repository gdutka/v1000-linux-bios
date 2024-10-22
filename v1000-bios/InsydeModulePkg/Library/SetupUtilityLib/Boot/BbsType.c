/** @file
  This file defines Bbs type table.

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "BbsType.h"

//
// the sequence of items is default boot order
//
BBS_TYPE_TABLE mBbsTypeTable[] = {
  {BBS_FLOPPY       , STRING_TOKEN(STR_FLOPPY_BOOT)       , 0, 0, 0, FDD_BOOT_DEVICE_LABEL          , BOOT_DEVICE_FDD_FORM_ID   , NULL},
  {BBS_HARDDISK     , STRING_TOKEN(STR_HDD_BOOT)          , 0, 0, 0, HDD_BOOT_DEVICE_LABEL          , BOOT_DEVICE_HDD_FORM_ID   , NULL},
  {BBS_CDROM        , STRING_TOKEN(STR_CD_BOOT)           , 0, 0, 0, CD_BOOT_DEVICE_LABEL           , BOOT_DEVICE_CD_FORM_ID    , NULL},
  {BBS_PCMCIA       , STRING_TOKEN(STR_PCMCIA_BOOT)       , 0, 0, 0, PCMCIA_BOOT_DEVICE_LABEL       , BOOT_DEVICE_PCMCIA_FORM_ID, NULL},
  {BBS_USB          , STRING_TOKEN(STR_USB_BOOT)          , 0, 0, 0, USB_BOOT_DEVICE_LABEL          , BOOT_DEVICE_USB_FORM_ID   , NULL},
  {BBS_EMBED_NETWORK, STRING_TOKEN(STR_EMBED_NETWORK_BOOT), 0, 0, 0, EMBED_NETWORK_BOOT_DEVICE_LABEL, BOOT_EMBED_NETWORK_FORM_ID, NULL},
  {BBS_BEV_DEVICE   , STRING_TOKEN(STR_BEV_BOOT)          , 0, 0, 0, BEV_BOOT_DEVICE_LABEL          , BOOT_DEVICE_BEV_FORM_ID   , NULL},
  {OTHER_DRIVER     , STRING_TOKEN(STR_OTHER_DRIVE_BOOT)  , 0, 0, 0, OTHER_BOOT_DEVICE_LABEL        , BOOT_DEVICE_OTHER_FORM_ID , NULL}
  };

UINT32 mBbsTypeTableCount = sizeof (mBbsTypeTable) / sizeof(BBS_TYPE_TABLE);


/**
 Get the BBS type table which contains the boot option number

 @param[in] BootOptionNum                   Boot option number

 @return the pointer of BBS type table or NULL if not found
**/
BBS_TYPE_TABLE *
GetBbsTypeTableByBootOptionNum (
  IN UINT16                                 BootOptionNum
  )
{
  UINTN                                     Index;
  UINTN                                     DevIndex;
  BBS_TYPE_TABLE                            *BbsTypeTable;

  for (Index = 0; Index < mBbsTypeTableCount; Index++) {
    BbsTypeTable = &mBbsTypeTable[Index];

    for (DevIndex = 0; DevIndex < BbsTypeTable->DevInfoCount; DevIndex++) {
      if (BbsTypeTable->DevInfoList[DevIndex].BootOptionNum == BootOptionNum) {
        return BbsTypeTable;
      }
    }
  }

  return NULL;
}

/**
 Get the BBS type table by device type

 @param[in] DevType                         BBS device type

 @return the pointer of BBS type table or NULL if not found
**/
BBS_TYPE_TABLE *
GetBbsTypeTableByDevType (
  IN UINT8                                  DevType
  )
{
  UINTN                                     Index;

  for (Index = 0; Index < mBbsTypeTableCount; Index++) {
    if (mBbsTypeTable[Index].DeviceType == DevType) {
      return &mBbsTypeTable[Index];
    }
  }

  return NULL;
}

/**
 Get legacy boot device type order

 @param[out] LegacyBootDevTypeCount         Pointer to the number of legacy boot device type
 @param[out] LegacyBootDevType              Double pointer to the legacy boot device type order

 @retval EFI_SUCCESS              Get legacy boot device type order successfully
 @retval EFI_OUT_OF_RESOURCES     Allocate pool fail
 @retval EFI_INVALID_PARAMETER    There is a undefined boot device type in default legacy boot device type table
**/
EFI_STATUS
GetLegacyBootDevType (
  OUT UINT16                                *LegacyBootDevTypeCount,
  OUT UINT16                                **LegacyBootDevType
  )
{
  UINT8                                     *BootTypeOrder;
  UINT16                                    Index;
  UINT16                                    TableIndex;
  UINT16                                    *TableIndexOrder;
  UINT16                                    Count;

  *LegacyBootDevType      = NULL;
  *LegacyBootDevTypeCount = 0;

  BootTypeOrder = (UINT8 *) PcdGetPtr (PcdLegacyBootTypeOrder);
  if (BootTypeOrder == NULL || BootTypeOrder[0] == 0) {
    return EFI_SUCCESS;
  }

  Count = 0;
  while (BootTypeOrder[Count] != 0) {
    Count++;
  }

  TableIndexOrder = AllocatePool (Count * sizeof (UINT16));
  if (TableIndexOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < Count; Index++) {
    for (TableIndex = 0; TableIndex < mBbsTypeTableCount; TableIndex++) {
      if (BootTypeOrder[Index] == mBbsTypeTable[TableIndex].DeviceType) {
        TableIndexOrder[Index] = TableIndex;
        break;
      }
    }
    if (TableIndex == mBbsTypeTableCount) {
      FreePool (TableIndexOrder);
      return EFI_INVALID_PARAMETER;
    }
  }

  *LegacyBootDevType      = TableIndexOrder;
  *LegacyBootDevTypeCount = Count;
  return EFI_SUCCESS;
}

/**
 Initialize module variable mBbsTypeTable[]

 @retval EFI_SUCCESS                        Update module variable successfully
**/
EFI_STATUS
InitBbsTypeTable (
  VOID
  )
{
  UINTN                                     TableIndex;
  UINTN                                     Index;
  BBS_TYPE_TABLE                            *BbsTypeTable;
  UINT16                                    KeyBase;

  KeyBase = KEY_BOOT_DEVICE_TYPE_BASE;

  ShutdownBbsTypeTable ();

  for (TableIndex = 0; TableIndex < mBbsTypeTableCount; TableIndex++) {
    BbsTypeTable = &mBbsTypeTable[TableIndex];

    for (Index = 0; Index < mBootDevInfoCount; Index++) {
      if (!mBootDevInfo[Index].IsEfiBootDev && mBootDevInfo[Index].DevType == BbsTypeTable->DeviceType) {
        BbsTypeTable->DevInfoCount++;
      }
    }
    if (BbsTypeTable->DevInfoCount == 0) {
      continue;
    }

    BbsTypeTable->DevInfoList = AllocatePool (sizeof (BOOT_DEV_INFO) * BbsTypeTable->DevInfoCount);
    if (BbsTypeTable->DevInfoList == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    for (Index = 0; Index < mBootDevInfoCount; Index++) {
      if (!mBootDevInfo[Index].IsEfiBootDev && mBootDevInfo[Index].DevType == BbsTypeTable->DeviceType) {
        CopyMem (
          &BbsTypeTable->DevInfoList[BbsTypeTable->CurrentIndex],
          &mBootDevInfo[Index],
          sizeof (BOOT_DEV_INFO)
          );
        BbsTypeTable->CurrentIndex++;
      }
    }

    BbsTypeTable->KeyBootDeviceBase = KeyBase;
    KeyBase += (UINT16) BbsTypeTable->DevInfoCount;
  }

  return EFI_SUCCESS;
}

/**
 Clear data and free the memory resource in module variable mBbsTypeTable[]
**/
VOID
ShutdownBbsTypeTable (
  VOID
  )
{
  UINTN                                     TableIndex;
  BBS_TYPE_TABLE                            *BbsTypeTable;

  for (TableIndex = 0; TableIndex < mBbsTypeTableCount; TableIndex++) {
    BbsTypeTable = &mBbsTypeTable[TableIndex];

    BbsTypeTable->DevInfoCount      = 0;
    BbsTypeTable->CurrentIndex      = 0;
    BbsTypeTable->KeyBootDeviceBase = 0;
    if (BbsTypeTable->DevInfoList != NULL) {
      FreePool (BbsTypeTable->DevInfoList);
      BbsTypeTable->DevInfoList = NULL;
    }
  }
}

/**
 Based on the BootOrder, sort the device list order in BBS type table.

 @param[in] BootOrder                       The pointer of BootOrder
 @param[in] BootDevNum                      The number of boot device

 @retval EFI_SUCCESS                        Success sort the BootOrder
 @retval EFI_OUT_OF_RESOURCES               Allocate pool fail
**/
EFI_STATUS
SortBbsTypeTable (
  IN UINT16                                 *BootOrder,
  IN UINT16                                 BootDevNum
  )
{
  UINT16                                    TableIndex;
  UINT16                                    OrderIndex;
  UINT16                                    DevIndex;
  BOOT_DEV_INFO                             *NewDevInfoList;
  UINT16                                    NewDevInfoCount;
  BBS_TYPE_TABLE                            *BbsTypeTable;

  if (BootDevNum == 0) {
    return EFI_SUCCESS;
  }

  NewDevInfoList = AllocatePool (sizeof (BOOT_DEV_INFO) * BootDevNum);
  if (NewDevInfoList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (TableIndex = 0; TableIndex < mBbsTypeTableCount; TableIndex++) {
    BbsTypeTable = &mBbsTypeTable[TableIndex];
    if (BbsTypeTable->DevInfoCount == 0) {
      continue;
    }

    NewDevInfoCount = 0;
    for (OrderIndex = 0; OrderIndex < BootDevNum; OrderIndex++) {
      for (DevIndex = 0; DevIndex < BbsTypeTable->DevInfoCount; DevIndex++) {
        if (BbsTypeTable->DevInfoList[DevIndex].BootOptionNum == BootOrder[OrderIndex]) {
          CopyMem (
            &NewDevInfoList[NewDevInfoCount++],
            &BbsTypeTable->DevInfoList[DevIndex],
            sizeof(BOOT_DEV_INFO)
            );
          break;
        }
      }
    }
    ASSERT (NewDevInfoCount == BbsTypeTable->DevInfoCount);

    BbsTypeTable->CurrentIndex = NewDevInfoCount;
    BbsTypeTable->DevInfoCount = NewDevInfoCount;
    CopyMem (
      BbsTypeTable->DevInfoList,
      NewDevInfoList,
      sizeof(BOOT_DEV_INFO) * NewDevInfoCount
      );
  }

  FreePool (NewDevInfoList);
  return EFI_SUCCESS;
}

