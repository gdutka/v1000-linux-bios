
Scope (\_SB.I2CA) {
		// Device CM32181
		Device(CMSR)
		{
			Name(_ADR, 0x0)
			Name(_HID, "CPLM3218")
			Name(_CID, "CPLM3218")
			Name(_UID, 0x1)
			Name(_DEP, Package(0x1)
			{
				\_SB.I2CA
			})				
			Method(_STA, 0x0, NotSerialized)
			{
        If (And (LGreaterEqual (TPOS, 0x60), LEqual (ALST, 1))) {
				  Return(0x0f)
        }
        Return (0)
			}
			Method(_CRS, 0x0, NotSerialized)
			{
				Name(SBUF,ResourceTemplate () {
					I2CSerialBus(0x10,          //SlaveAddress: bus address
						,                       //SlaveMode: default to ControllerInitiated
						400000,                 //ConnectionSpeed: in Hz
						,						//Addressing Mode: default to 7 bit
						"\\_SB.I2CA",           //ResourceSource: I2C bus controller name
						,                       //Descriptor Name: creates name for offset of resource descriptor
						)  //VendorData
          GpioInt(Level, ActiveLow, ExclusiveAndWake, PullUp, 0, "\\_SB.GPIO", 0, ResourceConsumer,,) {24}          
				})
				Return (SBUF)
			}
			
			// Initial setting
			Name(CPM0, Package(0x7)
			{
				32181,0x1,0xF,0x0840,0x013B,0x011D,0x0
			})

			// Calibration data
			Name(CPM1, Package(0x3)
			{
				2200,100000,100000
			})

			// Sensitivity factor
			Name(CPM2, Package(0x6)
			{
				25000,100000,100000,100000,200000,1600000
			})					
			
			// ALR curve
			Name(CPM3, Package()
			{
				Package() {60, 10},
				Package() {100,11},
				Package() {100,8000},
				Package() {500,8001},
			})

			// Default property
			Name(CPM5, Package(0x7)
			{
				0,150000,400,1000,2,50,5
			})					
			
			Method(_DSM, 0x4, NotSerialized)
			{
				If(LEqual(Arg0, Buffer(0x10)
				{
					0xB6, 0xC6, 0x03, 0x07, 0xCA, 0x1C, 0x44, 0x41, 0x9F, 0xE7, 0x46, 0x54,	0xF5, 0x3A, 0x0B, 0xD9
				}))
				{
					If(LEqual(Arg2, 0x0))
					{
						Return(Buffer(0x1)
						{
							0x03
						})
					}
					If(LEqual(Arg2, 0x1))
					{
						If(LEqual(ToInteger(Arg3, ), 0x0))
						{												
							Return(CPM0)
						}
						If(LEqual(ToInteger(Arg3, ), 0x1))
						{
							Return(CPM1)
						}
						If(LEqual(ToInteger(Arg3, ), 0x2))
						{
							Return(CPM2)
						}
						If(LEqual(ToInteger(Arg3, ), 0x3))
						{
							Return(CPM3)
						}
						If(LEqual(ToInteger(Arg3, ), 0x5))
						{
							Return(CPM5)
						}							
					}						
				}
				Return(Buffer(One)
				{
					0x0
				})
			}
		}// Device CM32181
} //End of Scope (\_SB.I2CA)

