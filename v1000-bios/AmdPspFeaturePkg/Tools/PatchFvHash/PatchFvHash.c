/** @file
  Auto Generation of PatchFvHash

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
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>

#define MAX_STRING_LEN               1000

#define _GET_FILE_NAME(f) #f
#define GET_FILE_NAME(f) _GET_FILE_NAME(f)

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
  int                        offset = 0;
  unsigned char              b;
  CHAR8                      FileName[1000] = {0};
  CHAR8                      HashFileName[1000] = {0};
  FILE                       *File;
  FILE                       *HashFile;

  if (Argc < 3) {
    printf ("PatchFvHash FdFilePath OutPutFdFilePath\n");
    return -1;
  }

  sprintf(FileName, Argv[1]);
  File = fopen(FileName, "rb+");
  if (File == NULL) {
    printf ("\nOpen file failure: Cannot open %s file!\n", FileName);
    return -1;
  }
  printf ("File %s load.\n", FileName);

  sprintf(HashFileName, Argv[2]);
  HashFile = fopen (HashFileName, "rb");
  if (HashFile == NULL) {
    printf ("\nOpen file failure: Cannot open %s file!\n", HashFileName);
    return -1;
  }
  printf ("File %s load.\n", HashFileName);

  offset = PcdGet32(PcdFlashFvMainHashBase)-PcdGet32 (PcdFlashAreaBaseAddress);
  fseek (File ,offset, SEEK_SET);
  printf ("File write to Fv offset %X\n", offset);
  
  while (1) {
   fread(&b,1,1,HashFile);
   if (feof(HashFile)) {
    break;
   }
   fwrite(&b,1,1,File);
  }

  fclose (File);
  fclose (HashFile);

  return 0;
}
