/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include <Uefi.h>
#include "PiDxe.h"
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include "CbsHookDxeLib.h"

typedef struct _CBS_CONFIG AMD_SYSTEM_CONFIGURATION;
extern EFI_RUNTIME_SERVICES *gRT;

CBS_CONFIG *pCbsConfig;
static CBS_DXE_PROTOCOL mCbsProtocolInstance=
{
  CbsHookFunc,
  0
};

VOID
CbsVariableLoadDefault(
  UINT8 *pVariableData
  )
{
  EFI_STATUS   Status;
  UINT32       RecordOffset;
  CBS_CONFIG   TempVariable;
  UINT8        *tempBuffer;
  UINT32       i;
  UINTN        BufferSize;

  tempBuffer = (UINT8*)&TempVariable;
  BufferSize = 0;

  Status = gRT->GetVariable (CBS_SYSTEM_CONFIGURATION_NAME,
                             &gCbsSystemConfigurationGuid,
                             NULL,
                             &BufferSize,
                             pVariableData
                             );
  if ((Status == EFI_BUFFER_TOO_SMALL) && (BufferSize == sizeof(CBS_CONFIG))) {
    Status = gRT->GetVariable (CBS_SYSTEM_CONFIGURATION_NAME,
                               &gCbsSystemConfigurationGuid,
                               NULL,
                               &BufferSize,
                               pVariableData
                               );
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
        for(i = 0; i < sizeof(CBS_CONFIG); i++) {
          pVariableData[i] = tempBuffer[i];
        }
      }
    }
  } else {
    DEBUG ((EFI_D_ERROR, "No CBS Variable, Force loading CBS variable default value.(OrgSize=0x%x, NewSize=0x%x)\n", BufferSize, sizeof(CBS_CONFIG)));
    CbsWriteDefalutValue(pVariableData);
  }
}

EFI_STATUS
CbsHookFunc (
  IN       EFI_BOOT_SERVICES    *BootServices,
  IN OUT   VOID                 *AMD_PARAMS,
  IN       UINTN                IdsHookId
  )
{
  DXE_CBS_COMPONENT_STRUCT   *pCbsFunctionTable;
  DXE_CBS_FUNCTION_STRUCT    *pCurTable;
  CPUID_DATA                 CpuId;
  UINT64                     SocFamilyID;

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
      // Dispatch CBS function in DXE_CBS_FUNCTION_STRUCT.
      pCurTable = pCbsFunctionTable->pFuncTable;
      while (pCurTable->CBSFuncPtr != NULL) {
        if ((pCurTable->IdsHookId == IdsHookId) &&
          ((pCurTable->Family == SocFamilyID) || (pCurTable->Family == 0xFFFFFFFF))) {
          pCurTable->CBSFuncPtr(BootServices,AMD_PARAMS,pCbsConfig);
        }
        pCurTable++;
      }
    }
    pCbsFunctionTable++;
  }
  return EFI_SUCCESS;
}

EFI_STATUS CbsInitEntry(
  IN  EFI_BOOT_SERVICES *BootServices
  )
{
  EFI_STATUS   Status;
  EFI_HANDLE   Handle;

  pCbsConfig = AllocateZeroPool (sizeof(CBS_CONFIG));
  if (pCbsConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CbsVariableLoadDefault ((UINT8*)pCbsConfig);

  Handle = NULL;
  Status = BootServices->InstallProtocolInterface ( &Handle,
                                        &gCbsProtocolGuid,
                                        EFI_NATIVE_INTERFACE,
                                        &mCbsProtocolInstance);
  return Status;
}

EFI_STATUS
CbsInterfaceFunc (
  IN  EFI_BOOT_SERVICES      *BootServices,
  IN  OUT VOID               *AMD_PARAMS,
  IN      UINTN              IdsHookId
  )
{
  EFI_STATUS                Status;
  CBS_DXE_PROTOCOL          *pCbsProtocol;

  Status = BootServices->LocateProtocol (&gCbsProtocolGuid, NULL, (VOID**)&pCbsProtocol);
  if (!EFI_ERROR(Status)) {
    Status = pCbsProtocol->CbsHookInterface(BootServices, AMD_PARAMS, IdsHookId);
  }
  return Status;
}

