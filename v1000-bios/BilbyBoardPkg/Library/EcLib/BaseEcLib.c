/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SharedEcLib.h"

STATIC ENABLE_EC_TABLE mEnableEcTable[] = {
  {LPCPnPChan,        TRUE},
  {LPCCOM0Chan,       TRUE},
  {LPCACPIChan,       TRUE},
  {LPCVI2CChan,       TRUE},
  {LPCPort80Chan,    FALSE},
  {LPCCOM1Chan,       TRUE},
  {LPC8042Chan,       TRUE},
  {LPCNOTHINGChan,   FALSE},
};

STATIC
BOOLEAN
IsTiEc (
  VOID
  )
{
  UINT8 Value8;
  UINT16	temp;

  //
  // Check vendor ID.
  //
  IoWrite8 (EC_INDEX_PORT, TIEC_DEVID0);
      temp = IoRead16(0x80);
      temp = IoRead16(0x80);
      temp = IoRead16(0x80);
      temp = IoRead16(0x80);
      temp = IoRead16(0x80);
  Value8 = IoRead8 (EC_DATA_PORT);
  if (Value8 != 0x10) {
    return FALSE;
  }
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  IoWrite8 (EC_INDEX_PORT, TIEC_DEVID1);
      temp = IoRead16(0x80);
      temp = IoRead16(0x80);
      temp = IoRead16(0x80);
      temp = IoRead16(0x80);
	  temp = IoRead16(0x80);

  Value8 = IoRead8 (EC_DATA_PORT);
  if (Value8 != 0x4c) {
    return FALSE;
  }

  return TRUE;
}


STATIC
BOOLEAN
EcSetChannel (
  IN ENABLE_EC_TABLE              *EcTable
  )
{
  UINT8 Value8L, Value8H;
  UINT16	temp;

  if (EcTable->Value) {
    IoWrite8 (EC_INDEX_PORT, TIEC_LDN_SELECT);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	  temp = IoRead16(0x80);
    IoWrite8 (EC_DATA_PORT, EcTable->Register);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	  temp = IoRead16(0x80);

    //
    // Check the IO base address of this LD
    //
    IoWrite8 (EC_INDEX_PORT, TIEC_IOBASE_MSB);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	  temp = IoRead16(0x80);
    Value8H = IoRead8 (EC_DATA_PORT);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	  temp = IoRead16(0x80);
    IoWrite8 (EC_INDEX_PORT, TIEC_IOBASE_LSB);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
    Value8L = IoRead8 (EC_DATA_PORT);

    if (!Value8L && !Value8H) {
      return FALSE;
    }

    //
    // Found a device.  Turn it on.
    //
    IoWrite8 (EC_INDEX_PORT, TIEC_ACTIVATE);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	temp = IoRead16(0x80);
	  temp = IoRead16(0x80);
    IoWrite8 (EC_DATA_PORT, 1);

    return TRUE;
  } else {
    return FALSE;
  }
}


STATIC
VOID
EcEnable (
  )
{
  UINT8                           Index;
  UINT16	temp;

  IoWrite8 (EC_INDEX_PORT, TIEC_CONFIG_ENABLE);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
    temp = IoRead16(0x80);
  IoWrite8 (EC_DATA_PORT,  TIEC_CONFIG_UNLOCK);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  IoWrite8 (EC_DATA_PORT,  TIEC_CONFIG_UNLOCK);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  IoWrite16 (0x80, 0xEC00);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  IoWrite16 (0x80, 0xEC02);

  if (IsTiEc()) {
    IoWrite16 (0x80, 0xEC03);
    for (Index = 0; Index < sizeof (mEnableEcTable) / sizeof (ENABLE_EC_TABLE); ++Index) {
      EcSetChannel (&mEnableEcTable[Index]);
    }
  }

  IoWrite16 (0x80, 0xEC04);
  IoWrite8 (EC_INDEX_PORT, TIEC_CONFIG_ENABLE);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  IoWrite8 (EC_DATA_PORT,  TIEC_CONFIG_LOCK);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  temp = IoRead16(0x80);
  IoWrite8 (EC_DATA_PORT,  TIEC_CONFIG_LOCK);
}

EFI_STATUS
EcAcDcSwitch (
  IN UINT8                        EnableAcDcSwitch,
  IN UINT8                        DcTimer,
  IN UINT8                        AcTimer
  )
{
  UINT8                           Value8;

  Value8 = EcRead (EC_STATUS_INDEX);
  if (Value8 == 0xFF) {
    return EFI_NOT_FOUND;
  }

  Value8&=~(1<<1);           //Set BIT1 to 0
  Value8 |= (EnableAcDcSwitch << 1);
  EcWrite (EC_STATUS_INDEX, Value8);

  if (EnableAcDcSwitch) {
    EcWrite (TIEC_DC_TIMER, DcTimer);
    EcWrite (TIEC_AC_TIMER, AcTimer);
  }

  return EFI_SUCCESS;
}


/**
  EC initialize.

  @retval     EFI_UNSUPPORTED     Returns unsupported by default.
  @retval     EFI_SUCCESS         The service is customized in the project.
  @retval     EFI_MEDIA_CHANGED   The value of IN OUT parameter is changed.
**/
EFI_STATUS
EcInit (
  )
{
//  EcEnable ();
//  EcAcDcSwitch (); // We don't know what is it, so we might add later.

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
BaseEcLibConstructor (
  )
{
  return EFI_SUCCESS;
}

