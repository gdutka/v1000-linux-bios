/** @file

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SioConflict.h"

/**
 Check resource conflict of Isa devices

 @param [in]   MyIfrNVData      Setup variable

 @retval TRUE                   Resources of ISA devices conflict.
 @retval FALSE                  Resources of ISA devices don't conflict.

**/
BOOLEAN
CheckSioConflict (
  IN  CHIPSET_CONFIGURATION                   *MyIfrNVData
  )
{ 
  BOOLEAN                               ConflictFlag = FALSE;

  //
  //  Check Resource Conflict:IRQ,IObase,DMA
  //
  if (MyIfrNVData->ComPortB == 2) {
    if (MyIfrNVData->ComPortA == 2) {
      if ((MyIfrNVData->ComPortABaseIo == MyIfrNVData->ComPortBBaseIo) ||
          (MyIfrNVData->ComPortAInterrupt == MyIfrNVData->ComPortBInterrupt)) {
        MyIfrNVData->ComPortA = 1;
        MyIfrNVData->ComPortB = 0;
        ConflictFlag = TRUE;
      }
    }
  }

  //
  //  Check ComPortA Auto & disable setting
  //
  if (MyIfrNVData->ComPortA < 2) {
    if ((MyIfrNVData->ComPortB == 2) && (MyIfrNVData->ComPortBBaseIo == 3)) {
      MyIfrNVData->ComPortABaseIo = 1;
    } else {
      MyIfrNVData->ComPortABaseIo = 3;
    }
    if ((MyIfrNVData->ComPortB == 2) && (MyIfrNVData->ComPortBInterrupt == 4)) {
      MyIfrNVData->ComPortAInterrupt = 3;
    } else {
      MyIfrNVData->ComPortAInterrupt = 4;
    }
  }

  //
  //  Check ComPortB Auto & disable setting
  //
  if (MyIfrNVData->ComPortB < 2) {
    if ((MyIfrNVData->ComPortA == 2) && (MyIfrNVData->ComPortABaseIo == 1)) {
      MyIfrNVData->ComPortBBaseIo = 3;
    } else {
      MyIfrNVData->ComPortBBaseIo = 1;
    }
    if ((MyIfrNVData->ComPortA == 2) && (MyIfrNVData->ComPortAInterrupt == 3)) {
      MyIfrNVData->ComPortBInterrupt = 4;
    } else {
      MyIfrNVData->ComPortBInterrupt = 3;
    }
    if (MyIfrNVData->ComPortB == 0) {
      MyIfrNVData->ComPortBMode = 0;
    }
  }

  return ConflictFlag;
}

/**
 Check resource conflict of Isa devices and PCI solts.

 @param [in]   MyIfrNVData      Setup variable
 @param [in]   PciIrqData       Setup variable of PCI solt
 @param [out]  DeviceKind       Return conflict device.

 @retval TRUE                   Resources of ISA devices conflict.
 @retval FALSE                  Resources of ISA devices don't conflict.

**/
BOOLEAN
CheckSioAndPciSoltConflict (
  IN  CHIPSET_CONFIGURATION                   *MyIfrNVData,
  IN  UINT8                                   *PciIrqData,
  OUT UINT8                                   *DeviceKind
  )
{
  BOOLEAN                               ConflictFlag = FALSE;
  UINT8                                 Index;                              
  
  *DeviceKind = 0;
                                                             
  if (MyIfrNVData->ComPortA > 0) {
    for(Index =0; Index < 3; Index++ ) {
      if ( MyIfrNVData->ComPortAInterrupt == *(PciIrqData + Index)) {
        ConflictFlag = TRUE;
        *DeviceKind = 0;
        break;
      }    
    }     
  } 
  if (MyIfrNVData->ComPortB > 0) {
    for(Index =0; Index < 3; Index++ ) {
      if ( MyIfrNVData->ComPortBInterrupt == *(PciIrqData + Index)) {
        ConflictFlag = TRUE; 
        *DeviceKind = 1;       
        break;
      }    
    } 
  } 

  
  return ConflictFlag;
}

