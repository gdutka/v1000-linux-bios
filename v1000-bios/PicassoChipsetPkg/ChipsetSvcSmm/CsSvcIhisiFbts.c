/** @file
 Implement the Chipset Servcie IHISI FBTS subfunction for this driver.

***************************************************************************
* Copyright (c) 2014-2015, Insyde Software Corp. All Rights Reserved.
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
#include <Library/IoLib.h>
#include <Library/FdSupportLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseOemSvcKernelLib.h>
#include <CsSvcIhisiFbts.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/SmmOemSvcChipsetLib.h>
#include <Library/BaseOemSvcKernelLib.h>
#include <Library/IoLib.h>
#include <Library/FchDxeLib.h>
#include <Protocol/SmmFwBlockService.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/H2OIhisiFbtsToolVersionProtocol.h>
#include <SecureFlash.h>
#include <Library/VariableLib.h>

#include <FchRegs.h>
#include <SetupConfig.h>

//[-start-190319-IB06070016-add]//
#include <Library/ProcessPendingCapsuleLib.h>
//[-end-190319-IB06070016-add]//

#define FBTSAPHOOK_SUPPORT        204

#define RTC_ADDRESS_REGISTER_B      0x0B
#define   ALARM_INTERRUPT_ENABLE      0x20
#define PM1_EN_HIGH_BYTE            0x03
#define   RTC_EVENT_ENABLE            0x04
#define PCAT_RTC_ADDRESS_REGISTER   0x70
#define PCAT_RTC_DATA_REGISTER      0x71

BOOLEAN   EcReinitRequired = FALSE;
extern EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *mSmmFwBlockService;

FBTS_PLATFORM_ROM_MAP mOemRomMap[] = {
//
//EC SHARED FLASH SUPPORT
//
//  {FbtsRomMapEc,           FixedPcdGet32 (PcdFlashFvEcBase),                FixedPcdGet32 (PcdFlashFvEcSize)                   },
  {FbtsRomMapDxe,          FixedPcdGet32 (PcdFlashFvMainBase),              FixedPcdGet32 (PcdFlashFvMainSize)                 },
  {FbtsRomMapNVRam,        FixedPcdGet32 (PcdFlashNvStorageVariableBase),   FixedPcdGet32 (PcdFlashNvStorageVariableSize)      },
  {FbtsRomMapFtwState,     FixedPcdGet32 (PcdFlashNvStorageFtwWorkingBase), FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize)    },
  {FbtsRomMapFtwBackup,    FixedPcdGet32 (PcdFlashNvStorageFtwSpareBase),   FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize)      },
  {FbtsRomMapPei,          FixedPcdGet32 (PcdFlashFvRecoveryBase),          FixedPcdGet32 (PcdFlashFvRecoverySize)},
  {FbtsRomMapEos,          0,          0      }

};

FLASH_BASE_MAP_TABLE                    mFlashBaseMapTable[] = {{FLASH_SIZE_1024K, ROM_1M_BASE },
                                                                {FLASH_SIZE_2048K, ROM_2M_BASE },
                                                                {FLASH_SIZE_4096K, ROM_4M_BASE },
                                                                {FLASH_SIZE_8192K, ROM_8M_BASE },
                                                                {FLASH_SIZE_16384K, ROM_16M_BASE }
                                                               };

//
// Add OEM private rom map table,
//
FBTS_PLATFORM_PRIVATE_ROM mOemPrivateRomMap[] = {
  {FixedPcdGet32 (PcdFlashNvStorageMsdmDataBase), FixedPcdGet32 (PcdFlashNvStorageMsdmDataSize)},
  {FixedPcdGet32 (PcdFlashRVPspNvRamBase), FixedPcdGet32 (PcdFlashRVPspNvRamSize)},
  {FbtsRomMapEos,  0}
};


UINT16
GetH2OIhisiFbtsToolVersion (
  VOID
  )
{
  EFI_STATUS                              Status;
  H2O_IHISI_FBTS_TOOL_VERSION_PROTOCOL    *ToolVersionProtocol = NULL;

  Status = gSmst->SmmLocateProtocol (
                    &gH2OIhisiFbtsToolVersionProtocolGuid,
                    NULL,
                    (VOID**)&ToolVersionProtocol
                    );

  if (EFI_ERROR (Status)) {
    return 0;
  }

  return ToolVersionProtocol->IhisiVersion;
}

/**
  AH=10h(FbtsGetSupportVersion), Get FBTS supported version and FBTS permission.

  @param[in] VersionStr          Flash tool version
  @param[in, out] Permission     Permission
**/
VOID
EFIAPI
IhisiFbtsGetPermission (
  IN  FBTS_TOOLS_VERSION_BUFFER           *VersionPtr,
  IN  OUT UINT16                          *Permission
  )
{

  if (VersionPtr == NULL || Permission == NULL) {
    return;
  }
  //
  // Intialial permission to FBTS_PERMISSION_ALLOWED
  //
  *Permission = FBTS_PERMISSION_ALLOWED;
  //
  // Check version signature
  //
  if (VersionPtr->Signature != FBTS_VERSION_SIGNATURE) {
    *Permission = FBTS_PERMISSION_DENY;
  }
}

