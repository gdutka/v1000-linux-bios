/** @file
  Auto Generation of GloblNvs.asi

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

#define MAX_STRING_LEN   1000
#define MAX_FGETS_LENGTH  0x200

struct _GLOBAL_NVS_NODE {
  UINT8                   Size;
  CHAR8                   *VariableName;
  CHAR8                   ASLName[5];
  struct _GLOBAL_NVS_NODE  *Next;
};

typedef struct _GLOBAL_NVS_NODE GLOBAL_NVS_NODE;

void
FreeNode (
  GLOBAL_NVS_NODE *HeadNode
  )
{
  GLOBAL_NVS_NODE  *CurrentNode;

  CurrentNode = HeadNode;

  while (CurrentNode != NULL) {
    free (CurrentNode->VariableName);
    CurrentNode = CurrentNode->Next;
  }
}

void
ConvertCharToSize (
  UINT8  *Size,
  CHAR8  *PtrType
  )
{
  *Size = 0;
  if (!strcmp (PtrType, "UINT8")) {
    *Size = 8;
  }

  if (!strcmp (PtrType, "UINT16")) {
    *Size = 16;
  }

  if (!strcmp (PtrType, "UINT32")) {
    *Size = 32;
  }

  if (!strcmp (PtrType, "UINT64")) {
    *Size = 64;
  }
}

int
main (
  IN  INTN  Argc,
  IN  CHAR8 **Argv,
  IN  CHAR8 **Envp
  )
/*++

Routine Description:
  GenGloblNvs utillity to generate the GloblNvs.asi

Arguments:
  Argc - Number of command line arguments
  Argv - Array of command line argument strings
  Envp - Array of environmemt variable strings

Returns:
  0 - Normal exit
  1 - Abnormal exit

--*/
{
  CHAR8   GlobalNvsAreaFileName[MAX_STRING_LEN];
  CHAR8   GloblNvsAsiFileName[MAX_STRING_LEN];

  CHAR8   *DataPtr;

  CHAR8   *PtrType;
  CHAR8   *PtrName;
  CHAR8   *PtrAslName;

  FILE    *File;

  UINT32  Index;
  GLOBAL_NVS_NODE  *HeadNode = NULL;
  GLOBAL_NVS_NODE  *CurrentNode = NULL;

  GLOBAL_NVS_NODE  *Node;

  if (Argc < 3) {
    printf ("GenGloblNvs.exe <GlobalNvsArea.h path> <GloblNvs.asi path> lost\n");
    return -1;  
  }

  sprintf(GlobalNvsAreaFileName, Argv[1]);
  sprintf(GloblNvsAsiFileName, Argv[2]);

  File = fopen(GlobalNvsAreaFileName, "r");
  if (File == NULL) {
    printf ("Read file failure: Cannot read %s file!\n", GlobalNvsAreaFileName);
    return -1;
  }

  DataPtr = malloc (sizeof(CHAR8) * MAX_FGETS_LENGTH);

  PtrType = malloc (sizeof(CHAR8) * 0x10);
  PtrName = malloc (sizeof(CHAR8) * 0x30);
  PtrAslName = malloc (sizeof(CHAR8) * 0x10);

  do {
    fgets (DataPtr, MAX_FGETS_LENGTH, File);
    printf ("%s", DataPtr);

    if (!strcmp (DataPtr,"//  GenGloblNvs.exe signature\n")){
      printf ("Find signature.\n");
      do {
        fgets (DataPtr, MAX_FGETS_LENGTH, File);

        if (!strcmp (DataPtr, "} EFI_GLOBAL_NVS_AREA;\n")) {
          printf ("Structure End.\n");
          break;
        }
         
        sscanf (DataPtr, "%s %[^;]; // %s", PtrType, PtrName, PtrAslName);
        Node = malloc (sizeof(GLOBAL_NVS_NODE));
        ConvertCharToSize (&Node->Size, PtrType);
        Node->VariableName = malloc (strlen(PtrName) + 1);
        strcpy (Node->VariableName, PtrName);
        memcpy (Node->ASLName, PtrAslName, 4);
        Node->ASLName[4] = 0;
        //printf ("%d, %s, %s\n", Node->Size, Node->VariableName, Node->ASLName);

        if (HeadNode == NULL) {
          HeadNode = Node;
          CurrentNode = HeadNode;
        } else {
          CurrentNode->Next = Node;
          CurrentNode = Node;
          Node->Next = NULL;
        }
        
      } while (!feof (File));
    }
  } while (!feof (File));

  fclose(File);

  CurrentNode = HeadNode;
  while (CurrentNode != NULL) {
    printf ("%d, %s, %s\n", CurrentNode->Size, CurrentNode->VariableName, CurrentNode->ASLName);
    CurrentNode = CurrentNode->Next;
  }

  File = fopen(GloblNvsAsiFileName, "w");
  if (File == NULL) {
    printf ("Open file failure: Cannot open %s file!\n", GloblNvsAsiFileName);
    return -1;
  }

  fprintf(File, "/** @file\n");
  fprintf(File, "  This file is Auto-generated by GenGloblNvs tool.\n");
  fprintf(File, "  Information passed from BIOS\n");
  fprintf(File, ";*******************************************************************************\n");
  fprintf(File, ";* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.\n");
  fprintf(File, ";*\n");
  fprintf(File, ";* You may not reproduce, distribute, publish, display, perform, modify, adapt,\n");
  fprintf(File, ";* transmit, broadcast, present, recite, release, license or otherwise exploit\n");
  fprintf(File, ";* any part of this publication in any form, by any means, without the prior\n");
  fprintf(File, ";* written permission of Insyde Software Corporation.\n");
  fprintf(File, ";*\n");
  fprintf(File, ";*******************************************************************************\n");
  fprintf(File, "*/\n");
  fprintf(File, "\n");
  fprintf(File, "#ifndef GLOBAL_NVS_ASI\n");
  fprintf(File, "#define GLOBAL_NVS_ASI\n");
  fprintf(File, "\n");
  fprintf(File, "OperationRegion(GNVS, SystemMemory, 0x55aa55aa, 0x55AA55AA)\n");
  fprintf(File, "  Field(GNVS, AnyAcc, NoLock, Preserve) {\n");
  fprintf(File, "    Offset(0),\n");

  Index = 0;
  CurrentNode = HeadNode;
  while (CurrentNode != NULL) {
    if (CurrentNode->Size < 10) {
      fprintf (File, "    %s, %d,       // (0x%02x) %s\n", CurrentNode->ASLName, CurrentNode->Size, Index, CurrentNode->VariableName);
    } else {
      fprintf (File, "    %s, %d,      // (0x%02x) %s\n", CurrentNode->ASLName, CurrentNode->Size, Index, CurrentNode->VariableName);
    }
    Index += (CurrentNode->Size / 8);
    CurrentNode = CurrentNode->Next;
  }

  fprintf(File, "  }\n");
  fprintf(File, "\n");
  fprintf(File, "#endif\n");
  fprintf(File, "\n");


  fclose(File);

  FreeNode (HeadNode);

  free (DataPtr);
  free (PtrType);
  free (PtrName);
  free (PtrAslName);
  return 0;
}