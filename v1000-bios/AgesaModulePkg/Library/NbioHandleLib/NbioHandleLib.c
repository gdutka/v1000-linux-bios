/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB function to create/locate PCIe configuration data area
 *
 * Contain code that create/locate/manages GNB/PCIe configuration
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800
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
#include  <GnbDxio.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/AmdBaseLib.h>
#include  <Library/GnbCommonLib.h>

#define FILECODE LIBRARY_NBIOHANDLELIB_NBIOHANDLELIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Get child descriptor of specific type
 *
 *
 * @param[in]       Type            Descriptor type
 * @param[in]       Descriptor      Pointer to buffer to pass information to callback
 */
STATIC
PCIe_DESCRIPTOR_HEADER * 
PcieConfigGetChild (
  IN       UINT32                        Type,
  IN       PCIe_DESCRIPTOR_HEADER        *Descriptor
  )
{
  while ((Descriptor->DescriptorFlags & Type) == 0) {
    if (Descriptor->Child != 0) {
      Descriptor = (PCIe_DESCRIPTOR_HEADER *) ((UINT8 *) Descriptor + Descriptor->Child);
    } else {
      return NULL;
    }
  }
  return Descriptor;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get GNB handle
 *
 *
 * @param[in]       Pcie           Pointer to global PCIe configuration
 */
GNB_HANDLE *
NbioGetHandle (
  IN PCIe_PLATFORM_CONFIG            *Pcie
  )
{
  GNB_HANDLE            *NbioHandle;
  NbioHandle = (GNB_HANDLE *) PcieConfigGetChild (DESCRIPTOR_SILICON, &Pcie->Header);
  return NbioHandle;
}

/*----------------------------------------------------------------------------------------*/
/*
 * Get PCI_ADDR of GNB
 *
 *
 * @param[in]  Handle           Pointer to GNB_HANDLE
 */

PCI_ADDR
NbioGetHostPciAddress (
  IN      GNB_HANDLE          *Handle
  )
{
  ASSERT (Handle != NULL);
  return Handle->Address;
}


