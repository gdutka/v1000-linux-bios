/** @file
  Initialize TPM2 device and measure FVs before handing off control to DXE.

;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/**

Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials 
are licensed and made available under the terms and conditions of the BSD License 
which accompanies this distribution.  The full text of the license may be found at 
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>

#include <IndustryStandard/UefiTcgPlatform.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Ppi/FirmwareVolumeInfo2.h>
#include <Ppi/LockPhysicalPresence.h>
#include <Ppi/TpmInitialized.h>
#include <Ppi/FirmwareVolume.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/FirmwareVolumeInfoMeasurementExcluded.h>
#include <Ppi/Tpm2InterfacePei.h>
#include <Ppi/Reset.h>
#include <Ppi/IoMmu.h>

#include <Guid/TcgEventHob.h>
#include <Guid/MeasuredFvHob.h>
#include <Guid/TpmInstance.h>
#include <Guid/MemoryOverwriteControl.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/TpmPlatformHierarchyLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/HashLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Protocol/Tcg2Protocol.h>
#include <Library/PerformanceLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/MemoryOverwriteLib.h>

#include <TpmPolicy.h>

#define PERF_ID_TCG2_PEI  0x3080

typedef struct {
  EFI_GUID                   *EventGuid;
  EFI_TCG2_EVENT_LOG_FORMAT  LogFormat;
} TCG2_EVENT_INFO_STRUCT;

TCG2_EVENT_INFO_STRUCT mTcg2EventInfo[] = {
  {&gTcgEventEntryHobGuid,   EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2},
  {&gTcgEvent2EntryHobGuid,  EFI_TCG2_EVENT_LOG_FORMAT_TCG_2},
};

BOOLEAN                 mImageInMemory  = FALSE;
EFI_PEI_FILE_HANDLE     mFileHandle;

EFI_PEI_PPI_DESCRIPTOR  mTpmInitializedPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiTpmInitializedPpiGuid,
  NULL
};

EFI_PEI_PPI_DESCRIPTOR  mTpmInitializationDonePpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiTpmInitializationDonePpiGuid,
  NULL
};

//
// Number of firmware blobs to grow by each time we run out of room
//
#define FIRMWARE_BLOB_GROWTH_STEP 4

EFI_PLATFORM_FIRMWARE_BLOB *mMeasuredBaseFvInfo;
UINT32 mMeasuredMaxBaseFvIndex = 0;
UINT32 mMeasuredBaseFvIndex = 0;

EFI_PLATFORM_FIRMWARE_BLOB *mMeasuredChildFvInfo;
UINT32 mMeasuredMaxChildFvIndex = 0;
UINT32 mMeasuredChildFvIndex = 0;

#define DMA_PROTECTION_DISABLED_EVENT_STRING  "DMA Protection Disabled"

/**
  Measure DMA protection policy, and extend the measurement result into a specific PCR.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_DEVICE_ERROR      The operation was unsuccessful.

**/
EFI_STATUS
MeasureDmaProtectionDisabled (
  VOID
  );

/**
  Measure and record the Firmware Volum Information once FvInfoPPI install.

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS          The FV Info is measured and recorded to TPM.
  @return Others               Fail to measure FV.

**/
EFI_STATUS
EFIAPI
FirmwareVolmeInfoPpiNotifyCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  );

/**
  Record all measured Firmware Volum Information into a Guid Hob

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS          The FV Info is measured and recorded to TPM.
  @return Others               Fail to measure FV.

**/
EFI_STATUS
EFIAPI
EndofPeiSignalNotifyCallBack (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  );

/**
  Clear memory below 1MB if MOR is set.

  @param[in] PeiServices        An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation
  @param[in] NotifyDescriptor   Address of the notification descriptor data structure.
  @param[in] Ppi                Address of the PPI that was installed.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval Others                Operation failed.

**/
STATIC
EFI_STATUS
EFIAPI
MemoryOverwriteCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

/**
  Process TPM initialization once PEI_TPM2_INTERFACE_PPI installed.

  @param[in] PeiServices        An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation
  @param[in] NotifyDescriptor   Address of the notification descriptor data structure.
  @param[in] Ppi                Address of the PPI that was installed.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval Others                Operation failed.

**/
STATIC
EFI_STATUS
EFIAPI
Tpm2InterfacePpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

/**
  S3 Resume TPM2 Hierarchy callback.

  @param[in] PeiServices        An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation
  @param[in] NotifyDescriptor   Address of the notification descriptor data structure.
  @param[in] Ppi                Address of the PPI that was installed.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval Others                Operation failed.

**/
STATIC
EFI_STATUS
EFIAPI
S3ErrorReportEndOfPeiNotify (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

/**
  Reset PPI callback.

  @param[in] PeiServices        An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation
  @param[in] NotifyDescriptor   Address of the notification descriptor data structure.
  @param[in] Ppi                Address of the PPI that was installed.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval Others                Operation failed.

**/
EFI_STATUS
EFIAPI
ResetSystemCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

EFI_PEI_NOTIFY_DESCRIPTOR           mNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiFirmwareVolumeInfoPpiGuid,
    FirmwareVolmeInfoPpiNotifyCallback 
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiFirmwareVolumeInfo2PpiGuid,
    FirmwareVolmeInfoPpiNotifyCallback 
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    EndofPeiSignalNotifyCallBack
  }
};

