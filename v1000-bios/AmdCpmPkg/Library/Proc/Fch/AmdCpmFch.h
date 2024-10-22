/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_FCH_H_
#define _AMD_CPM_FCH_H_

UINT8
CpmGetGpio (
  IN       VOID                        *This,
  IN       UINT16                      Pin
  );

VOID
CpmSetGpio (
  IN       VOID                        *This,
  IN       UINT16                      Pin,
  IN       UINT16                      Value
  );

VOID
CpmSetGevent (
  IN       VOID                        *This,
  IN       UINT16                      Pin,
  IN       UINT16                      Value
  );

UINT32
CpmGetStrap (
  IN       VOID                        *This
  );

VOID
CpmSetClkReq (
  IN       VOID                        *This,
  IN       UINT8                       Socket,
  IN       UINT8                       Die,
  IN       UINT8                       Bridge,
  IN       UINT8                       Miscid,
  IN       UINT8                       ClkId,
  IN       UINT8                       ClkReq
  );

UINT8
CpmGetClkReq (
  IN       VOID                        *This,
  IN       UINT8                       Socket,
  IN       UINT8                       Die,
  IN       UINT8                       Bridge,
  IN       UINT8                       Miscid,
  IN       UINT8                       ClkId
  );

VOID
CpmStall (
  IN       VOID                        *This,
  IN       UINT32                      Delay
  );

VOID
CpmSetFanOn (
  IN       VOID                        *This,
  IN       UINT8                       Fan
  );

VOID
EFIAPI
CpmRegisterFch (
  IN       VOID                        *This
  );

UINT8
CpmGetBootMode (
  );
#endif // _AMD_CPM_FCH_H_
