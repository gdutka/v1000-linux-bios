/** @file
  EFI HDD Password Protocol

;******************************************************************************
;* Copyright (c) 2024, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "HddPassword.h"

EFI_SMM_SYSTEM_TABLE2                   *mSmst2;
EFI_SMM_BASE2_PROTOCOL                  *mSmmBase2;

HDD_PASSWORD_PRIVATE                    *mHddPasswordPrivate;
DRIVER_INSTALL_INFO                     *mDriverInstallInfo;
VOID                                    *mHddPasswordS3Registration = NULL;
VOID                                    *mCommBuffer;
MEMORY_PAGE_TABLE                       *mMemoryBuffer;

BOOLEAN                                 mEnableBlockSid = FALSE;
BOOLEAN                                 mHddPasswordChecked = FALSE;

STATIC ATA_IDENTIFY_DATA                      mIdentifyData;
STATIC ATA_DEVICE_SERVER_SECURITY_STATE_DATA  mSecurityCheckStateData;
#define BUFFER_SIZE                     512


AGENT_FILE_GUID_LIST mAgentFileGuidListForVerifySSCP[] = {
  {0x3ACC966D, 0x8E33, 0x45c6, 0xb4, 0xfe, 0x62, 0x72, 0x4B, 0xCD, 0x15, 0xA9}, // AhciBusDxe
  {0x70D57D67, 0x7F05, 0x494d, 0xA0, 0x14, 0xB7, 0x5D, 0x73, 0x45, 0xB7, 0x00}, // StorageSecurityCommad
  {0x5BE3BDF4, 0x53CF, 0x46a3, 0xA6, 0xA9, 0x73, 0xC3, 0x4A, 0x6E, 0x5E, 0xE3}, // NVME
  {0x67BBC344, 0x84BC, 0x4e5c, 0xb4, 0xDF, 0xF5, 0xE4, 0xA0, 0x0E, 0x1F, 0x3A}, // SdHostDriver
  {0x86EDAAE5, 0x073C, 0x4c89, 0xB9, 0x49, 0x89, 0x84, 0xAC, 0x8A, 0x55, 0xF3}, // SdMmcDevice
};

/**
  Check if SSCP is installed by Insyde driver model driver

  @param Controller       PCI handle of SSCP's parent controller.
  @param Device           Device handle that install the driver model driver.

  @retval TRUE                 It is installed by Insyde driver
  @retval FALSE                It is not installed by Insyde driver
**/
BOOLEAN
EFIAPI
CheckSscpSource (
  IN  EFI_HANDLE                       Controller,
  IN  EFI_HANDLE                       Device
  )
{
  UINTN                                Index;
  UINTN                                AgentIndex;
  EFI_STATUS                           Status;
  EFI_GUID                             **ProtocolBuffer;
  UINTN                                ProtocolBufferCount;
  UINTN                                ProtocolIndex;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY  *OpenInfoBuffer;
  UINTN                                EntryCount;
  EFI_HANDLE                            *AgentHandleBuffer;
  UINTN                                AgentDriversCount;


  AgentDriversCount = sizeof(mAgentFileGuidListForVerifySSCP) / sizeof(AGENT_FILE_GUID_LIST);
  AgentHandleBuffer = FindAgentHandleListByFileGUID( mAgentFileGuidListForVerifySSCP, AgentDriversCount );
  if (AgentHandleBuffer == NULL) {
    return FALSE;
  }

  Status = gBS->ProtocolsPerHandle (
                  Controller,
                  &ProtocolBuffer,
                  &ProtocolBufferCount
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }


  for (ProtocolIndex = 0; ProtocolIndex < ProtocolBufferCount; ProtocolIndex++) {
    Status = gBS->OpenProtocolInformation (
                    Controller,
                    ProtocolBuffer[ProtocolIndex],
                    &OpenInfoBuffer,
                    &EntryCount
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    for (Index = 0; Index < EntryCount; Index++) {
      if (!(OpenInfoBuffer[Index].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER)) {
        continue;
      }
      //
      // Query all the children that opened with EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
      //
      for (AgentIndex = 0; AgentIndex < AgentDriversCount; AgentIndex++) {
        if (OpenInfoBuffer[Index].AgentHandle == AgentHandleBuffer[AgentIndex]) {
          if (OpenInfoBuffer[Index].ControllerHandle == Device) {
            FreePool (OpenInfoBuffer);
            FreePool (ProtocolBuffer);
            return TRUE;
          }
        }
      }
    }

    FreePool (OpenInfoBuffer);
  }

  FreePool (ProtocolBuffer);

  return FALSE;
}



/**
  Retrieve TCG related password from HddPasswordTable.

  @param[in]        This                EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        HddInfo             The array of HDD information used in HDD Password.
  @param[in]        Type                Type of HDD password.
  @param[out]       Password            The password from TCG extended data.
  @param[out]       PasswordLength      Length of the password from TCG extended data.

  @retval           EFI_SUCCESS         Password found.
  @retval           Others              Password not found.

**/
EFI_STATUS
RetrieveTcgPassword (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_PASSWORD_HDD_INFO             *HddInfo,
  IN  TCG_PASSWORD_TYPE                 Type,
  OUT VOID                              *Password,
  OUT UINTN                             *PasswordLength
  );

/**

  Get BlockSid attribute info.

  @param[in]      Session             OPAL_SESSION to retrieve info.
  @param[in,out]  LockingFeature      Return the BlockSid info.

  @retval Tcg result codes.

**/
STATIC
TCG_RESULT
OpalGetBlockSidInfo (
  IN     OPAL_SESSION                     *Session,
  IN OUT TCG_BLOCK_SID_FEATURE_DESCRIPTOR *BlockSidFeature
  )
{
  UINT8                              Buffer[BUFFER_SIZE];
  TCG_LEVEL0_DISCOVERY_HEADER        *DiscoveryHeader;
  OPAL_LEVEL0_FEATURE_DESCRIPTOR     *Feat;
  UINTN                              Size;

  NULL_CHECK (Session);
  NULL_CHECK (BlockSidFeature);

  ZeroMem (Buffer, BUFFER_SIZE);
  ASSERT (sizeof (Buffer) >= sizeof (TCG_SUPPORTED_SECURITY_PROTOCOLS));

  if (OpalRetrieveLevel0DiscoveryHeader (Session, BUFFER_SIZE, Buffer) == TcgResultFailure) {
    DEBUG ((DEBUG_INFO, "OpalRetrieveLevel0DiscoveryHeader failed\n"));
    return TcgResultFailure;
  }
  DiscoveryHeader = (TCG_LEVEL0_DISCOVERY_HEADER *)Buffer;

  Size = 0;
  Feat = (OPAL_LEVEL0_FEATURE_DESCRIPTOR *)TcgGetFeature (DiscoveryHeader, TCG_FEATURE_BLOCK_SID, &Size);
  if (Feat != NULL && Size >= sizeof (TCG_BLOCK_SID_FEATURE_DESCRIPTOR)) {
    CopyMem (BlockSidFeature, &Feat->BlockSid, sizeof (TCG_BLOCK_SID_FEATURE_DESCRIPTOR));
  }

  return TcgResultSuccess;
}

/**
  Send BlockSid command.

**/
STATIC
VOID
IssueBlockSid (
  VOID
  )
{
  TCG_RESULT                                 Result;
  OPAL_SESSION                               Session;
  HDD_PASSWORD_HDD_INFO_PRIVATE              *HddInfoPrivate;
  HDD_INFO_MANAGER                           *HddInfoManager;
  LIST_ENTRY                                 *HddInfoPrivateListHead;
  LIST_ENTRY                                 *Link;
  UINTN                                      Index;

  //
  // Send BlockSID command to each Opal disk
  //
  HddInfoManager = mHddPasswordPrivate->HddInfoManager;
  HddInfoPrivateListHead = &HddInfoManager->HddInfoPrivateListHead;
  for (Link = GetFirstNode (HddInfoPrivateListHead), Index = 0;
       !IsNull (HddInfoPrivateListHead, Link) && Index < HddInfoManager->NumOfHdd;
       Link = GetNextNode (HddInfoPrivateListHead, Link), Index++) {
    HddInfoPrivate = GET_HDD_INFO_PRIVATE_FROM_LINK (Link);
    if (!HddInfoPrivate->HddInfo.StorageTcgSecuritySupported) {
      continue;
    }

    if (HddInfoPrivate->OpalDisk.BlockSidFeature.SIDBlockedState == 1) {
      continue;
    }

    if ((HddInfoPrivate->OpalDisk.SupportedAttributes.BlockSid == 1) &&
        (HddInfoPrivate->OpalDisk.BlockSidFeature.SIDValueState == 0)) {
      ZeroMem(&Session, sizeof(Session));
      Session.Sscp = (mSmst2 == NULL) ? HddInfoPrivate->StorageSecurityCommandInDxe : HddInfoPrivate->StorageSecurityCommandInSmm;
      Session.MediaId = HddInfoPrivate->OpalDisk.MediaId;
      Session.OpalBaseComId = HddInfoPrivate->OpalDisk.OpalBaseComId;

      Result = OpalBlockSid (&Session, TRUE);  // HardwareReset must always be TRUE
      if (Result != TcgResultSuccess) {
        DEBUG ((DEBUG_ERROR, "OpalBlockSid fail\n"));
        continue;
      }
    }
  }
}

/**

  The function retrieves the ENABLE column for the specified rowUid (authority).

  @param[in]      LockingSpSession    OPAL_SESSION with OPAL_UID_LOCKING_SP to retrieve try limit
  @param[in]      RowUid              Row UID of the Locking SP C_PIN table to retrieve TryLimit column
  @param[in/out]  TryLimit            Value from TryLimit column

**/
STATIC
TCG_RESULT
TcgGetAuthorityEnabled (
  OPAL_SESSION   *LockingSpSession,
  TCG_UID        AuthorityUid,
  BOOLEAN        *Enabled
  )
{
  TCG_CREATE_STRUCT CreateStruct;
  TCG_PARSE_STRUCT  ParseStruct;
  UINT32            Size;
  UINT8             MethodStatus;
  UINT8             Buf[512];
  UINT32            Col;

  NULL_CHECK(LockingSpSession);
  NULL_CHECK(Enabled);

  ERROR_CHECK(TcgInitTcgCreateStruct(&CreateStruct, Buf, sizeof(Buf)));
  ERROR_CHECK(TcgStartComPacket(&CreateStruct, LockingSpSession->OpalBaseComId, LockingSpSession->ComIdExtension));
  ERROR_CHECK(TcgStartPacket(&CreateStruct, LockingSpSession->TperSessionId, LockingSpSession->HostSessionId, 0x0, 0x0, 0x0));
  ERROR_CHECK(TcgStartSubPacket(&CreateStruct, 0x0));
  ERROR_CHECK(TcgStartMethodCall(&CreateStruct, AuthorityUid, TCG_UID_METHOD_GET));
  ERROR_CHECK(TcgStartParameters(&CreateStruct));
  ERROR_CHECK(TcgAddStartList(&CreateStruct));
  ERROR_CHECK(TcgAddStartName(&CreateStruct));
  ERROR_CHECK(TcgAddUINT8(&CreateStruct, TCG_CELL_BLOCK_START_COLUMN_NAME));
  ERROR_CHECK(TcgAddUINT8(&CreateStruct, 0x05)); // Enabled
  ERROR_CHECK(TcgAddEndName(&CreateStruct));
  ERROR_CHECK(TcgAddStartName(&CreateStruct));
  ERROR_CHECK(TcgAddUINT8(&CreateStruct, TCG_CELL_BLOCK_END_COLUMN_NAME));
  ERROR_CHECK(TcgAddUINT8(&CreateStruct, 0x05)); // Enabled
  ERROR_CHECK(TcgAddEndName(&CreateStruct));
  ERROR_CHECK(TcgAddEndList(&CreateStruct));
  ERROR_CHECK(TcgEndParameters(&CreateStruct));
  ERROR_CHECK(TcgEndMethodCall(&CreateStruct));
  ERROR_CHECK(TcgEndSubPacket(&CreateStruct));
  ERROR_CHECK(TcgEndPacket(&CreateStruct));
  ERROR_CHECK(TcgEndComPacket(&CreateStruct, &Size));

  ERROR_CHECK(OpalPerformMethod(LockingSpSession, Size, Buf, sizeof(Buf), &ParseStruct, &MethodStatus));
  METHOD_STATUS_ERROR_CHECK(MethodStatus, TcgResultFailure);

  ERROR_CHECK(TcgGetNextStartList(&ParseStruct));
  ERROR_CHECK(TcgGetNextStartList(&ParseStruct));
  ERROR_CHECK(TcgGetNextStartName(&ParseStruct));
  ERROR_CHECK(TcgGetNextUINT32(&ParseStruct, &Col));
  ERROR_CHECK(TcgGetNextBOOLEAN(&ParseStruct, Enabled));
  ERROR_CHECK(TcgGetNextEndName(&ParseStruct));
  ERROR_CHECK(TcgGetNextEndList(&ParseStruct));
  ERROR_CHECK(TcgGetNextEndList(&ParseStruct));
  ERROR_CHECK(TcgGetNextEndOfData(&ParseStruct));

  if (Col != 0x05) {
    DEBUG ((DEBUG_INFO, "ERROR: got col %u, expected %u\n", Col, OPAL_LOCKING_SP_C_PIN_TRYLIMIT_COL));
    return TcgResultFailure;
  }

  return TcgResultSuccess;
}

/**
  The function checking if PSID authority is enabled.

  @param[in]  HddPasswordService Point to EFI_HDD_PASSWORD_SERVICE_PROTOCOL.
  @param[in]  Sscp               Point to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL.
  @param[in]  HddInfo            Point to HDD information data.
  @param[out] OpalDisk           The Opal device.

  @retval TRUE                 PSID authority is enabled;
                               Also returning TRUE for a drive which LockingSp is enabled
                               but SID/Admin password doesn't exist in NV
  @retval FALSE                PSID authority is disabled

**/
STATIC
BOOLEAN
IsPsidEnabled (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL      *HddPasswordService,
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *Sscp,
  IN  HDD_PASSWORD_HDD_INFO                  *HddInfo,
  IN  OPAL_DISK_INFO                         *OpalDisk
  )
{
  EFI_STATUS                  Status;
  TCG_RESULT                  TcgResult;
  OPAL_SESSION                Session;
  UINT8                       MethodStatus;
  UINT8                       Password[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                       PasswordLength;
  BOOLEAN                     PsidEnabled;

  if ((OpalDisk->SupportedAttributes.MediaEncryption == 0) &&
      (OpalDisk->SupportedAttributes.PyriteSscV2 == 0)) {
    //
    // Assuming this kind of device doesn't support PSID
    //
    return FALSE;
  }

  //
  // Use GET method to get PSID authority table, ENABLE column.
  //
  ZeroMem (&Session, sizeof (Session));
  Session.Sscp          = Sscp;
  Session.MediaId       = OpalDisk->MediaId;
  Session.OpalBaseComId = OpalDisk->OpalBaseComId;

  if ((OpalDisk->LockingFeature.LockingEnabled == 0) &&
      (OpalDisk->BlockSidFeature.SIDValueState == 0)) {
    TcgResult = OpalUtilGetMsid (&Session, OpalDisk->Msid, OPAL_MSID_LENGHT, &OpalDisk->MsidLength);
    if (TcgResult != TcgResultSuccess) {
      return FALSE;
    }
    //
    // Using MSID credential to authenticate Admin SP.
    //
    TcgResult = OpalStartSession (
                  &Session,
                  OPAL_UID_ADMIN_SP,
                  TRUE,
                  (UINT32)OpalDisk->MsidLength,
                  (VOID *)OpalDisk->Msid,
                  OPAL_ADMIN_SP_SID_AUTHORITY,
                  &MethodStatus
                  );
  } else {
    //
    // Need AdminSP SID password.
    //
    PasswordLength = sizeof (Password);
    ZeroMem ((VOID *)Password, PasswordLength);
    Status = RetrieveTcgPassword (
               HddPasswordService,
               HddInfo,
               SID,
               Password,
               &PasswordLength
               );
    if (EFI_ERROR (Status)) {
      //
      // Return TRUE for the drive which LockingSp is enabled but SID/Admin password doesn't exist in NV
      //
      return TRUE;
    }

    TcgResult = OpalStartSession (
                  &Session,
                  OPAL_UID_ADMIN_SP,
                  TRUE,
                  (UINT32)PasswordLength,
                  (VOID *)Password,
                  OPAL_ADMIN_SP_SID_AUTHORITY,
                  &MethodStatus
                  );
  }

  PsidEnabled = FALSE;
  if ((TcgResult == TcgResultSuccess) &&
      (MethodStatus == TCG_METHOD_STATUS_CODE_SUCCESS)) {
    TcgResult = TcgGetAuthorityEnabled (
                  &Session,
                  OPAL_ADMIN_SP_PSID_AUTHORITY,
                  &PsidEnabled
                  );
    OpalEndSession (&Session);
  }

  PasswordLength = sizeof (Password);
  ZeroMem ((VOID *)Password, PasswordLength);

  return PsidEnabled;
}

/**
  The function checking if USER1 authority is enabled.

  @param[in]  HddPasswordService Point to EFI_HDD_PASSWORD_SERVICE_PROTOCOL.
  @param[in]  Sscp               Point to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL.
  @param[in]  HddInfo            Point to HDD information data.
  @param[out] OpalDisk           The Opal device.

  @retval TRUE                 USER1 authority is enabled
  @retval FALSE                USER1 authority is disabled

**/
STATIC
BOOLEAN
IsUser1Enabled (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL      *HddPasswordService,
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *Sscp,
  IN  HDD_PASSWORD_HDD_INFO                  *HddInfo,
  IN  OPAL_DISK_INFO                         *OpalDisk
  )
{
  EFI_STATUS                  Status;
  TCG_RESULT                  TcgResult;
  OPAL_SESSION                Session;
  UINT8                       MethodStatus;
  UINT8                       Password[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                       PasswordLength;
  BOOLEAN                     User1Enabled;

  if (OpalDisk->LockingFeature.LockingEnabled == 0) {
    //
    // Locking feature is not enabled.
    //
    return FALSE;
  }

  User1Enabled = FALSE;
  PasswordLength = sizeof (Password);
  ZeroMem ((VOID *)Password, PasswordLength);

  Status = RetrieveTcgPassword (
             HddPasswordService,
             HddInfo,
             ADMIN1,
             Password,
             &PasswordLength
             );
  if (!EFI_ERROR (Status)) {
    //
    // Use GET method to get USER1 authority table, ENABLE column.
    // Need LockingSP Admin1 password.
    //
    ZeroMem (&Session, sizeof (Session));
    Session.Sscp          = Sscp;
    Session.MediaId       = OpalDisk->MediaId;
    Session.OpalBaseComId = OpalDisk->OpalBaseComId;

    TcgResult = OpalStartSession (
                  &Session,
                  OPAL_UID_LOCKING_SP,
                  TRUE,
                  (UINT32)PasswordLength,
                  (VOID *)&Password,
                  OPAL_LOCKING_SP_ADMIN1_AUTHORITY,
                  &MethodStatus
                  );
    if (TcgResult == TcgResultSuccess && MethodStatus == TCG_METHOD_STATUS_CODE_SUCCESS) {
      TcgResult = TcgGetAuthorityEnabled (
                    &Session,
                    OPAL_LOCKING_SP_USER1_AUTHORITY,
                    &User1Enabled
                    );
      OpalEndSession (&Session);
    } else {
      Status = EFI_DEVICE_ERROR;
    }
  }
  if (EFI_ERROR (Status)) {
    //
    // Fail to access USER1 Authority table. Try another way.
    // Get USER1 password.
    //
    Status = RetrieveTcgPassword (
               HddPasswordService,
               HddInfo,
               USER1,
               Password,
               &PasswordLength
               );
    if (!EFI_ERROR (Status)) {
      //
      // Assume USER1 is enabled if USER1 password is in HDD password table already.
      //
      User1Enabled = TRUE;
    } else {
      //
      // Assume USER1 is enabled if Locked.
      //
      User1Enabled = (BOOLEAN)OpalDisk->LockingFeature.Locked;
    }
  }

  PasswordLength = sizeof (Password);
  ZeroMem ((VOID *)Password, PasswordLength);

  return User1Enabled;
}

/**
  Update the device info.

  @param[in]  Sscp               Point to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL.
  @param[out] OpalDisk           The Opal device.

  @retval EFI_SUCCESS            Get info from device succeeded.
  @retval EFI_DEVICE_ERROR       Get info from device failed.
  @retval EFI_INVALID_PARAMETER  Not get Msid info before get ownership info.

**/
EFI_STATUS
OpalDiskUpdateStatus (
  IN     EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *Sscp,
     OUT OPAL_DISK_INFO                         *OpalDisk
  )
{
  TCG_RESULT                  TcgResult;
  OPAL_SESSION                Session;

  ZeroMem (&Session, sizeof (Session));
  Session.Sscp          = Sscp;
  Session.MediaId       = OpalDisk->MediaId;
  Session.OpalBaseComId = OpalDisk->OpalBaseComId;

  TcgResult = OpalGetLockingInfo (&Session, &OpalDisk->LockingFeature);
  if (TcgResult != TcgResultSuccess) {
    return EFI_DEVICE_ERROR;
  }

  TcgResult = OpalGetBlockSidInfo (&Session, &OpalDisk->BlockSidFeature);
  if (TcgResult != TcgResultSuccess) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Check to see if the device is Edrive

  @param[in]  Sscp            Point to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL.
  @param[in]  MediaId         Media ID.

  @retval TRUE                 It is Edrive
  @retval False                Not an Edrive

**/
BOOLEAN
IsEdrive (
  IN     EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *Sscp,
  IN     UINT16                                 MediaId
  )
{
  TCG_RESULT                      TcgResult;
  OPAL_SESSION                    Session;
  UINT8                           Buffer[BUFFER_SIZE];
  TCG_LEVEL0_DISCOVERY_HEADER     *DiscoveryHeader;
  OPAL_LEVEL0_FEATURE_DESCRIPTOR  *Feat;
  UINTN                           Size;
  OPAL_DISK_INFO                  OpalDisk;

  ZeroMem (&OpalDisk, sizeof (OpalDisk));
  ZeroMem(&Session, sizeof (Session));
  Session.Sscp = Sscp;
  Session.MediaId = MediaId;

  TcgResult = OpalGetSupportedAttributesInfo (&Session, &OpalDisk.SupportedAttributes, &OpalDisk.OpalBaseComId);
  if (TcgResult != TcgResultSuccess) {
    return FALSE;
  }

  if (OpalDisk.SupportedAttributes.SpIeee1667 != 1) {
    return FALSE;
  }

  if (!(OpalDisk.SupportedAttributes.OpalSsc1 == 1 ||
      OpalDisk.SupportedAttributes.OpalSsc2 == 1 ||
      OpalDisk.SupportedAttributes.OpalSscLite== 1)) {
    return FALSE;
  }

  if (OpalRetrieveLevel0DiscoveryHeader (&Session, BUFFER_SIZE, Buffer) == TcgResultFailure) {
    DEBUG ((DEBUG_INFO, "OpalRetrieveLevel0DiscoveryHeader failed\n"));
    return FALSE;
  }

  DiscoveryHeader = (TCG_LEVEL0_DISCOVERY_HEADER*) Buffer;

  Size = 0;
  Feat = (OPAL_LEVEL0_FEATURE_DESCRIPTOR*) TcgGetFeature (DiscoveryHeader, TCG_FEATURE_SINGLE_USER_MODE, &Size);
  if (!(Feat != NULL && Size >= sizeof (OPAL_SINGLE_USER_MODE_FEATURE))) {
    return FALSE;
  }

  Size = 0;
  Feat = (OPAL_LEVEL0_FEATURE_DESCRIPTOR*) TcgGetFeature (DiscoveryHeader, TCG_FEATURE_DATASTORE_TABLE, &Size);
  if (!(Feat != NULL && Size >= sizeof (OPAL_DATASTORE_TABLE_FEATURE))) {
    return FALSE;
  }

  return TRUE;
}



/**
  Check to see if the device is encrypted by 3rd party software

  @param[in]  Sscp            Point to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL.
  @param[in]  MediaId         Media ID.

  @retval TRUE                 It is encrypted by 3rd party software
  @retval False                Not by 3rd party software

**/
BOOLEAN
IsEncryptedBy3rdPartySoftware (
  IN     EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *Sscp,
  IN     UINT16                                 MediaId
  )
{
  UINT8                              Buffer[BUFFER_SIZE];
  TCG_LEVEL0_DISCOVERY_HEADER        *DiscoveryHeader;
  OPAL_LEVEL0_FEATURE_DESCRIPTOR     *Feat;
  UINTN                              Size;
  OPAL_SESSION                       Session;



  ZeroMem (&Session, sizeof (Session));
  Session.Sscp          = Sscp;

  ZeroMem(Buffer, BUFFER_SIZE);
  ASSERT(sizeof(Buffer) >= sizeof(TCG_SUPPORTED_SECURITY_PROTOCOLS));

  if (OpalRetrieveLevel0DiscoveryHeader (&Session, BUFFER_SIZE, Buffer) == TcgResultFailure) {
    DEBUG ((DEBUG_INFO, "OpalRetrieveLevel0DiscoveryHeader failed\n"));
    return FALSE;
  }
  DiscoveryHeader = (TCG_LEVEL0_DISCOVERY_HEADER*) Buffer;

  Size = 0;
  Feat = (OPAL_LEVEL0_FEATURE_DESCRIPTOR*) TcgGetFeature (DiscoveryHeader, TCG_FEATURE_LOCKING, &Size);
  if (Feat != NULL && Size >= sizeof (TCG_LOCKING_FEATURE_DESCRIPTOR)) {
    if (Feat->Locking.MbrEnabled) {
      return TRUE;
    }
  }

  return FALSE;
}


/**
  Initialize the Opal disk base on the hardware info get from device.

  @param[in]  Sscp            Point to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL.
  @param[in]  MediaId         Media ID.
  @param[out] OpalDisk        The Opal device.

  @retval EFI_SUCCESS         Initialize the device success.
  @retval EFI_DEVICE_ERROR    Get info from device failed.

**/
EFI_STATUS
OpalDiskInitialize (
  IN     EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *Sscp,
  IN     UINT16                                 MediaId,
     OUT OPAL_DISK_INFO                         *OpalDisk
  )
{
  TCG_RESULT                  TcgResult;
  OPAL_SESSION                Session;

  ZeroMem(&Session, sizeof (Session));
  Session.Sscp = Sscp;
  Session.MediaId = MediaId;

  TcgResult = OpalGetSupportedAttributesInfo (&Session, &OpalDisk->SupportedAttributes, &OpalDisk->OpalBaseComId);
  if (TcgResult != TcgResultSuccess) {
    return EFI_DEVICE_ERROR;
  }
  Session.OpalBaseComId = OpalDisk->OpalBaseComId;

  return OpalDiskUpdateStatus (Sscp, OpalDisk);
}

/**
  This function converts Unicode string to ASCII string.

  @param[in]        RawPasswordPtr      Password string address.
  @param[in]        RawPasswordLength   Password string length.
  @param[OUT]       PasswordToHdd       Encode password address.
  @param[OUT]       PasswordToHddLength Encode string length.

  @retval           EFI_SUCCESS
**/
EFI_STATUS
HddPasswordStringProcess (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN UINT8                              PasswordType,
  IN VOID                               *RawPasswordPtr,
  IN UINTN                              RawPasswordLength,
  OUT VOID                              **PasswordToHdd,
  OUT UINTN                             *PasswordToHddLength
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  CHAR16                                *RawPassword;
  UINT8                                 *ProcessedPassword;

  if (!mSmst2) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDxeHddPasswordStringProcess \n"));
    Status = OemSvcDxeHddPasswordStringProcess (
               PasswordType,
               RawPasswordPtr,
               RawPasswordLength,
               (UINT8 **)PasswordToHdd,
               PasswordToHddLength
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDxeHddPasswordStringProcess Status: %r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcSmmHddPasswordStringProcess \n"));
    Status = OemSvcSmmHddPasswordStringProcess (
               PasswordType,
               RawPasswordPtr,
               RawPasswordLength,
               (UINT8**)PasswordToHdd,
               PasswordToHddLength
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcSmmHddPasswordStringProcess Status: %r\n", Status));
  }

  if (Status == EFI_UNSUPPORTED) {
    RawPassword = RawPasswordPtr;
    ProcessedPassword = (UINT8 *)PasswordToHdd;

    for (Index = 0; Index < RawPasswordLength; Index++) {
      //
      // Unicode to ASCII
      //

      ProcessedPassword[Index] = (UINT8)RawPassword[Index];
    }

    *PasswordToHddLength = RawPasswordLength;
  }

  return EFI_SUCCESS;
}

/**
  Update Legacy EBDA.
  This information is used for INT 13 when reading block.
  If the mapped bit is enabled, the device will be skipped.

  @param[in]        HddPasswordService  EFI_HDD_PASSWORD_SERVICE_PROTOCOL

  @retval           EFI_SUCCESS
  @retval           EFI_INVALID_PARAMETER

**/
EFI_STATUS
UpdateLegacyEbda (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *HddPasswordService,
  HDD_PASSWORD_HDD_INFO                 *HddInfo
  )
{

  if (HddPasswordService == NULL || HddInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!mSmst2) {
    if (mDriverInstallInfo->LegacyBios != NULL) {
      if ((HddInfo->HddSecurityStatus & HDD_LOCKED_BIT) == HDD_LOCKED_BIT) {
        if (HddInfo->ControllerMode == ATA_IDE_MODE) {
          EBDA (EBDA_HDD_LOCKED_FLAG) |= BIT (HddInfo->MappedPort);
        } else {
          EBDA (EBDA_HDD_LOCKED_FLAG) |= BIT (HddInfo->PortNumber);
        }
      } else {
        if (HddInfo->ControllerMode == ATA_IDE_MODE) {
          EBDA (EBDA_HDD_LOCKED_FLAG) &= ~(BIT (HddInfo->MappedPort));
        } else {
          EBDA (EBDA_HDD_LOCKED_FLAG) &= ~(BIT (HddInfo->PortNumber));
        }
      }

      return EFI_SUCCESS;
    }
  }

  return EFI_UNSUPPORTED;
}

/**
  Get the HDD private information used in HDD Password internal.

  @param[in]        HddInfo             the HDD information pointer.
  @param[out]       HddInfoPrivate      the HDD private information pointer.

  @retval           EFI_NOT_FOUND       Not found the mapped HDD info.
  @retval           EFI_SUCCESS         Found the mapped HDD info.

**/
EFI_STATUS
GetPrivateFromHddPasswordHddInfo (
  IN HDD_PASSWORD_HDD_INFO              *HddInfo,
  OUT HDD_PASSWORD_HDD_INFO_PRIVATE     **HddInfoPrivatePtr
  )
{
  UINTN                                 Index;
  HDD_INFO_MANAGER                      *HddInfoManager;
  HDD_PASSWORD_HDD_INFO_PRIVATE         *HddInfoPrivate;
  LIST_ENTRY                            *Link;
  LIST_ENTRY                            *HddInfoPrivateListHead;

  HddInfoPrivate = NULL;

  HddInfoManager = mHddPasswordPrivate->HddInfoManager;

  HddInfoPrivateListHead = &HddInfoManager->HddInfoPrivateListHead;
  for (Link = GetFirstNode (HddInfoPrivateListHead), Index = 0;
       !IsNull (HddInfoPrivateListHead, Link) && Index < HddInfoManager->NumOfHdd;
       Link = GetNextNode (HddInfoPrivateListHead, Link), Index++) {
    HddInfoPrivate = GET_HDD_INFO_PRIVATE_FROM_LINK (Link);
    if (HddInfoPrivate->DeviceHandleInDxe == HddInfo->DeviceHandleInDxe) {
      break;
    }
  }

  if (Index == HddInfoManager->NumOfHdd) {
    *HddInfoPrivatePtr = NULL;
    return EFI_NOT_FOUND;
  }

  *HddInfoPrivatePtr = HddInfoPrivate;
  return EFI_SUCCESS;
}

/**
  Get the HDD information used in HDD Password.

  @param[in]        This                EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in, out]   HddInfoArray        Return the HDD information array to this pointer.
  @param[in, out]   NumOfHdd            Number of HDD

  @retval           EFI_SUCCESS

**/
EFI_STATUS
GetHddInfoArray (
  IN EFI_HDD_PASSWORD_SERVICE_PROTOCOL  *This,
  IN OUT HDD_PASSWORD_HDD_INFO          **HddInfoArray,
  IN OUT UINTN                          *NumOfHdd
  )
{
  EFI_STATUS                            Status;
  UINTN                                 HddIndex;
  HDD_PASSWORD_HDD_INFO                 *HddInfoArrayPublic;
  HDD_PASSWORD_PRIVATE                  *HddPasswordPrivate;
  HDD_INFO_MANAGER                      *HddInfoManager;
  HDD_PASSWORD_HDD_INFO_PRIVATE         *HddInfoPrivate;
  LIST_ENTRY                            *HddInfoPrivateListHead;
  LIST_ENTRY                            *Link;

  if (This == NULL || HddInfoArray == NULL || NumOfHdd == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HddPasswordPrivate = GET_PRIVATE_FROM_HDD_PASSWORD (This);
  HddInfoManager = HddPasswordPrivate->HddInfoManager;

  HddInfoArrayPublic = NULL;

  //
  // avoid allocate zero size memory, since core code will still reserve 0x40 bytes despite allocate zero size
  //
  if (HddInfoManager->NumOfHdd == 0) {
    return EFI_NOT_FOUND;
  }


  if (mSmst2 == NULL) {
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    (HddInfoManager->NumOfHdd) * sizeof(HDD_PASSWORD_HDD_INFO),
                    (VOID **)&HddInfoArrayPublic
                    );
  } else {
    Status = mSmst2->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      (HddInfoManager->NumOfHdd) * sizeof(HDD_PASSWORD_HDD_INFO),
                      (VOID **)&HddInfoArrayPublic
                      );
  }

  if (HddInfoArrayPublic == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (HddInfoArrayPublic, (HddInfoManager->NumOfHdd) * sizeof(HDD_PASSWORD_HDD_INFO));

  HddInfoPrivateListHead = &HddInfoManager->HddInfoPrivateListHead;
  for (Link = GetFirstNode (HddInfoPrivateListHead), HddIndex = 0;
       !IsNull (HddInfoPrivateListHead, Link) && HddIndex < HddInfoManager->NumOfHdd;
       Link = GetNextNode (HddInfoPrivateListHead, Link), HddIndex++) {
    HddInfoPrivate = GET_HDD_INFO_PRIVATE_FROM_LINK (Link);
    CopyMem (
      &HddInfoArrayPublic[HddIndex],
      &HddInfoPrivate->HddInfo,
      sizeof(HDD_PASSWORD_HDD_INFO)
      );
  }


  *HddInfoArray = HddInfoArrayPublic;
  *NumOfHdd = HddInfoManager->NumOfHdd;

  return EFI_SUCCESS;
}

/**
  Retrieve TCG related password from HddPasswordTable.

  @param[in]        This                EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        HddInfo             The array of HDD information used in HDD Password.
  @param[in]        Type                Type of HDD password.
  @param[out]       Password            The password from TCG extended data.
  @param[out]       PasswordLength      Length of the password from TCG extended data.

  @retval           EFI_SUCCESS         Password found.
  @retval           Others              Password not found.

**/
EFI_STATUS
RetrieveTcgPassword (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_PASSWORD_HDD_INFO             *HddInfo,
  IN  TCG_PASSWORD_TYPE                 Type,
  OUT VOID                              *Password,
  OUT UINTN                             *PasswordLength
  )
{
  EFI_STATUS                            Status;
  UINT8                                 PasswordToHdd[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                                 PasswordToHddLength;
  UINTN                                 HddPasswordTableSize;
  HDD_PASSWORD_TABLE                    *HddPasswordTable;
  HDD_PASSWORD_HDD_INFO_PRIVATE         *HddInfoPrivate;
  UINTN                                 HddPasswordTableIndex;
  UINTN                                 NumOfTable;
  HDD_PASSWORD_TABLE                    *HddPasswordTablePtr;
  TCG_OPAL_EXT_DATA                     *TcgExtDataPtr;

  if (This == NULL || HddInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPrivateFromHddPasswordHddInfo (HddInfo, &HddInfoPrivate);

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  HddPasswordTableSize = 0;
  HddPasswordTable = NULL;

  Status = CommonGetVariableDataAndSize (
             SAVE_HDD_PASSWORD_VARIABLE_NAME,
             &gSaveHddPasswordGuid,
             &HddPasswordTableSize,
             (VOID **) &HddPasswordTable
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SavedPasswordStringProcess (
        DecryptProcess,
        HddPasswordTable,
        HddPasswordTableSize
        );

  ZeroMem (&PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
  PasswordToHddLength = HddInfo->MaxPasswordLengthSupport;

  if (HddPasswordTableSize != 0 && HddPasswordTable != NULL) {
    //
    // Assume ExtData in the table are in the same size
    //
    NumOfTable = NumOfHddPasswordTable (HddPasswordTable ,HddPasswordTableSize);
    HddPasswordTablePtr = (HDD_PASSWORD_TABLE *)HddPasswordTable;
    for (HddPasswordTableIndex = 0; HddPasswordTableIndex < NumOfTable;
         HddPasswordTableIndex++, HddPasswordTablePtr = GetNextTableEntry (HddPasswordTablePtr)) {
      TcgExtDataPtr = (TCG_OPAL_EXT_DATA *)(HddPasswordTablePtr + 1);
      if (HddPasswordTablePtr->ControllerNumber == HddInfo->ControllerNumber &&
          HddPasswordTablePtr->PortNumber       == HddInfo->PortNumber &&
          HddPasswordTablePtr->PortMulNumber    == HddInfo->PortMulNumber &&
          TcgExtDataPtr->Signature == TCG_OPAL_EXT_DATA_SIGNATURE) {

        switch (Type) {

        case SID:
          Status = HddPasswordStringProcess (
                     This,
                     HddPasswordTablePtr->PasswordType,
                     TcgExtDataPtr->SIDPasswordStr,
                     StrLen (TcgExtDataPtr->SIDPasswordStr),
                     (VOID **)&PasswordToHdd,
                     &PasswordToHddLength
                     );
          break;

        case ADMIN1:
          Status = HddPasswordStringProcess (
                     This,
                     HddPasswordTablePtr->PasswordType,
                     TcgExtDataPtr->Admin1PasswordStr,
                     StrLen (TcgExtDataPtr->Admin1PasswordStr),
                     (VOID **)&PasswordToHdd,
                     &PasswordToHddLength
                     );
          break;

        case USER1:
          Status = HddPasswordStringProcess (
                     This,
                     HddPasswordTablePtr->PasswordType,
                     TcgExtDataPtr->User1PasswordStr,
                     StrLen (TcgExtDataPtr->User1PasswordStr),
                     (VOID **)&PasswordToHdd,
                     &PasswordToHddLength
                     );
          break;

        case USER2:
          Status = HddPasswordStringProcess (
                     This,
                     HddPasswordTablePtr->PasswordType,
                     TcgExtDataPtr->User2PasswordStr,
                     StrLen (TcgExtDataPtr->User2PasswordStr),
                     (VOID **)&PasswordToHdd,
                     &PasswordToHddLength
                     );
          break;

        default:
          Status = EFI_UNSUPPORTED;
        }

        if (EFI_ERROR (Status)) {
          return Status;
        }

        if (PasswordToHddLength == 0) {
          return EFI_NOT_FOUND;
        }

        if (PasswordToHddLength > *PasswordLength) {
          return EFI_BUFFER_TOO_SMALL;
        }

        CopyMem ((VOID *)Password, (VOID *)PasswordToHdd, PasswordToHddLength);
        *PasswordLength = PasswordToHddLength;

        break;
      } else {
        Status = EFI_NOT_FOUND;
      }
    }
  }
  return Status;
}

/**
  Update the security status of HDD.

  @param[in]        HddInfo             The HDD information used in HDD Password.

  @retval EFI_SUCCESS         Operation completed successfully.
  @retval Others              Operation was unsuccessful.

**/
EFI_STATUS
UpdateHddSecurityStatus (
  IN  HDD_PASSWORD_HDD_INFO             *HddInfo
  )
{
  EFI_STATUS                            Status;
  ATA_IDENTIFY_DATA                     IdentifyData;
  UINTN                                 Index;
  HDD_INFO_MANAGER                      *HddInfoManager;
  HDD_PASSWORD_HDD_INFO_PRIVATE         *HddInfoPrivate;
  LIST_ENTRY                            *Link;
  LIST_ENTRY                            *HddInfoPrivateListHead;
  EFI_STATUS                            OemSvcStatus;

  HddInfoPrivate = NULL;

  Status = mHddPasswordPrivate->HddPasswordProtocol.HddIdentify (
                                                      &(mHddPasswordPrivate->HddPasswordProtocol),
                                                      HddInfo,
                                                      (UINT16 *)&IdentifyData
                                                      );

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  HddInfoManager = mHddPasswordPrivate->HddInfoManager;

  HddInfoPrivateListHead = &HddInfoManager->HddInfoPrivateListHead;
  for (Link = GetFirstNode (HddInfoPrivateListHead), Index = 0;
       !IsNull (HddInfoPrivateListHead, Link) && Index < HddInfoManager->NumOfHdd;
       Link = GetNextNode (HddInfoPrivateListHead, Link), Index++) {
    HddInfoPrivate = GET_HDD_INFO_PRIVATE_FROM_LINK (Link);
    if (HddInfoPrivate->DeviceHandleInDxe == HddInfo->DeviceHandleInDxe) {
      HddInfoPrivate->HddInfo.HddSecurityStatus = (UINT8) IdentifyData.security_status;
      break;
    }
  }

  if (Index == HddInfoManager->NumOfHdd) {
    return EFI_NOT_FOUND;
  }

  if (HddInfoPrivate == NULL) {
    return EFI_NOT_FOUND;
  }

  HddInfo->HddSecurityStatus = (UINT8) IdentifyData.security_status;

  UpdateLegacyEbda (&(mHddPasswordPrivate->HddPasswordProtocol), HddInfo);

  //
  //  OemService for update security status
  //
  if (!mSmst2) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDxeHddUpdateSecurityStatus \n"));
    OemSvcStatus = OemSvcDxeHddUpdateSecurityStatus (
                     &(mHddPasswordPrivate->HddPasswordProtocol),
                     &(HddInfoPrivate->HddInfo),  //HddInfo,
                     Index
                     );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDxeHddUpdateSecurityStatus Status: %r\n", OemSvcStatus));
  } else {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcSmmHddUpdateSecurityStatus \n"));
    OemSvcStatus = OemSvcSmmHddUpdateSecurityStatus (
                      &(mHddPasswordPrivate->HddPasswordProtocol),
                      &(HddInfoPrivate->HddInfo),  //HddInfo,
                      Index
                      );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcSmmHddUpdateSecurityStatus Status: %r\n", OemSvcStatus));
  }

  return EFI_SUCCESS;
}


/**
  HDD identify command service.

  @param[in]        This                EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        HddInfo             The HDD information used in HDD Password.
  @param[in, out]   IdentifyData        Return the HDD identify data to this pointer.

  @retval           EFI_SUCCESS         Ata command success.

**/
EFI_STATUS
HddIdentify (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_PASSWORD_HDD_INFO             *HddInfo,
  IN  OUT UINT16                        *IdentifyData
  )
{
  EFI_STATUS                            Status;

  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL *StorageSecurityCommand;
  UINTN                                 PayloadBufferSize;
  UINTN                                 PayloadTransferSize;
  UINT64                                *CmdTimeoutTable;
  UINT64                                Timeout;
  HDD_PASSWORD_HDD_INFO_PRIVATE         *HddInfoPrivate;

  BOOLEAN                               InstalledByInsydeDriver;

  InstalledByInsydeDriver = FALSE;

  if (This == NULL || HddInfo == NULL || IdentifyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&mIdentifyData, sizeof (ATA_IDENTIFY_DATA));

  HddInfoPrivate = NULL;

  Status = GetPrivateFromHddPasswordHddInfo (HddInfo, &HddInfoPrivate);

  if (Status != EFI_SUCCESS || HddInfoPrivate == NULL) {
    return Status;
  }

  if (!mSmst2) {
    StorageSecurityCommand = HddInfoPrivate->StorageSecurityCommandInDxe;
  } else {
     //
     // when using 3rd party Uefi driver for HddPassword feature, might not installed Sscp in SMM
     //
     if (HddInfoPrivate->StorageSecurityCommandInSmm == NULL) {
       return EFI_UNSUPPORTED;
     }
     StorageSecurityCommand = HddInfoPrivate->StorageSecurityCommandInSmm;
  }

  if (HddInfo->StorageTcgSecuritySupported) {
    Status = OpalDiskUpdateStatus (StorageSecurityCommand, &HddInfoPrivate->OpalDisk);
    if (Status == EFI_SUCCESS) {
      HddInfoPrivate->HddInfo.HddSecurityStatus = ((HddInfoPrivate->OpalDisk.LockingFeature.LockingSupported << 0) |
                                                   (HddInfoPrivate->OpalDisk.LockingFeature.LockingEnabled   << 1) |
                                                   (HddInfoPrivate->OpalDisk.LockingFeature.Locked           << 2));

      mIdentifyData.security_status              = (UINT16)(HddInfoPrivate->HddInfo.HddSecurityStatus);
      mIdentifyData.command_set_feature_enb_85   = (UINT16)(IsUser1Enabled (This, StorageSecurityCommand, HddInfo, &HddInfoPrivate->OpalDisk));
      mIdentifyData.retired_20_21[0]             = (UINT16)(IsPsidEnabled (This, StorageSecurityCommand, HddInfo, &HddInfoPrivate->OpalDisk));
      mIdentifyData.time_for_security_erase_unit = (UINT16)(HddInfoPrivate->OpalDisk.EstimateTimeCost);
    }
  } else {
     InstalledByInsydeDriver = HddInfoPrivate->InstalledByInsyde;
     CmdTimeoutTable = (UINT64*)PcdGetPtr (PcdH2OHddPasswordCmdTimeoutTable);
     Timeout = CmdTimeoutTable[TIMEOUT_IDENTIFY_DEVICE_INDEX];
     PayloadBufferSize   = sizeof (ATA_IDENTIFY_DATA);
     PayloadTransferSize = 0;
     ZeroMem ((VOID*)&mSecurityCheckStateData, sizeof(ATA_DEVICE_SERVER_SECURITY_STATE_DATA));
     Status = StorageSecurityCommand->ReceiveData (
                                        StorageSecurityCommand,
                                        HddInfoPrivate->BlockIoMediaId,
                                        InstalledByInsydeDriver ? Timeout : TRUSTED_COMMAND_TIMEOUT_NS,
                                        InstalledByInsydeDriver ? H2O_SECURITY_PROTOCOL_ID : TCG_SECURITY_PROTOCOL_ATA_DEVICE_SERVER_PASS,
                                        InstalledByInsydeDriver ? RCV_IDENTIFY_DATA : SwapBytes16(TCG_SP_SPECIFIC_PROTOCOL_LIST),
                                        InstalledByInsydeDriver ? PayloadBufferSize : sizeof(ATA_DEVICE_SERVER_SECURITY_STATE_DATA),
                                        InstalledByInsydeDriver ? (VOID*)&mIdentifyData : (VOID*)&mSecurityCheckStateData,
                                        &PayloadTransferSize
                                        );
     if (!InstalledByInsydeDriver && !EFI_ERROR (Status)) {
       mIdentifyData.security_status |= mSecurityCheckStateData.Byte9;
       mIdentifyData.master_password_identifier |= mSecurityCheckStateData.MasterPasswordIdentifier;
     }
  }

  if (!EFI_ERROR(Status)) {
    CopyMem ((VOID *)IdentifyData, (VOID *)&mIdentifyData, sizeof (ATA_IDENTIFY_DATA));
  }

  return Status;
}

/**
  Send HDD freeze command service.

  @param[in]        This                EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        HddInfo             The HDD information used in HDD Password.

  @retval           EFI_SUCCESS         Ata command success.

**/
EFI_STATUS
HddFreeze (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL    *This,
  IN  HDD_PASSWORD_HDD_INFO                *HddInfo
  )
{
  EFI_STATUS                               Status;
  EFI_STATUS                               CmdStatus;
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *StorageSecurityCommand;
  UINT16                                   MasterPasswordIdentifier;
  H2O_STORAGE_SECURITY_COMMAND_SEND_HEADER *StorageSecurityCmdSendHeader;
  UINTN                                    PayloadBufferSize;
  UINT64                                   *CmdTimeoutTable;
  UINT64                                   Timeout;
  HDD_PASSWORD_HDD_INFO_PRIVATE            *HddInfoPrivate;

  UINT16                                   SecurityProtocolSpecificData;
  BOOLEAN                                  InstalledByInsydeDriver;

  InstalledByInsydeDriver = FALSE;

  if (This == NULL || HddInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HddInfoPrivate = NULL;

  Status = GetPrivateFromHddPasswordHddInfo (HddInfo, &HddInfoPrivate);

  if (Status != EFI_SUCCESS || HddInfoPrivate == NULL) {
    return Status;
  }

  if (HddInfo->StorageTcgSecuritySupported) {
    return EFI_UNSUPPORTED;
  }

  if (!mSmst2) {
    StorageSecurityCommand = HddInfoPrivate->StorageSecurityCommandInDxe;
  } else {
     if (HddInfoPrivate->StorageSecurityCommandInSmm == NULL) {
       return EFI_UNSUPPORTED;
     }
     StorageSecurityCommand = HddInfoPrivate->StorageSecurityCommandInSmm;
  }

  StorageSecurityCmdSendHeader = NULL;
  StorageSecurityCmdSendHeader = AllocateBuffer (sizeof (H2O_STORAGE_SECURITY_COMMAND_SEND_HEADER));

  if (StorageSecurityCmdSendHeader == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (StorageSecurityCmdSendHeader, sizeof (H2O_STORAGE_SECURITY_COMMAND_SEND_HEADER));

  if (HddInfo->MasterPasswordIdentifier != 0x0 && HddInfo->MasterPasswordIdentifier != 0xFFFF) {
    MasterPasswordIdentifier = PcdGet16(PcdH2OHddPasswordMasterPasswordIdentifier);
    if ( MasterPasswordIdentifier != DEFAULT_MASTER_PASSWORD_IDENTIFIER) {
      HddInfo->MasterPasswordIdentifier = MasterPasswordIdentifier;
    }
  }

  InstalledByInsydeDriver = HddInfoPrivate->InstalledByInsyde;
  StorageSecurityCmdSendHeader->HeaderSize = sizeof (H2O_STORAGE_SECURITY_COMMAND_SEND_HEADER);
  StorageSecurityCmdSendHeader->Control = 0;
  StorageSecurityCmdSendHeader->Identifier = HddInfo->MasterPasswordIdentifier;
  StorageSecurityCmdSendHeader->MessageLength = 0;


  CmdTimeoutTable = (UINT64*)PcdGetPtr (PcdH2OHddPasswordCmdTimeoutTable);
  Timeout = CmdTimeoutTable[TIMEOUT_SECURITY_FREEZE_LOCK_INDEX];
  PayloadBufferSize = sizeof (H2O_STORAGE_SECURITY_COMMAND_SEND_HEADER);
  SecurityProtocolSpecificData = ATA_SECURITY_FREEZE_LOCK;
  CmdStatus = StorageSecurityCommand->SendData(
                                        StorageSecurityCommand,
                                        HddInfoPrivate->BlockIoMediaId,
                                        InstalledByInsydeDriver ? Timeout : TRUSTED_COMMAND_TIMEOUT_NS,
                                        InstalledByInsydeDriver ? H2O_SECURITY_PROTOCOL_ID : TCG_SECURITY_PROTOCOL_ATA_DEVICE_SERVER_PASS,
                                        InstalledByInsydeDriver ? SEND_PWD_FREEZE : SwapBytes16(SecurityProtocolSpecificData),
                                        InstalledByInsydeDriver ? PayloadBufferSize : 0,
                                        InstalledByInsydeDriver ? StorageSecurityCmdSendHeader : NULL
                                        );
  UpdateHddSecurityStatus (HddInfo);

  if (StorageSecurityCmdSendHeader != NULL) {
    FreeBuffer (StorageSecurityCmdSendHeader);
  }

  return CmdStatus;
}

/**
  Send TCG security command.

  @param[in]        This                EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        CmdOpCode           Command operation code.
  @param[in]        HddInfo             The HDD information used in HDD Password.
  @param[in]        UserOrMaster        User or Master flag.
  @param[in]        PasswordPtr         Password string.
  @param[in]        PasswordLength      Password string length.

  @retval           EFI_SUCCESS         ATA command success.

**/
EFI_STATUS
SendTcgSecurityCmd (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL    *This,
  IN  UINT8                                CmdOpCode,
  IN  HDD_PASSWORD_HDD_INFO                *HddInfo,
  IN  BOOLEAN                              UserOrMaster,
  IN  UINT8                                *PasswordPtr,
  IN  UINTN                                PasswordLength
  )
{
  EFI_STATUS                               Status;
  TCG_RESULT                               TcgResult;
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *StorageSecurityCommand;
  UINT64                                   *CmdTimeoutTable;
  UINT64                                   Timeout;
  HDD_PASSWORD_HDD_INFO_PRIVATE            *HddInfoPrivate;
  OPAL_SESSION                             Session;
  OPAL_DISK_INFO                           *OpalDisk;
  BOOLEAN                                  PasswordFailed;
  UINT8                                    AdminPassword[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                                    AdminPasswordLength;
  UINT8                                    MethodStatus;

  HddInfoPrivate = NULL;

  Status = GetPrivateFromHddPasswordHddInfo (HddInfo, &HddInfoPrivate);

  if (Status != EFI_SUCCESS || HddInfoPrivate == NULL) {
    return Status;
  }

  if (!mSmst2) {
    StorageSecurityCommand = HddInfoPrivate->StorageSecurityCommandInDxe;
  } else {
    StorageSecurityCommand = HddInfoPrivate->StorageSecurityCommandInSmm;
  }

  OpalDisk = &HddInfoPrivate->OpalDisk;

  ZeroMem(&Session, sizeof(Session));
  Session.Sscp          = StorageSecurityCommand;
  Session.MediaId       = OpalDisk->MediaId;
  Session.OpalBaseComId = OpalDisk->OpalBaseComId;

  Timeout = 0;
  CmdTimeoutTable = (UINT64*)PcdGetPtr (PcdH2OHddPasswordCmdTimeoutTable);
  switch (CmdOpCode) {

  case ATA_CMD_SECURITY_SET_PASSWORD:
    Timeout = CmdTimeoutTable[TIMEOUT_SECURITY_SET_PASSWORD_INDEX];
    if (!HddInfoPrivate->OpalDisk.LockingFeature.LockingEnabled) {
      TcgResult = OpalUtilGetMsid (&Session, OpalDisk->Msid, OPAL_MSID_LENGHT, &OpalDisk->MsidLength);
      if (TcgResult != TcgResultSuccess) {
        return EFI_DEVICE_ERROR;
      }
      //
      // Set AdminSP\SID and LockingSP\Admin1 and enable global locking.
      //
      TcgResult = OpalUtilSetAdminPasswordAsSid (
                    &Session,
                    (VOID *)OpalDisk->Msid,
                    (UINT32)OpalDisk->MsidLength,
                    (VOID *)PasswordPtr,
                    (UINT32)PasswordLength
                    );
      if ((TcgResult == TcgResultSuccess) && (UserOrMaster == USER_PSW)) {
        //
        // Enable global locking range
        //
        TcgResult = OpalUtilSetOpalLockingRange (
                      &Session,
                      (VOID *)PasswordPtr,
                      (UINT32)PasswordLength,
                      OPAL_LOCKING_SP_LOCKING_GLOBALRANGE,
                      0,
                      0,
                      TRUE,
                      TRUE,
                      FALSE,
                      FALSE
                      );
        if (TcgResult == TcgResultSuccess) {
          //
          // Set LockingSP\User1
          //
          TcgResult = OpalUtilSetUserPassword (
                        &Session,
                        (VOID *)PasswordPtr,
                        (UINT32)PasswordLength,
                        (VOID *)PasswordPtr,
                        (UINT32)PasswordLength
                        );
        }
      }
    } else { // Locking Enabled
      if (UserOrMaster == USER_PSW) {
        if (IsUser1Enabled (This, StorageSecurityCommand, HddInfo, OpalDisk)) {
          //
          // Change USER1 password
          //
          TcgResult = OpalUtilSetUserPassword (
                        &Session,
                        (VOID *)HddInfoPrivate->CachedPassword,
                        (UINT32)HddInfoPrivate->CachedPasswordLength,
                        (VOID *)PasswordPtr,
                        (UINT32)PasswordLength
                        );
        } else {
          AdminPasswordLength = sizeof (AdminPassword);
          ZeroMem ((VOID *)AdminPassword, AdminPasswordLength);
          //
          // Get Admin1 password
          //
          Status = RetrieveTcgPassword (
                     This,
                     HddInfo,
                     ADMIN1,
                     AdminPassword,
                     &AdminPasswordLength
                     );
          if (EFI_ERROR (Status)) {
            return EFI_DEVICE_ERROR;
          }
          //
          // Enable global locking range
          //
          TcgResult = OpalUtilSetOpalLockingRange (
                        &Session,
                        (VOID *)AdminPassword,
                        (UINT32)AdminPasswordLength,
                        OPAL_LOCKING_SP_LOCKING_GLOBALRANGE,
                        0,
                        0,
                        TRUE,
                        TRUE,
                        FALSE,
                        FALSE
                        );
          if (TcgResult == TcgResultSuccess) {
            //
            // Enable USER1
            //
            TcgResult = OpalUtilSetUserPassword (
                          &Session,
                          (VOID *)AdminPassword,
                          (UINT32)AdminPasswordLength,
                          (VOID *)PasswordPtr,
                          (UINT32)PasswordLength
                          );
          }
        }
      } else {
        TcgResult = OpalUtilSetAdminPassword (
                      &Session,
                      (VOID *)HddInfoPrivate->CachedPassword,
                      (UINT32)HddInfoPrivate->CachedPasswordLength,
                      (VOID *)PasswordPtr,
                      (UINT32)PasswordLength
                      );
      }
    }

    //
    // Clear secret after use
    //
    ZeroMem ((VOID *)HddInfoPrivate->CachedPassword, sizeof (HddInfoPrivate->CachedPassword));
    HddInfoPrivate->CachedPasswordLength = 0;
    ZeroMem ((VOID *)AdminPassword, sizeof (AdminPassword));
    AdminPasswordLength = 0;

    if (TcgResult != TcgResultSuccess) {
      return EFI_DEVICE_ERROR;
    }

    break;

  case ATA_CMD_SECURITY_UNLOCK:
    if (UserOrMaster == USER_PSW) {
      TcgResult = OpalStartSession (
                    &Session,
                    OPAL_UID_LOCKING_SP,
                    TRUE,
                    (UINT32)PasswordLength,
                    (VOID *)PasswordPtr,
                    OPAL_LOCKING_SP_USER1_AUTHORITY,
                    &MethodStatus
                    );
    } else {
      TcgResult = OpalStartSession (
                    &Session,
                    OPAL_UID_LOCKING_SP,
                    TRUE,
                    (UINT32)PasswordLength,
                    (VOID *)PasswordPtr,
                    OPAL_LOCKING_SP_ADMIN1_AUTHORITY,
                    &MethodStatus
                    );
    }
    if (TcgResult == TcgResultSuccess && MethodStatus == TCG_METHOD_STATUS_CODE_SUCCESS) {
      TcgResult = OpalUpdateGlobalLockingRange (
                    &Session,
                    FALSE,
                    FALSE,
                    &MethodStatus
                    );
      OpalEndSession (&Session);
    }

    if (TcgResult != TcgResultSuccess || MethodStatus != TCG_METHOD_STATUS_CODE_SUCCESS) {
      return EFI_DEVICE_ERROR;
    }

    ZeroMem ((VOID *)HddInfoPrivate->CachedPassword, sizeof (HddInfoPrivate->CachedPassword));
    CopyMem ((VOID *)HddInfoPrivate->CachedPassword, (VOID *)PasswordPtr, PasswordLength);
    HddInfoPrivate->CachedPasswordLength = PasswordLength;

    break;

  case ATA_CMD_SECURITY_DISABLE_PASSWORD:
    if (UserOrMaster == USER_PSW) {
      TcgResult =  OpalUtilVerifyPassword (
                     &Session,
                     (VOID *)HddInfoPrivate->CachedPassword,
                     (UINT32)HddInfoPrivate->CachedPasswordLength,
                     OPAL_LOCKING_SP_USER1_AUTHORITY
                     );
      if (TcgResult != TcgResultSuccess) {
        DEBUG ((DEBUG_INFO, "Verify User Password failed\n"));
        return EFI_NOT_READY;
      }

      AdminPasswordLength = sizeof (AdminPassword);
      ZeroMem ((VOID *)AdminPassword, AdminPasswordLength);
      //
      // Get stored admin password
      //
      Status = RetrieveTcgPassword (
                 This,
                 HddInfo,
                 ADMIN1,
                 AdminPassword,
                 &AdminPasswordLength
                 );
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }

      TcgResult = OpalUtilDisableUser (
                    &Session,
                    (VOID *)AdminPassword,
                    (UINT32)AdminPasswordLength,
                    &PasswordFailed
                    );
      if (TcgResult == TcgResultSuccess) {
        //
        // Disable global locking range
        //
        TcgResult = OpalUtilSetOpalLockingRange (
                      &Session,
                      (VOID *)AdminPassword,
                      (UINT32)AdminPasswordLength,
                      OPAL_LOCKING_SP_LOCKING_GLOBALRANGE,
                      0,
                      0,
                      FALSE,
                      FALSE,
                      FALSE,
                      FALSE
                      );
      }
    } else {
      TcgResult = OpalUtilGetMsid (&Session, OpalDisk->Msid, OPAL_MSID_LENGHT, &OpalDisk->MsidLength);
      if (TcgResult != TcgResultSuccess) {
        return EFI_DEVICE_ERROR;
      }
      //
      // Reverts the device using the RevertSP method, and restore the MSID.
      //
      TcgResult = OpalUtilRevert (
                    &Session,
                    ((HddInfoPrivate->OpalDisk.SupportedAttributes.MediaEncryption == 0) &&
                     (HddInfoPrivate->OpalDisk.SupportedAttributes.PyriteSsc == 1))
                    ? FALSE : TRUE, // KeepUserData
                    (VOID *)PasswordPtr,
                    (UINT32)PasswordLength,
                    &PasswordFailed,
                    (VOID *)OpalDisk->Msid,
                    (UINT32)OpalDisk->MsidLength
                    );
    }
    //
    // Clear secret after use
    //
    ZeroMem ((VOID *)HddInfoPrivate->CachedPassword, sizeof (HddInfoPrivate->CachedPassword));
    HddInfoPrivate->CachedPasswordLength = 0;
    ZeroMem ((VOID *)AdminPassword, sizeof (AdminPassword));
    AdminPasswordLength = 0;

    //
    // If Password failed, return invalid password
    //
    if (PasswordFailed) {
      DEBUG ((DEBUG_INFO, "returning EFI_NOT_READY to indicate Password was not correct\n"));
      return EFI_NOT_READY;
    }

    if (TcgResult != TcgResultSuccess) {
      return EFI_DEVICE_ERROR;
    }

    break;

  case H2O_HDD_PASSWORD_CMD_COMRESET:
    if (!IsUser1Enabled (This, StorageSecurityCommand, HddInfo, OpalDisk)) {
      return EFI_SUCCESS;
    }

    if (UserOrMaster == USER_PSW) {
      TcgResult = OpalStartSession (
                    &Session,
                    OPAL_UID_LOCKING_SP,
                    TRUE,
                    (UINT32)PasswordLength,
                    (VOID *)PasswordPtr,
                    OPAL_LOCKING_SP_USER1_AUTHORITY,
                    &MethodStatus
                    );
    } else {
      TcgResult = OpalStartSession (
                    &Session,
                    OPAL_UID_LOCKING_SP,
                    TRUE,
                    (UINT32)PasswordLength,
                    (VOID *)PasswordPtr,
                    OPAL_LOCKING_SP_ADMIN1_AUTHORITY,
                    &MethodStatus
                    );
    }
    if (TcgResult == TcgResultSuccess && MethodStatus == TCG_METHOD_STATUS_CODE_SUCCESS) {
      TcgResult = OpalUpdateGlobalLockingRange (
                    &Session,
                    TRUE,
                    TRUE,
                    &MethodStatus
                    );
      OpalEndSession (&Session);
    }

    if (TcgResult != TcgResultSuccess || MethodStatus != TCG_METHOD_STATUS_CODE_SUCCESS) {
      return EFI_DEVICE_ERROR;
    }

    break;

  case H2O_HDD_PASSWORD_TCG_PSID_REVERT:
    if (UserOrMaster == USER_PSW) {
      TcgResult = OpalUtilPsidRevert (
                    &Session,
                    (VOID *)PasswordPtr,
                    (UINT32)PasswordLength
                    );
      if (TcgResult != TcgResultSuccess) {
        return EFI_DEVICE_ERROR;
      }
    } else {
      TcgResult = OpalStartSession (
                    &Session,
                    OPAL_UID_ADMIN_SP,
                    TRUE,
                    (UINT32)PasswordLength,
                    (VOID *)PasswordPtr,
                    OPAL_ADMIN_SP_PSID_AUTHORITY,
                    &MethodStatus
                    );
      if (TcgResult == TcgResultSuccess && MethodStatus == TCG_METHOD_STATUS_CODE_SUCCESS) {
        OpalEndSession (&Session);
      } else {
        return EFI_DEVICE_ERROR;
      }
    }

    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Send HDD security command.

  @param[in]        This                EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        CmdOpCode           Command operation code.
  @param[in]        HddInfo             The HDD information used in HDD Password.
  @param[in]        UserOrMaster        User or Master flag.
  @param[in]        PasswordPtr         Password string.
  @param[in]        PasswordLength      Password string length.

  @retval           EFI_SUCCESS         ATA command success.

**/
EFI_STATUS
SendHddSecurityCmd (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL    *This,
  IN  UINT8                                CmdOpCode,
  IN  HDD_PASSWORD_HDD_INFO                *HddInfo,
  IN  BOOLEAN                              UserOrMaster,
  IN  UINT8                                *PasswordPtr,
  IN  UINTN                                PasswordLength
  )
{
  EFI_STATUS                               Status;
  EFI_STATUS                               CmdStatus;

  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *StorageSecurityCommand;

  UINT16                                   MasterPasswordIdentifier;
  UINT8                                    MaxPasswordLengthSupport;

  H2O_STORAGE_SECURITY_COMMAND_SEND_HEADER *StorageSecurityCmdSendHeader;
  UINT16                                   SecurityProtocolSpecificData;
  UINTN                                    PayloadBufferSize;
  UINT64                                   *CmdTimeoutTable;
  UINT64                                   Timeout;

  HDD_PASSWORD_HDD_INFO_PRIVATE            *HddInfoPrivate;

  ATA_DEVICE_SERVER_SEND_CMD_DATA          *AtaSecuritySendData;
  BOOLEAN                                  InstalledByInsydeDriver;

  AtaSecuritySendData = NULL;
  InstalledByInsydeDriver = FALSE;
  if (This == NULL || HddInfo == NULL || PasswordPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((PasswordLength > HddInfo->MaxPasswordLengthSupport) || (PasswordLength == 0)) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (HddInfo->StorageTcgSecuritySupported) {
    CmdStatus = SendTcgSecurityCmd (
                  This,
                  CmdOpCode,
                  HddInfo,
                  UserOrMaster,
                  PasswordPtr,
                  PasswordLength
                  );
  } else {
    HddInfoPrivate = NULL;

    Status = GetPrivateFromHddPasswordHddInfo (HddInfo, &HddInfoPrivate);

    if (Status != EFI_SUCCESS || HddInfoPrivate == NULL) {
      return Status;
    }

    Timeout = 0;
    CmdTimeoutTable = (UINT64*)PcdGetPtr (PcdH2OHddPasswordCmdTimeoutTable);
    InstalledByInsydeDriver = HddInfoPrivate->InstalledByInsyde;
    switch (CmdOpCode) {

    case ATA_CMD_SECURITY_SET_PASSWORD:
      SecurityProtocolSpecificData = InstalledByInsydeDriver ? SEND_PWD_SET : ATA_SECURITY_SET_PASSWORD;
      Timeout = InstalledByInsydeDriver ? CmdTimeoutTable[TIMEOUT_SECURITY_SET_PASSWORD_INDEX] : TRUSTED_COMMAND_TIMEOUT_NS;
      break;

    case ATA_CMD_SECURITY_UNLOCK:
      SecurityProtocolSpecificData = InstalledByInsydeDriver ? SEND_PWD_UNLOCK : ATA_SECURITY_UNLOCK;
      Timeout = InstalledByInsydeDriver ? CmdTimeoutTable[TIMEOUT_SECURITY_UNLOCK_INDEX] : TRUSTED_COMMAND_TIMEOUT_NS;
      break;

    case ATA_CMD_SECURITY_DISABLE_PASSWORD:
      SecurityProtocolSpecificData = InstalledByInsydeDriver ? SEND_PWD_CLEAR : ATA_SECURITY_DISABLE_PASSWORD;
      Timeout = InstalledByInsydeDriver ? CmdTimeoutTable[TIMEOUT_SECURITY_DISABLE_PASSWORD_INDEX] : TRUSTED_COMMAND_TIMEOUT_NS;
      break;

    case H2O_HDD_PASSWORD_CMD_COMRESET:
      SecurityProtocolSpecificData = InstalledByInsydeDriver ? SEND_PWD_RESET_SECURITY_STATUS : ATA_SECURITY_UNSUPPORTED_COMMAND;
      Timeout = InstalledByInsydeDriver ? CmdTimeoutTable[TIMEOUT_SECURITY_RESET_STATUS_INDEX] : TRUSTED_COMMAND_TIMEOUT_NS;
      break;

    default:
      return EFI_INVALID_PARAMETER;
    }


    Status = EFI_SUCCESS;

    MaxPasswordLengthSupport = HddInfo->MaxPasswordLengthSupport;

    if ((UINT8)PasswordLength > (MaxPasswordLengthSupport + 1) || (PasswordLength == 0)) {
      return EFI_OUT_OF_RESOURCES;
    }

    if (!mSmst2) {
      StorageSecurityCommand = HddInfoPrivate->StorageSecurityCommandInDxe;
    } else {
       if (HddInfoPrivate->StorageSecurityCommandInSmm == NULL) {
         return EFI_UNSUPPORTED;
       }
      StorageSecurityCommand = HddInfoPrivate->StorageSecurityCommandInSmm;
    }

    StorageSecurityCmdSendHeader = NULL;
    StorageSecurityCmdSendHeader =
      AllocateBuffer (sizeof (H2O_STORAGE_SECURITY_COMMAND_SEND_HEADER) + MaxPasswordLengthSupport);

    if (StorageSecurityCmdSendHeader == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    ZeroMem (StorageSecurityCmdSendHeader, (sizeof (H2O_STORAGE_SECURITY_COMMAND_SEND_HEADER) + MaxPasswordLengthSupport));

    if (HddInfo->MasterPasswordIdentifier != 0x0 && HddInfo->MasterPasswordIdentifier != 0xFFFF) {
      MasterPasswordIdentifier = PcdGet16(PcdH2OHddPasswordMasterPasswordIdentifier);
      if ( MasterPasswordIdentifier != DEFAULT_MASTER_PASSWORD_IDENTIFIER) {
        HddInfo->MasterPasswordIdentifier = MasterPasswordIdentifier;
      }
    }

    StorageSecurityCmdSendHeader->HeaderSize = sizeof (H2O_STORAGE_SECURITY_COMMAND_SEND_HEADER);
    StorageSecurityCmdSendHeader->Control = UserOrMaster;
    StorageSecurityCmdSendHeader->Identifier = HddInfo->MasterPasswordIdentifier;
    StorageSecurityCmdSendHeader->MessageLength = PasswordLength;
    CopyMem (&StorageSecurityCmdSendHeader[1], PasswordPtr, MIN (MaxPasswordLengthSupport, PasswordLength));

    PayloadBufferSize = sizeof (H2O_STORAGE_SECURITY_COMMAND_SEND_HEADER) + MaxPasswordLengthSupport;

    AtaSecuritySendData = (ATA_DEVICE_SERVER_SEND_CMD_DATA*)AllocateBuffer (sizeof (ATA_DEVICE_SERVER_SEND_CMD_DATA));
    if (AtaSecuritySendData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    AtaSecuritySendData->Byte1 = UserOrMaster;
    CopyMem (&AtaSecuritySendData->Password, PasswordPtr, MIN (MaxPasswordLengthSupport, PasswordLength));
    CopyMem (&AtaSecuritySendData->MasterPasswordIdentifier, &HddInfo->MasterPasswordIdentifier, sizeof (UINT16));

    CmdStatus = StorageSecurityCommand->SendData(
                                          StorageSecurityCommand,
                                          HddInfoPrivate->BlockIoMediaId,
                                          Timeout,
                                          InstalledByInsydeDriver ? H2O_SECURITY_PROTOCOL_ID : TCG_SECURITY_PROTOCOL_ATA_DEVICE_SERVER_PASS,
                                          InstalledByInsydeDriver ? SecurityProtocolSpecificData : SwapBytes16(SecurityProtocolSpecificData),
                                          InstalledByInsydeDriver ? PayloadBufferSize : sizeof(ATA_DEVICE_SERVER_SEND_CMD_DATA),
                                          InstalledByInsydeDriver ? (VOID*)StorageSecurityCmdSendHeader : (VOID*)AtaSecuritySendData
                                          );

    if (StorageSecurityCmdSendHeader != NULL) {
      FreeBuffer (StorageSecurityCmdSendHeader);
    }

  }

  UpdateHddSecurityStatus (HddInfo);

  return CmdStatus;

}

/**
  Set HDD password command services.

  @param[in]        This                EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        HddInfo             The HDD information used in HDD Password.
  @param[in]        UserOrMaster        User or Master flag.
  @param[in]        PasswordPtr         Password string.
  @param[in]        PasswordLength      Password string length.

  @retval           EFI_SUCCESS         Set HDD password successfully.
**/
EFI_STATUS
SetHddPassword (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_PASSWORD_HDD_INFO             *HddInfo,
  IN  BOOLEAN                           UserOrMaster,
  IN  UINT8                             *PasswordPtr,
  IN  UINTN                             PasswordLength
  )
{
  EFI_STATUS                            Status;

  if (This == NULL ||
      HddInfo == NULL ||
      (UserOrMaster != USER_PSW && UserOrMaster != MASTER_PSW) ||
      PasswordPtr == NULL ||
      PasswordLength == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (!mSmst2) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDxeHddSetPassword \n"));
    Status = OemSvcDxeHddSetPassword (
               This,
               HddInfo,
               UserOrMaster,
               PasswordPtr,
               PasswordLength
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDxeHddSetPassword Status: %r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcSmmHddSetPassword \n"));
    Status = OemSvcSmmHddSetPassword (
               This,
               HddInfo,
               UserOrMaster,
               PasswordPtr,
               PasswordLength
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcSmmHddSetPassword Status: %r\n", Status));
  }

  if (Status == EFI_UNSUPPORTED) {
    Status = SendHddSecurityCmd (
               This,
               ATA_CMD_SECURITY_SET_PASSWORD,
               HddInfo,
               UserOrMaster,
               PasswordPtr,
               PasswordLength
               );
  } else if (Status == EFI_MEDIA_CHANGED) {
    //
    //  Oem feature code may occur error.
    //

    return EFI_ABORTED;
  }

  return Status;
}

/**
  Unlock HDD password command services.

  @param[in]        This                EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        HddInfo             The HDD information used in HDD Password.
  @param[in]        UserOrMaster        User or Master flag.
  @param[in]        PasswordPtr         Password string.
  @param[in]        PasswordLength      Password string length.

  @retval           EFI_SUCCESS         Unlock HDD password successfully.
**/
EFI_STATUS
UnlockHddPassword (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN  HDD_PASSWORD_HDD_INFO             *HddInfo,
  IN  BOOLEAN                           UserOrMaster,
  IN  UINT8                             *PasswordPtr,
  IN  UINTN                             PasswordLength
  )
{
  EFI_STATUS                            Status;

  if (This == NULL ||
      HddInfo == NULL ||
      PasswordPtr == NULL ||
      PasswordLength == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (!mSmst2) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDxeHddUnlockPassword \n"));
    Status = OemSvcDxeHddUnlockPassword (
               This,
               HddInfo,
               UserOrMaster,
               PasswordPtr,
               PasswordLength
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDxeHddUnlockPassword Status: %r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcSmmHddUnlockPassword \n"));
    Status = OemSvcSmmHddUnlockPassword (
               This,
               HddInfo,
               UserOrMaster,
               PasswordPtr,
               PasswordLength
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcSmmHddUnlockPassword Status: %r\n", Status));
  }

  if (Status == EFI_UNSUPPORTED) {
    Status = SendHddSecurityCmd (
               This,
               ATA_CMD_SECURITY_UNLOCK,
               HddInfo,
               UserOrMaster,
               PasswordPtr,
               PasswordLength
               );
  } else if (Status == EFI_MEDIA_CHANGED) {
    //
    //  Oem feature code may occur error.
    //

    return EFI_ABORTED;
  }

  return Status;
}

/**
  Disable HDD password command services.

  @param[in]        This                EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        HddInfo             The HDD information used in HDD Password.
  @param[in]        UserOrMaster        User or Master flag.
  @param[in]        PasswordPtr         Password string.
  @param[in]        PasswordLength      Password string length.

  @retval           EFI_SUCCESS         Disable HDD password successfully.
**/
EFI_STATUS
DisableHddPassword (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL  *This,
  IN  HDD_PASSWORD_HDD_INFO              *HddInfo,
  IN  BOOLEAN                            UserOrMaster,
  IN  UINT8                              *PasswordPtr,
  IN  UINTN                              PasswordLength
  )
{
  EFI_STATUS                             Status;

  if (This == NULL ||
      HddInfo == NULL ||
      PasswordPtr == NULL ||
      PasswordLength == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (!mSmst2) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDxeHddDisablePassword \n"));
    Status = OemSvcDxeHddDisablePassword (
               This,
               HddInfo,
               UserOrMaster,
               PasswordPtr,
               PasswordLength
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDxeHddDisablePassword Status %r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcSmmHddDisablePassword \n"));
    Status = OemSvcSmmHddDisablePassword (
               This,
               HddInfo,
               UserOrMaster,
               PasswordPtr,
               PasswordLength
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcSmmHddDisablePassword Status: %r\n", Status));
  }

  if (Status == EFI_UNSUPPORTED) {
    Status = SendHddSecurityCmd (
               This,
               ATA_CMD_SECURITY_DISABLE_PASSWORD,
               HddInfo,
               UserOrMaster,
               PasswordPtr,
               PasswordLength
               );
  } else if (Status == EFI_MEDIA_CHANGED) {
    //
    //  Oem feature code may occur error.
    //

    return EFI_ABORTED;
  }

  return Status;
}

/**
  This routine could unlock all of the HDD password when S3/S4 resume

  @param[in]        This                EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.

  @retval           EFI_SUCCESS
**/
EFI_STATUS
UnlockAllHdd (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This
  )
{
  EFI_STATUS                            Status;
  EFI_STATUS                            FreePoolStatus;
  UINTN                                 Index;
  UINTN                                 NumOfHdd;
  UINT8                                 PasswordToHdd[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                                 PasswordToHddLength;
  UINTN                                 RetryCount;
  UINTN                                 HddPasswordTableSize;
  HDD_PASSWORD_PRIVATE                  *HddPasswordPrivate;
  HDD_PASSWORD_TABLE                    *HddPasswordTable;
  HDD_PASSWORD_HDD_INFO                 *HddInfoArray;
  UINTN                                 HddPasswordTableIndex;
  BOOLEAN                               HddPasswordFound;
  UINTN                                 NumOfTable;
  HDD_PASSWORD_TABLE                    *HddPasswordTablePtr;

  Status           = EFI_SUCCESS;
  HddPasswordTable = NULL;
  PasswordToHddLength = 0;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HddPasswordPrivate = GET_PRIVATE_FROM_HDD_PASSWORD (This);


  Status = CommonGetVariableDataAndSize (
             SAVE_HDD_PASSWORD_VARIABLE_NAME,
             &gSaveHddPasswordGuid,
             &HddPasswordTableSize,
             (VOID **) &HddPasswordTable
             );

  if (HddPasswordTable == NULL) {
    return EFI_NOT_FOUND;
  }

  SavedPasswordStringProcess (
        DecryptProcess,
        HddPasswordTable,
        HddPasswordTableSize
        );

  Status = GetHddInfoArray (
             This,
             &HddInfoArray,
             &NumOfHdd
             );

  if (NumOfHdd == 0) {
    return EFI_SUCCESS;
  }

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  if (!mSmst2) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDxeHddPasswordUnlockAllHdd \n"));
    Status = OemSvcDxeHddPasswordUnlockAllHdd (
               This,
               HddInfoArray,
               NumOfHdd,
               HddPasswordTable,
               HddPasswordTableSize
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDxeHddPasswordUnlockAllHdd Status: %r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcSmmHddPasswordUnlockAllHdd \n"));
    Status = OemSvcSmmHddPasswordUnlockAllHdd (
               This,
               HddInfoArray,
               NumOfHdd,
               HddPasswordTable,
               HddPasswordTableSize
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcSmmHddPasswordUnlockAllHdd Status: %r\n", Status));
  }

  if (Status == EFI_UNSUPPORTED) {
    for (Index = 0; Index < NumOfHdd; Index++) {
      if ((mSmst2 != NULL) && (HddInfoArray[Index].DeviceHandleInSmm == NULL)) {
        continue;
      }

      if ((HddInfoArray[Index].HddSecurityStatus & HDD_ENABLE_BIT) != HDD_ENABLE_BIT) {
        continue;
      }

      if ((HddInfoArray[Index].HddSecurityStatus & HDD_LOCKED_BIT) != HDD_LOCKED_BIT) {
        continue;
      }

      if ((HddInfoArray[Index].HddSecurityStatus & HDD_FROZEN_BIT) == HDD_FROZEN_BIT) {
        continue;
      }

      NumOfTable = NumOfHddPasswordTable (HddPasswordTable ,HddPasswordTableSize);
      HddPasswordFound = FALSE;
      ZeroMem (&PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
      HddPasswordTablePtr = (HDD_PASSWORD_TABLE *)HddPasswordTable;
      for (HddPasswordTableIndex = 0; HddPasswordTableIndex < NumOfTable;
           HddPasswordTableIndex++, HddPasswordTablePtr = GetNextTableEntry (HddPasswordTablePtr)) {
        if (HddPasswordTablePtr->ControllerNumber == HddInfoArray[Index].ControllerNumber &&
            HddPasswordTablePtr->PortNumber       == HddInfoArray[Index].PortNumber &&
            HddPasswordTablePtr->PortMulNumber    == HddInfoArray[Index].PortMulNumber) {
          HddPasswordFound = TRUE;
          break;
        }
      }

      if (HddPasswordFound != TRUE) {
        continue;
      }

      Status = HddPasswordStringProcess (
                 This,
                 HddPasswordTablePtr->PasswordType,
                 HddPasswordTablePtr->PasswordStr,
                 StrLen (HddPasswordTablePtr->PasswordStr),
                 (VOID **)&PasswordToHdd,
                 &PasswordToHddLength
                 );


      for (RetryCount = 0; RetryCount < DEFAULT_RETRY_COUNT; RetryCount++) {
        Status = This->UnlockHddPassword (
                         This,
                         &HddInfoArray[Index],
                         HddPasswordTablePtr->PasswordType,
                         PasswordToHdd,
                         PasswordToHddLength
                         );
        if (!EFI_ERROR (Status)) {
          break;
        }
      }

      if ((HddInfoArray[Index].HddSecurityStatus & HDD_EXPIRED_BIT) == HDD_EXPIRED_BIT && Status != EFI_SUCCESS) {
        //
        //  No error handling
        //

      }

    }

  }

  if (HddPasswordTable != NULL) {
    if (mSmst2 == NULL) {
      FreePoolStatus = gBS->FreePool (HddPasswordTable);
    } else {
      FreePoolStatus = mSmst2->SmmFreePool (HddPasswordTable);
    }
  }

  if (HddInfoArray != NULL) {
    if (mSmst2 == NULL) {
      FreePoolStatus = gBS->FreePool (HddInfoArray);
    } else {
      FreePoolStatus = mSmst2->SmmFreePool (HddInfoArray);
    }
  }

  //
  // zeroing the PWD content in memory after it's being used
  //
  ZeroMem(PasswordToHdd, PasswordToHddLength);

  return Status;
}

/**
  Check HDD SSP feature. (SSP: Software Settings Preservation)
  If the security of HDDs is ENABLE and the status of HDDs is FROZEN,
  using this function to make the status of HDDs to be LOCK.

  @param[in]        HddPasswordProtocol EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        HddInfoArray        The array of HDD information used in HDD Password.
  @param[in]        NumOfHdd            Number of HDD password information.

  @retval           EFI_SUCCESS         the security of HDDs is DISABLE.
  @retval           EFI_NOT_READY       the security of HDDs is ENABLE.
  @retval           others              there may be something wrong.

**/
EFI_STATUS
ResetSecurityStatus (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN HDD_PASSWORD_HDD_INFO              *HddInfo
  )
{
  EFI_STATUS                            Status;
  BOOLEAN                               UserOrMaster;
  UINT8                                 PasswordToHdd[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                                 PasswordToHddLength;
  UINTN                                 HddPasswordTableSize;
  HDD_PASSWORD_TABLE                    *HddPasswordTable;
  HDD_PASSWORD_HDD_INFO_PRIVATE         *HddInfoPrivate;
  UINTN                                 HddPasswordTableIndex;
  UINTN                                 NumOfTable;
  HDD_PASSWORD_TABLE                    *HddPasswordTablePtr;

  if (This == NULL || HddInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPrivateFromHddPasswordHddInfo (HddInfo, &HddInfoPrivate);

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  HddPasswordTableSize = 0;
  HddPasswordTable = NULL;

  Status = CommonGetVariableDataAndSize (
             SAVE_HDD_PASSWORD_VARIABLE_NAME,
             &gSaveHddPasswordGuid,
             &HddPasswordTableSize,
             (VOID **) &HddPasswordTable
             );

  SavedPasswordStringProcess (
        DecryptProcess,
        HddPasswordTable,
        HddPasswordTableSize
        );

  if (!mSmst2) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDxeHddResetSecurityStatus \n"));
    Status = OemSvcDxeHddResetSecurityStatus (
               This,
               HddInfo,
               HddPasswordTable,
               HddPasswordTableSize
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDxeHddResetSecurityStatus Status: %r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcSmmHddResetSecurityStatus \n"));
    Status = OemSvcSmmHddResetSecurityStatus (
               This,
               HddInfo,
               HddPasswordTable,
               HddPasswordTableSize
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcSmmHddResetSecurityStatus Status: %r\n", Status));
  }

  if (Status == EFI_UNSUPPORTED) {
    UserOrMaster = USER_PSW;
    ZeroMem (&PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
    PasswordToHddLength = HddInfo->MaxPasswordLengthSupport;

    if (HddPasswordTableSize != 0 && HddPasswordTable != NULL) {
      NumOfTable = NumOfHddPasswordTable (HddPasswordTable ,HddPasswordTableSize);
      HddPasswordTablePtr = (HDD_PASSWORD_TABLE *)HddPasswordTable;
      for (HddPasswordTableIndex = 0; HddPasswordTableIndex < NumOfTable;
           HddPasswordTableIndex++, HddPasswordTablePtr = GetNextTableEntry (HddPasswordTablePtr)) {
        if (HddPasswordTablePtr->ControllerNumber == HddInfo->ControllerNumber &&
            HddPasswordTablePtr->PortNumber       == HddInfo->PortNumber &&
            HddPasswordTablePtr->PortMulNumber    == HddInfo->PortMulNumber) {

          Status = HddPasswordStringProcess (
                     This,
                     HddPasswordTablePtr->PasswordType,
                     HddPasswordTablePtr->PasswordStr,
                     StrLen (HddPasswordTablePtr->PasswordStr),
                     (VOID **)&PasswordToHdd,
                     &PasswordToHddLength
                     );

          UserOrMaster = HddPasswordTablePtr->PasswordType;
          break;
        }
      }
    }

    //
    //  Partial storage types (Ex:eMMC) need original password to set security status from unlocked to locked
    //  and others (Ex: SATA HDD) don't need.
    //
    Status = SendHddSecurityCmd (
               This,
               H2O_HDD_PASSWORD_CMD_COMRESET,
               HddInfo,
               UserOrMaster,
               PasswordToHdd,
               PasswordToHddLength
               );

  } else if (Status == EFI_MEDIA_CHANGED) {
    //
    //  Oem feature code may occur error.
    //

    return EFI_ABORTED;
  }

  return Status;
}

/**
  Disabled locked HDD SATA port.

  @param[in]        HddPasswordProtocol EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        HddInfoArray        The array of HDD information used in HDD Password.
  @param[in]        NumOfHdd            Number of HDD password information.

  @retval           EFI_SUCCESS         The security of HDDs is DISABLE.
  @retval           EFI_NOT_READY       The security of HDDs is ENABLE.
  @retval           EFI_UNSUPPORTED     Unsupported.
  @retval           Others              there may be something wrong.

**/
EFI_STATUS
DisabledLockedHdd (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *This,
  IN HDD_PASSWORD_HDD_INFO              *HddInfo
  )
{
  EFI_STATUS                            Status;
  UINT32                                AhciMemAddr;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  EFI_DISK_INFO_PROTOCOL                *DiskInfo;
  HDD_PASSWORD_PRIVATE                  *HddPasswordPrivate;

  if (This == NULL || HddInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;

  HddPasswordPrivate = GET_PRIVATE_FROM_HDD_PASSWORD (This);

  if (HddPasswordPrivate->HddPasswordS3Data.SmmMode) {
    //
    //  Only DXE supported, SMM not supported.
    //

    return EFI_UNSUPPORTED;
  }

  AhciMemAddr = 0;

  if ((HddInfo->HddSecurityStatus & HDD_LOCKED_BIT) == HDD_LOCKED_BIT) {

    Status = gBS->HandleProtocol (
                    HddInfo->DeviceHandleInDxe,
                    &gEfiDiskInfoProtocolGuid,
                    (VOID **)&DiskInfo
                    );

    if (Status != EFI_SUCCESS) {
      return EFI_UNSUPPORTED;
    }

    //
    // filter not HDD
    //
    if (!(CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoIdeInterfaceGuid) ||
          CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid))) {
      return EFI_UNSUPPORTED;
    }

    Status = gBS->HandleProtocol (
                    HddInfo->DeviceHandleInDxe,
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (Status == EFI_SUCCESS) {
      Status = PciIo->Pci.Read (
                            PciIo,
                            EfiPciIoWidthUint32,
                            PCI_AHCI_BAR,
                            sizeof (UINT32),
                            &AhciMemAddr
                            );
      if (Status == EFI_SUCCESS) {
        Mmio32Or (AhciMemAddr, (HBA_PORTS_START + HBA_PORTS_SCTL + (HddInfo->PortNumber) * HBA_PORTS_REG_WIDTH), HBA_PORTS_SCTL_DET_OFFLINE);
      }

    }

  }

  return Status;
}

/**
  Find SATA device port number through devicepath in RAID/AHCI mode

  @param[in]  DevicePath         Devicepath on the device handle.
  @param[in, out] UINT16         pointer of Port number
  @param[in, out] UINT16         porint of Port multiplier number, optional parameter

  @retval TRUE     Find SATA/RAID device port number
  @retval FALSE    Unable to find SATA/RAID device port number

**/

SATA_DEVICE_PATH*
FindSataDevicePath (
  IN     EFI_DEVICE_PATH_PROTOCOL              *DevicePath
  )
{
  SATA_DEVICE_PATH                      *SataDevPath;

  while (!IsDevicePathEnd (DevicePath)) {
    if (DevicePath->Type == MESSAGING_DEVICE_PATH && DevicePath->SubType == MSG_SATA_DP) {
      SataDevPath = (SATA_DEVICE_PATH *) DevicePath;
      return SataDevPath;
    }
    DevicePath = NextDevicePathNode (DevicePath);
  }
  return NULL;
}

/**
  Find SATA device port number through devicepath in RAID/AHCI mode

  @param[in]  DevicePath         Devicepath on the device handle.

  @retval Port number     if cannot found, return 0xffff

**/

UINT16
FindSataDevicePortNumber (
  IN     EFI_DEVICE_PATH_PROTOCOL              *DevicePath
  )
{
  SATA_DEVICE_PATH                      *SataDevPath;
  UINT16                                PortMap;
  UINT16                                Port;

  SataDevPath = FindSataDevicePath(DevicePath);
  if (SataDevPath == NULL) {
    return INVALID_SATA_PORT_NUMBER;
  }

  if (SataDevPath->PortMultiplierPortNumber == 0x8000) {
    PortMap = SataDevPath->HBAPortNumber;
    for (Port = 0; PortMap != 0; Port++, PortMap >>= 1);
    Port--;
    return Port;
  } else {
     return SataDevPath->HBAPortNumber;
  }

}


/**
  Find SATA device port multiplier number through devicepath in RAID/AHCI mode

  @param[in]  DevicePath         Devicepath on the device handle.

  @retval Port number     if cannot found, return 0xffff

**/

UINT16
FindSataDevicePortMultiplierNumber (
  IN     EFI_DEVICE_PATH_PROTOCOL              *DevicePath
  )
{
  SATA_DEVICE_PATH                      *SataDevPath;

  SataDevPath = FindSataDevicePath(DevicePath);
  if (SataDevPath == NULL) {
    return INVALID_SATA_PORT_NUMBER;
  }
  return SataDevPath->PortMultiplierPortNumber;
}


/**
  Check if SSCP is already being collecteed by Hdd Password driver

  @param Sscp                  StorageSecurityCommandProtocol instance on the device handle.
  @param SrcDevicePath         Devicepath on the device handle.

  @retval EFI_ALREADY_STARTED          It is already collected by Hdd Password driver
  @retval EFI_SUCCESS                  It is not collecteded by Hdd Password driver
**/

EFI_STATUS
CheckDuplicatedRAIDDevices (
  IN     EFI_STORAGE_SECURITY_COMMAND_PROTOCOL *SrcSscp,
  IN     EFI_DEVICE_PATH_PROTOCOL              *SrcDevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL              *TmpDevicePath;
  UINT16                                SrcPort;
  UINT16                                DesPort;
  HDD_INFO_MANAGER                      *HddInfoManager;
  LIST_ENTRY                            *HddInfoPrivateListHead;
  LIST_ENTRY                            *Link;
  EFI_STATUS                            Status;
  EFI_HANDLE                            SrcControllerHandle;
  EFI_HANDLE                            DesControllerHandle;
  HDD_PASSWORD_HDD_INFO_PRIVATE         *HddInfoPrivate;

  HddInfoManager = mHddPasswordPrivate->HddInfoManager;
  SrcControllerHandle = NULL;
  DesControllerHandle = NULL;

  //
  // find the port number from inpu device path
  //
  TmpDevicePath = SrcDevicePath;
  if (FindSataDevicePath(TmpDevicePath) == NULL) {
    return EFI_NOT_FOUND;
  }
  SrcPort = FindSataDevicePortNumber(TmpDevicePath);
  //
  // compare with pre-existing HddInfoPrivate nodes
  //
  HddInfoPrivateListHead = &HddInfoManager->HddInfoPrivateListHead;
  for (Link = GetFirstNode (HddInfoPrivateListHead);
       !IsNull (HddInfoPrivateListHead, Link);
       Link = GetNextNode (HddInfoPrivateListHead, Link)) {
    HddInfoPrivate = GET_HDD_INFO_PRIVATE_FROM_LINK (Link);
    //
    // If found the Sscp already in private data, return already collected
    //
    if (HddInfoPrivate->StorageSecurityCommandInDxe == SrcSscp) {
      return EFI_ALREADY_STARTED;
    }

    //
    // find the controller handle of the device in HddInfoPrivate
    //
    TmpDevicePath = HddInfoPrivate->DevicePath;
    Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &TmpDevicePath, &DesControllerHandle);
    if (EFI_ERROR(Status)) {
      continue;
    }

    //
    // find the controller handle from the input device path
    //
    TmpDevicePath = SrcDevicePath;
    Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &TmpDevicePath, &SrcControllerHandle);
    if (EFI_ERROR(Status)) {
      //
      // if there's no Pciio, currently will ignore checking
      //
      return EFI_SUCCESS;
    }

    DesPort = HddInfoPrivate->HddInfo.PortNumber;
    //
    // check if there's pre-exist a HddPassword device node that has the same port as another one
    //
    if ((SrcControllerHandle == DesControllerHandle) && (SrcPort == DesPort)) {
      return EFI_ALREADY_STARTED;
    }
  }

  //
  // if not found any HddPassword node return false to continue collecting
  //
  return EFI_SUCCESS;
}


/**
  Check if device is a INTEL re-mapped NVMe by device path.

  @param[in] DevicePath          Device path info for the device.

  @retval TRUE                   It is a Intel remapped device
  @retval FALSE                  It is not a Intel remapped device
**/
BOOLEAN
IsRemappedNvme (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL              *TmpDevicePath;
  EFI_HANDLE                            DiskInfoHandle;
  EFI_HANDLE                            AtaPassthruHandle;
  EFI_STATUS                            Status;
  EFI_DISK_INFO_PROTOCOL                *DiskInfo;

  DiskInfoHandle = NULL;
  AtaPassthruHandle = NULL;
  TmpDevicePath = DevicePath;
  Status = gBS->LocateDevicePath(&gEfiAtaPassThruProtocolGuid, &TmpDevicePath, &AtaPassthruHandle);
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  TmpDevicePath = DevicePath;
  Status = gBS->LocateDevicePath(&gEfiDiskInfoProtocolGuid, &TmpDevicePath, &DiskInfoHandle);
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  Status = gBS->HandleProtocol (
                  DiskInfoHandle,
                  &gEfiDiskInfoProtocolGuid,
                  (VOID **)&DiskInfo
                  );

  if (CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoNvmeInterfaceGuid)) {
    return TRUE;
  }

 return FALSE;
}

/**
 Get device name from Component Name (2) protocol.

 @param[in] ControllerHandle             The handle of a controller
 @param[in] SscpDeviceHandle             The handle of a SSCP device
 @param[in] ComponentNameProtocolGuid    A pointer to an EFI_GUID. It points to Component Name (2) protocol GUID
 @param[in] ComponentNameHandleCount     The number of Component Name (2) protocol handles
 @param[in] ComponentNameHandleList      A pointer to an array of Component Name (2) protocol handles

 @return A pointer to device name or NULL if not found.
**/
CHAR16 *
GetDevNameFromComponentNameProtocol (
  IN EFI_HANDLE                          ControllerHandle,
  IN EFI_HANDLE                          SscpDeviceHandle,
  IN EFI_GUID                            *ComponentNameProtocolGuid,
  IN UINTN                               ComponentNameHandleCount,
  IN EFI_HANDLE                          *ComponentNameHandleList
  )
{
  UINTN                                  Index;
  EFI_COMPONENT_NAME_PROTOCOL            *ComponentName;
  CHAR16                                 *DeviceName;
  EFI_STATUS                             Status;

  for (Index = 0; Index < ComponentNameHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    ComponentNameHandleList[Index],
                    ComponentNameProtocolGuid,
                    (VOID **) &ComponentName
                    );
    if (EFI_ERROR (Status) || !IsValidComponentNameProtocol (ComponentName, SscpDeviceHandle)) {
      continue;
    }

    Status = ComponentName->GetControllerName (
                               ComponentName,
                               ControllerHandle,
                               SscpDeviceHandle,
                               ComponentName->SupportedLanguages,
                               &DeviceName
                               );
    if (EFI_ERROR (Status)) {
      //
      // For some Single-LUN storages, BlockIo protocol may be installed on the ControllerHandle instead of a new handle created.
      // Trying the BlockIo handle as the ControllerHandle for retrieving the device name.
      //
      Status = ComponentName->GetControllerName (
                                 ComponentName,
                                 SscpDeviceHandle,
                                 SscpDeviceHandle,
                                 ComponentName->SupportedLanguages,
                                 &DeviceName
                                 );
    }
    if (EFI_ERROR (Status)) {
      continue;
    }

    return DeviceName;
  }

  return NULL;
}

/**
  get device model name from component name protocol

  @param[in] EFI_HANDLE          controller handle of the device.
  @param[in] EFI_HANDLE          device handle
  @param[out] CHAR16*            the model string of the device

  @retval EFI_SUCCESS            Successfully found the model string from component name protocol
  @retval EFI_NOT_FOUND          Can't found model string.
**/

EFI_STATUS
GetModelNameFromComponentName (
IN  EFI_HANDLE  ControllerHandle,
IN  EFI_HANDLE  SscpDeviceHandle,
OUT CHAR16      *HddModelString
  )
{
  CHAR16                                *ComponentNamePtr;
  UINTN                                 ComponentNameHandleCount;
  EFI_HANDLE                            *ComponentNameHandleBuffer;
  UINTN                                 StringIndex;
  EFI_STATUS                            Status;

  ComponentNamePtr = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiComponentName2ProtocolGuid,
                  NULL,
                  &ComponentNameHandleCount,
                  &ComponentNameHandleBuffer
                  );
  if (!EFI_ERROR (Status)) {
    ComponentNamePtr = GetDevNameFromComponentNameProtocol (ControllerHandle, SscpDeviceHandle, &gEfiComponentName2ProtocolGuid, ComponentNameHandleCount, ComponentNameHandleBuffer);
    FreePool (ComponentNameHandleBuffer);
  }
  if (ComponentNamePtr == NULL) {
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiComponentNameProtocolGuid,
                    NULL,
                    &ComponentNameHandleCount,
                    &ComponentNameHandleBuffer
                    );
    if (!EFI_ERROR (Status)) {
      ComponentNamePtr = GetDevNameFromComponentNameProtocol (ControllerHandle, SscpDeviceHandle, &gEfiComponentNameProtocolGuid, ComponentNameHandleCount, ComponentNameHandleBuffer);
      FreePool (ComponentNameHandleBuffer);
    }
  }
  if (ComponentNamePtr == NULL) {
    return EFI_NOT_FOUND;
  }

  CopyMem (
    HddModelString,
    ComponentNamePtr,
    MIN ((StrLen (ComponentNamePtr) * sizeof (CHAR16)), (MODEL_NUMBER_LENGTH * sizeof (CHAR16))));

  //
  // Add space to tail
  //
  StringIndex = MODEL_NUMBER_LENGTH - 1;
  while (HddModelString[StringIndex] == 0x0000) {
    HddModelString[StringIndex] = 0x20;
    StringIndex--;
  }
  return EFI_SUCCESS;
}


/**
  To Collect the storage device information.

  @param

  @retval EFI_SUCCESS
**/
STATIC
EFI_STATUS
CollectDeviceInfo (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_STATUS                            TcgStatus;
  EFI_STATUS                            IdentifyStatus;
  EFI_HANDLE                            ControllerHandle;
  EFI_HANDLE                            *StorageSecurityCommandHandleBuffer;
  UINTN                                 StorageSecurityCommandHandleCount;
  UINTN                                 StorageSecurityCommandHandleIndex;
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL *StorageSecurityCommand;
  EFI_BLOCK_IO_PROTOCOL                 *BlockIo;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *TmpDevicePath;
  UINTN                                 Seg;
  UINTN                                 Bus;
  UINTN                                 Device;
  UINTN                                 Function;
  UINT64                                *CmdTimeoutTable;
  UINT64                                Timeout;
  UINTN                                 PayloadBufferSize;
  UINTN                                 PayloadTransferSize;

  UINT8                                 NumOfHdd;
  UINT8                                 MaxPasswordLengthSupport;
  EFI_DISK_INFO_PROTOCOL                *DiskInfo;
  UINT32                                IdeChannel;
  UINT32                                IdeDevice;
  UINT8                                 SataClassCReg[3];
  HDD_PASSWORD_HDD_INFO_PRIVATE         *HddInfoPrivate;
  HDD_INFO_MANAGER                      *HddInfoManager;
  UINT32                                MediaId;
  OPAL_DISK_INFO                        *OpalDisk;
  PCI_TYPE00                            Pci;
  UINT16                                Index;
  BOOLEAN                               InstalledByInsydeDriver;
  UINT8                                 Mn[41];
  UINT8                                 Sn[21];
  UINT8                                 *RawModelName;
  UINT8                                 *RawSerialName;


  StorageSecurityCommandHandleBuffer = NULL;
  StorageSecurityCommandHandleCount  = 0;
  DiskInfo = NULL;
  BlockIo  = NULL;
  InstalledByInsydeDriver = FALSE;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiStorageSecurityCommandProtocolGuid,
                  NULL,
                  &StorageSecurityCommandHandleCount,
                  &StorageSecurityCommandHandleBuffer
                  );
  if (Status != EFI_SUCCESS || StorageSecurityCommandHandleBuffer == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (StorageSecurityCommandHandleCount == 0) {
    return EFI_SUCCESS;
  }

  HddInfoManager = mHddPasswordPrivate->HddInfoManager;
  NumOfHdd = 0;

  OpalDisk = NULL;
  OpalDisk = AllocateZeroPool (sizeof (OPAL_DISK_INFO));
  if (OpalDisk == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  //  Collect storage device Info
  //
  for (StorageSecurityCommandHandleIndex = 0; StorageSecurityCommandHandleIndex < StorageSecurityCommandHandleCount; StorageSecurityCommandHandleIndex++) {
    //
    //  Locate Storage Security Command Protocol
    //
    Status = gBS->HandleProtocol (
                    StorageSecurityCommandHandleBuffer[StorageSecurityCommandHandleIndex],
                    &gEfiStorageSecurityCommandProtocolGuid,
                    (VOID **)&StorageSecurityCommand
                    );
    if (Status != EFI_SUCCESS) {
      continue;
    }

    Status = gBS->HandleProtocol (
              StorageSecurityCommandHandleBuffer[StorageSecurityCommandHandleIndex],
              &gEfiDiskInfoProtocolGuid,
              (VOID **)&DiskInfo
              );

    //
    //  Locate Block IO Protocol to get Media ID
    //
    MediaId = 0;
    Status = gBS->HandleProtocol (
                    StorageSecurityCommandHandleBuffer[StorageSecurityCommandHandleIndex],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **)&BlockIo
                    );
    if (Status != EFI_SUCCESS) {
      //
      // In RAID mode, BlockIo is installed by RAID OPROM.
      // However, Intel RST driver won't produce BlockIo if it found the device being locked.
      // In this case, we will set media ID to 0.
      //
      MediaId = 0;
    } else {
      MediaId = BlockIo->Media->MediaId;
    }

    if (IsEdrive(StorageSecurityCommand, (UINT16)MediaId)) {
      continue;
    }

    if (IsEncryptedBy3rdPartySoftware(StorageSecurityCommand, (UINT16)MediaId)) {
      continue;
    }

    TcgStatus = EFI_UNSUPPORTED;
    ZeroMem ((VOID *)OpalDisk, sizeof (OPAL_DISK_INFO));
    if (PcdGetBool (PcdH2OHddPasswordTcgOpalSupported)) {
      //
      //  Check TCG supported
      //
      TcgStatus = OpalDiskInitialize (StorageSecurityCommand, (UINT16)MediaId, OpalDisk);
    }


    //
    //  Locate PCI IO Protocol to get Bus, Device, Function
    //
    Status = gBS->HandleProtocol (
                    StorageSecurityCommandHandleBuffer[StorageSecurityCommandHandleIndex],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &DevicePath
                    );
    if (Status != EFI_SUCCESS) {
      continue;
    }


    ControllerHandle = NULL;
    TmpDevicePath = DevicePath;
    Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &TmpDevicePath, &ControllerHandle);
    if (Status != EFI_SUCCESS || ControllerHandle == NULL) {
      continue;
    }

    InstalledByInsydeDriver = CheckSscpSource(ControllerHandle, StorageSecurityCommandHandleBuffer[StorageSecurityCommandHandleIndex]);

    //
    //  Get Identify information of device
    //
    CmdTimeoutTable = (UINT64*)PcdGetPtr (PcdH2OHddPasswordCmdTimeoutTable);
    Timeout = CmdTimeoutTable[TIMEOUT_IDENTIFY_DEVICE_INDEX];

    ZeroMem (&mIdentifyData, sizeof (ATA_IDENTIFY_DATA));

    PayloadBufferSize   = sizeof (ATA_IDENTIFY_DATA);
    PayloadTransferSize = 0;

    //
    // Get security status for Insyde supported HddPassword feature
    //
    IdentifyStatus = EFI_SUCCESS;

    ZeroMem (&mSecurityCheckStateData, sizeof(ATA_DEVICE_SERVER_SECURITY_STATE_DATA));
    IdentifyStatus = StorageSecurityCommand->ReceiveData (
                                               StorageSecurityCommand,
                                               MediaId,
                                               InstalledByInsydeDriver ? Timeout : TRUSTED_COMMAND_TIMEOUT_NS,
                                               InstalledByInsydeDriver ? H2O_SECURITY_PROTOCOL_ID : TCG_SECURITY_PROTOCOL_ATA_DEVICE_SERVER_PASS,
                                               InstalledByInsydeDriver ? RCV_IDENTIFY_DATA : SwapBytes16(TCG_SP_SPECIFIC_PROTOCOL_LIST),
                                               InstalledByInsydeDriver ? PayloadBufferSize : sizeof(ATA_DEVICE_SERVER_SECURITY_STATE_DATA),
                                               InstalledByInsydeDriver ? (VOID*)&mIdentifyData : (VOID*)&mSecurityCheckStateData,
                                               &PayloadTransferSize
                                               );
    if (!InstalledByInsydeDriver && !EFI_ERROR(IdentifyStatus)) {
      mIdentifyData.security_status |= mSecurityCheckStateData.Byte9;
      mIdentifyData.master_password_identifier |= mSecurityCheckStateData.MasterPasswordIdentifier;
    }

    //
    // In Raid mode, there's a possibility that other driver(such as INTEL Rst) will produce SSCP, if that's the case
    // we will only keep the one from other driver
    //
    Status = gBS->HandleProtocol (
                    ControllerHandle,
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }

    Status = PciIo->Pci.Read (
                      PciIo,
                      EfiPciIoWidthUint32,
                      0,
                      sizeof (Pci) / sizeof (UINT32),
                      &Pci
                      );

    if (IS_PCI_RAID(&Pci) && !EFI_ERROR(Status)) {
      Status = CheckDuplicatedRAIDDevices(StorageSecurityCommand, DevicePath);
      if (Status == EFI_ALREADY_STARTED) {
        continue;
      }
    }

    //
    //  Filter the unsupported device
    //
    if ((IdentifyStatus != EFI_SUCCESS && TcgStatus != EFI_SUCCESS) ||
        ((mIdentifyData.security_status & HDD_SUPPORT_BIT) != HDD_SUPPORT_BIT &&
         (TcgStatus != EFI_SUCCESS))) {
      continue;
    }

    Seg      = 0;
    Bus      = 0;
    Device   = 0;
    Function = 0;
    Status = PciIo->GetLocation (
                      PciIo,
                      &Seg,
                      &Bus,
                      &Device,
                      &Function
                      );
    if (Status != EFI_SUCCESS) {
      continue;
    }

    //
    // Get max password length supported.
    //
    PayloadBufferSize   = sizeof (UINT8);
    PayloadTransferSize = 0;
    MaxPasswordLengthSupport = 0;
    if (InstalledByInsydeDriver) {
      Status = StorageSecurityCommand->ReceiveData (
                                         StorageSecurityCommand,
                                         MediaId,
                                         0,
                                         H2O_SECURITY_PROTOCOL_ID,
                                         RCV_MAX_PWD_LEN,
                                         PayloadBufferSize,
                                         &MaxPasswordLengthSupport,
                                         &PayloadTransferSize
                                         );
    } else {
       MaxPasswordLengthSupport = MAXPASSWORD_LENGTH_SUPPORTED_FOR_TCG_SECURITY;
    }


    HddInfoPrivate = NULL;
    HddInfoPrivate = AllocateBuffer (sizeof (HDD_PASSWORD_HDD_INFO_PRIVATE));
    if (HddInfoPrivate == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    //
    //  Build device information
    //
    if (IdentifyStatus == EFI_SUCCESS) {
      HddInfoPrivate->HddInfo.HddSecurityStatus        = mIdentifyData.security_status;
      HddInfoPrivate->HddInfo.MasterPasswordIdentifier = mIdentifyData.master_password_identifier;
      if((AsciiStrnLenS(mIdentifyData.ModelName, sizeof(CHAR8) * 40) != 0) &&
        (AsciiStrnLenS(mIdentifyData.SerialNo, sizeof(CHAR8) * 20) != 0)) {

        //
        // Remove Space at the end of Serial number
        //
        for (Index = 20; Index > 0; Index--) {
          if (mIdentifyData.SerialNo[Index] == 0x20 || mIdentifyData.SerialNo[Index] == 0) {
            mIdentifyData.SerialNo[Index] = 0;
          } else {
             break;
          }
        }

        //
        // revert the string because the raw model name is in reverse order
        //
        RawModelName = (UINT8*) mIdentifyData.ModelName;
        for (Index = 0; Index < 40; Index += 2) {
          Mn[Index + 0] = RawModelName[Index + 1];
          Mn[Index + 1] = RawModelName[Index + 0];
        }
        Mn[40] = 0;

        //
        // revert the string because the raw Serial number is in reverse order
        //
        RawSerialName = (UINT8*) mIdentifyData.SerialNo;
        for (Index = 0; Index < 20; Index += 2) {
          Sn[Index + 0] = RawSerialName[Index + 1];
          Sn[Index + 1] = RawSerialName[Index + 0];
        }
        Sn[20] = 0;

        UnicodeSPrintAsciiFormat (HddInfoPrivate->HddInfo.HddModelString, sizeof (CHAR16) * DEVICE_MODEL_NAME_STRING_LENGTH, "%a-%a", Sn, Mn);
      } else {
         Status = GetModelNameFromComponentName(
                    ControllerHandle,
                    StorageSecurityCommandHandleBuffer[StorageSecurityCommandHandleIndex],
                    HddInfoPrivate->HddInfo.HddModelString
                    );
         if (Status == EFI_ABORTED) {
           continue;
         }
      }

    }

    HddInfoPrivate->HddInfo.ControllerNumber         = FindMappedController (Seg, Bus, Device, Function);
    HddInfoPrivate->HddInfo.PciSeg                   = Seg;
    HddInfoPrivate->HddInfo.PciBus                   = Bus;
    HddInfoPrivate->HddInfo.PciDevice                = Device;
    HddInfoPrivate->HddInfo.PciFunction              = Function;
    HddInfoPrivate->HddInfo.DeviceHandleInDxe        = StorageSecurityCommandHandleBuffer[StorageSecurityCommandHandleIndex];
    HddInfoPrivate->HddInfo.MaxPasswordLengthSupport = MaxPasswordLengthSupport;
    HddInfoPrivate->HddInfo.HddInfoExtPtr            = NULL;
    HddInfoPrivate->InstalledByInsyde                = InstalledByInsydeDriver;

    if (DiskInfo != NULL) {
      Status = PciIo->Pci.Read (
                            PciIo,
                            EfiPciIoWidthUint8,
                            PCI_CLASSCODE_OFFSET,
                            3,
                            SataClassCReg
                            );
      if (Status == EFI_SUCCESS) {
        switch (SataClassCReg[1]) {

        case PCI_SUB_CLASS_IDE:
          HddInfoPrivate->HddInfo.ControllerMode = ATA_IDE_MODE;
          break;
        case PCI_SUB_CLASS_AHCI:
          HddInfoPrivate->HddInfo.ControllerMode = ATA_AHCI_MODE;
          break;
        case PCI_SUB_CLASS_RAID:
          HddInfoPrivate->HddInfo.ControllerMode = ATA_RAID_MODE;
          break;
        case PCI_SUB_CLASS_NVME:
          HddInfoPrivate->HddInfo.ControllerMode = NVMe_MODE;
          break;
        default:
          HddInfoPrivate->HddInfo.ControllerMode = eMMC_MODE;
          break;
        }

        Status = DiskInfo->WhichIde (
                             DiskInfo,
                             &IdeChannel,
                             &IdeDevice
                             );
        if (Status == EFI_SUCCESS) {
          HddInfoPrivate->HddInfo.PortNumber    = (UINT16)IdeChannel;
          HddInfoPrivate->HddInfo.PortMulNumber = (UINT16)IdeDevice;

          if (HddInfoPrivate->HddInfo.ControllerMode == ATA_IDE_MODE) {
            Status = ChangeChannelDevice2PortNum (
                       (UINT32)HddInfoPrivate->HddInfo.PciBus,
                       (UINT32)HddInfoPrivate->HddInfo.PciDevice,
                       (UINT32)HddInfoPrivate->HddInfo.PciFunction,
                       (UINT8)HddInfoPrivate->HddInfo.PortNumber,
                       (UINT8)HddInfoPrivate->HddInfo.PortMulNumber,
                       (UINTN *)&HddInfoPrivate->HddInfo.MappedPort
                       );
          } else {
            HddInfoPrivate->HddInfo.MappedPort = 0xFF;
          }
        }
      }
    }

    if (Status != EFI_SUCCESS || DiskInfo == NULL) {
      Status = PciIo->Pci.Read (
                            PciIo,
                            EfiPciIoWidthUint8,
                            PCI_CLASSCODE_OFFSET,
                            3,
                            SataClassCReg
                            );
      if (Status == EFI_SUCCESS) {
        switch (SataClassCReg[1]) {

        case PCI_SUB_CLASS_IDE:
          HddInfoPrivate->HddInfo.ControllerMode = ATA_IDE_MODE;
          break;
        case PCI_SUB_CLASS_AHCI:
          HddInfoPrivate->HddInfo.ControllerMode = ATA_AHCI_MODE;
          break;
        case PCI_SUB_CLASS_RAID:
          HddInfoPrivate->HddInfo.ControllerMode = ATA_RAID_MODE;
          break;
        case PCI_SUB_CLASS_NVME:
          HddInfoPrivate->HddInfo.ControllerMode = NVMe_MODE;
          break;
        default:
          HddInfoPrivate->HddInfo.ControllerMode = eMMC_MODE;
          break;
        }
      }
      //
      // In some SATA device cases, such as RAID, can't find diskinfo for port number, but can found port number in device path
      //
      TmpDevicePath = DevicePath;
      if (FindSataDevicePath(TmpDevicePath) != NULL) {
        HddInfoPrivate->HddInfo.PortNumber     = FindSataDevicePortNumber(TmpDevicePath);             ;
        HddInfoPrivate->HddInfo.PortMulNumber  = FindSataDevicePortMultiplierNumber(TmpDevicePath);
        HddInfoPrivate->HddInfo.MappedPort     = 0xFF;
      } else {
        //
        // if can't find SATA devices, it means it would either be NVME or EMMC or SD, thus no port number needed
        //
        HddInfoPrivate->HddInfo.PortNumber     = 0xFF;
        HddInfoPrivate->HddInfo.PortMulNumber  = 0xFF;
        HddInfoPrivate->HddInfo.MappedPort     = 0xFF;
      }

    }


    HddInfoPrivate->Signature = HDD_PASSWORD_HDD_INFO_PRIVATE_SIGNATURE;
    HddInfoPrivate->BlockIoMediaId = MediaId;
    HddInfoPrivate->DeviceHandleInDxe = StorageSecurityCommandHandleBuffer[StorageSecurityCommandHandleIndex];
    HddInfoPrivate->StorageSecurityCommandInDxe = StorageSecurityCommand;

    Status = gBS->HandleProtocol (
                    StorageSecurityCommandHandleBuffer[StorageSecurityCommandHandleIndex],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &DevicePath
                    );
    if (!EFI_ERROR (Status)) {
      HddInfoPrivate->DevicePath = HddPasswordDuplicateDevicePath (DevicePath);
    }

    if (TcgStatus == EFI_SUCCESS) {
      CopyMem ((VOID *)&HddInfoPrivate->OpalDisk, (VOID *)OpalDisk, sizeof (HddInfoPrivate->OpalDisk));
      HddInfoPrivate->HddInfo.StorageTcgSecuritySupported = TRUE;
      HddInfoPrivate->HddInfo.HddSecurityStatus = ((HddInfoPrivate->OpalDisk.LockingFeature.LockingSupported << 0) |
                                                   (HddInfoPrivate->OpalDisk.LockingFeature.LockingEnabled   << 1) |
                                                   (HddInfoPrivate->OpalDisk.LockingFeature.Locked           << 2));
      if (IdentifyStatus != EFI_SUCCESS) {
        //
        //  Get device model name by Component name protocol
        //
        Status = GetModelNameFromComponentName(
                   ControllerHandle,
                   StorageSecurityCommandHandleBuffer[StorageSecurityCommandHandleIndex],
                   HddInfoPrivate->HddInfo.HddModelString
                   );
        if (Status == EFI_ABORTED) {
          continue;
        }
      }

    }


    InsertTailList (
      &HddInfoManager->HddInfoPrivateListHead,
      &HddInfoPrivate->Link
      );

    //
    //  Device count
    //
    NumOfHdd ++;
  }

  if (StorageSecurityCommandHandleBuffer != NULL) {
    FreePool (StorageSecurityCommandHandleBuffer);
  }

  if (OpalDisk != NULL) {
    FreePool (OpalDisk);
  }

  HddInfoManager->NumOfHdd = NumOfHdd;

  return EFI_SUCCESS;
}

/**
  To collect storage information.

  @param[in]        Event               Pointer to this event
  @param[in]        Context             Event handler private data

**/
VOID
HddPasswordDriverInstall (
  IN BOOLEAN                            ConnectController
  )
{
  EFI_STATUS                            Status;
  UINTN                                 HddIndex;
  HDD_INFO_MANAGER                      *HddInfoManager;
  HDD_PASSWORD_HDD_INFO_PRIVATE         *HddInfoPrivate;
  LIST_ENTRY                            *HddInfoPrivateListHead;
  LIST_ENTRY                            *Link;
  LIST_ENTRY                            *DelList;

  EFI_SMM_COMMUNICATION_PROTOCOL        *SmmCommunication;
  EFI_SMM_COMMUNICATE_HEADER            *SmmCommHeader;
  HDD_PASSWORD_SMM_PARAMETER_HEADER     *HddPasswordSmmHeader;
  UINT8                                 *HddPasswordSmmData;
  UINTN                                 CommBufferSize;
  ATA_CONTROLLER_INFO                   *AtaControllerInfo;
  DRIVER_INSTALL_INFO                   *DriverInstallInfo;
  LIST_ENTRY                            *AtaControllerInfoListHead;
  UINT16                                ControllerIndex;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordProtocolDxe;
  HDD_PASSWORD_PRIVATE                  *HddPasswordPrivateDxe;

  HddInfoManager = mHddPasswordPrivate->HddInfoManager;
  HddInfoPrivateListHead = &HddInfoManager->HddInfoPrivateListHead;
  for (Link = GetFirstNode (HddInfoPrivateListHead), HddIndex = 0;
       !IsNull (HddInfoPrivateListHead, Link) && HddIndex < HddInfoManager->NumOfHdd;
       HddIndex++) {
    DelList = Link;
    Link = GetNextNode (HddInfoPrivateListHead, Link);
    HddInfoPrivate = GET_HDD_INFO_PRIVATE_FROM_LINK (DelList);
    RemoveEntryList (DelList);
    FreeBuffer (HddInfoPrivate);
  }

  //
  // Because DriverInstallInfo is used in SAVE_HDD_PASSWORD_VARIABLE_NAME variable which in current flow will only be updated
  // during unlock or lock process, thus does not reallocate after
  // controllers being reconnected
  //
  DriverInstallInfo = mDriverInstallInfo;
  AtaControllerInfoListHead = &DriverInstallInfo->AtaControllerInfoListHead;
  for (Link = GetFirstNode (AtaControllerInfoListHead), ControllerIndex = CONTROLLER_INDEX_START;
       !IsNull (AtaControllerInfoListHead, Link) && ControllerIndex <= DriverInstallInfo->NumOfController &&
       !mHddPasswordPrivate->ControllerReconnected;
        ControllerIndex++) {
    DelList = Link;
    Link = GetNextNode (AtaControllerInfoListHead, Link);
    AtaControllerInfo = (ATA_CONTROLLER_INFO *)DelList;
    RemoveEntryList (DelList);
    FreeBuffer (AtaControllerInfo);
  }

  CollectStorageController (ConnectController);

  CollectDeviceInfo ();

  //
  // Trigger SMI for SMM instance
  //
  Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &SmmCommunication);
  if (Status == EFI_SUCCESS) {
    //
    // Get PrivateData
    //
    Status = gBS->LocateProtocol (
                    &gEfiHddPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **)&HddPasswordProtocolDxe
                    );
    if (EFI_ERROR (Status)) {
      return;
    }
    HddPasswordPrivateDxe = GET_PRIVATE_FROM_HDD_PASSWORD (HddPasswordProtocolDxe);

    //
    // HDD_PASSWORD_SMM_COLLECT_DEVICE_INSTANCE
    //

    SmmCommHeader = InitCommunicateBuffer ((VOID **)&HddPasswordSmmData, sizeof (HDD_PASSWORD_PRIVATE), HDD_PASSWORD_SMM_COLLECT_DEVICE_INSTANCE);
    if (SmmCommHeader == NULL) {
      return;
    }
    CopyMem ((VOID *)HddPasswordSmmData, (VOID *)HddPasswordPrivateDxe, sizeof (HDD_PASSWORD_PRIVATE));
    CommBufferSize = SmmCommHeader->MessageLength + SMM_COMMUNICATE_HEADER_SIZE;
    Status = SmmCommunication->Communicate (SmmCommunication, SmmCommHeader, &CommBufferSize);
    HddPasswordSmmHeader = (HDD_PASSWORD_SMM_PARAMETER_HEADER *)SmmCommHeader->Data;
    Status = (EFI_STATUS)HddPasswordSmmHeader->ReturnStatus;
    DEBUG ((EFI_D_INFO, "SMM Communicate: Collect Device - (%r)\n", Status));
  }
}

/**
  To install HDD password services.

  @param[in]        Event               Pointer to this event
  @param[in]        Handle              The handle associated with a previously registered checkpoint handler.

**/
VOID
H2OBdsCpConnectAllAfterCallback (
  IN EFI_EVENT                Event,
  IN H2O_CP_HANDLE            Handle
  )
{
   HddPasswordDriverInstall (FALSE);

}


/**
  S3 resume to check HDDs password.

  @param[in]        Protocol            Points to the protocol's unique identifier
  @param[in]        Interface           Points to the interface instance
  @param[in]        Handle              The handle on which the interface was installed

  @retval EFI_STATUS

**/
EFI_STATUS
S3CheckHddPassword (
  IN CONST EFI_GUID                     *Protocol,
  IN VOID                               *Interface,
  IN EFI_HANDLE                         Handle
  )
{
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordServiceSmm;
  EFI_STATUS                            Status;
  HDD_PASSWORD_HDD_INFO                 *HddInfoArray;
  UINTN                                 NumOfHdd;
  UINTN                                 Index;
  BOOLEAN                               HddLock;
  HDD_PASSWORD_S3_DATA                  *HddPasswordS3Data;

  if (mHddPasswordPrivate == NULL) {
    return EFI_UNSUPPORTED;
  }

  HddInfoArray = NULL;
  HddLock      = FALSE;

  HddPasswordServiceSmm = &(mHddPasswordPrivate->HddPasswordProtocol);

  HddPasswordS3Data      = &(mHddPasswordPrivate->HddPasswordS3Data);

  //
  // Get Hddinfo table
  //
  Status = GetHddInfoArray (
             HddPasswordServiceSmm,
             &HddInfoArray,
             &NumOfHdd
             );

  if (NumOfHdd == 0 || EFI_ERROR(Status)) {
    //
    // Not found any Harddisk
    //
    return EFI_SUCCESS;
  }

  for (Index = 0; Index < NumOfHdd; Index++) {
    if ((HddInfoArray[Index].DeviceHandleInSmm == NULL)) {
      continue;
    }

    if ((HddInfoArray[Index].HddSecurityStatus & HDD_ENABLE_BIT) != HDD_ENABLE_BIT) {
      continue;
    }


    Status = ResetSecurityStatus (
               HddPasswordServiceSmm,
               &(HddInfoArray[Index])
               );
  }

  if (HddInfoArray != NULL) {
    Status = mSmst2->SmmFreePool (HddInfoArray);
  }

  if (mEnableBlockSid) {
    IssueBlockSid ();
  }

  Status = UnlockAllHdd (HddPasswordServiceSmm);

  return Status;
}

/**
  Check the input memory buffer whether is in the pre-allocated range.

  @param[in] Buffer       The pointer to the buffer to be checked.
  @param[in] BufferSize   The size in bytes of the input buffer

  @retval  TRUE        The buffer is pre-allocated Region range.
  @retval  FALSE       The buffer is not pre-allocated range.
**/
BOOLEAN
InPreAllocateRegion (
  IN VOID     *Buffer,
  IN UINTN    BufferSize
  )
{
  EFI_PHYSICAL_ADDRESS  BufferEnd;
  EFI_PHYSICAL_ADDRESS  BufferStart;

  if (Buffer == NULL) {
    return FALSE;
  }

  BufferStart = (EFI_PHYSICAL_ADDRESS) (UINTN) mMemoryBuffer->DataAreaBase;
  BufferEnd   = (EFI_PHYSICAL_ADDRESS) ((UINTN) mMemoryBuffer->DataAreaBase + MEMORY_PREALLOCATE_SIZE);

  if (BufferEnd < BufferStart) {
    return FALSE;
  }

  if ((EFI_PHYSICAL_ADDRESS) (UINTN) Buffer + BufferSize - 1 >= BufferEnd ||
      (EFI_PHYSICAL_ADDRESS) (UINTN) Buffer < BufferStart) {
    return FALSE;
  }

  return TRUE;
}

/**
  Initialize the data which are used in SMM.

  @param[in]  HddPasswordPrivate   Points to the private data

  @retval EFI_STATUS

**/
EFI_STATUS
HddPasswordSmmDriverInstall (
  IN  HDD_PASSWORD_PRIVATE              *HddPasswordPrivate
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            HddPasswordHandle;

  if (HddPasswordPrivate->HddInfoManager == NULL) {
    return EFI_NOT_READY;
  }

  if (!InPreAllocateRegion ((VOID *)HddPasswordPrivate->HddInfoManager, sizeof (HDD_INFO_MANAGER))) {
    return EFI_ACCESS_DENIED;
  }

  if (mHddPasswordPrivate == NULL) {
    mHddPasswordPrivate = AllocateBuffer (sizeof (HDD_PASSWORD_PRIVATE));
    if (mHddPasswordPrivate == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  mHddPasswordPrivate->Signature      = HDD_PASSWORD_SIGNATURE;
  mHddPasswordPrivate->HddInfoManager = HddPasswordPrivate->HddInfoManager;

  mHddPasswordPrivate->HddPasswordProtocol.SetHddPassword          = SetHddPassword;
  mHddPasswordPrivate->HddPasswordProtocol.UnlockHddPassword       = UnlockHddPassword;
  mHddPasswordPrivate->HddPasswordProtocol.DisableHddPassword      = DisableHddPassword;
  mHddPasswordPrivate->HddPasswordProtocol.HddIdentify             = HddIdentify;
  mHddPasswordPrivate->HddPasswordProtocol.GetHddInfo              = GetHddInfoArray;
  mHddPasswordPrivate->HddPasswordProtocol.UnlockAllHdd            = UnlockAllHdd;
  mHddPasswordPrivate->HddPasswordProtocol.HddFreeze               = HddFreeze;
  mHddPasswordPrivate->HddPasswordProtocol.SendHddSecurityCmd      = SendHddSecurityCmd;
  mHddPasswordPrivate->HddPasswordProtocol.ResetSecurityStatus     = ResetSecurityStatus;
  mHddPasswordPrivate->HddPasswordProtocol.PasswordStringProcess   = HddPasswordStringProcess;
  mHddPasswordPrivate->HddPasswordProtocol.DisabledLockedHdd       = DisabledLockedHdd;
  mHddPasswordPrivate->HddPasswordS3Data.SmmMode                   = TRUE;
  mHddPasswordPrivate->UefiOsFastBootSupported                     = HddPasswordPrivate->UefiOsFastBootSupported;

  HddPasswordHandle = NULL;
  Status = mSmst2->SmmInstallProtocolInterface (
                     &HddPasswordHandle,
                     &gEfiHddPasswordServiceProtocolGuid,
                     EFI_NATIVE_INTERFACE,
                     &(mHddPasswordPrivate->HddPasswordProtocol)
                     );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mHddPasswordS3Registration == NULL) {
    //
    //  S3 resume event
    //
    Status = mSmst2->SmmRegisterProtocolNotify (
                       &gAcpiRestoreCallbackStartProtocolGuid,
                       (EFI_SMM_NOTIFY_FN)S3CheckHddPassword,
                       &mHddPasswordS3Registration
                       );
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}


/**
  DevicePath comapre.

  @param[in]        Event               Pointer to this event
  @param[in]        Context             Event handler private data

  @retval EFI_STATUS

**/
EFI_STATUS
HddPasswordCompareDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL           *DevicePathD,
  IN EFI_DEVICE_PATH_PROTOCOL           *DevicePathS
  )
{
  EFI_DEVICE_PATH_PROTOCOL              *DevicePathNodeD;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePathNodeS;

  DevicePathNodeD = DevicePathD;
  DevicePathNodeS = DevicePathS;

  while (!IsDevicePathEnd (DevicePathNodeD) && !IsDevicePathEnd (DevicePathNodeS)) {
    if (DevicePathNodeLength (DevicePathNodeD) != DevicePathNodeLength (DevicePathNodeS)) {
      return EFI_ABORTED;
    }
    if (CompareMem (DevicePathNodeD, DevicePathNodeS, DevicePathNodeLength (DevicePathNodeS)) != 0) {
      return EFI_ABORTED;
    }
    DevicePathNodeD = NextDevicePathNode (DevicePathNodeD);
    DevicePathNodeS = NextDevicePathNode (DevicePathNodeS);
  }

  if (IsDevicePathEnd (DevicePathNodeD) && IsDevicePathEnd (DevicePathNodeS)) {
    return EFI_SUCCESS;
  }

  return EFI_ABORTED;
}

/**
  Update HddPassword SMM private data

  @retval EFI_SUCCESS     Update SMM private data successfully
  @retval Others          Something error occured
**/
EFI_STATUS
EFIAPI
UpdateSmmPrivateData (
  VOID
  )
{
  EFI_STATUS                             Status;
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *StorageSecurityCommand;
  EFI_HANDLE                             *StorageSecurityCommandHandleBuffer;
  UINTN                                  StorageSecurityCommandHandleBufferSize;
  UINTN                                  HandleCount;
  UINTN                                  Index;
  LIST_ENTRY                             *HddInfoPrivateListHead;
  LIST_ENTRY                             *Link;
  HDD_PASSWORD_HDD_INFO_PRIVATE          *HddInfoPrivate;
  HDD_INFO_MANAGER                       *HddInfoManager;
  EFI_DEVICE_PATH_PROTOCOL               *DevicePath;
  BOOLEAN                                Found;

  HddInfoPrivate                         = NULL;
  HddInfoManager                         = mHddPasswordPrivate->HddInfoManager;
  StorageSecurityCommandHandleBuffer     = NULL;
  StorageSecurityCommandHandleBufferSize = 0;

  if (mSmst2 == NULL || !InPreAllocateRegion ((VOID *)HddInfoManager, sizeof (HDD_INFO_MANAGER))) {
    return EFI_ACCESS_DENIED;
  }

  Status = mSmst2->SmmLocateHandle (
                     ByProtocol,
                     &gEfiStorageSecurityCommandProtocolGuid,
                     NULL,
                     &StorageSecurityCommandHandleBufferSize,
                     StorageSecurityCommandHandleBuffer
                     );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Status = mSmst2->SmmAllocatePool (
                       EfiRuntimeServicesData,
                       StorageSecurityCommandHandleBufferSize,
                       (VOID*) &StorageSecurityCommandHandleBuffer
                       );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = mSmst2->SmmLocateHandle (
                       ByProtocol,
                       &gEfiStorageSecurityCommandProtocolGuid,
                       NULL,
                       &StorageSecurityCommandHandleBufferSize,
                       StorageSecurityCommandHandleBuffer
                       );
  }

  if (EFI_ERROR (Status) || StorageSecurityCommandHandleBuffer == NULL) {
    return Status;
  }

  HandleCount = StorageSecurityCommandHandleBufferSize / sizeof (EFI_HANDLE);
  for (Index = 0; Index < HandleCount; Index++) {
    Status = mSmst2->SmmHandleProtocol (
                       StorageSecurityCommandHandleBuffer[Index],
                       &gEfiStorageSecurityCommandProtocolGuid,
                       (VOID **)&StorageSecurityCommand
                       );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = mSmst2->SmmHandleProtocol (
                       StorageSecurityCommandHandleBuffer[Index],
                       &gEfiDevicePathProtocolGuid,
                       (VOID *)&DevicePath
                       );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Found = FALSE;
    HddInfoPrivateListHead = &HddInfoManager->HddInfoPrivateListHead;
    for (Link = GetFirstNode (HddInfoPrivateListHead);
         !IsNull (HddInfoPrivateListHead, Link);
         Link = GetNextNode (HddInfoPrivateListHead, Link)) {

      HddInfoPrivate = GET_HDD_INFO_PRIVATE_FROM_LINK (Link);
      if (!InPreAllocateRegion ((VOID *)HddInfoPrivate, sizeof (HDD_PASSWORD_HDD_INFO_PRIVATE)) ||
          HddInfoPrivate->Signature != HDD_PASSWORD_HDD_INFO_PRIVATE_SIGNATURE) {
        FreePool (StorageSecurityCommandHandleBuffer);
        return EFI_ACCESS_DENIED;
      }

      Status = HddPasswordCompareDevicePath (HddInfoPrivate->DevicePath, DevicePath);
      if (!EFI_ERROR (Status)) {
        Found = TRUE;
        break;
      }
    }

    if (!Found) {
      continue;
    }

    HddInfoPrivate->HddInfo.DeviceHandleInSmm   = StorageSecurityCommandHandleBuffer[Index];
    HddInfoPrivate->DeviceHandleInSmm           = StorageSecurityCommandHandleBuffer[Index];
    HddInfoPrivate->StorageSecurityCommandInSmm = StorageSecurityCommand;
  }

  FreePool (StorageSecurityCommandHandleBuffer);

  return EFI_SUCCESS;
}

/**
  Communication service SMI Handler entry.
  This SMI handler provides services for the firmware volume access through SMI.

  @param[in]     DispatchHandle               The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     RegisterContext              Points to an optional handler context which was specified when the
                                              handler was registered.
  @param[in, out] CommBuffer                  A pointer to a collection of data in memory that will
                                              be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize              The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.
**/
EFI_STATUS
EFIAPI
HddPasswordSmiHandler (
  IN     EFI_HANDLE                                DispatchHandle,
  IN     CONST VOID                                *RegisterContext,
  IN OUT VOID                                      *CommBuffer,
  IN OUT UINTN                                     *CommBufferSize
  )
{
  EFI_STATUS                                       Status;
  HDD_PASSWORD_SMM_PARAMETER_HEADER                *SmmCommHeader;
  HDD_PASSWORD_SMM_PARAMETER_HEADER                *SmmCommData;
  HDD_PASSWORD_PRIVATE                             *HddPasswordPrivateSmm;

  //
  // If input is invalid, stop processing this SMI
  //
  if (CommBuffer == NULL || CommBufferSize == NULL) {
    return EFI_SUCCESS;
  }

  //
  // CommBuffer should located inside preallocated fixed buffer
  //
  if ((UINTN)CommBuffer != (UINTN)mCommBuffer + SMM_COMMUNICATE_HEADER_SIZE) {
    return EFI_SUCCESS;
  }

  if (*CommBufferSize != OFFSET_OF (HDD_PASSWORD_SMM_PARAMETER_HEADER, Data) + sizeof (HDD_PASSWORD_PRIVATE)) {
    return EFI_SUCCESS;
  }

  //
  // Copy CommBuffer to SMRAM to prevent from TOCTOU attack.
  //
  SmmCommHeader = AllocateCopyPool (*CommBufferSize, CommBuffer);
  if (SmmCommHeader == NULL) {
    return EFI_SUCCESS;
  }

  SmmCommData = (HDD_PASSWORD_SMM_PARAMETER_HEADER*) SmmCommHeader;
  HddPasswordPrivateSmm = (HDD_PASSWORD_PRIVATE *)SmmCommData->Data;
  switch (SmmCommHeader->Function) {
  case HDD_PASSWORD_SMM_COLLECT_DEVICE_INSTANCE:
    if (mHddPasswordPrivate == NULL) {
      HddPasswordSmmDriverInstall (HddPasswordPrivateSmm);
    }

    Status = UpdateSmmPrivateData ();

    break;

  case HDD_PASSWORD_ACPI_RESTORE_CALLBACK_NOTIFY_INSTANCE:

    //
    // Register notify for S3 resuming
    //

    if (mHddPasswordS3Registration == NULL) {
      Status = mSmst2->SmmRegisterProtocolNotify (
                         &gAcpiRestoreCallbackStartProtocolGuid,
                         (EFI_SMM_NOTIFY_FN)S3CheckHddPassword,
                         &mHddPasswordS3Registration
                         );
    } else {
      Status = EFI_SUCCESS;
    }

    break;

  case HDD_PASSWORD_SMM_BLOCK_SID_INSTANCE:

    mEnableBlockSid = *(BOOLEAN *)SmmCommData->Data;
    Status = EFI_SUCCESS;
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  SmmCommHeader->ReturnStatus = Status;
  //
  // Restore HDD_PASSWORD_SMM_PARAMETER_HEADER data from SMRAM before free SMRAM.
  //
  CopyMem (CommBuffer, SmmCommHeader, *CommBufferSize);
  FreePool (SmmCommHeader);

  return EFI_SUCCESS;
}

/**
  ReadyToBootEvent callback function.
  This function is used to trigger SMI for register gAcpiRestoreCallbackStartProtocolGuid in SMM.

  @param[in]        Event               Pointer to this event
  @param[in]        Context             Event handler private data

**/
VOID
HddPasswordReadyToBootEventCallback (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_COMMUNICATION_PROTOCOL        *SmmCommunication;
  EFI_SMM_COMMUNICATE_HEADER            *SmmCommHeader;
  HDD_PASSWORD_SMM_PARAMETER_HEADER     *HddPasswordSmmHeader;
  UINT8                                 *HddPasswordSmmData;
  UINTN                                 CommBufferSize;
  UINT32                                PpStorageFlag;

  Status = EFI_SUCCESS;

  PpStorageFlag = Tcg2PhysicalPresenceLibGetManagementFlags ();
  mEnableBlockSid = ((PpStorageFlag & TCG2_BIOS_STORAGE_MANAGEMENT_FLAG_ENABLE_BLOCK_SID) != 0);
  if (mEnableBlockSid) {
    IssueBlockSid ();
  }

  //
  // Trigger SMI for SMM instance
  //
  Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &SmmCommunication);
  if (Status == EFI_SUCCESS) {
    //
    // HDD_PASSWORD_ACPI_RESTORE_CALLBACK_NOTIFY_INSTANCE
    //
    SmmCommHeader = InitCommunicateBuffer (NULL, sizeof (HDD_PASSWORD_PRIVATE), HDD_PASSWORD_ACPI_RESTORE_CALLBACK_NOTIFY_INSTANCE);
    if (SmmCommHeader == NULL) {
      return;
    }
    CommBufferSize = SmmCommHeader->MessageLength + SMM_COMMUNICATE_HEADER_SIZE;
    Status = SmmCommunication->Communicate (SmmCommunication, SmmCommHeader, &CommBufferSize);
    HddPasswordSmmHeader = (HDD_PASSWORD_SMM_PARAMETER_HEADER *)SmmCommHeader->Data;
    Status = (EFI_STATUS)HddPasswordSmmHeader->ReturnStatus;
    DEBUG ((EFI_D_INFO, "SMM Communicate: Setup ACPI Restore Callback - (%r)\n", Status));

    //
    // HDD_PASSWORD_SMM_BLOCK_SID_INSTANCE
    //
    SmmCommHeader = InitCommunicateBuffer ((VOID **)&HddPasswordSmmData, sizeof (HDD_PASSWORD_PRIVATE), HDD_PASSWORD_SMM_BLOCK_SID_INSTANCE);
    if (SmmCommHeader == NULL) {
      return;
    }
    CopyMem ((VOID *)HddPasswordSmmData, (VOID *)&mEnableBlockSid, sizeof (mEnableBlockSid));
    CommBufferSize = SmmCommHeader->MessageLength + SMM_COMMUNICATE_HEADER_SIZE;
    Status = SmmCommunication->Communicate (SmmCommunication, SmmCommHeader, &CommBufferSize);
    HddPasswordSmmHeader = (HDD_PASSWORD_SMM_PARAMETER_HEADER *)SmmCommHeader->Data;
    Status = (EFI_STATUS)HddPasswordSmmHeader->ReturnStatus;
    DEBUG ((EFI_D_INFO, "SMM Communicate: Setup BlockSID - (%r)\n", Status));
  }

  if (mDriverInstallInfo->LegacyBios != NULL) {
    //
    //  Process RAID mode in legacy boot type
    //
    ProcessLegacyRaidSupport ();
  }

  gBS->CloseEvent (Event);
}

/**
  To install HDD password services after supported devices and controllers being reconnected.

  @param[in]        Event               Pointer to this event
  @param[in]        Handle              The handle associated with a previously registered checkpoint handler.

**/
VOID
EFIAPI
SscpNotifyFunction (
  IN EFI_EVENT                  Event,
  IN VOID                       *Context
  )
{
  mHddPasswordPrivate->ControllerReconnected = TRUE;
}

/**
  To install HDD password services for all supported devices.

  @param[in]        Event               Pointer to this event
  @param[in]        Handle              The handle associated with a previously registered checkpoint handler.

**/
VOID
H2OBdsCpConOutAfterCallback (
  IN EFI_EVENT                              Event,
  IN H2O_CP_HANDLE                          Handle
  )
{
  EFI_STATUS                   Status;
  VOID                         *Registration;
  EFI_EVENT                    Events;

  H2OCpUnregisterHandler (Handle);


  if (!mHddPasswordPrivate->UefiOsFastBootSupported) {

    HddPasswordDriverInstall (TRUE);

    Status = HddUnlockDialogInit (&(mHddPasswordPrivate->HddPasswordProtocol));

    //
    // Check to see if there's any more SSCP created after this point
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    SscpNotifyFunction,
                    NULL,
                    &gEfiStorageSecurityCommandProtocolGuid,
                    &Events
                    );
    if (Status == EFI_SUCCESS) {
      Status = gBS->RegisterProtocolNotify (
                      &gEfiStorageSecurityCommandProtocolGuid,
                      Events,
                      &Registration
                      );
    }

  }
}


/**
  To check if Intel Raid devices have being security enabled, if Yes, reconnect the controller.
  This is due to Intel Rst Driver can't produce BlockIo, after unlock a device, until re-enumerate device again for security reason.

  @param[in]        Event               Pointer to this event
  @param[in]        Handle              The handle associated with a previously registered checkpoint handler.

**/
VOID
H2OBdsCpEnumerteBeforeCallback (
  IN EFI_EVENT                              Event,
  IN H2O_CP_HANDLE                          Handle
  )
{
  H2OCpUnregisterHandler (Handle);
  if (mHddPasswordPrivate->ControllerReconnected) {
    HddPasswordDriverInstall (TRUE);
    mHddPasswordPrivate->ControllerReconnected = FALSE;
  }
}

/**
  Main entry for this driver.

  @param[in]        ImageHandle         Image handle this driver.
  @param[in]        SystemTable         Pointer to SystemTable.

  @retval           EFI_SUCCESS         This function always complete successfully.

**/
EFI_STATUS
HddPasswordEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  BOOLEAN                               InSmm;
  HDD_INFO_MANAGER                      *HddInfoManager;
  EFI_HANDLE                            Handle;
  EFI_EVENT                             ReadyToBootEvent;
  HDD_PASSWORD_TABLE                    *HddPasswordTable;
  UINTN                                 HddPasswordTableSize;
  UINT8                                 H2OHddPasswordUefiOsFastBootSupport;
  H2O_CP_HANDLE                         H2OBdsCpConnectAllAfterHandle;
  EFI_HANDLE                            DispatchHandle;

  InSmm  = FALSE;

  //
  // Find the SMM base 2 protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&mSmmBase2
                  );

  if (!(EFI_ERROR (Status))) {
    Status = mSmmBase2->InSmm (
                          mSmmBase2,
                          &InSmm
                          );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if (!InSmm) {
    //
    //  In DXE
    //

    //
    // For memory managerment
    //

    Status = InitializeMemoryBuffer (&mMemoryBuffer);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    mCommBuffer = AllocateRuntimePool (
                    SMM_COMMUNICATE_HEADER_SIZE +
                    OFFSET_OF (HDD_PASSWORD_SMM_PARAMETER_HEADER, Data) +
                    sizeof (HDD_PASSWORD_PRIVATE)
                    );
    ASSERT (mCommBuffer != NULL);
    if (mCommBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    mMemoryBuffer->CommBuffPtr = mCommBuffer;

    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiCallerIdGuid,
                    EFI_NATIVE_INTERFACE,
                    mMemoryBuffer
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    //  Alloc private data
    //
    mHddPasswordPrivate = AllocateBuffer (sizeof (HDD_PASSWORD_PRIVATE));
    if (mHddPasswordPrivate == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    //
    //  Alloc HDD Info private
    //
    HddInfoManager = AllocateBuffer (sizeof (HDD_INFO_MANAGER));
    if (HddInfoManager == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    //
    // Initialize the platform specific string and language
    //
    InitializeStringSupport ();

    //
    //  Initialize Driver install info structure
    //
    mDriverInstallInfo = AllocateBuffer (sizeof (DRIVER_INSTALL_INFO));

    InitializeListHead (&mDriverInstallInfo->AtaControllerInfoListHead);

    mHddPasswordPrivate->Signature      = HDD_PASSWORD_SIGNATURE;
    mHddPasswordPrivate->HddInfoManager = HddInfoManager;
    InitializeListHead (&HddInfoManager->HddInfoPrivateListHead);

    mHddPasswordPrivate->HddPasswordProtocol.SetHddPassword          = SetHddPassword;
    mHddPasswordPrivate->HddPasswordProtocol.UnlockHddPassword       = UnlockHddPassword;
    mHddPasswordPrivate->HddPasswordProtocol.DisableHddPassword      = DisableHddPassword;
    mHddPasswordPrivate->HddPasswordProtocol.HddIdentify             = HddIdentify;
    mHddPasswordPrivate->HddPasswordProtocol.GetHddInfo              = GetHddInfoArray;
    mHddPasswordPrivate->HddPasswordProtocol.UnlockAllHdd            = UnlockAllHdd;
    mHddPasswordPrivate->HddPasswordProtocol.HddFreeze               = HddFreeze;
    mHddPasswordPrivate->HddPasswordProtocol.SendHddSecurityCmd      = SendHddSecurityCmd;
    mHddPasswordPrivate->HddPasswordProtocol.ResetSecurityStatus     = ResetSecurityStatus;
    mHddPasswordPrivate->HddPasswordProtocol.PasswordStringProcess   = HddPasswordStringProcess;
    mHddPasswordPrivate->HddPasswordProtocol.DisabledLockedHdd       = DisabledLockedHdd;

    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiHddPasswordServiceProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &(mHddPasswordPrivate->HddPasswordProtocol)
                    );
    if (Status != EFI_SUCCESS) {
      return Status;
    }

    //
    //  If support RAID mode with Legacy boot type, it have to hook the service "InstallPciRom" of Legace Bios protcol.
    //
    Status = InitLegacyRaidSupport ();
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (FeaturePcdGet (PcdH2OBdsCpConOutConnectAfterSupported)) {
      H2O_CP_HANDLE                         H2OBdsCpConOutAfterHandle;
      //
      // Register for callback on Storage Security Protocol publication
      //
      H2OBdsCpConOutAfterHandle = NULL;
      Status = H2OCpRegisterHandler (
                 &gH2OBdsCpConOutConnectAfterGuid,
                 H2OBdsCpConOutAfterCallback,
                 HDD_PASSWORD_CONNECT_CONTROLLER_NOTIFY_TPL,
                 &H2OBdsCpConOutAfterHandle
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpConOutConnectAfterGuid, Status));
        return Status;
      }
      DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpConOutConnectAfterGuid, Status));
    }

    if (FeaturePcdGet (PcdH2OBdsCpBootDeviceEnumBeforeSupported)) {
      //
      // Register for callback to collect HddPassword related information for possible controller handle reconnect
      //
      H2O_CP_HANDLE H2OBdsCpBootDeviceEnumBeforeHandle = NULL;
      Status = H2OCpRegisterHandler (
                 &gH2OBdsCpBootDeviceEnumBeforeGuid,
                 H2OBdsCpEnumerteBeforeCallback,
                 H2O_CP_LOW,
                 &H2OBdsCpBootDeviceEnumBeforeHandle
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpBootDeviceEnumBeforeGuid, Status));
      }
      DEBUG ((EFI_D_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpBootDeviceEnumBeforeGuid, Status));
    }

    Status = EfiCreateEventReadyToBootEx (
                  TPL_CALLBACK,
                  (EFI_EVENT_NOTIFY)HddPasswordReadyToBootEventCallback,
                  NULL,
                  &ReadyToBootEvent
                  );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Get PcdH2OHddPasswordUefiOsFastBootSupport to check UEFI OS Fast Boot support
    //
    mHddPasswordPrivate->UefiOsFastBootSupported = FALSE;

    H2OHddPasswordUefiOsFastBootSupport = PcdGetBool (PcdH2OHddPasswordUefiOsFastBootSupport);

    if (H2OHddPasswordUefiOsFastBootSupport) {
      //
      //  Check variable for HDD password exist?
      //

      HddPasswordTableSize = 0;
      HddPasswordTable = NULL;

      Status = CommonGetVariableDataAndSize (
                 SAVE_HDD_PASSWORD_VARIABLE_NAME,
                 &gSaveHddPasswordGuid,
                 &HddPasswordTableSize,
                 (VOID **) &HddPasswordTable
                 );
      if (HddPasswordTable == NULL) {
        //
        // Collect storage devices information after connect all
        //
        if (FeaturePcdGet (PcdH2OBdsCpConnectAllAfterSupported)) {
          Status = H2OCpRegisterHandler (
                     &gH2OBdsCpConnectAllAfterGuid,
                     H2OBdsCpConnectAllAfterCallback,
                     H2O_CP_MEDIUM,
                     &H2OBdsCpConnectAllAfterHandle
                     );
          if (EFI_ERROR (Status)) {
            DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &H2OBdsCpConnectAllAfterCallback, Status));
            return Status;
          }
          DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &H2OBdsCpConnectAllAfterCallback, Status));
        }
        mHddPasswordPrivate->UefiOsFastBootSupported = TRUE;
      } else {
        gBS->FreePool (HddPasswordTable);
      }

    }

  } else {
    //
    // In SMM
    //

    //
    // For memory managerment
    //
    Status = gBS->LocateProtocol (
                     &gEfiCallerIdGuid,
                     NULL,
                     (VOID**)&mMemoryBuffer
                     );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    mCommBuffer = mMemoryBuffer->CommBuffPtr;

    Status = mSmmBase2->GetSmstLocation (
                          mSmmBase2,
                          &mSmst2
                          );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    mHddPasswordPrivate = NULL;

    //
    // Register SMM communication handler
    //
    DispatchHandle = NULL;

    Status = mSmst2->SmiHandlerRegister (
                       HddPasswordSmiHandler,
                       &gEfiHddPasswordServiceProtocolGuid,
                       &DispatchHandle
                       );
  }

  return EFI_SUCCESS;
}
