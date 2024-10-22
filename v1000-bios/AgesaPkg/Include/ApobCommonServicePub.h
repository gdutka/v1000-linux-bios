/* $NoKeywords:$ */
/**
 * @file
 *
 * APOB Base Library
 *
 * Contains interface to the APOB library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  APOB
 * @e \$Revision: 317842 $   @e \$Date: 2015-04-29 15:06:52 +0800 (Wed, 29 Apr 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _APOB_COMMON_SERVICE_PUB_H_
#define _APOB_COMMON_SERVICE_PUB_H_


#ifndef MAKE_APOB_INSTANCE_ID
  #define MAKE_APOB_INSTANCE_ID(Socket, Die) (((UINT32) (Socket & 0x000000FF) << 8) | (UINT32) (Die & 0x000000FF))
#endif

typedef enum
{
  MemCtrllerProcOdtDddr4Current,
  MemCtrllerRttNomDddr4Current,
  MemCtrllerRttWrDddr4Current,
  MemCtrllerRttParkDddr4Current,
  MemCtrllerAddrCmdSetupDddr4Current,
  MemCtrllerCsOdtSetupDddr4Current,
  MemCtrllerCkeSetupDddr4Current,
  MemCtrllerCadBusAddrCmdDrvStrenDddr4Current,
  MemCtrllerCadBusClkDrvStrenDddr4Current,
  MemCtrllerCsOdtCmdDrvStrenDddr4Current,
  MemCtrllerCkeDrvStrenDddr4Current,
  InterLeaveCapability,
  InterLeaveCurrentMode,
  InterLeaveSize,
} APOB_MEM_GEN_INFO_ELEMENT_ID;

#endif //_APOB_COMMON_SERVICE_PUB_H_

