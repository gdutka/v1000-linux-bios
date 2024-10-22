/** @file
  Library for Getting Information of Flash Device Region Layout

;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Pi/PiSmmCis.h>
#include <Protocol/Hash.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/Pcd.h>
#include <Protocol/PcdInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FlashRegionLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/PerformanceLib.h>
#include <Library/H2OCpLib.h>
#include <Library/PostCodeLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>

#include <Guid/EventGroup.h>
#include <Guid/H2OCp.h>

STATIC BOOLEAN                    mInSmm;
STATIC BOOLEAN                    mIsAtRuntime                    = FALSE;
STATIC EFI_EVENT                  mExitBootServiceEvent           = NULL;
STATIC EFI_SMM_SYSTEM_TABLE2      *mSmst                          = NULL;
STATIC UINT64                     mFlashDeviceMapStart            = 0;
STATIC EFI_GUID                   mFdmAddressHobGuid              = { 0xcfae18a2, 0x35e1, 0x4ff1, { 0x8f, 0x98, 0xeb, 0x84, 0xcd, 0x70, 0xff, 0xb0 } };
STATIC EFI_GUID                   mAlternateFdmBinaryProtocolGuid = { 0xf50b7d57, 0x5c33, 0x40bd, { 0x89, 0x32, 0xcb, 0x5e, 0xe5, 0xd5, 0xf0, 0x5f } };
STATIC EFI_GUID                   mFdmContentHobGuid              = { 0x77f7a464, 0x7409, 0x4a19, { 0x8a, 0xae, 0x3f, 0x36, 0xa2, 0x02, 0x8b, 0x93 } };


//
// FDM signature, for not having signature in machine code of this library, assign value one by one byte
//
STATIC CHAR8 mSignature[] = {'H', 'F', 'D', 'M'};


STATIC
BOOLEAN
EFIAPI
IsFdmHeaderValid (
  H2O_FLASH_DEVICE_MAP_HEADER    *FdmHeader
 ){

  UINT32 Index;
  UINT8  Sum;
  UINT8  *Data;

  Sum = 0;
  Data = (UINT8*) FdmHeader;

  //
  // Verify check sum
  //
  for (Index = 0; Index < sizeof (H2O_FLASH_DEVICE_MAP_HEADER); Index++, Data++){
    Sum += (*Data);
  }

  return (Sum == 0);

}

STATIC
EFI_STATUS
EFIAPI
LocateProtocol(
  IN  EFI_GUID  *ProtocolGuid,
  OUT VOID      **Protocol
){
  EFI_STATUS    Status;

  if (mSmst != NULL) {
    Status = mSmst->SmmLocateProtocol (ProtocolGuid, NULL, Protocol);
  } else {
    Status = gBS->LocateProtocol (ProtocolGuid, NULL, Protocol);
  }

  if (EFI_ERROR (Status)) {
    *Protocol = NULL;
    return Status;
  }

  return EFI_SUCCESS;

}

/**
  Get Base Address

  @return    Base Address

**/
UINT64
EFIAPI
FdmGetBaseAddr (
  VOID
 ){

  H2O_FLASH_DEVICE_MAP_HEADER   *FdmHeader;

  if (mIsAtRuntime){
    return EFI_UNSUPPORTED;
  }

  //
  // Get FDM by DynamicEx Pcd
  //
  FdmHeader = (H2O_FLASH_DEVICE_MAP_HEADER*)(UINTN) mFlashDeviceMapStart;

  if ((FdmHeader == NULL) || CompareMem (FdmHeader, mSignature, sizeof(mSignature))){
    return 0;
  }
  return FdmHeader->FdBaseAddr;
 }



