/* $NoKeywords:$ */
/**
 * @file
 *
 * Agesa structures and definitions
 *
 * Contains AMD AGESA core interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Include
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/


#ifndef _AGESA_H_
#define _AGESA_H_

#include  "Porting.h"
#include  "AMD.h"
#include  "SocLogicalId.h"

//
//
// AGESA Types and Definitions
//
//

// AGESA BASIC CALLOUTS
#define AGESA_MEM_RELEASE              0x00028000ul

// AGESA ADVANCED CALLOUTS, Processor
#define AGESA_CHECK_UMA                         0x00028100ul
#define AGESA_DO_RESET                          0x00028101ul
#define AGESA_ALLOCATE_BUFFER                   0x00028102ul
#define AGESA_DEALLOCATE_BUFFER                 0x00028103ul
#define AGESA_LOCATE_BUFFER                     0x00028104ul
#define AGESA_RUNFUNC_ONAP                      0x00028105ul
#define AGESA_RUNFUNC_ON_ALL_APS                0x00028106ul
#define AGESA_IDLE_AN_AP                        0x00028107ul
#define AGESA_WAIT_FOR_ALL_APS                  0x00028108ul

// AGESA ADVANCED CALLOUTS, Memory
#define AGESA_READ_SPD                 0x00028140ul
#define AGESA_HOOKBEFORE_DRAM_INIT     0x00028141ul
#define AGESA_HOOKBEFORE_DQS_TRAINING  0x00028142ul
#define AGESA_READ_SPD_RECOVERY        0x00028143ul
#define AGESA_HOOKBEFORE_EXIT_SELF_REF 0x00028144ul
#define AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY     0x00028145ul
#define AGESA_EXTERNAL_2D_TRAIN_VREF_CHANGE     0x00028146ul
#define AGESA_EXTERNAL_VOLTAGE_ADJUST  0x00028147ul

// AGESA IDS CALLOUTS
#define AGESA_GET_IDS_INIT_DATA       0x00028200ul

// AGESA GNB CALLOUTS
#define AGESA_GNB_PCIE_SLOT_RESET      0x00028301ul
#define AGESA_GNB_GFX_GET_VBIOS_IMAGE  0x00028302ul
#define AGESA_GNB_PCIE_CLK_REQ         0x00028303ul

// AGESA FCH CALLOUTS
#define AGESA_FCH_OEM_CALLOUT          0x00028401ul

//-----------------------------------------------------------------------------
//                     FCH DEFINITIONS AND MACROS
//
//-----------------------------------------------------------------------------

/// Configuration values for SdConfig
typedef enum {
  SdDisable = 0,                      ///< Disabled
  SdVer2,                             ///< Version 2.0
  SdVer3,                              ///< Version 3.0
  SdV3SDR50,                            ///< V3 SdSDR50
  SdV3SDR104,                          ///< V3 SdSDR104
  SdV3DDR50,                             ///< V3 SdDDR50
  SdDump                             ///< SD DUMP, don't touch SD
} SD_MODE;

/// Configuration values for SdClockControl
typedef enum {
  Sd50MhzTraceCableLengthWithinSixInches = 4,           ///< 50Mhz, default
  Sd40MhzTraceCableLengthSix2ElevenInches = 6,          ///< 40Mhz
  Sd25MhzTraceCableLengthEleven2TwentyfourInches = 7,   ///< 25Mhz
} SD_CLOCK_CONTROL;

/// Configuration values for AzaliaController
typedef enum {
  AzAuto = 0,                         ///< Auto - Detect Azalia controller automatically
  AzDisable,                          ///< Diable - Disable Azalia controller
  AzEnable                            ///< Enable - Enable Azalia controller
} HDA_CONFIG;

/// Configuration values for IrConfig
typedef enum {
  IrDisable  = 0,                     ///< Disable
  IrRxTx0    = 1,                     ///< Rx and Tx0
  IrRxTx1    = 2,                     ///< Rx and Tx1
  IrRxTx0Tx1 = 3                      ///< Rx and both Tx0,Tx1
} IR_CONFIG;

/// Configuration values for SataClass
typedef enum {
  SataNativeIde = 0,                  ///< Native IDE mode
  SataRaid,                           ///< RAID mode
  SataAhci,                           ///< AHCI mode
  SataLegacyIde,                      ///< Legacy IDE mode
  SataIde2Ahci,                       ///< IDE->AHCI mode
  SataAhci7804,                       ///< AHCI mode as 7804 ID (AMD driver)
  SataIde2Ahci7804                    ///< IDE->AHCI mode as 7804 ID (AMD driver)
} SATA_CLASS;

/// Configuration values for BLDCFG_FCH_GPP_LINK_CONFIG
typedef enum {
  PortA4       = 0,                   ///< 4:0:0:0
  PortA2B2     = 2,                   ///< 2:2:0:0
  PortA2B1C1   = 3,                   ///< 2:1:1:0
  PortA1B1C1D1 = 4                    ///< 1:1:1:1
} GPP_LINKMODE;

/// Configuration values for FchPowerFail
typedef enum {
  AlwaysOff   = 0,                    ///< Always power off after power resumes
  AlwaysOn    = 1,                    ///< Always power on after power resumes
  UsePrevious = 3,                    ///< Resume to same setting when power fails
} POWER_FAIL;


/// Configuration values for SATA Link Speed
typedef enum {
  Gen1   = 1,                         ///< SATA port GEN1 speed
  Gen2   = 2,                         ///< SATA port GEN2 speed
  Gen3   = 3,                         ///< SATA port GEN3 speed
} SATA_SPEED;


/// Configuration values for GPIO function
typedef enum {
  Function0   = 0,                    ///< GPIO Function 1
  Function1   = 1,                    ///< GPIO Function 1
  Function2   = 2,                    ///< GPIO Function 2
  Function3   = 3,                    ///< GPIO Function 3
} GPIO_FUN;

/// Configuration values for memory phy voltage (VDDR)
#define  VOLT0_95   0                 ///< VDDR 0.95V
#define  VOLT1_05   1                 ///< VDDR 1.05V
#define  MAX_VDDR   2                 ///< Maxmum value for this enum definition

/// Configuration values for GPIO_CFG
typedef enum {
  OwnedByEc   = 1 << 0,               ///< This bit can only be written by EC
  OwnedByHost = 1 << 1,               ///< This bit can only be written by host (BIOS)
  Sticky      = 1 << 2,               ///< If set, [6:3] are sticky
  PullUpB     = 1 << 3,               ///< 0: Pullup enable; 1: Pullup disabled
  PullDown    = 1 << 4,               ///< 0: Pulldown disabled; 1: Pulldown enable
  GpioOutEnB  = 1 << 5,               ///< 0: Output enable; 1: Output disable
  GpioOut     = 1 << 6,               ///< Output state when GpioOutEnB is 0
  GpioIn      = 1 << 7,               ///< This bit is read only - current pin state
} CFG_BYTE;

/// Configuration values for GPIO_CFG2
typedef enum {
  DrvStrengthSel_4mA = 0 << 1,               ///< 18:17 DrvStrengthSel.
  DrvStrengthSel_8mA = 1 << 1,               ///< 18:17 DrvStrengthSel.
  DrvStrengthSel_12mA = 2 << 1,               ///< 18:17 DrvStrengthSel.
  DrvStrengthSel_16mA = 3 << 1,               ///< 18:17 DrvStrengthSel.
  PullUpSel_8K     = 1 << 3,               ///< 19 PullUpSel. Read-write. 0=4 K pull-up is selected. 1=8 K pull-up is selected.
  PullUpEnable    = 1 << 4,               ///< 20 PullUpEnable. Read-write. 0=Pull-up is disabled on the pin. 1=Pull-up is enabled on the pin.
  PullDownEnable  = 1 << 5,               ///< 21 PullDownEnable. Read-write. 0=Pull-down is disabled on the pin. 1=Pull-down is enabled on thepin.
  OutputValue     = 1 << 6,               ///< 22 OutputValue. Read-write. 0=low. 1=high.
  OutputEnable      = 1 << 7,               ///< 23 OutputEnable. Read-write. 0=Output is disabled on the pin. 1=Output is enabled on the pin.
} CFG2_BYTE;

/// FCH GPIO CONTROL
typedef struct {
  IN         UINT8        GpioPin;               ///< Gpio Pin, valid range: 0-67, 128-150, 160-228
  IN         GPIO_FUN     PinFunction;           ///< Multi-function selection
  IN         CFG_BYTE     CfgByte;               ///< GPIO Register value
} GPIO_CONTROL;

///
/// FCH SCI MAP CONTROL
///
typedef struct {
  IN         UINT8        InputPin;              ///< Input Pin, valid range 0-63
  IN         UINT8        GpeMap;                ///< Gpe Map, valid range 0-31
} SCI_MAP_CONTROL;

///
/// FCH SATA PHY CONTROL
///
typedef struct {
  IN         BOOLEAN      CommonPhy;             ///< Common PHY or not
                                      ///<   @li <b>FALSE</b> - Only applied to specified port
                                      ///<   @li <b>TRUE</b>  - Apply to all SATA ports
  IN         SATA_SPEED   Gen;                   ///< SATA speed
  IN         UINT8        Port;                  ///< Port number, valid range: 0-7
  IN         UINT32       PhyData;               ///< SATA PHY data, valid range: 0-0xFFFFFFFF
} SATA_PHY_CONTROL;

///
/// FCH Component Data Structure in InitReset stage
///
typedef struct {
  IN       BOOLEAN      UmiGen2;             ///< Enable Gen2 data rate of UMI
                                             ///<   @li <b>FALSE</b> - Disable Gen2
                                             ///<   @li <b>TRUE</b>  - Enable Gen2

  IN       BOOLEAN      SataEnable;          ///< SATA controller function
                                             ///<   @li <b>FALSE</b> - SATA controller is disabled
                                             ///<   @li <b>TRUE</b> - SATA controller is enabled

  IN       BOOLEAN      IdeEnable;           ///< SATA IDE controller mode enabled/disabled
                                             ///<   @li <b>FALSE</b> - IDE controller is disabled
                                             ///<   @li <b>TRUE</b> - IDE controller is enabled

  IN       BOOLEAN      GppEnable;           ///< Master switch of GPP function
                                             ///<   @li <b>FALSE</b> - GPP disabled
                                             ///<   @li <b>TRUE</b> - GPP enabled

  IN       BOOLEAN      Xhci0Enable;         ///< XHCI0 controller function
                                             ///<   @li <b>FALSE</b> - XHCI0 controller disabled
                                             ///<   @li <b>TRUE</b> - XHCI0 controller enabled

  IN       BOOLEAN      Xhci1Enable;         ///< XHCI1 controller function
                                             ///<   @li <b>FALSE</b> - XHCI1 controller disabled
                                             ///<   @li <b>TRUE</b> - XHCI1 controller enabled
} FCH_RESET_INTERFACE;


///
/// FCH Component Data Structure from InitEnv stage
///
typedef struct {
  IN       SD_MODE      SdConfig;            ///< Secure Digital (SD) controller mode
  IN       HDA_CONFIG   AzaliaController;    ///< Azalia HD Audio Controller

  IN       IR_CONFIG    IrConfig;            ///< Infrared (IR) Configuration
  IN       BOOLEAN      UmiGen2;             ///< Enable Gen2 data rate of UMI
                                             ///<   @li <b>FALSE</b> - Disable Gen2
                                             ///<   @li <b>TRUE</b>  - Enable Gen2

  IN       SATA_CLASS   SataClass;           ///< SATA controller mode
  IN       BOOLEAN      SataEnable;          ///< SATA controller function
                                             ///<   @li <b>FALSE</b> - SATA controller is disabled
                                             ///<   @li <b>TRUE</b> - SATA controller is enabled

  IN       BOOLEAN      IdeEnable;           ///< SATA IDE controller mode enabled/disabled
                                             ///<   @li <b>FALSE</b> - IDE controller is disabled
                                             ///<   @li <b>TRUE</b> - IDE controller is enabled

  IN       BOOLEAN      SataIdeMode;         ///< Native mode of SATA IDE controller
                                             ///<   @li <b>FALSE</b> - Legacy IDE mode
                                             ///<   @li <b>TRUE</b> - Native IDE mode

  IN       BOOLEAN      Ohci1Enable;         ///< OHCI controller #1 Function
                                             ///<   @li <b>FALSE</b> - OHCI1 is disabled
                                             ///<   @li <b>TRUE</b> - OHCI1 is enabled

  IN       BOOLEAN      Ohci2Enable;         ///< OHCI controller #2 Function
                                             ///<   @li <b>FALSE</b> - OHCI2 is disabled
                                             ///<   @li <b>TRUE</b> - OHCI2 is enabled

  IN       BOOLEAN      Ohci3Enable;         ///< OHCI controller #3 Function
                                             ///<   @li <b>FALSE</b> - OHCI3 is disabled
                                             ///<   @li <b>TRUE</b> - OHCI3 is enabled

  IN       BOOLEAN      Ohci4Enable;         ///< OHCI controller #4 Function
                                             ///<   @li <b>FALSE</b> - OHCI4 is disabled
                                             ///<   @li <b>TRUE</b> - OHCI4 is enabled

  IN       BOOLEAN      GppEnable;           ///< Master switch of GPP function
                                             ///<   @li <b>FALSE</b> - GPP disabled
                                             ///<   @li <b>TRUE</b> - GPP enabled

  IN       POWER_FAIL   FchPowerFail;        ///< FCH power failure option
} FCH_INTERFACE;


/*----------------------------------------------------------------------------
 *   CPU Feature related info
 *----------------------------------------------------------------------------
 */
/// Build Configuration values for BLDCFG_PLATFORM_CONNECTED_STANDBY_MODE
typedef enum {
  ConnectedStandbyAuto     = 0,     ///< Auto
  ConnectedStandbyDisabled = 1,     ///< Disabled
  MaxConnectedStandbyMode  = 2      ///< Not a valid value, used for verifying input
} PLATFORM_CONNECTED_STANDBY_MODES;

/// Build Configuration values for BLDCFG_PLATFORM_CSTATE_MODE
typedef enum {
  CStateModeDisabled = 0,           ///< Disabled
  CStateModeC6       = 1,           ///< C6 State
  MaxCStateMode      = 2            ///< Not a valid value, used for verifying input
} PLATFORM_CSTATE_MODES;

/// Build Configuration values for BLDCFG_PLATFORM_CPB_MODE
typedef enum {
  CpbModeAuto     = 0,           ///< Auto
  CpbModeDisabled = 1,           ///< Disabled
  MaxCpbMode      = 2            ///< Not a valid value, used for verifying input
} PLATFORM_CPB_MODES;


/// Build Configuration values for BLDCFG_ACPI_PSTATES_PSD_POLICY
#define PsdPolicyProcessorDefault  0       ///< PSD is dependent or independent per processor default
#define PsdPolicyDependent         1       ///< PSD is forced dependent
#define PsdPolicyIndependent       2       ///< PSD is forced independent
#define PsdPolicyMax               3       ///< Not a valid value, used for verifying input

/*----------------------------------------------------------------------------
 *   GNB PCIe configuration info
 *----------------------------------------------------------------------------
 */

// Event definitions

#define GNB_EVENT_INVALID_CONFIGURATION               0x20010000ul   // User configuration invalid
#define GNB_EVENT_INVALID_PCIE_TOPOLOGY_CONFIGURATION 0x20010001ul   // Requested lane allocation for PCIe port can not be supported
#define GNB_EVENT_INVALID_PCIE_PORT_CONFIGURATION     0x20010002ul   // Requested incorrect PCIe port device address
#define GNB_EVENT_INVALID_DDI_LINK_CONFIGURATION      0x20010003ul   // Incorrect parameter in DDI link configuration
#define GNB_EVENT_INVALID_LINK_WIDTH_CONFIGURATION    0x20010004ul   // Invalid with for PCIe port or DDI link
#define GNB_EVENT_INVALID_LANES_CONFIGURATION         0x20010005ul   // Lane double subscribe lanes
#define GNB_EVENT_INVALID_DDI_TOPOLOGY_CONFIGURATION  0x20010006ul   // Requested lane allocation for DDI link(s) can not be supported
#define GNB_EVENT_LINK_TRAINING_FAIL                  0x20020000ul   // PCIe Link training fail
#define GNB_EVENT_BROKEN_LANE_RECOVERY                0x20030000ul   // Broken lane workaround applied to recover link training
#define GNB_EVENT_GEN2_SUPPORT_RECOVERY               0x20040000ul   // Scale back to GEN1 to recover link training


#define DESCRIPTOR_TERMINATE_LIST           0x80000000ull
#define DESCRIPTOR_IGNORE                   0x40000000ull

/// PCIe link initialization
typedef enum {
  EndpointDetect = 0,                                     ///< Detect endpoint presence
  EndpointNotPresent                                      ///< Endpoint not present (or connected). Used in case there is alternative way to determine
                                                          ///< if device present on board or in slot. For example GPIO can be used to determine device presence.
} PCIE_ENDPOINT_STATUS;


/// PCIe port misc extended controls
typedef struct  {
  IN      UINT8                     LinkComplianceMode :1;  ///< Force port into compliance mode (device will not be trained, port output compliance pattern)
  IN      UINT8                     LinkSafeMode       :2;  /**< Safe mode PCIe capability. (Parameter may limit PCIe speed requested through PCIe_PORT_DATA::LinkSpeedCapability)
                                                             *  @li @b 0 - port can advertize muximum supported capability
                                                             *  @li @b 1 - port limit advertized capability and speed to PCIe Gen1
                                                             */
  IN      UINT8                     SbLink             :1;  /**< PCIe link type
                                                             *  @li @b 0 - General purpose port
                                                             *  @li @b 1 - Port connected to SB
                                                             */
  IN      UINT8                     ClkPmSupport       :1;  /**< Clock Power Management Support
                                                             *  @li @b 0 - Clock Power Management not configured
                                                             *  @li @b 1 - Clock Power Management configured according to PCIe device capability
                                                             */
} PCIe_PORT_MISC_CONTROL;

/// The IO APIC Interrupt Mapping Info
typedef struct {
  IN      UINT8                     GroupMap;               /**< Group mapping for slot or endpoint device (connected to PCIE port) interrupts .
                                                             *  @li <b>0</b> - IGNORE THIS STRUCTURE AND USE RECOMMENDED SETTINGS
                                                             *  @li <b>1</b> - mapped to Grp 0 (Interrupts 0..3   of IO APIC redirection table)
                                                             *  @li <b>2</b> - mapped to Grp 1 (Interrupts 4..7   of IO APIC redirection table)
                                                             *  @li ...
                                                             *  @li <b>8</b> - mapped to Grp 7 (Interrupts 28..31 of IO APIC redirection table)
                                                             */
  IN      UINT8                     Swizzle;                /**< Swizzle interrupt in the Group.
                                                             *  @li <b>0</b> - ABCD
                                                             *  @li <b>1</b> - BCDA
                                                             *  @li <b>2</b> - CDAB
                                                             *  @li <b>3</b> - DABC
                                                             */
  IN      UINT8                     BridgeInt;              /**< IOAPIC redirection table entry for PCIE bridge interrupt
                                                             *  @li <b>0</b>  - Entry 0  of IO APIC redirection table
                                                             *  @li <b>1</b>  - Entry 1  of IO APIC redirection table
                                                             *  @li ...
                                                             *  @li <b>31</b> - Entry 31 of IO APIC redirection table
                                                             */
} APIC_DEVICE_INFO;

/// Initial Offset Calibration Control
typedef enum {
  ADAPT_IOC_DISABLED = 0,                                     ///< Initial Offset Calibration Disabled
  ADAPT_IOC_ENABLED                                           ///< Initial Offset Calibration Enabled
} ADAPT_IOC_CONTROL;

/// DFE Control values
typedef enum {
  ADAPT_DFE_CONTROL_DISABLED = 0,                             ///< DFE Disabled
  ADAPD_DFE_CONTROL_1TAP_DFE = 4,                             ///< 1-tap DFE
  ADAPD_DFE_CONTROL_1TAP_DFE_FBF,                             ///< 1-tap DFE with Future Bit Filtering
  ADAPD_DFE_CONTROL_2TAP_DFE,                                ///< 2-tap DFE
  ADAPD_DFE_CONTROL_2TAP_DFE_FBF                              ///< 2-tap DFE with Future Bit Filtering
} ADAPT_DFE_CONTROL;

/// LEQ Control values
typedef enum {
  ADAPT_LEQ_CONTROL_DISABLED = 0,                              ///< LEQ Disabled
  ADAPT_LEQ_CONTROL_DC_GAIN  = 2,                              ///< DC Gain Adaptation
  ADAPT_LEQ_CONTROL_DC_GAIN_POLE                               ///< DC Gain and Pole Adaptation
} ADAPT_LEQ_CONTROL;

/// Dynamic Offset Calibration Control
typedef enum {
  ADAPT_DOC_DISABLED = 0,                                      ///< Dynamic Offset Calibration Disabled
  ADAPT_DOC_ENABLED                                            ///< Dynamic Offset Calibration Enabled
} ADAPT_DOC_CONTROL;

/// FOM Calculation Control
typedef enum {
  ADAPT_FOMC_DISABLED = 0,                                    ///< FOM Calculation Disabled
  ADAPT_FOMC_ENABLED                                          ///< FOM Calculation Enabled
} ADAPT_FOMC_CONTROL;

/// PI Offset Calibration Control
typedef enum {
  ADAPT_PIOC_DISABLED = 0,                                    ///< PI Offset Calibration Disabled
  ADAPT_PIOC_ENABLED                                          ///< PI Offset Calibration Enabled
} ADAPT_PIOC_CONTROL;

/// Bank Group Swap Mode
typedef enum {
  BANK_GROUP_SWAP_DISABLED = 0,                              ///< Bank Group Swap disabled
  BANK_GROUP_SWAP_CPU = 1,                                   ///< Bank Group Swap CPU Mode
  BANK_GROUP_SWAP_APU = 2                                    ///< Bank Group Swap APU Mode
} BANK_GROUP_SWAP_MODE;

/// GEN3 RxAdaptMode Configuration Structure
typedef struct {
  IN      BOOLEAN                InitOffsetCancellation;     ///< Initial Offset Cancellation Enable
  IN      UINT8                  DFEControl;                 ///< DFE Control
  IN      UINT8                  LEQControl;                 ///< LEQ Control
  IN      BOOLEAN                DynamicOffsetCalibration;   ///< Dynamic Offset Calibration Enable
  IN      BOOLEAN                FOMCalculation;             ///< FOM Calculation Enable
  IN      BOOLEAN                PIOffsetCalibration;        ///< PI Offset Calibratino Enable
} RX_ADAPT_MODE;

/// PCIe port configuration data
typedef struct  {
  IN       UINT8                   PortPresent;              ///< Enable PCIe port for initialization.
  IN       UINT8                   ChannelType;              /**< Channel type.
                                                               *  @li @b 0 - "lowLoss",
                                                               *  @li @b 1 - "highLoss",
                                                               *  @li @b 2 - "mob0db",
                                                               *  @li @b 3 - "mob3db",
                                                               *  @li @b 4 - "extnd6db"
                                                               *  @li @b 5 - "extnd8db"
                                                               */
  IN       UINT8                   DeviceNumber;             /**< PCI Device number for port.
                                                               *   @li @b 0 - Native port device number
                                                               *   @li @b N - Port device number (See available configurations in BKDG
                                                               */
  IN       UINT8                   FunctionNumber;           ///< Reserved for future use
  IN       UINT8                   LinkSpeedCapability;      /**< PCIe link speed/
                                                               *  @li @b 0 - Maximum supported by silicon
                                                               *  @li @b 1 - Gen1
                                                               *  @li @b 2 - Gen2
                                                               *  @li @b 3 - Gen3
                                                               */
  IN       UINT8                   LinkAspm;                 /**< ASPM control. (see AgesaPcieLinkAspm for additional option to control ASPM)
                                                               *  @li @b 0 - Disabled
                                                               *  @li @b 1 - L0s only
                                                               *  @li @b 2 - L1 only
                                                               *  @li @b 3 - L0s and L1
                                                               */
  IN       UINT8                   LinkHotplug;              /**< Hotplug control.
                                                               *  @li @b 0 - Disabled
                                                               *  @li @b 1 - Basic
                                                               *  @li @b 2 - Server
                                                               *  @li @b 3 - Enhanced
                                                               */
  IN       UINT8                   ResetId;                  /**< Arbitrary number greater than 0 assigned by platform firmware for GPIO
                                                               *   identification which control reset for given port.
                                                               *   Each port with unique GPIO should have unique ResetId assigned.
                                                               *   All ports use same GPIO to control reset should have same ResetId assigned.
                                                               *   see AgesaPcieSlotResetContol.
                                                               */
  IN       PCIe_PORT_MISC_CONTROL  MiscControls;             ///< Misc extended controls
  IN       APIC_DEVICE_INFO        ApicDeviceInfo;           ///< IOAPIC device programming info
  IN       PCIE_ENDPOINT_STATUS    EndpointStatus;           ///< PCIe endpoint (device connected to PCIe port) status
  IN       RX_ADAPT_MODE           RxAdaptMode;              ///< Gen3 RxAdaptMode configuration
} PCIe_PORT_DATA;

/// DDI channel lane mapping
typedef struct {                                          ///< Structure that discribe lane mapping
  IN      UINT8              Lane0   :2;                  /**< Lane 0 mapping
                                                           *  @li @b 0 - Map to lane 0
                                                           *  @li @b 1 - Map to lane 1
                                                           *  @li @b 2 - Map to lane 2
                                                           *  @li @b 2 - Map to lane 3
                                                           */
  IN      UINT8              Lane1   :2;                  ///< Lane 1 mapping (see "Lane 0 mapping")
  IN      UINT8              Lane2   :2;                  ///< Lane 2 mapping (see "Lane 0 mapping")
  IN      UINT8              Lane3   :2;                  ///< Lane 3 mapping (see "Lane 0 mapping")
} CHANNEL_MAPPING;                                        ///< Lane mapping

/// Common Channel Mapping
typedef union {
  IN      UINT8                ChannelMappingValue;       ///< Raw lane mapping
  IN      CHANNEL_MAPPING      ChannelMapping;            ///< Channel mapping
} CONN_CHANNEL_MAPPING;

