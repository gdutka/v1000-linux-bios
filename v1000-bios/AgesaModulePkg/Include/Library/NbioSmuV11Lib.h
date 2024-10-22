/* $NoKeywords:$ */
/**
 * @file
 *
 * SMU services
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
#ifndef _NBIOSMUV11LIB_H_
#define _NBIOSMUV11LIB_H_

//
// Assume standard IA-32 alignment.
//
typedef unsigned long long  uint64_t;
typedef long long           int64_t;
typedef unsigned int        uint32_t;
typedef int                 int32_t;
typedef unsigned short      uint16_t;
typedef short               int16_t;
typedef unsigned char       uint8_t;
typedef signed char         int8_t;


// SMU Response Codes:
#define BIOSSMC_Result_OK                    0x1
#define BIOSSMC_Result_Failed                0xFF
#define BIOSSMC_Result_UnknownCmd            0xFE
#define BIOSSMC_Result_CmdRejectedPrereq     0xFD
#define BIOSSMC_Result_CmdRejectedBusy       0xFC

#define INVALID_SMU_MESSAGE                  0xFF

// Message Definitions:
#define BIOSSMC_MSG_TestMessage                         0x1
#define BIOSSMC_MSG_GetSmuVersion                       0x2
#define BIOSSMC_MSG_EnableSmuFeatures                   0x3
#define BIOSSMC_MSG_DisableSmuFeatures                  0x4
#define BIOSSMC_MSG_SetBiosDramAddr                     0x5
#define BIOSSMC_MSG_SetToolsDramAddr                    0x6
#define BIOSSMC_MSG_SetDebugDramAddr                    0x7
#define BIOSSMC_MSG_DxioTestMessage                     0x8
#define BIOSSMC_MSG_ReadCoreCacWeightRegister           0x9
#define BIOSSMC_MSG_UsbInit                             0xA
#define BIOSSMC_MSG_SleepEntry                          0xB
#define BIOSSMC_MSG_DcBtc                               0xC
#define BIOSSMC_MSG_GetNameString                       0xD
#define BIOSSMC_MSG_GetClusterOnDieMode                 0xE
#define BIOSSMC_MSG_GetPerSrcBistPF                     0xF
#define BIOSSMC_MSG_TransferBiosIfTableToSmu            0x10
#define BIOSSMC_MSG_TransferPcieLaneMappingTableToSmu   0x11
#define BIOSSMC_MSG_TransferPcieHotPlugConfigTableToSmu INVALID_SMU_MESSAGE //0x12
#define BIOSSMC_MSG_UsbConfigUpdate                     0x13
#define BIOSSMC_MSG_UsbD3Entry                          0x14
#define BIOSSMC_MSG_UsbD3Exit                           0x15
#define BIOSSMC_Message_Count                           0x16


#define BIOSSMC_MSG_EnableHotPlug              INVALID_SMU_MESSAGE
#define BIOSSMC_MSG_SetI2CBusSwitchBaseAddress INVALID_SMU_MESSAGE

typedef struct {
  uint16_t Frequency; //MHz
  uint16_t LogicalCoreIndex;
} BIOSSMC_MSG_SetBoostLimitFrequency_t;

typedef union {
  uint32_t val;
  BIOSSMC_MSG_SetBoostLimitFrequency_t f;
} BIOSSMC_MSG_SetBoostLimitFrequency_u;

typedef enum {
  CLUSTER_ON_DIE_MODE_COD_1,
  CLUSTER_ON_DIE_MODE_COD_1_2,
  CLUSTER_ON_DIE_MODE_COD_1_2_4
} CLUSTER_ON_DIE_MODE_e;

// Address
#define MP1_C2PMSG_MESSAGE_ADDRESS                         0x3B10530ul

// Type
#define MP1_C2PMSG_MESSAGE_TYPE                            TYPE_SMN

#define MP1_C2PMSG_MESSAGE_CONTENT_OFFSET                  0
#define MP1_C2PMSG_MESSAGE_CONTENT_WIDTH                   32
#define MP1_C2PMSG_MESSAGE_CONTENT_MASK                    0xffffffffUL

/// MP1_C2PMSG_MESSAGE
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_MESSAGE_STRUCT;

// Address
#define MP1_C2PMSG_RESPONSE_ADDRESS                        0x3B1057Cul

// Type
#define MP1_C2PMSG_RESPONSE_TYPE                           TYPE_SMN

#define MP1_C2PMSG_RESPONSE_CONTENT_OFFSET                 0
#define MP1_C2PMSG_RESPONSE_CONTENT_WIDTH                  32
#define MP1_C2PMSG_RESPONSE_CONTENT_MASK                   0xffffffffUL

/// MP1_C2PMSG_RESPONSE
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_RESPONSE_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_0_ADDRESS                      0x3B109C4ul

// Type
#define MP1_C2PMSG_ARGUMENT_0_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_0_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_0_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_0_CONTENT_MASK                 0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_0
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_0_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_1_ADDRESS                      0x3B109C8ul

// Type
#define MP1_C2PMSG_ARGUMENT_1_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_1_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_1_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_1_CONTENT_MASK                 0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_1
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_1_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_2_ADDRESS                      0x3B109CCUL

// Type
#define MP1_C2PMSG_ARGUMENT_2_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_2_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_2_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_2_CONTENT_MASK                 0xffffffff

/// MP1_C2PMSG_ARGUMENT_2
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_2_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_3_ADDRESS                      0x3B109D0UL

// Type
#define MP1_C2PMSG_ARGUMENT_3_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_3_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_3_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_3_CONTENT_MASK                 0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_3
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_3_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_4_ADDRESS                      0x3B109D4ul

// Type
#define MP1_C2PMSG_ARGUMENT_4_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_4_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_4_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_4_CONTENT_MASK                 0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_4
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_4_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_5_ADDRESS                      0x3B109D8ul

// Type
#define MP1_C2PMSG_ARGUMENT_5_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_5_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_5_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_5_CONTENT_MASK                 0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_5
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_5_STRUCT;

/// SMU TDP contect
typedef struct {
  UINT32            SmuTdp;        ///< TDP returned by SMU
  UINT32            cTdp;          ///< Config TDP
} SMU_TDP_INFO;

/// SMU V11 basic lib

VOID
NbioSmuServiceCommonInitArgumentsV11 (
  IN OUT   UINT32                   *SmuArg
  );

UINT32
NbioSmuServiceRequestV11 (
  IN       PCI_ADDR                 NbioPciAddress,
  IN       UINT32                   RequestId,
  IN OUT   UINT32                   *RequestArgument,
  IN       UINT32                   AccessFlags
  );

AGESA_STATUS
NbioSmuFirmwareTestV11 (
  IN       GNB_HANDLE               *NbioHandle
  );

AGESA_STATUS
NbioDxioServiceRequestV11 (
  IN       GNB_HANDLE               *NbioHandle,
  IN       UINT32                   RequestId,
  IN OUT   UINT32                   *RequestArgument,
  IN       UINT32                   AccessFlags
  );

VOID
SmuNotifyS3Entry (
  );

#endif

