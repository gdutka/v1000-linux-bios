/** @file
  BootService DXE driver for SMART Report Status Code

;******************************************************************************
;* Copyright (c) 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Protocol/AtaPassThru.h>
#include <Protocol/NvmExpressPassthru.h>
#include <Protocol/DevicePath.h>

#include <Guid/EventGroup.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/H2OCpLib.h>

#include <IndustryStandard/Atapi.h>
#include <IndustryStandard/Nvme.h>

STATIC NVME_SMART_HEALTH_INFO_LOG     mNVMeHealthBuffer;
STATIC EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET mNVMeCommandPacket;
STATIC EFI_NVM_EXPRESS_COMMAND                  mNVMeCommand;
STATIC EFI_NVM_EXPRESS_COMPLETION               mNVMeCompletion;
STATIC NVME_ADMIN_CONTROLLER_DATA               mNVMeControllerDataBuffer;
STATIC EFI_ATA_COMMAND_BLOCK                    mAcb;
STATIC EFI_ATA_STATUS_BLOCK                     mAsb;
STATIC ATA_IDENTIFY_DATA                        mIdentifyData;
STATIC EFI_ATA_PASS_THRU_COMMAND_PACKET         mAtaPassThruCmdPacket;


#define SMART_STATUS_CHECKPOINT_NOTIFY_TPL    H2O_CP_MEDIUM_LOW
#define ATA_IDENTIFY_CMD            0xEC
#define NVME_ADMIN_QUEUE            0x00
#define NVME_GENERIC_TIMEOUT           EFI_TIMER_PERIOD_SECONDS (5)



/**
  Find SATA port from device path

  @param[in] DevicePath          SATA device path
  @param[out] Port               SATA port number
  @param[out] PortMultiplier     SATA portmultiplier number

  @retval EFI_SUCCESS   find the SATA device path
  @retval EFI_NOT_FOUND Cannot find SATA device path

**/

EFI_STATUS
FindSataDevicePort (
  IN   EFI_DEVICE_PATH_PROTOCOL              *DevicePath,
  OUT  UINT16 *Port, 
  OUT  UINT16 *PortMultiplier
  )
{
  SATA_DEVICE_PATH                     *SataDevPath;

  while (!IsDevicePathEnd (DevicePath)) {
    if (DevicePath->Type == MESSAGING_DEVICE_PATH && DevicePath->SubType == MSG_SATA_DP) {
      SataDevPath = (SATA_DEVICE_PATH *) DevicePath;
      *Port = SataDevPath->HBAPortNumber;
      *PortMultiplier = SataDevPath->PortMultiplierPortNumber;
      return EFI_SUCCESS;
    } 
    DevicePath = NextDevicePathNode (DevicePath);
  }
  
  return EFI_NOT_FOUND;
}