/// DDI Configuration data
typedef struct  {
  IN       UINT8                ConnectorType;            /**< Display Connector Type
                                                            *  @li @b 0 - DP
                                                            *  @li @b 1 - eDP
                                                            *  @li @b 2 - Single Link DVI-D
                                                            *  @li @b 3 - Dual  Link DVI-D
                                                            *  @li @b 4 - HDMI
                                                            *  @li @b 5 - DP-to-VGA
                                                            *  @li @b 6 - DP-to-LVDS
                                                            *  @li @b 7 - Hudson-2 NutMeg DP-to-VGA
                                                            *  @li @b 8 - Single Link DVI-I
                                                            *  @li @b 9 - Native CRT (Family 0x14)
                                                            *  @li @b 10 - Native LVDS (Family 0x14)
                                                            *  @li @b 11 - Auto detect LCD panel connector type. VBIOS is able to auto detect the LVDS connector type: native LVDS, eDP or DP-to-LVDS
                                                            *              The auto detection method only support panel with EDID.
                                                            */
  IN       UINT8                AuxIndex;                 /**< Indicates which AUX or DDC Line is used
                                                            *  @li @b 0 - AUX1
                                                            *  @li @b 1 - AUX2
                                                            *  @li @b 2 - AUX3
                                                            *  @li @b 3 - AUX4
                                                            *  @li @b 4 - AUX5
                                                            *  @li @b 5 - AUX6
                                                            */
  IN       UINT8                HdpIndex;                 /**< Indicates which HDP pin is used
                                                            *  @li @b 0 - HDP1
                                                            *  @li @b 1 - HDP2
                                                            *  @li @b 2 - HDP3
                                                            *  @li @b 3 - HDP4
                                                            *  @li @b 4 - HDP5
                                                            *  @li @b 5 - HDP6
                                                            */
  IN       CONN_CHANNEL_MAPPING Mapping[2];               /**< Set specific mapping of lanes to connector pins
                                                            *  @li Mapping[0] define mapping for group of 4 lanes starting at PCIe_ENGINE_DATA.StartLane
                                                            *  @li Mapping[1] define mapping for group of 4 lanes ending at PCIe_ENGINE_DATA.EndLane (only applicable for Dual DDI link)
                                                            *  if Mapping[x] set to 0 than default mapping assumed
                                                            */
  IN       UINT8                LanePnInversionMask;      /**< Specifies whether to invert the state of P and N for each lane. Each bit represents a PCIe lane on the DDI port.
                                                            *  @li 0 - Do not invert (default)
                                                            *  @li 1 - Invert P and N on this lane
                                                            */
  IN       UINT8                Flags;                    /**< Capabilities flags
                                                            *  @li Flags bit[0] DDI_DATA_FLAGS_DP1_1_ONLY Selects downgrade PHY link to DP1.1
                                                            *  @li Flags bit[7:1] Reserved
                                                            */
} PCIe_DDI_DATA;

/// Engine Configuration
typedef struct {
  IN       UINT8                EngineType;               /**< Engine type
                                                           *  @li @b 0 -  Ignore engine configuration
                                                           *  @li @b 1 -  PCIe port
                                                           *  @li @b 2 -  DDI
                                                           */
  IN       UINT16               StartLane;                /**< Start Lane ID (in reversed configuration StartLane > EndLane)
                                                           * Refer to lane descriptions and supported configurations in BKDG
                                                           */
  IN       UINT16               EndLane;                  /**< End lane ID (in reversed configuration StartLane > EndLane)
                                                           * Refer to lane descriptions and supported configurations in BKDG
                                                           */

} PCIe_ENGINE_DATA;

/// PCIe port descriptor
typedef struct {
  IN       UINT32               Flags;                    /**< Descriptor flags
                                                           * @li @b Bit31 - last descriptor in complex
                                                           */
  IN       PCIe_ENGINE_DATA     EngineData;               ///< Engine data
  IN       PCIe_PORT_DATA       Port;                     ///< PCIe port specific configuration info
} PCIe_PORT_DESCRIPTOR;

/// DDI descriptor
typedef struct {
  IN       UINT32               Flags;                    /**< Descriptor flags
                                                           * @li @b Bit31 - last descriptor in complex
                                                           */
  IN       PCIe_ENGINE_DATA     EngineData;               ///< Engine data
  IN       PCIe_DDI_DATA        Ddi;                      ///< DDI port specific configuration info
} PCIe_DDI_DESCRIPTOR;

/// PCIe Complex descriptor
typedef struct {
  IN       UINT32               Flags;                    /**< Descriptor flags
                                                           * @li @b Bit31 - last descriptor in topology
                                                           */
  IN       UINT32               SocketId;                 ///< Socket Id
  IN       PCIe_PORT_DESCRIPTOR *PciePortList;            ///< Pointer to array of PCIe port descriptors or NULL (Last element of array must be terminated with DESCRIPTOR_TERMINATE_LIST).
  IN       PCIe_DDI_DESCRIPTOR  *DdiLinkList;             ///< Pointer to array DDI link descriptors (Last element of array must be terminated with DESCRIPTOR_TERMINATE_LIST).
  IN       VOID                 *Reserved;                ///< Reserved for future use
} PCIe_COMPLEX_DESCRIPTOR;

/// Action to control PCIe slot reset
typedef enum {
  AssertSlotReset,                                        ///< Assert slot reset
  DeassertSlotReset                                       ///< Deassert slot reset
} PCIE_RESET_CONTROL;

#define GFX_VBIOS_IMAGE_FLAG_SPECIAL_POST  0x1

///VBIOS image info
typedef struct {
  IN      AMD_CONFIG_PARAMS     StdHeader;                ///< Standard configuration header
  OUT     VOID                  *ImagePtr;                ///< Pointer to VBIOS image
  IN      PCI_ADDR              GfxPciAddress;            ///< PCI address of integrated graphics controller
  IN      UINT32                Flags;                    ///< BIT[0] - special repost requred
} GFX_VBIOS_IMAGE_INFO;

/// Engine descriptor type
typedef enum {
  PcieUnusedEngine = 0,                                   ///< Unused descriptor
  PciePortEngine = 1,                                     ///< PCIe port
  PcieDdiEngine = 2,                                      ///< DDI
  MaxPcieEngine                                           ///< Max engine type for boundary check.
} PCIE_ENGINE_TYPE;

/// PCIe link capability/speed
typedef enum  {
  PcieGenMaxSupported,                                    ///< Maximum supported
  PcieGen1 = 1,                                           ///< Gen1
  PcieGen2,                                               ///< Gen2
  PcieGen3,                                               ///< Gen3
  PcieGen4,                                               ///< Gen4
  MaxPcieGen                                              ///< Max Gen for boundary check
} PCIE_LINK_SPEED_CAP;

//[-start-240426-IB18160212-modify-]//
/// PCIe PSPP Power policy
#ifndef _PCIE_PSPP_POLICY_
#define _PCIE_PSPP_POLICY_
typedef enum  {
  PsppDisabled,                                           ///< PSPP disabled
  PsppPerformance = 1,                                    ///< Performance
  PsppBalanceHigh,                                        ///< Balance-High
  PsppBalanceLow,                                         ///< Balance-Low
  PsppPowerSaving,                                        ///< Power Saving
  MaxPspp                                                 ///< Max Pspp for boundary check
} PCIE_PSPP_POLICY;
#endif
//[end-240426-IB18160212-modify]//

/// DDI display connector type
typedef enum {
  ConnectorTypeDP,                                        ///< DP
  ConnectorTypeEDP,                                       ///< eDP
  ConnectorTypeSingleLinkDVI,                             ///< Single Link DVI-D
  ConnectorTypeDualLinkDVI,                               ///< Dual  Link DVI-D
  ConnectorTypeHDMI,                                      ///< HDMI
  ConnectorTypeDpToVga,                                   ///< DP-to-VGA
  ConnectorTypeDpToLvds,                                  ///< DP-to-LVDS
  ConnectorTypeNutmegDpToVga,                             ///< Hudson-2 NutMeg DP-to-VGA
  ConnectorTypeSingleLinkDviI,                            ///< Single Link DVI-I
  ConnectorTypeCrt,                                       ///< CRT (VGA)
  ConnectorTypeLvds,                                      ///< LVDS
  ConnectorTypeEDPToLvds,                                 ///< 3rd party common eDP-to-LVDS translator chip without AMD SW init
  ConnectorTypeEDPToLvdsSwInit,                           ///< 3rd party eDP-to-LVDS translator which requires AMD SW init
  ConnectorTypeAutoDetect,                                ///< VBIOS auto detect connector type (native LVDS, eDP or DP-to-LVDS)
  UnusedType,                                              ///< UnusedType
  MaxConnectorType                                        ///< Not valid value, used to verify input
} PCIE_CONNECTOR_TYPE;

/// PCIe link channel type
typedef enum {
  ChannelTypeLowLoss,                                     ///< Low Loss
  ChannelTypeHighLoss,                                    ///< High Loss
  ChannelTypeMob0db,                                      ///< Mobile 0dB
  ChannelTypeMob3db,                                      ///< Mobile 3dB
  ChannelTypeExt6db,                                      ///< Extended 6dB
  ChannelTypeExt8db,                                      ///< Extended 8dB
  MaxChannelType                                          ///< Not valid value, used to verify input
} PCIE_CHANNEL_TYPE;

/// PCIe link ASPM
typedef enum {
  AspmDisabled,                                           ///< Disabled
  AspmL0s,                                                ///< PCIe L0s link state
  AspmL1,                                                 ///< PCIe L1 link state
  AspmL0sL1,                                              ///< PCIe L0s & L1 link state
  MaxAspm                                                 ///< Not valid value, used to verify input
} PCIE_ASPM_TYPE;

/// PCIe link hotplug support
typedef enum {
  HotplugDisabled,                                        ///< Hotplug disable
  HotplugBasic,                                           ///< Basic Hotplug
  HotplugServer,                                          ///< Server Hotplug
  HotplugEnhanced,                                        ///< Enhanced
  HotplugInboard,                                         ///< Inboard
  HotplugServerSsd,                                       ///< Server Ssd
  MaxHotplug                                              ///< Not valid value, used to verify input
} PCIE_HOTPLUG_TYPE;

/// PCIe link initialization
typedef enum {
  PortDisabled,                                           ///< Disable
  PortEnabled                                             ///< Enable
} PCIE_PORT_ENABLE;

/// PCIe ACS capability - Access Control Services
typedef enum  {
  PcieAcsDisabled,                                        ///< Disabled
  PcieAcsEnabled,                                         ///< Enabled
} PCIE_ACS_CAP;

/// PCIe ClkPmSupport initialization
typedef enum {
  ClkPmSupportDisabled,                                   ///< Disable
  ClkPmSupportEnabled                                     ///< Enable
} CLKPM_SUPPORT_ENABLE;

/// DDI Aux channel
typedef enum {
  Aux1,                                                   ///< Aux1
  Aux2,                                                   ///< Aux2
  Aux3,                                                   ///< Aux3
  Aux4,                                                   ///< Aux4
  Aux5,                                                   ///< Aux5
  Aux6,                                                   ///< Aux6
  MaxAux                                                  ///< Not valid value, used to verify input
} PCIE_AUX_TYPE;

/// DDI Hdp Index
typedef enum {
  Hdp1,                                                   ///< Hdp1
  Hdp2,                                                   ///< Hdp2
  Hdp3,                                                   ///< Hdp3
  Hdp4,                                                   ///< Hdp4
  Hdp5,                                                   ///< Hdp5
  Hdp6,                                                   ///< Hdp6
  MaxHdp                                                  ///< Not valid value, used to verify input
} PCIE_HDP_TYPE;

/// PCIe_DDI_DATA.Flags definitions
#define DDI_DATA_FLAGS_DP1_1_ONLY                        0x01         ///< BIT[0] Selects downgrade PHY link to DP1.1
#define EXT_DISPLAY_PATH_CAPS_DP_FIXED_VS_EN             0x02         ///< BIT[1] VBIOS will always output fixed voltage swing during DP link training
#define EXT_DISPLAY_PATH_CAPS_HDMI20_PI3EQX1204          0x04         ///< BIT[2] HDMI 2.0 connector
#define EXT_DISPLAY_PATH_CAPS_HDMI20_TISN65DP159RSBT     0x08         ///< BIT[3] HDMI 2.0 connector
#define EXT_DISPLAY_PATH_CAPS_HDMI20_PARADE_PS175        0x0C         ///< BIT[3:2] DP -> HDMI recoverter chip

/// DP receiver definitions with fixed voltage swing
typedef enum {
  DP_VS_0_4V_0DB,                                         ///< 0x00
  DP_VS_0_6V_0DB,                                         ///< 0x01
  DP_VS_0_8V_0DB,                                         ///< 0x02
  DP_VS_1_2V_0DB,                                         ///< 0x03
  DP_VS_0_4V_3_5DB = 0x8,                                 ///< 0x08
  DP_VS_0_6V_3_5DB,                                       ///< 0x09
  DP_VS_0_8V_3_5DB,                                       ///< 0x0a
  DP_VS_0_4V_6DB = 0x10,                                  ///< 0x10
  DP_VS_0_6V_6DB,                                         ///< 0x11
  DP_VS_0_4V_9_5DB = 0x18                                 ///< 0x18
} DP_FIXED_VOLT_SWING_TYPE;

// definition for eDP 1.4 VSMode
#define EDP_VS_LEGACY_MODE                  0             ///< Legacy Mode
#define EDP_VS_LOW_VDIFF_MODE               1             ///< Low Vdiff Training Mode
#define EDP_VS_HIGH_VDIFF_MODE              2             ///< High Vdiff Training Mode
#define EDP_VS_STRETCH_MODE                 3             ///< Stretched DP training mode
#define EDP_VS_SINGLE_VDIFF_MODE            4             ///< Single Vdiff Training Mode
#define EDP_VS_VARIABLE_PREM_MODE           5             ///< Single Vdiff Training with Variable Transition Vdiff

///  HDMI re-driver register/value
typedef struct _HDMI_RE_DRIVER_I2C_REG_INFO {
  IN  UINT8       RegIndex;                               ///< HDMI re-driver Register Index
  IN  UINT8       RegVal;                                 ///< HDMI re-driver Register Value
} HDMI_RE_DRIVER_I2C_REG_INFO;

/// AZ I2SBUS select
typedef enum {
  GnbAcpI2sBus,                                            ///< I2sBus
  GnbAcpAzalia,                                            ///< Azalia
  MaxAcp                                                   ///< Not valid value, used to verify input
} GNB_ACP_TYPE;

/// AZ I2SBUS pin configuration
typedef enum {
  GnbAcp4Tx4RxBluetooth,                                   ///< 4Tx4Rx and Bluetooth
  GnbAcp2Tx4RxBluetooth,                                   ///< 2Tx4Rx and Bluetooth
  GnbAcp6Tx4RxBluetooth,                                   ///< 6Tx4Rx and Bluetooth
  GnbAcpPinNotConfigured                                   ///< Not valid value, used to verify input
} GNB_ACP_AZ_I2SBUS_PIN;

/// Alternative DRAM MAC
typedef enum {
  MAC_UNTESTEDMAC,                               ///< Assign 0 to Untested MAC
  MAC_700k,                                      ///< Assign 1 to 700k
  MAC_600k,                                      ///< Assign 2 to 600k
  MAC_500k,                                      ///< Assign 3 to 500k
  MAC_400k,                                      ///< Assign 4 to 400k
  MAC_300k,                                      ///< Assign 5 to 300k
  MAC_200k,                                      ///< Assign 6 to 200k
  MAC_UNRESTRICTEDMAC = 8,                       ///< Assign 8 to Unrestricted MAC
} DRAM_MAXIMUM_ACTIVATE_COUNT;

// Macro for statically initializing various structures
#define  PCIE_ENGINE_DATA_INITIALIZER(mType, mStartLane, mEndLane) {mType, mStartLane, mEndLane}
#define  PCIE_PORT_DATA_INITIALIZER(mPortPresent, mChannelType, mDevAddress, mHotplug, mMaxLinkSpeed, mMaxLinkCap, mAspm, mResetId) \
{mPortPresent, mChannelType, mDevAddress, 0, mMaxLinkSpeed, mAspm, mHotplug, mResetId, {0, mMaxLinkCap, 0, 0}, {0, 0, 0}, EndpointDetect, \
{ADAPT_IOC_ENABLED, ADAPT_DFE_CONTROL_DISABLED, ADAPT_LEQ_CONTROL_DC_GAIN_POLE, ADAPT_DOC_DISABLED, ADAPT_FOMC_ENABLED, ADAPT_PIOC_DISABLED}}
#define  PCIE_PORT_DATA_INITIALIZER_V2(mPortPresent, mChannelType, mDevAddress, mDevFunction, mHotplug, mMaxLinkSpeed, mMaxLinkCap, mAspm, mResetId, mClkPmSupport) \
{mPortPresent, mChannelType, mDevAddress, mDevFunction, mMaxLinkSpeed, mAspm, mHotplug, mResetId, {0, mMaxLinkCap, 0, mClkPmSupport}, {0, 0, 0}, EndpointDetect, \
{ADAPT_IOC_ENABLED, ADAPT_DFE_CONTROL_DISABLED, ADAPT_LEQ_CONTROL_DC_GAIN_POLE, ADAPT_DOC_DISABLED, ADAPT_FOMC_ENABLED, ADAPT_PIOC_DISABLED}}
#define  PCIE_PORT_DATA_INITIALIZER_GEN3(mPortPresent, mChannelType, mDevAddress, mDevFunction, mHotplug, mMaxLinkSpeed, mMaxLinkCap, mAspm, mResetId, mClkPmSupport, \
mInitOffsetCancellation, mDFEControl, mLEQControl, mDynamicOffsetCalibration, mFOMCalculation, mPIOffsetCalibration) \
{mPortPresent, mChannelType, mDevAddress, mDevFunction, mMaxLinkSpeed, mAspm, mHotplug, mResetId, {0, mMaxLinkCap, 0, mClkPmSupport}, {0, 0, 0}, EndpointDetect, \
{mInitOffsetCancellation, mDFEControl, mLEQControl, mDynamicOffsetCalibration, mFOMCalculation, mPIOffsetCalibration}}
//[-start-240426-IB18160212-modify-]//
#define  PCIE_DDI_DATA_INITIALIZER(mConnectorType, mAuxIndex, mHpdIndex ) \
{mConnectorType, mAuxIndex, mHpdIndex, {{0}, {0}}, 0, 0}
//[end-240426-IB18160212-modify]//
#define  PCIE_DDI_DATA_INITIALIZER_V1(mConnectorType, mAuxIndex, mHpdIndex, mMapping0, mMapping1, mPNInversion) \
{mConnectorType, mAuxIndex, mHpdIndex, {mMapping0, mMapping1}, mPNInversion, 0}
#define  PCIE_DDI_DATA_INITIALIZER_V2(mConnectorType, mAuxIndex, mHpdIndex, mMapping0, mMapping1, mPNInversion, mFlags) \
{mConnectorType, mAuxIndex, mHpdIndex, {mMapping0, mMapping1}, mPNInversion, mFlags}

///IOMMU requestor ID
typedef struct {
  IN       UINT16     Bus       :8;                                ///< Bus
  IN       UINT16     Device    :5;                                ///< Device
  IN       UINT16     Function  :3;                                ///< Function
} IOMMU_REQUESTOR_ID;

/// IVMD exclusion range descriptor
typedef struct {
  IN       UINT32               Flags;                    /**< Descriptor flags
                                                           * @li @b Flags[31] - Terminate descriptor array.
                                                           * @li @b Flags[30] - Ignore descriptor.
                                                           */
  IN       IOMMU_REQUESTOR_ID   RequestorIdStart;         ///< Requestor ID start
  IN       IOMMU_REQUESTOR_ID   RequestorIdEnd;           ///< Requestor ID end (use same as start for single ID)
  IN       UINT64               RangeBaseAddress;         ///< Phisical base address of exclusion range
  IN       UINT64               RangeLength;              ///< Length of exclusion range in bytes
} IOMMU_EXCLUSION_RANGE_DESCRIPTOR;

/*----------------------------------------------------------------------------
 *   GNB configuration info
 *----------------------------------------------------------------------------
 */

/// LVDS Misc Control Field
typedef struct {
  IN  UINT8     FpdiMode:1;          ///< This item configures LVDS 888bit panel mode
                                     ///< @li FALSE = LVDS 888 panel in LDI mode
                                     ///< @li TRUE =  LVDS 888 panel in FPDI mode
                                     ///< @BldCfgItem{BLDCFG_LVDS_MISC_888_FPDI_MODE}
  IN  UINT8     DlChSwap:1;          ///< This item configures LVDS panel lower and upper link mapping
                                     ///< @li FALSE = Lower link and upper link not swap
                                     ///< @li TRUE = Lower link and upper link are swapped
                                     ///< @BldCfgItem{BLDCFG_LVDS_MISC_DL_CH_SWAP}
  IN  UINT8     BitDepth:1;          ///< Customer may use 888 bit LVDS panel, but the LVDS panel EDID does not support v1.4 so that VBIOS can not decide the panel bit per color
                                     ///< @li FALSE = LCD 666 18bit panel
                                     ///< @li TRUE = LCD 888 24bit panel
                                     ///< @BldCfgItem{BLDCFG_LVDS_BIT_DEPTH}
  IN  UINT8     ParamOverwriteEn:1;  ///< LVDS parameter overwrite enable
                                     ///< @li FALSE = LVDS parameter overwrite disable
                                     ///< @li TRUE = LVDS parameter overwrite enable
                                     ///< @BldCfgItem{BLDCFG_LVDS_PARAM_OVERWRITE_EN}
  IN  UINT8     BLONActiveLow:1;     ///< This item configures polarity of signal sent to digital BLON output pin
                                     ///< @li FALSE = Not inverted(active high)
                                     ///< @li TRUE = Inverted (active low)
                                     ///< @BldCfgItem{BLDCFG_LVDS_MISC_BLON_ACTIVE_LOW}
  IN  UINT8     LvdsVoltOverwriteEn:1;  ///< This item configures polarity of DP-to-LVDS output voltage overwrite
                                        ///< @li FALSE = DP-to-LVDS output voltage overwrite disable, use VBIOS default setting.
                                        ///< @li TRUE = Use ucLVDSVolAdjust value to program register LVDS_CTRL_4
                                        ///< @BldCfgItem{BLDCFG_LVDS_MISC_VOLT_OVERWRITE_ENABLE}
  IN  UINT8     Reserved:2;          ///< Reserved
} LVDS_MISC_CONTROL_FIELD;

/// LVDS Misc Control
typedef union _LVDS_MISC_CONTROL {
  IN LVDS_MISC_CONTROL_FIELD Field;  ///< LVDS_MISC_CONTROL_FIELD
  IN UINT8   Value;                  ///< LVDS Misc Control Value
} LVDS_MISC_CONTROL;

/// Display Misc Control Field
typedef struct {
  IN  UINT8     Reserved1:3;                  ///< Reserved
  IN  UINT8     VbiosFastBootEn:1;            ///< This item configures VBIOS skip display device detection in every set mode if LCD panel is connect and LID is open.
                                              ///< @li FALSE = VBIOS fast boot is disable.
                                              ///< @li TRUE = VBIOS fast boot is enable.
                                              ///< @BldCfgItem{BLDCFG_DISPLAY_MISC_VBIOS_FAST_BOOT_ENABLE}
  IN  UINT8     Reserved2:4;                  ///< Reserved
} DISPLAY_MISC_CONTROL_FIELD;

/// LVDS Misc Control
typedef union _DISPLAY_MISC_CONTROL {
  IN DISPLAY_MISC_CONTROL_FIELD Field;  ///< DISPLAY_MISC_CONTROL_FIELD
  IN UINT8   Value;                     ///< Display Misc Control Value
} DISPLAY_MISC_CONTROL;

/// HD Audio Codec table list
typedef struct _CODEC_VERB_TABLE_LIST {
  IN  UINT32           CodecId;             ///<  CodecID - Codec ID
} CODEC_VERB_TABLE_LIST;

/// POST Configuration settings for GNB.
typedef struct {
  IN UINT8       IgpuEnableDisablePolicy;   ///< This item defines the iGPU Enable/Disable policy
                                            ///< @li 0 = Auto - use existing default -
                                            ///< @li 1 = Disable iGPU if any PCIe/PCI graphics card present
                                            ///< @BldCfgItem{BLDCFG_IGPU_ENABLE_DISABLE_POLICY}
} GNB_POST_CONFIGURATION;

/// iGPU Enable/Disable Policy values
#define IGPU_DISABLE_AUTO        0        ///< Auto setting - disable iGPU if ANY PCI graphics or non-AMD PCIe graphics
#define IGPU_DISABLE_ANY_PCIE    1        ///< Disable iGPU if any PCI or PCIE graphics card is present

/// ENV Configuration settings for GNB.
typedef struct {
  IN  UINT8     Gnb3dStereoPinIndex;      ///< 3D Stereo Pin ID.
                                          ///< @li 0 = Stereo 3D is disabled (default).
                                          ///< @li 1 = Use processor pin HPD1.
                                          ///< @li 2 = Use processor pin HPD2
                                          ///< @li 3 = Use processor pin HPD3
                                          ///< @li 4 = Use processor pin HPD4
                                          ///< @li 5 = Use processor pin HPD5
                                          ///< @li 6 = Use processor pin HPD6
                                          ///< @BldCfgItem{BLDCFG_STEREO_3D_PINOUT}
  IN  BOOLEAN    IommuSupport;            ///< IOMMU support.
                                          ///< @li FALSE = Disabled. Disable and hide IOMMU device.
                                          ///< @li TRUE  = Initialize IOMMU subsystem. Generate ACPI IVRS table.
                                          ///< BldCfgItem{BLDCFG_IOMMU_SUPPORT}
  IN  UINT16     LvdsSpreadSpectrum;      ///< Spread spectrum value in 0.01 %
                                          ///< BldCfgItem{BLDCFG_GFX_LVDS_SPREAD_SPECTRUM}
  IN  UINT16     LvdsSpreadSpectrumRate;  ///< Spread spectrum frequency used by SS hardware logic in unit of 10Hz, 0 - default frequency 40kHz
                                          ///< BldCfgItem{BLDCFG_GFX_LVDS_SPREAD_SPECTRUM_RATE}
  IN  UINT8      LvdsPowerOnSeqDigonToDe;    ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_DIGON_TO_DE}
  IN  UINT8      LvdsPowerOnSeqDeToVaryBl;   ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_DE_TO_VARY_BL}
  IN  UINT8      LvdsPowerOnSeqDeToDigon;    ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_DE_TO_DIGON}
  IN  UINT8      LvdsPowerOnSeqVaryBlToDe;   ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWERS_ON_SEQ_VARY_BL_TO_DE}
  IN  UINT8      LvdsPowerOnSeqOnToOffDelay; ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_ON_TO_OFF_DELAY}
  IN  UINT8      LvdsPowerOnSeqVaryBlToBlon; ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_VARY_BL_TO_BLON}
  IN  UINT8      LvdsPowerOnSeqBlonToVaryBl; ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_BLON_TO_VARY_BL}
  IN  UINT16     LvdsMaxPixelClockFreq;      ///< This item configures the maximum pixel clock frequency supported.
                                             ///< @BldCfgItem{BLDCFG_LVDS_MAX_PIXEL_CLOCK_FREQ}
  IN  UINT32     LcdBitDepthControlValue;    ///< This item configures the LCD bit depth control settings.
                                             ///< @BldCfgItem{BLDCFG_LCD_BIT_DEPTH_CONTROL_VALUE}
  IN  UINT8      Lvds24bbpPanelMode;         ///< This item configures the LVDS 24 BBP mode.
                                             ///< @BldCfgItem{BLDCFG_LVDS_24BBP_PANEL_MODE}
  IN  LVDS_MISC_CONTROL      LvdsMiscControl;///< This item configures LVDS swap/Hsync/Vsync/BLON
  IN  UINT16     PcieRefClkSpreadSpectrum;   ///< Spread spectrum value in 0.01 %
                                             ///< @BldCfgItem{BLDCFG_PCIE_REFCLK_SPREAD_SPECTRUM}
  IN  BOOLEAN    GnbRemoteDisplaySupport;    ///< This item enables Wireless Display Support
                                             ///< @li TRUE  = Enable Wireless Display Support
                                             ///< @li FALSE = Disable Wireless Display Support
                                             ///< @BldCfgItem{BLDCFG_REMOTE_DISPLAY_SUPPORT}
  IN UINT8       LvdsMiscVoltAdjustment;     ///< Register LVDS_CTRL_4 to adjust LVDS output voltage
                                             ///< @BldCfgItem{BLDCFG_LVDS_MISC_VOL_ADJUSTMENT}
  IN DISPLAY_MISC_CONTROL DisplayMiscControl;///< This item configures display misc control
  IN DP_FIXED_VOLT_SWING_TYPE DpFixedVoltSwingType;///< To indicate fixed voltage swing value
                                                   ///< @BldCfgItem{BLDCFG_DP_FIXED_VOLT_SWING}
  IN  UINT32    GpuFrequencyLimit;           ///< GNB GPU Max Frequency(NULL if platform configured)
                                             ///< @BldCfgItem{BLDCFG_GPU_FREQUENCY_LIMIT}
  IN UINT8  EDPv1_4VSMode;                   ///< @BldCfgItem{BLDCFG_EDP_V1_4_VS_MODE}
  IN UINT8  ExtHDMIReDrvSlvAddr;             ///< @BldCfgItem{BLDCFG_EXT_HDMI_RE_DRIVE_SLAVE_ADDR}
  IN UINT8  ExtHDMIReDrvRegNum;              ///< @BldCfgItem{BLDCFG_EXT_HDMI_RE_DRIVE_REG_NUM}
  IN UINT64 ExtHDMIRegSetting;               ///< @BldCfgItem{BLDCFG_EXT_HDMI_RE_DRIVE_REG_INFO}
} GNB_ENV_CONFIGURATION;