/**
  AH=12h(FbtsGetPlatformRomMap), Get Oem define flash map.

  @param[in, out] RomMapBuffer          Pointer to the returned platform's ROM map protection structure.
                                        After version 1.7.6, InsydeFlash will skip this structure if it found definition in BVDT
  @param[in, out] PrivateRomMapBuffer   Pointer to the returned platform's private map structure.
                                        Flash utility will not flash these areas
                                        (even userenter /all in flashit or all=1 in platform.ini).

  @retval EFI_SUCCESS       Get OEM flash map successful.
  @retval EFI_UNSUPPORTED   FBTS_OEM_ROM_MAP_COUNT is 0 or module rom map buffer is full.
**/
EFI_STATUS
EFIAPI
IhisiFbtsGetOemFlashMap (
  IN OUT   FBTS_PLATFORM_ROM_MAP       **RomMapBuffer,
  IN OUT   FBTS_PLATFORM_PRIVATE_ROM   **PrivateRomMapBuffer
  )
{
  UINTN                         Media_mOemRomMapSize;
  UINTN                         Media_mOemPrivateRomMapSize;
  FBTS_PLATFORM_ROM_MAP         *Media_mOemRomMap;
  FBTS_PLATFORM_PRIVATE_ROM     *Media_mOemPrivateRomMap;
  UINTN                         Index;


  Media_mOemRomMapSize        = (sizeof (mOemRomMap) / sizeof (mOemRomMap[0]));
  Media_mOemPrivateRomMapSize = (sizeof (mOemPrivateRomMap) / sizeof (mOemPrivateRomMap[0]));

  if (Media_mOemRomMapSize > (UINTN)(sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER) / sizeof (FBTS_PLATFORM_ROM_MAP))) {
    return EFI_UNSUPPORTED;
  }

  if (Media_mOemPrivateRomMapSize > (UINTN)(sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER) / sizeof (FBTS_PLATFORM_PRIVATE_ROM))) {
    return EFI_UNSUPPORTED;
  }

  Media_mOemRomMap = (*RomMapBuffer);
  for (Index = 0; Index < Media_mOemRomMapSize; Index++) {
    Media_mOemRomMap[Index].Type = mOemRomMap[Index].Type;
    Media_mOemRomMap[Index].Address = mOemRomMap[Index].Address;
    Media_mOemRomMap[Index].Length = mOemRomMap[Index].Length;
    if (Media_mOemRomMap[Index].Type == FbtsRomMapEos) {
      break;
    }
  }

  Media_mOemPrivateRomMap = (*PrivateRomMapBuffer);
  for (Index = 0; Index < Media_mOemPrivateRomMapSize; Index++) {
    Media_mOemPrivateRomMap[Index].LinearAddress = mOemPrivateRomMap[Index].LinearAddress;
    Media_mOemPrivateRomMap[Index].Size = mOemPrivateRomMap[Index].Size;
    if (Media_mOemPrivateRomMap[Index].LinearAddress == (UINT32)FbtsRomMapEos) {
      break;
    }
  }

  return EFI_SUCCESS;
}

