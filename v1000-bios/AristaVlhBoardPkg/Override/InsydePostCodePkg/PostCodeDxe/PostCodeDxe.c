/** @file
  PostCodeDxe driver implementation

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

#include <PostCodeDxe.h>

//
// The global variable for indicating if the GOPs have been installed
//
BOOLEAN                      mGopInstalled;

//
// The global variables for GOP interface
//
EFI_GRAPHICS_OUTPUT_PROTOCOL *mGraphicsOutput;

//
// The interface which is provided for PostCodeSmm driver to use GOP
//
VGA_SUPPORT_PROTOCOL mVgaSupport = {
  &mGopInstalled,
  &mGraphicsOutput
};

/**
  To enable Dxe PostCode features

  @param [in]  Event     Event whose notification function is being invoked
  @param [in]  Context   Pointer to the notification function's context
*/
VOID
EFIAPI
EnableFeatureCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
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
  // This driver uses breakpoint 0 to trap debug port
  //
  DebugPort = PcdGet16 (PcdH2OPostCodeDebugPortAddress);
  SetUpDebugRegister (Breakpoint0, IoReadOrWrite, OneByte, DebugPort);
}

/**
  To disable Dxe PostCode features

  @param [in]  Event     Event whose notification function is being invoked
  @param [in]  Context   Pointer to the notification function's context

*/
VOID
EFIAPI
DisableFeatureCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  //
  // Clean the debug registers to disable breakpoint 0
  //
  ClearDebugRegister (Breakpoint0);
}

