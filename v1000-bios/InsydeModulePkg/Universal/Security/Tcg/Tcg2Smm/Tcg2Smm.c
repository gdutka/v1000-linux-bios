/** @file
  It updates TPM2 items in ACPI table and registers SMI2 callback
  functions for Tcg2 physical presence, ClearMemory, and sample
  for dTPM StartMethod.

  Caution: This module requires additional review when modified.
  This driver will have external input - variable and ACPINvs data in SMM mode.
  This external input must be validated carefully to avoid security issue.

  PhysicalPresenceCallback() and MemoryClearCallback() will receive untrusted input and do some check.

;******************************************************************************
;* Copyright (c) 2015 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/**
Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials 
are licensed and made available under the terms and conditions of the BSD License 
which accompanies this distribution.  The full text of the license may be found at 
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Tcg2Smm.h"
#define TPM2_CONTROL_AREA_SIZE            1   // 1 Pages for Control Area

EFI_TPM2_ACPI_TABLE  mTpm2AcpiTemplate = {
  {
    EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE,
    sizeof (mTpm2AcpiTemplate),
    EFI_TPM2_ACPI_TABLE_REVISION,
    //
    // Compiler initializes the remaining bytes to 0
    // These fields should be filled in in production
    //
  },
  0, // BIT0~15:  PlatformClass
     // BIT16~31: Reserved
  0, // Control Area
  EFI_TPM2_ACPI_TABLE_START_METHOD_TIS, // StartMethod
};

TCG_NVS                    *mTcgNvs               = NULL;
EFI_TPM2_ACPI_CONTROL_AREA *mControlAreaPtr       = NULL;
UINT32                     *mStartMethod          = NULL;
UINT64                     mNvsControlAreaAddress = 0;

/**
  Get BurstCount by reading the burstCount field of a TIS regiger and return response data.

  @param[in]      TisReg        TPM register space base address.  
  @param[in, out] BufferOut     Buffer for response data.  
  @param[in, out] SizeOut       Size of response data.  
 
  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_UNSUPPORTED       Unsupported TPM version

**/
EFI_STATUS
AcpiStartMethodTisReceive (
  IN     TIS_PC_REGISTERS_PTR       TisReg,
  IN OUT UINT8                      *BufferOut,
  IN OUT UINT32                     *SizeOut
  )
{
  UINT16                            Data16;
  UINT32                            Data32;
  UINT32                            TpmOutSize;
  EFI_STATUS                        Status;
  UINT32                            Index;
  UINT16                            BurstCount;

  TpmOutSize = 0;
  //
  // Get response data header
  //
  Index = 0;
  BurstCount = 0;
  Status = EFI_SUCCESS;
  while (Index < sizeof (TPM2_RESPONSE_HEADER)) {
    Status = TisPcReadBurstCount (TisReg, &BurstCount);
    if (EFI_ERROR (Status)) {
      Status = EFI_TIMEOUT;
      goto Exit;
    }
    for (; BurstCount > 0; BurstCount--) {
      *(BufferOut + Index) = MmioRead8 ((UINTN)&TisReg->DataFifo);
      Index++;
      if (Index == sizeof (TPM2_RESPONSE_HEADER)) break;
    }
  }
  DEBUG_CODE (
    DEBUG ((EFI_D_INFO, "TisTpmCommand ReceiveHeader - "));
    for (Index = 0; Index < sizeof (TPM2_RESPONSE_HEADER); Index++) {
      DEBUG ((EFI_D_INFO, "%02x ", BufferOut[Index]));
    }
    DEBUG ((EFI_D_INFO, "\n"));
  );
  //
  // Check the reponse data header (tag,parasize and returncode )
  //
  CopyMem (&Data16, BufferOut, sizeof (UINT16));
  //
  // TPM2 should not use this RSP_COMMAND
  //
  if (SwapBytes16 (Data16) == TPM_ST_RSP_COMMAND) {
    DEBUG ((EFI_D_ERROR, "TPM_ST_RSP error - %x\n", TPM_ST_RSP_COMMAND));
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  CopyMem (&Data32, (BufferOut + 2), sizeof (UINT32));
  TpmOutSize  = SwapBytes32 (Data32);
  if (*SizeOut < TpmOutSize) {
    Status = EFI_BUFFER_TOO_SMALL;
    goto Exit;
  }
  *SizeOut = TpmOutSize;
  //
  // Continue reading the remaining data
  //
  while (Index < TpmOutSize) {
    for (; BurstCount > 0; BurstCount--) {
      *(BufferOut + Index) = MmioRead8 ((UINTN)&TisReg->DataFifo);
      Index++;
      if (Index == TpmOutSize) {
        Status = EFI_SUCCESS;
        goto Exit;
      }
    }
    Status = TisPcReadBurstCount (TisReg, &BurstCount);
    if (EFI_ERROR (Status)) {
      Status = EFI_TIMEOUT;
      goto Exit;
    }
  }
Exit:
  DEBUG_CODE (
    DEBUG ((EFI_D_INFO, "TisTpmCommand Receive - "));
    for (Index = 0; Index < TpmOutSize; Index++) {
      DEBUG ((EFI_D_INFO, "%02x ", BufferOut[Index]));
    }
    DEBUG ((EFI_D_INFO, "\n"));
  );
  MmioWrite8((UINTN)&TisReg->Status, TIS_PC_STS_READY);
  return Status;
}

/**
  Initial control area specific data.

  @retval EFI_SUCCESS     Operation completed successfully.
**/
EFI_STATUS
ControlAreaInit (
  VOID
  )
{
  EFI_TPM2_ACPI_CONTROL_AREA              *ControlArea;

  if (IsCrbInterfaceActive () && FeaturePcdGet (PcdH2OTpm2PtpSupported)) {
    mTpm2AcpiTemplate.StartMethod = EFI_TPM2_ACPI_TABLE_START_METHOD_COMMAND_RESPONSE_BUFFER_INTERFACE;
    mTpm2AcpiTemplate.AddressOfControlArea = (UINT64)(UINTN) (PcdGet64 (PcdTpmBaseAddress) + R_CRB_CONTROL_REQ);

    ControlArea = (EFI_TPM2_ACPI_CONTROL_AREA *)(UINTN)mTpm2AcpiTemplate.AddressOfControlArea;
    ControlArea->CommandSize  = MmioRead32 ((UINTN) (PcdGet64 (PcdTpmBaseAddress) + R_CRB_CONTROL_CMD_SIZE));
    ControlArea->ResponseSize = MmioRead32 ((UINTN) (PcdGet64 (PcdTpmBaseAddress) + R_CRB_CONTROL_RESPONSE_SIZE));
    ControlArea->Command      = (UINT64)((UINTN)(PcdGet64 (PcdTpmBaseAddress) + R_CRB_DATA_BUFFER));
    ControlArea->Response     = (UINT64)((UINTN)(PcdGet64 (PcdTpmBaseAddress) + R_CRB_DATA_BUFFER));
  } else if (mTpm2AcpiTemplate.StartMethod == EFI_TPM2_ACPI_TABLE_START_METHOD_ACPI) {
    mTpm2AcpiTemplate.StartMethod = EFI_TPM2_ACPI_TABLE_START_METHOD_ACPI;
    mTpm2AcpiTemplate.AddressOfControlArea = mNvsControlAreaAddress; // Use NVS memory for control area.

    ControlArea = (EFI_TPM2_ACPI_CONTROL_AREA *)(UINTN)mTpm2AcpiTemplate.AddressOfControlArea;
    ControlArea->CommandSize  = EFI_PAGES_TO_SIZE (TPM2_CONTROL_AREA_SIZE) - sizeof (EFI_TPM2_ACPI_CONTROL_AREA);
    ControlArea->ResponseSize = EFI_PAGES_TO_SIZE (TPM2_CONTROL_AREA_SIZE) - sizeof (EFI_TPM2_ACPI_CONTROL_AREA);
    ControlArea->Command      = (UINT64)((UINT8 *)ControlArea + sizeof(EFI_TPM2_ACPI_CONTROL_AREA));
    ControlArea->Response     = (UINT64)((UINT8 *)ControlArea + sizeof(EFI_TPM2_ACPI_CONTROL_AREA));
    mControlAreaPtr = ControlArea;
    ASSERT (mControlAreaPtr != NULL);
  }
  return EFI_SUCCESS;
}
/**
  Software SMI callback for TPM physical presence which is called from ACPI method.

  Caution: This function may receive untrusted input.
  Variable and ACPINvs are external input, so this function will validate
  its data structure to be valid value.

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[in, out] CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS             The interrupt was handled successfully.

**/
EFI_STATUS
EFIAPI
PhysicalPresenceCallback (
  IN EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                  *Context,
  IN OUT VOID                    *CommBuffer,
  IN OUT UINTN                   *CommBufferSize
  )
{
  UINT32                MostRecentRequest;
  UINT32                Response;
  UINT32                OperationRequest;
  UINT32                RequestParameter;


  if (mTcgNvs->PhysicalPresence.Parameter == TCG_ACPI_FUNCTION_RETURN_REQUEST_RESPONSE_TO_OS) {
    mTcgNvs->PhysicalPresence.ReturnCode = Tcg2PhysicalPresenceLibReturnOperationResponseToOsFunction (
                                             &MostRecentRequest,
                                             &Response
                                             );
    mTcgNvs->PhysicalPresence.LastRequest = MostRecentRequest;
    mTcgNvs->PhysicalPresence.Response    = Response;
    return EFI_SUCCESS;
  } else if ((mTcgNvs->PhysicalPresence.Parameter == TCG_ACPI_FUNCTION_SUBMIT_REQUEST_TO_BIOS) 
          || (mTcgNvs->PhysicalPresence.Parameter == TCG_ACPI_FUNCTION_SUBMIT_REQUEST_TO_BIOS_2)) {

    OperationRequest = mTcgNvs->PhysicalPresence.Request;
    RequestParameter = mTcgNvs->PhysicalPresence.RequestParameter;
    mTcgNvs->PhysicalPresence.ReturnCode = Tcg2PhysicalPresenceLibSubmitRequestToPreOSFunctionEx (
                                             &OperationRequest,
                                             &RequestParameter
                                             );
    mTcgNvs->PhysicalPresence.Request = OperationRequest;
    mTcgNvs->PhysicalPresence.RequestParameter = RequestParameter;
  } else if (mTcgNvs->PhysicalPresence.Parameter == TCG_ACPI_FUNCTION_GET_USER_CONFIRMATION_STATUS_FOR_REQUEST) {
    mTcgNvs->PhysicalPresence.ReturnCode = Tcg2PhysicalPresenceLibGetUserConfirmationStatusFunction (mTcgNvs->PPRequestUserConfirm);
  }

  return EFI_SUCCESS;
}


/**
  Software SMI callback for MemoryClear which is called from ACPI method.

  Caution: This function may receive untrusted input.
  Variable and ACPINvs are external input, so this function will validate
  its data structure to be valid value.

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[in, out] CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS             The interrupt was handled successfully.

**/
EFI_STATUS
EFIAPI
MemoryClearCallback (
  IN EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                  *Context,
  IN OUT VOID                    *CommBuffer,
  IN OUT UINTN                   *CommBufferSize
  )
{
  EFI_STATUS                     Status;
  UINTN                          DataSize;
  UINT8                          MorControl;

  mTcgNvs->MemoryClear.ReturnCode = MOR_REQUEST_SUCCESS;
  if (mTcgNvs->MemoryClear.Parameter == ACPI_FUNCTION_DSM_MEMORY_CLEAR_INTERFACE) {
    MorControl = (UINT8) mTcgNvs->MemoryClear.Request;
  } else if (mTcgNvs->MemoryClear.Parameter == ACPI_FUNCTION_PTS_CLEAR_MOR_BIT) {
    DataSize = sizeof (UINT8);
    Status = CommonGetVariable (
               MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
               &gEfiMemoryOverwriteControlDataGuid,
               &DataSize,
               &MorControl
               );
    if (EFI_ERROR (Status)) {
      mTcgNvs->MemoryClear.ReturnCode = MOR_REQUEST_GENERAL_FAILURE;
      DEBUG ((EFI_D_ERROR, "[TPM] Get MOR variable failure! Status = %r\n", Status));
      return EFI_SUCCESS;
    }

    if (MOR_CLEAR_MEMORY_VALUE (MorControl) == 0x0) {
      return EFI_SUCCESS;
    }
    MorControl &= ~MOR_CLEAR_MEMORY_BIT_MASK;
  }

  DataSize = sizeof (UINT8);
  Status = CommonSetVariable (
             MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
             &gEfiMemoryOverwriteControlDataGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             DataSize,
             &MorControl
             );
  if (EFI_ERROR (Status)) {
    mTcgNvs->MemoryClear.ReturnCode = MOR_REQUEST_GENERAL_FAILURE;
    DEBUG ((EFI_D_ERROR, "[TPM] Set MOR variable failure! Status = %r\n", Status));
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ControlAreaHandlerCallback (
  IN EFI_HANDLE                     DispatchHandle,
  IN CONST VOID                     *Context,
  IN OUT VOID                       *CommBuffer,
  IN OUT UINTN                      *CommBufferSize
  )
{
  EFI_STATUS                        Status;
  TPM2_COMMAND_HEADER               *CmdHdr;
  UINT32                            CmdSize;
  UINT8                             *InputParameterBlock;
  UINT32                            InputParameterBlockSize;
  UINT32                            OutputParameterBlockSize;
  UINT8                             *OutputParameterBlock;
  UINT32                            *ReturnCode;

  ReturnCode = (UINT32 *) (UINTN) mControlAreaPtr->Response;
  
  if (mControlAreaPtr->Start == 0) {
    //
    // No action
    //
    return EFI_SUCCESS;
  }

  if (*ReturnCode != TPM_RC_CANCELED) {
    
    CmdHdr = (TPM2_COMMAND_HEADER*) (UINTN) mControlAreaPtr->Command;
    CmdSize = SwapBytes32 (CmdHdr->paramSize);
    if (CmdSize == 0) {
      //
      // Invalid command, clear Start bit and return TPM_RC_RETRY
      //
      *ReturnCode = TPM_RC_RETRY;
      mControlAreaPtr->Start = 0;
      return EFI_SUCCESS;
    }
    
    InputParameterBlockSize = CmdSize;
    InputParameterBlock = (UINT8 *) (UINTN) mControlAreaPtr->Command;
    
    Status = TisSend (
               (TIS_PC_REGISTERS_PTR)(UINTN) PcdGet64 (PcdTpmBaseAddress),
               InputParameterBlock,
               InputParameterBlockSize
               );
    if (EFI_ERROR (Status)) {
      //
      // Command can't be proceeded, clear Start bit and return TPM_RC_RETRY
      //
      *ReturnCode = TPM_RC_RETRY;
      mControlAreaPtr->Start = 0;
    } else {
      //
      //  Set response TPM_RC_CANCELED in advance in case of getting time-out in AML
      //
      *ReturnCode = TPM_RC_CANCELED;
    }
    return EFI_SUCCESS;
  }

  OutputParameterBlockSize = mControlAreaPtr->ResponseSize;
  OutputParameterBlock = (UINT8 *) (UINTN) mControlAreaPtr->Response;

  Status = AcpiStartMethodTisReceive (
             (TIS_PC_REGISTERS_PTR)(UINTN) PcdGet64 (PcdTpmBaseAddress),
             OutputParameterBlock,
             &OutputParameterBlockSize
             );
  if (EFI_ERROR (Status)) {
    //
    // Command failed, TPM_RC_RETRY should be returned
    //
    *ReturnCode = TPM_RC_RETRY;
  }

  //
  // Completion, clear Start bit
  //
  mControlAreaPtr->Start = 0;
  return EFI_SUCCESS;
}

/**
  Find the operation region in TCG ACPI table by given Name and Size,
  and initialize it if the region is found.

  @param[in, out] Table          The TPM item in ACPI table.
  @param[in]      Name           The name string to find in TPM table.
  @param[in]      Size           The size of the region to find.
  @param[in]      Address        The address for the found region.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval Others                 Operation was failed.

**/
EFI_STATUS
AssignOpRegion (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN     UINT32                       Name,
  IN     UINT16                       Size,
  IN     VOID                         *Address
  )
{
  EFI_STATUS                          Status;
  AML_OP_REGION_32_8                  *OpRegion;

  if (Address == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;
  //
  // Patch some pointers for the ASL code before loading the SSDT.
  //
  for (OpRegion  = (AML_OP_REGION_32_8 *) (Table + 1);
       OpRegion <= (AML_OP_REGION_32_8 *) ((UINT8 *) Table + Table->Length);
       OpRegion  = (AML_OP_REGION_32_8 *) ((UINT8 *) OpRegion + 1)) {
    if ((OpRegion->OpRegionOp  == AML_EXT_REGION_OP) &&
        (OpRegion->NameString  == Name) &&
        (OpRegion->DWordPrefix == AML_DWORD_PREFIX) &&
        (OpRegion->BytePrefix  == AML_BYTE_PREFIX)) {
      OpRegion->RegionOffset = (UINT32) (UINTN) Address;
      OpRegion->RegionLen    = (UINT8) Size;
      Status = EFI_SUCCESS;
      break;
    }
  }

  return Status;
}

/**
  Patch interrupt resources returned by TPM _PRS. ResourceTemplate to patch is determined by input
  interrupt buffer size. BufferSize, PkgLength and interrupt descirptor in ByteList need to be patched

  @param[in, out] Table            The TPM item in ACPI table.
  @param[in]      IrqBuffer        Input new IRQ buffer.
  @param[in]      IrqBuffserSize   Input new IRQ buffer size.
  @param[out]     IsShortFormPkgLength   If _PRS returns Short length Package(ACPI spec 20.2.4).

  @return                          patch status.

**/
EFI_STATUS
UpdatePossibleResource (
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER    *Table,
  IN      UINT32                         *IrqBuffer,
  IN      UINT32                         IrqBuffserSize,
     OUT  BOOLEAN                        *IsShortFormPkgLength
  )
{
  UINT8       *DataPtr;
  UINT8       *DataEndPtr;
  UINT32      NewPkgLength;
  UINT32      OrignalPkgLength;

  NewPkgLength     = 0;
  OrignalPkgLength = 0;
  DataEndPtr       = NULL;

  //
  // Follow ACPI spec
  //           6.4.3   Extend Interrupt Descriptor.
  //           19.3.3 ASL Resource Template
  //           20      AML specification
  // to patch TPM ACPI object _PRS returned ResourceTemplate() containing 2 resource descriptors and an auto appended End Tag
  //
  //  AML data is organized by following rule.
  //  Code need to patch BufferSize and PkgLength and interrupt descirptor in ByteList
  //
  // =============  Buffer ====================
  //           DefBuffer := BufferOp PkgLength BufferSize ByteList
  //            BufferOp := 0x11
  //
  // ==============PkgLength==================
  //          PkgLength := PkgLeadByte |
  //                              <PkgLeadByte ByteData> |
  //                              <PkgLeadByte ByteData ByteData> |
  //                              <PkgLeadByte ByteData ByteData ByteData>
  //
  //       PkgLeadByte := <bit 7-6: ByteData count that follows (0-3)>
  //                               <bit 5-4: Only used if PkgLength <= 63 >
  //                               <bit 3-0: Least significant package length nybble>
  //
  //==============BufferSize==================
  //        BufferSize := Integar
  //           Integar := ByteConst|WordConst|DwordConst....
  //
  //           ByteConst := BytePrefix ByteData
  //
  //==============ByteList===================
  //          ByteList := ByteData ByteList
  //
  //=========================================

  //
  // 1. Check TPM_PRS_RESS with PkgLength <=63 can hold the input interrupt number buffer for patching
  //
  for (DataPtr  = (UINT8 *)(Table + 1);
       DataPtr < (UINT8 *)((UINT8 *)Table + Table->Length - (TPM_PRS_RES_NAME_SIZE + TPM_POS_RES_TEMPLATE_MIN_SIZE));
       DataPtr += 1) {
    if (CompareMem (DataPtr, TPM_PRS_RESS, TPM_PRS_RES_NAME_SIZE) == 0) {
      //
      // Jump over object name & BufferOp
      //
      DataPtr += TPM_PRS_RES_NAME_SIZE + 1;

      if ((*DataPtr & (BIT7 | BIT6)) == 0) {
        OrignalPkgLength = (UINT32)*DataPtr;
        DataEndPtr       = DataPtr + OrignalPkgLength;

        //
        // Jump over PkgLength = PkgLeadByte only
        //
        NewPkgLength++;

        //
        // Jump over BufferSize
        //
        if (*(DataPtr + 1) == AML_BYTE_PREFIX) {
          NewPkgLength += 2;
        } else if (*(DataPtr + 1) == AML_WORD_PREFIX) {
          NewPkgLength += 3;
        } else if (*(DataPtr + 1) == AML_DWORD_PREFIX) {
          NewPkgLength += 5;
        } else {
          ASSERT (FALSE);
          return EFI_UNSUPPORTED;
        }
      } else {
        ASSERT (FALSE);
        return EFI_UNSUPPORTED;
      }

      //
      // Include Memory32Fixed Descritor (12 Bytes) + Interrupt Descriptor header(5 Bytes) + End Tag(2 Bytes)
      //
      NewPkgLength += 19 + IrqBuffserSize;
      if (NewPkgLength > 63) {
        break;
      }

      if (NewPkgLength > OrignalPkgLength) {
        ASSERT (FALSE);
        return EFI_INVALID_PARAMETER;
      }

      //
      // 1.1 Patch PkgLength
      //
      *DataPtr = (UINT8)NewPkgLength;

      //
      // 1.2 Patch BufferSize = sizeof(Memory32Fixed Descritor + Interrupt Descriptor + End Tag).
      //      It is Little endian. So only patch lowest byte of BufferSize due to current interrupt number limit.
      //
      *(DataPtr + 2) = (UINT8)(IrqBuffserSize + 19);

      //
      // Notify _PRS to report short formed ResourceTemplate
      //
      *IsShortFormPkgLength = TRUE;

      break;
    }
  }

  //
  // 2. Use TPM_PRS_RESL with PkgLength > 63 to hold longer input interrupt number buffer for patching
  //
  if (NewPkgLength > 63) {
    NewPkgLength     = 0;
    OrignalPkgLength = 0;
    for (DataPtr  = (UINT8 *)(Table + 1);
         DataPtr < (UINT8 *)((UINT8 *)Table + Table->Length - (TPM_PRS_RES_NAME_SIZE + TPM_POS_RES_TEMPLATE_MIN_SIZE));
         DataPtr += 1) {
      if (CompareMem (DataPtr, TPM_PRS_RESL, TPM_PRS_RES_NAME_SIZE) == 0) {
        //
        // Jump over object name & BufferOp
        //
        DataPtr += TPM_PRS_RES_NAME_SIZE + 1;

        if ((*DataPtr & (BIT7 | BIT6)) != 0) {
          OrignalPkgLength = (UINT32)(*(DataPtr + 1) << 4) + (*DataPtr & 0x0F);
          DataEndPtr       = DataPtr + OrignalPkgLength;
          //
          // Jump over PkgLength = PkgLeadByte + ByteData length
          //
          NewPkgLength += 1 + ((*DataPtr & (BIT7|BIT6)) >> 6);

          //
          // Jump over BufferSize
          //
          if (*(DataPtr + NewPkgLength) == AML_BYTE_PREFIX) {
            NewPkgLength += 2;
          } else if (*(DataPtr + NewPkgLength) == AML_WORD_PREFIX) {
            NewPkgLength += 3;
          } else if (*(DataPtr + NewPkgLength) == AML_DWORD_PREFIX) {
            NewPkgLength += 5;
          } else {
            ASSERT (FALSE);
            return EFI_UNSUPPORTED;
          }
        } else {
          ASSERT (FALSE);
          return EFI_UNSUPPORTED;
        }

        //
        // Include Memory32Fixed Descritor (12 Bytes) + Interrupt Descriptor header(5 Bytes) + End Tag(2  Bytes)
        //
        NewPkgLength += 19 + IrqBuffserSize;

        if (NewPkgLength > OrignalPkgLength) {
          ASSERT (FALSE);
          return EFI_INVALID_PARAMETER;
        }

        //
        // 2.1 Patch PkgLength. Only patch PkgLeadByte and first ByteData
        //
        *DataPtr = (UINT8)((*DataPtr) & 0xF0) | (NewPkgLength & 0x0F);
        *(DataPtr + 1) = (UINT8)((NewPkgLength & 0xFF0) >> 4);

        //
        // 2.2 Patch BufferSize = sizeof(Memory32Fixed Descritor + Interrupt Descriptor + End Tag).
        //     It is Little endian. Only patch lowest byte of BufferSize due to current interrupt number limit.
        //
        *(DataPtr + 2 + ((*DataPtr & (BIT7|BIT6)) >> 6)) = (UINT8)(IrqBuffserSize + 19);

        //
        // Notify _PRS to report long formed ResourceTemplate
        //
        *IsShortFormPkgLength = FALSE;
        break;
      }
    }
  }

  if (DataPtr >= (UINT8 *)((UINT8 *)Table + Table->Length - (TPM_PRS_RES_NAME_SIZE + TPM_POS_RES_TEMPLATE_MIN_SIZE))) {
    return EFI_NOT_FOUND;
  }

  //
  // 3. Move DataPtr to Interrupt descriptor header and patch interrupt descriptor.
  //     5 bytes for interrupt descriptor header, 2 bytes for End Tag
  //
  DataPtr += NewPkgLength - (5 + IrqBuffserSize + 2);
  //
  //   3.1 Patch Length bit[7:0] of Interrupt descirptor patch interrupt descriptor
  //
  *(DataPtr + 1) = (UINT8)(2 + IrqBuffserSize);
  //
  //   3.2 Patch Interrupt Table Length
  //
  *(DataPtr + 4) = (UINT8)(IrqBuffserSize / sizeof(UINT32));
  //
  //   3.3 Copy patched InterruptNumBuffer
  //
  CopyMem (DataPtr + 5, IrqBuffer, IrqBuffserSize);

  //
  // 4. Jump over Interrupt descirptor and Patch END Tag, set Checksum field to 0
  //
  DataPtr       += 5 + IrqBuffserSize;
  *DataPtr       = ACPI_END_TAG_DESCRIPTOR;
  *(DataPtr + 1) = 0;

  //
  // 5. Jump over new ResourceTemplate. Stuff rest bytes to NOOP
  //
  DataPtr += 2;
  if (DataPtr < DataEndPtr) {
    SetMem (DataPtr, (UINTN)DataEndPtr - (UINTN)DataPtr, AML_NOOP_OP);
  }

  return EFI_SUCCESS;
}

/**
  Patch TPM2 device HID string.  The initial string tag in TPM2 ACPI table is "NNN0000".

  @param[in, out] Table          The TPM2 SSDT ACPI table.

  @return                               HID Update status.

**/
EFI_STATUS
UpdateHID (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER    *Table
  )
{
  EFI_STATUS  Status;
  UINT8       *DataPtr;
  CHAR8       Hid[TPM_HID_ACPI_SIZE];
  UINT32      ManufacturerID;
  UINT32      FirmwareVersion1;
  UINT32      FirmwareVersion2;
  BOOLEAN     PnpHID;

  PnpHID = TRUE;

  //
  // Initialize HID with Default PNP string
  //
  ZeroMem (Hid, TPM_HID_ACPI_SIZE);

  //
  // Get Manufacturer ID
  //
  Status = Tpm2GetCapabilityManufactureID (&ManufacturerID);
  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "TPM_PT_MANUFACTURER 0x%08x\n", ManufacturerID));
    //
    // ManfacturerID defined in TCG Vendor ID Registry 
    // may tailed with 0x00 or 0x20
    //
    if ((ManufacturerID >> 24) == 0x00 || ((ManufacturerID >> 24) == 0x20)) {
      //
      //  HID containing PNP ID "NNN####"
      //   NNN is uppercase letter for Vendor ID specified by manufacturer
      //
      CopyMem (Hid, &ManufacturerID, 3);
    } else {
      //
      //  HID containing ACP ID "NNNN####"
      //   NNNN is uppercase letter for Vendor ID specified by manufacturer
      //
      CopyMem (Hid, &ManufacturerID, 4);
      PnpHID = FALSE;
    }
  } else {
    DEBUG ((EFI_D_ERROR, "Get TPM_PT_MANUFACTURER failed %x!\n", Status));
    ASSERT (FALSE);
    return Status;
  }

  Status = Tpm2GetCapabilityFirmwareVersion (&FirmwareVersion1, &FirmwareVersion2);
  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "TPM_PT_FIRMWARE_VERSION_1 0x%x\n", FirmwareVersion1));
    DEBUG ((EFI_D_INFO, "TPM_PT_FIRMWARE_VERSION_2 0x%x\n", FirmwareVersion2));
    //
    //   #### is Firmware Version 1
    //
    if (PnpHID) {
      AsciiSPrint (Hid + 3, TPM_HID_PNP_SIZE - 3, "%02d%02d", ((FirmwareVersion1 & 0xFFFF0000) >> 16), (FirmwareVersion1 & 0x0000FFFF));
    } else {
      AsciiSPrint (Hid + 4, TPM_HID_ACPI_SIZE - 4, "%02d%02d", ((FirmwareVersion1 & 0xFFFF0000) >> 16), (FirmwareVersion1 & 0x0000FFFF));
    }
    
  } else {
    DEBUG ((EFI_D_ERROR, "Get TPM_PT_FIRMWARE_VERSION_X failed %x!\n", Status));
    ASSERT(FALSE);
    return Status;
  }

  //
  // Patch HID in ASL code before loading the SSDT.
  //
  for (DataPtr  = (UINT8 *)(Table + 1);
       DataPtr <= (UINT8 *)((UINT8 *) Table + Table->Length - TPM_HID_PNP_SIZE);
       DataPtr += 1) {
    if (AsciiStrCmp ((CHAR8 *)DataPtr,  TPM_HID_TAG) == 0) {
      if (PnpHID) {
        CopyMem (DataPtr, Hid, TPM_HID_PNP_SIZE);
        //
        // if HID is PNP ID, patch the last byte in HID TAG to Noop
        //
        *(DataPtr + TPM_HID_PNP_SIZE) = AML_NOOP_OP;
      } else {
        CopyMem (DataPtr, Hid, TPM_HID_ACPI_SIZE);
      }
      DEBUG ((EFI_D_INFO, "TPM2 ACPI _HID updated to %a\n", Hid));
      return Status;
    }
  }

  DEBUG ((EFI_D_ERROR, "TPM2 ACPI HID TAG for patch not found!\n"));
  return EFI_NOT_FOUND;
}

