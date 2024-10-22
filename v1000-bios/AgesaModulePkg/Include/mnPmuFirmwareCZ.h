/* $NoKeywords:$ */
/**
 * @file
 *
 * mnPmuFirmwareCZ.h
 *
 * PMU Firmware for CZ
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/CZ)
 * @e \$Revision: 311790 $ @e \$Date: 2015-01-27 13:03:49 +0800 (Tue, 27 Jan 2015) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#ifndef _MNPMUFIRMWARECZ_H_
#define _MNPMUFIRMWARECZ_H_

// PMU package title, "Carrizo        "
// This string MUST be exactly 16 characters long
#define PMU_PACKAGE_STRING  {'C', 'a', 'r', 'r', 'i', 'z', 'o', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}

// PMU SRAM size for DMEM/IMEM
#define PMU_SRAM_DMEM_SIZE_CZ      0x2000  ///< PMU DMEM size is 8K Bytes
#define PMU_SRAM_IMEM_SIZE_CZ      0x4000  ///< PMU IMEM size is 16K Bytes
#define PMU_IMAGE_MIN_CZ           1       ///< CZ carry 1 PMU images as min.
#define PMU_IMAGE_MAX_CZ           2       ///< CZ carry 2 PMU images as max.

#endif /* _MNPMUFIRMWARECZ_H_ */

