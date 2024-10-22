/* $NoKeywords:$ */
/**
 * @file
 *
 * mfs3.c
 *
 * Main S3 resume memory Entrypoint file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/FEAT/S3)
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "S3.h"
#include "Library/AmdMemFeatS3Lib.h"
#include "Library/AmdHeapLib.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_NULL_FEATS3LIBNULL_MFS3_FILECODE


extern MEM_NB_SUPPORT memNBInstalled[];

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function is the main memory entry point for the S3 resume sequence
 *      Requirements:
 *
 *      Run-Time Requirements:
 *      1. Complete Hypertransport Bus Configuration
 *      4. BSP in Big Real Mode
 *      5. Stack available
 *
 *      @param[in]  *StdHeader - Config handle for library and services
 *
 *      @return     AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
AmdMemS3Resume (
  IN   AMD_CONFIG_PARAMS *StdHeader
  )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function deallocates heap space allocated in memory S3 resume.
 *
 *      @param[in]  *StdHeader - Config handle for library and services
 *
 *      @return     AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
MemS3Deallocate (
  IN   AMD_CONFIG_PARAMS *StdHeader
  )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function is the entrance to get device list for memory registers.
 *
 *      @param[in, out]  **DeviceBlockHdrPtr - Pointer to the memory containing the
 *                                             device descriptor list
 *      @param[in]       *StdHeader - Config handle for library and services
 *      @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
MemFS3GetDeviceList (
  IN OUT   DEVICE_BLOCK_HEADER **DeviceBlockHdrPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  return AGESA_SUCCESS;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function initialize the northbridge block and apply for heap space
 *      before any function call is made to memory component during S3 resume.
 *
 *      @param[in]       *StdHeader - Config handle for library and services
 *      @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
MemS3ResumeInitNB (
  IN   AMD_CONFIG_PARAMS *StdHeader
  )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the PCI device register list according to the register
 *      list ID.
 *
 *      @param[in]       *Device - pointer to the PCI_DEVICE_DESCRIPTOR
 *      @param[out]      **RegisterHdr - pointer to the address of the register list
 *      @param[in]       *StdHeader - Config handle for library and services
 *
 *      @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
MemFS3GetPciDeviceRegisterList (
  IN       PCI_DEVICE_DESCRIPTOR     *Device,
     OUT   PCI_REGISTER_BLOCK_HEADER **RegisterHdr,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the conditional PCI device register list according
 *      to the register list ID.
 *
 *      @param[in]       *Device - pointer to the CONDITIONAL_PCI_DEVICE_DESCRIPTOR
 *      @param[out]      **RegisterHdr - pointer to the address of the register list
 *      @param[in]       *StdHeader - Config handle for library and services
 *
 *      @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
MemFS3GetCPciDeviceRegisterList (
  IN       CONDITIONAL_PCI_DEVICE_DESCRIPTOR *Device,
     OUT   CPCI_REGISTER_BLOCK_HEADER        **RegisterHdr,
  IN       AMD_CONFIG_PARAMS                 *StdHeader
  )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the MSR device register list according to the register
 *      list ID.
 *
 *      @param[in]       *Device - pointer to the MSR_DEVICE_DESCRIPTOR
 *      @param[out]      **RegisterHdr - pointer to the address of the register list
 *      @param[in]       *StdHeader - Config handle for library and services
 *
 *      @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
MemFS3GetMsrDeviceRegisterList (
  IN       MSR_DEVICE_DESCRIPTOR     *Device,
     OUT   MSR_REGISTER_BLOCK_HEADER **RegisterHdr,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the conditional MSR device register list according
 *      to the register list ID.
 *
 *      @param[in]       *Device - pointer to the CONDITIONAL_PCI_DEVICE_DESCRIPTOR
 *      @param[out]      **RegisterHdr - pointer to the address of the register list
 *      @param[in]       *StdHeader - Config handle for library and services
 *
 *      @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
MemFS3GetCMsrDeviceRegisterList (
  IN       CONDITIONAL_MSR_DEVICE_DESCRIPTOR     *Device,
     OUT   CMSR_REGISTER_BLOCK_HEADER            **RegisterHdr,
  IN       AMD_CONFIG_PARAMS                     *StdHeader
  )
{
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function initialize needed data structures for S3 resume.
 *
 *      @param[in, out]  **S3NBPtr - Pointer to the pointer of northbridge block.
 *      @param[in, out]  *MemPtr - Pointer to MEM_DATA_STRUCT.
 *      @param[in, out]  *mmData - Pointer to MEM_MAIN_DATA_BLOCK.
 *      @param[in]       *StdHeader - Config handle for library and services.
 *
 *      @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
MemS3InitNB (
  IN OUT   S3_MEM_NB_BLOCK **S3NBPtr,
  IN OUT   MEM_DATA_STRUCT **MemPtr,
  IN OUT   MEM_MAIN_DATA_BLOCK *mmData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function determines if the PSP is present
 *
 *      @param[in]       *StdHeader - Config handle for library and services.
 *
 *      @return          AGESA_STATUS
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 */
AGESA_STATUS
AmdMemDoResume (
  IN   AMD_CONFIG_PARAMS *StdHeader
  )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function determines if the revision S3 data blob matches the S3 engine
 *
 *      @param[in]       Storage    - Beginning of the device list.
 *      @param[in]       *StdHeader - Config handle for library and services.
 *
 *      @return          AGESA_STATUS
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 */
AGESA_STATUS
AmdMemS3DatablobRevMatch (
  IN   VOID              *Storage,
  IN   AMD_CONFIG_PARAMS *StdHeader
  )
{
  return AGESA_SUCCESS;
}

