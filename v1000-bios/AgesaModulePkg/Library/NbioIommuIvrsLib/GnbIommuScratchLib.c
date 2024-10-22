/* $NoKeywords:$ */
/**
 * @file
 *
 * NB services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <AGESA.h>
//#include  "S3SaveState.h"
#include  <Gnb.h>
#include  <PiDxe.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbCommonLib.h>
#include  <GnbRegistersCommonV2.h>
#include  <Library/GnbHeapLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/NbioHandleLib.h>
#include  <Library/AmdHeapLib.h>
#include  <Protocol/AmdNbioPcieServicesProtocol.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/NbioIommuIvrsLib.h>
#include  <Guid/GnbPcieInfoHob.h>
#include  <Guid/HobList.h>
#include  <Filecode.h>

#define FILECODE LIBRARY_NBIOIOMMUIVRSLIB_GNBIOMMUSCRATCHLIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GnbIommuScratchMemoryRangeInterface (
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Set Iommu Scratch Memory Range
 * 1) code needs to be executed at Late Init
 * 2) Allocate heap using  heap type HEAP_RUNTIME_SYSTEM_MEM
 * 3) Allocate enough memory to be able to get address aligned required by register
 * 4) Assign same address to all Gnb in system
 *
 *
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
GnbIommuScratchMemoryRangeInterface (
  )
{
  AGESA_STATUS                        Status;
  ALLOCATE_HEAP_PARAMS                AllocHeapParams;
  UINT32                              AddressLow;
  UINT32                              AddressHigh;
  GNB_HANDLE                          *GnbHandle;
  DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  PCIe_PLATFORM_CONFIG                *Pcie;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbIommuScratchMemoryRangeInterface Enter\n");

  AllocHeapParams.RequestedBufferSize = 128;
  AllocHeapParams.BufferHandle = AMD_GNB_IOMMU_SCRATCH_MEM_HANDLE;
  AllocHeapParams.Persist = HEAP_RUNTIME_SYSTEM_MEM;
  Status = HeapAllocateBuffer (&AllocHeapParams, (AMD_CONFIG_PARAMS *)NULL);
  if (Status != AGESA_SUCCESS) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  Iommu Scratch Memory not allocated.\n");
    ASSERT (FALSE);
    return AGESA_FATAL;
  }

  AddressLow = (((UINT32) ((UINT64) AllocHeapParams.BufferPtr)) + 0x3F) & D0F0x98_x27_IOMMUUrAddr_31_6__MASK;
  AddressHigh = ((UINT32) (((UINT64) AllocHeapParams.BufferPtr) >> 32)) & D0F0x98_x26_IOMMUUrAddr_39_32__MASK;

  Status = gBS->LocateProtocol (
                  &gAmdNbioPcieServicesProtocolGuid,
                  NULL,
                  (VOID**)&PcieServicesProtocol
                  );
  PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
  Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
  GnbHandle = NbioGetHandle (Pcie);

  while (GnbHandle != NULL) {
    if (GnbCheckIommuPresent (GnbHandle, (AMD_CONFIG_PARAMS *)NULL)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Set Iommu Scratch Memory for Socket %d Die %d\n", GnbGetSocketId (GnbHandle) , GnbGetDieNumber (GnbHandle));
      GnbLibPciIndirectWrite (
        GnbHandle->Address.AddressValue | D0F0x94_ADDRESS,
        D0F0x98_x27_ADDRESS,
        AccessS3SaveWidth32,
        &AddressLow,
        (AMD_CONFIG_PARAMS *)NULL);

      GnbLibPciIndirectWrite (
        GnbHandle->Address.AddressValue | D0F0x94_ADDRESS,
        D0F0x98_x26_ADDRESS,
        AccessS3SaveWidth32,
        &AddressHigh,
        (AMD_CONFIG_PARAMS *)NULL);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbIommuScratchMemoryRangeInterface Exit\n");

  return AGESA_SUCCESS;
}

