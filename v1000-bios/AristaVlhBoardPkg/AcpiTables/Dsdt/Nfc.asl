
Scope (\_SB.I2CA) {
  Device (NFC1) {
    Name (_HID, EISAID("NXP8013"))
    Name (_UID, 1)

    Method (_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x29,                     // SlaveAddress: 0x1FA
          ControllerInitiated,      // SlaveMode: ControllerInitiated
          100000,                   // ConnectionSpeed: in Hz (100 KHz)
          AddressingMode7Bit,       // AddressingMode: default to 7bit
          "\\_SB.I2CA",             // ResourceSource: I2C Controller name
          ,                         // ResourceSourceIndex
          ,                         // ResourceUsage
          ,                         // DescriptorName
        )                           // VendorData

        GpioInt (
          Level,
          ActiveHigh,
          ExclusiveAndWake,
          PullNone,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {69}                      // NFC_IRQ = AGPIO69

// Don't change the sequence of these two GPIO pin, REQ must be the first GpioIo pin. NXP Device Driver will treat the first one as REQ.
        GpioIo (
          Exclusive,
          PullNone,
          0,
          0,
          IoRestrictionOutputOnly,
          "\\_SB.GPIO"
        ) {137}                      // NFC_REQ = AGPIO137

        GpioIo (
          Exclusive,
          PullNone,
          0,
          0,
          IoRestrictionOutputOnly,
          "\\_SB.GPIO"
        ) {138}                      // NFC_DEV_WAKE = AGPIO138
      })

      Return (RBUF)
    }

    Method (_STA, 0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (NFCS, 1))) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    }
  } // End of Device (NFC1)
} // End of Scope (\_SB.I2CA)

Scope (\_SB.I2CB) {
  Device (NFC1) {
    Name (_HID, EISAID("NXP8013"))
    Name (_UID, 2)

    Method (_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x29,                     // SlaveAddress: 0x1FA
          ControllerInitiated,      // SlaveMode: ControllerInitiated
          100000,                   // ConnectionSpeed: in Hz (100 KHz)
          AddressingMode7Bit,       // AddressingMode: default to 7bit
          "\\_SB.I2CB",             // ResourceSource: I2C Controller name
          ,                         // ResourceSourceIndex
          ,                         // ResourceUsage
          ,                         // DescriptorName
        )                           // VendorData

        GpioInt (
          Level,
          ActiveHigh,
          ExclusiveAndWake,
          PullNone,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {69}                      // NFC_IRQ = AGPIO69

// Don't change the sequence of these two GPIO pin, REQ must be the first GpioIo pin. NXP Device Driver will treat the first one as REQ.
        GpioIo (
          Exclusive,
          PullNone,
          0,
          0,
          IoRestrictionOutputOnly,
          "\\_SB.GPIO"
        ) {137}                      // NFC_REQ = AGPIO137

        GpioIo (
          Exclusive,
          PullNone,
          0,
          0,
          IoRestrictionOutputOnly,
          "\\_SB.GPIO"
        ) {138}                      // NFC_DEV_WAKE = AGPIO138
      })

      Return (RBUF)
    }

    Method (_STA, 0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (NFCS, 2))) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    }
  } // End of Device (NFC1)
} // End of Scope (\_SB.I2CB)


Scope (\_SB.I2CC) {
  Device (NFC1) {
    Name (_HID, EISAID("NXP8013"))
    Name (_UID, 3)

    Method (_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x29,                     // SlaveAddress: 0x1FA
          ControllerInitiated,      // SlaveMode: ControllerInitiated
          100000,                   // ConnectionSpeed: in Hz (100 KHz)
          AddressingMode7Bit,       // AddressingMode: default to 7bit
          "\\_SB.I2CC",             // ResourceSource: I2C Controller name
          ,                         // ResourceSourceIndex
          ,                         // ResourceUsage
          ,                         // DescriptorName
        )                           // VendorData

        GpioInt (
          Level,
          ActiveHigh,
          ExclusiveAndWake,
          PullNone,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {69}                      // NFC_IRQ = AGPIO69

// Don't change the sequence of these two GPIO pin, REQ must be the first GpioIo pin. NXP Device Driver will treat the first one as REQ.
        GpioIo (
          Exclusive,
          PullNone,
          0,
          0,
          IoRestrictionOutputOnly,
          "\\_SB.GPIO"
        ) {137}                      // NFC_REQ = AGPIO137

        GpioIo (
          Exclusive,
          PullNone,
          0,
          0,
          IoRestrictionOutputOnly,
          "\\_SB.GPIO"
        ) {138}                      // NFC_DEV_WAKE = AGPIO138
      })

      Return (RBUF)
    }

    Method (_STA, 0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (NFCS, 3))) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    }
  } // End of Device (NFC1)
} // End of Scope (\_SB.I2CC)

Scope (\_SB.I2CD) {
  Device (NFC1) {
    Name (_HID, EISAID("NXP8013"))
    Name (_UID, 4)

    Method (_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        I2CSerialBus (
          0x29,                     // SlaveAddress: 0x1FA
          ControllerInitiated,      // SlaveMode: ControllerInitiated
          100000,                   // ConnectionSpeed: in Hz (100 KHz)
          AddressingMode7Bit,       // AddressingMode: default to 7bit
          "\\_SB.I2CD",             // ResourceSource: I2C Controller name
          ,                         // ResourceSourceIndex
          ,                         // ResourceUsage
          ,                         // DescriptorName
        )                           // VendorData

        GpioInt (
          Level,
          ActiveHigh,
          ExclusiveAndWake,
          PullNone,
          0,
          "\\_SB.GPIO",
          0,
          ResourceConsumer,
          ,
          ) {69}                      // NFC_IRQ = AGPIO69

// Don't change the sequence of these two GPIO pin, REQ must be the first GpioIo pin. NXP Device Driver will treat the first one as REQ.
        GpioIo (
          Exclusive,
          PullNone,
          0,
          0,
          IoRestrictionOutputOnly,
          "\\_SB.GPIO"
        ) {137}                      // NFC_REQ = AGPIO137

        GpioIo (
          Exclusive,
          PullNone,
          0,
          0,
          IoRestrictionOutputOnly,
          "\\_SB.GPIO"
        ) {138}                      // NFC_DEV_WAKE = AGPIO138
      })

      Return (RBUF)
    }

    Method (_STA, 0, NotSerialized) {
      If (And (LGreaterEqual (TPOS, 0x60), LEqual (NFCS, 4))) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    }
  } // End of Device (NFC1)
} // End of Scope (\_SB.I2CD)

