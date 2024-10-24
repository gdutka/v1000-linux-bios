/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Library
 *
 * Contains interface to the AMD AGESA library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * AMD processors and its use is subject to the terms and conditions of the
 * AMD Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an AMD processor outside of
 * the applicable AMD product specifications will void any AMD warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and AMD disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of AMD processors.
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/ApcbRvLib.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ApobCommonServiceLib.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE        LIBRARY_APCBRVLIB_UPDATE_SHADOW_DIMM_CONFIG_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern BOOLEAN                   mApcbRecoveryFlag;

/*----------------------------------------------------------------------------------------
 *                          G L O B A L        V A L U E S
 *----------------------------------------------------------------------------------------
 */
extern UINT8                     mApcbInstance;
extern BOOLEAN                   mAtRuntime;

//[-start-240426-IB18160212-modify-]//
/**
 *
 *  Update Shadow DIMM configuration
 *
 *  @retval EFI_SUCCESS             Shadow DIMM configuration updated successfully
 *  @retval EFI_OUT_OF_RESOURCES    Can't allocate space for new APCB
 *  @retval EFI_NOT_FOUND           Can't find shadow DIMM configuration
 *
 **/
VOID
EFIAPI
AmdPspApcbUpdateShadowDimmConfig (
  IN       EFI_EVENT Event,
  IN       VOID      *Context
  )
{
  EFI_STATUS                    Status;
  EFI_STATUS                    ApobStatus;
  UINT32                        DimmConfigSize;
  UINT8                         *DimmConfigData;
  UINT8                         Socket;
  UINT8                         Die;
  UINT8                         Channel;
  UINT16                        Instance;
  UINT8                         Dimm;
  UINT8                         DimmIndex;
  UINT8                         i;
  UINT16                        j;
  PSP_SPD_STRUCT                *ShadowDimmConfigHeader;
  SPD_DEF_STRUCT                *CurrentShadowDimm;
  BOOLEAN                       DimmCfgUpdated;
  APCB_HEADER                   *ApcbShadowCopy;
  UINT8                         *NewApcb;
  AMD_CONFIG_PARAMS             StdHeader;
  UINT8                         MaxChannelsPerSocket;
  UINT8                         MaxDimmsPerChannel;
  APOB_SPD_STRUCT               *DimmSmbusInfo;

  IDS_HDT_CONSOLE_PSP_TRACE ("Enter AmdPspApcbUpdateShadowDimmConfig\n");

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    if (mApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return;
  }

  Status = ApobGetDimmCfgUpdatedFlag (&DimmCfgUpdated);
  if (EFI_ERROR (Status)) {
    return;
  }

  if (!DimmCfgUpdated) {
    IDS_HDT_CONSOLE_PSP_TRACE ("No DIMM configuration update\n");
    return;
  }

  Status = InternalApcbLocateType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_SPD_INFO, 0, &DimmConfigSize, (VOID *) &DimmConfigData);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Can't Locate Shadow DIMM configuration\n");
    return;
  }
