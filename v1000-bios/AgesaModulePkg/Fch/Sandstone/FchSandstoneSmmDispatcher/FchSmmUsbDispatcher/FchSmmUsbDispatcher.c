/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Hudson-2 USB Dispatcher Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  UEFI
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/******************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
**/

#include "FchSmmUsbDispatcher.h"
#include "Filecode.h"
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#define FILECODE UEFI_SMM_FCHSMMDISPATCHER_FCHSMMUSBDISPATCHER_FCHSMMUSBDISPATCHER_FILECODE

EFI_STATUS
EFIAPI
FchSmmUsbDispatch2Register (
  IN       CONST FCH_SMM_USB_DISPATCH2_PROTOCOL  *This,
  IN       FCH_SMM_USB_HANDLER_ENTRY_POINT2      CallBackFunction,
  IN       CONST FCH_SMM_USB_REGISTER_CONTEXT    *RegisterContext,
     OUT   EFI_HANDLE                            *DispatchHandle
  );

EFI_STATUS
EFIAPI
FchSmmUsbDispatch2UnRegister (
  IN       CONST FCH_SMM_USB_DISPATCH2_PROTOCOL  *This,
  IN       EFI_HANDLE                            DispatchHandle
  );

FCH_SMM_USB_DISPATCH2_PROTOCOL gFchSmmUsbDispatch2Protocol = {
  FchSmmUsbDispatch2Register,
  FchSmmUsbDispatch2UnRegister
};

