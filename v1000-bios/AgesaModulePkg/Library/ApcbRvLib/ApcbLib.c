/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Library
 *
 * Contains interface to the AMD AGESA library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * AMD processors and its use is subject to the terms and conditions of the
 * AMD Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an AMD processor outside of
 * the applicable AMD product specifications will void any AMD warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and AMD disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of AMD processors.
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/ApcbRvLib.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/AmdPspFlashUpdateLib.h>
#include <Library/ApobCommonServiceLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmReadyToBoot.h>
#include <Library/MemRestore.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE        LIBRARY_APCBRVLIB_APCBLIB_FILECODE

#define APCB_SIGNATURE  0x42435041ul

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern EFI_GUID gSmmApcbRtShadowAddressProtocolGuid;

/*----------------------------------------------------------------------------------------
 *                          G L O B A L        V A L U E S
 *----------------------------------------------------------------------------------------
 */
APCB_HEADER *mApcbSmmRtShadowCopyProtocol = NULL;
APCB_HEADER **mApcbSmmRtShadowCopyPtr = NULL;
BOOLEAN     mInSmm = FALSE;
BOOLEAN     mAtRuntime = FALSE;
UINT64      mApcbEntryAddress = 0;
UINT32      mApcbEntrySize = 0;
EFI_SMM_SYSTEM_TABLE2          *mSmst = NULL;
UINT8                     mApcbInstance = 0;
BOOLEAN                   mApcbRecoveryFlag = 0;

/*
 * Function to get ApcbShadowCopy Address, return NULL if fail
*/
VOID *
GetApcbShadowCopy (
  )
{
  VOID * ApcbData;

  ApcbData = NULL;
  if (mAtRuntime) {
    ApcbData = *mApcbSmmRtShadowCopyPtr;
  } else {
    ApcbData = (VOID *) (UINTN) PcdGet64 (PcdApcbShadowAddress);
  }
  ASSERT (ApcbData != NULL);
  ASSERT ((*(UINT32 *)ApcbData) == APCB_SIGNATURE);
  IDS_HDT_CONSOLE_PSP_TRACE ("GetApcbShadowCopy %x\n", ApcbData);

  return ApcbData;
}

/*
 * Function to set ApcbShadowCopy Address
*/
VOID
SetApcbShadowCopy (
  VOID * ApcbShadowAddress
  )
{
  IDS_HDT_CONSOLE_PSP_TRACE ("SetApcbShadowCopy %x\n", ApcbShadowAddress);
  if (mAtRuntime) {
    *mApcbSmmRtShadowCopyPtr = ApcbShadowAddress;
  } else {
    PcdSet64 (PcdApcbShadowAddress, (UINT64) (UINTN) ApcbShadowAddress);
  }
}
/*
 * Function to calculate APCB checksum
*/
UINT8
ApcbCalcCheckSum (
  IN       VOID             *ApcbPtr,
  IN       UINT32           Length
  )
{
  UINT8  CheckSum;
  UINT8  *DataPtr;

  CheckSum = 0;
  DataPtr = ApcbPtr;
  while (Length --) {
    CheckSum += *(DataPtr ++);
  }
  return CheckSum;
}

/**
 *  Write APCB data to BIOS Directory APCB Entry 0x60
 *
 *
 *  @retval EFI_SUCCESS       The Data save to FLASH successfully
 *
 **/
