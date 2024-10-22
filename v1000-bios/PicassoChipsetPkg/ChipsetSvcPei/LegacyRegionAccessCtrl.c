/** @file
 PEI Chipset Services Library.
 
 This file contains only one function that is PeiCsSvcLegacyRegionAccessCtrl().
 The function PeiCsSvcLegacyRegionAccessCtrl() use chipset services to control
 legacy region can be access or not.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/BaseLib.h>
#include <Ppi/H2OPeiChipsetServices.h>

#define PAM_C                     0xC0000
#define PAM_GRANULARITY           0x8000

#define SYS_CFG                   0xC0010010

#define ACCESS_TYPE_RD_DRAM       1
#define ACCESS_TYPE_RD_MMIO       2
#define ACCESS_TYPE_WR_DRAM       4
#define ACCESS_TYPE_WR_MMIO       8

typedef struct {
  UINT32                          Msr;
  UINT8                           AccessType;
} FIXED_MTRR_SETTING;

FIXED_MTRR_SETTING         mLockData[8] = {
                                  {0x00000268, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
                                  {0x00000269, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
                                  {0x0000026A, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
                                  {0x0000026B, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
                                  {0x0000026C, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
                                  {0x0000026D, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
                                  {0x0000026E, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
                                  {0x0000026F, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
};

FIXED_MTRR_SETTING          mUnlockData[8] = {
                                  {0x00000268, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
                                  {0x00000269, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
                                  {0x0000026A, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
                                  {0x0000026B, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
                                  {0x0000026C, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
                                  {0x0000026D, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
                                  {0x0000026E, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
                                  {0x0000026F, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
};

/**
 Modify MSR for region specified to MODE state.

 @param[in]         CtrlData            Fixed Mtrr setting to lock or unlock.

 @retval            None
*/
STATIC
VOID
CpuShadowCtrl (
  IN FIXED_MTRR_SETTING             CtrlData
  )
{
  UINT64                            MsrValue;
  
  // 
  // Enable FixMtrr mode
  //
  AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) | (1 << 19)));

  // 
  // Configure r/w attribute of fixed-size MTRRs below 1MB refer to mLockData and mUnlockData.
  // MSR0000_0[268:269] = C segment
  // MSR0000_0[26A:26B] = D segment
  // MSR0000_0[26C:26D] = E segment
  // MSR0000_0[26E:26F] = F segment
  // 
  MsrValue = AsmReadMsr64 (CtrlData.Msr);
  
  if (CtrlData.AccessType & ACCESS_TYPE_RD_DRAM) {
    MsrValue |= 0x1010101010101010;
  } else if (CtrlData.AccessType & ACCESS_TYPE_RD_MMIO) {
    MsrValue &= 0xEFEFEFEFEFEFEFEF;
  }

  if (CtrlData.AccessType & ACCESS_TYPE_WR_DRAM) {
    MsrValue |= 0x0808080808080808;
  } else if (CtrlData.AccessType & ACCESS_TYPE_WR_MMIO) {
    MsrValue &= 0xF7F7F7F7F7F7F7F7;
  }

  AsmWriteMsr64 (CtrlData.Msr, MsrValue);

  // 
  // Disable FixMtrr mode
  // 
  AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) & (~(1 << 19))));
}

/**
 Modify MSR for region specified to MODE stat.

 @param[in]         Start               Start of the region to lock or unlock.
 @param[in]         Length              Length of the region.
 @param[in]         Mode                LEGACY_REGION_ACCESS_LOCK or LEGACY_REGION_ACCESS_UNLOCK
                                        If LEGACY_REGION_ACCESS_LOCK, then LegacyRegionAccessCtrl()
                                        lock the specific legacy region.

 @retval            None
*/
STATIC
VOID
LegacyRegionManipulation (
  IN  UINT32                        Start,
  IN  UINT32                        Length,
  IN  UINT32                        Mode 
  )
{
  UINT8                             Index;
  UINT32                            TempAddr;
  FIXED_MTRR_SETTING                *Data;

  Data = (Mode == LEGACY_REGION_ACCESS_LOCK) ? mLockData : mUnlockData;

  for (TempAddr = (Start / PAM_GRANULARITY) * PAM_GRANULARITY, Index = (UINT8) ((TempAddr - PAM_C) / PAM_GRANULARITY);
       TempAddr <= (Start + Length);
       TempAddr += PAM_GRANULARITY, Index++) {

    if (Index >= 8) break;

    CpuShadowCtrl (Data[Index]);
  }
}

/**
 Legacy Region Access Control to make a region from 0xC0000 to 0xFFFFF
 read only or read/write, the GRANULARITY is 64KB. 

 @param[in]         Start               Start of the region to lock or unlock.
 @param[in]         Length              Length of the region.
 @param[in]         Mode                LEGACY_REGION_ACCESS_LOCK or LEGACY_REGION_ACCESS_UNLOCK
                                        If LEGACY_REGION_ACCESS_LOCK, then LegacyRegionAccessCtrl()
                                        lock the specific legacy region.

 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER 1. The parameter Start is small then 0xC0000.
                                          2. The parameter Length is too long. 
                                             The Start + Length - 1 should small then 0xF0000.
                                          3. The Mode parameter is neither
                                             LEGACY_REGION_ACCESS_LOCK nor LEGACY_REGION_ACCESS_UNLOCK

*/
EFI_STATUS
LegacyRegionAccessCtrl (
  IN  UINT32                        Start,
  IN  UINT32                        Length,
  IN  UINT32                        Mode  
  )
{
  if (Mode != LEGACY_REGION_ACCESS_UNLOCK && Mode != LEGACY_REGION_ACCESS_LOCK) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  LegacyRegionManipulation ((UINT32)Start, (UINT32)Length, (UINT32)Mode);

  return EFI_SUCCESS;
}
