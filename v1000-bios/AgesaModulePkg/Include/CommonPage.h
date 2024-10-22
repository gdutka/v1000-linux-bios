/* $NoKeywords:$ */
/**
 * @file
 *
 * Create outline and references for Processor Common Component mainpage documentation.
 *
 * Design guides, maintenance guides, and general documentation, are
 * collected using this file onto the documentation mainpage.
 * This file contains doxygen comment blocks, only.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Documentation
 * @e \$Revision: 334519 $   @e \$Date: 2016-01-15 15:48:51 +0800 (Fri, 15 Jan 2016) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/**
 * @page commonmain Processor Common Component Documentation
 *
 * Additional documentation for the Common component consists of
 *
 * - Maintenance Guides:
 *   - @subpage amdconfigparamname "Naming Guidelines for type AMD_CONFIG_PARAMS"
 * - Design Guides:
 *   - add here >>>
 *
 */

/**
 * @page amdconfigparamname Naming Guidelines for type AMD_CONFIG_PARAMS
 * @par
 * These are the guidelines for naming objects of type AMD_CONFIG_PARAMS and AMD_CONFIG_PARAMS * in AGESA code.
 * <ul>
 *
 * <li>
 * Formal parameter names of type AMD_CONFIG_PARAMS and AMD_CONFIG_PARAMS * will always be named
 * StdHeader. This covers all function prototypes, function definitions, and method typedefs (a
 * typedef of a function prototype) in AGESA code. Examples:
 * @n @code
 * VOID
 *   LibAmdPciFindNextCap (
 *     IN OUT   PCI_ADDR *Address,
 *     IN       AMD_CONFIG_PARAMS *StdHeader
 *     )
 *
 * typedef VOID F_DO_TABLE_ENTRY (
 *   IN       TABLE_ENTRY_DATA       *CurrentEntry,
 *   IN       PLATFORM_CONFIGURATION *PlatformConfig,
 *   IN       AMD_CONFIG_PARAMS      *StdHeader
 *   );
 *
 * @endcode
 *
 * <li>
 * Structure members of type AMD_CONFIG_PARAMS or AMD_CONFIG_PARAMS * will always be named StdHeader. Examples:
 * @n @code
   /// Example of struct member naming.
 * typedef struct {
 *   IN OUT   AMD_CONFIG_PARAMS StdHeader;             ///< Standard Header
 *   IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
 * } AMD_CPU_RECOVERY_PARAMS;
 *
 * @endcode
 *
 * <li>
 * Routines which define local variables of type AMD_CONFIG_PARAMS or AMD_CONFIG_PARAMS * should
 * name the local variable as closely as practical to StdHeader, but otherwise freedom is allowed. Example:
 * @n @code
 * AMD_CONFIG_PARAMS *NewStdHeader;
 * [...]
 * NewStdHeader = (AMD_CONFIG_PARAMS *)AllocHeapParams.BufferPtr;
 * @endcode
 *
 * <li>
 * Arguments to routines with AMD_CONFIG_PARAMS or AMD_CONFIG_PARAMS * formal parameters are not
 * checked.  Freedom is allowed in order to conform to these guidelines in a practical, readable
 * way.  This includes typecast arguments.  Examples:
 * @n @code
 * Status = GetEventLog (&LogEvent, (AMD_CONFIG_PARAMS *)Event);
 *
 * MemS3ExitSelfRefRegDA (NBPtr, &MemPtr->StdHeader);
 * @endcode
 *
 * </ul>
 *
 */