/// Configuration settings for GNB.
typedef struct {
  IN  UINT8     iGpuVgaMode;                 ///< VGA resources decoding configuration for iGPU
                                             ///< @li 0 = iGPU decode all VGA resources (must be primary VGA adapter)
                                             ///< @li 1 = iGPU will not decode any VGA resources (must be secondary graphics adapter)
  IN  UINT8     PcieAcsCapability;           ///< Pcie ACS Capability support
                                             ///< @li 0 = Disabled
                                             ///< @li 1 = Enabled
  IN  UINT64    GnbIoapicAddress;            ///< GNB IOAPIC Base Address(NULL if platform configured)
                                             ///< @BldCfgItem{BLDCFG_GNB_IOAPIC_ADDRESS}
  IN  UINT8     MaxNumAudioEndpoints;        ///< Max number of audio endpoints
                                             ///< @BldCfgItem{BLDCFG_MAX_NUM_AUDIO_ENDPOINTS}
} GNB_MID_CONFIGURATION;

/// GNB configuration info
typedef struct {
  IN       PCIe_COMPLEX_DESCRIPTOR  *PcieComplexList;  /**< Pointer to array of structures describe PCIe topology on each processor package or NULL.
                                                        * Last element of array must be terminated with DESCRIPTOR_TERMINATE_LIST
                                                        * Example of topology definition for single socket system:
                                                        * @code
                                                        *  PCIe_PORT_DESCRIPTOR PortList [] = {
                                                        *    // Initialize Port descriptor (PCIe port, Lanes 8:15, PCI Device Number 2, ...)
                                                        *    {
                                                        *      0,   //Descriptor flags
                                                        *      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 8, 15),
                                                        *      PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 2, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 0)
                                                        *    },
                                                        *    // Initialize Port descriptor (PCIe port, Lanes 16:19, PCI Device Number 3, ...)
                                                        *    {
                                                        *      0,   //Descriptor flags
                                                        *      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 16, 19),
                                                        *      PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 3, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 0)
                                                        *    },
                                                        *    // Initialize Port descriptor (PCIe port, Lanes 4, PCI Device Number 4, ...)
                                                        *    {
                                                        *      DESCRIPTOR_TERMINATE_LIST, //Descriptor flags  !!!IMPORTANT!!! Terminate last element of array
                                                        *      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 4, 4),
                                                        *      PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 4, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 0)
                                                        *    }
                                                        *  };
                                                        *  PCIe_PORT_DESCRIPTOR DdiList [] = {
                                                        *    // Initialize Ddi descriptor (DDI interface Lanes 24:27, Display Port Connector, ...)
                                                        *    {
                                                        *      0,   //Descriptor flags
                                                        *      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 24, 27),
                                                        *      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux1, Hdp1, 0)
                                                        *    },
                                                        *    // Initialize Ddi descriptor (DDI interface Lanes 28:31, HDMI, ...)
                                                        *    {
                                                        *      DESCRIPTOR_TERMINATE_LIST, //Descriptor flags  !!!IMPORTANT!!! Terminate last element of array
                                                        *      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 28, 31),
                                                        *      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux2, Hdp2, 0)
                                                        *    }
                                                        *  };
                                                        * PCIe_COMPLEX_DESCRIPTOR PlatformTopology = {
                                                        *   DESCRIPTOR_TERMINATE_LIST, //Descriptor flags  !!!IMPORTANT!!! Terminate complexes list
                                                        *   0,  //Socket ID
                                                        *   &PortList[0],
                                                        *   &DdiList[0],
                                                        * }
                                                        * @endcode
                                                        */
  IN       UINT8                    PsppPolicy;         /**< PSPP (PCIe Speed Power Policy)
                                                         *  @li @b 0 - Disabled
                                                         *  @li @b 1 - Performance
                                                         *  @li @b 2 - Balance-High
                                                         *  @li @b 3 - Balance-Low
                                                         *  @li @b 4 - Power Saving
                                                         */

} GNB_CONFIGURATION;

/// Late Configuration settings for GNB.
typedef struct {
  IN        BOOLEAN                 Reserved;           ///< Reserved -- Docked TDP headroom
  IN        UINT8                   GnbIoapicId;        ///< GNB IOAPIC ID, platform BIOS needs to pass correct id number, default is 0xFF.
                                                        ///< If BLDCFG_GNB_IOAPIC_ADDRESS == NULL or BLDCFG_IOMMU_SUPPORT == NULL or GnbIoapicId == default(0xFF), AGESA will skip it anyway.
                                                        ///< @li 0~n = IOAPIC ID number for IVRS which should be matched with MADT
  IN        UINT8                   FchIoapicId;        ///< Fch IOAPIC ID, platform BIOS needs to pass correct id number, default is 0xFF.
                                                        ///< If BLDCFG_IOMMU_SUPPORT == NULL or or FchIoapicId == default(0xFF), AGESA will skip it anyway.
                                                        ///< @li 0~n = IOAPIC ID number for IVRS which should be matched with MADT

} GNB_LATE_CONFIGURATION;

//
//  MEMORY-SPECIFIC DATA STRUCTURES
//
//
//
//
// AGESA MAXIMIUM VALUES
//
//   These Max values are used to define array sizes and associated loop
//   counts in the code.  They reflect the maximum values that AGESA
//   currently supports and does not necessarily reflect the hardware
//   capabilities of configuration.
//

#define MAX_SOCKETS_SUPPORTED   2   ///< Max number of sockets in system
#define MAX_DIES_PER_SOCKET     4   ///< Max number of dies per socket
#define MAX_RBS_PER_SOCKET      4   ///< Max number of root bridges per socket
#define MAX_CHANNELS_PER_DIE    2   ///< Max Channels per die
#define MAX_CHANNELS_PER_SOCKET 8   ///< Max Channels per sockets
#define MAX_DIMMS_PER_CHANNEL   4   ///< Max DIMMs on a memory channel (independent of platform)
#define NUMBER_OF_DELAY_TABLES  9   ///< Number of tables defined in CH_DEF_STRUCT.
                                    ///< Eg: UINT16  *RcvEnDlys;
                                    ///<     UINT8   *WrDqsDlys;
                                    ///<     UINT8   *RdDqsDlys;
                                    ///<     UINT8   *WrDatDlys;
                                    ///<     UINT8   *RdDqsMinDlys;
                                    ///<     UINT8   *RdDqsMaxDlys;
                                    ///<     UINT8   *WrDatMinDlys;
                                    ///<     UINT8   *WrDatMaxDlys;
#define NUMBER_OF_FAILURE_MASK_TABLES 1 ///< Number of failure mask tables

#define MAX_PLATFORM_TYPES     16   ///< Platform types per system

#define MCT_TRNG_KEEPOUT_START  0x00004000ul    ///< base [39:8]
#define MCT_TRNG_KEEPOUT_END    0x00007FFFul    ///< base [39:8]
#define DATAEYE_VREF_RANGE     31   ///< Number of VREF steps in Data Eye Bitmap

#define UMA_ATTRIBUTE_INTERLEAVE 0x80000000ul   ///< Uma Region is interleaved
#define UMA_ATTRIBUTE_ON_DCT0    0x40000000ul   ///< UMA resides on memory that belongs to DCT0
#define UMA_ATTRIBUTE_ON_DCT1    0x20000000ul   ///< UMA resides on memory that belongs to DCT1
#define UMA_ATTRIBUTE_ON_DCT2    0x10000000ul   ///< UMA resides on memory that belongs to DCT2
#define UMA_ATTRIBUTE_ON_DCT3    0x08000000ul   ///< UMA resides on memory that belongs to DCT3

typedef UINT8 PSO_TABLE;            ///< Platform Configuration Table

//        AGESA DEFINITIONS
//
//        Many of these are derived from the platform and hardware specific definitions

/// EccSymbolSize override value
#define ECCSYMBOLSIZE_USE_BKDG      0   ///< Use BKDG Recommended Value
#define ECCSYMBOLSIZE_FORCE_X4      4   ///< Force to x4
#define ECCSYMBOLSIZE_FORCE_X8      8   ///< Force to x8
/// CPU Package Type
#define PT_L1       0                 ///< L1 Package type
#define PT_M2       1                 ///< AM Package type
#define PT_S1       2                 ///< S1 Package type

/// Build Configuration values for BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT & BLDCFG_MEMORY_CLOCK_SELECT
#define DDR400_FREQUENCY  200     ///< DDR 400
#define DDR533_FREQUENCY  266     ///< DDR 533
#define DDR667_FREQUENCY  333     ///< DDR 667
#define DDR800_FREQUENCY  400     ///< DDR 800
#define DDR1066_FREQUENCY 533     ///< DDR 1066
#define DDR1333_FREQUENCY 667     ///< DDR 1333
#define DDR1600_FREQUENCY 800     ///< DDR 1600
#define DDR1866_FREQUENCY 933     ///< DDR 1866
#define DDR2100_FREQUENCY 1050    ///< DDR 2100
#define DDR2133_FREQUENCY 1066    ///< DDR 2133
#define DDR2400_FREQUENCY 1200    ///< DDR 2400
#define UNSUPPORTED_DDR_FREQUENCY 1201 ///< Highest limit of DDR frequency

/// Build Configuration values for BLDCFG_TIMING_MODE_SELECT
#define TIMING_MODE_AUTO     0  ///< Use best rate possible
#define TIMING_MODE_LIMITED  1  ///< Set user top limit
#define TIMING_MODE_SPECIFIC 2  ///< Set user specified speed

/// Build Configuration values for BLDCFG_MEMORY_QUADRANK_TYPE
#define QUADRANK_REGISTERED  0  ///< Quadrank registered DIMM
#define QUADRANK_UNBUFFERED  1  ///< Quadrank unbuffered DIMM

/// Build Configuration values for BLDCFG_POWER_DOWN_MODE
#define POWER_DOWN_BY_CHANNEL      0  ///< Channel power down mode
#define POWER_DOWN_BY_CHIP_SELECT  1  ///< Chip select power down mode
#define POWER_DOWN_MODE_AUTO       2  ///< AGESA to select power down mode

/// Build Configuration limit for BLDCFG_GNB_GPU_MAX_FREQUENCY
#define UNSUPPORTED_GPU_FREQUENCY 901 ///< Highest limit of GPU frequency

/// Structures use to report AMP status
typedef struct {
  OUT   BOOLEAN AmpVoltageValid;                 ///< Indicates if Amp voltage is valid
  OUT   BOOLEAN AmpSupportDetectedButNotEnabled; ///< Indicates if Amp support is detected but not enabled
  OUT   BOOLEAN AmpSelectedButNotEnabled;        ///< Indicates if Amp is selected but not enabled
} AMP_STATUS;

/// Normalized Critical Composite Data Eye
///  Bit 15 represents trained eye Center
///  Bit 0  represents eye center -15 delay steps
///  Bit 31 represents eye center +16 delay steps
///  Offset 0  represents +15 Vref Steps
///  Offset 31  represents -15 Vref Steps
typedef UINT32 COMPOSITE_DATAEYE[DATAEYE_VREF_RANGE];

/// Build Configuration values for BLDCFG_AMD_PLATFORM_TYPE
typedef enum {
  AMD_PLATFORM_SERVER = 0x8000,     ///< Server
  AMD_PLATFORM_DESKTOP = 0x10000,   ///< Desktop
  AMD_PLATFORM_MOBILE = 0x20000,    ///< Mobile
} AMD_PLATFORM_TYPE;

/// Dram technology type
typedef enum {
  DDR2_TECHNOLOGY,        ///< DDR2 technology
  DDR3_TECHNOLOGY,        ///< DDR3 technology
  GDDR5_TECHNOLOGY,       ///< GDDR5 technology
  DDR4_TECHNOLOGY,        ///< DDR4 technology
  UNSUPPORTED_TECHNOLOGY, ///< Unsupported technology
} TECHNOLOGY_TYPE;

/// Low voltage support
typedef enum {
  VOLT_INITIAL,                     ///< Initial value for VDDIO
  VOLT1_5,                          ///< 1.5 Volt
  VOLT1_35,                         ///< 1.35 Volt
  VOLT1_25,                         ///< 1.25 Volt
  VOLT_DDR4_RANGE_START,            ///< Start of DDR4 Voltage Range
  VOLT1_2 = VOLT_DDR4_RANGE_START,  ///< 1.2 Volt
  VOLT_TBD1,                        ///< TBD1 Voltage
  VOLT_TBD2,                        ///< TBD2 Voltage
  VOLT_UNSUPPORTED = 0xFF           ///< No common voltage found
} DIMM_VOLTAGE;

/// AMP voltage support
typedef enum {
  AMP_VOLT_RSVD,             ///< Reserved
  AMP_VOLT1_5,               ///< 1.5 Volt
  AMP_VOLT1_55,              ///< 1.55 Volt
  AMP_VOLT1_6,               ///< 1.6 Volt
  AMP_VOLT1_65,              ///< 1.65 Volt
  AMP_VOLT1_7,               ///< 1.7 Volt
  AMP_VOLT1_75,              ///< 1.75 Volt
  AMP_VOLT1_8,               ///< 1.8 Volt
  AMP_VOLT1_85,              ///< 1.85 Volt
  AMP_VOLT1_9,               ///< 1.9 Volt
  AMP_VOLT1_45 = 0x10,       ///< 1.45 Volt
  AMP_VOLT1_4  = 0x20,       ///< 1.4 Volt
  AMP_VOLT1_35 = 0x30,       ///< 1.35 Volt
  AMP_VOLT1_3  = 0x40,       ///< 1.3 Volt
  AMP_VOLT1_25 = 0x50,       ///< 1.25 Volt
  AMP_VOLT1_2  = 0x60        ///< 1.2 Volt
} AMP_DIMM_VOLTAGE;

/// Build Configuration values for BLDCFG_RESOLUTION
typedef enum {
  DISPLAY_1920x1080_And_Below = 0,        ///< 1920x1080 and below
  DISPLAY_2560x1600 = 1,                  ///< 2560x1600
  DISPLAY_3840x2160 = 2                   ///< 3840x2160
} DISPLAY_RESOLUTION;

/// Build Configuration values for BLDCFG_ACP_SIZE
typedef enum {
  NO_ACP_SIZE = 0x00,        ///< NO ACP
  ACP_SIZE_2MB = 0x20,       ///< UMA 4MB aligned
  ACP_SIZE_4MB = 0x40,       ///< UMA 128MB aligned
} ACP_SIZE;

/// UMA Version
typedef enum {
  UMA_LEGACY = 0,              ///< UMA Legacy Version
  UMA_NON_LEGACY = 1,          ///< UMA Non Legacy Version
  UMA_HSFB = 2                 ///< UMA HSFB Version
} UMA_VERSION;

/// UMA Mode
typedef enum {
  UMA_NONE = 0,              ///< UMA None
  UMA_SPECIFIED = 1,         ///< UMA Specified
  UMA_AUTO = 2               ///< UMA Auto
} UMA_MODE;

/// Force Training Mode
typedef enum {
  FORCE_TRAIN_1D = 0,              ///< 1D Training only
  FORCE_TRAIN_2D = 1,              ///< 2D Training only
  FORCE_TRAIN_AUTO = 2             ///< Auto - 1D or 2D depending on configuration
} FORCE_TRAIN_MODE;

/// PMU Training Mode
typedef enum {
  PMU_TRAIN_1D = 0,                ///< PMU 1D Training only
  PMU_TRAIN_1D_2D_READ = 1,        ///< PMU 1D and 2D Training read only
  PMU_TRAIN_1D_2D_WRITE = 2,       ///< PMU 1D and 2D Training write only
  PMU_TRAIN_1D_2D = 3,             ///< PMU 1D and 2D Training
  PMU_TRAIN_AUTO = 0xFF            ///< Auto - PMU Training depend on configuration
} PMU_TRAIN_MODE;

/// PMU Training Mode
typedef enum {
  BANK_SWAP_ONLY_DISABLED = 0,      ///< Disable Bank Swap Only
  BANK_SWAP_ONLY_ENABLED = 1,       ///< Enable Bank Swap Only
  BANK_SWAP_ONLY_AUTO = 2           ///< Auto - BankSwapOnly depending on family specific configuration
} BANK_SWAP_ONLY_MODE;

/// Build Configuration values for BLDCFG_UMA_ALIGNMENT
typedef enum {
  NO_UMA_ALIGNED = 0x00FFFFFF,           ///< NO UMA aligned
  UMA_4MB_ALIGNED = 0x00FFFFC0,          ///< UMA 4MB aligned
  UMA_128MB_ALIGNED = 0x00FFF800,        ///< UMA 128MB aligned
  UMA_256MB_ALIGNED = 0x00FFF000,        ///< UMA 256MB aligned
  UMA_512MB_ALIGNED = 0x00FFE000,        ///< UMA 512MB aligned
} UMA_ALIGNMENT;

///  The possible memory power policy settings.
typedef enum  {
  Performance,                                   ///< Optimize for performance.
  BatteryLife,                                   ///< Optimize for battery life.
  Auto,                                          ///< Auto
  MaxPowerPolicy                                 ///< Not a power policy mode, use for limit checking.
} MEMORY_POWER_POLICY;

///
///   Global MCT Configuration Status Word (GStatus)
///
typedef enum {
  GsbMTRRshort,              ///< Ran out of MTRRs while mapping memory
  GsbAllECCDimms,            ///< All banks of all Nodes are ECC capable
  GsbDramECCDis,             ///< Dram ECC requested but not enabled.
  GsbSoftHole,               ///< A Node Base gap was created
  GsbHWHole,                 ///< A HW dram remap was created
  GsbNodeIntlv,              ///< Node Memory interleaving was enabled
  GsbSpIntRemapHole,         ///< Special condition for Node Interleave and HW remapping
  GsbEnDIMMSpareNW,          ///< Indicates that DIMM Spare can be used without a warm reset

  GsbEOL                     ///< End of list
} GLOBAL_STATUS_FIELD;

///
///   Local Error Status (DIE_STRUCT.ErrStatus[31:0])
///
typedef enum {
  EsbNoDimms,                  ///< No DIMMs
  EsbSpdChkSum,                ///< SPD Checksum fail
  EsbDimmMismatchM,            ///< dimm module type(buffer) mismatch
  EsbDimmMismatchT,            ///< dimm CL/T mismatch
  EsbDimmMismatchO,            ///< dimm organization mismatch (128-bit)
  EsbNoTrcTrfc,                ///< SPD missing Trc or Trfc info
  EsbNoCycTime,                ///< SPD missing byte 23 or 25
  EsbBkIntDis,                 ///< Bank interleave requested but not enabled
  EsbDramECCDis,               ///< Dram ECC requested but not enabled
  EsbSpareDis,                 ///< Online spare requested but not enabled
  EsbMinimumMode,              ///< Running in Minimum Mode
  EsbNoRcvrEn,                 ///< No DQS Receiver Enable pass window found
  EsbSmallRcvr,                ///< DQS Rcvr En pass window too small (far right of dynamic range)
  EsbNoDqsPos,                 ///< No DQS-DQ passing positions
  EsbSmallDqs,                 ///< DQS-DQ passing window too small
  EsbDCBKScrubDis,             ///< DCache scrub requested but not enabled

  EsbEMPNotSupported,          ///< Processor is not capable for EMP.
  EsbEMPConflict,               ///< EMP requested but cannot be enabled since
                               ///< channel interleaving, bank interleaving, or bank swizzle is enabled.
  EsbEMPDis,                   ///< EMP requested but cannot be enabled since
                               ///< memory size of each DCT is not a power of two.

  EsbEOL                       ///< End of list
} ERROR_STATUS_FIELD;

///
///  Local Configuration Status (DIE_STRUCT.Status[31:0])
///
typedef enum {
  SbRegistered,                ///< All DIMMs are Registered
  SbEccDimms,                  ///< All banks ECC capable
  SbParDimms,                  ///< All banks Addr/CMD Parity capable
  SbDiagClks,                  ///< Jedec ALL slots clock enable diag mode
  Sb128bitmode,                ///< DCT in 128-bit mode operation
  Sb64MuxedMode,               ///< DCT in 64-bit mux'ed mode.
  Sb2TMode,                    ///< 2T CMD timing mode is enabled.
  SbSWNodeHole,                ///< Remapping of Node Base on this Node to create a gap.
  SbHWHole,                    ///< Memory Hole created on this Node using HW remapping.
  SbOver400Mhz,                ///< DCT freq greater than or equal to 400MHz flag
  SbDQSPosPass2,               ///< Used for TrainDQSPos DIMM0/1, when freq greater than or equal to 400MHz
  SbDQSRcvLimit,               ///< Used for DQSRcvEnTrain to know we have reached the upper bound.
  SbExtConfig,                 ///< Indicate the default setting for extended PCI configuration support
  SbLrdimms,                   ///< All DIMMs are LRDIMMs

  SbEOL                        ///< End of list
} LOCAL_STATUS_FIELD;


///< CPU MSR Register definitions ------------------------------------------
#define SYS_CFG     0xC0010010ul
#define TOP_MEM     0xC001001Aul
#define TOP_MEM2    0xC001001Dul
#define HWCR        0xC0010015ul
#define NB_CFG      0xC001001Ful

#define FS_BASE     0xC0000100ul
#define IORR0_BASE  0xC0010016ul
#define IORR0_MASK  0xC0010017ul
#define BU_CFG      0xC0011023ul
#define BU_CFG2     0xC001102Aul
#define COFVID_STAT 0xC0010071ul
#define TSC         0x10

//-----------------------------------------------------------------------------
///
/// SPD Data for each DIMM.
///
typedef struct _SPD_DEF_STRUCT {
  IN BOOLEAN DimmPresent;  ///< Indicates that the DIMM is present and Data is valid
  IN UINT8   PageAddress;  ///< Indicates the 256 Byte EE Page the data belongs to
                           ///<      0 = Lower Page
                           ///<      1 = Upper Page (DDR4 Only)
  IN UINT8 Data[512];      ///< Buffer for 256 Bytes of SPD data from DIMM
} SPD_DEF_STRUCT;

//-----------------------------------------------------------------------------
///
/// VDDP_VDDR Voltage Info for Low Power DIMM
///
typedef struct _VDDP_VDDR_VOLTAGE {
  IN BOOLEAN IsValid;               ///< Indicates if daata is valid
  IN UINT8 Voltage;    ///< VDDP VDDR Voltage Value
} VDDP_VDDR_VOLTAGE;

