/** @file
  Source Code Analysis for BaseMemoryLib.h

;******************************************************************************
;* Copyright (c) 2019 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __SCA_BASE_MEMORY_LIB__
#define __SCA_BASE_MEMORY_LIB__

/**
  Copies a source buffer to a destination buffer, and returns the destination buffer.

  This function copies Length bytes from SourceBuffer to DestinationBuffer, and returns
  DestinationBuffer.  The implementation must be reentrant, and it must handle the case
  where SourceBuffer overlaps DestinationBuffer.

  If Length is greater than (MAX_ADDRESS - DestinationBuffer + 1), then ASSERT().
  If Length is greater than (MAX_ADDRESS - SourceBuffer + 1), then ASSERT().

  @param  DestinationBuffer   The pointer to the destination buffer of the memory copy.
  @param  SourceBuffer        The pointer to the source buffer of the memory copy.
  @param  Length              The number of bytes to copy from SourceBuffer to DestinationBuffer.

  @return DestinationBuffer.

**/
_Post_equal_to_(DestinationBuffer)
VOID *
EFIAPI
CopyMem (
  _Out_writes_bytes_(Length)     OUT VOID       *DestinationBuffer,
  _In_reads_bytes_(Length)       IN CONST VOID  *SourceBuffer,
  _In_                           IN UINTN       Length
  );

/**
  Fills a target buffer with a byte value, and returns the target buffer.

  This function fills Length bytes of Buffer with Value, and returns Buffer.

  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  @param  Buffer    The memory to set.
  @param  Length    The number of bytes to set.
  @param  Value     The value with which to fill Length bytes of Buffer.

  @return Buffer.

**/
_Post_equal_to_(Buffer)
_At_buffer_(
    (UINT8 *)Buffer,
    _Iter_,
    Length / sizeof (UINT8),
    _Post_satisfies_(((UINT8 *)Buffer)[_Iter_] == Value)
)
VOID *
EFIAPI
SetMem (
  _Out_writes_bytes_(Length)     OUT VOID  *Buffer,
  _In_                           IN UINTN  Length,
  _In_                           IN UINT8  Value
  );

/**
  Fills a target buffer with a 16-bit value, and returns the target buffer.

  This function fills Length bytes of Buffer with the 16-bit value specified by
  Value, and returns Buffer. Value is repeated every 16-bits in for Length
  bytes of Buffer.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().
  If Buffer is not aligned on a 16-bit boundary, then ASSERT().
  If Length is not aligned on a 16-bit boundary, then ASSERT().

  @param  Buffer  The pointer to the target buffer to fill.
  @param  Length  The number of bytes in Buffer to fill.
  @param  Value   The value with which to fill Length bytes of Buffer.

  @return Buffer.

**/
_Post_equal_to_(Buffer)
_At_buffer_(
    (UINT16 *)Buffer,
    _Iter_,
    Length / sizeof (UINT16),
    _Post_satisfies_(((UINT16 *)Buffer)[_Iter_] == Value)
)
VOID *
EFIAPI
SetMem16 (
  _Out_writes_bytes_(Length)     OUT VOID   *Buffer,
  _In_                           IN UINTN   Length,
  _In_                           IN UINT16  Value
  );

/**
  Fills a target buffer with a 32-bit value, and returns the target buffer.

  This function fills Length bytes of Buffer with the 32-bit value specified by
  Value, and returns Buffer. Value is repeated every 32-bits in for Length
  bytes of Buffer.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().
  If Buffer is not aligned on a 32-bit boundary, then ASSERT().
  If Length is not aligned on a 32-bit boundary, then ASSERT().

  @param  Buffer  The pointer to the target buffer to fill.
  @param  Length  The number of bytes in Buffer to fill.
  @param  Value   The value with which to fill Length bytes of Buffer.

  @return Buffer.

**/
_Post_equal_to_(Buffer)
_At_buffer_(
    (UINT32 *)Buffer,
    _Iter_,
    Length / sizeof (UINT32),
    _Post_satisfies_(((UINT32 *)Buffer)[_Iter_] == Value)
)
VOID *
EFIAPI
SetMem32 (
  _Out_writes_bytes_(Length)     OUT VOID   *Buffer,
  _In_                           IN UINTN   Length,
  _In_                           IN UINT32  Value
  );

