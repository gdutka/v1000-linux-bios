/* $NoKeywords:$ */
/**
 * @file
 *
 *
 * AMD Register Table Related Functions
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


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "PiPei.h"
#include <Filecode.h>
#include <Library/BaseLib.h>
#include <Library/AmdTableLib.h>
#include "Library/PeiServicesTablePointerLib.h"
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/FabricTopologyServices2Ppi.h>
#include <Ppi/AmdCoreTopologyServicesPpi.h>

#define FILECODE LIBRARY_AMDTABLELIB_PEI_AMDTABLESMNPEILIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

PEI_AMD_NBIO_SMU_SERVICES_PPI                 *NbioSmuServices = NULL;
UINTN                                          SystemDieNumber = 0xFF;


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

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the SMN Register Entry.
 *
 * @TableEntryTypeMethod{::SmnRegisters}.
 *
 * Read - Modify - Write the SMN register, clearing masked bits, and setting the data bits.
 *
 * See if you can use the other entries or make an entry that covers the fix.
 * After all, the purpose of having a table entry is to @b NOT have code which
 * isn't generic feature code, but is family/model code specific to one case.
 *
 * @param[in]     Entry             The SMN Data entry to perform
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetSmnEntryLib (
  IN       UINT32                  InstanceId,
  IN       UINT32                  RegisterIndex,
  IN       UINT32                  RegisterANDValue,
  IN       UINT32                  RegisterORValue
  )
{
  UINTN                                  DieLoop;
  EFI_PEI_SERVICES                     **PeiServices;
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES2_PPI *FabricTopologyServices;

  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
  if (NbioSmuServices == NULL) {
    // Get NbioSmuServices
    (**PeiServices).LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gAmdNbioSmuServicesPpiGuid, 0, NULL, (VOID**)&NbioSmuServices);
  }

  if (InstanceId == SMN_ON_ALL_DIES) {
    if (SystemDieNumber == 0xFF) {
      // Get SystemDieNumber
      (*PeiServices)->LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gAmdFabricTopologyServices2PpiGuid, 0, NULL, (VOID**)&FabricTopologyServices);
      FabricTopologyServices->GetSystemInfo (NULL, &SystemDieNumber, NULL, NULL, NULL);
    }
    for (DieLoop = 0; DieLoop < SystemDieNumber; DieLoop++) {
      NbioSmuServices->SmuRegisterRMW (NbioSmuServices, DieLoop, RegisterIndex, RegisterANDValue, RegisterORValue);
    }
  } else {
    NbioSmuServices->SmuRegisterRMW (NbioSmuServices, InstanceId, RegisterIndex, RegisterANDValue, RegisterORValue);
  }
}


