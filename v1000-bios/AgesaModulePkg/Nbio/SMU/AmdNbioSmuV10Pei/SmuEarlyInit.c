/* $NoKeywords:$ */
/**
 * @file
 *
 * SMU Early Initialization Function
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioSmuV10Pei
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiPei.h>
#include <Filecode.h>
#include <GnbDxio.h>
#include <GnbRegistersRV.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/GnbRegisterAccZpLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/NbioSmuV10Lib.h>
//#include <PcieComplexDataZP.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/NbioBaseServicesZPPpi.h>
#include <Ppi/AmdCoreTopologyServicesPpi.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Guid/GnbNbioBaseZPInfoHob.h>
//<Embedded_Override_Strat>
#include <Library/SmnAccessLib.h>
//#defines for i-temp SOC Model name
#define ITEMP_OPN_V1404I "V1404I"
//<Embedded_Override_End>
#define FILECODE        NBIO_SMU_AMDNBIOSMUV10PEI_SMUEARLYINIT_FILECODE


/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */


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


//<Embedded_Override_Start>	
/* -----------------------------------------------------------------------------*/
/**
 *
 *  GetOPNNameString
 *
 *  @param[in]  OPNString     OPNString Pointer
 *
 *  Description:
 *    Gets the OPN part String .
 *
 *  @retval VOID
 *
 */
VOID GetOPNNameString( CHAR8 *OPNString )
{
    UINT32              RegisterData;

    if(OPNString == NULL)
        return;
    
    SmnRegisterRead (0, SMUFUSEx000005A0_ADDRESS, &RegisterData);
    //OPN string is present in the offsets 5-11,12-18,19-25,26-31
    
    OPNString[0] = (RegisterData >> 5)&0x7F;
    OPNString[1] = (RegisterData >> 12)&0x7F;
    OPNString[2] = (RegisterData >> 19)&0x7F;
    OPNString[3] = (RegisterData >> 26)&0x7F;
    
    SmnRegisterRead (0, SMUFUSEx000005A4_ADDRESS, &RegisterData);
    //OPN Contunuity string is present in the offsets 1-7,8-15
    OPNString[4] = (RegisterData >> 1)&0x7F;
    OPNString[5] = (RegisterData >> 8)&0x7F;
    
    IDS_HDT_CONSOLE (MAIN_FLOW, "\nOPN Name String:%c%c%c%c%c%c\n", OPNString[0],OPNString[1],OPNString[2],OPNString[3],OPNString[4],OPNString[5]);

}
//<Embedded_Override_End>
/*----------------------------------------------------------------------------------------*/
/**
 * Smu Early Init
 *
 * This is the main flow of the early initialization of the SMU.
 *
 * @param[in]       Pcie           Pointer to global PCIe configuration
 */
EFI_STATUS
AmdNbioSmuEarlyInit (
  IN       PCIe_PLATFORM_CONFIG            *Pcie
  )
{
  EFI_STATUS                      Status;
  GNB_HANDLE                      *NbioHandle;
  //<Embedded_Override_Start>	
  CHAR8 OPNNameString[10] = {0};
  //<Embedded_Override_End>

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioSmuEarlyInit Entry\n");

  Status = EFI_SUCCESS;
  NbioHandle = NbioGetHandle (Pcie);
  while (NbioHandle != NULL) {
    // Repeat initialization for each NbioHandle of the platform
    // NbioSmuFirmwareTestV10 (NbioHandle);
    // TBD - Add SMU initialization functions here
//<Embedded_Override_Start>	
    GetOPNNameString(OPNNameString);
    if(AsciiStrCmp(OPNNameString,ITEMP_OPN_V1404I) == 0)
      PcdSet8 (Pcditemp,1);
//<Embedded_Override_End>

    NbioHandle = GnbGetNextHandle (NbioHandle);
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioSmuEarlyInit Exit\n");
  return Status;
}

