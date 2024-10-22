/** @file
  Push Management Data to Management Controller

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Pldm.h"
#include "DashDxe.h"
#include <Library/PrintLib.h>


extern DASH_CONTEXT mDashContext;

#define ATTR_HANDLE_BOOTCONFIG   0x8C00
#define STR_HANDLE_BOOTCONFIG    0x8D00
#define VARIABLE_DATA_TEMP_SIZE  0x200
#define STR_HANDLE_BOOT0000      0x8D01

STATIC BOOLEAN mAcceptBiosAttrPendingValuesDone = FALSE;

/**
 Reallocte BIOS table memory if table size exceeds limit when data are appended to the table.

 @param        Table            BIOS table to be adjusted
 @param        AppendedSize     Numer of bytes to be appended to the table

 @retval None

**/
VOID
AdjustBiosTableCapacity (
  BIOS_TABLE *Table,
  UINTN      AppendedSize
  )
{
  UINTN NewCapacity;

  if (Table->MaxSize < Table->Size + AppendedSize) {
    NewCapacity = Table->Size + AppendedSize;
    if ((NewCapacity % 4) != 0) {
      NewCapacity += 4 - (NewCapacity % 4);
    }
    Table->Table = ReallocatePool (Table->MaxSize, NewCapacity, Table->Table);
    if (Table->Table != NULL) {
      Table->MaxSize = NewCapacity;
    }
  }
}

/**
 Append a string with specific Handle number to the BIOS String Table

 @param        This
 @param        StrHandle        The string Handle
 @param        StrLength        The string length
 @param        String           The string to be appended to the table

 @retval None

**/
VOID
EFIAPI
AppendBiosString (
  EFI_PLDM_PROTOCOL *This,
  UINT16     StrHandle,
  UINTN      StrLength,
  CHAR8      *String
  )
{
  PLDM_CONTEXT *Context;
  BIOS_TABLE   *Table;
  UINTN        AppendedSize;

  Context = PLDM_CONTEXT_FROM_THIS(This);
  Table = &Context->BiosStringTable;

   AppendedSize = StrLength + 4;
   AdjustBiosTableCapacity (Table, AppendedSize);

   AdjustBiosTableCapacity (Table, StrLength + 4);
   gBS->CopyMem (Table->Table + Table->Size, &StrHandle, 2);
   gBS->CopyMem (Table->Table + Table->Size + 2, &StrLength, 2);
   gBS->CopyMem (Table->Table + Table->Size + 4, String, (UINT16)StrLength);
   Table->Size += AppendedSize;

}

/**
 Append a BIOS Attribute with specific Handle number to the BIOS Attribute Table


 @param        This
 @param        AttrHandle       The Attribute Handle
 @param        AttrType

 @param        AttrNameHandle   The string Handle of the Attribute Name
 @param        AttrDataSize     Data size of the attribute to be appended
 @param        AttrData         The Attribute Data


 @retval None


**/
VOID
EFIAPI
AppendBiosAttr (
  EFI_PLDM_PROTOCOL *This,
  UINT16     AttrHandle,
  UINT8      AttrType,
  UINT16     AttrNameHandle,
  UINTN      AttrDataSize,
  VOID       *AttrData
)
{
  PLDM_CONTEXT *Context;
  BIOS_TABLE   *Table;
  UINTN        AppendedSize;

  Context = PLDM_CONTEXT_FROM_THIS(This);
  Table = &Context->BiosAttrTable;

   AppendedSize = AttrDataSize + 5;
   AdjustBiosTableCapacity (Table, AppendedSize);

   gBS->CopyMem (Table->Table + Table->Size, &AttrHandle, 2);
   gBS->CopyMem (Table->Table + Table->Size + 2, &AttrType, 1);
   gBS->CopyMem (Table->Table + Table->Size + 3, &AttrNameHandle, 2);
   gBS->CopyMem (Table->Table + Table->Size + 5, AttrData, AttrDataSize);
   Table->Size += AppendedSize;
}

