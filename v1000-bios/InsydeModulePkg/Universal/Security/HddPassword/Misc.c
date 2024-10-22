/** @file
  Misc. function in HDD Password Protocol

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

HDD_PASSWORD_PRIVATE             *mHddPasswordPrivate;
DRIVER_INSTALL_INFO              *mDriverInstallInfo;
MEMORY_PAGE_TABLE                *mMemoryBuffer;

CONTROLER_CLASS_CODE_LIST mClassCodeList[] = {
  {PCI_CLASS_MASS_STORAGE, PCI_SUB_CLASS_NVME, PCI_IF_NVMHCI},
  {PCI_CLASS_MASS_STORAGE, PCI_SUB_CLASS_AHCI, PCI_IF_AHCI},
  {PCI_CLASS_MASS_STORAGE, PCI_SUB_CLASS_RAID, PCI_IF_RAID},
  {PCI_CLASS_SD_HC, PCI_SUB_CLASS_SDHC, PCI_IF_SD}
};

//
// DelayUefiRaid should always listed before other drivers because it will
// register passthru related callbacks which produce by others
//
AGENT_FILE_GUID_LIST mAgentFileGuidList[] = {
  {0x13863F79, 0xD94B, 0x4205, 0xBB, 0x0F, 0xE4, 0xE0, 0x6A, 0xAA, 0x5A, 0x4E}, // DelayUefiRaid
  {0xBB65942B, 0x521F, 0x4ec3, 0xBA, 0xF9, 0xA9, 0x25, 0x40, 0xCF, 0x60, 0xD2}, // SataController INTEL
  {0x0325B5A1, 0x0937, 0x4A4F, 0xB8, 0xAF, 0xEC, 0x3F, 0x80, 0xEE, 0x6B, 0x35}, // SataController AMD
  {0x3ACC966D, 0x8E33, 0x45c6, 0xb4, 0xfe, 0x62, 0x72, 0x4B, 0xCD, 0x15, 0xA9}, // AhciBusDxe
  {0x5BE3BDF4, 0x53CF, 0x46a3, 0xA6, 0xA9, 0x73, 0xC3, 0x4A, 0x6E, 0x5E, 0xE3}, // NVME
  {0x67BBC344, 0x84BC, 0x4e5c, 0xb4, 0xDF, 0xF5, 0xE4, 0xA0, 0x0E, 0x1F, 0x3A}, // SD
  {0x70D57D67, 0x7F05, 0x494d, 0xA0, 0x14, 0xB7, 0x5D, 0x73, 0x45, 0xB7, 0x00}  // SSCP
};

/**
  Initializes pre-allocated memory.

  @param[out]  MemoryPageTable      Pointer to memory page table.

  @retval EFI_SUCCESS  Successful initialization.
  @retval Others       Initialization failed.

**/
EFI_STATUS
InitializeMemoryBuffer (
  OUT MEMORY_PAGE_TABLE     **MemoryPageTable
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  VOID                      *Buffer;
  MEMORY_PAGE_TABLE         *PageTable;

  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  (sizeof (MEMORY_PAGE_TABLE) + (sizeof (MEMORY_PAGE_ENTRY) * (MIN_REQUIRED_BLOCKS - 1))),
                  (VOID **)&PageTable
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Pre-allocates memory space
  //
  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  MEMORY_PREALLOCATE_SIZE,
                  &Buffer
                  );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (PageTable);
    return Status;
  }

  //
  // Initialize Internal Page Table for Memory Management
  //
  SetMem (Buffer, MEMORY_PREALLOCATE_SIZE, 0xFF);
  SetMem (PageTable, (sizeof (MEMORY_PAGE_TABLE) + (sizeof (MEMORY_PAGE_ENTRY) * (MIN_REQUIRED_BLOCKS - 1))), 0);

  PageTable->PageCount           = MEMORY_PREALLOCATE_SIZE / PAGE_BLOCK_SIZE;
  PageTable->LastEmptyPageOffset = 0x0;

  for (Index = 0; Index < PageTable->PageCount; Index++) {
    PageTable->Pages[Index].PageFlag        = PAGE_FREE;
    PageTable->Pages[Index].StartPageOffset = 0;
  }

  PageTable->DataAreaBase = Buffer;

  *MemoryPageTable = PageTable;

  return EFI_SUCCESS;
}


