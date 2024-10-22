
Scope(\_SB.I2CA) {
  Device(TPNL) {
    Name (_HID, EISAID("PNP0C50"))
    Name (_CID, "PNP0C50")
    Name (_UID, 1)

    Method(_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x4A,
          ControllerInitiated,
          1000000,
          AddressingMode7Bit,
          "\\_SB.I2CA",
          ,
          ,
          ,
        )
        GpioInt (
          Level,
          ActiveLow,
          Shared,
          PullUp,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {12}
      })

      Return(RBUF)
    }

    Method(_STA, 0x0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (THPN, 1))) {
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
            Return(0x0000)
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
  } // End Device(TPNL)
} // End Scope(\_SB.I2CA)

Scope(\_SB.I2CB) {
  Device(TPNL) {
    Name (_HID, EISAID("PNP0C50"))
    Name (_CID, "PNP0C50")
    Name (_UID, 2)

    Method(_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x4A,
          ControllerInitiated,
          1000000,
          AddressingMode7Bit,
          "\\_SB.I2CB",
          ,
          ,
          ,
        )
        GpioInt (
          Level,
          ActiveLow,
          Shared,
          PullUp,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {12}
      })

      Return(RBUF)
    }

    Method(_STA, 0x0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (THPN, 2))) {
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
            Return(0x0000)
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
  } // End Device(TPNL)
} // End Scope(\_SB.I2CB)

Scope(\_SB.I2CC) {
  Device(TPNL) {
    Name (_HID, EISAID("PNP0C50"))
    Name (_CID, "PNP0C50")
    Name (_UID, 3)

    Method(_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x4A,
          ControllerInitiated,
          1000000,
          AddressingMode7Bit,
          "\\_SB.I2CC",
          ,
          ,
          ,
        )
        GpioInt (
          Level,
          ActiveLow,
          Shared,
          PullUp,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {12}
      })

      Return(RBUF)
    }

    Method(_STA, 0x0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (THPN, 3))) {
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
            Return(0x0000)
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
  } // End Device(TPNL)
} // End Scope(\_SB.I2CC)

Scope(\_SB.I2CD) {
  Device(TPNL) {
    Name (_HID, EISAID("PNP0C50"))
    Name (_CID, "PNP0C50")
    Name (_UID, 4)

    Method(_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x4A,
          ControllerInitiated,
          1000000,
          AddressingMode7Bit,
          "\\_SB.I2CD",
          ,
          ,
          ,
        )
        GpioInt (
          Level,
          ActiveLow,
          Shared,
          PullUp,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {12}
      })

      Return(RBUF)
    }

    Method(_STA, 0x0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (THPN, 4))) {
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
            Return(0x0000)
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
  } // End Device(TPNL)
} // End Scope(\_SB.I2CD)

