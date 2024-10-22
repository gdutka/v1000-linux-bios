/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <PiDxe.h>
  
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
  
#include <AGESA.h>

/**
   This is the stub function will call the host environment through the binary block
   interface (call-out port) to provide a user hook opportunity

   @param[in]         DataSize The data size of the Non Volatile data to be saved
   @param[in]         *Data    The pointer point to the Non Volatile data to be saved

   @retval            VOID
  */


AGESA_STATUS
AgesaSaveNonVolatileS3Info (
  IN       UINTN                        DataSize,
  IN       VOID                         *Data
  )
{
  return AGESA_SUCCESS;
}

/**
   This is the stub function will call the host environment through the binary block
   interface (call-out port) to provide a user hook opportunity

   @param[in]         DataSize  The data size of the Volatile data to be saved
   @param[in]         *Data     The pointer point to the Volatile data to be saved

   @retval            VOID
  */


AGESA_STATUS
AgesaSaveVolatileS3Info (
  IN       UINTN                        DataSize,
  IN       VOID                         *Data
  )
{
  return AGESA_SUCCESS;
}