/**
 Append a BIOS Attribute Value to the BIOS Attribute Value Table

 @param        This
 @param        AttrHandle       The Attribute Handle
 @param        AttrType         The Attribute Type
 @param        AttrValueDataSize  Data size of the attribute value to be appended
 @param        AttrValueData    The Attribute Value Data

 @retval None

**/
VOID
EFIAPI
AppendBiosAttrValue (
  EFI_PLDM_PROTOCOL *This,
  UINT16     AttrHandle,
  UINT8      AttrType,
  UINTN      AttrValueDataSize,
  VOID       *AttrValueData
  )
{
  PLDM_CONTEXT *Context;
  BIOS_TABLE   *Table;
  UINTN        AppendedSize;

  Context = PLDM_CONTEXT_FROM_THIS(This);
  Table = &Context->BiosAttrValueTable;

   AppendedSize = AttrValueDataSize + 3;
   AdjustBiosTableCapacity (Table, AppendedSize);

   gBS->CopyMem (Table->Table + Table->Size, &AttrHandle, 2);
   gBS->CopyMem (Table->Table + Table->Size + 2, &AttrType, 1);
   gBS->CopyMem (Table->Table + Table->Size + 3, AttrValueData, AttrValueDataSize);
   Table->Size += AppendedSize;
}

/**
 Calcute length of a ASCII string

 @param        Str              The ASCII string

 @return String length

**/
UINTN
Char8StrLen(
  CHAR8 *Str
)
{
  UINTN Len;

  Len = 0;
  while (Str[Len]!=0) Len++;

  return Len;
}

/**
 Add pad bytes to a BIOS table to ensure that table size can be divided by 4

 @param        Table            The BIOS table to be modified

 @retval None

**/
VOID
AppendPadBytes (
  BIOS_TABLE *Table
)
{
  while ((Table->Size % 4) != 0) {
    Table->Table[Table->Size++] = 0;
  }
}



/**
 Get SBIOS Structure Table from BIOS

 @param [in]   Context          pointer to PLDM_CONTEXT structure

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
PrepareSmbiosStructureTable(
  IN PLDM_CONTEXT     *Context
)
{
  EFI_STATUS Status;
  SMBIOS_TABLE_ENTRY_POINT *SmbiosEntry;
  SMBIOS_TABLE_3_0_ENTRY_POINT  *SmbiosEntry64Bit;
  EFI_CPU_IO2_PROTOCOL *CpuIo;

  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, (VOID **)&CpuIo);
  EFI_ERROR_RETURN (Status);

  //
  // 32 Bit Smbios table
  //
  if (((PcdGet16 (PcdSmbiosVersion) >> 8) < 0x3) ||
      (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT0) == BIT0))) {

    Status = EfiGetSystemConfigurationTable (
             &gEfiSmbiosTableGuid,
             (VOID **)&SmbiosEntry
             );
    EFI_ERROR_RETURN (Status);

    Context->SmbiosTableLen = SmbiosEntry->TableLength;
    Context->SmbiosBuf = AllocatePool (Context->SmbiosTableLen);
    if (Context->SmbiosBuf == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = CpuIo->Mem.Read (CpuIo, EfiCpuIoWidthUint8, SmbiosEntry->TableAddress, SmbiosEntry->TableLength, Context->SmbiosBuf);
    if (EFI_ERROR(Status))
    {
      FreePool(Context->SmbiosBuf);
      return Status;
    }

    Context->SmbiosCrc32 = 0;
    gBS->CalculateCrc32 (Context->SmbiosBuf, Context->SmbiosTableLen, &Context->SmbiosCrc32);
  }

  //
  // 64 Bit Smbios table
  //
  if (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT1) == BIT1)) {
         Status = EfiGetSystemConfigurationTable (
             &gEfiSmbios3TableGuid,
             (VOID **)&SmbiosEntry64Bit
             );
    EFI_ERROR_RETURN (Status);

    Context->SmbiosTableLen64Bit = (UINT16) SmbiosEntry64Bit->TableMaximumSize;

    Context->SmbiosBuf64Bit = AllocatePool (Context->SmbiosTableLen64Bit);
    if (Context->SmbiosBuf64Bit == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = CpuIo->Mem.Read (CpuIo, EfiCpuIoWidthUint8, SmbiosEntry64Bit->TableAddress, SmbiosEntry64Bit->TableMaximumSize, Context->SmbiosBuf64Bit);
    if (EFI_ERROR(Status))
    {
      FreePool(Context->SmbiosBuf64Bit);
      return Status;
    }

    Context->Smbios64BitCrc32 = 0;
    gBS->CalculateCrc32 (Context->SmbiosBuf64Bit, Context->SmbiosTableLen64Bit, &Context->Smbios64BitCrc32);

  }

  return EFI_SUCCESS;

}

/**
 Initialize a single BIOS table with its size, capacity assigned and memory allocated

 @param        Table            The BIOS table to be initialized

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
InitBiosTable (
  BIOS_TABLE *Table
)
{
  Table->Size = 0;
  Table->MaxSize = TABLE_INIT_SIZE;
  Table->Table = AllocatePool (Table->MaxSize);
  if (Table->Table != NULL) {
    return EFI_SUCCESS;
  }
  return EFI_OUT_OF_RESOURCES;
}

/**
 Initialize all BIOS tables

 @param        PldmContext      pointer to the PLDM_CONTEXT structure

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
InitBiosTables (
PLDM_CONTEXT *PldmContext
)
{

  EFI_STATUS Status;

  PldmContext->BiosTags[0] = 0;
  PldmContext->BiosTags[1] = 0;
  PldmContext->BiosTags[2] = 0;

  Status =  InitBiosTable (&PldmContext->BiosStringTable);
  EFI_ERROR_RETURN (Status);

  Status =  InitBiosTable (&PldmContext->BiosAttrTable);
  if(EFI_ERROR(Status)) {
    FreePool (PldmContext->BiosStringTable.Table);
    return Status;
  }

  Status = InitBiosTable (&PldmContext->BiosAttrValueTable);
  if(EFI_ERROR(Status)) {
    FreePool (PldmContext->BiosStringTable.Table);
    FreePool (PldmContext->BiosAttrTable.Table);
  }

  return Status;
}

/**
 Reset all BIOS tables to their initial settings

 @param        This

 @retval EFI_SUCCESS            Successful
 @return Other values           Unsuccessful

**/
EFI_STATUS
EFIAPI
ResetBiosTables (
EFI_PLDM_PROTOCOL *This
)
{

//  EFI_STATUS Status;
  PLDM_CONTEXT *PldmContext;

  PldmContext = PLDM_CONTEXT_FROM_THIS(This);

  PldmContext->BiosTags[0] = 0;
  PldmContext->BiosTags[1] = 0;
  PldmContext->BiosTags[2] = 0;
  PldmContext->BiosStringTable.Size = 0;
  PldmContext->BiosAttrTable.Size = 0;
  PldmContext->BiosAttrValueTable.Size = 0;

  return EFI_SUCCESS;
}