/**
  Initialize and publish TPM items in ACPI table.

  @retval   EFI_SUCCESS     The TCG ACPI table is published successfully.
  @retval   Others          The TCG ACPI table is not published.

**/
EFI_STATUS
PublishAcpiTable (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTable;
  UINTN                          TableKey;
  EFI_ACPI_DESCRIPTION_HEADER    *Table;
  UINTN                          TableSize;
  UINT32                         *PossibleIrqNumBuf;
  UINT32                         PossibleIrqNumBufSize;
  BOOLEAN                        IsShortFormPkgLength;

  IsShortFormPkgLength = FALSE;

  Status = GetSectionFromFv (
             &gEfiCallerIdGuid,
             EFI_SECTION_RAW,
             0,
             (VOID **) &Table,
             &TableSize
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Measure to PCR[0] with event EV_POST_CODE ACPI DATA.
  // The measurement has to be done before any update.
  // Otherwise, the PCR record would be different after TPM FW update
  // or the PCD configuration change.
  //
  TpmMeasureAndLogData(
    0,
    EV_POST_CODE,
    EV_POSTCODE_INFO_ACPI_DATA,
    ACPI_DATA_LEN,
    Table,
    TableSize
    );

  //
  // Update TPM2 HID after measuring it to PCR
  //
  Status = UpdateHID (Table);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (PcdGet32 (PcdTpm2CurrentIrqNum) != 0) {
    //
    // Patch _PRS interrupt resource only when TPM interrupt is supported
    //
    PossibleIrqNumBuf     = (UINT32 *)PcdGetPtr (PcdTpm2PossibleIrqNumBuf);
    PossibleIrqNumBufSize = (UINT32)PcdGetSize (PcdTpm2PossibleIrqNumBuf);

    if (PossibleIrqNumBufSize <= MAX_PRS_INT_BUF_SIZE && (PossibleIrqNumBufSize % sizeof (UINT32)) == 0) {
      Status = UpdatePossibleResource (Table, PossibleIrqNumBuf, PossibleIrqNumBufSize, &IsShortFormPkgLength);
      DEBUG ((
        DEBUG_INFO,
        "UpdatePossibleResource status - %x. TPM2 service may not ready in OS.\n",
        Status
        ));
    } else {
      DEBUG ((
        DEBUG_INFO,
        "PcdTpm2PossibleIrqNumBuf size %x is not correct. TPM2 service may not ready in OS.\n",
        PossibleIrqNumBufSize
      ));
    }
  }

  ASSERT (Table->OemTableId == SIGNATURE_64 ('T', 'p', 'm', '2', 'T', 'a', 'b', 'l'));
  CopyMem (Table->OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof (Table->OemId) );
  Status = AssignOpRegion (Table, SIGNATURE_32 ('T', 'N', 'V', 'S'), (UINT16) sizeof (TCG_NVS), (VOID *)mTcgNvs);
  ASSERT_EFI_ERROR (Status);
  mTcgNvs->TpmIrqNum            = PcdGet32(PcdTpm2CurrentIrqNum);
  mTcgNvs->IsShortFormPkgLength = IsShortFormPkgLength;

  Status = ControlAreaInit ();
  ASSERT_EFI_ERROR (Status);

  Status = AssignOpRegion (Table, SIGNATURE_32 ('C', 'O', 'N', 'A'), (UINT16) sizeof (EFI_TPM2_ACPI_CONTROL_AREA), (VOID *)mControlAreaPtr);
  ASSERT_EFI_ERROR (Status);

  Status = AssignOpRegion (Table, SIGNATURE_32 ('S', 'T', 'R', 'T'), (UINT16) sizeof (UINT32), (VOID *)mStartMethod);
  ASSERT_EFI_ERROR (Status);
  if (!EFI_ERROR (Status)) {
    *mStartMethod = mTpm2AcpiTemplate.StartMethod;
  }
  //
  // Publish the TPM ACPI table
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);
  ASSERT_EFI_ERROR (Status);

  TableKey = 0;
  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        Table,
                        TableSize,
                        &TableKey
                        );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Publish TPM2 ACPI table

  @retval   EFI_SUCCESS     The TPM2 ACPI table is published successfully.
  @retval   Others          The TPM2 ACPI table is not published.

