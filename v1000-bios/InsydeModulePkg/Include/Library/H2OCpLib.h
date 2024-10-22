/** @file
  H2O Check point Library Defintion.
;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_CHECK_POINT_LIB_H_
#define _H2O_CHECK_POINT_LIB_H_

#include <Uefi.h>

typedef VOID *H2O_CP_HANDLE;

typedef enum {
  H2O_CP_LOW         = 6,
  H2O_CP_MEDIUM_LOW  = TPL_CALLBACK - 1,
  H2O_CP_MEDIUM      = TPL_CALLBACK,
  H2O_CP_MEDIUM_HIGH = TPL_CALLBACK + 1,
  H2O_CP_HIGH        = TPL_NOTIFY
} H2O_CP_PRIORITY;


typedef
VOID
(EFIAPI *H2O_CP_HANDLER) (
  IN EFI_EVENT         Event,
  IN H2O_CP_HANDLE     Handle
  );

#ifndef MDEPKG_NDEBUG
#define H2OCpRegisterHandler(a, b, c, d) H2OCpRegister(a, b, c, d, #b, __FUNCTION__, __FILE__)
#else
#define H2OCpRegisterHandler(a, b, c, d) H2OCpRegister(a, b, c, d, NULL, NULL, NULL)
#endif

/**
  This function registers a handler for the specified checkpoint with the specified priority.

  @param[in]  H2OCheckpoint     Pointer to a GUID that specifies the checkpoint for which the
                                handler is being registered.
  @param[in]  Handler           Pointer to the handler function.
  @param[in]  Priority          Enumerated value that specifies the priority with which the function
                                will be associated.
  @param[out] Handle            Pointer to the returned handle that is associated with the newly
                                registered checkpoint handler.
  @param[in]  HandlerName       Ascii string to handler function name.
  @param[in]  FunctionName      Ascii string to function name which we register handler function.
  @param[in]  FileName          Ascii string to file name which we register handler function.

  @retval EFI_SUCCESS           Register check point handle successfully.
  @retval EFI_INVALID_PARAMETER H2OCheckpoint ,Handler or Handle is NULL.
  @retval EFI_OUT_OF_RESOURCES  Allocate memory for Handle failed.
**/
EFI_STATUS
H2OCpRegister (
  IN  CONST EFI_GUID      *H2OCheckpoint,
  IN  H2O_CP_HANDLER      Handler,
  IN  H2O_CP_PRIORITY     Priority,
  OUT H2O_CP_HANDLE       *Handle,
  IN  CONST CHAR8         *HandlerName,
  IN  CONST CHAR8         *FunctionName,
  IN  CONST CHAR8         *FileName
  );

/**
  This function returns the checkpoint data structure that was installed when the checkpoint was
  triggered and, optionally, the GUID that was associated with the checkpoint.

  @param[in]  Handle            The handle associated with a previously registered checkpoint
                                handler.
  @param[out] H2OCheckpointData The pointer to the checkpoint structure that was installed.
  @param[out] H2OCheckpoint     Optional pointer to the returned pointer to the checkpoint GUID.

  @retval EFI_SUCCESS           Get check point data successfully.
  @retval EFI_INVALID_PARAMETER Handle or H2OCheckpointData is NULL or Handle is invalid.
  @retval EFI_INVALID_PARAMETER It does not refer to a previously registered checkpoint handler.
  @return Others                Other error occurred while getting check point information.
**/
EFI_STATUS
H2OCpLookup(
  IN  H2O_CP_HANDLE     Handle,
  OUT VOID              **H2OCheckpointData,
  OUT EFI_GUID          *H2OCheckpoint       OPTIONAL
  );

/**
  This function install checks point data to check point GUID and triggers check point
  according to check point GUID.

  @param[in] H2OCheckpoint      Pointer to the GUID associated with the H2O checkpoint.
  @param[in] H2OCheckpointData  Pointer to the data associated with the H2O checkpoint.


  @retval EFI_SUCCESS           Trigger check point successfully.
  @retval EFI_INVALID_PARAMETER H2OCheckpoint or H2OCheckpointData is NULL.
  @retval Other                 Install H2OCheckpoint protocol failed.
**/
EFI_STATUS
H2OCpTrigger (
  IN CONST EFI_GUID *H2OCheckpoint,
  IN CONST VOID     *H2OCheckpointData
  );

/**
  This function unregisters the handle and frees any associated resources.

  @param[in] Handle             The handle that is associated with the registered checkpoint handler.

  @retval EFI_SUCCESS           The function completed successfully.
  @return EFI_INVALID_PARAMETER Handle is NULL, Handle is invalid or does not refer to a previously
                                registered checkpoint handler.
**/
EFI_STATUS
H2OCpUnregisterHandler (
  IN H2O_CP_HANDLE Handle
  );

#endif

