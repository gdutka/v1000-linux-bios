/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Register Access Methods.
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
 */
#ifndef _FABRIC_REGISTER_ACC_SOC15_LIB_H_
#define _FABRIC_REGISTER_ACC_SOC15_LIB_H_
#pragma pack (push, 1)

/* Fabric Indirect Config Access Address 3 Register */
#define FICAA3_FUNC 0x4
#define FICAA3_REG 0x05C

/// Fabric Indirect Config Access Address 3 Register
typedef union {
  struct {                             ///< Bitfields of Fabric Indirect Config Access Address 3 Register
    UINT32 CfgRegInstAccEn:1;          ///< CfgRegInstAccEn
    UINT32 :1;                         ///< Reserved
    UINT32 IndCfgAccRegNum:9;          ///< IndCfgAccRegNum
    UINT32 IndCfgAccFuncNum:3;         ///< IndCfgAccFuncNum
    UINT32 SixtyFourBitRegEn:1;        ///< SixtyFourBitRegEn
    UINT32 :1;                         ///< Reserved
    UINT32 CfgRegInstID:8;             ///< CfgRegInstID
    UINT32 :8;                         ///< Reserved
  } Field;
  UINT32  Value;
} FABRIC_IND_CFG_ACCESS_ADDR_REGISTER;

/* Fabric Indirect Config Access Data 3 Low Register */
#define FICAD3_LO_FUNC 0x4
#define FICAD3_LO_REG 0x98

/* Fabric Indirect Config Access Data 3 High Register */
#define FICAD3_HI_FUNC 0x4
#define FICAD3_HI_REG 0x9C

#pragma pack (pop)
#endif

