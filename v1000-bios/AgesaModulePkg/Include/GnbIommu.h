/**
 * @file
 *
 * Misc common definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
#ifndef _GNBIOMMU_H_
#define _GNBIOMMU_H_

#pragma pack (push, 1)


/// IVRS block
typedef enum {
  IvrsIvhdBlock10h            = 0x10,       ///< I/O Virtualization Hardware Definition Block
  IvrsIvhdBlock11h            = 0x11,       ///< I/O Virtualization Hardware Definition Block
  IvrsIvmdBlock               = 0x20,       ///< I/O Virtualization Memory Definition Block for all peripherals
  IvrsIvmdBlockSingle         = 0x21,       ///< IVMD block for specified peripheral
  IvrsIvmdBlockRange          = 0x22,       ///< IVMD block for peripheral range
  IvrsIvhdrBlock40h           = 0x40,       ///< IVHDR (Relative) block
  IvrsIvhdrBlock41h           = 0x41,       ///< IVHDR (Relative) block
  IvrsIvmdrBlock              = 0x50,       ///< IVMDR (Relative) block for all peripherals
  IvrsIvmdrBlockSingle        = 0x51        ///< IVMDR block for last IVHDR
} IVRS_BLOCK_TYPE;

#define DEVICE_ID(PciAddress) (UINT16) (((PciAddress).Address.Bus << 8) | ((PciAddress).Address.Device << 3) | (PciAddress).Address.Function)

/// IVHD entry types
typedef enum {
  IvhdEntryPadding            =  0,         ///< Table padding
  IvhdEntrySelect             =  2,         ///< Select
  IvhdEntryStartRange         =  3,         ///< Start Range
  IvhdEntryEndRange           =  4,         ///< End Range
  IvhdEntryAliasSelect        =  66,        ///< Alias select
  IvhdEntryAliasStartRange    =  67,        ///< Alias start range
  IvhdEntryExtendedSelect     =  70,        ///< Extended select
  IvhdEntryExtendedStartRange =  71,        ///< Extended Start range
  IvhdEntrySpecialDevice      =  72,        ///< Special device
  IvhdEntryF0Type             =  240        /// Type 40h Fields
} IVHD_ENTRY_TYPE;

/// Special device variety
typedef enum {
  IvhdSpecialDeviceIoapic     = 0x1,        ///< IOAPIC
  IvhdSpecialDeviceHpet       = 0x2         ///< HPET
} IVHD_SPECIAL_DEVICE;


#define IVHD_FLAG_PPRSUB            BIT7
#define IVHD_FLAG_PREFSUP           BIT6
#define IVHD_FLAG_COHERENT          BIT5
#define IVHD_FLAG_IOTLBSUP          BIT4
#define IVHD_FLAG_ISOC              BIT3
#define IVHD_FLAG_RESPASSPW         BIT2
#define IVHD_FLAG_PASSPW            BIT1
#define IVHD_FLAG_HTTUNEN           BIT0

#define IVHD_EFR_XTSUP_OFFSET       0
#define IVHD_EFR_NXSUP_OFFSET       1
#define IVHD_EFR_GTSUP_OFFSET       2
#define IVHD_EFR_GLXSUP_OFFSET      3
#define IVHD_EFR_IASUP_OFFSET       5
#define IVHD_EFR_GASUP_OFFSET       6
#define IVHD_EFR_HESUP_OFFSET       7
#define IVHD_EFR_PASMAX_OFFSET      8
#define IVHD_EFR_PNCOUNTERS_OFFSET  13
#define IVHD_EFR_PNBANKS_OFFSET     17
#define IVHD_EFR_MSINUMPPR_OFFSET   23
#define IVHD_EFR_GATS_OFFSET        28
#define IVHD_EFR_HATS_OFFSET        30

#define IVINFO_HTATSRESV_MASK       0x00400000ul
#define IVINFO_VASIZE_MASK          0x003F8000ul
#define IVINFO_PASIZE_MASK          0x00007F00ul
#define IVINFO_GASIZE_MASK          0x000000E0ul
#define IVINFO_EFRSUP_MASK          0x00000001ul

#define IVHD_INFO_MSINUM_OFFSET     0
#define IVHD_INFO_UNITID_OFFSET     8

#define IVMD_FLAG_EXCLUSION_RANGE   BIT3
#define IVMD_FLAG_IW                BIT2
#define IVMD_FLAG_IR                BIT1
#define IVMD_FLAG_UNITY             BIT0

/// IVRS header
typedef struct {
  UINT8   Sign[4];           ///< Signature
  UINT32  TableLength;       ///< Table Length
  UINT8   Revision;          ///< Revision
  UINT8   Checksum;          ///< Checksum
  UINT8   OemId[6];          ///< OEM ID
  UINT8   OemTableId[8];     ///< OEM Tabled ID
  UINT32  OemRev;            ///< OEM Revision
  UINT8   CreatorId[4];      ///< Creator ID
  UINT32  CreatorRev;        ///< Creator Revision
  UINT32  IvInfo;            ///< IvInfo
  UINT64  Reserved;          ///< Reserved
} IOMMU_IVRS_HEADER;

/// IVRS IVHD Entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT8   Flags;              ///< Flags
  UINT16  Length;             ///< Length
  UINT16  DeviceId;           ///< DeviceId
  UINT16  CapabilityOffset;   ///< CapabilityOffset
  UINT64  BaseAddress;        ///< BaseAddress
  UINT16  PciSegment;         ///< Pci segment
  UINT16  IommuInfo;          ///< IOMMU info
} IVRS_IVHD_ENTRY;

/// IVRS IVHD Entry
typedef struct {
  IVRS_IVHD_ENTRY Ivhd;       ///< Ivhd
  UINT32          IommuEfr;   ///< Extended Features Register
} IVRS_IVHD_ENTRY_10H;

/// IVRS IVHD Entry
typedef struct {
  IVRS_IVHD_ENTRY Ivhd;               ///< Ivhd
  UINT32          IommuAttributes;    ///< Attributes
  UINT64          IommuEfr;           ///< Extended Features Register
  UINT64          Reserved;           ///< reserved
} IVRS_IVHD_ENTRY_11H;

/// IVRS IVHD Entry
typedef struct {
  IVRS_IVHD_ENTRY Ivhd;               ///< Ivhd
  UINT32          IommuAttributes;    ///< Attributes
  UINT64          IommuEfr;           ///< Extended Features Register
  UINT64          Reserved;           ///< reserved
} IVRS_IVHD_ENTRY_40H;

/// IVHD generic entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  DeviceId;           ///< Device id
  UINT8   DataSetting;        ///< Data settings
} IVHD_GENERIC_ENTRY;

///IVHD alias entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  DeviceId;           ///< Device id
  UINT8   DataSetting;        ///< Data settings
  UINT8   Reserved;           ///< Reserved
  UINT16  AliasDeviceId;      ///< Alias device id
  UINT8   Reserved2;          ///< Reserved
} IVHD_ALIAS_ENTRY;

///IVHD extended entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  DeviceId;           ///< Device id
  UINT8   DataSetting;        ///< Data settings
  UINT32  ExtSetting;         ///< Extended settings
} IVHD_EXT_ENTRY;

/// IVHD special entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  Reserved;           ///< Reserved
  UINT8   DataSetting;        ///< Data settings
  UINT8   Handle;             ///< Handle
  UINT16  AliasDeviceId;      ///< Alis device id
  UINT8   Variety;            ///< Variety
} IVHD_SPECIAL_ENTRY;

/// IVHD special entry
typedef struct {
  UINT8   IdByte0;            ///< ID Byte0
  UINT8   IdByte1;            ///< ID Byte1
  UINT8   IdByte2;            ///< ID Byte2
  UINT8   IdByte3;            ///< ID Byte3
  UINT8   IdByte4;            ///< ID Byte4
  UINT8   IdByte5;            ///< ID Byte5
  UINT8   IdByte6;            ///< ID Byte6
  UINT8   IdByte7;            ///< ID Byte7
} IVHD_ID;

/// IVHD special entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  DeviceId;           ///< DeviceID
  UINT8   DataSetting;        ///< Data settings
  IVHD_ID HardwareId;         ///< ACPI Hardware ID
  IVHD_ID CompatibleId;       ///< ACPI Compatible ID
  UINT8   UidFormat;          ///< Unique ID Format
  UINT8   UidLength;          ///< Unique ID Length
} IVHD_TYPEF0_ENTRY;

/// IVRS IVMD Entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT8   Flags;              ///< Flags
  UINT16  Length;             ///< Length
  UINT16  DeviceId;           ///< DeviceId
  UINT16  AuxiliaryData;      ///< Auxiliary data
  UINT64  Reserved;           ///< Reserved (0000_0000_0000_0000)
  UINT64  BlockStart;         ///< IVMD start address
  UINT64  BlockLength;        ///< IVMD memory block length
} IVRS_IVMD_ENTRY;

/// MMIO Offset 0x30
typedef union {
  struct {                                           ///<
    UINT64                PreFSup:1;                 ///<
    UINT64                PPRSup:1;                  ///<
    UINT64                XTSup:1;                   ///<
    UINT64                NXSup:1;                   ///<
    UINT64                GTSup:1;                   ///<
    UINT64                EFRignored:1;              ///<
    UINT64                IASup:1;                   ///<
    UINT64                GASup:1;                   ///<
    UINT64                HESup:1;                   ///<
    UINT64                PCSup:1;                   ///<
    UINT64                HATS:2;                    ///<
    UINT64                GATS:2;                    ///<
    UINT64                GLXSup:2;                  ///<
    UINT64                SmiFSup:2;                 ///<
    UINT64                SmiFRC:3;                  ///<
    UINT64                GAMSup:3;                  ///<
    UINT64                Reserved_31_24:8;          ///<
    UINT64                PASmax:5;                  ///<
    UINT64                Reserved_63_37:27;         ///<
  } Field;

  UINT64 Value;
} MMIO_0x30;

/// MMIO Offset 0x18
typedef union {
  struct {                                           ///<
    UINT64                IommuEn:1;                 ///<
    UINT64                HtTunEn:1;                 ///<
    UINT64                Field_7_2:6;               ///<
    UINT64                PassPW:1;                  ///<
    UINT64                ResPassPW:1;               ///<
    UINT64                Coherent:1;                ///<
    UINT64                Isoc:1;                    ///<
    UINT64                Field_63_12:52;            ///<
  } Field;

  UINT64 Value;
} MMIO_0x18;

/// MMIO Offset 0x4000
typedef union {
  struct {                                           ///<
    UINT64               Reserved_6_0:7;             ///<
    UINT64               NCounter:4;                 ///<
    UINT64               Reserved_11:1;              ///<
    UINT64               NCounterBanks:6;            ///<
    UINT64               Reserved_63_18:46;          ///<
  } Field;

  UINT64 Value;
} MMIO_0x4000;

/// Capability offset 0
typedef union {
  struct {                                           ///<
    UINT32                IommuCapId:8;              ///<
    UINT32                IommuCapPtr:8;             ///<
    UINT32                IommuCapType:3;            ///<
    UINT32                IommuCapRev:5;             ///<
    UINT32                IommuIoTlbsup:1;           ///<
    UINT32                IommuHtTunnelSup:1;        ///<
    UINT32                IommuNpCache:1;            ///<
    UINT32                IommuEfrSup:1;             ///<
    UINT32                Reserved_31_28:4;          ///<
  } Field;

  UINT32 Value;
} CAPABILITY_REG;

#pragma pack (pop)

#endif