EFI_PEI_FIRMWARE_VOLUME_INFO_MEASUREMENT_EXCLUDED_PPI *mMeasurementExcludedFvPpi;

STATIC EFI_PEI_NOTIFY_DESCRIPTOR    mMorCallbackDescriptor = 
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    MemoryOverwriteCallback
  };

STATIC EFI_PEI_NOTIFY_DESCRIPTOR    mTpm2InterfacePpiNotifyDescriptor = 
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiTpm2InterfacePpiGuid,
    Tpm2InterfacePpiNotifyCallback
  };

STATIC EFI_PEI_NOTIFY_DESCRIPTOR    mResetCallbackDescriptor = 
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiResetPpiGuid,
    ResetSystemCallback
  };

STATIC EFI_PEI_NOTIFY_DESCRIPTOR    mS3ErrorReportEndOfPeiNotifyDesc = 
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    S3ErrorReportEndOfPeiNotify
  };

/**
  Reset PPI callback.

  @param[in] PeiServices        An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation
  @param[in] NotifyDescriptor   Address of the notification descriptor data structure.
  @param[in] Ppi                Address of the PPI that was installed.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval Others                Operation failed.

**/
EFI_STATUS
EFIAPI
ResetSystemCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_RESET_PPI                 *ResetPpi;

  DEBUG ((DEBUG_INFO, "[TPM2] ResetsystemCallback\n"));

  Status = PeiServicesLocatePpi (
             &gEfiPeiResetPpiGuid,
             0,
             NULL,
             (VOID **)&ResetPpi
             );
  if (!EFI_ERROR (Status)) {
    return ResetPpi->ResetSystem ((CONST EFI_PEI_SERVICES **)PeiServices);
  }

  return Status;
}

/**
  Get TPM property.
  This funtion get the value of TPM_PT_PERMANENT got from TPM2_GetCapability.

  @param[out] TpmOwnerProperty   A value of TRUE indicates that the TPM has successfully installed an owner.
  
  @retval EFI_SUCCESS            Operation completed successfully.
  @retval Others                 Operation failed.
**/
STATIC
EFI_STATUS
Tpm2GetProperty (
  OUT BOOLEAN              *TpmProperty
  )
{
  TPMS_CAPABILITY_DATA     TpmCap;
  TPMI_YES_NO              MoreData;
  EFI_STATUS               Status;
  UINT32                   Permanent;
  
  Status = Tpm2GetCapability (
             TPM_CAP_TPM_PROPERTIES, 
             TPM_PT_PERMANENT, 
             1, 
             &MoreData, 
             &TpmCap
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
 Permanent = SwapBytes32 (TpmCap.data.tpmProperties.tpmProperty->value);
  //
  // bit0: ownerAuthSet
  // bit2: lockoutAuthSet
  //
  *TpmProperty = ((Permanent & (BIT0 | BIT2)) == 0) ? FALSE : TRUE;    

  return EFI_SUCCESS;
}

/**
  Record all measured Firmware Volum Information into a Guid Hob
  Guid Hob payload layout is 

     UINT32 *************************** FIRMWARE_BLOB number
     EFI_PLATFORM_FIRMWARE_BLOB******** BLOB Array

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS          The FV Info is measured and recorded to TPM.
  @return Others               Fail to measure FV.

**/
EFI_STATUS
EFIAPI
EndofPeiSignalNotifyCallBack (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
{  
  MEASURED_HOB_DATA *MeasuredHobData;
  EFI_STATUS        Status;
  EDKII_IOMMU_PPI   *IoMmu;

  if ((PcdGet32 (PcdPeiTpmPolicy) & DMA_PROTECTION_MEASUREMENT) != 0) {
    //
    // DMA protection measurement
    //
    Status = PeiServicesLocatePpi (
               &gEdkiiIoMmuPpiGuid,
               0,
               NULL,
               (VOID **)&IoMmu
               );
    if (EFI_ERROR (Status)) {
      //
      // DMA protection is disabled, extend an EV_EFI_ACTION event into PCR[7]
      //
      Status = MeasureDmaProtectionDisabled ();
      DEBUG ((EFI_D_INFO, "MeasureDmaProtectionDisabled - %r\n", Status));
    }
  }

  MeasuredHobData = NULL;

  //
  // Create a Guid hob to save all measured Fv 
  //
  MeasuredHobData = BuildGuidHob(
                      &gMeasuredFvHobGuid,
                      sizeof(UINTN) + sizeof(EFI_PLATFORM_FIRMWARE_BLOB) * (mMeasuredBaseFvIndex + mMeasuredChildFvIndex)
                      );

  if (MeasuredHobData != NULL) {
    //
    // Save measured FV info enty number
    //
    MeasuredHobData->Num = mMeasuredBaseFvIndex + mMeasuredChildFvIndex;

    //
    // Save measured base Fv info
    //
    CopyMem (MeasuredHobData->MeasuredFvBuf, mMeasuredBaseFvInfo, sizeof(EFI_PLATFORM_FIRMWARE_BLOB) * (mMeasuredBaseFvIndex));

    //
    // Save measured child Fv info
    //
    CopyMem (&MeasuredHobData->MeasuredFvBuf[mMeasuredBaseFvIndex] , mMeasuredChildFvInfo, sizeof(EFI_PLATFORM_FIRMWARE_BLOB) * (mMeasuredChildFvIndex));
  }

  return EFI_SUCCESS;
}

/**
  Set Tpm2HashMask PCD value accroding to TPM2 PCR bank.
**/
VOID
SetTpm2HashMask (
  VOID
  )
{
  EFI_STATUS           Status;
  UINT32               ActivePcrBanks;
  TPML_PCR_SELECTION   Pcrs;
  UINTN                Index;

  DEBUG ((EFI_D_ERROR, "SetTpm2HashMask!\n"));

  Status = Tpm2GetCapabilityPcrs (&Pcrs);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Tpm2GetCapabilityPcrs fail!\n"));
    ActivePcrBanks = EFI_TCG2_BOOT_HASH_ALG_SHA1;
  } else {
    DEBUG ((EFI_D_INFO, "Tpm2GetCapabilityPcrs Count - %08x\n", Pcrs.count));
    ActivePcrBanks = 0;
    for (Index = 0; Index < Pcrs.count; Index++) {
      DEBUG ((EFI_D_INFO, "hash - %x\n", Pcrs.pcrSelections[Index].hash));
      switch (Pcrs.pcrSelections[Index].hash) {
      case TPM_ALG_SHA1:
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          ActivePcrBanks |= EFI_TCG2_BOOT_HASH_ALG_SHA1;
        }        
        break;
      case TPM_ALG_SHA256:
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          ActivePcrBanks |= EFI_TCG2_BOOT_HASH_ALG_SHA256;
        }
        break;
      case TPM_ALG_SHA384:
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          ActivePcrBanks |= EFI_TCG2_BOOT_HASH_ALG_SHA384;
        }
        break;
      case TPM_ALG_SHA512:
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          ActivePcrBanks |= EFI_TCG2_BOOT_HASH_ALG_SHA512;
        }
        break;
      case TPM_ALG_SM3_256:
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          ActivePcrBanks |= EFI_TCG2_BOOT_HASH_ALG_SM3_256;
        }
        break;
      }
    }
  }
  Status = PcdSet32S (PcdTpm2HashMask, ActivePcrBanks);
  ASSERT_EFI_ERROR (Status);
}