/**
 Calcute the CRC32 checksum of the BIOS tables and store them in BiosTags array

 @param        PldmContext      pointer to the PLDM_CONTEXT structure

 @retval None

**/
VOID
CalculateBiosTags (
  PLDM_CONTEXT *PldmContext
  )
{
  BIOS_TABLE *Table;

  Table = &PldmContext->BiosStringTable;
  gBS->CalculateCrc32 (Table->Table, Table->Size, &PldmContext->BiosTags[0]);
  Table = &PldmContext->BiosAttrTable;
  gBS->CalculateCrc32 (Table->Table, Table->Size, &PldmContext->BiosTags[1]);
  Table = &PldmContext->BiosAttrValueTable;
  gBS->CalculateCrc32 (Table->Table, Table->Size, &PldmContext->BiosTags[2]);

}

/**
 Get the Boot String for a boot item with specific boot index

 @param        BootIndex        The index of the boot item
 @param        BootStr

 @retval SUCCESS                Successfull
 @return others               failed

**/
EFI_STATUS
GetNvRamBootString (
  UINTN BootIndex,
  CHAR8 *BootStr
  )
{
  CHAR16 BootOption[16];
  UINTN BufSize;
  UINTN Index;
  UINT8 BootBuf[VARIABLE_DATA_TEMP_SIZE];
  EFI_STATUS Status;

  BufSize = sizeof (BootBuf);
  UnicodeSPrint(BootOption, sizeof(BootOption), L"Boot%04x", BootIndex);
  Status = gRT->GetVariable (BootOption, &gEfiGlobalVariableGuid, NULL, &BufSize, BootBuf);
  EFI_ERROR_RETURN (Status);

  Index = 0;
  do {
    BootStr[Index] = BootBuf[6 + Index*2];
    Index++;
  } while ((BootStr[Index - 1] != 0) && (Index < 255));
  BootStr[Index] = 0;

  return EFI_SUCCESS;

}