STATIC
EFI_STATUS
EFIAPI
VerifyHash (
 IN UINT8                         HashMethod,
 IN UINT8                         *Hash,
 IN UINT8                         *Message,
 IN UINT64                        MessageSize
 ){

  EFI_HASH_OUTPUT                 Digest;
  EFI_STATUS                      Status;
  UINTN                           DigestSize;
  UINTN                           HashSize;
  EFI_HASH_PROTOCOL               *HashProtocol;

  if (Hash == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Message == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = LocateProtocol (
             &gEfiHashProtocolGuid,
             (VOID**) &HashProtocol
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_READY;
  }


  switch (HashMethod){

  case ENTRY_HASH_SHA256:

    Status = HashProtocol->GetHashSize (HashProtocol, &gEfiHashAlgorithmSha256Guid, &HashSize);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Digest.Sha256Hash =  AllocatePool (HashSize);

    if (Digest.Sha256Hash == NULL){
      return EFI_OUT_OF_RESOURCES;
    }

    Status = HashProtocol->Hash (
                   HashProtocol,
                   &gEfiHashAlgorithmSha256Guid,
                   FALSE,
                   Message,
                   MessageSize,
                   (EFI_HASH_OUTPUT *) &Digest
                   );

    if (EFI_ERROR(Status)){
      goto done;
    }

    DigestSize =  HashSize;
    break;

  default:

    return EFI_UNSUPPORTED;
    break;
  }

  if (CompareMem (Digest.Sha256Hash, Hash, DigestSize)){
    Status = EFI_SECURITY_VIOLATION;
    goto done;
  } else {
    Status = EFI_SUCCESS;
    goto done;
  }

done:
  FreePool(Digest.Sha256Hash);
  return Status;
}


VOID
EFIAPI
ExitBootServiceCallback (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
) {

  gBS->CloseEvent (mExitBootServiceEvent);
  mIsAtRuntime = TRUE;

}


/**
  Get count of entries that FDM recorded

  @param[out] Count             Unsigned integer that specifies the count of entries that FDM recorded.

  @return EFI_SUCCESS           Get region count success.
  @return EFI_NOT_FOUND         Can't find region type in the FDM.
  @return EFI_UNSUPPORTED       Function called in runtime phase

**/
EFI_STATUS
EFIAPI
FdmGetCount (
  OUT  UINT8  *Count
)
{

  H2O_FLASH_DEVICE_MAP_HEADER   *FdmHeader;
  UINT8                         *Entry;
  UINT32                        FirstEntryOffset;
  UINT32                        EntrySize;
  UINT32                        Size;
  UINT8                        *Entries;
  UINT8                        *EndPtr;
  UINT8                        InstanceNotIgnored;

  if (mIsAtRuntime){
    return EFI_UNSUPPORTED;
  }

  //
  // Get FDM by DynamicEx Pcd
  //
  FdmHeader = (H2O_FLASH_DEVICE_MAP_HEADER*)(UINTN) mFlashDeviceMapStart;

  if ((FdmHeader == NULL) || (CompareMem(FdmHeader, mSignature, sizeof(mSignature)))){
    return EFI_NOT_FOUND;
  }

  if (!IsFdmHeaderValid (FdmHeader)) {
    return EFI_SECURITY_VIOLATION;
  }

  FirstEntryOffset = FdmHeader->Offset;
  Size        = FdmHeader->Size;
  EntrySize   = FdmHeader->EntrySize;

  Entries = ((UINT8*) FdmHeader) + FirstEntryOffset;
  EndPtr = ((UINT8*) FdmHeader) + Size;
  InstanceNotIgnored = 0;

  for (Entry = Entries;;){

    if (Entry >= EndPtr){
      break;
    }

    if ((((H2O_FLASH_DEVICE_MAP_ENTRY*) Entry)->Attribs & H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_IGNORE) != H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_IGNORE){
      InstanceNotIgnored++;
    }
    Entry += EntrySize;
  }
  *Count = InstanceNotIgnored;

  return EFI_SUCCESS;
}

/**
  Get Region type record in FDM by specifying order in the list.

  @param[in]   RegionType            GUID that specifies the type of region that search for.
  @param[in]   Instance              Unsigned integer that specifies the N-th Region type instance
  @param[out]  RegionId              Identifier that specifies the identifier of this region..
  @param[out]  RegionOffset          Unsigned integer that specifies the offset of this region relative to the
                                     base of the flash device image.
  @param[out]  RegionSize            Unsigned integer that specifies the region size.
  @param[out]  Attribs               Bitmap that specifies the attributes of the flash device map entry.


  @return EFI_SUCCESS                find region type success.
  @return EFI_INVALID_PARAMETER      Incorrect parameter.
  @return EFI_SECURITY_VIOLATION     Region hash is not correct.
  @return EFI_NOT_FOUND              Can't find region type in the FDM.
  @return EFI_NOT_READY              EFI_HASH_PROTOCOL not installed yet
  @return EFI_UNSUPPORTED            Function called in runtime phase

**/
EFI_STATUS
EFIAPI
FdmGetNAt (
  IN CONST EFI_GUID  *RegionType,
  IN CONST UINT8     Instance,
  OUT UINT8          *RegionId,
  OUT UINT64         *RegionOffset,
  OUT UINT64         *RegionSize,
  OUT UINT32         *Attribs
  ){

  UINT8                         *EndPtr;
  UINT8                         Matched;
  UINT32                        EntrySize;
  H2O_FLASH_DEVICE_MAP_ENTRY    *Entry;
  UINT8                         *Fdm;

  if (mIsAtRuntime){
    return EFI_UNSUPPORTED;
  }

  //
  // Instance number start from 1
  //
  if (Instance == 0){
    return EFI_INVALID_PARAMETER;
  }
  //
  // Get FDM by DynamicEx Pcd
  //
  Fdm = (UINT8*)(UINTN) mFlashDeviceMapStart;
  if ((Fdm == NULL) || CompareMem(Fdm, mSignature, sizeof(mSignature))){
    return EFI_NOT_FOUND;
  }

  if (!IsFdmHeaderValid ((H2O_FLASH_DEVICE_MAP_HEADER *) Fdm)){
    return EFI_SECURITY_VIOLATION;
  }

  EntrySize  = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->EntrySize;
  EndPtr     = Fdm + ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Size;
  for (Matched = 0, Entry = (H2O_FLASH_DEVICE_MAP_ENTRY*)(Fdm + ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Offset);
       (UINT8*)Entry < EndPtr;
       Entry = (H2O_FLASH_DEVICE_MAP_ENTRY*)(((UINT8*)Entry) + EntrySize)){
    if (!CompareMem(&Entry->RegionType, RegionType, sizeof(EFI_GUID)) &&
        ((Entry->Attribs & H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_IGNORE) != H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_IGNORE)){
      Matched++;
      if (Matched == Instance){
        break;
      }
    }
  }
  if (Matched != Instance){
    return EFI_NOT_FOUND;
  }

  CopyMem (RegionId, Entry->RegionId, FDM_ENTRY_REGION_ID_SIZE);
  *RegionOffset = Entry->RegionOffset;
  *RegionSize   = Entry->RegionSize;
  *Attribs      = Entry->Attribs;
  return EFI_SUCCESS;

}


/**
  Get entry recorded in FDM

  @param[in]   RegionType            GUID that specifies the type of region that N-th entry is.
  @param[out]  Instance              Unsigned integer that specifies entry instance of FDM
  @param[out]  RegionId              Identifier that specifies the identifier of this region..
  @param[out]  RegionOffset          Unsigned integer that specifies the offset of this region relative to the
                                     base of the flash device image.
  @param[out]  RegionSize            Unsigned integer that specifies the region size.
  @param[out]  Attribs               Bitmap that specifies the attributes of the flash device map entry.


  @return EFI_SUCCESS                find region type success.
  @return EFI_INVALID_PARAMETER      Incorrect parameter.
  @return EFI_SECURITY_VIOLATION     Region hash is not correct.
  @return EFI_NOT_FOUND              Can't find region type in the FDM.
  @return EFI_NOT_READY              EFI_HASH_PROTOCOL not installed yet
  @return EFI_UNSUPPORTED            Function called in runtime phase

**/
EFI_STATUS
EFIAPI
FdmGetAt (
  IN CONST UINT8   Instance,
  OUT EFI_GUID     *RegionType,
  OUT UINT8        *RegionId,
  OUT UINT64       *RegionOffset,
  OUT UINT64       *RegionSize,
  OUT UINT32       *Attribs
  ){

  UINT8                         *EndPtr;
  UINT8                         InstanceNotIgnored;
  UINT32                        EntrySize;
  H2O_FLASH_DEVICE_MAP_ENTRY    *Entry;
  UINT8                         *Fdm;

  if (mIsAtRuntime){
    return EFI_UNSUPPORTED;
  }
  //
  // Instance number start from 1
  //
  if (Instance == 0){
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get FDM by DynamicEx Pcd
  //
  Fdm = (UINT8*)(UINTN) mFlashDeviceMapStart;
  if ((Fdm == NULL) || CompareMem(Fdm, mSignature, sizeof(mSignature))){
    return EFI_NOT_FOUND;
  }

  if (!IsFdmHeaderValid ((H2O_FLASH_DEVICE_MAP_HEADER *) Fdm)){
    return EFI_SECURITY_VIOLATION;
  }

  EntrySize = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->EntrySize;
  EndPtr    = Fdm + ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Size;
  for (InstanceNotIgnored = 0, Entry = (H2O_FLASH_DEVICE_MAP_ENTRY*) (Fdm + ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Offset);
       ;
       Entry = (H2O_FLASH_DEVICE_MAP_ENTRY*)(((UINT8*)Entry) + EntrySize)){

    if (Entry >= (H2O_FLASH_DEVICE_MAP_ENTRY*)EndPtr){
      return EFI_NOT_FOUND;
    }
    if ((Entry->Attribs & H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_IGNORE) != H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_IGNORE){
      InstanceNotIgnored++;
    }
    if(InstanceNotIgnored == Instance) {
      break;
    }
  }

  CopyMem (RegionType, &Entry->RegionType, sizeof(Entry->RegionType));
  CopyMem (RegionId, Entry->RegionId, FDM_ENTRY_REGION_ID_SIZE);
  *RegionOffset = Entry->RegionOffset;
  *RegionSize   = Entry->RegionSize;
  *Attribs      = Entry->Attribs;

  return EFI_SUCCESS;

}


/**
  Clone FDM into RuntimeServiceData or SMRAM

  @param  FlashDeviceMapStart     Pointer to FDM address in UINT64 value.

  @retval EFI_SUCCESS             Clone FDM successfully.
  @return Others                  Some error occurs during the execution of this function.

**/
STATIC
EFI_STATUS
CloneFdm (
 IN OUT UINT64       *FlashDeviceMapStart
){

  EFI_INSTALL_PROTOCOL_INTERFACE           InstallProtocolInterface;
  EFI_LOCATE_PROTOCOL                      LocateProtocolInterface;
  EFI_ALLOCATE_POOL                        EfiAllocatePool;
  VOID                                     *FdmBuffer;
  EFI_STATUS                               Status;
  EFI_HANDLE                               Handle;

  if (mInSmm){
    LocateProtocolInterface  = mSmst->SmmLocateProtocol;
    InstallProtocolInterface = mSmst->SmmInstallProtocolInterface;
    EfiAllocatePool          = mSmst->SmmAllocatePool;
  } else {
    LocateProtocolInterface  = gBS->LocateProtocol;
    InstallProtocolInterface = gBS->InstallProtocolInterface;
    EfiAllocatePool          = gBS->AllocatePool;
  }

  Status = LocateProtocolInterface (
                    &mAlternateFdmBinaryProtocolGuid,
                    NULL,
                    &FdmBuffer
                    );
  if (!EFI_ERROR(Status)){
    *FlashDeviceMapStart = (UINT64)(UINTN)FdmBuffer;
    return Status;
  }

  Status = EfiAllocatePool (
                    EfiRuntimeServicesData,
                    ((H2O_FLASH_DEVICE_MAP_HEADER*)(UINTN)(*FlashDeviceMapStart))->Size,
                    &FdmBuffer
                    );
  if (EFI_ERROR(Status)){
    ASSERT (FALSE);
    return Status;
  }

  CopyMem ((VOID*)FdmBuffer,
           (VOID*)(UINTN)(*FlashDeviceMapStart),
           ((H2O_FLASH_DEVICE_MAP_HEADER*)(UINTN)(*FlashDeviceMapStart))->Size
           );
  *FlashDeviceMapStart = (UINT64)(UINTN)FdmBuffer;
  Handle = NULL;
  Status = InstallProtocolInterface (
                    &Handle,
                    &mAlternateFdmBinaryProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    FdmBuffer
                    );
  return Status;
}


/**
  Register ExitBootService Callback for indicate not supported for run time.

  @param  ImageHandle             ImageHandle of the loaded driver.
  @param  SystemTable             Pointer to the EFI System Table.

  @return  EFI_SUCCESS            Register successfully.
  @return  EFI_NOT_FOUND          Can not get correct FDM address.  

**/
EFI_STATUS
EFIAPI
DxeFlashRegionLibInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
){

  EFI_STATUS                 Status;
  EFI_SMM_BASE2_PROTOCOL     *SmmBase;
  EFI_PEI_HOB_POINTERS       Hob;
  UINT64                     *FdmRegionAddress;


  Hob.Raw  = GetFirstGuidHob (&mFdmContentHobGuid);
  if (Hob.Raw != NULL) {
    mFlashDeviceMapStart = (UINT64)(UINTN)GET_GUID_HOB_DATA (Hob.Raw);
  } else if (GetBootModeHob() == BOOT_IN_RECOVERY_MODE){
    Hob.Raw  = GetFirstGuidHob (&mFdmAddressHobGuid);
    if (Hob.Raw == NULL) {
      mFlashDeviceMapStart = PcdGet64(PcdH2OFlashDeviceMapStart);
    } else {
      FdmRegionAddress = GET_GUID_HOB_DATA (Hob);
      mFlashDeviceMapStart = *FdmRegionAddress;
    }
  } else {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  //
  // SMM check
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    Status = SmmBase->InSmm (SmmBase, &mInSmm);
  } else {
    mInSmm = FALSE;
  }

  if (!mInSmm) {
    //
    // Register the event to declare not support for runtime.
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    ExitBootServiceCallback,
                    NULL,
                    &gEfiEventExitBootServicesGuid,
                    &mExitBootServiceEvent
                    );
  } else {
    //
    // Get Smm Syatem Table
    //
    Status = SmmBase->GetSmstLocation(
                        SmmBase,
                        &mSmst
                        );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  //
  // Clone FDM into RuntimeServiceData or SMRAM
  //
  Status = CloneFdm (&mFlashDeviceMapStart);

  return EFI_SUCCESS;
}


/**
  Close registered ExitBootService event

  @param  ImageHandle             ImageHandle of the loaded driver.
  @param  SystemTable             Pointer to the EFI System Table.

  @return  EFI_SUCCESS            Close successfully.

**/
EFI_STATUS
EFIAPI
DxeFlashRegionLibDestruct (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
){

  EFI_STATUS    Status;

  if (mExitBootServiceEvent != NULL){
    Status = gBS->CloseEvent (mExitBootServiceEvent);
  }

  return EFI_SUCCESS;

}



EFI_STATUS
EFIAPI
FdmGetNAtWorker (
  IN  CONST EFI_GUID  *RegionType,
  IN  UINT8           Instance,
  OUT UINT64          *Address,
  OUT UINT64          *Size
){

  UINT8   Id[FDM_ENTRY_REGION_ID_SIZE];
  UINT64  Offset;
  UINT64  RegionSize;
  UINT32  Attr;

  if (mIsAtRuntime){
    return EFI_UNSUPPORTED;
  }

  if (EFI_ERROR (FdmGetNAt (RegionType, Instance, Id, &Offset, &RegionSize, &Attr))){
    return EFI_NOT_FOUND;
  }

  *Address = Offset + FdmGetBaseAddr ();
  *Size = RegionSize;
  return EFI_SUCCESS;

}


/**
  Get Address of Region recorded in FDM

  @param[in]   RegionType            GUID that specifies the type of region
  @param[in]   Instance              Unsigned integer that specifies entry instance of Region Type in FDM

  @return      address of the Region.

**/
UINT64
EFIAPI
FdmGetNAtAddr (
  IN CONST EFI_GUID  *RegionType,
  IN UINT8           Instance
){

  UINT64  Address;
  UINT64  RegionSize;

  if (EFI_ERROR (FdmGetNAtWorker (RegionType, Instance, &Address, &RegionSize))){
    return 0;
  }

  return Address;

}


/**
  Get Size of Region recorded in FDM

  @param[in]   RegionType            GUID that specifies the type of region
  @param[in]   Instance              Unsigned integer that specifies entry instance of Region Type in FDM

  @return      size of the Region.

**/
UINT64
EFIAPI
FdmGetNAtSize (
  IN CONST EFI_GUID  *RegionType,
  IN UINT8           Instance
){

  UINT64  Address;
  UINT64  RegionSize;

  if (EFI_ERROR (FdmGetNAtWorker (RegionType, Instance, &Address, &RegionSize))){
    return 0;
  }

  return RegionSize;

}


EFI_STATUS
EFIAPI
FdmGetByIdWorker (
  IN CONST EFI_GUID  *RegionType,
  IN CONST EFI_GUID  *Id,
  IN  UINT8          Instance,
  OUT UINT64         *Address,
  OUT UINT64         *Size
){
  UINT8          MaxRegion;
  UINT8          Index;
  UINT8          Matched;
  EFI_STATUS     Status;
  UINT8          RegionId[FDM_ENTRY_REGION_ID_SIZE];
  UINT64         RegionOffset;
  UINT64         RegionSize;
  UINT32         Attribs;


  if (mIsAtRuntime){
    return EFI_UNSUPPORTED;
  }

  Status = FdmGetCount (&MaxRegion);

  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Matched = 0;

  for (Index = 1; Index < MaxRegion + 1; Index++){
    Status = FdmGetNAt (
               RegionType,
               Index,
               RegionId,
               &RegionOffset,
               &RegionSize,
               &Attribs
               );
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }

    if (!CompareMem(RegionId, Id, sizeof(EFI_GUID))){
      Matched++;
    }

    if (Matched == Instance) {
      *Address = RegionOffset + FdmGetBaseAddr ();
      *Size = RegionSize;
      return EFI_SUCCESS;
    }

  }

  return EFI_NOT_FOUND;
}

