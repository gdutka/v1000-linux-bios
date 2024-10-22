/** @file
  InstallVerbTablePei peim will produces the InstallVerbTable Ppi for Pei driver.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <InstallVerbTablePei.h>
#include <Ppi/AzaliaVerbTable.h>
#include <Guid/AzaliaPolicy.h>
#include <MmioAccess.h>
#include <Library/PciExpressLib.h>
#include <Library/KernelConfigLib.h>
#include <Ppi/ReadOnlyVariable2.h>
//
// Define PCD Value
//
#define pPcdAzaliaBus PcdGet8(PcdAzaliaBus)
#define pPcdAzaliaDev PcdGet8(PcdAzaliaDev)
#define pPcdAzaliaFunc PcdGet8(PcdAzaliaFunc)
#define pPcdAcpiMmioBase PcdGet32(PcdAcpiMmioBase)
#define pPcdGpioBase PcdGet32(PcdGpioBase)
#define pPcdPmioBase PcdGet32(PcdPmioBase)
#define pPcdIomuxBase PcdGet32(PcdIomuxBase)

EFI_STATUS
EFIAPI
DispatchInstallVerbTablePpi (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR   mNotifyVerbPpi = {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gPeiInstallAzaliaVerbTablePpiGuid,
    DispatchInstallVerbTablePpi
};
//
// Ppi Definition
//
STATIC
PEI_VERB_TABLE_PPI mVerbTablePpi = {
  InstallVerbTable
  };

STATIC
EFI_PEI_PPI_DESCRIPTOR mInstallVerbTablePpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiInstallVerbTablePpiGuid,
  &mVerbTablePpi
  };

/**
  Install InstallVerbTable PPI.

  @param      PeiServices             Pointer's pointer to EFI_PEI_SERVICES.

  @retval     EFI_SUCCESS 

**/
EFI_STATUS
InstallVerbTablePpi (
  IN CONST EFI_PEI_SERVICES                      **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = (**PeiServices).InstallPpi (PeiServices, &mInstallVerbTablePpi);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "\n\nInstall VerbTable PPI : %r \n", Status));
  }
  
  return Status;
}

/**
  A PEIM for install InstallVerbTable PPI.

  @param      FileHandle              Handle of the file being invoked.
  @param      PeiServices             Pointer to PEI Services table.

  @retval     EFI_SUCCESS 

**/
EFI_STATUS
PeimInstallVerbTableInitEntry (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                             Status;
  PEI_VERB_TABLE_PPI                     *VerbTablePpi;
  EFI_PEI_PPI_DESCRIPTOR                 *VerbTablePpiDescriptor;
  PEI_Install_AZALIA_VERB_TALBE_PPI      *InstallAzaliaVerbtable;
  //
  // Register so it will be automatically shadowed to memory
  //
  Status = PeiServicesRegisterForShadow (FileHandle);

  if (Status == EFI_ALREADY_STARTED) {
    //
    // Great!! Now that module is in memory,update the PPI 
    // that decribes verb table installation.
    //
    Status = (**PeiServices).LocatePpi (
                               (CONST EFI_PEI_SERVICES**)PeiServices, 
                               &gPeiInstallVerbTablePpiGuid,
                               0,
                               &VerbTablePpiDescriptor,
                               (VOID**)&VerbTablePpi
                               );

    //
    // Reinstall the PPI Interface using the memory-based descriptor
    //
    if (!EFI_ERROR (Status)) {
      //
      // Only reinstall the "VerbTable" PPI
      //
      Status = (**PeiServices).ReInstallPpi (
                                 PeiServices,
                                 VerbTablePpiDescriptor,
                                 &mInstallVerbTablePpi
                                 );

      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Reinstall Verb Table PPI To Memory is failed !\n"));
      }

      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Reinstall Verb Table PPI To Memory is successful !\n"));
    }

    return Status;
  } else if (!EFI_ERROR (Status)) {  

    Status = InstallVerbTablePpi (PeiServices);

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Publish the notification of Verb Table PPI is failed !\n"));
    }
    
  }
  Status = PeiServicesLocatePpi (
             &gPeiInstallAzaliaVerbTablePpiGuid,
             0,
             NULL,
             (VOID **)&InstallAzaliaVerbtable
             );
  if (Status == EFI_SUCCESS) {
    ConfigureAzalia (PeiServices);
  } else {
     //
     // Notify event
     //
     Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyVerbPpi);

    }
  return Status;
}

