/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_DEFINE_H_
#define _AMD_CPM_DEFINE_H_

#define AMD_CPM_PLATFORM_ID_AUTO                  0xFFFF
#ifndef AMD_CPM_PLATFORM_ID_DEFAULT
  #define AMD_CPM_PLATFORM_ID_DEFAULT             AMD_CPM_PLATFORM_ID_AUTO
#endif

#ifndef CPM_PLATFORM_ID_OVERRIDE
#define CPM_PLATFORM_ID_OVERRIDE                  0xFFFF
#endif

#ifndef AMD_PLATFORM_ID_TABLE_SIZE
  #define AMD_PLATFORM_ID_TABLE_SIZE              0x08
#endif

#ifndef AMD_PLATFORM_ID_CONVERT_TABLE_SIZE
  #define AMD_PLATFORM_ID_CONVERT_TABLE_SIZE      0x40
#endif

#ifndef AMD_PRE_INIT_SIZE
  #define AMD_PRE_INIT_SIZE                       0x40
#endif

#ifndef AMD_CPM_CORE_TOPOLOGY_SIZE
  #define AMD_CPM_CORE_TOPOLOGY_SIZE              0x10
#endif

#ifndef UCSI_PORT_NUMBER
  #define UCSI_PORT_NUMBER                        0x2
#endif

#ifndef AMD_GPIO_ITEM_SIZE
  #define AMD_GPIO_ITEM_SIZE                      0x100
#endif

#ifndef AMD_GEVENT_ITEM_SIZE
  #define AMD_GEVENT_ITEM_SIZE                    0x20
#endif

#ifndef AMD_GPIO_MEM_VOLTAGE_SIZE
  #define AMD_GPIO_MEM_VOLTAGE_SIZE               0x05
#endif

#ifndef AMD_GPIO_VDDP_VDDR_VOLTAGE_SIZE
  #define AMD_GPIO_VDDP_VDDR_VOLTAGE_SIZE         0x03
#endif

#ifndef AMD_GPIO_DEVICE_SIZE
  #define AMD_GPIO_DEVICE_SIZE                    0x64
#endif

#ifndef AMD_GPIO_DEVICE_DETECT_SIZE
  #define AMD_GPIO_DEVICE_DETECT_SIZE             0x20
#endif

#ifndef AMD_GPIO_DEVICE_RESET_SIZE
  #define AMD_GPIO_DEVICE_RESET_SIZE              0x20
#endif

#ifndef AMD_GPIO_DEVICE_POWER_SIZE
  #define AMD_GPIO_DEVICE_POWER_SIZE              0x30
#endif

#ifndef AMD_PT_GPIO_DEVICE_INIT_SIZE
  #define AMD_PT_GPIO_DEVICE_INIT_SIZE            0x10
#endif

#ifndef AMD_PCIE_CLOCK_SIZE
  #define AMD_PCIE_CLOCK_SIZE                     0x10
#endif

#ifndef AMD_EXT_CLKGEN_SIZE
  #define AMD_EXT_CLKGEN_SIZE                     0x20
#endif

#ifndef AMD_PCIE_PORT_DESCRIPTOR_SIZE
  #define AMD_PCIE_PORT_DESCRIPTOR_SIZE           0x08
#endif

#ifndef AMD_PCIE_DDI_DESCRIPTOR_SIZE
  #define AMD_PCIE_DDI_DESCRIPTOR_SIZE            0x08
#endif

#ifndef AMD_DXIO_DDI_DESCRIPTOR_SIZE
  #define AMD_DXIO_DDI_DESCRIPTOR_SIZE            0x04
#endif

#ifndef AMD_DXIO_PORT_DESCRIPTOR_SIZE
  #define AMD_DXIO_PORT_DESCRIPTOR_SIZE           0x10
#endif

#ifndef AMD_PCIE_TOPOLOGY_OVERRIDE_SIZE
  #define AMD_PCIE_TOPOLOGY_OVERRIDE_SIZE         0x10
#endif

#ifndef AMD_TABLE_LIST_ITEM_SIZE
  #define AMD_TABLE_LIST_ITEM_SIZE                0x40
#endif

#ifndef AMD_TABLE_LIST_ADDITIONAL_ITEM_SIZE
  #define AMD_TABLE_LIST_ADDITIONAL_ITEM_SIZE     0x10
#endif

#ifndef AMD_TABLE_ADDITIONAL_BUFFER_SIZE
  #define AMD_TABLE_ADDITIONAL_BUFFER_SIZE        0x400
