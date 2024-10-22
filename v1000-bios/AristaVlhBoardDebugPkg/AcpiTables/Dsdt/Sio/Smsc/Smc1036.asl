    Device(UAR3) {
      Name(_HID,EISAID("PNP0501"))            // Hardware Device ID
      Name(_UID,0x3)
      //
      // SIDP for dynamic update Super I/O Index port
      //
      Name (SIDP, 0x164E)

      OperationRegion (
        SMSC,       // name of Operation Region for SuperIO device
        SystemIO,   // type of address space
        SIDP,       // base address for Configuration Mode
        2)          // size of region in bytes

      Field (
        SMSC,       // name of Operation Region containing field
        ByteAcc,    // access in Byte mode
        NoLock,     // may change, depending on SMI behavior
        Preserve)   // Update Rule
      {
        INDX, 8,    // field named INDX is 8 bits wide
        DATA, 8     // field named DATA is 8 bits wide
      }

      IndexField (
        INDX,       // name of Index register field
        DATA,       // name of Data register field is DATA
        ByteAcc,    // both access as byte registers
        NoLock,     // may change, depending on SMI behavior
        Preserve)   // update rule
      {
        Offset(0x02),
        CR02, 8,    // UART Power

        Offset(0x0C),
        CR0C, 8,    // UART Mode

        Offset(0x0D),
        CR0D, 8,    // Chip ID

        Offset(0x25),
        CR25, 8,    // UART Address

        Offset(0x28),
        CR28, 8     // UART IRQ
      }

      // Enter Configuration mode
      Method (ENCG) {
        Store (0x55, INDX)
      }

      // Exit Configuation mode
      Method (EXCG) {
        Store (0xAA, INDX)
      }

      Method (_STA, 0, Serialized) {
        if (LEqual (SDMO, 1)) {             // Enable Serial Debug Message Under OS
          // Disable LPC SIO Serial Port, let OS cannot don't destory the EFI BIOS Legacy setting of Serial Port.
          Store (0x00, Local1)
        } Else {
          // Enter Configuration Mode
          ENCG ()

          //Read chip ID
          Store (CR0D, Local0)

          //Check chip ID
          If (LEqual (Local0, 0x82)) {
            Store (0x0F, Local1)
          } Else {
            Store (0x00, Local1)
          }

          // Exit Configuration Mode
          EXCG ()
        }
        Return (Local1)
      }

     Method(_CRS, 0, Serialized) {
       Name (BUF0, ResourceTemplate() {
         IO (Decode16, 0x3F8, 0x3F8, 0x08, 0x08)
         IRQNoFlags(){4}
       })

       // Create pointers to the specific byte.
       CreateByteField (BUF0, 0x02, IOL0)
       CreateByteField (BUF0, 0x03, IOH0)
       CreateByteField (BUF0, 0x04, IOL1)
       CreateByteField (BUF0, 0x05, IOH1)
       CreateByteField (BUF0, 0x07, LEN0)
       CreateWordField (BUF0, 0x09, IRQW)


       // Enter Configuration Mode
       ENCG ()

       // Write UART Base Address
       ShiftLeft (CR25, 2, IOL0)   // Low Byte.
       ShiftLeft (CR25, 2, IOL1)

       ShiftRight(CR25, 6, IOH0)   // High Byte.
       ShiftRight(CR25, 6, IOH1)

       // Write Len
       Store (8, LEN0)

       // Write the IRQ setting from the chip
       ShiftLeft (One, And (CR28, 0x0F), IRQW)

       // Exit Configuration Mode
       EXCG ()

       Return(BUF0) // return the result

     } // end _CRS method
    }

