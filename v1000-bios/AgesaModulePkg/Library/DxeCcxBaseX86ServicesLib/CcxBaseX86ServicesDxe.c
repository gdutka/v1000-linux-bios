/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX BASE Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CCX
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
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "Uefi.h"
#include <Library/BaseLib.h>
#include "AMD.h"
#include "Filecode.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/CcxBaseX86Lib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/CcxBaseServicesProtocol.h>
#include "CcxBaseX86ServicesDxe.h"

#define FILECODE LIBRARY_DXECCXBASEX86SERVICESLIB_CCXBASEX86SERVICESDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
STATIC CCX_BASE_SERVICES_PROTOCOL mCcxBaseServices = {
  0x1,
  CcxGetMaxPhysAddr,
  CcxGetMaxProcCap
};



/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
CcxBaseServicesProtocolInstall (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  return (gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gAmdCcxBaseServicesProtocolGuid,
                  &mCcxBaseServices,
                  NULL
                  ));
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  Returns the maximum physical address size
 *
 *  Parameters:
 *    @param[in]       This    Pointer to the CCX_BASE_SERVICES_PROTOCOL instance.
 *
 *    @return          UINT8   Maximum physical address size
 *
 *---------------------------------------------------------------------------------------
 */
UINT8
EFIAPI
CcxGetMaxPhysAddr (
  IN       CCX_BASE_SERVICES_PROTOCOL *This
  )
{
  UINT8       MemEncryptPhysAddWidth;
  UINT32      LFuncExt;
  CPUID_DATA  CpuId;


  MemEncryptPhysAddWidth = 0;

  LFuncExt = CcxGetLFuncExt ();

  if (LFuncExt >= 0x8000001F) {
    // Get value of CPUID_Fn8000001F_EBX[MemEncryptPhysAddWidth]
    AsmCpuid (
        0x8000001F,
        &(CpuId.EAX_Reg),
        &(CpuId.EBX_Reg),
        &(CpuId.ECX_Reg),
        &(CpuId.EDX_Reg)
      );
    MemEncryptPhysAddWidth = (UINT8) ((CpuId.EBX_Reg >> 6) & 0x3F);
  }

  return CcxGetPhysAddrSize () - MemEncryptPhysAddWidth;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  Returns the maximum processor capacity in the system
 *
 *  Parameters:
 *    @param[in]       This    Pointer to the CCX_BASE_SERVICES_PROTOCOL instance.
 *
 *    @return          UINT8   Maximum processor capacity
 *
 *---------------------------------------------------------------------------------------
 */
UINT8
EFIAPI
CcxGetMaxProcCap (
  IN       CCX_BASE_SERVICES_PROTOCOL *This
  )
{
  return ((1 << CcxGetApicIdCoreIdSize ()) * PcdGet8 (PcdAmdNumberOfPhysicalSocket));
}

