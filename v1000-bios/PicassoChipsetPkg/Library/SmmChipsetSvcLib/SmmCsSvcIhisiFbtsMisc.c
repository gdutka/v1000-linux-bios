/** @file
 SMM Chipset Services Library.

 This file contains Ihisi Fbts Chipset service Lib function.

***************************************************************************
* Copyright (c) 2014 - 2015, Insyde Software Corp. All Rights Reserved.
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
#include <Library/SmmChipsetSvcLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseOemSvcKernelLib.h>
#include <Library/IoLib.h>
#include <Library/FchDxeLib.h>
#include <Protocol/SmmFwBlockService.h>
#include <Protocol/SetupUtility.h>
#include <Library/VariableLib.h>

#include <FchRegs.h>
#include <SetupConfig.h>

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
  {FbtsRomMapEc,           FixedPcdGet32 (PcdFlashFvEcBase),                FixedPcdGet32 (PcdFlashFvEcSize)                   },
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

UINTN FBTS_OEM_ROM_MAP_COUNT = (sizeof (mOemRomMap) / sizeof (mOemRomMap[0]));

//
// Add OEM private rom map table,
//
FBTS_PLATFORM_PRIVATE_ROM mOemPrivateRomMap[] = {
  {FixedPcdGet32 (PcdFlashNvStorageMsdmDataBase), FixedPcdGet32 (PcdFlashNvStorageMsdmDataSize)},
  {FbtsRomMapEos,  0}
};

/**
  AH=10h(FbtsGetSupportVersion), Get FBTS supported version and FBTS permission.

  @param[in] VersionStr          Flash tool version
  @param[in, out] Permission     Permission
**/
VOID
EFIAPI
SmmCsSvcIhisiFbtsGetPermission (
  IN  FBTS_TOOLS_VERSION_BUFFER           *VersionPtr,
  IN  OUT UINT16                          *Permission
  )
{
  UINT16                                  IhisiToolVersion;

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
  //
  // OEM can add other check condition in this area
  //
  IhisiToolVersion = (UINT16)(VersionPtr->ATpVersion[0] - 0x30) * 100;
  IhisiToolVersion += (UINT16)(VersionPtr->ATpVersion[1] - 0x30) * 10;
  IhisiToolVersion += (UINT16)VersionPtr->ATpVersion[2] - 0x30;
  PcdSet16(PcdIhisiFbtsToolVersion, IhisiToolVersion);

  return;
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
SmmCsSvcIhisiFbtsGetOemFlashMap (
  IN OUT   FBTS_PLATFORM_ROM_MAP       **RomMapBuffer,
  IN OUT   FBTS_PLATFORM_PRIVATE_ROM   **PrivateRomMapBuffer
  ) 
{

  if (FBTS_OEM_ROM_MAP_COUNT == 0) {
    return EFI_UNSUPPORTED;
  }
  *RomMapBuffer = mOemRomMap;
  *PrivateRomMapBuffer = mOemPrivateRomMap;

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
SmmCsSvcIhisiFbtsDoBeforeWriteProcess (
  IN OUT    UINT8                      *WriteDataBufferPtr,
  IN OUT    UINTN                      *WriteSizePtr,
  IN OUT    UINTN                      *RomBaseAddressPtr
  )
{
  if ((*(mSmmFwBlockService->FlashMode)) == SMM_FW_DEFAULT_MODE){
    *(mSmmFwBlockService->FlashMode) = SMM_FW_FLASH_MODE;
    if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
      if (PcdGet16 (PcdIhisiFbtsToolVersion) < FBTSAPHOOK_SUPPORT) {
        OemSvcEcIdle (TRUE);
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  AH=15h(FBTS write) :  Hook function after Write process

  @param[in]      WriteStatus

  @retval EFI_SUCCESS        Function returns successfully
**/
EFI_STATUS
EFIAPI
SmmCsSvcIhisiFbtsDoAfterWriteProcess (
  IN EFI_STATUS        WriteStatus
  )
{
  EcReinitRequired = TRUE;

  return EFI_SUCCESS;
}

/**
  AH=14h(FbtsRead) :  Hook function before read process

  @param[in, out] ReadAddress           Target linear address to read.
  @param[in, out] ReadSize              Read size.
  @param[in, out] DataBuffer            Pointer to returned data buffer.

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
EFIAPI
SmmCsSvcIhisiFbtsDoBeforeReadProcess (
  IN OUT UINTN                            *ReadAddress,
  IN OUT UINTN                            *ReadSize,
  IN OUT UINT8                            *DataBuffer
  )
{
  if ((*(mSmmFwBlockService->FlashMode)) == SMM_FW_DEFAULT_MODE){
    *(mSmmFwBlockService->FlashMode) = SMM_FW_FLASH_MODE;
    if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
      if (PcdGet16 (PcdIhisiFbtsToolVersion) < FBTSAPHOOK_SUPPORT) {
        OemSvcEcIdle (TRUE);
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
SmmCsSvcIhisiFbtsDoAfterReadProcess (
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
SmmCsSvcIhisiFbtsApTerminated (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  AH=16h(Fbtscomplete), This function uses to execute normal flash.
                        (Update whole image or BIOS region by normal or secure flash.)

  @retval EFI_UNSUPPORTED    Returns unsupported by default.
  @retval EFI_SUCCESS        The service is customized in the project.
**/
EFI_STATUS
EFIAPI
SmmCsSvcIhisiFbtsNormalFlash (
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
SmmCsSvcIhisiFbtsPartialFlash (
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
SmmCsSvcIhisiFbtsOemComplete (
  IN UINT8           ApRequest
  )
{
  if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
    if (FeaturePcdGet (PcdEcReInitAfterFlash)) {
      if (PcdGet16 (PcdIhisiFbtsToolVersion) < FBTSAPHOOK_SUPPORT) {
        if (!EcReinitRequired) {
          OemSvcEcIdle (FALSE);
        }
      }
    } else {
      if (PcdGet16 (PcdIhisiFbtsToolVersion) < FBTSAPHOOK_SUPPORT) {
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
  
  return EFI_UNSUPPORTED;
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
SmmCsSvcIhisiFbtsShutDown (
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
SmmCsSvcIhisiFbtsReboot (
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
SmmCsSvcIhisiFbtsApRequestDoNothing (
  VOID
  )
{
  return EFI_SUCCESS;
}

