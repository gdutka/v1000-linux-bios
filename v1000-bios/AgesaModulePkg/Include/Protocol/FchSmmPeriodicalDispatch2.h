/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD FCH PERIODICAL Dispacther Protocol
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD FCH UEFI Drivers
 * @e sub-project:  Protocols
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/******************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
**/
#ifndef _FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL_H_
#define _FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL_H_

#include <Protocol/SmmPeriodicTimerDispatch2.h>

#define SHORT_TIMER_SMI_INTERVAL  (10000000 / 1000000)
#define LONG_TIMER_SMI_INTERVAL   (10000000 / 1000)

extern EFI_GUID gFchSmmPeriodicalDispatch2ProtocolGuid;

typedef struct _FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL;

///
/// AMD FCH Periodic Timer SMM Register Context
///
typedef struct {
  UINT64   Period;                        ///< The min period of time in 100ns units that child gets called
  UINT64   SmiTickInterval;               ///< The period of time interval between SMIs
  BOOLEAN  StartNow;                      ///< Whether SMI is to be generated instantly
} FCH_SMM_PERIODICAL_REGISTER_CONTEXT;


typedef
EFI_STATUS
(EFIAPI *FCH_SMM_PERIODICAL_HANDLER_ENTRY_POINT2)(
  IN       EFI_HANDLE                                DispatchHandle,
  IN CONST FCH_SMM_PERIODICAL_REGISTER_CONTEXT       *RegisterContext,
  IN OUT   EFI_SMM_PERIODIC_TIMER_CONTEXT            *PeriodicTimerContext,
  IN OUT   UINTN                                     *SizeOfContext
  );

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_PERIODICAL_DISPATCH2_REGISTER) (
  IN CONST FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL      *This,
  IN       FCH_SMM_PERIODICAL_HANDLER_ENTRY_POINT2    DispatchFunction,
  IN CONST FCH_SMM_PERIODICAL_REGISTER_CONTEXT        *PeriodicalRegisterContext,
     OUT   EFI_HANDLE                                 *DispatchHandle
);

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_PERIODICAL_DISPATCH2_UNREGISTER) (
  IN CONST FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL      *This,
  IN       EFI_HANDLE                                 DispatchHandle
);

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_PERIODIC_TIMER_INTERVAL2)(
  IN CONST FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL      *This,
  IN OUT   UINT64                                     **SmiTickInterval
  );

struct  _FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL {
  FCH_SMM_PERIODICAL_DISPATCH2_REGISTER             Register;
  FCH_SMM_PERIODICAL_DISPATCH2_UNREGISTER           UnRegister;
  FCH_SMM_PERIODIC_TIMER_INTERVAL2                  GetNextShorterInterval;
};

#endif