EFI_STATUS
AmdPspWriteBackApcbShadowCopy (
  VOID
  )
{
  APCB_HEADER                     *ApcbHeader;
  UINT8                           *ApcbData;
  APCB_HEADER                     *ApcbFromSpiRom;
  EFI_STATUS                      Status;
  UINT64                          ComparingAcpiAddress;

  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspWriteBackApcbShadowCopy Enter\n");

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    if (mApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }
  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }
  //Check the size of APCB DRAM copy
  ApcbData = GetApcbShadowCopy ();
  ApcbHeader = (APCB_HEADER *) ApcbData;
  if (ApcbHeader->SizeOfApcb > mApcbEntrySize) {
    IDS_HDT_CONSOLE_PSP_TRACE ("APCB SPI Entry too small\n");
    return EFI_UNSUPPORTED;
  }

  if (IS_SPI_OFFSET (mApcbEntryAddress)) {
    Status = gBS->AllocatePool ( EfiBootServicesData, mApcbEntrySize, (VOID **) &ApcbFromSpiRom);
    ASSERT (ApcbFromSpiRom != NULL);
    if (ApcbFromSpiRom == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Apcb buffer on SpiRom allocate fail\n");
      return EFI_OUT_OF_RESOURCES;
    }
    MapSpiDataToBuffer ((UINT32) mApcbEntryAddress, ApcbFromSpiRom, mApcbEntrySize);
    ComparingAcpiAddress = (UINT64) (UINTN) ApcbFromSpiRom;
  } else {
    ComparingAcpiAddress = mApcbEntryAddress;
  }

  //Compare if any changes
  if ((((APCB_HEADER *) ComparingAcpiAddress)->CheckSumByte != (((APCB_HEADER *) ApcbData)->CheckSumByte)) ||
      (((APCB_HEADER *) ComparingAcpiAddress)->SizeOfApcb != (((APCB_HEADER *) ApcbData)->SizeOfApcb)) ||
      (CompareMem (ApcbData, (VOID *) (UINTN) ComparingAcpiAddress, ApcbHeader->SizeOfApcb))) {

    IDS_HDT_CONSOLE_PSP_TRACE ("Data difference between APCB buffer & SPI copy\n");
    PspUpdateFlash ((UINT32) mApcbEntryAddress, ApcbHeader->SizeOfApcb, ApcbData);

    // APCB Update should trigger memory re-training in the next boot
    IDS_HDT_CONSOLE_PSP_TRACE ("Trigger memory re-training in the next boot\n");

    AmdMemRestoreDiscardCurrentMemContext ();
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspWriteBackApcbShadowCopy Exit\n");

  return EFI_SUCCESS;
}
/**
 *
 *  Init mApcbSmmRtShadowCopy
 *
 **/
EFI_STATUS
EFIAPI
ApcbRTBCallBack (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  IDS_HDT_CONSOLE_PSP_TRACE ("ApcbRTBCallBack\n");
  //Check if mApcbSmmRtShadowCopy already initialized
  //Use APCB signature to do the simple check below
  if (((APCB_HEADER *) *mApcbSmmRtShadowCopyPtr)->Signature != APCB_SIGNATURE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Copy the Apcb from SPI %x to APCB RT copy %x\n", mApcbEntryAddress, *mApcbSmmRtShadowCopyPtr);

    //Copy the Apcb from SPI to Shadow Copy
    if (IS_SPI_OFFSET (mApcbEntryAddress)) {
      MapSpiDataToBuffer ((UINT32)mApcbEntryAddress, *mApcbSmmRtShadowCopyPtr, mApcbEntrySize);
    } else {
      CopyMem (*mApcbSmmRtShadowCopyPtr, (VOID *) (UINTN) mApcbEntryAddress, mApcbEntrySize);
    }
  }
  mAtRuntime = TRUE;
  return EFI_SUCCESS;
}
#define SPI_MMIO_BASE 0xFEC10000
/**
 *
 *  ApcbLibConstructor, initial the shadow copy of APCB data, and save the address to PCD
 *
 **/