/**
  Get address of Region recorded in FDM by specific Id and region type.

  @param[in]   RegionType            GUID that specifies the type of region.
  @param[in]   Id                    Pointer of value that specifies the Id field must matched.
  @param[in]   Instance              Unsigned integer that specifies instance of Region Type in FDM

  @return      address of the Region.

**/
UINT64
EFIAPI
FdmGetAddressById (
  IN CONST EFI_GUID  *RegionType,
  IN CONST EFI_GUID  *Id,
  IN UINT8           Instance
  ){

  EFI_STATUS     Status;
  UINT64         RegionAddress;
  UINT64         RegionSize;

  Status = FdmGetByIdWorker (
             RegionType,
             Id,
             Instance,
             &RegionAddress,
             &RegionSize
             );

  if (EFI_ERROR(Status)){
    return 0;
  }

  return RegionAddress;

}


/**
  Get size of Region recorded in FDM by specific Id and region type.

  @param[in]   RegionType            GUID that specifies the type of region.
  @param[in]   Id                    Pointer of value that specifies the Id field must matched.
  @param[in]   Instance              Unsigned integer that specifies instance of Region Type in FDM

  @return      size of the Region.

**/
UINT64
EFIAPI
FdmGetSizeById (
  IN CONST EFI_GUID  *RegionType,
  IN CONST EFI_GUID  *Id,
  IN  UINT8          Instance
  ){

  EFI_STATUS     Status;
  UINT64         RegionAddress;
  UINT64         RegionSize;

  Status = FdmGetByIdWorker (
             RegionType,
             Id,
             Instance,
             &RegionAddress,
             &RegionSize
             );

  if (EFI_ERROR(Status)){
    return 0;
  }

  return RegionSize;

}