**/
EFI_STATUS
PublishTpm2 (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTable;
  UINTN                          TableKey;
  UINT64                         OemTableId;

  //
  // Measure to PCR[0] with event EV_POST_CODE ACPI DATA.
  // The measurement has to be done before any update.
  // Otherwise, the PCR record would be different after event log update
  // or the PCD configuration change.
  //
  TpmMeasureAndLogData(
    0,
    EV_POST_CODE,
    EV_POSTCODE_INFO_ACPI_DATA,
    ACPI_DATA_LEN,
    &mTpm2AcpiTemplate,
    sizeof(mTpm2AcpiTemplate)
    );

  DEBUG((DEBUG_INFO, "Tpm2 ACPI table revision is %d\n", mTpm2AcpiTemplate.Header.Revision));

  //
  // PlatformClass is only valid for version 4 and above
  //    BIT0~15:  PlatformClass 
  //    BIT16~31: Reserved
  //
  if (mTpm2AcpiTemplate.Header.Revision >= EFI_TPM2_ACPI_TABLE_REVISION_4) {
    mTpm2AcpiTemplate.Flags = (mTpm2AcpiTemplate.Flags & 0xFFFF0000) | PcdGet8(PcdTpmPlatformClass);
    DEBUG((DEBUG_INFO, "Tpm2 ACPI table PlatformClass is %d\n", (mTpm2AcpiTemplate.Flags & 0x0000FFFF)));
  }

  CopyMem (mTpm2AcpiTemplate.Header.OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof (mTpm2AcpiTemplate.Header.OemId));
  OemTableId = PcdGet64 (PcdAcpiDefaultOemTableId);
  CopyMem (&mTpm2AcpiTemplate.Header.OemTableId, &OemTableId, sizeof (UINT64));
  mTpm2AcpiTemplate.Header.OemRevision      = PcdGet32 (PcdAcpiDefaultOemRevision);
  mTpm2AcpiTemplate.Header.CreatorId        = PcdGet32 (PcdAcpiDefaultCreatorId);
  mTpm2AcpiTemplate.Header.CreatorRevision  = PcdGet32 (PcdAcpiDefaultCreatorRevision);

  //
  // Construct ACPI table
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);
  ASSERT_EFI_ERROR (Status);

  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        &mTpm2AcpiTemplate,
                        sizeof(mTpm2AcpiTemplate),
                        &TableKey
                        );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Allocate required memory for TPM use.

  @retval   EFI_SUCCESS     Operation was successful.
  @retval   Others          Operation was failed.