///
/// Channel Definition Structure.
/// This data structure defines entries that are specific to the channel initialization
///
typedef struct _CH_DEF_STRUCT {
  OUT UINT8   ChannelID;         ///< Physical channel ID of a socket(0 = CH A, 1 = CH B, 2 = CH C, 3 = CH D)
  OUT TECHNOLOGY_TYPE TechType;  ///< Technology type of this channel
  OUT UINT8   ChDimmPresent;     ///< For each bit n 0..7, 1 = DIMM n is present.
                                 ///<  DIMM#  Select Signal
                                 ///<  0      MA0_CS_L[0, 1]
                                 ///<  1      MB0_CS_L[0, 1]
                                 ///<  2      MA1_CS_L[0, 1]
                                 ///<  3      MB1_CS_L[0, 1]
                                 ///<  4      MA2_CS_L[0, 1]
                                 ///<  5      MB2_CS_L[0, 1]
                                 ///<  6      MA3_CS_L[0, 1]
                                 ///<  7      MB3_CS_L[0, 1]

  OUT struct _DCT_STRUCT *DCTPtr; ///< Pointer to the DCT data of this channel.
  OUT struct _DIE_STRUCT *MCTPtr; ///< Pointer to the node data of this channel.
  OUT SPD_DEF_STRUCT *SpdPtr;    ///< Pointer to the SPD data for this channel. (Setup by NB Constructor)
  OUT SPD_DEF_STRUCT *DimmSpdPtr[MAX_DIMMS_PER_CHANNEL]; ///< Array of pointers to
                                 ///<   SPD Data for each Dimm. (Setup by Tech Block Constructor)
  OUT UINT8   ChDimmValid;       ///< For each bit n 0..3, 1 = DIMM n is valid and is/will be configured where 4..7 are reserved.
                                 ///<
  OUT UINT8   RegDimmPresent;   ///< For each bit n 0..3, 1 = DIMM n is a registered DIMM where 4..7 are reserved.
  OUT UINT8   LrDimmPresent;     ///< For each bit n 0..3, 1 = DIMM n is Load Reduced DIMM where 4..7 are reserved.
  OUT UINT8   SODimmPresent;     ///< For each bit n 0..3, 1 = DIMM n is a SO-DIMM, where 4..7 are reserved.
  OUT UINT8   Loads;             ///< Number of devices loading bus
  OUT UINT8   Dimms;             ///< Number of DIMMs loading Channel
  OUT UINT8   Ranks;             ///< Number of ranks loading Channel DATA
  OUT BOOLEAN SlowMode;          ///< 1T or 2T CMD mode (slow access mode)
  OUT BOOLEAN SlowModePs1;       ///< 1T or 2T CMD mode (slow access mode) for Mem Pstate 1
                                 ///< FALSE = 1T
                                 ///< TRUE = 2T
  ///< The following pointers will be pointed to dynamically allocated buffers.
  ///< Each buffer is two dimensional (RowCount x ColumnCount) and is lay-outed as in below.
  ///< Example: If DIMM and Byte based training, then
  ///< XX is a value in Hex
  ///<                        BYTE 0, BYTE 1, BYTE 2, BYTE 3, BYTE 4, BYTE 5, BYTE 6, BYTE 7, ECC BYTE
  ///<  Row1 -  Logical DIMM0    XX      XX      XX      XX      XX      XX      XX      XX      XX
  ///<  Row2 -  Logical DIMM1    XX      XX      XX      XX      XX      XX      XX      XX      XX
  OUT UINT16  *RcvEnDlys;       ///< DQS Receiver Enable Delays
  OUT UINT8   *WrDqsDlys;       ///< Write DQS delays (only valid for DDR3)
  OUT UINT8   *RdDqsDlys;       ///< Read Dqs delays
  OUT UINT8   *WrDatDlys;       ///< Write Data delays
  OUT UINT8   *RdDqs2dDlys;     ///< 2d Read DQS data
  OUT UINT8   *RdDqsMinDlys;    ///< Minimum Window for Read DQS
  OUT UINT8   *RdDqsMaxDlys;    ///< Maximum Window for Read DQS
  OUT UINT8   *WrDatMinDlys;    ///< Minimum Window for Write data
  OUT UINT8   *WrDatMaxDlys;    ///< Maximum Window for Write data
  OUT UINT16  *RcvEnDlysMemPs1;       ///< DQS Receiver Enable Delays for Mem Pstate 1
  OUT UINT8   *WrDqsDlysMemPs1;       ///< Write DQS delays (only valid for DDR3) for Mem Pstate 1
  OUT UINT8   *RdDqsDlysMemPs1;       ///< Read Dqs delays for Memory Pstate 1
  OUT UINT8   *WrDatDlysMemPs1;       ///< Write Data delays for Memory Pstate 1
  OUT UINT8   *RdDqs2dDlysMemPs1;     ///< 2d Read DQS data for Memory Pstate 1
  OUT UINT8   *RdDqsMinDlysMemPs1;    ///< Minimum Window for Read DQS for Memory Pstate 1
  OUT UINT8   *RdDqsMaxDlysMemPs1;    ///< Maximum Window for Read DQS for Memory Pstate 1
  OUT UINT8   *WrDatMinDlysMemPs1;    ///< Minimum Window for Write data for Memory Pstate 1
  OUT UINT8   *WrDatMaxDlysMemPs1;    ///< Maximum Window for Write data for Memory Pstate 1
  OUT UINT8   RowCount;         ///< Number of rows of the allocated buffer.
  OUT UINT8   ColumnCount;      ///< Number of columns of the allocated buffer.
  OUT UINT8   *FailingBitMask;    ///< Table of masks to Track Failing bits
  OUT UINT8   *FailingBitMaskMemPs1;    ///< Table of masks to Track Failing bits for Memory Pstate 1
  OUT VOID    *RdDataEyes;        ///< Pointer to Read Data Eye Bitmaps
  OUT VOID    *WrDataEyes;        ///< Pointer to Write Data Eye Bitmaps
  OUT UINT32  DctOdcCtl;          ///< Output Driver Strength (see BKDG FN2:Offset 9Ch, index 00h)
  OUT UINT32  DctAddrTmg;         ///< Address Bus Timing (see BKDG FN2:Offset 9Ch, index 04h)
  OUT UINT32  DctAddrTmgPs1;      ///< Address Bus Timing (see BKDG FN2:Offset 9Ch, index 04h) for Mem Pstate 1
  OUT UINT32  PhyRODTCSLow;       ///< Phy Read ODT Pattern Chip Select low (see BKDG FN2:Offset 9Ch, index 180h)
  OUT UINT32  PhyRODTCSHigh;      ///< Phy Read ODT Pattern Chip Select high (see BKDG FN2:Offset 9Ch, index 181h)
  OUT UINT32  PhyWODTCSLow;       ///< Phy Write ODT Pattern Chip Select low (see BKDG FN2:Offset 9Ch, index 182h)
  OUT UINT32  PhyWODTCSHigh;      ///< Phy Write ODT Pattern Chip Select high (see BKDG FN2:Offset 9Ch, index 183)
  OUT UINT8   PhyWLODT[4];        ///< Write Levelization ODT Pattern for Dimm 0-3 or CS 0-7(see BKDG FN2:Offset 9Ch, index 0x8[11:8])
  OUT UINT16  DctEccDqsLike;      ///< DCT DQS ECC UINT8 like...
  OUT UINT8   DctEccDqsScale;     ///< DCT DQS ECC UINT8 scale
  OUT UINT16  PtrPatternBufA;     ///< Ptr on stack to aligned DQS testing pattern
  OUT UINT16  PtrPatternBufB;     ///< Ptr on stack to aligned DQS testing pattern
  OUT UINT8   ByteLane;           ///< Current UINT8 Lane (0..7)
  OUT UINT8   Direction;          ///< Current DQS-DQ training write direction (0=read, 1=write)
  OUT UINT8   Pattern;            ///< Current pattern
  OUT UINT8   DqsDelay;           ///< Current DQS delay value
  OUT UINT16  HostBiosSrvc1;      ///< UINT16 sized general purpose field for use by host BIOS.  Scratch space.
  OUT UINT32  HostBiosSrvc2;      ///< UINT32 sized general purpose field for use by host BIOS.  Scratch space.
  OUT UINT16  DctMaxRdLat[4];     ///< Max Read Latency (ns) for the DCT
                                  ///< DctMaxRdLat [i] is for NBPstate i
  OUT UINT8   DIMMValidCh;        ///< DIMM# in CH
  OUT UINT8   MaxCh;              ///< Max number of CH in system
  OUT UINT8   Dct;                ///< Dct pointer
  OUT UINT8   WrDatGrossH;        ///< Write Data Gross delay high value
  OUT UINT8   DqsRcvEnGrossL;     ///< DQS Receive Enable Gross Delay low

  OUT UINT8   TrwtWB;             ///<  Non-SPD timing value for TrwtWB
  OUT UINT8   CurrRcvrDctADelay;  ///< for keep current RcvrEnDly
  OUT UINT16  T1000;              ///< get the T1000 figure (cycle time (ns) * 1K)
  OUT UINT8   DqsRcvEnPass;       ///< for TrainRcvrEn UINT8 lane pass flag
  OUT UINT8   DqsRcvEnSaved;      ///< for TrainRcvrEn UINT8 lane saved flag
  OUT UINT8   SeedPass1Remainder; ///< for Phy assisted DQS receiver enable training

  OUT UINT8   ClToNbFlag;         ///< is used to restore ClLinesToNbDis bit after memory
  OUT UINT32  NodeSysBase;        ///< for channel interleave usage
  OUT UINT8   RefRawCard[MAX_DIMMS_PER_CHANNEL];   ///< Array of rawcards detected
  OUT UINT8   CtrlWrd02[MAX_DIMMS_PER_CHANNEL];    ///< Control Word 2 values per DIMM
  OUT UINT8   CtrlWrd03[MAX_DIMMS_PER_CHANNEL];    ///< Control Word 3 values per DIMM
  OUT UINT8   CtrlWrd04[MAX_DIMMS_PER_CHANNEL];    ///< Control Word 4 values per DIMM
  OUT UINT8   CtrlWrd05[MAX_DIMMS_PER_CHANNEL];    ///< Control Word 5 values per DIMM
  OUT UINT8   CtrlWrd08[MAX_DIMMS_PER_CHANNEL];    ///< Control Word 8 values per DIMM

  OUT UINT16  CsPresentDCT;       ///< For each bit n 0..7, 1 = Chip-select n is present
  OUT UINT8   DimmMirrorPresent;  ///< For each bit n 0..3, 1 = DIMM n is OnDimmMirror capable where 4..7 are reserved.
  OUT UINT8   DimmSpdCse;         ///< For each bit n 0..3, 1 = DIMM n SPD checksum error where 4..7 are reserved.
  OUT UINT8   DimmExclude;        ///< For each bit n 0..3, 1 = DIMM n gets excluded where 4..7 are reserved.
  OUT UINT8   DimmYr06;           ///< Bitmap indicating which Dimms have a manufacturer's year code <= 2006
  OUT UINT8   DimmWk2406;         ///< Bitmap indicating which Dimms have a manufacturer's week code <= 24 of 2006 (June)
  OUT UINT8   DimmPlPresent;      ///< Bitmap indicating that Planar (1) or Stacked (0) Dimms are present.
  OUT UINT8   DimmQrPresent;      ///< QuadRank DIMM present?
  OUT UINT8   DimmDrPresent;      ///< Bitmap indicating that Dual Rank Dimms are present
  OUT UINT8   DimmSRPresent;      ///< Bitmap indicating that Single Rank Dimms are present
  OUT UINT8   Dimmx4Present;      ///< For each bit n 0..3, 1 = DIMM n contains x4 data devices. where 4..7 are reserved.
  OUT UINT8   Dimmx8Present;      ///< For each bit n 0..3, 1 = DIMM n contains x8 data devices. where 4..7 are reserved.
  OUT UINT8   Dimmx16Present;     ///< For each bit n 0..3, 1 = DIMM n contains x16 data devices. where 4..7 are reserved.
  OUT UINT8   LrdimmPhysicalRanks[MAX_DIMMS_PER_CHANNEL];///< Number of Physical Ranks for LRDIMMs
  OUT UINT8   LrDimmLogicalRanks[MAX_DIMMS_PER_CHANNEL];///< Number of LRDIMM Logical ranks in this configuration
  OUT UINT8   LrDimmRankMult[MAX_DIMMS_PER_CHANNEL];///< Rank Multipication factor per dimm.
  OUT UINT8   DimmNibbleAccess;   ///< For each bit n 0..3, 1 = DIMM n will use nibble signaling. Where 4..7 are reserved.
  OUT UINT8   *MemClkDisMap;      ///<  This pointer will be set to point to an array that describes
                                  ///<  the routing of M[B,A]_CLK pins to the DIMMs' ranks. AGESA will
                                  ///<  base on this array to disable unused MemClk to save power.
                                  ///<
                                  ///<  The array must have 8 entries. Each entry, which associates with
                                  ///<  one MemClkDis bit, is a bitmap of 8 CS that that MemClk is routed to.
                                  ///<    Example:
                                  ///<    BKDG definition of Fn2x88[MemClkDis] bitmap for AM3 package
                                  ///<    is like below:
                                  ///<         Bit AM3/S1g3 pin name
                                  ///<         0   M[B,A]_CLK_H/L[0]
                                  ///<         1   M[B,A]_CLK_H/L[1]
                                  ///<         2   M[B,A]_CLK_H/L[2]
                                  ///<         3   M[B,A]_CLK_H/L[3]
                                  ///<         4   M[B,A]_CLK_H/L[4]
                                  ///<         5   M[B,A]_CLK_H/L[5]
                                  ///<         6   M[B,A]_CLK_H/L[6]
                                  ///<         7   M[B,A]_CLK_H/L[7]
                                  ///<    And platform has the following routing:
                                  ///<         CS0   M[B,A]_CLK_H/L[4]
                                  ///<         CS1   M[B,A]_CLK_H/L[2]
                                  ///<         CS2   M[B,A]_CLK_H/L[3]
                                  ///<         CS3   M[B,A]_CLK_H/L[5]
                                  ///<    Then MemClkDisMap should be pointed to the following array:
                                  ///<               CLK_2 CLK_3 CLK_4 CLK_5
                                  ///<    0x00, 0x00, 0x02, 0x04, 0x01, 0x08, 0x00, 0x00
                                  ///<  Each entry of the array is the bitmask of 8 chip selects.

  OUT UINT8   *CKETriMap;         ///<  This pointer will be set to point to an array that describes
                                  ///<  the routing of CKE pins to the DIMMs' ranks.
                                  ///<  The array must have 2 entries. Each entry, which associates with
                                  ///<  one CKE pin, is a bitmap of 8 CS that that CKE is routed to.
                                  ///<  AGESA will base on this array to disable unused CKE pins to save power.

  OUT UINT8   *ODTTriMap;         ///<  This pointer will be set to point to an array that describes
                                  ///<  the routing of ODT pins to the DIMMs' ranks.
                                  ///<  The array must have 4 entries. Each entry, which associates with
                                  ///<  one ODT pin, is a bitmap of 8 CS that that ODT is routed to.
                                  ///<  AGESA will base on this array to disable unused ODT pins to save power.

  OUT UINT8   *ChipSelTriMap;     ///<  This pointer will be set to point to an array that describes
                                  ///<  the routing of chip select pins to the DIMMs' ranks.
                                  ///<  The array must have 8 entries. Each entry is a bitmap of 8 CS.
                                  ///<  AGESA will base on this array to disable unused Chip select pins to save power.

  OUT UINT8   DimmSRTPresent;     ///< For each bit n 0..3, 1 = DIMM n supports Extended Temperature Range where 4..7 are reserved
  OUT UINT8   DimmASRPresent;     ///< For each bit n 0..3, 1 = DIMM n supports Auto Self Refresh where 4..7 are reserved
  OUT UINT8   DimmThermSensorPresent;  ///< For each bit n 0..3, 1 = DIMM n has an On Dimm Thermal Sensor where 4..7 are reserved
  OUT UINT8   MaxVref;            ///<  Maximum Vref Value for channel
  OUT UINT8   Reserved[100];      ///< Reserved
} CH_DEF_STRUCT;

///
/// DCT Channel Timing Parameters.
/// This data structure sets timings that are specific to the channel.
///
typedef struct _CH_TIMING_STRUCT {
  OUT UINT16  DctDimmValid;       ///< For each bit n 0..3, 1=DIMM n is valid and is/will be configured where 4..7 are reserved.
  OUT UINT16  DimmMirrorPresent;  ///< For each bit n 0..3, 1=DIMM n is OnDimmMirror capable where 4..7 are reserved.
  OUT UINT16  DimmSpdCse;         ///< For each bit n 0..3, 1=DIMM n SPD checksum error where 4..7 are reserved.
  OUT UINT16  DimmExclude;        ///< For each bit n 0..3, 1 = DIMM n gets excluded where 4..7 are reserved.
  OUT UINT16  CsPresent;          ///< For each bit n 0..7, 1=Chip-select n is present
  OUT UINT16  CsEnabled;          ///< For each bit n 0..7, 1=Chip-select n is enabled
  OUT UINT16  CsTestFail;         ///< For each bit n 0..7, 1=Chip-select n is present but disabled
  OUT UINT16  CsTrainFail;        ///< Bitmap showing which chipselects failed training
  OUT UINT16  DIMM1KPage;         ///< For each bit n 0..3, 1=DIMM n contains 1K page devices. where 4..7 are reserved
  OUT UINT16  DimmQrPresent;      ///< QuadRank DIMM present?
  OUT UINT16  DimmDrPresent;      ///< Bitmap indicating that Dual Rank Dimms are present , where 4..7 are reserved
  OUT UINT8   DimmSRPresent;      ///< Bitmap indicating that Single Rank Dimms are present, where 4..7 are reserved
  OUT UINT16  Dimmx4Present;      ///< For each bit n 0..3, 1=DIMM n contains x4 data devices. where 4..7 are reserved
  OUT UINT16  Dimmx8Present;      ///< For each bit n 0..3, 1=DIMM n contains x8 data devices. where 4..7 are reserved
  OUT UINT16  Dimmx16Present;     ///< For each bit n 0..3, 1=DIMM n contains x16 data devices. where 4..7 are reserved

  OUT UINT16  DIMMTrcd;           ///< Minimax Trcd*40 (ns) of DIMMs
  OUT UINT16  DIMMTrp;            ///< Minimax Trp*40 (ns) of DIMMs
  OUT UINT16  DIMMTrtp;           ///< Minimax Trtp*40 (ns) of DIMMs
  OUT UINT16  DIMMTras;           ///< Minimax Tras*40 (ns) of DIMMs
  OUT UINT16  DIMMTrc;            ///< Minimax Trc*40 (ns) of DIMMs
  OUT UINT16  DIMMTwr;            ///< Minimax Twr*40 (ns) of DIMMs
  OUT UINT16  DIMMTrrd;           ///< Minimax Trrd*40 (ns) of DIMMs
  OUT UINT16  DIMMTwtr;           ///< Minimax Twtr*40 (ns) of DIMMs
  OUT UINT16  DIMMTfaw;           ///< Minimax Tfaw*40 (ns) of DIMMs
  OUT UINT16  DIMMTrrdL;          ///< Minimax TrrdL*40 (ns) of DIMMs
  OUT UINT16  DIMMTwtrL;          ///< Minimax TtwrL*40 (ns) of DIMMs
  OUT UINT16  DIMMTccdL;          ///< Minimax TccdL*40 (ns) of DIMMs
  OUT UINT16  TargetSpeed;        ///< Target DRAM bus speed in MHz
  OUT UINT16  Speed;              ///< DRAM bus speed in MHz
                                  ///<  400 (MHz)
                                  ///<  533 (MHz)
                                  ///<  667 (MHz)
                                  ///<  800 (MHz)
                                  ///<  and so on...
  OUT UINT8   Trcpage;            ///< DCT Trcpage (10 ns)
  OUT UINT8   CasL;               ///< CAS latency DCT setting (busclocks)
  OUT UINT8   Trcd;               ///< DCT Trcd (busclocks)
  OUT UINT8   Trp;                ///< DCT Trp (busclocks)
  OUT UINT8   Trtp;               ///< DCT Trtp (busclocks)
  OUT UINT8   Tras;               ///< DCT Tras (busclocks)
  OUT UINT8   Trc;                ///< DCT Trc (busclocks)
  OUT UINT8   Twr;                ///< DCT Twr (busclocks)
  OUT UINT8   Trrd;               ///< DCT Trrd (busclocks)
  OUT UINT8   Twtr;               ///< DCT Twtr (busclocks)
  OUT UINT8   Tfaw;               ///< DCT Tfaw (busclocks)
  OUT UINT8   TrrdL;              ///< DCT TrrdL (busclocks)
  OUT UINT8   TwtrL;              ///< DCT TwtrL (busclocks)
  OUT UINT8   TccdL;              ///< DCT TccdL (busclocks)
  OUT UINT16  Trfc0;              ///< DCT Logical DIMM0 Trfc (in ns)
  OUT UINT16  Trfc1;              ///< DCT Logical DIMM1 Trfc (in ns)
  OUT UINT16  Trfc2;              ///< DCT Logical DIMM2 Trfc (in ns)
  OUT UINT16  Trfc3;              ///< DCT Logical DIMM3 Trfc (in ns)
  OUT UINT16  Trfc4;              ///< DCT Trfc4min All DIMMS (in ns) - DDR4 Only
  OUT UINT32  DctMemSize;         ///< Base[47:16], total DRAM size controlled by this DCT.
                                  ///<
  OUT BOOLEAN SlowMode;           ///< 1T or 2T CMD mode (slow access mode)
                                  ///< FALSE = 1T
                                  ///< TRUE = 2T
  OUT UINT8   TrwtTO;             ///< DCT TrwtTO (busclocks)
  OUT UINT8   Twrrd;              ///< DCT Twrrd (busclocks)
  OUT UINT8   Twrwr;              ///< DCT Twrwr (busclocks)
  OUT UINT8   Trdrd;              ///< DCT Trdrd (busclocks)
  OUT UINT8   TrwtWB;             ///< DCT TrwtWB (busclocks)
  OUT UINT8   TrdrdSD;            ///< DCT TrdrdSD (busclocks)
  OUT UINT8   TwrwrSD;            ///< DCT TwrwrSD (busclocks)
  OUT UINT8   TwrrdSD;            ///< DCT TwrrdSD (busclocks)
  OUT UINT16  MaxRdLat0;          ///< Max Read Latency 0
  OUT UINT16  MaxRdLat1;          ///< Max Read Latency 1
  OUT UINT16  MaxRdLat2;          ///< Max Read Latency 2
  OUT UINT16  MaxRdLat3;          ///< Max Read Latency 3
  OUT UINT8   WrDatGrossH;        ///< Temporary variables must be removed
  OUT UINT8   DqsRcvEnGrossL;     ///< Temporary variables must be removed
  OUT UINT8   RdOdtOnDuration;    ///< RdOdtOnDuration
  OUT UINT8   WrOdtOnDuration;    ///< WrOdtOnDuration
} CH_TIMING_STRUCT;

///
/// Data for each DCT.
/// This data structure defines data used to configure each DRAM controller.
///
typedef struct _DCT_STRUCT {
  OUT UINT8   Dct;                ///< Current Dct
  OUT CH_TIMING_STRUCT Timings;   ///< Channel Timing structure
  OUT CH_TIMING_STRUCT *TimingsMemPs1;   ///< Pointed to channel timing structure for memory Pstate 1
  OUT CH_DEF_STRUCT    *ChData;   ///< Pointed to a dynamically allocated array of Channel structures
  OUT UINT8   ChannelCount;       ///< Number of channel per this DCT
  OUT BOOLEAN BkIntDis;           ///< Bank interleave requested but not enabled on current DCT
  OUT UINT8 BankAddrMap;          ///< Bank Address Mapping
  OUT UINT8 EnabledChipSels;      ///< Number of enabled chip selects on current DCT
} DCT_STRUCT;


///
/// Data Structure defining each Die.
/// This data structure contains information that is used to configure each Die.
///
typedef struct _DIE_STRUCT {

  /// Advanced:

  OUT UINT8   NodeId;              ///< Node ID of current controller
  OUT UINT8   SocketId;            ///< Socket ID of this Die
  OUT UINT8   DieId;               ///< ID of this die relative to the socket
  OUT PCI_ADDR      PciAddr;       ///< Pci bus and device number of this controller.
  OUT AGESA_STATUS  ErrCode;       ///< Current error condition of Node
                                   ///<  0x0 = AGESA_SUCCESS
                                   ///<  0x1 = AGESA_UNSUPPORTED
                                   ///<  0x2 = AGESA_BOUNDS_CHK
                                   ///<  0x3 = AGESA_ALERT
                                   ///<  0x4 = AGESA_WARNING
                                   ///<  0x5 = AGESA_ERROR
                                   ///<  0x6 = AGESA_CRITICAL
                                   ///<  0x7 = AGESA_FATAL
                                   ///<
  OUT BOOLEAN ErrStatus[EsbEOL];   ///< Error Status bit Field
                                   ///<
  OUT BOOLEAN Status[SbEOL];       ///< Status bit Field
                                   ///<
  OUT UINT32  NodeMemSize;         ///< Base[47:16], total DRAM size controlled by both DCT0 and DCT1 of this Node.
                                   ///<
  OUT UINT32  NodeSysBase;         ///< Base[47:16] (system address) DRAM base address of this Node.
                                   ///<
  OUT UINT32  NodeHoleBase;        ///< If not zero, Base[47:16] (system address) of dram hole for HW remapping.  Dram hole exists on this Node
                                   ///<
  OUT UINT32  NodeSysLimit;        ///< Base[47:16] (system address) DRAM limit address of this Node.
                                   ///<
  OUT UINT32  DimmPresent;         ///< For each bit n 0..7, 1 = DIMM n is present.
                                   ///<   DIMM#  Select Signal
                                   ///<   0      MA0_CS_L[0, 1]
                                   ///<   1      MB0_CS_L[0, 1]
                                   ///<   2      MA1_CS_L[0, 1]
                                   ///<   3      MB1_CS_L[0, 1]
                                   ///<   4      MA2_CS_L[0, 1]
                                   ///<   5      MB2_CS_L[0, 1]
                                   ///<   6      MA3_CS_L[0, 1]
                                   ///<   7      MB3_CS_L[0, 1]
                                   ///<
  OUT UINT32  DimmValid;           ///< For each bit n 0..7, 1 = DIMM n is valid and is / will be configured
  OUT UINT32  RegDimmPresent;      ///< For each bit n 0..7, 1 = DIMM n is registered DIMM
  OUT UINT32  LrDimmPresent;       ///< For each bit n 0..7, 1 = DIMM n is Load Reduced DIMM
  OUT UINT32  DimmEccPresent;      ///< For each bit n 0..7, 1 = DIMM n is ECC capable.
  OUT UINT32  DimmParPresent;      ///< For each bit n 0..7, 1 = DIMM n is ADR/CMD Parity capable.
                                   ///<
  OUT UINT16  DimmTrainFail;       ///< Bitmap showing which dimms failed training
  OUT UINT16  ChannelTrainFail;    ///< Bitmap showing the channel information about failed Chip Selects
                                   ///<  0 in any bit field indicates Channel 0
                                   ///<  1 in any bit field indicates Channel 1
  OUT UINT8   Dct;                 ///<  Need to be removed
                                   ///<  DCT pointer
  OUT BOOLEAN GangedMode;          ///< Ganged mode
                                   ///<  0 = disabled
                                   ///<  1 = enabled
  OUT SOC_LOGICAL_ID LogicalCpuid; ///< The logical CPUID of the node
                                   ///<
  OUT UINT16  HostBiosSrvc1;       ///< UINT16 sized general purpose field for use by host BIOS.  Scratch space.
                                   ///<
  OUT UINT32  HostBiosSrvc2;       ///< UINT32 sized general purpose field for use by host BIOS.  Scratch space.
                                   ///<
  OUT UINT8   MLoad;               ///< Need to be removed
                                   ///< Number of devices loading MAA bus
                                   ///<
  OUT UINT8   MaxAsyncLat;         ///< Legacy wrapper
                                   ///<
  OUT UINT8   ChbD3Rcvrdly;        ///< Legacy wrapper
                                   ///<
  OUT UINT16  ChaMaxRdLat;         ///< Max Read Latency (ns) for DCT 0
                                   ///<
  OUT UINT8   ChbD3BcRcvrdly;      ///< CHB DIMM 3 Check UINT8 Receiver Enable Delay

  OUT DCT_STRUCT *DctData;         ///< Pointed to a dynamically allocated array of DCT_STRUCTs
  OUT UINT8   DctCount;            ///< Number of DCTs per this Die
  OUT UINT8   Reserved[16];        ///< Reserved
} DIE_STRUCT;

/**********************************************************************
 * S3 data block structure
 **********************************************************************/
/// AmdInitResume, AmdS3LateRestore, and AmdInitRtb param structure
typedef struct {
     OUT   UINT32 Signature;           ///< "ASTR" for AMD Suspend-To-RAM
     OUT   UINT16 Version;             ///< S3 Params version number
  IN OUT   UINT32 Flags;               ///< Indicates operation
  IN OUT   VOID   *NvStorage;          ///< Pointer to memory critical save state data
  IN OUT   UINT32 NvStorageSize;       ///< Size in bytes of the NvStorage region
  IN OUT   VOID   *VolatileStorage;    ///< Pointer to remaining AMD save state data
  IN OUT   UINT32 VolatileStorageSize; ///< Size in bytes of the VolatileStorage region
} S3_DATA_BLOCK;

/// Header at the beginning of a context save buffer.
typedef struct {
  _2BYTE_ALIGN UINT16 Version; ///< Version of header
  _4BYTE_ALIGN UINT32 Revision; ///< Revision of the S3 data blob
  _2BYTE_ALIGN UINT16 NumDevices; ///< Number of devices in the list
  _2BYTE_ALIGN UINT16 RelativeOrMaskOffset; ///< Size of device list + header
  _4BYTE_ALIGN UINT32 BlobSize; ///<Size of the whole S3 data blob (including the header)
  _4BYTE_ALIGN UINT32 NextBlockOffset; ///< Size of the whole device save context
} DEVICE_BLOCK_HEADER;

///===============================================================================
/// CPU_VREF_OVERRIDE
///
typedef struct _CPU_VREF_OVERRIDE{
  IN UINT8 VrefOp;     ///< Operater to adjust VrefHspeed
  IN UINT8 VrefOffset; ///< Offset to adjust VrefHspeed
} CPU_VREF_OVERRIDE;

