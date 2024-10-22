/* $NoKeywords:$ */
/**
 * @file
 *
 * SATA Controller family specific service procedure
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */

/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "FchPlatform.h"
#include  "Filecode.h"
#define FILECODE PROC_FCH_SANDSTONE_SSSATA_SSSATAENVSERVICE_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
//
// Local Routine
//
VOID FchSataCombineControlDataByte (IN UINT8  *ControlReg);
VOID FchSataCombineControlDataWord (IN UINT16  *ControlReg);


/**
 * FchInitEnvProgramSataPciRegs - Sata Pci Configuration Space
 * register setting
 *
 *
 * @param[in] DieBusNum - Bus Number of current Die
 * @param[in] FchDataPtr - Fch configuration structure pointer
 *
 */
VOID
FchInitEnvProgramSata (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT8                  *PortRegByte;
  UINT16                 *PortRegWord;
  UINT32                 PortNum;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  //
  //Caculate SataPortReg for SATA_ESP_PORT
  //
  PortRegByte = &(LocalCfgPtr->Sata.SataEspPort.SataPortReg);
  FchSataCombineControlDataByte (PortRegByte);
  PortRegByte = &(LocalCfgPtr->Sata.SataPortPower.SataPortReg);
  FchSataCombineControlDataByte (PortRegByte);
  PortRegWord = &(LocalCfgPtr->Sata.SataPortMd.SataPortMode);
  FchSataCombineControlDataWord (PortRegWord);
  PortRegByte = &(LocalCfgPtr->Sata.SataHotRemovalEnhPort.SataPortReg);
  FchSataCombineControlDataByte (PortRegByte);

  //
  // All ports are put in offline mode in PEI. Enable ports here
  //
  for (PortNum = 0; PortNum < SANDSTONE_SATA_PORT_NUM; PortNum++) {
    if (((LocalCfgPtr->Sata.SataPortPower.SataPortReg >> PortNum) & BIT0) == 0) {
      FchSSSataInitPortActive (0, PortNum, FchDataPtr);
    }
  }

  // Do Sata init
  FchSSSataInitRsmuCtrl (0, FchDataPtr);

  FchSSSataInitCtrlReg (0, FchDataPtr);

  FchSSSataInitEsata (0, FchDataPtr);

  //Arista
  //FchSSSataInitDevSlp (0, FchDataPtr);

  FchSSSataInitMpssMap (0, FchDataPtr);

  FchSSSataInitEnableErr (0, FchDataPtr);

  FchSSSataSetPortGenMode (0, FchDataPtr);

  //Arista
  /*if (LocalCfgPtr->Sata.SataSgpio0) {
    FchSSSataGpioInitial (0, FchDataPtr);
  }*/
}

/**
 * FchSataCombineControlDataByte - Combine port control options
 * to one control byte.
 *
 *
 * @param[in] *ControlReg - Data pointer for control byte.
 *
 */
VOID
FchSataCombineControlDataByte (
  IN  UINT8    *ControlReg
  )
{
  UINT8   Index;
  UINT8   PortControl;

  *ControlReg = 0;
  for ( Index = 0; Index < 8; Index++ ) {
    PortControl = *( ControlReg + 1 + Index );
    *ControlReg |= PortControl << Index;
  }
}
/**
 * FchSataCombineControlDataWord - Combine port control options
 * to one control Word.
 *
 *
 * @param[in] *ControlReg - Data pointer for control byte.
 *
 */
VOID
FchSataCombineControlDataWord (
  IN  UINT16    *ControlReg
  )
{
  UINT8   Index;
  UINT8   PortControl;

  *ControlReg = 0;
  for ( Index = 0; Index < 8; Index++ ) {
    PortControl = *( (UINT8 *)ControlReg + 2 + Index );
    *ControlReg |= PortControl << (Index * 2);
  }
}


/**
 * FchInitEnvSataRaidProgram - Configuration SATA Raid
 * controller (No longer exist)
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSataRaidProgram (
  IN  VOID     *FchDataPtr
  )
{
}

