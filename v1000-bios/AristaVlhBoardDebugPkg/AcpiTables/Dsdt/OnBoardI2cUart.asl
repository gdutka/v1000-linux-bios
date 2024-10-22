  Scope(\_SB.FUR0)
    {

      //
      // UART HCK test
      //
      Device(UART) {
        Name (_HID, "UTK0001")
        Name (_CID, "UARTTest")
        Name (_UID,0)

        Method (_CRS, 0x0, NotSerialized) {
          Name (
            RBUF,
            ResourceTemplate () {
              UARTSerialBus (
                115200, // Baud Rate = 115200
                DataBitsEight,
                StopBitsOne,
                0xC0,
                LittleEndian,
                ParityTypeNone,
                FlowControlHardware,
                32,
                32,
                "\\_SB.FUR0",
                ,
                ,
                ,
              )
            }
          )
          Return(RBUF)
        }
        Method(_STA) {
          If (LEqual(MWTT,0x00)) {      // MITT Only
            Return (0x0F)               // Exist
          } Else {
            Return (0x00)
          }
        }
      } // End Device(UART)
    } // End Device(FUR0)

  Scope(\_SB.FUR1)
    {
      //
      // UART HCK test
      //
      Device(UART) {
        Name (_HID, "UTK0001")
        Name (_CID, "UARTTest")
        Name (_UID,1)

        Method (_CRS, 0x0, NotSerialized) {
          Name (
            RBUF,
            ResourceTemplate () {
              UARTSerialBus (
                115200, // Baud Rate = 115200
                DataBitsEight,
                StopBitsOne,
                0xC0,
                LittleEndian,
                ParityTypeNone,
                FlowControlHardware,
                32,
                32,
                "\\_SB.FUR1",
                ,
                ,
                ,
              )
            }
          )
          Return(RBUF)
        }
        Method(_STA) {
          If (LEqual(MWTT,0x00)) {      // MITT Only
            Return (0x0F)               // Exist
          } Else {
            Return (0x00)
          }
        }
      } // End Device(UART)

    } // End Device(FUR1)

    Scope(\_SB.I2CA) 
    {
      Device(WTP1) {
        Name(_ADR,0)
        Name (_HID, "STK0001")
        Name (_CID, "SPBTestKMDF")
        Method(_CRS, 0x0, NotSerialized) {
          Name (RBUF, ResourceTemplate () {
            I2CSerialBus (
              0x7f,
              ControllerInitiated,
              100000,
              AddressingMode7Bit,
              "\\_SB.I2CA",
              ,
              ,
              ,
            )
          })
          Return(RBUF)
        }
        Method(_STA) {
          If (LEqual(MWTT,0x01)) {      // WITT Only
            Return (0x0F)               // Exist
          } Else {
            Return (0x00)
          }
        }
      } // End Device(WTP1)

      //TP1 100Khz Standard Target 
      Device(MTP1) { 
        Name (_HID, "STK0001") 
        Name (_CID, "WITTTest") 
        Method(_CRS, 0x0, NotSerialized)  { 
          Name (RBUF, ResourceTemplate ()  { 
          I2CSerialBus ( 0x11, ControllerInitiated, 100000,AddressingMode7Bit, "\\_SB_.I2CA",,,,)
        }) 
        Return(RBUF) 
        } 
        Method(_STA) {
          If (LEqual(MWTT,0x00)) {     // MITT Only
            Return (0x0F)               // Exist
          } Else {
            Return (0x00)
          }
        }
      } // End Device(MTP1)

        Device(WTP2) {
          Name(_ADR,0)
          Name (_HID, "STK0002")
          Name (_CID, "SPBTestKMDF")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x11,
                ControllerInitiated,
                100000,
                AddressingMode7Bit,
                "\\_SB.I2CA",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP2)

        //TP2 400Khz Fast Target 
        Device(MTP2) { 
          Name (_HID, "STK0002") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x12, ControllerInitiated, 400000,AddressingMode7Bit, "\\_SB_.I2CA",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(MTP2)

        Device(WTP3) {
          Name(_ADR,0)
          Name (_HID, "STK0003")
          Name (_CID, "SPBTestKMDF")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x12,
                ControllerInitiated,
                400000,
                AddressingMode7Bit,
                "\\_SB.I2CA",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP3)

        ////TP3 1 Mhz FastPlus Target 
        Device(MTP3) { 
          Name (_HID, "STK0003") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x13, ControllerInitiated, 1000000,AddressingMode7Bit, "\\_SB_.I2CA",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(MTP3)

        Device(WTP4) {
          Name(_ADR,0)
          Name (_HID, "STK0004")
          Name (_CID, "SPBTestKMDF")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x13,
                ControllerInitiated,
                1000000,
                AddressingMode7Bit,
                "\\_SB.I2CA",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP4)

        //TP4 1.4 Mhz High Speed, optional target 
        Device(MTP4) { 
          Name (_HID, "STK0004") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x14, ControllerInitiated, 140000,AddressingMode7Bit, "\\_SB_.I2CA",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(TP4)

        //TP4 1.4 Mhz High Speed, optional target 
        Device(MTP5) { 
          Name (_HID, "STK0005") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x15, ControllerInitiated, 340000,AddressingMode7Bit, "\\_SB_.I2CA",,,,)
          }) 
          Return(RBUF) 
          } 

          Method(_STA) {
            If (LEqual(MWTT,0x00)) {      // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }

        } // End Device(TP5)
    } // End Device(I2CA)

    Scope(\_SB.I2CB)
    {
      Device(WT21) {
        Name(_ADR,0)
        Name (_HID, "STK00012")
        Name (_CID, "SPBTestKMDF2")
        Method(_CRS, 0x0, NotSerialized) {
          Name (RBUF, ResourceTemplate () {
            I2CSerialBus (
              0x7f,
              ControllerInitiated,
              100000,
              AddressingMode7Bit,
              "\\_SB.I2CB",
              ,
              ,
              ,
            )
          })
          Return(RBUF)
        }
        Method(_STA) {
          If (LEqual(MWTT,0x01)) {      // WITT Only
            Return (0x0F)               // Exist
          } Else {
            Return (0x00)
          }
        }
      } // End Device(WTP1)

      //TP1 100Khz Standard Target 
      Device(MT21) { 
        Name (_HID, "STK00012") 
        Name (_CID, "WITTTest") 
        Method(_CRS, 0x0, NotSerialized)  { 
          Name (RBUF, ResourceTemplate ()  { 
          I2CSerialBus ( 0x11, ControllerInitiated, 100000,AddressingMode7Bit, "\\_SB_.I2CB",,,,)
        }) 
        Return(RBUF) 
        } 
        Method(_STA) {
          If (LEqual(MWTT,0x00)) {     // MITT Only
            Return (0x0F)               // Exist
          } Else {
            Return (0x00)
          }
        }
      } // End Device(MTP1)

        Device(WT22) {
          Name(_ADR,0)
          Name (_HID, "STK00022")
          Name (_CID, "SPBTestKMDF2")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x11,
                ControllerInitiated,
                100000,
                AddressingMode7Bit,
                "\\_SB.I2CB",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP2)

        //TP2 400Khz Fast Target 
        Device(MT22) { 
          Name (_HID, "STK00022") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x12, ControllerInitiated, 400000,AddressingMode7Bit, "\\_SB_.I2CB",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(MTP2)

        Device(WT23) {
          Name(_ADR,0)
          Name (_HID, "STK00032")
          Name (_CID, "SPBTestKMDF2")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x12,
                ControllerInitiated,
                400000,
                AddressingMode7Bit,
                "\\_SB.I2CB",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP3)

        ////TP3 1 Mhz FastPlus Target 
        Device(MT23) { 
          Name (_HID, "STK00032") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x13, ControllerInitiated, 1000000,AddressingMode7Bit, "\\_SB_.I2CB",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(MTP3)

        Device(WT24) {
          Name(_ADR,0)
          Name (_HID, "STK00042")
          Name (_CID, "SPBTestKMDF2")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x13,
                ControllerInitiated,
                1000000,
                AddressingMode7Bit,
                "\\_SB.I2CB",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP4)

        //TP4 1.4 Mhz High Speed, optional target 
        Device(MT24) { 
          Name (_HID, "STK00042") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x14, ControllerInitiated, 140000,AddressingMode7Bit, "\\_SB_.I2CB",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(TP4)

        //TP4 1.4 Mhz High Speed, optional target 
        Device(MT25) { 
          Name (_HID, "STK00052") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x15, ControllerInitiated, 340000,AddressingMode7Bit, "\\_SB_.I2CB",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {      // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }

        } // End Device(TP5)

    } // End Device(I2CB)

    Scope(\_SB.I2CC)
    {
      Device(WT31) {
        Name(_ADR,0)
        Name (_HID, "STK00013")
        Name (_CID, "SPBTestKMDF3")
        Method(_CRS, 0x0, NotSerialized) {
          Name (RBUF, ResourceTemplate () {
            I2CSerialBus (
              0x7f,
              ControllerInitiated,
              100000,
              AddressingMode7Bit,
              "\\_SB.I2CC",
              ,
              ,
              ,
            )
          })
          Return(RBUF)
        }
        Method(_STA) {
          If (LEqual(MWTT,0x01)) {      // WITT Only
            Return (0x0F)               // Exist
          } Else {
            Return (0x00)
          }
        }
      } // End Device(WTP1)

      //TP1 100Khz Standard Target 
      Device(MT31) { 
        Name (_HID, "STK00013") 
        Name (_CID, "WITTTest") 
        Method(_CRS, 0x0, NotSerialized)  { 
          Name (RBUF, ResourceTemplate ()  { 
          I2CSerialBus ( 0x11, ControllerInitiated, 100000,AddressingMode7Bit, "\\_SB_.I2CC",,,,)
        }) 
        Return(RBUF) 
        } 
        Method(_STA) {
          If (LEqual(MWTT,0x00)) {     // MITT Only
            Return (0x0F)               // Exist
          } Else {
            Return (0x00)
          }
        }
      } // End Device(MTP1)

        Device(WT32) {
          Name(_ADR,0)
          Name (_HID, "STK00023")
          Name (_CID, "SPBTestKMDF3")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x11,
                ControllerInitiated,
                100000,
                AddressingMode7Bit,
                "\\_SB.I2CC",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP2)

        //TP2 400Khz Fast Target 
        Device(MT32) { 
          Name (_HID, "STK00023") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x12, ControllerInitiated, 400000,AddressingMode7Bit, "\\_SB_.I2CC",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(MTP2)

        Device(WT33) {
          Name(_ADR,0)
          Name (_HID, "STK00033")
          Name (_CID, "SPBTestKMDF3")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x12,
                ControllerInitiated,
                400000,
                AddressingMode7Bit,
                "\\_SB.I2CC",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP3)

        ////TP3 1 Mhz FastPlus Target 
        Device(MT33) { 
          Name (_HID, "STK00033") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x13, ControllerInitiated, 1000000,AddressingMode7Bit, "\\_SB_.I2CC",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(MTP3)

        Device(WT34) {
          Name(_ADR,0)
          Name (_HID, "STK00043")
          Name (_CID, "SPBTestKMDF3")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x13,
                ControllerInitiated,
                1000000,
                AddressingMode7Bit,
                "\\_SB.I2CC",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP4)

        //TP4 1.4 Mhz High Speed, optional target 
        Device(MT34) { 
          Name (_HID, "STK00043") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x14, ControllerInitiated, 140000,AddressingMode7Bit, "\\_SB_.I2CC",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(TP4)

        //TP4 1.4 Mhz High Speed, optional target 
        Device(MT35) { 
          Name (_HID, "STK00053") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x15, ControllerInitiated, 340000,AddressingMode7Bit, "\\_SB_.I2CC",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {      // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }

        } // End Device(TP5)

    }

    Scope(\_SB.I2CD)
    {
      Device(WT41) {
        Name(_ADR,0)
        Name (_HID, "STK00014")
        Name (_CID, "SPBTestKMDF4")
        Method(_CRS, 0x0, NotSerialized) {
          Name (RBUF, ResourceTemplate () {
            I2CSerialBus (
              0x7f,
              ControllerInitiated,
              100000,
              AddressingMode7Bit,
              "\\_SB.I2CD",
              ,
              ,
              ,
            )
          })
          Return(RBUF)
        }
        Method(_STA) {
          If (LEqual(MWTT,0x01)) {      // WITT Only
            Return (0x0F)               // Exist
          } Else {
            Return (0x00)
          }
        }
      } // End Device(WTP1)

      //TP1 100Khz Standard Target 
      Device(MT41) { 
        Name (_HID, "STK00014") 
        Name (_CID, "WITTTest") 
        Method(_CRS, 0x0, NotSerialized)  { 
          Name (RBUF, ResourceTemplate ()  { 
          I2CSerialBus ( 0x11, ControllerInitiated, 100000,AddressingMode7Bit, "\\_SB_.I2CD",,,,)
        }) 
        Return(RBUF) 
        } 
        Method(_STA) {
          If (LEqual(MWTT,0x00)) {     // MITT Only
            Return (0x0F)               // Exist
          } Else {
            Return (0x00)
          }
        }
      } // End Device(MTP1)

        Device(WT42) {
          Name(_ADR,0)
          Name (_HID, "STK00024")
          Name (_CID, "SPBTestKMDF4")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x11,
                ControllerInitiated,
                100000,
                AddressingMode7Bit,
                "\\_SB.I2CD",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP2)

        //TP2 400Khz Fast Target 
        Device(MT42) { 
          Name (_HID, "STK00024") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x12, ControllerInitiated, 400000,AddressingMode7Bit, "\\_SB_.I2CD",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(MTP2)

        Device(WT43) {
          Name(_ADR,0)
          Name (_HID, "STK00034")
          Name (_CID, "SPBTestKMDF4")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x12,
                ControllerInitiated,
                400000,
                AddressingMode7Bit,
                "\\_SB.I2CD",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP3)

        ////TP3 1 Mhz FastPlus Target 
        Device(MT43) { 
          Name (_HID, "STK00034") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x13, ControllerInitiated, 1000000,AddressingMode7Bit, "\\_SB_.I2CD",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(MTP3)

        Device(WT44) {
          Name(_ADR,0)
          Name (_HID, "STK00044")
          Name (_CID, "SPBTestKMDF4")
          Method(_CRS, 0x0, NotSerialized) {
            Name (RBUF, ResourceTemplate () {
              I2CSerialBus (
                0x13,
                ControllerInitiated,
                1000000,
                AddressingMode7Bit,
                "\\_SB.I2CD",
                ,
                ,
                ,
              )
            })
            Return(RBUF)
          }
          Method(_STA) {
            If (LEqual(MWTT,0x01)) {      // WITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(WTP4)

        //TP4 1.4 Mhz High Speed, optional target 
        Device(MT44) { 
          Name (_HID, "STK00044") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x14, ControllerInitiated, 140000,AddressingMode7Bit, "\\_SB_.I2CD",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {     // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }
        } // End Device(TP4)

        //TP4 1.4 Mhz High Speed, optional target 
        Device(MT45) { 
          Name (_HID, "STK00054") 
          Name (_CID, "WITTTest") 
          Method(_CRS, 0x0, NotSerialized)  { 
            Name (RBUF, ResourceTemplate ()  { 
            I2CSerialBus ( 0x15, ControllerInitiated, 340000,AddressingMode7Bit, "\\_SB_.I2CD",,,,)
          }) 
          Return(RBUF) 
          } 
          Method(_STA) {
            If (LEqual(MWTT,0x00)) {      // MITT Only
              Return (0x0F)               // Exist
            } Else {
              Return (0x00)
            }
          }

        } // End Device(TP5)

    }    