/**
  Get NVMe SMART log page attribute from controller identify structure

  @param[in]   NvmeDevice             Nvmexpresspassthru
  @param[in]   Nsid                   NnamespaceId
  @param[out]  Lpa                    Log page attribute

  @retval      EFI_SUCCESS            Get the attribute successfully.
  @retval      Others                 Failure to get the attribute.

**/
EFI_STATUS
EFIAPI
NVMeGetLogAttribute (
  IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL   *NvmeDevice,
  IN UINT32                               Nsid,
  OUT UINT8                               *Lpa
  )
{
  EFI_STATUS                               Status;

  
  ZeroMem (&mNVMeCommandPacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&mNVMeCommand, sizeof(EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&mNVMeCompletion, sizeof(EFI_NVM_EXPRESS_COMPLETION));
  ZeroMem (&mNVMeControllerDataBuffer, sizeof(NVME_ADMIN_CONTROLLER_DATA));

  mNVMeCommand.Cdw0.Opcode = NVME_ADMIN_IDENTIFY_CMD;
  //
  // According to Nvm Express 1.1 spec Figure 38, When not used, the field shall be cleared to 0h.
  // For the Identify command, the Namespace Identifier is only used for the Namespace data structure.
  //
  mNVMeCommand.Nsid        = 0;
  mNVMeCommandPacket.NvmeCmd        = &mNVMeCommand;
  mNVMeCommandPacket.NvmeCompletion = &mNVMeCompletion;
  mNVMeCommandPacket.TransferBuffer = (VOID*)&mNVMeControllerDataBuffer;
  mNVMeCommandPacket.TransferLength = sizeof (NVME_ADMIN_CONTROLLER_DATA);
  mNVMeCommandPacket.CommandTimeout = NVME_GENERIC_TIMEOUT;
  mNVMeCommandPacket.QueueType      = NVME_ADMIN_QUEUE;
  //
  // Set bit 0 (Cns bit) to 1 to identify a controller
  //
  mNVMeCommand.Cdw10                = 1;
  mNVMeCommand.Flags                = CDW10_VALID;

  Status = NvmeDevice->PassThru (
                               NvmeDevice,
                               0,
                               &mNVMeCommandPacket,
                               NULL
                               );
  if (!EFI_ERROR(Status)) {
    *Lpa = mNVMeControllerDataBuffer.Lpa;
    return Status;
  }
  return Status;
}


/**
  Find NVMe namespaceid from devicepath

  @param[in]   DevicePath             NVMe devicepath
  @param[out]   Nsid                  NamespaceId

  @retval      EFI_SUCCESS            Find the namespaceid
  @retval      Others                 Failured

**/

EFI_STATUS
EFIAPI
FindNVMeNsid (
  IN EFI_DEVICE_PATH_PROTOCOL    *DevicePath,    
  OUT UINT32                     *Nsid 
  )
{
  NVME_NAMESPACE_DEVICE_PATH           *NVMeDevPath;

  while (!IsDevicePathEnd (DevicePath)) {
    if (DevicePath->Type == MESSAGING_DEVICE_PATH && DevicePath->SubType == MSG_NVME_NAMESPACE_DP) {
      NVMeDevPath = (NVME_NAMESPACE_DEVICE_PATH *) DevicePath;
      *Nsid = (UINT8)NVMeDevPath->NamespaceId;
      return EFI_SUCCESS;
    } 
    DevicePath = NextDevicePathNode (DevicePath);
  }
  
  return EFI_NOT_FOUND;
}


/**
  Send SMART Return Status command to check if the execution of SMART cmd is successful or not.

  @param  PciIo               The PCI IO protocol instance.
  @param  AhciRegisters       The pointer to the EFI_AHCI_REGISTERS.
  @param  Port                The number of port.
  @param  PortMultiplier      The port multiplier port number.
  @param  AtaStatusBlock      A pointer to EFI_ATA_STATUS_BLOCK data structure.

  @retval EFI_SUCCESS          Successfully get the return status of S.M.A.R.T command execution.
                               and underthreshold
  @retval EFI_UNSUPPORTED      Can't determine the error, report SMART not support
  @retval EFI_DEVICE_ERROR     Overthreshold warning happened

**/
EFI_STATUS
EFIAPI
NVMeSmartReturnStatusCheck (
  IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL   *NvmeDevice,
  IN EFI_DEVICE_PATH_PROTOCOL    *DevicePath    
  )
{
  EFI_STATUS                               Status;
  UINT32                                   Nsid;
  UINT8                                    Lpa;
  NVME_SMART_HEALTH_INFO_LOG               *tempLog;
    
  Lpa = 0;
  Status = FindNVMeNsid(DevicePath, &Nsid);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  NVMeGetLogAttribute(NvmeDevice, Nsid, &Lpa);

  ZeroMem (&mNVMeCommandPacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&mNVMeCommand, sizeof(EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&mNVMeCompletion, sizeof(EFI_NVM_EXPRESS_COMPLETION));
  ZeroMem (&mNVMeHealthBuffer, sizeof (NVME_SMART_HEALTH_INFO_LOG));

  mNVMeCommand.Cdw0.Opcode = NVME_ADMIN_GET_LOG_PAGE_CMD;
  //
  // According to Nvm Express 1.1 spec Figure 38, When not used, the field shall be cleared to 0h.
  // For the Identify command, the Namespace Identifier is only used for the Namespace data structure.
  //
  mNVMeCommand.Nsid         =  (Lpa & BIT0) ? Nsid : 0xffffffff;
  mNVMeCommandPacket.NvmeCmd        = &mNVMeCommand;
  mNVMeCommandPacket.NvmeCompletion = &mNVMeCompletion;
  mNVMeCommandPacket.TransferBuffer = (VOID*)&mNVMeHealthBuffer;
  mNVMeCommandPacket.TransferLength = sizeof (NVME_SMART_HEALTH_INFO_LOG);
  mNVMeCommandPacket.CommandTimeout = NVME_GENERIC_TIMEOUT;
  mNVMeCommandPacket.QueueType      = NVME_ADMIN_QUEUE;
  //
  // Set log identifier to 2 for SMART/Health information
  //
  mNVMeCommand.Cdw10                = 0x02 | (((sizeof (NVME_SMART_HEALTH_INFO_LOG) / 4) - 1) << 16);;
  mNVMeCommand.Flags                = CDW10_VALID;

  Status = NvmeDevice->PassThru (
                               NvmeDevice,
                               mNVMeCommand.Nsid,
                               &mNVMeCommandPacket,
                               NULL
                               );
  if (EFI_ERROR(Status)) {
    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_PROGRESS_CODE,
      (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_NOTSUPPORTED),
      DevicePath
      );
    return EFI_UNSUPPORTED;
  }
  
  tempLog = &mNVMeHealthBuffer;
  if (
      tempLog->CriticalWarningAvailableSpare ||
      tempLog->CriticalWarningTemperature ||
      tempLog->CriticalWarningReliability ||
      tempLog->CriticalWarningMediaReadOnly ||
      tempLog->CriticalWarningVolatileBackup
      ) {
   REPORT_STATUS_CODE_WITH_DEVICE_PATH (
     EFI_PROGRESS_CODE,
     (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_OVERTHRESHOLD),
     DevicePath
     );      
   return EFI_DEVICE_ERROR;
  }

  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_UNDERTHRESHOLD),
    DevicePath
    );
  return EFI_SUCCESS;
}