**/
EFI_STATUS
AllocateMemoryForTpm (
  VOID
  )
{
  EFI_STATUS                          Status;
  EFI_PHYSICAL_ADDRESS                MemoryAddress;
  UINTN                               Page;  

  Page          = 1;
  MemoryAddress = SIZE_4GB - 1;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  Page,
                  &MemoryAddress
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem ((VOID *)(UINTN)MemoryAddress, EFI_PAGE_SIZE * Page);

  mTcgNvs = (TCG_NVS *)(UINTN)MemoryAddress;
  MemoryAddress += sizeof (TCG_NVS);

  mControlAreaPtr = (EFI_TPM2_ACPI_CONTROL_AREA *)(UINTN)MemoryAddress;
  MemoryAddress += sizeof (EFI_TPM2_ACPI_CONTROL_AREA);

  mStartMethod = (UINT32 *)(UINTN)MemoryAddress;
  MemoryAddress += sizeof (UINT32);

  Page          = TPM2_CONTROL_AREA_SIZE;
  MemoryAddress = SIZE_4GB - 1;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  Page,
                  &MemoryAddress
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem ((VOID *)(UINTN)MemoryAddress, EFI_PAGE_SIZE * Page);
  mNvsControlAreaAddress = (UINT64)(UINTN)MemoryAddress;

  return EFI_SUCCESS;
}

