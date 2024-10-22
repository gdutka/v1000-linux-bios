/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD OEM Agesa Customization Function Prototype Header File
 *
 * This file is placed in the user's platform directory and contains the
 * build option selections desired for that platform.
 *
 * For Information about this file, see @ref platforminstall.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  UEFI
 * @e \$Revision$   @e \$Date$
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _OEM_AGESA_CCX_PLATFORM_LIB_H_
#define _OEM_AGESA_CCX_PLATFORM_LIB_H_

/*---------------------------------------------------------------------------------------*/
/**
 *  SaveApInitVector
 *
 *  Description:
 *    This function will store the offset of the function which can be invoked
 *    by AP after setting up the Cache as RAM and bypass PeiCore, and PEI Services
 *    to perform AGESA AmdInitReset and AmdInitEarly. Since APs, as of now, do not
 *    use any of the PEI Core and PEI Services.
 *  Parameters:
 *    @param[in]          ApInitAddress
 *    @param[in, out]     *ContentToRestore
 *
 *    @retval         VOID
 *
 **/
/*---------------------------------------------------------------------------------------*/
VOID
SaveApInitVector (
  IN       UINT32  ApInitAddress,
  IN OUT   UINT32  *ContentToRestore
  );

/*---------------------------------------------------------------------------------------*/
/**
 *  RestoreContentVector
 *
 *  Description:
 *    This function will restore the previous content of the location
 *    where ApInit vector is being saved. It is upto platform owner if
 *    this function need to be implemented. In case there is no implementation,
 *    this function need to be present for compatibility reasons.
 *  Parameters:
 *    @param[in]      ContentToRestore
 *
 *    @retval         VOID
 *
 **/
/*---------------------------------------------------------------------------------------*/
VOID
RestoreContentVector (
  IN       UINT32  ContentToRestore
  );

#endif
