/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access various CPU registers.
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
#include  "GnbLibPciAcc.h"
#include  "GnbLibCpuAcc.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBCOMMONLIB_GNBLIBCPUACC_FILECODE
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
 * Read CPU (DCT) indirect registers
 *
 *
 *
 * @param[in]  Address         PCI address of DCT register
 * @param[in]  IndirectAddress Offset of DCT register
 * @param[out] Value           Pointer to value
 * @param[in]  Config          Pointer to standard header
 */
VOID
GnbLibCpuPciIndirectRead (
  IN       UINT32        Address,
  IN       UINT32        IndirectAddress,
     OUT   UINT32        *Value,
  IN       VOID          *Config
  )
{
  UINT32  OffsetRegisterValue;
  GnbLibPciWrite (Address, AccessWidth32, &IndirectAddress, Config);
  do {
    GnbLibPciRead (Address , AccessWidth32, &OffsetRegisterValue, Config);
  } while ((OffsetRegisterValue & BIT31) == 0);
  GnbLibPciRead (Address + 4, AccessWidth32, Value, Config);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Write CPU (DCT) indirect registers
 *
 *
 *
 * @param[in] Address         PCI address of DCT register
 * @param[in] IndirectAddress Offset of DCT register
 * @param[in] Value           Pointer to value
 * @param[in] Config          Pointer to standard header
 */
VOID
GnbLibCpuPciIndirectWrite (
  IN      UINT32        Address,
  IN      UINT32        IndirectAddress,
  IN      UINT32        *Value,
  IN      VOID          *Config
  )
{
  UINT32  OffsetRegisterValue;
  OffsetRegisterValue = IndirectAddress | BIT30;
  GnbLibPciWrite (Address + 4, AccessWidth32, Value, Config);
  GnbLibPciWrite (Address, AccessWidth32, &IndirectAddress, Config);
  do {
    GnbLibPciRead (Address , AccessWidth32, &OffsetRegisterValue, Config);
  } while ((OffsetRegisterValue & BIT31) == 0);
}



