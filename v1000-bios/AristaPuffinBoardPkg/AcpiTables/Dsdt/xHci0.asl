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

#ifndef _XHCI0_
#define _XHCI0_

Device(XHC0)
{
  Name(_ADR, 0x00000003)

  Method(_PRW,0,NotSerialized) {
    Return (GPRW (0x19,0x04))
  }

  Device(RHUB)
  {
    Name(_ADR, 0x00000000)

    // xHCI Port #1, Stack USB Connecter port;
    // map to EHCI Port #5;
    Device(PRT1) {
      Name(_ADR, 0x00000001)
      
      Name(UPC1, Package(4) { 0xFF, 0x09, 0x00000000, 0x00000000 } ) //RV1  Type-C
      Name(UPC2, Package(4) { 0xFF, 0x09, 0x00000000, 0x00000000 } ) //RV2  Type-C      

      Method (_UPC,0,Serialized)
      {       
        If (RV2I) 
        {      
          Return (UPC2)
        }
        Else
        {
          Return (UPC1)        
        }           
      }
        
      Name(PLD1, Package() {// RV1
        Buffer(0x14) {      
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x11, 0x0C, 0x80, 0x00, //User visible, Group Token =0;
                                  // Group Position 1st;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        } 
      })
      
      Name(PLD2, Package() {// RV2
        Buffer(0x14) {         
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x11, 0x0C, 0x80, 0x00, //User visible, Group Token =0;
                                // Group Position 1st;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve       
          }    
      })
            
      Method (_PLD,0,Serialized) 
      {
        If (RV2I) 
        {
          Return (PLD2)
        }
        Else
        {              
          Return (PLD1)
        }
      }
    }

    // xHCI Port #2, Stack USB Connecter port;
    // Map to EHCI Port #6;
    Device(PRT2) {
      Name(_ADR, 0x00000002)
      
      Name(UPC1, Package(4) { 0xFF, 0x03, 0x00000000, 0x00000000 } ) //RV1 Standard-A Connector
      Name(UPC2, Package(4) { 0xFF, 0x03, 0x00000000, 0x00000000 } ) //RV2 Standard-A Connector

      Method (_UPC,0,Serialized)
      {       
        If (RV2I) 
        {      
          Return (UPC2)
        }
        Else
        {
          Return (UPC1)        
        }             
      } 
      
      Name(PLD1, Package(1) {// RV1
        Buffer(0x14) {       
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x91, 0x0C, 0x00, 0x01, // User visible, Group Token = 0;
                                // Group Position 2nd;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      })
      
      Name(PLD2, Package(1) {// RV2 
        Buffer(0x14) { 
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x91, 0x0C, 0x00, 0x01, // User visible, Group Token = 0;
                                // Group Position 2nd;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }      
      } )
            
      Method (_PLD,0,Serialized) 
      {
        If (RV2I) 
        {
          Return (PLD2)
        }
        Else
        {              
          Return (PLD1)
        }
      }           
    }

    // xHCI Port #3, Single USB Connecter port;
    // Map to EHCI Port #7;
    Device(PRT3) {
      Name(_ADR, 0x00000003)
      
      Name(UPC1, Package(4) { 0xFF, 0x03, 0x00000000, 0x00000000 } ) //RV1 Standard-A Connector
      Name(UPC2, Package(4) { 0xFF, 0x03, 0x00000000, 0x00000000 } ) //RV2 Standard-A Connector

      Method (_UPC,0,Serialized)
      {       
        If (RV2I) 
        {      
          Return (UPC2)
        }
        Else
        {
          Return (UPC1)        
        }             
      } 
      
      Name(PLD1, Package(1) {// RV1
        Buffer(0x14) {       
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x91, 0x1C, 0x80, 0x01, // User visible, Group Token = 0;
                                  // Group Position = 3rd;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      })
      
      Name(PLD2, Package(1) {// RV1 
        Buffer(0x14) {       
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x91, 0x1C, 0x80, 0x01, // User visible, Group Token = 0;
                                  // Group Position = 3rd;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }   
      })
            
      Method (_PLD,0,Serialized) 
      {
        If (RV2I) 
        {
          Return (PLD2)
        }
        Else
        {              
          Return (PLD1)
        }
      }         
    }        

    // xHCI Port #4
    // Map to EHCI Port #8;
    Device(PRT4) {
      Name(_ADR, 0x00000004)
      
      Name(UPC1, Package(4) { 0xFF, 0x09, 0x00000000, 0x00000000 } ) //RV1 Type C connector - USB2 and SS with Switch
      Name(UPC2, Package(4) { 0xFF, 0x09, 0x00000000, 0x00000000 } ) //RV2 Type C connector - USB2 and SS with Switch

      Method (_UPC,0,Serialized)
      { 
        If (RV2I) 
        {      
          Return (UPC2)
        }
        Else
        {
          Return (UPC1)        
        }        
      } 
      
      Name(PLD1, Package(1) {// RV1
        Buffer(0x14) {       
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x91, 0x1D, 0x00, 0x02, // User visible, Group Token = 0;
                                  // Group Position = 4th;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      })
      
      Name(PLD2, Package(1) {// RV2 
        Buffer(0x14) {       
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x91, 0x1D, 0x00, 0x02, // User visible, Group Token = 0;
                                  // Group Position = 4th;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve     
        }
      })
            
      Method (_PLD,0,Serialized) 
      {
        If (RV2I) 
        {
          Return (PLD2)
        }
        Else
        {              
          Return (PLD1)
        }
      }    
    }

    // Port #5
    Device(PRT5) {
      Name(_ADR, 0x00000005)
      
      Name(UPC1, Package(4) { 0xFF, 0x09, 0x00000000, 0x00000000 } ) //RV1 Type C connector - USB2 and SS with Switch
      Name(UPC2, Package(4) { 0xFF, 0x03, 0x00000000, 0x00000000 } ) //RV2 Standard-A

      Method (_UPC,0,Serialized)
      {       
        If (RV2I) 
        {      
          Return (UPC2)
        }
        Else
        {
          Return (UPC1)        
        }              
      } 
      
      Name(PLD1, Package(1) {// RV1
        Buffer(0x14) {       
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x11, 0x0C, 0x80, 0x00, //User visible, Group Token =0;
                                  // Group Position 1st;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      })
      
      Name(PLD2, Package(1) {// RV2
        Buffer(0x14) {        
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x11, 0x0C, 0x80, 0x02, //User visible, Group Token =0;
                                  // Group Position 5th;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      })
                  
      Method (_PLD,0,Serialized) 
      {
        If (RV2I) 
        {
          Return (PLD2)
        }
        Else
        {              
          Return (PLD1)
        }
      }         
    }

    // Port #6
    Device(PRT6) {
      Name(_ADR, 0x00000006)

      Name(UPC1, Package(4) { 0xFF, 0x03, 0x00000000, 0x00000000 } ) //RV1 Standard-A
      Name(UPC2, Package(4) { 0xFF, 0xFF, 0x00000000, 0x00000000 } ) //RV2 Proprietary connector

      Method (_UPC,0,Serialized)
      {       
        If (RV2I) 
        {      
          Return (UPC2)
        }
        Else
        {
          Return (UPC1)        
        }              
      }     

      Name(PLD1, Package(1) {// RV1
        Buffer(0x14) {      
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x91, 0x0C, 0x00, 0x01, // User visible, Group Token = 0;
                                  // Group Position 2nd;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      })
      
      Name(PLD2, Package(1) {// RV2
        Buffer(0x14) {       
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x10, 0x0C, 0x00, 0x03, // Not User visible, Group Token = 0;
                                  // Group Position 6th;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      })
            
      Method (_PLD,0,Serialized) 
      {
        If (RV2I) 
        {
          Return (PLD2)
        }
        Else
        {              
          Return (PLD1)
        }
      }
    }

    // Port #7
    Device(PRT7) {
      Name(_ADR, 0x00000007)
      
      Name(UPC1, Package(4) { 0xFF, 0x03, 0x00000000, 0x00000000 } ) //RV1 Standard-A
      Name(UPC2, Package(4) { 0xFF, 0x09, 0x00000000, 0x00000000 } ) //RV2 Type C connector

      Method (_UPC,0,Serialized)
      {       
        If (RV2I) 
        {      
          Return (UPC2)
        }
        Else
        {
          Return (UPC1)        
        }              
      }

      Name(PLD1, Package(1) {// RV1
        Buffer(0x14) {      
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x91, 0x1C, 0x80, 0x01, // User visible, Group Token = 0;
                                  // Group Position = 3rd;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      })
      
      Name(PLD2, Package(1) {// RV2
        Buffer(0x14) {       
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x11, 0x0C, 0x80, 0x00, //User visible, Group Token =0;
                                  // Group Position 1st;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }       
      }) 
            
      Method (_PLD,0,Serialized) 
      {
        If (RV2I) 
        {
          Return (PLD2)
        }
        Else
        {              
          Return (PLD1)
        }
      }              
    }

    // Port #8;
    Device(PRT8)
    {
      Name(_ADR, 0x00000008)
      
      Name(UPC1, Package(4) { 0xFF, 0x09, 0x00000000, 0x00000000 } ) //RV1 Type C connector
      Name(UPC2, Package(4) { 0xFF, 0x03, 0x00000000, 0x00000000 } ) //RV2 Standard-A

      Method (_UPC,0,Serialized)
      {       
        If (RV2I) 
        {      
          Return (UPC2)
        }
        Else
        {
          Return (UPC1)        
        }              
      }
      
      Name(PLD1, Package(1) {// RV1
        Buffer(0x14) {      
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x91, 0x1D, 0x00, 0x02, // User visible, Group Token = 0;
                                  // Group Position = 4th;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      })
      
      Name(PLD2, Package(1) {// RV2
        Buffer(0x14) {       
          0x82,                   // Revision 2, Ignore color;
          0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
          0x00, 0x00, 0x00, 0x00, // Width & Height;
          0x91, 0x0C, 0x00, 0x01, // User visible, Group Token = 0;
                                  // Group Position 2nd;
          0x01, 0x00, 0x00, 0x00,
          0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      }) 
            
      Method (_PLD,0,Serialized) 
      {
        If (RV2I) 
        {
          Return (PLD2)
        }
        Else
        {              
          Return (PLD1)
        }
      }                
    }
    
    // Port #9; Only avaliable on RV2
    Device(PRT9)
    {
      Name(_ADR, 0x00000009)
      
      Method(_STA,0,NotSerialized) {
        If (RV2I)
        {
          Return(0xF)
        }
        Else
        {      
          Return(0)
        }
      }      
      
      Name(_UPC, Package(4) { 0xFF, 0x03, 0x00000000, 0x00000000 } ) //RV2 Standard-A

      Name(_PLD, Package(1){
        Buffer(0x14) { 
        0x82,                   // Revision 2, Ignore color;
        0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
        0x00, 0x00, 0x00, 0x00, // Width & Height;
        0x91, 0x1C, 0x80, 0x01, // User visible, Group Token = 0;
                                // Group Position = 3rd;
        0x01, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      })                          
    }

    // Port #10; Only avaliable on RV2
    Device(PRTA)
    {
      Name(_ADR, 0x0000000A)
      
      Method(_STA,0,NotSerialized) {
        If (RV2I)
        {
          Return(0xF)
        }
        Else
        {      
          Return(0)
        }
      }      
      
      Name(_UPC, Package(4) { 0xFF, 0x09, 0x00000000, 0x00000000 } ) //RV2 Type C connector
      
      Name(_PLD, Package(1){
        Buffer(0x14) {      
        0x82,                   // Revision 2, Ignore color;
        0x00, 0x00, 0x00,       // 24-bit RGB value for the color of the device;
        0x00, 0x00, 0x00, 0x00, // Width & Height;
        0x91, 0x1D, 0x00, 0x02, // User visible, Group Token = 0;
                                // Group Position = 4th;
        0x01, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF  // Reserve
        }
      })        
    }          
  }
} // XHC0


#endif