/**
  Look-up Free memory Region for object allocation.

  @param[in]  AllocationSize  Bytes to be allocated.

  @return  Return available page offset for object allocation.

**/
UINTN
LookupFreeMemRegion (
  IN  UINTN  AllocationSize
  )
{
  UINTN  StartPageIndex;
  UINTN  Index;
  UINTN  SubIndex;
  UINTN  ReqPages;
  MEMORY_PAGE_TABLE      *PageTable;

  PageTable = mMemoryBuffer;

  StartPageIndex = SIZE_TO_PAGES (PageTable->LastEmptyPageOffset);
  ReqPages       = SIZE_TO_PAGES (AllocationSize);

  //
  // Look up the free memory region from the beginning of the memory table
  // until the StartCursorOffset
  //
  for (Index = 0; Index < (StartPageIndex - ReqPages); ) {
    //
    // Check Consecutive ReqPages Pages.
    //
    for (SubIndex = 0; SubIndex < ReqPages; SubIndex++) {
      if ((PageTable->Pages[SubIndex + Index].PageFlag & PAGE_USED) != 0) {
        break;
      }
    }

    if (SubIndex == ReqPages) {
      //
      // Succeed! Return the Starting Offset.
      //
      return PAGES_TO_SIZE (Index);
    }

    //
    // Failed! Skip current adjacent Used pages
    //
    while ((SubIndex < (StartPageIndex - ReqPages)) &&
           ((PageTable->Pages[SubIndex + Index].PageFlag & PAGE_USED) != 0)) {
      SubIndex++;
    }

    Index += SubIndex;
  }

  //
  // Look up the free memory region with in current memory map table.
  //
  for (Index = StartPageIndex; Index <= (PageTable->PageCount - ReqPages); ) {
    //
    // Check consecutive ReqPages pages.
    //
    for (SubIndex = 0; SubIndex < ReqPages; SubIndex++) {
      if ((PageTable->Pages[SubIndex + Index].PageFlag & PAGE_USED) != 0) {
        break;
      }
    }

    if (SubIndex == ReqPages) {
      //
      // Succeed! Return the Starting Offset.
      //
      return PAGES_TO_SIZE (Index);
    }

    //
    // Failed! Skip current free memory pages and adjacent Used pages
    //
    while ((PageTable->Pages[SubIndex + Index].PageFlag & PAGE_USED) != 0) {
      SubIndex++;
    }

    Index += SubIndex;
  }

  //
  // No availabe region for object allocation!
  //
  return (UINTN)(-1);
}


/**
  Allocates a buffer at runtime phase.

  @param[in]  AllocationSize    Bytes to be allocated.

  @return  A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
AllocateBuffer (
  IN  UINTN  AllocationSize
  )
{
  UINT8  *AllocPtr;
  UINTN  ReqPages;
  UINTN  Index;
  UINTN  StartPage;
  UINTN  AllocOffset;
  EFI_STATUS                Status;
  MEMORY_PAGE_TABLE      *PageTable;

  PageTable = mMemoryBuffer;

  if (mSmst2 != NULL) {
    Status = mSmst2->SmmAllocatePool (EfiRuntimeServicesData, AllocationSize, (VOID **)&AllocPtr);
    if (EFI_ERROR (Status)) {
      return NULL;
    }
    ZeroMem (AllocPtr, AllocationSize);
    return AllocPtr;
  }

  AllocPtr = NULL;
  ReqPages = 0;

  //
  // Look for available consecutive memory region starting from LastEmptyPageOffset.
  // If no proper memory region found, look up from the beginning.
  // If still not found, return NULL to indicate failed allocation.
  //
  AllocOffset = LookupFreeMemRegion (AllocationSize);
  if (AllocOffset == (UINTN)(-1)) {
    return NULL;
  }

  //
  // Allocates consecutive memory pages with length of Size. Update the page
  // table status. Returns the starting address.
  //
  ReqPages  = SIZE_TO_PAGES (AllocationSize);
  AllocPtr  = PageTable->DataAreaBase + AllocOffset;
  StartPage = SIZE_TO_PAGES (AllocOffset);
  Index     = 0;
  while (Index < ReqPages) {
    PageTable->Pages[StartPage + Index].PageFlag       |= PAGE_USED;
    PageTable->Pages[StartPage + Index].StartPageOffset = AllocOffset;

    Index++;
  }

  PageTable->LastEmptyPageOffset = AllocOffset + PAGES_TO_SIZE (ReqPages);

  ZeroMem (AllocPtr, AllocationSize);
  //
  // Returns a void pointer to the allocated space
  //
  return AllocPtr;
}


/**
  Frees a buffer that was previously allocated at runtime phase.

  @param[in]  Buffer  Pointer to the buffer to free.

**/
VOID
FreeBuffer (
  IN  VOID  *Buffer
  )
{
  UINTN  StartOffset;
  UINTN  StartPageIndex;
  MEMORY_PAGE_TABLE      *PageTable;

  PageTable = mMemoryBuffer;

  if (mSmst2 != NULL) {
    mSmst2->SmmFreePool (Buffer);
    return;
  }

  StartOffset    = (UINTN) ((UINT8 *)Buffer - PageTable->DataAreaBase);
  StartPageIndex = SIZE_TO_PAGES (PageTable->Pages[SIZE_TO_PAGES(StartOffset)].StartPageOffset);

  while (StartPageIndex < PageTable->PageCount) {
    if (((PageTable->Pages[StartPageIndex].PageFlag & PAGE_USED) != 0) &&
        (PageTable->Pages[StartPageIndex].StartPageOffset == StartOffset)) {
        //
        // Free this page
        //
        PageTable->Pages[StartPageIndex].PageFlag       &= ~PAGE_USED;
        PageTable->Pages[StartPageIndex].PageFlag       |= PAGE_FREE;
        PageTable->Pages[StartPageIndex].StartPageOffset = 0;

        StartPageIndex++;
    } else {
      break;
    }
  }

  return;
}

