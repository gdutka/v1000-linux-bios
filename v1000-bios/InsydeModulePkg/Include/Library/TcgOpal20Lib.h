/** @file
  TCG Opal 2.0 Library
  
;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _TCG_OPAL20_LIB_H_
#define _TCG_OPAL20_LIB_H_

#include <Uefi.h>
#include <TcgOpal.h>
#include <Protocol/StorageSecurityCommand.h>

//
// LockingGlobalRange table
//
#define LOCKING_DISABLED                   0x00
#define READLOCKING_ENABLED                0x01
#define WRITELOCKING_ENABLED               0x02

#define LOCKING_UNLOCKED                   0x00
#define READ_LOCKED                        0x01
#define WRITE_LOCKED                       0x02

typedef enum _OPAL_UID {
  NONE_UID = 0x00,
  //
  // Invoking ID
  //
  SMUID_UID,
  THISSP_UID,
  ADMINSP_UID,
  LOCKINGSP_UID,
  ANYBODY_UID,
  SID_UID,
  ADMIN1_UID,
  USER1_UID,
  PSID_UID,
  //
  // Table
  //
  LOCKINGRANGE_GLOBAL,
  MBRCONTROL,
  MBR,
  AUTHORITY_TABLE,
  C_PIN_TABLE,
  LOCKING_INFO_TABLE,
  //
  // C_PIN
  //
  C_PIN_MSID,
  C_PIN_SID,
  C_PIN_ADMIN1,
  UID_MAX
} OPAL_UID;

typedef enum _OPAL_METHOD {
  PROPERTIES,
  STARTSESSION,
  REVERT,
  ACTIVATE,
  NEXT,
  GETACL,
  GENKEY,
  REVERTSP,
  GET,
  SET,
  AUTHENTICATE,
  RANDOM,
  ERASE,
  METHOD_MAX
} OPAL_METHOD;

typedef struct _DISCOVERY0_INFO {
  //
  // Allocated size of the structure
  //
  UINT8  Size;
  //
  // Supported features
  //
  UINT8  TPer;
  UINT8  Locking;
  UINT8  Geometry;
  UINT8  Opal20;
  UINT8  Opalite;
  UINT8  Pyrite;
  UINT8  Opal10;
  UINT8  SingleUser;
  UINT8  DataStore;
  //
  // Valid if corresponding feature supported
  //
  UINT8  TPerVersion;
  UINT8  TPerSync;
  UINT8  TPerAsync;
  UINT8  TPerAckNak;
  UINT8  TPerBufferMgt;
  UINT8  TPerStreaming;
  UINT8  TPerComIDMgt;
  
  UINT8  LockingVersion;
  UINT8  LockingLockingSupported;
  UINT8  LockingLockingEnabled;
  UINT8  LockingLocked;
  UINT8  LockingMediaEncryption;
  UINT8  LockingMBREnabled;
  UINT8  LockingMBRDone;
  
  UINT8  GeometryVersion;
  UINT8  GeometryAlign;
  UINT32 GeometryLogicalBlockSize;
  UINT64 GeometryAlignmentGranularity;
  UINT64 GeometryLowestAlignedLBA;
  
  UINT8  Opal20Version;
  UINT16 Opal20BaseComID;
  UINT16 Opal20NumOfComIDs;
  UINT8  Opal20RangeCrossing;
  UINT16 Opal20NumOfAdmin;
  UINT16 Opal20NumOfUser;
  UINT8  Opal20InitialPIN;
  UINT8  Opal20RevertedPIN;

  UINT8  OpaliteVersion;
  UINT16 OpaliteBaseComID;
  UINT16 OpaliteNumOfComIDs;
  UINT8  OpaliteInitialPIN;
  UINT8  OpaliteRevertedPIN;

  UINT8  PyriteVersion;
  UINT16 PyriteBaseComID;
  UINT16 PyriteNumOfComIDs;
  UINT8  PyriteInitialPIN;
  UINT8  PyriteRevertedPIN;

  UINT8  Opal10Version;
  UINT16 Opal10BaseComID;
  UINT16 Opal10NumOfComIDs;
  UINT8  Opal10RangeCrossing;

  UINT8  SingleUserVersion;
  UINT32 SingleUserNumOfLockingObjects;
  UINT8  SingleUserAny;
  UINT8  SingleUserAll;
  UINT8  SingleUserPolicy;

  UINT8  DataStoreVersion;
  UINT16 DataStoreMaxTables;
  UINT32 DataStoreMaxTableSize;
  UINT32 DataStoreAlignment;
} DISCOVERY0_INFO;

/**
  Reset the state of the synchronous protocol stack.

  @param[in]  Ssc            Pointer to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL
  @param[in]  MediaId        The media ID of the device
  @param[in]  BaseComID      ComID associated with the session

**/
EFI_STATUS
EFIAPI
StackReset (
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL     *Ssc,
  IN  UINT32                                    MediaId,
  IN  UINT16                                    BaseComID
  );

