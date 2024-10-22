/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU POST API, and related functions.
 *
 * Contains code that initialized the CPU after memory init.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/*++
Module Name:

  PspPei.c
  Init PSP device
  Intercept MemoryDiscovery protocol and
    a) Send mailbox to PSP to inform the DRAM information

Abstract:
--*/

#include "PiPei.h"
#include <Library/HobLib.h>
#include <Ppi/AmdPspRecoveryDetectPpi.h>
#include <Ppi/AmdPspCommonServicePpi.h>
#include <Ppi/EndOfPeiPhase.h>

#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/AmdPspMboxLibV2.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Addendum/Apcb/Inc/APOBCMN.h>
#include <Library/AmdPspApobLib.h>
#include <Library/AmdCapsuleLib.h>
#include <Ppi/AmdErrorLogPpi.h>
#include <Ppi/AmdErrorLogServicePpi.h>
#include <Library/FchBaseLib.h>
#include <Ppi/FabricTopologyServices2Ppi.h>
#include <Filecode.h>

#define FILECODE PSP_AMDPSPPEIV2_AMDPSPPEIV2_FILECODE

STATIC AMD_PSP_COMMON_SERVICE_PPI mPspCommonServicePpi = {
  GetFtpmControlAreaV2,
  SwitchPspMmioDecodeV2,
  CheckPspDevicePresentV2,
  CheckFtpmCapsV2,
  PSPEntryInfoV2,
  PspLibTimeOutV2,
  AcquirePspSmiRegMutexV2,
  ReleasePspSmiRegMutexV2,
  CheckPspCapsV2
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPspCommonServicePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPspCommonServicePpiGuid,
  &mPspCommonServicePpi
};

STATIC AMD_PSP_RECOVERY_DETECT_PPI mPspRecoveryDetectPpi = {
  0x01
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPspRecoveryDetectedPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPspRecoveryDetectPpiGuid,
  &mPspRecoveryDetectPpi
};

extern  EFI_GUID  gAmdErrorLogServicePpiGuid;
extern  EFI_GUID  gAmdFabricTopologyServices2PpiGuid;
EFI_STATUS
EFIAPI
EndOfPeiSignalPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );


EFI_PEI_NOTIFY_DESCRIPTOR   mEndOfPeiSignalPpiCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  EndOfPeiSignalPpiCallback
};

