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
#ifndef _OEM_GPIO_RESET_CONTROL_LIB_H_
#define _OEM_GPIO_RESET_CONTROL_LIB_H_

///Gpio Reset Info
typedef struct {
  IN      UINT32                 ResetId;                  ///< Slot reset ID as specified in GPIO_PORT_DESCRIPTOR
  IN      UINT32                 ResetControl;             ///< Reset control as in GPIO_RESET_CONTROL
} GPIO_RESET_INFO;

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
AgesaGpioSlotResetControl (
  IN      UINTN                         FcnData,
  IN      GPIO_RESET_INFO               *ResetInfo
  );

#endif

