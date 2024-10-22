/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include <Base.h>
#include <Uefi.h>

#include <Protocol/HiiConfigRouting.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>

#include <Guid/MdeModuleHii.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>

#include <Protocol/AmdNbioSmuServicesProtocol.h>

#include <Library/AmdCbsSetupLib.h>
#include <Protocol/AmdCbsHookProtocol.h>
#include <Guid/AmdCbsConfig.h>
#include "AmdCbsVariable.h"
#include "AmdCbsFormID.h"
#include "AmdSoc.h"

#include "PiDxe.h"
#include "Porting.h"
#include "Addendum/Apcb/Inc/RV/APCB.h"
#include "ApcbCommon.h"
#include "Protocol/AmdApcbProtocol.h"
#include "Library/AmdPspBaseLibV2.h"
#include "CbsCustomCorePstates.h"


typedef struct {
  UINT16  OpnSpare;
  UINT16  OpnSpareChkMask;
} COMBO_FLAG_XLAT_OPN_SPARE;
#define  OPN_SPARE_DOESNT_MATTER  (0xFFFFu)

#define  OPN_SPARE_MASK           (0x007Fu)
#define  OPN_SPARE_CHECK_MASK     (0x0060u)

#define  OPN_SPARE_00XXXXX        (0x0 << 5)
#define  OPN_SPARE_01XXXXX        (0x1 << 5)
#define  OPN_SPARE_10XXXXX        (0x2 << 5)
#define  OPN_SPARE_11XXXXX        (0x3 << 5)
typedef union {
  struct {
    UINT32  OPN_Reserved:25;
    UINT32  OPN_OPN_SPARE:7;
  } Field;
  UINT32  Value;
} SMU_FUSE_OPN_SPARE;

typedef struct {
  UINT16  MaxCpuCof;
  UINT16  OcChkMask;
} COMBO_FLAG_XLAT_OC_CAP;
#define  OC_CAP_DOESNT_MATTER  (0xFFFFu)
#define  OC_CAP_CHECK_MASK     (0x003Fu)
#define  OC_CAP_CAPABLE        (0x0000u)

typedef union {
  struct {
    UINT32  Reserved:15;
    UINT32  MaxCpuCof:6;
    UINT32  Reserved1:11;
    UINT32  Reserved2:32;
  } Field;
  UINT64  Value;
} MSR_PMGT_MISC_REGISTER;
#define MSR_PMGT_MISC                 0xC0010292

typedef struct {
  COMBO_FLAG_XLAT             CpuidBase;
  COMBO_FLAG_XLAT_OPN_SPARE   OpnSpare;
  COMBO_FLAG_XLAT_OC_CAP      OcCap;
} COMBO_FLAG_XLAT_EXT;


VOID
PrepareCmnTypeAttrib (
  UINT32 *Size,
  APCB_PARAM_ATTRIBUTE *ApcbParaAttrib,
  VOID *CbsVariable
  );

VOID
PrepareCmnTypeValue (
  UINT32 *Size,
  UINT8 *ApcbParaValue,
  VOID *CbsVariable
  );


extern  EFI_BOOT_SERVICES *gBS;
extern  EFI_GUID gAmdApcbDxeServiceProtocolGuid;

UINTN CbsVariableSize = sizeof(CBS_CONFIG);