///===============================================================================
/// MEM_PARAMETER_STRUCT
/// This data structure is used to pass wrapper parameters to the memory configuration code
///
typedef struct _MEM_PARAMETER_STRUCT {

  // Basic (Return parameters)
  // (This section contains the outbound parameters from the memory init code)

  OUT BOOLEAN GStatus[GsbEOL]; ///< Global Status bitfield.
                               ///<
  OUT UINT32 HoleBase; ///< If not zero Base[47:16] (system address) of sub 4GB dram hole for HW remapping.
                       ///<
  OUT UINT32 Sub4GCacheTop; ///< If not zero, the 32-bit top of cacheable memory.
                       ///<
  OUT UINT32 Sub1THoleBase; ///< If not zero Base[47:16] (system address) of sub 1TB dram hole.
                       ///<
  OUT UINT32 SysLimit; ///< Limit[47:16] (system address).
                       ///<
  OUT DIMM_VOLTAGE DDRVoltage;   ///< Find support voltage and send back to platform BIOS for DDR3 or DDR4.
                        ///<
  OUT VDDP_VDDR_VOLTAGE VddpVddrVoltage; ///< For a given configuration, request is made to change the VDDP/VDDR
                                        ///< voltage in platform BIOS via AgesaHookBeforeDramInit callout and
                                        ///< MEM_PARAMETER_STRUCT.VddpVddrVoltage.Voltage parameter if
                                        ///< MEM_PARAMETER_STRUCT.VddpVddrVoltage.IsValid is TRUE. The
                                        ///< MEM_PARAMETER_STRUCT.VddpVddrVoltage.Voltage is defined in
                                        ///< MEMORY_PHY_VOLTAGE
  OUT UINT8 ExternalVrefValue; ///< Target reference voltage for external Vref for 2D training
                               ///<
  OUT struct _MEM_DATA_STRUCT *MemData; ///< Access to global memory init data.

  //  Advanced (Optional parameters)
  //  Optional (all defaults values will be initialized by the
  //  'AmdMemInitDataStructDef' based on AMD defaults. It is up
  //  to the IBV/OEM to change the defaults after initialization
  //  but prior to the main entry to the memory code):

  // Memory Map/Mgt.

  IN  UINT16 BottomIo; ///< Bottom of 32-bit IO space (8-bits).
                      ///<   NV_BOTTOM_IO[7:0]=Addr[31:24]
                     ///<
  IN  BOOLEAN MemHoleRemapping; ///< Memory Hole Remapping (1-bit).
                                  ///<  FALSE = disable
                                  ///<  TRUE  = enable
                                  ///<
  IN  BOOLEAN LimitMemoryToBelow1Tb; ///< Limit memory address space to below 1 TB
                                     ///<  FALSE = disable
                                     ///<  TRUE  = enable
                                     ///<
                                     ///< @BldCfgItem{BLDCFG_LIMIT_MEMORY_TO_BELOW_1TB}

                                     // Dram Timing

  IN  UINT32 UserTimingMode; ///< User Memclock Mode.
                                     ///< @BldCfgItem{BLDCFG_TIMING_MODE_SELECT}

  IN  UINT32 MemClockValue; ///< Memory Clock Value.
                                  ///< @BldCfgItem{BLDCFG_MEMORY_CLOCK_SELECT}

                                  // Dram Configuration
  OUT UINT32 TargetSpeed;         // Target memory speed
  IN  UINT16  CustomVddioVoltage;               ///< Custom VDDIO voltage
  IN  UINT8   CustomCasLatency;   ///< Custom CAS Latency for DRAM
  IN  BANK_SWAP_ONLY_MODE EnableBankSwapOnly; ///< Bank Swap Only
                                  ///<  - FALSE =disable (default)
                                  ///<  - TRUE = enable
                                  ///<
                                  ///< @BldCfgItem{BLDCFG_MEMORY_ENABLE_BANK_SWAP_ONLY}

  IN  BOOLEAN EnableBankIntlv; ///< Dram Bank (chip-select) Interleaving (1-bit).
                                  ///<  - FALSE =disable (default)
                                  ///<  - TRUE = enable
                                  ///<
                                  ///< @BldCfgItem{BLDCFG_MEMORY_ENABLE_BANK_INTERLEAVING}

  IN  BOOLEAN EnableNodeIntlv; ///< Node Memory Interleaving (1-bit).
                              ///<   - FALSE = disable (default)
                              ///<   - TRUE = enable
                              ///<
                              ///< @BldCfgItem{BLDCFG_MEMORY_ENABLE_NODE_INTERLEAVING}

  IN  BOOLEAN EnableChannelIntlv; ///< Channel Interleaving (1-bit).
                              ///<   - FALSE = disable (default)
                              ///<   - TRUE = enable
                              ///<
                              ///< @BldCfgItem{BLDCFG_MEMORY_CHANNEL_INTERLEAVING}
  // DllPDBypassMode

  IN  BOOLEAN EnableDllPDBypassMode; ///< Enable low-power DDR phy operation. This feature is used for low-power
                                ///< solder-down DRAM motherboard designs with route matched CK/DQS/DQ signals.
                                ///< It limits maximim achieveable DDR rates on the platform and should not be
                                ///< enabled for systems requiring high DDR rate operation and/or DIMM-based systems.
                                ///<   - FALSE = disable
                                ///<   - TRUE = enable (default)
                                ///<
                                ///< @BldCfgItem{BLDCFG_DDR_PHY_DLL_BYPASS_MODE}
  // ECC

  IN  BOOLEAN EnableEccFeature; ///< enable ECC error to go into MCE.
                              ///<   - FALSE = disable (default)
                              ///<   - TRUE = enable
                              ///<
                              ///< @BldCfgItem{BLDCFG_ENABLE_ECC_FEATURE}
  // Dram Power

  IN  BOOLEAN EnablePowerDown; ///< CKE based power down mode (1-bit).
                              ///<   - FALSE =disable (default)
                              ///<   - TRUE =enable
                             ///<
                             ///< @BldCfgItem{BLDCFG_MEMORY_POWER_DOWN}

  // Dram Mac Default

  IN  UINT8 DramMacDefault;  ///< Default Maximum Activate Count
                             ///<
                             ///< @BldCfgItem{BLDCFG_MEMORY_ALTERNATIVE_MAX_ACTIVATE_COUNT}

  // Dram Extended Temperature Range

  IN  BOOLEAN EnableExtendedTemperatureRange; ///< enable extended temperature support.
                              ///<   - FALSE =disable (default)
                              ///<   - TRUE =enable
                             ///<
                             ///< @BldCfgItem{BLDCFG_MEMORY_EXTENDED_TEMPERATURE_RANGE}

  // Temperature Controlled Refresh

  IN BOOLEAN DramTempControlledRefreshEn; ///< Enable Temperature Controlled Refresh
                                          ///< - FALSE = Disable
                                          ///< - TRUE = Enable (Default)
                                          ///< @BldCfgItem{BLDCFG_DRAM_TEMP_CONTROLLED_REFRESH_EN}
                                          ///< If EnableExtendedTemperatureRange is enabled with this feature
                                          ///< then CfgDramDoubleRefreshrate must also be enabled.

  // Online Spare

  IN  BOOLEAN EnableOnLineSpareCtl; ///< Chip Select Spare Control bit 0.
                              ///<  - FALSE = disable Spare (default)
                              ///<  - TRUE = enable Spare
                              ///<
                              ///< @BldCfgItem{BLDCFG_ONLINE_SPARE}

  IN  UINT8 *TableBasedAlterations; ///< Desired modifications to register settings.

  IN  PSO_TABLE *PlatformMemoryConfiguration;
  ///< A table that contains platform specific settings.
  ///< For example, MemClk routing, the number of DIMM slots per channel, ....
  ///< AGESA initializes this pointer with DefaultPlatformMemoryConfiguration that
  ///< contains default conservative settings. Platform BIOS can either tweak
  ///< DefaultPlatformMemoryConfiguration or reassign this pointer to its own table.
  ///<
  IN  BOOLEAN EnableParity; ///< Parity control.
                               ///<  - TRUE = enable
                               ///<  - FALSE = disable (default)
                               ///<
                               ///< @BldCfgItem{BLDCFG_MEMORY_PARITY_ENABLE}

  IN  BOOLEAN EnableBankSwizzle; ///< BankSwizzle control.
                               ///<  - FALSE = disable
                               ///<  - TRUE = enable  (default)
                              ///<
                              ///< @BldCfgItem{BLDCFG_BANK_SWIZZLE}
                              ///<

  IN  BOOLEAN EnableMemClr; ///< Memory Clear functionality control.
                              ///<  - FALSE = disable
                              ///<  - TRUE = enable  (default)
                              ///<
  // Uma Configuration

  IN  UMA_VERSION UmaVersion; ///< Uma Version
                              ///<  0 = Legacy Version
                              ///<  1 = Non-Legacy Version
  IN  UMA_MODE UmaMode; ///<  Uma Mode
                       ///<  0 = None
                       ///<  1 = Specified
                       ///<  2 = Auto
  IN OUT  UINT32 UmaSize; ///<  The size of shared graphics dram (16-bits)
                     ///<  NV_UMA_Size[31:0]=Addr[47:16]
                     ///<
  OUT UINT32 UmaBase; ///<  The allocated Uma base address (32-bits)
                     ///<  NV_UMA_Base[31:0]=Addr[47:16]
                     ///<

                      /// Memory Restore Feature

  IN  BOOLEAN MemRestoreCtl; ///< Memory context restore control
                        ///<   FALSE = perform memory init as normal (AMD default)
                        ///<   TRUE = restore memory context and skip training. This requires
                        ///<          MemContext is valid before AmdInitPost
                        ///<
  IN  BOOLEAN SaveMemContextCtl; ///< Control switch to save memory context at the end of MemAuto
                          ///<   TRUE = AGESA will setup MemContext block before exit AmdInitPost
                          ///<   FALSE = AGESA will not setup MemContext block. Platform is
                          ///<           expected to call S3Save later in POST if it wants to
                          ///<           use memory context restore feature.
   ///<
  IN OUT S3_DATA_BLOCK MemContext; ///< Memory context block describes the data that platform needs to
                           ///< save and restore for memory context restore feature to work.
                           ///< It uses the subset of S3Save block to save/restore. Hence platform
                           ///< may save only S3 block and uses it for both S3 resume and
                           ///< memory context restore.
                            ///<  - If MemRestoreCtl is TRUE, platform needs to pass in MemContext
                            ///<    before AmdInitPost.
                            ///<  - If SaveMemContextCtl is TRUE, platform needs to save MemContext
                            ///<    right after AmdInitPost.
  ///<
  IN     BOOLEAN IsCapsuleMode; ///< Capsule reboot control
                           ///<   FALSE = This is not a capsule reboot.
                           ///<   TRUE = This is a capsule reboot.
                           ///<
  IN     BOOLEAN ExternalVrefCtl; ///< Control the use of external Vref
                           ///<   TRUE = AGESA will use the function defined in "AGESA_EXTERNAL_VREF_CHANGE" in function list
                          ///<          to change the vref
                          ///<   FALSE = AGESA will will use the internal vref control.
                          ///< @BldCfgItem{BLDCFG_ENABLE_EXTERNAL_VREF_FEATURE}
                            ///<
  IN     FORCE_TRAIN_MODE ForceTrainMode; ///<  Training Mode
                                          ///<  0 = Force 1D Training for all configurations
                                          ///<  1 = Force 2D Training for all configurations
                                          ///<  2 = Auto - AGESA will control 1D or 2D
  IN     TECHNOLOGY_TYPE DimmTypeUsedInMixedConfig; ///< Select the preferred technology type that AGESA will enable
                                               ///< when it is mixed with other technology types.
                                               ///<   DDR3_TECHNOLOGY = Use DDR3 DIMMs
                                               ///<   GDDR5_TECHNOLOGY = Use GDDR5 DIMMs
                                               ///<   UNSUPPORTED_TECHNOLOGY = Exit with fatal error when DDR3 and GDDR5 DIMMs
                                               ///<                            are installed on the same system
                                               ///< @BldCfgItem{BLDCFG_DIMM_TYPE_USED_IN_MIXED_CONFIG}
  IN     BOOLEAN AmpEnable; ///< AMP functionality control
                       ///<   TRUE = Enable, platform BIOS requests to enable memory overclocking function, and AGESA
                       ///<          detects if memory is capable of it
                       ///<   FALSE = Disable, there is no request to enable memory overclocking function
                       ///<
  IN     BOOLEAN AmpWarningMsgEnable; ///< AMP warning messages control
                                ///<   TRUE = Enable to log the warning messages of AMP
                                ///<   FALSE = Disable
                                ///<
  OUT    AMP_STATUS AmpStatus; ///< AMP status allows platform BIOS to check which voltage or warning message it should
                        ///< use/apply.
                        ///<
                        ///<   AmpVoltageValid :
                        ///<       TRUE - AGESA does enable AMP function, so use AmpVoltage for voltage adjustment
                         ///<       FALSE - AGESA does not enable AMP function, so use DDRVoltage for voltage adjustment
                         ///<
                         ///<   AmpSupportDetectedButNotEnabled :
                         ///<       TRUE - Display warning message of "AMP support detected but not enabled"
                         ///<       FALSE - No corresponding message should be displayed
                         ///<
                         ///<   AmpSelectedButNotEnabled :
                         ///<       TRUE - Display warning message of "AMP selected but not enabled"
                         ///<       FALSE - No corresponding message should be displayed
                         ///<
                         ///<   Note that both of warning message status reports are controlled by AmpWarningMsgEnable
                         ///<
  OUT    AMP_DIMM_VOLTAGE AmpVoltage; ///< AMP voltage which will be sent back to platform BIOS, and
                         ///< the value in AmpVoltage is valid only if AmpStatus is TRUE returned
  IN     BOOLEAN DataEyeEn; ///< Get 2D training data eye
                         ///<   TRUE  = Enable to get the 2D data eye
                         ///<   FALSE = The 2D data eye is not enabled
                         ///< @BldCfgItem{BLDCFG_ENABLE_DATA_EYE}
  IN     BOOLEAN DramDoubleRefreshRateEn; ///< Specify the average time between refresh requests to all DRAM devices.
                                ///<   TRUE  = 2x refresh rate.
                                ///<   FALSE = 1x refresh rate.
                                ///< @BldCfgItem{BLDCFG_DRAM_DOUBLE_REFRESH_RATE}
  IN     PMU_TRAIN_MODE PmuTrainMode; ///<  PMU Training Mode
                                  ///< @BldCfgItem{BLDCFG_PMU_TRAINING_MODE}
                                  ///<  0 = PMU 1D Training only for all configurations
                                  ///<  1 = PMU 1D and 2D Training read only for all configurations
                                  ///<  2 = PMU 1D and 2D Training for all configurations
                                  ///<  3 = AGESA control type of training depend on configurations
  IN     BOOLEAN EccRedirection; ///< ECC Redirection.
                               ///< Build-time customizable only - @BldCfgItem{BLDCFG_ECC_REDIRECTION}
  IN     UINT16 ScrubDramRate; ///< Scrub Dram Rate.
                               ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_DRAM_RATE}
  IN     UINT16 ScrubL2Rate; ///< Scrub L2Rate.
                               ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_L2_RATE}
  IN     UINT16 ScrubL3Rate; ///< Scrub L3Rate.
                               ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_L3_RATE}
  IN     UINT16 ScrubIcRate; ///< Scrub Ic Rate.
                               ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_IC_RATE}
  IN     UINT16 ScrubDcRate; ///< Scrub Dc Rate.
                               ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_DC_RATE}
  IN     BOOLEAN EccSyncFlood; ///< ECC Sync Flood.
                               ///< Build-time customizable only - @BldCfgItem{BLDCFG_ECC_SYNC_FLOOD}
  IN     UINT16 EccSymbolSize; ///< ECC Symbol Size.
                               ///< Build-time customizable only - @BldCfgItem{BLDCFG_ECC_SYMBOL_SIZE}
  IN     BOOLEAN DqsTrainingControl; ///< Dqs Training Control.
                                 ///< Build-time customizable only - @BldCfgItem{BLDCFG_DQS_TRAINING_CONTROL}
  IN     BOOLEAN UmaAbove4G; ///< Uma Above 4G Support
  IN     UMA_ALIGNMENT UmaAlignment; ///< Uma alignment
  IN     BOOLEAN MemoryAllClocksOn; ///< Memory All Clocks On.
                                ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_ALL_CLOCKS_ON}
  IN     UINT32 MemoryBusFrequencyLimit; ///< Memory Bus Frequency Limit.
                                ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT}
  IN     UINT32 PowerDownMode; ///< Power Down Mode.
  IN     BOOLEAN IgnoreSpdChecksum; ///< Ignore Spd Checksum.
                               ///< Build-time customizable only - @BldCfgItem{BLDCFG_IGNORE_SPD_CHECKSUM}
  IN     BOOLEAN MemoryModeUnganged; ///< Memory Mode Unganged.
                               ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_MODE_UNGANGED}
  IN     BOOLEAN MemoryQuadRankCapable; ///< Memory Quad Rank Capable.
  IN     BOOLEAN MemoryRDimmCapable; ///< Memory RDIMM Capable.
  IN     BOOLEAN MemoryLRDimmCapable; ///< Memory LRDIMM Capable.
  IN     BOOLEAN MemoryUDimmCapable; ///< Memory UDIMM Capable.
  IN     BOOLEAN MemorySODimmCapable; ///< Memory SODimm Capable.
                               ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_SODIMM_CAPABLE}
                                 ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_QUAD_RANK_CAPABLE}
  IN     BOOLEAN DimmTypeDddr4Capable;        ///< Indicates that the system is DDR4 Capable
                                             ///<   TRUE = Enable, platfrom BIOS requests support for DDR4
                                             ///<   FALSE = Disable, platform BIOS requests no DDR4 support
                                             ///< @BldCfgItem{BLDCFG_DIMM_TYPE_DDR4_CAPABLE}
  IN     BOOLEAN DimmTypeDddr3Capable;       ///< Indicates that the system is DDR3 Capable
                                             ///<   TRUE = Enable, platfrom BIOS requests support for DDR3
                                             ///<   FALSE = Disable, platform BIOS requests no DDR3 support
                                             ///< @BldCfgItem{BLDCFG_DIMM_TYPE_DDR3_CAPABLE}
  IN     UINT16 CustomVddioSupport;          ///< CustomVddioSupport
                                             ///< @BldCfgItem{BLDCFG_CUSTOM_VDDIO_VOLTAGE}
  IN     CPU_VREF_OVERRIDE CpuVrefOverride[2][4];     ///< Structure to adjust VrefHspeed
                                                      ///< PerDct, Per MemPstate
  IN     MEMORY_POWER_POLICY MemoryPowerPolicy;
  IN     ACP_SIZE GnbAcpSize;
  IN     DISPLAY_RESOLUTION GnbResolution;
  IN     PLATFORM_CSTATE_MODES CStateMode;
} MEM_PARAMETER_STRUCT;


///
/// Function definition.
/// This data structure passes function pointers to the memory configuration code.
/// The wrapper can use this structure with customized versions.
///
typedef struct _MEM_FUNCTION_STRUCT {

  // PUBLIC required Internal functions

  IN OUT BOOLEAN (*amdMemGetPsCfgU) ( VOID *pMemData);  ///< Proc for Unbuffered DIMMs, platform specific
  IN OUT BOOLEAN (*amdMemGetPsCfgR) (VOID *pMemData);   ///< Proc for Registered DIMMs, platform specific

  // PUBLIC optional functions

  IN OUT VOID (*amdMemEccInit) (VOID *pMemData);                  ///< NB proc for ECC feature
  IN OUT VOID (*amdMemChipSelectInterleaveInit) (VOID *pMemData); ///< NB proc for CS interleave feature
  IN OUT VOID (*amdMemDctInterleavingInit) (VOID *pMemData);      ///< NB proc for Channel interleave feature
  IN OUT VOID (*amdMemMctInterleavingInit) (VOID *pMemData);      ///< NB proc for Node interleave feature
  IN OUT VOID (*amdMemParallelTraining) (VOID *pMemData);         ///< NB proc for parallel training feature
  IN OUT VOID (*amdMemEarlySampleSupport) (VOID *pMemData);       ///< NB code for early sample support feature
  IN OUT VOID (*amdMemMultiPartInitSupport) (VOID *pMemData);     ///< NB code for 'multi-part'
  IN OUT VOID (*amdMemOnlineSpareSupport) (VOID *pMemData);       ///< NB code for On-Line Spare feature
  IN OUT VOID (*amdMemUDimmInit) (VOID *pMemData);                ///< NB code for UDIMMs
  IN OUT VOID (*amdMemRDimmInit) (VOID *pMemData);                ///< NB code for RDIMMs
  IN OUT VOID (*amdMemLrDimmInit) (VOID *pMemData);               ///< NB code for LRDIMMs
  IN OUT UINT32   Reserved[100]; ///< Reserved for later function definition
} MEM_FUNCTION_STRUCT;

///
/// Socket Structure
///
///
typedef struct _MEM_SOCKET_STRUCT {
  OUT VOID *ChannelPtr[MAX_CHANNELS_PER_SOCKET];  ///< Pointers to each channels training data

  OUT VOID *TimingsPtr[MAX_CHANNELS_PER_SOCKET];  ///< Pointers to each channels timing data
} MEM_SOCKET_STRUCT;

///
/// Contains all data relevant to Memory Initialization.
///
typedef struct _MEM_DATA_STRUCT {
  IN AMD_CONFIG_PARAMS StdHeader;             ///< Standard configuration header

  IN MEM_PARAMETER_STRUCT *ParameterListPtr;  ///< List of input Parameters

  OUT MEM_FUNCTION_STRUCT FunctionList;       ///< List of function Pointers

  IN OUT AGESA_STATUS (*GetPlatformCfg[MAX_PLATFORM_TYPES]) (struct _MEM_DATA_STRUCT *MemData, UINT8 SocketID, CH_DEF_STRUCT *CurrentChannel); ///< look-up platform info

  IN OUT BOOLEAN (*ErrorHandling)(struct _DIE_STRUCT *MCTPtr, UINT8 DCT, UINT16 ChipSelMask, AMD_CONFIG_PARAMS *StdHeader); ///< Error Handling


  OUT MEM_SOCKET_STRUCT SocketList[MAX_SOCKETS_SUPPORTED];  ///< Socket list for memory code.
                                   ///< SocketList is a shortcut for IBVs to retrieve training
                                   ///< and timing data for each channel indexed by socket/channel,
                                   ///< eliminating their need to parse die/dct/channel etc.
                                   ///< It contains pointers to the populated data structures for
                                   ///< each channel and skips the channel structures that are
                                   ///< unpopulated. In the case of channels sharing the same DCT,
                                   ///< the pTimings pointers will point to the same DCT Timing data.

  OUT DIE_STRUCT *DiesPerSystem;  ///< Pointed to an array of DIE_STRUCTs
  OUT UINT8      DieCount;        ///< Number of MCTs in the system.

  IN SPD_DEF_STRUCT *SpdDataStructure;              ///< Pointer to SPD Data structure

  IN OUT  struct _PLATFORM_CONFIGURATION   *PlatFormConfig;    ///< Platform profile/build option config structure

  IN OUT BOOLEAN IsFlowControlSupported;    ///< Indicates if flow control is supported

  OUT UINT32 TscRate;             ///< The rate at which the TSC increments in megahertz.
  IN BOOLEAN PhyReceiverLowPower;  ///< Force PHY receiver in low power.
                                   ///<   TRUE  = PHY receiver low power
                                   ///<   FALSE = PHY receiver high power
  IN VOID *OptionMemoryInstallPtr;    ///< Pointer to OptionMemoryInstall
} MEM_DATA_STRUCT;

///
/// Contains all data relevant to Memory Public Services
///
typedef struct _MEM_PUBLICS_DATA_STRUCT {
  OUT UINT64 UmaBase;          ///< UmaBase[63:0] = Addr[63:0]
  OUT UINT32 UmaSize;          ///< UmaSize[31:0] = Addr[31:0]
} MEM_PUBLICS_DATA_STRUCT;

///
/// Uma Structure
///
///
typedef struct _UMA_INFO {
  OUT UINT64 UmaBase;          ///< UmaBase[63:0] = Addr[63:0]
  OUT UINT32 UmaSize;          ///< UmaSize[31:0] = Addr[31:0]
  OUT UINT32 UmaAttributes;    ///< Indicate the attribute of Uma
  OUT UINT8  UmaMode;          ///< Indicate the mode of Uma
  OUT UINT16 MemClock;         ///< Indicate memory running speed in MHz
  OUT UINT8  MemType;          ///< Indicate the DRAM technology type that is being used
  OUT UINT8  Reserved[2];      ///< Reserved for future usage
} UMA_INFO;


/// Bitfield for ID
typedef struct {
  OUT UINT16 SocketId:8;       ///< Socket ID
  OUT UINT16 ModuleId:8;       ///< Module ID
} ID_FIELD;
///
/// Union for ID of socket and module that will be passed out in call out
///
typedef union {
  OUT ID_FIELD IdField;         ///< Bitfield for ID
  OUT UINT16 IdInformation;     ///< ID information for call out
} ID_INFO;

//  AGESA MEMORY ERRORS

// AGESA_SUCCESS memory events
#define MEM_EVENT_CAPSULE_IN_EFFECT      0x04013600ul          ///< Capsule is in effect
#define MEM_EVENT_CONTEXT_RESTORE_IN_EFFECT     0x04023600ul   ///< Context restore is in effect

// AGESA_ALERT Memory Errors
#define MEM_ALERT_USER_TMG_MODE_OVERRULED   0x04010000ul       ///< TIMING_MODE_SPECIFIC is requested but
                                                               ///< cannot be applied to current configurations.
#define MEM_ALERT_ORG_MISMATCH_DIMM 0x04010100ul               ///< DIMM organization miss-match
#define MEM_ALERT_BK_INT_DIS 0x04010200ul                      ///< Bank interleaving disable for internal issue
#define MEM_ALERT_DRAM_DOUBLE_REFRESH_RATE_ENABLED 0x04010300ul ///< CfgDramDoubleRefreshRate has been enabled due
                                                                ///      to Extended Temperature Range feature

