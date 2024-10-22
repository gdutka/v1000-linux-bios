/* $NoKeywords:$ */
/**
 * @file
 *
 * SnpDxeConfig.h
 *
 * Ethernet Driver DXE-Phase configuration file.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: FDK
 * @e sub-project: UEFI
 * @e version: $Revision: 298339 $ @e date: $Date: 2014-07-14 16:27:48 -0700 (Mon, 14 Jul 2014) $
 *
 */
/*
 ****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef __SNP_DXE_CONFIG_H__
#define __SNP_DXE_CONFIG_H__


//Main switch for using MAC_ADDRESS_STR as debugging MAC address
#define MAC_ADDRESS_DEBUG 0

  #if MAC_ADDRESS_DEBUG
//MAC address string for debug
#define MAC_ADDRESS0_STR "001122334455"
#define MAC_ADDRESS1_STR "000102030405"
  #endif


#endif  //__SNP_DXE_CONFIG_H__