#endif

#ifndef DEFAULT_BRIGHTNESS_LEVEL
  #define DEFAULT_BRIGHTNESS_LEVEL                60
#endif

#ifndef DEFAULT_CURRENT_DISPLAY_STATE
  #define DEFAULT_CURRENT_DISPLAY_STATE           0x0101          // (PIPE_A_CRT | (PIPE_B_CRT))
#endif

#ifndef AMD_CPM_ASL_CPU0
  #define AMD_CPM_ASL_CPU0                        0               // If 0, AMD_CPM_CPM_ASL_CPU0 = C000
#endif

#ifndef AMD_CPM_ASL_EC_NAME
  #define AMD_CPM_ASL_EC_NAME                     0               // If 0, AMD_CPM_ASL_EC_NAME = EC0_
#endif

#ifndef AMD_CPM_ASL_EC_MUTEX
  #define AMD_CPM_ASL_EC_MUTEX                    0               // If 0, AMD_CPM_ASL_EC_MUTEX = M042
#endif

#ifndef AMD_CPM_ASL_LPC_NAME
  #define AMD_CPM_ASL_LPC_NAME                    0               // If 0, AMD_CPM_ASL_LPC_NAME = LPC0
#endif

#ifndef AMD_CPM_ASL_OKEC_NAME
  #define AMD_CPM_ASL_OKEC_NAME                   0               // If 0, AMD_CPM_ASL_OKEC_NAME = OKEC
#endif

#ifndef AMD_CPM_ASL_OEM_CALLBACK
  #define AMD_CPM_ASL_OEM_CALLBACK                0               // If 0, AMD_CPM_ASL_OEM_CALLBACK = MOEM
#endif

#ifndef AMD_CPM_ASL_OEM_MLIB
  #define AMD_CPM_ASL_OEM_MLIB                    0               // If 0, AMD_CPM_ASL_OEM_MLIB = MLIB
#endif

#ifndef AMD_CPM_ASL_ACP_NAME
  #define AMD_CPM_ASL_ACP_NAME                    0               // If 0, AMD_CPM_ASL_ACP_NAME = ACP_
#endif

#ifndef AMD_CPM_ASL_AZALIA_NAME
  #define AMD_CPM_ASL_AZALIA_NAME                 0               // If 0, AMD_CPM_ASL_ACP_NAME = AZAL
#endif

#ifndef AMD_CPM_ASL_LAN_NAME
  #define AMD_CPM_ASL_LAN_NAME                    0               // If 0, AMD_CPM_ASL_LAN_NAME = BCM5
#endif

#ifndef AMD_CPM_ACPI_VERSION_SUPPORT
  #define AMD_CPM_ACPI_VERSION_SUPPORT            0x0000003Eul    // BIT0: EFI_ACPI_TABLE_VERSION_NONE
                                                                  // BIT1: EFI_ACPI_TABLE_VERSION_1_0B
                                                                  // BIT2: EFI_ACPI_TABLE_VERSION_2_0
                                                                  // BIT3: EFI_ACPI_TABLE_VERSION_3_0
                                                                  // BIT4: EFI_ACPI_TABLE_VERSION_4_0
                                                                  // BIT5: EFI_ACPI_TABLE_VERSION_5_0
#endif

#ifndef AMD_CPM_SSDT_SUPPORT
  #define AMD_CPM_SSDT_SUPPORT                    1               // If 0, SSDT table will not be loaded
#endif

#ifndef AMD_CPM_PCIE_GFX0_NAME_1
  #define AMD_CPM_PCIE_GFX0_NAME_1                SIGNATURE_32 ('P', 'B', '2', '_')          // PB2_
#endif
#ifndef AMD_CPM_PCIE_GFX1_NAME_1
  #define AMD_CPM_PCIE_GFX1_NAME_1                SIGNATURE_32 ('P', 'B', '3', '_')          // PB3_
#endif
#ifndef AMD_CPM_PCIE_GPP0_NAME_1
  #define AMD_CPM_PCIE_GPP0_NAME_1                SIGNATURE_32 ('P', 'B', '4', '_')          // PB4_
#endif
#ifndef AMD_CPM_PCIE_GPP1_NAME_1
  #define AMD_CPM_PCIE_GPP1_NAME_1                SIGNATURE_32 ('P', 'B', '5', '_')          // PB5_
#endif
#ifndef AMD_CPM_PCIE_GPP2_NAME_1
  #define AMD_CPM_PCIE_GPP2_NAME_1                SIGNATURE_32 ('P', 'B', '6', '_')          // PB6_