/**
  Send SMART Return Status command to check if the execution of SMART cmd is successful or not.

  @param  AtaPassThru       Atapassthru instance
  @param  Port              The number of port.
  @param  PortMultiplier    The port multiplier port number.
  @param  DevicePath        devicepath of the controller 

  @retval EFI_SUCCESS          Successfully get the return status of S.M.A.R.T command execution.
                               and underthreshold
  @retval EFI_UNSUPPORTED      Can't determine the error, report SMART not support
  @retval EFI_DEVICE_ERROR     Overthreshold warning happened
  @retval EFI_INVALID_PARAMETER Support SMART but disabled

**/
EFI_STATUS
EFIAPI
AtaSmartReturnStatusCheck (
  IN EFI_ATA_PASS_THRU_PROTOCOL  *AtaPassThru,
  IN UINT16                       Port,
  IN UINT16                       PortMultiplier,
  IN EFI_DEVICE_PATH_PROTOCOL    *DevicePath    
  )
{
  EFI_STATUS              Status;

  ZeroMem (&mAcb, sizeof (EFI_ATA_COMMAND_BLOCK));
  ZeroMem (&mAtaPassThruCmdPacket, sizeof (EFI_ATA_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&mAsb, sizeof (EFI_ATA_STATUS_BLOCK));  


  mAtaPassThruCmdPacket.Acb->AtaCommand      = ATA_IDENTIFY_CMD;
  mAtaPassThruCmdPacket.InDataBuffer         = (VOID *)&mIdentifyData;
  mAtaPassThruCmdPacket.InTransferLength     = sizeof(ATA_IDENTIFY_DATA);
  mAtaPassThruCmdPacket.Protocol             = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_IN;
  mAtaPassThruCmdPacket.Length               = EFI_ATA_PASS_THRU_LENGTH_BYTES;
  Status = AtaPassThru->PassThru (
                                   AtaPassThru,
                                   Port,
                                   PortMultiplier,
                                   &mAtaPassThruCmdPacket,
                                   NULL
                                   );
  if (EFI_ERROR(Status)) {
    return RETURN_ABORTED;
  }
  
  if (!(mIdentifyData.command_set_supported_82 & BIT0)) {
    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_PROGRESS_CODE,
      (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_NOTSUPPORTED),
      DevicePath
      );
    if (!(mIdentifyData.command_set_feature_enb_85 & BIT0)) {
      REPORT_STATUS_CODE_WITH_DEVICE_PATH (
        EFI_PROGRESS_CODE,
        (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_DISABLE),
        DevicePath
        );
      return EFI_INVALID_PARAMETER;
    }
    return EFI_UNSUPPORTED;
  }

  mAcb.AtaCommand      = ATA_CMD_SMART;
  mAcb.AtaFeatures     = ATA_SMART_RETURN_STATUS;
  mAcb.AtaCylinderLow  = ATA_CONSTANT_4F;
  mAcb.AtaCylinderHigh = ATA_CONSTANT_C2;

  mAtaPassThruCmdPacket.Protocol            = EFI_ATA_PASS_THRU_PROTOCOL_ATA_NON_DATA;
  mAtaPassThruCmdPacket.Acb = &mAcb;
  mAtaPassThruCmdPacket.Asb = &mAsb;

  Status = AtaPassThru->PassThru (
                                   AtaPassThru,
                                   Port,
                                   PortMultiplier,
                                   &mAtaPassThruCmdPacket,
                                   NULL
                                   );
   
  if (!EFI_ERROR (Status) && 
      (mAsb.AtaCylinderLow == 0xf4) && (mAsb.AtaCylinderHigh == 0x2c)) {
    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_PROGRESS_CODE,
      (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_OVERTHRESHOLD),
      DevicePath
      );
    return EFI_DEVICE_ERROR;
  }

  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_UNDERTHRESHOLD),
    DevicePath
    );
  return EFI_SUCCESS;
}