/**
  Take ownership of TCG OPAL compliant SED.

  @param[in]  Ssc                 Pointer to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL
  @param[in]  MediaId             The media ID of the device
  @param[in]  BaseComID           ComID associated with the session
  @param[in]  NewPassword         New SID password
  @param[in]  NewPasswordLength   The length of new SID password
  
  @retval EFI_SUCCESS        Operation succeeded
  @retval Others             Operation failed

**/
EFI_STATUS
EFIAPI
TakeOwnership (
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL     *Ssc,
  IN  UINT32                                    MediaId,
  IN  UINT16                                    BaseComID,
  IN  UINT8                                     *NewPassword,
  IN  UINTN                                     NewPasswordLength
  );

/**
  Configure Locking Global Range.

  @param[in]  Ssc                 Pointer to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL
  @param[in]  MediaId             The media ID of the device
  @param[in]  BaseComID           ComID associated with the session
  @param[in]  Admin1              Admin1 password
  @param[in]  Admin1Length        The length of Admin1 password
  @param[in]  Enable              Enable ReadLocked/WriteLocked
  
  @retval EFI_SUCCESS        Operation succeeded
  @retval Others             Operation failed

**/
EFI_STATUS
EFIAPI
ConfigureLockingGlobalRange (
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL     *Ssc,
  IN  UINT32                                    MediaId,
  IN  UINT16                                    BaseComID,
  IN  UINT8                                     *Admin1,
  IN  UINTN                                     Admin1Length,
  IN  UINT8                                     Enable
  );

/**
  Set Locking Global Range.

  @param[in]  Ssc                 Pointer to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL
  @param[in]  MediaId             The media ID of the device
  @param[in]  BaseComID           ComID associated with the session
  @param[in]  Admin1              Admin1 password
  @param[in]  Admin1Length        The length of Admin1 password
  @param[in]  Lock                Lock/Unlock the data read/write
  
  @retval EFI_SUCCESS        Operation succeeded
  @retval Others             Operation failed

**/
EFI_STATUS
EFIAPI
SetLockingGlobalRange (
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL     *Ssc,
  IN  UINT32                                    MediaId,
  IN  UINT16                                    BaseComID,
  IN  UINT8                                     *Admin1,
  IN  UINTN                                     Admin1Length,
  IN  UINT8                                     Lock
  );

/**
  Set SID password of AdminSP.

  @param[in]  Ssc                 Pointer to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL
  @param[in]  MediaId             The media ID of the device
  @param[in]  BaseComID           ComID associated with the session
  @param[in]  SID                 SID password
  @param[in]  SIDLength           The length of SID password
  @param[in]  NewSID              New SID password
  @param[in]  NewSIDLength        The length of new SID password
  
  @retval EFI_SUCCESS        Operation succeeded
  @retval Others             Operation failed

**/
EFI_STATUS
EFIAPI
SetSIDAdminSP (
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL     *Ssc,
  IN  UINT32                                    MediaId,
  IN  UINT16                                    BaseComID,
  IN  UINT8                                     *SID,
  IN  UINTN                                     SIDLength,
  IN  UINT8                                     *NewSID,
  IN  UINTN                                     NewSIDLength
  );

