/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access heap.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Porting.h>
#include <AMD.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/GnbHeapLib.h>
#include <Library/MemoryAllocationLib.h>
#include <GnbGuidList.h>
#include <Library/UefiLib.h>
#include <Guid/HobList.h>
#include <Library/HobLib.h>
#include <Library/AmdHeapLib.h>

#define FILECODE LIBRARY_GNBHEAPDXELIB_GNBHEAPDXELIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Allocates space for a new buffer in the heap
 *
 *
 * @param[in]      Handle            Buffer handle
 * @param[in]      Length            Buffer length
 * @param[in]      StdHeader         Standard configuration header
 *
 * @retval         NULL              Buffer allocation fail
 *
 */

VOID *
GnbAllocateHeapBuffer (
  IN      UINT32              Handle,
  IN      UINTN               Length,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS          Status;
  ALLOCATE_HEAP_PARAMS  AllocHeapParams;

  AllocHeapParams.RequestedBufferSize = (UINT32) Length;
  AllocHeapParams.BufferHandle = Handle;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  Status = HeapAllocateBuffer (&AllocHeapParams, StdHeader);
  if (Status != AGESA_SUCCESS) {
    return NULL;
  }
  return AllocHeapParams.BufferPtr;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Allocates space for a new buffer in the heap and clear it
 *
 *
 * @param[in]      Handle            Buffer handle
 * @param[in]      Length            Buffer length
 * @param[in]      StdHeader         Standard configuration header
 *
 * @retval         NULL              Buffer allocation fail
 *
 */

VOID *
GnbAllocateHeapBufferAndClear (
  IN      UINT32              Handle,
  IN      UINTN               Length,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  VOID  *Buffer;
  Buffer = GnbAllocateHeapBuffer (Handle, Length, StdHeader);
  if (Buffer != NULL) {
    LibAmdMemFill (Buffer, 0x00, Length, StdHeader);
  }
  return Buffer;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Locates a previously allocated buffer on the heap.
 *
 *
 * @param[in]      Handle            Buffer handle
 * @param[in]      StdHeader         Standard configuration header
 *
 * @retval         NULL              Buffer handle not found
 *
 */

VOID *
GnbLocateHeapBuffer (
  IN      UINT32              Handle,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS          Status;
  LOCATE_HEAP_PTR       LocHeapParams;
  LocHeapParams.BufferHandle = Handle;
  Status = HeapLocateBuffer (&LocHeapParams, StdHeader);
  if (Status != AGESA_SUCCESS) {
    return NULL;
  }
  return LocHeapParams.BufferPtr;
}