/**
  Check ATA SMART error and report status

**/

VOID
CheckAtaSmartErrorAndReportStatus (
  VOID
  )
{
  EFI_STATUS                           Status;
  EFI_STATUS                           AtaStatus;
  UINTN                                HandleNum;
  EFI_HANDLE                           *AtaPassThruHandles;
  UINT8                                Index;
  UINT8                                PassthruIndex;  
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath;  
  EFI_GUID                             **ProtocolBuffer;
  UINTN                                ProtocolBufferCount;
  UINTN                                ProtocolIndex;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY  *OpenInfoBuffer;  
  UINTN                                EntryCount;
  EFI_HANDLE                          *VisitedDeviceHandles;
  UINTN                               VisitedDeviceHandlesCount;
  UINTN                               HandlesIndex;
  BOOLEAN                             FoundVisitedHandle;
  EFI_ATA_PASS_THRU_PROTOCOL          *AtaPassThru;
  UINT16                               Port;
  UINT16                               PortMultiplier;  

  
  FoundVisitedHandle = FALSE;
  DevicePath = NULL;
  VisitedDeviceHandles = NULL;    
  VisitedDeviceHandlesCount = 0;
  
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiAtaPassThruProtocolGuid,
                  NULL,
                  &HandleNum,
                  &AtaPassThruHandles
                  );

  if (EFI_ERROR (Status)) {
    return;
  }

  AtaStatus = EFI_SUCCESS;
  for (PassthruIndex = 0; PassthruIndex < HandleNum; PassthruIndex++) {
    Status = gBS->ProtocolsPerHandle (
                    AtaPassThruHandles[PassthruIndex],
                    &ProtocolBuffer,
                    &ProtocolBufferCount
                    );
    if (EFI_ERROR (Status)) {
      return;
    }


    for (ProtocolIndex = 0; ProtocolIndex < ProtocolBufferCount; ProtocolIndex++) {
      Status = gBS->OpenProtocolInformation (
                      AtaPassThruHandles[PassthruIndex],
                      ProtocolBuffer[ProtocolIndex],
                      &OpenInfoBuffer,
                      &EntryCount
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }
  
      for (Index = 0; Index < EntryCount; Index++) {
        if (!(OpenInfoBuffer[Index].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER)) {
          continue;
        }


        //
        // Check if the handle has already being checked SMART status
        //
        for (HandlesIndex = 0; HandlesIndex < VisitedDeviceHandlesCount; HandlesIndex++) {
          if (OpenInfoBuffer[Index].ControllerHandle == VisitedDeviceHandles[HandlesIndex]) {
            FoundVisitedHandle = TRUE;
            break;
          }
        }

        //
        // if found, continue with next handle
        //
        if (FoundVisitedHandle) {
          FoundVisitedHandle = FALSE;
          continue;  
        }
        

        VisitedDeviceHandles = ReallocatePool (
                                 VisitedDeviceHandlesCount * sizeof(EFI_HANDLE),
                                 (VisitedDeviceHandlesCount + 1) * sizeof(EFI_HANDLE),
                                 (VOID *)VisitedDeviceHandles);
        if (VisitedDeviceHandles == NULL) {
          continue;
        }
        VisitedDeviceHandles[VisitedDeviceHandlesCount] = OpenInfoBuffer[Index].ControllerHandle;
        VisitedDeviceHandlesCount++;
        //
        // Query all the children that opened with EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 
        //
        Status = gBS->HandleProtocol (
                        OpenInfoBuffer[Index].ControllerHandle,
                        &gEfiDevicePathProtocolGuid,
                        (VOID **) &DevicePath
                        );
        if (DevicePath != NULL) {
          Status = gBS->HandleProtocol (
                          AtaPassThruHandles[PassthruIndex],
                          &gEfiAtaPassThruProtocolGuid,
                          (VOID **) &AtaPassThru
                          );
          if (EFI_ERROR(Status)) {
            continue;
          }
          FindSataDevicePort(DevicePath, &Port, &PortMultiplier);
          Status = AtaSmartReturnStatusCheck(AtaPassThru, Port, PortMultiplier, DevicePath);
        }        
        
      }

      if (VisitedDeviceHandles != NULL) {
        FreePool (VisitedDeviceHandles);
      }
      
      if (OpenInfoBuffer != NULL) {
        FreePool (OpenInfoBuffer);
      }
    }

    if (ProtocolBuffer != NULL) {
      FreePool (ProtocolBuffer);
    }
  }

  if (AtaPassThruHandles != NULL) {
    FreePool (AtaPassThruHandles);  
  }
}