// AGESA_ERROR Memory Errors
#define MEM_ERROR_NO_DQS_POS_RD_WINDOW 0x04010300ul            ///< No DQS Position window for RD DQS
#define MEM_ERROR_SMALL_DQS_POS_RD_WINDOW 0x04020300ul         ///< Small DQS Position window for RD DQS
#define MEM_ERROR_NO_DQS_POS_WR_WINDOW 0x04030300ul            ///< No DQS Position window for WR DQS
#define MEM_ERROR_SMALL_DQS_POS_WR_WINDOW 0x04040300ul         ///< Small DQS Position window for WR DQS
#define MEM_ERROR_DIMM_SPARING_NOT_ENABLED 0x04010500ul        ///< DIMM sparing has not been enabled for an internal issues
#define MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE 0x04050300ul         ///< Receive Enable value is too large
#define MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW 0x04060300ul       ///< There is no DQS receiver enable window
#define MEM_ERROR_DRAM_ENABLED_TIME_OUT 0x04010600ul           ///< Time out when polling DramEnabled bit
#define MEM_ERROR_DCT_ACCESS_DONE_TIME_OUT 0x04010700ul        ///< Time out when polling DctAccessDone bit
#define MEM_ERROR_SEND_CTRL_WORD_TIME_OUT 0x04010800ul         ///< Time out when polling SendCtrlWord bit
#define MEM_ERROR_PREF_DRAM_TRAIN_MODE_TIME_OUT 0x04010900ul   ///< Time out when polling PrefDramTrainMode bit
#define MEM_ERROR_ENTER_SELF_REF_TIME_OUT 0x04010A00ul         ///< Time out when polling EnterSelfRef bit
#define MEM_ERROR_FREQ_CHG_IN_PROG_TIME_OUT 0x04010B00ul       ///< Time out when polling FreqChgInProg bit
#define MEM_ERROR_EXIT_SELF_REF_TIME_OUT 0x04020A00ul          ///< Time out when polling ExitSelfRef bit
#define MEM_ERROR_SEND_MRS_CMD_TIME_OUT 0x04010C00ul           ///< Time out when polling SendMrsCmd bit
#define MEM_ERROR_SEND_ZQ_CMD_TIME_OUT 0x04010D00ul            ///< Time out when polling SendZQCmd bit
#define MEM_ERROR_DCT_EXTRA_ACCESS_DONE_TIME_OUT 0x04010E00ul  ///< Time out when polling DctExtraAccessDone bit
#define MEM_ERROR_MEM_CLR_BUSY_TIME_OUT 0x04010F00ul           ///< Time out when polling MemClrBusy bit
#define MEM_ERROR_MEM_CLEARED_TIME_OUT 0x04020F00ul            ///< Time out when polling MemCleared bit
#define MEM_ERROR_FLUSH_WR_TIME_OUT 0x04011000ul               ///< Time out when polling FlushWr bit
#define MEM_ERROR_NBPSTATE_TRANSITION_TIME_OUT 0x04012600ul    ///< Time out when polling CurNBPstate bit
#define MEM_ERROR_MAX_LAT_NO_WINDOW 0x04070300ul               ///< Fail to find pass during Max Rd Latency training
#define MEM_ERROR_PARALLEL_TRAINING_LAUNCH_FAIL 0x04080300ul   ///< Fail to launch training code on an AP
#define MEM_ERROR_PARALLEL_TRAINING_TIME_OUT 0x04090300ul      ///< Fail to finish parallel training
#define MEM_ERROR_NO_ADDRESS_MAPPING 0x04011100ul              ///< No address mapping found for a dimm
#define MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW_EQUAL_LIMIT  0x040A0300ul ///< There is no DQS receiver enable window and the value is equal to the largest value
#define MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE_LIMIT_LESS_ONE 0x040B0300ul ///< Receive Enable value is too large and is 1 less than limit
#define MEM_ERROR_CHECKSUM_NV_SPDCHK_RESTRT_ERROR  0x04011200ul       ///< SPD Checksum error for NV_SPDCHK_RESTRT
#define MEM_ERROR_NO_CHIPSELECT 0x04011300ul                   ///< No chipselects found
#define MEM_ERROR_UNSUPPORTED_333MHZ_UDIMM 0x04011500ul        ///< Unbuffered dimm is not supported at 333MHz
#define MEM_ERROR_WL_PRE_OUT_OF_RANGE 0x040C0300ul             ///< Returned PRE value during write levelizzation was out of range
#define MEM_ERROR_NO_2D_RDDQS_WINDOW 0x040D0300ul              ///< No 2D RdDqs Window
#define MEM_ERROR_NO_2D_RDDQS_HEIGHT 0x040E0300ul              ///< No 2D RdDqs Height
#define MEM_ERROR_2D_DQS_ERROR  0x040F0300ul                   ///< 2d RdDqs Error
#define MEM_ERROR_INVALID_2D_RDDQS_VALUE  0x04022400ul         ///< 2d RdDqs invalid value found
#define MEM_ERROR_2D_DQS_VREF_MARGIN_ERROR  0x04023400ul       ///< 2d RdDqs Vef Margin error found
#define MEM_ERROR_LR_IBT_NOT_FOUND  0x04013500ul               ///< No LR dimm IBT value is found
#define MEM_ERROR_MR0_NOT_FOUND  0x04023500ul                  ///< No MR0 value is found
#define MEM_ERROR_ODT_PATTERN_NOT_FOUND  0x04033500ul          ///< No odt pattern value is found
#define MEM_ERROR_RC2_IBT_NOT_FOUND  0x04043500ul              ///< No RC2 IBT value is found
#define MEM_ERROR_RC10_OP_SPEED_NOT_FOUND  0x04053500ul        ///< No RC10 op speed is found
#define MEM_ERROR_RTT_NOT_FOUND  0x04063500ul                  ///< No RTT value is found
#define MEM_ERROR_P2D_NOT_FOUND  0x04073500ul                  ///< No 2D training config value is found
#define MEM_ERROR_SAO_NOT_FOUND  0x04083500ul                  ///< No slow access mode, Address timing and Output driver compensation value is found
#define MEM_ERROR_CLK_DIS_MAP_NOT_FOUND  0x04093500ul          ///< No CLK disable map is found
#define MEM_ERROR_CKE_TRI_MAP_NOT_FOUND  0x040A3500ul          ///< No CKE tristate map is found
#define MEM_ERROR_ODT_TRI_MAP_NOT_FOUND  0x040B3500ul          ///< No ODT tristate map is found
#define MEM_ERROR_CS_TRI_MAP_NOT_FOUND  0x040C3500ul           ///< No CS tristate map is found
#define MEM_ERROR_TRAINING_SEED_NOT_FOUND  0x040D3500ul        ///< No training seed is found
#define MEM_ERROR_CAD_BUS_TMG_NOT_FOUND  0x040E3500ul          ///< No CAD Bus Timing Entries found
#define MEM_ERROR_DATA_BUS_CFG_NOT_FOUND  0x040F3500ul         ///< No Data Bus Config Entries found
#define MEM_ERROR_NO_2D_WRDAT_WINDOW 0x040D0400ul              ///< No 2D WrDat Window
#define MEM_ERROR_NO_2D_WRDAT_HEIGHT 0x040E0400ul              ///< No 2D WrDat Height
#define MEM_ERROR_2D_WRDAT_ERROR  0x040F0400ul                 ///< 2d WrDat Error
#define MEM_ERROR_INVALID_2D_WRDAT_VALUE  0x04100400ul         ///< 2d WrDat invalid value found
#define MEM_ERROR_2D_WRDAT_VREF_MARGIN_ERROR  0x04110400ul     ///< 2d WrDat Vef Margin error found
#define MEM_ERROR_PMU_TRAINING 0x04120400ul                    ///< Fail PMU training.

// AGESA_WARNING Memory Errors
#define MEM_WARNING_UNSUPPORTED_QRDIMM      0x04011600ul       ///< QR DIMMs detected but not supported
#define MEM_WARNING_UNSUPPORTED_UDIMM       0x04021600ul       ///< U DIMMs detected but not supported
#define MEM_WARNING_UNSUPPORTED_SODIMM      0x04031600ul       ///< SO-DIMMs detected but not supported
#define MEM_WARNING_UNSUPPORTED_X4DIMM      0x04041600ul       ///< x4 DIMMs detected but not supported
#define MEM_WARNING_UNSUPPORTED_RDIMM       0x04051600ul       ///< R DIMMs detected but not supported
#define MEM_WARNING_UNSUPPORTED_LRDIMM      0x04061600ul       ///< LR DIMMs detected but not supported
#define MEM_WARNING_EMP_NOT_SUPPORTED       0x04011700ul       ///< Processor is not capable for EMP
#define MEM_WARNING_EMP_CONFLICT            0x04021700ul       ///< EMP cannot be enabled if channel interleaving,
#define MEM_WARNING_EMP_NOT_ENABLED         0x04031700ul       ///< Memory size is not power of two.
#define MEM_WARNING_ECC_DIS                 0x04041700ul       ///< ECC has been disabled as a result of an internal issue
#define MEM_WARNING_PERFORMANCE_ENABLED_BATTERY_LIFE_PREFERRED 0x04011800ul  ///< Performance has been enabled, but battery life is preferred.
                                                                             ///< bank interleaving, or bank swizzle is enabled.
#define MEM_WARNING_NO_SPDTRC_FOUND               0x04011900ul ///< No Trc timing value found in SPD of a dimm.
#define MEM_WARNING_NODE_INTERLEAVING_NOT_ENABLED 0x04012000ul ///< Node Interleaveing Requested, but could not be enabled
#define MEM_WARNING_CHANNEL_INTERLEAVING_NOT_ENABLED 0x04012100ul ///< Channel Interleaveing Requested, but could not be enabled
#define MEM_WARNING_BANK_INTERLEAVING_NOT_ENABLED 0x04012200ul ///< Bank Interleaveing Requested, but could not be enabled
#define MEM_WARNING_VOLTAGE_1_35_NOT_SUPPORTED    0x04012300ul ///< Voltage 1.35 determined, but could not be supported
#define MEM_WARNING_INITIAL_DDR3VOLT_NONZERO      0x04012400ul ///< DDR3 voltage initial value is not 0
#define MEM_WARNING_NO_COMMONLY_SUPPORTED_VDDIO   0x04012500ul ///< Cannot find a commonly supported VDDIO
#define MEM_WARNING_AMP_SUPPORT_DETECTED_BUT_NOT_ENABLED 0x04012900ul ///< AMP support detected but not enabled
#define MEM_WARNING_AMP_SELECTED_BUT_NOT_ENABLED  0x04022900ul ///< AMP selected but not enabled

// AGESA_FATAL Memory Errors
#define MEM_ERROR_MINIMUM_MODE              0x04011A00ul       ///< Running in minimum mode
#define MEM_ERROR_MODULE_TYPE_MISMATCH_DIMM 0x04011B00ul       ///< DIMM modules are miss-matched
#define MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM   0x04011C00ul       ///< No DIMMs have been found
#define MEM_ERROR_MISMATCH_DIMM_CLOCKS      0x04011D00ul       ///< DIMM clocks miss-matched
#define MEM_ERROR_NO_CYC_TIME               0x04011E00ul       ///< No cycle time found
#define MEM_ERROR_HEAP_ALLOCATE_DYN_STORING_OF_TRAINED_TIMINGS     0x04011F00ul  ///< Heap allocation error with dynamic storing of trained timings
#define MEM_ERROR_HEAP_ALLOCATE_FOR_DCT_STRUCT_AND_CH_DEF_STRUCTs  0x04021F00ul  ///< Heap allocation error for DCT_STRUCT and CH_DEF_STRUCT
#define MEM_ERROR_HEAP_ALLOCATE_FOR_REMOTE_TRAINING_ENV   0x04031F00ul           ///< Heap allocation error with REMOTE_TRAINING_ENV
#define MEM_ERROR_HEAP_ALLOCATE_FOR_SPD               0x04041F00ul    ///< Heap allocation error for SPD data
#define MEM_ERROR_HEAP_ALLOCATE_FOR_RECEIVED_DATA     0x04051F00ul    ///< Heap allocation error for RECEIVED_DATA during parallel training
#define MEM_ERROR_HEAP_ALLOCATE_FOR_S3_SPECIAL_CASE_REGISTERS     0x04061F00ul   ///< Heap allocation error for S3 "SPECIAL_CASE_REGISTER"
#define MEM_ERROR_HEAP_ALLOCATE_FOR_TRAINING_DATA     0x04071F00ul    ///< Heap allocation error for Training Data
#define MEM_ERROR_HEAP_ALLOCATE_FOR_IDENTIFY_DIMM_MEM_NB_BLOCK    0x04081F00ul   ///< Heap allocation error for  DIMM Identify "MEM_NB_BLOCK
#define MEM_ERROR_NO_CONSTRUCTOR_FOR_IDENTIFY_DIMM    0x04022300ul    ///< No Constructor for DIMM Identify
#define MEM_ERROR_VDDIO_UNSUPPORTED                   0x04022500ul    ///< VDDIO of the dimms on the board is not supported
#define MEM_ERROR_VDDPVDDR_UNSUPPORTED                0x04032500ul    ///< VDDP/VDDR value indicated by the platform BIOS is not supported
#define MEM_ERROR_HEAP_ALLOCATE_FOR_2D                0x040B1F00ul    ///< Heap allocation error for 2D training data
#define MEM_ERROR_HEAP_DEALLOCATE_FOR_2D              0x040C1F00ul    ///< Heap de-allocation error for 2D training data
#define MEM_ERROR_HEAP_ALLOCATE_FOR_DATAEYE           0x040F1F00ul    ///< Heap allocation error for DATAEYE Storage
#define MEM_ERROR_HEAP_DEALLOCATE_FOR_DATAEYE         0x040E1F00ul    ///< Heap de-allocation error for DATAEYE Storage
#define MEM_ERROR_HEAP_ALLOCATE_FOR_PMU_SRAM_MSG_BLOCK            0x04101F00ul    ///< Heap allocation error for PMU SRAM Message Block Storage
#define MEM_ERROR_HEAP_DEALLOCATE_FOR_PMU_SRAM_MSG_BLOCK          0x04111F00ul    ///< Heap de-allocation error for PMU SRAM Message Block Storage
#define MEM_ERROR_HEAP_LOCATE_FOR_PMU_SRAM_MSG_BLOCK              0x04121F00ul    ///< Heap location error for PMU SRAM Message Block Storage

// AGESA_CRITICAL Memory Errors
#define MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR3    0x04091F00ul    ///< Heap allocation error for DMI table for DDR3
#define MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR4    0x040A1F00ul    ///< Heap allocation error for DMI table for DDR4
#define MEM_ERROR_UNSUPPORTED_DIMM_CONFIG             0x04011400ul    ///< Dimm population is not supported
#define MEM_ERROR_HEAP_ALLOCATE_FOR_CRAT_MEM_AFFINITY 0x040D1F00ul    ///< Heap allocation error for CRAT memory affinity info



/*----------------------------------------------------------------------------
 *
 *                END OF MEMORY-SPECIFIC DATA STRUCTURES
 *
 *----------------------------------------------------------------------------
 */




/*----------------------------------------------------------------------------
 *
 *                    CPU RELATED DEFINITIONS
 *
 *----------------------------------------------------------------------------
 */

// CPU Event definitions.

// Defines used to filter CPU events based on functional blocks
#define CPU_EVENT_PM_EVENT_MASK                         0xFF00FF00ul
#define CPU_EVENT_PM_EVENT_CLASS                        0x08000400ul

//================================================================
// CPU General events
//    Heap allocation                     (AppFunction =      01h)
#define CPU_ERROR_HEAP_BUFFER_IS_NOT_PRESENT            0x08000100ul
#define CPU_ERROR_HEAP_IS_ALREADY_INITIALIZED           0x08010100ul
#define CPU_ERROR_HEAP_IS_FULL                          0x08020100ul
#define CPU_ERROR_HEAP_BUFFER_HANDLE_IS_ALREADY_USED    0x08030100ul
#define CPU_ERROR_HEAP_BUFFER_HANDLE_IS_NOT_PRESENT     0x08040100ul
//    BrandId                             (AppFunction =      02h)
#define CPU_ERROR_BRANDID_HEAP_NOT_AVAILABLE            0x08000200ul
//    Micro code patch                    (AppFunction =      03h)
#define CPU_ERROR_MICRO_CODE_PATCH_IS_NOT_LOADED        0x08000300ul
//    Power management                    (AppFunction =      04h)
#define CPU_EVENT_PM_PSTATE_OVERCURRENT                 0x08000400ul
#define CPU_EVENT_PM_ALL_PSTATE_OVERCURRENT             0x08010400ul
#define CPU_ERROR_PSTATE_HEAP_NOT_AVAILABLE             0x08020400ul
#define CPU_ERROR_PM_NB_PSTATE_MISMATCH                 0x08030400ul
#define CPU_ERROR_PM_ALL_PSTATE_OVER_FREQUENCY_LIMIT    0x08040400ul
#define CPU_EVENT_PM_PSTATE_FREQUENCY_LIMIT             0x08050400ul
//    Other CPU events                    (AppFunction =      05h)
#define CPU_EVENT_BIST_ERROR                            0x08000500ul
#define CPU_EVENT_UNKNOWN_PROCESSOR_FAMILY              0x08010500ul
#define CPU_EVENT_STACK_REENTRY                         0x08020500ul
#define CPU_EVENT_CORE_NOT_IDENTIFIED                   0x08030500ul
#define CPU_EVENT_STACK_BASE_OUT_OF_BOUNDS              0x08040500ul
#define CPU_EVENT_STACK_SIZE_INVALID                    0x08050500ul

//=================================================================
// CPU Feature events
//    Execution cache                     (AppFunction =      21h)
//        AGESA_CACHE_SIZE_REDUCED                            2101
//        AGESA_CACHE_REGIONS_ACROSS_1MB                      2102
//        AGESA_CACHE_REGIONS_ACROSS_4GB                      2103
//        AGESA_REGION_NOT_ALIGNED_ON_BOUNDARY                2104
//        AGESA_CACHE_START_ADDRESS_LESS_D0000                2105
//        AGESA_THREE_CACHE_REGIONS_ABOVE_1MB                 2106
//        AGESA_DEALLOCATE_CACHE_REGIONS                      2107
#define CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR      0x08002100ul
//    Core Leveling                       (AppFunction =      22h)
#define CPU_WARNING_ADJUSTED_LEVELING_MODE              0x08002200ul
//    SCS initialization                  (AppFunction =      24h)
//        AGESA_SCS_HEAP_ENTRY_MISSING                        2401
//        AGESA_SCS_BUFFER_EMPTY                              2402
//        AGESA_SCS_WEIGHTS_MISMATCH                          2403
#define CPU_EVENT_SCS_INITIALIZATION_ERROR              0x08002400ul
//    BTC vid adjustment error
#define CPU_EVENT_BTC_INITIALIZATION_ERROR              0x08002500ul

// CPU Build Configuration structures and definitions

#define AMD_AP_MTRR_FIX64k_00000    0x00000250ul
#define AMD_AP_MTRR_FIX16k_80000    0x00000258ul
#define AMD_AP_MTRR_FIX16k_A0000    0x00000259ul
#define AMD_AP_MTRR_FIX4k_C0000     0x00000268ul
#define AMD_AP_MTRR_FIX4k_C8000     0x00000269ul
#define AMD_AP_MTRR_FIX4k_D0000     0x0000026Aul
#define AMD_AP_MTRR_FIX4k_D8000     0x0000026Bul
#define AMD_AP_MTRR_FIX4k_E0000     0x0000026Cul
#define AMD_AP_MTRR_FIX4k_E8000     0x0000026Dul
#define AMD_AP_MTRR_FIX4k_F0000     0x0000026Eul
#define AMD_AP_MTRR_FIX4k_F8000     0x0000026Ful
#define CPU_LIST_TERMINAL           0xFFFFFFFFul

/// Data structure for the Mapping Item between Unified ID for IDS Setup Option
/// and the option value.
///
typedef struct {
  IN    UINT16 IdsNvId;           ///< Unified ID for IDS Setup Option.
  OUT UINT16 IdsNvValue;        ///< The value of IDS Setup Option.
} IDS_NV_ITEM;

/// Data Structure for IDS CallOut Function
typedef struct {
  IN    AMD_CONFIG_PARAMS StdHeader;        ///< Standard configuration header
  IN    IDS_NV_ITEM *IdsNvPtr;              ///< Memory Pointer of IDS NV Table
  IN OUT UINTN Reserved;              ///< reserved
} IDS_CALLOUT_STRUCT;

/// Data Structure for Connected Standby Function
typedef struct {
  IN       AMD_CONFIG_PARAMS StdHeader;           ///< Standard configuration header
  IN       VOID              *CsRestoreTable;     ///< Pointer to the CsRestoreTable
  IN       UINT32            CsRestoreTableSize;  ///< Size in bytes of the CsRestoreTable
} CS_CALLOUT_STRUCT;

/************************************************************************
 *
 *  AGESA interface Call-Out function parameter structures
 *
 ***********************************************************************/

/// Parameters structure for interface call-out AgesaRunCodeOnAp
typedef struct {
  IN OUT    AMD_CONFIG_PARAMS   StdHeader;            ///< Standard configuration header
  IN        UINT32              FunctionNumber;       ///< Index of the procedure to execute
  IN        VOID                *RelatedDataBlock;    ///< Location of data structure the procedure will use
  IN        UINT32              RelatedBlockLength;   ///< Size of the related data block
  IN        BOOLEAN             AllAPs;               ///< run on all Aps or on one only
} AP_EXE_PARAMS;

/// VoltageType values
typedef enum {
  VTYPE_CPU_VREF,                                    ///< Cpu side Vref
  VTYPE_DIMM_VREF,                                   ///< Dimm Side Vref
  VTYPE_VDDIO                                        ///< Vddio
} VTYPE;

/// Buffer Handles
typedef enum {
  AMD_DMI_INFO_BUFFER_HANDLE = 0x000D000,       ///< Assign 0x000D000 buffer handle to DMI function
  AMD_PSTATE_DATA_BUFFER_HANDLE,                ///< Assign 0x000D001 buffer handle to Pstate data
  AMD_PSTATE_ACPI_BUFFER_HANDLE,                ///< Assign 0x000D002 buffer handle to Pstate table
  AMD_BRAND_ID_BUFFER_HANDLE,                   ///< Assign 0x000D003 buffer handle to Brand ID
  AMD_WHEA_BUFFER_HANDLE,                       ///< Assign 0x000D004 buffer handle to WHEA function
  AMD_S3_INFO_BUFFER_HANDLE,                    ///< Assign 0x000D005 buffer handle to S3 function
  AMD_S3_NB_INFO_BUFFER_HANDLE,                 ///< Assign 0x000D006 buffer handle to S3 NB device info
  AMD_ACPI_ALIB_BUFFER_HANDLE,                  ///< Assign 0x000D007 buffer handle to ALIB SSDT table
  AMD_ACPI_IVRS_BUFFER_HANDLE,                  ///< Assign 0x000D008 buffer handle to IOMMU IVRS table
  AMD_CRAT_INFO_BUFFER_HANDLE,                  ///< Assign 0x000D009 buffer handle to CRAT function
  AMD_ACPI_CDIT_BUFFER_HANDLE,                  ///< Assign 0x000D00A buffer handle to CDIT function
  AMD_GNB_SMU_MEMORY_INFO_BUFFER_HANDLE,        ///< Assign 0x000D00B buffer handle to SMU memory parameters function
  AMD_CSTATE_DATA_BUFFER_HANDLE                 ///< Assign 0x000D00C buffer handle to Cstate data
} AMD_BUFFER_HANDLE;


/************************************************************************
 *
 *  AGESA interface Call-Out function prototypes
 *
 ***********************************************************************/


AGESA_STATUS
AgesaGetVbiosImage (
  IN       UINTN                 FcnData,
  IN OUT   GFX_VBIOS_IMAGE_INFO *VbiosImageInfo
  );

AGESA_STATUS
AgesaGnbOemCallout (
  IN        AMD_CONFIG_PARAMS   *StdHeader,
  IN        UINTN               FcnData,
  IN OUT    VOID                *GnbCalloutData
 );

/************************************************************************
 *
 *  AGESA interface structure definition and function prototypes
 *
 ***********************************************************************/

/**********************************************************************
 * Platform Configuration:  The parameters in boot branch function
 **********************************************************************/

///  The possible platform control flow settings.
typedef enum  {
  Nfcm,                                          ///< Normal Flow Control Mode.
  UmaDr,                                         ///< UMA using Display Refresh flow control.
  UmaIfcm,                                       ///< UMA using Isochronous Flow Control.
  Ifcm,                                          ///< Isochronous Flow Control Mode (other than for UMA).
  Iommu,                                         ///< An IOMMU is in use in the system.
  MaxControlFlow                                 ///< Not a control flow mode, use for limit checking.
} PLATFORM_CONTROL_FLOW;

///  Platform performance settings for optimized settings.
///  Several configuration settings for the processor depend upon other parts and
///  general designer choices for the system. The determination of these data points
///  is not standard for all platforms, so the host environment needs to provide these
///  to specify how the system is to be configured.
typedef struct {
  IN PLATFORM_CONTROL_FLOW PlatformControlFlowMode;    ///< The platform's control flow mode for optimum platform performance.
                                                       ///< @BldCfgItem{BLDCFG_PLATFORM_CONTROL_FLOW_MODE}
  IN BOOLEAN               Use32ByteRefresh;           ///< Display Refresh traffic generates 32 byte requests.
                                                       ///< @BldCfgItem{BLDCFG_USE_32_BYTE_REFRESH}
  IN BOOLEAN               UseVariableMctIsocPriority; ///< The Memory controller will be set to Variable Isoc Priority.
                                                       ///< @BldCfgItem{BLDCFG_USE_VARIABLE_MCT_ISOC_PRIORITY}
  IN MEMORY_POWER_POLICY   MemoryPowerPolicy;          ///< The memory's desired power policy
                                                       ///< @BldCfgItem{BLDCFG_MEMORY_POWER_POLICY_MODE}
  IN BOOLEAN               NbPstatesSupported;         ///< The Nb-Pstates is supported or not
                                                       ///< @BldCfgItem{BLDCFG_NB_PSTATES_SUPPORTED}
} PERFORMANCE_PROFILE;

///  Platform settings that describe the voltage regulator modules of the system.
///  Many power management settings are dependent upon the characteristics of the
///  on-board voltage regulator module (VRM).  The host environment needs to provide
///  these to specify how the system is to be configured.
typedef struct {
  IN UINT32  CurrentLimit;                         ///< Vrm Current Limit.
                                                   ///< @BldCfgItem{BLDCFG_VRM_CURRENT_LIMIT}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_CURRENT_LIMIT}
                                                   ///< @BldCfgItem{BLDCFG_VRM_GFX_CURRENT_LIMIT}
  IN UINT32  LowPowerThreshold;                    ///< Vrm Low Power Threshold.
                                                   ///< @BldCfgItem{BLDCFG_VRM_LOW_POWER_THRESHOLD}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_LOW_POWER_THRESHOLD}
                                                   ///< @BldCfgItem{BLDCFG_VRM_GFX_LOW_POWER_THRESHOLD}
  IN UINT32  SlewRate;                             ///< Vrm Slew Rate.
                                                   ///< @BldCfgItem{BLDCFG_VRM_SLEW_RATE}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_SLEW_RATE}
                                                   ///< @BldCfgItem{BLDCFG_VRM_GFX_SLEW_RATE}
  IN BOOLEAN HiSpeedEnable;                        ///< Select high speed VRM.
                                                   ///< @BldCfgItem{BLDCFG_VRM_HIGH_SPEED_ENABLE}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_HIGH_SPEED_ENABLE}
                                                   ///< @BldCfgItem{BLDCFG_VRM_GFX_HIGH_SPEED_ENABLE}
  IN UINT32  MaximumCurrentLimit;                  ///< Vrm Maximum Current Limit.
                                                   ///< @BldCfgItem{BLDCFG_VRM_MAXIMUM_CURRENT_LIMIT}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_MAXIMUM_CURRENT_LIMIT}
                                                   ///< @BldCfgItem{BLDCFG_VRM_GFX_MAXIMUM_CURRENT_LIMIT}
  IN UINT32  SviOcpLevel;                          ///< SVI OCP Level.
                                                   ///< @BldCfgItem{BLDCFG_VRM_SVI_OCP_LEVEL}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_SVI_OCP_LEVEL}
                                                   ///< @BldCfgItem{BLDCFG_VRM_GFX_SVI_OCP_LEVEL}
} PLATFORM_VRM_CONFIGURATION;

///  The VRM types to characterize.
typedef enum  {
  CoreVrm,                                       ///< VDD plane.
  NbVrm,                                         ///< VDDNB plane.
  GfxVrm,                                        ///< GFX plane.
  MaxVrmType                                     ///< Not a valid VRM type, use for limit checking.
} PLATFORM_VRM_TYPE;

///  The StapmBoost settings.
typedef enum  {
  StapmBoostDisabled,                            ///< Disable Stapm Boost
  StapmBoostEnabled,                             ///< Enable Stapm Boost
  StapmBoostSystemConfig                         ///< Default setting uses IRM programming setting
} STAPM_BOOST_TYPE;


