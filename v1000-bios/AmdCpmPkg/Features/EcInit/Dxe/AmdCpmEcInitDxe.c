/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmDxe.h>

EFI_GUID EC_ACPI_DEVICE_GUID = {
  0x11ef89e3, 0xaf8f, 0x4653, 0x80, 0x5c, 0x90, 0xc5, 0x9c, 0x6e, 0xac, 0x26
};

VOID
EFIAPI
AmdCpmEcInitLateEntry (
  IN      EFI_EVENT           Event,
  IN      VOID                *Context
  );


/**
 *---------------------------------------------------------------------------------------
 *
 *  CpmDxeDriverEntryPoint
 *
 *  Description:
 *     Entry point for the cpu driver.
 *
 *  Parameters:
 *    @param[in]     ImageHandle
 *    @param[in]     *SystemTable
 *
 *    @retval         PMODULE_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/

EFI_STATUS
EFIAPI
AmdCpmEcInitDxeEntry (
  IN      EFI_HANDLE                      ImageHandle,
  IN      EFI_SYSTEM_TABLE                *SystemTable
  )
{
  EFI_STATUS                              Status;
  EFI_EVENT                               ReadyToBootEvent;

  //
  // Initialize Global Variable
  //
  CpmInitializeDriverLib (ImageHandle, SystemTable);

  Status = gBS->CreateEventEx (
                              CPM_EVENT_NOTIFY_SIGNAL,
                              CPM_TPL_NOTIFY,
                              AmdCpmEcInitLateEntry,
                              NULL,
                              &gEfiEventReadyToBootGuid,
                              &ReadyToBootEvent
                              );

  return Status;
}

BOOLEAN
EFIAPI
AmdCpmEcSsdtCallBack (
  IN      AMD_CPM_TABLE_PROTOCOL  *This,
  IN      VOID                    *AmlObjPtr,
  IN      VOID                    *Context
  )
{
  UINT32  *BufferPtr;

  BufferPtr = (UINT32*)Context;

  switch ( *((UINT32*)AmlObjPtr)) {
  case (CPM_SIGNATURE_32 ('E', 'C', '0', '_')):
    if (*BufferPtr) {
      *(UINT32*) AmlObjPtr = *(UINT32*)BufferPtr;
    }
    break;
  case (CPM_SIGNATURE_32 ('M', '0', '4', '2')):     // CpmKbcMutex: M042
    if (*(BufferPtr + 1)) {
      *(UINT32*) AmlObjPtr = *(UINT32*)(BufferPtr + 1);
    }
    break;
  case (CPM_SIGNATURE_32 ('L', 'P', 'C', '0')):
    if (*(BufferPtr + 2)) {
      *(UINT32*) AmlObjPtr = *(UINT32*)(BufferPtr + 2);
    }
    break;
  }
  return FALSE;
}

VOID
EFIAPI
AmdCpmEcInitLateEntry (
  IN      EFI_EVENT               Event,
  IN      VOID                    *Context
  )
/*++

Routine Description:

  Invoke AmdinitLate entry point. This function gets called
  each time the EFI_EVENT_SIGNAL_READY_TO_BOOT gets signaled

Arguments & Return Values: Standard event handling function prototype

--*/
{
  AMD_CPM_TABLE_PROTOCOL          *CpmTableProtocolPtr;
  STATIC BOOLEAN                  InitlateInvoked = FALSE;
  EFI_STATUS                      Status;
  UINT32                          Buffer[3];

  if (!InitlateInvoked) {
    Status = gBS->LocateProtocol (  &gAmdCpmTableProtocolGuid,
                                    NULL,
                                    (VOID**)&CpmTableProtocolPtr
                                    );
    if (EFI_ERROR (Status)) {
      return;
    }

    CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpEcInitDxeDriverBegin);
    Buffer[0] = CFG_AMD_CPM_ASL_EC_NAME;      // PcdGet32 (EcAslName)
    Buffer[1] = CFG_AMD_CPM_ASL_EC_MUTEX;     // PcdGet32 (EcMutexAslName)
    Buffer[2] = CFG_AMD_CPM_ASL_LPC_NAME;     // PcdGet32 (LpcAslName)

    CpmTableProtocolPtr->CommonFunction.AddSsdtTable (
                                                      CpmTableProtocolPtr,
                                                      &EC_ACPI_DEVICE_GUID,
                                                      NULL,
                                                      AmdCpmEcSsdtCallBack,
                                                      &Buffer[0]
                                                      );

    CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpEcInitDxeDriverEnd);
  }

  InitlateInvoked = TRUE;
  return;
}
