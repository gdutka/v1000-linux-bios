/** @file
  PostCodeSmm driver implementation

;******************************************************************************
;* Copyright (c) 2016 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PostCodeSmm.h>

//
// The global variable for indicating if the GOPs have been installed
//
BOOLEAN                      *mGopInstalled;

//
// The global variables for GOP interface
//
EFI_GRAPHICS_OUTPUT_PROTOCOL **mGraphicsOutput;

//
// To indicate if the debug registers are needed to be initialized
// when the processor enters SMM
//
BOOLEAN                      mEnableFeature;

//
// The interface which is provided for SmmCorePlatformHookLib to
// set up the debug registers
//
SMM_DEBUG_REGISTER_CONFIG_PROTOCOL mSmmDebugRegisterConfig = {
  SmmSetUpDebugRegister
};

/**
  This function initializes the debug registers to trap port 80
  when mEnableFeature is true

*/
VOID
EFIAPI
SmmSetUpDebugRegister (
  VOID
  )
{
  UINT16 DebugPort;

  //
  // When driver disables PostCode feature, the debug registers will not be configured
  //
  if (!mEnableFeature) {
    return;
  }

  //
  // Configure the debug registers to trap debug port
  // This driver uses breakpoint 0 to trap debug port
  //
  DebugPort = PcdGet16 (PcdH2OPostCodeDebugPortAddress);
  SetUpDebugRegister (Breakpoint0, IoReadOrWrite, OneByte, DebugPort);
}

/**
  To enable SMM PostCode features

  @param [in] Protocol    Points to the protocol's unique identifier
  @param [in] Interface   Points to the interface instance
  @param [in] Handle      The handle on which the interface was installed

  @retval EFI_SUCCESS     EnableFeatureCallback runs successfully
 **/
EFI_STATUS
EFIAPI
EnableFeatureCallback (
  IN CONST EFI_GUID   *Protocol,
  IN       VOID       *Interface,
  IN       EFI_HANDLE Handle
  )
{
  UINT16 DebugPort;

  //
  // Register PostCodeFeatureHandlerEntry () to the debug exception entry of IDT
  // for performing PostCode Feature
  //
  RegisterIdtHandler (EXCEPT_X64_DEBUG, (UINTN) PostCodeFeatureHandlerEntry);

  //
  // Configure the debug registers to trap debug port
  // This driver uses breakpoint0 to trap debug port
  //
  DebugPort = PcdGet16 (PcdH2OPostCodeDebugPortAddress);
  SetUpDebugRegister (Breakpoint0, IoReadOrWrite, OneByte, DebugPort);

  //
  // Set mEnableFeature to indicate the debug registers are needed to be initialized
  // when the processor enters SMM
  //
  mEnableFeature = TRUE;

  return EFI_SUCCESS;
}

/**
  To disable SMM PostCode features

  @param [in] Protocol    Points to the protocol's unique identifier
  @param [in] Interface   Points to the interface instance
  @param [in] Handle      The handle on which the interface was installed

  @retval EFI_SUCCESS     DisableFeatureCallback runs successfully
 **/
EFI_STATUS
EFIAPI
DisableFeatureCallback (
  IN CONST EFI_GUID   *Protocol,
  IN       VOID       *Interface,
  IN       EFI_HANDLE Handle
  )
{
  //
  // Clean the debug registers to disable breakpoint 0
  //
  ClearDebugRegister (Breakpoint0);

  //
  // If mEnableFeature is false, the debug registers will not be initialized
  // when the processor enters SMM
  //
  mEnableFeature = FALSE;

  return EFI_SUCCESS;
}

/**
 The PostCode feature handler which will be called by PostCodeFeatureHandlerEntry ()

 When PostCode is written to port 80, the processor will enter debug exception
 And the PostCodeFeatureHandlerEntry () will be called

 @param [in]  InterruptType  Exception type
 @param [in]  SystemContext  System context data

**/
VOID
EFIAPI
PostCodeFeatureHandler (
  IN CONST  EFI_EXCEPTION_TYPE InterruptType,
  IN CONST  EFI_SYSTEM_CONTEXT SystemContext
  )
{
  UINT64      PostCode;
  UINT8       PostCodeLength;
  CHAR8       String[BUFFER_SIZE];
  EFI_STATUS  Status;
  UINTN       CharCount;
  //
  // Get the length of Post Code which was output to debug port
  //
  GetPostCodeLength (SystemContext.SystemContextX64->Rip, &PostCodeLength);
  //
  // Read IO port 0x80
  //
  if (PostCodeLength == 0xFF) {
    //
    // When exits exception, cleans the trap status
    //
    ClearTrapStatus (Breakpoint0);
    return;
  }

  //
  // Get the Post Code from RAX (Because of Out instruction)
  //
  PostCode = SystemContext.SystemContextX64->Rax & ~(POST_CODE_MASK << (PostCodeLength * 8));


  //
  // Send Post Code to serial port
  //
  if (FeaturePcdGet (PcdH2OPostCodeSerialPortSupported)) {
    SerialPortPostCode (PostCode);
  }

  //
  // Print Post Code on screen
  //
  if (FeaturePcdGet (PcdH2OPostCodeScreenSupported)) {
    //
    // When GOP is installed and *mGraphicsOutput is NULL, stop printing PostCode with Graphic Library
    //
	//[-start-240426-IB18160212-modify-]//
    if (*mGopInstalled) {
      if (*mGraphicsOutput != NULL) {
        //
        // Use Graphic Library to print PostCode first
        // GraphicVgaShowString can detect current backgrond color, and use it.
        //
        CharCount = GetPostCodeString (
                      ScreenDisplayStr,
                      PostCode,
                      BUFFER_SIZE,
                      String
                      );
        Status = GraphicVgaShowString (
                   (UINT16) (GRAPHIC_VGA_MAX_COL - CharCount - 2),
                   (UINT16) (GRAPHIC_VGA_MAX_ROW - 2),
                   GRAPHIC_VGA_WHITE,
                   GRAPHIC_VGA_BLACK,
                   (UINT8*)String
                   );
        //
        // Use GOP to print PostCode
        //
        if (EFI_ERROR (Status)) {
          VgaShowPostCode (*mGraphicsOutput, PostCode);
        }
      }
	//[-start-240426-IB18160212-modify-]//
    } else {
      //
      // Use Graphic Library to print PostCode
      //
      CharCount = GetPostCodeString (
                    ScreenDisplayStr,
                    PostCode,
                    BUFFER_SIZE,
                    String
                    );
      GraphicVgaShowString (
        (UINT16) (GRAPHIC_VGA_MAX_COL - CharCount - 2),
        (UINT16) (GRAPHIC_VGA_MAX_ROW - 2),
        GRAPHIC_VGA_WHITE,
        GRAPHIC_VGA_BLACK,
        (UINT8*)String
        );
    }
  }

  //
  // When exits exception, cleans the trap status
  //
  ClearTrapStatus (Breakpoint0);
}

