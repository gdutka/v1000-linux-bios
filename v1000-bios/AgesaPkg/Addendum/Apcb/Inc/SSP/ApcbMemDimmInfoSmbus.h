/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _APCB_MEM_DIMM_INFO_SMBUS_H_
#define _APCB_MEM_DIMM_INFO_SMBUS_H_
//
// @APCB_START
//

///
/// Memory Board Layout Info
///
#define MEMORY_LAYOUT_TYPE_DIMM           1
#define MEMORY_LAYOUT_TYPE_ONBOARD        2


//-----------------------------------------------------------------------------
///
/// SPD Data for each DIMM.
///
/** DIMM_INFO_SMBUS
*    @GROUPID: APCB_GROUP_MEMORY
*    @TYPEID: APCB_MEM_TYPE_DIMM_INFO_SMBUS
*    This data structure is used to pass wrapper parameters to the memory configuration code
*    regarding Memory DIMM SMUBS information
*/
typedef struct _DIMM_INFO {
  UINT8  DimmSlotPresent;     ///< Indicates that the DIMM is present and Data is valid
  UINT8  SocketId;            ///< Indicates the socket number
  UINT8  ChannelId;           ///< Indicates the channel number
  UINT8  DimmId;              ///< Indicates the channel number
  UINT8  DimmSmbusAdderess;   ///< SMBus address of the DRAM
  UINT8  I2CMuxAddress;       ///< I2C Mux Address
  UINT8  MuxControlAddress;
  UINT8  MuxChannel;          ///< I2C Mux Channel assocaited iwth this SPD
} DIMM_INFO_SMBUS;

#endif // _APCB_MEM_DIMM_INFO_SMBUS_H_