/**
  Add a new entry to the Event Log.

  @param[in]     DigestList    A list of digest.
  @param[in,out] NewEventHdr   Pointer to a TCG_PCR_EVENT_HDR data structure.
  @param[in]     NewEventData  Pointer to the new event data.

  @retval EFI_SUCCESS           The new event log entry was added.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
**/
EFI_STATUS
LogHashEvent (
  IN TPML_DIGEST_VALUES             *DigestList,
  IN OUT  TCG_PCR_EVENT_HDR         *NewEventHdr,
  IN      UINT8                     *NewEventData
  )
{
  VOID                              *HobData;
  EFI_STATUS                        Status;
  UINTN                             Index;
  EFI_STATUS                        RetStatus;
  UINT32                            SupportedEventLogs;
  TCG_PCR_EVENT2                    *TcgPcrEvent2;
  UINT8                             *DigestBuffer;

  SupportedEventLogs = EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2 | EFI_TCG2_EVENT_LOG_FORMAT_TCG_2;

  RetStatus = EFI_SUCCESS;
  for (Index = 0; Index < sizeof(mTcg2EventInfo)/sizeof(mTcg2EventInfo[0]); Index++) {
    if ((SupportedEventLogs & mTcg2EventInfo[Index].LogFormat) != 0) {
      DEBUG ((EFI_D_INFO, "  LogFormat - 0x%08x\n", mTcg2EventInfo[Index].LogFormat));
      switch (mTcg2EventInfo[Index].LogFormat) {
      case EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2:
        Status = GetDigestFromDigestList (TPM_ALG_SHA1, DigestList, &NewEventHdr->Digest);
        if (!EFI_ERROR (Status)) {
          HobData = BuildGuidHob (
                     &gTcgEventEntryHobGuid,
                     sizeof (*NewEventHdr) + NewEventHdr->EventSize
                     );
          if (HobData == NULL) {
            RetStatus = EFI_OUT_OF_RESOURCES;
            break;
          }

          CopyMem (HobData, NewEventHdr, sizeof (*NewEventHdr));
          HobData = (VOID *) ((UINT8*)HobData + sizeof (*NewEventHdr));
          CopyMem (HobData, NewEventData, NewEventHdr->EventSize);
        }
        break;
      case EFI_TCG2_EVENT_LOG_FORMAT_TCG_2:
        HobData = BuildGuidHob (
                   &gTcgEvent2EntryHobGuid,
                   sizeof(TcgPcrEvent2->PCRIndex) + sizeof(TcgPcrEvent2->EventType) + GetDigestListSize (DigestList) + sizeof(TcgPcrEvent2->EventSize) + NewEventHdr->EventSize
                   );
        if (HobData == NULL) {
          RetStatus = EFI_OUT_OF_RESOURCES;
          break;
        }

        TcgPcrEvent2 = HobData;
        TcgPcrEvent2->PCRIndex = NewEventHdr->PCRIndex;
        TcgPcrEvent2->EventType = NewEventHdr->EventType;
        DigestBuffer = (UINT8 *)&TcgPcrEvent2->Digest;
        DigestBuffer = CopyDigestListToBuffer (DigestBuffer, DigestList, PcdGet32 (PcdTpm2HashMask));
        CopyMem (DigestBuffer, &NewEventHdr->EventSize, sizeof(TcgPcrEvent2->EventSize));
        DigestBuffer = DigestBuffer + sizeof(TcgPcrEvent2->EventSize);
        CopyMem (DigestBuffer, NewEventData, NewEventHdr->EventSize);
        break;
      }
    }
  }

  return RetStatus;
}

