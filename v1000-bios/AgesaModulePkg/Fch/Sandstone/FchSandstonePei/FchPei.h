
/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH PEIM
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH PEIM
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _FCH_PEI_H_
#define _FCH_PEI_H_

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/FchInitHookLib.h>
#include "FchPlatform.h"
#include <Ppi/AmdFchInitPpi.h>

#include  <IdsHookId.h>
#include  <Library/AmdIdsHookLib.h>
/**
 *  @page FchPei  AMD FCH PEI Driver
 *  @section ACPI_States  To define values for each ACPI Sx
 *           state.
 */
#define ACPI_S0       (0 << 10)
#define ACPI_S3       (3 << 10)
#define ACPI_S4       (4 << 10)
#define ACPI_S5       (5 << 10)
#define ACPI_BM_MASK  (7 << 10)

/// Module data structure
typedef struct _FCH_PEI_PRIVATE {
  UINTN                  Signature;          ///< Signature
  AMD_FCH_INIT_PPI       FchInitPpi;         ///< FCH INIT PPI
  AMD_CONFIG_PARAMS      StdHdr;             ///< AMD_CONFIG_PARAMS
} FCH_PEI_PRIVATE;

#define FCH_PEI_PRIVATE_DATA_SIGNATURE   SIGNATURE_32 ('$', 'F', 'C', 'H')

#define FCH_PEI_PRIVATE_FROM_THIS(a) \
  CR (a, FCH_PEI_PRIVATE, FchInitPpi, FCH_PEI_PRIVATE_DATA_SIGNATURE)


/// Functions Prototypes
EFI_STATUS
EFIAPI
FchPeiInit (
  IN       EFI_PEI_FILE_HANDLE    FileHandle,
  IN       CONST EFI_PEI_SERVICES **PeiServices
  );

UINT8 PmIoRead8 (
  IN       EFI_PEI_SERVICES  **PeiServices,
  IN       UINT8             RegIndex
  );

UINT16
PmIoRead16 (
  IN       EFI_PEI_SERVICES  **PeiServices,
  IN       UINT8             RegIndex
  );

EFI_STATUS
FchUpdateBootMode (
  IN       EFI_PEI_SERVICES    **PeiServices
  );

EFI_STATUS
EFIAPI
FchMemoryDiscoveredPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );
//[-start-240426-IB18160212-modify-]//
VOID
EFIAPI
FchInitXhciOnRecovery (
  IN       AMD_FCH_INIT_PPI   *This,
  IN       UINT32             XhciRomAddress
  );

VOID
EFIAPI
FchInitEhciOnRecovery (
  IN       AMD_FCH_INIT_PPI   *This,
  IN       UINT32             EhciTemporaryBarAddress
  );

VOID
EFIAPI
FchInitSataOnRecovery (
  IN       AMD_FCH_INIT_PPI   *This,
  IN       UINT32             SataBar0,
  IN       UINT32             SataBar5
  );

VOID
EFIAPI
FchInitGppOnRecovery (
  IN      AMD_FCH_INIT_PPI     *This,
  IN      FCH_GPP_R            *FchGpp
  );

VOID
EFIAPI
FchInitEmmcOnRecovery (
  IN      AMD_FCH_INIT_PPI     *This
  );
//[-start-240426-IB18160212-modify-]//
extern EFI_STATUS
FchXhciOnRecovery (
  IN      FCH_PEI_PRIVATE      FchPrivate,
  IN      UINT32               XhciRomAddress
  );

extern EFI_STATUS
FchEhciOnRecovery (
  IN      FCH_PEI_PRIVATE      FchPrivate,
  IN      UINT32               EhciTemporaryBarAddress
  );

extern EFI_STATUS
FchSataOnRecovery (
  IN      FCH_PEI_PRIVATE      FchPrivate,
  IN      UINT32               SataBar0,
  IN      UINT32               SataBar5
  );

extern EFI_STATUS
FchGppOnRecovery (
  IN      FCH_PEI_PRIVATE      FchPrivate,
  IN      FCH_GPP_R            *FchGpp
  );

#ifndef GEC_ROM_SHADOW_ADDRESS
  #define GEC_ROM_SHADOW_ADDRESS   0xFED61000ul           //GecShadowRomBase
#endif
#endif // _FCH_PEI_H_