/**
  Fills a target buffer with a 64-bit value, and returns the target buffer.

  This function fills Length bytes of Buffer with the 64-bit value specified by
  Value, and returns Buffer. Value is repeated every 64-bits in for Length
  bytes of Buffer.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().
  If Buffer is not aligned on a 64-bit boundary, then ASSERT().
  If Length is not aligned on a 64-bit boundary, then ASSERT().

  @param  Buffer  The pointer to the target buffer to fill.
  @param  Length  The number of bytes in Buffer to fill.
  @param  Value   The value with which to fill Length bytes of Buffer.

  @return Buffer.

**/
_Post_equal_to_(Buffer)
_At_buffer_(
    (UINT64 *)Buffer,
    _Iter_,
    Length / sizeof (UINT64),
    _Post_satisfies_(((UINT64 *)Buffer)[_Iter_] == Value)
)
VOID *
EFIAPI
SetMem64 (
  _Out_writes_bytes_(Length)     OUT VOID   *Buffer,
  _In_                           IN UINTN   Length,
  _In_                           IN UINT64  Value
  );

/**
  Fills a target buffer with a value that is size UINTN, and returns the target buffer.

  This function fills Length bytes of Buffer with the UINTN sized value specified by
  Value, and returns Buffer. Value is repeated every sizeof(UINTN) bytes for Length
  bytes of Buffer.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().
  If Buffer is not aligned on a UINTN boundary, then ASSERT().
  If Length is not aligned on a UINTN boundary, then ASSERT().

  @param  Buffer  The pointer to the target buffer to fill.
  @param  Length  The number of bytes in Buffer to fill.
  @param  Value   The value with which to fill Length bytes of Buffer.

  @return Buffer.

**/
_Post_equal_to_(Buffer)
_At_buffer_(
    (UINTN *)Buffer,
    _Iter_,
    Length / sizeof (UINTN),
    _Post_satisfies_(((UINTN *)Buffer)[_Iter_] == Value)
)
VOID *
EFIAPI
SetMemN (
  _Out_writes_bytes_(Length)     OUT VOID  *Buffer,
  _In_                           IN UINTN  Length,
  _In_                           IN UINTN  Value
  );

/**
  Fills a target buffer with zeros, and returns the target buffer.

  This function fills Length bytes of Buffer with zeros, and returns Buffer.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  @param  Buffer      The pointer to the target buffer to fill with zeros.
  @param  Length      The number of bytes in Buffer to fill with zeros.

  @return Buffer.

**/
 _Post_equal_to_(Buffer)
_At_buffer_((UINT8 *)Buffer, _Iter_, Length, _Post_satisfies_(((UINT8 *)Buffer)[_Iter_] == 0))
VOID *
EFIAPI
ZeroMem (
  _Out_writes_bytes_(Length) OUT VOID  *Buffer,
  _In_                       IN UINTN  Length
  );

/**
  Compares the contents of two buffers.

  This function compares Length bytes of SourceBuffer to Length bytes of DestinationBuffer.
  If all Length bytes of the two buffers are identical, then 0 is returned.  Otherwise, the
  value returned is the first mismatched byte in SourceBuffer subtracted from the first
  mismatched byte in DestinationBuffer.

  If Length > 0 and DestinationBuffer is NULL, then ASSERT().
  If Length > 0 and SourceBuffer is NULL, then ASSERT().
  If Length is greater than (MAX_ADDRESS - DestinationBuffer + 1), then ASSERT().
  If Length is greater than (MAX_ADDRESS - SourceBuffer + 1), then ASSERT().

  @param  DestinationBuffer The pointer to the destination buffer to compare.
  @param  SourceBuffer      The pointer to the source buffer to compare.
  @param  Length            The number of bytes to compare.

  @return 0                 All Length bytes of the two buffers are identical.
  @retval Non-zero          The first mismatched byte in SourceBuffer subtracted from the first
                            mismatched byte in DestinationBuffer.

**/
_Check_return_
INTN
EFIAPI
CompareMem (
  _In_reads_bytes_ (Length) IN CONST VOID  *DestinationBuffer,
  _In_reads_bytes_ (Length) IN CONST VOID  *SourceBuffer,
  _In_                      IN UINTN       Length
  );

