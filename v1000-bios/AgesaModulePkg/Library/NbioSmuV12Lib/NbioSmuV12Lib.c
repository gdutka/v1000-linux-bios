/* $NoKeywords:$ */
/**
 * @file
 *
 * SMU services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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
#include  <AGESA.h>
#include  <Gnb.h>
#include  <Filecode.h>
#include  <Library/GnbLib.h>
#include  <Library/IdsLib.h>
#include  <Library/AmdBaseLib.h>
#include  <Library/NbioHandleLib.h>
#include  <Library/NbioSmuV12Lib.h>
#include  <Library/CpuLib.h>
#include  <Library/AmdS3SaveLib.h>

#define FILECODE LIBRARY_NBIOSMUV12LIB_NBIOSMUV12LIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#ifndef INVALID_SMU_MSG
  #define INVALID_SMU_MSG  0xFF
#endif
#ifndef BIOSSMC_Result_FATAL
  #define BIOSSMC_Result_FATAL      ((UINT8)0xFC)     // i.e. -4
#endif
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to write SMU register space
 *
 *
 * @param[in] GnbPciAddress      PCI_ADDR
 * @param[in] Address            Register offset, but PortDevice
 * @param[in] Value              The value to write
 * @retval    AGESA_STATUS
 */
VOID
STATIC
SmuRegisterWrite (
  IN       PCI_ADDR            GnbPciAddress,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  )
{
  ACCESS_WIDTH    Width;

  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;
  GnbLibPciIndirectWrite (
    GnbPciAddress.AddressValue | 0xB8,
    Address,
    Width,
    Value,
    NULL
  );
  return;
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to read SMU register space
 *
 *
 *
 * @param[in]  GnbPciAddress      PCI_ADDR
 * @param[in]  Address            Register offset, but PortDevice
 * @param[out] Value              Return value
 * @retval     AGESA_STATUS
 */
VOID
STATIC
SmuRegisterRead (
  IN       PCI_ADDR            GnbPciAddress,
  IN       UINT32              Address,
     OUT   VOID                *Value,
  IN       UINT32              Flags
  )
{
  ACCESS_WIDTH  Width;

  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;
  GnbLibPciIndirectRead (
    GnbPciAddress.AddressValue | 0xB8,
    (Address & (~0x3ull)),
    Width,
    Value,
    NULL
  );
  return;
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to Init Arguments
 *
 *
 *
 * @param[in, out]  SmuArg Request Argument
 */
VOID
NbioSmuServiceCommonInitArgumentsV12 (
  IN OUT   UINT32                   *SmuArg
  )
{
  LibAmdMemFill (SmuArg, 0x00, 24, NULL);
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU service request
 *
 *
 * @param[in]      NbioPciAddress  PCI_ADDR of this NBIO
 * @param[in]      RequestId       Request ID
 * @param[in, out] RequestArgument Request Argument
 * @param[in]      AccessFlags     See GNB_ACCESS_FLAGS_* definitions
 */

UINT32
NbioSmuServiceRequestV12 (
  IN       PCI_ADDR                 NbioPciAddress,
  IN       UINT32                   RequestId,
  IN OUT   UINT32                   *RequestArgument,
  IN       UINT32                   AccessFlags
  )
{
  UINT32                      RxSmuMessageResponse;
  UINT32                      DataMask;
  UINT32                      Address;

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioSmuServiceRequestV12 Enter\n");

  DataMask = 0xFFFFFFFF;

  if (RequestId > BIOSSMC_Message_Count) {
    IDS_HDT_CONSOLE (GNB_TRACE, "NbioSmuServiceRequestV12 INVALID!!\n");
    return BIOSSMC_Result_OK;
  }

  IDS_HDT_CONSOLE (NB_MISC, "  Service Request 0x%x\n", RequestId);
  IDS_HDT_CONSOLE (NB_MISC, "  Service Request Argument 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", RequestArgument[0], RequestArgument[1], RequestArgument[2], RequestArgument[3], RequestArgument[4], RequestArgument[5]);

  SmuRegisterRead (NbioPciAddress, MP1_C2PMSG_RESPONSE_ADDRESS, &RxSmuMessageResponse, 0);

  // 1 Clear Response
  RxSmuMessageResponse = 0;
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_RESPONSE_ADDRESS, &RxSmuMessageResponse, AccessFlags);

  // 2 Write message arguments
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_0_ADDRESS, &RequestArgument[0], AccessFlags);
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_1_ADDRESS, &RequestArgument[1], AccessFlags);
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_2_ADDRESS, &RequestArgument[2], AccessFlags);
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_3_ADDRESS, &RequestArgument[3], AccessFlags);
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_4_ADDRESS, &RequestArgument[4], AccessFlags);
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_5_ADDRESS, &RequestArgument[5], AccessFlags);

  // 3 Write message ID
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_MESSAGE_ADDRESS, &RequestId, AccessFlags);

  // 4 Poll Response until non-zero
  do {
    SmuRegisterRead (NbioPciAddress, MP1_C2PMSG_RESPONSE_ADDRESS, &RxSmuMessageResponse, 0);
  } while (RxSmuMessageResponse == 0x0);