/*----------------------------------------------------------------------------------------*/
/**
 * FCH SMM USB dispatcher handler
 *
 *
 * @param[in]       SmmImageHandle        Image Handle
 * @param[in, out]   OPTIONAL CommunicationBuffer   Communication Buffer (see PI 1.1 for more details)
 * @param[in, out]   OPTIONAL SourceSize            Buffer size (see PI 1.1 for more details)

 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
FchSmmUsbDispatchHandler (
  IN       EFI_HANDLE   SmmImageHandle,
  IN OUT   VOID         *CommunicationBuffer OPTIONAL,
  IN OUT   UINTN        *SourceSize OPTIONAL
  )
{
  EFI_STATUS       Status;
  FCH_SMM_USB_NODE *CurrentFchSmmUsbNodePtr;

  Status = EFI_NOT_FOUND;

  if (HeadFchSmmUsbNodePtr->FchUsbNodePtr == NULL) {
    Status = EFI_NOT_FOUND;
  } else {
    CurrentFchSmmUsbNodePtr = HeadFchSmmUsbNodePtr;
    while (CurrentFchSmmUsbNodePtr->FchUsbNodePtr!= NULL) {
      if (CurrentFchSmmUsbNodePtr->CallBack2Function != NULL) {
        Status = CurrentFchSmmUsbNodePtr->CallBack2Function (
                                            CurrentFchSmmUsbNodePtr->DispatchHandle,
                                            &CurrentFchSmmUsbNodePtr->Context,
                                            NULL,
                                            NULL
                                            );
        DEBUG ((DEBUG_VERBOSE, "[FchSmmDispatcher] USB SMM handler dispatched: Order = 0x%x, \
               return - %r\n", CurrentFchSmmUsbNodePtr->Context.Order, Status));
      }
      CurrentFchSmmUsbNodePtr = CurrentFchSmmUsbNodePtr->FchUsbNodePtr;
    }
  }
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84) = UsbSmi;
  FchSmnRW (0, 0x13B10120, 0xfffffffe, BIT0, NULL);
  FchSmnRW (0, 0x13B20020, 0xfffffeff, BIT8, NULL);
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84) = UsbSmi; //PLAT-11747
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Register USB child handler
 *
 *
 * @param[in]       This                  Pointer to protocol
 * @param[in]       CallBackFunction
 * @param[in, out]   RegisterContext            Register contecxt (see PI 1.1 for more details)
 * @param[out]      DispatchHandle        Handle (see PI 1.1 for more details)
 *
 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
FchSmmUsbDispatchHandler2 (
  IN       EFI_HANDLE   SmmImageHandle,
  IN OUT   VOID         *CommunicationBuffer OPTIONAL,
  IN OUT   UINTN        *SourceSize OPTIONAL
  )
{
  EFI_STATUS       Status;
  FCH_SMM_USB_NODE *CurrentFchSmmUsbNodePtr;
  Status = EFI_NOT_FOUND;
  if (HeadFchSmmUsbNodePtr->FchUsbNodePtr == NULL) {
    Status = EFI_NOT_FOUND;
  } else {
    CurrentFchSmmUsbNodePtr = HeadFchSmmUsbNodePtr;
    while (CurrentFchSmmUsbNodePtr->FchUsbNodePtr!= NULL) {
      if (CurrentFchSmmUsbNodePtr->CallBack2Function != NULL) {
        Status = CurrentFchSmmUsbNodePtr->CallBack2Function (
                                            CurrentFchSmmUsbNodePtr->DispatchHandle,
                                            &CurrentFchSmmUsbNodePtr->Context,
                                            NULL,
                                            NULL
                                            );
        DEBUG ((DEBUG_VERBOSE, "[FchSmmDispatcher] USB SMM handler dispatched: Order = 0x%x, \
               return - %r\n", CurrentFchSmmUsbNodePtr->Context.Order, Status));
      }
      CurrentFchSmmUsbNodePtr = CurrentFchSmmUsbNodePtr->FchUsbNodePtr;
    }
  }
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88) = BIT12;
  return  Status;
}
EFI_STATUS
EFIAPI
FchSmmUsbDispatch2Register (
  IN       CONST FCH_SMM_USB_DISPATCH2_PROTOCOL  *This,
  IN       FCH_SMM_USB_HANDLER_ENTRY_POINT2      CallBackFunction,
  IN       CONST FCH_SMM_USB_REGISTER_CONTEXT    *RegisterContext,
     OUT   EFI_HANDLE                            *DispatchHandle
  )
{
  EFI_STATUS       Status;
  FCH_SMM_USB_NODE *NewFchSmmUsbNodePtr;
  FCH_SMM_USB_NODE *CurrentFchSmmUsbNodePtr;
  FCH_SMM_USB_NODE *PreviousFchSmmUsbNodePtr;
  Status = EFI_OUT_OF_RESOURCES;

  if (CallBackFunction == NULL || RegisterContext == NULL || DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) |= BIT24;
  ACPIMMIO32 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGED) |= (BIT4);
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_USB_NODE),
                    (VOID**)&NewFchSmmUsbNodePtr
                    );
  ASSERT_EFI_ERROR (Status);

  NewFchSmmUsbNodePtr->CallBack2Function = CallBackFunction;
  NewFchSmmUsbNodePtr->Context = *RegisterContext;
  *DispatchHandle = &NewFchSmmUsbNodePtr->DispatchHandle;
  NewFchSmmUsbNodePtr->DispatchHandle = *DispatchHandle;

  DEBUG ((DEBUG_INFO, "[FchSmmDispatcher] Registering USB SMM handler: Type = 0x%x, Order = 0x%x\n", \
         RegisterContext->Type, RegisterContext->Order));
  if (HeadFchSmmUsbNodePtr->FchUsbNodePtr == NULL) {
    NewFchSmmUsbNodePtr->FchUsbNodePtr = HeadFchSmmUsbNodePtr;
    HeadFchSmmUsbNodePtr = NewFchSmmUsbNodePtr;
  } else {
    PreviousFchSmmUsbNodePtr = HeadFchSmmUsbNodePtr;
    CurrentFchSmmUsbNodePtr = HeadFchSmmUsbNodePtr;
    while (CurrentFchSmmUsbNodePtr->FchUsbNodePtr != NULL) {
      if (NewFchSmmUsbNodePtr->Context.Order <= CurrentFchSmmUsbNodePtr->Context.Order &&
          CurrentFchSmmUsbNodePtr->Context.Type == NewFchSmmUsbNodePtr->Context.Type &&
          (CurrentFchSmmUsbNodePtr->Context.Device != NULL && NewFchSmmUsbNodePtr->Context.Device != NULL &&
          CompareMem (CurrentFchSmmUsbNodePtr->Context.Device, NewFchSmmUsbNodePtr->Context.Device, GetDevicePathSize (NewFchSmmUsbNodePtr->Context.Device)) == 0)) {

        if (PreviousFchSmmUsbNodePtr == CurrentFchSmmUsbNodePtr) {
          NewFchSmmUsbNodePtr->FchUsbNodePtr = HeadFchSmmUsbNodePtr;
          HeadFchSmmUsbNodePtr = NewFchSmmUsbNodePtr;
          Status = EFI_SUCCESS;
          return Status;
        }
        NewFchSmmUsbNodePtr->FchUsbNodePtr = PreviousFchSmmUsbNodePtr->FchUsbNodePtr;
        PreviousFchSmmUsbNodePtr->FchUsbNodePtr = NewFchSmmUsbNodePtr;

        Status = EFI_SUCCESS;
        return  Status;
      }
      PreviousFchSmmUsbNodePtr = CurrentFchSmmUsbNodePtr;
      CurrentFchSmmUsbNodePtr = CurrentFchSmmUsbNodePtr->FchUsbNodePtr;
    }
    PreviousFchSmmUsbNodePtr->FchUsbNodePtr = NewFchSmmUsbNodePtr;
    NewFchSmmUsbNodePtr->FchUsbNodePtr = CurrentFchSmmUsbNodePtr;
  }

  if ((ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC) & (BIT10 + BIT11)) != BIT10) {
    FchSmnRW (0, 0x13B20020, 0xfffffeff, BIT8, NULL);
    FchSmnRW (0, 0x13B20090, 0xffffbfff, BIT14, NULL);
    FchSmnRW (0, 0x16D80118, 0xfffff000, 0x100, NULL);
    FchSmnRW (0, 0x16F80118, 0xfffff000, 0x100, NULL);
    FchSmnRW (0, 0x13B10120, 0xfffffffe, BIT0, NULL);

    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84) = UsbSmi;
    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC) |= BIT10;
    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84) = UsbSmi;
  }


  Status = EFI_SUCCESS;
  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Unregister USB child handler
 *
 *
 * @param[in]       This                  Pointer to protocol
 * @param[in]       DispatchHandle
 *
 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
FchSmmUsbDispatch2UnRegister (
  IN       CONST FCH_SMM_USB_DISPATCH2_PROTOCOL  *This,
  IN       EFI_HANDLE                            DispatchHandle
  )
{
  EFI_STATUS       Status;
  FCH_SMM_USB_NODE *CurrentFchSmmUsbNodePtr;
  FCH_SMM_USB_NODE *PreviousFchSmmUsbNodePtr;

  if (DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (HeadFchSmmUsbNodePtr->FchUsbNodePtr == NULL) {
    Status = EFI_NOT_FOUND;
    return  Status;
  } else {
    PreviousFchSmmUsbNodePtr = HeadFchSmmUsbNodePtr;
    CurrentFchSmmUsbNodePtr = HeadFchSmmUsbNodePtr;
    if (CurrentFchSmmUsbNodePtr->DispatchHandle == DispatchHandle) {
      HeadFchSmmUsbNodePtr = CurrentFchSmmUsbNodePtr->FchUsbNodePtr;
    } else {
      while (CurrentFchSmmUsbNodePtr->DispatchHandle != DispatchHandle) {
        PreviousFchSmmUsbNodePtr = CurrentFchSmmUsbNodePtr;
        CurrentFchSmmUsbNodePtr = CurrentFchSmmUsbNodePtr->FchUsbNodePtr;
        if (CurrentFchSmmUsbNodePtr->DispatchHandle == NULL) {
          Status = EFI_NOT_FOUND;
          return  Status;
        }
      }
      PreviousFchSmmUsbNodePtr->FchUsbNodePtr = CurrentFchSmmUsbNodePtr->FchUsbNodePtr;
    }

    Status = gSmst->SmmFreePool (
                      CurrentFchSmmUsbNodePtr
                      );
    ASSERT_EFI_ERROR (Status);
  }
  Status = EFI_SUCCESS;
  return  Status;
}


EFI_STATUS
EFIAPI
EfiSmmUsbDispatch2Register (
  IN       CONST EFI_SMM_USB_DISPATCH2_PROTOCOL *This,
  IN       EFI_SMM_HANDLER_ENTRY_POINT2         DispatchFunction,
  IN       CONST EFI_SMM_USB_REGISTER_CONTEXT   *RegisterContext,
     OUT   EFI_HANDLE                           *DispatchHandle
  )
{
  FCH_SMM_USB_REGISTER_CONTEXT     FchRegisterContext;

  FchRegisterContext.Type   = RegisterContext->Type;
  FchRegisterContext.Device = RegisterContext->Device;
  FchRegisterContext.Order  = 0x80;      // SMM_DEFAULT_ORDER
  return FchSmmUsbDispatch2Register (
           &gFchSmmUsbDispatch2Protocol,
           (FCH_SMM_USB_HANDLER_ENTRY_POINT2)DispatchFunction,
           &FchRegisterContext,
           DispatchHandle
           );
}

EFI_STATUS
EFIAPI
EfiSmmUsbDispatch2UnRegister (
  IN       CONST EFI_SMM_USB_DISPATCH2_PROTOCOL *This,
  IN       EFI_HANDLE                           DispatchHandle
  )
{
  return FchSmmUsbDispatch2UnRegister (
           &gFchSmmUsbDispatch2Protocol,
           DispatchHandle
           );
}

EFI_SMM_USB_DISPATCH2_PROTOCOL gEfiSmmUsbDispatch2Protocol = {
  EfiSmmUsbDispatch2Register,
  EfiSmmUsbDispatch2UnRegister
};