/**
  Scans a target buffer for an 8-bit value, and returns a pointer to the matching 8-bit value
  in the target buffer.

  This function searches target the buffer specified by Buffer and Length from the lowest
  address to the highest address for an 8-bit value that matches Value.  If a match is found,
  then a pointer to the matching byte in the target buffer is returned.  If no match is found,
  then NULL is returned.  If Length is 0, then NULL is returned.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  @param  Buffer      The pointer to the target buffer to scan.
  @param  Length      The number of bytes in Buffer to scan.
  @param  Value       The value to search for in the target buffer.

  @return A pointer to the matching byte in the target buffer, otherwise NULL.

**/
_Check_return_
VOID *
EFIAPI
ScanMem8 (
  _In_reads_bytes_ (Length) IN CONST VOID  *Buffer,
  _In_                      IN UINTN       Length,
  _In_                      IN UINT8       Value
  );

/**
  Scans a target buffer for a 16-bit value, and returns a pointer to the matching 16-bit value
  in the target buffer.

  This function searches target the buffer specified by Buffer and Length from the lowest
  address to the highest address for a 16-bit value that matches Value.  If a match is found,
  then a pointer to the matching byte in the target buffer is returned.  If no match is found,
  then NULL is returned.  If Length is 0, then NULL is returned.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Buffer is not aligned on a 16-bit boundary, then ASSERT().
  If Length is not aligned on a 16-bit boundary, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  @param  Buffer      The pointer to the target buffer to scan.
  @param  Length      The number of bytes in Buffer to scan.
  @param  Value       The value to search for in the target buffer.

  @return A pointer to the matching byte in the target buffer, otherwise NULL.

**/
_Check_return_
VOID *
EFIAPI
ScanMem16 (
  _In_reads_bytes_ (Length) IN CONST VOID  *Buffer,
  _In_                      IN UINTN       Length,
  _In_                      IN UINT16      Value
  );

/**
  Scans a target buffer for a 32-bit value, and returns a pointer to the matching 32-bit value
  in the target buffer.

  This function searches target the buffer specified by Buffer and Length from the lowest
  address to the highest address for a 32-bit value that matches Value.  If a match is found,
  then a pointer to the matching byte in the target buffer is returned.  If no match is found,
  then NULL is returned.  If Length is 0, then NULL is returned.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Buffer is not aligned on a 32-bit boundary, then ASSERT().
  If Length is not aligned on a 32-bit boundary, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  @param  Buffer      The pointer to the target buffer to scan.
  @param  Length      The number of bytes in Buffer to scan.
  @param  Value       The value to search for in the target buffer.

  @return A pointer to the matching byte in the target buffer, otherwise NULL.

**/
_Check_return_
VOID *
EFIAPI
ScanMem32 (
  _In_reads_bytes_ (Length) IN CONST VOID  *Buffer,
  _In_                      IN UINTN       Length,
  _In_                      IN UINT32      Value
  );

/**
  Scans a target buffer for a 64-bit value, and returns a pointer to the matching 64-bit value
  in the target buffer.

  This function searches target the buffer specified by Buffer and Length from the lowest
  address to the highest address for a 64-bit value that matches Value.  If a match is found,
  then a pointer to the matching byte in the target buffer is returned.  If no match is found,
  then NULL is returned.  If Length is 0, then NULL is returned.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Buffer is not aligned on a 64-bit boundary, then ASSERT().
  If Length is not aligned on a 64-bit boundary, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  @param  Buffer      The pointer to the target buffer to scan.
  @param  Length      The number of bytes in Buffer to scan.
  @param  Value       The value to search for in the target buffer.

  @return A pointer to the matching byte in the target buffer, otherwise NULL.

**/
_Check_return_
VOID *
EFIAPI
ScanMem64 (
  _In_reads_bytes_ (Length) IN CONST VOID  *Buffer,
  _In_                      IN UINTN       Length,
  _In_                      IN UINT64      Value
  );

/**
  Scans a target buffer for a UINTN sized value, and returns a pointer to the matching
  UINTN sized value in the target buffer.

  This function searches target the buffer specified by Buffer and Length from the lowest
  address to the highest address for a UINTN sized value that matches Value.  If a match is found,
  then a pointer to the matching byte in the target buffer is returned.  If no match is found,
  then NULL is returned.  If Length is 0, then NULL is returned.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Buffer is not aligned on a UINTN boundary, then ASSERT().
  If Length is not aligned on a UINTN boundary, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  @param  Buffer      The pointer to the target buffer to scan.
  @param  Length      The number of bytes in Buffer to scan.
  @param  Value       The value to search for in the target buffer.

  @return A pointer to the matching byte in the target buffer, otherwise NULL.

**/
_Check_return_
VOID *
EFIAPI
ScanMemN (
  _In_reads_bytes_ (Length) IN CONST VOID  *Buffer,
  _In_                      IN UINTN       Length,
  _In_                      IN UINTN       Value
  );


