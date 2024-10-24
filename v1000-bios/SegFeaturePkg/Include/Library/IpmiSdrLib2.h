/** @file
 H2O IPMI SDR library 2 header file.

 This file contains functions prototype that can easily retrieve SDR data via
 using H2O IPMI SDR protocol.

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _IPMI_SDR_LIB2_H_
#define _IPMI_SDR_LIB2_H_

#include <Library/IpmiLibDefine.h>

/**
 Get sensor data from "SDR Repository Device" or "Device SDR".

 @param[in]         RecordId            Speficied ID of SDR that want to get.
                                        0xFFFF means getting all recoreds.
 @param[in]         Flag                Low byte: if low byte set to 0x01(DISPLAY_LONG_UNIT), it will
                                        return full unit string in IPMI_SDR_DATA_STURCT. Otherwise
                                        it will return short unit string by default.
                                        High byte: decimal number in all FLOAT string. Maximum is 10 digital.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI.
                                        FALSE: This command will send to BMC.
 @param[out]        Count               Number of records if user passes 0xFFFF in RecordId.
 @param[out]        Record              A buffer to IPMI_SDR_DATA_STURCT. It is caller's responsibility
                                        to free allocated memory.

 @retval EFI_SUCCESS                    Get SDR data success.
 @retval EFI_UNSUPPORTED                Cannot get HOB.
 @retval EFI_INVALID_PARAMETER          Count is NULL or Record is NULL.
*/
EFI_STATUS
IpmiLibGetSdrData2 (
  IN  UINT16                            RecordId,
  IN  UINT16                            Flag,
  IN  BOOLEAN                           IsHeci,
  OUT UINT16                            *Count,
  OUT IPMI_SDR_DATA_STURCT              **Record
  );


/**
 Get sensor raw data from "SDR Repository Device" or "Device SDR".

 @param[in]         RecordId            Speficied ID of SDR that want to get.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI
                                        FALSE: This command will send to BMC
 @param[out]        Record              A buffer to IPMI_GET_SDR_RESPONSE. It is caller's responsibility
                                        to free allocated memory.

 @retval EFI_SUCCESS                    Get SDR data success.
 @retval EFI_UNSUPPORTED                Speificed Id does not exist or cannot get SDR data.
 @retval EFI_INVALID_PARAMETER          Record is NULL.
*/
EFI_STATUS
IpmiLibGetSdrRaw2 (
  IN  UINT16                            RecordId,
  IN  BOOLEAN                           IsHeci,
  OUT IPMI_GET_SDR_RESPONSE             **Record
  );


/**
 Convert analog sensor reading or threshold event data (trigger reading or trigger threshold) value to string.

 @param[in]         SensorNumber        Unique number identifying the sensor.
 @param[in]         SensorReading       A byte sensor reading or event data (contain trigger reading or
                                        trigger threshold) value need to convert.
 @param[in]         Precision           The precision of value to string.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI
                                        FALSE: This command will send to BMC
 @param[out]        ConvertValueStr     The point of CHAR16 string space to carry converted value string.

 @retval EFI_SUCCESS                    Convert sensor reading value success.
 @retval EFI_NOT_READY                  Get sensor data record fail.
 @retval EFI_UNSUPPORTED                Unique sensor number can't find the corresponding SDR or
                                        the record type of sensor number is not full type.
 @retval EFI_INVALID_PARAMETER          1. This is NULL
                                        2. ConvertValueStr is NULL
*/
EFI_STATUS
IpmiLibSensorReadingToStr2 (
  IN  UINT8                             SensorNumber,
  IN  UINT8                             SensorReading,
  IN  UINT8                             Precision,
  IN  BOOLEAN                           IsHeci,
  OUT CHAR16                            *ConvertValueStr
  );

#endif

