/** @file
  Generation of Flash layout information
;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <stdio.h>
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>

#define PRINT_FLASH_LAYOUT_PCD(FdLayoutPcd)                \
          if (PcdGet32(FdLayoutPcd) != 0) {                \
            fprintf(FileIa32, "#define _PCD_VALUE_");                 \
            fprintf(FileX64, "#define _PCD_VALUE_");                 \
            fprintf(FileIa32, #FdLayoutPcd);                          \
            fprintf(FileX64, #FdLayoutPcd);                          \
            fprintf(FileIa32, "  0x%08XU\n", PcdGet32(FdLayoutPcd)); \
            fprintf(FileX64, "  0x%08XU\n", PcdGet32(FdLayoutPcd)); \
          }

/*
  FlashLayout utillity to generate FdLayout.h automatically

  @param[in] Argc               Number of command line arguments
  @param[in] Argv               Array of command line argument strings
  @param[in] Envp               Array of environmemt variable strings


  @retval 0                     Normal exit
  @retval others                Abnormal exit
*/
int
main (
  IN  INTN  Argc,
  IN  CHAR8 **Argv,
  IN  CHAR8 **Envp
  )
{

  FILE *FileIa32 = NULL;
  FILE *FileX64 = NULL;
  CHAR8 Ia32FilePath[256];
  CHAR8 X64FilePath[256];

  AsciiSPrint(Ia32FilePath, 256, "%a/IA32/FdLayout.h", Argv[1]);
  AsciiSPrint(X64FilePath, 256, "%a/X64/FdLayout.h", Argv[1]);
  FileIa32 = fopen(Ia32FilePath, "w");
  if (FileIa32 == NULL) {
    printf("Create file failure!\n");
    return -1;
  }

  FileX64 = fopen(X64FilePath, "w");
  if (FileX64 == NULL) {
    printf("Create file failure!\n");
    fclose(FileIa32);
    return -1;
  }
  PRINT_FLASH_LAYOUT_PCD(PcdFlashAreaBaseAddress);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashAreaSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvEcBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvEcSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvSigBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvSigSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvEc2Base);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvEc2Size);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashUsbFirmwareBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashUsbFirmwareSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvBvmBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvBvmSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashPspNvRamBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashPspNvRamSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashPspFixedBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashPspFixedSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashPspUpdateableBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashPspUpdateableSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvMainBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvMainSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvMainReserveBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvMainReserveSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashUnsignedFvRegionBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashUnsignedFvRegionSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageDmiBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageDmiSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageBvdtBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageBvdtSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageMsdmDataBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageMsdmDataSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageVariableBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageVariableSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFtwWorkingBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFtwWorkingSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFtwSpareBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFtwSpareSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFactoryCopyBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFactoryCopySize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashMctDataStoreBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashMctDataStoreSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashSpdDataStoreBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashSpdDataStoreSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvMainHashBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvMainHashSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvRecoveryBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvRecoverySize);

  fclose(FileIa32);
  fclose(FileX64);

  return 0;

}
