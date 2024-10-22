/** @file
  Library for enumerate Flash Device Region Layout

;******************************************************************************
;* Copyright (c) 2014 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _FLASH_REGION_LIB_H_
#define _FLASH_REGION_LIB_H_

#include <FlashRegionLayout.h>

#define PCD_FLASH_MAP_START_EX_TOKEN_NUMBER          0x30000017

#define SHA256_DIGEST_SIZE          32
#define FDM_ENTRY_REGION_ID_SIZE    16
#define SUPPORT_BOARD_COUNT         6

#define ENTRY_HASH_SHA256               0

#define H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_MUTABLE       0x1
#define H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_IGNORE        0x2

// Id of Variable defalut: 0x0000
#define FDM_VARIABLE_DEFAULT_ID_VARIABLE_DEFAULT \
  { \
    0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} \
  }

#define DEFAULT_SKU_ID                               0
#define VARIABLE_REGION_INACTIVE                     0x8000


#pragma pack(push, 1)
typedef struct _H2O_FLASH_DEVICE_MAP_HEADER {
  UINT32 Signature;
  UINT32 Size;
  UINT32 Offset;
  UINT32 EntrySize;
  UINT8  EntryFormat;
  UINT8  Revision;
  UINT8  Reserved;
  UINT8  Checksum;
  UINT64 FdBaseAddr;
} H2O_FLASH_DEVICE_MAP_HEADER;

typedef struct _H2O_FLASH_DEVICE_MAP_ENTRY {
  EFI_GUID RegionType;
  UINT8    RegionId[16];
  UINT64   RegionOffset;
  UINT64   RegionSize;
  UINT32   Attribs;
//UINT8    Hash[...];
} H2O_FLASH_DEVICE_MAP_ENTRY;

typedef struct _FDM_REGION_GUID_TABLE {
  UINT8      Index;
  EFI_GUID   Guid;
}FDM_REGION_GUID_TABLE;


typedef struct _FDM_VARIABLE_REGION_ID {
  UINT16      SkuId;
  UINT16      DefaultId;
  UINT16      Status;
}FDM_VARIABLE_REGION_ID;

typedef struct _H2O_FLASH_MAP_OPTION_ROM_ID {
  UINT16 VendorId;
  UINT16 DeviceId;
  UINT8  Flags;
  UINT8  Reserved[5];
  UINT8  Boards[SUPPORT_BOARD_COUNT];
} H2O_FLASH_MAP_OPTION_ROM_ID;

typedef struct _H2O_FLASH_MAP_OPTION_ROM_BDF_ID {
  UINT8  Segment;
  UINT8  Bus;
  UINT8  Device;
  UINT8  Function;
  UINT8  Flags;
  UINT8  Reserved[5];
  UINT8  Boards[SUPPORT_BOARD_COUNT];
} H2O_FLASH_MAP_OPTION_ROM_BDF_ID;

typedef struct _H2O_FLASH_MAP_USB_OPTION_ROM_ID {
  UINT16 VendorId;
  UINT16 ProductId;
  UINT8  Flags;
  UINT8  Reserved[5];
  UINT8  Boards[SUPPORT_BOARD_COUNT];
} H2O_FLASH_MAP_USB_OPTION_ROM_ID;

#define H2O_FLASH_MAP_REGION_LOGO_FORMAT_BMP             0x00
#define H2O_FLASH_MAP_REGION_LOGO_FORMAT_JPEG            0x01
#define H2O_FLASH_MAP_REGION_LOGO_FORMAT_GIF             0x02
#define H2O_FLASH_MAP_REGION_LOGO_FORMAT_PNG             0x03
#define H2O_FLASH_MAP_REGION_LOGO_FORMAT_MASK            0x03

#define H2O_FLASH_MAP_REGION_LOGO_POS_Y_PIXEL            0x00
#define H2O_FLASH_MAP_REGION_LOGO_POS_Y_PERCENTAGE       0x04
#define H2O_FLASH_MAP_REGION_LOGO_POS_X_PIXEL            0x00
#define H2O_FLASH_MAP_REGION_LOGO_POS_X_PERCENTAGE       0x08

#define H2O_FLASH_MAP_REGION_LOGO_ORIGIN_Y_TOP           0x00
#define H2O_FLASH_MAP_REGION_LOGO_ORIGIN_Y_BOTTOM        0x10
#define H2O_FLASH_MAP_REGION_LOGO_ORIGIN_Y_CENTER        0x20
#define H2O_FLASH_MAP_REGION_LOGO_ORIGIN_Y_MASK          0x30

#define H2O_FLASH_MAP_REGION_LOGO_ORIGIN_X_LEFT          0x00
#define H2O_FLASH_MAP_REGION_LOGO_ORIGIN_X_RIGHT         0x40
#define H2O_FLASH_MAP_REGION_LOGO_ORIGIN_X_CENTER        0x80
#define H2O_FLASH_MAP_REGION_LOGO_ORIGIN_X_MASK          0xC0

#define H2O_FLASH_MAP_REGION_LOGO_ID_BOOT                0
#define H2O_FLASH_MAP_REGION_LOGO_ID_CPU_BADGE           1
#define H2O_FLASH_MAP_REGION_LOGO_ID_CHIPSET_BADGE       2
#define H2O_FLASH_MAP_REGION_LOGO_ID_OEM                 255
#define H2O_FLASH_MAP_REGION_LOGO_BOARDS_END             0xFF

typedef struct _H2O_FLASH_MAP_REGION_LOGO_ID {
  INT16  PosX;
  INT16  PosY;
  UINT8  Flags;
  UINT8  Id;
  INT8   Priority;
  UINT8  Reserved[3];
  UINT8  Boards[SUPPORT_BOARD_COUNT];
} H2O_FLASH_MAP_REGION_LOGO_ID;
#pragma pack(pop)

/**
  Get count of entries that FDM recorded

  @param[out] Count             Unsigned integer that specifies the count of entries that FDM recorded.

  @return EFI_SUCCESS           Get region count success.
  @return EFI_NOT_FOUND         Can't find region type in the FDM.
  @return EFI_UNSUPPORTED       Function called in runtime phase

**/
EFI_STATUS
EFIAPI
FdmGetCount (
  OUT  UINT8  *Count
);


