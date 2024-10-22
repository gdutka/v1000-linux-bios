/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Register Access library
 *
 * Contains interface to the fabric register access library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _FABRIC_REGISTER_ACC_LIB_H_
#define _FABRIC_REGISTER_ACC_LIB_H_

#define FABRIC_REG_ACC_BC    (0xFF)

UINT32
FabricRegisterAccRead (
  IN       UINTN  Socket,
  IN       UINTN  Die,
  IN       UINTN  Function,
  IN       UINTN  Offset,
  IN       UINTN  Instance
  );

VOID
FabricRegisterAccWrite (
  IN       UINTN    Socket,
  IN       UINTN    Die,
  IN       UINTN    Function,
  IN       UINTN    Offset,
  IN       UINTN    Instance,
  IN       UINT32   Value,
  IN       BOOLEAN  LogForS3
  );

UINT32
FabricRegisterAccRMW (
  IN       UINTN    Socket,
  IN       UINTN    Die,
  IN       UINTN    Function,
  IN       UINTN    Offset,
  IN       UINTN    Instance,
  IN       UINT32   NandValue,
  IN       UINT32   OrValue,
  IN       BOOLEAN  LogForS3
  );

#endif // _FABRIC_REGISTER_ACC_LIB_H_