/**
  Copies a source GUID to a destination GUID.

  This function copies the contents of the 128-bit GUID specified by SourceGuid to
  DestinationGuid, and returns DestinationGuid.

  If DestinationGuid is NULL, then ASSERT().
  If SourceGuid is NULL, then ASSERT().

  @param  DestinationGuid   The pointer to the destination GUID.
  @param  SourceGuid        The pointer to the source GUID.

  @return DestinationGuid.

**/
_Post_equal_to_(DestinationGuid)
_At_buffer_(
    (UINT8*)DestinationGuid,
    _Iter_,
    sizeof (GUID),
    _Post_satisfies_(((UINT8*)DestinationGuid)[_Iter_] == ((UINT8*)SourceGuid)[_Iter_])
)
GUID *
EFIAPI
CopyGuid (
  _Out_writes_bytes_(sizeof(GUID))      OUT GUID       *DestinationGuid,
  _In_reads_bytes_(sizeof(GUID))        IN CONST GUID  *SourceGuid
  );

/**
  Compares two GUIDs.

  This function compares Guid1 to Guid2.  If the GUIDs are identical then TRUE is returned.
  If there are any bit differences in the two GUIDs, then FALSE is returned.

  If Guid1 is NULL, then ASSERT().
  If Guid2 is NULL, then ASSERT().

  @param  Guid1       A pointer to a 128 bit GUID.
  @param  Guid2       A pointer to a 128 bit GUID.

  @retval TRUE        Guid1 and Guid2 are identical.
  @retval FALSE       Guid1 and Guid2 are not identical.

**/
_Check_return_
BOOLEAN
EFIAPI
CompareGuid (
  _In_reads_bytes_ (sizeof (GUID)) IN CONST GUID  *Guid1,
  _In_reads_bytes_ (sizeof (GUID)) IN CONST GUID  *Guid2
  );

/**
  Scans a target buffer for a GUID, and returns a pointer to the matching GUID
  in the target buffer.

  This function searches target the buffer specified by Buffer and Length from
  the lowest address to the highest address at 128-bit increments for the 128-bit
  GUID value that matches Guid.  If a match is found, then a pointer to the matching
  GUID in the target buffer is returned.  If no match is found, then NULL is returned.
  If Length is 0, then NULL is returned.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Buffer is not aligned on a 32-bit boundary, then ASSERT().
  If Length is not aligned on a 128-bit boundary, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  @param  Buffer  The pointer to the target buffer to scan.
  @param  Length  The number of bytes in Buffer to scan.
  @param  Guid    The value to search for in the target buffer.

  @return A pointer to the matching Guid in the target buffer, otherwise NULL.

**/
_Check_return_
VOID *
EFIAPI
ScanGuid (
  _In_reads_bytes_ (Length) IN CONST VOID  *Buffer,
  _In_                      IN UINTN       Length,
  _In_                      IN CONST GUID  *Guid
  );

/**
  Checks if the given GUID is a zero GUID.

  This function checks whether the given GUID is a zero GUID. If the GUID is
  identical to a zero GUID then TRUE is returned. Otherwise, FALSE is returned.

  If Guid is NULL, then ASSERT().

  @param  Guid        The pointer to a 128 bit GUID.

  @retval TRUE        Guid is a zero GUID.
  @retval FALSE       Guid is not a zero GUID.

**/
_Check_return_
BOOLEAN
EFIAPI
IsZeroGuid (
  _In_reads_bytes_ (sizeof (GUID)) IN CONST GUID  *Guid
  );

/**
  Checks if the contents of a buffer are all zeros.

  This function checks whether the contents of a buffer are all zeros. If the
  contents are all zeros, return TRUE. Otherwise, return FALSE.

  If Length > 0 and Buffer is NULL, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  @param  Buffer      The pointer to the buffer to be checked.
  @param  Length      The size of the buffer (in bytes) to be checked.

  @retval TRUE        Contents of the buffer are all zeros.
  @retval FALSE       Contents of the buffer are not all zeros.

**/
_Check_return_
BOOLEAN
EFIAPI
IsZeroBuffer (
  _In_reads_bytes_(Length) IN CONST VOID  *Buffer,
  _In_                     IN UINTN       Length
  );

#endif

