/** @file
 H2O IPMI SDR Null library implement code.

 This c file contains SDR library instance in DXE phase.

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
//[-start-170718-IB09330369-modify]//
#include <Library/IpmiLibDefine.h>

//[-start-170609-IB09330361-modify]//
/**
 Get sensor data from "SDR Repository Device" or "Device SDR".

 @param[in]         RecordId            Speficied ID of SDR that want to get. 0xFFFF means getting all recoreds.
 @param[in]         Flag                Low byte: if low byte set to 0x01(DISPLAY_LONG_UNIT), it will
                                                  return full unit string in IPMI_SDR_DATA_STURCT. Otherwise
                                                  it will return short unit string by default.
                                        High byte: decimal number in all FLOAT string. Maximum is 10 digital.
 @param[out]        Count               Number of records if user passes 0xFFFF in RecordId.
 @param[out]        Record              A buffer to IPMI_SDR_DATA_STURCT. It is caller's responsibility
                                        to free allocated memory.

 @retval EFI_UNSUPPORTED                Function Unsupported.
*/
EFI_STATUS
IpmiLibGetSdrData (
  IN  UINT16                            RecordId,
  IN  UINT16                            Flag,
  OUT UINT16                            *Count,
  OUT IPMI_SDR_DATA_STURCT              **Record
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Get sensor raw data from "SDR Repository Device" or "Device SDR".

 @param[in]         RecordId            Speficied ID of SDR that want to get.
 @param[out]        Record              A buffer to IPMI_GET_SDR_RESPONSE. It is caller's responsibility
                                        to free allocated memory.

 @retval EFI_UNSUPPORTED                Function Unsupported.
*/
EFI_STATUS
IpmiLibGetSdrRaw (
  IN  UINT16                            RecordId,
  OUT IPMI_GET_SDR_RESPONSE             **Record
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Convert analog sensor reading or threshold event data (trigger reading or trigger threshold) value to string.

 @param[in]         SensorNumber        Unique number identifying the sensor.
 @param[in]         SensorReading       A byte sensor reading or event data (contain trigger reading or
                                        trigger threshold) value need to convert.
 @param[in]         Precision           The precision of value to string.
 @param[out]        ConvertValueStr     The point of CHAR16 string space to carry converted value string.

 
 @retval EFI_UNSUPPORTED                Function Unsupported.
*/
//[-end-170718-IB09330369-modify]//
EFI_STATUS
IpmiLibSensorReadingToStr (
  IN  UINT8                             SensorNumber,
  IN  UINT8                             SensorReading,
  IN  UINT8                             Precision,
  OUT CHAR16                            *ConvertValueStr
  )
{
  return EFI_UNSUPPORTED;
}
//[-end-170609-IB09330361-modify]//

