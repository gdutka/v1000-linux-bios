/** @file
    This driver implements Global Pcd definition block in ACPI table and     
    registers SMI callback functions for ReadPcd

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
DefinitionBlock (
  "Pcd.aml",
  "SSDT",
  0x01,
  "Insyde",
  "PcdTabl",
  0x1000
  )
{
  Scope (\_SB)
  {

    Name (MDSZ,480)  // 512-32 bytes
    
    //
    // Operational region for Smi port access
    //
    OperationRegion (PDSP, SystemIO, FixedPcdGet16 (PcdSoftwareSmiPort), 1)
    Field (PDSP, ByteAcc, NoLock, Preserve)
    {
        IOB2, 8
    }
    
    //
    // Operational region for ReadPCD support
    // Region Offset 0xFFFF0000 and Length 0xF0 will be fixed in C code.
    //
    OperationRegion (GPCD, SystemMemory, 0xFFFF0000, 0x55AA)
    Field (GPCD, AnyAcc, NoLock, Preserve)
    {
      DTSZ,   32,  // Unsigned integer that specifies the size of the PCD data, in bytes.
      TKNO,   32,  // Unsigned integer that, along with PcdGuid, uniquely identifies the PCD in the PCD database.   
      PTYP,   8,   // Enumerated value that specifies the PCD type.
      I_PS,   8,   // the result of retrieving the PCD
      PACT,   8,   // Enumerated value that specifies the PCD action to take,0 - Read a PCD from the PCD database.
      RSVD,   40,       
      PGUI,   128, // GUID that, along with the PcdTokenNumber, uniquely identifies a PCD in the PCD database. 
      Offset(32), 
      DBUF,  3840  // the buffer that holds the returned PCD data. The size of this buffer is specified by PcdDataSize. 
    }
    
    //
    // Read PCD
    //
    // Arguments: (2)
    //    Arg0 - Buffer that specifies the PCD GUID. Must be exactly 16 bytes in length.
    //    Arg1 - Unsigned integer that specifies the PCD token number.    
    //    Return Values
    //           Buffer that contains the result of the PCD.  
    //           If the PCD does not exist or there was some sort of error, then the buffer will be empty.
    //           If the type in Arg2 was EFI_PCD_TYPE_PTR, then the return type is Buffer. 
    //           If the type in Arg2 was EFI_PCD_TYPE_BOOL, then the return type is an unsigned integer. 
    //           If the type in Arg2 was any other type, then the return type is an unsigned integer. 
    //

    Method(PCDE, 2, Serialized)    
    {        
        Store (Arg0, PGUI)
        Store (Arg1, TKNO)
        Store (0x00,Local0)
        
        //
        // Triggle the SMI interrupt
        //
        Store (0x17, IOB2)
        If(LEqual(I_PS,0)) {     // H2O_PCD_ACPI_STATUS_SUCCESS   
            If (LLessEqual (DTSZ,MDSZ)) {            
                Mid (
                    DBUF,           // TermArg => <Buffer | String>
                    0,              // TermArg => Integer
                    DTSZ,           // TermArg => Integer
                    Local0          // Target
                    )
           }
        }
        Return (Local0)
    }

    Method(HBID)    // H2OBoardId method
    {
        Store(\_SB.PCDE(gInsydeTokenSpaceGuid,PcdH2OBoardId), Local0)
        Return (Local0)
    } 

    Method(SPBD, 1, Serialized)    // Supported board method
    {
        Name (POWR, 0)
        Name (IDEX, 0)

        //
        // Arg0 is BoardId.
        //
        If(LEqual(Arg0,0)) {   
          Return (1)
        }        

        Store (\_SB.PCDE(gInsydeTokenSpaceGuid,PcdH2OBoardIds), Local0)

        //
        // Find the board is supported or not.
        //
        Divide (Arg0, 8, IDEX, POWR)  // Divide (Dividend, Divisor, Remainder, Result)      
        Store (DeRefOf (Index (Local0, POWR)), Local1)          
        ShiftLeft (1, IDEX, Local2)   // ShiftLeft (Source, ShiftCount, Result)
        And (Local1, Local2, Local3)  // And (Source1, Source2, Result)

        If(LEqual(Local3,0)) {   
          Return (0)
        }
        Else
        {
          Return (1)
        }
    }    
  }
}

