/* $NoKeywords:$ */
/**
 * @file
 *
 * mnS3.c
 *
 * Common Northbridge S3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include <Library/BaseLib.h>
#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdCalloutLib.h"
#include "Library/CcxPstatesLib.h"
#include "Library/IdsLib.h"
#include <Library/PciLib.h>
#include "Ids.h"
#include "OptionMemory.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "S3.h"
#include "Library/AmdMemFeatS3Lib.h"
#include "Library/AmdHeapLib.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE LIBRARY_MEM_NBLIB_MNS3_FILECODE
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
VOID
STATIC
MemNS3GetSetBitField (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN       BOOLEAN IsSet,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function executes the S3 resume for a node on a UNB
 *
 *     @param[in,out]   *S3NBPtr - Pointer to the S3_MEM_NB_BLOCK
 *     @param[in]       NodeID - The Node id of the target die
 *
 *     @return         BOOLEAN
 *                         TRUE - This is the correct constructor for the targeted node.
 *                         FALSE - This isn't the correct constructor for the targeted node.
 */
BOOLEAN
MemNS3ResumeUNb (
  IN OUT   S3_MEM_NB_BLOCK *S3NBPtr,
  IN       UINT8 NodeID
  )
{
  UINT8 DCT;
  MEM_NB_BLOCK *NBPtr;
  MEM_DATA_STRUCT *MemPtr;

  NBPtr = S3NBPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  // Errata before S3 resume sequence

  // Add a hook here
  AGESA_TESTPOINT (TpProcMemBeforeAgesaHookBeforeExitSelfRef, &MemPtr->StdHeader);
  if (AgesaHookBeforeExitSelfRefresh (0, MemPtr) == AGESA_SUCCESS) {
  }
  AGESA_TESTPOINT (TpProcMemAfterAgesaHookBeforeExitSelfRef, &MemPtr->StdHeader);

  //Override the NB Pstate if needed
  IDS_OPTION_HOOK (IDS_NB_PSTATE_DIDVID, S3NBPtr->NBPtr, &MemPtr->StdHeader);
  // Set F2x[1,0]90[ExitSelfRef]
  // Wait for F2x[1,0]90[ExitSelfRef]=0
  for (DCT = 0; DCT < NBPtr->DctCount; DCT ++) {
    MemNSwitchDCTNb (NBPtr, DCT);
    if (MemNGetBitFieldNb (NBPtr, BFDisDramInterface) == 0) {
      MemNSetBitFieldNb (NBPtr, BFDisDllShutdownSR, 1);
      MemNSetBitFieldNb (NBPtr, BFExitSelfRef, 1);
      while (MemNGetBitFieldNb (NBPtr, BFExitSelfRef) != 0) {}
      if (NBPtr->IsSupported[SetDllShutDown]) {
        MemNSetBitFieldNb (NBPtr, BFDisDllShutdownSR, 0);
      }
    }
  }

  // Errata After S3 resume sequence
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function reads register bitfield
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value be read.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3GetBitFieldNb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  MemNS3GetSetBitField (AccessWidth, Address, FALSE, Value, ConfigPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function writes register bitfield
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3SetBitFieldNb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  MemNS3GetSetBitField (AccessWidth, Address, TRUE, Value, ConfigPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function restores scrubber base register
 *
 *     @param[in,out]   *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Node - The Node id of the target die
 *
 */
VOID
MemNS3RestoreScrubNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Node
  )
{
  UINT32 ScrubAddrRJ16;

  ScrubAddrRJ16 = (MemNGetBitFieldNb (NBPtr, BFDramBaseReg0 + Node) & 0xFFFF0000) >> 8;
  ScrubAddrRJ16 |= MemNGetBitFieldNb (NBPtr, BFDramBaseHiReg0 + Node) << 24;
  MemNSetBitFieldNb (NBPtr, BFScrubAddrLoReg, ScrubAddrRJ16 << 16);
  MemNSetBitFieldNb (NBPtr, BFScrubAddrHiReg, ScrubAddrRJ16 >> 16);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function retores Pre Driver Calibration with pre driver calibration code
 *   code valid bit set.
 *
 *     @param[in]   AccessWidth - Access width of the register.
 *     @param[in]   Address - address in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3SetPreDriverCalUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT16 RegValue;

  RegValue = 0x8000 | *(UINT16 *) Value;
  MemNS3SetBitFieldNb (AccessS3SaveWidth16, Address, &RegValue, ConfigPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function is used by families that use a separate DctCfgSel bit to
 *     select the current DCT which will be accessed by function 2.
 *     NOTE:  This function must be called BEFORE the NBPtr->Dct variable is
 *     updated.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *Dct     - Pointer to ID of the target DCT
 *
 */

BOOLEAN
MemNS3DctCfgSelectUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *Dct
  )
{
  // Set the DctCfgSel to new DCT
  //
  MemNSetBitFieldNb (NBPtr, BFDctCfgSel, *(UINT8*)Dct);

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function write to a register that has one copy for each NB Pstate
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value be read.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3GetNBPStateDepRegUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT8 NBPstate;
  UINT8 TempValue;
  UINT8 Dct;
  UINT32 Temp;

  Temp = Address.Address.Register;
  NBPstate = (UINT8) (Temp >> 10);
  Dct = (UINT8) Address.Address.Function;
  Temp &= 0x3FF;

  // Switch Dct
  // Function field contains DCT value
  Address.Address.Function = FUNC_1;
  Address.Address.Register = 0x10C;
  TempValue = PciRead8 (Address.AddressValue);
  TempValue = (TempValue & 0xC8) | ((NBPstate << 4) | Dct);
  PciWrite8 (Address.AddressValue, TempValue);

  Address.Address.Function = FUNC_2;
  Address.Address.Register = Temp;

  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    *((UINT8 *) Value) = PciRead8 (Address.AddressValue);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    *((UINT16 *) Value) = PciRead16 (Address.AddressValue);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    *((UINT32 *) Value) = PciRead32 (Address.AddressValue);
    break;
  default:
    IDS_ERROR_TRAP;
  }

  Address.Address.Function = FUNC_1;
  Address.Address.Register = 0x10C;
  TempValue = 0;
  PciWrite8 (Address.AddressValue, TempValue);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function write to a register that has one copy for each NB Pstate
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value be read.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3SetNBPStateDepRegUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT8 NBPstate;
  UINT8 TempValue;
  UINT8 Dct;
  UINT32 Temp;

  Temp = Address.Address.Register;
  NBPstate = (UINT8) (Temp >> 10);
  Dct = (UINT8) Address.Address.Function;
  Temp &= 0x3FF;

  // Switch Dct
  // Function field contains DCT value
  Address.Address.Function = FUNC_1;
  Address.Address.Register = 0x10C;
  TempValue = PciRead8 (Address.AddressValue);
  TempValue = (TempValue & 0xCE) | ((NBPstate << 4) | Dct);
  PciWrite8 (Address.AddressValue, TempValue);

  Address.Address.Function = FUNC_2;
  Address.Address.Register = Temp;

  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    PciWrite8 (Address.AddressValue, *((UINT8 *) Value));
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    PciWrite16 (Address.AddressValue, *((UINT16 *) Value));
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    PciWrite32 (Address.AddressValue, *((UINT32 *) Value));
    break;
  default:
    IDS_ERROR_TRAP;
  }

  Address.Address.Function = FUNC_1;
  Address.Address.Register = 0x10C;
  TempValue = 0;
  PciWrite32 (Address.AddressValue, TempValue);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function read the value of Function 2 PCI register.
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the NB register in PCI_ADDR format.
 *     @param[in]  *Value - Pointer to the value be read.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3SaveNBRegisterUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT8 TempValue;
  UINT8 Dct;
  UINT32 Temp;

  Temp = Address.Address.Register;
  Dct = (UINT8) Address.Address.Function;

  // Switch Dct
  // Function field contains DCT value
  Address.Address.Function = FUNC_1;
  Address.Address.Register = 0x10C;
  TempValue = PciRead8 (Address.AddressValue);
  TempValue = (TempValue & 0xFE) | Dct;
  PciWrite8 (Address.AddressValue, TempValue);

  Address.Address.Register = Temp;
  Address.Address.Function = FUNC_2;

  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    *((UINT8 *) Value) = PciRead8 (Address.AddressValue);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    *((UINT16 *) Value) = PciRead16 (Address.AddressValue);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    *((UINT32 *) Value) = PciRead32 (Address.AddressValue);
    break;
  default:
    IDS_ERROR_TRAP;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function set the value of Function 2 PCI register.
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the NB register in PCI_ADDR format.
 *     @param[in]  *Value - Pointer to the value be write.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3RestoreNBRegisterUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT8 TempValue;
  UINT8 Dct;
  UINT32 Temp;

  Temp = Address.Address.Register;
  Dct = (UINT8) Address.Address.Function;

  // Switch Dct
  // Function field contains DCT value
  Address.Address.Function = FUNC_1;
  Address.Address.Register = 0x10C;
  TempValue = PciRead8 (Address.AddressValue);
  TempValue = (TempValue & 0xFE) | Dct;
  PciWrite8 (Address.AddressValue, TempValue);

  Address.Address.Register = Temp;
  Address.Address.Function = FUNC_2;

  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    PciWrite8 (Address.AddressValue, *((UINT8 *) Value));
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    PciWrite16 (Address.AddressValue, *((UINT16 *) Value));
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    PciWrite32 (Address.AddressValue, *((UINT32 *) Value));
    break;
  default:
    IDS_ERROR_TRAP;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets MemClkFreqVal bit.
 *
 *     @param[in]   AccessWidth - Access width of the register.
 *     @param[in]   Address - address in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3SetMemClkFreqValUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT32 TempValue;

  // 1. Program MemClkFreqVal = 1
  MemNS3SaveNBRegisterUnb (AccessWidth, Address, &TempValue, ConfigPtr);
  TempValue |= 0x80;
  MemNS3RestoreNBRegisterUnb (AccessWidth, Address, &TempValue, ConfigPtr);

  // 2. Wait for FreqChgInPrg = 0
  MemNS3SaveNBRegisterUnb (AccessWidth, Address, &TempValue, ConfigPtr);
  while ((TempValue & 0x200000) != 0) {
    MemNS3SaveNBRegisterUnb (AccessWidth, Address, &TempValue, ConfigPtr);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function changes memory Pstate context
 *
 *     @param[in]   AccessWidth - Access width of the register.
 *     @param[in]   Address - address in PCI_ADDR format. Target MemPState is in
 *                            Address.Address.Register.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
VOID
MemNS3ChangeMemPStateContextNb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  LOCATE_HEAP_PTR LocateBufferPtr;
  S3_MEM_NB_BLOCK *S3NBPtr;
  UINT8 Die;

  IDS_SKIP_HOOK (IDS_BEFORE_S3_SPECIAL, NULL, ConfigPtr) {
    // See which Node should be accessed
    Die = (UINT8) (Address.Address.Device - 24);

    LocateBufferPtr.BufferHandle = AMD_MEM_S3_NB_HANDLE;
    if (HeapLocateBuffer (&LocateBufferPtr, ConfigPtr) == AGESA_SUCCESS) {
      S3NBPtr = (S3_MEM_NB_BLOCK *) LocateBufferPtr.BufferPtr;
      NBPtr = S3NBPtr[Die].NBPtr;
      MemNChangeMemPStateContextNb (NBPtr, (MEM_PSTATE)Address.Address.Register);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function forces NBPstate to NBP0
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value be read or  written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3ForceNBP0Unb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT8   NbPstateMaxVal;
  MEM_NB_BLOCK *NBPtr;
  LOCATE_HEAP_PTR LocateBufferPtr;
  S3_MEM_NB_BLOCK *S3NBPtr;
  BOOLEAN SkipTransToLo;
  UINTN   VoltageInuV;
  UINTN   PowerInmW;

  IDS_SKIP_HOOK (IDS_BEFORE_S3_SPECIAL, NULL, ConfigPtr) {
    LocateBufferPtr.BufferHandle = AMD_MEM_S3_NB_HANDLE;
    if (HeapLocateBuffer (&LocateBufferPtr, ConfigPtr) == AGESA_SUCCESS) {
      S3NBPtr = (S3_MEM_NB_BLOCK *) LocateBufferPtr.BufferPtr;
      NBPtr = S3NBPtr[0].NBPtr;

      // Find out if Current NBPstate is NBPstateLo or not
      // If yes, skip the steps that transition the Pstate to Lo
      if (!NBPtr->MemRunningOnPsp (NBPtr)) {
        SkipTransToLo = FALSE;
        if (MemNGetBitFieldNb (NBPtr, BFCurNbPstateLo) == 1) {
          SkipTransToLo = TRUE;
        }

        if (MemNGetBitFieldNb (NBPtr, BFCurNbPstate) != 0) {

          NBPtr->NbPsCtlReg = MemNGetBitFieldNb (NBPtr, BFNbPstateCtlReg);

          // If current NBPstate is already in NBPstateLo, do not do transition to NBPstateLo.
          if (!SkipTransToLo) {
            // Switch to NbPstateHi in order to modify NbPstateLo
            MemNSetBitFieldNb (NBPtr, BFSwNbPstateLoDis, 1);
            while (MemNGetBitFieldNb (NBPtr, BFCurNbPstateLo) != 0) {}
            while (MemNGetBitFieldNb (NBPtr, BFNbPstateReqBusy) != 0) {}

            // 2.Program D18F5x170 to transition the NB P-state:
            //   NbPstateLo = NbPstateMaxVal. (HW requires an intermediate transition to low)
            //   SwNbPstateLoDis = NbPstateDisOnP0 = NbPstateThreshold = 0.
            NbPstateMaxVal = (UINT8) MemNGetBitFieldNb (NBPtr, BFNbPstateMaxVal);
            MemNSetBitFieldNb (NBPtr, BFNbPstateLo, NbPstateMaxVal);
            MemNSetBitFieldNb (NBPtr, BFNbPstateCtlReg, MemNGetBitFieldNb (NBPtr, BFNbPstateCtlReg) & 0xFFFF81FF);
            // 3.Wait for D18F5x174[CurNbPstate] to equal NbPstateLo.
            while (MemNGetBitFieldNb (NBPtr, BFCurNbPstateLo) != 1) {}
            while (MemNGetBitFieldNb (NBPtr, BFNbPstateReqBusy) != 0) {}
          }
          // 4.Program D18F5x170 to force the NB P-state:
          //   NbPstateHi = target NB P-state.
          //   SwNbPstateLoDis = 1 (triggers the transition)
          MemNSetBitFieldNb (NBPtr, BFNbPstateHi, 0);
          MemNSetBitFieldNb (NBPtr, BFSwNbPstateLoDis, 1);
          // 5.Wait for D18F5x174[CurNbPstate] to equal the target NB P-state.
          while (MemNGetBitFieldNb (NBPtr, BFCurNbPstateLo) != 0) {}
          while (MemNGetBitFieldNb (NBPtr, BFNbPstateReqBusy) != 0) {}

          // Update TSC rate
          CcxGetPstateInfo (0, SwPstate0, (UINTN *)&NBPtr->MemPtr->TscRate, &VoltageInuV, &PowerInmW, ConfigPtr);
        }
      }
    } else {
      ASSERT (FALSE);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function releases NBPState force
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value be read or  written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3ReleaseNBPSUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  LOCATE_HEAP_PTR LocateBufferPtr;
  S3_MEM_NB_BLOCK *S3NBPtr;

  IDS_SKIP_HOOK (IDS_BEFORE_S3_SPECIAL, NULL, ConfigPtr) {
    LocateBufferPtr.BufferHandle = AMD_MEM_S3_NB_HANDLE;
    if (HeapLocateBuffer (&LocateBufferPtr, ConfigPtr) == AGESA_SUCCESS) {
      S3NBPtr = (S3_MEM_NB_BLOCK *) LocateBufferPtr.BufferPtr;
      NBPtr = S3NBPtr[0].NBPtr;

      if (!NBPtr->MemRunningOnPsp (NBPtr)) {
        if (NBPtr->NbPsCtlReg != 0) {
          // Switch to NBPstateLo before restoring NBPstateHi
          //   SwNbPstateLoDis = NbPstateDisOnP0 = NbPstateThreshold = 0.
          MemNSetBitFieldNb (NBPtr, BFNbPstateCtlReg, MemNGetBitFieldNb (NBPtr, BFNbPstateCtlReg) & 0xFFFF81FF);
          while (MemNGetBitFieldNb (NBPtr, BFCurNbPstateLo) != 1) {}
          while (MemNGetBitFieldNb (NBPtr, BFNbPstateReqBusy) != 0) {}

          // 7. Restore the initial D18F5x170[NbPstateThreshold, NbPstateHi] values.
          MemNSetBitFieldNb (NBPtr, BFNbPstateCtlReg, (MemNGetBitFieldNb (NBPtr, BFNbPstateCtlReg) & 0xFFFFE13F) | (NBPtr->NbPsCtlReg & 0x1EC0));

          // Switch to NBPstateHi before restoring NBPstateLo
          MemNSetBitFieldNb (NBPtr, BFSwNbPstateLoDis, 1);
          while (MemNGetBitFieldNb (NBPtr, BFCurNbPstateLo) != 0) {}
          while (MemNGetBitFieldNb (NBPtr, BFNbPstateReqBusy) != 0) {}

          // 8. Restore the initial D18F5x170[NbPstateLo] values.
          MemNSetBitFieldNb (NBPtr, BFNbPstateLo, (NBPtr->NbPsCtlReg >> 3) & 3);

          // 6. Restore the initial D18F5x170[SwNbPstateLoDis, NbPstateDisOnP0] values.
          MemNSetBitFieldNb (NBPtr, BFNbPstateCtlReg, (MemNGetBitFieldNb (NBPtr, BFNbPstateCtlReg) & 0xFFFF9FFF) | (NBPtr->NbPsCtlReg & 0x6000));
        }
      }
    } else {
      ASSERT (FALSE);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function saves hob data into NV Ram.
 *
 *     @param[in]   AccessWidth - Access width of the register.
 *     @param[in]   Address - address in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNSaveHobDataUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  LOCATE_HEAP_PTR LocateBufferPtr;
  S3_MEM_NB_BLOCK *S3NBPtr;
  UINT8 Die;

  // See which Node should be accessed
  Die = (UINT8) (Address.Address.Device - 24);

  if (Die == 0) {
    // Only do this on first node
    LocateBufferPtr.BufferHandle = AMD_MEM_S3_NB_HANDLE;
    if (HeapLocateBuffer (&LocateBufferPtr, ConfigPtr) == AGESA_SUCCESS) {
      S3NBPtr = (S3_MEM_NB_BLOCK *) LocateBufferPtr.BufferPtr;
      NBPtr = S3NBPtr[Die].NBPtr;

      // Only save value when RefPtr is not NULL
      if (NBPtr->RefPtr != NULL) {
        if (Address.Address.Function == S3_UMA_SIZE) {
          *(UINT32 *) Value = NBPtr->RefPtr->UmaSize;
        } else if (Address.Address.Function == S3_UMA_BASE) {
          *(UINT32 *) Value = NBPtr->RefPtr->UmaBase;
        } else if (Address.Address.Function == S3_UMA_MODE) {
          *(UINT8 *) Value = (UINT8 ) NBPtr->RefPtr->UmaMode;
        } else if (Address.Address.Function == S3_SUB_4G_CACHE_TOP) {
          *(UINT32 *) Value = NBPtr->RefPtr->Sub4GCacheTop;
        } else if (Address.Address.Function == S3_SYSLIMIT) {
          *(UINT32 *) Value = NBPtr->RefPtr->SysLimit;
        } else if (Address.Address.Function == S3_UMA_ATTRIBUTE) {
          LocateBufferPtr.BufferHandle = AMD_UMA_INFO_HANDLE;
          if (HeapLocateBuffer (&LocateBufferPtr, ConfigPtr) == AGESA_SUCCESS) {
            *(UINT32 *) Value = ((UMA_INFO *) LocateBufferPtr.BufferPtr)->UmaAttributes;
          }
        } else if (Address.Address.Function == S3_VDDIO) {
          *(UINT8 *) Value = (UINT8) NBPtr->RefPtr->DDRVoltage;
        } else {
          ASSERT (FALSE);
        }
      }
    } else {
      ASSERT (FALSE);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets hob data from NV Ram.
 *
 *     @param[in]   AccessWidth - Access width of the register.
 *     @param[in]   Address - address in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNRestoreHobDataUnb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  LOCATE_HEAP_PTR LocateBufferPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  S3_MEM_NB_BLOCK *S3NBPtr;
  UINT8 Die;

  // See which Node should be accessed
  Die = (UINT8) (Address.Address.Device - 24);

  if (Die == 0) {
    // Only do this on first node
    LocateBufferPtr.BufferHandle = AMD_MEM_S3_NB_HANDLE;
    if (HeapLocateBuffer (&LocateBufferPtr, ConfigPtr) == AGESA_SUCCESS) {
      S3NBPtr = (S3_MEM_NB_BLOCK *) LocateBufferPtr.BufferPtr;
      NBPtr = S3NBPtr[Die].NBPtr;

      // Only save value when RefPtr is not NULL
      if (NBPtr->RefPtr != NULL) {
        if (Address.Address.Function == S3_UMA_SIZE) {
          NBPtr->RefPtr->UmaSize = *(UINT32 *) Value;
        } else if (Address.Address.Function == S3_UMA_BASE) {
          NBPtr->RefPtr->UmaBase = *(UINT32 *) Value;
        } else if (Address.Address.Function == S3_UMA_MODE) {
          NBPtr->RefPtr->UmaMode = (UMA_MODE) (*(UINT8 *) Value);
        } else if (Address.Address.Function == S3_SUB_4G_CACHE_TOP) {
          NBPtr->RefPtr->Sub4GCacheTop = *(UINT32 *) Value;
        } else if (Address.Address.Function == S3_SYSLIMIT) {
          NBPtr->RefPtr->SysLimit = *(UINT32 *) Value;
        } else if (Address.Address.Function == S3_UMA_ATTRIBUTE) {
          // Allocate heap for UMA_INFO
          AllocHeapParams.RequestedBufferSize = sizeof (UMA_INFO);
          AllocHeapParams.BufferHandle = AMD_UMA_INFO_HANDLE;
          AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
          if (HeapAllocateBuffer (&AllocHeapParams, ConfigPtr) == AGESA_SUCCESS) {
            ((UMA_INFO *) AllocHeapParams.BufferPtr)->UmaAttributes = *(UINT32 *) Value;
            ((UMA_INFO *) AllocHeapParams.BufferPtr)->UmaMode = (UINT8) NBPtr->RefPtr->UmaMode;
            ((UMA_INFO *) AllocHeapParams.BufferPtr)->UmaBase = (UINT64) ((UINT64) NBPtr->RefPtr->UmaBase << 16);
            ((UMA_INFO *) AllocHeapParams.BufferPtr)->UmaSize = (NBPtr->RefPtr->UmaSize) << 16;
            ((UMA_INFO *) AllocHeapParams.BufferPtr)->MemClock = MemNS3GetMemClkFreqUnb (NBPtr, (UINT8) NBPtr->GetBitField (NBPtr, BFMemClkFreq));
          }
        } else if (Address.Address.Function == S3_VDDIO) {
          NBPtr->RefPtr->DDRVoltage = (DIMM_VOLTAGE) *(UINT8 *) Value;
        } else {
          ASSERT (FALSE);
        }
      }
    } else {
      ASSERT (FALSE);
    }
  }
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function enables and disables the fixed MTRR modify bit.
 *
 *     @param[in]   MsrAddress - Target MrsAddress
 *     @param[in]   *Value - Value to be written
 *     @param[in]   *StdHeader - Pointer to standard header
 */
VOID
MemNModdifyMtrrFixDramModEn (
  IN       UINT32             MsrAddress,
  IN       UINT64             *Value,
  IN       VOID               *StdHeader
  )
{
  S_UINT64 SMsr;
  if (MsrAddress == 0) {
    // turn on modification enable bit for fixed MTRR
    SMsr.Value64 = AsmReadMsr64 (SYS_CFG);
    SMsr.Values32.lo |= 0x80000;
    AsmWriteMsr64 (SYS_CFG, SMsr.Value64);
  } else {
    // turn off modification enable bit for fixed MTRR
    SMsr.Value64 = AsmReadMsr64 (SYS_CFG);
    SMsr.Values32.lo &= ~0x80000;
    AsmWriteMsr64 (SYS_CFG, SMsr.Value64);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Configure disable DLL shutdown in self-refresh mode.
 *
 *     @param[in]   AccessWidth - Access width of the register.
 *     @param[in]   Address - address in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 *
 */

VOID
MemNS3ConfigureDisDllShutdownSrKV (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  LOCATE_HEAP_PTR LocateBufferPtr;
  S3_MEM_NB_BLOCK *S3NBPtr;
  UINT8 Die;

  // See which Node should be accessed
  Die = (UINT8) (Address.Address.Device - 24);

  LocateBufferPtr.BufferHandle = AMD_MEM_S3_NB_HANDLE;
  if (HeapLocateBuffer (&LocateBufferPtr, ConfigPtr) == AGESA_SUCCESS) {
    S3NBPtr = (S3_MEM_NB_BLOCK *) LocateBufferPtr.BufferPtr;
    NBPtr = S3NBPtr[Die].NBPtr;

    MemNBrdcstSetUnConditionalNb (NBPtr, BFDisDllShutdownSR, 0);
  }
}
/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function reads and writes register bitfield
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in]   IsSet - if this is a register read or write
 *     @param[in, out]  *Value - Pointer to the value be read or  written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
STATIC
MemNS3GetSetBitField (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN       BOOLEAN IsSet,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  BIT_FIELD_NAME BitField;
  MEM_NB_BLOCK *NBPtr;
  LOCATE_HEAP_PTR LocateBufferPtr;
  S3_MEM_NB_BLOCK *S3NBPtr;
  UINT32 RegValue;
  UINT8 Die;

  RegValue = 0;
  // See which Node should be accessed
  Die = (UINT8) (Address.Address.Device - 24);

  LocateBufferPtr.BufferHandle = AMD_MEM_S3_NB_HANDLE;
  if (HeapLocateBuffer (&LocateBufferPtr, ConfigPtr) == AGESA_SUCCESS) {
    S3NBPtr = (S3_MEM_NB_BLOCK *) LocateBufferPtr.BufferPtr;
    NBPtr = S3NBPtr[Die].NBPtr;

    // Function field contains the DCT number
    NBPtr->SwitchDCT (NBPtr, (UINT8) Address.Address.Function);

    // Get the bitfield name to be accessed
    // Register field contains the bitfield name
    BitField = (BIT_FIELD_NAME) Address.Address.Register;

    if (IsSet) {
      switch (AccessWidth) {
      case AccessS3SaveWidth8:
        RegValue = *(UINT8 *) Value;
        break;
      case AccessS3SaveWidth16:
        RegValue = *(UINT16 *) Value;
        break;
      case AccessS3SaveWidth32:
        RegValue = *(UINT32 *) Value;
        break;
      default:
        ASSERT (FALSE);
      }
      MemNSetBitFieldNb (NBPtr, BitField, RegValue);
    } else {
      RegValue = MemNGetBitFieldNb (NBPtr, BitField);

      switch (AccessWidth) {
      case AccessS3SaveWidth8:
        *(UINT8 *) Value = (UINT8) RegValue;
        break;
      case AccessS3SaveWidth16:
        *(UINT16 *) Value = (UINT16) RegValue;
        break;
      case AccessS3SaveWidth32:
        *(UINT32 *) Value = RegValue;
        break;
      default:
        ASSERT (FALSE);
      }
    }
  } else {
    ASSERT (FALSE);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function converts MemClkFreq Id value to MemClk frequency in MHz
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       FreqId   - FreqId from Register
 *
 *     @return          MemClk frequency in MHz
 */
UINT16
MemNS3GetMemClkFreqUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 FreqId
  )
{
  UINT16 MemClkFreq;
  if (FreqId == 0x1F) {
    MemClkFreq = 1200;
  } else if (FreqId > 2) {
    MemClkFreq = (FreqId == 14) ? 667 : (300 + ((FreqId - 3) * 33) + (FreqId - 3) / 3);
  } else if (FreqId == 2) {
    MemClkFreq = 200;
  } else {
    MemClkFreq = 50 + (50 * FreqId);
  }
  return MemClkFreq;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function is the default return and always return PSP not present
 *
  *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *      @return            FALSE - PSP is not present
 */
BOOLEAN
MemS3PspNotDetected (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return FALSE;
}