Scope (\_SB.I2CB) {
		// Device CM32181
		Device(CMSR)
		{
			Name(_ADR, 0x0)
			Name(_HID, "CPLM3218")
			Name(_CID, "CPLM3218")
			Name(_UID, 0x1)
			Name(_DEP, Package(0x1)
			{
				\_SB.I2CB
			})				
			Method(_STA, 0x0, NotSerialized)
			{
        If (And (LGreaterEqual (TPOS, 0x60), LEqual (ALST, 2))) {
				  Return(0x0f)
        }
        Return (0)
			}
			Method(_CRS, 0x0, NotSerialized)
			{
				Name(SBUF,ResourceTemplate () {
					I2CSerialBus(0x10,          //SlaveAddress: bus address
						,                       //SlaveMode: default to ControllerInitiated
						400000,                 //ConnectionSpeed: in Hz
						,						//Addressing Mode: default to 7 bit
						"\\_SB.I2CB",           //ResourceSource: I2C bus controller name
						,                       //Descriptor Name: creates name for offset of resource descriptor
						)  //VendorData
          GpioInt(Level, ActiveLow, ExclusiveAndWake, PullUp, 0, "\\_SB.GPIO", 0, ResourceConsumer,,) {24}          
				})
				Return (SBUF)
			}
			
			// Initial setting
			Name(CPM0, Package(0x7)
			{
				32181,0x1,0xF,0x0840,0x013B,0x011D,0x0
			})

			// Calibration data
			Name(CPM1, Package(0x3)
			{
				2200,100000,100000
			})

			// Sensitivity factor
			Name(CPM2, Package(0x6)
			{
				25000,100000,100000,100000,200000,1600000
			})					
			
			// ALR curve
			Name(CPM3, Package()
			{
				Package() {60, 10},
				Package() {100,11},
				Package() {100,8000},
				Package() {500,8001},
			})

			// Default property
			Name(CPM5, Package(0x7)
			{
				0,150000,400,1000,2,50,5
			})					
			
			Method(_DSM, 0x4, NotSerialized)
			{
				If(LEqual(Arg0, Buffer(0x10)
				{
					0xB6, 0xC6, 0x03, 0x07, 0xCA, 0x1C, 0x44, 0x41, 0x9F, 0xE7, 0x46, 0x54,	0xF5, 0x3A, 0x0B, 0xD9
				}))
				{
					If(LEqual(Arg2, 0x0))
					{
						Return(Buffer(0x1)
						{
							0x03
						})
					}
					If(LEqual(Arg2, 0x1))
					{
						If(LEqual(ToInteger(Arg3, ), 0x0))
						{												
							Return(CPM0)
						}
						If(LEqual(ToInteger(Arg3, ), 0x1))
						{
							Return(CPM1)
						}
						If(LEqual(ToInteger(Arg3, ), 0x2))
						{
							Return(CPM2)
						}
						If(LEqual(ToInteger(Arg3, ), 0x3))
						{
							Return(CPM3)
						}
						If(LEqual(ToInteger(Arg3, ), 0x5))
						{
							Return(CPM5)
						}							
					}						
				}
				Return(Buffer(One)
				{
					0x0
				})
			}
		}// Device CM32181
} //End of Scope (\_SB.I2CB)

Scope (\_SB.I2CC) {
		// Device CM32181
		Device(CMSR)
		{
			Name(_ADR, 0x0)
			Name(_HID, "CPLM3218")
			Name(_CID, "CPLM3218")
			Name(_UID, 0x1)
			Name(_DEP, Package(0x1)
			{
				\_SB.I2CC
			})				
			Method(_STA, 0x0, NotSerialized)
			{
        If (And (LGreaterEqual (TPOS, 0x60), LEqual (ALST, 3))) {
				  Return(0x0f)
        }
        Return (0)
			}
			Method(_CRS, 0x0, NotSerialized)
			{
				Name(SBUF,ResourceTemplate () {
					I2CSerialBus(0x10,          //SlaveAddress: bus address
						,                       //SlaveMode: default to ControllerInitiated
						400000,                 //ConnectionSpeed: in Hz
						,						//Addressing Mode: default to 7 bit
						"\\_SB.I2CC",           //ResourceSource: I2C bus controller name
						,                       //Descriptor Name: creates name for offset of resource descriptor
						)  //VendorData
          GpioInt(Level, ActiveLow, ExclusiveAndWake, PullUp, 0, "\\_SB.GPIO", 0, ResourceConsumer,,) {24}          
				})
				Return (SBUF)
			}
			
			// Initial setting
			Name(CPM0, Package(0x7)
			{
				32181,0x1,0xF,0x0840,0x013B,0x011D,0x0
			})

			// Calibration data
			Name(CPM1, Package(0x3)
			{
				2200,100000,100000
			})

			// Sensitivity factor
			Name(CPM2, Package(0x6)
			{
				25000,100000,100000,100000,200000,1600000
			})					
			
			// ALR curve
			Name(CPM3, Package()
			{
				Package() {60, 10},
				Package() {100,11},
				Package() {100,8000},
				Package() {500,8001},
			})

			// Default property
			Name(CPM5, Package(0x7)
			{
				0,150000,400,1000,2,50,5
			})					
			
			Method(_DSM, 0x4, NotSerialized)
			{
				If(LEqual(Arg0, Buffer(0x10)
				{
					0xB6, 0xC6, 0x03, 0x07, 0xCA, 0x1C, 0x44, 0x41, 0x9F, 0xE7, 0x46, 0x54,	0xF5, 0x3A, 0x0B, 0xD9
				}))
				{
					If(LEqual(Arg2, 0x0))
					{
						Return(Buffer(0x1)
						{
							0x03
						})
					}
					If(LEqual(Arg2, 0x1))
					{
						If(LEqual(ToInteger(Arg3, ), 0x0))
						{												
							Return(CPM0)
						}
						If(LEqual(ToInteger(Arg3, ), 0x1))
						{
							Return(CPM1)
						}
						If(LEqual(ToInteger(Arg3, ), 0x2))
						{
							Return(CPM2)
						}
						If(LEqual(ToInteger(Arg3, ), 0x3))
						{
							Return(CPM3)
						}
						If(LEqual(ToInteger(Arg3, ), 0x5))
						{
							Return(CPM5)
						}							
					}						
				}
				Return(Buffer(One)
				{
					0x0
				})
			}
		}// Device CM32181
} //End of Scope (\_SB.I2CC)

