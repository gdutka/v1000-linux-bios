/* $NoKeywords:$ */
/**
 * @file
 *
 * APCB.h
 *
 * AGESA PSP Output Block
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (APOB)
 * @e \$Revision$ @e \$Date$
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#ifndef _APOB_CMN_H_
#define _APOB_CMN_H_

//Common structure across programs

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
#define APOB_SIGNATURE        0x424F5041  //!!APOB
/// AMD APOB_HMAC
#define APOB_HMAC_SIZE 32

#define MAX_APOB_ERROR_ENTRIES  64
// APOB Group Definitons
#define APOB_MEM      1
#define APOB_DF       2
#define APOB_CCX      3
#define APOB_GNB      4
#define APOB_FCH      5
#define APOB_PSP      6
#define APOB_GEN      7
#define APOB_SMBIOS   8
#define APOB_FABRIC   9
// APOB Type Definitons
#define APOB_MEM_GENERAL_ERRORS_TYPE   1
#define APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE   2
#define APOB_GEN_CONFIGURATION_INFO_TYPE   3
#define APOB_GEN_S3_REPLAY_BUFFER_INFO_TYPE   4
#define APOB_MEM_PMU_SMB_TYPE   5
#define APOB_GEN_EVENT_LOG_TYPE   6
#define APOB_MEM_DIMM_SMBUS_INFO_TYPE   7
#define APOB_MEM_SMBIOS_TYPE  8
#define APOB_SYS_MAP_INFO_TYPE   9
#define APOB_MEM_NVDIMM_INFO_TYPE   15
#define APOB_APCB_BOOT_INFO_TYPE    16
#define APOB_MEM_DIMM_SPD_DATA_TYPE     17
#define APOB_MEM_MBIST_RESULT_INFO_TYPE 18
#define APOB_SYS_NPS_INFO_TYPE   19
#define APOB_SYS_SLINK_INFO_TYPE   20
#define APOB_DF_DXIO_PHY_FW_OVERRIDE_INFO_TYPE 21
#define APOB_MEM_PMU_TRAINING_FAILURE_INFO_TYPE 22
#define APOB_ENV_FLAGS_INFO_TYPE 23
#define APOB_MEM_S3_DDR_PHY_REPLAY_PHASE0_BUFFER_INFO_TYPE   30
#define APOB_MEM_S3_DDR_PHY_REPLAY_MAX_ENTRIES               10
// APOB CCX Type Definitons
#define APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE  1
#define APOB_CCX_EDC_THROTTLE_THRESH_TYPE      2
#define APOB_CCD_LOGICAL_TO_PHYSICAL_MAP_TYPE  3

typedef struct {
  UINT8  ApobHmac[APOB_HMAC_SIZE];                      ///< APOB HMAC for secutity check of each type
} APOB_HMAC;

/// AMD APOB_HEADER Header
typedef struct {
  UINT32   Signature;                 ///< APOB signature
  UINT32   Version;                   ///< Version
  UINT32   Size;                      ///< APOB Size
  UINT32   OffsetOfFirstEntry;        ///< APOB Header Size
} APOB_BASE_HEADER;


/// AMD APOB_HEADER Header
typedef struct {
  UINT32  GroupID;                          ///< Group ID
  UINT32  DataTypeID;                       ///< Data Type ID
  UINT32  InstanceID;                       ///< Instance ID
                                            ///< - Bit 15:0 - SocketID
                                            ///< - Bit  8:0 - DieID
  UINT32  TypeSize;                         ///< Type Size (including header)
  APOB_HMAC ApobTypeHmac;                   ///< Hmac location for type
} APOB_TYPE_HEADER;

///
/// APOB SPD Data for each DIMM.
///
typedef struct _APOB_SPD_STRUCT {
  UINT8  SocketNumber;          ///< Indicates the socket number
  UINT8  ChannelNumber;          ///< Indicates the channel number
  UINT8  DimmNumber;          ///< Indicates the channel number
  UINT8   PageAddress;  ///< Indicates the 256 Byte EE Page the data belongs to
                        //      0 = Lower Page
                        //      1 = Upper Page
  BOOLEAN DimmPresent;   //< Indicates if the DIMM is present
  UINT8  MuxPresent;      ///< SpdMux Present or not. if 1, then yes otherwise no
  UINT8  MuxI2CAddress;      ///< MuxI2cAddress
  UINT8  MuxChannel;         ///< MuxChannel no.
  UINT32 Address;         ///< SMBus address of the DRAM
  UINT32 SerialNumber;   ///< DIMM Serial Number
  UINT32 DeviceWidth:3;         ///< Device Width i.e. x4, x8, x16 and x32
  UINT32 DpprSupported:2;       ///< Dppr Support Present
  UINT32 SpprSupported:1;       ///< Sppd Support Present
  UINT32 Reserved:26;           ///< Reserved for Future Use
  UINT8 Data[512];      ///< Buffer for 256 Bytes of SPD data from DIMM
} APOB_SPD_STRUCT;

#define  EDC_THROTTLE_TYPE_INVALID  0
#define  EDC_THROTTLE_TYPE_L3       1
#define  EDC_THROTTLE_TYPE_CORE     2
#define  EDC_THROTTLE_TYPE_FP       3
#define  MAX_NUMBER_OF_THROTTLERS   3
#define  CCX_NOT_PRESENT (0xFF)

typedef struct {
  UINT32   Type;
  UINT32   OpnValue;
  UINT32   AblProgrammedValue;
} EDC_THROTTLE_THRESHOLD;

/// Event log entry
typedef struct {
  UINT32    EventClass;   ///< The severity of the event, its associated AGESA_STATUS.
  UINT32    EventInfo;        ///< Uniquely identifies the event.
  UINT32    DataA;
  UINT32    DataB;
} APOB_ERROR_LOG;
typedef struct {
  APOB_ERROR_LOG AgesaEvent[MAX_APOB_ERROR_ENTRIES];    ///< The entry itself.
} AGESA_EVENT_STRUCT;
typedef struct {
  APOB_TYPE_HEADER  ApobTypeHeader;    ///< APOB Type Header
  UINT16        Count;           ///< The total number of active entries.
  AGESA_EVENT_STRUCT ApobEventStruct;   ///< The entries.
} EVENT_LOG_STRUCT;
#endif  /* _APOB_CMN_H_ */