#endif
#ifndef AMD_CPM_PCIE_GPP3_NAME_1
  #define AMD_CPM_PCIE_GPP3_NAME_1                SIGNATURE_32 ('P', 'B', '7', '_')          // PB7_
#endif

#ifndef AMD_CPM_PCIE_GFX0_NAME_2
  #define AMD_CPM_PCIE_GFX0_NAME_2                SIGNATURE_32 ('P', 'B', '2', '1')          // PB21
#endif
#ifndef AMD_CPM_PCIE_GFX1_NAME_2
  #define AMD_CPM_PCIE_GFX1_NAME_2                SIGNATURE_32 ('P', 'B', '2', '2')          // PB22
#endif
#ifndef AMD_CPM_PCIE_GPP0_NAME_2
  #define AMD_CPM_PCIE_GPP0_NAME_2                SIGNATURE_32 ('P', 'B', '3', '1')          // PB31
#endif
#ifndef AMD_CPM_PCIE_GPP1_NAME_2
  #define AMD_CPM_PCIE_GPP1_NAME_2                SIGNATURE_32 ('P', 'B', '3', '2')          // PB32
#endif
#ifndef AMD_CPM_PCIE_GPP2_NAME_2
  #define AMD_CPM_PCIE_GPP2_NAME_2                SIGNATURE_32 ('P', 'B', '3', '3')          // PB33
#endif
#ifndef AMD_CPM_PCIE_GPP3_NAME_2
  #define AMD_CPM_PCIE_GPP3_NAME_2                SIGNATURE_32 ('P', 'B', '3', '4')          // PB34
#endif
#ifndef AMD_CPM_PCIE_GPP4_NAME_2
  #define AMD_CPM_PCIE_GPP4_NAME_2                SIGNATURE_32 ('P', 'B', '3', '5')          // PB35
#endif

#ifndef AMD_CPM_PCIE_GPP0_NAME_3
  #define AMD_CPM_PCIE_GPP0_NAME_3                SIGNATURE_32 ('G', 'P', 'P', '0')          // GPP0
#endif
#ifndef AMD_CPM_PCIE_GPP1_NAME_3
  #define AMD_CPM_PCIE_GPP1_NAME_3                SIGNATURE_32 ('G', 'P', 'P', '1')          // GPP1
#endif
#ifndef AMD_CPM_PCIE_GPP2_NAME_3
  #define AMD_CPM_PCIE_GPP2_NAME_3                SIGNATURE_32 ('G', 'P', 'P', '2')          // GPP2
#endif
#ifndef AMD_CPM_PCIE_GPP3_NAME_3
  #define AMD_CPM_PCIE_GPP3_NAME_3                SIGNATURE_32 ('G', 'P', 'P', '3')          // GPP3
#endif
#ifndef AMD_CPM_PCIE_GFX_NAME_3
  #define AMD_CPM_PCIE_GFX_NAME_3                 SIGNATURE_32 ('G', 'F', 'X', '_')          // GFX_
#endif

#ifndef AMD_CPM_PCIE_GPP0_NAME_4
  #define AMD_CPM_PCIE_GPP0_NAME_4                SIGNATURE_32 ('G', 'P', 'P', '0')          // GPP0
#endif
#ifndef AMD_CPM_PCIE_GPP1_NAME_4
  #define AMD_CPM_PCIE_GPP1_NAME_4                SIGNATURE_32 ('G', 'P', 'P', '1')          // GPP1
#endif
#ifndef AMD_CPM_PCIE_GPP2_NAME_4
  #define AMD_CPM_PCIE_GPP2_NAME_4                SIGNATURE_32 ('G', 'P', 'P', '2')          // GPP2
#endif
#ifndef AMD_CPM_PCIE_GPP3_NAME_4
  #define AMD_CPM_PCIE_GPP3_NAME_4                SIGNATURE_32 ('G', 'P', 'P', '3')          // GPP3
#endif
#ifndef AMD_CPM_PCIE_GPP4_NAME_4
  #define AMD_CPM_PCIE_GPP4_NAME_4                SIGNATURE_32 ('G', 'P', 'P', '4')          // GPP4
#endif
#ifndef AMD_CPM_PCIE_GPP5_NAME_4
  #define AMD_CPM_PCIE_GPP5_NAME_4                SIGNATURE_32 ('G', 'P', 'P', '5')          // GPP5
