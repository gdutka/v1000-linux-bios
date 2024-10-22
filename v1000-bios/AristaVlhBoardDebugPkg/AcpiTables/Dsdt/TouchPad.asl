
Scope (\_SB.I2CA) {
  Device (TPDD) {
    Name (_HID, EISAID("PNP0C50"))
    Name (_CID, "PNP0C50")
    Name (_UID, 5)

    Method(_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x20,
          ControllerInitiated,
          400000,
          AddressingMode7Bit,
          "\\_SB.I2CA",
          ,
          ,
          ,
        )
        GpioInt (
          Level,
          ActiveLow,
          ExclusiveAndWake,
          PullUp,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {9}
      })

      Return(RBUF)
    }

    Method(_STA, 0x0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (THPD, 1))) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    }

    Method(_DSW,3) {
      If(Arg0) {
      } Else {
      }
    }

    //Method(_S0W,0) {
    //  Return(3)
    //}

    Method(_PS0,0) {
    }

    Method(_PS3,0) {
    }

    Method(_DSM, 0x4, Serialized) {
      If(LEqual(Arg0, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE"))) {
        switch(ToInteger(Arg2)) {
          // Function 0: Query function, return based on revision
          case(0) {
            // DSM Revision
            switch(ToInteger(Arg1)) {
              // Revision 1: Function 1 supported
              case(1) {
                Return(Buffer(One) { 0x03 })
              }
              // Revision 2+: no functions supported
              default {
                Return(Buffer(One) { 0x00 })
              }
            }
          }
          // Function 1 : HID Function
          case(1) {
            // HID Descriptor Address
            Return(0x0020)
          }
          // Functions 2+: not supported
          default {
            Return (0x00)
          }
        } // End switch()
      } Else {
        Return(Buffer(One) { 0x00 })
      }
    } // End Method(_DSM)
  } // End Device(TPDD)
} // End Scope(\_SB.I2CA)

Scope(\_SB.I2CB) {
  Device(TPDD) {
    Name (_HID, EISAID("PNP0C50"))
    Name (_CID, "PNP0C50")
    Name (_UID, 6)

    Method(_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x20,
          ControllerInitiated,
          400000,
          AddressingMode7Bit,
          "\\_SB.I2CB",
          ,
          ,
          ,
        )
        GpioInt (
          Level,
          ActiveLow,
          ExclusiveAndWake,
          PullUp,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {9}
      })

      Return(RBUF)
    }

    Method(_STA, 0x0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (THPD, 2))) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    }

    Method(_DSW,3) {
      If(Arg0) {
      } Else {
      }
    }

    //Method(_S0W,0) {
    //  Return(3)
    //}

    Method(_PS0,0) {
    }

    Method(_PS3,0) {
    }

    Method(_DSM, 0x4, Serialized) {
      If(LEqual(Arg0, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE"))) {
        switch(ToInteger(Arg2)) {
          // Function 0: Query function, return based on revision
          case(0) {
            // DSM Revision
            switch(ToInteger(Arg1)) {
              // Revision 1: Function 1 supported
              case(1) {
                Return(Buffer(One) { 0x03 })
              }
              // Revision 2+: no functions supported
              default {
                Return(Buffer(One) { 0x00 })
              }
            }
          }
          // Function 1 : HID Function
          case(1) {
            // HID Descriptor Address
            Return(0x0020)
          }
          // Functions 2+: not supported
          default {
            Return (0x00)
          }
        } // End switch()
      } Else {
        Return(Buffer(One) { 0x00 })
      }
    } // End Method(_DSM)
  } // End Device(TPDD)
} // End Scope(\_SB.I2CB)

Scope(\_SB.I2CC) {
  Device(TPDD) {
    Name (_HID, EISAID("PNP0C50"))
    Name (_CID, "PNP0C50")
    Name (_UID, 7)

    Method(_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x20,
          ControllerInitiated,
          400000,
          AddressingMode7Bit,
          "\\_SB.I2CC",
          ,
          ,
          ,
        )
        GpioInt (
          Level,
          ActiveLow,
          ExclusiveAndWake,
          PullUp,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {9}
      })

      Return(RBUF)
    }

    Method(_STA, 0x0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (THPD, 3))) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    }

    Method(_DSW,3) {
      If(Arg0) {
      } Else {
      }
    }

    //Method(_S0W,0) {
    //  Return(3)
    //}

    Method(_PS0,0) {
    }

    Method(_PS3,0) {
    }

    Method(_DSM, 0x4, Serialized) {
      If(LEqual(Arg0, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE"))) {
        switch(ToInteger(Arg2)) {
          // Function 0: Query function, return based on revision
          case(0) {
            // DSM Revision
            switch(ToInteger(Arg1)) {
              // Revision 1: Function 1 supported
              case(1) {
                Return(Buffer(One) { 0x03 })
              }
              // Revision 2+: no functions supported
              default {
                Return(Buffer(One) { 0x00 })
              }
            }
          }
          // Function 1 : HID Function
          case(1) {
            // HID Descriptor Address
            Return(0x0020)
          }
          // Functions 2+: not supported
          default {
            Return (0x00)
          }
        } // End switch()
      } Else {
        Return(Buffer(One) { 0x00 })
      }
    } // End Method(_DSM)
  } // End Device(TPDD)
} // End Scope(\_SB.I2CC)

Scope(\_SB.I2CD) {
  Device(TPDD) {
    Name (_HID, EISAID("PNP0C50"))
    Name (_CID, "PNP0C50")
    Name (_UID, 8)

    Method(_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x20,
          ControllerInitiated,
          400000,
          AddressingMode7Bit,
          "\\_SB.I2CD",
          ,
          ,
          ,
        )
        GpioInt (
          Level,
          ActiveLow,
          ExclusiveAndWake,
          PullUp,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {9}
      })

      Return(RBUF)
    }

    Method(_STA, 0x0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (THPD, 4))) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    }

    Method(_DSW,3) {
      If(Arg0) {
      } Else {
      }
    }

    //Method(_S0W,0) {
    //  Return(3)
    //}

    Method(_PS0,0) {
    }

    Method(_PS3,0) {
    }

    Method(_DSM, 0x4, Serialized) {
      If(LEqual(Arg0, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE"))) {
        switch(ToInteger(Arg2)) {
          // Function 0: Query function, return based on revision
          case(0) {
            // DSM Revision
            switch(ToInteger(Arg1)) {
              // Revision 1: Function 1 supported
              case(1) {
                Return(Buffer(One) { 0x03 })
              }
              // Revision 2+: no functions supported
              default {
                Return(Buffer(One) { 0x00 })
              }
            }
          }
          // Function 1 : HID Function
          case(1) {
            // HID Descriptor Address
            Return(0x0020)
          }
          // Functions 2+: not supported
          default {
            Return (0x00)
          }
        } // End switch()
      } Else {
        Return(Buffer(One) { 0x00 })
      }
    } // End Method(_DSM)
  } // End Device(TPDD)
} // End Scope(\_SB.I2CD)