/**
 Initialize the debug exception entry of IDT and debuger registers to trap port 80
 Install gH2OPostCodeSmmDebugRegProtocolGuid protocol
 Register notify function to disable to trap port 80
 Locate VgaSupportProtocol to get the VGA interface from PostCodeDxe driver

 @param [in]           ImageHandle
 @param [in]           SystemTable

 @retval EFI_SUCCESS   Initializes successfully
 @return Other         Registration failed

**/
EFI_STATUS
EFIAPI
PostCodeSmmEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS           Status;
  EFI_HANDLE           Handle;
  VOID                 *Registration;
  VGA_SUPPORT_PROTOCOL *VgaSupport;
  EFI_GUID             ZeroGuid;
  EFI_GUID             *NotifyGuid;
  UINT16               DebugPort;

  //
  // Initialize the global variables
  //
  mGopInstalled   = NULL;
  mGraphicsOutput = NULL;

  //
  // Initialize the ZeroGuid
  //
  ZeroMem (&ZeroGuid, sizeof (EFI_GUID));

  //
  // Register PostCodeFeatureHandlerEntry () to the debug exception entry of IDT
  // for performing PostCode Feature
  //
  RegisterIdtHandler (EXCEPT_X64_DEBUG, (UINTN) PostCodeFeatureHandlerEntry);

  //
  // Configure the debug registers to trap debug port
  // This driver uses breakpoint0 to trap debug port
  //
  DebugPort = PcdGet16 (PcdH2OPostCodeDebugPortAddress);
  SetUpDebugRegister (Breakpoint0, IoReadOrWrite, OneByte, DebugPort);

  //
  // Set mEnableFeature to indicate the debug registers are needed to be initialized
  // when the processor enters SMM
  //
  mEnableFeature = TRUE;

  //
  // Install protocol to provide SmmCorePlatformHookLib to initialize the debug registers
  //
  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gH2OPostCodeSmmDebugRegProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mSmmDebugRegisterConfig
                    );
  if( EFI_ERROR(Status)) {
    goto Error;
  }

  //
  // Register Enable Features notify function
  // To Enable SMM PostCode features
  //
  NotifyGuid = PcdGetPtr (PcdH2OPostCodeTrapDebugPortNotifyList);
  while (!CompareGuid (NotifyGuid, &ZeroGuid)) {
    Status = gSmst->SmmRegisterProtocolNotify (
                      NotifyGuid,
                      EnableFeatureCallback,
                      &Registration
                      );
    if(EFI_ERROR(Status)) {
      goto Error;
    }
    NotifyGuid++;
  }

  //
  // Register Disable Features notify function
  // To disable SMM PostCode features
  //
  NotifyGuid = PcdGetPtr (PcdH2OPostCodeNonTrapDebugPortNotifyList);
  while (!CompareGuid (NotifyGuid, &ZeroGuid)) {
    Status = gSmst->SmmRegisterProtocolNotify (
                      NotifyGuid,
                      DisableFeatureCallback,
                      &Registration
                      );
    if(EFI_ERROR(Status)) {
      goto Error;
    }
    NotifyGuid++;
  }

//[-start-240426-IB18160212-modify-]//
  //
  // Get the VGA interface from PostCodeDxe driver
  //
  if (FeaturePcdGet (PcdH2OPostCodeScreenSupported)) {
    Status = gBS->LocateProtocol (
                    &gH2OPostCodeVgaProtocolGuid,
                    NULL,
                    (VOID**)&VgaSupport
                    );
    if (EFI_ERROR (Status)) {
      goto Error;
    }
//[-end-240426-IB18160212-modify-]//

    mGopInstalled   = VgaSupport->GopInstalled;
    mGraphicsOutput = VgaSupport->GraphicsOutput;
  }

  return EFI_SUCCESS;

Error:
  ClearDebugRegister (Breakpoint0);
  return Status;
}