/**
  Do a hash operation on a data buffer, extend a specific TPM PCR with the hash result,
  and build a GUIDed HOB recording the event which will be passed to the DXE phase and
  added into the Event Log.

  @param[in]      Flags         Bitmap providing additional information.
  @param[in]      HashData      Physical address of the start of the data buffer 
                                to be hashed, extended, and logged.
  @param[in]      HashDataLen   The length, in bytes, of the buffer referenced by HashData.
  @param[in]      NewEventHdr   Pointer to a TCG_PCR_EVENT_HDR data structure.  
  @param[in]      NewEventData  Pointer to the new event data.  

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
HashLogExtendEvent (
  IN      UINT64                    Flags,
  IN      UINT8                     *HashData,
  IN      UINTN                     HashDataLen,
  IN      TCG_PCR_EVENT_HDR         *NewEventHdr,
  IN      UINT8                     *NewEventData
  )
{
  EFI_STATUS                        Status;
  TPML_DIGEST_VALUES                DigestList;

  if (GetFirstGuidHob (&gTpmErrorHobGuid) != NULL) {
    return EFI_DEVICE_ERROR;
  }

  Status = HashAndExtend (
             NewEventHdr->PCRIndex,
             HashData,
             HashDataLen,
             &DigestList
             );
  if (!EFI_ERROR (Status)) {
    if ((Flags & EFI_TCG2_EXTEND_ONLY) == 0) {
      Status = LogHashEvent (&DigestList, NewEventHdr, NewEventData);
    }
  }
  
  if (Status == EFI_DEVICE_ERROR) {
    DEBUG ((EFI_D_ERROR, "HashLogExtendEvent - %r. Disable TPM.\n", Status));
    BuildGuidHob (&gTpmErrorHobGuid,0);
    REPORT_STATUS_CODE (
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (PcdGet32 (PcdStatusCodeSubClassTpmDevice) | EFI_P_EC_INTERFACE_ERROR)
      );
  }

  return Status;
}

/**
  Measure DMA protection policy, and extend the measurement result into a specific PCR.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_DEVICE_ERROR      The operation was unsuccessful.

**/
EFI_STATUS
MeasureDmaProtectionDisabled (
  VOID
  )
{
  TCG_PCR_EVENT_HDR                 TcgEventHdr;

  TcgEventHdr.PCRIndex  = 7;
  TcgEventHdr.EventType = EV_EFI_ACTION;
  TcgEventHdr.EventSize = sizeof (DMA_PROTECTION_DISABLED_EVENT_STRING) - 1;

  return HashLogExtendEvent (
           0,
           (UINT8 *)DMA_PROTECTION_DISABLED_EVENT_STRING,
           sizeof (DMA_PROTECTION_DISABLED_EVENT_STRING) - 1,
           &TcgEventHdr,
           (UINT8 *)DMA_PROTECTION_DISABLED_EVENT_STRING
           );
}

