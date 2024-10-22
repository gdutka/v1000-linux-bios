/** @file
  SDMMC Supplement Protocol definition
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_SD_MMC_SUPPLEMENT_H_
#define _H2O_SD_MMC_SUPPLEMENT_H_

#define H2O_SD_MMC_SUPPLEMENT_PROTOCOL_GUID \
  { \
  0x46784364, 0xb9f1, 0x4f21, 0xa3, 0xa9, 0x36, 0xb8, 0xf7, 0x31, 0x85, 0xdf \
  }

typedef struct _H2O_SD_MMC_SUPPLEMENT_PROTOCOL  H2O_SD_MMC_SUPPLEMENT_PROTOCOL;

#define H2O_SD_MMC_SUPPLEMENT_PROTOCOL_REVISION 0x01 

/**
  Switch the high speed timing according to request.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.6.8 and SD Host Controller
  Simplified Spec 3.0 Figure 2-29 for details.

  @param[in] This           A pointer to the H2O_SD_MMC_SUPPLEMENT_PROTOCOL instance.
  @param[in] PciIo          A pointer to the EFI_PCI_IO_PROTOCOL instance.
  @param[in] PassThru       A pointer to the EFI_SD_MMC_PASS_THRU_PROTOCOL instance.
  @param[in] Slot           The slot number of the SD card to send the command to.
  @param[in] Rca            The relative device address to be assigned.
  @param[in] DeviceType     Desired DeviceType to be set.
  @param[in] SDRMode        Desired bus width to be set.

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
typedef
EFI_STATUS
(EFIAPI *H2O_SD_MMC_SUPPLEMENT_EMMC_SET_BUS_MODE) (
  IN H2O_SD_MMC_SUPPLEMENT_PROTOCOL     *This,
  IN EFI_PCI_IO_PROTOCOL                *PciIo,
  IN EFI_SD_MMC_PASS_THRU_PROTOCOL      *PassThru,
  IN UINT8                              Slot,
  IN UINT16                             Rca,
  IN UINT8                              DeviceType,
  IN UINT8                              BusWidth
  );

/**
  Execute EMMC device identification procedure.

  Refer to EMMC Electrical Standard Spec 5.1 Section 6.4 for details.

  @param[in] This           A pointer to the H2O_SD_MMC_SUPPLEMENT_PROTOCOL instance.
  @param[in] PciIo          A pointer to the EFI_PCI_IO_PROTOCOL instance.
  @param[in] PassThru       A pointer to the EFI_SD_MMC_PASS_THRU_PROTOCOL instance.
  @param[in] Slot           The slot number of the SD card to send the command to.

  @retval EFI_SUCCESS       There is a EMMC card.
  @retval Others            There is not a EMMC card.

**/
typedef
EFI_STATUS
(EFIAPI *H2O_SD_MMC_SUPPLEMENT_EMMC_ID) (
  IN H2O_SD_MMC_SUPPLEMENT_PROTOCOL *This,
  IN EFI_PCI_IO_PROTOCOL            *PciIo,
  IN EFI_SD_MMC_PASS_THRU_PROTOCOL  *PassThru,
  IN UINT8                          Slot
  );

struct _H2O_SD_MMC_SUPPLEMENT_PROTOCOL {
  UINT32                                                Revision;
  H2O_SD_MMC_SUPPLEMENT_EMMC_SET_BUS_MODE               EmmcSetBusMode;
  H2O_SD_MMC_SUPPLEMENT_EMMC_ID                         EmmcIdentification;
};

extern EFI_GUID gH2OSdMmcSupplementProtocolGuid;

#endif