/**
  Creates a new copy of an existing device path.

  This function allocates space for a new copy of the device path specified by DevicePath.
  If DevicePath is NULL, then NULL is returned.  If the memory is successfully
  allocated, then the contents of DevicePath are copied to the newly allocated
  buffer, and a pointer to that buffer is returned.  Otherwise, NULL is returned.
  The memory for the new device path is allocated from EFI boot services memory.
  It is the responsibility of the caller to free the memory allocated.

  @param[in]  DevicePath    A pointer to a device path data structure.

  @retval NULL          DevicePath is NULL or invalid.
  @retval Others        A pointer to the duplicated device path.

**/
EFI_DEVICE_PATH_PROTOCOL *
HddPasswordDuplicateDevicePath (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  UINTN                     Size;
  VOID                      *Memory;

  //
  // Compute the size
  //
  Size = GetDevicePathSize (DevicePath);
  if (Size == 0) {
    return NULL;
  }

  //
  // Allocate space for duplicate device path
  //
  Memory = AllocateBuffer ((UINTN)Size);
  if (Memory == NULL) {
    return NULL;
  }

  return (EFI_DEVICE_PATH_PROTOCOL*)CopyMem (Memory, DevicePath, Size);
}

/**
  Initialize the communicate buffer using DataSize and Function.

  @param[out]      DataPtr          Points to the data in the communicate buffer.
  @param[in]       DataSize         The data size to send to SMM.
  @param[in]       Function         The function number to initialize the communicate header.

  @retval The communicate buffer. Caller should free it after use.

**/
VOID *
InitCommunicateBuffer (
     OUT  VOID                              **DataPtr OPTIONAL,
  IN      UINTN                             DataSize,
  IN      UINTN                             Function
  )
{
  EFI_SMM_COMMUNICATE_HEADER                *SmmCommunicateHeader;
  HDD_PASSWORD_SMM_PARAMETER_HEADER         *SmmFunctionHeader;

  SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)mCommBuffer;
  CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gEfiHddPasswordServiceProtocolGuid);
  SmmCommunicateHeader->MessageLength = OFFSET_OF (HDD_PASSWORD_SMM_PARAMETER_HEADER, Data) + DataSize;

  SmmFunctionHeader = (HDD_PASSWORD_SMM_PARAMETER_HEADER *) SmmCommunicateHeader->Data;

  //
  // For security check in SMM
  //
  ZeroMem (SmmFunctionHeader, SmmCommunicateHeader->MessageLength);
  SmmFunctionHeader->Function     = Function;
  SmmFunctionHeader->ReturnStatus = 0;
  if (DataPtr != NULL) {
    *DataPtr = SmmFunctionHeader->Data;
  }

  return mCommBuffer;
}