EFI_STATUS
ProcessPendingBootConfig (
  IN EFI_PLDM_PROTOCOL *Pldm,
  OUT UINT8 *NewBootIndex,
  OUT UINTN *NumberOfBootSource
  )
{
  EFI_STATUS Status;
  BOOT_CONFIG_SETTING_ATTR_VALUE AttrData;
  UINTN Size;
  UINTN Index;
  UINT8 BootSourceCnt[MAX_BOOT_COUNT] = {0};

  Size = sizeof (BOOT_CONFIG_SETTING_ATTR_VALUE);

  Status = Pldm->GetBiosTable (Pldm, BiosAttrPendingValueTable, &AttrData, &Size);
  EFI_ERROR_RETURN(Status);

  if(AttrData.NumberOfPendingBootSourceSetting > MAX_BOOT_COUNT) {
    return EFI_PLDM_COMPLETION_ERROR;
  }

  *NumberOfBootSource = AttrData.NumberOfPendingBootSourceSetting;

  for (Index=0; Index<AttrData.NumberOfPendingBootSourceSetting;Index++) {
    if (AttrData.BootSourceIndex[Index] >= AttrData.NumberOfPendingBootSourceSetting) {
      return EFI_PLDM_COMPLETION_ERROR;
    } else {
      BootSourceCnt[AttrData.BootSourceIndex[Index]]++;
      NewBootIndex[Index] = AttrData.BootSourceIndex[Index];
    }
  }
  for (Index=0; Index<AttrData.NumberOfPendingBootSourceSetting;Index++) {
    if (BootSourceCnt[Index]!=1 ) {
      return EFI_PLDM_COMPLETION_ERROR;
    }
  }

  return EFI_SUCCESS;



}


/**
 Prepare Boot Configuration for the BIOS tables

 @param        PldmContext      pointer to the PLDM_CONTEXT structure

 @retval SUCCESS                Successfull
 @return others               failed

**/
EFI_STATUS
ProcessBootConfig (
PLDM_CONTEXT *PldmContext
)
{
  UINTN Index;
  UINTN BufSize;
  EFI_STATUS Status;
  CHAR8 *BootOptionMenu="Boot Option Menu";
  CHAR8 BootStr[VARIABLE_DATA_TEMP_SIZE];
  CHAR8 *UnknownBootItem="Unknown";
  UINT16 BootOrder[MAX_BOOT_COUNT];
  UINT16 BootConfigAttrValueBootOrder[MAX_BOOT_COUNT];
  UINT16 BootOrderTemp[MAX_BOOT_COUNT];
  UINT16 MinVlaue;
  UINTN BootCount;
  UINTN Index2;
  UINTN MinVlaueIndex;
  BOOT_CONFIG_ATTR BootConfigAttr;
  BOOT_CONFIG_ATTR_VALUE BootConfigAttrValue;
  EFI_PLDM_PROTOCOL *Pldm;

  Pldm = &PldmContext->Pldm;
  BufSize = sizeof (BootOrder);
  Status = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid, NULL, &BufSize, BootOrder);
  EFI_ERROR_RETURN (Status);
  gBS->CopyMem(BootOrderTemp, BootOrder, BufSize);
  BootCount = BufSize/sizeof(UINT16);

  AppendBiosString (Pldm, STR_HANDLE_BOOTCONFIG, Char8StrLen(BootOptionMenu), BootOptionMenu);

  for (Index = 0; Index < BootCount; Index++) {
    MinVlaue = BootOrderTemp[0];
    MinVlaueIndex = 0;
    for (Index2 = 0; Index2 < BootCount; Index2++) {
      if(MinVlaue > BootOrderTemp[Index2]) {
         MinVlaue = BootOrderTemp[Index2];
         MinVlaueIndex = Index2;
      }
    }
    BootOrderTemp[MinVlaueIndex] = 0xFFFF;
    BootConfigAttrValueBootOrder[MinVlaueIndex] = (UINT16) Index;
    Status = GetNvRamBootString (MinVlaue, BootStr);
    if (EFI_ERROR(Status)) {
      gBS->CopyMem(BootStr, UnknownBootItem, Char8StrLen(UnknownBootItem) + 1);
    }
    AppendBiosString (Pldm, (UINT16)(STR_HANDLE_BOOT0000+Index), Char8StrLen(BootStr), BootStr );
  }


  BootConfigAttr.BootConfigType = 0x01;
  BootConfigAttr.OrderedFailThroughMode = 0x03;
  BootConfigAttr.MinBootSource = 0;
  BootConfigAttr.MaxBootSource = MAX_BOOT_COUNT;
  BootConfigAttr.NumPossibleBootSource = (UINT8)BootCount;
  for (Index = 0; Index < BootCount; Index++) {
    BootConfigAttr.BootSourceStrHandle[Index] = (UINT16)(STR_HANDLE_BOOT0000 + Index);
  }
  BufSize = 5 + 2*BootCount;
  AppendBiosAttr ( Pldm,
                   ATTR_HANDLE_BOOTCONFIG,
                   BiosBootConfigSetting,
                   STR_HANDLE_BOOTCONFIG,
                   BufSize, &BootConfigAttr);


  BootConfigAttrValue.BootConfigType = 0x01;
  BootConfigAttrValue.OrderedFailThroughMode = 0x03;
  BootConfigAttrValue.NumBootSource = (UINT8)BootCount;
  for (Index = 0; Index < BootCount; Index++) {
    BootConfigAttrValue.BootSourceStrHandleIndex[Index] = (UINT8) BootConfigAttrValueBootOrder[Index];
  }
  BufSize = 3 + BootCount;
  AppendBiosAttrValue ( Pldm,
                        ATTR_HANDLE_BOOTCONFIG,
                        BiosBootConfigSetting,
                        BufSize,
                        &BootConfigAttrValue);

  return EFI_SUCCESS;
}


