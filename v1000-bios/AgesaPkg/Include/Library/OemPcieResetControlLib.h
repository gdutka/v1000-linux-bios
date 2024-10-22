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
#ifndef _OEM_PCIE_SLOT_RESET_CONTROL_LIB_H_
#define _OEM_PCIE_SLOT_RESET_CONTROL_LIB_H_

/**
 * Callout method to the host environment.
 *
 * Callout using a dispatch with appropriate thunk layer, which is determined by the host environment.
 *
 * @param[in]        Function      The specific callout function being invoked.
 * @param[in]        FcnData       Function specific data item.
 * @param[in,out]    ConfigPtr     Reference to Callout params.
 */

///Slot Reset Info
typedef struct {
  IN      AMD_CONFIG_PARAMS     StdHeader;                ///< Standard configuration header
  IN      UINT8                 ResetId;                  ///< Slot reset ID as specified in PCIe_PORT_DESCRIPTOR
  IN      UINT8                 ResetControl;             ///< Reset control as in PCIE_RESET_CONTROL
} PCIe_SLOT_RESET_INFO;


/*---------------------------------------------------------------------------------------*/
/**
 *  PCIE slot reset control.
 *
 *  Description:
 *
 *  Parameters:
 *    @param[in]       FcnData    Function data
 *    @param[in, out]  ResetInfo  Reset information
 *
 *    @retval         AGESA_SUCCESS       Function returns successfully
 *    @retval         AGESA_UNSUPPORTED   Function is not supported
 *
 **/
/*---------------------------------------------------------------------------------------*/
EFI_STATUS
AgesaPcieSlotResetControl (
  IN      UINTN                         FcnData,
  IN      PCIe_SLOT_RESET_INFO          *ResetInfo
  );

#endif