/**
  According Bus, Device, Function, PrimarySecondary, SlaveMaster to get corresponding
  SATA port number

  @param[in]        Bus                 PCI bus number
  @param[in]        Device              PCI device number
  @param[in]        Function            PCI function number
  @param[in]        PrimarySecondary    primary or scondary
  @param[in]        SlaveMaster         slave or master
  @param[out]       PortNum             output port number

  @retval           EFI_SUCCESS         Get corresponding port number successfully
  @retval           EFI_NOT_FOUND       Can't get corresponding port number
**/
EFI_STATUS
ChangeChannelDevice2PortNum (
  IN     UINT32                         Bus,
  IN     UINT32                         Device,
  IN     UINT32                         Function,
  IN     UINT8                          PrimarySecondary,
  IN     UINT8                          SlaveMaster,
  OUT    UINTN                          *PortNum
  )
{
  UINTN                                 Index;
  PORT_NUMBER_MAP                       *PortMappingTable;
  PORT_NUMBER_MAP                       EndEntry;
  UINTN                                 NoPorts;

  if (PortNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  NoPorts = 0;
  PortMappingTable   = NULL;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  while (CompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    NoPorts++;
  }

  if (NoPorts == 0) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < NoPorts; Index++) {
    if ((PortMappingTable[Index].Bus == Bus) &&
        (PortMappingTable[Index].Device == Device) &&
        (PortMappingTable[Index].Function == Function) &&
        (PortMappingTable[Index].PrimarySecondary == PrimarySecondary) &&
        (PortMappingTable[Index].SlaveMaster == SlaveMaster)) {
      *PortNum = PortMappingTable[Index].PortNum;
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

/**
  According the Bus, Device, Function to check this controller is in Port Number Map table or not.
  If yes, then this is a on board PCI device.

  @param[in]        Bus                 PCI bus number
  @param[in]        Device              PCI device number
  @param[in]        Function            PCI function number

  @retval           TRUE                Is on board device
  @retval           FALSE               Not on board device
**/
BOOLEAN
IsOnBoardPciDevice (
  IN     UINT32                         Bus,
  IN     UINT32                         Device,
  IN     UINT32                         Function
  )
{
  UINTN                                 Index;
  PORT_NUMBER_MAP                       *PortMappingTable;
  PORT_NUMBER_MAP                       EndEntry;
  UINTN                                 NumOfPorts;

  PortMappingTable      = NULL;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  NumOfPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NumOfPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    NumOfPorts++;
  }

  if (NumOfPorts == 0) {
    return FALSE;
  }

  for (Index = 0; Index < NumOfPorts; Index++) {
    if ((PortMappingTable[Index].Bus == Bus) &&
        (PortMappingTable[Index].Device == Device) &&
        (PortMappingTable[Index].Function == Function)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Copy SourceBufferPtr string to DescBufferPtr.

  @param[in]        DescBufferPtr       Destination buffer address.
  @param[in]        SourceBufferPtr     Source buffer addess.

  @retval           EFI_SUCCESS
**/
EFI_STATUS
GetModelNumber (
  IN VOID                               *DescBufferPtr,
  IN VOID                               *SourceBufferPtr
  )
{
  UINT8                                 *IdentifyPtr;
  UINT16                                *HddInfoPtr;
  UINT16                                Index;
  UINT32                                *SwapPtr;
  UINTN                                 DwordConunt;

  IdentifyPtr = SourceBufferPtr;
  HddInfoPtr  = DescBufferPtr;
  SwapPtr     = DescBufferPtr;

  for (Index = 0; Index < MODEL_NUMBER_LENGTH; Index++) {
    HddInfoPtr[Index] = (UINT16) (IdentifyPtr[Index] & 0x0ff);
  }

  //
  // MAX_SUPPORT_SERIAL_NUM_COUNT is Serial Count.
  // It's unit is CHAR16.
  //
  DwordConunt = MODEL_NUMBER_LENGTH >> 1;
  for (Index = 0; Index < DwordConunt; Index++) {
    SwapPtr[Index] = (UINT32) (((SwapPtr[Index] & 0xffff0000) >> 16) + \
                               ((SwapPtr[Index] & 0x0000ffff) << 16));
  }
  return EFI_SUCCESS;
}

/**
 Find all agent handles that support HDD Password feature.
 @param[in]        AgentFileGuidList         Pointer to AGENT_FILE_GUID_LIST.
 @param[in]        size                      size of the AGENT_FILE_GUID_LIST.

 @retval *EFI_HANDLE  A list of agent handles that discovered from file Guids
**/
EFI_HANDLE*
FindAgentHandleListByFileGUID (
AGENT_FILE_GUID_LIST*   AgentFileGuidList,
UINTN                   Size
  )
{
  EFI_HANDLE                            *FoundAgentHandleList;
  UINTN                                 HandleCount;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleIndex;
  EFI_DRIVER_BINDING_PROTOCOL           *DriverBinding;
  EFI_GUID                              *GuidPoint;
  EFI_DEVICE_PATH_PROTOCOL              *TempDevicePath;
  EFI_LOADED_IMAGE_PROTOCOL             *LoadedImage;
  UINT8                                 Index;
  UINT8                                 Index1;
  EFI_STATUS                            Status;

  Index1 = 0;

  //
  // Add one more entry to the handle list because according to UEFI spec 2.6
  // "The list is terminated by a NULL handle value."
  //
  FoundAgentHandleList = AllocateZeroPool((Size + 1) * sizeof(EFI_HANDLE));
  if (FoundAgentHandleList == NULL) {
    return NULL;
  }
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadedImageProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  //
  // find the right driverbinding handle for HDD Password
  //
  if (Status == EFI_SUCCESS && HandleBuffer != NULL) {
    for (Index = 0; Index < Size; Index++) {
      for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
        Status = gBS->HandleProtocol (
                        HandleBuffer[HandleIndex],
                        &gEfiDriverBindingProtocolGuid,
                        (VOID**) &DriverBinding
                        );
        if (EFI_ERROR(Status)) {
          continue;
        }

        Status = gBS->HandleProtocol (
                  DriverBinding->ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **) &LoadedImage
                  );
        if (EFI_ERROR(Status)) {
          continue;
        }

        TempDevicePath = LoadedImage->FilePath;
        GuidPoint = EfiGetNameGuidFromFwVolDevicePathNode (
                      (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) TempDevicePath
                      );
        if (GuidPoint == NULL) {
          DEBUG ((EFI_D_INFO, " Empty GUID found\n"));
          continue;
        }

        if (CompareGuid(&(AgentFileGuidList[Index].FileGuid), GuidPoint)) {
          DEBUG_CODE_BEGIN ();
            switch(Index) {
            case 0:
              DEBUG ((EFI_D_INFO, " DelayUefiRaid driver found\n"));
              break;

            case 1:
              DEBUG ((EFI_D_INFO, " SataController driver for INTEL found\n"));
              break;

            case 2:
              DEBUG ((EFI_D_INFO, " SataController driver for AMD found\n"));
              break;

            case 3:
              DEBUG ((EFI_D_INFO, " AhciBusDxe driver found\n"));
              break;

            case 4:
              DEBUG ((EFI_D_INFO, " NVMExpressDxe driver found\n"));
              break;

            case 5:
              DEBUG ((EFI_D_INFO, " SDHostDxe driver found\n"));
              break;

            case 6:
              DEBUG ((EFI_D_INFO, " StorageSecurityCommandProtocolDxe driver found\n"));
              break;
            default:
              break;
            }
          DEBUG_CODE_END ();

          //
          // store found Agent handles to FoundAgentHandleList in the same order as
          // mAgentFileGuidList defined.
          //
          FoundAgentHandleList[Index1] = DriverBinding->ImageHandle;
          Index1++;
        }
      }
    }
  }
  return FoundAgentHandleList;
}



/**
 Find controller handle that satisfy class code listed in .

 @retval EFI_SUCCESS  Connect SATA controller successfully.
 @retval Other        Connect SATA controller failed.
**/
BOOLEAN
SpecificControllerFound (
UINT8               *ClassCode
  )
{
  UINT8                                 EntryCount;
  UINT8                                 Index;

  EntryCount = (sizeof(mClassCodeList) / sizeof(CONTROLER_CLASS_CODE_LIST));
  for (Index = 0; Index < EntryCount; Index++) {
    if ((ClassCode[2] == mClassCodeList[Index].BaseClassCode &&
        ClassCode[1] == mClassCodeList[Index].SubClassCode)) {
      return TRUE;
    }
  }

  return FALSE;
}


/**
 Connect all Storage controller.

 @retval EFI_SUCCESS  Connect SATA controller successfully.
 @retval Other        Connect SATA controller failed.
**/
EFI_STATUS
EFIAPI
CollectStorageController (
  IN BOOLEAN                            NeedConnectController
  )
{
  EFI_STATUS                            Status;
  UINTN                                 PciIoHandleCount;
  EFI_HANDLE                            *PciIoHandleBuffer;
  UINTN                                 PciIoHandleIndex;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  UINTN                                 Device;
  UINTN                                 Seg;
  UINTN                                 Bus;
  UINTN                                 Function;
  UINT8                                 PCIClassCReg[3];
  ATA_CONTROLLER_INFO                   *AtaControllerInfo;
  DRIVER_INSTALL_INFO                   *DriverInstallInfo;
  EFI_HANDLE                            *AgentHandleBuffer;
  LIST_ENTRY                            *AtaControllerInfoListHead;
  LIST_ENTRY                            *Link;

  PciIoHandleCount  = 0;
  PciIoHandleBuffer = NULL;
  AgentHandleBuffer = NULL;
  AtaControllerInfo = NULL;
  AgentHandleBuffer = FindAgentHandleListByFileGUID(mAgentFileGuidList, sizeof(mAgentFileGuidList)/sizeof(AGENT_FILE_GUID_LIST));
  if (AgentHandleBuffer == NULL) {
    return EFI_NOT_FOUND;
  }
  DriverInstallInfo = mDriverInstallInfo;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &PciIoHandleCount,
                  &PciIoHandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  for (PciIoHandleIndex = 0; PciIoHandleIndex < PciIoHandleCount; PciIoHandleIndex++) {
    Status = gBS->HandleProtocol (
                    PciIoHandleBuffer[PciIoHandleIndex],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (Status != EFI_SUCCESS) {
      continue;
    }


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

    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint8,
                          PCI_CLASSCODE_OFFSET,
                          3,
                          PCIClassCReg
                          );
    if (Status != EFI_SUCCESS) {
      continue;
    }
    if (!SpecificControllerFound(PCIClassCReg) || !NeedConnectController) {
      continue;
    }

    //
    // don't need to connect again when controllers are being reconnected
    //
    if (!mHddPasswordPrivate->ControllerReconnected) {
      Status = gBS->ConnectController (
                      PciIoHandleBuffer[PciIoHandleIndex],
                      AgentHandleBuffer,
                      NULL,
                      TRUE
                       );
    }

    if ((FindMappedController(Seg, Bus, Device, Function) != INVALID_CONTROLLER_INDEX_NUMBER) || (EFI_ERROR(Status))) {
      continue;
    }

    //
    // only update AtaControllerInfo when reconnect
    //
    if (mHddPasswordPrivate->ControllerReconnected) {
      AtaControllerInfoListHead = &DriverInstallInfo->AtaControllerInfoListHead;
      for (Link = GetFirstNode (AtaControllerInfoListHead);
           !IsNull (AtaControllerInfoListHead, Link) ;
           Link = GetNextNode (AtaControllerInfoListHead, Link)) {
        AtaControllerInfo = (ATA_CONTROLLER_INFO *)Link;
        if (AtaControllerInfo->PciSeg == Seg &&
            AtaControllerInfo->PciBus == Bus &&
            AtaControllerInfo->PciDevice == Device &&
            AtaControllerInfo->PciFunction == Function) {
          AtaControllerInfo->AtaControllerHandle = PciIoHandleBuffer[PciIoHandleIndex];
          AtaControllerInfo->AtaMode             = PCIClassCReg[1];
          AtaControllerInfo->BaseCode            = PCIClassCReg[2];
          AtaControllerInfo->SubClassCode        = PCIClassCReg[1];
          AtaControllerInfo->PI                  = PCIClassCReg[0];
          break;
        }
      }
      //
      // when controller reconnect, don't need to collect AtaControllerInfo again
      //
      continue;
    }

    AtaControllerInfo = NULL;

    AtaControllerInfo = AllocateZeroPool(sizeof (ATA_CONTROLLER_INFO));

    if (AtaControllerInfo != NULL) {
      AtaControllerInfo->AtaControllerHandle = PciIoHandleBuffer[PciIoHandleIndex];
      AtaControllerInfo->AtaMode             = PCIClassCReg[1];
      AtaControllerInfo->PciSeg              = Seg;
      AtaControllerInfo->PciBus              = Bus;
      AtaControllerInfo->PciDevice           = Device;
      AtaControllerInfo->PciFunction         = Function;
      AtaControllerInfo->BaseCode            = PCIClassCReg[2];
      AtaControllerInfo->SubClassCode        = PCIClassCReg[1];
      AtaControllerInfo->PI                  = PCIClassCReg[0];

      InsertTailList (
        &DriverInstallInfo->AtaControllerInfoListHead,
        &AtaControllerInfo->Link
        );

      DriverInstallInfo->NumOfController++;
    }

  }


  DriverInstallInfo->AtaControllerSearched = TRUE;

  if (AgentHandleBuffer !=  NULL) {
    gBS->FreePool (AgentHandleBuffer);
  }

  if (PciIoHandleBuffer != NULL) {
    gBS->FreePool (PciIoHandleBuffer);
  }
  return Status;
}

/**
 Find mapped controller.

 @retval Others        found.
 @retval 0xFFFF        not found.
**/
UINT16
EFIAPI
FindMappedController (
  IN UINTN                                 Seg,
  IN UINTN                                 Bus,
  IN UINTN                                 Device,
  IN UINTN                                 Function
  )
{
  ATA_CONTROLLER_INFO                   *AtaControllerInfo;
  DRIVER_INSTALL_INFO                   *DriverInstallInfo;
  UINT16                                ControllerIndex;
  LIST_ENTRY                            *AtaControllerInfoListHead;
  LIST_ENTRY                            *Link;

  DriverInstallInfo = mDriverInstallInfo;

  AtaControllerInfoListHead = &DriverInstallInfo->AtaControllerInfoListHead;
  for (Link = GetFirstNode (AtaControllerInfoListHead), ControllerIndex = CONTROLLER_INDEX_START;
       !IsNull (AtaControllerInfoListHead, Link) && ControllerIndex <= DriverInstallInfo->NumOfController;
       Link = GetNextNode (AtaControllerInfoListHead, Link), ControllerIndex++) {
    AtaControllerInfo = (ATA_CONTROLLER_INFO *)Link;
    if (AtaControllerInfo->PciSeg == Seg &&
        AtaControllerInfo->PciBus == Bus &&
        AtaControllerInfo->PciDevice == Device &&
        AtaControllerInfo->PciFunction == Function) {
      return ControllerIndex;
    }

  }

  return INVALID_CONTROLLER_INDEX_NUMBER;
}

/**
  Check whether the EFI_COMPONENT_NAME_PROTOCOL instance is valid for use.

  @param[in] ComponentName      A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance
  @param[in] Handle             The handle of Block IO device

  @retval TRUE                  The protocol is valid
  @retval FALSE                 The protocol is invalid

**/
BOOLEAN
IsValidComponentNameProtocol (
  IN  EFI_COMPONENT_NAME_PROTOCOL    *ComponentName,
  IN  EFI_HANDLE                     Handle
  )
{
  EFI_STATUS                     Status;
  CHAR16                         *String1;
  CHAR16                         *String2;

  //
  // Some incomplete ComponentName protocols return success regardless of invalid inputs, which brings the wrong device name received.
  // Try the cases that gives illegal inputs to filter these incomplete ComponentName protocol.
  //
  // Give illegal inputs (ControllerHandle = NULL)
  //
  Status = ComponentName->GetControllerName (
                            ComponentName,
                            NULL,
                            Handle,
                            ComponentName->SupportedLanguages,
                            &String1
                            );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }

  //
  // Give illegal inputs (ControllerHandle = NULL, ChildHandle = NULL)
  //
  Status = ComponentName->GetControllerName (
                            ComponentName,
                            NULL,
                            NULL,
                            ComponentName->SupportedLanguages,
                            &String2
                            );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }

  //
  // The protocol that return a fixed data in the above cases is considered invalid
  //
  return (CompareMem (String1, String2, StrLen (String1)) != 0);
}