/**
 Push BIOS tables to the management controller

 @param        Pldm             pointer to the EFI_PLDM_PROTOCOL structure

 @retval SUCCESS                Successfull
 @return others               failed

**/
EFI_STATUS
EFIAPI
PushBiosTables (
EFI_PLDM_PROTOCOL *Pldm
)
{
  PLDM_CONTEXT *PldmContext;
  EFI_STATUS   Status;
  UINT8 TableTypes[3]={0, 1, 2};
  UINT32 BiosTags[3];

  PldmContext = PLDM_CONTEXT_FROM_THIS(Pldm);

  AppendPadBytes(&PldmContext->BiosStringTable);
  AppendPadBytes(&PldmContext->BiosAttrTable);
  AppendPadBytes(&PldmContext->BiosAttrValueTable);

  CalculateBiosTags (PldmContext);

  Status = Pldm->GetBiosTableTags (Pldm, 3, TableTypes, BiosTags);
  if ( EFI_ERROR(Status)                                                         ||
       (PldmContext->BiosTags[BiosStringTable]    != BiosTags[BiosStringTable])  ||
       (PldmContext->BiosTags[BiosAttrTable]      != BiosTags[BiosAttrTable])    ||
       (PldmContext->BiosTags[BiosAttrValueTable] != BiosTags[BiosAttrValueTable]) ) {

    Status = Pldm->SetBiosTable (Pldm, BiosStringTable, PldmContext->BiosStringTable.Table, PldmContext->BiosStringTable.Size);
    EFI_ERROR_RETURN (Status);

    Status = Pldm->SetBiosTable (Pldm, BiosAttrTable, PldmContext->BiosAttrTable.Table, PldmContext->BiosAttrTable.Size);
    EFI_ERROR_RETURN (Status);

    Status = Pldm->SetBiosTable (Pldm, BiosAttrValueTable, PldmContext->BiosAttrValueTable.Table, PldmContext->BiosAttrValueTable.Size);
    EFI_ERROR_RETURN (Status);

    Status = Pldm->SetBiosTableTags (Pldm, 3, TableTypes, PldmContext->BiosTags);
    EFI_ERROR_RETURN (Status);
  }

  return EFI_SUCCESS;
}

/**
 Push SMBIOS structure table to the management controller

 @param        Pldm             pointer to the EFI_PLDM_PROTOCOL structure

 @retval SUCCESS                Successfull
 @return others               failed

**/
EFI_STATUS
EFIAPI
PushSmbiosStructureTable (
  EFI_PLDM_PROTOCOL *Pldm
)
{
  EFI_STATUS Status;
  PLDM_CONTEXT *PldmContext;
  PLDM_SMBIOS_STRUCTURE_TABLE_METADATA MetaData;
  UINT32                              SmbiosCrc32;

  PldmContext = PLDM_CONTEXT_FROM_THIS(Pldm);
  Status = PrepareSmbiosStructureTable (PldmContext);

  EFI_ERROR_RETURN (Status);

  if (((PcdGet16 (PcdSmbiosVersion) >> 8) < 0x3) ||
      (((PcdGet16 (PcdSmbiosVersion) >> 8) >= 0x3) && ((PcdGet32 (PcdSmbiosEntryPointProvideMethod) & BIT0) == BIT0))) {

    SmbiosCrc32 = PldmContext->SmbiosCrc32;
  } else {

    SmbiosCrc32 = PldmContext->Smbios64BitCrc32;
  }
  Status = Pldm->GetSmbiosStructureTableMetaData(Pldm, &MetaData);
  if (EFI_ERROR(Status) || (MetaData.Crc32 != SmbiosCrc32)) {
    Status = Pldm->SetSmbiosStructureTable(Pldm);
    EFI_ERROR_RETURN (Status);

    Status = Pldm->SetSmbiosStructureTableMetaData (Pldm);
    EFI_ERROR_RETURN (Status);
  }
  return EFI_SUCCESS;

}