/**
  Measure CRTM version.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
MeasureCRTMVersion (
  VOID
  )
{
  TCG_PCR_EVENT_HDR                 TcgEventHdr;

  if ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_MEASURE_CRTM_VERSION) != 0) {
    return EFI_SUCCESS;
  }
  //
  // Use FirmwareVersion string to represent CRTM version.
  // OEMs should get real CRTM version string and measure it.
  //

  TcgEventHdr.PCRIndex  = 0;
  TcgEventHdr.EventType = EV_S_CRTM_VERSION;
  TcgEventHdr.EventSize = (UINT32) StrSize((CHAR16*)PcdGetPtr (PcdFirmwareVersionString));

  return HashLogExtendEvent (
           0,
           (UINT8*)PcdGetPtr (PcdFirmwareVersionString),
           TcgEventHdr.EventSize,
           &TcgEventHdr,
           (UINT8*)PcdGetPtr (PcdFirmwareVersionString)
           );
}

/**
  Measure FV image. 
  Add it into the measured FV list after the FV is measured successfully. 

  @param[in]  FvBase            Base address of FV image.
  @param[in]  FvLength          Length of FV image.

  @retval EFI_SUCCESS           Fv image is measured successfully 
                                or it has been already measured.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
MeasureFvImage (
  IN EFI_PHYSICAL_ADDRESS           FvBase,
  IN UINT64                         FvLength
  )
{
  UINT32                            Index;
  EFI_STATUS                        Status;
  EFI_PLATFORM_FIRMWARE_BLOB        FvBlob;
  TCG_PCR_EVENT_HDR                 TcgEventHdr;

  if ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_MEASURE_FV_IMAGE) != 0) {
    return EFI_SUCCESS;
  }

  //
  // Check if it is in Excluded FV list
  //
  if (mMeasurementExcludedFvPpi != NULL) {
    for (Index = 0; Index < mMeasurementExcludedFvPpi->Count; Index ++) {
      if (mMeasurementExcludedFvPpi->Fv[Index].FvBase == FvBase) {
        DEBUG ((DEBUG_INFO, "The FV which is excluded by Tcg2Pei starts at: 0x%x\n", FvBase));
        DEBUG ((DEBUG_INFO, "The FV which is excluded by Tcg2Pei has the size: 0x%x\n", FvLength));
        return EFI_SUCCESS;
      }
    }
  }

  //
  // Check whether FV is in the measured FV list.
  //
  for (Index = 0; Index < mMeasuredBaseFvIndex; Index ++) {
    if (mMeasuredBaseFvInfo[Index].BlobBase == FvBase) {
      return EFI_SUCCESS;
    }
  }
  
  //
  // Measure and record the FV to the TPM
  //
  FvBlob.BlobBase   = FvBase;
  FvBlob.BlobLength = FvLength;

  DEBUG ((DEBUG_INFO, "The FV which is measured by Tcg2Pei starts at: 0x%x\n", FvBlob.BlobBase));
  DEBUG ((DEBUG_INFO, "The FV which is measured by Tcg2Pei has the size: 0x%x\n", FvBlob.BlobLength));

  TcgEventHdr.PCRIndex = 0;
  TcgEventHdr.EventType = EV_EFI_PLATFORM_FIRMWARE_BLOB;
  TcgEventHdr.EventSize = sizeof (FvBlob);

  Status = HashLogExtendEvent (
             0,
             (UINT8*) (UINTN) FvBlob.BlobBase,
             (UINTN) FvBlob.BlobLength,
             &TcgEventHdr,
             (UINT8*) &FvBlob
             );

  //
  // Add new FV into the measured FV list.
  //
  if (mMeasuredBaseFvIndex >= mMeasuredMaxBaseFvIndex) {
    mMeasuredBaseFvInfo = ReallocatePool (
                            sizeof (EFI_PLATFORM_FIRMWARE_BLOB) * mMeasuredMaxBaseFvIndex,
                            sizeof (EFI_PLATFORM_FIRMWARE_BLOB) * (mMeasuredMaxBaseFvIndex + FIRMWARE_BLOB_GROWTH_STEP),
                            mMeasuredBaseFvInfo
                            );
    ASSERT (mMeasuredBaseFvInfo != NULL);
    if (mMeasuredBaseFvInfo == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    mMeasuredMaxBaseFvIndex = mMeasuredMaxBaseFvIndex + FIRMWARE_BLOB_GROWTH_STEP;
  }

  mMeasuredBaseFvInfo[mMeasuredBaseFvIndex].BlobBase   = FvBase;
  mMeasuredBaseFvInfo[mMeasuredBaseFvIndex].BlobLength = FvLength;
  mMeasuredBaseFvIndex++;

  return Status;
}

/**
  Measure main BIOS.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
MeasureMainBios (
  VOID
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_FV_HANDLE                 VolumeHandle;
  EFI_FV_INFO                       VolumeInfo;
  EFI_PEI_FIRMWARE_VOLUME_PPI       *FvPpi;

  if ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_MEASURE_MAIN_BIOS) != 0) {
    return EFI_SUCCESS;
  }

  PERF_START_EX (mFileHandle, "EventRec", "Tcg2Pei", 0, PERF_ID_TCG2_PEI);

  //
  // Only measure BFV at the very beginning. Other parts of Static Core Root of
  // Trust for Measurement(S-CRTM) will be measured later on FvInfoNotify.
  // BFV is processed without installing FV Info Ppi. Other FVs either inside BFV or
  // reported by platform will be installed with Fv Info Ppi
  // This firmware volume measure policy can be modified/enhanced by special
  // platform for special CRTM TPM measuring.
  //
  Status = PeiServicesFfsFindNextVolume (0, &VolumeHandle);
  ASSERT_EFI_ERROR (Status);

  //
  // Measure and record the firmware volume that is dispatched by PeiCore
  //
  Status = PeiServicesFfsGetVolumeInfo (VolumeHandle, &VolumeInfo);
  ASSERT_EFI_ERROR (Status);
  //
  // Locate the corresponding FV_PPI according to founded FV's format guid
  //
  Status = PeiServicesLocatePpi (
             &VolumeInfo.FvFormat,
             0,
             NULL,
             (VOID**)&FvPpi
             );
  ASSERT_EFI_ERROR (Status);

  Status = MeasureFvImage ((EFI_PHYSICAL_ADDRESS) (UINTN) VolumeInfo.FvStart, VolumeInfo.FvSize);

  PERF_END_EX (mFileHandle, "EventRec", "Tcg2Pei", 0, PERF_ID_TCG2_PEI + 1);

  return Status;
}

/**
  Measure and record the Firmware Volum Information once FvInfoPPI install.

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS          The FV Info is measured and recorded to TPM.
  @return Others               Fail to measure FV.

**/
EFI_STATUS
EFIAPI
FirmwareVolmeInfoPpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_PEI_FIRMWARE_VOLUME_INFO_PPI  *Fv;
  EFI_STATUS                        Status;
  EFI_PEI_FIRMWARE_VOLUME_PPI       *FvPpi;
  UINTN                             Index;

  Fv = (EFI_PEI_FIRMWARE_VOLUME_INFO_PPI *) Ppi;

  //
  // The PEI Core can not dispatch or load files from memory mapped FVs that do not support FvPpi.
  //
  Status = PeiServicesLocatePpi (
             &Fv->FvFormat, 
             0, 
             NULL,
             (VOID**)&FvPpi
             );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }
  
  //
  // This is an FV from an FFS file, and the parent FV must have already been measured,
  // No need to measure twice, so just record the FV and return
  //
  if (Fv->ParentFvName != NULL || Fv->ParentFileName != NULL ) {

    if (mMeasuredChildFvIndex >= mMeasuredMaxChildFvIndex) {
      mMeasuredChildFvInfo = ReallocatePool (
                               sizeof (EFI_PLATFORM_FIRMWARE_BLOB) * mMeasuredMaxChildFvIndex,
                               sizeof (EFI_PLATFORM_FIRMWARE_BLOB) * (mMeasuredMaxChildFvIndex + FIRMWARE_BLOB_GROWTH_STEP),
                               mMeasuredChildFvInfo
                               );
      ASSERT (mMeasuredChildFvInfo != NULL);
      if (mMeasuredChildFvInfo == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      mMeasuredMaxChildFvIndex = mMeasuredMaxChildFvIndex + FIRMWARE_BLOB_GROWTH_STEP;
    }
    //
    // Check whether FV is in the measured child FV list.
    //
    for (Index = 0; Index < mMeasuredChildFvIndex; Index++) {
      if (mMeasuredChildFvInfo[Index].BlobBase == (EFI_PHYSICAL_ADDRESS) (UINTN) Fv->FvInfo) {
        return EFI_SUCCESS;
      }
    }
    mMeasuredChildFvInfo[mMeasuredChildFvIndex].BlobBase   = (EFI_PHYSICAL_ADDRESS) (UINTN) Fv->FvInfo;
    mMeasuredChildFvInfo[mMeasuredChildFvIndex].BlobLength = Fv->FvInfoSize;
    mMeasuredChildFvIndex++;
    return EFI_SUCCESS;
  }

  return MeasureFvImage ((EFI_PHYSICAL_ADDRESS) (UINTN) Fv->FvInfo, Fv->FvInfoSize);
}