#endif
#ifndef AMD_CPM_PCIE_GPP6_NAME_4
  #define AMD_CPM_PCIE_GPP6_NAME_4                SIGNATURE_32 ('G', 'P', 'P', '6')          // GPP6
#endif
#ifndef AMD_CPM_PCIE_GPP7_NAME_4
  #define AMD_CPM_PCIE_GPP7_NAME_4                SIGNATURE_32 ('G', 'P', 'P', '7')          // GPP7
#endif
#ifndef AMD_CPM_PCIE_GPP8_NAME_4
  #define AMD_CPM_PCIE_GPP8_NAME_4                SIGNATURE_32 ('G', 'P', 'P', '8')          // GPP8
#endif
#ifndef AMD_CPM_PCIE_GPP9_NAME_4
  #define AMD_CPM_PCIE_GPP9_NAME_4                SIGNATURE_32 ('G', 'P', 'P', '9')          // GPP9
#endif
#ifndef AMD_CPM_PCIE_GPPA_NAME_4
  #define AMD_CPM_PCIE_GPPA_NAME_4                SIGNATURE_32 ('G', 'P', 'P', 'A')          // GPPA
#endif
#ifndef AMD_CPM_PCIE_GPPB_NAME_4
  #define AMD_CPM_PCIE_GPPB_NAME_4                SIGNATURE_32 ('G', 'P', 'P', 'B')          // GPPB
#endif
#ifndef AMD_CPM_PCIE_GPPC_NAME_4
  #define AMD_CPM_PCIE_GPPC_NAME_4                SIGNATURE_32 ('G', 'P', 'P', 'C')          // GPPC
#endif
#ifndef AMD_CPM_PCIE_GPPD_NAME_4
  #define AMD_CPM_PCIE_GPPD_NAME_4                SIGNATURE_32 ('G', 'P', 'P', 'D')          // GPPD
#endif
#ifndef AMD_CPM_PCIE_GPPE_NAME_4
  #define AMD_CPM_PCIE_GPPE_NAME_4                SIGNATURE_32 ('G', 'P', 'P', 'E')          // GPPE
#endif
#ifndef AMD_CPM_PCIE_GPPF_NAME_4
  #define AMD_CPM_PCIE_GPPF_NAME_4                SIGNATURE_32 ('G', 'P', 'P', 'F')          // GPPF
#endif
#ifndef AMD_CPM_PCIE_GPPG_NAME_4
  #define AMD_CPM_PCIE_GPPG_NAME_4                SIGNATURE_32 ('G', 'P', 'P', 'G')          // GPPG
#endif
#ifndef AMD_CPM_PCIE_GPPH_NAME_4
  #define AMD_CPM_PCIE_GPPH_NAME_4                SIGNATURE_32 ('G', 'P', 'P', 'H')          // GPPH
#endif

#ifndef AMD_CPM_PCIE_GFX0_NAME_4
  #define AMD_CPM_PCIE_GFX0_NAME_4                SIGNATURE_32 ('G', 'F', 'X', '0')          // GFX0
#endif
#ifndef AMD_CPM_PCIE_GFX1_NAME_4
  #define AMD_CPM_PCIE_GFX1_NAME_4                SIGNATURE_32 ('G', 'F', 'X', '1')          // GFX1
#endif
#ifndef AMD_CPM_PCIE_GFX2_NAME_4
  #define AMD_CPM_PCIE_GFX2_NAME_4                SIGNATURE_32 ('G', 'F', 'X', '2')          // GFX2
#endif
#ifndef AMD_CPM_PCIE_GFX3_NAME_4
  #define AMD_CPM_PCIE_GFX3_NAME_4                SIGNATURE_32 ('G', 'F', 'X', '3')          // GFX3
#endif
#ifndef AMD_CPM_PCIE_GFX4_NAME_4
  #define AMD_CPM_PCIE_GFX4_NAME_4                SIGNATURE_32 ('G', 'F', 'X', '4')          // GFX4
#endif

#ifndef AMD_CPM_PCIE_GP17_NAME_4
  #define AMD_CPM_PCIE_GP17_NAME_4                SIGNATURE_32 ('G', 'P', '1', '7')          // GP17
#endif
#ifndef AMD_CPM_PCIE_GP18_NAME_4
  #define AMD_CPM_PCIE_GP18_NAME_4                SIGNATURE_32 ('G', 'P', '1', '8')          // GP18
#endif
#ifndef AMD_CPM_PCIE_GP28_NAME_4
  #define AMD_CPM_PCIE_GP28_NAME_4                SIGNATURE_32 ('G', 'P', '2', '8')          // GP28