/**
  Set Admin1 password of LockingSP.

  @param[in]  Ssc                 Pointer to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL
  @param[in]  MediaId             The media ID of the device
  @param[in]  BaseComID           ComID associated with the session
  @param[in]  Admin1              Admin1 password
  @param[in]  Admin1Length        The length of Admin1 password
  @param[in]  NewAdmin1           New Admin1 password
  @param[in]  NewAdmin1Length     The length of new Admin1 password
  
  @retval EFI_SUCCESS        Operation succeeded
  @retval Others             Operation failed

**/
EFI_STATUS
EFIAPI
SetAdmin1LockingSP (
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL     *Ssc,
  IN  UINT32                                    MediaId,
  IN  UINT16                                    BaseComID,
  IN  UINT8                                     *Admin1,
  IN  UINTN                                     Admin1Length,
  IN  UINT8                                     *NewAdmin1,
  IN  UINTN                                     NewAdmin1Length
  );

/**
  Revert AdminSP with PSID authority.

  @param[in]  Ssc            Pointer to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL
  @param[in]  MediaId        The media ID of the device
  @param[in]  BaseComID      ComID associated with the session
  @param[in]  PSID           Physical Presence SID of the device
  @param[in]  PSIDLength     The length of Physical Presence SID
  
  @retval EFI_SUCCESS        Operation succeeded
  @retval Others             Operation failed

**/
EFI_STATUS
EFIAPI
PSIDRevert (
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL     *Ssc,
  IN  UINT32                                    MediaId,
  IN  UINT16                                    BaseComID,
  IN  UINT8                                     *PSID,
  IN  UINTN                                     PSIDLength
  );

/**
  RevertSP LockingSP.

  @param[in]  Ssc                 Pointer to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL
  @param[in]  MediaId             The media ID of the device
  @param[in]  BaseComID           ComID associated with the session
  @param[in]  Admin1              Admin1 password
  @param[in]  Admin1Length        The length of Admin1 password
  @param[in]  KeepGlobalLocking   Whether eradicating the media encryption key for 
                                  the Global locking range
  
  @retval EFI_SUCCESS        Operation succeeded
  @retval Others             Operation failed

**/
EFI_STATUS
EFIAPI
RevertLockingSP (
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL     *Ssc,
  IN  UINT32                                    MediaId,
  IN  UINT16                                    BaseComID,
  IN  UINT8                                     *Admin1,
  IN  UINTN                                     Admin1Length,
  IN  BOOLEAN                                   KeepGlobalLocking
  );

/**
  Get default SID password (MSID).

  @param[in]      Ssc                 Pointer to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL
  @param[in]      MediaId             The media ID of the device
  @param[in]      BaseComID           ComID associated with the session
  @param[out]     SID                 MSID password
  @param[in,out]  SIDLength           The length of MSID password
  
  @retval EFI_SUCCESS        Operation succeeded
  @retval Others             Operation failed

**/
EFI_STATUS
EFIAPI
GetMSID (
  IN     EFI_STORAGE_SECURITY_COMMAND_PROTOCOL     *Ssc,
  IN     UINT32                                    MediaId,
  IN     UINT16                                    BaseComID,
     OUT UINT8                                     *MSID,
  IN OUT UINTN                                     *MSIDLength
  );

/**
  Get level 0 discovery info.

  @param[in]  Ssc                   Pointer to EFI_STORAGE_SECURITY_COMMAND_PROTOCOL
  @param[in]  MediaId               The media ID of the device
  @param[in]  Discovery0Info        A pointer to a buffer to store DISCOVERY0_INFO
  
  @retval EFI_SUCCESS        Operation succeeded
  @retval Others             Operation failed

**/
EFI_STATUS
EFIAPI
GetDiscovery0Info (
  IN     EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *Ssc,
  IN     UINT32                                 MediaId,
     OUT DISCOVERY0_INFO                        *Discovery0Info
  );
#endif //_TCG_OPAL20_LIB_H_