EFI_STATUS
CustomFchSataClass (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT CBS_CONFIG                             *pSetup_Config,
  IN  EFI_HII_HANDLE                         HiiHandle
  )
{
  if (pSetup_Config->CbsCmnFchSataEnable == 0xf) {
    pSetup_Config->CbsCmnFchSataClass = 0x2;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
AmdCbsSetupCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT UINT8                                  *IfrData,
  IN  EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CBS_CONFIG *pSetup_Config;

  pSetup_Config = (CBS_CONFIG *)IfrData;

  switch (Action) {
    case EFI_BROWSER_ACTION_RETRIEVE:
      Status = OcMode (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
      break;
    case EFI_BROWSER_ACTION_CHANGING:
    case EFI_BROWSER_ACTION_CHANGED:
      switch (QuestionId) {
      case KEY_CBS_CMN_CPU_OC_MODE:
      case KEY_CBS_CPU_PST_CUSTOM_P0:
      case KEY_CBS_CPU_PST_CUSTOM_P1:
      case KEY_CBS_CPU_PST_CUSTOM_P2:
      case KEY_CBS_CPU_PST_CUSTOM_P3:
      case KEY_CBS_CPU_PST_CUSTOM_P4:
      case KEY_CBS_CPU_PST_CUSTOM_P5:
      case KEY_CBS_CPU_PST_CUSTOM_P6:
      case KEY_CBS_CPU_PST_CUSTOM_P7:
      case KEY_CBS_CPU_PST0_FID:
      case KEY_CBS_CPU_PST0_DID:
      case KEY_CBS_CPU_PST0_VID:
      case KEY_CBS_CPU_PST1_FID:
      case KEY_CBS_CPU_PST1_DID:
      case KEY_CBS_CPU_PST1_VID:
      case KEY_CBS_CPU_PST2_FID:
      case KEY_CBS_CPU_PST2_DID:
      case KEY_CBS_CPU_PST2_VID:
      case KEY_CBS_CPU_PST3_FID:
      case KEY_CBS_CPU_PST3_DID:
      case KEY_CBS_CPU_PST3_VID:
      case KEY_CBS_CPU_PST4_FID:
      case KEY_CBS_CPU_PST4_DID:
      case KEY_CBS_CPU_PST4_VID:
      case KEY_CBS_CPU_PST5_FID:
      case KEY_CBS_CPU_PST5_DID:
      case KEY_CBS_CPU_PST5_VID:
      case KEY_CBS_CPU_PST6_FID:
      case KEY_CBS_CPU_PST6_DID:
      case KEY_CBS_CPU_PST6_VID:
      case KEY_CBS_CPU_PST7_FID:
      case KEY_CBS_CPU_PST7_DID:
      case KEY_CBS_CPU_PST7_VID:
        CustomCorePstate (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
        break;
      case KEY_CBS_CMN_FCH_SATA_ENABLE:
        CustomFchSataClass (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
        break;
      default:
        break;
      }

      break;
    case EFI_BROWSER_ACTION_DEFAULT_STANDARD:
//[-start-221013-IB14860855-remove]//
//      CbsWriteDefalutValue(IfrData);
//[-end-221013-IB14860855-remove]//
      CbsComboIdentify (IfrData);
//[-start-190830-IB06070047-modify]//
      switch (QuestionId) {
      default:
        Status = EFI_UNSUPPORTED;
        break;
      }
//[-end-190830-IB06070047-modify]//
      break;
    default:
//[-start-221013-IB14860855-add]//
      Status = EFI_UNSUPPORTED;
//[-end-221013-IB14860855-add]//
      break;
  }

  return Status;
}

VOID
AmdSaveCbsConfigData (
  IN UINT8   *IfrData
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         ApcbEntryExist;
  TYPE_ATTRIB                     TypeAttrib;
  UINT64                          OldApcbPtr;
  UINT32                          EntrySize;
  UINT64                          EntryDest;
  UINT32                          AttributeSize;
  UINT32                          ValueSize;
  UINT32                          TotalSizeExt;
  UINT8                           *DataStream;
  UINT8                           *ValueStream;
  UINT8                           *NewApcbPtr;
  AMD_APCB_SERVICE_PROTOCOL       *mApcbDxeServiceProtocol;

  NewApcbPtr = NULL;
  // Check the existence and size of APCB  entry 0x60
  ApcbEntryExist = BIOSEntryInfo (BIOS_APCB_INFO, INSTANCE_IGNORED, &TypeAttrib, &OldApcbPtr, &EntrySize, &EntryDest);
    if (!ApcbEntryExist) {
      DEBUG ((EFI_D_ERROR, "APCB Entry does not exist in the SPI ROM\n"));
      return;
    }

  // Calculate the buffer needed to store the external byte stream
  AttributeSize = 0;
  ValueSize = 0;
  DataStream = NULL;
  PrepareCmnTypeAttrib (&AttributeSize, (APCB_PARAM_ATTRIBUTE *)DataStream, (CBS_CONFIG *)IfrData);
  PrepareCmnTypeValue (&ValueSize, DataStream, (CBS_CONFIG *)IfrData);
  TotalSizeExt = AttributeSize + ValueSize;
  TotalSizeExt += ((ALIGN_SIZE_OF_TYPE - (TotalSizeExt % ALIGN_SIZE_OF_TYPE)) & (ALIGN_SIZE_OF_TYPE - 1));

  if (0 != TotalSizeExt) {
    DEBUG ((EFI_D_ERROR, "Replace CBS Common Options\n"));
    // Allocate buffer for new Type data stream
    DataStream = NULL;
    DataStream = AllocateZeroPool (TotalSizeExt);
    if (DataStream == NULL) {
      DEBUG ((EFI_D_ERROR, "Unable to allocate buffer for external APCB type data stream\n"));
      return;
    }

    // Create new Type data stream
    PrepareCmnTypeAttrib (&AttributeSize, (APCB_PARAM_ATTRIBUTE *)DataStream, (CBS_CONFIG *)IfrData);
    ValueStream = &DataStream[AttributeSize];
    PrepareCmnTypeValue (&ValueSize, ValueStream, (CBS_CONFIG *)IfrData);

    Status = gBS->LocateProtocol (&gAmdApcbDxeServiceProtocolGuid, NULL, (VOID**)&mApcbDxeServiceProtocol);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Unable to locate APCB Protocol\n"));
    }

    Status = mApcbDxeServiceProtocol->ApcbUpdateCbsData (mApcbDxeServiceProtocol, DataStream, TotalSizeExt, FALSE);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Unable to update external APCB data\n"));
    }

    Status = mApcbDxeServiceProtocol->ApcbFlushData (mApcbDxeServiceProtocol);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Unable to flush external APCB data\n"));
    }

    // Free temporary buffer
    Status = gBS->FreePool (DataStream);
    if (Status != EFI_SUCCESS) {
      DEBUG ((EFI_D_ERROR, "Fails to free temporary external type data stream buffer\n"));
    }
  }

  return;
}


COMBO_FLAG_XLAT_EXT CbsComboFlagExtTable[] =
{
  // Family, ExtModel, BaseModel, Stepping, PkgType, ComboFlag,     OpnSpare,                 OpnSpareChkMask,                 MaxCpuCof,              OcChkMask
  {{  0x17,   0x1,      0x1,       'x',      0,       1},      { OPN_SPARE_DOESNT_MATTER,  OPN_SPARE_DOESNT_MATTER },      { OC_CAP_DOESNT_MATTER,  OC_CAP_DOESNT_MATTER }}, // RV_FP5
  {{  0x17,   0x1,      0x1,       'x',      1,       1},      { OPN_SPARE_DOESNT_MATTER,  OPN_SPARE_DOESNT_MATTER },      { OC_CAP_DOESNT_MATTER,  OC_CAP_DOESNT_MATTER }}, // RV_FP5_NPU
  {{  0x17,   0x1,      0x1,       'x',      2,      13},      { OPN_SPARE_11XXXXX,        OPN_SPARE_CHECK_MASK    },      { OC_CAP_DOESNT_MATTER,  OC_CAP_DOESNT_MATTER }}, // RV_AM4, RV2 dual source
  {{  0x17,   0x1,      0x1,       'x',      2,       2},      { OPN_SPARE_DOESNT_MATTER,  OPN_SPARE_DOESNT_MATTER },      { OC_CAP_DOESNT_MATTER,  OC_CAP_DOESNT_MATTER }}, // RV_AM4
  {{  0x17,   0x1,      0x8,       'x',      0,      12},      { OPN_SPARE_DOESNT_MATTER,  OPN_SPARE_DOESNT_MATTER },      { OC_CAP_DOESNT_MATTER,  OC_CAP_DOESNT_MATTER }}, // PCO_FP5
  {{  0x17,   0x1,      0x8,       'x',      2,      14},      { OPN_SPARE_11XXXXX,        OPN_SPARE_CHECK_MASK    },      { OC_CAP_CAPABLE,        OC_CAP_CHECK_MASK    }}, // PCO_AM4, RV2 dual source, OC capable
  {{  0x17,   0x1,      0x8,       'x',      2,      15},      { OPN_SPARE_11XXXXX,        OPN_SPARE_CHECK_MASK    },      { OC_CAP_DOESNT_MATTER,  OC_CAP_DOESNT_MATTER }}, // PCO_AM4, RV2 dual source
  {{  0x17,   0x1,      0x8,       'x',      2,      11},      { OPN_SPARE_DOESNT_MATTER,  OPN_SPARE_DOESNT_MATTER },      { OC_CAP_DOESNT_MATTER,  OC_CAP_DOESNT_MATTER }}, // PCO_AM4
  {{  0x17,   0x2,      0x0,       'x',      0,       6},      { OPN_SPARE_DOESNT_MATTER,  OPN_SPARE_DOESNT_MATTER },      { OC_CAP_DOESNT_MATTER,  OC_CAP_DOESNT_MATTER }}, // RV2_FP5
  {{  0x17,   0x2,      0x0,       'x',      3,      16},      { OPN_SPARE_DOESNT_MATTER,  OPN_SPARE_DOESNT_MATTER },      { OC_CAP_DOESNT_MATTER,  OC_CAP_DOESNT_MATTER }}, // RV2_FT5
  {COMBO_FLAG_XLAT_TERMINATOR}
};

VOID
CbsComboIdentify (
  IN UINT8   *IfrData
  )
{
  UINT8      RawPkgType;
  UINT8      RawStepping;
  UINT8      RawBaseModel;
  UINT8      RawExtModel;
  UINT16     RawFamily;
  UINT32     EAX_Reg;
  UINT32     EBX_Reg;
  UINT32     ECX_Reg;
  UINT32     EDX_Reg;
  CBS_CONFIG *Setup_Config;
  COMBO_FLAG_XLAT_EXT *ComboFlagXlatExt;
  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL   *NbioSmuServices;
  SMU_FUSE_OPN_SPARE      FuseOpnSpare;
  MSR_PMGT_MISC_REGISTER  MsrOcCap;
  UINT16                  OpnSpare;
  UINT16                  OcCap;


  Setup_Config = (CBS_CONFIG *)IfrData;

  AsmCpuid (
      0x80000001,
      &EAX_Reg,
      &EBX_Reg,
      &ECX_Reg,
      &EDX_Reg
      );
  // get Raw CPUID
  RawFamily    = (UINT16) (((EAX_Reg & CPUID_BASE_FAMILY_MASK) >> 8) +
                           ((EAX_Reg & CPUID_EXT_FAMILY_MASK)  >> 20));
  RawBaseModel = (UINT8)   ((EAX_Reg & CPUID_BASE_MODEL_MASK)  >> 4);
  RawExtModel  = (UINT8)   ((EAX_Reg & CPUID_EXT_MODEL_MASK)   >> 16);
  RawStepping  = (UINT8)    (EAX_Reg & CPUID_STEPPING_MASK);
  RawPkgType   = (EBX_Reg >> 28) & 0x0F;

  if (RawPkgType == 0) {
    RawPkgType = (UINT8)((PcdGet32 (PcdGetRVFamilyOPN) >> 16) & 0xF);
  }

  NbioSmuServices = NULL;
  gBS->LocateProtocol (&gAmdNbioSmuServicesProtocolGuid, NULL, (VOID**)&NbioSmuServices);
  if (NbioSmuServices != NULL) {
    NbioSmuServices->AmdSmuRegisterRead (NbioSmuServices, 0, 0x5D5C0, &FuseOpnSpare.Value);
    OpnSpare = FuseOpnSpare.Field.OPN_OPN_SPARE & OPN_SPARE_MASK;
  } else {
    OpnSpare = OPN_SPARE_DOESNT_MATTER;
  }
  MsrOcCap.Field.MaxCpuCof = 0;
  MsrOcCap.Value = AsmReadMsr64 (MSR_PMGT_MISC);
  OcCap = MsrOcCap.Field.MaxCpuCof & OC_CAP_CHECK_MASK;

  ComboFlagXlatExt = &CbsComboFlagExtTable[0];
  DEBUG ((EFI_D_ERROR, "Family %x, ExtModel %x, BaseModel %x, Stepping %x, PkgType %x, OpnSpare %x, OcCap %x", RawFamily, RawExtModel, RawBaseModel, RawStepping, RawPkgType, OpnSpare, OcCap));

  while (ComboFlagXlatExt->CpuidBase.ComboFlag != COMBO_FLAG_UNKNOWN) {
    if (((RawFamily    == ComboFlagXlatExt->CpuidBase.RawFamily)    || (ComboFlagXlatExt->CpuidBase.RawFamily    == 'x')) &&
        ((RawExtModel  == ComboFlagXlatExt->CpuidBase.RawExtModel)  || (ComboFlagXlatExt->CpuidBase.RawExtModel  == 'x')) &&
        ((RawBaseModel == ComboFlagXlatExt->CpuidBase.RawBaseModel) || (ComboFlagXlatExt->CpuidBase.RawBaseModel == 'x')) &&
        ((RawStepping  == ComboFlagXlatExt->CpuidBase.RawStepping)  || (ComboFlagXlatExt->CpuidBase.RawStepping  == 'x')) &&
        ((RawPkgType   == ComboFlagXlatExt->CpuidBase.RawPkgType)   || (ComboFlagXlatExt->CpuidBase.RawPkgType   == 'x')) &&
        ((OpnSpare == (ComboFlagXlatExt->OpnSpare.OpnSpare & ComboFlagXlatExt->OpnSpare.OpnSpareChkMask)) || (OPN_SPARE_DOESNT_MATTER == ComboFlagXlatExt->OpnSpare.OpnSpare)) &&
        ((OcCap == (ComboFlagXlatExt->OcCap.MaxCpuCof & ComboFlagXlatExt->OcCap.OcChkMask)) || (OC_CAP_DOESNT_MATTER == ComboFlagXlatExt->OcCap.MaxCpuCof))
        ) {
      Setup_Config->CbsComboFlag = ComboFlagXlatExt->CpuidBase.ComboFlag;
      DEBUG ((EFI_D_ERROR, "ComboFlag %x\n", Setup_Config->CbsComboFlag));
      break;
    }
    ComboFlagXlatExt++;
  }
}

EFI_STATUS
EFIAPI
CbsSetupLoadDefaultFunc (
  )
{
  EFI_STATUS Status;
  CBS_CONFIG *AmdCbsConfig;

  Status = EFI_SUCCESS;
  // Load CBS default value
  AmdCbsConfig = AllocateZeroPool (CbsVariableSize);
  if (AmdCbsConfig != NULL) {
    CbsWriteDefalutValue ((UINT8 *) AmdCbsConfig);
    CbsComboIdentify ((UINT8 *) AmdCbsConfig);

    Status = gRT->SetVariable (
                CBS_SYSTEM_CONFIGURATION_NAME,
                &gCbsSystemConfigurationGuid,
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                CbsVariableSize,
                (VOID *)AmdCbsConfig
                );
    gBS->FreePool (AmdCbsConfig);
  }
  return Status;
}

