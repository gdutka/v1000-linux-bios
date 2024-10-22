/** @file
  Header file for .FDM region GUID
;******************************************************************************
;* Copyright (c) 2014 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _FLASH_REGION_LAYOUT_H_
#define _FLASH_REGION_LAYOUT_H_


#define H2O_FLASH_MAP_REGION_AUXILLIARY_FV_GUID \
  { 0x8cc7cc2d, 0xc926, 0x473b, { 0xb9, 0xd7, 0xb2, 0x97, 0xbb, 0x0f, 0xca, 0x5f }}

#define H2O_FLASH_MAP_REGION_BOOT_FV_GUID \
  { 0xe3d76d56, 0x988a, 0x4d6b, { 0x89, 0x13, 0x64, 0xf2, 0xdf, 0x1d, 0xf6, 0xa6 }}

#define H2O_FLASH_MAP_REGION_BVDT_GUID \
  { 0x32415dfc, 0xd106, 0x48c7, { 0x9e, 0xb5, 0x80, 0x6c, 0x11, 0x4d, 0xd1, 0x07 }}


#define H2O_FLASH_MAP_REGION_EC_GUID \
  { 0xa73ef3bf, 0x33cc, 0x43a9, { 0xb3, 0x9c, 0xa9, 0x12, 0xc7, 0x48, 0x9a, 0x57 }}


#define H2O_FLASH_MAP_REGION_FTW_BACKUP_GUID \
  { 0xb78e15d3, 0xf0a5, 0x4248, { 0x8e, 0x2f, 0xd3, 0x15, 0x7a, 0xef, 0x88, 0x36 }}


#define H2O_FLASH_MAP_REGION_FTW_STATE_GUID \
  { 0xc8416e04, 0x9934, 0x4079, { 0xbe, 0x9a, 0x39, 0xf8, 0xd6, 0x02, 0x84, 0x98 }}


#define H2O_FLASH_MAP_REGION_FV_GUID \
  { 0xb5e8e758, 0xa7e6, 0x4c8b, { 0xab, 0x85, 0xff, 0x2a, 0x95, 0x9b, 0x99, 0xba }}

#define H2O_FLASH_MAP_REGION_FV_OTHER_GUID \
  { 0xa36cbfed, 0x0a2f, 0x4a9d, { 0x82, 0x3c, 0x3c, 0x49, 0x8c, 0x06, 0xdd, 0xd1 }}

#define H2O_FLASH_MAP_REGION_FLASH_MAP_GUID \
  { 0xf078c1a0, 0xfc52, 0x4c3f, { 0xbe, 0x1f, 0xd6, 0x88, 0x81, 0x5a, 0x62, 0xc0 }}


#define H2O_FLASH_MAP_REGION_GPNV_GUID \
  { 0x29280631, 0x623b, 0x43e4, { 0xbc, 0xa1, 0x00, 0x52, 0x14, 0xc4, 0x83, 0xa6 }}


#define H2O_FLASH_MAP_REGION_LICENSE_GUID \
  { 0x1ab43faf, 0x4988, 0x47b9, { 0x96, 0x9b, 0x3e, 0x31, 0x58, 0x7a, 0x75, 0xde }}

#define H2O_FLASH_MAP_REGION_LOGO_GUID \
  { 0xdacfab69, 0xf977, 0x4784, { 0x8a, 0xd8, 0x77, 0x24, 0xa6, 0xf4, 0xb4, 0x40 }}


#define H2O_FLASH_MAP_REGION_MICROCODE_GUID \
  { 0xb49866f8, 0x8cd2, 0x49e4, { 0xa1, 0x6d, 0xb6, 0x0f, 0xbe, 0xc3, 0x1c, 0x4b }}

#define H2O_FLASH_MAP_REGION_MSDM_GUID \
  { 0xb344eb1a, 0xf97e, 0x4f14, { 0xa1, 0xe1, 0x7e, 0x63, 0xbc, 0x40, 0xc8, 0xce }}

#define H2O_FLASH_MAP_REGION_MULTI_CONFIG_GUID \
  { 0x5994b592, 0x2f14, 0x48d5, { 0xbb, 0x40, 0xbd, 0x27, 0x96, 0x9c, 0x77, 0x80 }}

#define H2O_FLASH_MAP_REGION_VAR_DEFAULT_GUID \
  { 0xd9ddaca2, 0x0816, 0x48f3, { 0xad, 0xed, 0x6b, 0x71, 0x65, 0x6b, 0x24, 0x8a }}

#define H2O_FLASH_MAP_REGION_ODM_GUID \
  { 0xa42c1051, 0x73b5, 0x41a9, { 0xb6, 0x35, 0x0c, 0xc5, 0x1c, 0x82, 0x72, 0xf8 }}


#define H2O_FLASH_MAP_REGION_OEM_GUID \
  { 0x2fd91ad6, 0xd8e3, 0x4fd6, { 0xb6, 0x79, 0x30, 0x30, 0xe8, 0x6a, 0xe5, 0x7a }}


#define H2O_FLASH_MAP_REGION_PASSWORD_GUID \
  { 0xc0027e32, 0x8ee5, 0x4d17, { 0x9b, 0x28, 0xba, 0x50, 0x16, 0x6c, 0x4c, 0xb4 }}


#define H2O_FLASH_MAP_REGION_SMBIOS_EVENT_LOG_GUID \
  { 0xb95d2198, 0x8e70, 0x4cdc, { 0x93, 0x7d, 0x9a, 0x3f, 0x79, 0x5f, 0x99, 0x05 }}


#define H2O_FLASH_MAP_REGION_SMBIOS_UPDATE_GUID \
  { 0x8964fedc, 0x6fe7, 0x4e1e, { 0xa5, 0x5e, 0xff, 0x82, 0x1d, 0x71, 0xff, 0xcf }}


#define H2O_FLASH_MAP_REGION_VAR_GUID \
  { 0x773c5374, 0x81d1, 0x4d43, { 0xb2, 0x93, 0xf3, 0xd7, 0x4f, 0x18, 0x1d, 0x6b }}


#define H2O_FLASH_MAP_REGION_UNKNOWN_GUID \
  { 0x201d65e5, 0xbe23, 0x4875, { 0x80, 0xf8, 0xb1, 0xd4, 0x79, 0x5e, 0x7e, 0x08 }}


#define H2O_FLASH_MAP_REGION_UNUSED_GUID \
  { 0x13c8b020, 0x4f27, 0x453b, { 0x8f, 0x80, 0x1b, 0xfc, 0xa1, 0x87, 0x38, 0x0f }}

#define H2O_FLASH_MAP_REGION_USB_OPTION_ROM_GUID \
  { 0x607bf30f, 0x5f2b, 0x4da2, { 0xae, 0xed, 0x56, 0xf9, 0xbd, 0xcd, 0x2d, 0x21 }}

#define H2O_FLASH_MAP_DXE_FV_GUID \
  { 0x1fd0bace, 0x6f0a, 0x4085, { 0x90, 0x1e, 0xf6, 0x21, 0x03, 0x85, 0xcb, 0x6f }}

#define H2O_FLASH_MAP_PEI_FV_GUID \
  { 0xcf1406c5, 0x3fec, 0x47eb, { 0xa6, 0xc3, 0xb7, 0x1a, 0x3e, 0xe0, 0x0b, 0x95 }}

#define H2O_FLASH_MAP_UNSIGNED_FV_GUID \
  { 0xf2a016b6, 0xe814, 0x402e, { 0xa3, 0x95, 0x46, 0xd3, 0xcf, 0x75, 0x26, 0x4a }}

#define H2O_FLASH_MAP_REGION_OPTION_ROM_GUID \
  { 0x244A24AF, 0xC124, 0x49A3, { 0xB2, 0x86, 0xAC, 0xE1, 0xAB, 0x31, 0xFD, 0x25 }}

#define H2O_FLASH_MAP_REGION_OPTION_ROM_BDF_GUID \
  { 0x8C493122, 0xCE49, 0x4504, { 0x92, 0x50, 0x1B, 0x29, 0x6C, 0x49, 0xA5, 0xC3 }}

#define H2O_FLASH_MAP_REGION_VERB_TABLE_GUID \
  { 0x1a6047f6, 0x7b12, 0x45e1, { 0xa2, 0x6f, 0xe8, 0xdd, 0xa5, 0x5d, 0x72, 0x56 }}


extern EFI_GUID gH2OFlashMapRegionAuxFvGuid;
extern EFI_GUID gH2OFlashMapRegionBootFvGuid;
extern EFI_GUID gH2OFlashMapRegionBvdtGuid;
extern EFI_GUID gH2OFlashMapRegionEcGuid;
extern EFI_GUID gH2OFlashMapRegionFtwBackupGuid;
extern EFI_GUID gH2OFlashMapRegionFtwStateGuid;
extern EFI_GUID gH2OFlashMapRegionFvGuid;
extern EFI_GUID gH2OFlashMapRegionFvOtherGuid;
extern EFI_GUID gH2OFlashMapRegionFlashMapGuid;
extern EFI_GUID gH2OFlashMapRegionGpnvGuid;
extern EFI_GUID gH2OFlashMapRegionLicenseGuid;
extern EFI_GUID gH2OFlashMapRegionLogoGuid;
extern EFI_GUID gH2OFlashMapRegionMicrocodeGuid;
extern EFI_GUID gH2OFlashMapRegionMsdmGuid;
extern EFI_GUID gH2OFlashMapRegionMultiConfigGuid;
extern EFI_GUID gH2OFlashMapRegionOdmGuid;
extern EFI_GUID gH2OFlashMapRegionOemGuid;
extern EFI_GUID gH2OFlashMapRegionPasswordGuid;
extern EFI_GUID gH2OFlashMapRegionSmbiosEventLogGuid;
extern EFI_GUID gH2OFlashMapRegionSmbiosUpdateGuid;
extern EFI_GUID gH2OFlashMapRegionVarGuid;
extern EFI_GUID gH2OFlashMapRegionVarDefaultGuid;
extern EFI_GUID gH2OFlashMapRegionUnknownGuid;
extern EFI_GUID gH2OFlashMapRegionUnusedGuid;
extern EFI_GUID gH2OFlashMapRegionUsbOptionRomGuid;

extern EFI_GUID gH2OFlashMapRegionDxeFvGuid;
extern EFI_GUID gH2OFlashMapRegionPeiFvGuid;
extern EFI_GUID gH2OFlashMapRegionUnsignedFvGuid;

extern EFI_GUID gH2OFlashMapRegionOptionRomGuid;
extern EFI_GUID gH2OFlashMapRegionOptionRomBdfGuid;
extern EFI_GUID gH2OFlashMapRegionVerbTableGuid;

#endif
