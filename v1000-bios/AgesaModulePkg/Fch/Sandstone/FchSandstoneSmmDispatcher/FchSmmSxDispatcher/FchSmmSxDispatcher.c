/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD FCH SMM Sleep Type Dispatcher Driver
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

#include "FchSmmSxDispatcher.h"
#include "GnbDxio.h"
#include <Library/NbioSmuV10Lib.h>
#include "Filecode.h"
#define FILECODE UEFI_SMM_FCHSMMDISPATCHER_FCHSMMSXDISPATCHER_FCHSMMSXDISPATCHER_FILECODE

EFI_STATUS
EFIAPI
FchSmmSxDispatch2Register (
  IN       CONST FCH_SMM_SX_DISPATCH2_PROTOCOL  *This,
  IN       FCH_SMM_SX_HANDLER_ENTRY_POINT2      CallBackFunction,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT    *RegisterContext,
     OUT   EFI_HANDLE                           *DispatchHandle
  );

EFI_STATUS
EFIAPI
FchSmmSxDispatch2UnRegister (
  IN       CONST FCH_SMM_SX_DISPATCH2_PROTOCOL  *This,
  IN       EFI_HANDLE                           DispatchHandle
  );

FCH_SMM_SX_DISPATCH2_PROTOCOL gFchSmmSxDispatch2Protocol = {
  FchSmmSxDispatch2Register,
  FchSmmSxDispatch2UnRegister
};