EFI_STATUS
EFIAPI
ApcbLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  APCB_HEADER               *ApcbShadowCopy;
  BOOLEAN                   ApcbEntryExist;
  TYPE_ATTRIB               TypeAttrib;
  UINT64                    EntryDest;
  EFI_SMM_BASE2_PROTOCOL    *SmmBase2;
  EFI_STATUS                Status;
  VOID                      *Registration;
  EFI_HANDLE                Handle;
  UINT8                     AltSpiCs;

  //Locate the global data structure via PCD
  ApcbShadowCopy = NULL;
  mInSmm = FALSE;
  IDS_HDT_CONSOLE_PSP_TRACE ("ApcbLibConstructor\n");

  //Exit if BR program
  if (SocFamilyIdentificationCheck (F15_BR_RAW_ID)) {
    return EFI_SUCCESS;
  }
  //Exit service, if recovery flag set
  mApcbRecoveryFlag = FALSE;
  Status = ApobGetApcbRecoveryFlag (&mApcbRecoveryFlag);
  if (mApcbRecoveryFlag) {
    IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
    return EFI_SUCCESS;
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_SUCCESS;
  }

  //Check the existence and size of APCB  entry 0x60
  Status = ApobGetApcbInstance (&mApcbInstance);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("APCB Instance get fail\n");
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Active APCB Instance %x\n", mApcbInstance);
  ApcbEntryExist = BIOSEntryInfo (BIOS_APCB_INFO, mApcbInstance, &TypeAttrib, &mApcbEntryAddress, &mApcbEntrySize, &EntryDest);
  if (!ApcbEntryExist) {
    IDS_HDT_CONSOLE_PSP_TRACE ("APCB Entry not exist\n");
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID**) &SmmBase2);
  if (!EFI_ERROR (Status)) {
    SmmBase2->InSmm (SmmBase2, &mInSmm);
  } else {
    IDS_HDT_CONSOLE_PSP_TRACE ("gEfiSmmBase2ProtocolGuid Locate Fail\n");
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("%a\n", mInSmm ? "InSmm" : "OutSmm");
  //Check if Posttime APCB shadow already initialized
  ApcbShadowCopy = (APCB_HEADER *) (UINTN) PcdGet64 (PcdApcbShadowAddress);
  if (ApcbShadowCopy == NULL) {
    //Shadow Copy haven't init yet
    IDS_HDT_CONSOLE_PSP_TRACE ("Allocate Posttime APCB Shadow Copy with 0x%x bytes\n", mApcbEntrySize);
    Status = gBS->AllocatePool (
              EfiBootServicesData,
              mApcbEntrySize,
              (VOID **) &ApcbShadowCopy
              );
    ASSERT (ApcbShadowCopy != NULL);
    if (ApcbShadowCopy == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB Shadow Copy allocate fail\n");
      return EFI_OUT_OF_RESOURCES;
    }
    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", ApcbShadowCopy, mApcbEntrySize);

    ZeroMem (ApcbShadowCopy, mApcbEntrySize);
    //Copy the Apcb from SPI to Shadow Copy
    //If ROMID set to 1, switch SPI decode to ROM1
    if (TypeAttrib.RomId == 1) {
      AltSpiCs = *((UINT8 *) (UINTN) (SPI_MMIO_BASE +0x1D));
      AltSpiCs &= ~(BIT0 + BIT1);
      AltSpiCs |= 1;
      *((UINT8 *) (UINTN) (SPI_MMIO_BASE +0x1D)) = AltSpiCs;
    }

    //Copy the Apcb from SPI to Shadow Copy
    if (IS_SPI_OFFSET (mApcbEntryAddress)) {
      MapSpiDataToBuffer ((UINT32)mApcbEntryAddress, ApcbShadowCopy, mApcbEntrySize);
    } else {
      CopyMem (ApcbShadowCopy, (VOID *) (UINTN) mApcbEntryAddress, mApcbEntrySize);
    }

    //Switch back to ROM 0
    if (TypeAttrib.RomId == 1) {
      AltSpiCs &= ~(BIT0 + BIT1);
      *((UINT8 *) (UINTN) (SPI_MMIO_BASE +0x1D)) = AltSpiCs;
    }
    //Calc the checksum, and report the warning
    if (ApcbCalcCheckSum (ApcbShadowCopy, ApcbShadowCopy->SizeOfApcb) != 0) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Warning: APCB Checksum not zero\n");
    }
    PcdSet64 (PcdApcbShadowAddress, (UINT64) (UINTN) ApcbShadowCopy);
  }

  if (!mInSmm) {
    return EFI_SUCCESS;
  }

  // We are now in SMM
    // get SMM table base
  Status = SmmBase2->GetSmstLocation (SmmBase2, &mSmst);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("GetSmstLocation Fail\n");
    return Status;
  }
  //Check if Posttime APCB shadow already initialized by locate SmmProtocol
  Status = mSmst->SmmLocateProtocol (&gSmmApcbRtShadowAddressProtocolGuid, NULL, (VOID **) &mApcbSmmRtShadowCopyPtr);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Allocate RT APCB Shadow Copy with 0x%x bytes\n", mApcbEntrySize);
    //Allocate APCB shadow for SMM RT
    Status = mSmst->SmmAllocatePool (
                     EfiRuntimeServicesData,
                     mApcbEntrySize,
                     (VOID **) &ApcbShadowCopy
                     );
    ASSERT (ApcbShadowCopy != NULL);
    if (ApcbShadowCopy == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB Shadow Copy allocate fail\n");
      return Status;
    }
    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", ApcbShadowCopy, mApcbEntrySize);

    ZeroMem (ApcbShadowCopy, mApcbEntrySize);
    mApcbSmmRtShadowCopyProtocol = ApcbShadowCopy;
    mApcbSmmRtShadowCopyPtr = &mApcbSmmRtShadowCopyProtocol;
    Handle = NULL;
    Status = mSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gSmmApcbRtShadowAddressProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mApcbSmmRtShadowCopyProtocol
                      );
  }

  //Register SMM Exit boot service callback used init mApcbSmmRtShadowCopy
  mSmst->SmmRegisterProtocolNotify (
           &gEdkiiSmmReadyToBootProtocolGuid,
           ApcbRTBCallBack,
           &Registration
           );

  return EFI_SUCCESS;
}