VOID
AutoPsbFusing (
  )
{
  TYPE_ATTRIB                 TypeAttrib;
  UINT64                      EntryAddress;
  UINT32                      EntrySize;
  UINT64                      EntryDest;
  PSB_HSTI_STATUS_1           PsbHstiStatus1;
  PSB_STATUS_2                PsbStatus2;
  UINT32                      MailboxHSTIState;
  EFI_STATUS                  Status;
  UINT8                       Value8 = 0x0;
  UINT32                      RpmcAddressToUse = 0x0;
  UINT32                      PspCaps = 0x0;

  Status = PspMboxPspCapsQuery(&PspCaps);
  IDS_HDT_CONSOLE_PSP_TRACE ("PSP Capabilities = 0x%X, Status = %r\n", PspCaps, Status);

  if ((!EFI_ERROR(Status)) && (PspCaps & PSP_RPMC_ENABLE)) { //SPI ROM supports RPMC.
    RpmcAddressToUse = PcdGet8(PcdAmdPspRPMCAddress);
    if ((RpmcAddressToUse != 0) && (RpmcAddressToUse < PSP_RPMC_COUNTERS)) {
      //Test to make sure if RPMC root key is already provisioned or not, through HSTI Query.
      PspMboxBiosQueryHSTIState (&MailboxHSTIState);
      IDS_HDT_CONSOLE_PSP_TRACE ("RPMC HSTI value 0x%X\n", MailboxHSTIState);

      if ((MailboxHSTIState & PSP_RPMC_PRODUCTION_ENABLED) == 0) { //RPMC root key is not programmed.
        Status = PspMboxBiosCmdSetRpmcAddress(RpmcAddressToUse);

        if (Status == EFI_SUCCESS) {
          //Trigger warm reset
          IDS_HDT_CONSOLE_PSP_TRACE ("Trigger warm reset to initiate RPMC key provisioning after reset.\n");

          Value8 = 0x6;
          LibAmdIoWrite (AccessWidth8, 0xCF9, &Value8, NULL);
        }
        else {
          IDS_HDT_CONSOLE_PSP_TRACE ("MboxBiosCmdSetRpmcAddress Cmd failed with Status = 0x%X.\n", Status);
        }
      } else {
        IDS_HDT_CONSOLE_PSP_TRACE ("RPMC Root Key is already provisioned...\n");
      }
    } else {
      IDS_HDT_CONSOLE_PSP_TRACE ("RPMC counter is not within the required range of 1 to %d.\n", PSP_RPMC_COUNTERS-1);
    }
  } else {
    IDS_HDT_CONSOLE_PSP_TRACE ("SPI ROM doesn't support RPMC.\n");
  }

  // Check PCD master token is enabled
  if (PcdGetBool (PcdAmdPspAutoPsb) == FALSE) {
    return;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp AutoPsb Feature\n");

  GetPsbStatus2 (&PsbStatus2);
  IDS_HDT_CONSOLE_PSP_TRACE ("PSB_STATUS_2:\n");
  IDS_HDT_CONSOLE_PSP_TRACE ("\tPlatformVendorId: %X\n", PsbStatus2.PlatformVendorId);
  IDS_HDT_CONSOLE_PSP_TRACE ("\tPlatformModelId: %X\n", PsbStatus2.PlatformModelId);
  IDS_HDT_CONSOLE_PSP_TRACE ("\tBiosKeyRevisionId: %X\n", PsbStatus2.BiosKeyRevisionId);
  IDS_HDT_CONSOLE_PSP_TRACE ("\tRootKeySelect: %X\n", PsbStatus2.RootKeySelect);
  IDS_HDT_CONSOLE_PSP_TRACE ("\tPlatformSecureBootEn: %X\n", PsbStatus2.PlatformSecureBootEn);
  IDS_HDT_CONSOLE_PSP_TRACE ("\tDisableBiosKeyAntiRollback: %X\n", PsbStatus2.DisableBiosKeyAntiRollback);
  IDS_HDT_CONSOLE_PSP_TRACE ("\tDisableAmdKeyUsage: %X\n", PsbStatus2.DisableAmdKeyUsage);
  IDS_HDT_CONSOLE_PSP_TRACE ("\tDisableSecureDebugUnlock: %X\n", PsbStatus2.DisableSecureDebugUnlock);
  IDS_HDT_CONSOLE_PSP_TRACE ("\tCustomerKeyLock: %X\n", PsbStatus2.CustomerKeyLock);

  // Check if PSB fuse already burned
  if (PsbStatus2.PlatformSecureBootEn == 1) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Exit: PSB fuse already burned\n");
    return;
  }
  // Check if Super CPU
  if ((PsbStatus2.CustomerKeyLock == 1) &&
      (PsbStatus2.PlatformVendorId == 0) &&
      (PsbStatus2.PlatformModelId == 0)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Exit: Super CPU deteted\n");
    return;
  }

  GetPsbHstiStatus1 (&PsbHstiStatus1);
  IDS_HDT_CONSOLE_PSP_TRACE ("PSB_HSTI_STATUS:\n");
  IDS_HDT_CONSOLE_PSP_TRACE ("\tPsbTestStatus: %X\n", PsbHstiStatus1.PsbTestStatus);
  IDS_HDT_CONSOLE_PSP_TRACE ("\tPsbFusingReadiness: %X\n", PsbHstiStatus1.PsbFusingReadiness);
  IDS_HDT_CONSOLE_PSP_TRACE ("\tHstiState: %X\n", PsbHstiStatus1.HstiState);
  // Check PSB status
  if (PsbHstiStatus1.PsbTestStatus != 0) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Exit: PSB Test fail\n");
    return;
  }

  // Check if required PSP entry exist, BIOS Pub Key and BIOS Signature
  if ((BIOSEntryInfo (BIOS_PUBLIC_KEY, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest) == FALSE) ||
      (BIOSEntryInfo (BIOS_RTM_SIGNATURE, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest) == FALSE)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Exit: Required FW entries are not existed\n");
    return;
  }

  //Test to make sure PSP return the same value between C2P mailbox and C2P register
  PspMboxBiosQueryHSTIState (&MailboxHSTIState);
  if (MailboxHSTIState != PsbHstiStatus1.HstiState) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Exit: HSTI value between C2P mailbox and C2P register not equal\n");
    return;
  }

  //Before send the command Check PsbFusingReadiness
  if ((PsbHstiStatus1.PsbFusingReadiness == 1) && (PsbStatus2.CustomerKeyLock == 0)) {
    // Send AutoPsb command to PSP
    IDS_HDT_CONSOLE_PSP_TRACE ("Send AutoPsb command to PSP\n");
    Status = PspMboxBiosPsbAutoFusing ();
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Error occurs during AutoPsb\n");
      ASSERT (FALSE);
    } else {
      IDS_HDT_CONSOLE_PSP_TRACE ("AutoPsb succeeded\n");
    }
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp AutoPsb Feature Exit\n");
}


EFI_STATUS
EFIAPI
EndOfPeiSignalPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  //This callback will only be called in S3 Path
  //Send Command to PSP to lock DF register
  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspPeiV2.EndOfPeiSignalPpiCallback\n");
  if (FchReadSleepType () == 0x03) {
    PspMboxBiosLockDFReg ();
  } else {
    //PSP OS is loading in parallel of BIOS PEI execution, do auto PSB at the end of PEI
    AutoPsbFusing ();
  }
  return (EFI_SUCCESS);
}

