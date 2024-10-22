/** @file
 This file contains information about SIO itself.

;******************************************************************************
;* Copyright 2017 - 2019 Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#ifndef _SIO_FEATURE_POLICY_H_
#define _SIO_FEATURE_POLICY_H_

typedef enum _SIO_CHIP_INSTANCE_NUMBER {
  SioChip1 = 1,
  SioChip2,
  SioInstanceMaximum
} SIO_CHIP_INSTANCE_NUMBER;

typedef enum _SIO_CONFIG_PORT_ADDRESS {
  SioAddress2E      = 0x2E,
  SioAddress4E      = 0x4E,
  SioAddress162E    = 0x162E,
  SioAddress164E    = 0x164E,
  SioAddressMaximum = 0xFFFF
} SIO_CONFIG_PORT_ADDRESS;

typedef enum _SIO_CHIP_VENDOR_TYPE {
  SioE01ChipVendor = 0x01,
  SioF01ChipVendor = 0x02,
  SioI01ChipVendor = 0x03,
  SioM01ChipVendor = 0x04,
  SioN01ChipVendor = 0x05,
  SioA01ChipVendor = 0x06,
  SioM02ChipVendor = 0x07,
  SioF02ChipVendor = 0x08,
  SioN02ChipVendor = 0x09,
  SioN03ChipVendor = 0x0A,
  SioF03ChipVendor = 0x0B,
  SioA02ChipVendor = 0x0C,
  SioI02ChipVendor = 0x0D,
  SioInsydeDummy   = 0x10,
  SioChipVendorMaximum
} SIO_CHIP_VENDOR_TYPE;

typedef enum _SIO_LOGICAL_DEVICE_TYPE {
  SioDeviceStart       = 0x00,
  SioFloppyDev         = 0x01,
  SioParallelDev       = 0x02,
  SioUartDev           = 0x03,
  SioInfraredDev       = 0x04,
  SioPs2KeyboardDev    = 0x05,
  SioPs2MouseDev       = 0x06,
  SioKbcDev            = 0x07,
  SioIsaDeviceTypeEnd  = 0x10,
  SioWdtFn             = 0x11,
  SioHwmFn             = 0x12,
  SioFanCtrlFn         = 0x13,
  SioCaseOpenFn        = 0x14,
  SioGpioFn            = 0x15,
  SioLedFn             = 0x16,
  SioAcpiFn            = 0x17,
  SioPmeFn             = 0x18,
  SioPowerLossFn       = 0x19,
  SioDeepSleepFn       = 0x1A,
  SioRuntimeFn         = 0x1B,
  SioEmiFn             = 0x1C,
  SioSmiFn             = 0x1D,
  SioEcFn              = 0x1E,
  SioTriggerPinFn      = 0x1F,
  SioKcsFn             = 0x20,
  SioiLpc2AhbFn        = 0x21,
  SioDeviceTypeMaximum = 0xFF
} SIO_LOGICAL_DEVICE_TYPE;

typedef enum _SIO_DEVICE_INSTANCE_NUMBER {
  SioDevInstance1 = 1,
  SioDevInstance2,
  SioDevInstance3,
  SioDevInstance4,
  SioDevInstance5,
  SioDevInstance6,
  SioDevInstance7,
  SioDevInstance8,
  SioDevInstance9,
  SioDevInstance10,
  SioDevInstance11,
  SioDevInstance12,
  SioDevInstance13,
  SioDevInstance14,
  SioDevInstance15,
  SioDevInstance16,
  SioDevInstance17,
  SioDevInstance18,
  SioDevInstance19,
  SioDevInstance20,
  SioDevInstance21,
  SioDevInstance22,
  SioDevInstance23,
  SioDevInstance24,
  SioDevInstance25,
  SioDevInstance26,
  SioDevInstance27,
  SioDevInstance28,
  SioDevInstance29,
  SioDevInstance30,
  SioDevInstance31,
  SioDevInstance32,
  SioDevInstanceMaximum
} SIO_DEVICE_INSTANCE_NUMBER;

//
// Maximum number of UART devices that H2O SIO protocols and infrastructure
// code can support.
// This number may be hard-coded in many places.
//
#define SIO_MAX_UART_DEVICES  6

// +--------------------------------------------------------------------+
// |Form Id Naming Rule                                                 |
// +---------+----------------+--------------------------+--------------+
// | Field3  | Field2         | Field1                   | Field0       |
// | [15:14] | [13:8]         | [7:2]                    | [1:0]        |
// +---------+----------------+--------------------------+--------------+
// |0 (Root) |0  (Root)       |[7:6] SensorType          |0 (VarstoreId)|
// |1 (Chip1)|18 (HWM)        |Hwm                       |1 (FormId)    |
// |2 (Chip2)|19 (Fan Control)|0 Voltage Update Label    |2 (RefreshID) |
// |         |21 (GPIO)       |1 Temperature Update Label|              |
// |         |                |2 Fan In Update Label     |              |
// |         |                |3 Hwm Configuration       |              |
// |         |                |FanCtrl                   |              |
// |         |                |0 Fan In Update Label     |              |
// |         |                |1 Temperature Update Label|              |
// |         |                |[5:2]                     |              |
// |         |                |0-F (DeviceInstance)      |              |
// |         |                |0-9 (GPIO group)          |              |
// |         |                |                          |              |
// |         |                |                          |              |
// |         |                |                          |              |
// |         |                |                          |              |
// |         |                |                          |              |
// |         |                |                          |              |
// +---------+----------------+--------------------------+--------------+

//
// Policy Define
//
#define SIO_CHIP_FAN_CTRL_UPDATE_STEP                0x04
#define SIO_CHIP_FAN_CTRL_MAXIMUM_SUPPORTED          16
#define SIO_CHIP_HWM_UPDATE_STEP                     0x40
#define SIO_CHIP_HWM_MAXIMUM_SUPPORTED               16
//
// Field0
//
#define LABEL_TYPE(LabelType)                        (LabelType)
#define SIO_CHIP_VARSTORE_ID                         0x00
#define SIO_CHIP_FORM_ID                             0x01
#define SIO_CHIP_HWM_REFRESH_ID                      0x02
#define SIO_CHIP_ROOT                                0x000

//
// Field1
//
#define DEVICE_INSTANCE(DevInst)                     (DevInst << 2)

//
// Field2
//
#define LOGICAL_DEVICE(DevType)                      (DevType << 8)

//
// Field3
//
#define CHIP_INSTANCE(ChipInst)                      (ChipInst << 14)

//
// Create Form Id
//
//	#define CREATE_FORM_ID(ChipInst, DevType, DevInst, LabelType)     (NIBBLE3 (ChipInst) | NIBBLE2 (DevType) | NIBBLE1 (DevInst) | NIBBLE0 (LabelType))

//
// Analysis Form Id
//
typedef union {
  UINT16 SioFormID;
  struct {
    UINT8 LabelType      :2;
    UINT8 DeviceInstance :6;
    UINT8 LogicalDevice  :6;
    UINT8 ChipInstance   :2;
  } cell;
} SIO_FORM_POLICY;

#define FORM_ID_LABEL_TYPE                           (BIT1  | BIT0)
#define FORM_ID_DEVICE_INSTANCE                      (BIT7  | BIT6  | BIT5  | BIT4  | BIT3 | BIT2)
#define FORM_ID_LOGICAL_DEVICE                       (BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define FORM_ID_CHIP_INSTANCE                        (BIT15 | BIT14)
#define FORM_ID_DEVICE_INDEX                         (BIT5  | BIT4  | BIT3 | BIT2)
#define FORM_ID_SENSOR_TYPE                          (BIT7  | BIT6)

#define SIO_CHIP_HWM                                 LOGICAL_DEVICE (SioHwmFn)
#define SIO_CHIP_HWM_UPDATE_TYPE                     SIO_CHIP_HWM

#define SIO_CHIP_FAN_CTRL                            LOGICAL_DEVICE (SioFanCtrlFn)
#define SIO_CHIP_FAN_CTRL_FAN_UPDATE_TYPE            SIO_CHIP_FAN_CTRL
#define SIO_CHIP_FAN_CTRL_TEMPERATURE_UPDATE_TYPE    (SIO_CHIP_FAN_CTRL | SIO_CHIP_HWM_UPDATE_STEP)

#define SIO_ISA_DEVICE_DEFAULT_UID                   0xFFFF

#pragma pack(1)
typedef struct _SIO_AND_THEN_OR_SETTING {
  BOOLEAN                      AccessIoSpace;
  UINT16                       Offset;
  UINT8                        AndData8;
  UINT8                        OrData8;
} SIO_AND_THEN_OR_SETTING;

typedef struct _SIO_INSTANCE_DATA {
  UINT8                        ChipInstance;
  UINT16                       ConfigPort;
  BOOLEAN                      ChipSupported;
  SIO_AND_THEN_OR_SETTING      *PeiCustomizedTable;
  UINTN                        PeiCustomizedTableSize;
  SIO_AND_THEN_OR_SETTING      *DxeCustomizedTable;
  UINTN                        DxeCustomizedTableSize;
} SIO_INSTANCE_DATA;

typedef struct _SIO_CHIP_INFORMATION {
  SIO_CHIP_VENDOR_TYPE          ChipVendor;
  UINT8                         ChipIdMSB;
  UINT8                         ChipIdLSB;
  SIO_INSTANCE_DATA             *SioInstanceList;
} SIO_CHIP_INFORMATION;

typedef struct _SIO_PCD_ISA_DEVICE_UID {
  UINT8   LogicalDeviceType;
  UINT8   DeviceInstance;
  UINT16  DeviceUid;
} SIO_PCD_ISA_DEVICE_UID;
#pragma pack()

#endif