/**
  Check resource conflict of LPT devices and PCI solts.

  @param [in]   MyIfrNVData                Setup variable
  @param [in]   PciIrqData                 Setup variable of PCI solt

  @retval       0                          No device conflict
  @retval       1                          IRQ5 conflict.
  @retval       2                          IRQ7 conflict.
  @retval       3                          IRQ5 and IRQ7 conflict.

**/
UINT8
CheckLptAndPciSoltConflict (
  IN  CHIPSET_CONFIGURATION                   *MyIfrNVData,
  IN  UINT8                                   *PciIrqData
  )
{
  BOOLEAN                               ConflictFlag[2] = {0}; 
  UINT8                                 Index; 

  for(Index = 0; Index < 3; Index++ ) {    
    if (*(PciIrqData + Index) == 5) {
      //
      // IRQ5 Conflict
      //  
      ConflictFlag[0] = TRUE;
    }
    if (*(PciIrqData + Index) == 7) { 
      //
      // IRQ7 Conflict
      //
      ConflictFlag[1] = TRUE; 
    }
  }     

  if (ConflictFlag[0] && ConflictFlag[1]) {
    //
    //  IRQ5 and IRQ7 have conflict.
    //
    return 3;	  	
  } else if (ConflictFlag[0]) {
    //
    //  IRQ5 has conflict and set LPT1 interrupt IRQ7
    //
    if (MyIfrNVData->Lpt1Interrupt == 5) {
      MyIfrNVData->Lpt1Interrupt = 7;
      return 1; 
    } else {
      return 0;
    }       	
  } else if (ConflictFlag[1]) {
    //
    //  IRQ7 has conflict and set LPT1 interrupt IRQ5
    //
    if (MyIfrNVData->Lpt1Interrupt == 7) {
      MyIfrNVData->Lpt1Interrupt = 5;
      return 2; 
    } else {
      return 0;
    }      	
  }
  return 0;	
}

/**
 Check IRQ resorces setting of Isa devices and Pci solt

 @param [in]   MyIfrNVData      Setup variable
 @param [in]   PciIrqData       First PCI solt variable
 @param [out]  DeviceKind       conflict device
                                0: No devcice conflict
                                1: COMPORT A
                                2: COMPORT B
                                3: LPT PORT

 @retval TRUE                   Resources of ISA devices and PCI solt conflict.
 @retval FALSE                  Resources of ISA devices and PCI solt don't conflict.

**/
BOOLEAN
CheckPciSioConflict (
  IN  CHIPSET_CONFIGURATION                   *MyIfrNVData,
  IN  UINT8                                   *PciIrqData,
  OUT UINT8                                   *DeviceKind
  )
{
  BOOLEAN                               ConflictFlag = FALSE;
  UINTN                                 SlotNum;
  
  *DeviceKind = 0;
  //
  //  Check Resource Conflict:IRQ
  //
  for (SlotNum = 0; SlotNum < MAXPCISLOT; SlotNum++) {
   	
    //
    // check COMPORT A status 
    //
    if (MyIfrNVData->ComPortA == 0 && 
        *(PciIrqData + SlotNum) == MyIfrNVData->ComPortAInterrupt) {      
      	break;      
    }
    //
    // check COMPORT B status 
    //
    if (MyIfrNVData->ComPortB == 0 &&
        *(PciIrqData + SlotNum) == MyIfrNVData->ComPortBInterrupt) {     
      	break;      
    }
    //
    // check LPT port status 
    //
    if (MyIfrNVData->Lpt1 == 0 && 
        *(PciIrqData + SlotNum) == MyIfrNVData->Lpt1Interrupt) {      
      	break;      
    }

    if (*(PciIrqData + SlotNum) == MyIfrNVData->ComPortAInterrupt ) {
      //
      //  PCI IRQ and comprot A IRQ have conflict
      //  
      ConflictFlag = TRUE;
      *DeviceKind = 1;
      //
      //  set the PCI solt IRQ Auto
      //
      *(PciIrqData + SlotNum) = 0;
      break;
    }
    if (*(PciIrqData + SlotNum) == MyIfrNVData->ComPortBInterrupt ) {
      //
      //  PCI IRQ and comprot A IRQ have conflict
      //  
      ConflictFlag = TRUE;
      *DeviceKind = 2;
      *(PciIrqData + SlotNum) = 0 ;
      break;
    }
    if (*(PciIrqData + SlotNum) == MyIfrNVData->Lpt1Interrupt ) {
      //
      //  PCI IRQ and comprot A IRQ have conflict
      //  
      ConflictFlag = TRUE;
      *DeviceKind = 3;
      *(PciIrqData + SlotNum) = 0 ;
      break;
    }        
  }   
 
  return ConflictFlag;
}
