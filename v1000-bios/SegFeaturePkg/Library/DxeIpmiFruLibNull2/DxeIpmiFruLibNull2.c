/** @file
 H2O IPMI FRU Null library implement code.

 This c file contains FRU library instance in DXE phase.

;******************************************************************************
;* Copyright (c) 2017 - 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

//[-start-170524-IB09330350-modify]//
/**
 Read specified string/data based on field from FRU.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         Field               Field id want to get.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI
                                        FALSE: This command will send to BMC
 @param[in, out]    StringBufferSize    When in, the size of string buffer to store the result;
                                        When out, the real size of string buffer.
 @param[out]        StringBuffer        Buffer to store string/data.
 @param[out]        CompletionCode      Completion code that BMC/IE returned.

 @retval EFI_INVALID_PARAMETER          StinrBufferSize or StringBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than return data size.
*/
EFI_STATUS
IpmiLibFruReadFieldData2 (
  IN     UINT8                          FruId,
  IN     UINT16                         Field,
  IN     BOOLEAN                        IsHeci,
  IN OUT UINT8                          *StringBufferSize,
  OUT    UINT8                          *StringBuffer,
  OUT    UINT8                          *CompletionCode
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Write specified string/data based on field to FRU.

 @param[in]         FruId               FRU ID that want to write to.
 @param[in]         Field               Field id want to write.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI
                                        FALSE: This command will send to BMC
 @param[in, out]    StringBufferSize    When in, the size of string buffer to store the result;
                                        When out, the real size of write string buffer.
 @param[in]         StringBuffer        Buffer to store string/data.
 @param[out]        CompletionCode      Completion code that BMC/IE returned.

 @retval EFI_INVALID_PARAMETER          StinrBufferSize or StringBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot write data.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than write data size.
*/
EFI_STATUS
IpmiLibFruWriteFieldData2 (
  IN     UINT8                          FruId,
  IN     UINT16                         Field,
  IN     BOOLEAN                        IsHeci,
  IN OUT UINT8                          *StringBufferSize,
  IN     UINT8                          *StringBuffer,
  OUT    UINT8                          *CompletionCode
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Get Chassis Type value/string field in Chassis info area from FRU.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI
                                        FALSE: This command will send to BMC
 @param[in,out]     ChassisTypeStrSize  When in, the size of chassis type string in bytes to store the result;
                                        When out, the real size of string buffer in bytes.
 @param[out]        ChassisType         Enumeration value for chassis type field.
 @param[out]        ChassisTypeString   Chassis type string.
 @param[out]        CompletionCode      Completion code that BMC/IE returned.

 @retval EFI_UNSUPPORTED                Common header didn't exist.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than return data size.
*/
EFI_STATUS
IpmiLibFruGetChassisType2 (
  IN     UINT8                          FruId,
  IN     BOOLEAN                        IsHeci,
  IN OUT UINT8                          *ChassisTypeStrSize,
  OUT    UINT8                          *ChassisType,
  OUT    UINT8                          *ChassisTypeString,
  OUT    UINT8                          *CompletionCode
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Get EFI_TIME convert from MFG Date / Time field in Board area from FRU.

 @param[in]         FruId               FRU ID that want to read from.
 @param[out]        MfgDateTime         The EFI_TIME struct convert from MFG Date / Time field in Board Info Area.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI
                                        FALSE: This command will send to BMC.
 @param[out]        CompletionCode      Completion code that BMC/IE returned.

 @retval EFI_SUCCESS                    Get MFG date and time successfully.
 @retval EFI_UNSUPPORTED                Get MFG date and time fail.
*/
EFI_STATUS
IpmiLibFruGetMfgDateTime2 (
  IN  UINT8                             FruId,
  OUT EFI_TIME                          *MfgDateTime,
  IN  BOOLEAN                           IsHeci,
  OUT    UINT8                          *CompletionCode
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Read data from specified record.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         RecordType          Specified record type.
 @param[in]         RecordIndex         Index of specified record, if multiple instances, start from 0.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI
                                        FALSE: This command will send to BMC
 @param[in, out]    DataBufferSize      IN:  Data buffer size that can store read data.
                                        OUT: Real read data.
 @param[out]        DataBuffer          Buffer to store read data.
 @param[out]        DataHeader          Record header of specified multi record.
 @param[out]        CompletionCode      Completion code that BMC/IE returned.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_BUFFER_TOO_SMALL           Buffer is smaller than return data size.
 @retval EFI_SUCCESS                    Get success.
*/
EFI_STATUS
IpmiLibFruReadMultiRecord2 (
  IN     UINT8                          FruId,
  IN     UINT8                          RecordType,
  IN     UINTN                          RecordIndex,
  IN     BOOLEAN                        IsHeci,
  IN OUT UINT8                          *DataBufferSize,
  OUT    VOID                           *DataBuffer,
  OUT    VOID                           *DataHeader OPTIONAL,
  OUT    UINT8                          *CompletionCode
  )
{
  return EFI_UNSUPPORTED;

}


/**
 Write data to specified record.

 @param[in]         FruId               FRU ID that want to write to.
 @param[in]         RecordType          Specified record type.
 @param[in]         RecordIndex         Index of specified record, if multiple instances, start from 0.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI
                                        FALSE: This command will send to BMC
 @param[in, out]    DataBufferSize      IN:  Data buffer size that want to write;
                                        OUT: Data size that can be written.
 @param[out]        DataBuffer          Buffer to store write data.
 @param[out]        CompletionCode      Completion code that BMC/IE returned.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot write data.
 @retval EFI_BUFFER_TOO_SMALL           Record buffer is smaller than write data.
 @retval EFI_SUCCESS                    Get success.
*/
EFI_STATUS
IpmiLibFruWriteMultiRecord2 (
  IN     UINT8                          FruId,
  IN     UINT8                          RecordType,
  IN     UINTN                          RecordIndex,
  IN     BOOLEAN                        IsHeci,
  IN OUT UINT8                          *DataBufferSize,
  OUT    VOID                           *DataBuffer,
  OUT    UINT8                          *CompletionCode
  )
{
  return EFI_UNSUPPORTED;

}


/**
 Read specified size of raw data from specified offset.

 @param[in]         FruId               FRU ID that want to read from.
 @param[in]         Offset              Offset of FRU that user wants to read from.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI
                                        FALSE: This command will send to BMC
 @param[in, out]    DataBufferSize      IN:  Data buffer size. It also means data size that want to read.
                                        OUT: FRU data size that can be read from specfied offset.
 @param[out]        DataBuffer          Buffer to store read data.
 @param[out]        CompletionCode      Completion code that BMC/IE returned.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_OUT_OF_RESOURCES           If Offset is larger then total FRU size.
 @retval EFI_BUFFER_TOO_SMALL           If Offset + DataBufferSize is larger then total FRU size.
 @retval EFI_SUCCESS                    Get success.
*/
EFI_STATUS
IpmiLibFruReadRawData2 (
  IN     UINT8                          FruId,
  IN     UINT16                         Offset,
  IN     BOOLEAN                        IsHeci,
  IN OUT UINT16                         *DataBufferSize,
  OUT    VOID                           *DataBuffer,
  OUT    UINT8                          *CompletionCode
  )
{
  return EFI_UNSUPPORTED;

}


/**
 Write specified size of raw data to specified offset.

 @param[in]         FruId               FRU ID that want to write to.
 @param[in]         Offset              Offset of FRU that user wants to write to.
 @param[in]         IsHeci              TRUE : This command will send to IE by HECI
                                        FALSE: This command will send to BMC
 @param[in, out]    DataBufferSize      IN:  Data buffer size that want to write.
                                        OUT: FRU data size that can be written from specfied offset.
 @param[out]        DataBuffer          Buffer to store read data.
 @param[out]        CompletionCode      Completion code that BMC/IE returned.

 @retval EFI_INVALID_PARAMETER          DataBufferSize or DataBuffer is NULL.
 @retval EFI_UNSUPPORTED                Cannnot get data.
 @retval EFI_OUT_OF_RESOURCES           If Offset is larger then total FRU size.
 @retval EFI_BUFFER_TOO_SMALL           If Offset + DataBufferSize is larger then total FRU size.
 @retval EFI_SUCCESS                    Get success.
*/
EFI_STATUS
IpmiLibFruWriteRawData2 (
  IN     UINT8                          FruId,
  IN     UINT16                         Offset,
  IN     BOOLEAN                        IsHeci,
  IN OUT UINT16                         *DataBufferSize,
  IN     VOID                           *DataBuffer,
  OUT    UINT8                          *CompletionCode
  )
{
  return EFI_UNSUPPORTED;

}
//[-end-170524-IB09330350-modify]//