/**
  Search Codec and Initialize them by verb table installation.

  @param   PeiServices             Pointer's pointer to EFI_PEI_SERVICES
  @param   HdaBar                  The MMIO base address of High Definition Audio (Azalia) Controller.
  @param   OemVerbTableAddress     A Pointer to OEM defined verb table data.
  
  @retval  EFI_SUCCESS 
  
**/
EFI_STATUS
InstallVerbTable (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN COMMON_CHIPSET_AZALIA_VERB_TABLE      *OemVerbTableAddress
  )
{
  EFI_STATUS                               Status;
  UINT8                                    Index;
  UINT8                                    CodecNum;
  UINT16                                   SdinWake;
  UINT32                                   SdinBit;
  UINT32                                   CodecAddress;
  UINT32                                   DeliverCommandData;
  UINT32                                   ResponseData;
  UINT32                                   VendorDeviceId;
  UINT32                                   RevisionId;
  UINT32                                   JackIndex;
  UINT32                                   TotalJackNum;
  UINT32                                   *VerbTableDataBuffer;
  EFI_PEI_STALL_PPI                        *StallPpi;
  COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  *VerbTableHeaderBuffer;
  COMMON_CHIPSET_AZALIA_VERB_TABLE         *VerbTableHeaderDataAddress;

  SdinBit                    = HDA_SDIN_INIT;
  SdinWake                   = HDA_NO_CODEC;
  VerbTableHeaderDataAddress = NULL;

  //
  // If no verb table data updated, it will load default Verb table data.
  //
  if (OemVerbTableAddress != NULL) {
    VerbTableHeaderDataAddress = OemVerbTableAddress;
  } else {
    //
    // OemServices
    //
    Status = OemSvcGetVerbTable (
               &VerbTableHeaderDataAddress
               );

    if (!EFI_ERROR (Status)) {
      return EFI_SUCCESS;
    }
  }
  if (VerbTableHeaderDataAddress == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (VerbTableHeaderDataAddress[0].VerbTableHeader == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = (**PeiServices).LocatePpi (
                             (CONST EFI_PEI_SERVICES**)PeiServices, 
                             &gEfiPeiStallPpiGuid,
                             0,
                             NULL,
                             (VOID**)&StallPpi
                             );
  
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "To locate EFI_PEI_STALL_PPI is failed !\n"));
    return Status;
  } 
  
  if (!(MmioRead32 (HdaBar + HDA_REG_GCTL) & HDA_REG_GCTL_BIT_CRST)) {
    //
    // If Hda had been initialized, system don't need to init the controller.
    //
    
    //
    // To initialize HDA.
    //
    Status = HdaInitialize ((CONST EFI_PEI_SERVICES**)PeiServices, HdaBar, StallPpi);

    if (EFI_ERROR (Status)) {
      //
      // Can't initialize HDA controller.
      //
      goto ExitCodecInitialize;
    }

    Status = PeiCsSvcHdaInitHook (HdaBar);
    
    if (EFI_ERROR (Status)) {
      //
      // Can't initialize HDA controller.
      //
      goto ExitCodecInitialize;
    }

    //
    // Clear State Change Status (STATESTS reg.)
    //
    MmioOr16 (HdaBar + HDA_REG_STATESTS, HDA_MAX_SDIN_FLG);
    
    //
    // To reset HDA.
    //
    Status = HdaReset ((CONST EFI_PEI_SERVICES**)PeiServices, HdaBar, StallPpi);
  
    if (EFI_ERROR (Status)) {
      //
      // Can't reset HDA controller.
      //
      goto ExitCodecInitialize;
    }
  
    //
    // To initialize HDA controller.
    //
    Status = HdaInitialize ((CONST EFI_PEI_SERVICES**)PeiServices, HdaBar, StallPpi);
  
    if (EFI_ERROR (Status)) {
      //
      // Can't initialize HDA controller.
      //
      goto ExitCodecInitialize;
    }
  }

  if (MmioRead32 (HdaBar + HDA_REG_GCTL) & HDA_REG_GCTL_BIT_CRST) {
    Status = EFI_SUCCESS;
  } else {
    //
    // Retry to init HDA.
    //
    MmioOr32 (HdaBar + HDA_REG_GCTL, HDA_REG_GCTL_BIT_CRST);
    Status = StallPpi->Stall ((CONST EFI_PEI_SERVICES**)PeiServices, StallPpi, HDA_INIT_FAIL_RETRY_STALL_TIME);
  }
  
  //
  // To find out all codec through identification of the state change status
  // "STATESTS" bits.
  //
  if (MmioRead32 (HdaBar + HDA_REG_GCTL) & HDA_REG_GCTL_BIT_CRST) {
    Status = FindOutCodec ((CONST EFI_PEI_SERVICES**)PeiServices, HdaBar, StallPpi, &SdinWake);
  }
    
  if (EFI_ERROR (Status)) {
    //
    // No Codec be detected.
    //
    goto ExitCodecInitialize;
  }
  
  for (CodecNum = 0; CodecNum < HDA_MAX_SDIN_NUM; CodecNum++, SdinBit <<= 1) {
    //
    // To confirm every SDIN if there is a codec on link.
    //
    if ((SdinWake & SdinBit) != SdinBit) {
      //
      // To search the next SDIN to detect Codec.
      //
      continue;
    }

    // Verb Layout Format:
    //
    //  31    28 27 26      20 19                           0
    //  -----------------------------------------------------
    //  |  Cad  | I |   NID   |     Verb & Command data     |
    //  -----------------------------------------------------
    //           ^^^
    //            0 = Direct NID reference
    //            1 = Indirect NID reference
    // Note : when to use controls and parameters through this Format.

    //
    // To read the Codec's vendor and device ID.
    //
    CodecAddress = (UINT32)CodecNum;
    DeliverCommandData = ((CONTROL_GET_PARAMETER << 8) | PARAMETER_VENDOR_DEVICE_ID) | (CodecAddress << 28);
    Status = DeliverCommandToCodec ((CONST EFI_PEI_SERVICES**)PeiServices, HdaBar, StallPpi, DeliverCommandData);
    Status = ReceiveCodecData ((CONST EFI_PEI_SERVICES**)PeiServices, HdaBar, StallPpi, &ResponseData);

    if (EFI_ERROR (Status)) {
      goto ExitCodecInitialize;
    }
    
    VendorDeviceId = ResponseData;

    //
    // To read the Codec's revision ID.
    //
    DeliverCommandData = ((CONTROL_GET_PARAMETER << 8) | PARAMETER_REVISION_ID) | (CodecAddress << 28);
    Status = DeliverCommandToCodec ((CONST EFI_PEI_SERVICES**)PeiServices, HdaBar, StallPpi, DeliverCommandData);
    Status = ReceiveCodecData ((CONST EFI_PEI_SERVICES**)PeiServices, HdaBar, StallPpi, &ResponseData);

    if (EFI_ERROR (Status)) {
      goto ExitCodecInitialize;
    }
    
    RevisionId = (ResponseData >> 8) & 0xFF;

    //
    // Start to program codec with verb table.
    //

    for (Index = 0; VerbTableHeaderDataAddress[Index].VerbTableHeader != NULL; Index++) {
      VerbTableHeaderBuffer = VerbTableHeaderDataAddress[Index].VerbTableHeader;
      
      if ((VerbTableHeaderBuffer->VendorDeviceId == VendorDeviceId) && 
          ((VerbTableHeaderBuffer->RevisionId == RevisionId) || 
           (VerbTableHeaderBuffer->RevisionId == 0xFF))) {
        TotalJackNum = VerbTableHeaderBuffer->NumberOfFrontJacks + 
                       VerbTableHeaderBuffer->NumberOfRearJacks;

        VerbTableDataBuffer = VerbTableHeaderDataAddress[Index].VerbTableData;
        
        for (JackIndex = 0; JackIndex < (TotalJackNum * 4); JackIndex++) {
          //
          // To send command data to Codec for all Jacks.(Front and Rear)
          //
          DeliverCommandData = VerbTableDataBuffer[JackIndex] | (CodecAddress << 28);
          Status = DeliverCommandToCodec ((CONST EFI_PEI_SERVICES**)PeiServices, HdaBar, StallPpi, DeliverCommandData);

          if (EFI_ERROR (Status)) {
            goto ExitCodecInitialize;
          }
        }
      }
    }

    Status = EFI_SUCCESS;
  }

  return Status;