Scope (\_SB.I2CD) {
		// Device CM32181
		Device(CMSR)
		{
			Name(_ADR, 0x0)
			Name(_HID, "CPLM3218")
			Name(_CID, "CPLM3218")
			Name(_UID, 0x1)
			Name(_DEP, Package(0x1)
			{
				\_SB.I2CD
			})				
			Method(_STA, 0x0, NotSerialized)
			{
        If (And (LGreaterEqual (TPOS, 0x60), LEqual (ALST, 4))) {
				  Return(0x0f)
        }
        Return (0)
			}
			Method(_CRS, 0x0, NotSerialized)
			{
				Name(SBUF,ResourceTemplate () {
					I2CSerialBus(0x10,          //SlaveAddress: bus address
						,                       //SlaveMode: default to ControllerInitiated
						400000,                 //ConnectionSpeed: in Hz
						,						//Addressing Mode: default to 7 bit
						"\\_SB.I2CD",           //ResourceSource: I2C bus controller name
						,                       //Descriptor Name: creates name for offset of resource descriptor
						)  //VendorData
          GpioInt(Level, ActiveLow, ExclusiveAndWake, PullUp, 0, "\\_SB.GPIO", 0, ResourceConsumer,,) {24}          
				})
				Return (SBUF)
			}
			
			// Initial setting
			Name(CPM0, Package(0x7)
			{
				32181,0x1,0xF,0x0840,0x013B,0x011D,0x0
			})

			// Calibration data
			Name(CPM1, Package(0x3)
			{
				2200,100000,100000
			})

			// Sensitivity factor
			Name(CPM2, Package(0x6)
			{
				25000,100000,100000,100000,200000,1600000
			})					
			
			// ALR curve
			Name(CPM3, Package()
			{
				Package() {60, 10},
				Package() {100,11},
				Package() {100,8000},
				Package() {500,8001},
			})

			// Default property
			Name(CPM5, Package(0x7)
			{
				0,150000,400,1000,2,50,5
			})					
			
			Method(_DSM, 0x4, NotSerialized)
			{
				If(LEqual(Arg0, Buffer(0x10)
				{
					0xB6, 0xC6, 0x03, 0x07, 0xCA, 0x1C, 0x44, 0x41, 0x9F, 0xE7, 0x46, 0x54,	0xF5, 0x3A, 0x0B, 0xD9
				}))
				{
					If(LEqual(Arg2, 0x0))
					{
						Return(Buffer(0x1)
						{
							0x03
						})
					}
					If(LEqual(Arg2, 0x1))
					{
						If(LEqual(ToInteger(Arg3, ), 0x0))
						{												
							Return(CPM0)
						}
						If(LEqual(ToInteger(Arg3, ), 0x1))
						{
							Return(CPM1)
						}
						If(LEqual(ToInteger(Arg3, ), 0x2))
						{
							Return(CPM2)
						}
						If(LEqual(ToInteger(Arg3, ), 0x3))
						{
							Return(CPM3)
						}
						If(LEqual(ToInteger(Arg3, ), 0x5))
						{
							Return(CPM5)
						}							
					}						
				}
				Return(Buffer(One)
				{
					0x0
				})
			}
		}// Device CM32181
} //End of Scope (\_SB.I2CD)