/**
  AH=15h(FBTS write) :  Hook function before Write process

  @param[in, out] WriteDataBufferPtr    Pointer to data buffer for write.
  @param[in, out] WriteSizePtr          Write size.
  @param[in, out] RomBaseAddressPtr     Target linear address to write.

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
EFIAPI
IhisiFbtsDoBeforeWriteProcess (
  IN OUT    UINT8                      *WriteDataBufferPtr,
  IN OUT    UINTN                      *WriteSizePtr,
  IN OUT    UINTN                      *RomBaseAddressPtr
  )
{
  EFI_STATUS OemSvcStatus;

  if ((*(mSmmFwBlockService->FlashMode)) == SMM_FW_DEFAULT_MODE){
    *(mSmmFwBlockService->FlashMode) = SMM_FW_FLASH_MODE;
    if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
//      if (PcdGet16 (PcdIhisiFbtsToolVersion) < FBTSAPHOOK_SUPPORT) {
      if (GetH2OIhisiFbtsToolVersion() < FBTSAPHOOK_SUPPORT) {
        DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEcIdle \n"));
        OemSvcStatus = OemSvcEcIdle (TRUE);
        DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEcIdle Status: %r\n", OemSvcStatus));
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  AH=15h(FBTS write) : Hook function after Write process.

  @param[in]      WriteStatus

  @retval EFI_SUCCESS        Function returns successfully
**/
EFI_STATUS
EFIAPI
IhisiFbtsDoAfterWriteProcess (
  IN EFI_STATUS        WriteStatus
  )
{
  EcReinitRequired = TRUE;

  return EFI_SUCCESS;
}

/**
  AH=14h(FbtsRead) : Hook function before read process

  @param[in, out] ReadAddress           Target linear address to read.
  @param[in, out] ReadSize              Read size.
  @param[in, out] DataBuffer            Pointer to returned data buffer.

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
EFIAPI
IhisiFbtsDoBeforeReadProcess (
  IN OUT UINTN                            *ReadAddress,
  IN OUT UINTN                            *ReadSize,
  IN OUT UINT8                            *DataBuffer
  )
{
  EFI_STATUS OemSvcStatus;

  if ((*(mSmmFwBlockService->FlashMode)) == SMM_FW_DEFAULT_MODE){
    *(mSmmFwBlockService->FlashMode) = SMM_FW_FLASH_MODE;
    if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
//      if (PcdGet16 (PcdIhisiFbtsToolVersion) < FBTSAPHOOK_SUPPORT) {
      if (GetH2OIhisiFbtsToolVersion() < FBTSAPHOOK_SUPPORT) {
        DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEcIdle \n"));
        OemSvcStatus = OemSvcEcIdle (TRUE);
        DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEcIdle Status: %r\n", OemSvcStatus));
      }
    }
  }
  return EFI_SUCCESS;
}

/**
  AH=14h(FbtsRead) :  Hook function after read process

  @retval EFI_SUCCESS        Function returns successfully
**/
EFI_STATUS
EFIAPI
IhisiFbtsDoAfterReadProcess (
  IN EFI_STATUS        ReadStatus
  )
{
  return EFI_SUCCESS;
}