ExitCodecInitialize:
  MmioAnd32 (HdaBar + HDA_REG_GCTL, (UINTN)~HDA_REG_GCTL_BIT_CRST);
  
  return Status;
}


/**
  Hda initialize itself.
  
  @param      PeiServices    Pointer's pointer to EFI_PEI_SERVICES
  @param      HdaBar         The MMIO base address of High Definition Audio Controller.
  @param      StallPpi       A pointer to PEI_STALL_PPI.
  
  @retval     EFI_SUCCESS 

**/
EFI_STATUS
HdaInitialize (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi
  )
{
  UINTN       LoopCounter;
  EFI_STATUS  Status;

  LoopCounter = TIME_OUT_LOOP_INIT;
  
  //
  // Let controller initializes itself. Turn on link and let state machines initialize themselves.
  // Until CRST bit be read back as 1, controller has initialized itself and registers accept to 
  // be writen.
  //
  MmioOr32 (HdaBar + HDA_REG_GCTL, HDA_REG_GCTL_BIT_CRST);

  do {
    if (MmioRead32 (HdaBar + HDA_REG_GCTL) & HDA_REG_GCTL_BIT_CRST) {
      break;
    } else{
      Status = StallPpi->Stall (PeiServices, StallPpi, STALL_TIME);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      LoopCounter++;
    }
  } while (LoopCounter != TIME_OUT_MAX_LOOP);

  if (LoopCounter == TIME_OUT_MAX_LOOP) {
    //
    // Can't initialize HDA.
    //
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}


/**
  Hda reset itself.
  
  @param      PeiServices    Pointer's pointer to EFI_PEI_SERVICES
  @param      HdaBar         The MMIO base address of High Definition Audio Controller.
  @param      StallPpi       A pointer to PEI_STALL_PPI.
  
  @retval     EFI_SUCCESS 

**/
EFI_STATUS
HdaReset (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi
  )
{
  UINTN       LoopCounter;
  EFI_STATUS  Status;

  LoopCounter = TIME_OUT_LOOP_INIT;
  
  //
  // Let controller in reset and turn off link.This step is a goal to reset HDA controller.
  //
  MmioAnd32 (HdaBar + HDA_REG_GCTL, (UINTN)~HDA_REG_GCTL_BIT_CRST);

  do {
    if (MmioRead32 (HdaBar + HDA_REG_GCTL) & HDA_REG_GCTL_BIT_CRST) {
      Status = StallPpi->Stall (PeiServices, StallPpi, STALL_TIME);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      LoopCounter++;
    } else{
      break;
    }
  } while (LoopCounter != TIME_OUT_MAX_LOOP);

  if (LoopCounter == TIME_OUT_MAX_LOOP) {
    //
    // Can't reset HDA.
    //
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}


/**
  To find out all Codec.

  @param      PeiServices    Pointer's pointer to EFI_PEI_SERVICES
  @param      HdaBar         The MMIO base address of High Definition Audio Controller.
  @param      StallPpi       A pointer to PEI_STALL_PPI.
  @param      SdinWake       A pointer to the data of register "STATESTS"
  
  @retval     EFI_SUCCESS 

**/
EFI_STATUS
FindOutCodec (
  IN     CONST  EFI_PEI_SERVICES                  **PeiServices,
  IN     UINT32                            HdaBar,
  IN     EFI_PEI_STALL_PPI                 *StallPpi,
  IN OUT UINT16                            *SdinWake
  )
{
  UINTN       LoopCounter;
  EFI_STATUS  Status;

  LoopCounter = TIME_OUT_LOOP_INIT;
  
  do {
    *SdinWake = MmioRead16 (HdaBar + HDA_REG_STATESTS) & HDA_MAX_SDIN_FLG;
    if (*SdinWake != HDA_NO_CODEC) {
      break;
    } else{
      Status = StallPpi->Stall (PeiServices, StallPpi, DETECT_CODEC_STALL_TIME);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      LoopCounter++;
    }
  } while (LoopCounter != DETECT_CODEC_TIME_OUT_MAX_LOOP);

  if ((LoopCounter == DETECT_CODEC_TIME_OUT_MAX_LOOP) || (*SdinWake == HDA_NO_CODEC)) {
    //
    // No Codec be found.
    //
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}


/**
  To deliver command data in ICW register to deliver to Codec.

  @param      PeiServices  -  Pointer's pointer to EFI_PEI_SERVICES
  @param      HdaBar       -  The MMIO base address of High Definition Audio Controller.
  @param      StallPpi     -  A pointer to PEI_STALL_PPI.
  @param      CommandData  -  The command data.

  @retval     EFI_SUCCESS 

**/
EFI_STATUS
DeliverCommandToCodec (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi,
  IN UINT32                                CommandData
  )
{
  UINTN       LoopCounter;
  EFI_STATUS  Status;

  LoopCounter = TIME_OUT_LOOP_INIT;

  do {
    if (MmioRead16 (HdaBar + HDA_REG_ICS) & HDA_REG_ICS_BIT_ICB) {
      Status = StallPpi->Stall (PeiServices, StallPpi, STALL_TIME);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      LoopCounter++;
    } else {
      break;
    }
  } while (LoopCounter != TIME_OUT_MAX_LOOP);

  if (LoopCounter == TIME_OUT_MAX_LOOP) {
    return EFI_TIMEOUT;
  }

  MmioOr16 (HdaBar + HDA_REG_ICS, HDA_REG_ICS_BIT_IRV);
  MmioWrite32 ((HdaBar + HDA_REG_ICW), CommandData);
  MmioOr16 (HdaBar + HDA_REG_ICS, HDA_REG_ICS_BIT_ICB);
  
  return EFI_SUCCESS;
}


/**
  To receive response data from Codec.

  @param      FPeiServices   -  Pointer's pointer to EFI_PEI_SERVICES
  @param      PHdaBar        -  The MMIO base address of High Definition Audio Controller.
  @param      StallPpi      -  A pointer to PEI_STALL_PPI.
  @param      ResponseData  -  data be received from Codec.

  @retval     EFI_SUCCESS 

**/
EFI_STATUS
ReceiveCodecData (
  IN     CONST EFI_PEI_SERVICES                  **PeiServices,
  IN     UINT32                            HdaBar,
  IN     EFI_PEI_STALL_PPI                 *StallPpi,
  IN OUT UINT32                            *ResponseData
  )
{
  UINTN       LoopCounter;
  EFI_STATUS  Status;

  LoopCounter = TIME_OUT_LOOP_INIT;

  do {
    if (MmioRead16 (HdaBar + HDA_REG_ICS) & HDA_REG_ICS_BIT_IRV) {
      break;
    } else {
      Status = StallPpi->Stall (PeiServices, StallPpi, STALL_TIME);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      LoopCounter++;
    }
  } while (LoopCounter != TIME_OUT_MAX_LOOP);

  if (LoopCounter == TIME_OUT_MAX_LOOP) {
    return EFI_TIMEOUT;
  }

  *ResponseData = MmioRead32 (HdaBar + HDA_REG_IRR);

  return EFI_SUCCESS;
}

/**
 Initialize the High Definition Audio Codec(s) present in the system.
 For each codec, a predefined codec verb table should be programmed.
 The list contains 32-bit verbs to be sent to the corresponding codec.
 If it is not programmed, the codec uses the default verb table, which may or may not
 correspond to the platform jack information.

 @param [in]        PeiServices         Describes the list of possible PEI Services
 @param [in]        AzaliaPolicy        The Azalia Policy data

 @retval            EFI_SUCCESS         Codec is initialized successful by verb table installation.
                    EFI_DEVICE_ERROR    Azalia device has a problem.
                    EFI_UNSUPPORTED     VerbTable PPI doesn't be installed.
                    Other               Return value from VerbTablePpi.
*/
EFI_STATUS
DetectAndInitializeAzalia (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN       AZALIA_POLICY            *AzaliaPolicy
  )
{
  UINT8                             Value;
  UINT32                            RegBAR0;
  EFI_STATUS                        Status;
  PEI_VERB_TABLE_PPI                *VerbTablePpi;
  
  Value = 0;
  VerbTablePpi = NULL;

  MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdPmioBase + FCH_PMIOA_REGEB, (UINT8) ~BIT0, (UINT8) BIT0);

  PciExpressAndThenOr8 (
    PCI_EXPRESS_LIB_ADDRESS(pPcdAzaliaBus, pPcdAzaliaDev, pPcdAzaliaFunc, FCH_AZ_REG04),
    (UINT8) ~BIT1,
    (UINT8) BIT1
    );

  //
  // To set the MMIO base address of HDA (azalia) controller.
  //
  PciExpressWrite32 (
    PCI_EXPRESS_LIB_ADDRESS(pPcdAzaliaBus, pPcdAzaliaDev, pPcdAzaliaFunc, FCH_AZ_REG10),
    FixedPcdGet32 (PcdHdaBaseAddress)
    );

  RegBAR0 = PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS(pPcdAzaliaBus, pPcdAzaliaDev, pPcdAzaliaFunc, FCH_AZ_REG10));
  if ((RegBAR0 != 0) && (RegBAR0 != 0xFFFFFFFF)) {
    RegBAR0 &=  ~(0x03FFF);
  } else {
    PciExpressAndThenOr16 (
      PCI_EXPRESS_LIB_ADDRESS(pPcdAzaliaBus, pPcdAzaliaDev, pPcdAzaliaFunc, FCH_AZ_REG04),
      0,
      0);
    return EFI_DEVICE_ERROR;
  }

  // Get SDIN Configuration
  if (AzaliaPolicy->AZALIACONFIGINFO.AzaliaConfig.AzaliaSdin0 == 2 ) {
    //RWMEM (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG167, AccWidthUint8, 0, 0x3E);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdGpioBase  + FCH_GPIO_REG167, 0, 0x3E);
    //RWMEM (ACPI_MMIO_BASE + pPcdIomuxBase + FCH_GPIO_REG167, AccWidthUint8, 0, 0x00);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdIomuxBase + FCH_GPIO_REG167, 0, 0x00);
  } else {
    //RWMEM (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG167, AccWidthUint8, 0, 0x0);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdGpioBase  + FCH_GPIO_REG167, 0, 0x00);
    //RWMEM (ACPI_MMIO_BASE + pPcdIomuxBase + FCH_GPIO_REG167, AccWidthUint8, 0, 0x01);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdIomuxBase + FCH_GPIO_REG167, 0, 0x01);
  }
  if (AzaliaPolicy->AZALIACONFIGINFO.AzaliaConfig.AzaliaSdin1 == 2 ) {
    //RWMEM (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG168, AccWidthUint8, 0, 0x3E);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdGpioBase  + FCH_GPIO_REG168, 0, 0x3E);
    //RWMEM (ACPI_MMIO_BASE + pPcdIomuxBase + FCH_GPIO_REG168, AccWidthUint8, 0, 0x00);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdIomuxBase + FCH_GPIO_REG168, 0, 0x00);
  } else {
    //RWMEM (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG168, AccWidthUint8, 0, 0x0);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdGpioBase  + FCH_GPIO_REG168, 0, 0x00);
    //RWMEM (ACPI_MMIO_BASE + pPcdIomuxBase + FCH_GPIO_REG168, AccWidthUint8, 0, 0x01);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdIomuxBase + FCH_GPIO_REG168, 0, 0x01);
  }
  if (AzaliaPolicy->AZALIACONFIGINFO.AzaliaConfig.AzaliaSdin2 == 2 ) {
    //RWMEM (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG169, AccWidthUint8, 0, 0x3E);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdGpioBase  + FCH_GPIO_REG169, 0, 0x3E);
    //RWMEM (ACPI_MMIO_BASE + pPcdIomuxBase + FCH_GPIO_REG169, AccWidthUint8, 0, 0x00);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdIomuxBase + FCH_GPIO_REG169, 0, 0x00);
  } else {
    //RWMEM (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG169, AccWidthUint8, 0, 0x0);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdGpioBase  + FCH_GPIO_REG169, 0, 0x00);
    //RWMEM (ACPI_MMIO_BASE + pPcdIomuxBase + FCH_GPIO_REG169, AccWidthUint8, 0, 0x01);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdIomuxBase + FCH_GPIO_REG169, 0, 0x01);
  }
  if (AzaliaPolicy->AZALIACONFIGINFO.AzaliaConfig.AzaliaSdin3 == 2 ) {
    //RWMEM (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG170, AccWidthUint8, 0, 0x3E);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdGpioBase  + FCH_GPIO_REG170, 0, 0x3E);
    //RWMEM (ACPI_MMIO_BASE + pPcdIomuxBase + FCH_GPIO_REG170, AccWidthUint8, 0, 0x00);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdIomuxBase + FCH_GPIO_REG170, 0, 0x00);
  } else {
    //RWMEM (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG170, AccWidthUint8, 0, 0x0);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdGpioBase  + FCH_GPIO_REG170, 0, 0x00);
    //RWMEM (ACPI_MMIO_BASE + pPcdIomuxBase + FCH_GPIO_REG170, AccWidthUint8, 0, 0x01);
    MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdIomuxBase + FCH_GPIO_REG170, 0, 0x01);
  }

  // INT#A Azalia resource
  Value = 0x93; // Azalia APIC index
  //WriteIO (FCH_IOMAP_REGC00, AccWidthUint8, &Data);
  (**PeiServices).CpuIo->IoWrite8 (PeiServices, ((**PeiServices).CpuIo), FCH_IOMAP_REGC00, Value);
  Value = 0x10; // IRQ16 (INTA#)
  //WriteIO (FCH_IOMAP_REGC01, AccWidthUint8, &Data);
  (**PeiServices).CpuIo->IoWrite8 (PeiServices, ((**PeiServices).CpuIo), FCH_IOMAP_REGC01, Value);

  //
  // To program codec by verb table installation.
  //
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gPeiInstallVerbTablePpiGuid,
                            0,
                            NULL,
                            (VOID**)&VerbTablePpi
                            );

  if (EFI_ERROR (Status)) {
    PciExpressAndThenOr16 (
      PCI_EXPRESS_LIB_ADDRESS(pPcdAzaliaBus, pPcdAzaliaDev, pPcdAzaliaFunc, FCH_AZ_REG04),
      0,
      0
      );
    return EFI_UNSUPPORTED;
  }

  Status = VerbTablePpi->InstallVerbTable ((EFI_PEI_SERVICES**)PeiServices, RegBAR0, NULL);
  if (EFI_ERROR (Status)) {
    PciExpressAndThenOr16 (
      PCI_EXPRESS_LIB_ADDRESS(pPcdAzaliaBus, pPcdAzaliaDev, pPcdAzaliaFunc, FCH_AZ_REG04),
      0,
      0
      );
    return Status;
  }
  if (AzaliaPolicy->AzaliaSnoop == 1 ) {
    PciExpressAndThenOr8 (
      PCI_EXPRESS_LIB_ADDRESS(pPcdAzaliaBus, pPcdAzaliaDev, pPcdAzaliaFunc, FCH_AZ_REG42),
      0xFF,
      (UINT8) (BIT1 + BIT0)
      );
  }

  return EFI_SUCCESS;
}

