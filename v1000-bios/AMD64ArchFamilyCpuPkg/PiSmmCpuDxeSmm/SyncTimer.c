/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "PiSmmCpuDxeSmm.h"

UINT64   mTimeoutTicker = 0;
//
//  Number of counts in a roll-over cycle of the performance counter.
//
UINT64   mCycle = 0;
//
// Flag to indicate the performance counter is count-up or count-down.
//
BOOLEAN  mCountDown;

/**
  Initialize Timer for Smm AP Sync.
  
**/
VOID
InitializeSmmTimer (
  VOID
  )
{
  UINT64  TimerFrequency;
  UINT64  Start;
  UINT64  End;

  TimerFrequency = GetPerformanceCounterProperties (&Start, &End);
  mTimeoutTicker = DivU64x32 (
                     MultU64x64(TimerFrequency, PcdGet64 (PcdCpuSmmApSyncTimeout)),
                     1000 * 1000
                     ); 
  if (End < Start) {
    mCountDown = TRUE;
    mCycle = Start - End;
  } else {
    mCountDown = FALSE;
    mCycle = End - Start;
  }
}

/**
  Start Timer for Smm AP Sync.
  
**/
UINT64
EFIAPI
StartSyncTimer (
  VOID
  )
{
  return GetPerformanceCounter ();
}


/**
  Check if the Smm AP Sync timer is timeout.
  
  @param Timer  The start timer from the begin.

**/
BOOLEAN
EFIAPI
IsSyncTimerTimeout (
  IN      UINT64                    Timer
  )
{
  UINT64  CurrentTimer;
  UINT64  Delta;

  CurrentTimer = GetPerformanceCounter ();

  if (mCountDown) {
    //
    // The performance counter counts down.  Check for roll over condition.
    //
    if (CurrentTimer < Timer) {
      Delta = Timer - CurrentTimer;
    } else {
      //
      // Handle one roll-over. 
      //
      Delta = mCycle - (CurrentTimer - Timer);
    }
  } else {
    //
    // The performance counter counts up.  Check for roll over condition.
    //
    if (CurrentTimer > Timer) {
      Delta = CurrentTimer - Timer;
    } else {
      //
      // Handle one roll-over. 
      //
      Delta = mCycle - (Timer - CurrentTimer);
    }
  }
 
  return (BOOLEAN) (Delta >= mTimeoutTicker);
}