/**
  The driver's entry point.

  It install callbacks for TPM physical presence and MemoryClear, and locate 
  SMM variable to be used in the callback function.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.  
  @param[in] SystemTable  A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval Others          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeTcgSmm (
  IN EFI_HANDLE                  ImageHandle,
  IN EFI_SYSTEM_TABLE            *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT    SwContext;
  EFI_HANDLE                     SwHandle;
  VOID                           *Interface;

  //
  // Always allocate memory in case memory map changed during S4 resume.
  //
  Status = AllocateMemoryForTpm ();
  ASSERT_EFI_ERROR (Status);

  if (!CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20DtpmGuid)){
    DEBUG ((EFI_D_ERROR, "No TPM2 DTPM instance required!\n"));
    return EFI_UNSUPPORTED;
  }

  if (PcdGetBool (PcdTpmHide)) {
    DEBUG((EFI_D_INFO, "TPM is hidden\n"));
    return EFI_UNSUPPORTED;
  }

  if ((PcdGet32 (PcdDxeTpmPolicy) & SKIP_INSTALL_TCG_ACPI_TABLE) != 0) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (
                  &gEfiTcg2ProtocolGuid,
                  NULL,
                  (VOID **)&Interface
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = PublishAcpiTable ();
  ASSERT_EFI_ERROR (Status);

  //
  // Get the Sw dispatch protocol and register SMI callback functions.
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&SwDispatch);
  ASSERT_EFI_ERROR (Status);
  SwContext.SwSmiInputValue = (UINTN) SMM_TCG_PPI_CALL;
  Status = SwDispatch->Register (SwDispatch, PhysicalPresenceCallback, &SwContext, &SwHandle);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mTcgNvs->PhysicalPresence.SoftwareSmi = (UINT8) SwContext.SwSmiInputValue;

  SwContext.SwSmiInputValue = (UINTN) SMM_TCG_MOR_CALL;
  Status = SwDispatch->Register (SwDispatch, MemoryClearCallback, &SwContext, &SwHandle);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mTcgNvs->MemoryClear.SoftwareSmi = (UINT8) SwContext.SwSmiInputValue;

  if (mTpm2AcpiTemplate.StartMethod == EFI_TPM2_ACPI_TABLE_START_METHOD_ACPI) {
    //
    // Register SMI callback for ACPI method
    //
    SwContext.SwSmiInputValue = (UINTN) SMM_TPM2_CTRL_AREA_START_CALL;
    Status = SwDispatch->Register (SwDispatch, ControlAreaHandlerCallback, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mTcgNvs->ControlAreaSoftwareSmi = (UINT8) SwContext.SwSmiInputValue;
  }

  //
  // Set TPM2 ACPI table
  //
  Status = PublishTpm2 ();
  ASSERT_EFI_ERROR (Status);

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gH2OTpm20DtpmPublishAcpiTableDoneGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

