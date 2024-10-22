/** @file

 Implementation of SIO driver for PEI phase.
 This PEIM is to 

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

 Module Name: SioInit.c
*/
#include "SioRegs.h"
#include <PostCode.h>
// #include <FchRegs/FchRegcs.h>

#include <Ppi/SioInitialized.h>
#include <Library/PeiServicesLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>



//
// SioInitPeim
//
EFI_SIO_TABLE mSioTable[] ={
  //
  // Init Com1 for EFI_DEBUG
  //
  {
    R_SIO_GLOBAL_LDN,
    SIO_COM1
  },
  {
    R_SIO_BASE_ADDRESS_LOW,
    0xf8
  },
  {
    R_SIO_BASE_ADDRESS_HIGH,
    0x03
  },
  {
    R_SIO_PRIMARY_INTERRUPT_SELECT,
    0x04
  },
  {
    R_SIO_ACTIVATE,
    0x01
  }
};

//
// SioInit2
//
EFI_SIO_TABLE mSioTable2[] = {

  //
  // Early Init KB Controller
  //
  {
    R_SIO_GLOBAL_LDN,
    SIO_KBD
  },
  {
    R_SIO_PRIMARY_INTERRUPT_SELECT,
    0x01
  },
  {
    R_SIO_SECOND_INTERRUPT_SELECT,
    0x0C
  },
  {
    R_SIO_ACTIVATE,
    0x01
  },
  //
  // Early Init EC
  //
  {
    R_SIO_GLOBAL_LDN,
    SIO_EC0
  },
  {
    R_SIO_BASE_ADDRESS_HIGH,
    0x00
  },
  {
    R_SIO_BASE_ADDRESS_LOW,
    0x62
  },
  {
    R_SIO_ACTIVATE,
    0x01
  },
};

static EFI_PEI_PPI_DESCRIPTOR mSioInitializedPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiSioInitializedPpiGuid,
  NULL
};

/**

   INIT the SIO. Ported this code and I don't undertand the comments either.

   @param   FfsHeader             FV this PEIM was loaded from.
   @param   PeiServices           General purpose services available to every PEIM.
   
   @retval  None
 
**/
EFI_STATUS
EFIAPI
SioInitPeim (
  IN       EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  UINT16          ConfigPort;
  UINT16          IndexPort;
  UINT16          DataPort;
  UINT8           Index;
  UINT8           Value8;
  EFI_STATUS      Status;

  Value8 = 0;
  Index = 0;
  
  //
  // hard code for sio init
  //
  ConfigPort = CONFIG_PORT0;
  DataPort   = DATA_PORT0;
  IndexPort  = INDEX_PORT0;

  POST_CODE (PEI_SIO_INIT); 
  //
  // Enable 2E / 2F Port, Wide Generic I/O Range
  //
  IoWrite32 (
    0xcf8,
    PCI_LIB_ADDRESS(
      0,
      20,
      3,
      0x48
    )
  );
  Value8 = IoRead8 (0xcfc);
  Value8 |= BIT0;
  
  IoWrite8 (0xcfc, Value8);
  
  //
  // Enable LPT & COM1 I/O Decode
  // 
  IoWrite32 (
    0xcf8,
    PCI_LIB_ADDRESS (
      0,
      20,
      3,
      0x44
    )
  );
  
  //
  // Enable FDC, KB, EC I/O Decode
  //
  // Because CF8 Already point to REG 44h, so Access 47h by 0xcff
  Value8 = IoRead8 (0xcff);
  Value8 |= BIT5 + BIT6;

  IoWrite8 (0xcff, Value8);

  //
  // Enter Config Mode
  //
  IoWrite8 (UNLOCK_PORT, UNLOCK_CODE0);

  //
  // Configure SIO
  //
  SioInit2 (PeiServices);

  //
  // Exit Config Mode
  //
  IoWrite8 (UNLOCK_PORT, LOCK_CODE);

  Status = (*PeiServices)->InstallPpi (PeiServices, &mSioInitializedPpi);

  return Status;
}

EFI_STATUS
SioInit2 (
  IN CONST EFI_PEI_SERVICES          **PeiServices
)
/*++

Routine Description:

  Initialize the SIO IO selection, GPIO USE selection, and GPIO signal inversion registers

--*/
{
  UINT8                           Index;

  //
  // Do early SIO init
  //

  //
  // Common init
  //
  for (Index = 0; Index < sizeof(mSioTable2)/sizeof(EFI_SIO_TABLE); Index++) {
    IoWrite8 (INDEX_PORT0, mSioTable2[Index].Register);
    IoWrite8 (DATA_PORT0, mSioTable2[Index].Value);
  }
  
  return EFI_SUCCESS;
}