/**
  Get Flash Size

  @return    Flash Size

**/
UINT64
EFIAPI
FdmGetFlashAreaSize (
  VOID
){

  UINT64  FlashSize;
  UINT8   Index;
  UINT8   MaxRegion;
  EFI_STATUS              Status;
  UINT8                   RegionId[FDM_ENTRY_REGION_ID_SIZE];
  UINT64                  RegionOffset;
  UINT64                  RegionSize;
  UINT32                  Attribs;
  EFI_GUID                RegionType;

  FlashSize = 0;

  Status = FdmGetCount (&MaxRegion);

  if (EFI_ERROR (Status)) {
    return 0;
  }

  for (Index = 1; Index < MaxRegion + 1; Index++){

    Status = FdmGetAt (
               Index,
               &RegionType,
               RegionId,
               &RegionOffset,
               &RegionSize,
               &Attribs
               );

    if (EFI_ERROR (Status)) {
      return 0;
    }
    FlashSize += RegionSize;
  }

 return FlashSize;

}


/**
  Get the count of secific Region in FDM.

  @param[in]   RegionType       GUID that specifies the type of region that search for.
  @param[out]  Count            Unsigned integer that specifies the count of secific Region in FDM.

  @return EFI_SUCCESS           Get region count success.
  @return EFI_NOT_FOUND         Can't find region type in the FDM.
  @return EFI_UNSUPPORTED       Function called in runtime phase

**/
EFI_STATUS
EFIAPI
FdmGetNCount (
  IN CONST EFI_GUID  *RegionType,
  OUT UINT8          *Count
){
  UINT8                         *EndPtr;
  UINT32                        FirstEntryOffset;
  UINT32                        EntrySize;
  UINT32                        Size;
  H2O_FLASH_DEVICE_MAP_ENTRY    *Entry;
  UINT8                         *Fdm;

  if (mIsAtRuntime){
    return EFI_UNSUPPORTED;
  }

  *Count = 0;

  //
  // Get FDM by DynamicEx Pcd
  //
  Fdm = (UINT8*)(UINTN) mFlashDeviceMapStart;

  if ((Fdm == NULL) || CompareMem(Fdm, mSignature, sizeof(mSignature))){
    return EFI_NOT_FOUND;
  }

  if (!IsFdmHeaderValid ((H2O_FLASH_DEVICE_MAP_HEADER *) Fdm)){
    return EFI_SECURITY_VIOLATION;
  }

  FirstEntryOffset = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Offset;
  Size         = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->Size;
  EntrySize    = ((H2O_FLASH_DEVICE_MAP_HEADER*)Fdm)->EntrySize;
  Entry        = (H2O_FLASH_DEVICE_MAP_ENTRY*) (Fdm + FirstEntryOffset);

  EndPtr = Fdm + Size;

  for (; (UINT8*)Entry < EndPtr;  Entry = (H2O_FLASH_DEVICE_MAP_ENTRY*)(((UINT8*)Entry) + EntrySize)){
    if (!CompareMem(&Entry->RegionType, RegionType, sizeof(EFI_GUID))){
      (*Count)++;
    }
  }

  if (*Count == 0){
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;

}

/**
  Internal function to verify the FV region by checking the hash value in FDM by region type
  ,ID and instance number.
  Also invoke gH2OBaseCpVerifyFvGuid checkpoint after verifying region. It may enter deadloop
  if verify failed.

  @param[in]  RegionType          EFI_GUID that specifies the type of region.
  @param[in]  Id                  Pointer of value that specifies the Id field must matched.
  @param[in]  Instance            Unsigned integer that specifies entry instance of FDM

  @retval EFI_SUCCESS             Region verified successfully.
  @retval EFI_NOT_FOUND           Can't find region type in the FDM.
  @retval EFI_SECURITY_VIOLATION  Region verified failed.

**/
STATIC
EFI_STATUS
FdmVerifyRegion (
  IN CONST EFI_GUID  *RegionType,    OPTIONAL
  IN CONST EFI_GUID  *Id,            OPTIONAL
  IN UINT8           Instance
  )
{

  UINT8                        EntryFormat;
  UINT8                        *Hash;
  UINT8                        InstanceNotIgnored;
  UINT32                       EntrySize;
  H2O_FLASH_DEVICE_MAP_ENTRY   *Entries;
  H2O_FLASH_DEVICE_MAP_ENTRY   *Entry;
  H2O_FLASH_DEVICE_MAP_ENTRY   *EndPtr;
  H2O_FLASH_DEVICE_MAP_HEADER  *Fdm;
  EFI_STATUS                   Status;


  Fdm                = (H2O_FLASH_DEVICE_MAP_HEADER*)(UINTN) mFlashDeviceMapStart;
  EntrySize          = Fdm->EntrySize;
  EntryFormat        = Fdm->EntryFormat;
  Entries            = (H2O_FLASH_DEVICE_MAP_ENTRY *)(((UINT8 *)Fdm) + Fdm->Offset);
  EndPtr             = (H2O_FLASH_DEVICE_MAP_ENTRY *)(((UINT8 *)Fdm) + Fdm->Size);
  InstanceNotIgnored = 0;

  Status = EFI_NOT_FOUND;
  for (Entry = Entries;
       Entry < EndPtr;
       Entry = (H2O_FLASH_DEVICE_MAP_ENTRY*)(((UINT8*)Entry) + EntrySize)) {
    if ((Entry->Attribs & H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_IGNORE) != H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_IGNORE &&
        (RegionType == NULL || CompareGuid (RegionType, &Entry->RegionType)) &&
        (Id == NULL || !CompareMem (Id, Entry->RegionId, sizeof(EFI_GUID)))) {
      InstanceNotIgnored++;
    }
    if(InstanceNotIgnored == Instance) {
      if ((Entry->Attribs & H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_MUTABLE) == H2O_FLASH_DEVICE_MAP_ENTRY_ATTRIB_MUTABLE) {
        Status = EFI_SUCCESS;
        break;
      }
      Hash = (UINT8*)(Entry + 1);
      PERF_START (0, "DxeCheckRgn", NULL, 0);
      Status = VerifyHash (
                 EntryFormat,
                 Hash,
                 (UINT8*)(UINTN)(Fdm->FdBaseAddr + Entry->RegionOffset),
                 (UINTN)Entry->RegionSize
                 );
      PERF_END (0, "DxeCheckRgn", NULL, 0);
      if (EFI_ERROR (Status)) {
        Status = EFI_SECURITY_VIOLATION;
      }
      break;
    }
  }

  if (FeaturePcdGet (PcdH2OBaseCpVerifyFvSupported)) {
    H2O_BASE_CP_VERIFY_FV_DATA      VerifyFvData;

    VerifyFvData.Size     = sizeof (H2O_BASE_CP_VERIFY_FV_DATA);
    VerifyFvData.Status   = H2O_CP_TASK_NORMAL;
    VerifyFvData.FvBase   = Fdm->FdBaseAddr + Entry->RegionOffset;
    VerifyFvData.FvLength = Entry->RegionSize;
    VerifyFvData.Trusted  = !EFI_ERROR (Status) ? TRUE : FALSE;
    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBaseCpVerifyFvGuid));
    H2OCpTrigger (&gH2OBaseCpVerifyFvGuid, &VerifyFvData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", VerifyFvData.Status));
    if (VerifyFvData.Status == H2O_BDS_TASK_SKIP) {
      return Status;
    } else if (VerifyFvData.Status == H2O_BDS_TASK_UPDATE) {
      Status = VerifyFvData.Trusted ? EFI_SUCCESS : EFI_SECURITY_VIOLATION;
    }
  }

  if (Status == EFI_SECURITY_VIOLATION) {
    POST_CODE (0xDE);
    DEBUG((DEBUG_ERROR, "ERROR!!! Firmware Volume Base Address: 0x%lx, Size: 0x%lx is corrupted.\n", FdmGetNAtAddr (RegionType, Instance), FdmGetNAtSize (RegionType, Instance)));
    ASSERT (FALSE);
    CpuDeadLoop ();
  }
  return Status;
}

