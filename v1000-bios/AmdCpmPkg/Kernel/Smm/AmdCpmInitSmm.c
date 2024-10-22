/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmSmm.h>
#include <AmdCpmBaseIo.h>
#include <AmdCpmCpu.h>
#include <AmdCpmFch.h>
#include <AmdCpmTable.h>

CPM_DEFINE_gBS;
CPM_DEFINE_gSmst;

VOID
EFIAPI
AmdCpmSmmRegisterKernel (
  IN       VOID                    *This
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM Init SMM driver
 *
 * This function installs AmdCpmTableSmmProtocol and registers CPM common functions
 * at AmdCpmInitSmm.
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmInitSmmEntryPoint (
  IN       EFI_HANDLE              ImageHandle,
  IN       EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      Handle;

  AMD_CPM_TABLE_PROTOCOL          *CpmTableProtocolPtr;
  AMD_CPM_TABLE_PROTOCOL          *CpmTableSmmProtocolPtr;

  If_CpmInSmm (ImageHandle, SystemTable, gBS, gSmst, Status) {

    DEBUG((DEBUG_INFO, "FEA-SMM-%a-Start\n", __FUNCTION__));
    //
    // Initialize global variables
    //
    DEBUG((DEBUG_INFO, "FEA-SMM-%a-SmmAllocatePool-CpmTableSmmProtocolPtr\n", __FUNCTION__));
    Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (AMD_CPM_TABLE_PROTOCOL),
                    (VOID**)&CpmTableSmmProtocolPtr
                    );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_INFO, "FEA-SMM-%a-End-1-SmmAllocatePool-FAIL\n", __FUNCTION__));
      return Status;
    }

    DEBUG((DEBUG_INFO, "FEA-SMM-%a-LocateProtocol-gAmdCpmTableProtocolGuid\n", __FUNCTION__));
    Status = gBS->LocateProtocol (
                    &gAmdCpmTableProtocolGuid,
                    NULL,
                    (VOID**)&CpmTableProtocolPtr
                    );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_INFO, "FEA-SMM-%a-End-2-LocateProtocol-FAIL\n", __FUNCTION__));
      return Status;
    }

    CpmTableSmmProtocolPtr->Revision      = CpmTableProtocolPtr->Revision;
    CpmTableSmmProtocolPtr->MainTablePtr  = CpmTableProtocolPtr->MainTablePtr;

    DEBUG((DEBUG_INFO, "FEA-SMM-%a-AmdCpmSmmRegisterKernel\n", __FUNCTION__));
    AmdCpmSmmRegisterKernel (CpmTableSmmProtocolPtr);
    DEBUG((DEBUG_INFO, "FEA-SMM-%a-CpmRegisterBaseIo\n", __FUNCTION__));
    CpmRegisterBaseIo (CpmTableSmmProtocolPtr);
    DEBUG((DEBUG_INFO, "FEA-SMM-%a-CpmRegisterCpu\n", __FUNCTION__));
    CpmRegisterCpu (CpmTableSmmProtocolPtr);
    DEBUG((DEBUG_INFO, "FEA-SMM-%a-CpmRegisterFch\n", __FUNCTION__));
    CpmRegisterFch (CpmTableSmmProtocolPtr);

    Handle = NULL;
    DEBUG((DEBUG_INFO, "FEA-SMM-%a-SmmInstallProtocolInterface-gAmdCpmTableSmmProtocolGuid\n", __FUNCTION__));
    Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gAmdCpmTableSmmProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    CpmTableSmmProtocolPtr
                    );
    DEBUG((DEBUG_INFO, "FEA-SMM-%a-End-3-Status=%r\n", __FUNCTION__, Status));
  }

  RECORD_TIME_DXE (EndAmdCpmInitSmmDriver);

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Register Common Kenerl functions at the AmdCpmInitSmm entry point.
 *
 * This function registers CPM common kernel functions in AmdCpmTableSmmProtocol
 * at AmdCpmInitSmm.
 *
 *  @param[in]   This                Pointer to Protocol.
 *
 */
VOID
EFIAPI
AmdCpmSmmRegisterKernel (
  IN       VOID                    *This
  )
{
  AMD_CPM_TABLE_PROTOCOL  *CpmTableSmmProtocolPtr         = This;
  DEBUG((DEBUG_INFO, "KER-SMM-%a-Start\n", __FUNCTION__));
  CpmTableSmmProtocolPtr->CommonFunction.GetTablePtr      = CpmGetTablePtr;
  CpmTableSmmProtocolPtr->CommonFunction.GetTablePtr2     = CpmGetTablePtr2;
  CpmTableSmmProtocolPtr->CommonFunction.RelocateTable    = CpmRelocateTableList;
  CpmTableSmmProtocolPtr->CommonFunction.CopyMem          = CpmCopyMem;
  CpmTableSmmProtocolPtr->CommonFunction.ReadSmbus        = NULL;
  CpmTableSmmProtocolPtr->CommonFunction.WriteSmbus       = NULL;
  CpmTableSmmProtocolPtr->CommonFunction.ReadSmbusBlock   = NULL;
  CpmTableSmmProtocolPtr->CommonFunction.WriteSmbusBlock  = NULL;
  DEBUG((DEBUG_INFO, "KER-SMM-%a-End\n", __FUNCTION__));
  return;
}
