/** @file
  Auto Generation of GenFvHash

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseCryptLib.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>

#define MAX_STRING_LEN               1000

#define _GET_FILE_NAME(f) #f
#define GET_FILE_NAME(f) _GET_FILE_NAME(f)
#define PRINTED_GUID_BUFFER_SIZE  37  // including null-termination

//
// Module global variables
//
EFI_FIRMWARE_VOLUME_HEADER  *mFvHeader  = NULL;
UINT32                      mFvLength   = 0;

EFI_STATUS
InitializeFvLib (
  IN VOID                         *Fv,
  IN UINT32                       FvLength
  )
/*++

Routine Description:

  This initializes the FV lib with a pointer to the FV and length.  It does not
  verify the FV in any way.

Arguments:

  Fv            Buffer containing the FV.
  FvLength      Length of the FV
    
Returns:
 
  EFI_SUCCESS             Function Completed successfully.
  EFI_INVALID_PARAMETER   A required parameter was NULL.

--*/
{
  //
  // Verify input arguments
  //
  if (Fv == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  mFvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) Fv;
  mFvLength = FvLength;

  return EFI_SUCCESS;
}

EFI_STATUS
GetFvHeader (
  OUT EFI_FIRMWARE_VOLUME_HEADER  **FvHeader,
  OUT UINT32                      *FvLength
  )
