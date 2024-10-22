/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

DefinitionBlock (
  "Tpm.aml",   // Output Filename
  "SSDT",      // Signature
  0x01,        // DSDT Compliance Revision
  "AMD",       // OEMID
  "TPMACPI",   // TABLE ID
  0x1000       // OEM Revision
  )
{
  //The TPM 2.0 Device Object MUST be located under the system bus at “root\_SB”.
  Scope (\_SB)
  {
    //Debug purpose
    OperationRegion (DBG1, SystemIO, 0x80, 0x2)
    Field (DBG1, WordAcc, NoLock, Preserve)
    {
        P80H, 16
    }
    Device (TPM2)
    {
      //The actual plug and play hardware identifier (e.g. _HID) for the TPM 2.0
      // device object MUST be MSFT0101 or the device MUST have a compatible ID of 
      //MSFT0101 and the _CID could be vendor specific.
      Name (_HID, "MSFT0101")
      Name (_CID, "MSFT0101")
      Name (_STR, Unicode ("TPM 2.0 Device"))

      Method (_STA, 0)
      {
        Return (0x0F)
      }

      Name (_CRS, ResourceTemplate () {
        //TPM2.0 Command Buffer allocate by BIOS, should be updated during POST
        //Memory32Fixed (ReadWrite, 0xBBBBBBBB, 0x100000)
        Memory32Fixed (ReadWrite, 0xBBBBBBBB, 0x100000)
        //TPM2.0 Response Buffer allocate by BIOS, should be updated during POST
        //Memory32Fixed (ReadWrite, 0xCCCCCCCC, 0x100000)
        Memory32Fixed (ReadWrite, 0xCCCCCCCC, 0x100000)
      })

      //TPM2 ACPI Start Method
      Method (TPMS, 3, Serialized, 0, {BuffObj, PkgObj, IntObj, StrObj}, {UnknownObj, UnknownObj, UnknownObj})
      {
        //
        // Function 0: Return supported functions, based on revision
        //
        If (LEqual (Arg1, 0)) {
          Store (0xA2, P80H)
          Return (Buffer () {0x03})
        }
        //
        // Function 1: TPM Start
        //
        If (LEqual (Arg1, 1)) {
          //Do Nothing currently
          Store (0xA3, P80H)
          Return (0)
        }
        //No support Function ID
        Return (1)
      }
      //
      // _DSM – Device Specific Method
      //
      // Arg0:  UUID    Unique function identifier
      // Arg1:   Integer   Revision Level
      // Arg2:   Integer   Function Index (0 = Return Supported Functions)
      // Arg3:   Package   Parameters
      Method (_DSM, 4, Serialized, 0, {IntObj,BuffObj},{BuffObj, IntObj, IntObj, PkgObj})
      {
        Store (0xA0, P80H)
        Sleep (2000)
        // Optional ACPI Start Method
        If (LEqual (Arg0, ToUUID("6bbf6cab-5463-4714-b7cd-f0203c0368d4"))) {
          Store (0xA1, P80H)
          Sleep (2000)
          Return (TPMS (Arg1, Arg2, Arg3))
        }
        //Platform specific implementation >>

        //clear the MOR bit, refer to "TCG Platform Reset Attack Mitigation Specification " for details
        // If (LEqual (Arg0, ToUUID("376054ED-CC13-4675-901C-4756D7F2D45D"))) {
        // }

        //Physical Presence Interface, refer to "TCG Physical Presence Interface Specification" for details
        // If (LEqual (Arg0, ToUUID("376054ED-CC13-4675-901C-4756D7F2D45D"))) {
        // }

        //Default return
        return (Buffer () {0})
      }
    }
  }
}