/**
  To disable Dxe PostCode features and clear the Post Code which was printed on screen

  @param [in]  Event     Event whose notification function is being invoked
  @param [in]  Context   Pointer to the notification function's context

*/
VOID
EFIAPI
PostCodeReadyToBootCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  CHAR8       String[BUFFER_SIZE];
  EFI_STATUS  Status;
  UINTN       CharCount;

  //
  // Clean the debug registers to disable breakpoint 0
  //
  ClearDebugRegister (Breakpoint0);

  //
  // Clean the PostCode which was printed on the screen
  //
  if (FeaturePcdGet (PcdH2OPostCodeScreenSupported)) {
    if (mGraphicsOutput != NULL) {
      //
      // Use Graphic Library to clean the PostCode first
      // GraphicVgaShowString can detect current backgrond color, and use it.
      //
      CharCount = GetPostCodeString (
                    ScreenClearStr,
                    0,
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
      // Use GOP to clean the PostCode
      //
      if (EFI_ERROR (Status)) {
        VgaClearPostCode (mGraphicsOutput);
      }
    } else {
      //
      // Use Graphic Library to clean the PostCode
      //
      CharCount = GetPostCodeString (
                    ScreenClearStr,
                    0,
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
}

/**
  Locate GOP when gEfiGraphicsOutputProtocolGuid is installed
  If it loacates successfully, this driver can start to use GOP to show PostCode on screen

  @param [in]  Event     Event whose notification function is being invoked
  @param [in]  Context   Pointer to the notification function's context

*/
VOID
EFIAPI
VgaReadyCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS               Status;
  UINTN                    GopHandleCount;
  EFI_HANDLE               *GopHandleBuffer;
  UINTN                    GopIndex;
  EFI_DEVICE_PATH_PROTOCOL *DisplayDevicePath;
  CHAR8                    String[BUFFER_SIZE];
  UINTN                    CharCount;

  //
  // To indicate that drivers must stop printing PostCode with Graphic Library
  // They may print PostCode on wrong position during PCI emulation
  // The first GOP will be installed before PCI emulation
  //
  mGopInstalled = TRUE;

  //
  // Get the GOP handles
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &GopHandleCount,
                  &GopHandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }

  //
  // Get the GOP interface
  //
  for (GopIndex = 0; GopIndex < GopHandleCount; GopIndex++) {
    //
    // Check if the handle is installed device path protocol
    //
    Status = gBS->HandleProtocol (
                    GopHandleBuffer[GopIndex],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &DisplayDevicePath
                    );
    if(EFI_ERROR(Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    GopHandleBuffer[GopIndex],
                    &gEfiGraphicsOutputProtocolGuid,
                    (VOID **) &mGraphicsOutput
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Clean the PostCode which was printed by Graphic Library
    //
    CharCount = GetPostCodeString (
                  ScreenClearStr,
                  0,
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

    gBS->CloseEvent (Event);

    break;
  }

  gBS->FreePool (GopHandleBuffer);
}

/**
 The PostCode feature handler which will be called by PostCodeFeatureHandlerEntry ()

 When PostCode is written to port 80, the processor will enter debug exception
 And the PostCodeFeatureHandlerEntry () will be called

 @param [in]  InterruptType  Exception type.
 @param [in]  SystemContext  System context data.

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
    // When GOP is installed and mGraphicsOutput is NULL, stop printing PostCode with Graphic Library
    //
    if (mGopInstalled) {
      if (mGraphicsOutput != NULL) {
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
          VgaShowPostCode (mGraphicsOutput, PostCode);
        }
      }
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
 Register notify function to check IDT debug exception entry
 Register notify function to disable to trap port 80
 Register notify function to get the GOP
 Install VgaSupportProtocol to support PostCodeSmm driver to use the GOP

 @param [in]           ImageHandle
 @param [in]           SystemTable

 @retval EFI_SUCCESS   Initializes successfully.
 @return Other         Registration failed.

**/
EFI_STATUS
EFIAPI
PostCodeDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  EFI_EVENT  Event;
  VOID       *Registration;
  EFI_GUID   ZeroGuid;
  EFI_GUID   *NotifyGuid;
  UINT16     DebugPort;

  //
  // Initialize the global variables
  //
  mGopInstalled   = FALSE;
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
  // Configure the debug registers to trap port 80
  // This driver uses breakpoint 0 to trap port 80
  //
  DebugPort = PcdGet16 (PcdH2OPostCodeDebugPortAddress);
  SetUpDebugRegister (Breakpoint0, IoReadOrWrite, OneByte, DebugPort);

  //
  // Register Enable Features notify function
  // To enable PostCode features
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  EnableFeatureCallback,
                  NULL,
                  &Event
                  );
  if(EFI_ERROR(Status)) {
    goto Error;
  }
  NotifyGuid = PcdGetPtr (PcdH2OPostCodeTrapDebugPortNotifyList);
  while (!CompareGuid (NotifyGuid, &ZeroGuid)) {
    Status = gBS->RegisterProtocolNotify (
                    NotifyGuid,
                    Event,
                    &Registration
                    );
    if(EFI_ERROR(Status)) {
      goto Error;
    }
    NotifyGuid++;
  }

  //
  // Register Disable Features notify function
  // To disable PostCode features
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  DisableFeatureCallback,
                  NULL,
                  &Event
                  );
  if(EFI_ERROR(Status)) {
    goto Error;
  }
  NotifyGuid = PcdGetPtr (PcdH2OPostCodeNonTrapDebugPortNotifyList);
  while (!CompareGuid (NotifyGuid, &ZeroGuid)) {
    Status = gBS->RegisterProtocolNotify (
                    NotifyGuid,
                    Event,
                    &Registration
                    );
    if(EFI_ERROR(Status)) {
      goto Error;
    }
    NotifyGuid++;
  }

  //
  // Register notify function
  // To disable PostCode feature when ReadyToBoot event is signaled
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             PostCodeReadyToBootCallback,
             NULL,
             &Event
             );
  if(EFI_ERROR(Status)) {
    goto Error;
  }

  //
  // VGA features
  //
  if (FeaturePcdGet (PcdH2OPostCodeScreenSupported)) {
    //
    // Register notify function
    // To locate GOP when gEfiGraphicsOutputProtocolGuid is installed
    //
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    VgaReadyCallback,
                    NULL,
                    &Event
                    );
    if(EFI_ERROR(Status)) {
      goto Error;
    }
    Status = gBS->RegisterProtocolNotify (
                    &gEfiGraphicsOutputProtocolGuid,
                    Event,
                    &Registration
                    );
    if(EFI_ERROR(Status)) {
      goto Error;
    }

    //
    // Support PostCodeSmm driver to use mGopInstalled and mGraphicsOutput
    //
    Status = gBS->InstallProtocolInterface (
                    &ImageHandle,
                    &gH2OPostCodeVgaProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mVgaSupport
                    );
    if (EFI_ERROR (Status)) {
      goto Error;
    }
  }

  return EFI_SUCCESS;

Error:
  ClearDebugRegister (Breakpoint0);
  return Status;
}

