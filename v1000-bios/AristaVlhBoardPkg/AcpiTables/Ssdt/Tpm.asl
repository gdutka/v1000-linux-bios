/** @file

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

DefinitionBlock (
  "Tpm.aml",
  "SSDT",
  1,
  "Insyde",
  "TcgTable",
  0x1000
  )
{
  Scope (\_SB)
  {

        Device (TPM)
        {
            Name (_HID, EisaId ("PNP0C31"))  // _HID: Hardware ID
            Name (_CID, EisaId ("PNP0C31"))  // _CID: Compatible ID
            Name (_UID, 1)  // _UID: Unique ID
            Name (_STR, Unicode ("TPM"))  // _STR: Description String
            Method (_STA, 0, NotSerialized)  // _STA: Status
            {
                Return (0x0F)
            }

            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xFED40000,         // Address Base
                    0x00005000,         // Address Length
                    )
                IO (Decode16,
                    0x002E,             // Range Minimum
                    0x002E,             // Range Maximum
                    0x01,               // Alignment
                    0x02,               // Length
                    )
            })

            Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
            {
                ToBuffer (Arg0, Local0)
                If (LEqual (Arg0, ToUUID ("3dddfaa6-361b-4eb4-a424-8d10089d1653")) /* Physical Presence   Interface */)
                {
                    ToInteger (Arg2, Local1)
                    If (LEqual (Arg2, 0))
                    {
                        Return (Buffer (0x02)
                        {
                             0xFF, 0x01                                       /* .. */
                        })
                    }

                    If (LEqual (Arg2, 1))
                    {
                        Return ("1.2")
                    }

                    If (LEqual (Arg2, 2))
                    {
                        Return (0x00)
                    }

                    If (LEqual (Arg2, 3))
                    {
                        Return (Package (0x02)
                        {
                            0x00,
                            0x00
                        })
                    }

                    If (LEqual (Arg2, 4))
                    {
                        Return (2)
                    }

                    If (LEqual (Arg2, 5))
                    {
                        Return (Package (3)
                        {
                            0x00,
                            0x00,
                            0x00
                        })
                    }

                    If (LEqual (Arg2, 6))
                    {
                        Return (3)
                    }

                    If (LEqual (Arg2, 7))
                    {
                        Return (3)
                    }

                    If (LEqual (Arg2, 8))
                    {
                        Return (1)
                    }

                    Return (Buffer (1)
                    {
                         0x00                                             /* . */
                    })
                }
                If (LEqual (Arg0, ToUUID ("376054ed-cc13-4675-901c-4756d7f2d45d")))
                {
                    ToInteger (Arg2, Local1)
                    If (LEqual (Arg2, 0))
                    {
                        Return (Buffer (1)
                        {
                             0x03                                             /* . */
                        })
                    }

                    If (LEqual (Arg2, 1))
                    {
                        Return (0x00)
                    }

                    Return (Buffer (1)
                    {
                         0x00                                             /* . */
                    })
                }

                Return (Buffer (1)
                {
                     0x00                                             /* . */
                })

           }
       }
  }
}