/**
  AH=16h(Fbtscomplete), This function uses to execute Ap terminate.

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
EFIAPI
IhisiFbtsApTerminated (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Function to entry S3.

  @retval EFI_SUCCESS      Successfully returns.
**/
EFI_STATUS
EFIAPI
IhisiFbtsS3 (
  VOID
  )
{
  EFI_STATUS                    Status;
  IMAGE_INFO                    ImageInfo;
  UINTN                         Size;
  UINT32                        Data32;
  UINT32                        AcpiMmioAddr;
  UINT32                        SmiMmioAddr;
  UINT16                        AcpiBaseAddr;
  UINT32                        Buffer;

  AcpiBaseAddr = 0;
  AcpiMmioAddr = 0;

  //
  // To get FCH ACPI Base Address.
  //
  LibFchGetAcpiPmBase (&AcpiBaseAddr);

  //
  // To get FCH ACPI MMIO Base Address.
  //
  LibFchGetAcpiMmioBase (&AcpiMmioAddr);

  SmiMmioAddr = AcpiMmioAddr + SMI_BASE;

  Size = sizeof (IMAGE_INFO);
  Status = CommonGetVariable (
             SECURE_FLASH_INFORMATION_NAME,
             &gSecureFlashInfoGuid,
             &Size,
             &ImageInfo);
  if ((Status == EFI_SUCCESS) && (ImageInfo.FlashMode)) {

    //
    // Disable Global SMI
    //
    Data32 = MmioRead32 (SmiMmioAddr + FCH_SMI_REG98);
    Data32 |= BIT31;
    MmioWrite32 (SmiMmioAddr + FCH_SMI_REG98, Data32);

    //
    // Enable SLP_TYPE and SLP_EN for entry S3 state.
    //
    Buffer = IoRead32 (AcpiBaseAddr + R_FCH_ACPI_PM_CONTROL);
    Buffer &= ~(B_ACPI_SLP_EN | B_SLP_TYPE);
    Buffer |= V_SLP_TYPE_S3;
    IoWrite32 ((AcpiBaseAddr + R_FCH_ACPI_PM_CONTROL), Buffer);
    Buffer |= V_ACPI_SLP_EN;
    IoWrite32 ((AcpiBaseAddr + R_FCH_ACPI_PM_CONTROL), Buffer);

    //
    // Enable Global SMI
    //
    Data32 = MmioRead32 (SmiMmioAddr + FCH_SMI_REG98);
    Data32 &= ~BIT31;
    MmioWrite32 (SmiMmioAddr + FCH_SMI_REG98, Data32);

    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}

/**
  AH=16h(Fbtscomplete), This function uses to execute normal flash.
                        (Update whole image or BIOS region by normal or secure flash.)

  @retval EFI_UNSUPPORTED    Returns unsupported by default.
  @retval EFI_SUCCESS        The service is customized in the project.
**/
EFI_STATUS
EFIAPI
IhisiFbtsNormalFlash (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

/**
  AH=16h(Fbtscomplete), This function uses to execute Partial flash.
                        (Update specific address or update single firmware volume.)

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
EFIAPI
IhisiFbtsPartialFlash (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  AH=16h(Fbtscomplete), This function is a hook funciton berfore ApRequest execute.

  @retval EFI_UNSUPPORTED    Returns unsupported by default.
  @retval EFI_SUCCESS        The service is customized in the project.
**/
EFI_STATUS
EFIAPI
IhisiFbtsOemComplete (
  IN UINT8           ApRequest
  )
{
  EFI_STATUS    Status;

  Status = EFI_UNSUPPORTED;

  if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
    if (FeaturePcdGet (PcdEcReInitAfterFlash)) {
//      if (PcdGet16 (PcdIhisiFbtsToolVersion) < FBTSAPHOOK_SUPPORT) {
      if (GetH2OIhisiFbtsToolVersion() < FBTSAPHOOK_SUPPORT) {
        if (!EcReinitRequired) {
          OemSvcEcIdle (FALSE);
        }
      }
    } else {
//      if (PcdGet16 (PcdIhisiFbtsToolVersion) < FBTSAPHOOK_SUPPORT) {
      if (GetH2OIhisiFbtsToolVersion() < FBTSAPHOOK_SUPPORT) {
        OemSvcEcIdle (FALSE);
      }
    }
  }

  if (PcdGetBool (PcdEcReInitAfterFlash)) {
    if (EcReinitRequired) {
      //
      // Add SmmEcReinit (); after EcLib Down
      //
      //SmmEcReinit ();
      EcReinitRequired = FALSE;
    }
  }

//[-start-190319-IB06070016-modify]//
  if (ApRequest == FlashCompleteS3) {
    Status = IhisiFbtsS3 ();
  }

  if (ApRequest == FlashCompleteReboot) {
    Status = ProcessPendingCapsule ();
  }

  return Status;
//[-end-190319-IB06070016-modify]//

}

/**
  For the system to do RTC wakeup when it is shutted down by IHISI 16h.

  @param [in] AcpiBaseAddr         ACPI Base Address.

  @retval void

**/
VOID
S5WakeUpSetting (
  IN UINT16                         AcpiBaseAddr
  )
{
  EFI_STATUS                     Status;
  UINT8                          Buffer;
  SYSTEM_CONFIGURATION           SetupVariable;
  UINTN                          VariableSize;

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = CommonGetVariable (
             SETUP_VARIABLE_NAME,
             &gSystemConfigurationGuid,
             &VariableSize,
             &SetupVariable
             );
  //
  // To see if S5LongRunTest is enabled or not.
  //
  if (SetupVariable.S5LongRunTest == TRUE) {
    IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_B);
    Buffer = IoRead8 (PCAT_RTC_DATA_REGISTER);
    Buffer |= ALARM_INTERRUPT_ENABLE;
    IoWrite8 (PCAT_RTC_DATA_REGISTER, Buffer);

    Buffer = IoRead8 (AcpiBaseAddr + PM1_EN_HIGH_BYTE);
    Buffer |= RTC_EVENT_ENABLE;
    IoWrite8 (AcpiBaseAddr + PM1_EN_HIGH_BYTE, Buffer);
  }

  return;
}