/**
  Get Region type record in FDM by specifying order in the list.

  @param[in]   RegionType            GUID that specifies the type of region that search for.
  @param[in]   Instance              Unsigned integer that specifies the N-th Region type instance
  @param[out]  RegionId              Identifier that specifies the identifier of this region..
  @param[out]  RegionOffset          Unsigned integer that specifies the offset of this region relative to the
                                     base of the flash device image.
  @param[out]  RegionSize            Unsigned integer that specifies the region size.
  @param[out]  Attribs               Bitmap that specifies the attributes of the flash device map entry.


  @return EFI_SUCCESS                find region type success.
  @return EFI_INVALID_PARAMETER      Incorrect parameter.
  @return EFI_SECURITY_VIOLATION     Region hash is not correct.
  @return EFI_NOT_FOUND              Can't find region type in the FDM.
  @return EFI_NOT_READY              EFI_HASH_PROTOCOL not installed yet
  @return EFI_UNSUPPORTED            Function called in runtime phase

**/
EFI_STATUS
EFIAPI
FdmGetNAt (
  IN CONST EFI_GUID  *RegionType,
  IN CONST UINT8     Instance,
  OUT UINT8          *RegionId,
  OUT UINT64         *RegionOffset,
  OUT UINT64         *RegionSize,
  OUT UINT32         *Attribs
  );


/**
  Get entry recorded in FDM

  @param[in]   RegionType            GUID that specifies the type of region that N-th entry is.
  @param[out]  Instance              Unsigned integer that specifies entry instance of FDM
  @param[out]  RegionId              Identifier that specifies the identifier of this region..
  @param[out]  RegionOffset          Unsigned integer that specifies the offset of this region relative to the
                                     base of the flash device image.
  @param[out]  RegionSize            Unsigned integer that specifies the region size.
  @param[out]  Attribs               Bitmap that specifies the attributes of the flash device map entry.


  @return EFI_SUCCESS                find region type success.
  @return EFI_INVALID_PARAMETER      Incorrect parameter.
  @return EFI_SECURITY_VIOLATION     Region hash is not correct.
  @return EFI_NOT_FOUND              Can't find region type in the FDM.
  @return EFI_NOT_READY              EFI_HASH_PROTOCOL not installed yet
  @return EFI_UNSUPPORTED            Function called in runtime phase

**/
EFI_STATUS
EFIAPI
FdmGetAt (
  IN CONST UINT8   Instance,
  OUT EFI_GUID     *RegionType,
  OUT UINT8        *RegionId,
  OUT UINT64       *RegionOffset,
  OUT UINT64       *RegionSize,
  OUT UINT32       *Attribs
  );



