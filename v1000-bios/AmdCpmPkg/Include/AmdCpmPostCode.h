/*****************************************************************************
 *
 * Copyright (C) 2014-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_POST_CODE_H_
#define _AMD_CPM_POST_CODE_H_

/// <PostCodePrefix> 0xB0000C00

#ifdef CPM_TP_BASE_VALUE
#define CpmTpBaseValue                        CPM_TP_BASE_VALUE
#else
#define CpmTpBaseValue                        0xB0000C00
#endif

/// The CPM post code

typedef enum {
  CpmTpErrorNoEnoughSizeInHobBuffer =         0x01,           ///< No free table item or no enough size in Hob Buffer
  CpmTpErrorTableNumberOverflow =             0x02,           ///< Table number is greater than AMD_TABLE_LIST_ITEM_SIZE
  CpmTpErrorMainTablePointerInvalid =         0x03,           ///< Main Table pointer is invalid

  CpmTpAcpiThermalFanPeimDriverBegin =        0x10,           ///< Begin of ACPI Thermal Fan PEIM driver
  CpmTpAcpiThermalFanPeimDriverEnd =          0x11,           ///< End of ACPI Thermal Fan PEIM driver
  CpmTpAcpiThermalFanDxeDriverBegin =         0x12,           ///< Begin of ACPI Thermal Fan DXE driver
  CpmTpAcpiThermalFanDxeDriverEnd =           0x13,           ///< End of ACPI Thermal Fan DXE driver
  CpmTpLoadThermalFanSsdtBegin =              0x14,           ///< Begin of loading thermal fan SSDT table
  CpmTpLoadThermalFanSsdtEnd =                0x15,           ///< End of loading thermal fan SSDT table
  CpmTpAdaptiveS4PeimDriverBegin =            0x16,           ///< Begin of Adaptive S4 PEIM driver
  CpmTpAdaptiveS4PeimDriverEnd =              0x17,           ///< End of Adaptive S4 PEIM driver
  CpmTpAdaptiveS4DxeDriverBegin =             0x18,           ///< Begin of Adaptive S4 DXE driver
  CpmTpAdaptiveS4DxeDriverEnd =               0x19,           ///< End of Adaptive S4 DXE driver
  CpmTpAdaptiveS4DxeDisplayTurnOffBegin =     0x1A,           ///< Begin to turn off display in Adaptive S4 DXE driver
  CpmTpAdaptiveS4DxeDisplayTurnOffEnd =       0x1B,           ///< End to turn off display in Adaptive S4 DXE driver
  CpmTpAdaptiveS4SmmDriverBegin =             0x1C,           ///< Begin of Adaptive S4 SMM driver
  CpmTpAdaptiveS4SmmDriverEnd =               0x1D,           ///< End of Adaptive S4 SMM driver
  CpmTpAdaptiveS4SmmRtcAlarmProgramBegin =    0x1E,           ///< Begin to program RTC alarm register in Adaptive S4 SMM driver
  CpmTpAdaptiveS4SmmRtcAlarmProgramEnd =      0x1F,           ///< End to program RTC alarm register in Adaptive S4 SMM driver
  CpmTpDisplayFeaturePeimDriverBegin =        0x20,           ///< Begin of Display Feature PEIM driver
  CpmTpDisplayFeaturePeimDriverEnd =          0x21,           ///< End of Display Feature PEIM driver
  CpmTpDisplayFeatureDxeDriverBegin =         0x22,           ///< Begin of Display Feature DXE driver
  CpmTpDisplayFeatureDxeDriverEnd =           0x23,           ///< End of Display Feature DXE driver
  CpmTpDisplayFeatureVbiosShadowBegin =       0x24,           ///< Begin of Display Feature DXE VBIOS Shadow
  CpmTpDisplayFeatureVbiosShadowEnd =         0x25,           ///< End of Display Feature DXE VBIOS Shadow
  CpmTpDisplayFeatureDisableIoDecodeBegin =   0x26,           ///< Begin of Display Feature DXE Disable IO Decode of Possible Pcie Gfx
  CpmTpDisplayFeatureDisableIoDecodeEnd =     0x27,           ///< End of Display Feature DXE Disable IO Decode of Possible Pcie Gfx
  CpmTpDisplayFeatureDxeInitLateBegin =       0x28,           ///< Begin of Display Feature DXE Init Late
  CpmTpDisplayFeatureDxeInitLateEnd =         0x29,           ///< End of Display Feature DXE Init Late
  CpmTpDisplayFeatureSmmDriverBegin =         0x2A,           ///< Begin of Display Feature SMM driver
  CpmTpDisplayFeatureSmmDriverEnd =           0x2B,           ///< End of Display Feature SMM driver
  CpmTpEcInitPeimDriverBegin =                0x2C,           ///< Begin of EC Init PEIM driver
  CpmTpEcInitPeimDriverEnd =                  0x2D,           ///< End of EC Init PEIM driver
  CpmTpEcInitDxeDriverBegin =                 0x2E,           ///< Begin of EC Init DXE driver
  CpmTpEcInitDxeDriverEnd =                   0x2F,           ///< End of EC Init DXE driver
  CpmTpGpioInitPeimDriverBegin =              0x30,           ///< Begin of GPIO Init PEIM driver
  CpmTpGpioInitPeimDriverEnd =                0x31,           ///< End of GPIO Init PEIM driver
  CpmTpGpioInitPeimResetDeviceBegin =         0x32,           ///< Begin to Reset Device in GPIO Init PEIM driver
  CpmTpGpioInitPeimResetDeviceEnd =           0x33,           ///< End to Reset Device in GPIO Init PEIM driver
  CpmTpGpioInitPeimSetMemVoltageBegin =       0x34,           ///< Begin to Set Mem Voltage in GPIO Init PEIM driver
  CpmTpGpioInitPeimSetMemVoltageEnd =         0x35,           ///< End to Set Mem Voltage in GPIO Init PEIM driver
  CpmTpGpioInitPeimSetVddpVddrVoltageBegin =  0x36,           ///< Begin to Set Vddp/Vddr Voltage in GPIO Init PEIM driver
  CpmTpGpioInitPeimSetVddpVddrVoltageEnd =    0x37,           ///< End to Set Vddp/Vddr Voltage in GPIO Init PEIM driver
  CpmTpGpioInitDxeDriverBegin =               0x38,           ///< Begin of GPIO Init DXE driver
  CpmTpGpioInitDxeDriverEnd =                 0x39,           ///< Begin of GPIO Init DXE driver
  CpmTpGpioInitDxePcieClockInitBegin =        0x3A,           ///< Begin to init PCIe Clock in GPIO Init DXE driver
  CpmTpGpioInitDxePcieClockInitEnd =          0x3B,           ///< Begin to init PCIe Clock in GPIO Init DXE driver
  CpmTpGpioInitSmmDriverBegin =               0x3C,           ///< Begin of GPIO Init SMM driver
  CpmTpGpioInitSmmDriverEnd =                 0x3D,           ///< Begin of GPIO Init SMM driver
  CpmTpPcieInitPeimDriverBegin =              0x3E,           ///< Begin of PCIE Init PEIM driver
  CpmTpPcieInitPeimDriverEnd =                0x3F,           ///< End of PCIE Init PEIM driver
  CpmTpPcieInitDxeDriverBegin =               0x40,           ///< Begin of PCIE Init DXE driver
  CpmTpPcieInitDxeDriverEnd =                 0x41,           ///< End of PCIE Init DXE driver
  CpmTpProchotPeimDriverBegin =               0x42,           ///< Begin of PROCHOT PEIM driver
  CpmTpProchotPeimDriverEnd =                 0x43,           ///< End of PROCHOT PEIM driver
  CpmTpZeroPowerOddPeimDriverBegin =          0x44,           ///< Begin of Zero Power Odd PEIM driver
  CpmTpZeroPowerOddPeimDriverEnd =            0x45,           ///< End of Zero Power Odd PEIM driver
  CpmTpZeroPowerOddDxeDriverBegin =           0x46,           ///< Begin of Zero Power Odd DXE driver
  CpmTpZeroPowerOddDxeDriverEnd =             0x47,           ///< End of Zero Power Odd DXE drive
  CpmTpUcsiPeimDriverBegin =                  0x48,           ///< Begin of UCSI PEIM driver
  CpmTpUcsiPeimDriverEnd =                    0x49,           ///< End of UCSI PEIM drive
  CpmTpUcsiDxeDriverBegin =                   0x4A,           ///< Begin of UCSI DXE driver
  CpmTpUcsiDxeDriverEnd =                     0x4B,           ///< End of UCSI DXE drive
  CpmTpOemInitDxeDriverBegin =                0x4C,           ///< Begin of OEM Init DXE driver
  CpmTpOemInitDxeDriverEnd =                  0x4D,           ///< End of OEM Init DXE drive
  CpmTpSensorFusionDxeDriverBegin =           0x4E,           ///< Begin of SensorFusion DXE driver
  CpmTpSensorFusionDxeDriverEnd =             0x4F,           ///< End of SensorFusion DXE drive
  CpmTpAcpPowerGatingDxeDriverBegin =         0x50,           ///< Begin of AcpPowerGating DXE driver
  CpmTpAcpPowerGatingDxeDriverEnd =           0x51,           ///< End of AcpPowerGating DXE drive

} AMD_CPM_POST_CODE;

#endif