/**
  Function to do system shutdown.

  @retval EFI_SUCCESS      Successfully returns.
**/
EFI_STATUS
EFIAPI
IhisiFbtsShutDown (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         Value8, Data8;
  UINT32                        Data32;
  UINT32                        AcpiMmioAddr;
  UINT32                        SmiMmioAddr;
  UINT16                        AcpiBaseAddr;
  UINT32                        Buffer;

  AcpiBaseAddr = 0;
  AcpiMmioAddr = 0;

  //
  // To get FCH ACPI Base Address.
  //
  LibFchGetAcpiPmBase (&AcpiBaseAddr);

  //
  // To get FCH ACPI MMIO Base Address.
  //
  LibFchGetAcpiMmioBase (&AcpiMmioAddr);

  S5WakeUpSetting (AcpiBaseAddr);

  SmiMmioAddr = AcpiMmioAddr + SMI_BASE;
  Data32 = MmioRead32 (SmiMmioAddr + FCH_SMI_REGB0);
  Data32 &= ~(BIT2 + BIT3);
  MmioWrite32 ((SmiMmioAddr + FCH_SMI_REGB0), Data32);

  do {
    Value8 = FCH_PMIOA_REGBE;
    IoWrite8 (FCH_PM_INDEX_PORT, Value8);
    Data8 = IoRead8 (FCH_PM_DATA_PORT);
    Data8 |= BIT5;
    IoWrite8 (FCH_PM_DATA_PORT, Value8);
    Data8 = IoRead8 (FCH_PM_DATA_PORT);
  } while ((Data8 & BIT5) == 0);

  do {
    Value8 = (FCH_PMIOA_REG08+3);
    IoWrite8 (FCH_PM_INDEX_PORT, Value8);
    Data8 = IoRead8 (FCH_PM_DATA_PORT);
    Data8 &= ~(BIT1 + BIT0);
    Data8 |= BIT0;
    IoWrite8 (FCH_PM_DATA_PORT, Value8);
    Data8 = IoRead8 (FCH_PM_DATA_PORT);
  } while ((Data8 & BIT0) == 0);

  //
  // Shut it off now.
  //
  Buffer = IoRead32 (AcpiBaseAddr + R_FCH_ACPI_PM_CONTROL);
  Buffer &= ~(B_ACPI_SLP_EN | B_SLP_TYPE);
  Buffer |= V_SLP_TYPE_S5;
  IoWrite32 ((AcpiBaseAddr + R_FCH_ACPI_PM_CONTROL), Buffer);
  Buffer |= V_ACPI_SLP_EN;
  IoWrite32 ((AcpiBaseAddr + R_FCH_ACPI_PM_CONTROL), Buffer);

  Status = EFI_SUCCESS;

  return Status;
}

/**
  Function to reboot system.

  @retval EFI_SUCCESS      Successfully returns.
**/
EFI_STATUS
EFIAPI
IhisiFbtsReboot (
  VOID
  )
{
  UINT8                         Buffer;

  //
  // PCI reset command
  //
  Buffer = 6;
  IoWrite8 (0xCF9, Buffer);

  return EFI_SUCCESS;
}

/**
  Function to flash complete do nothing.

  @retval EFI_SUCCESS      Successfully returns.
**/
EFI_STATUS
EFIAPI
IhisiFbtsApRequestDoNothing (
  VOID
  )
{
  return EFI_SUCCESS;
}

