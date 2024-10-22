/** @file
 SMM Chipset Services Library.

 This file contains only one function that is SmmCsSvcSataComReset().
 The function SmmCsSvcSataComReset() use chipset services to reset specified SATA port.

***************************************************************************
* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Uefi.h>
#include <H2OIhisi.h>
#include <Library/DebugLib.h>
#include <Protocol/H2OSmmChipsetServices.h>

extern H2O_SMM_CHIPSET_SERVICES_PROTOCOL *mSmmChipsetSvc;

/**
  AH=41h, OEM Extra Data Communication type 53h for VBIOS relative function.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
VbiosFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  AH=41h, OEM Extra Data Communication type 54h to update logo.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   Update logo successful.
**/
EFI_STATUS
LogoUpdateFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{

  return EFI_SUCCESS;
}

/**
  AH=41h, OEM Extra Data Communication type 55h to Check BIOS sign by System BIOS.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
CheckBiosSignBySystemBiosFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  AH=41h, OEM Extra Data Communication type 02h~03h are reserved functions.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_UNSUPPORTED   Return unsupported to indicate this is reserved function.
**/
EFI_STATUS
ReservedFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{

  return EFI_UNSUPPORTED;
}

/**
  AH=41h(OemExtraDataCommunication),
  This function offers an interface to do IHISI Sub function AH=41h.

 @param[in]         ApCommDataBuffer    Pointer to AP communication data buffer.
 @param[in, out]    BiosCommDataBuffer  On entry, pointer to BIOS communication data buffer.
                                        On exit, points to updated BIOS communication data buffer.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
EFIAPI
SmmCsSvcIhisiOemExtCommunication (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  IN OUT BIOS_COMMUNICATION_DATA_TABLE     *BiosCommDataBuffer
)
{
  EFI_STATUS        Status;

  switch (ApCommDataBuffer->DataType) {
    case Vbios:
      Status = VbiosFunction (ApCommDataBuffer, BiosCommDataBuffer);
      break;

    case LogoUpdate:
      Status = LogoUpdateFunction (ApCommDataBuffer, BiosCommDataBuffer);
      break;

    case CheckBiosSignBySystemBios:
      Status = CheckBiosSignBySystemBiosFunction (ApCommDataBuffer, BiosCommDataBuffer);
      break;

    default:
      Status = ReservedFunction (ApCommDataBuffer, BiosCommDataBuffer);
      break;
  }
  return Status;
}

/**
  AH=42h(OemExtraDataWrite)
  This function offers an interface to do IHISI Sub function AH=42h,

 @param[in]         FunctionType      Function type.
 @param[in, out]    WriteDataBuffer   Pointer to input file data buffer.
 @param[in, out]    WriteSize         Write size.
 @param[in, out]    RomBaseAddress    Target offset to write.
 @param[in]         ShutdownMode      Shutdown mode.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
EFIAPI
SmmCsSvcIhisiOemExtDataWrite (
  IN      UINT8                   FunctionType,
  IN OUT  UINT8                  *WriteDataBuffer,
  IN OUT  UINTN                  *WriteSize,
  IN OUT  UINTN                  *RomBaseAddress,
  IN      UINT8                   ShutdownMode
){
  return EFI_UNSUPPORTED;
}

/**
 AH=47h(OemExtraDataRead),
 This function offers an interface to do IHISI Sub function AH=47h,

 @param[in]         FunctionType        Function type
 @param[in, out]    DataBuffer          Pointer to return data buffer.
 @param[in, out]    Size                Read size.
 @param[in, out]    RomBaseAddress      Read address.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
EFIAPI
SmmCsSvcIhisiOemExtDataRead (
  IN UINT8                        FunctionType,
  IN OUT  UINT8                  *DataBuffer,
  IN OUT  UINTN                  *Size,
  IN OUT  UINTN                  *RomBaseAddress
){
  return EFI_UNSUPPORTED;
}