/**
  Configure Tpm2 platform Hierarchy for S3 error report

  @param[in] PeiServices        An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation
  @param[in] NotifyDescriptor   Address of the notification descriptor data structure.
  @param[in] Ppi                Address of the PPI that was installed.

  @retval EFI_SUCCESS           Operation completed successfully.

**/
STATIC
EFI_STATUS
EFIAPI
S3ErrorReportEndOfPeiNotify (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  ConfigureTpmPlatformHierarchy ();
  return EFI_SUCCESS;
}

/**
  Clear memory below 1MB if MOR is set.

  @param[in] PeiServices        An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation
  @param[in] NotifyDescriptor   Address of the notification descriptor data structure.
  @param[in] Ppi                Address of the PPI that was installed.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval Others                Operation failed.

**/
STATIC
EFI_STATUS
EFIAPI
MemoryOverwriteCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *Variable;
  UINTN                             VariableSize ;
  UINT8                             MorVariable;
  BOOLEAN                           TpmProperty;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  //  Check MemoryOverwriteRequestControl variable
  //
  VariableSize = sizeof (MorVariable);
  Status = Variable->GetVariable ( 
                       Variable, 
                       MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                       &gEfiMemoryOverwriteControlDataGuid,
                       NULL,
                       &VariableSize,
                       &MorVariable
                       );
  if (!EFI_ERROR (Status) && MOR_CLEAR_MEMORY_VALUE (MorVariable) == 0x0) {
    //
    // No need to clear memory.
    //
    return EFI_SUCCESS;
  }
  
  //
  // Get property is moved from driver's entry to here,
  // since some TPM may fail in the command execution in MA.
  //
  TpmProperty = FALSE;
  if ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_PEI_TPM_PROPERTY_CHECK) == 0) {    
    Status = Tpm2GetProperty (&TpmProperty);
    if (EFI_ERROR (Status)) {
      TpmProperty = TRUE;
    }
  } else {
    TpmProperty = TRUE;
  }
  
  if (TpmProperty) {
    //
    // To clear the memory region below 1M here before CSM use it.
    // The other memory regions will be cleared in DXE phase.
    //
    Status = MemoryOverwriteBelow1M ();
  }
  return Status;
}

