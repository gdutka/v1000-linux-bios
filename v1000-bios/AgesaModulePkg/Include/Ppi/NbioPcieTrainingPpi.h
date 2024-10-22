/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_NBIO_PCIE_TRAINING_PPI_H_
#define _AMD_NBIO_PCIE_TRAINING_PPI_H_

///-------------------------------------------------------------------------------
///  PPI for PCIe TRAINING NOTIFICATION
///
/// Global ID for the AMD_NBIO_PCIE_SERVICES_PPI
///

#define PEI_AMD_NBIO_PCIE_TRAINING_PPI_GUID \
{ \
  0x72166411, 0x442c, 0x4aab, { 0xa2, 0x60, 0x57, 0xd5, 0x84, 0xd3, 0x21, 0x39} \
}

// Current PPI revision
#define AMD_TRAINING_START_STATUS  TRUE
#define AMD_TRAINING_DONE_STATUS   TRUE

///
/// The Ppi of Pcie sevices
///

typedef struct _PEI_AMD_NBIO_PCIE_TRAINING_START_PPI {
  BOOLEAN   TrainingStart;            ///< Training Start
} PEI_AMD_NBIO_PCIE_TRAINING_START_PPI;

typedef struct _PEI_AMD_NBIO_PCIE_TRAINING_DONE_PPI {
  BOOLEAN   TrainingComplete;         ///< Training Complete
} PEI_AMD_NBIO_PCIE_TRAINING_DONE_PPI;

extern EFI_GUID gAmdNbioPcieTrainingStartPpiGuid;
extern EFI_GUID gAmdNbioPcieTrainingDonePpiGuid;

#endif //