/*++

Routine Description:

  This function returns a pointer to the current FV and the size.

Arguments:

  FvHeader      Pointer to the FV buffer.
  FvLength      Length of the FV
    
Returns:
 
  EFI_SUCCESS             Function Completed successfully.
  EFI_INVALID_PARAMETER   A required parameter was NULL.
  EFI_ABORTED             The library needs to be initialized.

--*/
{
  //
  // Verify library has been initialized.
  //
  if (mFvHeader == NULL || mFvLength == 0) {
    return EFI_ABORTED;
  }
  //
  // Verify input arguments
  //
  if (FvHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *FvHeader = mFvHeader;
  *FvLength = mFvLength;
  return EFI_SUCCESS;
}

EFI_STATUS
VerifyFv (
  IN EFI_FIRMWARE_VOLUME_HEADER   *FvHeader
  )
/*++

Routine Description:

  Verify the current pointer points to a valid FV header.

Arguments:

  FvHeader     Pointer to an alleged FV file.

Returns:

  EFI_SUCCESS             The FV header is valid.
  EFI_VOLUME_CORRUPTED    The FV header is not valid.
  EFI_INVALID_PARAMETER   A required parameter was NULL.
  EFI_ABORTED             Operation aborted.

--*/
{
  UINT16  Checksum;

  //
  // Verify input parameters
  //
  if (FvHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (FvHeader->Signature != EFI_FVH_SIGNATURE) {
    printf ("invalid FV header signature\n");
    return EFI_VOLUME_CORRUPTED;
  }
  //
  // Verify header checksum
  //
  Checksum = CalculateSum16 ((UINT16 *) FvHeader, FvHeader->HeaderLength);

  if (Checksum != 0) {
    printf ("invalid FV header checksum %04X.\n", Checksum);
    return EFI_ABORTED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetErasePolarity (
  OUT BOOLEAN   *ErasePolarity
  )
/*++

Routine Description:

  This function returns with the FV erase polarity.  If the erase polarity
  for a bit is 1, the function return TRUE.

Arguments:

  ErasePolarity   A pointer to the erase polarity.

Returns:

  EFI_SUCCESS              The function completed successfully.
  EFI_INVALID_PARAMETER    One of the input parameters was invalid.
  EFI_ABORTED              Operation aborted.
  
--*/
{
  EFI_STATUS  Status;

  //
  // Verify library has been initialized.
  //
  if (mFvHeader == NULL || mFvLength == 0) {
    return EFI_ABORTED;
  }
  //
  // Verify FV header
  //
  Status = VerifyFv (mFvHeader);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }
  //
  // Verify input parameters.
  //
  if (ErasePolarity == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mFvHeader->Attributes & EFI_FVB2_ERASE_POLARITY) {
    *ErasePolarity = TRUE;
  } else {
    *ErasePolarity = FALSE;
  }

  return EFI_SUCCESS;
}

UINT32
GetLength (
  UINT8     *ThreeByteLength
  )
/*++

Routine Description:

  Converts a three byte length value into a UINT32.

Arguments:

  ThreeByteLength   Pointer to the first of the 3 byte length.

Returns:

  UINT32      Size of the section

--*/
{
  UINT32  Length;

  if (ThreeByteLength == NULL) {
    return 0;
  }

  Length  = *((UINT32 *) ThreeByteLength);
  Length  = Length & 0x00FFFFFF;

  return Length;
}

UINT32
GetFfsFileLength (
  EFI_FFS_FILE_HEADER *FfsHeader
  )
/*++

Routine Description:

  Get FFS file length including FFS header.

Arguments:

  FfsHeader   Pointer to EFI_FFS_FILE_HEADER.

Returns:

  UINT32      Length of FFS file header.

--*/
{
  if (FfsHeader == NULL) {
    return 0;
  }
  if (FfsHeader->Attributes & FFS_ATTRIB_LARGE_FILE) {
    return ((EFI_FFS_FILE_HEADER2 *)FfsHeader)->ExtendedSize;
  } else {
    return GetLength(FfsHeader->Size);
  }
}

UINT32
GetFfsHeaderLength(
   IN EFI_FFS_FILE_HEADER *FfsHeader
   )
{
  if (FfsHeader == NULL) {
    return 0;
  }
  if (FfsHeader->Attributes & FFS_ATTRIB_LARGE_FILE) {
    return sizeof(EFI_FFS_FILE_HEADER2);
  }
  return sizeof(EFI_FFS_FILE_HEADER);
}

EFI_STATUS
PrintGuidToBuffer (
  IN EFI_GUID     *Guid,
  IN OUT UINT8    *Buffer,
  IN UINT32       BufferLen,
  IN BOOLEAN      Uppercase
  )
/*++

Routine Description:

  This function prints a GUID to a buffer

Arguments:

  Guid      - Pointer to a GUID to print.
  Buffer    - Pointer to a user-provided buffer to print to
  BufferLen - Size of the Buffer
  Uppercase - If use upper case.

Returns:

  EFI_SUCCESS             The GUID was printed.
  EFI_INVALID_PARAMETER   The input was NULL.
  EFI_BUFFER_TOO_SMALL    The input buffer was not big enough
  
--*/
{
  if (Guid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferLen < PRINTED_GUID_BUFFER_SIZE) {
    return EFI_BUFFER_TOO_SMALL;
  }

  if (Uppercase) {
    sprintf (
      (CHAR8 *)Buffer,
      "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
      (unsigned) Guid->Data1,
      Guid->Data2,
      Guid->Data3,
      Guid->Data4[0],
      Guid->Data4[1],
      Guid->Data4[2],
      Guid->Data4[3],
      Guid->Data4[4],
      Guid->Data4[5],
      Guid->Data4[6],
      Guid->Data4[7]
      );
  } else {
    sprintf (
      (CHAR8 *)Buffer,
      "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
      (unsigned) Guid->Data1,
      Guid->Data2,
      Guid->Data3,
      Guid->Data4[0],
      Guid->Data4[1],
      Guid->Data4[2],
      Guid->Data4[3],
      Guid->Data4[4],
      Guid->Data4[5],
      Guid->Data4[6],
      Guid->Data4[7]
      );
  }

  return EFI_SUCCESS;
}

EFI_STATUS
VerifyFfsFile (
  IN EFI_FFS_FILE_HEADER  *FfsHeader
  )
/*++

Routine Description:

  Verify the current pointer points to a FFS file header.

Arguments:

  FfsHeader     Pointer to an alleged FFS file.

Returns:

  EFI_SUCCESS           The Ffs header is valid.
  EFI_NOT_FOUND         This "file" is the beginning of free space.
  EFI_VOLUME_CORRUPTED  The Ffs header is not valid.
  EFI_ABORTED           The erase polarity is not known.

--*/
{
  BOOLEAN             ErasePolarity;
  EFI_STATUS          Status;
  EFI_FFS_FILE_HEADER2 BlankHeader;
  UINT8               Checksum;
  UINT32              FileLength;
  UINT8               SavedChecksum;
  UINT8               SavedState;
  UINT8               FileGuidString[80];
  UINT32              FfsHeaderSize;

  //
  // Verify library has been initialized.
  //
  if (mFvHeader == NULL || mFvLength == 0) {
    return EFI_ABORTED;
  }
  //
  // Verify FV header
  //
  Status = VerifyFv (mFvHeader);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }
  //
  // Get the erase polarity.
  //
  Status = GetErasePolarity (&ErasePolarity);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }

  FfsHeaderSize = GetFfsHeaderLength(FfsHeader);
  //
  // Check if we have free space
  //
  if (ErasePolarity) {
    memset (&BlankHeader, -1, FfsHeaderSize);
  } else {
    memset (&BlankHeader, 0, FfsHeaderSize);
  }

  if (memcmp (&BlankHeader, FfsHeader, FfsHeaderSize) == 0) {
    return EFI_NOT_FOUND;
  }
  //
  // Convert the GUID to a string so we can at least report which file
  // if we find an error.
  //
  PrintGuidToBuffer (&FfsHeader->Name, FileGuidString, sizeof (FileGuidString), TRUE);
  //
  // Verify file header checksum
  //
  SavedState = FfsHeader->State;
  FfsHeader->State = 0;
  SavedChecksum = FfsHeader->IntegrityCheck.Checksum.File;
  FfsHeader->IntegrityCheck.Checksum.File = 0;
  Checksum = CalculateSum8 ((UINT8 *) FfsHeader, FfsHeaderSize);
  FfsHeader->State = SavedState;
  FfsHeader->IntegrityCheck.Checksum.File = SavedChecksum;
  if (Checksum != 0) {
    printf ("invalid FFS file header checksum, Ffs file with Guid %s", FileGuidString);
    return EFI_ABORTED;
  }
  //
  // Verify file checksum
  //
  if (FfsHeader->Attributes & FFS_ATTRIB_CHECKSUM) {
    //
    // Verify file data checksum
    //
    FileLength          = GetFfsFileLength (FfsHeader);
    Checksum            = CalculateSum8 ((UINT8 *) ((UINT8 *)FfsHeader + FfsHeaderSize), FileLength - FfsHeaderSize);
    Checksum            = Checksum + FfsHeader->IntegrityCheck.Checksum.File;
    if (Checksum != 0) {
      printf ("invalid FFS file checksum, Ffs file with Guid %s", FileGuidString);
      return EFI_ABORTED;
    }
  } else {
    //
    // File does not have a checksum
    // Verify contents are 0xAA as spec'd
    //
    if (FfsHeader->IntegrityCheck.Checksum.File != FFS_FIXED_CHECKSUM) {
      printf ("invalid fixed FFS file header checksum, Ffs file with Guid %s", FileGuidString);
      return EFI_ABORTED;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetNextFile (
  IN EFI_FFS_FILE_HEADER          *CurrentFile,
  OUT EFI_FFS_FILE_HEADER         **NextFile
  )
/*++

Routine Description:

  This function returns the next file.  If the current file is NULL, it returns
  the first file in the FV.  If the function returns EFI_SUCCESS and the file 
  pointer is NULL, then there are no more files in the FV.

Arguments:

  CurrentFile   Pointer to the current file, must be within the current FV.
  NextFile      Pointer to the next file in the FV.
    
Returns:
 
  EFI_SUCCESS             Function completed successfully.
  EFI_INVALID_PARAMETER   A required parameter was NULL or is out of range.
  EFI_ABORTED             The library needs to be initialized.

--*/
{
  EFI_STATUS  Status;

  //
  // Verify library has been initialized.
  //
  if (mFvHeader == NULL || mFvLength == 0) {
    return EFI_ABORTED;
  }
  //
  // Verify input arguments
  //
  if (NextFile == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Verify FV header
  //
  Status = VerifyFv (mFvHeader);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }
  //
  // Get first file
  //
  if (CurrentFile == NULL) {
    CurrentFile = (EFI_FFS_FILE_HEADER *) ((UINTN) mFvHeader + mFvHeader->HeaderLength);

    //
    // Verify file is valid
    //
    Status = VerifyFfsFile (CurrentFile);
    if (EFI_ERROR (Status)) {
      //
      // no files in this FV
      //
      *NextFile = NULL;
      return EFI_SUCCESS;
    } else {
      //
      // Verify file is in this FV.
      //
      if ((UINTN) CurrentFile + GetFfsFileLength(CurrentFile) > (UINTN) mFvHeader + mFvLength) {
        *NextFile = NULL;
        return EFI_SUCCESS;
      }

      *NextFile = CurrentFile;
      return EFI_SUCCESS;
    }
  }
  //
  // Verify current file is in range
  //
  if (((UINTN) CurrentFile < (UINTN) mFvHeader + mFvHeader->HeaderLength) ||
      ((UINTN) CurrentFile + GetFfsFileLength(CurrentFile) > (UINTN) mFvHeader + mFvLength)
     ) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Get next file, compensate for 8 byte alignment if necessary.
  //
  *NextFile = (EFI_FFS_FILE_HEADER *) ((((UINTN) CurrentFile - (UINTN) mFvHeader + GetFfsFileLength(CurrentFile) + 0x07) & (-1 << 3)) + (UINT8 *) mFvHeader);

  //
  // Verify file is in this FV.
  //
  if (((UINTN) *NextFile + GetFfsHeaderLength(*NextFile) >= (UINTN) mFvHeader + mFvLength) ||
      ((UINTN) *NextFile + GetFfsFileLength (*NextFile) > (UINTN) mFvHeader + mFvLength)
     ) {
    *NextFile = NULL;
    return EFI_SUCCESS;
  }
  //
  // Verify file is valid
  //
  Status = VerifyFfsFile (*NextFile);
  if (EFI_ERROR (Status)) {
    //
    // no more files in this FV
    //
    *NextFile = NULL;
    return EFI_SUCCESS;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CalculateSha256Hash (
  IN UINT8                      *Message,
  IN UINTN                      MessageSize,
  OUT UINT8                     *Digest
  )
{
  VOID              *HashCtx;
  UINTN             CtxSize;
  EFI_STATUS        Status;

  SetMem (Digest, SHA256_DIGEST_SIZE, 0);
  CtxSize = Sha256GetContextSize ();
  HashCtx = NULL;
  HashCtx = AllocatePool (CtxSize);
  if (HashCtx == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (!Sha256Init (HashCtx)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha256Update (HashCtx, Message, MessageSize)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha256Final (HashCtx, Digest)) {
    Status = EFI_SECURITY_VIOLATION;
  } else {
    Status = EFI_SUCCESS;
  }

Done:
  FreePool (HashCtx);
  return Status;
}

int
main (
  IN  INTN  Argc,
  IN  CHAR8 **Argv,
  IN  CHAR8 **Envp
  )
/*++

Routine Description:
  GenFvHash utillity to generate the Fv Hash

Arguments:
  Argc - Number of command line arguments
  Argv - Array of command line argument strings
  Envp - Array of environmemt variable strings

Returns:
  0 - Normal exit
  1 - Abnormal exit

--*/
{
  int                        Status = 0;
  EFI_STATUS                 EfiStatus;
  EFI_FFS_FILE_HEADER        *FfsFileHeader;
  EFI_FFS_FILE_HEADER        *CurrentFile;
  UINT32                     FileLength;
  UINT8                      HashValue[SHA256_DIGEST_SIZE];
  CHAR8                      MeatureFileName[1000] = {0};
  CHAR8                      HashFileName[1000] = {0};
  FILE                       *File;
  UINT8                      *Message;
  UINTN                      MessageSize;
  UINTN                      ResultSize;
  UINTN                      Index;
  UINT8                      FileGuidString[80];

  if (Argc < 3) {
    printf ("GenFvHash FvFilePath OutPutHashPath\n");
    return -1;
  }

  //
  // Load FvMainCompact.fv and load data into memory and get file size.
  //
  sprintf(MeatureFileName, Argv[1]);
  File = fopen(MeatureFileName, "rb");
  if (File == NULL) {
    printf ("\nOpen file failure: Cannot open %s file!\n", MeatureFileName);
    return -1;
  }
  printf ("File %s load.\n", MeatureFileName);

  // obtain file size:
  fseek (File , 0 , SEEK_END);
  MessageSize = ftell (File);
  rewind (File);

  printf ("File size = %x.\n", MessageSize);

  Message = (UINT8*) malloc (sizeof(UINT8) * MessageSize);
  if (Message == NULL) {
    printf ("\nmalloc failed.\n", MessageSize);
    Status = -1;
    goto CloseFileExit;
  }

  ResultSize = fread (Message,1,MessageSize,File);
  if (ResultSize != MessageSize) {
    printf ("Result size = %x. Not match file size %x.\n", ResultSize, MessageSize);
    Status = -1;
    goto CloseFileExit;
  }
  fclose(File);

  //
  // Initialize the FV library.
  //
  EfiStatus = InitializeFvLib (Message, MessageSize);
  if (EFI_ERROR (EfiStatus)) {
    printf ("InitializeFv fail!!\n");
    return -1;
  }

  //
  // Look for the last FFS file in this FV.
  //
  CurrentFile   = NULL;
  FfsFileHeader = NULL;
  do {
    EfiStatus = GetNextFile (CurrentFile, &CurrentFile);
    if (!EFI_ERROR (EfiStatus) && CurrentFile != NULL) {
      FfsFileHeader = CurrentFile;
      PrintGuidToBuffer (&FfsFileHeader->Name, FileGuidString, sizeof (FileGuidString), TRUE);
      printf ("Find valid FFS file, Ffs file with Guid %s", FileGuidString);
    }
  } while (CurrentFile != NULL);

  if (FfsFileHeader == NULL) {
    printf ("Can't find valid FFS!!\n");
    return -1;
  }
 
  FileLength = (((UINTN) FfsFileHeader - (UINTN) mFvHeader + GetFfsFileLength(FfsFileHeader) + 0x07) & (-1 << 3));
  printf ("FileLength = %X\n", FileLength);
  EfiStatus = CalculateSha256Hash ((UINT8*)mFvHeader, (UINTN)FileLength, HashValue);
  if (EFI_ERROR (EfiStatus)) {
    printf ("Calculate Sha256 Hash failure!!\n");
    return -1;
  }
  for (Index = 0; Index < 32; Index ++) {
    printf ("%X", HashValue[Index]);
  }
  printf ("\n", HashValue[Index]);

  //
  // Save HashValue into binary file.
  //
  sprintf(HashFileName, Argv[2]);
  File = fopen (HashFileName, "wb");
  if (File == NULL) {
    printf ("\nOpen file failure: Cannot open %s file!\n", HashFileName);
    return -1;
  }
  printf ("File %s Create.\n", HashFileName);

  fwrite (HashValue, sizeof(UINT8), 32, File);
  fclose (File);

CloseFileExit:
  free (Message);
  fclose (File);

  return Status;
}
