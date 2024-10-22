/** @file
  Auto Generation of OemRomSig

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
//#include <direct.h>
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>

#define MAX_STRING_LEN               1000
#define NUM_OF_MULLINS_ROMSIG_FILED  8
#define ROMSIG_HEADER_FILED_NUM         0
#define ROMSIG_IEC_FILED_NUM            1
#define ROMSIG_XHCI_FILED_NUM           3
#define ROMSIG_PSP_DIRECTORY_FIELD_NUM_LEGACY  4        //For prior Program, ML, BP, CZ
#define ROMSIG_PSP_DIRECTORY_FIELD_NUM         5        //For New Program, ST, ZP
#define ROMSIG_BIOS_DIRECTORY_FIELD_NUM        7        //Bios Directory for RV, seperate PSP SPI config

#define ROM_SIG_ADDR  0x20000

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
  GenOemRomSig utillity to generate the OemRomSig

Arguments:
  Argc - Number of command line arguments
  Argv - Array of command line argument strings
  Envp - Array of environmemt variable strings

Returns:
  0 - Normal exit
  1 - Abnormal exit

--*/
{
  CHAR8   OemRomSigFileName[MAX_STRING_LEN];
  UINT32  RomSigData[NUM_OF_MULLINS_ROMSIG_FILED] = {0};
//  UINT8   *RomSigData8;
  UINT32  FlashFvSigBase;
  UINT32  FlashAreaBase;
  FILE    *File;

  if (Argc < 2) {
    printf ("GenOemRomSig OemRomSig FilePath\n");
    return -1;
  }

  sprintf(OemRomSigFileName, Argv[1]);

  File = fopen(OemRomSigFileName, "w");
  if (File == NULL) {
    printf ("\nCreate file failure: Cannot create %s file!\n", OemRomSigFileName);
    return -1;
  }


  
  RomSigData[ROMSIG_HEADER_FILED_NUM] = 0x55AA55AA;

// Enable DualData Read
  RomSigData[ROMSIG_HEADER_FILED_NUM + 0x43] = 0x4; //Normal read mode SpiReadMode
//  RomSigData[ROMSIG_HEADER_FILED_NUM + 0x44] = 0x1; //Speed 33.33MHz


  RomSigData[ROMSIG_IEC_FILED_NUM] = 0;
  RomSigData[ROMSIG_XHCI_FILED_NUM] = PcdGet32(PcdFlashUsbFirmwareBase);
//	  RomSigData[ROMSIG_PSP_DIRECTORY_FIELD_NUM_LEGACY] = PcdGet32(PcdFlashPspFixedBase);
  RomSigData[ROMSIG_PSP_DIRECTORY_FIELD_NUM] = PcdGet32(PcdFlashRVPspDirBase);
  RomSigData[ROMSIG_BIOS_DIRECTORY_FIELD_NUM] = PcdGet32(PcdFlashRVBiosDirBase);

  FlashFvSigBase = PcdGet32 (PcdFlashFvSigBase);
  FlashAreaBase = PcdGet32 (PcdFlashAreaBaseAddress);
  //FlashFvSigBase &= 0xFFFFF;
  //if (FlashFvSigBase != ROM_SIG_ADDR) {

  //Enable SingleData Read
//  RomSigData8 = (UINT8 *)&RomSigData[0];
//  RomSigData8[0x43] = 0x0; // Normal read mode SpiReadMode
//  RomSigData8[0x44] = 0x1; // Speed 33.33MHz
// RomSigData8[0x45] = 0xFF; //
 
  //if (FlashFvSigBase - FlashAreaBase != ROM_SIG_ADDR) {
  //  printf ("\nRomsig address error..%x!!\n", FlashFvSigBase);
  //  return -1;
  //}

  fwrite (RomSigData , sizeof(UINT32), NUM_OF_MULLINS_ROMSIG_FILED, File);

  fclose(File);
  return 0;

}