/*++

Routine Description:

  PSP Driver Entry. Initialize PSP device and report DRAM info to PSP once found

Arguments:

Returns:

  EFI_STATUS

--*/
extern EFI_GUID gAmdCapsuleStatusHobGuid;
EFI_STATUS
EFIAPI
AmdPspPeiV2DriverEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  APOB_TYPE_HEADER      *ApobEntries[APOB_ENTRY_INSTANCE_MAX];
  PEI_AMD_ERROR_LOG_PPI *AmdErrorLog;
  EVENT_LOG_STRUCT      *EventLogPtr;
  UINT16                i;
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES2_PPI  *FabricTopologyServices;
  UINT32                NumberOfSockets;
  UINT32                NumberOfDies;
  UINT32                socketId;
  UINT32                dieId;
  UINT32                InstanceId;

  //PEI_AMD_ERROR_LOG_SERVICE_PPI *ErrorLogService;
  //ERROR_LOG_DATA_STRUCT   ErrorLogData;
  AGESA_TESTPOINT (TpPspPeiV2Entry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.AmdPspPeiV2 Enter\n");
  if (AmdCapsuleGetStatus ()) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Build CapsuleStatusHob\n");
   BuildGuidHob (&gAmdCapsuleStatusHobGuid, 1);
  }
  // Public PSP Common Service PPI V1 instance
  IDS_HDT_CONSOLE_PSP_TRACE ("\tInstall PSP common service PPI\n");
  Status = (**PeiServices).InstallPpi (PeiServices, &mPspCommonServicePpiList);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("InstallPpi fail\n");
    ASSERT (FALSE);
  }

  //Check PSP Recovery required Flag, if set publish PPI
  if ((FchReadSleepType () != 3) && (AmdCapsuleGetStatus () == FALSE)) {
    if (CheckPspRecoveryFlagV2 ()) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Recovery Flag Detected\n");
      Status = (**PeiServices).InstallPpi (PeiServices, &mPspRecoveryDetectedPpiList);
      ASSERT (Status == EFI_SUCCESS);
    }
  }
  //
  // Always Initialize PSP BAR to support RdRand Instruction
  //
  IDS_HDT_CONSOLE_PSP_TRACE ("\tPsp BAR init\n");
  PspBarInitEarlyV2 ();

  // Retrieve event log data from ApobEntries and insert to AmdErrorLog
  Status = (*PeiServices)->LocatePpi (
                                 PeiServices,
                                 &gAmdFabricTopologyServices2PpiGuid,
                                 0,
                                 NULL,
                                 (VOID**)&FabricTopologyServices
                                 );
  ASSERT (Status == EFI_SUCCESS);

  FabricTopologyServices->GetSystemInfo (&NumberOfSockets, NULL, NULL, NULL, NULL);

  IDS_HDT_CONSOLE (MAIN_FLOW, "APOB EventLog, start\n");
  for (socketId = 0; socketId < NumberOfSockets; socketId++) {
    FabricTopologyServices->GetProcessorInfo (socketId, &NumberOfDies, NULL);
    for (dieId = 0; dieId < NumberOfDies; dieId++) {
      InstanceId = (socketId << 8) | dieId;
      Status = AmdPspGetApobEntryInstance(APOB_GEN, APOB_GEN_EVENT_LOG_TYPE, InstanceId, FALSE, ApobEntries);

      if (Status == EFI_SUCCESS) {
        Status = (*PeiServices)->LocatePpi (PeiServices, &gAmdErrorLogPpiGuid, 0, NULL, (VOID**)&AmdErrorLog);
        if (Status == EFI_SUCCESS) {
          EventLogPtr = (EVENT_LOG_STRUCT *) *ApobEntries;
          for (i = 0; i < EventLogPtr->Count; i++) {
            IDS_HDT_CONSOLE (MAIN_FLOW, "class: %08x, info: %08x, dataA: %08x, dataB: %08x\n",
                             EventLogPtr->ApobEventStruct.AgesaEvent[i].EventClass,
                             EventLogPtr->ApobEventStruct.AgesaEvent[i].EventInfo,
                             EventLogPtr->ApobEventStruct.AgesaEvent[i].DataA,
                             EventLogPtr->ApobEventStruct.AgesaEvent[i].DataB);
            AmdErrorLog->AmdErrorLogPei (AmdErrorLog,
                                         EventLogPtr->ApobEventStruct.AgesaEvent[i].EventClass,
                                         EventLogPtr->ApobEventStruct.AgesaEvent[i].EventInfo,
                                         EventLogPtr->ApobEventStruct.AgesaEvent[i].DataA,
                                         EventLogPtr->ApobEventStruct.AgesaEvent[i].DataB,
                                         0,   // AmdErrorLog has DataParam3 & DataParam4, but even log data doesn't
                                         0);
          }
        }
      }
    }
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "APOB EventLog, end\n");

  //If PSP feature turn off, exit the driver
  if ((CheckPspDevicePresentV2 () == FALSE) ||
      (PcdGetBool (PcdAmdPspEnable) == FALSE)) {
    return EFI_SUCCESS;
  }

  //
  // Register end of PEI callback for S3
  //

  Status = (**PeiServices).NotifyPpi (PeiServices, &mEndOfPeiSignalPpiCallback);
  ASSERT (Status == EFI_SUCCESS);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Notify EndOfPei Ppi fail\n");
  }


  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspPeiV2 Exit\n");
  AGESA_TESTPOINT (TpPspPeiV2Exit, NULL);

  return (Status);
}