/**
 Get BootOrder Data from Variable and Compared with MC,
 If not match, sync it.

 @param[in] Event            Event whose notification function is being invoked.
 @param[in] Handle           Checkpoint handle.

 @retval None

**/
VOID
EFIAPI
SyncBootOrderDataFromMC(
  IN EFI_EVENT      Event,
  IN H2O_CP_HANDLE  Handle
)
{
  EFI_STATUS                                Status;
  UINTN                                     Index;
  UINTN                                     BufSize;
  UINT16                                    BootOrder[MAX_BOOT_COUNT];
  UINT16                                    NewBootOrder[MAX_BOOT_COUNT];
  UINTN                                     BootCount;
  UINTN                                     NewBootCount;
  UINT8                                     NewBootIndex[MAX_BOOT_COUNT];
  UINT8                                     TempNewBootIndex[MAX_BOOT_COUNT];
  UINT16                                    BootOrderTemp[MAX_BOOT_COUNT];
  UINT16                                    AttrHandles[]={ATTR_HANDLE_BOOTCONFIG};
  UINT8                                     TableTypes[3]={0, 1, 2};
  UINT32                                    BiosTags[3];
  PLDM_CONTEXT                              *PldmContext;
  UINTN                                     Index2;
  UINTN                                     MinVlaueIndex;
  UINTN                                     MinVlaueIndex2;
  UINT16                                    MinVlaue;
  UINT16                                    MinVlaue2;
  BOOT_CONFIG_SETTING_ATTR_VALUE            *AttrValueTablePtr;
  EFI_PLDM_PROTOCOL                         *Pldm;

  H2OCpUnregisterHandler (Handle);

  PldmContext = &mDashContext.PldmContext;
  if (PldmContext->Signature != PLDM_SIGNATURE) {
    return;
  }
  
  Pldm = &PldmContext->Pldm;
  BufSize = sizeof (BootOrder);

  Status = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid, NULL, &BufSize, BootOrder);
  if (EFI_ERROR (Status)) {
    return;
  }

  BootCount = BufSize/sizeof(UINT16);

  Pldm->SetDateTime (Pldm);
  ResetBiosTables(Pldm);
  PldmContext = PLDM_CONTEXT_FROM_THIS(Pldm);
  Status = ProcessBootConfig (PldmContext);
  if (EFI_ERROR(Status)) {
    return;
  }
  AppendPadBytes(&PldmContext->BiosStringTable);
  AppendPadBytes(&PldmContext->BiosAttrTable);
  AppendPadBytes(&PldmContext->BiosAttrValueTable);
  CalculateBiosTags (PldmContext);

  Status = Pldm->GetBiosTableTags (Pldm, 3, TableTypes, BiosTags);
  if ( EFI_ERROR(Status)                                                         ||
       (PldmContext->BiosTags[BiosStringTable]    != BiosTags[BiosStringTable])  ||
       (PldmContext->BiosTags[BiosAttrTable]      != BiosTags[BiosAttrTable])    ||
       (PldmContext->BiosTags[BiosAttrValueTable] != BiosTags[BiosAttrValueTable]) ) {

    return;
  }

  Status = ProcessPendingBootConfig (Pldm, NewBootIndex, &NewBootCount);
  if ((Status == EFI_SUCCESS) && (NewBootCount == BootCount)) {
    gBS->CopyMem(TempNewBootIndex, NewBootIndex, BufSize);
    gBS->CopyMem(BootOrderTemp, BootOrder, BufSize);
    for (Index = 0; Index < NewBootCount; Index++) {
      MinVlaue = TempNewBootIndex[0];
      MinVlaueIndex = 0;
      MinVlaue2 = BootOrderTemp[0];
      MinVlaueIndex2 = 0;
      for (Index2 = 0; Index2 < NewBootCount; Index2++) {
        if(MinVlaue > TempNewBootIndex[Index2]) {
           MinVlaue = TempNewBootIndex[Index2];
           MinVlaueIndex = Index2;
        }
        if(MinVlaue2 > BootOrderTemp[Index2]) {
          MinVlaue2 = BootOrderTemp[Index2];
          MinVlaueIndex2 = Index2;
        }
      }
      TempNewBootIndex[MinVlaueIndex] = 0xFF;
      BootOrderTemp[MinVlaueIndex2] = 0xFFFF;
      NewBootOrder[MinVlaueIndex] = MinVlaue2;
    }
    Status = gRT->SetVariable (L"BootOrder",
                                 &gEfiGlobalVariableGuid,
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                 BufSize,
                                 NewBootOrder
                               );
    if (EFI_ERROR(Status)) {
      Pldm->AcceptBiosAttrPendingValues (Pldm, 0, NULL);
    } else {
      Pldm->AcceptBiosAttrPendingValues (Pldm, 1, AttrHandles);
      AttrValueTablePtr = (BOOT_CONFIG_SETTING_ATTR_VALUE *) PldmContext->BiosAttrValueTable.Table;
      for (Index = 0; Index < AttrValueTablePtr->NumberOfPendingBootSourceSetting; Index++) {
        AttrValueTablePtr->BootSourceIndex[Index] = NewBootIndex[Index];
      }
      Status = Pldm->SetBiosTable (Pldm, BiosAttrValueTable, PldmContext->BiosAttrValueTable.Table, PldmContext->BiosAttrValueTable.Size);
      if (!EFI_ERROR(Status)) {
        AppendPadBytes(&PldmContext->BiosStringTable);
        AppendPadBytes(&PldmContext->BiosAttrTable);
        AppendPadBytes(&PldmContext->BiosAttrValueTable);
        CalculateBiosTags (PldmContext);
        Status = Pldm->SetBiosTableTags (Pldm, 3, TableTypes, PldmContext->BiosTags);
        if (!EFI_ERROR(Status)) {
           mAcceptBiosAttrPendingValuesDone = TRUE;
        }
      }
    }
  }
}