///  These to specify how the system is to be configured for STAPM
typedef struct {
  IN UINT32  CfgStapmScalar;                      ///< Specify a % scalar to adjust this. Leave this unused for now until it is figured out.
                                                  ///< @BldCfgItem{BLDCFG_STAPM_SCALAR}
  IN UINT32  CfgStapmBoost;                       ///< If STAPM boost is enabled. System will actively track skin temperature and will allow higher performance till Skin temperature is below the limit
                                                  ///< @BldCfgItem{BLDCFG_STAPM_BOOST}
  IN UINT32  CfgStapmTimeConstant;                ///< Maximum power that the APU is allowed to consume when STAPM is enabled and Skin temperature is below the limit.
                                                  ///< Specified time in seconds.
                                                  ///< @BldCfgItem{BLDCFG_STAPM_TIME_CONSTANT}
} PLATFORM_STAPM_CONFIGURATION;

/// FCH Platform Configuration Policy
typedef struct {
  IN UINT16     CfgSmbus0BaseAddress;             ///< SMBUS0 Controller Base Address
  IN UINT16     CfgSmbus1BaseAddress;             ///< SMBUS1 Controller Base Address
  IN UINT16     CfgSioPmeBaseAddress;             ///< I/O base address for LPC I/O target range
  IN UINT16     CfgAcpiPm1EvtBlkAddr;             ///< I/O base address of ACPI power management Event Block
  IN UINT16     CfgAcpiPm1CntBlkAddr;             ///< I/O base address of ACPI power management Control Block
  IN UINT16     CfgAcpiPmTmrBlkAddr;              ///< I/O base address of ACPI power management Timer Block
  IN UINT16     CfgCpuControlBlkAddr;             ///< I/O base address of ACPI power management CPU Control Block
  IN UINT16     CfgAcpiGpe0BlkAddr;               ///< I/O base address of ACPI power management General Purpose Event Block
  IN UINT16     CfgSmiCmdPortAddr;                ///< I/O base address of ACPI SMI Command Block
  IN UINT16     CfgAcpiPmaCntBlkAddr;             ///< I/O base address of ACPI power management additional control block
  IN UINT32     CfgGecShadowRomBase;              ///< 32-bit base address to the GEC shadow ROM
  IN UINT32     CfgWatchDogTimerBase;             ///< Watchdog Timer base address
  IN UINT32     CfgSpiRomBaseAddress;             ///< Base address for the SPI ROM controller
  IN UINT32     CfgHpetBaseAddress;               ///< HPET MMIO base address
  IN UINT32     CfgAzaliaSsid;                    ///< Subsystem ID of HD Audio controller
  IN UINT32     CfgSmbusSsid;                     ///< Subsystem ID of SMBUS controller
  IN UINT32     CfgIdeSsid;                       ///< Subsystem ID of IDE controller
  IN UINT32     CfgSataAhciSsid;                  ///< Subsystem ID of SATA controller in AHCI mode
  IN UINT32     CfgSataIdeSsid;                   ///< Subsystem ID of SATA controller in IDE mode
  IN UINT32     CfgSataRaid5Ssid;                 ///< Subsystem ID of SATA controller in RAID5 mode
  IN UINT32     CfgSataRaidSsid;                  ///< Subsystem ID of SATA controller in RAID mode
  IN UINT32     CfgEhciSsid;                      ///< Subsystem ID of EHCI
  IN UINT32     CfgOhciSsid;                      ///< Subsystem ID of OHCI
  IN UINT32     CfgLpcSsid;                       ///< Subsystem ID of LPC ISA Bridge
  IN UINT32     CfgSdSsid;                        ///< Subsystem ID of SecureDigital controller
  IN UINT32     CfgXhciSsid;                      ///< Subsystem ID of XHCI
  IN BOOLEAN    CfgFchPort80BehindPcib;           ///< Is port80 cycle going to the PCI bridge
  IN BOOLEAN    CfgFchEnableAcpiSleepTrap;        ///< ACPI sleep SMI enable/disable
  IN GPP_LINKMODE CfgFchGppLinkConfig;            ///< GPP link configuration
  IN BOOLEAN    CfgFchGppPort0Present;            ///< Is FCH GPP port 0 present
  IN BOOLEAN    CfgFchGppPort1Present;            ///< Is FCH GPP port 1 present
  IN BOOLEAN    CfgFchGppPort2Present;            ///< Is FCH GPP port 2 present
  IN BOOLEAN    CfgFchGppPort3Present;            ///< Is FCH GPP port 3 present
  IN BOOLEAN    CfgFchGppPort0HotPlug;            ///< Is FCH GPP port 0 hotplug capable
  IN BOOLEAN    CfgFchGppPort1HotPlug;            ///< Is FCH GPP port 1 hotplug capable
  IN BOOLEAN    CfgFchGppPort2HotPlug;            ///< Is FCH GPP port 2 hotplug capable
  IN BOOLEAN    CfgFchGppPort3HotPlug;            ///< Is FCH GPP port 3 hotplug capable

  IN UINT8   CfgFchEsataPortBitMap;               ///< ESATA Port definition, eg: [0]=1, means port 0 is ESATA capable
  IN UINT8   CfgFchIrPinControl;                  ///< Register bitfield describing Infrared Pin Control:
                                                  ///<   [0] - IR Enable 0
                                                  ///<   [1] - IR Enable 1
                                                  ///<   [2] - IR Tx0
                                                  ///<   [3] - IR Tx1
                                                  ///<   [4] - IR Open Drain
                                                  ///<   [5] - IR Enable LED
  IN SD_CLOCK_CONTROL CfgFchSdClockControl;       ///< FCH SD Clock Control
  IN SCI_MAP_CONTROL  *CfgFchSciMapControl;       ///< FCH SCI Mapping Control
  IN SATA_PHY_CONTROL *CfgFchSataPhyControl;      ///< FCH SATA PHY Control
  IN GPIO_CONTROL     *CfgFchGpioControl;         ///< FCH GPIO Control
  IN BOOLEAN          CfgFchRtcWorkAround;        ///< FCH RTC Workaround
  IN BOOLEAN          CfgFchUsbPortDisWorkAround; ///< FCH USB Workaround
} FCH_PLATFORM_POLICY;


/// Build Option/Configuration Boolean Structure.
typedef struct {
  IN  AMD_CODE_HEADER VersionString;              ///< AMD embedded code version string

  //Build Option Area
  IN BOOLEAN OptionUDimms;                        ///< @ref BLDOPT_REMOVE_UDIMMS_SUPPORT "BLDOPT_REMOVE_UDIMMS_SUPPORT"
  IN BOOLEAN OptionRDimms;                        ///< @ref BLDOPT_REMOVE_RDIMMS_SUPPORT "BLDOPT_REMOVE_RDIMMS_SUPPORT"
  IN BOOLEAN OptionLrDimms;                       ///< @ref BLDOPT_REMOVE_LRDIMMS_SUPPORT "BLDOPT_REMOVE_LRDIMMS_SUPPORT"
  IN BOOLEAN OptionEcc;                           ///< @ref BLDOPT_REMOVE_ECC_SUPPORT "BLDOPT_REMOVE_ECC_SUPPORT"
  IN BOOLEAN OptionBankInterleave;                ///< @ref BLDOPT_REMOVE_BANK_INTERLEAVE "BLDOPT_REMOVE_BANK_INTERLEAVE"
  IN BOOLEAN OptionDctInterleave;                 ///< @ref BLDOPT_REMOVE_DCT_INTERLEAVE "BLDOPT_REMOVE_DCT_INTERLEAVE"
  IN BOOLEAN OptionNodeInterleave;                ///< @ref BLDOPT_REMOVE_NODE_INTERLEAVE "BLDOPT_REMOVE_NODE_INTERLEAVE"
  IN BOOLEAN OptionParallelTraining;              ///< @ref BLDOPT_REMOVE_PARALLEL_TRAINING "BLDOPT_REMOVE_PARALLEL_TRAINING"
  IN BOOLEAN OptionOnlineSpare;                   ///< @ref BLDOPT_REMOVE_ONLINE_SPARE_SUPPORT "BLDOPT_REMOVE_ONLINE_SPARE_SUPPORT"
  IN BOOLEAN OptionMemRestore;                    ///< @ref BLDOPT_REMOVE_MEM_RESTORE_SUPPORT "BLDOPT_REMOVE_MEM_RESTORE_SUPPORT"
  IN BOOLEAN OptionAcpiPstates;                   ///< @ref BLDOPT_REMOVE_ACPI_PSTATES "BLDOPT_REMOVE_ACPI_PSTATES"
  IN BOOLEAN OptionCrat;                          ///< @ref BLDOPT_REMOVE_CRAT "BLDOPT_REMOVE_CRAT"
  IN BOOLEAN OptionCdit;                          ///< @ref BLDOPT_REMOVE_CDIT "BLDOPT_REMOVE_CDIT"
  IN BOOLEAN OptionWhea;                          ///< @ref BLDOPT_REMOVE_WHEA "BLDOPT_REMOVE_WHEA"
  IN BOOLEAN OptionDmi;                           ///< @ref BLDOPT_REMOVE_DMI "BLDOPT_REMOVE_DMI"
  IN BOOLEAN OptionEarlySamples;                  ///< @ref BLDOPT_REMOVE_EARLY_SAMPLES "BLDOPT_REMOVE_EARLY_SAMPLES"
  IN BOOLEAN OptionAddrToCsTranslator;            ///< ADDR_TO_CS_TRANSLATOR

  //Build Configuration Area
  IN UINT64 CfgPciMmioAddress;                    ///< Pci Mmio Base Address to use for PCI Config accesses.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_PCI_MMIO_BASE}
  IN UINT32 CfgPciMmioSize;                       ///< Pci Mmio region Size.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_PCI_MMIO_SIZE}
  IN PLATFORM_VRM_CONFIGURATION CfgPlatVrmCfg[MaxVrmType]; ///< Several configuration settings for the voltage regulator modules.
  IN UINT32 CfgCpuFrequencyLimit;                 ///< CPU frequency limit
  IN PLATFORM_CONNECTED_STANDBY_MODES CfgPlatformConnectedStandbyMode; ///< Enable or disable connected standby
  IN UINT32 CfgPlatNumIoApics;                    ///< The number of IO APICS for the platform.
  IN UINT32 CfgMemInitPstate;                     ///< Memory Init Pstate.
  IN PLATFORM_CSTATE_MODES CfgPlatformCStateMode; ///< Select the C-State Mode that will used.
  IN UINT32 CfgPlatformCStateOpData;              ///< An IO port or additional C-State setup data, depends on C-State mode.
  IN UINT16 CfgPlatformCStateIoBaseAddress;       ///< Specifies I/O ports that can be used to allow CPU to enter CStates
  IN PLATFORM_CPB_MODES CfgPlatformCpbMode;       ///< Enable or disable core performance boost
  IN UINT32 CfgCoreLevelingMode;                  ///< Apply any downcoring or core count leveling as specified.
  IN PERFORMANCE_PROFILE CfgPerformanceProfile;   ///< The platform's control flow mode and platform performance settings.

  IN UINT32 CfgAmdPlatformType;                   ///< Designate the platform as a Server, Desktop, or Mobile.
  IN UINT32 CfgAmdPowerCeiling;                   ///< PowerCeiling, specifies a maximum power usage limit for the platform
  IN UINT16 CfgHtcTemperatureLimit;               ///< Hardware Thermal Control temperature limit in tenths of degrees Celsius.
  IN UINT16 CfgLhtcTemperatureLimit;              ///< Local Hardware Thermal Control temperature limit in tenths of degrees Celsius.

  IN UINT32 CfgMemoryBusFrequencyLimit;           ///< Memory Bus Frequency Limit.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT}
  IN BOOLEAN CfgMemoryModeUnganged;               ///< Memory Mode Unganged.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_MODE_UNGANGED}
  IN BOOLEAN CfgMemoryQuadRankCapable;            ///< Memory Quad Rank Capable.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_QUAD_RANK_CAPABLE}
  IN UINT32 CfgMemoryQuadrankType;        ///< Memory Quadrank Type.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_QUADRANK_TYPE}
  IN BOOLEAN CfgMemoryRDimmCapable;               ///< Memory RDIMM Capable.
  IN BOOLEAN CfgMemoryLRDimmCapable;              ///< Memory LRDIMM Capable.
  IN BOOLEAN CfgMemoryUDimmCapable;               ///< Memory UDIMM Capable.
  IN BOOLEAN CfgMemorySODimmCapable;              ///< Memory SODimm Capable.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_SODIMM_CAPABLE}
  IN BOOLEAN CfgLimitMemoryToBelow1Tb;            ///< Limit memory address space to below 1TB
  IN BOOLEAN CfgMemoryEnableBankSwapOnly;         ///< Memory Enable Bank Swap Only @BldCfgItem{BLDCFG_MEMORY_ENABLE_BANK_SWAP_ONLY}
  IN BOOLEAN CfgMemoryEnableBankInterleaving;     ///< Memory Enable Bank Interleaving.
  IN BOOLEAN CfgMemoryEnableNodeInterleaving;     ///< Memory Enable Node Interleaving.
  IN BOOLEAN CfgMemoryChannelInterleaving;        ///< Memory Channel Interleaving.
  IN BOOLEAN CfgMemoryPowerDown;                  ///< Memory Power Down.
  IN UINT8   CfgMemoryMacDefault;                 ///< Memory DRAM MAC Default
  IN BOOLEAN CfgMemoryExtendedTemperatureRange;   ///< Memory Extended Temperature Range
  IN BOOLEAN CfgDramTempControlledRefreshEn;      ///< Temperature Controlled Refresh Rate - @BldCfgItem{BLDCFG_DRAM_TEMP_CONTROLLED_REFRESH_EN}
  IN UINT32  CfgPowerDownMode;                    ///< Power Down Mode.
  IN BOOLEAN CfgOnlineSpare;                      ///< Online Spare.
  IN BOOLEAN CfgMemoryParityEnable;               ///< Memory Parity Enable.
  IN BOOLEAN CfgBankSwizzle;                      ///< Bank Swizzle.
  IN UINT32  CfgTimingModeSelect;                 ///< Timing Mode Select.
  IN UINT32  CfgMemoryClockSelect;                ///< Memory Clock Select.
  IN BOOLEAN CfgDqsTrainingControl;               ///< Dqs Training Control.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_DQS_TRAINING_CONTROL}
  IN BOOLEAN CfgIgnoreSpdChecksum;                ///< Ignore Spd Checksum.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_IGNORE_SPD_CHECKSUM}
  IN BOOLEAN CfgUseBurstMode;                     ///< Use Burst Mode.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_USE_BURST_MODE}
  IN BOOLEAN CfgMemoryAllClocksOn;                ///< Memory All Clocks On.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_ALL_CLOCKS_ON}
  IN BOOLEAN CfgDdrPhyDllBypassMode;              ///< Enable DllPDBypassMode
  IN BOOLEAN CfgEnableEccFeature;                 ///< Enable ECC Feature.
  IN BOOLEAN CfgEccRedirection;                   ///< ECC Redirection.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_ECC_REDIRECTION}
  IN UINT16  CfgScrubDramRate;                    ///< Scrub Dram Rate.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_DRAM_RATE}
  IN UINT16  CfgScrubL2Rate;                      ///< Scrub L2Rate.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_L2_RATE}
  IN UINT16  CfgScrubL3Rate;                      ///< Scrub L3Rate.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_L3_RATE}
  IN UINT16  CfgScrubIcRate;                      ///< Scrub Ic Rate.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_IC_RATE}
  IN UINT16  CfgScrubDcRate;                      ///< Scrub Dc Rate.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_DC_RATE}
  IN BOOLEAN CfgEccSyncFlood;                     ///< ECC Sync Flood.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_ECC_SYNC_FLOOD}
  IN UINT16  CfgEccSymbolSize;                    ///< ECC Symbol Size.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_ECC_SYMBOL_SIZE}
  IN UINT64  CfgHeapDramAddress;                  ///< Heap contents will be temporarily stored in this address during the transition.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_HEAP_DRAM_ADDRESS}
  IN BOOLEAN CfgNodeMem1GBAlign;                  ///< Node Mem 1GB boundary Alignment
  IN BOOLEAN CfgS3LateRestore;                    ///< S3 Late Restore
  IN BOOLEAN CfgAcpiPstateIndependent;            ///< PSD method dependent/Independent
  IN UINT32  CfgAcpiPstatesPsdPolicy;             ///< PSD policy
  IN UMA_VERSION CfgUmaVersion;                   ///< Uma Version
  IN UMA_MODE CfgUmaMode;                         ///< Uma Mode
  IN UINT32 CfgUmaSize;                           ///< Uma Size [31:0]=Addr[47:16]
  IN BOOLEAN CfgUmaAbove4G;                       ///< Uma Above 4G Support
  IN UMA_ALIGNMENT CfgUmaAlignment;               ///< Uma alignment
  IN BOOLEAN CfgProcessorScopeInSb;               ///< ACPI Processor Object in \\_SB scope
  IN CHAR8   CfgProcessorScopeName0;              ///< OEM specific 1st character of processor scope name.
  IN CHAR8   CfgProcessorScopeName1;              ///< OEM specific 2nd character of processor scope name.
  IN UINT8   CfgGnbHdAudio;                       ///< GNB HD Audio
  IN UINT8   CfgAbmSupport;                       ///< Abm Support
  IN UINT8   CfgDynamicRefreshRate;               ///< DRR Dynamic Refresh Rate
  IN UINT16  CfgLcdBackLightControl;              ///< LCD Backlight Control
  IN UINT8   CfgGnb3dStereoPinIndex;                ///< 3D Stereo Pin ID.
  IN UINT32  CfgTempPcieMmioBaseAddress;          ///< Temp pcie MMIO base Address
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_TEMP_PCIE_MMIO_BASE_ADDRESS}
  IN UINT32  CfgGnbIGPUSSID;                      ///< Gnb internal GPU SSID
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_IGPU_SUBSYSTEM_ID}
  IN UINT32  CfgGnbHDAudioSSID;                   ///< Gnb HD Audio SSID
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_IGPU_HD_AUDIO_SUBSYSTEM_ID}
  IN UINT32  CfgGnbPcieSSID;                      ///< Gnb PCIe SSID
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_APU_PCIE_PORTS_SUBSYSTEM_ID}
  IN UINT16  CfgLvdsSpreadSpectrum;               ///< Lvds Spread Spectrum
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_GFX_LVDS_SPREAD_SPECTRUM}
  IN UINT16  CfgLvdsSpreadSpectrumRate;           ///< Lvds Spread Spectrum Rate
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_GFX_LVDS_SPREAD_SPECTRUM_RATE}
  IN FCH_PLATFORM_POLICY  *FchBldCfg;             ///< FCH platform build configuration policy

  IN BOOLEAN    CfgIommuSupport;                  ///< IOMMU support
  IN UINT8      CfgLvdsPowerOnSeqDigonToDe;       ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqDeToVaryBl;      ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqDeToDigon;       ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqVaryBlToDe;      ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqOnToOffDelay;    ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqVaryBlToBlon;    ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqBlonToVaryBl;    ///< Panel initialization timing
  IN UINT16     CfgLvdsMaxPixelClockFreq;         ///< The maximum pixel clock frequency supported
  IN UINT32     CfgLcdBitDepthControlValue;       ///< The LCD bit depth control settings
  IN UINT8      CfgLvds24bbpPanelMode;            ///< The LVDS 24 BBP mode
  IN LVDS_MISC_CONTROL CfgLvdsMiscControl;        ///< THe LVDS Misc control
  IN UINT16     CfgPcieRefClkSpreadSpectrum;      ///< PCIe Reference Clock Spread Spectrum
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_PCIE_REFCLK_SPREAD_SPECTRUM}
  IN BOOLEAN    CfgExternalVrefCtlFeature;        ///< External Vref control
  IN FORCE_TRAIN_MODE   CfgForceTrainMode;        ///< Force Train Mode
  IN BOOLEAN    CfgGnbRemoteDisplaySupport;       ///< Wireless Display Support
  IN IOMMU_EXCLUSION_RANGE_DESCRIPTOR *CfgIvrsExclusionRangeList;
  IN BOOLEAN    CfgGnbSyncFloodPinAsNmi;          ///< This item defines the function of the SYNCFLOOD/NMI# pin
                                                  ///< @BldCfgItem{BLDCFG_USE_SYNCFLOOD_AS_NMI}
  IN UINT8      CfgIgpuEnableDisablePolicy;       ///< This item defines the iGPU Enable/Disable policy
                                                  ///< @li 0 = Auto - use current default
                                                  ///< @li 2 = Disable iGPU if ANY PCI or PCIe Graphics card is present
                                                  ///< @BldCfgItem{BLDCFG_IGPU_ENABLE_DISABLE_POLICY}
  IN UINT8      CfgGnbSwTjOffset;                 ///< Software-writeable TjOffset to account for changes in junction temperature
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_GNB_THERMAL_SENSOR_CORRECTION}
  IN UINT8      CfgLvdsMiscVoltAdjustment;        ///< Register LVDS_CTRL_4 to adjust LVDS output voltage
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_LVDS_MISC_VOL_ADJUSTMENT}
  IN DISPLAY_MISC_CONTROL CfgDisplayMiscControl;  ///< The Display Misc control
  IN DP_FIXED_VOLT_SWING_TYPE CfgDpFixedVoltSwingType;///< To indicate fixed voltage swing value
                                                      ///< @BldCfgItem{BLDCFG_DP_FIXED_VOLT_SWING}
  IN TECHNOLOGY_TYPE CfgDimmTypeUsedInMixedConfig; ///< Select the preferred technology type that AGESA will enable
                                                   ///< when it is mixed with other technology types.
  IN BOOLEAN CfgDimmTypeDdr4Capable;              ///< Select DDR4 as technology type that AGESA will enable
                                                  ///< @BldCfgItem{BLDCFG_DIMM_TYPE_DDR4_CAPABLE}
  IN BOOLEAN CfgDimmTypeDdr3Capable;              ///< Select DDR3 as technology type that AGESA will enable
                                                  ///< @BldCfgItem{BLDCFG_DIMM_TYPE_DDR3_CAPABLE}
  IN BOOLEAN CfgHybridBoostEnable;                ///< HyBrid Boost support
                                                  ///< @BldCfgItem{BLDCFG_HYBRID_BOOST_ENABLE}
  IN UINT64  CfgGnbIoapicAddress;                 ///< GNB IOAPIC Base Address(NULL if platform configured)
                                                  ///< @BldCfgItem{BLDCFG_GNB_IOAPIC_ADDRESS}
  IN BOOLEAN CfgDataEyeEn;                        ///< Enable get 2D Data Eye
  IN UINT32 CfgBatteryBoostTune;                  ///< @BldCfgItem{BLDCFG_BATTERY_BOOST_TUNE}
  IN BOOLEAN CfgDramDoubleRefreshRateEn;          ///< Double DRAM refresh rate
  IN DISPLAY_RESOLUTION CfgGnbResolution;         ///< Display Resolution
                                                  ///< @BldCfgItem{BLDCFG_RESOLUTION}
  IN ACP_SIZE  CfgGnbAcpSize;                     ///< ACP size [31:0]=Addr[47:16]
                                                  ///< @BldCfgItem{BLDCFG_ACP_SIZE}
  IN PMU_TRAIN_MODE   CfgPmuTrainMode;            ///< Force Train Mode
                                                  ///< @BldCfgItem{BLDCFG_PMU_TRAINING_MODE}
  IN UINT8 CfgMemoryPhyVoltage;                   ///< Memory Phy voltage (VDDR)
                                                  ///< @BldCfgItem{BLDCFG_MEMORY_PHY_VOLTAGE}
  IN UINT32 CfgGpuFrequencyLimit;                 ///< @BldCfgItem{BLDCFG_GPU_FREQUENCY_LIMIT}
  IN UINT8 CfgMaxNumAudioEndpoints;               ///< @BldCfgItem{BLDCFG_MAX_NUM_AUDIO_ENDPOINTS}
  IN BOOLEAN CfgBapmEnable;                       ///< @BldCfgItem{BLDCFG_BAPM_ENABLE}
  IN UINT32  CfgGnbAzI2sBusSelect;                ///< Acp AZ/I2sBus select
                                                  ///< @BldCfgItem{BLDCFG_GNB_AZ_I2SBUS_SELECT}
  IN UINT32  CfgGnbAzI2sBusPinConfig;             ///< Acp AZ/I2sBus pin configuration
                                                  ///< @BldCfgItem{BLDCFG_GNB_AZ_I2SBUS_PIN_CONFIG}
  IN UINT32  CfgPkgPwrLimitAC;                    ///< Package Power Limit under AC
                                                  ///< @BldCfgItem{BLDCFG_PPT_LIMIT_AC}
  IN UINT32  CfgPkgPwrLimitDC;                    ///< Package Power Limit under DC
                                                  ///< @BldCfgItem{BLDCFG_PPT_LIMIT_DC}
  IN UINT32  CfgSystemConfiguration;              ///< SMU System Configuration
                                                  ///< @BldCfgItem{BLDCFG_SYSTEM_CONFIGURATION}
  IN PLATFORM_STAPM_CONFIGURATION CfgPlatStapmConfig; ///< Several configuration settings for the STAPM.
  IN UINT8  CfgEDPv1_4VSMode;                     ///< @BldCfgItem{BLDCFG_EDP_V1_4_VS_MODE}
  IN UINT8  CfgExtHDMIReDrvSlvAddr;               ///< @BldCfgItem{BLDCFG_EXT_HDMI_RE_DRIVE_SLAVE_ADDR}
  IN UINT8  CfgExtHDMIReDrvRegNum;                ///< @BldCfgItem{BLDCFG_EXT_HDMI_RE_DRIVE_REG_NUM}
  IN UINT64 CfgExtHDMIRegSetting;                 ///< @BldCfgItem{BLDCFG_EXT_HDMI_RE_DRIVE_REG_INFO}
  IN UINT32 CfgThermCtlLimit;                     ///< @BldCfgItem{BLDCFG_THERMCTL_LIMIT}
  IN UINT64 CfgCodecVerbTable;                    ///< @BldCfgItem{BLDCFG_CODEC_VERB_TABLE}
  IN UINT32 CfgGnbAzSsid;                         ///< @BldCfgItem{BLDCFG_GNB_AZ_SSID}
  IN UINT16  CfgCustomVddioVoltage;               ///< Custom VDDIO voltage
                                                  ///< @BldCfgItem{BLDCFG_CUSTOM_VDDIO_VOLTAGE}
  IN BOOLEAN CfgAcpPowerGating;                   ///< @BldCfgItem{BLDCFG_ACP_POWER_GATING}
  IN BOOLEAN Reserved;                            ///< reserved...
} BUILD_OPT_CFG;