/**
  Do measurement after memory is ready.

  @param[in]      PeiServices   Describes the list of possible PEI Services.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
PeimEntryMP (
  IN      EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS                        Status;

  Status = PeiServicesLocatePpi (
               &gEfiPeiFirmwareVolumeInfoMeasurementExcludedPpiGuid, 
               0, 
               NULL,
               (VOID**)&mMeasurementExcludedFvPpi
               );
  // Do not check status, because it is optional

  Status = MeasureCRTMVersion ();

  Status = MeasureMainBios ();
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Post callbacks:
  // for the FvInfoPpi services to measure and record
  // the additional Fvs to TPM
  //
  Status = PeiServicesNotifyPpi (&mNotifyList[0]);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Measure and log Separator event with error, and extend the measurement result into a specific PCR.

  @param[in] PCRIndex         PCR index.  

  @retval EFI_SUCCESS         Operation completed successfully.
  @retval EFI_DEVICE_ERROR    The operation was unsuccessful.

**/
EFI_STATUS
MeasureSeparatorEventWithError (
  IN      TPM_PCRINDEX              PCRIndex
  )
{
  TCG_PCR_EVENT_HDR                 TcgEvent;
  UINT32                            EventData;

  //
  // Use EventData 0x1 to indicate there is error.
  //
  EventData = 0x1;
  TcgEvent.PCRIndex  = PCRIndex;
  TcgEvent.EventType = EV_SEPARATOR;
  TcgEvent.EventSize = (UINT32)sizeof (EventData);
  return HashLogExtendEvent (0,(UINT8 *)&EventData, TcgEvent.EventSize, &TcgEvent,(UINT8 *)&EventData);
}

/**
  TPM initialization entry.

  @param[in] PeiServices  Describes the list of possible PEI Services.

  @return Status.

**/
EFI_STATUS
InitializationEntry (
  IN EFI_PEI_SERVICES               **PeiServices
  )
{
  EFI_STATUS                        Status;
  EFI_STATUS                        Status2;
  EFI_BOOT_MODE                     BootMode;
  TPM_PCRINDEX                      PcrIndex;
  BOOLEAN                           S3ErrorReport;

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (!mImageInMemory) {
    S3ErrorReport = FALSE;
    if ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_TPM_STARTUP) == 0) {
      if (BootMode == BOOT_ON_S3_RESUME) {
        Status = Tpm2Startup (TPM_SU_STATE);
        if (EFI_ERROR (Status) ) {
          Status = Tpm2Startup (TPM_SU_CLEAR);
          if (!EFI_ERROR(Status)) {
            S3ErrorReport = TRUE;
          }
        }
      } else {
        Status = Tpm2Startup (TPM_SU_CLEAR);
      }
      if (EFI_ERROR (Status) ) {
        goto Done;
      }
    }

    //
    // Update Tpm2HashMask according to PCR bank.
    //
    SetTpm2HashMask ();

    if (S3ErrorReport) {
      Status = PeiServicesNotifyPpi (&mS3ErrorReportEndOfPeiNotifyDesc);
      if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "ServicesNotifyPpi fail, Status: %r\n", Status));
      }

      //
      // The system firmware that resumes from S3 MUST deal with a
      // TPM2_Startup error appropriately.
      // For example, issue a TPM2_Startup(TPM_SU_CLEAR) command and
      // configuring the device securely by taking actions like extending a
      // separator with an error digest (0x01) into PCRs 0 through 7.
      //
      for (PcrIndex = 0; PcrIndex < 8; PcrIndex++) {
        Status = MeasureSeparatorEventWithError (PcrIndex);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "Separator Event with Error not Measured. Error!\n"));
        }
      }

      //
      // Disable EH & SH
      //
      Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, NULL, TPM_RH_ENDORSEMENT, NO);
      DEBUG ((EFI_D_INFO, "[TPM2] Disable Endorsement Hierarchy %r\n", Status));
      Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, NULL, TPM_RH_OWNER, NO);
      DEBUG ((EFI_D_INFO, "[TPM2] Disable Storage Hierarchy %r\n", Status));

      if (FeaturePcdGet (PcdH2OTpmS3StartupFailureReboot)) {
        //
        // Need reset system
        //
        DEBUG ((EFI_D_INFO, "[TPM2] TPM S3 Startup Failed, Reset System\n"));
        Status = PeiServicesNotifyPpi (&mResetCallbackDescriptor);
      }
    }

    //
    // TpmSelfTest is optional on S3 path, skip it to save S3 time
    //
    if (BootMode != BOOT_ON_S3_RESUME) {
      if ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_TPM_SELF_TEST) == 0) {    
        Status = Tpm2SelfTest (NO);
        if (EFI_ERROR (Status)) {
          goto Done;
        }
      }
      if (BootMode != BOOT_ON_FLASH_UPDATE) {
        Status = PeiServicesNotifyPpi (&mMorCallbackDescriptor);
        ASSERT_EFI_ERROR (Status);
      }
    }

    //
    // Only intall TpmInitializedPpi on success
    //
    Status = PeiServicesInstallPpi (&mTpmInitializedPpiList);
    ASSERT_EFI_ERROR (Status);
  }

  if (mImageInMemory) {
    Status = PeimEntryMP ((EFI_PEI_SERVICES**)PeiServices);
    return Status;
  }