/**
  Check ATA SMART error and report status

**/

VOID
CheckNVMeSmartErrorAndReportStatus (
  VOID
  )
{
  EFI_STATUS                           Status;
  UINTN                                HandleNum;
  EFI_HANDLE                           *NVMePassThruHandles;
  UINTN                                Index;
  UINTN                                PassthruIndex;    
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath;  
  EFI_GUID                             **ProtocolBuffer;
  UINTN                                ProtocolBufferCount;
  UINTN                                ProtocolIndex;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY  *OpenInfoBuffer;  
  UINTN                                EntryCount;
  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL   *NvmeDevice;
  UINTN                               HandlesIndex;
  BOOLEAN                             FoundVisitedHandle;
  EFI_HANDLE                          *VisitedDeviceHandles;
  UINTN                               VisitedDeviceHandlesCount;
  
  
  FoundVisitedHandle = FALSE;
  DevicePath = NULL;
  VisitedDeviceHandles = NULL;    
  VisitedDeviceHandlesCount = 0;

  
  DevicePath = NULL;
    
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiNvmExpressPassThruProtocolGuid,
                  NULL,
                  &HandleNum,
                  &NVMePassThruHandles
                  );

  if (EFI_ERROR (Status)) {
    return;
  }

  for (PassthruIndex = 0; PassthruIndex < HandleNum; PassthruIndex++) {
    Status = gBS->ProtocolsPerHandle (
                    NVMePassThruHandles[PassthruIndex],
                    &ProtocolBuffer,
                    &ProtocolBufferCount
                    );
    if (EFI_ERROR (Status)) {
      return;
    }

    for (ProtocolIndex = 0; ProtocolIndex < ProtocolBufferCount; ProtocolIndex++) {
      Status = gBS->OpenProtocolInformation (
                      NVMePassThruHandles[PassthruIndex],
                      ProtocolBuffer[ProtocolIndex],
                      &OpenInfoBuffer,
                      &EntryCount
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }
  
      for (Index = 0; Index < EntryCount; Index++) {
        if (!(OpenInfoBuffer[Index].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER)) {
          continue;
        }


        //
        // Check if the handle has already being checked SMART status
        //
        for (HandlesIndex = 0; HandlesIndex < VisitedDeviceHandlesCount; HandlesIndex++) {
          if (OpenInfoBuffer[Index].ControllerHandle == VisitedDeviceHandles[HandlesIndex]) {
            FoundVisitedHandle = TRUE;
            break;
          }
        }

        //
        // if found, continue with next handle
        //
        if (FoundVisitedHandle) {
          FoundVisitedHandle = FALSE;
          continue;  
        }
        

        VisitedDeviceHandles = ReallocatePool (
                                VisitedDeviceHandlesCount * sizeof(EFI_HANDLE),
                                (VisitedDeviceHandlesCount + 1) * sizeof(EFI_HANDLE),
                                (VOID *)VisitedDeviceHandles);
        if (VisitedDeviceHandles == NULL) {
          continue;
        }
        VisitedDeviceHandles[VisitedDeviceHandlesCount] = OpenInfoBuffer[Index].ControllerHandle;
        VisitedDeviceHandlesCount++;
        //
        // Query all the children that opened with EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 
        //
        Status = gBS->HandleProtocol (
                        OpenInfoBuffer[Index].ControllerHandle,
                        &gEfiDevicePathProtocolGuid,
                        (VOID **) &DevicePath
                        );
        if (DevicePath != NULL) {
          Status = gBS->HandleProtocol (
                          NVMePassThruHandles[PassthruIndex],
                          &gEfiNvmExpressPassThruProtocolGuid,
                          (VOID **) &NvmeDevice
                          );
          if (EFI_ERROR(Status)) {
            continue;
          }
          Status = NVMeSmartReturnStatusCheck(NvmeDevice, DevicePath);
        }        
        
      }
      if (VisitedDeviceHandles != NULL) {
        FreePool (VisitedDeviceHandles);
      }
      
      if (OpenInfoBuffer != NULL) {
        FreePool (OpenInfoBuffer);
      }
    }

    if (ProtocolBuffer != NULL) {
      FreePool (ProtocolBuffer);
    }
  }

  if (NVMePassThruHandles != NULL) {
    FreePool (NVMePassThruHandles);  
  }

}


