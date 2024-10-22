/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include "CbsHookPeiLib.h"
#include <Ppi/AmdSocPcdInitPpi.h>

typedef struct _CBS_CONFIG AMD_SYSTEM_CONFIGURATION;

STATIC PEI_CBS_HOOK_PPI mCbsHookInstance =
{
  CbsHookFunc
};

EFI_GUID SystemConfigurationGuid = AMD_SYSTEM_CONFIGURATION_GUID;

STATIC EFI_PEI_PPI_DESCRIPTOR mCbsHookInstancePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCbsHookPpiGuid,
  &mCbsHookInstance
};

EFI_STATUS
EFIAPI
AgesaPcdInitReadyCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

EFI_PEI_NOTIFY_DESCRIPTOR   mAgesaPcdInitReadyCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPcdInitReadyPpiGuid,
  AgesaPcdInitReadyCallback
};

VOID CbsVariableLoadDefault(
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableService,
  UINT8 *pVariableData
) 
{
  EFI_STATUS   Status;
  CBS_CONFIG   TempVariable;
  UINT8        *tempBuffer;
  UINT32       i;
  UINT32       RecordOffset;
  UINTN        VariableSize;

  tempBuffer = (UINT8*)&TempVariable;
  VariableSize = 0;
  Status = VariableService->GetVariable (VariableService,L"AmdSetup", &SystemConfigurationGuid, NULL, &VariableSize, pVariableData);
  if ((Status == EFI_BUFFER_TOO_SMALL) && (VariableSize == sizeof (AMD_SYSTEM_CONFIGURATION))) {
    Status = VariableService->GetVariable (VariableService,L"AmdSetup", &SystemConfigurationGuid, NULL, &VariableSize, pVariableData);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "No CBS Variable, Force loading CBS variable default value.(Status=0x%x)\n", Status));
      CbsWriteDefalutValue(pVariableData);
    } else {
      CbsWriteDefalutValue(tempBuffer);
      DEBUG ((EFI_D_ERROR, "Found CBS Variable: oID=0x%08x, nID=0x%08x, Offset = 0x%08x\n", 
              (UINT32) *(UINT32 *)pVariableData, 
              (UINT32) *(UINT32 *)tempBuffer,
              (UINT32) *(UINT32*)(pVariableData + sizeof(UINT32))
              ));
      if ((UINT32) *(UINT32 *)pVariableData != (UINT32) *(UINT32 *)tempBuffer) {
        RecordOffset = (UINT32) *(UINT32*)(pVariableData + sizeof(UINT32));
        for(i = 32; i < RecordOffset; i++) {
          tempBuffer[i] = pVariableData[i];
        }
        for(i = 0; i < sizeof (CBS_CONFIG); i++) {
          pVariableData[i] = tempBuffer[i];
        }
      }
    }
  } else {
      DEBUG ((EFI_D_ERROR, "No CBS Variable, Force loading CBS variable default value.(Status=0x%x)\n", Status));
      CbsWriteDefalutValue(pVariableData);
  }
}

EFI_STATUS
CbsHookFunc (
  IN       EFI_PEI_SERVICES     **PeiServices,
  IN OUT   VOID                 *AMD_PARAMS,
  IN       UINTN                IdsHookId
  )
{
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Variable;
  CBS_CONFIG                      CbsVariable;
  EFI_STATUS                      Status;
  PEI_CBS_COMPONENT_STRUCT        *pCbsFunctionTable;
  PEI_CBS_FUNCTION_STRUCT         *pCurTable;
  CPUID_DATA                      CpuId;
  UINT64                          SocFamilyID;

  (**PeiServices).SetMem (&CbsVariable, sizeof (AMD_SYSTEM_CONFIGURATION), 0);

  Status = (**PeiServices).LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID**)&Variable);
  if (!EFI_ERROR(Status)) {
    CbsVariableLoadDefault (Variable, (UINT8*) &CbsVariable);
  } else {
    CbsWriteDefalutValue((UINT8 *) &CbsVariable);
  }

  AsmCpuid (
      0x80000001,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  SocFamilyID = CpuId.EAX_Reg & RAW_FAMILY_ID_MASK;

  pCbsFunctionTable = &CbsCommonFuncTable[0];
  while (pCbsFunctionTable->pFuncTable != NULL) {
    if ((pCbsFunctionTable->Family == SocFamilyID) || (pCbsFunctionTable->Family == 0xFFFFFFFF)) {
      // Dispatch CBS function in PEI_CBS_FUNCTION_STRUCT.
      pCurTable = pCbsFunctionTable->pFuncTable;
      while (pCurTable->CBSFuncPtr != NULL) {
        if ((pCurTable->IdsHookId == IdsHookId) &&
            ((pCurTable->Family == SocFamilyID) || (pCurTable->Family == 0xFFFFFFFF))) {
          pCurTable->CBSFuncPtr(PeiServices, AMD_PARAMS, &CbsVariable);
        }
        pCurTable++;
      }
    }
    pCbsFunctionTable++;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AgesaPcdInitReadyCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Variable;
  EFI_STATUS                      Status;
  CBS_CONFIG                      CbsVariable;

  (**PeiServices).SetMem (&CbsVariable, sizeof (AMD_SYSTEM_CONFIGURATION), 0);

  Status = (**PeiServices).LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID**)&Variable);
  if (!EFI_ERROR(Status)) {
    CbsVariableLoadDefault (Variable, (UINT8*)&CbsVariable);
  } else {
    CbsWriteDefalutValue((UINT8 *)&CbsVariable);
  }

  //Call Auto Gen code to set AGESA PCDs
  CbsSetAgesaPcds (&CbsVariable);
  return (EFI_SUCCESS);
}

EFI_STATUS CbsInitEntry(
  IN CONST  EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS   Status;

  Status = (**PeiServices).InstallPpi (PeiServices, &mCbsHookInstancePpiList);
  Status = (**PeiServices).NotifyPpi (PeiServices, &mAgesaPcdInitReadyCallback);

  return Status;
}

EFI_STATUS
CbsInterfaceFunc (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  OUT VOID               *AMD_PARAMS,
  IN      UINTN              IdsHookId
  )
{
  EFI_STATUS          Status;
  PEI_CBS_HOOK_PPI    *CbsHookPtr;
  
  Status = EFI_SUCCESS;
  if (PeiServices) {
    Status = (*PeiServices)->LocatePpi (
                               (CONST EFI_PEI_SERVICES **)PeiServices,
                               &gAmdCbsHookPpiGuid,
                               0,
                               NULL,
                               (VOID**)&CbsHookPtr
                               );
    if (!EFI_ERROR (Status)) {
      Status = CbsHookPtr->CbsHookInterface (PeiServices, AMD_PARAMS, IdsHookId);
    }
  }
  return Status;
}

