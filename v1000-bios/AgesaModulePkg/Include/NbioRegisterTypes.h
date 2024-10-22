/* $NoKeywords:$ */
/**
 * @file
 *
 * NBIO Register Type Definitions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _NBIOREGISTERTYPES_H_
#define _NBIOREGISTERTYPES_H_

#define  TYPE_D0F0                  0x1
#define  TYPE_PCI                   TYPE_D0F0
#define  TYPE_D0F0x64               0x2
#define  TYPE_D0F0x98               0x3
#define  TYPE_D0F0xBC               0x4
#define  TYPE_SMN                   TYPE_D0F0xBC
#define  TYPE_NBIOAPICCFG           TYPE_SMN
#define  TYPE_IOHC                  TYPE_SMN
#define  TYPE_NBMISC                TYPE_SMN
#define  TYPE_PWR                   TYPE_SMN

#endif