/**
 Disable Azalia Controler

 @param [in]        None

 @retval            None
*/
VOID
DisableAzaliaControler (
  VOID
  )
{
  PciExpressAndThenOr16 (
    PCI_EXPRESS_LIB_ADDRESS(pPcdAzaliaBus, pPcdAzaliaDev, pPcdAzaliaFunc, FCH_AZ_REG04),
    0,
    0
    );
  MmioAndThenOr8 (pPcdAcpiMmioBase + pPcdPmioBase  + FCH_PMIOA_REGEB, (UINT8) ~BIT0, 0);
}

/**
 Initialize the High Definition Audio Codec(s) present in the system.
 For each codec, a predefined codec verb table should be programmed.
 The list contains 32-bit verbs to be sent to the corresponding codec.
 If it is not programmed, the codec uses the default verb table, which may or may not
 correspond to the platform jack information.

 @param [in]        PeiServices         Describes the list of possible PEI Services

 @retval            EFI_SUCCESS         Codec is initialized successful by verb table installation or Azalia is disabled
                                        by Setup Utility.
                    EFI_ABORTED         It can't get Azalia's policy.
                    Other               Return value from DetectAndInitializeAzalia function.
*/
EFI_STATUS
ConfigureAzalia (
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                        Status;
  UINTN                             VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *Variable;
  AZALIA_POLICY                     AzaliaPolicy;
  KERNEL_CONFIGURATION              KernelConfiguration;

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gEfiPeiReadOnlyVariable2PpiGuid,
                             0,
                             NULL,
                             (VOID**)&Variable
                             );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return EFI_ABORTED;
  }

  VariableSize = sizeof (AZALIA_POLICY);
  Status = Variable->GetVariable (
                        Variable,
                        AZALIA_POLICY_VARIABLE,
                        &gAzaliaPolicyGuid,
                        NULL,
                        &VariableSize,
                        &AzaliaPolicy
                        );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetKernelConfiguration (&KernelConfiguration);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return EFI_ABORTED;
  }

  AzaliaPolicy.AzaliaController = KernelConfiguration.Azalia;

  if (AzaliaPolicy.AzaliaController == AZALIA_ENABLE) {
    Status = DetectAndInitializeAzalia (PeiServices, &AzaliaPolicy);
  } else {
    DisableAzaliaControler ();
    return EFI_SUCCESS;
  }

  return Status;
}

EFI_STATUS
EFIAPI
DispatchInstallVerbTablePpi (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                        Status;
  
  Status = ConfigureAzalia ((CONST EFI_PEI_SERVICES**)PeiServices);
  
  return Status;
}

