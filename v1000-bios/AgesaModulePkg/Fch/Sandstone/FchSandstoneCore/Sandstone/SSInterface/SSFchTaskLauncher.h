/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH task launcher
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/
#ifndef  _TS_FCH_TASK_LAUNCHER_H_
#define  _TS_FCH_TASK_LAUNCHER_H_


AGESA_STATUS
FchTaskLauncher (
  IN       FCH_TASK_ENTRY     **TaskPtr,
  IN       VOID               *FchCfg,
  IN       AGESA_POST_CODE           TestPoint
  );

#endif