//[-end-240426-IB18160212-modify-]//

  ShadowDimmConfigHeader = AllocateZeroPool (DimmConfigSize);
  ASSERT (ShadowDimmConfigHeader != NULL);
  LibAmdMemCopy (ShadowDimmConfigHeader, DimmConfigData, DimmConfigSize, &StdHeader);

  // Clear ShadowSpdValid for all Shadow SPD entries
  for (i = 0, CurrentShadowDimm = ShadowDimmConfigHeader->SpdData; i < ShadowDimmConfigHeader->TotalDimms; i ++, CurrentShadowDimm ++) {
    if (!CurrentShadowDimm->DramDownSpdValid) {
      CurrentShadowDimm->ShadowSpdValid = FALSE;
    }
  }

  for (Socket = 0; Socket < 1; Socket ++) {
    for (Die = 0; Die < 1; Die ++) {
      //Instance = (TranslatedChannel >> 1);
      Instance = MAKE_APOB_INSTANCE_ID(Socket, Die);
      DimmIndex = 0;
      IDS_HDT_CONSOLE_PSP_TRACE ("Get Spd Data from APOB Socket: %d, Die: %d\n", Socket, Die);
      ApobStatus = ApobGetMaxChannelsPerSocket (Instance, &MaxChannelsPerSocket);
      if (ApobStatus == EFI_SUCCESS) {
        for (Channel = 0; Channel < MaxChannelsPerSocket; Channel ++) {
          ApobGetMaxDimmsPerChannel (Instance, &MaxDimmsPerChannel);
          for (Dimm = 0; Dimm < MaxDimmsPerChannel; Dimm ++, DimmIndex ++) {
            ApobStatus = ApobGetDimmSmbusInfo (Instance, DimmIndex, &DimmSmbusInfo);
            ASSERT (ApobStatus == EFI_SUCCESS);
            if (DimmSmbusInfo->DimmPresent == TRUE) {
              IDS_HDT_CONSOLE_PSP_TRACE ("Dimm Present on Channel: %d, Slot: %d\n",
                                DimmSmbusInfo->ChannelNumber,
                                DimmSmbusInfo->DimmNumber);
              for (i = 0, CurrentShadowDimm = ShadowDimmConfigHeader->SpdData; i < ShadowDimmConfigHeader->TotalDimms; i ++, CurrentShadowDimm ++) {
                if (!CurrentShadowDimm->DramDownSpdValid) {
                  if (CurrentShadowDimm->SocketNumber == DimmSmbusInfo->SocketNumber) {
                    if (CurrentShadowDimm->ChannelNumber == DimmSmbusInfo->ChannelNumber) {
                      if (CurrentShadowDimm->DimmNumber == DimmSmbusInfo->DimmNumber) {
                        IDS_HDT_CONSOLE_PSP_TRACE ("Udpate Shadow DIMM config for Socket: %d, Channel: %d, Dimm: %d\n",
                                          CurrentShadowDimm->SocketNumber, CurrentShadowDimm->ChannelNumber, CurrentShadowDimm->DimmNumber);
                        CurrentShadowDimm->ShadowSpdValid = TRUE;
                        LibAmdMemCopy (CurrentShadowDimm->Data, DimmSmbusInfo->Data, 512, &StdHeader);
                        IDS_HDT_CONSOLE_PSP_TRACE ("\n\t");
                        for (j = 0; j < 512; j ++) {
                          if (0 == j % 16) {
                            IDS_HDT_CONSOLE_PSP_TRACE ("\n");
                          }
                          IDS_HDT_CONSOLE_PSP_TRACE ("%02x ", CurrentShadowDimm->Data[j]);
                        }
                        IDS_HDT_CONSOLE_PSP_TRACE ("\n");
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

//[-start-240426-IB18160212-modify-]//
  //Allocate space for hold new APCB
  ApcbShadowCopy = GetApcbShadowCopy ();
  NewApcb = NULL;
  NewApcb = AllocateZeroPool (ApcbShadowCopy->SizeOfApcb);
  IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", NewApcb, ApcbShadowCopy->SizeOfApcb);
  ASSERT (NewApcb != NULL);
  if (NewApcb == NULL) {
    return;
  }
  //Shadow Copy pointer will be updated in below routine
  Status = ApcbReplaceType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_SPD_INFO, 0, (UINT8 *)ShadowDimmConfigHeader, DimmConfigSize, NewApcb);
  if (EFI_ERROR (Status)) {
    return;
  }

  //Free resource
  FreePool (ShadowDimmConfigHeader);
  IDS_HDT_CONSOLE_PSP_TRACE ("FREE[%x]\n", ShadowDimmConfigHeader);

  Status = AmdPspWriteBackApcbShadowCopy ();

  return;
}
//[-end-240426-IB18160212-modify-]//