/**
  Get Address of Region recorded in FDM

  @param[in]   RegionType            GUID that specifies the type of region
  @param[in]   Instance              Unsigned integer that specifies entry instance of Region Type in FDM

  @return      address of the Region.

**/
UINT64
EFIAPI
FdmGetNAtAddr (
  IN CONST EFI_GUID  *RegionType,
  IN UINT8           Instance
);


/**
  Get Size of Region recorded in FDM

  @param[in]   RegionType            GUID that specifies the type of region
  @param[in]   Instance              Unsigned integer that specifies entry instance of Region Type in FDM

  @return      size of the Region.

**/
UINT64
EFIAPI
FdmGetNAtSize (
  IN CONST EFI_GUID  *RegionType,
  IN UINT8           Instance
);


/**
  Get address of Region recorded in FDM by specific Id and region type.

  @param[in]   RegionType            GUID that specifies the type of region.
  @param[in]   Id                    Pointer of value that specifies the Id field must matched.
  @param[in]   Instance              Unsigned integer that specifies instance of Region Type in FDM

  @return      address of the Region.

**/
UINT64
EFIAPI
FdmGetAddressById (
  IN CONST EFI_GUID  *RegionType,
  IN CONST EFI_GUID     *Id,
  IN UINT8           Instance
);


/**
  Get size of Region recorded in FDM by specific Id and region type.

  @param[in]   RegionType            GUID that specifies the type of region.
  @param[in]   Id                    Pointer of value that specifies the Id field must matched.
  @param[in]   Instance              Unsigned integer that specifies instance of Region Type in FDM

  @return      size of the Region.

**/
UINT64
EFIAPI
FdmGetSizeById (
  IN CONST EFI_GUID  *RegionType,
  IN CONST EFI_GUID     *Id,
  IN UINT8           Instance
);



/**
  Get Base Address

  @return    Base Address

**/
UINT64
EFIAPI
FdmGetBaseAddr (
  VOID
 );


/**
  Get Flash Size

  @return    Flash Size

**/
UINT64
EFIAPI
FdmGetFlashAreaSize (
  VOID
);


/**
  Get the count of secific Region in FDM.

  @param[in]   RegionType       GUID that specifies the type of region that search for.
  @param[out]  Count            Unsigned integer that specifies the count of secific Region in FDM.

  @return EFI_SUCCESS           Get region count success.
  @return EFI_NOT_FOUND         Can't find region type in the FDM.
  @return EFI_UNSUPPORTED       Function called in runtime phase

**/
EFI_STATUS
EFIAPI
FdmGetNCount (
  IN CONST EFI_GUID  *RegionType,
  OUT UINT8          *Count
);


/**
  Verify the FV region by checking the hash value in FDM by instance number.

  NOTE: This function will hang if the region has a security issue with the deadloop.

  @param[in]  Instance             Unsigned integer that specifies entry instance of FDM

  @retval EFI_SUCCESS              Region verified successfully.
  @retval EFI_NOT_FOUND            Can't find region type in the FDM.
  @retval EFI_SECURITY_VIOLATION   Region verified failed.

**/
EFI_STATUS
EFIAPI
FdmVerifyAt (
  IN CONST UINT8   Instance
);

/**
  Verify the FV region by checking the hash value in FDM by region type and instance number.

  NOTE: This function will hang if the region has a security issue with the deadloop.

  @param[in]  RegionType          EFI_GUID that specifies the type of region.
  @param[in]  Instance            Unsigned integer that specifies entry instance of FDM

  @retval EFI_SUCCESS             Region verified successfully.
  @retval EFI_NOT_FOUND           Can't find region type in the FDM.
  @retval EFI_SECURITY_VIOLATION  Region verified failed.

**/
EFI_STATUS
EFIAPI
FdmVerifyNAt (
  IN CONST  EFI_GUID  *RegionType,
  IN CONST  UINT8     Instance
  );

/**
  Verify the FV region by checking the hash value in FDM by region type , ID and instance number.

  NOTE: This function will hang if the region has a security issue with the deadloop.

  @param[in]  RegionType          EFI_GUID that specifies the type of region.
  @param[in]  Id                  Pointer of value that specifies the Id field must matched.
  @param[in]  Instance            Unsigned integer that specifies entry instance of FDM

  @retval EFI_SUCCESS             Region verified successfully.
  @retval EFI_NOT_FOUND           Can't find region type in the FDM.
  @retval EFI_SECURITY_VIOLATION  Region verified failed.

**/
EFI_STATUS
EFIAPI
FdmVerifyById (
  IN CONST EFI_GUID  *RegionType,
  IN CONST EFI_GUID  *Id,
  IN UINT8           Instance
  );

#endif