/**
  Verify the FV region by checking the hash value in FDM by instance number.

  NOTE: This function will hang if the region has a security issue with the deadloop.

  @param[in]  Instance             Unsigned integer that specifies entry instance of FDM

  @retval EFI_SUCCESS              Region verified successfully.
  @retval EFI_NOT_FOUND            Can't find region type in the FDM.
  @retval EFI_SECURITY_VIOLATION   Region verified failed.

**/
EFI_STATUS
EFIAPI
FdmVerifyAt (
  IN CONST UINT8   Instance
  )
{
  return FdmVerifyRegion (NULL, NULL, Instance);
}

/**
  Verify the FV region by checking the hash value in FDM by region type and instance number.

  NOTE: This function will hang if the region has a security issue with the deadloop.

  @param[in]  RegionType          EFI_GUID that specifies the type of region.
  @param[in]  Instance            Unsigned integer that specifies entry instance of FDM

  @retval EFI_SUCCESS             Region verified successfully.
  @retval EFI_NOT_FOUND           Can't find region type in the FDM.
  @retval EFI_SECURITY_VIOLATION  Region verified failed.

**/
EFI_STATUS
EFIAPI
FdmVerifyNAt (
  IN CONST  EFI_GUID  *RegionType,
  IN CONST  UINT8     Instance
  )
{
  return FdmVerifyRegion (RegionType, NULL, Instance);
}

/**
  Verify the FV region by checking the hash value in FDM by region type , ID and instance number.

  NOTE: This function will hang if the region has a security issue with the deadloop.

  @param[in]  RegionType          EFI_GUID that specifies the type of region.
  @param[in]  Id                  Pointer of value that specifies the Id field must matched.
  @param[in]  Instance            Unsigned integer that specifies entry instance of FDM

  @retval EFI_SUCCESS             Region verified successfully.
  @retval EFI_NOT_FOUND           Can't find region type in the FDM.
  @retval EFI_SECURITY_VIOLATION  Region verified failed.

**/
EFI_STATUS
EFIAPI
FdmVerifyById (
  IN CONST EFI_GUID  *RegionType,
  IN CONST EFI_GUID  *Id,
  IN UINT8           Instance
  )
{
  return FdmVerifyRegion (RegionType, Id, Instance);
}