///@todo - implement S3 save for resume
  if (AccessFlags) {
    Address = MP1_C2PMSG_RESPONSE_ADDRESS;
    AmdS3SaveScriptPciWrite (AccessWidth32, NbioPciAddress.AddressValue | 0xB8, &Address);
    RxSmuMessageResponse = 0x1;
    DataMask = 0xFFFFFFFF;
    AmdS3SaveScriptPciPoll (AccessWidth32, NbioPciAddress.AddressValue | 0xBC, &RxSmuMessageResponse, &DataMask, 0xffffffff);
  }

  while (RxSmuMessageResponse == BIOSSMC_Result_FATAL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "SMU Assert AccessFlags\n");
    ASSERT (FALSE);   // ASSERT if asserts are enabled
  }

  // 5 Read updated SMU message arguments
  SmuRegisterRead (NbioPciAddress, MP1_C2PMSG_ARGUMENT_0_ADDRESS, &RequestArgument[0], 0);
  SmuRegisterRead (NbioPciAddress, MP1_C2PMSG_ARGUMENT_1_ADDRESS, &RequestArgument[1], 0);
  SmuRegisterRead (NbioPciAddress, MP1_C2PMSG_ARGUMENT_2_ADDRESS, &RequestArgument[2], 0);
  SmuRegisterRead (NbioPciAddress, MP1_C2PMSG_ARGUMENT_3_ADDRESS, &RequestArgument[3], 0);
  SmuRegisterRead (NbioPciAddress, MP1_C2PMSG_ARGUMENT_4_ADDRESS, &RequestArgument[4], 0);
  SmuRegisterRead (NbioPciAddress, MP1_C2PMSG_ARGUMENT_5_ADDRESS, &RequestArgument[5], 0);

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioSmuServiceRequestV12 Exit\n");
  return (RxSmuMessageResponse);
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU firmware test
 *
 *
 * @param[in]  NbioHandle     Pointer to GNB_HANDLE
 */

AGESA_STATUS
NbioSmuFirmwareTestV12 (
  IN       GNB_HANDLE               *NbioHandle
  )
{
  UINT32                      SmuArg[6];
  UINT32                      RequestId;

  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioSmuFirmwareTestV12 Enter\n");

  NbioSmuServiceCommonInitArgumentsV12 (SmuArg);
  SmuArg[0] = 0xAA55AA55;
  RequestId = BIOSSMC_MSG_TestMessage;

  NbioSmuServiceRequestV12 (NbioGetHostPciAddress (NbioHandle), RequestId, SmuArg, 0);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  Service Test Return %x\n", SmuArg[0]);
  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioSmuFirmwareTestV12 Exit\n");

  if (SmuArg[0] == 0) {
    return AGESA_ERROR;
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * DXIO firmware request - Common function
 *
 *
 * @param[in]      NbioHandle      Pointer to GNB_HANDLE
 * @param[in]      DxioMBoxRequest DXIO Mailbox Request ID
 * @param[in, out] DxioMBoxArg     SMU Argument structure - [0] entry reserved for DXIO Mailbox Request ID
 * @param[in]      AccessFlags     See GNB_ACCESS_FLAGS_* definitions
 */

UINT32
NbioDxioServiceRequestV12 (
  IN       GNB_HANDLE               *NbioHandle,
  IN       UINT32                   DxioMBoxRequest,
  IN OUT   UINT32                   *DxioMBoxArg,
  IN       UINT32                   AccessFlags
  )
{
  UINT32            RetVal;
  *DxioMBoxArg = DxioMBoxRequest;
  IDS_HDT_CONSOLE (
    GNB_TRACE,
    "Input Args = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
    DxioMBoxRequest,
    DxioMBoxArg[1],
    DxioMBoxArg[2],
    DxioMBoxArg[3],
    DxioMBoxArg[4],
    DxioMBoxArg[5]
    );
//  SmuRegisterRead (
//    NbioGetHostPciAddress (NbioHandle),
//    0x13B00068,
//    &RetVal,
//    0
//  );
//  if ((RetVal & 0x01) == 0) {
    RetVal = NbioSmuServiceRequestV12(
      NbioGetHostPciAddress (NbioHandle),
      BIOSSMC_MSG_DxioTestMessage,
      DxioMBoxArg,
      AccessFlags
      );
//  } else {
//    RetVal = 1;
//  }
  IDS_HDT_CONSOLE (
    GNB_TRACE,
    "Return Args = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
    DxioMBoxArg[0],
    DxioMBoxArg[1],
    DxioMBoxArg[2],
    DxioMBoxArg[3],
    DxioMBoxArg[4],
    DxioMBoxArg[5]
    );

  return RetVal;
}


/*----------------------------------------------------------------------------------------*/
/**
 * SMU S3 Entry Notification
 *
 */

VOID
SmuNotifyS3Entry (
  )
{
  PCI_ADDR            NbioPciAddress;
  UINT32              RequestId;
  UINT32              RxSmuMessageResponse;

  NbioPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);

  SmuRegisterRead (NbioPciAddress, MP1_C2PMSG_RESPONSE_ADDRESS, &RxSmuMessageResponse, 0);

  // 1 Clear Response
  RxSmuMessageResponse = 0;
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_RESPONSE_ADDRESS, &RxSmuMessageResponse, 0);

  // 2 Clear Message Arguments
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_0_ADDRESS, &RxSmuMessageResponse, 0);
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_1_ADDRESS, &RxSmuMessageResponse, 0);
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_2_ADDRESS, &RxSmuMessageResponse, 0);
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_3_ADDRESS, &RxSmuMessageResponse, 0);
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_4_ADDRESS, &RxSmuMessageResponse, 0);
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_ARGUMENT_5_ADDRESS, &RxSmuMessageResponse, 0);

  // 3 Write message ID
  RequestId = BIOSSMC_MSG_SleepEntry;
  SmuRegisterWrite (NbioPciAddress, MP1_C2PMSG_MESSAGE_ADDRESS, &RequestId, 0);

  // Halt execution
  CpuSleep ();

  return;
}


