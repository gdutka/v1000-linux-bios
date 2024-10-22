/** @file

  Implementation of CPU driver for PEI phase.
  This PEIM is to expose the Cache PPI.

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

#include "CpuPei.h"

/**
  Reset all the MTRRs to a known state.

  This function provides the PPI for PEI phase to Reset all the MTRRs to a known state(UC)
  @param  PeiServices     General purpose services available to every PEIM.  
  @param  This            Current instance of Pei Cache PPI.                 

  @retval EFI_SUCCESS     All MTRRs have been reset successfully.  
                      
**/
EFI_STATUS
EFIAPI
PeiResetCacheAttributes (
  IN  EFI_PEI_SERVICES     **PeiServices,
  IN  PEI_CACHE_PPI        *This
  );

/**
  program the MTRR according to the given range and cache type.

  This function provides the PPI for PEI phase to set the memory attribute by program
  the MTRR according to the given range and cache type. Actually this function is a 
  wrapper of the MTRR libaray to suit the PEI_CACHE_PPI interface

  @param  PeiServices     General purpose services available to every PEIM.  
  @param  This            Current instance of Pei Cache PPI.                 
  @param  MemoryAddress   Base Address of Memory to program MTRR.            
  @param  MemoryLength    Length of Memory to program MTRR.                  
  @param  MemoryCacheType Cache Type.   
  @retval EFI_SUCCESS             Mtrr are set successfully.        
  @retval EFI_LOAD_ERROR          No empty MTRRs to use.            
  @retval EFI_INVALID_PARAMETER   The input parameter is not valid. 
  @retval others                  An error occurs when setting MTTR.
                                          
**/
EFI_STATUS
EFIAPI
PeiSetCacheAttributes (
  IN  EFI_PEI_SERVICES         **PeiServices,
  IN  PEI_CACHE_PPI            *This,
  IN  EFI_PHYSICAL_ADDRESS     MemoryAddress,
  IN  UINT64                   MemoryLength,
  IN  EFI_MEMORY_CACHE_TYPE    MemoryCacheType
  );

PEI_CACHE_PPI   mCachePpi = {
  PeiSetCacheAttributes,
  PeiResetCacheAttributes
};


/**
  program the MTRR according to the given range and cache type.

  This function provides the PPI for PEI phase to set the memory attribute by program
  the MTRR according to the given range and cache type. Actually this function is a 
  wrapper of the MTRR libaray to suit the PEI_CACHE_PPI interface

  @param  PeiServices     General purpose services available to every PEIM.  
  @param  This            Current instance of Pei Cache PPI.                 
  @param  MemoryAddress   Base Address of Memory to program MTRR.            
  @param  MemoryLength    Length of Memory to program MTRR.                  
  @param  MemoryCacheType Cache Type.   
  @retval EFI_SUCCESS             Mtrr are set successfully.        
  @retval EFI_LOAD_ERROR          No empty MTRRs to use.            
  @retval EFI_INVALID_PARAMETER   The input parameter is not valid. 
  @retval others                  An error occurs when setting MTTR.
                                          
**/
EFI_STATUS
EFIAPI
PeiSetCacheAttributes (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN PEI_CACHE_PPI             *This,
  IN EFI_PHYSICAL_ADDRESS      MemoryAddress,
  IN UINT64                    MemoryLength,
  IN EFI_MEMORY_CACHE_TYPE     MemoryCacheType
  )
{

   RETURN_STATUS            Status;
   //
   // call MTRR libary function
   //
   Status = MtrrSetMemoryAttribute(
              MemoryAddress,
              MemoryLength,
              (MTRR_MEMORY_CACHE_TYPE) MemoryCacheType
            );
   return (EFI_STATUS)Status;
}

/**
  Reset all the MTRRs to a known state.

  This function provides the PPI for PEI phase to Reset all the MTRRs to a known state(UC)
  @param  PeiServices     General purpose services available to every PEIM.  
  @param  This            Current instance of Pei Cache PPI.                 

  @retval EFI_SUCCESS     All MTRRs have been reset successfully.  
                      
**/
EFI_STATUS
EFIAPI
PeiResetCacheAttributes (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN PEI_CACHE_PPI             *This
  )
{

  MTRR_SETTINGS  ZeroMtrr;
  UINT32 RegEax;

  // Only set variable MTRR for family 15
  AsmCpuid (0x80000001, &RegEax, NULL, NULL, NULL);
  if (((RegEax >> 20) & 0xFF) == 0x6) {

   //
   // reset all Mtrrs to 0 include fixed MTRR and variable MTRR
   //
   ZeroMem(&ZeroMtrr, sizeof(MTRR_SETTINGS));
   MtrrSetAllMtrrs(&ZeroMtrr);
  }

  return EFI_SUCCESS;
}
