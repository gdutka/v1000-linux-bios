/* $NoKeywords:$ */
/**
 * @file
 *
 * ma.h
 *
 * ARDK common header file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#ifndef _MA_H_
#define _MA_H_

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


#define MAX_CS_PER_CHANNEL      8   ///< Max CS per channel
/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/** MARDK Structure*/
typedef struct {
  UINT16   Speed;                  ///< Dram speed in MHz
  UINT8    Loads;                  ///< Number of Data Loads
  UINT32   AddrTmg;                ///< Address Timing value
  UINT32   Odc;                    ///< Output Driver Compensation Value
} PSCFG_ENTRY;

/** MARDK Structure*/
typedef struct {
  UINT16   Speed;                  ///< Dram speed in MHz
  UINT8    Loads;                  ///< Number of Data Loads
  UINT32   AddrTmg;                ///< Address Timing value
  UINT32   Odc;                    ///< Output Driver Compensation Value
  UINT8    Dimms;                  ///< Number of Dimms
} ADV_PSCFG_ENTRY;

/** MARDK Structure for RDIMMs*/
typedef struct {
  UINT16   Speed;                  ///< Dram speed in MHz
  UINT16   DIMMRankType;         ///< Bitmap of Ranks   //Bit0-3:DIMM0(1:SR, 2:DR, 4:QR, 0:No Dimm, 0xF:Any), Bit4-7:DIMM1, Bit8-11:DIMM2, Bit12-16:DIMM3
  UINT32   AddrTmg;                ///< Address Timing value
  UINT16   RC2RC8;                   ///< RC2 and RC8 value  //High byte: 1st pair value, Low byte: 2nd pair value
  UINT8    Dimms;                  ///< Number of Dimms
} ADV_R_PSCFG_ENTRY;

/** MARDK Structure*/
typedef struct {
  UINT16   DIMMRankType;         ///< Bitmap of Ranks   //Bit0-3:DIMM0(1:SR, 2:DR, 4:QR, 0:No Dimm, 0xF:Any), Bit4-7:DIMM1, Bit8-11:DIMM2, Bit12-16:DIMM3
  UINT32   PhyRODTCSLow;          ///< Fn2_9C 180
  UINT32   PhyRODTCSHigh;         ///< Fn2_9C 181
  UINT32   PhyWODTCSLow;          ///< Fn2_9C 182
  UINT32   PhyWODTCSHigh;         ///< Fn2_9C 183
  UINT8    Dimms;                  ///< Number of Dimms
} ADV_PSCFG_ODT_ENTRY;

/** MARDK Structure for Write Levelization ODT*/
typedef struct {
  UINT16   DIMMRankType;         ///< Bitmap of Ranks   //Bit0-3:DIMM0(1:SR, 2:DR, 4:QR, 0:No Dimm, 0xF:Any), Bit4-7:DIMM1, Bit8-11:DIMM2, Bit12-16:DIMM3
  UINT8    PhyWrLvOdt[MAX_CS_PER_CHANNEL / 2];            ///< WrLvOdt (Fn2_9C_0x08[11:8]) Value for each Dimm
  UINT8    Dimms;                ///< Number of Dimms
} ADV_R_PSCFG_WL_ODT_ENTRY;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

AGESA_STATUS
MemAGetPsCfgDef (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

UINT16
MemAGetPsRankType (
  IN       CH_DEF_STRUCT *CurrentChannel
  );

#endif  /* _MA_H_ */