/**

  Check SMART status and report status for NVMe and SATA
  @param[in]        Event               Pointer to this event
  @param[in]        Handle              The handle associated with a previously registered checkpoint handler.

**/
VOID
H2OBdsCpBootDeviceSelectCallback (
  IN EFI_EVENT                              Event,
  IN H2O_CP_HANDLE                          Handle
  )
{
  H2OCpUnregisterHandler (Handle);
  CheckAtaSmartErrorAndReportStatus ();
  CheckNVMeSmartErrorAndReportStatus ();
}


/**
  Entry point of Smart Status Code Driver.

  This function is the entry point of this DXE Status Code Driver.
  It initializes registers status code handlers, and registers event for EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
SmartStatusCodeDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                          Status;

  if (FeaturePcdGet (PcdH2OBdsCpBootDeviceSelectSupported)) {
    H2O_CP_HANDLE                         H2OBdsCpBootDeviceSelectHandle;
    //
    // Register for callback on Storage Security Protocol publication
    //
    H2OBdsCpBootDeviceSelectHandle = NULL;
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpBootDeviceSelectGuid,
               H2OBdsCpBootDeviceSelectCallback,
               SMART_STATUS_CHECKPOINT_NOTIFY_TPL,
               &H2OBdsCpBootDeviceSelectHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG_CP ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpBootDeviceSelectGuid, Status));
      return Status;
    }
    DEBUG_CP ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpBootDeviceSelectGuid, Status));
  }    

  return EFI_SUCCESS;
}
