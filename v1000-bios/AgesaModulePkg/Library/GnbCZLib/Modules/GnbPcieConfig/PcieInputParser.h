/* $NoKeywords:$ */
/**
 * @file
 *
 *  Procedure to parse PCIe input configuration data
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
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
#ifndef _PCIEINPUTPARSER_H_
#define _PCIEINPUTPARSER_H_


UINTN
PcieInputParserGetNumberOfComplexes (
  IN      PCIe_COMPLEX_DESCRIPTOR       *ComplexList
 );

UINTN
PcieInputParserGetNumberOfEngines (
  IN      PCIe_COMPLEX_DESCRIPTOR       *Complex
 );


PCIe_COMPLEX_DESCRIPTOR*
PcieInputParserGetComplexDescriptor (
  IN      PCIe_COMPLEX_DESCRIPTOR       *ComplexList,
  IN      UINTN                         Index
 );

PCIe_ENGINE_DESCRIPTOR*
PcieInputParserGetEngineDescriptor (
  IN      PCIe_COMPLEX_DESCRIPTOR       *Complex,
  IN      UINTN                         Index
 );

PCIe_COMPLEX_DESCRIPTOR*
PcieInputParserGetComplexDescriptorOfSocket (
  IN      PCIe_COMPLEX_DESCRIPTOR       *ComplexList,
  IN      UINT32                        SocketId
  );

UINTN
PcieInputParserGetLengthOfPcieEnginesList (
  IN      PCIe_COMPLEX_DESCRIPTOR       *Complex
  );

UINTN
PcieInputParserGetLengthOfDdiEnginesList (
  IN      PCIe_COMPLEX_DESCRIPTOR       *Complex
  );

#endif