#endif
#ifndef AMD_CPM_PCIE_GP38_NAME_4
  #define AMD_CPM_PCIE_GP38_NAME_4                SIGNATURE_32 ('G', 'P', '3', '8')          // GP38
#endif

#ifndef AMD_CPM_FCH_PCIE_GPP0_NAME
  #define AMD_CPM_FCH_PCIE_GPP0_NAME              SIGNATURE_32 ('S', 'P', 'B', '0')          // SPB0
#endif
#ifndef AMD_CPM_FCH_PCIE_GPP1_NAME
  #define AMD_CPM_FCH_PCIE_GPP1_NAME              SIGNATURE_32 ('S', 'P', 'B', '1')          // SPB1
#endif
#ifndef AMD_CPM_FCH_PCIE_GPP2_NAME
  #define AMD_CPM_FCH_PCIE_GPP2_NAME              SIGNATURE_32 ('S', 'P', 'B', '2')          // SPB2
#endif
#ifndef AMD_CPM_FCH_PCIE_GPP3_NAME
  #define AMD_CPM_FCH_PCIE_GPP3_NAME              SIGNATURE_32 ('S', 'P', 'B', '3')          // SPB3
#endif

//
// OFFSET_OF DEFINITION
//
#ifndef OFFSET_OF
  #define OFFSET_OF(type, member) ((UINT8) &(((type*)0)->member))
#endif

//
// GPIO DEFINITION
//
#define GPIO_DEFINITION(gpio, function, output, pullup, sticky) \
{gpio, {(function <= 3 ? 0x800 : 0) + (function << 8) + (output < 3 ? (output < 2 ? output : 0x02) : 0x08) + \
((sticky == 1) || (pullup < 4) ? ((sticky == 1 ? 0x10 : 0) + (pullup == 0 ? 0x40 : 0) + \
(pullup == 1 ? 0x20 : 0) + (pullup == 3 ? 0x24 : 0) + 0x80) : 0)}}

//
// Multi Socket/Die GPIO Definition
//
#define GPIO_DEF_V2(socket, die, gpio, function, output, pullup) \
{(socket << 15) + (die << 13) + (0 << 11) + gpio, (function <= 3 ? 0x800 : 0) + (function << 8) + \
(output < 3 ? (output < 2 ? output : 0x02) : 0x08) + ((pullup == 0 ? 0x40 : 0) + (pullup == 1 ? 0x20 : 0) + (pullup == 3 ? 0x24 : 0) + 0x80)}

#define GPIO_DEF_V3(socket, die, bridge, gpio, function, output, pullup) \
{(socket << 15) + (die << 13) + (bridge << 11) + gpio, (function <= 3 ? 0x800 : 0) + (function << 8) + \
(output < 3 ? (output < 2 ? output : 0x02) : 0x08) + ((pullup == 0 ? 0x40 : 0) + (pullup == 1 ? 0x20 : 0) + (pullup == 3 ? 0x24 : 0) + 0x80)}

#define EC_GPIO_DEFINITION(gpio, output) \
{gpio, output}

#define EC_GPIO_DEF_V2(gpio, output) \
{(0x200 + gpio), output}

//
// PROMONTORY GPIO DEFINITION
//
#define PT_GPIO_DEFINITION(gpio, OutEnB, Out, intEnb, intLevelTrigType, intType, intMask ) \
{gpio, OutEnB + (Out << 1) + (intEnb << 2) + (intLevelTrigType << 3) + (intType << 4) + (intMask << 6)}

//
// GEVENT DEFINITION
//
#define GEVENT_DEFINITION(gevent, enable, scitrig, scilevel, smisci, scis0, scimap, smitrig, smicontrol) \
{gevent, {enable + (scitrig <= 1 ? scitrig << 1 : 1 << 10) + (scilevel << 2) + (smisci << 3) + (scis0 << 4) + (scimap << 5) + \
(smitrig << 11) + (smicontrol << 12)}}

//
// GPIO INTERRUPT DEFINITION
//
#define GPIO_INTERRUPT_DEFINITION(gpio, enable, leveltrig, activelevel, wakecontrol, debouncecontrol, debouncetimerOutunit, debouncetimerOut) \
{gpio | 0x100, debouncetimerOut + (debouncetimerOutunit << 4) + (debouncecontrol << 5) + (leveltrig << 8) + (activelevel << 9) + (enable << 11) + \
(wakecontrol << 13)}