/*----------------------------------------------------------------------------------------*/
/**
 * FCH SMM Sleep Type dispatcher handler (Stack)
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
FchSmmSxDispatchHandler (
  IN       EFI_HANDLE   SmmImageHandle,
  IN OUT   VOID         *CommunicationBuffer OPTIONAL,
  IN OUT   UINTN        *SourceSize OPTIONAL
  )
{
  EFI_STATUS      Status;
  UINT16          AcpiPmCntBase;
  UINT16          SleepTypeValue;
  UINT16          SleepTypeDispatched;
  FCH_SMM_SX_NODE *CurrentFchSmmSxNodePtr;

  Status = EFI_UNSUPPORTED;
  SleepTypeDispatched = 0;

  AcpiPmCntBase = ACPIMMIO16 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG62);
  LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT16, AcpiPmCntBase, &SleepTypeValue);
  SleepTypeValue = (SleepTypeValue >> 10) & 0x7;

  if (HeadFchSmmSxNodePtr->FchSxNodePtr == NULL) {
    Status = EFI_NOT_FOUND;
  } else {
    CurrentFchSmmSxNodePtr = HeadFchSmmSxNodePtr;
    while (CurrentFchSmmSxNodePtr->FchSxNodePtr != NULL) {
      if (CurrentFchSmmSxNodePtr->Context.Type == SleepTypeValue &&
          CurrentFchSmmSxNodePtr->Context.Phase == SxEntry) {
        if (CurrentFchSmmSxNodePtr->CallBack2Function != NULL) {
          Status = CurrentFchSmmSxNodePtr->CallBack2Function (
                                             CurrentFchSmmSxNodePtr->DispatchHandle,
                                             &CurrentFchSmmSxNodePtr->Context,
                                             NULL,
                                             NULL
                                             );
          DEBUG ((DEBUG_INFO, "[FchSmmDispatcher] Sx SMM handler dispatched (Phase = SxEntry) SleepType = 0x%x, Order = 0x%x, return - %r\n", \
                 CurrentFchSmmSxNodePtr->Context.Type, CurrentFchSmmSxNodePtr->Context.Order, Status));
        }
        ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88) = Slp_Type;
        SleepTypeDispatched++;
      }
      CurrentFchSmmSxNodePtr = CurrentFchSmmSxNodePtr->FchSxNodePtr;
    }
    if (SleepTypeDispatched == 0) {
      Status = EFI_NOT_FOUND;
    } else {
      Status = EFI_SUCCESS;
      ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBE) |= BIT5;
      ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) &= ~ (BIT0 + BIT1);
      ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) |= BIT0;

      ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) &= ~(BIT2 + BIT3);

      LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT16, AcpiPmCntBase, &SleepTypeValue);
      SleepTypeValue &= (BIT10 + BIT11 + BIT12);
      LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, AcpiPmCntBase, &SleepTypeValue);

      AsmWbinvd();
      SmuNotifyS3Entry ();

      CurrentFchSmmSxNodePtr = HeadFchSmmSxNodePtr;
      while (CurrentFchSmmSxNodePtr->FchSxNodePtr != NULL) {
        if (CurrentFchSmmSxNodePtr->Context.Phase == SxExit) {
          if (CurrentFchSmmSxNodePtr->CallBack2Function != NULL) {
            Status = CurrentFchSmmSxNodePtr->CallBack2Function (
                                               CurrentFchSmmSxNodePtr->DispatchHandle,
                                               &CurrentFchSmmSxNodePtr->Context,
                                               NULL,
                                               NULL
                                               );
          DEBUG ((DEBUG_INFO, "[FchSmmDispatcher] Sx SMM handler dispatched (Phase = SxExit) SleepType = 0x%x, Order = 0x%x, return - %r\n", \
                 CurrentFchSmmSxNodePtr->Context.Type, CurrentFchSmmSxNodePtr->Context.Order, Status));
          }
        }
        CurrentFchSmmSxNodePtr = CurrentFchSmmSxNodePtr->FchSxNodePtr;
      }

      ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) &= ~(BIT2 + BIT3);
      ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) |= BIT2;

      ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBE) &= ~ (BIT5);
      ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) &= ~ (BIT0 + BIT1);
      ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) |= BIT1;

      ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) |= Eos;
    }
  }
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Register Sleep Type child handler
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
FchSmmSxDispatch2Register (
  IN       CONST FCH_SMM_SX_DISPATCH2_PROTOCOL  *This,
  IN       FCH_SMM_SX_HANDLER_ENTRY_POINT2      CallBackFunction,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT    *RegisterContext,
     OUT   EFI_HANDLE                           *DispatchHandle
  )
{
  EFI_STATUS      Status;
  FCH_SMM_SX_NODE *NewFchSmmSxNodePtr;
  FCH_SMM_SX_NODE *CurrentFchSmmSxNodePtr;
  FCH_SMM_SX_NODE *PreviousFchSmmSxNodePtr;

  Status = EFI_OUT_OF_RESOURCES;

  if (CallBackFunction == NULL || RegisterContext == NULL || DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }
//[-start-240426-IB18160212-modify-]//
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_SX_NODE),
                    (VOID**)&NewFchSmmSxNodePtr
                    );
  ASSERT_EFI_ERROR (Status);
//[end-240426-IB18160212-modify]//

  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) &= ~(BIT2 + BIT3); //Enable Sleep type Smi
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) |= BIT2;

  NewFchSmmSxNodePtr->CallBack2Function = CallBackFunction;
  NewFchSmmSxNodePtr->Context = *RegisterContext;
  *DispatchHandle = &NewFchSmmSxNodePtr->DispatchHandle;
  NewFchSmmSxNodePtr->DispatchHandle = *DispatchHandle;

  DEBUG ((DEBUG_INFO, "[FchSmmDispatcher] Registering Sx SMM handler: SleepType = 0x%x, Phase = 0x%x, Order = 0x%x\n", \
         RegisterContext->Type, RegisterContext->Phase, RegisterContext->Order));
  if (HeadFchSmmSxNodePtr->FchSxNodePtr == NULL) {
    NewFchSmmSxNodePtr->FchSxNodePtr = HeadFchSmmSxNodePtr;
    HeadFchSmmSxNodePtr = NewFchSmmSxNodePtr;
  } else {
    PreviousFchSmmSxNodePtr = HeadFchSmmSxNodePtr;
    CurrentFchSmmSxNodePtr = HeadFchSmmSxNodePtr;
    while (CurrentFchSmmSxNodePtr->FchSxNodePtr != NULL) {
      if (NewFchSmmSxNodePtr->Context.Order <= CurrentFchSmmSxNodePtr->Context.Order &&
          CurrentFchSmmSxNodePtr->Context.Type == NewFchSmmSxNodePtr->Context.Type &&
          CurrentFchSmmSxNodePtr->Context.Phase == NewFchSmmSxNodePtr->Context.Phase ) {

        if (PreviousFchSmmSxNodePtr == CurrentFchSmmSxNodePtr) {
          NewFchSmmSxNodePtr->FchSxNodePtr = HeadFchSmmSxNodePtr;
          HeadFchSmmSxNodePtr = NewFchSmmSxNodePtr;
          Status = EFI_SUCCESS;
          return Status;
        }
        NewFchSmmSxNodePtr->FchSxNodePtr = PreviousFchSmmSxNodePtr->FchSxNodePtr;
        PreviousFchSmmSxNodePtr->FchSxNodePtr = NewFchSmmSxNodePtr;

        Status = EFI_SUCCESS;
        return Status;
      }
      PreviousFchSmmSxNodePtr = CurrentFchSmmSxNodePtr;
      CurrentFchSmmSxNodePtr = CurrentFchSmmSxNodePtr->FchSxNodePtr;
    }
    PreviousFchSmmSxNodePtr->FchSxNodePtr = NewFchSmmSxNodePtr;
    NewFchSmmSxNodePtr->FchSxNodePtr = CurrentFchSmmSxNodePtr;
  }
  Status = EFI_SUCCESS;
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Unregister Sleep Type child handler
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
FchSmmSxDispatch2UnRegister (
  IN       CONST FCH_SMM_SX_DISPATCH2_PROTOCOL   *This,
  IN       EFI_HANDLE                            DispatchHandle
  )
{
  EFI_STATUS      Status;
  FCH_SMM_SX_NODE *CurrentFchSmmSxNodePtr;
  FCH_SMM_SX_NODE *PreviousFchSmmSxNodePtr;

  if (DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (HeadFchSmmSxNodePtr->FchSxNodePtr == NULL) {
    Status = EFI_NOT_FOUND;
    return  Status;
  } else {
    PreviousFchSmmSxNodePtr = HeadFchSmmSxNodePtr;
    CurrentFchSmmSxNodePtr = HeadFchSmmSxNodePtr;
    if (CurrentFchSmmSxNodePtr->DispatchHandle == DispatchHandle) {
      HeadFchSmmSxNodePtr = CurrentFchSmmSxNodePtr->FchSxNodePtr;
    } else {
      while (CurrentFchSmmSxNodePtr->DispatchHandle != DispatchHandle) {
        PreviousFchSmmSxNodePtr = CurrentFchSmmSxNodePtr;
        CurrentFchSmmSxNodePtr = CurrentFchSmmSxNodePtr->FchSxNodePtr;
        if (CurrentFchSmmSxNodePtr->DispatchHandle == NULL) {
          Status = EFI_NOT_FOUND;
          return  Status;
        }
      }
      PreviousFchSmmSxNodePtr->FchSxNodePtr = CurrentFchSmmSxNodePtr->FchSxNodePtr;
    }

    Status = gSmst->SmmFreePool (
                         CurrentFchSmmSxNodePtr
                         );
    ASSERT_EFI_ERROR (Status);
  }
  Status = EFI_SUCCESS;
  return  Status;
}


EFI_STATUS
EFIAPI
EfiSmmSxDispatch2Register (
  IN       CONST EFI_SMM_SX_DISPATCH2_PROTOCOL   *This,
  IN       EFI_SMM_HANDLER_ENTRY_POINT2          DispatchFunction,
  IN       CONST EFI_SMM_SX_REGISTER_CONTEXT     *RegisterContext,
     OUT   EFI_HANDLE                            *DispatchHandle
  )
{
  FCH_SMM_SX_REGISTER_CONTEXT    FchRegisterContext;

  FchRegisterContext.Type  = RegisterContext->Type;
  FchRegisterContext.Phase = RegisterContext->Phase;
  FchRegisterContext.Order = 0x80;      // SMM_DEFAULT_ORDER
  //[-start-240426-IB18160212-modify-]//
  return FchSmmSxDispatch2Register (
           &gFchSmmSxDispatch2Protocol,
           (FCH_SMM_SX_HANDLER_ENTRY_POINT2)DispatchFunction,
           &FchRegisterContext,
           DispatchHandle
           );
  //[-start-240426-IB18160212-modify-]//
}


EFI_STATUS
EFIAPI
EfiSmmSxDispatch2UnRegister (
  IN       CONST EFI_SMM_SX_DISPATCH2_PROTOCOL *This,
  IN       EFI_HANDLE                          DispatchHandle
  )
{
  return FchSmmSxDispatch2UnRegister (
           &gFchSmmSxDispatch2Protocol,
           DispatchHandle
           );
}

EFI_SMM_SX_DISPATCH2_PROTOCOL gEfiSmmSxDispatch2Protocol = {
  EfiSmmSxDispatch2Register,
  EfiSmmSxDispatch2UnRegister
};


