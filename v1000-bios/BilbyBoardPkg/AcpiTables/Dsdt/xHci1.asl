/** @file

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _XHCI1_
#define _XHCI1_

Device(XHC1)
{
  Name(_ADR, 0x00000004)

  Method(_PRW,0,NotSerialized) {
    Return (GPRW (0x19,0x04))
  }

  Device(RHUB)
  {
    Name(_ADR, 0x00000000)

//************************EHCI Ports*******************
//*****************************************************
    // EHCI Port #1, Stack USB Connecter port;
    Device(PRT1) {
      Name(_ADR, 0x00000001)

      Name(_UPC, Package(4){
          0xFF,     // Port is connetable;
          0x03,     // USB 3 Standard-A Connector;
          0x00000000,   // Reserved 0;
          0x00000000    // Reserved 1;
      })

      Name(_PLD, Package(1){
        Buffer(0x14) {
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x11, 0x0C, 0x80, 0x02, //User visible, Group Token =0;
                                  // Group Position 5st;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }

      })
    }

    // EHCI Port #2;
    Device(PRT2) {
      Name(_ADR, 0x00000002)

      Name(_UPC, Package(4){
          0xFF,     // Port is connetable;
          0xFF,     // 0xFF: Proprietary connector;
          0x00000000,   // Reserved 0;
          0x00000000    // Reserved 1;
      })

      Name(_PLD, Package(1){
        Buffer(0x14) {
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x10, 0x0C, 0x00, 0x03, // Not User visible, Group Token = 0;
                                  // Group Position 6st;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }

      })
    }    

//************************xHCI Ports*******************
//*****************************************************

    // xHCI Port #3, Stack USB Connecter port;
     Device(PRT3) {
      Name(_ADR, 0x00000003)        // port number#
      Name(_UPC, Package(4){
          0xFF,                     // 0xFF: Connectable    0x00: Non-connectable
          0x03,                     // USB 3 Standard-A Connector
          0x00000000,               // Reserved 0;
          0x00000000                // Reserved 1;
      })
      Name(_PLD, Package(1){
        Buffer(0x14) {
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x11, 0x0C, 0x80, 0x02, //User visible, Group Token =0;
                                  // Group Position 5st;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }

      })      
    }
  }
} // XHC1


#endif
