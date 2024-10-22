/** @file
 H2O IPMI SEL Info library header file.

 This file contains functions prototype that can easily retrieve SEL Info via
 using H2O IPMI SEL Data protocol.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _IPMI_LIB_DEFINE_H_
#define _IPMI_LIB_DEFINE_H_

#include <Library/H2OIpmiSdr.h>

#define SEL_INFO_STRING_LEN               64
#define SEL_INFO_DESC_STRING_LEN          256
#define SDR_STRING_LENGTH                 0x20


#pragma pack(1)

typedef struct {
  CHAR16    SensorName[SEL_INFO_STRING_LEN];
  CHAR16    Type[SEL_INFO_STRING_LEN];
  CHAR16    Desc[SEL_INFO_DESC_STRING_LEN];
  CHAR16    Generator[SEL_INFO_STRING_LEN];
  CHAR16    IdType[SEL_INFO_STRING_LEN];
  CHAR16    Lun[SEL_INFO_STRING_LEN];
} SEL_INFO_STRUCT;

typedef struct {
  UINT8   CompletionCode;
  UINT8   Reserved;
  UINT16  RecordId;
  CHAR16  Name[SDR_STRING_LENGTH];
  UINT16  DataStatus;
  UINT64  Value;
  CHAR16  ValueStr[SDR_STRING_LENGTH];
  CHAR16  Unit[SDR_STRING_LENGTH * 2];
  UINT64  LowNonCriticalThres;
  CHAR16  LowNonCriticalThresStr[SDR_STRING_LENGTH];
  UINT64  LowCriticalThres;
  CHAR16  LowCriticalThresStr[SDR_STRING_LENGTH];
  UINT64  LowNonRecoverThres;
  CHAR16  LowNonRecoverThresStr[SDR_STRING_LENGTH];
  UINT64  UpNonCriticalThres;
  CHAR16  UpNonCriticalThresStr[SDR_STRING_LENGTH];
  UINT64  UpCriticalThres;
  CHAR16  UpCriticalThresStr[SDR_STRING_LENGTH];
  UINT64  UpNonRecoverThres;
  CHAR16  UpNonRecoverThresStr[SDR_STRING_LENGTH];
  UINT8   SensorNumber;
  UINT8   SensorOwnerId;
} IPMI_SDR_DATA_STURCT;

#pragma pack()

#endif

