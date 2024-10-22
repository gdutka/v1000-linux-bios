/* $NoKeywords:$ */
/**
 * @file
 *
 * Base Fabric Pstates Lib implementation for UNB
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _FABRIC_PSTATES_UNB_LIB_H_
#define _FABRIC_PSTATES_UNB_LIB_H_

#include "Porting.h"
#include <UnbRegisters.h>
#pragma pack (push, 1)

/// Save and Restore Nb Pstate Control Register
typedef struct {
  BOOLEAN                 Valid;          ///< Valid
  NB_PSTATE_CTRL_REGISTER NbPstateCtrl;   ///< NB Pstate Control
} SAVE_NB_PSTATE_CTRL_REG;

#pragma pack (pop)
#endif // _FABRIC_PSTATES_UNB_LIB_H_