/**
 Push Mangement Data to the management controller

 @param [in]   Event
 @param [in]   Context

 @retval None

**/
VOID
EFIAPI
PushManagementDataToMC(
  IN EFI_EVENT        Event,
  IN VOID             *Context
)
{
  EFI_STATUS Status;
  PLDM_CONTEXT *PldmContext;
  EFI_PLDM_PROTOCOL *Pldm;
  UINT32 BiosTags[3] = {0};

  PldmContext = &mDashContext.PldmContext;
  if(PldmContext->Signature != PLDM_SIGNATURE) {
    return;
  }
  Pldm = &(PldmContext->Pldm);
  Status = PushSmbiosStructureTable (Pldm);
  if(EFI_ERROR(Status)) {
    return;
  }

  if (mAcceptBiosAttrPendingValuesDone) {
    BiosTags[BiosStringTable] = PldmContext->BiosTags[BiosStringTable];
    BiosTags[BiosAttrTable] = PldmContext->BiosTags[BiosAttrTable];
    BiosTags[BiosAttrValueTable] = PldmContext->BiosTags[BiosAttrValueTable];
  }

  ResetBiosTables(Pldm);
  Status = ProcessBootConfig (PldmContext);
  if(EFI_ERROR(Status)) {
    return;
  }

  if (mAcceptBiosAttrPendingValuesDone) {
    AppendPadBytes(&PldmContext->BiosStringTable);
    AppendPadBytes(&PldmContext->BiosAttrTable);
    AppendPadBytes(&PldmContext->BiosAttrValueTable);
    CalculateBiosTags (PldmContext);
    if ( (PldmContext->BiosTags[BiosStringTable]  == BiosTags[BiosStringTable])  &&
       (PldmContext->BiosTags[BiosAttrTable]      == BiosTags[BiosAttrTable])    &&
       (PldmContext->BiosTags[BiosAttrValueTable] == BiosTags[BiosAttrValueTable])) {

      return;
    }
  }
  PushBiosTables (Pldm);
}