///  A structure containing platform specific operational characteristics. This
///  structure is initially populated by the initializer with a copy of the same
///  structure that was created at build time using the build configuration controls.
typedef struct _PLATFORM_CONFIGURATION {
  IN SOC_LOGICAL_ID      CpuLogicalId;                ///< Logical Id for the executing processor.
  IN PERFORMANCE_PROFILE PlatformProfile;             ///< Several configuration settings for the processor.
  IN UINT8               CoreLevelingMode;            ///< Indicates how to balance the number of cores per processor.
                                                      ///< @BldCfgItem{BLDCFG_CORE_LEVELING_MODE}
  IN PLATFORM_CSTATE_MODES  CStateMode;               ///< Specifies the method of C-State enablement - Disabled, or C6.
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_CSTATE_MODE}
  IN UINT32              CStatePlatformData;          ///< This element specifies some pertinent data needed for the operation of the Cstate feature
                                                      ///< If CStateMode is CStateModeC6, this item is reserved
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_CSTATE_OPDATA}
  IN UINT16              CStateIoBaseAddress;         ///< This item specifies a free block of 8 consecutive bytes of I/O ports that
                                                      ///< can be used to allow the CPU to enter Cstates.
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_CSTATE_IO_BASE_ADDRESS}
  IN PLATFORM_CPB_MODES  CpbMode;                     ///< Specifies the method of core performance boost enablement - Disabled, or Auto.
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_CPB_MODE}
  IN BOOLEAN             UserOptionDmi;               ///< When set to TRUE, the DMI data table is generated.
  IN BOOLEAN             UserOptionPState;            ///< When set to TRUE, the PState data tables are generated.
  IN BOOLEAN             UserOptionCrat;              ///< When set to TRUE, the CRAT data table is generated.
  IN BOOLEAN             UserOptionCdit;              ///< When set to TRUE, the CDIT data table is generated.
  IN BOOLEAN             UserOptionWhea;              ///< When set to TRUE, the WHEA data table is generated.
  IN UINT32              PowerCeiling;                ///< P-State Ceiling Enabling Deck - Max power milli-watts.
  IN BOOLEAN             ForcePstateIndependent;      ///< Deprecated in favor of PstatesPsdPolicy.
                                                      ///< P-State _PSD is forced independent.
                                                      ///< @BldCfgItem{BLDCFG_FORCE_INDEPENDENT_PSD_OBJECT}
  IN UINT32              PstatesPsdPolicy;            ///< PSD policy
                                                      ///< @BldCfgItem{BLDCFG_ACPI_PSTATES_PSD_POLICY}
  IN UINT32              CpuFrequencyLimit;           ///< @BldCfgItem{BLDCFG_CPU_FREQUENCY_LIMIT}
  IN PLATFORM_CONNECTED_STANDBY_MODES CfgPlatformConnectedStandbyMode; ///< @BldCfgItem{BLDCFG_CPU_CONNECTED_STANDBY_MODE}
  IN UINT32              NumberOfIoApics;             ///< Number of I/O APICs in the system
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_NUM_IO_APICS}
  IN PLATFORM_VRM_CONFIGURATION VrmProperties[MaxVrmType]; ///< Several configuration settings for the voltage regulator modules.
  IN BOOLEAN             ProcessorScopeInSb;          ///< ACPI Processor Object in \\_SB scope
                                                      ///< @BldCfgItem{BLDCFG_PROCESSOR_SCOPE_IN_SB}
  IN CHAR8               ProcessorScopeName0;         ///< OEM specific 1st character of processor scope name.
                                                      ///< @BldCfgItem{BLDCFG_PROCESSOR_SCOPE_NAME0}
  IN CHAR8               ProcessorScopeName1;         ///< OEM specific 2nd character of processor scope name.
                                                      ///< @BldCfgItem{BLDCFG_PROCESSOR_SCOPE_NAME1}
  IN UINT8               GnbHdAudio;                  ///< Control GFX HD Audio controller(Used for HDMI and DP display output),
                                                      ///< essentially it enables function 1 of graphics device.
                                                      ///< @li 0 = HD Audio disable
                                                      ///< @li 1 = HD Audio enable
                                                      ///< @BldCfgItem{BLDCFG_CFG_GNB_HD_AUDIO}
  IN UINT8               AbmSupport;                  ///< Automatic adjust LVDS/eDP Back light level support.It is
                                                      ///< characteristic specific to display panel which used by platform design.
                                                      ///< @li 0 = ABM support disabled
                                                      ///< @li 1 = ABM support enabled
                                                      ///< @BldCfgItem{BLDCFG_CFG_ABM_SUPPORT}
  IN UINT8               DynamicRefreshRate;          ///< Adjust refresh rate on LVDS/eDP.
                                                      ///< @BldCfgItem{BLDCFG_CFG_DYNAMIC_REFRESH_RATE}
  IN UINT16              LcdBackLightControl;         ///< The PWM frequency to LCD backlight control.
                                                      ///< If equal to 0 backlight not controlled by iGPU
                                                      ///< @BldCfgItem{BLDCFG_CFG_LCD_BACK_LIGHT_CONTROL}
  IN UINT16              HtcTemperatureLimit;         ///< The Hardware Thermal Control temperature limit in tenths of degrees Celsius.
                                                      ///< If equal to 0, use hardware defaults.
                                                      ///< @BldCfgItem{BLDCFG_HTC_TEMPERATURE_LIMIT}
  IN UINT16              LhtcTemperatureLimit;        ///< The Local Hardware Thermal Control temperature limit in tenths of degrees Celsius.
                                                      ///< If equal to 0, use hardware defaults.
                                                      ///< @BldCfgItem{BLDCFG_LHTC_TEMPERATURE_LIMIT}
  IN DISPLAY_RESOLUTION  Resolution;                  ///< Display Resolution
                                                      ///< @BldCfgItem{BLDCFG_RESOLUTION}
  IN ACP_SIZE            AcpSize;                     ///< The size of ACP dram
                                                      ///< ACP_Size[31:0]=Addr[47:16]
                                                      ///< @BldCfgItem{BLDCFG_ACP_SIZE}
  IN UINT32              BatteryBoostTune;            ///< @BldCfgItem{BLDCFG_BATTERY_BOOST_TUNE}
  IN UINT32              PkgPwrLimitAC;               ///< Package Power Limit under AC
                                                      ///< @BldCfgItem{BLDCFG_PPT_LIMIT_AC}
  IN UINT32              PkgPwrLimitDC;               ///< Package Power Limit under DC
                                                      ///< @BldCfgItem{BLDCFG_PPT_LIMIT_DC}
  IN UINT32              SystemConfiguration;         ///< SMU System Configuration
                                                      ///< @BldCfgItem{BLDCFG_SYSTEM_CONFIGURATION}
  IN PLATFORM_STAPM_CONFIGURATION PlatStapmConfig; ///< Several configuration settings for the STAPM.
  IN UINT32              ThermCtlLimit;               ///< @BldCfgItem{BLDCFG_THERMCTL_LIMIT}
  IN UINT64              AzaliaCodecVerbTable;        ///< @BldCfgItem{BLDCFG_CODEC_VERB_TABLE}
  IN UINT32              AzaliaSsid;                  ///< @BldCfgItem{BLDCFG_GNB_AZ_SSID}
  IN UINT32              GnbAzI2sBusSelect;           ///< @BldCfgItem{BLDCFG_GNB_AZ_I2SBUS_SELECT}
  IN UINT32              GnbAzI2sBusPinConfig;        ///< @BldCfgItem{BLDCFG_GNB_AZ_I2SBUS_PIN_CONFIG}
  IN BOOLEAN             AcpPowerGating;              ///< @BldCfgItem{BLDCFG_ACP_POWER_GATING}
  IN BOOLEAN             NbPstateEnabled;             ///< Flag to indicate whether Nb Pstate is enabled
} PLATFORM_CONFIGURATION;


/**********************************************************************
 * Structures for: AmdInitLate
 **********************************************************************/

/**********************************************************************
 * Interface call:  AllocateExecutionCache
 **********************************************************************/
#define MAX_CACHE_REGIONS    3

/// AllocateExecutionCache sub param structure for cached memory region
typedef struct {
  IN OUT   UINT32               ExeCacheStartAddr;      ///< Start address
  IN OUT   UINT32               ExeCacheSize;           ///< Size
} EXECUTION_CACHE_REGION;

/**********************************************************************
 * Interface call:  AmdGetAvailableExeCacheSize
 **********************************************************************/
/// Get available Cache remain
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS    StdHeader;              ///< Standard configuration header
     OUT   UINT32               AvailableExeCacheSize;  ///< Remain size
} AMD_GET_EXE_SIZE_PARAMS;

AGESA_STATUS
AmdGetAvailableExeCacheSize (
  IN OUT   AMD_GET_EXE_SIZE_PARAMS *AmdGetExeSizeParams
  );

/// Selection type for core leveling
typedef enum {
  CORE_LEVEL_LOWEST,            ///< Level to lowest common denominator
  CORE_LEVEL_TWO,               ///< Level to 2 cores
  CORE_LEVEL_POWER_OF_TWO,      ///< Level to 1,2,4 or 8
  CORE_LEVEL_NONE,              ///< Do no leveling
  CORE_LEVEL_COMPUTE_UNIT,      ///< Level cores to one core per compute unit
  CORE_LEVEL_COMPUTE_UNIT_TWO,  ///< Level cores to two cores per compute unit
  CORE_LEVEL_COMPUTE_UNIT_THREE,  ///< Level cores to three cores per compute unit
  CORE_LEVEL_ONE,               ///< Level to 1 core
  CORE_LEVEL_THREE,             ///< Level to 3 cores
  CORE_LEVEL_FOUR,              ///< Level to 4 cores
  CORE_LEVEL_FIVE,              ///< Level to 5 cores
  CORE_LEVEL_SIX,               ///< Level to 6 cores
  CORE_LEVEL_SEVEN,             ///< Level to 7 cores
  CORE_LEVEL_EIGHT,             ///< Level to 8 cores
  CORE_LEVEL_NINE,              ///< Level to 9 cores
  CORE_LEVEL_TEN,               ///< Level to 10 cores
  CORE_LEVEL_ELEVEN,            ///< Level to 11 cores
  CORE_LEVEL_TWELVE,            ///< Level to 12 cores
  CORE_LEVEL_THIRTEEN,          ///< Level to 13 cores
  CORE_LEVEL_FOURTEEN,          ///< Level to 14 cores
  CORE_LEVEL_FIFTEEN,           ///< Level to 15 cores
  CoreLevelModeMax              ///< Used for bounds checking
} CORE_LEVELING_TYPE;





/************************************************************************
 *
 *  AGESA Basic Level interface structure definition and function prototypes
 *
 ***********************************************************************/

/**********************************************************************
 * Interface call:  AmdCreateStruct
 **********************************************************************/
AGESA_STATUS
AmdCreateStruct (
  IN OUT   AMD_INTERFACE_PARAMS *InterfaceParams
  );

/**********************************************************************
 * Interface call:  AmdReleaseStruct
 **********************************************************************/
AGESA_STATUS
AmdReleaseStruct (
  IN OUT   AMD_INTERFACE_PARAMS *InterfaceParams
  );

/**********************************************************************
 * Interface call:  AmdInitReset
 **********************************************************************/
/// AmdInitReset param structure
typedef struct {
  IN       AMD_CONFIG_PARAMS         StdHeader;        ///< Standard configuration header
  IN       EXECUTION_CACHE_REGION    CacheRegion[3];   ///< The cached memory region
  IN       FCH_RESET_INTERFACE       FchInterface;     ///< Interface for FCH configuration
} AMD_RESET_PARAMS;

AGESA_STATUS
AmdInitReset (
  IN OUT   AMD_RESET_PARAMS     *ResetParams
  );


/**********************************************************************
 * Interface call:  AmdInitEarly
 **********************************************************************/
/// InitEarly param structure
///
/// Provide defaults or customizations to each service performed in AmdInitEarly.
///
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;        ///< Standard configuration header
  IN       EXECUTION_CACHE_REGION CacheRegion[3];   ///< Execution Map Interface
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
  IN       GNB_CONFIGURATION      GnbConfig;        ///< GNB configuration
} AMD_EARLY_PARAMS;

AGESA_STATUS
AmdInitEarly (
  IN OUT   AMD_EARLY_PARAMS     *EarlyParams
  );


/**********************************************************************
 * Interface call:  AmdInitPost
 **********************************************************************/
/// AmdInitPost param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;        ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
  IN       MEM_PARAMETER_STRUCT   MemConfig;        ///< Memory post param
  IN       GNB_POST_CONFIGURATION GnbPostConfig;    ///< GNB post param
} AMD_POST_PARAMS;

AGESA_STATUS
AmdInitPost (
  IN OUT   AMD_POST_PARAMS      *PostParams         ///< Amd Cpu init param
  );


/**********************************************************************
 * Interface call:  AmdInitEnv
 **********************************************************************/
/// AmdInitEnv param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;            ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;       ///< platform operational characteristics.
  IN       GNB_ENV_CONFIGURATION  GnbEnvConfiguration;  ///< GNB configuration
  IN       FCH_INTERFACE          FchInterface;         ///< FCH configuration
} AMD_ENV_PARAMS;

AGESA_STATUS
AmdInitEnv (
  IN OUT   AMD_ENV_PARAMS       *EnvParams
  );


/**********************************************************************
 * Interface call:  AmdInitMid
 **********************************************************************/
/// AmdInitMid param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;        ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
  IN       GNB_MID_CONFIGURATION  GnbMidConfiguration;  ///< GNB configuration
  IN       FCH_INTERFACE          FchInterface;     ///< FCH configuration
} AMD_MID_PARAMS;

AGESA_STATUS
AmdInitMid (
  IN OUT   AMD_MID_PARAMS       *MidParams
  );


/**********************************************************************
 * Interface call:  AmdInitLate
 **********************************************************************/
/// AmdInitLate param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;              ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;         ///< platform operational characteristics.
  IN       IOMMU_EXCLUSION_RANGE_DESCRIPTOR  *IvrsExclusionRangeList;   ///< Pointer to array of exclusion ranges
     OUT   VOID                   *AcpiPState;            ///< Acpi Pstate SSDT Table
     OUT   VOID                   *AcpiWheaMce;           ///< WHEA MCE Table
     OUT   VOID                   *AcpiWheaCmc;           ///< WHEA CMC Table
     OUT   VOID                   *AcpiAlib;              ///< ACPI SSDT table with ALIB implementation
     OUT   VOID                   *AcpiIvrs;              ///< IOMMU ACPI IVRS(I/O Virtualization Reporting Structure) table
     OUT   VOID                   *AcpiCrat;              ///< Component Resource Affinity Table table
     OUT   VOID                   *AcpiCdit;              ///< Component Locality Distance Information table
  IN       GNB_LATE_CONFIGURATION GnbLateConfiguration;   ///< GNB configuration
} AMD_LATE_PARAMS;

AGESA_STATUS
AmdInitLate (
  IN OUT   AMD_LATE_PARAMS      *LateParams
  );

/**********************************************************************
 * Interface call:  AmdInitRecovery
 **********************************************************************/
/// CPU Recovery Parameters
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS StdHeader;             ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
} AMD_CPU_RECOVERY_PARAMS;

/// AmdInitRecovery param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS        StdHeader;            ///< Standard configuration header
  IN       MEM_PARAMETER_STRUCT     MemConfig;            ///< Memory post param
  IN       EXECUTION_CACHE_REGION   CacheRegion[3];       ///< The cached memory region. And the max cache region is 3
  IN       AMD_CPU_RECOVERY_PARAMS  CpuRecoveryParams;    ///< Params for CPU related recovery init.
} AMD_RECOVERY_PARAMS;

AGESA_STATUS
AmdInitRecovery (
  IN OUT   AMD_RECOVERY_PARAMS    *RecoveryParams
  );

/**********************************************************************
 * Interface call:  AmdInitResume
 **********************************************************************/
/// AmdInitResume param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;      ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig; ///< Platform operational characteristics
  IN       S3_DATA_BLOCK          S3DataBlock;    ///< Save state data
} AMD_RESUME_PARAMS;

AGESA_STATUS
AmdInitResume (
  IN       AMD_RESUME_PARAMS    *ResumeParams
  );


/**********************************************************************
 * Interface call:  AmdS3LateRestore
 **********************************************************************/
/// AmdS3LateRestore param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS    StdHeader;          ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< Platform operational characteristics.
  IN       S3_DATA_BLOCK          S3DataBlock;      ///< Save state data
} AMD_S3LATE_PARAMS;

AGESA_STATUS
AmdS3LateRestore (
  IN OUT   AMD_S3LATE_PARAMS    *S3LateParams
  );


/**********************************************************************
 * Interface call:  AmdS3FinalRestore
 **********************************************************************/
/// AmdS3FinalRestore param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS    StdHeader;          ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< Platform operational characteristics.
  IN       S3_DATA_BLOCK          S3DataBlock;      ///< Save state data
} AMD_S3FINAL_PARAMS;

AGESA_STATUS
AmdS3FinalRestore (
  IN OUT   AMD_S3FINAL_PARAMS    *S3FinalParams
  );

/**********************************************************************
 * Interface call:  AmdInitRtb
 **********************************************************************/
/// AmdInitRtb param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS    StdHeader;          ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
  IN       VOID                   *OptionMemoryInstallPtr;    ///< Pointer to OptionMemoryInstall
     OUT   S3_DATA_BLOCK          S3DataBlock;      ///< Save state data
  IN       FCH_INTERFACE          FchInterface;     ///< FCH configuration
} AMD_RTB_PARAMS;

AGESA_STATUS
AmdInitRtb (
  IN OUT   AMD_RTB_PARAMS    *AmdInitRtbParams
  );


/**********************************************************************
 * Interface call:  AmdLateRunApTask
 **********************************************************************/
/**
 * Entry point for AP tasking.
 */
AGESA_STATUS
AmdLateRunApTask (
  IN       AP_EXE_PARAMS  *AmdApExeParams
);

//
// General Services API
//

/**********************************************************************
 * Interface service call:  AmdGetApicId
 **********************************************************************/
/// Request the APIC ID of a particular core.

typedef struct {
  IN       AMD_CONFIG_PARAMS StdHeader;        ///< Standard configuration header
  IN       UINT8             Socket;           ///< The Core's Socket.
  IN       UINT8             Core;             ///< The Core id.
     OUT   BOOLEAN           IsPresent;        ///< The Core is present, and  ApicAddress is valid.
     OUT   UINT8             ApicAddress;      ///< The Core's APIC ID.
} AMD_APIC_PARAMS;

/**
 * Get a specified Core's APIC ID.
 */
AGESA_STATUS
AmdGetApicId (
  IN OUT AMD_APIC_PARAMS *AmdParamApic
);

/**********************************************************************
 * Interface service call:  AmdGetPciAddress
 **********************************************************************/
/// Request the PCI Address of a Processor Module (that is, its Northbridge)

typedef struct {
  IN       AMD_CONFIG_PARAMS StdHeader;        ///< Standard configuration header
  IN       UINT8             Socket;           ///< The Processor's socket
  IN       UINT8             Module;           ///< The Module in that Processor
     OUT   BOOLEAN           IsPresent;        ///< The Core is present, and  PciAddress is valid.
     OUT   PCI_ADDR          PciAddress;       ///< The Processor's PCI Config Space address (Function 0, Register 0)
} AMD_GET_PCI_PARAMS;

/**
 * Get Processor Module's PCI Config Space address.
 */
AGESA_STATUS
AmdGetPciAddress (
  IN OUT   AMD_GET_PCI_PARAMS *AmdParamGetPci
);

/**********************************************************************
 * Interface service call:  AmdIdentifyCore
 **********************************************************************/
/// Request the identity (Socket, Module, Core) of the current Processor Core

typedef struct {
  IN       AMD_CONFIG_PARAMS StdHeader;         ///< Standard configuration header
     OUT   UINT8             Socket;            ///< The current Core's Socket
     OUT   UINT8             Module;            ///< The current Core's Processor Module
     OUT   UINT8             Core;              ///< The current Core's core id.
} AMD_IDENTIFY_PARAMS;

/**
 * "Who am I" for the current running core.
 */
AGESA_STATUS
AmdIdentifyCore (
  IN OUT  AMD_IDENTIFY_PARAMS *AmdParamIdentify
);

/**********************************************************************
 * Interface service call:  AmdReadEventLog
 **********************************************************************/
/// An Event Log Entry.
typedef struct {
  IN       AMD_CONFIG_PARAMS StdHeader;         ///< Standard configuration header
     OUT   UINT32            EventClass;        ///< The severity of this event, matches AGESA_STATUS.
     OUT   UINT32            EventInfo;         ///< The unique event identifier, zero means "no event".
     OUT   UINT32            DataParam1;        ///< Data specific to the Event.
     OUT   UINT32            DataParam2;        ///< Data specific to the Event.
     OUT   UINT32            DataParam3;        ///< Data specific to the Event.
     OUT   UINT32            DataParam4;        ///< Data specific to the Event.
} EVENT_PARAMS;

/**
 * Read an Event from the Event Log.
 */
AGESA_STATUS
AmdReadEventLog (
  IN       EVENT_PARAMS *Event
);

/**********************************************************************
 * Interface service call:  AmdIdentifyDimm
 **********************************************************************/
/// Request the identity of dimm from system address

typedef struct {
  IN OUT   AMD_CONFIG_PARAMS StdHeader;            ///< Standard configuration header
  IN       UINT64            MemoryAddress;        ///< System Address that needs to be translated to dimm identification.
  OUT      UINT8             SocketId;             ///< The socket on which the targeted address locates.
  OUT      UINT8             MemChannelId;         ///< The channel on which the targeted address locates.
  OUT      UINT8             DimmId;               ///< The dimm on which the targeted address locates.
  OUT      UINT8             ChipSelect;           ///< The chip select on which the targeted address locates.
  OUT      UINT8             Bank;                 ///< The Bank for which the error address resides
  OUT      UINT32            Row;                  ///< The Row for which the error address resides
  OUT      UINT16            Column;               ///< The Column for which the error address resides
} AMD_IDENTIFY_DIMM;

/**
 * Get the dimm identification for the address.
 */
AGESA_STATUS
AmdIdentifyDimm (
  IN OUT   AMD_IDENTIFY_DIMM *AmdDimmIdentify
);

/// Request the 2D Data Eye Training Data
#define RD_DATAEYE  0
#define WR_DATAEYE  1

/**********************************************************************
 * Interface service call:  AmdGet2DDataEye
 **********************************************************************/
/// Request the training data eye on Socket, Channel, Dimm.

typedef struct _AMD_GET_DATAEYE {
  IN OUT AMD_CONFIG_PARAMS StdHeader;            ///< Standard configuration header
  IN OUT AMD_POST_PARAMS *PostParamsPtr;         ///< Pointer to AMD_POST_PARAMS
  IN  UINT8 SocketId;                            ///< The socket number to get the 2D data eye
  IN  UINT8 MemChannelId;                        ///< The channel number to get the 2D data eye
  IN  UINT8 DimmId;                              ///< The dimm number to get the 2D data eye
  IN  UINT8 DataEyeType;                         ///< Get the 2D data eye on read or write training
  OUT UINT8* DataEyeBuffer;                      ///< The buffer to stores the 2D data eye
} AMD_GET_DATAEYE;

AGESA_STATUS
AmdGet2DDataEye (
  IN OUT   AMD_GET_DATAEYE *AmdGetDataEye
  );

#define AGESA_IDS_DFT_VAL   0xFFFF                  ///<  Default value of every uninitlized NV item, the action for it will be ignored
#define AGESA_IDS_NV_END    0xFFFF                  ///< Flag specify end of option structure
/// WARNING: Don't change the comment below, it used as signature for script
/// AGESA IDS NV ID Definitions
typedef enum {
  AGESA_IDS_EXT_ID_START                   = 0x0000,///< 0x0000                                                             specify the start of external NV id

  AGESA_IDS_NV_UCODE,                               ///< 0x0001                                                            Enable or disable microcode patching

  AGESA_IDS_NV_TARGET_PSTATE,                       ///< 0x0002                                                        Set the P-state required to be activated
  AGESA_IDS_NV_POSTPSTATE,                          ///< 0x0003                                           Set the P-state required to be activated through POST

  AGESA_IDS_NV_BANK_INTERLEAVE,                     ///< 0x0004                                                               Enable or disable Bank Interleave
  AGESA_IDS_NV_CHANNEL_INTERLEAVE,                  ///< 0x0005                                                            Enable or disable Channel Interleave
  AGESA_IDS_NV_NODE_INTERLEAVE,                     ///< 0x0006                                                               Enable or disable Node Interleave
  AGESA_IDS_NV_MEMHOLE,                             ///< 0x0007                                                                  Enables or disable memory hole

  AGESA_IDS_NV_SCRUB_REDIRECTION,                   ///< 0x0008                                           Enable or disable a write to dram with corrected data
  AGESA_IDS_NV_DRAM_SCRUB,                          ///< 0x0009                                                   Set the rate of background scrubbing for DRAM
  AGESA_IDS_NV_DCACHE_SCRUB,                        ///< 0x000A                                            Set the rate of background scrubbing for the DCache.
  AGESA_IDS_NV_L2_SCRUB,                            ///< 0x000B                                           Set the rate of background scrubbing for the L2 cache
  AGESA_IDS_NV_L3_SCRUB,                            ///< 0x000C                                           Set the rate of background scrubbing for the L3 cache
  AGESA_IDS_NV_ICACHE_SCRUB,                        ///< 0x000D                                             Set the rate of background scrubbing for the Icache
  AGESA_IDS_NV_SYNC_ON_ECC_ERROR,                   ///< 0x000E                                    Enable or disable the sync flood on un-correctable ECC error
  AGESA_IDS_NV_ECC_SYMBOL_SIZE,                     ///< 0x000F                                                                             Set ECC symbol size

  AGESA_IDS_NV_ALL_MEMCLKS,                         ///< 0x0010                                                      Enable or disable all memory clocks enable
  AGESA_IDS_NV_DCT_GANGING_MODE,                    ///< 0x0011                                                                             Set the Ganged mode
  AGESA_IDS_NV_DRAM_BURST_LENGTH32,                 ///< 0x0012                                                                    Set the DRAM Burst Length 32
  AGESA_IDS_NV_MEMORY_POWER_DOWN,                   ///< 0x0013                                                        Enable or disable Memory power down mode
  AGESA_IDS_NV_MEMORY_POWER_DOWN_MODE,              ///< 0x0014                                                                  Set the Memory power down mode
  AGESA_IDS_NV_DLL_SHUT_DOWN,                       ///< 0x0015                                                                   Enable or disable DLLShutdown
  AGESA_IDS_NV_ONLINE_SPARE,                        ///< 0x0016      Enable or disable the Dram controller to designate a DIMM bank as a spare for logical swap

  AGESA_IDS_NV_HDTOUT,                              ///< 0x0017                                                                Enable or disable HDTOUT feature

  AGESA_IDS_NV_GNBHDAUDIOEN,                        ///< 0x0018                                                                  Enable or disable GNB HD Audio

  AGESA_IDS_NV_CPB_EN,                              ///< 0x0019                                                                          Core Performance Boost

  AGESA_IDS_NV_HTC_EN,                              ///< 0x001A                                                                                      HTC Enable
  AGESA_IDS_NV_HTC_OVERRIDE,                        ///< 0x001B                                                                                    HTC Override
  AGESA_IDS_NV_HTC_PSTATE_LIMIT,                    ///< 0x001C                                                                        HTC P-state limit select
  AGESA_IDS_NV_HTC_TEMP_HYS,                        ///< 0x001D                                                                      HTC Temperature Hysteresis
  AGESA_IDS_NV_HTC_ACT_TEMP,                        ///< 0x001E                                                                             HTC Activation Temp

  AGESA_IDS_NV_DRAMCON,                             ///< 0x001F                                                  Specify the mode for controller initialization
  AGESA_IDS_NV_ALL_CKE,                             ///< 0x0020                                                                       Enable or disable all CKE
  AGESA_IDS_NV_ALL_CS,                              ///< 0x0021                                                                        Enable or disable all CS
  AGESA_IDS_NV_MSR_DE_CFG_BIT16,                    ///< 0x0022                                               Workaround GCC/C000005 issue for XV Core on CZ A0
  AGESA_IDS_EXT_ID_END,                             ///< 0x0023                                                               specify the end of external NV ID
} IDS_EX_NV_ID;


#define IDS_NUM_EXT_NV_ITEM (AGESA_IDS_EXT_ID_END - AGESA_IDS_EXT_ID_START + 1)

#endif // _AGESA_H_