//
// GPIO DEVICE CONFIG DEFINITION
//
#define GPIO_DEVICE_DEFINITION(deviceid, enable, assert, deassert, hotplug) \
{deviceid, {enable + (assert << 2) + (deassert << 3) + (hotplug << 4)}}

//
// Internal Clock Request Definition
//
#define PCIE_INCLK(socket, die, clkid, clkreq, devid, devnum, funnum, slotchk) \
{clkid, clkreq, 0, 0, devid, devnum, funnum, slotchk, (socket << 24) + (die << 20) + (0 << 16) + (0 << 28)}

#define PCIE_CLKV2(socket, die, bridge, clkid, clkreq, devid, devnum, funnum, slotchk) \
{clkid, clkreq, 0, 0, devid, devnum, funnum, slotchk, (socket << 24) + (die << 20) + (bridge << 16) + (0 << 28)}

#define PCIE_CLKV3(socket, die, bridge, miscid, clkid, clkreq, devid, devnum, funnum, slotchk) \
{clkid, clkreq, 0, 0, devid, devnum, funnum, slotchk, (socket << 24) + (die << 20) + (bridge << 16) + (miscid << 28)}

#define PcieAddress(Ptr, Bus, Device, Function, Register) \
        Ptr->MainTablePtr->PcieMemIoBaseAddr + (Bus << 20) + (Device << 15) + (Function << 12) + Register

#define CPM_DEVICE_OFF                          0
#define CPM_DEVICE_ON                           1
#define CPM_DEVICE_AUTO                         2

#define CPM_TYPE_GPIO_1                         0
#define CPM_TYPE_GPIO_2                         1
#define CPM_TYPE_GPIO_3                         2
#define CPM_TYPE_GPIO_2_OR                      3
#define CPM_TYPE_GPIO_3_OR                      4

#define CPM_GPIO_PIN(pin, value)    {(pin + (value << 16))}
#define CPM_EC_GPIO_PIN(pin, value) ((pin | 0x100) + (value << 16))
#define CPM_ECRAM_GPIO_PIN(pin, value) ((pin | 0x200) + (value << 16))

///< Device Reset mode.
#define CPM_RESET_ASSERT                        0     ///<   0: Reset Assert.
#define CPM_RESET_DEASSERT                      1     ///<   1: Reset De-assert.
#define CPM_RESET_DELAY                         2     ///<   2: Delay between Assert and Deassert.
#define CPM_PT_RESET_DELAY                      3     ///<   3: Delay between Assert and Deassert for Promontory GPIO.

#define CPM_RESET_GPIO                          0
#define CPM_RESET_NA                            0

#define CPM_POWER_OFF                           0
#define CPM_POWER_ON                            1

///< Device Power Type.
#define CPM_POWER_SET                           0     ///<   0: Set GPIO.
#define CPM_POWER_WAIT                          1     ///<   1: Wait GPIO.
#define CPM_POWER_DELAY                         2     ///<   2: Add Delay.
#define CPM_PT_POWER_DELAY                      3     ///<   3: Add Delay for Promontory GPIO.

// AML Define

#ifndef AML_OPREGION_OP
  #define AML_OPREGION_OP                       0x805B
#endif

//
// BIOS & ACPI SW SMI function number
//
#ifndef CPM_ACPI_RESTORE_SW_SMI
  #define CPM_ACPI_RESTORE_SW_SMI               0xE3
#endif
#ifndef CPM_ACPI_ENABLE_SW_SMI
  #define CPM_ACPI_ENABLE_SW_SMI                0xA0
#endif

#ifndef CPM_ECRAM_BASE_ADDR
  #define CPM_ECRAM_BASE_ADDR                   0x62
#endif

#ifndef CPM_ECRAM_GPIO_OFFSET
  #define CPM_ECRAM_GPIO_OFFSET                 0xAB
#endif

#ifndef CPM_SWSMICMD_SETAUTOUMAMODE
  #define CPM_SWSMICMD_SETAUTOUMAMODE           0xDF
#endif

#ifndef CPM_ACPI_DISABLE_DGPU_AUDIO_SW_SMI
  #define CPM_ACPI_DISABLE_DGPU_AUDIO_SW_SMI    0xE4
#endif

#ifndef CPM_X86_IO_REMAP_BASE
  #define CPM_X86_IO_REMAP_BASE                 0x000000FDFC000000ull
#endif

#ifndef CPM_PCI_MMIO_BASE
  #define CPM_PCI_MMIO_BASE                     0x0000000080000000ull
#endif


#endif
