/* $NoKeywords:$ */
/**
 * @file
 *
 * SnpDxeImplement.c
 *
 * Contains Ethernet Implement Driver for the DXE phase.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: FDK
 * @e sub-project: UEFI
 * @e version: $Revision: 300489 $ @e date: $Date: 2014-08-08 14:36:46 -0700 (Fri, 08 Aug 2014) $
 *
 */
/*
 ****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "SnpDxe.h"
#include "mdio.h"

extern  EFI_EVENT LinkStsTimerEvent;
extern 	UINT8 TxCount;
EFI_STATUS
ImplementStart (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  //
  // Set simple network state to Started and return success.
  //
  DEBUG ((EFI_D_ERROR, "ImplementStart  \n"));
  XgbeHwinitDelay();
  Private->Mode.State = EfiSimpleNetworkStarted;
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementStop (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  DEBUG ((EFI_D_ERROR, "ImplementStop  \n")); 

  if ( LinkStsTimerEvent != NULL) {
    gBS->CloseEvent (LinkStsTimerEvent);
  }

  //
  // Set simple network state to Stopped and return success.
  //
  Private->Mode.State = EfiSimpleNetworkStopped;
  return EFI_SUCCESS;
}
EFI_STATUS
ImplementInit (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  EFI_STATUS Status = EFI_NOT_READY;
    
  DEBUG ((EFI_D_ERROR, "ImplementInit  \n"));

  XgbeHwinitDelay();
  XgmacInit (Private, Private->xgmac_pdata.xgmac_regs);
  
  XgbeHwinitDelay();
  Status = xgbe_phy_start (Private);
  
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  
  XgbeHwinitDelay();
  xgbe_an_state_machine (Private);
  
  XgbeHwinitDelay();
  UpdateLinkStatus (Private);

  XgbeHwinitDelay();
  if(Private->Mode.MediaPresent ==  FALSE)
		return EFI_DEVICE_ERROR;

  
  //
  // Set simple network state to Initialized and return success.
  //  
  Private->Mode.State = EfiSimpleNetworkInitialized;
  
  return EFI_SUCCESS;
  
}

EFI_STATUS
ImplementReset (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  DEBUG ((EFI_D_ERROR, "ImplementReset  \n"));
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementShutdown (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
   DEBUG ((EFI_D_ERROR, "ImplementShutdown  \n"));

   xgbe_phy_stop (Private);
 
  if ( LinkStsTimerEvent != NULL) {
    gBS->SetTimer (LinkStsTimerEvent, TimerCancel, 0);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementRecvFilterEnable (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  Private->Mode.ReceiveFilterSetting = 1;

  return EFI_SUCCESS;
}

EFI_STATUS
ImplementRecvFilterDisable (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementRecvFilterRead (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementSetStnAddr (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       EFI_MAC_ADDRESS *NewMacAddr
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementStatistics (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementIp2Mac (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       BOOLEAN             IPv6,
  IN       EFI_IP_ADDRESS      *IP,
  IN       EFI_MAC_ADDRESS     *MAC
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementNvDataRead (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       UINTN      Offset,
  IN       UINTN      BufferSize,
  IN       VOID       *Buffer
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementGetStatus (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       UINT32     *InterruptStatusPtr,
  IN       VOID       **TransmitBufferListPtr
  )
{
  PXE_DB_GET_STATUS *Db = NULL;

  if (TransmitBufferListPtr != NULL) {
    Db = Private->Db;
	if(TxCount > 0){	 
		*TransmitBufferListPtr = (VOID *) (UINTN) Db->TxBuffer[TxCount-1];
		Db->TxBuffer[TxCount-1]= (PXE_UINT64)NULL;
		TxCount--;
	}
	else
		*TransmitBufferListPtr = NULL;
		
		
  }

  if (InterruptStatusPtr != NULL) {
    *InterruptStatusPtr = EFI_SIMPLE_NETWORK_TRANSMIT_INTERRUPT;
    *InterruptStatusPtr |= EFI_SIMPLE_NETWORK_RECEIVE_INTERRUPT;
  }

  UpdateLinkStatus (Private);
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementFillHeader (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       VOID            *MacHeaderPtr,
  IN       UINTN           HeaderSize,
  IN       VOID            *Buffer,
  IN       UINTN           BufferSize,
  IN       EFI_MAC_ADDRESS *DestAddr,
  IN       EFI_MAC_ADDRESS *SrcAddr,
  IN       UINT16          *ProtocolPtr
  )
{
  ETHER_HEAD *EnetHeader = NULL;

  if (SrcAddr == NULL) {
    SrcAddr = &Private->Mode.CurrentAddress;
  }

  EnetHeader = (ETHER_HEAD *)MacHeaderPtr;

  CopyMem (EnetHeader->DstMac, DestAddr, NET_ETHER_ADDR_LEN);
  CopyMem (EnetHeader->SrcMac, SrcAddr, NET_ETHER_ADDR_LEN);

  EnetHeader->EtherType = HTONS (*ProtocolPtr);

  return EFI_SUCCESS;
}

EFI_STATUS
ImplementTransmit (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       VOID       *Buffer,
  IN       UINTN      BufferSize
  )
{
  DEBUG ((EFI_D_ERROR, "ImplementTransmit  \n"));
  if (BufferSize > ENET_MAX_PACKET_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }

  if ((BufferSize == 0) || (Buffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  return xgmac_pre_xmit (Private, Buffer, BufferSize);
}

EFI_STATUS
ImplementReceive (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       VOID            *Buffer,
  IN       UINTN           *BufferSize,
  IN       UINTN           *HeaderSize,
  IN       EFI_MAC_ADDRESS *SrcAddr,
  IN       EFI_MAC_ADDRESS *DestAddr,
  IN       UINT16          *Protocol
  )
{
  EFI_STATUS Status = EFI_NOT_READY;
  ENET_HEADER *EnetHeader = NULL;
  Status = xgmac_dev_read (Private, Buffer, BufferSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (HeaderSize != NULL) {
    *HeaderSize = sizeof (ENET_HEADER);
  }

  EnetHeader = (ENET_HEADER *)Buffer;

  if (SrcAddr != NULL) {
    ZeroMem (SrcAddr, sizeof (EFI_MAC_ADDRESS));
    CopyMem (SrcAddr->Addr, EnetHeader->SrcAddr, ENET_ADDR_LEN);
  }

  if (DestAddr != NULL) {
    ZeroMem (DestAddr, sizeof (EFI_MAC_ADDRESS));
    CopyMem (DestAddr->Addr, EnetHeader->DstAddr, ENET_ADDR_LEN);
  }

  if (Protocol != NULL) {
    *Protocol = NTOHS (EnetHeader->Type);
  }

  Status = EFI_SUCCESS;

  return Status;
}

