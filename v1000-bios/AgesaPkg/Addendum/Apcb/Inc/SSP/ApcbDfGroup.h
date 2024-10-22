/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _APCB_DF_GROUP_H_
#define _APCB_DF_GROUP_H_


#if !APCB_V3_UID_USED
#define   DF_DRAM_INTLV_SIZE_256   0
#define   DF_DRAM_INTLV_SIZE_512   1
#define   DF_DRAM_INTLV_SIZE_1024  2
#define   DF_DRAM_INTLV_SIZE_2048  3

#define   DF3_XGMI2_LINK_CFG_2LINK  0
#define   DF3_XGMI2_LINK_CFG_3LINK  1
#define   DF3_XGMI2_LINK_CFG_4LINK  2
#endif

#define  DF_DRAM_NPS0         0
#define  DF_DRAM_NPS1         1
#define  DF_DRAM_NPS2         2
#define  DF_DRAM_NPS4         3


//
// DF Type ID
//
#define APCB_DF_TYPE_XGMI_TX_EQ                     0x00D0
#define   APCB_DF_TYPE_XGMI_INSTANCE_SPEED_COMBINED   0   // Instance ID 0
#define   APCB_DF_TYPE_XGMI_INSTANCE_SPEED_COMBINED_1 1   // Instance ID 1
#include "ApcbDfXgmiTxEq.h"

#endif // _APCB_DF_GROUP_H_