Done:
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "TPM2 error! Build Hob\n"));
    BuildGuidHob (&gTpmErrorHobGuid,0);
    REPORT_STATUS_CODE (
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (PcdGet32 (PcdStatusCodeSubClassTpmDevice) | EFI_P_EC_INTERFACE_ERROR)
      );
  }
  //
  // Always intall TpmInitializationDonePpi no matter success or fail.
  // Other driver can know TPM initialization state by TpmInitializedPpi.
  //
  Status2 = PeiServicesInstallPpi (&mTpmInitializationDonePpiList);
  ASSERT_EFI_ERROR (Status2);

  return Status;
}

/**
  Process TPM initialization once PEI_TPM2_INTERFACE_PPI installed.

  @param[in] PeiServices        An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation
  @param[in] NotifyDescriptor   Address of the notification descriptor data structure.
  @param[in] Ppi                Address of the PPI that was installed.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval Others                Operation failed.

**/
EFI_STATUS
EFIAPI
Tpm2InterfacePpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_STATUS                        Status;
  VOID                              *Interface;

  Status = PeiServicesLocatePpi (
             &gPeiTpm2InterfacePpiGuid,
             0,
             NULL,
             (VOID **)&Interface
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = PeiServicesLocatePpi (
             &gPeiTpmInitializationDonePpiGuid,
             0,
             NULL,
             (VOID **)&Interface
             );
  if (!EFI_ERROR (Status)) {
    //
    // TPM initialization is done already
    //
    return Status;
  }

  Status = Tpm2RequestUseTpm ();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  return InitializationEntry (PeiServices);
}

/**
  Entry point of this module.

  @param[in] FileHandle   Handle of the file being invoked.
  @param[in] PeiServices  Describes the list of possible PEI Services.

  @return Status.

**/
EFI_STATUS
EFIAPI
PeimEntryMA (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;
  VOID                              *Interface;

  if (CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceNoneGuid) ||
      CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid)){
    DEBUG ((DEBUG_INFO, "No TPM2 instance required!\n"));
    return EFI_UNSUPPORTED;
  }

  if (GetFirstGuidHob (&gTpmErrorHobGuid) != NULL) {
    DEBUG ((EFI_D_ERROR, "TPM2 error!\n"));
    return EFI_DEVICE_ERROR;
  }

  if (PcdGetBool (PcdTpmHide)) {
    DEBUG((EFI_D_INFO, "TPM is hidden\n"));
    return EFI_UNSUPPORTED;
  }

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // In S3 path, skip shadow logic. no measurement is required
  //
  if (BootMode != BOOT_ON_S3_RESUME) {
    Status = (**PeiServices).RegisterForShadow(FileHandle);
    if (Status == EFI_ALREADY_STARTED) {
      mImageInMemory = TRUE;
      mFileHandle = FileHandle;
    } else if (Status == EFI_NOT_FOUND) {
      ASSERT_EFI_ERROR (Status);
    }
  }

  if (!mImageInMemory) {
    //
    // Initialize TPM device
    //
    Status = Tpm2RequestUseTpm ();
    if (Status == EFI_UNSUPPORTED) {
      DEBUG ((DEBUG_ERROR, "TPM2 not detected!\n"));
      return PeiServicesNotifyPpi (&mTpm2InterfacePpiNotifyDescriptor);
    }
  } else {
    Status = PeiServicesLocatePpi (
               &gPeiTpmInitializationDonePpiGuid,
               0,
               NULL,
               (VOID **)&Interface
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "TPM2 error! Build Hob\n"));
      BuildGuidHob (&gTpmErrorHobGuid,0);
      return EFI_DEVICE_ERROR;
    }
  }
  return InitializationEntry ((EFI_PEI_SERVICES **)PeiServices);
}
