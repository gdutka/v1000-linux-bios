/* $NoKeywords:$ */
/**
 * @file
 *
 * Procedure to parse PCIe input configuration data
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "PcieConfigLib.h"
#include  "PcieInputParser.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBPCIECONFIG_PCIEINPUTPARSER_FILECODE
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



/*----------------------------------------------------------------------------------------*/
/**
 * Get number of complexes in platform topology configuration
 *
 *
 *
 * @param[in] ComplexList  First complex configuration in complex configuration array
 * @retval                 Number of Complexes
 *
 */
UINTN
PcieInputParserGetNumberOfComplexes (
  IN      PCIe_COMPLEX_DESCRIPTOR       *ComplexList
  )
{
  UINTN                Result;
  Result = 0;
  while (ComplexList != NULL) {
    Result++;
    ComplexList = PcieInputParsetGetNextDescriptor (ComplexList);
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get number of PCIe engines in given complex
 *
 *
 *
 * @param[in] Complex     Complex configuration
 * @retval                Number of Engines
 */
UINTN
PcieInputParserGetLengthOfPcieEnginesList (
  IN      PCIe_COMPLEX_DESCRIPTOR       *Complex
  )
{
  UINTN                 Result;
  PCIe_PORT_DESCRIPTOR  *PciePortList;
  Result = 0;
  PciePortList = Complex->PciePortList;
  while (PciePortList != NULL) {
    Result++;
    PciePortList = PcieInputParsetGetNextDescriptor (PciePortList);
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get number of DDI engines in given complex
 *
 *
 *
 * @param[in] Complex     Complex configuration
 * @retval                Number of Engines
 */
UINTN
PcieInputParserGetLengthOfDdiEnginesList (
  IN      PCIe_COMPLEX_DESCRIPTOR       *Complex
  )
{
  UINTN                 Result;
  PCIe_DDI_DESCRIPTOR  *DdiLinkList;
  Result = 0;
  DdiLinkList = Complex->DdiLinkList;
  while (DdiLinkList != NULL) {
    Result++;
    DdiLinkList = PcieInputParsetGetNextDescriptor (DdiLinkList);
  }
  return Result;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get number of engines in given complex
 *
 *
 *
 * @param[in] Complex     Complex configuration header
 * @retval                Number of Engines
 */
UINTN
PcieInputParserGetNumberOfEngines (
  IN      PCIe_COMPLEX_DESCRIPTOR         *Complex
  )
{
  UINTN                     Result;

  Result = PcieInputParserGetLengthOfDdiEnginesList (Complex) +
           PcieInputParserGetLengthOfPcieEnginesList (Complex);
  return Result;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get Complex descriptor by index from given Platform configuration
 *
 *
 *
 * @param[in] ComplexList Platform topology configuration
 * @param[in] Index       Complex descriptor Index
 * @retval                Pointer to Complex Descriptor
 */
PCIe_COMPLEX_DESCRIPTOR*
PcieInputParserGetComplexDescriptor (
  IN      PCIe_COMPLEX_DESCRIPTOR         *ComplexList,
  IN      UINTN                           Index
  )
{
  ASSERT (Index < (PcieInputParserGetNumberOfComplexes (ComplexList)));
  return &ComplexList[Index];
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Complex descriptor by index from given Platform configuration
 *
 *
 *
 * @param[in] ComplexList  Platform topology configuration
 * @param[in] SocketId     Socket Id
 * @retval                Pointer to Complex Descriptor
 */
PCIe_COMPLEX_DESCRIPTOR*
PcieInputParserGetComplexDescriptorOfSocket (
  IN      PCIe_COMPLEX_DESCRIPTOR         *ComplexList,
  IN      UINT32                          SocketId
  )
{
  PCIe_COMPLEX_DESCRIPTOR *Result;
  Result = NULL;
  while (ComplexList != NULL) {
    if (ComplexList->SocketId == SocketId) {
      Result = ComplexList;
      break;
    }
    ComplexList = PcieInputParsetGetNextDescriptor (ComplexList);
  }
  return Result;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get Engine descriptor from given complex by index
 *
 *
 *
 * @param[in] Complex     Complex descriptor
 * @param[in] Index       Engine descriptor index
 * @retval                Pointer to Engine Descriptor
 */
PCIe_ENGINE_DESCRIPTOR*
PcieInputParserGetEngineDescriptor (
  IN      PCIe_COMPLEX_DESCRIPTOR         *Complex,
  IN      UINTN                           Index
  )
{
  UINTN PcieListlength;
  ASSERT (Index < (PcieInputParserGetNumberOfEngines (Complex)));
  PcieListlength = PcieInputParserGetLengthOfPcieEnginesList (Complex);
  if (Index < PcieListlength) {
    return (PCIe_ENGINE_DESCRIPTOR*) &((Complex->PciePortList)[Index]);
  } else {
    return (PCIe_ENGINE_DESCRIPTOR*) &((Complex->DdiLinkList)[Index - PcieListlength]);
  }
}


