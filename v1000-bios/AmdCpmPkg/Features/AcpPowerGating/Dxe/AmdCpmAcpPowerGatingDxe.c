/*****************************************************************************
 *
 * Copyright (C) 2018-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmDxe.h>

EFI_GUID ACP_POWER_GATING_ACPI_DEVICE_GUID = {
  0xF1216807, 0x1A1E, 0x4306, {0xB3, 0xFF, 0xF6, 0xDA, 0x55, 0x14, 0xD1, 0x07}
};

VOID
EFIAPI
AmdCpmAcpPowerGatingInitLate (
  IN      EFI_EVENT       Event,
  IN      VOID            *Context
  );

BOOLEAN
EFIAPI
AmdCpmAcpPowerGatingCallBack (
  IN      VOID                    *This,
  IN      VOID                    *AmlObjPtr,
  IN      VOID                    *Context
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM AcpPowerGating DXE driver
 *
 * This function stores the setting of AcpPowerGating in NV Data and loads, updates
 * and installs AcpPowerGating SSDT Table.
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmAcpPowerGatingDxeEntryPoint (
  IN      EFI_HANDLE              ImageHandle,
  IN      EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_EVENT                       ReadyToBootEvent;

  //
  // Initialize Global Variable
  //
  CpmInitializeDriverLib (ImageHandle, SystemTable);

  Status = gBS->CreateEventEx (
                  CPM_EVENT_NOTIFY_SIGNAL,
                  CPM_TPL_CALLBACK,
                  AmdCpmAcpPowerGatingInitLate,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &ReadyToBootEvent
                  );

  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * The function to load AcpPowerGating SSDT table.
 *
 * This function gets called each time the EFI_EVENT_SIGNAL_READY_TO_BOOT gets signaled.
 *
 * @param[in]     Event          EFI_EVENT
 * @param[in]     Context        The Parameter Buffer
 *
 */
VOID
EFIAPI
AmdCpmAcpPowerGatingInitLate (
  IN      EFI_EVENT               Event,
  IN      VOID                    *Context
  )
{
  STATIC BOOLEAN                  InitlateInvoked = FALSE;
  AMD_CPM_TABLE_PROTOCOL          *CpmTableProtocolPtr;
  AMD_CPM_MAIN_TABLE              *CpmMainTablePtr;
  EFI_STATUS                      Status;
  UINT32                          Buffer[4];

  if (!InitlateInvoked) {

    Status = gBS->LocateProtocol (&gAmdCpmTableProtocolGuid, NULL, (VOID**)&CpmTableProtocolPtr);
    if (EFI_ERROR (Status)) {
      return;
    }

    CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpAcpPowerGatingDxeDriverBegin);

    CpmMainTablePtr = CpmTableProtocolPtr->MainTablePtr;
    if (CpmMainTablePtr->AcpPowerGatingEn) {
      ZeroMem (Buffer, sizeof (Buffer));
      Buffer[0] = CpmTableProtocolPtr->CommonFunction.GetPcieAslName ( CpmTableProtocolPtr, 8, 1);
      Buffer[1] = CFG_AMD_CPM_ASL_ACP_NAME;               // PcdGet32 (AcpAslName)
      Buffer[2] = CFG_AMD_CPM_ASL_AZALIA_NAME;            // PcdGet32 (AzaliaAslName)
      if (!PcdGetBool (PcdAcpController)) {
        Buffer[3] = 1;                                    // ACP Controller is disable
      }
      CpmTableProtocolPtr->CommonFunction.AddSsdtTable (  (VOID*)CpmTableProtocolPtr,
                                                          &ACP_POWER_GATING_ACPI_DEVICE_GUID,
                                                          NULL,
                                                          AmdCpmAcpPowerGatingCallBack,
                                                          &Buffer[0] );
    } // End of if (CpmMainTablePtr->AcpPowerGatingEn)

    CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpAcpPowerGatingDxeDriverEnd);
  }

  InitlateInvoked = TRUE;

  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback function to update AcpPowerGating SSDT table
 *
 * This function is used to update the name of PCIe bridge for ACP in ASL
 *
 * @param[in]     This           Pointer to Protocol
 * @param[in]     AmlObjPtr      The AML Object Buffer
 * @param[in]     Context        The Parameter Buffer
 *
 * @retval        TRUE           SSDT Table has been updated completely
 * @retval        FALSE          SSDT Table has not been updated completely
 */

BOOLEAN
EFIAPI
AmdCpmAcpPowerGatingCallBack (
  IN      VOID                    *This,
  IN      VOID                    *AmlObjPtr,
  IN      VOID                    *Context
  )
{
  UINT32    *BufferPtr;

  BufferPtr = (UINT32*)Context;

  switch ( *((UINT32*)AmlObjPtr)) {
  case CPM_SIGNATURE_32 ('P', 'B', 'C', '_'):     // ASL code: \_SB.PCI0.PBC
    if (*BufferPtr) {
      *((UINT32*)AmlObjPtr) = *BufferPtr;
    }
    break;
  case CPM_SIGNATURE_32 ('A', 'C', 'P', '_'):     // ASL code: \_SB.PCI0.PBC.ACP
    if (*(BufferPtr + 1)) {
      *(UINT32*) AmlObjPtr = *(UINT32*)(BufferPtr + 1);
    }
    break;
  case CPM_SIGNATURE_32 ('A', 'Z', 'A', 'L'):     // ASL code: \_SB.PCI0.PBC.AZAL
    if (*(BufferPtr + 2)) {
      *(UINT32*) AmlObjPtr = *(UINT32*)(BufferPtr + 2);
    }
    break;
  case CPM_SIGNATURE_32 ('M', '2', '7', 'A'):     // ASL code: CpmAcpController: M27A
    if (*(BufferPtr + 3)) {
      *(((UINT8*) AmlObjPtr) + 4) = 0;
      *(BufferPtr + 3) = 0;
    }
    break;
  }
  return FALSE;
}

